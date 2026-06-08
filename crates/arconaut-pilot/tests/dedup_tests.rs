use arconaut_pilot::Deduplicator;
use serde_json::json;

#[test]
fn new_dedup_is_empty() {
    let dedup = Deduplicator::new();
    assert!(!dedup.is_duplicate("read", &json!({"path": "/tmp"})));
}

#[test]
fn record_makes_duplicate() {
    let mut dedup = Deduplicator::new();
    dedup.record("read", &json!({"path": "/tmp"}));
    assert!(dedup.is_duplicate("read", &json!({"path": "/tmp"})));
}

#[test]
fn different_args_not_duplicate() {
    let mut dedup = Deduplicator::new();
    dedup.record("read", &json!({"path": "/tmp"}));
    assert!(!dedup.is_duplicate("read", &json!({"path": "/other"})));
}

#[test]
fn clear_resets_state() {
    let mut dedup = Deduplicator::new();
    dedup.record("read", &json!({"path": "/tmp"}));
    dedup.clear();
    assert!(!dedup.is_duplicate("read", &json!({"path": "/tmp"})));
    assert_eq!(dedup.consecutive_count(), 0);
}

#[test]
fn consecutive_count_tracks_repeats() {
    let mut dedup = Deduplicator::new();
    dedup.record("read", &json!({"path": "/tmp"}));
    assert_eq!(dedup.consecutive_count(), 1);

    dedup.record("read", &json!({"path": "/tmp"}));
    assert_eq!(dedup.consecutive_count(), 2);

    dedup.record("read", &json!({"path": "/tmp"}));
    assert_eq!(dedup.consecutive_count(), 3);
}

#[test]
fn consecutive_resets_on_different_args() {
    let mut dedup = Deduplicator::new();
    dedup.record("read", &json!({"path": "/tmp"}));
    assert_eq!(dedup.consecutive_count(), 1);

    dedup.record("read", &json!({"path": "/other"}));
    assert_eq!(dedup.consecutive_count(), 1);
}

#[test]
fn consecutive_resets_on_different_tool() {
    let mut dedup = Deduplicator::new();
    dedup.record("read", &json!({"path": "/tmp"}));
    assert_eq!(dedup.consecutive_count(), 1);

    dedup.record("write", &json!({"path": "/tmp"}));
    assert_eq!(dedup.consecutive_count(), 1);
}
