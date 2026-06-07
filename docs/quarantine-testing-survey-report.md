# Quarantine Testing Survey Report

**Date:** 2026-06-07  
**Scope:** 28 repos surveyed across coding agents, testing tools, vector search, and infrastructure  
**Method:** Parallel exploration of source, test, and CI configuration files  

---

## 1. Executive Summary

The quarantined repos reveal a wide spectrum of testing maturity. At the primitive end are Python agents with ad-hoc pytest suites and no mocking strategy. At the sophisticated end are Rust systems (pi-agent-rust, cargo-mutants, proptest) with property-based testing, model-checked concurrency, fuzz targets, snapshot assertions, and conformance regression gates.

**The gap:** arconaut currently sits at the primitive end for a Rust project. We have unit tests and basic integration tests, but lack: property-based testing, HTTP mocking, snapshot testing, benchmark baselines, CLI black-box tests, and structured test classification.

**The opportunity:** Several repos demonstrate patterns that would be high-value, low-effort adoptions for arconaut.

---

## 2. Findings by Category

### 2.1 Property-Based & Fuzz Testing

| Repo | Technique | What It Tests |
|------|-----------|---------------|
| **pi-agent-rust** | Proptest | `HostcallRequest` construction, hashing invariants, JSON payload round-trips |
| **pi-agent-rust** | Loom | Epoch-based reclamation (EBR), queue contention under all scheduler interleavings |
| **pi-agent-rust** | cargo-fuzz | 15 targets: SSE parser, session JSONL, edit match, config, message roundtrip, extension payload |
| **proptest** | Self-testing | Uses its own framework to test itself; `trybuild` for proc-macro expansion |
| **quickcheck** | Self-testing | Classic examples (sieve, reverse) embedded as regression tests |
| **cargo-fuzz** | Synthetic fixtures | Ephemeral Cargo workspaces to test CLI behavior end-to-end |

**Key insight:** Property-based testing is especially powerful for LLM-agent code because the input space is enormous and edge cases are hard to anticipate. Proptest's custom strategies for tool names, JSON payloads, and nested structures would directly apply to arconaut's tool registry and message serialization.

### 2.2 HTTP Mocking & Integration Testing

| Repo | Technique | Details |
|------|-----------|---------|
| **goose** | Wiremock + Mockall | Mockall for unit isolation, Wiremock for integration tests against fake LLM endpoints |
| **pi-agent-rust** | MockHttpServer | Shared test harness with cost-budget checkers, JSONL validators, redaction helpers |
| **pi-agent-rust** | VCR tests | Recorded HTTP interactions replayed in tests (`suite.vcr` classification) |
| **openhands** | pytest-playwright | Browser automation for end-to-end IDE integration |
| **cline** | Playwright + TUI-test | VSCode extension E2E testing |

**Key insight:** No arconaut provider has a mock server test. Wiremock would let us test the Anthropic/OpenAI provider clients without API keys, verifying request serialization and response parsing against real HTTP boundaries.

### 2.3 Snapshot Testing

| Repo | Tool | Use Case |
|------|------|----------|
| **tabby** | `insta` | YAML snapshot assertions for golden tests (tool output, config, index state) |
| **cargo-mutants** | `insta` | CLI output, JSON schemas, mutation reports locked across refactors |

**Key insight:** Snapshot testing is ideal for LLM-agent output validation. Instead of asserting `output.contains("hello")`, we snapshot the full expected output. When the model or prompt changes, `insta review` shows a diff. This is perfect for: tool result formatting, context compaction summaries, system prompt injection order, and audit event JSONL.

### 2.4 Benchmarking & Performance

| Repo | Approach | Details |
|------|----------|---------|
| **pi-agent-rust** | Criterion | 7 benchmark suites: tools, extensions, semantic context, TUI perf, session save |
| **tabby** | CLI bench | `tabby-index-cli bench` — Tantivy search latency in microseconds |
| **ussearch** | Dataset matrix | BigANN, Turing-ANNS benchmarks with recall@1 and QPS |
| **criterion.rs** | Short-config | Aggressively lower iteration counts/timeouts in test mode for fast CI |
| **hyperfine** | Executor trait | Warmup → measure → cleanup → export lifecycle with calibration |

**Key insight:** Arconaut has no benchmarks. Criterion.rs would let us baseline: context token estimation accuracy, compaction trigger timing, provider HTTP latency, TUI frame times. The short-config pattern means benchmarks can run as fast integration tests in CI.

### 2.5 CLI Black-Box Testing

| Repo | Tool | What It Tests |
|------|------|---------------|
| **hyperfine** | `assert_cmd` | Subprocess invocation, stdout/stderr capture, exit code assertions |
| **cargo-fuzz** | Synthetic fixtures | Ephemeral Cargo workspaces to test CLI behavior end-to-end |

**Key insight:** `assert_cmd` would let arconaut test `arconaut login kimi`, `arconaut logout kimi`, and `arconaut --provider moonshot` as real subprocesses without mocking the CLI internals. This tests argument parsing, config loading, and file I/O at the integration boundary.

### 2.6 Concurrency & Race Testing

| Repo | Tool | What It Tests |
|------|------|---------------|
| **pi-agent-rust** | Loom | Model-checking epoch-based reclamation and queue contention |
| **tabby** | `serial_test` | Global-state-mutating tests run serially via `file_serial(set_tabby_root)` |

