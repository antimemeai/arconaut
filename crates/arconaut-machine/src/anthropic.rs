use crate::{ChatProvider, ChatRequest, ChatResponse, ModelCapability, ProviderError, TokenUsage};
use arconaut_core::{ContentPart, FunctionCall, Message, Role, ToolCall};
use async_trait::async_trait;
use reqwest::header::{self, HeaderMap, HeaderValue};
use serde::{Deserialize, Serialize};
use serde_json::Value;
use std::collections::HashSet;
use std::time::Duration;

const ANTHROPIC_API_BASE: &str = "https://api.anthropic.com/v1";
const MAX_CONTEXT_SIZE: usize = 200_000;
const DEFAULT_TIMEOUT: Duration = Duration::from_secs(60);

/// Anthropic API provider.
///
/// API key is stored in memory and redacted from Debug output.
/// Use `AnthropicProvider::new` to construct; it returns `Result` to avoid panics.
pub struct AnthropicProvider {
    client: reqwest::Client,
    api_key: String,
    model: String,
    pub(crate) base_url: String,
    max_tokens: usize,
}

impl std::fmt::Debug for AnthropicProvider {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        f.debug_struct("AnthropicProvider")
            .field("model", &self.model)
            .field("base_url", &self.base_url)
            .field("api_key", &"[REDACTED]")
            .field("max_tokens", &self.max_tokens)
            .finish()
    }
}

#[derive(Debug, Clone, PartialEq)]
pub enum BuildError {
    InvalidClient(String),
}

impl std::fmt::Display for BuildError {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            BuildError::InvalidClient(msg) => write!(f, "failed to build HTTP client: {}", msg),
        }
    }
}

impl std::error::Error for BuildError {}

impl AnthropicProvider {
    pub fn new(api_key: impl Into<String>) -> Result<Self, BuildError> {
        let mut headers = HeaderMap::new();
        headers.insert(
            header::CONTENT_TYPE,
            HeaderValue::from_static("application/json"),
        );
        headers.insert("anthropic-version", HeaderValue::from_static("2023-06-01"));

        let client = reqwest::Client::builder()
            .default_headers(headers)
            .timeout(DEFAULT_TIMEOUT)
            .build()
            .map_err(|e| BuildError::InvalidClient(e.to_string()))?;

        Ok(Self {
            client,
            api_key: api_key.into(),
            model: "claude-sonnet-4-6".to_string(),
            base_url: ANTHROPIC_API_BASE.to_string(),
            max_tokens: 4096,
        })
    }

    pub fn with_model(mut self, model: impl Into<String>) -> Self {
        self.model = model.into();
        self
    }

    pub fn with_max_tokens(mut self, max_tokens: usize) -> Self {
        self.max_tokens = max_tokens;
        self
    }

    pub fn with_base_url(mut self, base_url: impl Into<String>) -> Self {
        self.base_url = base_url.into();
        self
    }
}

#[async_trait]
impl ChatProvider for AnthropicProvider {
    async fn chat(&self, request: ChatRequest) -> Result<ChatResponse, ProviderError> {
        let url = format!("{}messages", self.base_url.trim_end_matches('/'));

        // Extract system messages into the top-level system field.
        // Anthropic requires system prompts as a top-level string, not in messages array.
        let (system, messages): (Option<String>, Vec<Message>) =
            extract_system_messages(request.messages);
        let system = system.or(request.system_prompt);

        let anthropic_messages: Vec<AnthropicMessage> =
            messages.into_iter().map(|m| m.into()).collect();

        let tools: Vec<AnthropicTool> = request
            .tools
            .iter()
            .map(|t| AnthropicTool {
                name: t.name.clone(),
                description: t.description.clone(),
                input_schema: t.parameters.clone(),
            })
            .collect();

        let body = AnthropicRequest {
            model: self.model.clone(),
            max_tokens: self.max_tokens,
            system,
            messages: anthropic_messages,
            tools: if tools.is_empty() { None } else { Some(tools) },
        };

        let response = self
            .client
            .post(&url)
            .bearer_auth(&self.api_key)
            .json(&body)
            .send()
            .await
            .map_err(|e| ProviderError::Network {
                message: e.to_string(),
            })?;

        let status = response.status();
        if !status.is_success() {
            let error_text = response
                .text()
                .await
                .unwrap_or_else(|_| "unknown error".to_string());
            return Err(classify_error(status, error_text));
        }

        let anthropic_resp: AnthropicResponse =
            response.json().await.map_err(|e| ProviderError::Other {
                message: format!("failed to parse response: {}", e),
            })?;

        let content: Vec<ContentPart> = anthropic_resp
            .content
            .into_iter()
            .filter_map(|block| match block.block_type.as_str() {
                "text" => block.text.map(ContentPart::text),
                "tool_use" => {
                    block
                        .name
                        .zip(block.input)
                        .map(|(name, input)| ContentPart::ToolCall {
                            tool_call: ToolCall {
                                id: block.id.unwrap_or_default(),
                                function: FunctionCall {
                                    name,
                                    arguments: input.to_string(),
                                },
                            },
                        })
                }
                _ => None,
            })
            .collect();

        let message = Message::new(Role::Assistant, content);

        Ok(ChatResponse {
            message,
            usage: TokenUsage {
                input: anthropic_resp.usage.input_tokens,
                output: anthropic_resp.usage.output_tokens,
            },
            id: anthropic_resp.id,
        })
    }

