pub mod dedup;
pub mod soul;
pub mod tempo;
pub mod types;

pub use dedup::Deduplicator;
pub use soul::{DefaultSoul, Soul, SoulError};
pub use tempo::{NarrationLevel, Tempo};
pub use types::{SoulConfig, StopReason, ToolDescriptor, TurnRequest, TurnResponse};
