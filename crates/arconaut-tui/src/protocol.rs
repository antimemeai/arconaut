use arconaut_core::{Message, ToolResult};
use serde_json::Value;

/// Commands sent from the TUI event loop to the Soul/CLI task.
#[derive(Debug, Clone)]
pub enum SoulCommand {
    /// User submitted a message.
    UserInput(String),
    /// User wants to interrupt the current turn.
    Interrupt,
    /// User wants to quit the application.
    Quit,
    /// Terminal pane produced output (forwarded to Soul context).
    TerminalOutput(String),
    /// Resize event (rows, cols).
    Resize(u16, u16),
}

/// Events sent from the Soul/CLI task to the TUI for display.
#[derive(Debug, Clone)]
pub enum TuiEvent {
    /// A new message from the assistant.
    AssistantMessage(Message),
    /// A tool call was invoked.
    ToolCall { name: String, args: Value },
    /// A tool call returned a result.
    ToolResult { name: String, result: ToolResult },
    /// Assistant is thinking (reasoning models).
    Thinking { text: String },
    /// An error occurred.
    Error { message: String },
    /// Output from the persistent terminal.
    ShellOutput(String),
    /// Status update (e.g., "compacting context...").
    Status(String),
    /// The current turn has completed.
    TurnComplete,
}
