use arconaut_core::{ContentPart, Tool, ToolError, ToolResult};
use async_trait::async_trait;
use regex::Regex;
use serde_json::Value;
use std::path::Path;
use std::time::Duration;
use tokio::process::Command;
use tokio::time::timeout;

// ---------------------------------------------------------------------------
// ReadTool
// ---------------------------------------------------------------------------

pub struct ReadTool {
    params: Value,
}

impl Default for ReadTool {
    fn default() -> Self {
        Self::new()
    }
}

impl ReadTool {
    pub fn new() -> Self {
        Self {
            params: serde_json::json!({
                "type": "object",
                "properties": {
                    "path": { "type": "string" },
                    "offset": { "type": "integer", "description": "1-based line offset" },
                    "limit": { "type": "integer", "description": "Maximum lines to read" }
                },
                "required": ["path"]
            }),
        }
    }
}

#[async_trait]
impl Tool for ReadTool {
    fn name(&self) -> &str {
        "read"
    }

    fn description(&self) -> &str {
        "Read the contents of a file. Supports optional line offset and limit."
    }

    fn parameters(&self) -> &Value {
        &self.params
    }

    async fn call(&self, args: Value) -> Result<ToolResult, ToolError> {
        let path = args["path"].as_str().ok_or_else(|| ToolError {
            message: "missing 'path' argument".to_string(),
            brief: "bad args".to_string(),
        })?;

        let contents = tokio::fs::read_to_string(path)
            .await
            .map_err(|e| ToolError {
                message: format!("failed to read '{}': {}", path, e),
                brief: "read failed".to_string(),
            })?;

        let lines: Vec<&str> = contents.lines().collect();
        let offset = args["offset"].as_u64().unwrap_or(1).saturating_sub(1) as usize;
        let limit = args["limit"].as_u64().unwrap_or(u64::MAX) as usize;

        let selected: Vec<&str> = lines.into_iter().skip(offset).take(limit).collect();

        Ok(ToolResult::success(vec![ContentPart::text(
            selected.join("\n"),
        )]))
    }
}

// ---------------------------------------------------------------------------
// WriteTool
// ---------------------------------------------------------------------------

pub struct WriteTool {
    params: Value,
}

impl Default for WriteTool {
    fn default() -> Self {
        Self::new()
    }
}

impl WriteTool {
    pub fn new() -> Self {
        Self {
            params: serde_json::json!({
                "type": "object",
                "properties": {
                    "path": { "type": "string" },
                    "content": { "type": "string" }
                },
                "required": ["path", "content"]
            }),
        }
    }
}

#[async_trait]
impl Tool for WriteTool {
    fn name(&self) -> &str {
        "write"
    }

    fn description(&self) -> &str {
        "Write content to a file. Creates parent directories if needed. Overwrites existing files."
    }

    fn parameters(&self) -> &Value {
        &self.params
    }

    async fn call(&self, args: Value) -> Result<ToolResult, ToolError> {
        let path = args["path"].as_str().ok_or_else(|| ToolError {
            message: "missing 'path' argument".to_string(),
            brief: "bad args".to_string(),
        })?;
        let content = args["content"].as_str().ok_or_else(|| ToolError {
            message: "missing 'content' argument".to_string(),
            brief: "bad args".to_string(),
        })?;

        if let Some(parent) = Path::new(path).parent() {
            tokio::fs::create_dir_all(parent)
                .await
                .map_err(|e| ToolError {
                    message: format!("failed to create parent dirs: {}", e),
                    brief: "write failed".to_string(),
                })?;
        }

        tokio::fs::write(path, content)
            .await
            .map_err(|e| ToolError {
                message: format!("failed to write '{}': {}", path, e),
                brief: "write failed".to_string(),
            })?;

        Ok(ToolResult::success(vec![ContentPart::text(format!(
            "Wrote {} bytes to {}",
            content.len(),
            path
        ))]))
    }
}

// ---------------------------------------------------------------------------
// EditTool
// ---------------------------------------------------------------------------

pub struct EditTool {
    params: Value,
}

impl Default for EditTool {
    fn default() -> Self {
        Self::new()
    }
}

impl EditTool {
    pub fn new() -> Self {
        Self {
            params: serde_json::json!({
                "type": "object",
                "properties": {
                    "path": { "type": "string" },
                    "old_string": { "type": "string" },
                    "new_string": { "type": "string" },
                    "replace_all": { "type": "boolean", "default": false }
                },
                "required": ["path", "old_string", "new_string"]
            }),
        }
    }
}

