use arconaut_core::{ContentPart, Tool, ToolError, ToolResult};
use async_trait::async_trait;
use serde_json::Value;
use std::sync::Arc;

use crate::bus::{Bus, BusMessage, Target};

/// Tools that let an agent interact with the SLLMack bus.
pub struct BusTool {
    bus: Arc<Bus>,
    params: Value,
}

impl BusTool {
    pub fn new(bus: Arc<Bus>) -> Self {
        Self {
            bus,
            params: serde_json::json!({
                "type": "object",
                "properties": {
                    "action": {
                        "type": "string",
                        "enum": ["broadcast", "whisper", "presence"],
                        "description": "Action to perform"
                    },
                    "topic": { "type": "string", "description": "For broadcast: topic name" },
                    "to": { "type": "string", "description": "For whisper: target agent name" },
                    "message": { "type": "string", "description": "For broadcast/whisper: message body" },
                    "agent": { "type": "string", "description": "For presence: agent to query (omit to list all)" }
                },
                "required": ["action"]
            }),
        }
    }
}

#[async_trait]
impl Tool for BusTool {
    fn name(&self) -> &str {
        "bus"
    }

    fn description(&self) -> &str {
        "Interact with the agent bus (SLLMack). Broadcast to topics, whisper to agents, or check presence."
    }

    fn parameters(&self) -> &Value {
        &self.params
    }

    async fn call(&self, args: Value) -> Result<ToolResult, ToolError> {
        let action = args["action"].as_str().ok_or_else(|| ToolError {
            message: "missing 'action' argument".to_string(),
            brief: "bad args".to_string(),
        })?;

        match action {
            "broadcast" => {
                let topic = args["topic"].as_str().ok_or_else(|| ToolError {
                    message: "missing 'topic' for broadcast".to_string(),
                    brief: "bad args".to_string(),
                })?;
                let message = args["message"].as_str().unwrap_or("");
                let bus_msg = BusMessage {
                    from: "self".to_string(),
                    to: Target::Topic(topic.to_string()),
                    body: message.to_string(),
                };
                self.bus.broadcast(topic, bus_msg).await;
                Ok(ToolResult::success(vec![ContentPart::text(format!(
                    "Broadcasted to {}",
                    topic
                ))]))
            }
            "whisper" => {
                let to = args["to"].as_str().ok_or_else(|| ToolError {
                    message: "missing 'to' for whisper".to_string(),
                    brief: "bad args".to_string(),
                })?;
                let message = args["message"].as_str().unwrap_or("");
                let bus_msg = BusMessage {
                    from: "self".to_string(),
                    to: Target::Agent(to.to_string()),
                    body: message.to_string(),
                };
                self.bus.whisper(to, bus_msg).await;
                Ok(ToolResult::success(vec![ContentPart::text(format!(
                    "Whispered to {}",
                    to
                ))]))
            }
            "presence" => {
                let agent = args["agent"].as_str();
                let text = if let Some(name) = agent {
                    match self.bus.get_presence(name).await {
                        Some(p) => format!(
                            "{}: online={}, task={:?}",
                            name, p.online, p.current_task
                        ),
                        None => format!("{}: unknown", name),
                    }
                } else {
                    let list = self.bus.list_presence().await;
                    if list.is_empty() {
                        "No agents online".to_string()
                    } else {
                        list.into_iter()
                            .map(|(name, p)| {
                                format!(
                                    "{}: online={}, task={:?}",
                                    name, p.online, p.current_task
                                )
                            })
                            .collect::<Vec<_>>()
                            .join("\n")
                    }
                };
                Ok(ToolResult::success(vec![ContentPart::text(text)]))
            }
            _ => Err(ToolError {
                message: format!("unknown bus action: {}", action),
                brief: "bad action".to_string(),
            }),
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[tokio::test]
    async fn bus_broadcast_tool() {
        let bus = Arc::new(Bus::new());
        let mut rx = bus.join_topic("#test").await;
        let tool = BusTool::new(Arc::clone(&bus));

        let result = tool
            .call(serde_json::json!({
                "action": "broadcast",
                "topic": "#test",
                "message": "hello world"
            }))
            .await
            .unwrap();

        match result {
            ToolResult::Success { output } => {
                assert!(output[0].as_text().unwrap().contains("Broadcasted"));
            }
            _ => panic!("expected success"),
        }

        let msg = rx.recv().await.unwrap();
        assert_eq!(msg.body, "hello world");
    }

    #[tokio::test]
    async fn bus_whisper_tool() {
        let bus = Arc::new(Bus::new());
        let mut rx = bus.register_mailbox("alice").await;
        let tool = BusTool::new(Arc::clone(&bus));

        tool
            .call(serde_json::json!({
                "action": "whisper",
                "to": "alice",
                "message": "secret"
            }))
            .await
            .unwrap();

        let msg = rx.recv().await.unwrap();
        assert_eq!(msg.body, "secret");
    }

    #[tokio::test]
    async fn bus_presence_tool() {
        let bus = Arc::new(Bus::new());
        bus.set_presence("bob", crate::bus::Presence {
            online: true,
            current_task: Some("coding".to_string()),
        })
        .await;

        let tool = BusTool::new(Arc::clone(&bus));
        let result = tool
            .call(serde_json::json!({"action": "presence", "agent": "bob"}))
            .await
            .unwrap();

        match result {
            ToolResult::Success { output } => {
                let text = output[0].as_text().unwrap();
                assert!(text.contains("online=true"));
                assert!(text.contains("coding"));
            }
            _ => panic!("expected success"),
        }
    }
}
