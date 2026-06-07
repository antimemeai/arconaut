//! Shared OpenAI-compatible HTTP client.
//!
//! Used by OpenAI, Moonshot, and OpenRouter providers.

use crate::{
    ChatProvider, ChatRequest, ChatResponse, ModelCapability, ProviderError, TokenUsage,
};
use arconaut_core::{ContentPart, Message, Role};
use async_trait::async_trait;
use reqwest::header::{self, HeaderMap, HeaderValue};
use serde::{Deserialize, Serialize};
use serde_json::Value;
use std::collections::HashSet;
use std::time::Duration;

const DEFAULT_TIMEOUT: Duration = Duration::from_secs(60);

/// HTTP client for OpenAI-compatible APIs.
///
/// Configurable base URL, API key, and extra headers.
/// All brand-specific OpenAI-compatible providers wrap this.
pub struct OpenAiCompatClient {
    client: reqwest::Client,
    api_key: String,
    base_url: String,
    model: String,
    extra_headers: HeaderMap,
}

impl std::fmt::Debug for OpenAiCompatClient {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        f.debug_struct("OpenAiCompatClient")
            .field("base_url", &self.base_url)
            .field("model", &self.model)
            .field("api_key", &"[REDACTED]")
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

impl OpenAiCompatClient {
    pub fn new(api_key: impl Into<String>, base_url: impl Into<String>) -> Result<Self, BuildError> {
        let mut headers = HeaderMap::new();
        headers.insert(
            header::CONTENT_TYPE,
            HeaderValue::from_static("application/json"),
        );

        let client = reqwest::Client::builder()
            .default_headers(headers)
            .timeout(DEFAULT_TIMEOUT)
            .build()
            .map_err(|e| BuildError::InvalidClient(e.to_string()))?;

        Ok(Self {
            client,
            api_key: api_key.into(),
            base_url: base_url.into(),
            model: "gpt-4o".to_string(),
            extra_headers: HeaderMap::new(),
        })
    }

    pub fn with_model(mut self, model: impl Into<String>) -> Self {
        self.model = model.into();
        self
    }

    pub fn with_base_url(mut self, base_url: impl Into<String>) -> Self {
        self.base_url = base_url.into();
        self
    }

    pub fn with_extra_header(
        mut self,
        key: impl AsRef<str>,
        value: impl AsRef<str>,
    ) -> Result<Self, BuildError> {
        let key = HeaderName::from_bytes(key.as_ref().as_bytes())
            .map_err(|e| BuildError::InvalidClient(e.to_string()))?;
        let value = HeaderValue::from_str(value.as_ref())
            .map_err(|e| BuildError::InvalidClient(e.to_string()))?;
        self.extra_headers.insert(key, value);
        Ok(self)
    }

    pub fn model(&self) -> &str {
        &self.model
    }

    pub fn base_url(&self) -> &str {
        &self.base_url
    }