#[async_trait]
impl Tool for EditTool {
    fn name(&self) -> &str {
        "edit"
    }

    fn description(&self) -> &str {
        "Replace an exact string in a file with another string."
    }

    fn parameters(&self) -> &Value {
        &self.params
    }

    async fn call(&self, args: Value) -> Result<ToolResult, ToolError> {
        let path = args["path"].as_str().ok_or_else(|| ToolError {
            message: "missing 'path' argument".to_string(),
            brief: "bad args".to_string(),
        })?;
        let old = args["old_string"].as_str().ok_or_else(|| ToolError {
            message: "missing 'old_string' argument".to_string(),
            brief: "bad args".to_string(),
        })?;
        let new = args["new_string"].as_str().ok_or_else(|| ToolError {
            message: "missing 'new_string' argument".to_string(),
            brief: "bad args".to_string(),
        })?;
        let replace_all = args["replace_all"].as_bool().unwrap_or(false);

        let contents = tokio::fs::read_to_string(path)
            .await
            .map_err(|e| ToolError {
                message: format!("failed to read '{}': {}", path, e),
                brief: "read failed".to_string(),
            })?;

        let count = contents.matches(old).count();
        if count == 0 {
            return Err(ToolError {
                message: format!("old_string not found in '{}'", path),
                brief: "no match".to_string(),
            });
        }

        if !replace_all && count > 1 {
            return Err(ToolError {
                message: format!(
                    "old_string appears {} times in '{}'; use replace_all=true to replace all",
                    count, path
                ),
                brief: "ambiguous match".to_string(),
            });
        }

        let replaced = if replace_all {
            contents.replace(old, new)
        } else {
            contents.replacen(old, new, 1)
        };

        tokio::fs::write(path, replaced)
            .await
            .map_err(|e| ToolError {
                message: format!("failed to write '{}': {}", path, e),
                brief: "write failed".to_string(),
            })?;

        Ok(ToolResult::success(vec![ContentPart::text(format!(
            "Edited {} ({} replacement{})",
            path,
            count,
            if count == 1 { "" } else { "s" }
        ))]))
    }
}

// ---------------------------------------------------------------------------
// BashTool
// ---------------------------------------------------------------------------

pub struct BashTool {
    default_timeout: Duration,
    params: Value,
}

impl Default for BashTool {
    fn default() -> Self {
        Self::new()
    }
}

impl BashTool {
    pub fn new() -> Self {
        Self {
            default_timeout: Duration::from_secs(30),
            params: serde_json::json!({
                "type": "object",
                "properties": {
                    "command": { "type": "string" },
                    "timeout_ms": { "type": "integer", "description": "Timeout in milliseconds (default 30000)" }
                },
                "required": ["command"]
            }),
        }
    }

    pub fn with_timeout(mut self, duration: Duration) -> Self {
        self.default_timeout = duration;
        self
    }

    fn is_safe_command(command: &str) -> Result<(), ToolError> {
        let forbidden = ["&&", "||", ";", "|", "`", "$()", "\n", "\r"];
        for pat in &forbidden {
            if command.contains(pat) {
                return Err(ToolError {
                    message: format!(
                        "command contains forbidden sequence '{}'; run single commands only",
                        pat.escape_debug()
                    ),
                    brief: "unsafe command".to_string(),
                });
            }
        }
        Ok(())
    }
}

#[async_trait]
impl Tool for BashTool {
    fn name(&self) -> &str {
        "bash"
    }

    fn description(&self) -> &str {
        "Execute a single shell command. Chained commands (&&, ;, |) are rejected for safety."
    }

    fn parameters(&self) -> &Value {
        &self.params
    }

