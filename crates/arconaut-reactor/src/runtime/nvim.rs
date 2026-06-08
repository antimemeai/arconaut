use std::process::Child;

/// Neovim runtime via rmp-rpc.
pub struct NvimRuntime {
    _process: Child,
}

/// Errors from the nvim runtime.
#[derive(Debug, Clone, PartialEq)]
pub enum NvimError {
    Spawn(String),
    Rpc(String),
    Nvim { error_type: String, message: String },
    Timeout,
    NotRunning,
}

impl std::fmt::Display for NvimError {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            NvimError::Spawn(e) => write!(f, "spawn error: {e}"),
            NvimError::Rpc(e) => write!(f, "rpc error: {e}"),
            NvimError::Nvim { error_type, message } => {
                write!(f, "nvim error ({error_type}): {message}")
            }
            NvimError::Timeout => write!(f, "nvim operation timed out"),
            NvimError::NotRunning => write!(f, "nvim process not running"),
        }
    }
}

impl std::error::Error for NvimError {}

impl NvimRuntime {
    /// Spawn `nvim --embed`. Timeout: 200ms for startup handshake.
    pub async fn spawn() -> Result<Self, NvimError> {
        // TODO(Phase A): implement actual msgpack-rpc spawn.
        Err(NvimError::NotRunning)
    }

    /// Health check: ping nvim, return bool.
    pub async fn is_alive(&self) -> bool {
        // TODO(Phase A): implement health check.
        false
    }

    /// Execute an ex command.
    pub async fn command(&mut self, _cmd: &str) -> Result<String, NvimError> {
        Err(NvimError::NotRunning)
    }

    /// Evaluate a VimL expression.
    pub async fn eval(&mut self, _expr: &str) -> Result<rmpv::Value, NvimError> {
        Err(NvimError::NotRunning)
    }

    /// Get buffer lines.
    pub async fn buf_get_lines(
        &mut self,
        _buf: i64,
        _start: i64,
        _end: i64,
        _strict_indexing: bool,
    ) -> Result<Vec<String>, NvimError> {
        Err(NvimError::NotRunning)
    }

    /// Set buffer lines.
    pub async fn buf_set_lines(
        &mut self,
        _buf: i64,
        _start: i64,
        _end: i64,
        _strict_indexing: bool,
        _lines: Vec<String>,
    ) -> Result<(), NvimError> {
        Err(NvimError::NotRunning)
    }

    /// Graceful shutdown.
    pub async fn shutdown(self) -> Result<(), NvimError> {
        Ok(())
    }
}
