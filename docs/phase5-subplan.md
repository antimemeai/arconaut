# Phase 5 Subplan: Advanced Features

**Issue:** arconaut-nfv (Phase 5 continuation)
**Scope:** Multi-provider auth & API capabilities, off-pulse intervention, assistant model, audit logging.
**Deferred to Phase 7:** mojave eval integration, MCP integration.
**MATERIA discipline:** JSMNTL cycle — subplan → conformance spec → red tests → implementation → green tests → review → repeat.

---

## 0. Strategic Overview

Phase 4 delivered multi-agent infrastructure. Phase 5 makes arconaut useful for the operator's immediate needs: logging into Kimi Vivace, using OpenRouter, Claude, OpenAI, Gemini — all in the same session if desired — while keeping Megan from spinning (off-pulse intervention), giving her a second brain (assistant model), and recording everything (audit logging).

The architectural thesis: **providers are first-class, named, configurable, and composable**. A session can have a primary provider (Kimi Vivace) and an assistant provider (Claude Pro) simultaneously. The provider registry is the factory; the VariableStore is the config source; the Soul consumes multiple providers.

---

## 1. Provider System

### 1.1 Provider Registry & Factory

**Files:**
- `crates/arconaut-machine/src/providers/mod.rs` — trait extensions, factory, registry
- `crates/arconaut-machine/src/providers/openai_compat.rs` — shared OpenAI-compatible HTTP client
- `crates/arconaut-machine/src/providers/openai.rs` — OpenAI brand-specific provider
- `crates/arconaut-machine/src/providers/moonshot.rs` — Moonshot AI (Kimi) brand-specific provider
- `crates/arconaut-machine/src/providers/openrouter.rs` — OpenRouter brand-specific provider
- `crates/arconaut-machine/src/providers/gemini.rs` — Google Gemini provider
- `crates/arconaut-machine/src/providers/anthropic.rs` — refactor existing into module
- `crates/arconaut-machine/src/provider.rs` — extend ChatProvider trait if needed

**Design:**

```rust
pub enum ProviderKind {
    Anthropic,
    OpenAi,
    Gemini,
    Moonshot,
    OpenRouter,
}

pub struct ProviderConfig {
    pub kind: ProviderKind,
    pub api_key: String,
    pub model: String,
    pub base_url: Option<String>,
    pub extra_headers: Option<HashMap<String, String>>,
}

pub struct ProviderFactory;

impl ProviderFactory {
    pub fn create(cfg: &ProviderConfig) -> Result<Box<dyn ChatProvider>, ProviderBuildError>;
    pub fn create_named(name: &str, vars: &VariableStore) -> Result<Box<dyn ChatProvider>, ProviderBuildError>;
}

pub struct ProviderRegistry {
    providers: HashMap<String, Box<dyn ChatProvider>>,
}
```

**Shared OpenAI-compatible client** (`openai_compat.rs`):
- `OpenAiCompatClient` — wraps `reqwest::Client`, handles `/chat/completions`, request/response serialization.
- Used by `OpenAiProvider`, `MoonshotProvider`, `OpenRouterProvider`.
- Each brand-specific provider configures its own default `base_url` and headers.

**Brand-specific distinctions:**
- `OpenAiProvider` — `base_url = https://api.openai.com/v1`, standard headers.
- `MoonshotProvider` — `base_url = https://api.moonshot.cn/v1`, Moonshot-specific model names.
- `OpenRouterProvider` — `base_url = https://openrouter.ai/api/v1`, injects `HTTP-Referer` and `X-Title` headers, parses OpenRouter-specific response metadata (provider name, pricing).
- `GeminiProvider` — native Gemini API (`generativelanguage.googleapis.com`), content parts format, safety settings, generation config.
- `AnthropicProvider` — existing code, moved to `providers/anthropic.rs`.

**ChatProvider trait extension** (if needed):
- `fn list_models(&self) -> impl Future<Output = Result<Vec<ModelInfo>, ProviderError>>` — optional, default returns "not supported".
- `fn capabilities(&self) -> HashSet<ModelCapability>` — already exists; for OpenRouter, query `/models` at runtime if possible, else fallback to docs.

