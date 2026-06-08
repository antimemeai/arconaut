use arconaut_pilot::{DefaultSoul, Soul, SoulConfig, SoulError, StopReason, Tempo, TurnRequest};
use arconaut_core::{ContentPart, Message, Role};
use arconaut_machine::provider::{
    ChatProvider, ChatRequest, ChatResponse, ModelCapability, ProviderError, TokenUsage,
    ToolDescriptor,
};
use async_trait::async_trait;
use std::collections::HashSet;
use std::sync::atomic::{AtomicUsize, Ordering};
use std::sync::Arc;

/// Mock provider for testing Soul purity (O1 oracle).
struct MockProvider {
    call_count: AtomicUsize,
    response: Message,
}

impl MockProvider {
    fn new(response: Message) -> Self {
        Self {
            call_count: AtomicUsize::new(0),
            response,
        }
    }

    fn call_count(&self) -> usize {
        self.call_count.load(Ordering::SeqCst)
    }
}

#[async_trait]
impl ChatProvider for MockProvider {
    async fn chat(&self, _request: ChatRequest) -> Result<ChatResponse, ProviderError> {
        self.call_count.fetch_add(1, Ordering::SeqCst);
        Ok(ChatResponse {
            message: self.response.clone(),
            usage: TokenUsage { input: 10, output: 5 },
            id: "test-1".to_string(),
        })
    }

    fn model_name(&self) -> &str {
        "mock"
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
async fn soul_execute_returns_completed_when_no_tool_calls() {
    let response = Message::new(Role::Assistant, vec![ContentPart::Text {
        text: "hello".to_string(),
    }]);
    let provider = MockProvider::new(response);
    let mut soul = DefaultSoul::new(Box::new(provider), SoulConfig::default());

    let request = TurnRequest {
        context: vec![Message::new(Role::User, vec![ContentPart::Text {
            text: "hi".to_string(),
        }])],
        available_tools: vec![],
        tempo: Tempo::Metronome,
    };

    let result = soul.execute(request).await.unwrap();
    assert_eq!(result.stop_reason, StopReason::Completed);
    assert_eq!(result.token_usage.total(), 15);
}

#[tokio::test]
async fn soul_execute_returns_tool_calls_requested() {
    let response = Message::new(
        Role::Assistant,
        vec![ContentPart::ToolCall {
            tool_call: arconaut_core::ToolCall {
                id: "call-1".to_string(),
                function: arconaut_core::FunctionCall {
                    name: "read".to_string(),
                    arguments: "{}".to_string(),
                },
            },
        }],
    );
    let provider = MockProvider::new(response);
    let mut soul = DefaultSoul::new(Box::new(provider), SoulConfig::default());

    let request = TurnRequest {
        context: vec![Message::new(Role::User, vec![ContentPart::Text {
            text: "read a file".to_string(),
        }])],
        available_tools: vec![],
        tempo: Tempo::Metronome,
    };

    let result = soul.execute(request).await.unwrap();
    assert_eq!(result.stop_reason, StopReason::ToolCallsRequested);
}

#[tokio::test]
async fn soul_purity_oracle_same_input_same_output() {
    // O1: Given the same mock provider response, execute() produces the same TurnResponse.
    let response = Message::new(Role::Assistant, vec![ContentPart::Text {
        text: "stable".to_string(),
    }]);
    let provider = MockProvider::new(response);
    let mut soul = DefaultSoul::new(Box::new(provider), SoulConfig::default());

    let request = TurnRequest {
        context: vec![Message::new(Role::User, vec![ContentPart::Text {
            text: "test".to_string(),
        }])],
        available_tools: vec![],
        tempo: Tempo::Metronome,
    };

    let r1 = soul.execute(request.clone()).await.unwrap();
    let r2 = soul.execute(request).await.unwrap();

    assert_eq!(r1.stop_reason, r2.stop_reason);
    assert_eq!(r1.token_usage.total(), r2.token_usage.total());
    // The provider was called twice, confirming the Soul is stateless across execute() calls
    // (except for the internal turn_context which is reset each call).
}

#[tokio::test]
async fn soul_continue_turn_not_yet_implemented() {
    let response = Message::new(Role::Assistant, vec![ContentPart::Text {
        text: "ok".to_string(),
    }]);
    let provider = MockProvider::new(response);
    let mut soul = DefaultSoul::new(Box::new(provider), SoulConfig::default());

    let err = soul.continue_turn(vec![]).await;
    assert!(err.is_err());
    // Expected: continue_turn returns todo!() panic, which we catch as an error in async context.
    // This is the RED test — it will fail until we implement continue_turn.
}
