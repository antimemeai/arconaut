# Phase 5.5 Conformance Spec

**Source:** `docs/quarantine-testing-survey-report.md`
**Scope:** Testing infrastructure hardening — quick wins through medium effort.

---

## Assertions

### A. Tooling

| # | Assertion | Test / Verification | Oracle |
|---|-----------|---------------------|--------|
| A.1 | `cargo nextest` runs all workspace tests successfully | `cargo nextest run --workspace` | Must pass with same count as `cargo test` |
| A.2 | `cargo llvm-cov` generates HTML coverage report | `cargo llvm-cov --workspace --html` | Report generated, >0% coverage shown |

### B. Isolation

| # | Assertion | Test | Oracle |
|---|-----------|------|--------|
| B.1 | FileLock tests run serially, no filesystem races | `lock::tests::lock_uncontended` with `#[serial]` | Passes under `cargo test -- --test-threads=8` |
| B.2 | FileStorage tests run serially | `storage::tests::*` with `#[serial]` | Passes under parallel execution |

### C. Log Assertions

| # | Assertion | Test | Oracle |
|---|-----------|------|--------|
| C.1 | Refresh task logs token save failure | `refresh::tests::logs_on_save_failure` with `#[traced_test]` | `logs_contain("failed to save token")` |

### D. Snapshot Testing

| # | Assertion | Test | Oracle |
|---|-----------|------|--------|
| D.1 | Post-compaction context matches snapshot | `compaction::tests::snapshot_post_compaction` with `insta` | `.snap` file created and matched |
| D.2 | CompositeInjector output matches snapshot | `injection::tests::snapshot_composite_prompts` with `insta` | `.snap` file created and matched |

### E. HTTP Mocking

| # | Assertion | Test | Oracle |
|---|-----------|------|--------|
| E.1 | AnthropicProvider sends correct request body | `anthropic::tests::mock_request_shape` with Wiremock | Mock server receives expected JSON |
| E.2 | AnthropicProvider parses tool_use response correctly | `anthropic::tests::mock_tool_use_response` with Wiremock | `ChatResponse` contains correct `ToolCall` |

### F. CLI Black-Box

| # | Assertion | Test | Oracle |
|---|-----------|------|--------|
| F.1 | `arconaut --help` exits 0 | `tests/cli_integration.rs` with `assert_cmd` | `Command::cargo_bin("arconaut")?.arg("--help").assert().success()` |
| F.2 | `arconaut logout kimi` succeeds when no token exists | `tests/cli_integration.rs` with `assert_cmd` | Exit code 0, stdout contains "Logged out" |

### G. Property-Based

| # | Assertion | Test | Oracle |
|---|-----------|------|--------|
| G.1 | Message JSON round-trip preserves all fields | `message::tests::prop_roundtrip_json` with Proptest | 100 random messages round-trip successfully |
| G.2 | ContentPart text serialization never panics | `message::tests::prop_text_serialization` with Proptest | 100 random strings serialize without panic |

### H. Benchmarks

| # | Assertion | Test | Oracle |
|---|-----------|------|--------|
| H.1 | CJK token estimate is ~1.5x ASCII per char | `benches/estimate_tokens.rs` with Criterion | Benchmark runs, results inspectable |
| H.2 | Context append scales linearly | `benches/context_append.rs` with Criterion | Benchmark runs across history sizes |

---

## Quality Gates

- [ ] All 198+ existing tests pass
- [ ] `cargo clippy --workspace --all-targets` clean
- [ ] New tests added for each assertion above
- [ ] No regressions in test count or coverage
