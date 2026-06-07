use chrono::{DateTime, Utc};
use serde::{Deserialize, Serialize};
use std::path::{Path, PathBuf};

/// A named session with its own audit partition and persisted state.
#[derive(Debug, Clone, Serialize, Deserialize)]
pub struct Session {
    pub name: String,
    pub agent_name: String,
    pub created_at: DateTime<Utc>,
    pub audit_partition: String,
    pub state_path: PathBuf,
}

impl Session {
    pub fn new(name: impl Into<String>, agent_name: impl Into<String>) -> Self {
        let name = name.into();
        let agent_name = agent_name.into();
        let audit_partition = format!("{}-{}", agent_name, name);
        let home = std::env::var("HOME").map(PathBuf::from).ok();
        let state_path = home
            .map(|h| h.join(".local").join("share").join("arconaut").join("sessions"))
            .unwrap_or_else(|| PathBuf::from(".arconaut").join("sessions"))
            .join(&audit_partition)
            .join("state.json");
        Self {
            name,
            agent_name,
            created_at: Utc::now(),
            audit_partition,
            state_path,
        }
    }

    pub fn audit_log_path(&self) -> PathBuf {
        self.state_path
            .parent()
            .unwrap_or(Path::new("."))
            .join("audit.jsonl")
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn audit_partition() {
        let session = Session::new("debug-2026-06-07", "implementer");
        assert!(session.audit_partition.contains("implementer"));
        assert!(session.audit_partition.contains("debug"));
        assert!(session.audit_log_path().to_string_lossy().ends_with("audit.jsonl"));
    }
}
