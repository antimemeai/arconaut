use arconaut_core::{Context, Message};

/// Summarizes old context when token usage exceeds a threshold.
pub struct CompactionEngine {
    /// Trigger compaction when token_count / max_tokens exceeds this ratio.
    pub threshold: f64,
    /// Number of most recent messages to preserve untouched.
    pub preserve_window: usize,
}

impl Default for CompactionEngine {
    fn default() -> Self {
        Self {
            threshold: 0.8,
            preserve_window: 10,
        }
    }
}

impl CompactionEngine {
    pub fn new() -> Self {
        Self::default()
    }

    pub fn with_threshold(mut self, threshold: f64) -> Self {
        self.threshold = threshold;
        self
    }

    pub fn with_preserve_window(mut self, window: usize) -> Self {
        self.preserve_window = window;
        self
    }

    /// Returns true if compaction was performed.
    pub fn compact(&self, context: &mut Context) -> bool {
        let max = context.max_tokens();
        let current = context.token_count();
        if max == 0 {
            return false;
        }
        let ratio = current as f64 / max as f64;
        if ratio < self.threshold {
            return false;
        }

        let history = context.history();
        if history.len() <= self.preserve_window.saturating_add(1) {
            // Not enough messages to compact while preserving window
            return false;
        }

        let split = history.len() - self.preserve_window;
        let summarized_count = split;

        // Build summary text from summarized messages
        let summary_text = format!(
            "[SUMMARY] {} previous messages summarized ({} tokens)",
            summarized_count, current
        );

        // We need to modify the context's history directly.
        // Context doesn't expose mutable history, so we use a workaround:
        // checkpoint current state, build new history, clear and rebuild.
        let preserved: Vec<Message> = history[split..].to_vec();

        context.clear();
        context.append_message(Message::system(summary_text));
        for msg in preserved {
            context.append_message(msg);
        }

        true
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use arconaut_core::Role;

    #[test]
    fn no_compaction_under_threshold() {
        let engine = CompactionEngine::new().with_threshold(0.9);
        let mut ctx = Context::new(10_000);
        ctx.append_message(Message::user("hello"));

        assert!(!engine.compact(&mut ctx));
        assert_eq!(ctx.history().len(), 1);
    }

    #[test]
    fn compaction_reduces_tokens() {
        let engine = CompactionEngine::new()
            .with_threshold(0.5)
            .with_preserve_window(2);
        let mut ctx = Context::new(100);

        // Fill context with large messages to exceed threshold
        for i in 0..20 {
            ctx.append_message(Message::user(format!(
                "this is a much longer message number {} with lots of text",
                i
            )));
        }

        let before = ctx.token_count();
        assert!(engine.compact(&mut ctx));
        let after = ctx.token_count();

        assert!(after < before, "token count should decrease: {} -> {}", before, after);
        assert_eq!(ctx.history().len(), 3); // summary + 2 preserved
        assert_eq!(ctx.history()[0].role, Role::System);
        assert!(ctx.history()[0].content[0].as_text().unwrap().starts_with("[SUMMARY]"));
    }

    #[test]
    fn compaction_preserves_recent() {
        let engine = CompactionEngine::new()
            .with_threshold(0.5)
            .with_preserve_window(3);
        let mut ctx = Context::new(100);

        for i in 0..10 {
            ctx.append_message(Message::user(format!("msg{}", i)));
        }

        engine.compact(&mut ctx);

        // Last 3 messages should be preserved
        let history = ctx.history();
        let last_preserved = &history[history.len() - 3];
        assert_eq!(last_preserved.content[0].as_text().unwrap(), "msg7");
    }

    #[test]
    fn no_compaction_when_too_few_messages() {
        let engine = CompactionEngine::new()
            .with_threshold(0.1)
            .with_preserve_window(10);
        let mut ctx = Context::new(100);

        for i in 0..5 {
            ctx.append_message(Message::user(format!("msg{}", i)));
        }

        assert!(!engine.compact(&mut ctx));
        assert_eq!(ctx.history().len(), 5);
    }
}
