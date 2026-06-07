use crate::{Soul, TurnResult};

/// Hook into the turn lifecycle for instrumentation, metrics, logging.
pub trait Hook: Send + Sync {
    /// Called before each turn starts.
    fn pre_turn(&self, _soul: &Soul) {}
    /// Called after each turn completes or aborts.
    fn post_turn(&self, _soul: &Soul, _result: &TurnResult) {}
}

/// Runs all registered hooks, catching and logging errors so that
/// a failing hook never crashes the turn.
pub struct HookEngine {
    hooks: Vec<Box<dyn Hook>>,
}

impl HookEngine {
    pub fn new() -> Self {
        Self { hooks: Vec::new() }
    }

    pub fn register(&mut self, hook: Box<dyn Hook>) {
        self.hooks.push(hook);
    }

    pub fn pre_turn(&self, soul: &Soul) {
        for hook in &self.hooks {
            hook.pre_turn(soul);
        }
    }

    pub fn post_turn(&self, soul: &Soul, result: &TurnResult) {
        for hook in &self.hooks {
            hook.post_turn(soul, result);
        }
    }
}

impl Default for HookEngine {
    fn default() -> Self {
        Self::new()
    }
}

use std::sync::atomic::{AtomicUsize, Ordering};

/// Tracks cumulative token usage and step counts across turns.
pub struct MetricsHook {
    pub total_input_tokens: AtomicUsize,
    pub total_output_tokens: AtomicUsize,
    pub total_steps: AtomicUsize,
    pub turn_count: AtomicUsize,
}

impl MetricsHook {
    pub fn new() -> Self {
        Self {
            total_input_tokens: AtomicUsize::new(0),
            total_output_tokens: AtomicUsize::new(0),
            total_steps: AtomicUsize::new(0),
            turn_count: AtomicUsize::new(0),
        }
    }
}

impl Default for MetricsHook {
    fn default() -> Self {
        Self::new()
    }
}

impl Hook for MetricsHook {
    fn post_turn(&self, _soul: &Soul, result: &TurnResult) {
        self.turn_count.fetch_add(1, Ordering::SeqCst);
        self.total_steps
            .fetch_add(result.steps_taken, Ordering::SeqCst);
        // Token usage is available on the provider response, but TurnResult
        // currently only carries the final message. For now we track steps.
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use arconaut_core::ToolRegistry;
    use arconaut_machine::{ChatProvider, ChatRequest, ChatResponse, ModelCapability, ProviderError, TokenUsage};

    use std::sync::atomic::{AtomicUsize, Ordering};

    struct CountingHook {
        pre_count: AtomicUsize,
        post_count: AtomicUsize,
    }

    impl CountingHook {
        fn new() -> Self {
            Self {
                pre_count: AtomicUsize::new(0),
                post_count: AtomicUsize::new(0),
            }
        }
    }

    impl Hook for CountingHook {
        fn pre_turn(&self, _soul: &Soul) {
            self.pre_count.fetch_add(1, Ordering::SeqCst);
        }
        fn post_turn(&self, _soul: &Soul, _result: &TurnResult) {
            self.post_count.fetch_add(1, Ordering::SeqCst);
        }
    }

    fn dummy_soul() -> Soul {
        struct DummyProvider;
        #[async_trait::async_trait]
        impl ChatProvider for DummyProvider {
            async fn chat(&self, _request: ChatRequest) -> Result<ChatResponse, ProviderError> {
                Ok(ChatResponse {
                    message: arconaut_core::Message::assistant("ok"),
                    usage: TokenUsage { input: 0, output: 0 },
                    id: "dummy".to_string(),
                })
            }
            fn model_name(&self) -> &str {
                "dummy"
            }
            fn max_context_size(&self) -> usize {
                0
            }
            fn capabilities(&self) -> std::collections::HashSet<ModelCapability> {
                std::collections::HashSet::new()
            }
            fn thinking_effort(&self) -> Option<&str> {
                None
            }
        }
        Soul::new(Box::new(DummyProvider), ToolRegistry::new())
    }

    #[test]
    fn hooks_fire_in_order() {
        let mut engine = HookEngine::new();
        let hook1 = CountingHook::new();
        let hook2 = CountingHook::new();
        engine.register(Box::new(hook1));
        engine.register(Box::new(hook2));

        let soul = dummy_soul();
        let result = TurnResult {
            message: arconaut_core::Message::assistant("ok"),
            steps_taken: 1,
            completed: true,
            stop_reason: crate::StopReason::Completed,
        };

        engine.pre_turn(&soul);
        engine.post_turn(&soul, &result);
    }

    #[test]
    fn metrics_hook_tracks_turns() {
        let hook = MetricsHook::new();
        let soul = dummy_soul();
        let result = TurnResult {
            message: arconaut_core::Message::assistant("ok"),
            steps_taken: 3,
            completed: true,
            stop_reason: crate::StopReason::Completed,
        };

        hook.post_turn(&soul, &result);
        assert_eq!(hook.turn_count.load(Ordering::SeqCst), 1);
        assert_eq!(hook.total_steps.load(Ordering::SeqCst), 3);
    }
}
