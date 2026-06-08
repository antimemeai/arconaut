use std::io::{self, Cursor};
use tokio::io::{AsyncReadExt, AsyncWriteExt};
use tokio::process::{Child, ChildStdin, ChildStdout, Command};
use tokio::sync::Mutex;
use tokio::time::{timeout, Duration};

/// Neovim runtime via msgpack-rpc.
pub struct NvimRuntime {
    process: Mutex<Child>,
    stdin: Mutex<ChildStdin>,
    stdout: Mutex<ChildStdout>,
    msgid: std::sync::atomic::AtomicU32,
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
        let mut child = Command::new("nvim")
            .arg("--embed")
            .arg("--headless")
            .stdin(std::process::Stdio::piped())
            .stdout(std::process::Stdio::piped())
            .stderr(std::process::Stdio::null())
            .spawn()
            .map_err(|e| NvimError::Spawn(e.to_string()))?;

        let stdin = child
            .stdin
            .take()
            .ok_or_else(|| NvimError::Spawn("failed to capture stdin".to_string()))?;
        let stdout = child
            .stdout
            .take()
            .ok_or_else(|| NvimError::Spawn("failed to capture stdout".to_string()))?;

        let runtime = Self {
            process: Mutex::new(child),
            stdin: Mutex::new(stdin),
            stdout: Mutex::new(stdout),
            msgid: std::sync::atomic::AtomicU32::new(1),
        };

        // Load arconaut.lua if it exists.
        let lua_path = std::path::PathBuf::from(env!("CARGO_MANIFEST_DIR")).join("arconaut.lua");
        if lua_path.exists() {
            let lua_str = std::fs::read_to_string(&lua_path).unwrap_or_default();
            let _ = runtime
                .call(
                    "nvim_exec_lua",
                    vec![rmpv::Value::String(lua_str.into()), rmpv::Value::Array(vec![])],
                )
                .await;
        }

        // Verify nvim is responsive within 2000ms.
        let ping = timeout(
            Duration::from_millis(2000),
            runtime.call("nvim_eval", vec![rmpv::Value::String("1".into())]),
        )
        .await;