    async fn call(&self, args: Value) -> Result<ToolResult, ToolError> {
        let command = args["command"].as_str().ok_or_else(|| ToolError {
            message: "missing 'command' argument".to_string(),
            brief: "bad args".to_string(),
        })?;

        BashTool::is_safe_command(command)?;

        let timeout_ms = args["timeout_ms"].as_u64().unwrap_or(30_000);
        let duration = Duration::from_millis(timeout_ms);

        let output = match timeout(
            duration,
            Command::new("bash").arg("-c").arg(command).output(),
        )
        .await
        {
            Ok(Ok(output)) => output,
            Ok(Err(e)) => {
                return Err(ToolError {
                    message: format!("failed to spawn process: {}", e),
                    brief: "spawn failed".to_string(),
                });
            }
            Err(_) => {
                return Err(ToolError {
                    message: format!("command timed out after {}ms", timeout_ms),
                    brief: "timeout".to_string(),
                });
            }
        };

        let stdout = String::from_utf8_lossy(&output.stdout);
        let stderr = String::from_utf8_lossy(&output.stderr);
        let exit_code = output.status.code().unwrap_or(-1);

        let result_text = format!(
            "exit_code: {}\nstdout:\n{}\nstderr:\n{}",
            exit_code, stdout, stderr
        );

        if output.status.success() {
            Ok(ToolResult::success(vec![ContentPart::text(result_text)]))
        } else {
            Ok(ToolResult::error(
                result_text,
                format!("command exited with code {}", exit_code),
            ))
        }
    }
}

// ---------------------------------------------------------------------------
// GrepTool
// ---------------------------------------------------------------------------

pub struct GrepTool {
    params: Value,
}

impl Default for GrepTool {
    fn default() -> Self {
        Self::new()
    }
}

impl GrepTool {
    pub fn new() -> Self {
        Self {
            params: serde_json::json!({
                "type": "object",
                "properties": {
                    "pattern": { "type": "string" },
                    "path": { "type": "string", "description": "Directory to search in" },
                    "include_ignored": { "type": "boolean", "default": false }
                },
                "required": ["pattern", "path"]
            }),
        }
    }

    fn load_gitignore_patterns(root: &Path) -> Vec<String> {
        let gitignore = root.join(".gitignore");
        if let Ok(contents) = std::fs::read_to_string(&gitignore) {
            contents
                .lines()
                .map(|l| l.trim().to_string())
                .filter(|l| !l.is_empty() && !l.starts_with('#'))
                .collect()
        } else {
            Vec::new()
        }
    }

    fn is_ignored(path: &Path, patterns: &[String]) -> bool {
        let path_str = path.to_string_lossy();
        for pat in patterns {
            if pat.ends_with('/') && path_str.contains(&pat[..pat.len() - 1]) {
                return true;
            }
            if path_str.contains(pat) {
                return true;
            }
        }
        false
    }

    async fn walk_dir(
        dir: &Path,
        pattern: &Regex,
        include_ignored: bool,
        gitignore_patterns: &[String],
        results: &mut Vec<String>,
    ) -> Result<(), ToolError> {
        let mut entries = tokio::fs::read_dir(dir).await.map_err(|e| ToolError {
            message: format!("failed to read directory '{}': {}", dir.display(), e),
            brief: "read dir failed".to_string(),
        })?;

        while let Ok(Some(entry)) = entries.next_entry().await {
            let path = entry.path();

            if !include_ignored && Self::is_ignored(&path, gitignore_patterns) {
                continue;
            }

            let metadata = tokio::fs::metadata(&path).await;
            let is_dir = metadata.as_ref().map(|m| m.is_dir()).unwrap_or(false);
            let is_file = metadata.as_ref().map(|m| m.is_file()).unwrap_or(false);

            if is_dir {
                if path.file_name() == Some(std::ffi::OsStr::new(".git")) {
                    continue;
                }
                Box::pin(Self::walk_dir(
                    &path,
                    pattern,
                    include_ignored,
                    gitignore_patterns,
                    results,
                ))
                .await?;
            } else if is_file {
                if let Ok(contents) = tokio::fs::read_to_string(&path).await {
                    let mut file_results = Vec::new();
                    for (line_num, line) in contents.lines().enumerate() {
                        if pattern.is_match(line) {
                            file_results.push(format!(
                                "{}:{}:{}",
                                path.display(),
                                line_num + 1,
                                line
                            ));
                        }
                    }
                    results.extend(file_results);
                }
            }
        }

        Ok(())
    }
}

#[async_trait]
impl Tool for GrepTool {
    fn name(&self) -> &str {
        "grep"
    }

    fn description(&self) -> &str {
        "Search for a regex pattern in files within a directory. Respects .gitignore by default."
    }

    fn parameters(&self) -> &Value {
        &self.params
    }

