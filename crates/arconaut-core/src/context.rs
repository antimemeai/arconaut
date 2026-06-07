use crate::Message;
use chrono::{DateTime, Utc};
use serde::{Deserialize, Serialize};

#[derive(Debug, Clone, Serialize, Deserialize, PartialEq)]
pub struct Context {
    history: Vec<Message>,
    token_count: usize,
    max_tokens: usize,
    checkpoints: Vec<Checkpoint>,
}

#[derive(Debug, Clone, Serialize, Deserialize, PartialEq)]
pub struct Checkpoint {
    pub history_len: usize,
    pub token_count: usize,
    pub timestamp: DateTime<Utc>,
}

#[derive(Debug, Clone, PartialEq)]
pub struct InvalidCheckpoint {
    pub id: usize,
    pub max: usize,
}

impl std::fmt::Display for InvalidCheckpoint {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(f, "invalid checkpoint id: {} (max: {})", self.id, self.max)
    }
}

impl std::error::Error for InvalidCheckpoint {}

impl Context {
    pub fn new(max_tokens: usize) -> Self {
        Self {
            history: Vec::new(),
            token_count: 0,
            max_tokens,
            checkpoints: Vec::new(),
        }
    }

    pub fn append_message(&mut self, message: Message) {
        let estimated = estimate_tokens(&message);
        self.token_count += estimated;
        self.history.push(message);
    }

    pub fn insert_message(&mut self, index: usize, message: Message) {
        let estimated = estimate_tokens(&message);
        self.token_count += estimated;
        self.history.insert(index, message);
    }

    pub fn clear(&mut self) {
        self.history.clear();
        self.token_count = 0;
    }

    /// Clear all checkpoints. Use with care — this invalidates revert targets.
    pub fn clear_checkpoints(&mut self) {
        self.checkpoints.clear();
    }

    pub fn token_count(&self) -> usize {
        self.token_count
    }

    pub fn max_tokens(&self) -> usize {
        self.max_tokens
    }

    pub fn history(&self) -> &[Message] {
        &self.history
    }

    pub fn checkpoint(&mut self) -> usize {
        let id = self.checkpoints.len();
        self.checkpoints.push(Checkpoint {
            history_len: self.history.len(),
            token_count: self.token_count,
            timestamp: Utc::now(),
        });
        id
    }

    pub fn revert_to(&mut self, checkpoint_id: usize) -> Result<(), InvalidCheckpoint> {
        let cp = self
            .checkpoints
            .get(checkpoint_id)
            .ok_or(InvalidCheckpoint {
                id: checkpoint_id,
                max: self.checkpoints.len().saturating_sub(1),
            })?;
        self.history.truncate(cp.history_len);
        self.token_count = cp.token_count;
        // Prune checkpoints after the reversion point
        self.checkpoints.truncate(checkpoint_id + 1);
        Ok(())
    }
}