### 1.2 Auth & Config System

**File:** `crates/arconaut-core/src/vars.rs` (extend), `crates/arconaut-cli/src/main.rs` (consume)

**Config TOML schema:**

```toml
# ~/.config/arconaut/vars.toml
[provider.anthropic]
api_key = "sk-ant-api03-..."
model = "claude-sonnet-4-20250514"

[provider.openai]
api_key = "sk-proj-..."
model = "gpt-4o"

[provider.moonshot]
api_key = "sk-moonshot-..."
model = "moonshot-v1-8k"

[provider.openrouter]
api_key = "sk-or-v1-..."
model = "anthropic/claude-sonnet-4"

[provider.gemini]
api_key = "..."
model = "gemini-1.5-pro"
```

**VariableStore extension:**
- `pub fn get_provider_config(&self, name: &str) -> Option<ProviderConfig>` — parses `[provider.{name}]` table into structured config.

**CLI integration:**
- `--provider <name>` — select primary provider (default: "anthropic" or first available).
- `--model <name>` — override model for primary provider.
- `--assistant-provider <name>` — select assistant provider (optional).

**Multi-auth per session:**
- Primary provider: used for `Soul.run_turn()`.
- Assistant provider: used for `AssistantModel.pulse()`.
- Both loaded from config at session start.

### 1.3 Kimi Vivace `/login` OAuth Flow (Stretch)

**File:** `crates/arconaut-machine/src/auth/oauth.rs`

**Design:**
- `OAuthFlow` trait: `start() -> OAuthUrl`, `exchange(code) -> TokenResult`.
- `KimiOAuthFlow` — Kimi Vivace-specific implementation.
- Local HTTP server on `localhost:PORT` to receive redirect.
- Browser opened via `open` (macOS) / `xdg-open` (Linux).
- Token stored in `VariableStore` session scope (TOML for now; keychain in FUTURE_WORK.md).
- CLI: `arconaut login kimi` starts the flow.

**Open questions:**
- Exact Kimi OAuth endpoints (need to verify against Kimi docs).
- Token refresh semantics.
- Whether Kimi Vivace uses OAuth or session-cookie-based auth.

**Fallback:** If OAuth flow is not viable for Kimi, implement `arconaut login kimi` as an interactive prompt that opens the browser and asks the user to paste the API key — a UX improvement over manual TOML editing.

---

## 2. Off-Pulse Intervention (Churn Detection)

**Files:**
- `crates/arconaut-agent/src/intervention.rs` — `ChurnDetector`, `InterventionInjector`
- `crates/arconaut-agent/src/injection.rs` — extend `Injector` usage
- `crates/arconaut-agent/src/soul.rs` — integrate into turn loop

**Design:**

```rust
pub struct ChurnDetector {
    trigger_phrases: Vec<&'static str>,
    max_consecutive_dups: usize,
    stall_threshold_steps: usize,
}

impl ChurnDetector {
    pub fn detect_message_churn(&self, message: &Message) -> bool;
    pub fn detect_tool_loop(&self, dedup: &Deduplicator) -> bool;
    pub fn detect_stall(&self, steps_taken: usize, last_progress_step: usize) -> bool;
}

pub struct InterventionInjector {
    detector: ChurnDetector,
}

impl Injector for InterventionInjector {
    fn inject(&self, context: &mut Context) {
        // Only injects if churn detected on the LAST assistant message
    }
}
```

**Trigger phrases:** "actually", "but wait", "on second thought", "let me reconsider", "let me rethink", "hold on", "wait".

**Intervention prompt (architecture §3.5):**
```
<system-reminder>
Churn detected. You appear to be oscillating or reconsidering without
making progress. Take a breath. State clearly:
1. What you were trying to do
2. What blocked you
3. Your next concrete action
If you are genuinely stuck, say so and stop rather than spinning.
</system-reminder>
```

