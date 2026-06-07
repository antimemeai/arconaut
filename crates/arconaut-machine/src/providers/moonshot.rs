use super::openai_compat::{BuildError, OpenAiCompatClient};
use crate::{ChatProvider, ProviderError};
use async_trait::async_trait;

const MOONSHOT_API_BASE: &str = "https://api.moonshot.cn/v1";

/// Moonshot AI (Kimi) brand-specific provider.
///
/// Wraps `OpenAiCompatClient` with Moonshot defaults.
pub struct MoonshotProvider {
    inner: OpenAiCompatClient,
}

impl MoonshotProvider {
    pub fn new(api_key: impl Into<String>) -> Result<Self, BuildError> {
        let inner = OpenAiCompatClient::new(api_key, MOONSHOT_API_BASE)?
            .with_model("moonshot-v1-8k");
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
}

impl std::fmt::Debug for MoonshotProvider {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        f.debug_struct("MoonshotProvider")
            .field("inner", &self.inner)
            .finish()
    }
}

#[async_trait]
impl ChatProvider for MoonshotProvider {
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
        let provider = MoonshotProvider::new("test-key").unwrap();
        let debug = format!("{:?}", provider);
        assert!(debug.contains("https://api.moonshot.cn/v1"));
    }

    #[test]
    fn default_model() {
        let provider = MoonshotProvider::new("test-key").unwrap();
        assert_eq!(provider.model_name(), "moonshot-v1-8k");
    }

    #[test]
    fn with_model_override() {
        let provider = MoonshotProvider::new("test-key")
            .unwrap()
            .with_model("moonshot-v1-128k");
        assert_eq!(provider.model_name(), "moonshot-v1-128k");
    }
}
