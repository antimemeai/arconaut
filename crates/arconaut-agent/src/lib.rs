pub mod agent;
pub mod compaction;
pub mod dedup;
pub mod hooks;
pub mod injection;
pub mod persistent_shell;
pub mod session;
pub mod soul;

pub use agent::{Agent, AgentMode, AgentRegistry};
pub use compaction::CompactionEngine;
pub use dedup::Deduplicator;
pub use hooks::{Hook, HookEngine, MetricsHook};
pub use injection::{CompositeInjector, Injector, SystemPromptInjector};
pub use persistent_shell::{PersistentShell, TerminalSendTool};
pub use session::Session;
pub use soul::{MockProvider, Soul, SoulError, StopReason, TurnResult};

// gRPC inbox module (includes generated protobuf code)
pub mod inbox {
    tonic::include_proto!("arconaut.inbox");
}
