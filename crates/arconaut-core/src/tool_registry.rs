use crate::{Tool, ToolError, ToolResult};
use serde_json::Value;
use std::collections::HashMap;

/// Registry for tools that can be called by name.
///
/// Tools are stored in insertion order and can be dispatched by name.
/// The registry is not thread-safe; external synchronization is required
/// if shared across threads.
pub struct ToolRegistry {
    tools: HashMap<String, Box<dyn Tool>>,
    order: Vec<String>,
}

impl Default for ToolRegistry {
    fn default() -> Self {
        Self::new()
    }
}

impl ToolRegistry {
    pub fn new() -> Self {
        Self {
            tools: HashMap::new(),
            order: Vec::new(),
        }
    }

    /// Register a tool. If a tool with the same name already exists,
    /// it is replaced and the position in the order list is updated.
    pub fn register(&mut self, tool: Box<dyn Tool>) {
        let name = tool.name().to_string();
        if !self.tools.contains_key(&name) {
            self.order.push(name.clone());
        }
        self.tools.insert(name, tool);
    }

    /// Call a tool by name with the given arguments.
    ///
    /// Returns `ToolError::UnknownTool` if no tool with the given name is registered.
    pub async fn call(&self, name: &str, args: Value) -> Result<ToolResult, ToolError> {
        let tool = self.tools.get(name).ok_or_else(|| ToolError {
            message: format!("no tool named '{}' is registered", name),
            brief: "unknown tool".to_string(),
        })?;
        tool.call(args).await
    }

    /// List all registered tools in insertion order.
    pub fn list(&self) -> Vec<&dyn Tool> {
        self.order
            .iter()
            .filter_map(|name| self.tools.get(name).map(|t| t.as_ref()))
            .collect()
    }

    /// Check if a tool with the given name is registered.
    pub fn contains(&self, name: &str) -> bool {
        self.tools.contains_key(name)
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::{ContentPart, Tool};
    use async_trait::async_trait;
    use serde_json::Value;

    struct EchoTool;

    #[async_trait]
    impl Tool for EchoTool {
        fn name(&self) -> &str {
            "echo"
        }
        fn description(&self) -> &str {
            "Echoes the input back"
        }
        fn parameters(&self) -> &Value {
            &Value::Null
        }
        async fn call(&self, args: Value) -> Result<ToolResult, ToolError> {
            Ok(ToolResult::success(vec![ContentPart::text(
                args.to_string(),
            )]))
        }
    }

    struct FailTool;

    #[async_trait]
    impl Tool for FailTool {
        fn name(&self) -> &str {
            "fail"
        }
        fn description(&self) -> &str {
            "Always fails"
        }
        fn parameters(&self) -> &Value {
            &Value::Null
        }
        async fn call(&self, _args: Value) -> Result<ToolResult, ToolError> {
            Err(ToolError {
                message: "intentional failure".to_string(),
                brief: "failed".to_string(),
            })
        }
    }

    #[tokio::test]
    async fn register_and_call() {
        let mut registry = ToolRegistry::new();
        registry.register(Box::new(EchoTool));

        let result = registry
            .call("echo", Value::String("hello".to_string()))
            .await;
        assert!(result.is_ok());
        let result = result.unwrap();
        match result {
            ToolResult::Success { output } => {
                assert_eq!(output.len(), 1);
                match &output[0] {
                    ContentPart::Text { text } => assert_eq!(text, "\"hello\""),
                    _ => panic!("Expected text"),
                }
            }
            _ => panic!("Expected success"),
        }
    }

    #[tokio::test]
    async fn unknown_tool_returns_error() {
        let registry = ToolRegistry::new();
        let result = registry.call("nonexistent", Value::Null).await;
        assert!(result.is_err());
        let err = result.unwrap_err();
        assert_eq!(err.brief, "unknown tool");
        assert!(err.message.contains("nonexistent"));
    }

    #[tokio::test]
    async fn list_tools() {
        let mut registry = ToolRegistry::new();
        registry.register(Box::new(EchoTool));
        registry.register(Box::new(FailTool));

        let tools = registry.list();
        assert_eq!(tools.len(), 2);
        assert_eq!(tools[0].name(), "echo");
        assert_eq!(tools[1].name(), "fail");
    }

    #[tokio::test]
    async fn case_sensitive_names() {
        let mut registry = ToolRegistry::new();
        registry.register(Box::new(EchoTool));

        let result = registry.call("Echo", Value::Null).await;
        assert!(result.is_err());
    }

    #[tokio::test]
    async fn replace_existing_tool() {
        let mut registry = ToolRegistry::new();
        registry.register(Box::new(EchoTool));
        registry.register(Box::new(FailTool));
        // Replace echo with a new implementation
        registry.register(Box::new(FailTool));

        let tools = registry.list();
        assert_eq!(tools.len(), 2);
        // order should still be echo, fail
        assert_eq!(tools[0].name(), "echo");
        assert_eq!(tools[1].name(), "fail");
    }
}
