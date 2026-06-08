use arconaut_core::{ContentPart, Message, Role};
use arconaut_machine::provider::{
    ChatProvider, ChatRequest, ChatResponse, ModelCapability, ProviderError, TokenUsage,
};
use arconaut_pilot::{DefaultSoul, SoulConfig, StopReason};
use arconaut_reactor::{ArconautLogic, ReactorConfig};
use async_trait::async_trait;
use std::collections::HashSet;

/// Mock provider that returns tool calls on first response, text on second.
struct TwoStepProvider {
    call_count: std::sync::atomic::AtomicUsize,
}

impl TwoStepProvider {
    fn new() -> Self {
        Self {
            call_count: std::sync::atomic::AtomicUsize::new(0),
        }
    }
}

#[async_trait]
impl ChatProvider for TwoStepProvider {
    async fn chat(&self, _request: ChatRequest) -> Result<ChatResponse, ProviderError> {
        let count = self.call_count.fetch_add(1, std::sync::atomic::Ordering::SeqCst);
        let message = if count == 0 {
            // First call: request a tool.
            Message::new(
                Role::Assistant,
                vec![ContentPart::ToolCall {
                    tool_call: arconaut_core::ToolCall {
                        id: "call-1".to_string(),
                        function: arconaut_core::FunctionCall {
                            name: "echo".to_string(),
                            arguments: "{\"text\":\"hello\"}".to_string(),
                        },
                    },
                }],
            )
        } else {
            // Second call: respond with text.
            Message::new(
                Role::Assistant,
                vec![ContentPart::Text {
                    text: "done".to_string(),
                }],
            )
        };

        Ok(ChatResponse {
            message,
            usage: TokenUsage { input: 10, output: 5 },
            id: format!("test-{count}"),
        })
    }

    fn model_name(&self) -> &str {
        "two-step-mock"
    }

    fn max_context_size(&self) -> usize {
        200_000
    }

    fn capabilities(&self) -> HashSet<ModelCapability> {
        let mut caps = HashSet::new();
        caps.insert(ModelCapability::Text);
        caps.insert(ModelCapability::ToolUse);
        caps
    }

    fn thinking_effort(&self) -> Option<&str> {
        None
    }
}

#[tokio::test]
#[ignore = "requires run_turn + continue_turn implementation — Phase B"]
async fn reactor_run_turn_not_yet_implemented() {
    // O2: Turn loop completeness oracle.
    // This test expects run_turn to orchestrate the full loop.
    // It will FAIL (panic on todo!) until run_turn is implemented.
    let mut logic = ArconautLogic::new(ReactorConfig::default());
    let provider = TwoStepProvider::new();
    let mut soul = DefaultSoul::new(Box::new(provider), SoulConfig::default());

    let result = logic.run_turn(&mut soul, "do something").await;

    // Once implemented, this should succeed and:
    // - execute exactly one tool call (echo)
    // - return the second response ("done")
    // - steps_taken == 2
    assert!(result.is_ok(), "run_turn should succeed after implementation");
    let output = result.unwrap();
    assert_eq!(output.steps_taken, 2);
    assert!(output.completed);
    assert_eq!(output.stop_reason, StopReason::Completed);
}
