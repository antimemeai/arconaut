use arconaut_core::{ContentPart, Tool, ToolError, ToolResult};
use async_trait::async_trait;
use base64::Engine;
use serde_json::Value;
use std::collections::hash_map::DefaultHasher;
use std::hash::{Hash, Hasher};

/// Collection of small utility tools for the LLM.
pub struct UtilsBin;

impl UtilsBin {
    pub fn tools() -> Vec<Box<dyn Tool>> {
        vec![
            Box::new(UuidTool::new()),
            Box::new(TimestampTool::new()),
            Box::new(HashTool::new()),
            Box::new(Base64Tool::new()),
            Box::new(JsonFormatTool::new()),
        ]
    }
}

// ---------------------------------------------------------------------------
// uuid
// ---------------------------------------------------------------------------

struct UuidTool {
    params: Value,
}

impl UuidTool {
    fn new() -> Self {
        Self {
            params: serde_json::json!({"type": "object", "properties": {}}),
        }
    }
}

#[async_trait]
impl Tool for UuidTool {
    fn name(&self) -> &str { "uuid" }
    fn description(&self) -> &str { "Generate a random UUID v4." }
    fn parameters(&self) -> &Value { &self.params }
    async fn call(&self, _args: Value) -> Result<ToolResult, ToolError> {
        let id = uuid::Uuid::new_v4().to_string();
        Ok(ToolResult::success(vec![ContentPart::text(id)]))
    }
}

// ---------------------------------------------------------------------------
// timestamp
// ---------------------------------------------------------------------------

struct TimestampTool {
    params: Value,
}

impl TimestampTool {
    fn new() -> Self {
        Self {
            params: serde_json::json!({"type": "object", "properties": {}}),
        }
    }
}

#[async_trait]
impl Tool for TimestampTool {
    fn name(&self) -> &str { "timestamp" }
    fn description(&self) -> &str { "Return the current UTC timestamp in ISO 8601 format." }
    fn parameters(&self) -> &Value { &self.params }
    async fn call(&self, _args: Value) -> Result<ToolResult, ToolError> {
        let ts = chrono::Utc::now().to_rfc3339();
        Ok(ToolResult::success(vec![ContentPart::text(ts)]))
    }
}

// ---------------------------------------------------------------------------
// hash
// ---------------------------------------------------------------------------

struct HashTool {
    params: Value,
}

impl HashTool {
    fn new() -> Self {
        Self {
            params: serde_json::json!({
                "type": "object",
                "properties": {
                    "input": { "type": "string", "description": "String to hash" }
                },
                "required": ["input"]
            }),
        }
    }
}

#[async_trait]
impl Tool for HashTool {
    fn name(&self) -> &str { "hash" }
    fn description(&self) -> &str { "Compute a 64-bit hash of the input string (FNV-style, consistent across calls)." }
    fn parameters(&self) -> &Value { &self.params }
    async fn call(&self, args: Value) -> Result<ToolResult, ToolError> {
        let input = args["input"].as_str().ok_or_else(|| ToolError {
            message: "missing 'input'".to_string(),
            brief: "bad args".to_string(),
        })?;
        let mut hasher = DefaultHasher::new();
        input.hash(&mut hasher);
        let hash = hasher.finish();
        Ok(ToolResult::success(vec![ContentPart::text(format!("{:x}", hash))]))
    }
}

// ---------------------------------------------------------------------------
// base64
// ---------------------------------------------------------------------------

struct Base64Tool {
    params: Value,
}

impl Base64Tool {
    fn new() -> Self {
        Self {
            params: serde_json::json!({
                "type": "object",
                "properties": {
                    "input": { "type": "string", "description": "String to encode or decode" },
                    "decode": { "type": "boolean", "description": "If true, decode instead of encode" }
                },
                "required": ["input"]
            }),
        }
    }
}

