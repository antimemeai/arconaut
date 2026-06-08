use arconaut_reactor::tool::{RemoteExecuteTool, RemoteReadTool, RemoteSearchTool, RemoteWriteTool};
use arconaut_core::tool::Tool;

/// Check if SSH to localhost is available for testing.
async fn ssh_localhost_available() -> bool {
    let output = tokio::process::Command::new("ssh")
        .args(["-o", "BatchMode=yes", "-o", "ConnectTimeout=1", "localhost", "echo", "ok"])
        .output()
        .await;
    match output {
        Ok(o) => o.status.success() && String::from_utf8_lossy(&o.stdout).trim() == "ok",
        Err(_) => false,
    }
}

#[tokio::test]
async fn remote_execute_localhost_hostname() {
    if !ssh_localhost_available().await {
        eprintln!("Skipping: SSH to localhost not available");
        return;
    }

    let tool = RemoteExecuteTool::new();

    let result = tool
        .call(serde_json::json!({
            "host": "localhost",
            "command": "hostname"
        }))
        .await;
    assert!(result.is_ok(), "RemoteExecute should succeed: {:?}", result.err());

    match result.unwrap() {
        arconaut_core::tool::ToolResult::Success { output } => {
            let text = output[0].as_text().unwrap_or("");
            assert!(text.contains("exit_code: 0"), "hostname should exit 0");
            // Should contain the hostname (non-empty)
            assert!(!text.lines().nth(2).unwrap_or("").is_empty(), "should have hostname output");
        }
        arconaut_core::tool::ToolResult::Error { message, .. } => {
            panic!("RemoteExecute returned error: {message}");
        }
    }
}

#[tokio::test]
async fn remote_read_localhost_etc_hostname() {
    if !ssh_localhost_available().await {
        eprintln!("Skipping: SSH to localhost not available");
        return;
    }

    let tool = RemoteReadTool::new();

    let result = tool
        .call(serde_json::json!({
            "host": "localhost",
            "path": "/etc/hostname",
            "offset": 0,
            "limit": 10
        }))
        .await;
    assert!(result.is_ok(), "RemoteRead should succeed: {:?}", result.err());

    match result.unwrap() {
        arconaut_core::tool::ToolResult::Success { output } => {
            let text = output[0].as_text().unwrap_or("");
            // /etc/hostname usually contains a single hostname line
            assert!(!text.is_empty(), "should read non-empty content");
        }
        arconaut_core::tool::ToolResult::Error { message, .. } => {
            panic!("RemoteRead returned error: {message}");
        }
    }
}

#[tokio::test]
async fn remote_write_and_read_roundtrip() {
    if !ssh_localhost_available().await {
        eprintln!("Skipping: SSH to localhost not available");
        return;
    }

    let write_tool = RemoteWriteTool::new();
    let read_tool = RemoteReadTool::new();

    let tmp = "/tmp/arconaut_remote_test.txt";
    let content = "hello from remote write";

    // Write
    let write_result = write_tool
        .call(serde_json::json!({
            "host": "localhost",
            "path": tmp,
            "content": content
        }))
        .await;
    assert!(write_result.is_ok(), "RemoteWrite should succeed: {:?}", write_result.err());

    // Read back
    let read_result = read_tool
        .call(serde_json::json!({
            "host": "localhost",
            "path": tmp,
            "offset": 0,
            "limit": 10
        }))
        .await;
    assert!(read_result.is_ok(), "RemoteRead should succeed after write");

    match read_result.unwrap() {
        arconaut_core::tool::ToolResult::Success { output } => {
            let text = output[0].as_text().unwrap_or("");
            assert!(text.contains("hello from remote write"), "should contain written content");
        }
        arconaut_core::tool::ToolResult::Error { message, .. } => {
            panic!("RemoteRead returned error: {message}");
        }
    }
}

#[tokio::test]
async fn remote_search_localhost() {
    if !ssh_localhost_available().await {
        eprintln!("Skipping: SSH to localhost not available");
        return;
    }

    let tool = RemoteSearchTool::new();

    let result = tool
        .call(serde_json::json!({
            "host": "localhost",
            "pattern": "fn main",
            "glob": "*.rs"
        }))
        .await;
    assert!(result.is_ok(), "RemoteSearch should succeed: {:?}", result.err());

    match result.unwrap() {
        arconaut_core::tool::ToolResult::Success { output } => {
            let _text = output[0].as_text().unwrap_or("");
            // May or may not find results depending on cwd — just check it doesn't error
            // The result should be parseable (ripgrep output format)
        }
        arconaut_core::tool::ToolResult::Error { message, .. } => {
            panic!("RemoteSearch returned error: {message}");
        }
    }
}
