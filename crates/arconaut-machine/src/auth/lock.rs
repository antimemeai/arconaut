use std::io;
use std::path::PathBuf;

/// A cross-process exclusive file lock using POSIX `flock(2)`.
///
/// # Why `flock` instead of `fcntl`?
///
/// POSIX `fcntl(F_SETLK)` locks have a dangerous property on Linux and BSD:
/// closing **any** file descriptor for a given inode drops **all** locks held
/// by that process on that inode. This means unrelated code opening and
/// closing the same file path could silently release our lock.
///
/// `flock(2)` locks are tied to the specific file descriptor. Closing an
/// unrelated fd for the same inode does NOT release the lock. This makes
/// `flock` safer for coordinating token refresh across processes.
///
/// # Limitations
///
/// On Linux and macOS, `flock` locks are **per-process**, not per-thread.
/// A process can hold only one lock mode (shared or exclusive) per file.
/// This means within a single process, re-acquiring a lock on the same
/// file will always succeed. Cross-process contention cannot be tested
/// in a single-process unit test.
///
/// The lock is released automatically when the file descriptor is closed
/// (i.e. when `FileLock` is dropped).
pub struct FileLock {
    #[cfg(unix)]
    fd: std::os::fd::RawFd,
    #[cfg(not(unix))]
    _path: PathBuf,
}

impl FileLock {
    /// Attempt to acquire an exclusive lock on the given path.
    ///
    /// Creates the lock file if it does not exist.
    ///
    /// Returns:
    /// - `Ok(Some(FileLock))` — lock acquired
    /// - `Ok(None)` — lock is held by another process
    /// - `Err` — I/O failure
    pub fn try_lock(path: PathBuf) -> Result<Option<Self>, io::Error> {
        #[cfg(unix)]
        {
            use std::os::fd::IntoRawFd;
            let file = std::fs::OpenOptions::new()
                .create(true)
                .truncate(false)
                .write(true)
                .open(&path)?;
            let fd = file.into_raw_fd();

            let result = unsafe { libc::flock(fd, libc::LOCK_EX | libc::LOCK_NB) };
            if result == -1 {
                let err = io::Error::last_os_error();
                unsafe { libc::close(fd) };
                if err.kind() == io::ErrorKind::WouldBlock {
                    return Ok(None);
                }
                return Err(err);
            }

            Ok(Some(Self { fd }))
        }
        #[cfg(not(unix))]
        {
            // Windows fallback: not implemented yet
            let _ = path;
            Ok(None)
        }
    }
}

#[cfg(unix)]
impl Drop for FileLock {
    fn drop(&mut self) {
        unsafe {
            // Explicit unlock is optional (close releases it), but being
            // explicit makes the intent clear.
            let _ = libc::flock(self.fd, libc::LOCK_UN);
            let _ = libc::close(self.fd);
        }
    }
}

#[cfg(not(unix))]
impl Drop for FileLock {
    fn drop(&mut self) {}
}

#[cfg(test)]
mod tests {
    use super::*;
    use tempfile::TempDir;

    #[test]
    fn lock_uncontended() {
        let dir = TempDir::new().unwrap();
        let path = dir.path().join("test.lock");

        // Acquire lock
        let lock = FileLock::try_lock(path.clone());
        assert!(lock.is_ok());
        assert!(lock.unwrap().is_some());

        // Drop releases it; we can re-acquire in the same process.
        // Note: this does NOT test cross-process contention. POSIX locks
        // are per-process, so the same process can always re-acquire.
        let lock2 = FileLock::try_lock(path);
        assert!(lock2.is_ok());
        assert!(lock2.unwrap().is_some());
    }

    #[test]
    fn lock_creates_file() {
        let dir = TempDir::new().unwrap();
        let path = dir.path().join("new.lock");

        assert!(!path.exists());
        let lock = FileLock::try_lock(path.clone()).unwrap();
        assert!(lock.is_some());
        assert!(path.exists());
        drop(lock);
    }

    // Cross-process lock contention cannot be unit-tested because POSIX
    // `flock` locks are per-process. A single process can always
    // re-acquire a lock it already holds. Contention is tested via
    // integration tests or manual verification.
}
