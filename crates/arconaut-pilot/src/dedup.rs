use serde_json::Value;
use std::collections::HashSet;

/// Per-turn deduplication.
///
/// Tracks which (tool_name, canonical_args) pairs have been seen in the current turn.
pub struct Deduplicator {
    seen: HashSet<(String, String)>,
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
            seen: HashSet::new(),
            last: None,
            consecutive: 0,
        }
    }

    /// Clear at the start of each turn.
    pub fn clear(&mut self) {
        self.seen.clear();
        self.last = None;
        self.consecutive = 0;
    }

    /// Check if this tool call was already made this turn.
    pub fn is_duplicate(&self, tool_name: &str, args: &Value) -> bool {
        self.seen.contains(&(tool_name.to_string(), args.to_string()))
    }

    /// Record a tool call.
    pub fn record(&mut self, tool_name: &str, args: &Value) {
        let key = (tool_name.to_string(), args.to_string());
        self.seen.insert(key.clone());
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
}
