pub use arconaut_pilot::Deduplicator;

#[cfg(test)]
mod tests {
    use super::*;
    use arconaut_core::ContentPart;
    use arconaut_core::ToolResult;

    #[test]
    fn identical_calls_cached() {
        let mut dedup = Deduplicator::new();
        let args = serde_json::json!({"x": 1});
        let result = ToolResult::success(vec![ContentPart::text("ok")]);

        dedup.insert("echo", &args, result.clone());
        let cached = dedup.get("echo", &args);
        assert!(cached.is_some());
        assert_eq!(cached.unwrap(), result);
    }

    #[test]
    fn different_args_not_cached() {
        let mut dedup = Deduplicator::new();
        let args1 = serde_json::json!({"x": 1});
        let result = ToolResult::success(vec![ContentPart::text("ok")]);
        dedup.insert("echo", &args1, result);

        let args2 = serde_json::json!({"x": 2});
        assert!(dedup.get("echo", &args2).is_none());
    }

    #[test]
    fn cache_cleared() {
        let mut dedup = Deduplicator::new();
        let args = serde_json::json!({"x": 1});
        let result = ToolResult::success(vec![ContentPart::text("ok")]);
        dedup.insert("echo", &args, result);

        dedup.clear();
        assert!(dedup.get("echo", &args).is_none());
    }

    #[test]
    fn consecutive_identical_calls_tracked() {
        let mut dedup = Deduplicator::new();
        let args = serde_json::json!({"x": 1});
        let result = ToolResult::success(vec![ContentPart::text("ok")]);

        dedup.insert("echo", &args, result.clone());
        assert_eq!(dedup.consecutive_count(), 1);

        dedup.insert("echo", &args, result.clone());
        assert_eq!(dedup.consecutive_count(), 2);

        dedup.insert("echo", &args, result);
        assert_eq!(dedup.consecutive_count(), 3);
    }

    #[test]
    fn consecutive_resets_on_different_call() {
        let mut dedup = Deduplicator::new();
        let args1 = serde_json::json!({"x": 1});
        let args2 = serde_json::json!({"x": 2});
        let result = ToolResult::success(vec![ContentPart::text("ok")]);

        dedup.insert("echo", &args1, result.clone());
        assert_eq!(dedup.consecutive_count(), 1);

        dedup.insert("echo", &args2, result);
        assert_eq!(dedup.consecutive_count(), 1);
    }

    #[test]
    fn consecutive_cleared_with_cache() {
        let mut dedup = Deduplicator::new();
        let args = serde_json::json!({"x": 1});
        let result = ToolResult::success(vec![ContentPart::text("ok")]);

        dedup.insert("echo", &args, result);
        assert_eq!(dedup.consecutive_count(), 1);

        dedup.clear();
        assert_eq!(dedup.consecutive_count(), 0);
    }
}
