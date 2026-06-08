use arconaut_core::ToolResult;
use serde_json::Value;
use std::collections::HashMap;

/// Per-turn deduplication.
///
/// Tracks which (tool_name, canonical_args) pairs have been seen in the current turn
/// and caches their results for reuse.
pub struct Deduplicator {
    cache: HashMap<(String, String), ToolResult>,
    last: Option<(String, String)>,
    consecutive: usize,
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
            last: None,
            consecutive: 0,
        }
    }

    /// Clear at the start of each turn.
    pub fn clear(&mut self) {
        self.cache.clear();
        self.last = None;
        self.consecutive = 0;
    }

    /// Check if this tool call was already made this turn.
    pub fn is_duplicate(&self, tool_name: &str, args: &Value) -> bool {
        self.get(tool_name, args).is_some()
    }

    /// Record a tool call (without caching a result).
    pub fn record(&mut self, tool_name: &str, args: &Value) {
        self.insert(tool_name, args, ToolResult::success(vec![]));
    }

    /// Look up a cached result for the given tool call.
    pub fn get(&self, name: &str, args: &Value) -> Option<ToolResult> {
        let key = Self::key(name, args);
        self.cache.get(&key).cloned()
    }

    /// Store a result in the cache.
    pub fn insert(&mut self, name: &str, args: &Value, result: ToolResult) {
        let key = Self::key(name, args);
        self.cache.insert(key.clone(), result);
        self.update_consecutive(key);
    }

    /// Consecutive duplicate count (for churn detection).
    pub fn consecutive_count(&self) -> usize {
        self.consecutive
    }

    fn update_consecutive(&mut self, key: (String, String)) {
        if let Some(ref last) = self.last {
            if last == &key {
                self.consecutive += 1;
                return;
            }
        }
        self.last = Some(key);
        self.consecutive = 1;
    }

    fn key(name: &str, args: &Value) -> (String, String) {
        (name.to_string(), args.to_string())
    }
}
