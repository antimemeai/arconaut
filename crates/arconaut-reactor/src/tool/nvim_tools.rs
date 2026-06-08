use crate::runtime::nvim::NvimRuntime;
use arconaut_core::tool::{Tool, ToolError, ToolResult};
use arconaut_core::ContentPart;
use async_trait::async_trait;
use serde_json::Value;
use std::sync::Arc;
use tokio::sync::Mutex;

/// Read a file through the headless neovim runtime.
pub struct ReadNvTool {
    nvim: Arc<Mutex<NvimRuntime>>,
    params: Value,
}

impl ReadNvTool {
    pub fn new(nvim: Arc<Mutex<NvimRuntime>>) -> Self {
        Self {
            nvim,
            params: serde_json::json!({
                "type": "object",
                "properties": {
                    "path": { "type": "string", "description": "Relative or absolute file path" },
                    "offset": { "type": "integer", "description": "Line offset (0-based, inclusive)", "default": 0 },
                    "limit": { "type": "integer", "description": "Max lines to read", "default": 200 }
                },
                "required": ["path"]
            }),
        }
    }
}

#[async_trait]
impl Tool for ReadNvTool {
    fn name(&self) -> &str {
        "ReadNV"
    }

    fn description(&self) -> &str {
        "Read a file through the headless neovim runtime."
    }

    fn parameters(&self) -> &Value {
        &self.params
    }

    async fn call(&self, args: Value) -> Result<ToolResult, ToolError> {
        let path = args["path"].as_str().ok_or_else(|| ToolError {
            message: "missing 'path' argument".to_string(),
            brief: "invalid arguments".to_string(),
        })?;
        let offset = args["offset"].as_i64().unwrap_or(0);
        let limit = args["limit"].as_i64().unwrap_or(200);

        let mut nvim = self.nvim.lock().await;

        // Open the file in nvim
        let escaped = shell_escape(path);
        nvim.command(&format!("edit {escaped}"))
            .await
            .map_err(|e| ToolError {
                message: e.to_string(),
                brief: "nvim command failed".to_string(),
            })?;

        let buf = nvim.get_current_buf().await.map_err(|e| ToolError {
            message: e.to_string(),
            brief: "nvim get_current_buf failed".to_string(),
        })?;

        let lines = nvim
            .buf_get_lines(buf, offset, offset + limit, false)
            .await
            .map_err(|e| ToolError {
                message: e.to_string(),
                brief: "nvim buf_get_lines failed".to_string(),
            })?;

        let total_lines = lines.len();
        let content = lines.join("\n");
        let output = format!(
            "<file path=\"{}\" lines=\"{}-{}\" total=\"{}\">\n{}\n</file>",
            path,
            offset,
            offset + total_lines as i64 - 1,
            total_lines,
            content
        );

        Ok(ToolResult::success(vec![ContentPart::Text {
            text: output,
        }]))
    }
}

/// Write a file through the headless neovim runtime.
pub struct WriteNvTool {
    nvim: Arc<Mutex<NvimRuntime>>,
    params: Value,
}

impl WriteNvTool {
    pub fn new(nvim: Arc<Mutex<NvimRuntime>>) -> Self {
        Self {
            nvim,
            params: serde_json::json!({
                "type": "object",
                "properties": {
                    "path": { "type": "string" },
                    "content": { "type": "string", "description": "Full file contents" }
                },
                "required": ["path", "content"]
            }),
        }
    }
}

#[async_trait]
impl Tool for WriteNvTool {
    fn name(&self) -> &str {
        "WriteNV"
    }

    fn description(&self) -> &str {
        "Write a file through the headless neovim runtime."
    }

    fn parameters(&self) -> &Value {
        &self.params
    }

