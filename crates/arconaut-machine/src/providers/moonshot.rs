use super::openai_compat::{BuildError, OpenAiCompatClient};
use crate::{
    auth::{ActivityTracker, CredentialStorage, FileStorage, KimiOAuthFlow},
    ChatProvider, ProviderError,
};
use async_trait::async_trait;
use std::sync::Arc;

const MOONSHOT_API_BASE: &str = "https://api.moonshot.cn/v1";
const KIMI_OAUTH_KEY: &str = "kimi";

/// Moonshot AI (Kimi) brand-specific provider.
///
/// Wraps `OpenAiCompatClient` with Moonshot defaults.
/// Supports both API key and OAuth authentication.
pub struct MoonshotProvider {
    inner: OpenAiCompatClient,
    oauth: Option<KimiOAuthFlow>,
    storage: Option<Arc<dyn CredentialStorage>>,
    fallback_key: String,
    activity: Option<Arc<ActivityTracker>>,
}

impl MoonshotProvider {
    pub fn new(api_key: impl Into<String>) -> Result<Self, BuildError> {
        let key = api_key.into();
        let inner = OpenAiCompatClient::new(&key, MOONSHOT_API_BASE)?
            .with_model("moonshot-v1-8k");
        Ok(Self {
            inner,
            oauth: None,
            storage: None,
            fallback_key: key,
            activity: None,
        })
    }

    pub fn with_model(mut self, model: impl Into<String>) -> Self {
        self.inner = self.inner.with_model(model);
        self
    }

    pub fn with_base_url(mut self, base_url: impl Into<String>) -> Self {
        self.inner = self.inner.with_base_url(base_url);
        self
    }

    /// Enable OAuth authentication for this provider.
    ///
    /// When OAuth is enabled, the provider will load the access token from
    /// storage before each request, refreshing it if necessary.
    pub fn with_oauth(mut self) -> Self {
        self.oauth = Some(KimiOAuthFlow::new());
        self.storage = Some(Arc::new(
            FileStorage::new().unwrap_or_else(|_| FileStorage::with_dir(std::env::temp_dir())),
        ));
        self.activity = Some(Arc::new(ActivityTracker::new()));
        self
    }

    /// Resolve the current API key to use for requests.
    ///
    /// If OAuth is configured, loads the token from storage and refreshes
    /// if expired or near expiry. Falls back to the static API key.
    #[allow(dead_code)]
    async fn resolve_api_key(&mut self) -> String {
        let Some(ref storage) = self.storage else {
            return self.fallback_key.clone();
        };

        let token = match storage.load(KIMI_OAUTH_KEY) {
            Ok(Some(t)) => t,
            _ => return self.fallback_key.clone(),
        };

        if token.is_expired() || token.needs_refresh() {
            if let Some(ref flow) = self.oauth {
                match flow.refresh_token(&token.refresh_token).await {
                    Ok(new_token) => {
                        let _ = storage.save(KIMI_OAUTH_KEY, &new_token);
                        self.inner.set_api_key(&new_token.access_token);
                        return new_token.access_token;
                    }
                    Err(e) => {
                        eprintln!("oauth refresh failed: {}, falling back to api_key", e);
                        return self.fallback_key.clone();
                    }
                }
            }
        }

        self.inner.set_api_key(&token.access_token);
        token.access_token
    }
}

impl MoonshotProvider {
    /// Start a background refresh task for OAuth tokens.
    ///
    /// Returns `None` if OAuth is not enabled.
    pub fn start_refresh_task(&self) -> Option<crate::auth::RefreshTask> {
        let storage = self.storage.clone()?;
        let flow = self.oauth.clone()?;
        let activity = self.activity.clone()?;
        let lock_path = dirs::config_dir()
            .unwrap_or_else(std::env::temp_dir)
            .join("arconaut")
            .join("oauth")
            .join("refresh.lock");
        Some(crate::auth::RefreshTask::start(
            storage,
            flow,
            KIMI_OAUTH_KEY.to_string(),
            activity,
            lock_path,
        ))
    }
}

impl std::fmt::Debug for MoonshotProvider {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        f.debug_struct("MoonshotProvider")
            .field("inner", &self.inner)
            .field("oauth_enabled", &self.oauth.is_some())
            .finish()
    }
}

#[async_trait]
impl ChatProvider for MoonshotProvider {
    async fn chat(&self, request: crate::ChatRequest) -> Result<crate::ChatResponse, ProviderError> {
        // Record prompt activity for the background refresh task.
        if let Some(ref activity) = self.activity {
            activity.record_prompt();
        }
        self.inner.chat(request).await
    }

    fn start_refresh_task(&self) -> Option<crate::auth::RefreshTask> {
        self.start_refresh_task()
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

    #[test]
    fn oauth_enabled_in_debug() {
        let provider = MoonshotProvider::new("test-key")
            .unwrap()
            .with_oauth();
        let debug = format!("{:?}", provider);
        assert!(debug.contains("oauth_enabled: true"));
    }

    #[test]
    fn oauth_disabled_by_default() {
        let provider = MoonshotProvider::new("test-key").unwrap();
        let debug = format!("{:?}", provider);
        assert!(debug.contains("oauth_enabled: false"));
    }
}
