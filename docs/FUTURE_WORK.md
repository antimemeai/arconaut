# Future Work

## Completed

### Kimi Vivace `/login` OAuth Flow
- **Status:** ✅ Implemented in auth module (RFC 8628 Device Authorization Grant).
- **What works:** `arconaut login kimi`, `arconaut logout kimi`, file-based token storage, request-time refresh, `oauth = true` in vars.toml.
- **What's in Phase Next:** Background refresh, login UX polish, model discovery, secure storage.

---

## Deferred to Phase Next (Immediate)

### OAuth Token Refresh — Background + Cross-Process
- **Current:** Token refreshes synchronously before each LLM call. Slow. No coordination between multiple arconaut processes.
- **Future:** Background refresh task, cross-process file locking (`fcntl.flock`), rejected-token tombstones, sleep/wake detection.
- **Rationale:** Current implementation is MVP-quality. Production use requires non-blocking refresh and multi-process safety.

### Login UX Polish
- **Current:** Basic polling with dot output. Manual vars.toml editing after login.
- **Future:** Spinner/progress indicator, `--no-browser` flag, auto-update vars.toml (with backup), post-login model discovery, user identity display, timeout handling.
- **Rationale:** The flow works but the UX is rough. Polishing it now prevents user confusion.

### Secure Credential Storage
- **Current:** OAuth tokens in `~/.config/arconaut/oauth/*.json` with `0o600`.
- **Future:** macOS Keychain (`security-framework`), Linux secret-service (`keyring-rs`), Windows Credential Manager. File as fallback.
- **Rationale:** Plaintext on disk is acceptable for alpha but not for production.

---

## Deferred to Phase 6+ (Polish / Production)

### Provider Fallback / Routing
- **Current:** One provider per session, manual selection via `--provider`.
- **Future:** Auto-fallback on rate-limit or outage. Retry with secondary provider.
- **Rationale:** Not needed for single-user alpha; critical for production reliability.

### Streaming Responses
- **Current:** Synchronous HTTP round-trips for all providers.
- **Future:** SSE/streaming for all providers (Anthropic streaming, OpenAI streaming).
- **Rationale:** Improves perceived latency; not blocking for alpha.

### Gemini Provider Implementation
- **Current:** Stub provider with correct defaults but unimplemented `chat()`.
- **Future:** Full Gemini native API implementation (`generativelanguage.googleapis.com`).
- **Rationale:** Not immediately needed; OpenRouter provides Gemini access via OpenAI-compatible API.

---

## Deferred to Phase 7 (Major Features)

### MCP Integration
- **Status:** Architecture specified, no implementation.
- **Scope:** `McpManager` deferred loading, tool discovery, MCP server registry.
- **Blocker:** Needs `rmcp` or similar crate evaluation.

### mojave Eval Integration
- **Status:** `arconaut-eval` crate is a placeholder.
- **Scope:** Stream `TrialRecord`s to mojave, consume `Decision` JSON, drive agent behavior.
- **Blocker:** Waiting on mojave interface response (letter sent 2026-06-07).

### Corpus Search
- **Status:** `arconaut-corpus` crate is a placeholder.
- **Scope:** Search neurotic_library embeddings from agent turn loop.
- **Blocker:** Waiting on neurotic_library interface response (letter sent 2026-06-07).

---

## P5.7 Commitments (Deferred within Phase 5)

These items were explicitly deferred during P5.7 pre-design. They are not in the P5.7 implementation path but are tracked for follow-up.

### Ground-Up `nvim-rs` Reimplementation
- **Status:** P5.7 uses `rmp-rpc` as an intermediate MessagePack RPC binding.
- **Trigger:** After ~2 weeks of live use + collected data on which RPC calls dominate.
- **Goal:** MIT-licensed replacement for the LGPL-3.0 `nvim-rs` crate.
- **Rationale:** License compatibility. Current `rmp-rpc` intermediate is acceptable for alpha but must be replaced for production distribution.

### Cross-Buffer Atomic Edit Coordinator
- **Status:** Deferred from Section 3.1.
- **Trigger:** When multi-file refactorings become common enough that per-buffer undo is painful.
- **Goal:** Transaction coordinator that can rollback edits across multiple neovim buffers on failure.
- **Rationale:** Neovim undo is per-buffer. P5.7 handles single-buffer edits well; multi-buffer atomicity is future work.

### Block-Level CPT for BufferManager
- **Status:** Deferred from Section 3.3.
- **Trigger:** After file-level CPT is instrumented and measured in Phase 7.
- **Goal:** Predict and prefetch not just files, but regions within files (functions, structs, comment blocks).
- **Rationale:** File-level CPT is the right granularity for P5.7. Block-level may improve hit rates but adds significant complexity.

### Remote Network Host Tomography
- **Status:** Deferred from Section 3.4.
- **Trigger:** When remote development workflows become a primary use case.
- **Goal:** Measure latency, packet loss, and bandwidth to remote hosts; adapt tool behavior (e.g., batch operations, reduce round-trips).
- **Rationale:** Phase 1 remote tools over SSH are sufficient for ad-hoc use. Adaptive behavior requires infrastructure not yet built.
