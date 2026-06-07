use chrono::{DateTime, Utc};
use serde::{Deserialize, Serialize};
use serde_json::Value;

/// A single audit event.
#[derive(Debug, Clone, Serialize, Deserialize, PartialEq)]
pub struct AuditEvent {
    pub timestamp: DateTime<Utc>,
    pub session_id: String,
    pub event_type: EventType,
    pub payload: Value,
}

impl AuditEvent {
    pub fn new(session_id: impl Into<String>, event_type: EventType, payload: Value) -> Self {
        Self {
            timestamp: Utc::now(),
            session_id: session_id.into(),
            event_type,
            payload,
        }
    }

    pub fn turn_begin(session_id: impl Into<String>) -> Self {
        Self::new(session_id, EventType::TurnBegin, Value::Null)
    }

    pub fn turn_end(session_id: impl Into<String>, steps_taken: usize, stop_reason: &str) -> Self {
        Self::new(
            session_id,
            EventType::TurnEnd,
            serde_json::json!({
                "steps_taken": steps_taken,
                "stop_reason": stop_reason,
            }),
        )
    }

    pub fn tool_call(
        session_id: impl Into<String>,
        tool_name: impl Into<String>,
        args: Value,
    ) -> Self {
        Self::new(
            session_id,
            EventType::ToolCall,
            serde_json::json!({
                "tool_name": tool_name.into(),
                "args": args,
            }),
        )
    }

    pub fn tool_result(
        session_id: impl Into<String>,
        tool_name: impl Into<String>,
        success: bool,
        brief: impl Into<String>,
    ) -> Self {
        Self::new(
            session_id,
            EventType::ToolResult,
            serde_json::json!({
                "tool_name": tool_name.into(),
                "success": success,
                "brief": brief.into(),
            }),
        )
    }

    pub fn tool_error(
        session_id: impl Into<String>,
        tool_name: impl Into<String>,
        error: impl Into<String>,
    ) -> Self {
        Self::new(
            session_id,
            EventType::ToolError,
            serde_json::json!({
                "tool_name": tool_name.into(),
                "error": error.into(),
            }),
        )
    }

    pub fn compaction_begin(session_id: impl Into<String>, before_tokens: usize) -> Self {
        Self::new(
            session_id,
            EventType::CompactionBegin,
            serde_json::json!({"before_tokens": before_tokens}),
        )
    }

    pub fn compaction_end(session_id: impl Into<String>, after_tokens: usize) -> Self {
        Self::new(
            session_id,
            EventType::CompactionEnd,
            serde_json::json!({"after_tokens": after_tokens}),
        )
    }

    pub fn injection_applied(
        session_id: impl Into<String>,
        injector_type: impl Into<String>,
    ) -> Self {
        Self::new(
            session_id,
            EventType::InjectionApplied,
            serde_json::json!({"injector_type": injector_type.into()}),
        )
    }

    pub fn provider_switch(
        session_id: impl Into<String>,
        from: impl Into<String>,
        to: impl Into<String>,
    ) -> Self {
        Self::new(
            session_id,
            EventType::ProviderSwitch,
            serde_json::json!({
                "from": from.into(),
                "to": to.into(),
            }),
        )
    }

    pub fn churn_detected(session_id: impl Into<String>, level: &str) -> Self {
        Self::new(
            session_id,
            EventType::ChurnDetected,
            serde_json::json!({"level": level}),
        )
    }

    pub fn assistant_query(session_id: impl Into<String>, trigger: impl Into<String>) -> Self {
        Self::new(
            session_id,
            EventType::AssistantQuery,
            serde_json::json!({"trigger": trigger.into()}),
        )
    }
}

/// Types of audit events.
#[derive(Debug, Clone, Copy, Serialize, Deserialize, PartialEq, Eq)]
#[serde(rename_all = "snake_case")]
pub enum EventType {
    TurnBegin,
    TurnEnd,
    StepBegin,
    StepEnd,
    ToolCall,
    ToolResult,
    ToolError,
    CompactionBegin,
    CompactionEnd,
    ContextRevert,
    InjectionApplied,
    HookTrigger,
    HookBlock,
    UserInput,
    StatusUpdate,
    PlanModeToggle,
    ProviderSwitch,
    AssistantQuery,
    ChurnDetected,
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn event_serialization() {
        let event = AuditEvent::turn_begin("session-1");
        let json = serde_json::to_string(&event).unwrap();
        assert!(json.contains("turn_begin"));
        assert!(json.contains("session-1"));
    }

    #[test]
    fn event_deserialization() {
        let event = AuditEvent::turn_end("session-1", 5, "completed");
        let json = serde_json::to_string(&event).unwrap();
        let deserialized: AuditEvent = serde_json::from_str(&json).unwrap();
        assert_eq!(event, deserialized);
    }

    #[test]
    fn turn_end_payload() {
        let event = AuditEvent::turn_end("s1", 3, "max_steps");
        assert_eq!(event.event_type, EventType::TurnEnd);
        assert_eq!(event.payload["steps_taken"], 3);
        assert_eq!(event.payload["stop_reason"], "max_steps");
    }

    #[test]
    fn provider_switch_payload() {
        let event = AuditEvent::provider_switch("s1", "anthropic", "openrouter");
        assert_eq!(event.event_type, EventType::ProviderSwitch);
        assert_eq!(event.payload["from"], "anthropic");
        assert_eq!(event.payload["to"], "openrouter");
    }

    #[test]
    fn churn_detected_payload() {
        let event = AuditEvent::churn_detected("s1", "advisory");
        assert_eq!(event.event_type, EventType::ChurnDetected);
        assert_eq!(event.payload["level"], "advisory");
    }
}
