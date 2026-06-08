use arconaut_reactor::NvimRuntime;

#[tokio::test]
async fn nvim_spawn_succeeds_within_timeout() {
    // O3: Neovim roundtrip oracle — part 1: spawn.
    // This will FAIL (panic on todo!) until spawn is implemented.
    let nvim = NvimRuntime::spawn().await;
    assert!(nvim.is_ok(), "nvim spawn should succeed within 200ms");
}

#[tokio::test]
async fn nvim_command_echo_returns_output() {
    // O3: Neovim roundtrip oracle — part 2: command roundtrip.
    // This will FAIL (panic on todo!) until command is implemented.
    let mut nvim = NvimRuntime::spawn().await.expect("spawn should work");
    let result = nvim.command("echo 'hello'").await;
    assert_eq!(result.unwrap(), "hello");
}

#[tokio::test]
async fn nvim_eval_arithmetic() {
    // O3: Neovim roundtrip oracle — part 3: eval.
    // This will FAIL (panic on todo!) until eval is implemented.
    let mut nvim = NvimRuntime::spawn().await.expect("spawn should work");
    let result = nvim.eval("1 + 1").await;
    assert_eq!(result.unwrap(), rmpv::Value::Integer(2.into()));
}

#[tokio::test]
async fn nvim_shutdown_exits_cleanly() {
    // This will FAIL (panic on todo!) until shutdown is implemented.
    let nvim = NvimRuntime::spawn().await.expect("spawn should work");
    let result = nvim.shutdown().await;
    assert!(result.is_ok(), "shutdown should exit cleanly");
}

#[tokio::test]
async fn nvim_buf_lines_roundtrip() {
    // This will FAIL (panic on todo!) until buf_set_lines and buf_get_lines are implemented.
    let mut nvim = NvimRuntime::spawn().await.expect("spawn should work");

    let buf = 0; // current buffer
    nvim.buf_set_lines(buf, 0, -1, true, vec!["line1".to_string(), "line2".to_string()])
        .await
        .expect("set lines should work");

    let lines = nvim.buf_get_lines(buf, 0, -1, true).await.expect("get lines should work");
    assert_eq!(lines, vec!["line1".to_string(), "line2".to_string()]);
}