**Key insight:** Loom is overkill for arconaut today (we don't have lock-free data structures), but `serial_test` is immediately useful. Our `FileLock`, `ActivityTracker`, and token storage tests all mutate shared filesystem state and could race in `cargo test --parallel`.

### 2.7 Test Classification & Organization

| Repo | Approach | Details |
|------|----------|---------|
| **pi-agent-rust** | `suite_classification.toml` | Every test file classified as `suite.unit`, `suite.vcr`, or `suite.e2e` |
| **pi-agent-rust** | Quarantine system | Flaky tests auto-quarantined with 14-day expiry enforcement |
| **criterion.rs** | Feature-matrix CI | `no-default-features`, `all-features`, MSRV tested in parallel |
| **proptest** | Workspace separation | Core, macros, derive, extensions as separate crates tested together |

**Key insight:** arconaut's tests are unclassified. A `tests/suite_classification.toml` would let us run `cargo test --lib` for fast feedback, `cargo test --test integration` for medium, and `cargo test --test e2e` for slow. The quarantine system would prevent flaky tests from destabilizing CI.

### 2.8 Coverage & Quality Gates

| Repo | Tool | Details |
|------|------|---------|
| **tabby** | `cargo-llvm-cov` | LCOV reports uploaded to Codecov on every push |
| **pi-agent-rust** | Conformance regression | Python script enforces pass-rate thresholds against `conformance_summary.json` |
| **openhands** | `pytest-cov` | Branch coverage with `--cov=openhands --cov-branch` |
| **cline** | `nyc` / `c8` / Qlty | Multiple coverage tools with PR comment integration |

**Key insight:** arconaut has no coverage tracking. `cargo-llvm-cov` + Codecov would be a one-time setup with ongoing value. The conformance regression gate is more advanced but would be powerful for preventing test regressions.

### 2.9 Configuration Testing

| Repo | Approach | Details |
|------|----------|---------|
| **Figment** | Jail sandbox | Race-free env var + filesystem isolation for config tests |
| **Figment** | Provider trait | Composable, testable, provenance-aware configuration |
| **OxideAgent** | Auto-detection + merge | JSON/YAML/TOML with CLI override precedence |

**Key insight:** Figment's Jail would replace our current approach of mutating real `~/.config/arconaut/vars.toml` in tests. The Provider trait would make our configuration system composable and testable — each config source (system file, project file, env var, CLI arg) becomes a separate Provider.

### 2.10 Logging & Tracing in Tests

| Repo | Tool | Details |
|------|------|----------|
| **tabby** | `tracing-test` | Captures log output in tests for debugging |
| **pi-agent-rust** | Transcript diffing | Compares expected vs actual conversation transcripts |

**Key insight:** `tracing-test` would let us assert on log output in tests (e.g., "refresh task should log 'token saved'"). Transcript diffing is ideal for turn-loop testing — snapshot the full conversation and diff when behavior changes.

---

## 3. Comparison Matrix

| Technique | Repos Using It | Effort to Adopt | Value for Arconaut |
|-----------|---------------|-----------------|-------------------|
| `cargo nextest` | codex | Low | Faster parallel test execution |
| Wiremock + Mockall | goose | Medium | Provider HTTP tests without API keys |
| Proptest | pi-agent-rust, proptest | Medium | Property-based message/tool testing |
| `insta` (snapshots) | tabby, cargo-mutants | Low | Golden tests for output formatting |
| Criterion benchmarks | pi-agent-rust, criterion.rs | Low | Baseline performance |
| `assert_cmd` | hyperfine | Low | Black-box CLI testing |
| `serial_test` | tabby | Low | Prevent filesystem test races |
| `tracing-test` | tabby | Low | Assert on log output |
| `cargo-llvm-cov` | tabby, pi-agent-rust | Low | Coverage tracking |
| Figment Jail | Figment | Medium | Isolated config tests |
| Test classification TOML | pi-agent-rust | Medium | Unit / integration / e2e separation |
| Flaky test quarantine | pi-agent-rust | High | Requires CI infrastructure |
| Loom | pi-agent-rust | High | Overkill until we have lock-free structures |
| cargo-fuzz | pi-agent-rust, cargo-fuzz | Medium | Parser/protocol fuzzing |

---

## 4. Recommended Adoption Order

### Phase 5.5 Immediate (this session)
1. **`cargo nextest`** — install and configure; no code changes needed
2. **`insta`** — add for snapshot testing context states, tool outputs
3. **`serial_test`** — add for FileLock and storage tests
4. **`tracing-test`** — add for log assertion in refresh task tests

### Phase 5.5 Short-term (next session)
5. **Wiremock** — mock LLM endpoints for provider integration tests
6. **`assert_cmd`** — black-box tests for `arconaut login`, `arconaut logout`
7. **Criterion** — benchmark token estimation, compaction, provider latency
8. **`cargo-llvm-cov`** — CI coverage with Codecov

### Phase 6 (Polish)
9. **Proptest** — property-based testing for message serialization, tool args
10. **cargo-fuzz** — fuzz the TOML parser, JSON serializer, edit tool matcher
11. **Figment** — replace ad-hoc vars.toml parsing with Provider trait
12. **Test classification** — TOML-based suite separation

### Phase 7 (Advanced)
13. **Flaky test quarantine** — auto-detect and quarantine with expiry
14. **Loom** — if/when we add lock-free structures
15. **VCR tests** — recorded HTTP interactions for provider regression