**Integration into Soul turn loop:**
- After each assistant message, check for churn.
- If detected, inject intervention prompt BEFORE the next LLM call (via injector).
- If tool loop detected (5+ identical calls), inject stronger warning.
- If 10+ identical calls, hard stop the turn (return `StopReason::ChurnDetected`).

---

## 3. Assistant Model

**Files:**
- `crates/arconaut-agent/src/assistant.rs` — `AssistantModel`, `TriggerEvent`, `TriggerEngine`
- `crates/arconaut-agent/src/soul.rs` — hold `Option<AssistantModel>`
- `crates/arconaut-agent/src/lib.rs` — re-export

**Design:**

```rust
pub struct AssistantModel {
    provider: Box<dyn ChatProvider>,
    triggers: Vec<TriggerEvent>,
    proactive: bool,
    bus: Option<Arc<Bus>>,
}

pub enum TriggerEvent {
    OnToolFailure { tool_name: String },
    OnCompaction,
    OnMaxStepsWarning,
    OnUserRequest { pattern: Regex },
    Periodic { interval: Duration, last_fired: Instant },
}

impl AssistantModel {
    pub async fn query(&self, context: &str) -> Result<Message, ProviderError>;
    pub fn check_triggers(&mut self, event: &AgentEvent) -> bool;
}
```

**Integration:**
- Soul holds `Option<AssistantModel>`.
- After tool failure: assistant diagnoses root cause.
- After compaction: assistant summarizes what was lost.
- After max steps warning: assistant suggests next approach.
- Periodic: assistant reports on background analysis (if proactive).
- Assistant responses delivered to primary via Bus whisper or context injection.

**Bidirectional communication:**
- Primary → Assistant: direct method call within Soul.
- Assistant → Primary: Bus whisper with `priority: high` flag, or context injection.

---

## 4. Audit Logging

**Files:**
- `crates/arconaut-audit/src/lib.rs` — full implementation
- `crates/arconaut-audit/src/event.rs` — event types
- `crates/arconaut-audit/src/logger.rs` — JSONL appender
- `crates/arconaut-agent/src/hooks.rs` — wire `AuditHook`

**Design:**

```rust
pub struct AuditLogger {
    session_id: String,
    writer: Arc<Mutex<BufWriter<File>>>,
}

impl AuditLogger {
    pub fn new(session_id: &str, log_dir: PathBuf) -> Result<Self, AuditError>;
    pub fn log(&self, event: AuditEvent);
}

#[derive(Serialize)]
pub struct AuditEvent {
    pub timestamp: DateTime<Utc>,
    pub session_id: String,
    pub event_type: EventType,
    pub payload: Value,
}

pub enum EventType {
    TurnBegin,
    TurnEnd,
    StepBegin,
    StepEnd,
    ToolCall,
    ToolResult,
    ToolError,
    CompactionBegin,
    CompactionEnd,
    ContextRevert,
    InjectionApplied,
    HookTrigger,
    HookBlock,
    UserInput,
    StatusUpdate,
    PlanModeToggle,
    ProviderSwitch { from: String, to: String },
    AssistantQuery,
    ChurnDetected,
}
```

**Storage:**
- `~/.local/share/arconaut/audit/{session_id}/events.jsonl`
- Append-only, never modify.
- Partitioned by session.
- High-fidelity: complete verbatim where practical.

**Hook integration:**
- `AuditHook` implements `Hook` trait.
- `pre_turn`: log `TurnBegin`.
- `post_turn`: log `TurnEnd` with `steps_taken`, `stop_reason`.
- Tool calls logged via `PreToolUse` / `PostToolUse` hooks (need to extend Hook trait).

**Hook trait extension:**
```rust
pub trait Hook: Send + Sync {
    fn pre_turn(&self, _soul: &Soul) {}
    fn post_turn(&self, _soul: &Soul, _result: &TurnResult) {}
    fn pre_tool_use(&self, _soul: &Soul, _tool_name: &str, _args: &Value) {}
    fn post_tool_use(&self, _soul: &Soul, _tool_name: &str, _result: &ToolResult) {}
}
```

---

## 5. Dependencies

