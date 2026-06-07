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

    pub fn revert_to(&mut self, checkpoint_id: usize) {
        if let Some(cp) = self.checkpoints.get(checkpoint_id) {
            self.history.truncate(cp.history_len);
            self.token_count = cp.token_count;
        }
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
    (text_len + 3) / 4
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn context_append_and_count() {
        let mut ctx = Context::new(1000);
        ctx.append_message(Message::user("hello world"));
        assert_eq!(ctx.token_count(), 3); // 11 chars / 4 = 2.75 → 3
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

        ctx.revert_to(cp);
        assert_eq!(ctx.history().len(), 1);
        assert_eq!(ctx.token_count(), 1); // "A" = 1 char / 4 = 1
    }

    #[test]
    fn token_estimate_accuracy() {
        let msg = Message::user("a".repeat(400));
        let mut ctx = Context::new(1000);
        ctx.append_message(msg);
        let count = ctx.token_count();
        // 400 chars / 4 = 100, within ±10% = 90-110
        assert!(count >= 90 && count <= 110, "token count {} not in range", count);
    }
}
