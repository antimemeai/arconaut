use super::oauth::OAuthToken;
use std::io::Write;
use std::path::PathBuf;

/// Error from credential storage operations.
#[derive(Debug, Clone, PartialEq)]
pub enum StorageError {
    Io(String),
    Serialize(String),
}

impl std::fmt::Display for StorageError {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            StorageError::Io(msg) => write!(f, "storage io error: {}", msg),
            StorageError::Serialize(msg) => write!(f, "storage serialize error: {}", msg),
        }
    }
}

impl std::error::Error for StorageError {}

/// Trait for persisting OAuth tokens.
pub trait CredentialStorage: Send + Sync {
    /// Load a token by key. Returns `None` if not found.
    fn load(&self, key: &str) -> Result<Option<OAuthToken>, StorageError>;

    /// Save a token under the given key.
    fn save(&self, key: &str, token: &OAuthToken) -> Result<(), StorageError>;

    /// Delete a token by key.
    fn delete(&self, key: &str) -> Result<(), StorageError>;
}

/// File-based credential storage.
///
/// Tokens are stored as JSON files under a base directory.
/// On Unix, files are created with 0o600 permissions.
pub struct FileStorage {
    base_dir: PathBuf,
}

impl FileStorage {
    /// Create a new file storage using the default config directory.
    ///
    /// Default path: `~/.config/arconaut/oauth/`
    pub fn new() -> Result<Self, StorageError> {
        let base_dir = dirs::config_dir()
            .unwrap_or_else(std::env::temp_dir)
            .join("arconaut")
            .join("oauth");
        std::fs::create_dir_all(&base_dir)
            .map_err(|e| StorageError::Io(e.to_string()))?;
        Ok(Self { base_dir })
    }

    /// Create a file storage with an explicit base directory.
    pub fn with_dir(base_dir: PathBuf) -> Self {
        Self { base_dir }
    }

    fn path(&self, key: &str) -> PathBuf {
        let name = key.trim_start_matches("oauth/");
        let name = name.split('/').next_back().unwrap_or(name);
        self.base_dir.join(format!("{}.json", name))
    }
}

impl CredentialStorage for FileStorage {
    fn load(&self, key: &str) -> Result<Option<OAuthToken>, StorageError> {
        let path = self.path(key);
        if !path.exists() {
            return Ok(None);
        }
        let content =
            std::fs::read_to_string(&path).map_err(|e| StorageError::Io(e.to_string()))?;
        let token: OAuthToken =
            serde_json::from_str(&content).map_err(|e| StorageError::Serialize(e.to_string()))?;
        Ok(Some(token))
    }

    fn save(&self, key: &str, token: &OAuthToken) -> Result<(), StorageError> {
        let path = self.path(key);
        if let Some(parent) = path.parent() {
            std::fs::create_dir_all(parent)
                .map_err(|e| StorageError::Io(e.to_string()))?;
        }
        let json =
            serde_json::to_string_pretty(token).map_err(|e| StorageError::Serialize(e.to_string()))?;

        // Atomic write: write to temp file, then rename.
        let tmp = path.with_extension("tmp");
        {
            let mut file = std::fs::File::create(&tmp)
                .map_err(|e| StorageError::Io(e.to_string()))?;
            file.write_all(json.as_bytes())
                .map_err(|e| StorageError::Io(e.to_string()))?;
            file.sync_all()
                .map_err(|e| StorageError::Io(e.to_string()))?;
        }
        #[cfg(unix)]
        {
            use std::os::unix::fs::PermissionsExt;
            let _ = std::fs::set_permissions(&tmp, std::fs::Permissions::from_mode(0o600));
        }
        std::fs::rename(&tmp, &path).map_err(|e| StorageError::Io(e.to_string()))?;
        Ok(())
    }

    fn delete(&self, key: &str) -> Result<(), StorageError> {
        let path = self.path(key);
        if path.exists() {
            std::fs::remove_file(&path).map_err(|e| StorageError::Io(e.to_string()))?;
        }
        Ok(())
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use chrono::Utc;
    use tempfile::TempDir;

    fn test_token() -> OAuthToken {
        OAuthToken {
            access_token: "access-123".to_string(),
            refresh_token: "refresh-456".to_string(),
            expires_at: Some(Utc::now() + chrono::Duration::hours(1)),
            scope: "all".to_string(),
            token_type: "Bearer".to_string(),
            expires_in: 3600,
        }
    }

    #[test]
    fn save_load_roundtrip() {
        let dir = TempDir::new().unwrap();
        let storage = FileStorage::with_dir(dir.path().to_path_buf());
        let token = test_token();

        storage.save("kimi", &token).unwrap();
        let loaded = storage.load("kimi").unwrap().unwrap();
        assert_eq!(loaded.access_token, token.access_token);
        assert_eq!(loaded.refresh_token, token.refresh_token);
    }

    #[test]
    fn load_missing_returns_none() {
        let dir = TempDir::new().unwrap();
        let storage = FileStorage::with_dir(dir.path().to_path_buf());
        assert!(storage.load("nonexistent").unwrap().is_none());
    }

    #[test]
    fn delete_removes_file() {
        let dir = TempDir::new().unwrap();
        let storage = FileStorage::with_dir(dir.path().to_path_buf());
        let token = test_token();

        storage.save("kimi", &token).unwrap();
        assert!(storage.load("kimi").unwrap().is_some());

        storage.delete("kimi").unwrap();
        assert!(storage.load("kimi").unwrap().is_none());
    }

    #[test]
    fn token_fields_preserved() {
        let dir = TempDir::new().unwrap();
        let storage = FileStorage::with_dir(dir.path().to_path_buf());
        let token = test_token();

        storage.save("kimi", &token).unwrap();
        let loaded = storage.load("kimi").unwrap().unwrap();
        assert_eq!(loaded.scope, "all");
        assert_eq!(loaded.token_type, "Bearer");
        assert_eq!(loaded.expires_in, 3600);
        assert!(loaded.expires_at.is_some());
    }

    #[test]
    fn expired_token_detected() {
        let mut token = test_token();
        token.expires_at = Some(Utc::now() - chrono::Duration::minutes(1));
        assert!(token.is_expired());
    }

    #[test]
    fn needs_refresh_near_expiry() {
        let mut token = test_token();
        // 4 minutes to expiry → should need refresh
        token.expires_at = Some(Utc::now() + chrono::Duration::minutes(4));
        assert!(token.needs_refresh());

        // 10 minutes to expiry → should not need refresh
        token.expires_at = Some(Utc::now() + chrono::Duration::minutes(10));
        assert!(!token.needs_refresh());
    }
}