    async fn call(&self, args: Value) -> Result<ToolResult, ToolError> {
        let pattern_str = args["pattern"].as_str().ok_or_else(|| ToolError {
            message: "missing 'pattern' argument".to_string(),
            brief: "bad args".to_string(),
        })?;
        let path = args["path"].as_str().ok_or_else(|| ToolError {
            message: "missing 'path' argument".to_string(),
            brief: "bad args".to_string(),
        })?;
        let include_ignored = args["include_ignored"].as_bool().unwrap_or(false);

        let pattern = Regex::new(pattern_str).map_err(|e| ToolError {
            message: format!("invalid regex '{}': {}", pattern_str, e),
            brief: "bad regex".to_string(),
        })?;

        let root = Path::new(path);
        let gitignore_patterns = if include_ignored {
            Vec::new()
        } else {
            Self::load_gitignore_patterns(root)
        };

        let mut results = Vec::new();
        Self::walk_dir(root, &pattern, include_ignored, &gitignore_patterns, &mut results).await?;

        if results.is_empty() {
            Ok(ToolResult::success(vec![ContentPart::text(
                "No matches found.",
            )]))
        } else {
            Ok(ToolResult::success(vec![ContentPart::text(
                results.join("\n"),
            )]))
        }
    }
}

#[cfg(test)]
mod tests {
    use super::*;
    use std::fs;
    use tempfile::TempDir;

    // -----------------------------------------------------------------------
    // ReadTool
    // -----------------------------------------------------------------------

    #[tokio::test]
    async fn read_tool_reads_file() {
        let dir = TempDir::new().unwrap();
        let file = dir.path().join("test.txt");
        fs::write(&file, "line1\nline2\nline3\n").unwrap();

        let tool = ReadTool::new();
        let result = tool
            .call(serde_json::json!({"path": file.to_str().unwrap()}))
            .await;
        assert!(result.is_ok());
        let output = match result.unwrap() {
            ToolResult::Success { output } => output,
            _ => panic!("expected success"),
        };
        assert_eq!(output[0].as_text().unwrap(), "line1\nline2\nline3");
    }

    #[tokio::test]
    async fn read_tool_missing_file() {
        let tool = ReadTool::new();
        let result = tool
            .call(serde_json::json!({"path": "/nonexistent/path"}))
            .await;
        assert!(result.is_err());
    }

    #[tokio::test]
    async fn read_tool_offset_and_limit() {
        let dir = TempDir::new().unwrap();
        let file = dir.path().join("test.txt");
        fs::write(&file, "a\nb\nc\nd\ne\n").unwrap();

        let tool = ReadTool::new();
        let result = tool
            .call(serde_json::json!({"path": file.to_str().unwrap(), "offset": 2, "limit": 2}))
            .await;
        assert!(result.is_ok());
        let output = match result.unwrap() {
            ToolResult::Success { output } => output,
            _ => panic!("expected success"),
        };
        assert_eq!(output[0].as_text().unwrap(), "b\nc");
    }

    // -----------------------------------------------------------------------
    // WriteTool
    // -----------------------------------------------------------------------

    #[tokio::test]
    async fn write_tool_writes_file() {
        let dir = TempDir::new().unwrap();
        let file = dir.path().join("out.txt");

        let tool = WriteTool::new();
        let result = tool
            .call(serde_json::json!({
                "path": file.to_str().unwrap(),
                "content": "hello world"
            }))
            .await;
        assert!(result.is_ok());
        assert_eq!(fs::read_to_string(&file).unwrap(), "hello world");
    }

    #[tokio::test]
    async fn write_tool_creates_parents() {
        let dir = TempDir::new().unwrap();
        let file = dir.path().join("a/b/c/out.txt");

        let tool = WriteTool::new();
        let result = tool
            .call(serde_json::json!({
                "path": file.to_str().unwrap(),
                "content": "nested"
            }))
            .await;
        assert!(result.is_ok());
        assert_eq!(fs::read_to_string(&file).unwrap(), "nested");
    }

    // -----------------------------------------------------------------------
    // EditTool
    // -----------------------------------------------------------------------

    #[tokio::test]
    async fn edit_tool_exact_replace() {
        let dir = TempDir::new().unwrap();
        let file = dir.path().join("edit.txt");
        fs::write(&file, "foo bar baz").unwrap();

        let tool = EditTool::new();
        let result = tool
            .call(serde_json::json!({
                "path": file.to_str().unwrap(),
                "old_string": "bar",
                "new_string": "qux"
            }))
            .await;
        assert!(result.is_ok());
        assert_eq!(fs::read_to_string(&file).unwrap(), "foo qux baz");
    }

