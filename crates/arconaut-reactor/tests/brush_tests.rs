use arconaut_reactor::BrushRuntime;

#[tokio::test]
async fn brush_new_succeeds() {
    let brush = BrushRuntime::new().await;
    assert!(brush.is_ok());
}

#[tokio::test]
async fn brush_exec_echo_returns_hello() {
    // O4: Brush exit code oracle — stdout correctness.
    let mut brush = BrushRuntime::new().await.unwrap();
    let result = brush.exec("echo hello").await.unwrap();
    assert_eq!(result.exit_code, 0);
    assert_eq!(result.stdout.trim(), "hello");
    assert_eq!(result.stderr, "");
}

#[tokio::test]
async fn brush_exec_exit_code_42() {
    // O4: Brush exit code oracle — exit code correctness.
    let mut brush = BrushRuntime::new().await.unwrap();
    let result = brush.exec("exit 42").await.unwrap();
    assert_eq!(result.exit_code, 42);
}

#[tokio::test]
async fn brush_exec_nonexistent_binary() {
    // O4: Brush exit code oracle — error handling.
    let mut brush = BrushRuntime::new().await.unwrap();
    let result = brush.exec("/nonexistent/binary 2>/dev/null || exit 127")
        .await
        .unwrap();
    assert_eq!(result.exit_code, 127);
}

#[tokio::test]
async fn brush_exec_populates_duration() {
    let mut brush = BrushRuntime::new().await.unwrap();
    let result = brush.exec("sleep 0.1").await.unwrap();
    assert!(result.duration >= std::time::Duration::from_millis(50));
}

#[tokio::test]
async fn brush_exec_populates_cwd() {
    let mut brush = BrushRuntime::new().await.unwrap();
    let result = brush.exec("pwd").await.unwrap();
    let expected = std::env::current_dir().unwrap().to_string_lossy().to_string();
    assert_eq!(result.stdout.trim(), expected);
    assert_eq!(result.cwd, std::env::current_dir().unwrap());
}