**New workspace dependencies:**
- None for core provider system (uses existing `reqwest`, `serde`, `serde_json`).
- `wiremock` (dev-dependency, optional) — for HTTP provider Silver-tier tests. Justified: provider HTTP mocking is essential for testable network code without real API calls.

**Internal dependencies:**
- `arconaut-machine` → `arconaut-core` (already exists).
- `arconaut-agent` → `arconaut-audit` (new, for hook integration).
- `arconaut-cli` → `arconaut-machine` providers module (new re-exports).

---

## 6. FUTURE_WORK.md Entries

Add to `docs/FUTURE_WORK.md` (create if missing):
- **Secure credential storage:** macOS Keychain / Linux secret-service / Windows Credential Manager. TOML is temporary.
- **Provider fallback / routing:** Auto-fallback on rate-limit or outage.
- **Streaming responses:** SSE/streaming for all providers (currently synchronous HTTP).
- **MCP integration:** Phase 7.
- **mojave eval integration:** Phase 7.
- **Corpus search:** Blocked on neurotic_library interface response.

---

## 7. Implementation Order

| # | Task | Files | Complexity | Blockers |
|---|------|-------|-----------|----------|
| 1 | Refactor `anthropic.rs` → `providers/anthropic.rs` | `arconaut-machine/src/providers/` | Low | None |
| 2 | `OpenAiCompatClient` shared module | `providers/openai_compat.rs` | Medium | None |
| 3 | `OpenAiProvider`, `MoonshotProvider`, `OpenRouterProvider` | `providers/openai.rs`, `moonshot.rs`, `openrouter.rs` | Medium | #2 |
| 4 | `GeminiProvider` | `providers/gemini.rs` | Medium | None |
| 5 | `ProviderFactory` + `ProviderRegistry` | `providers/mod.rs` | Medium | #1–4 |
| 6 | VariableStore provider config parsing | `arconaut-core/src/vars.rs` | Low | None |
| 7 | CLI `--provider`, `--model`, `--assistant-provider` | `arconaut-cli/src/main.rs` | Low | #5–6 |
| 8 | Wire multi-provider into Soul/Runtime | `arconaut-cli/src/main.rs` | Medium | #7 |
| 9 | `ChurnDetector` + `InterventionInjector` | `arconaut-agent/src/intervention.rs` | Medium | None |
| 10 | Integrate intervention into Soul turn loop | `arconaut-agent/src/soul.rs` | Low | #9 |
| 11 | `AuditEvent` types + `AuditLogger` | `arconaut-audit/src/` | Low | None |
| 12 | Extend Hook trait + `AuditHook` | `arconaut-agent/src/hooks.rs` | Low | #11 |
| 13 | Wire audit into runtime | `arconaut-cli/src/main.rs` | Low | #12 |
| 14 | `AssistantModel` + `TriggerEngine` | `arconaut-agent/src/assistant.rs` | Medium | #5 |
| 15 | Integrate assistant into Soul | `arconaut-agent/src/soul.rs` | Medium | #14 |
| 16 | Kimi `/login` OAuth or interactive flow | `arconaut-machine/src/auth/oauth.rs` | High | External docs |

**Parallel tracks:**
- Track A (Providers): #1–8
- Track B (Intervention + Audit): #9–13
- Track C (Assistant): #14–15
- Track D (OAuth): #16 (stretch, deferred if blocked)

---

## 8. Open Questions

1. **Gemini API version:** v1beta or v1? v1beta has thinking mode; v1 is stable. Default to v1beta for feature parity.
2. **OpenRouter metadata:** Do we expose provider name / pricing in the TUI? (Defer to Phase 6 polish.)
3. **Assistant model default:** If no assistant provider configured, should we auto-select a cheaper model from the same provider, or disable assistant? (Decision: disable if not explicitly configured.)
4. **Audit log retention:** Unlimited append-only, or rotate at N GB? (Decision: unlimited for now; rotation in FUTURE_WORK.md.)
5. **Kimi OAuth endpoints:** Need to verify Kimi's actual OAuth/API key flow. If unavailable, implement interactive API key prompt.
