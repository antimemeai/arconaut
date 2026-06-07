use serde_json::Value;
use std::collections::HashMap;
use std::path::Path;

/// Scope for variable lookup.
#[derive(Debug, Clone, Copy, PartialEq, Eq, Hash)]
pub enum VariableScope {
    System,
    Project,
    Session,
}

/// Three-level variable store: system → project → session.
/// Session overrides project, which overrides system.
pub struct VariableStore {
    system: HashMap<String, Value>,
    project: HashMap<String, Value>,
    session: HashMap<String, Value>,
}

impl VariableStore {
    pub fn new() -> Self {
        Self {
            system: HashMap::new(),
            project: HashMap::new(),
            session: HashMap::new(),
        }
    }

    /// Load system variables from `~/.config/arconaut/vars.toml`.
    pub async fn load_system(&mut self, path: impl AsRef<Path>) {
        if let Ok(content) = tokio::fs::read_to_string(path).await {
            if let Ok(table) = content.parse::<toml::Table>() {
                Self::flatten_table("", &table, &mut self.system);
            }
        }
    }

    /// Load project variables from `./.arconaut/vars.toml`.
    pub async fn load_project(&mut self, path: impl AsRef<Path>) {
        if let Ok(content) = tokio::fs::read_to_string(path).await {
            if let Ok(table) = content.parse::<toml::Table>() {
                Self::flatten_table("", &table, &mut self.project);
            }
        }
    }

    /// Get a variable by key, with scope precedence: session > project > system.
    pub fn get(&self, key: &str) -> Option<&Value> {
        self.session
            .get(key)
            .or_else(|| self.project.get(key))
            .or_else(|| self.system.get(key))
    }

    /// Set a session-scoped variable.
    pub fn set(&mut self, key: impl Into<String>, value: Value) {
        self.session.insert(key.into(), value);
    }

    /// Set a variable in a specific scope.
    pub fn set_scoped(&mut self, scope: VariableScope, key: impl Into<String>, value: Value) {
        match scope {
            VariableScope::System => self.system.insert(key.into(), value),
            VariableScope::Project => self.project.insert(key.into(), value),
            VariableScope::Session => self.session.insert(key.into(), value),
        };
    }

    /// Replace `{var:scope.key}` templates in a string.
    pub fn substitute(&self, input: &str) -> String {
        let mut output = input.to_string();
        // Simple regex-like replacement for {var:SCOPE.KEY}
        for (scope_name, map) in [
            ("session", &self.session),
            ("project", &self.project),
            ("system", &self.system),
        ] {
            for (key, value) in map {
                let template = format!("{{var:{}.{}}}", scope_name, key);
                let replacement = value.as_str().map(|s| s.to_string()).unwrap_or_else(|| value.to_string());
                output = output.replace(&template, &replacement);
            }
        }
        output
    }

    fn flatten_table(prefix: &str, table: &toml::Table, target: &mut HashMap<String, Value>) {
        for (key, value) in table {
            let full_key = if prefix.is_empty() {
                key.clone()
            } else {
                format!("{}.{}", prefix, key)
            };
            match value {
                toml::Value::Table(sub) => Self::flatten_table(&full_key, sub, target),
                _ => {
                    if let Ok(json) = serde_json::to_value(value) {
                        target.insert(full_key, json);
                    }
                }
            }
        }
    }
}

impl Default for VariableStore {
    fn default() -> Self {
        Self::new()
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use serde_json::json;
    use tempfile::TempDir;

    #[tokio::test]
    async fn system_load() {
        let dir = TempDir::new().unwrap();
        let path = dir.path().join("vars.toml");
        tokio::fs::write(&path, "api_key = \"secret\"\n")
            .await
            .unwrap();

        let mut store = VariableStore::new();
        store.load_system(&path).await;
        assert_eq!(store.get("api_key"), Some(&json!("secret")));
    }

    #[tokio::test]
    async fn project_load() {
        let dir = TempDir::new().unwrap();
        let path = dir.path().join("vars.toml");
        tokio::fs::write(&path, "[project]\nname = \"arconaut\"\n")
            .await
            .unwrap();

        let mut store = VariableStore::new();
        store.load_project(&path).await;
        assert_eq!(store.get("project.name"), Some(&json!("arconaut")));
    }

    #[test]
    fn session_ephemeral() {
        let mut store = VariableStore::new();
        store.set("temp", json!(42));
        assert_eq!(store.get("temp"), Some(&json!(42)));
    }

    #[test]
    fn precedence() {
        let mut store = VariableStore::new();
        store.set_scoped(VariableScope::System, "key", json!("system"));
        store.set_scoped(VariableScope::Project, "key", json!("project"));
        store.set_scoped(VariableScope::Session, "key", json!("session"));
        assert_eq!(store.get("key"), Some(&json!("session")));
    }

    #[test]
    fn substitution() {
        let mut store = VariableStore::new();
        store.set_scoped(VariableScope::System, "name", json!("world"));
        let result = store.substitute("Hello {var:system.name}!");
        assert_eq!(result, "Hello world!");
    }
}
