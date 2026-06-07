pub mod context;
pub mod docs;
pub mod message;
pub mod pdf;
pub mod tool;
pub mod tool_registry;
pub mod vars;

pub use context::Context;
pub use docs::{Document, DocumentIndex};
pub use message::{ContentPart, FunctionCall, Message, Role, ToolCall, ToolResultPart};
pub use pdf::PdfGenerator;
pub use tool::{Tool, ToolError, ToolResult};
pub use tool_registry::ToolRegistry;
pub use vars::{VariableScope, VariableStore};