    async fn call(&self, args: Value) -> Result<ToolResult, ToolError> {
        let path = args["path"].as_str().ok_or_else(|| ToolError {
            message: "missing 'path' argument".to_string(),
            brief: "invalid arguments".to_string(),
        })?;
        let content = args["content"].as_str().ok_or_else(|| ToolError {
            message: "missing 'content' argument".to_string(),
            brief: "invalid arguments".to_string(),
        })?;

        let mut nvim = self.nvim.lock().await;

        let escaped = shell_escape(path);
        nvim.command(&format!("edit {escaped}"))
            .await
            .map_err(|e| ToolError {
                message: e.to_string(),
                brief: "nvim edit failed".to_string(),
            })?;

        let buf = nvim.get_current_buf().await.map_err(|e| ToolError {
            message: e.to_string(),
            brief: "nvim get_current_buf failed".to_string(),
        })?;

        let lines: Vec<String> = content.lines().map(|s| s.to_string()).collect();
        let line_count = lines.len();

        nvim.buf_set_lines(buf, 0, -1, false, lines)
            .await
            .map_err(|e| ToolError {
                message: e.to_string(),
                brief: "nvim buf_set_lines failed".to_string(),
            })?;

        nvim.command("write")
            .await
            .map_err(|e| ToolError {
                message: e.to_string(),
                brief: "nvim write failed".to_string(),
            })?;

        Ok(ToolResult::success(vec![ContentPart::Text {
            text: format!("Wrote {line_count} lines to {path}"),
        }]))
    }
}

/// Edit a file using tree-sitter queries or extmarks.
pub struct EditNvTool {
    nvim: Arc<Mutex<NvimRuntime>>,
    params: Value,
}

impl EditNvTool {
    pub fn new(nvim: Arc<Mutex<NvimRuntime>>) -> Self {
        Self {
            nvim,
            params: serde_json::json!({
                "type": "object",
                "properties": {
                    "path": { "type": "string" },
                    "mode": { "type": "string", "enum": ["query", "extmark"] },
                    "selector": { "type": "string", "description": "tree-sitter query or extmark id" },
                    "replacement": { "type": "string" }
                },
                "required": ["path", "mode", "selector", "replacement"]
            }),
        }
    }
}

#[async_trait]
impl Tool for EditNvTool {
    fn name(&self) -> &str {
        "EditNV"
    }

    fn description(&self) -> &str {
        "Edit a file using tree-sitter queries or extmarks."
    }

    fn parameters(&self) -> &Value {
        &self.params
    }

    async fn call(&self, args: Value) -> Result<ToolResult, ToolError> {
        let path = args["path"].as_str().ok_or_else(|| ToolError {
            message: "missing 'path' argument".to_string(),
            brief: "invalid arguments".to_string(),
        })?;
        let mode = args["mode"].as_str().ok_or_else(|| ToolError {
            message: "missing 'mode' argument".to_string(),
            brief: "invalid arguments".to_string(),
        })?;
        let selector = args["selector"].as_str().ok_or_else(|| ToolError {
            message: "missing 'selector' argument".to_string(),
            brief: "invalid arguments".to_string(),
        })?;
        let replacement = args["replacement"].as_str().ok_or_else(|| ToolError {
            message: "missing 'replacement' argument".to_string(),
            brief: "invalid arguments".to_string(),
        })?;

        let mut nvim = self.nvim.lock().await;

        match mode {
            "query" => {
                let escaped = shell_escape(path);
                nvim.command(&format!("edit {escaped}"))
                    .await
                    .map_err(|e| ToolError {
                        message: e.to_string(),
                        brief: "nvim edit failed".to_string(),
                    })?;

                // Call Lua function exposed by arconaut.lua
                let result = nvim
                    .lua_call(
                        "ArconautEditByQuery",
                        vec![
                            rmpv::Value::String(selector.into()),
                            rmpv::Value::String(replacement.into()),
                        ],
                    )
                    .await
                    .map_err(|e| ToolError {
                        message: e.to_string(),
                        brief: "nvim lua call failed".to_string(),
                    })?;

                nvim.command("write")
                    .await
                    .map_err(|e| ToolError {
                        message: e.to_string(),
                        brief: "nvim write failed".to_string(),
                    })?;

                Ok(ToolResult::success(vec![ContentPart::Text {
                    text: format!("Edit result: {result:?}"),
                }]))
            }
            "extmark" => Err(ToolError {
                message: "extmark mode not yet implemented".to_string(),
                brief: "not implemented".to_string(),
            }),
            _ => Err(ToolError {
                message: format!("unknown mode: {mode}"),
                brief: "invalid mode".to_string(),
            }),
        }
    }
}

