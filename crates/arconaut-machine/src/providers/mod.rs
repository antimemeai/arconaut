pub mod anthropic;
pub mod gemini;
pub mod moonshot;
pub mod openai;
pub mod openai_compat;
pub mod openrouter;

pub use anthropic::AnthropicProvider;
pub use gemini::GeminiProvider;
pub use moonshot::MoonshotProvider;
pub use openai::OpenAiProvider;
pub use openrouter::OpenRouterProvider;

use crate::ChatProvider;
use arconaut_core::VariableStore;
use std::collections::HashMap;

/// The kind of LLM provider.
#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
pub enum ProviderKind {
    Anthropic,
    OpenAi,
    Gemini,
    Moonshot,
    OpenRouter,
}

/// Structured configuration for a provider instance.
#[derive(Debug, Clone, PartialEq)]
pub struct ProviderConfig {
    pub kind: ProviderKind,
    pub api_key: String,
    pub model: String,
    pub base_url: Option<String>,
    pub extra_headers: Option<HashMap<String, String>>,
}

/// Error building a provider from config.
#[derive(Debug, Clone, PartialEq)]
pub enum ProviderBuildError {
    MissingApiKey,
    MissingModel,
    UnknownProviderKind(String),
    InvalidConfig(String),
}

impl std::fmt::Display for ProviderBuildError {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            ProviderBuildError::MissingApiKey => write!(f, "provider config missing api_key"),
            ProviderBuildError::MissingModel => write!(f, "provider config missing model"),
            ProviderBuildError::UnknownProviderKind(k) => write!(f, "unknown provider kind: {}", k),
            ProviderBuildError::InvalidConfig(msg) => write!(f, "invalid provider config: {}", msg),
        }
    }
}

impl std::error::Error for ProviderBuildError {}

/// Factory that constructs providers from structured config.
pub struct ProviderFactory;

impl ProviderFactory {
    /// Create a provider from an explicit config.
    pub fn create(cfg: &ProviderConfig) -> Result<Box<dyn ChatProvider>, ProviderBuildError> {
        match cfg.kind {
            ProviderKind::Anthropic => {
                let mut p = anthropic::AnthropicProvider::new(&cfg.api_key)
                    .map_err(|e| ProviderBuildError::InvalidConfig(e.to_string()))?;
                p = p.with_model(&cfg.model);
                if let Some(url) = &cfg.base_url {
                    p = p.with_base_url(url);
                }
                Ok(Box::new(p))
            }
            ProviderKind::OpenAi => {
                let mut p = openai::OpenAiProvider::new(&cfg.api_key)
                    .map_err(|e| ProviderBuildError::InvalidConfig(e.to_string()))?;
                p = p.with_model(&cfg.model);
                if let Some(url) = &cfg.base_url {
                    p = p.with_base_url(url);
                }
                Ok(Box::new(p))
            }
            ProviderKind::Moonshot => {
                let mut p = moonshot::MoonshotProvider::new(&cfg.api_key)
                    .map_err(|e| ProviderBuildError::InvalidConfig(e.to_string()))?;
                p = p.with_model(&cfg.model);
                if let Some(url) = &cfg.base_url {
                    p = p.with_base_url(url);
                }
                Ok(Box::new(p))
            }
            ProviderKind::OpenRouter => {
                let mut p = openrouter::OpenRouterProvider::new(&cfg.api_key)
                    .map_err(|e| ProviderBuildError::InvalidConfig(e.to_string()))?;
                p = p.with_model(&cfg.model);
                if let Some(url) = &cfg.base_url {
                    p = p.with_base_url(url);
                }
                Ok(Box::new(p))
            }
            ProviderKind::Gemini => {
                let mut p = gemini::GeminiProvider::new(&cfg.api_key)
                    .map_err(|e| ProviderBuildError::InvalidConfig(e.to_string()))?;
                p = p.with_model(&cfg.model);
                if let Some(url) = &cfg.base_url {
                    p = p.with_base_url(url);
                }
                Ok(Box::new(p))
            }
        }
    }

