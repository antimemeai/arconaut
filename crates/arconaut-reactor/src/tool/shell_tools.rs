use crate::runtime::brush::BrushRuntime;
use arconaut_core::tool::{Tool, ToolError, ToolResult};
use arconaut_core::ContentPart;
use async_trait::async_trait;
use serde_json::Value;
use std::sync::Arc;
use tokio::sync::Mutex;

/// Read recent output from the structured shell log.
pub struct RTermReadTool {
    brush: Arc<Mutex<BrushRuntime>>,
    params: Value,
}

impl RTermReadTool {
    pub fn new(brush: Arc<Mutex<BrushRuntime>>) -> Self {
        Self {
            brush,
            params: serde_json::json!({
                "type": "object",
                "properties": {
                    "lines": { "type": "integer", "description": "Number of recent lines to read", "default": 50 }
                },
                "required": []
            }),
        }
    }
}

#[async_trait]
impl Tool for RTermReadTool {
    fn name(&self) -> &str {
        "RTermRead"
    }

    fn description(&self) -> &str {
        "Read recent output from the persistent shell."
    }

    fn parameters(&self) -> &Value {
        &self.params
    }

    async fn call(&self, args: Value) -> Result<ToolResult, ToolError> {
        let lines = args["lines"].as_i64().unwrap_or(50);

        let mut brush = self.brush.lock().await;
        let result = brush
            .exec_persistent(&format!("echo '__arconaut_marker__' && tail -n {lines} /dev/null 2>&1 || true"))
            .await
            .map_err(|e| ToolError {
                message: e.to_string(),
                brief: "shell read failed".to_string(),
            })?;

        // Phase B: return raw stdout. In Phase C, this reads from structured ShellLog.
        let output = result.stdout;
        let trimmed = output.lines().take(lines as usize).collect::<Vec<_>>().join("\n");

        Ok(ToolResult::success(vec![ContentPart::Text {
            text: trimmed,
        }]))
    }
}

/// Write a command to the persistent shell.
pub struct RTermWriteTool {
    brush: Arc<Mutex<BrushRuntime>>,
    params: Value,
}

impl RTermWriteTool {
    pub fn new(brush: Arc<Mutex<BrushRuntime>>) -> Self {
        Self {
            brush,
            params: serde_json::json!({
                "type": "object",
                "properties": {
                    "command": { "type": "string" },
                    "timeout_ms": { "type": "integer", "default": 120000 }
                },
                "required": ["command"]
            }),
        }
    }
}

#[async_trait]
impl Tool for RTermWriteTool {
    fn name(&self) -> &str {
        "RTermWrite"
    }

    fn description(&self) -> &str {
        "Write a command to the persistent shell."
    }

    fn parameters(&self) -> &Value {
        &self.params
    }

    async fn call(&self, args: Value) -> Result<ToolResult, ToolError> {
        let command = args["command"].as_str().ok_or_else(|| ToolError {
            message: "missing 'command' argument".to_string(),
            brief: "invalid arguments".to_string(),
        })?;
        let _timeout_ms = args["timeout_ms"].as_i64().unwrap_or(120000);

        let mut brush = self.brush.lock().await;
        let result = brush
            .exec_persistent(command)
            .await
            .map_err(|e| ToolError {
                message: e.to_string(),
                brief: "shell write failed".to_string(),
            })?;

        let output = format!(
            "exit_code: {}\nstdout:\n{}\nstderr:\n{}",
            result.exit_code, result.stdout, result.stderr
        );

        Ok(ToolResult::success(vec![ContentPart::Text {
            text: output,
        }]))
    }
}
