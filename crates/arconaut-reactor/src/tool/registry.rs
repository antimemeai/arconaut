use arconaut_core::tool::Tool;
use serde_json::Value;
use std::collections::HashMap;

/// ToolRegistry owns all tool implementations.
pub struct ToolRegistry {
    tools: HashMap<String, Box<dyn Tool>>,
    sequence: u64,
}

impl ToolRegistry {
    pub fn new() -> Self {
        Self {
            tools: HashMap::new(),
            sequence: 1,
        }
    }

    pub fn register(&mut self, tool: Box<dyn Tool>) {
        self.tools.insert(tool.name().to_string(), tool);
    }

    pub fn execute(&self, name: &str, _args: Value) -> arconaut_core::tool::ToolResult {
        match self.tools.get(name) {
            Some(_tool) => {
                // Note: Tool::call is async, but we can't call async in a sync fn.
                // TODO(Phase A): make execute async.
                arconaut_core::tool::ToolResult::Error {
                    message: "async execution not yet wired".to_string(),
                    brief: "not implemented".to_string(),
                }
            }
            None => arconaut_core::tool::ToolResult::Error {
                message: format!("tool '{name}' not found"),
                brief: "unknown tool".to_string(),
            },
        }
    }

    pub fn descriptors(&self) -> Vec<arconaut_machine::provider::ToolDescriptor> {
        self.tools
            .values()
            .map(|t| arconaut_machine::provider::ToolDescriptor::new(
                t.name(),
                t.description(),
                t.parameters().clone(),
            ))
            .collect()
    }

    pub fn schema_sequence(&self) -> u64 {
        self.sequence
    }
}

impl Default for ToolRegistry {
    fn default() -> Self {
        Self::new()
    }
}
