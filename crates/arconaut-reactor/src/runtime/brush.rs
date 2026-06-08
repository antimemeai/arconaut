use std::path::PathBuf;
use std::time::Duration;

/// Brush runtime for one-off and persistent shell execution.
pub struct BrushRuntime;

/// Result of executing a command.
#[derive(Debug, Clone, PartialEq)]
pub struct ExecResult {
    pub exit_code: i32,
    pub stdout: String,
    pub stderr: String,
    pub duration: Duration,
    pub cwd: PathBuf,
    pub osc_633: Option<Osc633>,
}

/// Parsed OSC 633 sequences.
#[derive(Debug, Clone, PartialEq)]
pub struct Osc633 {
    pub command: String,
    pub cwd: Option<PathBuf>,
    pub continuation: bool,
}

/// Errors from the brush runtime.
#[derive(Debug, Clone, PartialEq)]
pub enum BrushError {
    Spawn(String),
    Exec(String),
    Timeout,
}

impl std::fmt::Display for BrushError {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            BrushError::Spawn(e) => write!(f, "spawn error: {e}"),
            BrushError::Exec(e) => write!(f, "execution error: {e}"),
            BrushError::Timeout => write!(f, "brush command timed out"),
        }
    }
}

impl std::error::Error for BrushError {}

impl BrushRuntime {
    /// Create a new brush shell with default environment.
    pub fn new() -> Result<Self, BrushError> {
        // TODO(Phase A): initialize brush-core shell.
        Ok(Self)
    }

    /// Execute a one-off command.
    pub async fn exec(&mut self, cmd: &str) -> Result<ExecResult, BrushError> {
        // TODO(Phase A): replace with actual brush-core execution.
        // Stub: fall back to std::process::Command for red tests.
        let start = std::time::Instant::now();
        let output = std::process::Command::new("sh")
            .arg("-c")
            .arg(cmd)
            .output()
            .map_err(|e| BrushError::Exec(e.to_string()))?;

        Ok(ExecResult {
            exit_code: output.status.code().unwrap_or(-1),
            stdout: String::from_utf8_lossy(&output.stdout).to_string(),
            stderr: String::from_utf8_lossy(&output.stderr).to_string(),
            duration: start.elapsed(),
            cwd: std::env::current_dir().unwrap_or_else(|_| PathBuf::from(".")),
            osc_633: None,
        })
    }

    /// Execute a command in the persistent session.
    pub async fn exec_persistent(&mut self, cmd: &str) -> Result<ExecResult, BrushError> {
        // Phase A: same as exec. Differentiated in Phase B.
        self.exec(cmd).await
    }
}

impl Default for BrushRuntime {
    fn default() -> Self {
        Self::new().expect("BrushRuntime::new should succeed in tests")
    }
}
