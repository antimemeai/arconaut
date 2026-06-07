use crate::AuditEvent;
use std::io::{BufWriter, Write};
use std::path::PathBuf;
use std::sync::{Arc, Mutex};

/// Append-only JSONL audit logger.
///
/// Events are written to `log_dir/{session_id}/events.jsonl`.
/// The file is never truncated — only appended to.
pub struct AuditLogger {
    session_id: String,
    writer: Arc<Mutex<BufWriter<std::fs::File>>>,
}

impl AuditLogger {
    /// Create a new audit logger for the given session.
    ///
    /// Creates the log directory if it does not exist.
    pub fn new(session_id: impl Into<String>, log_dir: PathBuf) -> Result<Self, AuditError> {
        let session_id = session_id.into();
        let dir = log_dir.join(&session_id);
        std::fs::create_dir_all(&dir).map_err(|e| AuditError::Io(e.to_string()))?;

        let path = dir.join("events.jsonl");
        let file = std::fs::OpenOptions::new()
            .create(true)
            .append(true)
            .open(&path)
            .map_err(|e| AuditError::Io(e.to_string()))?;

        Ok(Self {
            session_id,
            writer: Arc::new(Mutex::new(BufWriter::new(file))),
        })
    }

    /// Log a single event.
    pub fn log(&self, event: AuditEvent) {
        let json = match serde_json::to_string(&event) {
            Ok(s) => s,
            Err(e) => {
                eprintln!("audit serialize error: {}", e);
                return;
            }
        };
        let mut writer = self.writer.lock().unwrap();
        if let Err(e) = writeln!(writer, "{}", json) {
            eprintln!("audit write error: {}", e);
            return;
        }
        if let Err(e) = writer.flush() {
            eprintln!("audit flush error: {}", e);
        }
    }

    /// Convenience: log with the session_id pre-filled.
    pub fn log_event(&self, event_type: crate::EventType, payload: serde_json::Value) {
        self.log(AuditEvent::new(
            self.session_id.clone(),
            event_type,
            payload,
        ));
    }

    pub fn session_id(&self) -> &str {
        &self.session_id
    }
}

/// Errors from audit operations.
#[derive(Debug, Clone, PartialEq)]
pub enum AuditError {
    Io(String),
}

impl std::fmt::Display for AuditError {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            AuditError::Io(msg) => write!(f, "audit io error: {}", msg),
        }
    }
}

impl std::error::Error for AuditError {}

#[cfg(test)]
mod tests {
    use super::*;
    use crate::EventType;
    use tempfile::TempDir;

    #[test]
    fn creates_session_dir() {
        let dir = TempDir::new().unwrap();
        let logger = AuditLogger::new("test-session", dir.path().to_path_buf()).unwrap();
        assert_eq!(logger.session_id(), "test-session");
        assert!(dir.path().join("test-session").exists());
        assert!(dir.path().join("test-session/events.jsonl").exists());
    }

    #[test]
    fn append_preserves_history() {
        let dir = TempDir::new().unwrap();
        let logger = AuditLogger::new("s1", dir.path().to_path_buf()).unwrap();

        logger.log_event(EventType::TurnBegin, serde_json::Value::Null);
        logger.log_event(EventType::TurnEnd, serde_json::json!({"steps": 1}));

        let path = dir.path().join("s1/events.jsonl");
        let content = std::fs::read_to_string(&path).unwrap();
        let lines: Vec<_> = content.lines().collect();
        assert_eq!(lines.len(), 2);

        // Re-open and append more
        let logger2 = AuditLogger::new("s1", dir.path().to_path_buf()).unwrap();
        logger2.log_event(EventType::ToolCall, serde_json::json!({"tool": "read"}));

        let content = std::fs::read_to_string(&path).unwrap();
        let lines: Vec<_> = content.lines().collect();
        assert_eq!(lines.len(), 3);
    }

    #[test]
    fn log_roundtrip() {
        let dir = TempDir::new().unwrap();
        let logger = AuditLogger::new("s1", dir.path().to_path_buf()).unwrap();

        let event = AuditEvent::turn_begin("s1");
        logger.log(event.clone());

        let path = dir.path().join("s1/events.jsonl");
        let content = std::fs::read_to_string(&path).unwrap();
        let deserialized: AuditEvent = serde_json::from_str(content.trim()).unwrap();
        assert_eq!(deserialized.event_type, EventType::TurnBegin);
        assert_eq!(deserialized.session_id, "s1");
    }
}
