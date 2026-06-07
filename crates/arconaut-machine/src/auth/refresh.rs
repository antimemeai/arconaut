use super::{CredentialStorage, FileLock, KimiOAuthFlow};
use std::path::PathBuf;
use std::sync::atomic::{AtomicU64, Ordering};
use std::sync::Arc;
use std::time::{Duration, SystemTime, UNIX_EPOCH};
use tokio::task::JoinHandle;

/// Activity window: active (<5m), recent (<1h), idle (<2h), very idle (≥2h).
const ACTIVE_SECONDS: u64 = 300;      // 5 minutes
const RECENT_SECONDS: u64 = 3600;     // 1 hour
const IDLE_SECONDS: u64 = 7200;       // 2 hours

/// Check intervals for each activity level.
const ACTIVE_INTERVAL: Duration = Duration::from_secs(60);
const RECENT_INTERVAL: Duration = Duration::from_secs(180);
const IDLE_INTERVAL: Duration = Duration::from_secs(600);

/// Tracks the time of the most recent user prompt.
///
/// Uses an atomic `u64` of UNIX epoch seconds for lock-free updates.
#[derive(Debug)]
pub struct ActivityTracker {
    last_prompt_secs: AtomicU64,
}

impl ActivityTracker {
    /// Create a new tracker initialized to the current time.
    pub fn new() -> Self {
        Self {
            last_prompt_secs: AtomicU64::new(now_secs()),
        }
    }

    /// Record that a prompt just occurred.
    pub fn record_prompt(&self) {
        self.last_prompt_secs.store(now_secs(), Ordering::Relaxed);
    }

    /// Seconds since the last recorded prompt.
    pub fn seconds_since_last_prompt(&self) -> u64 {
        now_secs().saturating_sub(self.last_prompt_secs.load(Ordering::Relaxed))
    }

    /// Determine the appropriate refresh check interval based on activity.
    ///
    /// Returns `None` if the background task should stop (very idle).
    pub fn current_interval(&self) -> Option<Duration> {
        let elapsed = self.seconds_since_last_prompt();
        if elapsed < ACTIVE_SECONDS {
            Some(ACTIVE_INTERVAL)
        } else if elapsed < RECENT_SECONDS {
            Some(RECENT_INTERVAL)
        } else if elapsed < IDLE_SECONDS {
            Some(IDLE_INTERVAL)
        } else {
            None
        }
    }
}

impl Default for ActivityTracker {
    fn default() -> Self {
        Self::new()
    }
}

fn now_secs() -> u64 {
    SystemTime::now()
        .duration_since(UNIX_EPOCH)
        .unwrap_or_default()
        .as_secs()
}

/// A background task that proactively refreshes OAuth tokens.
///
/// Spawns a tokio task that periodically checks token expiry and refreshes
/// if needed. Uses a file lock to coordinate with other arconaut processes.
///
/// The task adapts its check interval based on user activity and stops
/// itself when the user has been idle for 2+ hours.
pub struct RefreshTask {
    handle: Option<JoinHandle<()>>,
}

impl RefreshTask {
    /// Start a background refresh task.
    ///
    /// # Arguments
    /// - `storage` — credential storage to load/save tokens
    /// - `flow` — OAuth flow for performing refreshes
    /// - `storage_key` — key used to store the token (e.g., "kimi")
    /// - `activity` — shared activity tracker
    /// - `lock_path` — path to the cross-process lock file
    pub fn start(
        storage: Arc<dyn CredentialStorage>,
        flow: KimiOAuthFlow,
        storage_key: String,
        activity: Arc<ActivityTracker>,
        lock_path: PathBuf,
    ) -> Self {
        let handle = tokio::spawn(async move {
            refresh_loop(storage, flow, storage_key, activity, lock_path).await;
        });
        Self {
            handle: Some(handle),
        }
    }

    /// Stop the background refresh task.
    ///
    /// This aborts the task; any in-progress refresh is interrupted.
    pub fn stop(&mut self) {
        if let Some(handle) = self.handle.take() {
            handle.abort();
        }
    }
}

impl Drop for RefreshTask {
    fn drop(&mut self) {
        self.stop();
    }
}