#[async_trait]
impl Tool for Base64Tool {
    fn name(&self) -> &str { "base64" }
    fn description(&self) -> &str { "Base64 encode or decode a string." }
    fn parameters(&self) -> &Value { &self.params }
    async fn call(&self, args: Value) -> Result<ToolResult, ToolError> {
        let input = args["input"].as_str().ok_or_else(|| ToolError {
            message: "missing 'input'".to_string(),
            brief: "bad args".to_string(),
        })?;
        let decode = args["decode"].as_bool().unwrap_or(false);
        let result = if decode {
            let engine = base64::engine::general_purpose::STANDARD;
            String::from_utf8(engine.decode(input).map_err(|e| ToolError {
                message: format!("decode error: {}", e),
                brief: "decode error".to_string(),
            })?).map_err(|e| ToolError {
                message: format!("invalid utf8: {}", e),
                brief: "decode error".to_string(),
            })?
        } else {
            base64::engine::general_purpose::STANDARD.encode(input)
        };
        Ok(ToolResult::success(vec![ContentPart::text(result)]))
    }
}

// ---------------------------------------------------------------------------
// json_format
// ---------------------------------------------------------------------------

struct JsonFormatTool {
    params: Value,
}

impl JsonFormatTool {
    fn new() -> Self {
        Self {
            params: serde_json::json!({
                "type": "object",
                "properties": {
                    "input": { "type": "string", "description": "JSON string to pretty-print" }
                },
                "required": ["input"]
            }),
        }
    }
}

#[async_trait]
impl Tool for JsonFormatTool {
    fn name(&self) -> &str { "json_format" }
    fn description(&self) -> &str { "Pretty-print a JSON string with 2-space indentation." }
    fn parameters(&self) -> &Value { &self.params }
    async fn call(&self, args: Value) -> Result<ToolResult, ToolError> {
        let input = args["input"].as_str().ok_or_else(|| ToolError {
            message: "missing 'input'".to_string(),
            brief: "bad args".to_string(),
        })?;
        let value: Value = serde_json::from_str(input).map_err(|e| ToolError {
            message: format!("invalid json: {}", e),
            brief: "parse error".to_string(),
        })?;
        let pretty = serde_json::to_string_pretty(&value).map_err(|e| ToolError {
            message: format!("serialize error: {}", e),
            brief: "serialize error".to_string(),
        })?;
        Ok(ToolResult::success(vec![ContentPart::text(pretty)]))
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[tokio::test]
    async fn uuid_valid() {
        let tool = UuidTool::new();
        let result = tool.call(serde_json::json!({})).await.unwrap();
        match result {
            ToolResult::Success { output } => {
                let text = output[0].as_text().unwrap();
                assert_eq!(text.len(), 36); // UUID v4 string length
                assert!(text.contains('-'));
            }
            _ => panic!("expected success"),
        }
    }

    #[tokio::test]
    async fn timestamp_iso() {
        let tool = TimestampTool::new();
        let result = tool.call(serde_json::json!({})).await.unwrap();
        match result {
            ToolResult::Success { output } => {
                let text = output[0].as_text().unwrap();
                assert!(text.contains('T'));
                assert!(text.contains('+') || text.contains('Z'));
            }
            _ => panic!("expected success"),
        }
    }

    #[tokio::test]
    async fn hash_consistent() {
        let tool = HashTool::new();
        let r1 = tool.call(serde_json::json!({"input": "hello"})).await.unwrap();
        let r2 = tool.call(serde_json::json!({"input": "hello"})).await.unwrap();
        assert_eq!(format!("{:?}", r1), format!("{:?}", r2));
    }

    #[tokio::test]
    async fn base64_roundtrip() {
        let tool = Base64Tool::new();
        let encoded = tool.call(serde_json::json!({"input": "hello world"})).await.unwrap();
        let enc_text = match encoded {
            ToolResult::Success { output } => output[0].as_text().unwrap().to_string(),
            _ => panic!(),
        };
        let decoded = tool.call(serde_json::json!({"input": enc_text, "decode": true})).await.unwrap();
        match decoded {
            ToolResult::Success { output } => {
                assert_eq!(output[0].as_text().unwrap(), "hello world");
            }
            _ => panic!(),
        }
    }

    #[tokio::test]
    async fn json_format() {
        let tool = JsonFormatTool::new();
        let result = tool.call(serde_json::json!({"input": "{\"a\":1}"})).await.unwrap();
        match result {
            ToolResult::Success { output } => {
                let text = output[0].as_text().unwrap();
                assert!(text.contains("\"a\": 1"));
            }
            _ => panic!(),
        }
    }
}
