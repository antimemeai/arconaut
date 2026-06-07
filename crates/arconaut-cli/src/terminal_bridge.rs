use arconaut_core::{ContentPart, Tool, ToolError, ToolResult};
use async_trait::async_trait;
use serde_json::Value;
use tokio::sync::{mpsc, oneshot};

/// A tool that proxies terminal input to the CLI task via a channel,
/// allowing a shared persistent shell between the LLM and the user.
pub struct TerminalBridge {
    input_tx: mpsc::Sender<(String, oneshot::Sender<String>)>,
    params: Value,
}

impl TerminalBridge {
    pub fn new(input_tx: mpsc::Sender<(String, oneshot::Sender<String>)>) -> Self {
        Self {
            input_tx,
            params: serde_json::json!({
                "type": "object",
                "properties": {
                    "input": {
                        "type": "string",
                        "description": "Text to send to the persistent terminal. Supports multi-line scripts."
                    }
                },
                "required": ["input"]
            }),
        }
    }
}

#[async_trait]
impl Tool for TerminalBridge {
    fn name(&self) -> &str {
        "terminal_send"
    }

    fn description(&self) -> &str {
        "Send input to the persistent interactive terminal. State (cwd, env vars) persists between calls. Supports multi-line input."
    }

    fn parameters(&self) -> &Value {
        &self.params
    }

    async fn call(&self, args: Value) -> Result<ToolResult, ToolError> {
        let input = args["input"].as_str().ok_or_else(|| ToolError {
            message: "missing 'input' argument".to_string(),
            brief: "bad args".to_string(),
        })?;

        let (reply_tx, reply_rx) = oneshot::channel();
        self.input_tx
            .send((input.to_string(), reply_tx))
            .await
            .map_err(|e| ToolError {
                message: format!("failed to send to terminal bridge: {}", e),
                brief: "bridge error".to_string(),
            })?;

        let output = reply_rx.await.map_err(|e| ToolError {
            message: format!("terminal bridge closed: {}", e),
            brief: "bridge error".to_string(),
        })?;

        Ok(ToolResult::success(vec![ContentPart::text(output)]))
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use tokio::sync::mpsc;

    #[tokio::test]
    async fn bridge_forwards_input() {
        let (tx, mut rx) = mpsc::channel::<(String, oneshot::Sender<String>)>(10);
        let bridge = TerminalBridge::new(tx);

        let handle = tokio::spawn(async move {
            if let Some((input, reply)) = rx.recv().await {
                assert_eq!(input, "echo hello");
                let _ = reply.send("hello\n".to_string());
            }
        });

        let result = bridge
            .call(serde_json::json!({"input": "echo hello"}))
            .await
            .unwrap();

        handle.await.unwrap();

        match result {
            ToolResult::Success { output } => {
                assert_eq!(output[0].as_text().unwrap(), "hello\n");
            }
            _ => panic!("expected success"),
        }
    }
}