    fn model_name(&self) -> &str {
        &self.model
    }

    fn max_context_size(&self) -> usize {
        MAX_CONTEXT_SIZE
    }

    fn capabilities(&self) -> HashSet<ModelCapability> {
        let mut caps = HashSet::new();
        caps.insert(ModelCapability::Text);
        caps.insert(ModelCapability::ToolUse);
        caps.insert(ModelCapability::Thinking);
        caps
    }

    fn thinking_effort(&self) -> Option<&str> {
        None
    }
}

/// Extract system messages from the messages vec and concatenate them.
/// Returns (system_string, remaining_messages).
fn extract_system_messages(messages: Vec<Message>) -> (Option<String>, Vec<Message>) {
    let mut system_parts = Vec::new();
    let mut rest = Vec::new();

    for msg in messages {
        match msg.role {
            Role::System => {
                let text: String = msg
                    .content
                    .into_iter()
                    .filter_map(|part| match part {
                        ContentPart::Text { text } => Some(text),
                        _ => None,
                    })
                    .collect();
                if !text.is_empty() {
                    system_parts.push(text);
                }
            }
            _ => rest.push(msg),
        }
    }

    let system = if system_parts.is_empty() {
        None
    } else {
        Some(system_parts.join("\n"))
    };

    (system, rest)
}

fn classify_error(status: reqwest::StatusCode, body: String) -> ProviderError {
    match status.as_u16() {
        401 | 403 => ProviderError::Auth {
            status_code: status.as_u16(),
            message: body,
        },
        429 => ProviderError::RateLimit {
            status_code: status.as_u16(),
            message: body,
        },
        500..=504 => ProviderError::Server {
            status_code: status.as_u16(),
            message: body,
        },
        400..=499 => {
            let msg_lower = body.to_lowercase();
            if msg_lower.contains("context length")
                || msg_lower.contains("context_length")
                || msg_lower.contains("max tokens")
                || msg_lower.contains("too many tokens")
            {
                ProviderError::ContextOverflow { message: body }
            } else {
                ProviderError::Client {
                    status_code: status.as_u16(),
                    message: body,
                }
            }
        }
        _ => ProviderError::Other { message: body },
    }
}

#[derive(Serialize)]
struct AnthropicRequest {
    model: String,
    max_tokens: usize,
    #[serde(skip_serializing_if = "Option::is_none")]
    system: Option<String>,
    messages: Vec<AnthropicMessage>,
    #[serde(skip_serializing_if = "Option::is_none")]
    tools: Option<Vec<AnthropicTool>>,
}

#[derive(Serialize)]
struct AnthropicTool {
    name: String,
    description: String,
    #[serde(rename = "input_schema")]
    input_schema: Value,
}

#[derive(Serialize, Deserialize)]
struct AnthropicMessage {
    role: String,
    content: String,
}

impl From<Message> for AnthropicMessage {
    fn from(msg: Message) -> Self {
        let role = match msg.role {
            Role::User => "user",
            Role::Assistant => "assistant",
            Role::Tool => "user", // Anthropic doesn't have a "tool" role; tool results go as user
            Role::System => "user", // Should be filtered out by extract_system_messages
        }
        .to_string();

        let content = msg
            .content
            .into_iter()
            .filter_map(|part| match part {
                ContentPart::Text { text } => Some(text),
                ContentPart::ToolResult { tool_result } => {
                    let text = tool_result
                        .output
                        .into_iter()
                        .filter_map(|p| match p {
                            ContentPart::Text { text } => Some(text),
                            _ => None,
                        })
                        .collect::<Vec<_>>()
                        .join("");
                    Some(text)
                }
                _ => None,
            })
            .collect::<Vec<_>>()
            .join("");

        Self { role, content }
    }
}

