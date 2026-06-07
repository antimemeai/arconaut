# Future Work

## Deferred from Phase 5

### Secure Credential Storage
- **Current:** API keys stored in plaintext TOML (`~/.config/arconaut/vars.toml`).
- **Future:** macOS Keychain, Linux secret-service (libsecret), Windows Credential Manager.
- **Rationale:** TOML is acceptable for alpha; production requires secure storage.

### Provider Fallback / Routing
- **Current:** One provider per session, manual selection via `--provider`.
- **Future:** Auto-fallback on rate-limit or outage. Retry with secondary provider.
- **Rationale:** Not needed for single-user alpha; critical for production reliability.

### Streaming Responses
- **Current:** Synchronous HTTP round-trips for all providers.
- **Future:** SSE/streaming for all providers (Anthropic streaming, OpenAI streaming).
- **Rationale:** Improves perceived latency; not blocking for alpha.

### Kimi Vivace `/login` OAuth Flow
- **Current:** API key configured manually in TOML.
- **Future:** Browser-based OAuth flow with local redirect handler (`arconaut login kimi`).
- **Blocker:** Need to verify Kimi's actual OAuth endpoints and token refresh semantics.
- **Fallback:** Interactive API key prompt (`arconaut login kimi` opens browser, user pastes key).

### Gemini Provider Implementation
- **Current:** Stub provider with correct defaults but unimplemented `chat()`.
- **Future:** Full Gemini native API implementation (`generativelanguage.googleapis.com`).
- **Rationale:** Not immediately needed; OpenRouter provides Gemini access via OpenAI-compatible API.

## Deferred to Phase 7

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