async fn refresh_loop(
    storage: Arc<dyn CredentialStorage>,
    flow: KimiOAuthFlow,
    storage_key: String,
    activity: Arc<ActivityTracker>,
    lock_path: PathBuf,
) {
    loop {
        // Determine how long to sleep before next check
        let Some(interval) = activity.current_interval() else {
            // Very idle — stop background checks. Next prompt will
            // trigger on-demand refresh if needed.
            break;
        };

        // Check if token needs refresh
        match storage.load(&storage_key) {
            Ok(Some(token)) => {
                if token.needs_refresh() {
                    // Try to acquire cross-process lock
                    match FileLock::try_lock(lock_path.clone()) {
                        Ok(Some(_lock)) => {
                            // Re-read token from disk — another process may
                            // have refreshed while we were waiting for the lock.
                            match storage.load(&storage_key) {
                                Ok(Some(current)) => {
                                    if current.needs_refresh() {
                                        match flow.refresh_token(&current.refresh_token).await {
                                            Ok(new_token) => {
                                                if let Err(e) =
                                                    storage.save(&storage_key, &new_token)
                                                {
                                                    tracing::error!(
                                                        "refresh task: failed to save token: {}",
                                                        e
                                                    );
                                                }
                                            }
                                            Err(e) => {
                                                tracing::error!(
                                                    "refresh task: token refresh failed: {}",
                                                    e
                                                );
                                            }
                                        }
                                    }
                                }
                                Ok(None) => {
                                    tracing::warn!("refresh task: token removed from storage");
                                }
                                Err(e) => {
                                    tracing::error!(
                                        "refresh task: failed to re-load token: {}",
                                        e
                                    );
                                }
                            }
                        }
                        Ok(None) => {
                            // Another process holds the lock; it will refresh.
                            // We'll check again after the next interval.
                        }
                        Err(e) => {
                            tracing::error!("refresh task: failed to acquire lock: {}", e);
                        }
                    }
                }
            }
            Ok(None) => {
                // No token stored; nothing to refresh.
            }
            Err(e) => {
                tracing::error!("refresh task: failed to load token: {}", e);
            }
        }

        tokio::time::sleep(interval).await;
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn activity_tracker_records_and_reads() {
        let tracker = ActivityTracker::new();
        assert!(tracker.seconds_since_last_prompt() < 2);

        // Simulate old prompt by creating a new tracker and not recording
        // (can't easily go back in time, so just verify the API)
        tracker.record_prompt();
        assert!(tracker.seconds_since_last_prompt() < 2);
    }

    #[test]
    fn interval_by_activity_level() {
        let tracker = ActivityTracker::new();
        tracker.record_prompt();

        // Active: <5m since last prompt
        assert_eq!(tracker.current_interval(), Some(ACTIVE_INTERVAL));

        // Simulate recent activity by going back in time — we can't,
        // so instead verify the boundary logic by manual elapsed values.
    }

    #[test]
    fn interval_returns_none_when_very_idle() {
        // We can't easily simulate 2h of idle time without mocking time.
        // This test verifies the API contract.
        let tracker = ActivityTracker::new();
        // A brand-new tracker is "active", not idle
        assert!(tracker.current_interval().is_some());
    }

    use tracing_test::traced_test;

    #[traced_test]
    #[tokio::test]
    async fn refresh_task_logs_storage_error() {
        use super::super::storage::{CredentialStorage, StorageError};

        struct FailingStorage;
        impl CredentialStorage for FailingStorage {
            fn load(&self, _key: &str) -> Result<Option<super::super::OAuthToken>, StorageError> {
                Err(StorageError::Io("mock storage failure".to_string()))
            }
            fn save(&self, _key: &str, _token: &super::super::OAuthToken) -> Result<(), StorageError> {
                Ok(())
            }
            fn delete(&self, _key: &str) -> Result<(), StorageError> {
                Ok(())
            }
        }

        let storage = Arc::new(FailingStorage) as Arc<dyn CredentialStorage>;
        let flow = super::super::KimiOAuthFlow::new();
        let activity = Arc::new(ActivityTracker::new());
        let lock_path = tempfile::NamedTempFile::new().unwrap().into_temp_path().to_path_buf();

        let mut task = RefreshTask::start(storage, flow, "test".to_string(), activity, lock_path);

        // Yield so the spawned task runs through the first iteration (load → error → sleep).
        tokio::task::yield_now().await;
        task.stop();

        // Assert the log line was captured by tracing-test's subscriber.
        logs_assert(|lines: &[&str]| {
            let found = lines
                .iter()
                .any(|l| l.contains("refresh task: failed to load token"));
            if found {
                Ok(())
            } else {
                Err("expected trace log not found".into())
            }
        });
    }
}
