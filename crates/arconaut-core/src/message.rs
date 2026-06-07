use chrono::{DateTime, Utc};
use serde::{Deserialize, Serialize};

#[derive(Debug, Clone, Serialize, Deserialize, PartialEq)]
pub struct Message {
    pub role: Role,
    pub content: Vec<ContentPart>,
    pub timestamp: Option<DateTime<Utc>>,
    pub metadata: MessageMetadata,
}

#[derive(Debug, Clone, Serialize, Deserialize, PartialEq, Default)]
pub struct MessageMetadata {
    pub source: Option<String>,
}

#[derive(Debug, Clone, Serialize, Deserialize, PartialEq)]
#[serde(rename_all = "snake_case")]
pub enum Role {
    User,
    Assistant,
    System,
    Tool,
}

#[derive(Debug, Clone, Serialize, Deserialize, PartialEq)]
#[serde(tag = "type", rename_all = "snake_case")]
pub enum ContentPart {
    Text { text: String },
    Image { url: String },
    ToolCall { tool_call: ToolCall },
    ToolResult { tool_result: ToolResultPart },
}

#[derive(Debug, Clone, Serialize, Deserialize, PartialEq)]
pub struct ToolCall {
    pub id: String,
    pub function: FunctionCall,
}

#[derive(Debug, Clone, Serialize, Deserialize, PartialEq)]
pub struct FunctionCall {
    pub name: String,
    pub arguments: String,
}

#[derive(Debug, Clone, Serialize, Deserialize, PartialEq)]
pub struct ToolResultPart {
    pub tool_call_id: String,
    pub output: Vec<ContentPart>,
    pub is_error: bool,
}

impl Message {
    pub fn new(role: Role, content: Vec<ContentPart>) -> Self {
        Self {
            role,
            content,
            timestamp: Some(Utc::now()),
            metadata: MessageMetadata::default(),
        }
    }

    pub fn user(text: impl Into<String>) -> Self {
        Self::new(Role::User, vec![ContentPart::text(text)])
    }

    pub fn assistant(text: impl Into<String>) -> Self {
        Self::new(Role::Assistant, vec![ContentPart::text(text)])
    }

    pub fn system(text: impl Into<String>) -> Self {
        Self::new(Role::System, vec![ContentPart::text(text)])
    }
}

impl ContentPart {
    pub fn text(text: impl Into<String>) -> Self {
        ContentPart::Text { text: text.into() }
    }

    pub fn as_text(&self) -> Option<&str> {
        match self {
            ContentPart::Text { text } => Some(text),
            _ => None,
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn message_roundtrip() {
        let msg = Message {
            role: Role::User,
            content: vec![ContentPart::text("hello")],
            timestamp: Some(Utc::now()),
            metadata: MessageMetadata::default(),
        };
        let json = serde_json::to_string(&msg).unwrap();
        let decoded: Message = serde_json::from_str(&json).unwrap();
        assert_eq!(msg, decoded);
    }

    #[test]
    fn content_part_discrimination() {
        let json = r#"{"type":"tool_call","tool_call":{"id":"123","function":{"name":"bash","arguments":"{}"}}}"#;
        let part: ContentPart = serde_json::from_str(json).unwrap();
        match part {
            ContentPart::ToolCall { tool_call } => {
                assert_eq!(tool_call.id, "123");
                assert_eq!(tool_call.function.name, "bash");
            }
            _ => panic!("Expected ToolCall variant, got {:?}", part),
        }
    }

    #[test]
    fn message_has_timestamp() {
        let before = Utc::now();
        let msg = Message::user("test");
        let after = Utc::now();
        let ts = msg.timestamp.expect("timestamp should be set");
        assert!(ts >= before && ts <= after);
    }

    #[test]
    fn message_convenience_constructors() {
        let user = Message::user("hello");
        assert_eq!(user.role, Role::User);
        assert_eq!(user.content, vec![ContentPart::text("hello")]);

        let assistant = Message::assistant("hi there");
        assert_eq!(assistant.role, Role::Assistant);

        let system = Message::system("you are helpful");
        assert_eq!(system.role, Role::System);
    }
}
