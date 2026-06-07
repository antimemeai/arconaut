use arconaut_core::{ContentPart, Tool, ToolError, ToolResult};
use async_trait::async_trait;
use serde_json::Value;
use std::sync::Arc;
use tokio::io::{AsyncBufReadExt, AsyncWriteExt, BufReader};
use tokio::sync::Mutex as AsyncMutex;
use tokio::process::{Child, ChildStdin, Command};
use tokio::sync::mpsc;

/// A persistent bash session with piped stdin/stdout/stderr.
/// A background task continuously reads output and forwards it
/// via an mpsc channel. The accumulated buffer is also queryable.
pub struct PersistentShell {
    stdin: ChildStdin,
    #[allow(dead_code)]
    output_tx: mpsc::Sender<String>,
    buffer: Arc<std::sync::Mutex<String>>,
    #[allow(dead_code)]
    child: Child,
}

impl PersistentShell {
    pub async fn new(output_tx: mpsc::Sender<String>) -> std::io::Result<Self> {
        let mut child = Command::new("bash")
            .arg("-i") // interactive mode for prompt/readline behavior
            .stdin(std::process::Stdio::piped())
            .stdout(std::process::Stdio::piped())
            .stderr(std::process::Stdio::piped())
            .spawn()?;

        let stdin = child
            .stdin
            .take()
            .expect("stdin was piped");
        let stdout = child
            .stdout
            .take()
            .expect("stdout was piped");
        let stderr = child
            .stderr
            .take()
            .expect("stderr was piped");

        let buffer = Arc::new(std::sync::Mutex::new(String::new()));
        let buf_stdout = Arc::clone(&buffer);
        let buf_stderr = Arc::clone(&buffer);
        let tx_stdout = output_tx.clone();
        let tx_stderr = output_tx.clone();

        // Spawn stdout reader task
        tokio::spawn(async move {
            let mut reader = BufReader::new(stdout);
            let mut line = String::new();
            loop {
                line.clear();
                match reader.read_line(&mut line).await {
                    Ok(0) => break, // EOF
                    Ok(_) => {
                        let trimmed = line.trim_end_matches('\n').to_string();
                        {
                            let mut buf = buf_stdout.lock().unwrap();
                            buf.push_str(&line);
                        }
                        let _ = tx_stdout.send(trimmed).await;
                    }
                    Err(_) => break,
                }
            }
        });

        // Spawn stderr reader task
        tokio::spawn(async move {
            let mut reader = BufReader::new(stderr);
            let mut line = String::new();
            loop {
                line.clear();
                match reader.read_line(&mut line).await {
                    Ok(0) => break,
                    Ok(_) => {
                        let trimmed = line.trim_end_matches('\n').to_string();
                        {
                            let mut buf = buf_stderr.lock().unwrap();
                            buf.push_str(&line);
                        }
                        let _ = tx_stderr.send(trimmed).await;
                    }
                    Err(_) => break,
                }
            }
        });

        Ok(Self {
            stdin,
            output_tx,
            buffer,
            child,
        })
    }

    /// Send a line to the shell's stdin. The line should NOT include a trailing newline.
    pub async fn send(&mut self, line: &str) -> std::io::Result<()> {
        let stdin = &mut self.stdin;
        stdin.write_all(line.as_bytes()).await?;
        stdin.write_all(b"\n").await?;
        stdin.flush().await?;
        Ok(())
    }

    /// Get the full accumulated output buffer.
    pub fn buffer(&self) -> String {
        self.buffer.lock().unwrap().clone()
    }

    /// Take (clear) the accumulated output buffer and return it.
    pub fn take_buffer(&self) -> String {
        std::mem::take(&mut *self.buffer.lock().unwrap())
    }
}

// ---------------------------------------------------------------------------
// terminal_send tool
// ---------------------------------------------------------------------------

pub struct TerminalSendTool {
    shell: Arc<AsyncMutex<Option<PersistentShell>>>,
    output_tx: mpsc::Sender<String>,
    params: Value,
}

impl TerminalSendTool {
    pub fn new(output_tx: mpsc::Sender<String>) -> Self {
        Self {
            shell: Arc::new(AsyncMutex::new(None)),
            output_tx,
            params: serde_json::json!({
                "type": "object",
                "properties": {
                    "input": {
                        "type": "string",
                        "description": "Text to send to the persistent terminal. Supports multi-line scripts."
                    }
                },
                "required": ["input"]
            }),
        }
    }

    pub async fn init_shell(&self) -> Result<(), ToolError> {
        let mut guard = self.shell.lock().await;
        if guard.is_none() {
            let shell = PersistentShell::new(self.output_tx.clone())
                .await
                .map_err(|e| ToolError {
                    message: format!("failed to start shell: {}", e),
                    brief: "shell error".to_string(),
                })?;
            *guard = Some(shell);
        }
        Ok(())
    }
}

#[async_trait]
impl Tool for TerminalSendTool {
    fn name(&self) -> &str {
        "terminal_send"
    }

    fn description(&self) -> &str {
        "Send input to the persistent interactive terminal. State (cwd, env vars) persists between calls. Supports multi-line input."
    }

    fn parameters(&self) -> &Value {
        &self.params
    }

    async fn call(&self, args: Value) -> Result<ToolResult, ToolError> {
        let input = args["input"].as_str().ok_or_else(|| ToolError {
            message: "missing 'input' argument".to_string(),
            brief: "bad args".to_string(),
        })?;

        self.init_shell().await?;

        let mut guard = self.shell.lock().await;
        let shell = guard.as_mut().unwrap();

        shell.send(input).await.map_err(|e| ToolError {
            message: format!("failed to send to terminal: {}", e),
            brief: "shell error".to_string(),
        })?;

        // Give the shell a moment to produce output, then capture buffer
        tokio::time::sleep(tokio::time::Duration::from_millis(100)).await;
        let output = shell.take_buffer();

        Ok(ToolResult::success(vec![ContentPart::text(output)]))
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use tokio::time::{sleep, Duration};

    #[tokio::test]
    async fn shell_echo_works() {
        let (tx, mut rx) = mpsc::channel::<String>(100);
        let mut shell = PersistentShell::new(tx).await.unwrap();

        shell.send("echo hello_from_test").await.unwrap();
        sleep(Duration::from_millis(300)).await;

        // Drain channel
        let mut found = false;
        while let Ok(line) = rx.try_recv() {
            if line.contains("hello_from_test") {
                found = true;
                break;
            }
        }
        assert!(found, "expected shell output to contain hello_from_test");
    }

    #[tokio::test]
    async fn shell_state_persists() {
        let (tx, _rx) = mpsc::channel::<String>(100);
        let mut shell = PersistentShell::new(tx).await.unwrap();

        shell.send("cd /tmp").await.unwrap();
        sleep(Duration::from_millis(200)).await;
        shell.send("pwd").await.unwrap();
        sleep(Duration::from_millis(200)).await;

        let buf = shell.buffer();
        assert!(buf.contains("/tmp"), "expected cwd to be /tmp, got: {}", buf);
    }

    #[tokio::test]
    async fn terminal_send_tool_registered() {
        let (tx, _rx) = mpsc::channel::<String>(100);
        let tool = TerminalSendTool::new(tx);
        assert_eq!(tool.name(), "terminal_send");
    }
}
