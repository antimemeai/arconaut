use crate::types::{SoulConfig, StopReason, TurnRequest, TurnResponse};
use arconaut_core::{Message, ToolResult};
use arconaut_machine::provider::{ChatProvider, ChatRequest, ProviderError};
use async_trait::async_trait;

/// Soul v2 trait. The reactor drives the loop; the Soul is pure provider interaction.
#[async_trait]
pub trait Soul: Send + Sync {
    /// Execute one provider call. Does NOT execute tools.
    async fn execute(&mut self, request: TurnRequest) -> Result<TurnResponse, SoulError>;

    /// Signal that the reactor has executed tool results and wants to continue the turn.
    async fn continue_turn(
        &mut self,
        tool_results: Vec<ToolResult>,
    ) -> Result<TurnResponse, SoulError>;
}

/// Concrete Soul implementation.
pub struct DefaultSoul {
    provider: Box<dyn ChatProvider>,
    _config: SoulConfig,
    // Internal context accumulator for the current turn.
    turn_context: Vec<Message>,
}

impl DefaultSoul {
    pub fn new(provider: Box<dyn ChatProvider>, config: SoulConfig) -> Self {
        Self {
            provider,
            _config: config,
            turn_context: Vec::new(),
        }
    }

    async fn call_provider(&mut self, request: TurnRequest) -> Result<TurnResponse, SoulError> {
        let chat_request = ChatRequest {
            messages: request.context.clone(),
            tools: request
                .available_tools
                .into_iter()
                .map(|t| arconaut_machine::provider::ToolDescriptor::new(t.name, t.description, t.parameters))
                .collect(),
            system_prompt: None,
        };

        let response = self
            .provider
            .chat(chat_request)
            .await
            .map_err(SoulError::Provider)?;

        let stop_reason = if has_tool_calls(&response.message) {
            StopReason::ToolCallsRequested
        } else {
            StopReason::Completed
        };

        Ok(TurnResponse {
            message: response.message,
            stop_reason,
            token_usage: response.usage,
        })
    }
}

#[async_trait]
impl Soul for DefaultSoul {
    async fn execute(&mut self, request: TurnRequest) -> Result<TurnResponse, SoulError> {
        self.turn_context = request.context.clone();
        self.call_provider(request).await
    }

    async fn continue_turn(
        &mut self,
        _tool_results: Vec<ToolResult>,
    ) -> Result<TurnResponse, SoulError> {
        // TODO(Phase A): append tool results to turn_context and re-call provider.
        Err(SoulError::Serialization("continue_turn not yet implemented".to_string()))
    }
}

/// Soul-level errors.
#[derive(Debug, Clone, PartialEq)]
pub enum SoulError {
    Provider(ProviderError),
    Serialization(String),
    ContextOverflow { requested: usize, max: usize },
}

impl std::fmt::Display for SoulError {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            SoulError::Provider(e) => write!(f, "provider error: {e}"),
            SoulError::Serialization(e) => write!(f, "serialization error: {e}"),
            SoulError::ContextOverflow { requested, max } => {
                write!(f, "context overflow: {requested} > {max}")
            }
        }
    }
}

impl std::error::Error for SoulError {}

fn has_tool_calls(message: &Message) -> bool {
    message
        .content
        .iter()
        .any(|p| matches!(p, arconaut_core::ContentPart::ToolCall { .. }))
}
