use arconaut_core::ToolResult;
use serde_json::Value;
use std::collections::HashMap;

/// Deduplicates tool calls within a single turn.
///
/// If the LLM issues identical (tool_name, arguments) calls in one turn,
/// the second and subsequent calls return the cached result without re-executing.
/// The cache is cleared at the start of each turn.
pub struct Deduplicator {
    cache: HashMap<(String, String), ToolResult>,
}

impl Default for Deduplicator {
    fn default() -> Self {
        Self::new()
    }
}

impl Deduplicator {
    pub fn new() -> Self {
        Self {
            cache: HashMap::new(),
        }
    }

    /// Clear all cached entries. Call at the start of each turn.
    pub fn clear(&mut self) {
        self.cache.clear();
    }

    /// Look up a cached result for the given tool call.
    pub fn get(&self, name: &str, args: &Value) -> Option<ToolResult> {
        let key = Self::key(name, args);
        self.cache.get(&key).cloned()
    }

    /// Store a result in the cache.
    pub fn insert(&mut self, name: &str, args: &Value, result: ToolResult) {
        let key = Self::key(name, args);
        self.cache.insert(key, result);
    }

    fn key(name: &str, args: &Value) -> (String, String) {
        (name.to_string(), args.to_string())
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use arconaut_core::ContentPart;

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
}
