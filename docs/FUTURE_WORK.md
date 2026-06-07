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