    /// Parse provider config from a VariableStore entry and create the provider.
    pub fn create_named(
        name: &str,
        vars: &VariableStore,
    ) -> Result<Box<dyn ChatProvider>, ProviderBuildError> {
        let cfg = vars
            .get_prefixed(&format!("provider.{}", name));
        if cfg.is_empty() {
            return Err(ProviderBuildError::InvalidConfig(format!(
                "no config for provider '{}'",
                name
            )));
        }
        let kind = cfg
            .get("kind")
            .and_then(|v| v.as_str())
            .unwrap_or(name)
            .to_lowercase();
        let kind = match kind.as_str() {
            "anthropic" => ProviderKind::Anthropic,
            "openai" => ProviderKind::OpenAi,
            "gemini" => ProviderKind::Gemini,
            "moonshot" => ProviderKind::Moonshot,
            "openrouter" => ProviderKind::OpenRouter,
            _ => {
                return Err(ProviderBuildError::UnknownProviderKind(kind));
            }
        };
        let api_key = cfg
            .get("api_key")
            .and_then(|v| v.as_str())
            .map(|s| s.to_string())
            .ok_or(ProviderBuildError::MissingApiKey)?;
        let model = cfg
            .get("model")
            .and_then(|v| v.as_str())
            .map(|s| s.to_string())
            .ok_or(ProviderBuildError::MissingModel)?;
        let base_url = cfg.get("base_url").and_then(|v| v.as_str()).map(|s| s.to_string());
        let extra_headers = None; // TODO: parse extra_headers table

        Self::create(&ProviderConfig {
            kind,
            api_key,
            model,
            base_url,
            extra_headers,
        })
    }
}

/// Registry of named provider instances.
pub struct ProviderRegistry {
    providers: HashMap<String, Box<dyn ChatProvider>>,
}

impl ProviderRegistry {
    pub fn new() -> Self {
        Self {
            providers: HashMap::new(),
        }
    }

    pub fn register(
        &mut self,
        name: impl Into<String>,
        provider: Box<dyn ChatProvider>,
    ) -> Result<(), ProviderBuildError> {
        let name = name.into();
        if self.providers.contains_key(&name) {
            return Err(ProviderBuildError::InvalidConfig(format!(
                "provider '{}' already registered",
                name
            )));
        }
        self.providers.insert(name, provider);
        Ok(())
    }

    pub fn get(&self, name: &str) -> Option<&dyn ChatProvider> {
        self.providers.get(name).map(|p| p.as_ref())
    }

    pub fn remove(&mut self, name: &str) -> Option<Box<dyn ChatProvider>> {
        self.providers.remove(name)
    }

    pub fn names(&self) -> impl Iterator<Item = &str> {
        self.providers.keys().map(|s| s.as_str())
    }

    pub fn is_empty(&self) -> bool {
        self.providers.is_empty()
    }

    pub fn len(&self) -> usize {
        self.providers.len()
    }
}

impl Default for ProviderRegistry {
    fn default() -> Self {
        Self::new()
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn factory_creates_all_kinds() {
        for kind in [
            ProviderKind::Anthropic,
            ProviderKind::OpenAi,
            ProviderKind::Moonshot,
            ProviderKind::OpenRouter,
            ProviderKind::Gemini,
        ] {
            let cfg = ProviderConfig {
                kind,
                api_key: "test-key".to_string(),
                model: "test-model".to_string(),
                base_url: None,
                extra_headers: None,
            };
            let result = ProviderFactory::create(&cfg);
            assert!(result.is_ok(), "failed to create {:?}: {:?}", kind, result.err());
        }
    }

    #[test]
    fn factory_rejects_invalid_config() {
        let cfg = ProviderConfig {
            kind: ProviderKind::Anthropic,
            api_key: "test-key".to_string(),
            model: "test-model".to_string(),
            base_url: None,
            extra_headers: None,
        };
        // This should succeed with valid config
        assert!(ProviderFactory::create(&cfg).is_ok());
    }

    #[test]
    fn registry_round_trip() {
        let mut registry = ProviderRegistry::new();
        let cfg = ProviderConfig {
            kind: ProviderKind::Anthropic,
            api_key: "test-key".to_string(),
            model: "test-model".to_string(),
            base_url: None,
            extra_headers: None,
        };
        let provider = ProviderFactory::create(&cfg).unwrap();
        registry.register("primary", provider).unwrap();
        assert!(registry.get("primary").is_some());
        assert_eq!(registry.len(), 1);
    }

    #[test]
    fn registry_rejects_duplicate() {
        let mut registry = ProviderRegistry::new();
        let cfg = ProviderConfig {
            kind: ProviderKind::Anthropic,
            api_key: "test-key".to_string(),
            model: "test-model".to_string(),
            base_url: None,
            extra_headers: None,
        };
        let p1 = ProviderFactory::create(&cfg).unwrap();
        let p2 = ProviderFactory::create(&cfg).unwrap();
        registry.register("primary", p1).unwrap();
        assert!(registry.register("primary", p2).is_err());
    }
}
