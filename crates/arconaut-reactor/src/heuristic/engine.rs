use arconaut_core::{Message, ToolResult};
use arconaut_machine::provider::TokenUsage;

/// Tier 1 heuristic engine.
pub struct HeuristicEngine {
    _config: HeuristicConfig,
}

/// Heuristic configuration.
#[derive(Debug, Clone, Default)]
pub struct HeuristicConfig {
    pub doom_loop_threshold: usize,
    pub premature_patch_threshold: usize,
}

/// Events fed to the heuristic engine.
#[derive(Debug, Clone)]
pub enum TurnEvent {
    ToolCall {
        name: String,
        args: serde_json::Value,
        result: ToolResult,
    },
    ProviderResponse {
        message: Message,
        token_usage: TokenUsage,
    },
    Compaction,
    TempoChange {
        from: arconaut_pilot::Tempo,
        to: arconaut_pilot::Tempo,
    },
}

/// Interventions the heuristic engine can request.
#[derive(Debug, Clone, PartialEq)]
pub enum Intervention {
    Nudge(String),
    Warning(String),
    Pause(String),
    BeatTaken,
}

impl HeuristicEngine {
    pub fn new(config: HeuristicConfig) -> Self {
        Self { _config: config }
    }

    /// Evaluate one turn step. Returns any interventions.
    pub fn evaluate(&mut self, _event: TurnEvent) -> Vec<Intervention> {
        // TODO(Phase A): implement Tier 1 detectors.
        vec![]
    }
}
