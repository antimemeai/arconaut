use crate::{ChatProvider, ChatRequest, ChatResponse, ModelCapability, ProviderError, TokenUsage};
use arconaut_core::{ContentPart, Message, Role};
use async_trait::async_trait;
use reqwest::header::{self, HeaderMap, HeaderValue};
use serde::{Deserialize, Serialize};
use std::collections::HashSet;

const ANTHROPIC_API_BASE: &str = "https://api.anthropic.com/v1";
const MAX_CONTEXT_SIZE: usize = 200_000;

pub struct AnthropicProvider {
    client: reqwest::Client,
    api_key: String,
    model: String,
    base_url: String,
}

impl AnthropicProvider {
    pub fn new(api_key: impl Into<String>) -> Self {
        let mut headers = HeaderMap::new();
        headers.insert(
            header::CONTENT_TYPE,
            HeaderValue::from_static("application/json"),
        );
        headers.insert(
            "anthropic-version",
            HeaderValue::from_static("2023-06-01"),
        );

        let client = reqwest::Client::builder()
            .default_headers(headers)
            .build()
            .expect("reqwest client build");

        Self {
            client,
            api_key: api_key.into(),
            model: "claude-sonnet-4-6".to_string(),
            base_url: ANTHROPIC_API_BASE.to_string(),
        }
    }

    pub fn with_model(mut self, model: impl Into<String>) -> Self {
        self.model = model.into();
        self
    }
}

#[async_trait]
impl ChatProvider for AnthropicProvider {
    async fn chat(&self, request: ChatRequest) -> Result<ChatResponse, ProviderError> {
        let url = format!("{}/messages", self.base_url);

        let messages: Vec<AnthropicMessage> = request
            .messages
            .into_iter()
            .map(|m| m.into())
            .collect();

        let body = AnthropicRequest {
            model: self.model.clone(),
            max_tokens: 4096,
            system: request.system_prompt,
            messages,
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

        let anthropic_resp: AnthropicResponse = response
            .json()
            .await
            .map_err(|e| ProviderError::Other {
                message: format!("failed to parse response: {}", e),
            })?;

        let content = anthropic_resp
            .content
            .into_iter()
            .map(|c| ContentPart::text(c.text))
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

fn classify_error(status: reqwest::StatusCode, body: String) -> ProviderError {
    match status.as_u16() {
        401 | 403 => ProviderError::Auth {
            status_code: status.as_u16(),
        },
        429 => ProviderError::RateLimit {
            status_code: status.as_u16(),
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
                ProviderError::ContextOverflow {
                    message: body,
                }
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
            _ => "user",
        }
        .to_string();

        let content = msg
            .content
            .into_iter()
            .map(|part| match part {
                ContentPart::Text { text } => text,
                _ => String::new(),
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
    _type: String,
    text: String,
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
        let provider = AnthropicProvider::new("test-key");
        assert_eq!(provider.model_name(), "claude-sonnet-4-6");
        assert_eq!(provider.max_context_size(), 200_000);
    }

    #[test]
    fn provider_with_model() {
        let provider = AnthropicProvider::new("test-key").with_model("claude-opus-4-6");
        assert_eq!(provider.model_name(), "claude-opus-4-6");
    }

    #[test]
    fn provider_capabilities() {
        let provider = AnthropicProvider::new("test-key");
        let caps = provider.capabilities();
        assert!(caps.contains(&ModelCapability::Text));
        assert!(caps.contains(&ModelCapability::ToolUse));
        assert!(caps.contains(&ModelCapability::Thinking));
    }

    #[test]
    fn error_classification_auth() {
        let err = classify_error(
            reqwest::StatusCode::UNAUTHORIZED,
            "invalid key".to_string(),
        );
        assert_eq!(
            err,
            ProviderError::Auth { status_code: 401 }
        );
    }

    #[test]
    fn error_classification_rate_limit() {
        let err = classify_error(
            reqwest::StatusCode::TOO_MANY_REQUESTS,
            "rate limited".to_string(),
        );
        assert_eq!(
            err,
            ProviderError::RateLimit { status_code: 429 }
        );
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
}
