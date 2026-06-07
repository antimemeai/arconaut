use arconaut_core::Message;
use arconaut_machine::{ChatProvider, ProviderError};
use regex::Regex;
use std::sync::Arc;
use std::time::{Duration, Instant};

use crate::bus::Bus;

/// Events that can trigger the assistant model.
#[derive(Debug, Clone, PartialEq)]
pub enum AgentEvent {
    ToolFailure { tool_name: String, error: String },
    Compaction,
    MaxStepsWarning { steps_taken: usize },
    UserInput { text: String },
    Periodic,
}

/// Configured triggers for the assistant model.
#[derive(Debug, Clone)]
pub enum TriggerEvent {
    OnToolFailure,
    OnCompaction,
    OnMaxStepsWarning,
    OnUserRequest { pattern: Regex },
    Periodic { interval: Duration },
}

impl PartialEq for TriggerEvent {
    fn eq(&self, other: &Self) -> bool {
        match (self, other) {
            (TriggerEvent::OnToolFailure, TriggerEvent::OnToolFailure) => true,
            (TriggerEvent::OnCompaction, TriggerEvent::OnCompaction) => true,
            (TriggerEvent::OnMaxStepsWarning, TriggerEvent::OnMaxStepsWarning) => true,
            (TriggerEvent::OnUserRequest { pattern: a }, TriggerEvent::OnUserRequest { pattern: b }) => {
                a.as_str() == b.as_str()
            }
            (TriggerEvent::Periodic { interval: a }, TriggerEvent::Periodic { interval: b }) => {
                a == b
            }
            _ => false,
        }
    }
}

/// A secondary model that the primary agent can query for help.
pub struct AssistantModel {
    provider: Box<dyn ChatProvider>,
    triggers: Vec<TriggerEvent>,
    proactive: bool,
    bus: Option<Arc<Bus>>,
    last_periodic_check: Option<Instant>,
}

impl AssistantModel {
    pub fn new(provider: Box<dyn ChatProvider>) -> Self {
        Self {
            provider,
            triggers: vec![
                TriggerEvent::OnToolFailure,
                TriggerEvent::OnMaxStepsWarning,
            ],
            proactive: false,
            bus: None,
            last_periodic_check: None,
        }
    }

    pub fn with_triggers(mut self, triggers: Vec<TriggerEvent>) -> Self {
        self.triggers = triggers;
        self
    }

    pub fn with_proactive(mut self, proactive: bool) -> Self {
        self.proactive = proactive;
        self
    }

    pub fn with_bus(mut self, bus: Arc<Bus>) -> Self {
        self.bus = Some(bus);
        self
    }

    /// Check if any trigger fires for the given event.
    pub fn check_triggers(&mut self, event: &AgentEvent) -> bool {
        for trigger in &self.triggers {
            match (trigger, event) {
                (TriggerEvent::OnToolFailure, AgentEvent::ToolFailure { .. }) => return true,
                (TriggerEvent::OnCompaction, AgentEvent::Compaction) => return true,
                (TriggerEvent::OnMaxStepsWarning, AgentEvent::MaxStepsWarning { .. }) => {
                    return true
                }
                (
                    TriggerEvent::OnUserRequest { pattern },
                    AgentEvent::UserInput { text },
                ) => {
                    if pattern.is_match(text) {
                        return true;
                    }
                }
                (TriggerEvent::Periodic { interval }, AgentEvent::Periodic) => {
                    if let Some(last) = self.last_periodic_check {
                        if last.elapsed() >= *interval {
                            self.last_periodic_check = Some(Instant::now());
                            return true;
                        }
                    } else {
                        self.last_periodic_check = Some(Instant::now());
                        return true;
                    }
                }
                _ => {}
            }
        }
        false
    }

    /// Query the assistant with a context summary.
    pub async fn query(&self, context: &str) -> Result<Message, ProviderError> {
        use arconaut_machine::ChatRequest;
        let request = ChatRequest {
            messages: vec![arconaut_core::Message::user(format!(
                "You are an assistant to the primary agent. \
                 The primary agent needs help with the following situation:\n\n{}",
                context
            ))],
            tools: vec![],
            system_prompt: Some(
                "You are a helpful assistant. Provide concise, actionable advice. \
                 If the primary agent is stuck, suggest a concrete next step."
                    .to_string(),
            ),
        };
        let response = self.provider.chat(request).await?;
        Ok(response.message)
    }

    /// Deliver the assistant's response to the primary agent.
    pub fn deliver(&self, message: &Message) {
        if let Some(ref bus) = self.bus {
            let text = message
                .content
                .iter()
                .filter_map(|p| p.as_text())
                .collect::<Vec<_>>()
                .join("");
            std::mem::drop(bus.broadcast("assistant", crate::bus::BusMessage {
                from: "assistant".to_string(),
                to: crate::bus::Target::Topic("assistant".to_string()),
                body: text,
            }));
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use arconaut_machine::{ChatResponse, ModelCapability, ProviderError, TokenUsage};

    struct DummyProvider;

    #[async_trait::async_trait]
    impl ChatProvider for DummyProvider {
        async fn chat(
            &self,
            _request: arconaut_machine::ChatRequest,
        ) -> Result<ChatResponse, ProviderError> {
            Ok(ChatResponse {
                message: arconaut_core::Message::assistant("try rebooting"),
                usage: TokenUsage { input: 10, output: 5 },
                id: "test".to_string(),
            })
        }

        fn model_name(&self) -> &str {
            "dummy"
        }

        fn max_context_size(&self) -> usize {
            1000
        }

        fn capabilities(&self) -> std::collections::HashSet<ModelCapability> {
            let mut caps = std::collections::HashSet::new();
            caps.insert(ModelCapability::Text);
            caps
        }

        fn thinking_effort(&self) -> Option<&str> {
            None
        }
    }

    #[test]
    fn trigger_tool_failure() {
        let mut assistant = AssistantModel::new(Box::new(DummyProvider));
        let event = AgentEvent::ToolFailure {
            tool_name: "bash".to_string(),
            error: "command not found".to_string(),
        };
        assert!(assistant.check_triggers(&event));
    }

    #[test]
    fn trigger_max_steps() {
        let mut assistant = AssistantModel::new(Box::new(DummyProvider));
        let event = AgentEvent::MaxStepsWarning { steps_taken: 50 };
        assert!(assistant.check_triggers(&event));
    }

    #[test]
    fn trigger_no_false_positive() {
        let mut assistant = AssistantModel::new(Box::new(DummyProvider));
        assistant.triggers = vec![TriggerEvent::OnCompaction];
        let event = AgentEvent::ToolFailure {
            tool_name: "bash".to_string(),
            error: "fail".to_string(),
        };
        assert!(!assistant.check_triggers(&event));
    }

    #[test]
    fn trigger_user_request_pattern() {
        let mut assistant = AssistantModel::new(Box::new(DummyProvider));
        assistant.triggers = vec![TriggerEvent::OnUserRequest {
            pattern: Regex::new("help").unwrap(),
        }];
        let event = AgentEvent::UserInput {
            text: "I need help with this".to_string(),
        };
        assert!(assistant.check_triggers(&event));

        let event2 = AgentEvent::UserInput {
            text: "All good".to_string(),
        };
        assert!(!assistant.check_triggers(&event2));
    }

    #[tokio::test]
    async fn query_returns_message() {
        let assistant = AssistantModel::new(Box::new(DummyProvider));
        let result = assistant.query("test context").await.unwrap();
        assert_eq!(result.content[0].as_text().unwrap(), "try rebooting");
    }
}
