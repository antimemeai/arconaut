use crate::Deduplicator;
use arconaut_core::Message;

/// Detects churn (oscillation, spinning, lack of progress) in agent output.
pub struct ChurnDetector {
    trigger_phrases: Vec<&'static str>,
    max_consecutive_dups: usize,
    stall_threshold_steps: usize,
}

impl ChurnDetector {
    pub fn new() -> Self {
        Self {
            trigger_phrases: vec![
                "actually",
                "but wait",
                "on second thought",
                "let me reconsider",
                "let me rethink",
                "hold on",
                "wait",
            ],
            max_consecutive_dups: 5,
            stall_threshold_steps: 10,
        }
    }

    /// Detect churn phrases in an assistant message.
    pub fn detect_message_churn(&self, message: &Message) -> bool {
        let text = message
            .content
            .iter()
            .filter_map(|part| part.as_text())
            .collect::<Vec<_>>()
            .join(" ")
            .to_lowercase();

        self.trigger_phrases
            .iter()
            .any(|phrase| text.contains(phrase))
    }

    /// Detect tool-call loops via the deduplicator.
    pub fn detect_tool_loop(&self, dedup: &Deduplicator) -> bool {
        dedup.consecutive_count() >= self.max_consecutive_dups
    }

    /// Detect stall: N steps with no progress.
    pub fn detect_stall(&self, steps_taken: usize, _last_progress_step: usize) -> bool {
        steps_taken >= self.stall_threshold_steps
    }

    /// Check all churn signals.
    pub fn detect(&self, message: &Message, dedup: &Deduplicator, steps_taken: usize) -> ChurnLevel {
        if dedup.consecutive_count() >= 10 {
            return ChurnLevel::HardStop;
        }
        if self.detect_message_churn(message)
            || self.detect_tool_loop(dedup)
            || self.detect_stall(steps_taken, 0)
        {
            return ChurnLevel::Advisory;
        }
        ChurnLevel::None
    }
}

impl Default for ChurnDetector {
    fn default() -> Self {
        Self::new()
    }
}

/// Severity of churn detected.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum ChurnLevel {
    /// No churn detected.
    None,
    /// Churn detected; inject advisory prompt.
    Advisory,
    /// Severe churn; hard-stop the turn.
    HardStop,
}

/// Injects an intervention prompt into context when churn is detected.
pub struct InterventionInjector {
    detector: ChurnDetector,
    last_injected: bool,
}

impl InterventionInjector {
    pub fn new() -> Self {
        Self {
            detector: ChurnDetector::new(),
            last_injected: false,
        }
    }

    /// Check the last assistant message for churn and return the intervention prompt if needed.
    pub fn check(&mut self, message: &Message, dedup: &Deduplicator, steps_taken: usize) -> ChurnLevel {
        let level = self.detector.detect(message, dedup, steps_taken);
        match level {
            ChurnLevel::None => {
                self.last_injected = false;
            }
            ChurnLevel::Advisory => {
                self.last_injected = true;
            }
            ChurnLevel::HardStop => {
                self.last_injected = true;
            }
        }
        level
    }

    /// Returns the intervention prompt text.
    pub fn intervention_prompt() -> &'static str {
        "Churn detected. You appear to be oscillating or reconsidering without making progress. \
         Take a breath. State clearly:\n\
         1. What you were trying to do\n\
         2. What blocked you\n\
         3. Your next concrete action\n\
         If you are genuinely stuck, say so and stop rather than spinning."
    }

    /// Returns the stronger warning for repeated tool loops.
    pub fn tool_loop_prompt() -> &'static str {
        "You are repeating the exact same tool call without making progress. \
         This is a loop. Stop and reconsider your approach. \
         If you are stuck, say so and stop."
    }
}

impl Default for InterventionInjector {
    fn default() -> Self {
        Self::new()
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use arconaut_core::{ContentPart, ToolResult};

    #[test]
    fn detects_trigger_phrases() {
        let detector = ChurnDetector::new();
        let msg = Message::assistant("Actually, let me reconsider that approach.");
        assert!(detector.detect_message_churn(&msg));
    }

    #[test]
    fn ignores_normal_text() {
        let detector = ChurnDetector::new();
        let msg = Message::assistant("The implementation looks correct. Moving on.");
        assert!(!detector.detect_message_churn(&msg));
    }

    #[test]
    fn detects_tool_loop() {
        let detector = ChurnDetector::new();
        let mut dedup = Deduplicator::new();
        let args = serde_json::json!({"path": "/tmp"});
        let result = ToolResult::success(vec![ContentPart::text("ok")]);

        for _ in 0..5 {
            dedup.insert("read", &args, result.clone());
        }

        assert!(detector.detect_tool_loop(&dedup));
    }

    #[test]
    fn detects_stall() {
        let detector = ChurnDetector::new();
        assert!(detector.detect_stall(10, 0));
        assert!(!detector.detect_stall(5, 0));
    }

    #[test]
    fn hard_stop_at_10_dups() {
        let detector = ChurnDetector::new();
        let mut dedup = Deduplicator::new();
        let args = serde_json::json!({"path": "/tmp"});
        let result = ToolResult::success(vec![ContentPart::text("ok")]);

        for _ in 0..10 {
            dedup.insert("read", &args, result.clone());
        }

        let msg = Message::assistant("ok");
        assert_eq!(
            detector.detect(&msg, &dedup, 1),
            ChurnLevel::HardStop
        );
    }

    #[test]
    fn advisory_at_5_dups() {
        let detector = ChurnDetector::new();
        let mut dedup = Deduplicator::new();
        let args = serde_json::json!({"path": "/tmp"});
        let result = ToolResult::success(vec![ContentPart::text("ok")]);

        for _ in 0..5 {
            dedup.insert("read", &args, result.clone());
        }

        let msg = Message::assistant("ok");
        assert_eq!(
            detector.detect(&msg, &dedup, 1),
            ChurnLevel::Advisory
        );
    }

    #[test]
    fn no_churn_when_clean() {
        let detector = ChurnDetector::new();
        let dedup = Deduplicator::new();
        let msg = Message::assistant("Everything is fine.");
        assert_eq!(
            detector.detect(&msg, &dedup, 1),
            ChurnLevel::None
        );
    }

    #[test]
    fn injector_tracks_state() {
        let mut injector = InterventionInjector::new();
        let dedup = Deduplicator::new();

        let msg = Message::assistant("Actually, wait.");
        let level = injector.check(&msg, &dedup, 1);
        assert_eq!(level, ChurnLevel::Advisory);
        assert!(injector.last_injected);

        let msg2 = Message::assistant("All good now.");
        let level2 = injector.check(&msg2, &dedup, 2);
        assert_eq!(level2, ChurnLevel::None);
        assert!(!injector.last_injected);
    }
}
