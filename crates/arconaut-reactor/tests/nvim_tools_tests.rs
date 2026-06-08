use arconaut_reactor::runtime::nvim::NvimRuntime;
use arconaut_reactor::tool::{EditNvTool, QueryAstTool, ReadNvTool, WriteNvTool};
use arconaut_core::tool::Tool;
use std::sync::Arc;
use tokio::sync::Mutex;

async fn setup_nvim() -> Arc<Mutex<NvimRuntime>> {
    Arc::new(Mutex::new(NvimRuntime::spawn().await.expect("nvim spawn should work")))
}

/// Check if a tree-sitter parser is available in nvim.
async fn ts_parser_available(nvim: &Arc<Mutex<NvimRuntime>>, lang: &str) -> bool {
    let mut guard = nvim.lock().await;
    let result = guard
        .eval(&format!("luaeval('vim.treesitter.get_parser(0, \"{}\") ~= nil')", lang))
        .await;
    match result {
        Ok(rmpv::Value::Boolean(b)) => b,
        Ok(rmpv::Value::Integer(i)) => i.as_i64().unwrap_or(0) != 0,
        _ => false,
    }
}

#[tokio::test]
async fn readnv_reads_existing_file() {
    let nvim = setup_nvim().await;
    let tool = ReadNvTool::new(nvim.clone());

    let result = tool.call(serde_json::json!({"path": "Cargo.toml"})).await;
    assert!(result.is_ok(), "ReadNV should succeed: {:?}", result.err());

    let output = result.unwrap();
    match output {
        arconaut_core::tool::ToolResult::Success { output } => {
            let text = output[0].as_text().unwrap_or("");
            assert!(text.contains("<file path=\"Cargo.toml\""));
            assert!(text.contains("[package]"), "expected [package] in: {text}");
        }
        arconaut_core::tool::ToolResult::Error { message, .. } => {
            panic!("ReadNV returned error: {message}");
        }
    }
}

#[tokio::test]
async fn writenv_roundtrip() {
    let nvim = setup_nvim().await;
    let write_tool = WriteNvTool::new(nvim.clone());
    let read_tool = ReadNvTool::new(nvim.clone());

    let tmp = "/tmp/arconaut_test_roundtrip.txt";
    let content = "hello from WriteNV\nline two\nline three";

    // Write
    let write_result = write_tool
        .call(serde_json::json!({"path": tmp, "content": content}))
        .await;
    assert!(write_result.is_ok(), "WriteNV should succeed");

    // Read back
    let read_result = read_tool
        .call(serde_json::json!({"path": tmp, "offset": 0, "limit": 10}))
        .await;
    assert!(read_result.is_ok(), "ReadNV should succeed after write");

    match read_result.unwrap() {
        arconaut_core::tool::ToolResult::Success { output } => {
            let text = output[0].as_text().unwrap_or("");
            assert!(text.contains("hello from WriteNV"));
            assert!(text.contains("line two"));
        }
        arconaut_core::tool::ToolResult::Error { message, .. } => {
            panic!("ReadNV returned error: {message}");
        }
    }
}

#[tokio::test]
async fn queryast_finds_functions() {
    let nvim = setup_nvim().await;

    if !ts_parser_available(&nvim, "rust").await {
        eprintln!("Skipping: Rust tree-sitter parser not available");
        return;
    }

    let tool = QueryAstTool::new(nvim.clone());

    // Query this very test file for function_item nodes
    let result = tool
        .call(serde_json::json!({
            "path": "tests/nvim_tools_tests.rs",
            "query": "(function_item) @fn"
        }))
        .await;
    assert!(result.is_ok(), "QueryAST should succeed");

    match result.unwrap() {
        arconaut_core::tool::ToolResult::Success { output } => {
            let text = output[0].as_text().unwrap_or("");
            // Should be valid JSON with at least one function
            let parsed: serde_json::Value = serde_json::from_str(text).expect("should be valid JSON");
            let arr = parsed.as_array().expect("should be array");
            assert!(!arr.is_empty(), "should find at least one function");
        }
        arconaut_core::tool::ToolResult::Error { message, .. } => {
            panic!("QueryAST returned error: {message}");
        }
    }
}

#[tokio::test]
async fn editnv_query_mode_replaces_text() {
    let nvim = setup_nvim().await;

    if !ts_parser_available(&nvim, "rust").await {
        eprintln!("Skipping: Rust tree-sitter parser not available");
        return;
    }

    let write_tool = WriteNvTool::new(nvim.clone());
    let edit_tool = EditNvTool::new(nvim.clone());
    let read_tool = ReadNvTool::new(nvim.clone());

    let tmp = "/tmp/arconaut_test_edit.rs";
    let content = "fn old_name() {}\n";

    // Write test file
    write_tool
        .call(serde_json::json!({"path": tmp, "content": content}))
        .await
        .expect("write should succeed");

    // Edit by query
    let edit_result = edit_tool
        .call(serde_json::json!({
            "path": tmp,
            "mode": "query",
            "selector": "(function_item name: (identifier) @name)",
            "replacement": "new_name"
        }))
        .await;
    assert!(edit_result.is_ok(), "EditNV should succeed");

    // Read back and verify
    let read_result = read_tool
        .call(serde_json::json!({"path": tmp}))
        .await;
    match read_result.unwrap() {
        arconaut_core::tool::ToolResult::Success { output } => {
            let text = output[0].as_text().unwrap_or("");
            assert!(
                text.contains("new_name"),
                "should contain new_name, got: {text}"
            );
        }
        arconaut_core::tool::ToolResult::Error { message, .. } => {
            panic!("ReadNV returned error: {message}");
        }
    }
}
