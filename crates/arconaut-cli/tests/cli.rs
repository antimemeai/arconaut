use assert_cmd::Command;
use predicates::prelude::*;

#[test]
fn cli_help_shows_usage() {
    let mut cmd = Command::cargo_bin("arconaut").unwrap();
    cmd.arg("--help");
    cmd.assert()
        .success()
        .stdout(predicate::str::contains("AI-native dev environment"))
        .stdout(predicate::str::contains("run"))
        .stdout(predicate::str::contains("login"))
        .stdout(predicate::str::contains("logout"));
}

#[test]
fn login_help_shows_options() {
    let mut cmd = Command::cargo_bin("arconaut").unwrap();
    cmd.args(["login", "--help"]);
    cmd.assert()
        .success()
        .stdout(predicate::str::contains("Provider to log in to"))
        .stdout(predicate::str::contains("--no-browser"));
}

#[test]
fn logout_help_shows_options() {
    let mut cmd = Command::cargo_bin("arconaut").unwrap();
    cmd.args(["logout", "--help"]);
    cmd.assert()
        .success()
        .stdout(predicate::str::contains("Provider to log out from"));
}

#[test]
fn version_flag_is_recognized() {
    let mut cmd = Command::cargo_bin("arconaut").unwrap();
    cmd.arg("--version");
    // The CLI may not define --version; if it fails, that's still useful signal.
    // We just assert it doesn't panic with the TTY error.
    let output = cmd.output().unwrap();
    let stderr = String::from_utf8_lossy(&output.stderr);
    // Should not contain the TTY "Device not configured" panic.
    assert!(
        !stderr.contains("Device not configured"),
        "CLI panicked on --version: {stderr}"
    );
}
