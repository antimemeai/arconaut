use arconaut_core::Context;
use serde::{Deserialize, Serialize};
use std::collections::HashMap;

/// What an agent is authorized to do.
#[derive(Debug, Clone, Copy, PartialEq, Eq, Serialize, Deserialize)]
pub enum AgentMode {
    Implement,
    Review,
    Explore,
    Test,
    Assist,
}

/// A named agent with its own persisted context and configuration.
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct Agent {
    pub name: String,
    pub callsign: String,
    pub mode: AgentMode,
    pub context: Context,
    pub session_ids: Vec<String>,
    pub model_name: String,
    pub max_steps: usize,
}

impl Agent {
    pub fn new(name: impl Into<String>, callsign: impl Into<String>, mode: AgentMode) -> Self {
        Self {
            name: name.into(),
            callsign: callsign.into(),
            mode,
            context: Context::new(200_000),
            session_ids: Vec::new(),
            model_name: "claude-sonnet-4-6".to_string(),
            max_steps: 50,
        }
    }

    pub fn with_model(mut self, model: impl Into<String>) -> Self {
        self.model_name = model.into();
        self
    }

    pub fn with_max_steps(mut self, max_steps: usize) -> Self {
        self.max_steps = max_steps;
        self
    }
}

/// Registry of all known agents.
#[derive(Debug, Default, Serialize, Deserialize)]
pub struct AgentRegistry {
    agents: HashMap<String, Agent>,
}

impl AgentRegistry {
    pub fn new() -> Self {
        Self::default()
    }

    pub fn register(&mut self, agent: Agent) {
        self.agents.insert(agent.name.clone(), agent);
    }

    pub fn get(&self, name: &str) -> Option<&Agent> {
        self.agents.get(name)
    }

    pub fn get_mut(&mut self, name: &str) -> Option<&mut Agent> {
        self.agents.get_mut(name)
    }

    pub fn list(&self) -> Vec<&Agent> {
        self.agents.values().collect()
    }

    pub fn remove(&mut self, name: &str) -> Option<Agent> {
        self.agents.remove(name)
    }

    pub fn load(path: impl AsRef<std::path::Path>) -> std::io::Result<Self> {
        let content = std::fs::read_to_string(path)?;
        let registry: Self = serde_json::from_str(&content)
            .map_err(|e| std::io::Error::new(std::io::ErrorKind::InvalidData, e))?;
        Ok(registry)
    }

    pub fn save(&self, path: impl AsRef<std::path::Path>) -> std::io::Result<()> {
        let content = serde_json::to_string_pretty(self)
            .map_err(|e| std::io::Error::new(std::io::ErrorKind::InvalidData, e))?;
        std::fs::write(path, content)
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn create_all_modes() {
        for mode in [
            AgentMode::Implement,
            AgentMode::Review,
            AgentMode::Explore,
            AgentMode::Test,
            AgentMode::Assist,
        ] {
            let agent = Agent::new("test", "T", mode);
            assert_eq!(agent.mode, mode);
        }
    }

    #[test]
    fn registry_lookup() {
        let mut reg = AgentRegistry::new();
        reg.register(Agent::new("alpha", "A", AgentMode::Implement));
        reg.register(Agent::new("beta", "B", AgentMode::Review));
        assert_eq!(reg.list().len(), 2);
        assert!(reg.get("alpha").is_some());
        assert!(reg.get("gamma").is_none());
    }

    #[test]
    fn persist_roundtrip() {
        let dir = tempfile::TempDir::new().unwrap();
        let path = dir.path().join("agents.json");
        let mut reg = AgentRegistry::new();
        reg.register(Agent::new("alpha", "A", AgentMode::Implement));
        reg.save(&path).unwrap();
        let loaded = AgentRegistry::load(&path).unwrap();
        assert_eq!(loaded.list().len(), 1);
        assert_eq!(loaded.get("alpha").unwrap().callsign, "A");
    }
}
