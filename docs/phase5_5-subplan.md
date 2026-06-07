# Phase 5.5 Subplan: Testing Infrastructure Hardening

**Scope:** Adopt proven testing patterns from quarantine survey. Quick wins → medium effort. Excluded: Loom, flaky test quarantine, VCR tests (agreed not right now).
**MATERIA discipline:** JSMNTL cycle.

---

## 0. Strategic Overview

Arconaut has 198 tests and zero warnings, but the test infrastructure is primitive by quarantine standards. No HTTP mocking, no snapshot testing, no property-based testing, no benchmarks, no coverage tracking, no CLI black-box tests. Phase 5.5 closes this gap systematically — quick tooling wins first, then medium-effort integrations, stopping before architectural refactoring.

**Excluded (agreed):**
- Loom — overkill until lock-free structures exist
- Flaky test quarantine — requires CI infrastructure
- VCR tests — Wiremock is sufficient for now

---

## 1. Immediate Tooling (No Code Changes)

### A.1 cargo nextest
**Why:** Parallel test runner; 2-10x faster than `cargo test` on multi-core. Used by codex, pi-agent-rust.
**Files:** `.config/nextest.toml` (new)
**Config:**
- `test-threads = "num-cpus"`
- `retries = 1` for flaky tests
- `slow-timeout = "30s"`
- `failure-output = "immediate-final"`

### A.2 cargo-llvm-cov
**Why:** Coverage tracking. Shows what 198 tests actually exercise.
**Files:** CI config (deferred to Phase 6), `.github/codecov.yml` (optional)
**Command:** `cargo llvm-cov --workspace --html --open`

---

## 2. Low-Effort Code Changes

### B. serial_test
**Why:** Our FileLock and FileStorage tests touch real filesystem. `#[file_serial]` prevents races in parallel test runs.
**Files:** `crates/arconaut-machine/Cargo.toml`, `crates/arconaut-machine/src/auth/lock.rs`, `crates/arconaut-machine/src/auth/storage.rs`
**Changes:**
1. Add `serial_test` dev-dependency
2. Annotate `lock_uncontended`, `lock_creates_file` with `#[serial]`
3. Annotate storage tests with `#[serial]`

### C. tracing-test
**Why:** Assert on log output. Currently we can only test side effects; this lets us test intent (e.g., "refresh task should log 'token saved'").
**Files:** `crates/arconaut-machine/Cargo.toml`, `crates/arconaut-machine/src/auth/refresh.rs`
**Changes:**
1. Add `tracing-test` dev-dependency
2. Add `#[traced_test]` attribute to refresh task tests
3. Add test asserting `logs_contain("refresh task: failed to save token")` on error path

### D. insta (Snapshot Testing)
**Why:** Golden tests for output formatting. When prompts/model behavior changes, `cargo insta review` shows diffs instead of brittle string assertions.
**Files:** `crates/arconaut-agent/Cargo.toml`, `crates/arconaut-agent/src/compaction.rs`
**Changes:**
1. Add `insta` dev-dependency
2. Snapshot test: context after compaction (history length, roles, message content)
3. Snapshot test: system prompt injection order in CompositeInjector

---

## 3. Medium-Effort Integrations

### E. Wiremock (Provider Integration Tests)
**Why:** Spin up fake Anthropic/OpenAI API server. Verify request headers, body shape, response parsing — no API keys needed.
**Files:** `crates/arconaut-machine/Cargo.toml`, `crates/arconaut-machine/src/providers/anthropic.rs`
**Changes:**
1. Add `wiremock` dev-dependency
2. Create `tests/provider_anthropic_integration.rs` (or inline test)
3. Mock `/v1/messages` endpoint with expected request body
4. Assert response parsing produces correct `ChatResponse`

### F. assert_cmd (CLI Black-Box Tests)
**Why:** Test `arconaut login`, `arconaut logout` as real subprocesses. Tests argument parsing, config loading, file I/O at integration boundary.
**Files:** `crates/arconaut-cli/Cargo.toml`, `crates/arconaut-cli/tests/cli_integration.rs` (new)
**Changes:**
1. Add `assert_cmd` + `predicates` dev-dependencies
2. Test `arconaut --help` returns exit 0
3. Test `arconaut logout kimi` succeeds when no token exists

### G. Proptest (Property-Based Testing)
**Why:** Generate random inputs and verify invariants. Perfect for message serialization, tool arg parsing.
**Files:** `crates/arconaut-core/Cargo.toml`, `crates/arconaut-core/src/message.rs`
**Changes:**
1. Add `proptest` dev-dependency
2. Property: round-trip `Message` → JSON → `Message` preserves all fields
3. Property: `ContentPart::text` + `ContentPart::ToolCall` serialization invariants

### H. Criterion (Benchmarks)
**Why:** Baseline performance. Track regressions in token estimation, compaction, provider latency.
**Files:** `crates/arconaut-core/Cargo.toml`, `crates/arconaut-core/benches/estimate_tokens.rs` (new)
**Changes:**
1. Add `criterion` dev-dependency
2. Benchmark `estimate_tokens` with ASCII vs CJK text
3. Benchmark `Context::append_message` with growing history

---

## 4. Implementation Order

| # | Task | Effort | Files | Blockers |
|---|------|--------|-------|----------|
| A | cargo nextest + llvm-cov config | Low | `.config/nextest.toml` | None |
| B | serial_test filesystem isolation | Low | `auth/lock.rs`, `auth/storage.rs` | None |
| C | tracing-test log assertions | Low | `auth/refresh.rs` | None |
| D | insta snapshot tests | Low | `compaction.rs`, `injection.rs` | None |
| E | Wiremock provider tests | Medium | `providers/anthropic.rs` | None |
| F | assert_cmd CLI tests | Medium | `tests/cli_integration.rs` | None |
| G | Proptest property tests | Medium | `message.rs` | None |
| H | Criterion benchmarks | Medium | `benches/estimate_tokens.rs` | None |

**Parallel tracks:**
- Track A (Tooling): #A
- Track B (Isolation): #B, #C
- Track C (Snapshots): #D
- Track D (Integration): #E, #F
- Track E (Advanced): #G, #H

---

## 5. Open Questions

1. **Snapshot storage:** `insta` puts snapshots in `src/snapshots/`. Should we commit them? Yes — they are the test oracle.
2. **Wiremock port selection:** Use port 0 (OS-assigned) to avoid conflicts.
3. **Proptest shrinking:** Default shrinking is usually fine. Custom shrinkers only if needed.
4. **Benchmark CI:** Criterion needs stable hardware for meaningful comparison. Run locally for now; CI in Phase 6.