    /// Update the API key (Bearer token) used for authentication.
    ///
    /// This is used by OAuth-enabled providers to inject a fresh access token
    /// before each request without reconstructing the client.
    pub fn set_api_key(&mut self, api_key: impl Into<String>) {
        self.api_key = api_key.into();
    }
}

use reqwest::header::HeaderName;

#[async_trait]
impl ChatProvider for OpenAiCompatClient {
    async fn chat(&self, request: ChatRequest) -> Result<ChatResponse, ProviderError> {
        let url = format!("{}chat/completions", self.base_url.trim_end_matches('/'));

        let messages: Vec<OpenAiMessage> = request.messages.into_iter().map(|m| m.into()).collect();

        let tools: Option<Vec<OpenAiTool>> = if request.tools.is_empty() {
            None
        } else {
            Some(
                request
                    .tools
                    .into_iter()
                    .map(|t| OpenAiTool {
                        r#type: "function".to_string(),
                        function: OpenAiFunctionTool {
                            name: t.name,
                            description: t.description,
                            parameters: t.parameters,
                        },
                    })
                    .collect(),
            )
        };

        let body = OpenAiRequest {
            model: self.model.clone(),
            messages,
            tools,
        };

        let mut req = self
            .client
            .post(&url)
            .bearer_auth(&self.api_key)
            .json(&body);

        for (key, value) in &self.extra_headers {
            req = req.header(key, value);
        }

        let response = req.send().await.map_err(|e| ProviderError::Network {
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

        let openai_resp: OpenAiResponse = response.json().await.map_err(|e| ProviderError::Other {
            message: format!("failed to parse response: {}", e),
        })?;

        let choice = openai_resp.choices.into_iter().next().ok_or_else(|| ProviderError::Other {
            message: "no choices in response".to_string(),
        })?;

        let content: Vec<ContentPart> = choice
            .message
            .content
            .map(|text| vec![ContentPart::text(text)])
            .unwrap_or_default();

        // TODO: tool_calls parsing

        let message = Message::new(Role::Assistant, content);

        Ok(ChatResponse {
            message,
            usage: TokenUsage {
                input: openai_resp.usage.prompt_tokens,
                output: openai_resp.usage.completion_tokens,
            },
            id: openai_resp.id,
        })
    }

    fn model_name(&self) -> &str {
        &self.model
    }

    fn max_context_size(&self) -> usize {
        128_000
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
struct OpenAiRequest {
    model: String,
    messages: Vec<OpenAiMessage>,
    #[serde(skip_serializing_if = "Option::is_none")]
    tools: Option<Vec<OpenAiTool>>,
}

#[derive(Serialize, Deserialize, Debug, Clone)]
struct OpenAiMessage {
    role: String,
    #[serde(skip_serializing_if = "Option::is_none")]
    content: Option<String>,
}

impl From<Message> for OpenAiMessage {
    fn from(msg: Message) -> Self {
        let role = match msg.role {
            Role::System => "system",
            Role::User => "user",
            Role::Assistant => "assistant",
            Role::Tool => "user", // tool results go as user messages
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

        let content = if content.is_empty() { None } else { Some(content) };
        Self { role, content }
    }
}

#[derive(Serialize, Deserialize, Debug, Clone)]
struct OpenAiTool {
    r#type: String,
    function: OpenAiFunctionTool,
}

#[derive(Serialize, Deserialize, Debug, Clone)]
struct OpenAiFunctionTool {
    name: String,
    description: String,
    parameters: Value,
}

#[derive(Deserialize, Debug)]
struct OpenAiResponse {
    id: String,
    choices: Vec<OpenAiChoice>,
    usage: OpenAiUsage,
}

#[derive(Deserialize, Debug)]
struct OpenAiChoice {
    message: OpenAiChoiceMessage,
}

#[derive(Deserialize, Debug)]
struct OpenAiChoiceMessage {
    content: Option<String>,
    // TODO: tool_calls
}

#[derive(Deserialize, Debug)]
struct OpenAiUsage {
    prompt_tokens: usize,
    completion_tokens: usize,
}

#[cfg(test)]
mod tests {
    use super::*;
    use arconaut_core::ContentPart;

    #[test]
    fn client_constructs_with_defaults() {
        let client = OpenAiCompatClient::new("test-key", "https://api.openai.com/v1").unwrap();
        assert_eq!(client.model(), "gpt-4o");
        assert_eq!(client.base_url(), "https://api.openai.com/v1");
    }

    #[test]
    fn client_with_model_override() {
        let client = OpenAiCompatClient::new("test-key", "https://api.openai.com/v1")
            .unwrap()
            .with_model("gpt-4o-mini");
        assert_eq!(client.model(), "gpt-4o-mini");
    }

    #[test]
    fn client_debug_redacts_key() {
        let client = OpenAiCompatClient::new("secret123", "https://api.openai.com/v1").unwrap();
        let debug = format!("{:?}", client);
        assert!(!debug.contains("secret123"));
        assert!(debug.contains("[REDACTED]"));
    }

    #[test]
    fn message_conversion_user() {
        let msg = Message::user("hello");
        let om: OpenAiMessage = msg.into();
        assert_eq!(om.role, "user");
        assert_eq!(om.content, Some("hello".to_string()));
    }

    #[test]
    fn message_conversion_assistant() {
        let msg = Message::assistant("hi there");
        let om: OpenAiMessage = msg.into();
        assert_eq!(om.role, "assistant");
        assert_eq!(om.content, Some("hi there".to_string()));
    }

    #[test]
    fn message_conversion_system() {
        let msg = Message::system("you are helpful");
        let om: OpenAiMessage = msg.into();
        assert_eq!(om.role, "system");
        assert_eq!(om.content, Some("you are helpful".to_string()));
    }

    #[test]
    fn message_conversion_tool_result() {
        let msg = Message::tool_result(
            "call-1",
            arconaut_core::ToolResult::success(vec![ContentPart::text("file contents here")]),
        );
        let om: OpenAiMessage = msg.into();
        assert_eq!(om.role, "user");
        assert_eq!(om.content, Some("file contents here".to_string()));
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
    fn error_classification_context_overflow() {
        let err = classify_error(
            reqwest::StatusCode::BAD_REQUEST,
            "context length exceeded".to_string(),
        );
        assert!(matches!(err, ProviderError::ContextOverflow { .. }));
    }

    #[test]
    fn request_serialization_shape() {
        let req = OpenAiRequest {
            model: "gpt-4o".to_string(),
            messages: vec![OpenAiMessage {
                role: "user".to_string(),
                content: Some("hello".to_string()),
            }],
            tools: None,
        };
        let json = serde_json::to_value(&req).unwrap();
        assert_eq!(json["model"], "gpt-4o");
        assert_eq!(json["messages"][0]["role"], "user");
        assert_eq!(json["messages"][0]["content"], "hello");
        assert!(json.get("tools").is_none());
    }

    #[test]
    fn request_serialization_with_tools() {
        let req = OpenAiRequest {
            model: "gpt-4o".to_string(),
            messages: vec![],
            tools: Some(vec![OpenAiTool {
                r#type: "function".to_string(),
                function: OpenAiFunctionTool {
                    name: "read".to_string(),
                    description: "read a file".to_string(),
                    parameters: serde_json::json!({"type": "object"}),
                },
            }]),
        };
        let json = serde_json::to_value(&req).unwrap();
        assert!(json.get("tools").is_some());
        assert_eq!(json["tools"][0]["type"], "function");
    }

    #[test]
    fn response_deserialization() {
        let json = r#"{
            "id": "chatcmpl-123",
            "choices": [{"message": {"content": "hello"}}],
            "usage": {"prompt_tokens": 10, "completion_tokens": 5}
        }"#;
        let resp: OpenAiResponse = serde_json::from_str(json).unwrap();
        assert_eq!(resp.id, "chatcmpl-123");
        assert_eq!(resp.choices[0].message.content, Some("hello".to_string()));
        assert_eq!(resp.usage.prompt_tokens, 10);
        assert_eq!(resp.usage.completion_tokens, 5);
    }
}
