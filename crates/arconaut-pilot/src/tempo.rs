/// Tempo state machine.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum Tempo {
    Metronome,
    Tuned { ratio: u8 },
    MainCharacter,
    Evolving,
}

impl Default for Tempo {
    fn default() -> Self {
        Tempo::Metronome
    }
}

/// Narration density level.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum NarrationLevel {
    Full,
    Summary,
    CheckpointOnly,
}

impl Tempo {
    /// Whether the Soul may issue parallel tool calls.
    pub fn allows_parallel_tools(&self) -> bool {
        matches!(self, Tempo::Tuned { .. } | Tempo::MainCharacter | Tempo::Evolving)
    }

    /// Narration density: full, summary, checkpoint-only.
    pub fn narration_level(&self) -> NarrationLevel {
        match self {
            Tempo::Metronome => NarrationLevel::Full,
            Tempo::Tuned { .. } => NarrationLevel::Summary,
            Tempo::MainCharacter | Tempo::Evolving => NarrationLevel::CheckpointOnly,
        }
    }
}
