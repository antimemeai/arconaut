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

    pub fn clear(&mut self) {
        self.history.clear();
        self.token_count = 0;
    }

    pub fn token_count(&self) -> usize {
        self.token_count
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
    let text_len: usize = msg
        .content
        .iter()
        .map(|part| match part {
            crate::ContentPart::Text { text } => text.len(),
            _ => 0,
        })
        .sum();
    text_len / 4
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
}
