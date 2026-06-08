use arconaut_core::tool::{Tool, ToolError, ToolResult};
use arconaut_core::ContentPart;
use async_trait::async_trait;
use serde_json::Value;
use std::process::Stdio;
use std::time::Duration;
use tokio::process::Command;
use tokio::time::timeout;

/// Remote file read over SSH.
pub struct RemoteReadTool {
    params: Value,
}

impl Default for RemoteReadTool {
    fn default() -> Self {
        Self::new()
    }
}

impl RemoteReadTool {
    pub fn new() -> Self {
        Self {
            params: serde_json::json!({
                "type": "object",
                "properties": {
                    "host": { "type": "string", "description": "SSH host spec (user@host)" },
                    "path": { "type": "string" },
                    "offset": { "type": "integer", "default": 0 },
                    "limit": { "type": "integer", "default": 200 }
                },
                "required": ["host", "path"]
            }),
        }
    }
}

#[async_trait]
impl Tool for RemoteReadTool {
    fn name(&self) -> &str {
        "RemoteRead"
    }

    fn description(&self) -> &str {
        "Read a file on a remote host via SSH."
    }

    fn parameters(&self) -> &Value {
        &self.params
    }

    async fn call(&self, args: Value) -> Result<ToolResult, ToolError> {
        let host = args["host"].as_str().ok_or_else(|| ToolError {
            message: "missing 'host' argument".to_string(),
            brief: "invalid arguments".to_string(),
        })?;
        let path = args["path"].as_str().ok_or_else(|| ToolError {
            message: "missing 'path' argument".to_string(),
            brief: "invalid arguments".to_string(),
        })?;
        let offset = args["offset"].as_i64().unwrap_or(0);
        let limit = args["limit"].as_i64().unwrap_or(200);

        let remote_cmd = format!(
            "tail -n +{} '{}' | head -n {}",
            offset + 1,
            path.replace('\'', "'\"'\"'"),
            limit
        );

        let output = run_ssh(host, &remote_cmd, Duration::from_secs(30))
            .await
            .map_err(|e| ToolError {
                message: e,
                brief: "remote read failed".to_string(),
            })?;

        Ok(ToolResult::success(vec![ContentPart::Text {
            text: output.stdout,
        }]))
    }
}

/// Remote file write over SSH.
pub struct RemoteWriteTool {
    params: Value,
}

impl Default for RemoteWriteTool {
    fn default() -> Self {
        Self::new()
    }
}

impl RemoteWriteTool {
    pub fn new() -> Self {
        Self {
            params: serde_json::json!({
                "type": "object",
                "properties": {
                    "host": { "type": "string" },
                    "path": { "type": "string" },
                    "content": { "type": "string" }
                },
                "required": ["host", "path", "content"]
            }),
        }
    }
}

#[async_trait]
impl Tool for RemoteWriteTool {
    fn name(&self) -> &str {
        "RemoteWrite"
    }

    fn description(&self) -> &str {
        "Write a file on a remote host via SSH."
    }

    fn parameters(&self) -> &Value {
        &self.params
    }

    async fn call(&self, args: Value) -> Result<ToolResult, ToolError> {
        let host = args["host"].as_str().ok_or_else(|| ToolError {
            message: "missing 'host' argument".to_string(),
            brief: "invalid arguments".to_string(),
        })?;
        let path = args["path"].as_str().ok_or_else(|| ToolError {
            message: "missing 'path' argument".to_string(),
            brief: "invalid arguments".to_string(),
        })?;
        let content = args["content"].as_str().ok_or_else(|| ToolError {
            message: "missing 'content' argument".to_string(),
            brief: "invalid arguments".to_string(),
        })?;

        let remote_cmd = format!("cat > '{}'", path.replace('\'', "'\"'\"'"));
        let output = run_ssh_stdin(host, &remote_cmd, content, Duration::from_secs(30))
            .await
            .map_err(|e| ToolError {
                message: e,
                brief: "remote write failed".to_string(),
            })?;

        if output.exit_code != 0 {
            return Err(ToolError {
                message: format!("remote write failed: {}", output.stderr),
                brief: "remote write failed".to_string(),
            });
        }

        Ok(ToolResult::success(vec![ContentPart::Text {
            text: format!("Wrote to {path} on {host}"),
        }]))
    }
}

/// Remote search over SSH.
pub struct RemoteSearchTool {
    params: Value,
}

impl Default for RemoteSearchTool {
    fn default() -> Self {
        Self::new()
    }
}

impl RemoteSearchTool {
    pub fn new() -> Self {
        Self {
            params: serde_json::json!({
                "type": "object",
                "properties": {
                    "host": { "type": "string" },
                    "pattern": { "type": "string" },
                    "glob": { "type": "string", "default": "*" }
                },
                "required": ["host", "pattern"]
            }),
        }
    }
}

#[async_trait]
impl Tool for RemoteSearchTool {
    fn name(&self) -> &str {
        "RemoteSearch"
    }

    fn description(&self) -> &str {
        "Search files on a remote host via SSH using ripgrep."
    }

    fn parameters(&self) -> &Value {
        &self.params
    }

