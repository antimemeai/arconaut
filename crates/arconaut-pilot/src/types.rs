use arconaut_core::Message;
use arconaut_machine::provider::TokenUsage;
use serde_json::Value;

/// The pilot's view of what the reactor assembles.
#[derive(Debug, Clone)]
pub struct TurnRequest {
    pub context: Vec<Message>,
    pub available_tools: Vec<ToolDescriptor>,
    pub tempo: crate::Tempo,
}

/// Lightweight tool descriptor for the LLM API.
#[derive(Debug, Clone)]
pub struct ToolDescriptor {
    pub name: String,
    pub description: String,
    pub parameters: Value,
}

/// The pilot's response after one provider call.
#[derive(Debug, Clone)]
pub struct TurnResponse {
    pub message: Message,
    pub stop_reason: StopReason,
    pub token_usage: TokenUsage,
}

/// Reasons a turn step terminated.
#[derive(Debug, Clone, PartialEq)]
pub enum StopReason {
    Completed,
    ToolCallsRequested,
    MaxStepsReached,
    ChurnDetected,
    ProviderError(String),
}

/// Soul configuration.
#[derive(Debug, Clone)]
pub struct SoulConfig {
    pub max_steps: usize,
}

impl Default for SoulConfig {
    fn default() -> Self {
        Self { max_steps: 100 }
    }
}
