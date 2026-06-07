use std::path::PathBuf;

/// Stable device identification for OAuth requests.
///
/// Kimi's OAuth backend requires device info headers on all token requests.
/// The device_id is a stable UUID persisted to disk.
#[derive(Debug, Clone)]
pub struct DeviceInfo {
    pub platform: String,
    pub version: String,
    pub device_name: String,
    pub device_model: String,
    pub os_version: String,
    pub device_id: String,
}

impl DeviceInfo {
    /// Generate device info from the current environment.
    pub fn generate() -> Self {
        Self {
            platform: "arconaut".to_string(),
            version: env!("CARGO_PKG_VERSION").to_string(),
            device_name: get_hostname(),
            device_model: device_model(),
            os_version: get_os_version(),
            device_id: get_device_id(),
        }
    }

    /// Return the device info as HTTP headers for OAuth requests.
    pub fn headers(&self) -> Vec<(String, String)> {
        vec![
            ("X-Msh-Platform".to_string(), ascii_safe(&self.platform)),
            ("X-Msh-Version".to_string(), ascii_safe(&self.version)),
            (
                "X-Msh-Device-Name".to_string(),
                ascii_safe(&self.device_name),
            ),
            (
                "X-Msh-Device-Model".to_string(),
                ascii_safe(&self.device_model),
            ),
            (
                "X-Msh-Os-Version".to_string(),
                ascii_safe(&self.os_version),
            ),
            ("X-Msh-Device-Id".to_string(), ascii_safe(&self.device_id)),
        ]
    }
}

/// Get or create a stable device UUID.
///
/// The UUID is persisted to `~/.config/arconaut/device_id`.
/// On Unix, the file is created with 0o600 permissions.
pub fn get_device_id() -> String {
    let path = device_id_path();
    if let Ok(existing) = std::fs::read_to_string(&path) {
        let trimmed = existing.trim();
        if !trimmed.is_empty() {
            return trimmed.to_string();
        }
    }
    let id = uuid::Uuid::new_v4().to_string();
    if let Some(parent) = path.parent() {
        let _ = std::fs::create_dir_all(parent);
    }
    let _ = std::fs::write(&path, &id);
    #[cfg(unix)]
    {
        use std::os::unix::fs::PermissionsExt;
        let _ = std::fs::set_permissions(&path, std::fs::Permissions::from_mode(0o600));
    }
    id
}

fn device_id_path() -> PathBuf {
    dirs::config_dir()
        .unwrap_or_else(std::env::temp_dir)
        .join("arconaut")
        .join("device_id")
}

fn get_hostname() -> String {
    // Try HOSTNAME env var first (common on Unix).
    if let Ok(h) = std::env::var("HOSTNAME") {
        return h;
    }
    // Fallback: use COMPUTERNAME on Windows.
    if let Ok(h) = std::env::var("COMPUTERNAME") {
        return h;
    }
    // Last resort: use a generic name.
    "arconaut-device".to_string()
}

fn device_model() -> String {
    let os = std::env::consts::OS;
    let arch = std::env::consts::ARCH;
    let release = get_os_version();
    if release.is_empty() || release == "unknown" {
        format!("{} {}", os, arch)
    } else {
        format!("{} {} {}", os, release, arch)
    }
}

fn get_os_version() -> String {
    #[cfg(target_os = "macos")]
    {
        // Try sw_vers on macOS.
        if let Ok(output) = std::process::Command::new("sw_vers")
            .arg("-productVersion")
            .output()
        {
            if output.status.success() {
                let ver = String::from_utf8_lossy(&output.stdout);
                let ver = ver.trim();
                if !ver.is_empty() {
                    return ver.to_string();
                }
            }
        }
    }
    #[cfg(target_os = "linux")]
    {
        // Try /etc/os-release on Linux.
        if let Ok(content) = std::fs::read_to_string("/etc/os-release") {
            for line in content.lines() {
                if let Some(val) = line.strip_prefix("VERSION_ID=") {
                    return val.trim_matches('"').to_string();
                }
            }
        }
    }
    #[cfg(target_os = "windows")]
    {
        if let Ok(ver) = std::env::var("OS") {
            return ver;
        }
    }
    "unknown".to_string()
}

fn ascii_safe(value: &str) -> String {
    match value.is_ascii() {
        true => value.to_string(),
        false => value
            .chars()
            .filter(|c| c.is_ascii())
            .collect::<String>()
            .trim()
            .to_string(),
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn info_has_all_fields() {
        let info = DeviceInfo::generate();
        assert!(!info.platform.is_empty());
        assert!(!info.version.is_empty());
        assert!(!info.device_name.is_empty());
        assert!(!info.device_model.is_empty());
        assert!(!info.os_version.is_empty());
        assert!(!info.device_id.is_empty());
    }

    #[test]
    fn headers_are_ascii() {
        let info = DeviceInfo::generate();
        for (key, value) in info.headers() {
            assert!(key.is_ascii(), "header key must be ascii: {}", key);
            assert!(value.is_ascii(), "header value must be ascii: {}", value);
            assert!(!key.is_empty());
        }
    }

    #[test]
    #[serial_test::serial]
    fn device_id_is_stable() {
        let id1 = get_device_id();
        let id2 = get_device_id();
        assert_eq!(id1, id2);
        assert!(!id1.is_empty());
        // Should be a valid UUID
        assert!(uuid::Uuid::parse_str(&id1).is_ok());
    }

    #[test]
    fn ascii_safe_strips_non_ascii() {
        let input = "hello\u{1F600}world";
        let result = ascii_safe(input);
        assert_eq!(result, "helloworld");
    }

    #[test]
    fn ascii_safe_passes_through() {
        let input = "hello world";
        let result = ascii_safe(input);
        assert_eq!(result, "hello world");
    }
}
