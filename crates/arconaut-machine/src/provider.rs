use arconaut_core::Message;
use async_trait::async_trait;
use serde_json::Value;
use std::collections::HashSet;

#[async_trait]
pub trait ChatProvider: Send + Sync {
    async fn chat(&self, request: ChatRequest) -> Result<ChatResponse, ProviderError>;
    fn model_name(&self) -> &str;
    fn max_context_size(&self) -> usize;
    fn capabilities(&self) -> HashSet<ModelCapability>;
    fn thinking_effort(&self) -> Option<&str>;
}

/// Lightweight descriptor of a tool for LLM API consumption.
/// The provider only needs name, description, and parameter schema —
/// it never calls the tool directly.
#[derive(Debug, Clone)]
pub struct ToolDescriptor {
    pub name: String,
    pub description: String,
    pub parameters: Value,
}

impl ToolDescriptor {
    pub fn new(name: impl Into<String>, description: impl Into<String>, parameters: Value) -> Self {
        Self {
            name: name.into(),
            description: description.into(),
            parameters,
        }
    }
}

pub struct ChatRequest {
    pub messages: Vec<Message>,
    pub tools: Vec<ToolDescriptor>,
    pub system_prompt: Option<String>,
}

impl std::fmt::Debug for ChatRequest {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        f.debug_struct("ChatRequest")
            .field("messages", &self.messages)
            .field("tools", &self.tools.len())
            .field("system_prompt", &self.system_prompt)
            .finish()
    }
}

#[derive(Debug, Clone)]
pub struct ChatResponse {
    pub message: Message,
    pub usage: TokenUsage,
    pub id: String,
}

#[derive(Debug, Clone, Copy, PartialEq)]
pub struct TokenUsage {
    pub input: usize,
    pub output: usize,
}

#[derive(Debug, Clone, PartialEq)]
pub enum ProviderError {
    RateLimit { status_code: u16, message: String },
    Auth { status_code: u16, message: String },
    Network { message: String },
    ContextOverflow { message: String },
    Server { status_code: u16, message: String },
    Client { status_code: u16, message: String },
    Other { message: String },
}

impl std::fmt::Display for ProviderError {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            ProviderError::RateLimit {
                status_code,
                message,
            } => {
                write!(f, "rate limit ({}): {}", status_code, message)
            }
            ProviderError::Auth {
                status_code,
                message,
            } => {
                write!(f, "auth error ({}): {}", status_code, message)
            }
            ProviderError::Network { message } => {
                write!(f, "network error: {}", message)
            }
            ProviderError::ContextOverflow { message } => {
                write!(f, "context overflow: {}", message)
            }
            ProviderError::Server {
                status_code,
                message,
            } => {
                write!(f, "server error ({}): {}", status_code, message)
            }
            ProviderError::Client {
                status_code,
                message,
            } => {
                write!(f, "client error ({}): {}", status_code, message)
            }
            ProviderError::Other { message } => {
                write!(f, "{}", message)
            }
        }
    }
}

impl std::error::Error for ProviderError {}

#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
pub enum ModelCapability {
    Text,
    Images,
    ToolUse,
    Thinking,
}

impl TokenUsage {
    pub fn total(&self) -> usize {
        self.input + self.output
    }
}
