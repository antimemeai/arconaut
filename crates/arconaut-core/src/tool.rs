use crate::ContentPart;
use async_trait::async_trait;
use serde_json::Value;

#[async_trait]
pub trait Tool: Send + Sync {
    fn name(&self) -> &str;
    fn description(&self) -> &str;
    fn parameters(&self) -> &Value;
    async fn call(&self, args: Value) -> Result<ToolResult, ToolError>;
}

#[derive(Debug, Clone, PartialEq)]
pub enum ToolResult {
    Success { output: Vec<ContentPart> },
    Error { message: String, brief: String },
}

#[derive(Debug, Clone, PartialEq)]
pub struct ToolError {
    pub message: String,
    pub brief: String,
}

impl ToolResult {
    pub fn success(output: Vec<ContentPart>) -> Self {
        ToolResult::Success { output }
    }

    pub fn error(message: impl Into<String>, brief: impl Into<String>) -> Self {
        ToolResult::Error {
            message: message.into(),
            brief: brief.into(),
        }
    }
}

impl std::fmt::Display for ToolError {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        write!(f, "{}: {}", self.brief, self.message)
    }
}

impl std::error::Error for ToolError {}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn tool_result_success() {
        let result = ToolResult::success(vec![ContentPart::text("ok")]);
        match result {
            ToolResult::Success { output } => {
                assert_eq!(output, vec![ContentPart::text("ok")]);
            }
            _ => panic!("Expected Success variant"),
        }
    }

    #[test]
    fn tool_result_error() {
        let result = ToolResult::error("it broke", "failed");
        match result {
            ToolResult::Error { message, brief } => {
                assert_eq!(message, "it broke");
                assert_eq!(brief, "failed");
            }
            _ => panic!("Expected Error variant"),
        }
    }

    struct TestTool {
        name: String,
        description: String,
    }

    #[async_trait]
    impl Tool for TestTool {
        fn name(&self) -> &str {
            &self.name
        }
        fn description(&self) -> &str {
            &self.description
        }
        fn parameters(&self) -> &Value {
            &Value::Null
        }
        async fn call(&self, _args: Value) -> Result<ToolResult, ToolError> {
            Ok(ToolResult::success(vec![ContentPart::text("done")]))
        }
    }

    #[test]
    fn tool_trait_contract() {
        let tool = TestTool {
            name: "test_tool".to_string(),
            description: "A test".to_string(),
        };
        assert_eq!(tool.name(), "test_tool");
        assert_eq!(tool.description(), "A test");
    }
}
