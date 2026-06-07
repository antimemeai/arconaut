pub mod compaction;
pub mod dedup;
pub mod hooks;
pub mod injection;
pub mod soul;

pub use compaction::CompactionEngine;
pub use dedup::Deduplicator;
pub use hooks::{Hook, HookEngine, MetricsHook};
pub use injection::{CompositeInjector, Injector, SystemPromptInjector};
pub use soul::{MockProvider, Soul, SoulError, StopReason, TurnResult};
