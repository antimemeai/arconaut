use crate::context::BeatState;
use arconaut_core::Message;
use arconaut_pilot::{Tempo, TurnRequest, ToolDescriptor};

/// ContextAssembler builds the TurnRequest that the Soul receives.
pub struct ContextAssembler {
    history: Vec<Message>,
    system_prompt: String,
}

impl ContextAssembler {
    pub fn new() -> Self {
        Self {
            history: Vec::new(),
            system_prompt: String::new(),
        }
    }

    /// Build a TurnRequest for the Soul.
    pub fn assemble(&self, _user_input: &str, _tempo: Tempo) -> TurnRequest {
        // TODO(Phase A): real assembly with Beat injection, working set, etc.
        TurnRequest {
            context: self.history.clone(),
            available_tools: vec![],
            tempo: Tempo::Metronome,
        }
    }

    /// Append a message to history.
    pub fn append(&mut self, message: Message) {
        self.history.push(message);
    }

    /// Inject Beat state into the next assembled context.
    pub fn inject_beat(&mut self, _beat: BeatState) {
        // TODO(Phase A): convert BeatState to system message and prepend.
    }

    pub fn set_system_prompt(&mut self, prompt: impl Into<String>) {
        self.system_prompt = prompt.into();
    }
}

impl Default for ContextAssembler {
    fn default() -> Self {
        Self::new()
    }
}