    async fn call(&self, args: Value) -> Result<ToolResult, ToolError> {
        let host = args["host"].as_str().ok_or_else(|| ToolError {
            message: "missing 'host' argument".to_string(),
            brief: "invalid arguments".to_string(),
        })?;
        let pattern = args["pattern"].as_str().ok_or_else(|| ToolError {
            message: "missing 'pattern' argument".to_string(),
            brief: "invalid arguments".to_string(),
        })?;
        let glob = args["glob"].as_str().unwrap_or("*");

        let remote_cmd = format!(
            "rg --line-number --glob '{}' '{}' || true",
            shell_escape(glob),
            shell_escape(pattern)
        );

        let output = run_ssh(host, &remote_cmd, Duration::from_secs(30))
            .await
            .map_err(|e| ToolError {
                message: e,
                brief: "remote search failed".to_string(),
            })?;

        Ok(ToolResult::success(vec![ContentPart::Text {
            text: output.stdout,
        }]))
    }
}

/// Remote command execution over SSH.
pub struct RemoteExecuteTool {
    params: Value,
}

impl Default for RemoteExecuteTool {
    fn default() -> Self {
        Self::new()
    }
}

impl RemoteExecuteTool {
    pub fn new() -> Self {
        Self {
            params: serde_json::json!({
                "type": "object",
                "properties": {
                    "host": { "type": "string" },
                    "command": { "type": "string" },
                    "cwd": { "type": "string" },
                    "timeout_ms": { "type": "integer", "default": 120000 }
                },
                "required": ["host", "command"]
            }),
        }
    }
}

#[async_trait]
impl Tool for RemoteExecuteTool {
    fn name(&self) -> &str {
        "RemoteExecute"
    }

    fn description(&self) -> &str {
        "Execute a command on a remote host via SSH."
    }

    fn parameters(&self) -> &Value {
        &self.params
    }

    async fn call(&self, args: Value) -> Result<ToolResult, ToolError> {
        let host = args["host"].as_str().ok_or_else(|| ToolError {
            message: "missing 'host' argument".to_string(),
            brief: "invalid arguments".to_string(),
        })?;
        let command = args["command"].as_str().ok_or_else(|| ToolError {
            message: "missing 'command' argument".to_string(),
            brief: "invalid arguments".to_string(),
        })?;
        let cwd = args["cwd"].as_str();
        let timeout_ms = args["timeout_ms"].as_i64().unwrap_or(120000);

        let remote_cmd = if let Some(dir) = cwd {
            format!("cd '{}' && {}", shell_escape(dir), command)
        } else {
            command.to_string()
        };

        let output = run_ssh(
            host,
            &remote_cmd,
            Duration::from_millis(timeout_ms as u64),
        )
        .await
        .map_err(|e| ToolError {
            message: e,
            brief: "remote execute failed".to_string(),
        })?;

        let text = format!(
            "exit_code: {}\nstdout:\n{}\nstderr:\n{}",
            output.exit_code, output.stdout, output.stderr
        );

        Ok(ToolResult::success(vec![ContentPart::Text { text }]))
    }
}

/// Result of running an SSH command.
struct SshOutput {
    stdout: String,
    stderr: String,
    exit_code: i32,
}

/// Run an SSH command and return output.
async fn run_ssh(host: &str, command: &str, dur: Duration) -> Result<SshOutput, String> {
    let mut child = Command::new("ssh")
        .arg(host)
        .arg(command)
        .stdout(Stdio::piped())
        .stderr(Stdio::piped())
        .spawn()
        .map_err(|e| format!("ssh spawn failed: {e}"))?;

    match timeout(dur, child.wait()).await {
        Ok(Ok(status)) => {
            let mut stdout = String::new();
            let mut stderr = String::new();
            if let Some(mut r) = child.stdout.take() {
                tokio::io::AsyncReadExt::read_to_string(&mut r, &mut stdout).await.ok();
            }
            if let Some(mut r) = child.stderr.take() {
                tokio::io::AsyncReadExt::read_to_string(&mut r, &mut stderr).await.ok();
            }
            Ok(SshOutput {
                stdout,
                stderr,
                exit_code: status.code().unwrap_or(-1),
            })
        }
        Ok(Err(e)) => Err(format!("ssh process error: {e}")),
        Err(_) => {
            let _ = child.start_kill();
            Err("ssh command timed out".to_string())
        }
    }
}

/// Run an SSH command with stdin.
async fn run_ssh_stdin(
    host: &str,
    command: &str,
    stdin_data: &str,
    dur: Duration,
) -> Result<SshOutput, String> {
    let mut child = Command::new("ssh")
        .arg(host)
        .arg(command)
        .stdin(Stdio::piped())
        .stdout(Stdio::piped())
        .stderr(Stdio::piped())
        .spawn()
        .map_err(|e| format!("ssh spawn failed: {e}"))?;

    if let Some(mut stdin) = child.stdin.take() {
        use tokio::io::AsyncWriteExt;
        stdin
            .write_all(stdin_data.as_bytes())
            .await
            .map_err(|e| format!("ssh stdin write failed: {e}"))?;
        stdin.shutdown().await.ok();
    }

    match timeout(dur, child.wait()).await {
        Ok(Ok(status)) => {
            let mut stdout = String::new();
            let mut stderr = String::new();
            if let Some(mut r) = child.stdout.take() {
                tokio::io::AsyncReadExt::read_to_string(&mut r, &mut stdout).await.ok();
            }
            if let Some(mut r) = child.stderr.take() {
                tokio::io::AsyncReadExt::read_to_string(&mut r, &mut stderr).await.ok();
            }
            Ok(SshOutput {
                stdout,
                stderr,
                exit_code: status.code().unwrap_or(-1),
            })
        }
        Ok(Err(e)) => Err(format!("ssh process error: {e}")),
        Err(_) => {
            let _ = child.start_kill();
            Err("ssh command timed out".to_string())
        }
    }
}

fn shell_escape(s: &str) -> String {
    format!("'{}'", s.replace('\'', "'\"'\"'"))
}
