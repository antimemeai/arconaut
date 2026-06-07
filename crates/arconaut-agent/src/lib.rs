pub mod agent;
pub mod bus;
pub mod bus_tool;
pub mod compaction;
pub mod dedup;
pub mod dispatch;
pub mod hooks;
pub mod injection;
pub mod inbox_server;
pub mod persistent_shell;
pub mod session;
pub mod soul;

pub use agent::{Agent, AgentMode, AgentRegistry};
pub use bus::{Bus, BusMessage, Presence, Target};
pub use bus_tool::BusTool;
pub use compaction::CompactionEngine;
pub use dedup::Deduplicator;
pub use dispatch::{Brief, DispatchResult, Dispatcher, Intent};
pub use hooks::{Hook, HookEngine, MetricsHook};
pub use injection::{CompositeInjector, Injector, SystemPromptInjector};
pub use inbox_server::InboxServer;
pub use persistent_shell::{PersistentShell, TerminalSendTool};
pub use session::Session;
pub use soul::{MockProvider, Soul, SoulError, StopReason, TurnResult};

// gRPC inbox module (includes generated protobuf code)
pub mod inbox {
    tonic::include_proto!("arconaut.inbox");
}
