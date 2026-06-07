use super::openai_compat::{BuildError, OpenAiCompatClient};
use crate::{ChatProvider, ProviderError};
use async_trait::async_trait;

const OPENROUTER_API_BASE: &str = "https://openrouter.ai/api/v1";
const DEFAULT_REFERER: &str = "https://github.com/antimemeai/arconaut";
const DEFAULT_TITLE: &str = "arconaut";

/// OpenRouter brand-specific provider.
///
/// Wraps `OpenAiCompatClient` with OpenRouter defaults and required headers.
pub struct OpenRouterProvider {
    inner: OpenAiCompatClient,
}

impl OpenRouterProvider {
    pub fn new(api_key: impl Into<String>) -> Result<Self, BuildError> {
        let inner = OpenAiCompatClient::new(api_key, OPENROUTER_API_BASE)?
            .with_model("anthropic/claude-sonnet-4")
            .with_extra_header("HTTP-Referer", DEFAULT_REFERER)
            .map_err(|e| BuildError::InvalidClient(e.to_string()))?
            .with_extra_header("X-Title", DEFAULT_TITLE)
            .map_err(|e| BuildError::InvalidClient(e.to_string()))?;
        Ok(Self { inner })
    }

    pub fn with_model(mut self, model: impl Into<String>) -> Self {
        self.inner = self.inner.with_model(model);
        self
    }

    pub fn with_base_url(mut self, base_url: impl Into<String>) -> Self {
        self.inner = self.inner.with_base_url(base_url);
        self
    }

    pub fn with_referer(mut self, referer: impl AsRef<str>) -> Result<Self, BuildError> {
        self.inner = self
            .inner
            .with_extra_header("HTTP-Referer", referer)
            .map_err(|e| BuildError::InvalidClient(e.to_string()))?;
        Ok(self)
    }
}

impl std::fmt::Debug for OpenRouterProvider {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        f.debug_struct("OpenRouterProvider")
            .field("inner", &self.inner)
            .finish()
    }
}

#[async_trait]
impl ChatProvider for OpenRouterProvider {
    async fn chat(&self, request: crate::ChatRequest) -> Result<crate::ChatResponse, ProviderError> {
        self.inner.chat(request).await
    }

    fn model_name(&self) -> &str {
        self.inner.model_name()
    }

    fn max_context_size(&self) -> usize {
        self.inner.max_context_size()
    }

    fn capabilities(&self) -> std::collections::HashSet<crate::ModelCapability> {
        self.inner.capabilities()
    }

    fn thinking_effort(&self) -> Option<&str> {
        self.inner.thinking_effort()
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn default_base_url() {
        let provider = OpenRouterProvider::new("test-key").unwrap();
        let debug = format!("{:?}", provider);
        assert!(debug.contains("https://openrouter.ai/api/v1"));
    }

    #[test]
    fn default_model() {
        let provider = OpenRouterProvider::new("test-key").unwrap();
        assert_eq!(provider.model_name(), "anthropic/claude-sonnet-4");
    }

    #[test]
    fn with_model_override() {
        let provider = OpenRouterProvider::new("test-key")
            .unwrap()
            .with_model("openai/gpt-4o");
        assert_eq!(provider.model_name(), "openai/gpt-4o");
    }
}