        match ping {
            Ok(Ok(_)) => Ok(runtime),
            Ok(Err(e)) => Err(e),
            Err(_) => Err(NvimError::Timeout),
        }
    }

    /// Health check: return true if the child process is still running.
    pub async fn is_alive(&self) -> bool {
        let mut process = self.process.lock().await;
        matches!(process.try_wait(), Ok(None))
    }

    /// Execute an ex command. Returns command output string.
    pub async fn command(&mut self, cmd: &str) -> Result<String, NvimError> {
        // Prefer nvim_exec2 to capture output.
        let result = self
            .call(
                "nvim_exec2",
                vec![
                    rmpv::Value::String(cmd.into()),
                    rmpv::Value::Map(vec![(
                        rmpv::Value::String("output".into()),
                        rmpv::Value::Boolean(true),
                    )]),
                ],
            )
            .await;

        match result {
            Ok(rmpv::Value::Map(map)) => {
                for (k, v) in map {
                    if let rmpv::Value::String(ref s) = k {
                        if s.as_str() == Some("output") {
                            if let rmpv::Value::String(out) = v {
                                return Ok(out
                                    .as_str()
                                    .unwrap_or("")
                                    .trim_end()
                                    .to_string());
                            }
                        }
                    }
                }
                Ok(String::new())
            }
            Ok(rmpv::Value::String(s)) => {
                Ok(s.as_str().unwrap_or("").trim_end().to_string())
            }
            Ok(_) => Ok(String::new()),
            Err(NvimError::Nvim { .. }) => {
                // nvim_exec2 may not be available; fall back to nvim_command.
                self.call("nvim_command", vec![rmpv::Value::String(cmd.into())])
                    .await?;
                Ok(String::new())
            }
            Err(e) => Err(e),
        }
    }

    /// Evaluate a VimL expression.
    pub async fn eval(&mut self, expr: &str) -> Result<rmpv::Value, NvimError> {
        self.call("nvim_eval", vec![rmpv::Value::String(expr.into())])
            .await
    }

    /// Get buffer lines.
    pub async fn buf_get_lines(
        &mut self,
        buf: i64,
        start: i64,
        end: i64,
        strict_indexing: bool,
    ) -> Result<Vec<String>, NvimError> {
        let result = self
            .call(
                "nvim_buf_get_lines",
                vec![
                    rmpv::Value::Integer(buf.into()),
                    rmpv::Value::Integer(start.into()),
                    rmpv::Value::Integer(end.into()),
                    rmpv::Value::Boolean(strict_indexing),
                ],
            )
            .await?;

        match result {
            rmpv::Value::Array(arr) => arr
                .into_iter()
                .map(|v| match v {
                    rmpv::Value::String(s) => Ok(s.as_str().unwrap_or("").to_string()),
                    other => Ok(other.to_string()),
                })
                .collect(),
            _ => Err(NvimError::Rpc(
                "expected array of strings from buf_get_lines".to_string(),
            )),
        }
    }

    /// Set buffer lines.
    pub async fn buf_set_lines(
        &mut self,
        buf: i64,
        start: i64,
        end: i64,
        strict_indexing: bool,
        lines: Vec<String>,
    ) -> Result<(), NvimError> {
        let lines_value: Vec<rmpv::Value> =
            lines.into_iter().map(|s| rmpv::Value::String(s.into())).collect();
        self.call(
            "nvim_buf_set_lines",
            vec![
                rmpv::Value::Integer(buf.into()),
                rmpv::Value::Integer(start.into()),
                rmpv::Value::Integer(end.into()),
                rmpv::Value::Boolean(strict_indexing),
                rmpv::Value::Array(lines_value),
            ],
        )
        .await
        .map(|_| ())
    }

    /// Graceful shutdown: `nvim_command("qa!")`, wait for exit.
    pub async fn shutdown(mut self) -> Result<(), NvimError> {
        // Try to quit gracefully; ignore errors since process may already be gone.
        let _ = self.command("qa!").await;

        let mut process = self.process.into_inner();
        match timeout(Duration::from_secs(5), process.wait()).await {
            Ok(Ok(_status)) => Ok(()),
            Ok(Err(e)) => Err(NvimError::Spawn(e.to_string())),
            Err(_) => {
                let _ = process.start_kill();
                Err(NvimError::Timeout)
            }
        }
    }

    /// Get current buffer number.
    /// Nvim returns buffers as msgpack Ext type 0; decode the embedded msgpack integer.
    pub async fn get_current_buf(&mut self) -> Result<i64, NvimError> {
        let result = self.call("nvim_get_current_buf", vec![]).await?;
        match result {
            rmpv::Value::Integer(i) => Ok(i.as_i64().unwrap_or(0)),
            rmpv::Value::Ext(0, bytes) => {
                // Buffer handle: the Ext data is itself a msgpack-encoded integer.
                let mut cursor = std::io::Cursor::new(&bytes);
                match rmpv::decode::read_value(&mut cursor) {
                    Ok(rmpv::Value::Integer(i)) => Ok(i.as_i64().unwrap_or(0)),
                    _ => Ok(0),
                }
            }
            _ => Err(NvimError::Rpc(
                "expected integer or ext from nvim_get_current_buf".to_string(),
            )),
        }
    }

    /// Call a Lua function exposed by arconaut.lua.
    pub async fn lua_call(
        &mut self,
        fn_name: &str,
        args: Vec<rmpv::Value>,
    ) -> Result<rmpv::Value, NvimError> {
        // Use _G._arconaut because vim.g copies tables on access.
        let lua_code = format!(
            "return _G._arconaut and _G._arconaut['{fn_name}'] and _G._arconaut['{fn_name}'](...) or nil"
        );
        let result = self
            .call("nvim_exec_lua", vec![rmpv::Value::String(lua_code.into()), rmpv::Value::Array(args)])
            .await?;
        Ok(result)
    }

    /// Send a msgpack-rpc request and await the matching response.
    async fn call(&self, method: &str, params: Vec<rmpv::Value>) -> Result<rmpv::Value, NvimError> {
        if !self.is_alive().await {
            return Err(NvimError::NotRunning);
        }

        let msgid = self
            .msgid
            .fetch_add(1, std::sync::atomic::Ordering::SeqCst);

        let request = rmpv::Value::Array(vec![
            rmpv::Value::Integer(0.into()),
            rmpv::Value::Integer(msgid.into()),
            rmpv::Value::String(method.into()),
            rmpv::Value::Array(params),
        ]);

        let mut buf = Vec::new();
        rmpv::encode::write_value(&mut buf, &request)
            .map_err(|e| NvimError::Rpc(e.to_string()))?;

        let mut stdin = self.stdin.lock().await;
        stdin
            .write_all(&buf)
            .await
            .map_err(|e| NvimError::Rpc(e.to_string()))?;
        stdin
            .flush()
            .await
            .map_err(|e| NvimError::Rpc(e.to_string()))?;
        drop(stdin);

        let mut stdout = self.stdout.lock().await;
        loop {
            let response = read_value(&mut *stdout)
                .await
                .map_err(|e| NvimError::Rpc(e.to_string()))?;

            let arr = response.as_array().ok_or_else(|| {
                NvimError::Rpc("expected array response from nvim".to_string())
            })?;

            if arr.len() < 4 {
                // Could be a notification [2, event, args] — skip it.
                continue;
            }

            let resp_type = arr[0].as_u64().ok_or_else(|| {
                NvimError::Rpc("expected response type as uint".to_string())
            })?;

            if resp_type != 1 {
                // Not a response (could be request from server) — skip it.
                continue;
            }

            let resp_msgid = arr[1].as_u64().ok_or_else(|| {
                NvimError::Rpc("expected msgid as uint".to_string())
            })?;

            if resp_msgid != u64::from(msgid) {
                // Response for a different request — skip it.
                continue;
            }

            drop(stdout);

            if arr[2].is_nil() {
                return Ok(arr[3].clone());
            }

            let message = match &arr[2] {
                rmpv::Value::String(s) => s.as_str().unwrap_or("unknown error").to_string(),
                other => format!("{other:?}"),
            };

            return Err(NvimError::Nvim {
                error_type: "Error".to_string(),
                message,
            });
        }
    }
}

/// Read one complete msgpack value from an async reader.
async fn read_value<R>(reader: &mut R) -> io::Result<rmpv::Value>
where
    R: AsyncReadExt + Unpin,
{
    let mut buf = Vec::with_capacity(1024);
    loop {
        let mut cursor = Cursor::new(&buf);
        match rmpv::decode::read_value(&mut cursor) {
            Ok(v) => {
                let consumed = cursor.position() as usize;
                buf.drain(..consumed);
                return Ok(v);
            }
            Err(e) if e.kind() == io::ErrorKind::UnexpectedEof => {
                let mut chunk = [0u8; 4096];
                let n = reader.read(&mut chunk).await?;
                if n == 0 {
                    return Err(io::Error::new(
                        io::ErrorKind::UnexpectedEof,
                        "EOF while reading msgpack value",
                    ));
                }
                buf.extend_from_slice(&chunk[..n]);
            }
            Err(e) => {
                return Err(io::Error::new(
                    io::ErrorKind::InvalidData,
                    format!("msgpack decode error: {e:?}"),
                ));
            }
        }
    }
}
