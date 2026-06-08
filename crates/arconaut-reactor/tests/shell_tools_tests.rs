use arconaut_reactor::runtime::brush::BrushRuntime;
use arconaut_reactor::tool::{RTermReadTool, RTermWriteTool};
use arconaut_core::tool::Tool;
use std::sync::Arc;
use tokio::sync::Mutex;

async fn setup_brush() -> Arc<Mutex<BrushRuntime>> {
    Arc::new(Mutex::new(
        BrushRuntime::new().await.expect("brush new should work"),
    ))
}

#[tokio::test]
async fn rtermwrite_executes_command() {
    let brush = setup_brush().await;
    let tool = RTermWriteTool::new(brush);

    let result = tool
        .call(serde_json::json!({"command": "echo marker123"}))
        .await;
    assert!(result.is_ok(), "RTermWrite should succeed");

    match result.unwrap() {
        arconaut_core::tool::ToolResult::Success { output } => {
            let text = output[0].as_text().unwrap_or("");
            assert!(text.contains("marker123"), "output should contain marker123");
            assert!(text.contains("exit_code: 0"), "should have exit code 0");
        }
        arconaut_core::tool::ToolResult::Error { message, .. } => {
            panic!("RTermWrite returned error: {message}");
        }
    }
}

#[tokio::test]
async fn rtermwrite_nonzero_exit_code() {
    let brush = setup_brush().await;
    let tool = RTermWriteTool::new(brush);

    let result = tool
        .call(serde_json::json!({"command": "exit 42"}))
        .await;
    assert!(result.is_ok(), "RTermWrite should succeed even with nonzero exit");

    match result.unwrap() {
        arconaut_core::tool::ToolResult::Success { output } => {
            let text = output[0].as_text().unwrap_or("");
            assert!(text.contains("exit_code: 42"), "should have exit code 42");
        }
        arconaut_core::tool::ToolResult::Error { message, .. } => {
            panic!("RTermWrite returned error: {message}");
        }
    }
}

#[tokio::test]
async fn rtermread_returns_output() {
    let brush = setup_brush().await;
    let write_tool = RTermWriteTool::new(brush.clone());
    let read_tool = RTermReadTool::new(brush);

    // First write something to generate output
    let _ = write_tool
        .call(serde_json::json!({"command": "echo rterm_test_output"}))
        .await;

    // Then read
    let result = read_tool.call(serde_json::json!({"lines": 10})).await;
    assert!(result.is_ok(), "RTermRead should succeed");

    match result.unwrap() {
        arconaut_core::tool::ToolResult::Success { output } => {
            let text = output[0].as_text().unwrap_or("");
            // Phase B: returns raw stdout. Should contain something.
            assert!(!text.is_empty(), "RTermRead should return non-empty output");
        }
        arconaut_core::tool::ToolResult::Error { message, .. } => {
            panic!("RTermRead returned error: {message}");
        }
    }
}
