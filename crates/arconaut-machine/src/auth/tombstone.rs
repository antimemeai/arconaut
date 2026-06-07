use std::collections::HashMap;
use std::sync::{Mutex, OnceLock};
use std::time::{Duration, Instant};

/// How long to block retries after a token is rejected.
const TOMBSTONE_COOLDOWN: Duration = Duration::from_secs(300); // 5 minutes

struct TokenTombstone {
    rejected_at: Instant,
}

fn tombstones() -> &'static Mutex<HashMap<String, TokenTombstone>> {
    static TOMBSTONES: OnceLock<Mutex<HashMap<String, TokenTombstone>>> = OnceLock::new();
    TOMBSTONES.get_or_init(|| Mutex::new(HashMap::new()))
}

/// Check if a refresh token is currently tombstoned (rejected within the cooldown).
///
/// Also cleans expired tombstones from the map.
pub fn is_tombstoned(refresh_token: &str) -> bool {
    let mut map = tombstones().lock().unwrap();
    map.retain(|_, ts| ts.rejected_at.elapsed() < TOMBSTONE_COOLDOWN);
    map.contains_key(refresh_token)
}

/// Mark a refresh token as rejected (tombstoned).
///
/// Subsequent refresh attempts with this token will be blocked until the
/// cooldown expires.
pub fn tombstone(refresh_token: &str) {
    let mut map = tombstones().lock().unwrap();
    map.insert(
        refresh_token.to_string(),
        TokenTombstone {
            rejected_at: Instant::now(),
        },
    );
}

/// Remove a tombstone for a refresh token (e.g., after successful refresh).
pub fn remove_tombstone(refresh_token: &str) {
    let mut map = tombstones().lock().unwrap();
    map.remove(refresh_token);
}

/// Clear all tombstones. Primarily useful for testing.
pub fn clear_all() {
    let mut map = tombstones().lock().unwrap();
    map.clear();
}

#[cfg(test)]
mod tests {
    use super::*;

    fn setup() {
        clear_all();
    }

    #[test]
    fn tombstone_blocks_then_expires() {
        setup();
        let token = "test_refresh_token_123";

        assert!(!is_tombstoned(token));
        tombstone(token);
        assert!(is_tombstoned(token));

        remove_tombstone(token);
        assert!(!is_tombstoned(token));
    }

    #[test]
    fn multiple_tombstones() {
        setup();
        tombstone("token_a");
        tombstone("token_b");

        assert!(is_tombstoned("token_a"));
        assert!(is_tombstoned("token_b"));
        assert!(!is_tombstoned("token_c"));
    }
}
