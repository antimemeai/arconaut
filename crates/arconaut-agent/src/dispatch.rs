use crate::{Agent, AgentMode, AgentRegistry};
use serde::{Deserialize, Serialize};

/// A task brief — what needs doing, and what kind of agent should do it.
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct Brief {
    pub title: String,
    pub description: String,
    pub intent: Intent,
    pub tags: Vec<String>,
}

/// High-level intent of a brief.
#[derive(Debug, Clone, Copy, PartialEq, Eq, Serialize, Deserialize)]
pub enum Intent {
    /// Write or modify code.
    Implement,
    /// Review existing code.
    Review,
    /// Explore or research.
    Explore,
    /// Write or run tests.
    Test,
    /// General assistance.
    Assist,
}

impl Brief {
    pub fn new(title: impl Into<String>, description: impl Into<String>, intent: Intent) -> Self {
        Self {
            title: title.into(),
            description: description.into(),
            intent,
            tags: Vec::new(),
        }
    }

    pub fn with_tags(mut self, tags: Vec<String>) -> Self {
        self.tags = tags;
        self
    }
}

/// Dispatcher routes briefs to agents based on intent.
pub struct Dispatcher {
    registry: AgentRegistry,
}

impl Dispatcher {
    pub fn new(registry: AgentRegistry) -> Self {
        Self { registry }
    }

    /// Pick the best agent for a brief, or suggest creating one.
    pub fn dispatch(&self, brief: &Brief) -> DispatchResult {
        let candidates: Vec<&Agent> = self
            .registry
            .list()
            .into_iter()
            .filter(|a| mode_matches(brief.intent, a.mode))
            .collect();

        if candidates.is_empty() {
            return DispatchResult::SpawnNeeded {
                suggested_mode: brief.intent.into(),
                reason: format!("no agent available for {:?}", brief.intent),
            };
        }

        // Simple heuristic: pick the first matching agent.
        // Future: load-balancing, task queue depth, specialization tags.
        DispatchResult::Routed {
            agent_name: candidates[0].name.clone(),
        }
    }

    /// Create a new agent on the fly for a brief.
    pub fn spawn_for(&self, brief: &Brief, name: impl Into<String>, callsign: impl Into<String>) -> Agent {
        Agent::new(name, callsign, brief.intent.into())
    }
}

/// Outcome of dispatching a brief.
#[derive(Debug, Clone, PartialEq)]
pub enum DispatchResult {
    /// Routed to an existing agent.
    Routed { agent_name: String },
    /// No suitable agent — caller should spawn one.
    SpawnNeeded { suggested_mode: AgentMode, reason: String },
}

fn mode_matches(intent: Intent, mode: AgentMode) -> bool {
    match (intent, mode) {
        (Intent::Implement, AgentMode::Implement) => true,
        (Intent::Review, AgentMode::Review) => true,
        (Intent::Explore, AgentMode::Explore) => true,
        (Intent::Test, AgentMode::Test) => true,
        (Intent::Assist, AgentMode::Assist) => true,
        // Assist agents can handle anything as fallback.
        (_, AgentMode::Assist) => true,
        _ => false,
    }
}

impl From<Intent> for AgentMode {
    fn from(intent: Intent) -> Self {
        match intent {
            Intent::Implement => AgentMode::Implement,
            Intent::Review => AgentMode::Review,
            Intent::Explore => AgentMode::Explore,
            Intent::Test => AgentMode::Test,
            Intent::Assist => AgentMode::Assist,
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn dispatch_finds_matching_agent() {
        let mut reg = AgentRegistry::new();
        reg.register(Agent::new("coder", "C", AgentMode::Implement));
        reg.register(Agent::new("reviewer", "R", AgentMode::Review));

        let dispatcher = Dispatcher::new(reg);
        let brief = Brief::new("fix bug", "fix the login bug", Intent::Implement);

        match dispatcher.dispatch(&brief) {
            DispatchResult::Routed { agent_name } => assert_eq!(agent_name, "coder"),
            other => panic!("expected Routed, got {:?}", other),
        }
    }

    #[test]
    fn dispatch_falls_back_to_assist() {
        let mut reg = AgentRegistry::new();
        reg.register(Agent::new("helper", "H", AgentMode::Assist));

        let dispatcher = Dispatcher::new(reg);
        let brief = Brief::new("question", "how does this work", Intent::Explore);

        match dispatcher.dispatch(&brief) {
            DispatchResult::Routed { agent_name } => assert_eq!(agent_name, "helper"),
            other => panic!("expected Routed, got {:?}", other),
        }
    }

    #[test]
    fn dispatch_spawns_when_no_match() {
        let reg = AgentRegistry::new();
        let dispatcher = Dispatcher::new(reg);
        let brief = Brief::new("test", "write tests", Intent::Test);

        match dispatcher.dispatch(&brief) {
            DispatchResult::SpawnNeeded { suggested_mode, .. } => {
                assert_eq!(suggested_mode, AgentMode::Test);
            }
            other => panic!("expected SpawnNeeded, got {:?}", other),
        }
    }

    #[test]
    fn spawn_for_creates_agent() {
        let reg = AgentRegistry::new();
        let dispatcher = Dispatcher::new(reg);
        let brief = Brief::new("impl", "implement feature", Intent::Implement);
        let agent = dispatcher.spawn_for(&brief, "impl-1", "I1");
        assert_eq!(agent.mode, AgentMode::Implement);
        assert_eq!(agent.name, "impl-1");
    }
}
