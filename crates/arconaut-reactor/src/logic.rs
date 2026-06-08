use crate::context::ContextAssembler;
use crate::heuristic::{HeuristicConfig, HeuristicEngine};
use crate::tool::ToolRegistry;
use arconaut_core::Message;
use arconaut_pilot::{Soul, StopReason};

/// The central orchestrator. Owns the turn loop.
pub struct ArconautLogic {
    _context_assembler: ContextAssembler,
    _tool_registry: ToolRegistry,
    _heuristic_engine: HeuristicEngine,
    _config: ReactorConfig,
}

/// Reactor configuration.
#[derive(Debug, Clone)]
pub struct ReactorConfig {
    pub max_steps: usize,
}

impl Default for ReactorConfig {
    fn default() -> Self {
        Self { max_steps: 100 }
    }
}

/// Output of a complete turn (after all tool-call loops).
#[derive(Debug, Clone)]
pub struct TurnOutput {
    pub final_message: Message,
    pub steps_taken: usize,
    pub completed: bool,
    pub stop_reason: StopReason,
}

/// Reactor-level errors.
#[derive(Debug, Clone, PartialEq)]
pub enum ReactorError {
    Soul(arconaut_pilot::SoulError),
    ToolExecution { tool: String, error: String },
    Heuristic(String),
}

impl std::fmt::Display for ReactorError {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            ReactorError::Soul(e) => write!(f, "soul error: {e}"),
            ReactorError::ToolExecution { tool, error } => {
                write!(f, "tool '{tool}' failed: {error}")
            }
            ReactorError::Heuristic(e) => write!(f, "heuristic error: {e}"),
        }
    }
}

impl std::error::Error for ReactorError {}

impl ArconautLogic {
    pub fn new(config: ReactorConfig) -> Self {
        Self {
            _context_assembler: ContextAssembler::new(),
            _tool_registry: ToolRegistry::new(),
            _heuristic_engine: HeuristicEngine::new(HeuristicConfig::default()),
            _config: config,
        }
    }

    /// Run one full turn: assemble → soul.execute → execute tools → loop.
    pub async fn run_turn<S: Soul>(
        &mut self,
        _soul: &mut S,
        _user_input: &str,
    ) -> Result<TurnOutput, ReactorError> {
        // TODO(Phase A): implement the full turn loop.
        // This is intentionally stubbed to make the red tests fail.
        Err(ReactorError::Heuristic("run_turn not yet implemented".to_string()))
    }
}
