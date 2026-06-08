use std::path::PathBuf;
use std::time::Duration;

/// Brush runtime for one-off and persistent shell execution.
pub struct BrushRuntime {
    shell: brush_core::Shell<brush_core::extensions::DefaultShellExtensions>,
}

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
    pub async fn new() -> Result<Self, BrushError> {
        use brush_builtins::ShellBuilderExt as _;
        let shell = brush_core::Shell::builder()
            .default_builtins(brush_builtins::BuiltinSet::BashMode)
            .build()
            .await
            .map_err(|e| BrushError::Spawn(e.to_string()))?;
        Ok(Self { shell })
    }

    /// Execute a one-off command.
    pub async fn exec(&mut self, cmd: &str) -> Result<ExecResult, BrushError> {
        self.exec_inner(cmd).await
    }

    /// Execute a command in the persistent session.
    pub async fn exec_persistent(&mut self, cmd: &str) -> Result<ExecResult, BrushError> {
        // Phase A: same as exec. Differentiated in Phase B.
        self.exec_inner(cmd).await
    }

    #[allow(clippy::incompatible_msrv)]
    async fn exec_inner(&mut self, cmd: &str) -> Result<ExecResult, BrushError> {
        let start = std::time::Instant::now();

        // Create pipes for stdout/stderr capture.
        let (mut stdout_r, stdout_w) =
            std::io::pipe().map_err(|e| BrushError::Exec(e.to_string()))?;
        let (mut stderr_r, stderr_w) =
            std::io::pipe().map_err(|e| BrushError::Exec(e.to_string()))?;

        let mut params = self.shell.default_exec_params();
        params.set_fd(
            brush_core::openfiles::OpenFiles::STDOUT_FD,
            brush_core::openfiles::OpenFile::from(stdout_w),
        );
        params.set_fd(
            brush_core::openfiles::OpenFiles::STDERR_FD,
            brush_core::openfiles::OpenFile::from(stderr_w),
        );

        let result = self
            .shell
            .run_string(cmd, &brush_core::SourceInfo::default(), &params)
            .await
            .map_err(|e| BrushError::Exec(e.to_string()))?;

        let exit_code = u8::from(result.exit_code).into();

        // Drop params (and the pipe writers) before reading so the read ends get EOF.
        drop(params);

        let mut stdout = String::new();
        std::io::Read::read_to_string(&mut stdout_r, &mut stdout)
            .map_err(|e| BrushError::Exec(e.to_string()))?;

        let mut stderr = String::new();
        std::io::Read::read_to_string(&mut stderr_r, &mut stderr)
            .map_err(|e| BrushError::Exec(e.to_string()))?;

        let cwd = self.shell.working_dir().to_path_buf();

        Ok(ExecResult {
            exit_code,
            stdout,
            stderr,
            duration: start.elapsed(),
            cwd,
            osc_633: None,
        })
    }
}

/// OSC 633 parser (Phase A stub).
pub struct Osc633Parser;

impl Osc633Parser {
    /// Parse OSC 633 sequences from raw terminal output.
    /// Phase A: returns the input unchanged with no parsed OSC 633.
    pub fn parse(input: &str) -> (String, Option<Osc633>) {
        (input.to_string(), None)
    }
}