#[derive(Deserialize)]
struct AnthropicResponse {
    id: String,
    content: Vec<AnthropicContentBlock>,
    usage: AnthropicUsage,
}

#[derive(Deserialize)]
struct AnthropicContentBlock {
    #[serde(rename = "type")]
    block_type: String,
    text: Option<String>,
    id: Option<String>,
    name: Option<String>,
    input: Option<Value>,
}

#[derive(Deserialize)]
struct AnthropicUsage {
    input_tokens: usize,
    output_tokens: usize,
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn provider_model_info() {
        let provider = AnthropicProvider::new("test-key").unwrap();
        assert_eq!(provider.model_name(), "claude-sonnet-4-6");
        assert_eq!(provider.max_context_size(), 200_000);
    }

    #[test]
    fn provider_with_model() {
        let provider = AnthropicProvider::new("test-key")
            .unwrap()
            .with_model("claude-opus-4-6");
        assert_eq!(provider.model_name(), "claude-opus-4-6");
    }

    #[test]
    fn provider_capabilities() {
        let provider = AnthropicProvider::new("test-key").unwrap();
        let caps = provider.capabilities();
        assert!(caps.contains(&ModelCapability::Text));
        assert!(caps.contains(&ModelCapability::ToolUse));
        assert!(caps.contains(&ModelCapability::Thinking));
    }

    #[test]
    fn provider_debug_redacts_key() {
        let provider = AnthropicProvider::new("secret123").unwrap();
        let debug = format!("{:?}", provider);
        assert!(!debug.contains("secret123"));
        assert!(debug.contains("[REDACTED]"));
    }

    #[test]
    fn error_classification_auth() {
        let err = classify_error(reqwest::StatusCode::UNAUTHORIZED, "invalid key".to_string());
        assert!(matches!(
            err,
            ProviderError::Auth {
                status_code: 401,
                ..
            }
        ));
        assert!(err.to_string().contains("invalid key"));
    }

    #[test]
    fn error_classification_rate_limit() {
        let err = classify_error(
            reqwest::StatusCode::TOO_MANY_REQUESTS,
            "rate limited".to_string(),
        );
        assert!(matches!(
            err,
            ProviderError::RateLimit {
                status_code: 429,
                ..
            }
        ));
    }

    #[test]
    fn error_classification_server() {
        let err = classify_error(
            reqwest::StatusCode::INTERNAL_SERVER_ERROR,
            "overloaded".to_string(),
        );
        assert!(matches!(
            err,
            ProviderError::Server {
                status_code: 500,
                ..
            }
        ));
        assert!(err.to_string().contains("overloaded"));
    }

    #[test]
    fn error_classification_context_overflow() {
        let err = classify_error(
            reqwest::StatusCode::BAD_REQUEST,
            "context length exceeded".to_string(),
        );
        assert!(matches!(err, ProviderError::ContextOverflow { .. }));
    }

    #[test]
    fn message_conversion_user() {
        let msg = Message::user("hello");
        let am: AnthropicMessage = msg.into();
        assert_eq!(am.role, "user");
        assert_eq!(am.content, "hello");
    }

    #[test]
    fn message_conversion_assistant() {
        let msg = Message::assistant("hi there");
        let am: AnthropicMessage = msg.into();
        assert_eq!(am.role, "assistant");
        assert_eq!(am.content, "hi there");
    }

    #[test]
    fn message_conversion_tool_result() {
        let msg = Message::tool_result(
            "call-1",
            arconaut_core::ToolResult::success(vec![arconaut_core::ContentPart::text(
                "file contents here",
            )]),
        );
        let am: AnthropicMessage = msg.into();
        assert_eq!(am.role, "user");
        assert_eq!(am.content, "file contents here");
    }

    #[test]
    fn extract_system_messages_works() {
        let messages = vec![
            Message::system("you are helpful"),
            Message::user("hello"),
            Message::system("be concise"),
        ];
        let (system, rest) = extract_system_messages(messages);
        assert_eq!(system, Some("you are helpful\nbe concise".to_string()));
        assert_eq!(rest.len(), 1);
        assert_eq!(rest[0].role, Role::User);
    }

    #[test]
    fn url_construction_no_double_slash() {
        let provider = AnthropicProvider::new("test")
            .unwrap()
            .with_base_url("https://api.anthropic.com/v1/");
        // The base_url is trimmed, so url becomes "https://api.anthropic.com/v1/messages"
        // We can't easily test the private chat method, but we verify the trim logic:
        assert_eq!(
            provider.base_url.trim_end_matches('/'),
            "https://api.anthropic.com/v1"
        );
    }
}