/// Query AST nodes using tree-sitter.
pub struct QueryAstTool {
    nvim: Arc<Mutex<NvimRuntime>>,
    params: Value,
}

impl QueryAstTool {
    pub fn new(nvim: Arc<Mutex<NvimRuntime>>) -> Self {
        Self {
            nvim,
            params: serde_json::json!({
                "type": "object",
                "properties": {
                    "path": { "type": "string" },
                    "query": { "type": "string", "description": "tree-sitter query string" }
                },
                "required": ["path", "query"]
            }),
        }
    }
}

#[async_trait]
impl Tool for QueryAstTool {
    fn name(&self) -> &str {
        "QueryAST"
    }

    fn description(&self) -> &str {
        "Query AST nodes using tree-sitter."
    }

    fn parameters(&self) -> &Value {
        &self.params
    }

    async fn call(&self, args: Value) -> Result<ToolResult, ToolError> {
        let path = args["path"].as_str().ok_or_else(|| ToolError {
            message: "missing 'path' argument".to_string(),
            brief: "invalid arguments".to_string(),
        })?;
        let query = args["query"].as_str().ok_or_else(|| ToolError {
            message: "missing 'query' argument".to_string(),
            brief: "invalid arguments".to_string(),
        })?;

        let mut nvim = self.nvim.lock().await;

        let escaped = shell_escape(path);
        nvim.command(&format!("edit {escaped}"))
            .await
            .map_err(|e| ToolError {
                message: e.to_string(),
                brief: "nvim edit failed".to_string(),
            })?;

        let result = nvim
            .lua_call("ArconautQueryAST", vec![rmpv::Value::String(query.into())])
            .await
            .map_err(|e| ToolError {
                message: e.to_string(),
                brief: "nvim lua call failed".to_string(),
            })?;

        // Convert rmpv::Value to JSON string
        let json_str = rmpv_to_json_string(&result).map_err(|e| ToolError {
            message: e.to_string(),
            brief: "json conversion failed".to_string(),
        })?;

        Ok(ToolResult::success(vec![ContentPart::Text {
            text: json_str,
        }]))
    }
}

/// Escape a path for safe use in nvim ex commands.
/// Nvim ex commands use backslash escaping (not shell quoting).
fn shell_escape(s: &str) -> String {
    s.replace('\\', "\\\\").replace(' ', "\\ ")
}

/// Convert an rmpv::Value to a JSON string.
fn rmpv_to_json_string(value: &rmpv::Value) -> Result<String, serde_json::Error> {
    let json_value = rmpv_to_json(value);
    serde_json::to_string_pretty(&json_value)
}

/// Convert rmpv::Value to serde_json::Value.
fn rmpv_to_json(value: &rmpv::Value) -> serde_json::Value {
    use rmpv::Value as R;
    match value {
        R::Nil => serde_json::Value::Null,
        R::Boolean(b) => serde_json::Value::Bool(*b),
        R::Integer(i) => {
            if let Some(v) = i.as_i64() {
                serde_json::Value::Number(v.into())
            } else if let Some(v) = i.as_u64() {
                serde_json::Value::Number(v.into())
            } else {
                serde_json::Value::Null
            }
        }
        R::F64(f) => serde_json::Value::Number(
            serde_json::Number::from_f64(*f).unwrap_or(0.into()),
        ),
        R::String(s) => serde_json::Value::String(s.to_string()),
        R::Binary(b) => serde_json::Value::Array(
            b.iter().map(|&v| serde_json::Value::Number(v.into())).collect(),
        ),
        R::Array(a) => serde_json::Value::Array(a.iter().map(rmpv_to_json).collect()),
        R::Map(m) => {
            let mut obj = serde_json::Map::new();
            for (k, v) in m {
                let key = match k {
                    R::String(s) => s.to_string(),
                    other => other.to_string(),
                };
                obj.insert(key, rmpv_to_json(v));
            }
            serde_json::Value::Object(obj)
        }
        R::Ext(_, _) => serde_json::Value::Null,
        R::F32(f) => serde_json::Value::Number(
            serde_json::Number::from_f64(*f as f64).unwrap_or(0.into()),
        ),
    }
}