    #[tokio::test]
    async fn edit_tool_no_match_error() {
        let dir = TempDir::new().unwrap();
        let file = dir.path().join("edit.txt");
        fs::write(&file, "foo bar baz").unwrap();

        let tool = EditTool::new();
        let result = tool
            .call(serde_json::json!({
                "path": file.to_str().unwrap(),
                "old_string": "nope",
                "new_string": "qux"
            }))
            .await;
        assert!(result.is_err());
    }

    #[tokio::test]
    async fn edit_tool_replace_all() {
        let dir = TempDir::new().unwrap();
        let file = dir.path().join("edit.txt");
        fs::write(&file, "a a a").unwrap();

        let tool = EditTool::new();
        let result = tool
            .call(serde_json::json!({
                "path": file.to_str().unwrap(),
                "old_string": "a",
                "new_string": "b",
                "replace_all": true
            }))
            .await;
        assert!(result.is_ok());
        assert_eq!(fs::read_to_string(&file).unwrap(), "b b b");
    }

    #[tokio::test]
    async fn edit_tool_ambiguous_match() {
        let dir = TempDir::new().unwrap();
        let file = dir.path().join("edit.txt");
        fs::write(&file, "a a a").unwrap();

        let tool = EditTool::new();
        let result = tool
            .call(serde_json::json!({
                "path": file.to_str().unwrap(),
                "old_string": "a",
                "new_string": "b"
            }))
            .await;
        assert!(result.is_err());
    }

    // -----------------------------------------------------------------------
    // BashTool
    // -----------------------------------------------------------------------

    #[tokio::test]
    async fn bash_tool_captures_output() {
        let tool = BashTool::new();
        let result = tool
            .call(serde_json::json!({"command": "echo hello"}))
            .await;
        assert!(result.is_ok());
        let output = match result.unwrap() {
            ToolResult::Success { output } => output[0].as_text().unwrap().to_string(),
            _ => panic!("expected success"),
        };
        assert!(output.contains("hello"));
        assert!(output.contains("exit_code: 0"));
    }

    #[tokio::test]
    async fn bash_tool_rejects_chained_commands() {
        let tool = BashTool::new();
        for cmd in ["echo a && echo b", "echo a; echo b", "echo a | cat"] {
            let result = tool.call(serde_json::json!({"command": cmd})).await;
            assert!(result.is_err(), "should reject: {}", cmd);
        }
    }

    #[tokio::test]
    async fn bash_tool_timeout() {
        let tool = BashTool::new().with_timeout(Duration::from_millis(100));
        let result = tool
            .call(serde_json::json!({
                "command": "sleep 5",
                "timeout_ms": 100
            }))
            .await;
        assert!(result.is_err());
        let err = result.unwrap_err();
        assert_eq!(err.brief, "timeout");
    }

    // -----------------------------------------------------------------------
    // GrepTool
    // -----------------------------------------------------------------------

    #[tokio::test]
    async fn grep_tool_finds_matches() {
        let dir = TempDir::new().unwrap();
        fs::write(dir.path().join("a.txt"), "hello world\nfoo bar\n").unwrap();
        fs::write(dir.path().join("b.txt"), "hello moon\n").unwrap();

        let tool = GrepTool::new();
        let result = tool
            .call(serde_json::json!({
                "pattern": "hello",
                "path": dir.path().to_str().unwrap()
            }))
            .await;
        assert!(result.is_ok());
        let output = match result.unwrap() {
            ToolResult::Success { output } => output[0].as_text().unwrap().to_string(),
            _ => panic!("expected success"),
        };
        assert!(output.contains("a.txt:1:hello world"));
        assert!(output.contains("b.txt:1:hello moon"));
    }

    #[tokio::test]
    async fn grep_tool_respects_gitignore() {
        let dir = TempDir::new().unwrap();
        fs::write(dir.path().join("a.txt"), "hello world\n").unwrap();
        fs::write(dir.path().join("ignored.txt"), "hello ignored\n").unwrap();
        fs::write(dir.path().join(".gitignore"), "ignored.txt\n").unwrap();

        let tool = GrepTool::new();
        let result = tool
            .call(serde_json::json!({
                "pattern": "hello",
                "path": dir.path().to_str().unwrap()
            }))
            .await;
        assert!(result.is_ok());
        let output = match result.unwrap() {
            ToolResult::Success { output } => output[0].as_text().unwrap().to_string(),
            _ => panic!("expected success"),
        };
        assert!(output.contains("a.txt"));
        assert!(!output.contains("ignored.txt"));
    }
}