fn estimate_tokens(msg: &Message) -> usize {
    let text: String = msg.content.iter().filter_map(|p| p.as_text()).collect();
    let ascii_chars = text.chars().filter(|c| c.is_ascii()).count();
    let non_ascii_chars = text.chars().filter(|c| !c.is_ascii()).count();
    // ASCII: ~4 chars per token. CJK and other non-ASCII: ~1.5 chars per token.
    (ascii_chars / 4) + (non_ascii_chars * 2 / 3)
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn context_append_and_count() {
        let mut ctx = Context::new(1000);
        ctx.append_message(Message::user("hello world"));
        assert_eq!(ctx.token_count(), 2); // 11 chars / 4 = 2 (floor)
        assert_eq!(ctx.history().len(), 1);
    }

    #[test]
    fn context_clear_resets() {
        let mut ctx = Context::new(1000);
        ctx.append_message(Message::user("a"));
        ctx.append_message(Message::assistant("b"));
        ctx.checkpoint();
        ctx.clear();
        assert!(ctx.history().is_empty());
        assert_eq!(ctx.token_count(), 0);
        assert_eq!(ctx.checkpoints.len(), 1); // preserved
    }

    #[test]
    fn context_checkpoint_revert() {
        let mut ctx = Context::new(1000);
        let a = Message::user("A");
        let b = Message::assistant("B");
        let c = Message::user("C");

        ctx.append_message(a);
        let cp = ctx.checkpoint();
        ctx.append_message(b);
        ctx.append_message(c);
        assert_eq!(ctx.history().len(), 3);

        ctx.revert_to(cp).unwrap();
        assert_eq!(ctx.history().len(), 1);
        assert_eq!(ctx.token_count(), 0); // "A" = 1 char / 4 = 0 (floor)
        assert_eq!(ctx.checkpoints.len(), 1); // pruned
    }

    #[test]
    fn context_revert_invalid_checkpoint() {
        let mut ctx = Context::new(1000);
        let err = ctx.revert_to(999).unwrap_err();
        assert_eq!(err.id, 999);
        assert_eq!(err.max, 0);
    }

    #[test]
    fn token_estimate_accuracy() {
        let msg = Message::user("a".repeat(400));
        let mut ctx = Context::new(1000);
        ctx.append_message(msg);
        let count = ctx.token_count();
        // 400 chars / 4 = 100, within ±10% = 90-110
        assert!(
            (90..=110).contains(&count),
            "token count {} not in range",
            count
        );
    }

    #[test]
    fn cjk_token_estimate() {
        // CJK characters are ~1.5 chars per token.
        // 90 CJK chars ≈ 60 tokens. Old formula (bytes/4) would give ~67.
        // New formula (chars * 2/3) gives exactly 60.
        let cjk = "中文字符测试".repeat(10); // 60 CJK chars
        let msg = Message::user(cjk);
        let mut ctx = Context::new(1000);
        ctx.append_message(msg);
        let count = ctx.token_count();
        // 60 CJK chars * 2/3 = 40 tokens, allow ±30%
        assert!(
            (28..=52).contains(&count),
            "CJK token count {} not in reasonable range",
            count
        );
    }

    mod proptest_tests {
        use super::*;
        use proptest::prelude::*;

        fn arb_message_text() -> impl Strategy<Value = String> {
            // Mix of ASCII and arbitrary Unicode to stress the estimator.
            prop::string::string_regex(".*").unwrap()
        }

        proptest! {
            #[test]
            fn estimate_does_not_panic(text in arb_message_text()) {
                let msg = Message::user(text);
                // If estimate_tokens panics or overflows, the test fails automatically.
                let _est = estimate_tokens(&msg);
            }

            #[test]
            fn empty_string_yields_zero(text in "") {
                let msg = Message::user(text);
                prop_assert_eq!(estimate_tokens(&msg), 0);
            }

            #[test]
            fn estimate_never_exceeds_char_count(text in arb_message_text()) {
                let msg = Message::user(text.clone());
                let est = estimate_tokens(&msg);
                // Worst case: all non-ASCII => ~1.5 chars/token => tokens <= chars
                // ASCII => ~4 chars/token => tokens <= chars
                prop_assert!(est <= text.chars().count(),
                    "estimate {} > char count {} for {:?}", est, text.chars().count(), text);
            }

            #[test]
            fn appending_increases_or_preserves_estimate(
                base in arb_message_text(),
                suffix in arb_message_text()
            ) {
                let msg_base = Message::user(base.clone());
                let est_base = estimate_tokens(&msg_base);

                let combined = format!("{}{}", base, suffix);
                let msg_combined = Message::user(combined);
                let est_combined = estimate_tokens(&msg_combined);

                prop_assert!(
                    est_combined >= est_base,
                    "estimate dropped from {} to {} when appending {:?} to {:?}",
                    est_base, est_combined, suffix, base
                );
            }
        }
    }
}
