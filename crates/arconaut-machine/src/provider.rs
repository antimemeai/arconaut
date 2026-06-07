use arconaut_core::{Message, Tool};
use async_trait::async_trait;
use serde::Serialize;
use std::collections::HashSet;

#[async_trait]
pub trait ChatProvider: Send + Sync {
    async fn chat(&self, request: ChatRequest) -> Result<ChatResponse, ProviderError>;
    fn model_name(&self) -> &str;
    fn max_context_size(&self) -> usize;
    fn capabilities(&self) -> HashSet<ModelCapability>;
    fn thinking_effort(&self) -> Option<&str>;
}

pub struct ChatRequest {
    pub messages: Vec<Message>,
    pub tools: Vec<Box<dyn Tool>>,
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
    RateLimit { status_code: u16 },
    Auth { status_code: u16 },
    Network { message: String },
    ContextOverflow { message: String },
    Server { status_code: u16, message: String },
    Client { status_code: u16, message: String },
    Other { message: String },
}

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
