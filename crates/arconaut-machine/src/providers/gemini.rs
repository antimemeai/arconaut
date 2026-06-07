use crate::{
    ChatProvider, ChatRequest, ChatResponse, ModelCapability, ProviderError,
};
use async_trait::async_trait;
use reqwest::header::{self, HeaderMap, HeaderValue};
use std::collections::HashSet;
use std::time::Duration;

const GEMINI_API_BASE: &str = "https://generativelanguage.googleapis.com/v1beta";
const DEFAULT_TIMEOUT: Duration = Duration::from_secs(60);

/// Google Gemini brand-specific provider.
///
/// Uses the native Gemini API (not OpenAI-compatible).
#[allow(dead_code)]
pub struct GeminiProvider {
    client: reqwest::Client,
    api_key: String,
    model: String,
    base_url: String,
}

impl std::fmt::Debug for GeminiProvider {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        f.debug_struct("GeminiProvider")
            .field("model", &self.model)
            .field("base_url", &self.base_url)
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

impl GeminiProvider {
    pub fn new(api_key: impl Into<String>) -> Result<Self, BuildError> {
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
            model: "gemini-1.5-pro".to_string(),
            base_url: GEMINI_API_BASE.to_string(),
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
}

#[async_trait]
impl ChatProvider for GeminiProvider {
    async fn chat(&self, _request: ChatRequest) -> Result<ChatResponse, ProviderError> {
        // TODO: Implement Gemini native API
        // Gemini uses `models/{model}:generateContent` endpoint
        // Content format is different from OpenAI/Anthropic
        Err(ProviderError::Other {
            message: "Gemini provider not yet implemented".to_string(),
        })
    }

    fn model_name(&self) -> &str {
        &self.model
    }

    fn max_context_size(&self) -> usize {
        1_000_000 // Gemini 1.5 Pro has 1M token context
    }

    fn capabilities(&self) -> HashSet<ModelCapability> {
        let mut caps = HashSet::new();
        caps.insert(ModelCapability::Text);
        caps.insert(ModelCapability::Images);
        caps.insert(ModelCapability::ToolUse);
        caps
    }

    fn thinking_effort(&self) -> Option<&str> {
        None
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn default_base_url() {
        let provider = GeminiProvider::new("test-key").unwrap();
        let debug = format!("{:?}", provider);
        assert!(debug.contains("https://generativelanguage.googleapis.com/v1beta"));
    }

    #[test]
    fn default_model() {
        let provider = GeminiProvider::new("test-key").unwrap();
        assert_eq!(provider.model_name(), "gemini-1.5-pro");
    }

    #[test]
    fn with_model_override() {
        let provider = GeminiProvider::new("test-key")
            .unwrap()
            .with_model("gemini-1.5-flash");
        assert_eq!(provider.model_name(), "gemini-1.5-flash");
    }

    #[test]
    fn provider_debug_redacts_key() {
        let provider = GeminiProvider::new("secret123").unwrap();
        let debug = format!("{:?}", provider);
        assert!(!debug.contains("secret123"));
        assert!(debug.contains("[REDACTED]"));
    }

    #[test]
    fn capabilities_include_images() {
        let provider = GeminiProvider::new("test-key").unwrap();
        let caps = provider.capabilities();
        assert!(caps.contains(&ModelCapability::Images));
    }
}
