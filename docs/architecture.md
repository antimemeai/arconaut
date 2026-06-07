# arconaut Architecture

**Status:** Design Draft v0.1  
**Date:** 2026-06-07  
**Purpose:** Comprehensive architecture for arconaut — a personal, Rust-based, Ghostty-native AI coding agent CLI. Model ergonomics are A-1 priority.

---

## Table of Contents

1. [Design Philosophy](#1-design-philosophy)
2. [System Overview](#2-system-overview)
3. [Core Engine (Soul)](#3-core-engine-soul)
4. [Machine Interface](#4-machine-interface)
5. [TUI Layer](#5-tui-layer)
6. [Multi-Agent System](#6-multi-agent-system)
7. [Scaffolding Layer](#7-scaffolding-layer)
8. [Context & Memory](#8-context--memory)
9. [Audit & Telemetry](#9-audit--telemetry)
10. [Eval Integration](#10-eval-integration)
11. [Visual Design Language](#11-visual-design-language)
12. [Implementation Phases](#12-implementation-phases)

---

## 1. Design Philosophy

### arconaut is for the Agent

Not the human. Model ergonomics are the A-1 priority. Every design decision optimizes for the agent's ability to reason, act, and recover.

### Principles

1. **Poweruser Mode** — No "safety belt" sandbox. Sanity checks and user choice, not helmets. Permission model: meticulous, not restrictive. The user is competent.
2. **Devilishly Performant** — Rust. Zero-copy where possible. Lock-free where possible. Async throughout. Sub-50ms TUI frame times.
3. **Lazy Everything** — Skills load on demand. Embeddings compute on demand. Tools register on demand. Context compacts aggressively.
4. **Ghostty-Native** — Kitty keyboard protocol, Kitty graphics protocol, mode 2031 theme detection. We target the best terminal, not the lowest common denominator.
5. **Single User** — No generality, no scaling, no caring what others think. This is YOUR agent.

---

## 2. System Overview

### Layer Diagram

```
┌─────────────────────────────────────────────────────────────────────────────┐
│                              HUMAN LAYER                                     │
│  ┌─────────────┐  ┌─────────────┐  ┌─────────────┐  ┌─────────────────────┐ │
│  │   Ghostty   │  │   Ghostty   │  │   Ghostty   │  │   IRC Client /      │ │
│  │  Window 1   │  │  Window 2   │  │  Window N   │  │   Web Dashboard     │ │
│  │  (Agent A)  │  │  (Agent B)  │  │  (Session X)│  │   (Overview)        │ │
│  └──────┬──────┘  └──────┬──────┘  └──────┬──────┘  └──────────┬──────────┘ │
│         │                │                │                    │            │
│         └────────────────┴────────────────┘                    │            │
│                          │                                     │            │
│                   ┌──────┴──────┐                              │            │
│                   │   TUI Bus   │◄─────────────────────────────┘            │
│                   │  (ratatui)  │                                           │
│                   └──────┬──────┘                                           │
└──────────────────────────┼──────────────────────────────────────────────────┘
                           │
┌──────────────────────────┼──────────────────────────────────────────────────┐
│                      AGENT LAYER                                             │
│                           │                                                  │
│  ┌────────────────────────┼──────────────────────────────────────────────┐   │
│  │                        ▼                                              │   │
│  │  ┌─────────────────────────────────────────────────────────────────┐  │   │
│  │  │                      CORE ENGINE (Soul)                          │  │   │
│  │  │  ┌─────────────┐  ┌─────────────┐  ┌─────────────────────────┐  │  │   │
│  │  │  │   Context   │  │   Agent     │  │     Orchestrator        │  │  │   │
│  │  │  │   Manager   │  │   Loop      │  │  (Turn/Step Lifecycle)  │  │  │   │
│  │  │  └─────────────┘  └─────────────┘  └─────────────────────────┘  │  │   │
│  │  │  ┌─────────────┐  ┌─────────────┐  ┌─────────────────────────┐  │  │   │
│  │  │  │ Compaction  │  │   Hook      │  │    Intervention         │  │  │   │
│  │  │  │   Engine    │  │   Engine    │  │     Detector            │  │  │   │
│  │  │  └─────────────┘  └─────────────┘  └─────────────────────────┘  │  │   │
│  │  └─────────────────────────────────────────────────────────────────┘  │   │
│  │                        │                                              │   │
│  │  ┌─────────────────────┼──────────────────────────────────────────┐  │   │
│  │  │              MACHINE INTERFACE                                     │  │   │
│  │  │  ┌─────────────┐  ┌─────────────┐  ┌─────────────────────────┐  │  │   │
│  │  │  │   LLM       │  │   Tool      │  │       MCP Client        │  │  │   │
│  │  │  │  Provider   │  │   Registry  │  │    (Deferred Load)      │  │  │   │
│  │  │  │  Abstraction│  │             │  │                         │  │  │   │
│  │  │  └─────────────┘  └─────────────┘  └─────────────────────────┘  │  │   │
│  │  │  ┌─────────────┐  ┌─────────────┐  ┌─────────────────────────┐  │  │   │
│  │  │  │   Auth      │  │   Skill     │  │    Assistant Model      │  │  │   │
│  │  │  │  (API Key/  │  │   Loader    │  │    (Secondary LLM)      │  │  │   │
│  │  │  │   OAuth)    │  │  (Lazy)     │  │                         │  │  │   │
│  │  │  └─────────────┘  └─────────────┘  └─────────────────────────┘  │  │   │
│  │  └─────────────────────────────────────────────────────────────────┘  │   │
│  └─────────────────────────────────────────────────────────────────────┘   │
│                           │                                                  │
│  ┌────────────────────────┼──────────────────────────────────────────────┐   │
│  │                   SCAFFOLDING LAYER                                      │   │
│  │  ┌─────────────┐  ┌─────────────┐  ┌─────────────────────────────────┐  │   │
│  │  │   Tools     │  │   Skills    │  │   Attached Embeddings           │  │   │
│  │  │  (Built-in) │  │  (Lazy Load)│  │   (Corpus Search)               │  │   │
│  │  └─────────────┘  └─────────────┘  └─────────────────────────────────┘  │   │
│  │  ┌─────────────┐  ┌─────────────┐  ┌─────────────────────────────────┐  │   │
│  │  │  Variables  │  │  Documents  │  │   Utilities Bin                 │  │   │
│  │  │(sys/proj/  │  │  /Reports   │  │   (LLM-legible metadata)        │  │   │
│  │  │  session)   │  │  (PDF gen)  │  │                                 │  │   │
│  │  └─────────────┘  └─────────────┘  └─────────────────────────────────┘  │   │
│  └─────────────────────────────────────────────────────────────────────────┘   │
│                           │                                                    │
│  ┌────────────────────────┼──────────────────────────────────────────────┐     │
│  │                   INFRASTRUCTURE                                         │     │
│  │  ┌─────────────┐  ┌─────────────┐  ┌─────────────────────────────────┐  │     │
│  │  │  Audit Log  │  │   Metrics   │  │   Eval Interface (mojave)       │  │     │
│  │  │ (Append-   │  │  (Always-on │  │                                 │  │     │
│  │  │  only)      │  │   + Custom) │  │                                 │  │     │
│  │  └─────────────┘  └─────────────┘  └─────────────────────────────────┘  │     │
│  │  ┌─────────────┐  ┌─────────────┐  ┌─────────────────────────────────┐  │     │
│  │  │   Neurotic  │  │   gRPC      │  │   Local IRC Server              │  │     │
│  │  │   Library   │  │   Inbox     │  │   (Agent Coordination)          │  │     │
│  │  │   Search    │  │             │  │                                 │  │     │
│  │  └─────────────┘  └─────────────┘  └─────────────────────────────────┘  │     │
│  └─────────────────────────────────────────────────────────────────────────┘     │
└──────────────────────────────────────────────────────────────────────────────────┘
```

### Crate Structure

```
arconaut/
├── Cargo.toml                    # Workspace root
├── crates/
│   ├── arconaut-core/            # Soul, context, compaction, hooks
│   ├── arconaut-machine/         # LLM providers, tool registry, MCP
│   ├── arconaut-tui/             # ratatui frontend, Ghostty optimizations
│   ├── arconaut-agent/           # Multi-agent orchestration, gRPC, IRC
│   ├── arconaut-audit/           # Append-only event log, partitions
│   ├── arconaut-eval/            # mojave eval integration
│   ├── arconaut-corpus/          # Neurotic library search, embeddings
│   └── arconaut-cli/             # Binary entry point, config
```

---

## 3. Core Engine (Soul)

### 3.1 Turn/Step Lifecycle

Inspired by KimiSoul's explicit lifecycle:

```
TURN
├── 1. Initialization
│   ├── Discard stale steers
│   ├── Start deferred MCP loading (background)
│   └── Wait for MCP if needed
│
├── 2. Step Loop (repeat until stop)
│   ├── 2a. Step Guard (max steps per turn)
│   ├── 2b. Step Begin (emit event)
│   ├── 2c. Context Compaction (auto-trigger if >ratio)
│   ├── 2d. Checkpoint (persist before LLM call)
│   ├── 2e. Step Execution
│   │   ├── Notification delivery (root only)
│   │   ├── Dynamic injection collection
│   │   ├── History normalization
│   │   ├── LLM call with retry
│   │   ├── Usage & status update
│   │   ├── Tool execution (parallel where possible)
│   │   ├── Context growth (append results)
│   │   └── Outcome resolution
│   └── 2f. Error handling
│       ├── BackToTheFuture (revert to checkpoint)
│       └── Fatal error (break loop)
│
└── 3. Turn Resolution
    ├── Consume pending steers
    ├── Auto-title (first turn)
    └── Emit TurnEnd
```

### 3.2 Context Management

**Context is the agent's working memory.** It must be:
- **Checkpointable:** Save state before each LLM call for rewind
- **Compactable:** Summarize old history when approaching token limits
- **Observable:** Token count, usage ratio, growth rate always visible

```rust
pub struct Context {
    history: Vec<Message>,
    token_count: usize,
    max_tokens: usize,
    checkpoints: Vec<Checkpoint>,
    compaction_trigger_ratio: f32,  // e.g., 0.8
    reserved_context_size: usize,   // headroom for tool results
}
```

**Compaction strategies (pluggable):**
- `SimpleCompaction`: LLM summarizes history into a single system message
- `SegmentedCompaction`: Preserve recent N messages, summarize older segments
- `CheckpointCompaction`: Keep full history at checkpoints, summarize between

### 3.3 Dynamic Injection

Before each LLM call, collect injections from registered providers:

```rust
#[async_trait]
pub trait InjectionProvider: Send + Sync {
    async fn get_injections(&self, history: &[Message], soul: &Soul) -> Vec<Injection>;
    async fn on_context_compacted(&self) {}
    async fn on_afk_changed(&self, enabled: bool) {}
}
```

**Built-in providers:**
- `PlanModeProvider`: Periodic reminder of active plan file
- `AfkModeProvider`: "You are running autonomously" prompts
- `InterventionProvider`: Off-pulse churn detection (see §3.5)
- `PeriodicPromptProvider`: User-configurable reminder text

### 3.4 Hook Engine

Event-driven extensibility. Hooks can block, modify, or observe.

```rust
pub enum HookEvent {
    UserPromptSubmit { session_id, cwd, prompt },
    PreToolUse { session_id, cwd, tool_name, tool_input },
    PostToolUse { session_id, cwd, tool_name, tool_output },
    PostToolUseFailure { session_id, cwd, tool_name, error },
    PreCompact { session_id, cwd, trigger, token_count },
    PostCompact { session_id, cwd, estimated_token_count },
    Stop { session_id, cwd },
    StopFailure { session_id, cwd, error_type, error_message },
    Notification { session_id, cwd, sink, notification_type, title, body },
}
```

**Hook result:**
```rust
pub enum HookAction {
    Pass,
    Block { reason: String },
    Modify { data: JsonValue },
}
```

### 3.5 Off-Pulse Intervention

**Detect churn phrases** in the agent's output and automatically inject an interrupt prompt.

**Trigger phrases:**
- "actually," / "but wait," / "on second thought," / "let me reconsider"
- Repeated identical tool calls (see deduplication)
- Token usage stall (N steps with no progress on task)

**Intervention prompt:**
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

### 3.6 Deduplication

Track tool calls across steps to prevent loops:

```rust
pub struct DedupTracker {
    seen_call_keys: HashSet<(String, String)>,  // (tool_name, canonical_args)
    consecutive_key: Option<(String, String)>,
    consecutive_count: usize,
}
```

**Rules:**
- Same-step dup: Wait for original task, copy result
- Cross-step dup (3x): Inject reminder: "You are repeating the exact same tool call..."
- Cross-step dup (5x, 8x): Inject detailed reminder with call history
- Cross-step dup (10x): Hard stop the turn

---

## 4. Machine Interface

### 4.1 LLM Provider Abstraction

```rust
#[async_trait]
pub trait ChatProvider: Send + Sync {
    async fn chat(&self, request: ChatRequest) -> Result<ChatResponse, ProviderError>;
    fn model_name(&self) -> &str;
    fn max_context_size(&self) -> usize;
    fn capabilities(&self) -> HashSet<ModelCapability>;
    fn thinking_effort(&self) -> Option<&str>;
}
```

**Supported providers:**
- Anthropic (Claude) — native tool use, thinking mode
- OpenAI (GPT-4o, o3) — function calling
- Google (Gemini) — native tool use
- Kimi (Moonshot) — via OpenAI-compatible API
- Local (Ollama, vLLM, llama.cpp) — OpenAI-compatible

**Auth modes:**
- API key (stored in system variables, never in prompt)
- OAuth (`/login` flow with token refresh)

### 4.2 Tool Registry

Tools are typed Rust structs implementing a trait:

```rust
#[async_trait]
pub trait Tool: Send + Sync {
    fn name(&self) -> &str;
    fn description(&self) -> &str;
    fn parameters(&self) -> &Value;  // JSON Schema
    async fn call(&self, args: Value) -> Result<ToolResult, ToolError>;
}
```

**Built-in tools:**
- `read` — File/directory read with offset/limit
- `write` — File create/overwrite
- `edit` — Surgical text replacement (old_string/new_string)
- `bash` — Shell execution
- `grep` — Content search (regex)
- `find` — File globbing
- `skill` — Load and execute a skill file
- `agent` — Spawn subagent (fork or inline)
- `ask_user` — Interactive question with choices
- `think` — Explicit reasoning step (visible in TUI)

**Tool visibility rules:**
- Plan mode: Hide `write`, `edit`, `bash`; keep `read`, `grep`, `find`
- Subagent: Same toolset as parent unless restricted
- Hidden tools: Not exposed to LLM but callable programmatically

### 4.3 MCP Integration

MCP servers loaded **deferred** in background on session start:

```rust
pub struct McpManager {
    servers: HashMap<String, McpServer>,
    loading_task: Option<JoinHandle<()>>,
}
```

- Start loading in background immediately
- Wait before first tool-using step if tools are needed
- Toast notification on connect/fail
- OAuth-enabled servers require explicit auth

### 4.4 Skill Loader (Lazy)

**Pi pattern:** Only metadata in system prompt. Content loaded on demand.

```rust
pub struct Skill {
    pub name: String,
    pub description: String,
    pub file_path: PathBuf,
    pub source: SkillSource,  // user | project | path
    pub disable_model_invocation: bool,
}

pub struct SkillLoader {
    user_skills_dir: PathBuf,      // ~/.config/arconaut/skills/
    project_skills_dir: PathBuf,   // ./.arconaut/skills/
}
```

**Discovery rules:**
1. Directory contains `SKILL.md` → skill root, no recursion
2. Otherwise, direct `.md` children in root
3. Recurse into subdirectories to find `SKILL.md`

**System prompt format:**
```xml
<available_skills>
  <skill>
    <name>{name}</name>
    <description>{description}</description>
    <location>{file_path}</location>
  </skill>
</available_skills>
```

**Invocation:** `skill://{name}` URL or `/skill:{name}` slash command

---

## 5. TUI Layer

### 5.1 Architecture

```
┌─────────────────────────────────────────────┐
│  Header: Agent name | Model | Session clock │
├─────────────────────────────────────────────┤
│                                             │
│  ┌─────────────────────────────────────┐   │
│  │         Conversation Pane           │   │
│  │  (Messages with timestamps,         │   │
│  │   tool calls, thinking blocks)      │   │
│  │                                     │   │
│  └─────────────────────────────────────┘   │
│                                             │
│  ┌──────────────────┬───────────────────┐  │
│  │   Context Bar    │   Metrics Panel   │  │
│  │  (token usage    │  (progress hooks, │  │
│  │   ratio, plan    │   custom metrics) │  │
│  │   mode indicator)│                   │  │
│  └──────────────────┴───────────────────┘  │
│                                             │
│  ┌─────────────────────────────────────┐   │
│  │         Input Line                  │   │
│  │  (with slash command completion)    │   │
│  └─────────────────────────────────────┘   │
└─────────────────────────────────────────────┘
```

### 5.2 Ghostty Optimizations

**Kitty Keyboard Protocol:**
- Enable progressive enhancement (report all key events with modifiers)
- Support key release events for chorded shortcuts
- Distinguish `Ctrl+Enter` from `Enter`

**Kitty Graphics Protocol:**
- Inline images in conversation (model outputs, screenshots)
- ASCII art on startup (Caves of Qud style)
- Document thumbnails in document list

**Mode 2031 (Theme Detection):**
- Detect dark/light mode from terminal
- Auto-switch color palette
- Emit `OSC 11` query on startup

**Synchronized Output (Mode 2026):**
- Batch frame updates to prevent tearing
- Essential for rapid streaming output

### 5.3 Rendering Performance

- **Immediate mode:** ratatui redraws entire frame each tick
- **Double-buffer diff:** Only changed cells written
- **Target:** <1ms frame time, 60+ FPS
- **Streaming:** LLM output rendered token-by-token without full redraw

### 5.4 Dual Interface

The TUI is purely a view layer. All state lives in the Core Engine. This enables:
- Headless mode (no TUI, JSON/line output)
- Multiple TUI windows attached to same session
- Programmatic control via gRPC/IRC

---

## 6. Multi-Agent System

### 6.1 Named Agents

```rust
pub struct Agent {
    pub name: String,              // e.g., "implementer", "reviewer", "explorer"
    pub callsign: String,          // e.g., "ALPHA", "BETA"
    pub mode: AgentMode,
    pub model_config: ModelConfig,
    pub context: Context,          // Persisted per-agent
    pub session_ids: Vec<String>,  // Audit partition per session
}
```

**Agent modes:**
- `implement` — Code changes, file edits
- `review` — Read-only analysis, feedback
- `explore` — Codebase research, mapping
- `test` — Test writing, verification
- `assist` — Secondary model, triggered by events

### 6.2 Named Sessions

```rust
pub struct Session {
    pub name: String,
    pub agent_name: String,
    pub created_at: DateTime<Utc>,
    pub audit_partition: String,   // Separate append-only log
    pub state_path: PathBuf,       // Persisted context + config
}
```

### 6.3 Agent Inbox (gRPC)

Local gRPC endpoint for inter-agent communication:

```protobuf
service AgentInbox {
  rpc SendMessage(Message) returns (Empty);
  rpc StreamMessages(StreamRequest) returns (stream Message);
  rpc GetStatus(AgentId) returns (AgentStatus);
}
```

**Use cases:**
- Parent agent sends directive to subagent
- Assistant model reports findings to primary
- Agent A requests code review from Agent B
- Progress notifications (X/Y complete)

### 6.4 Local IRC Server

Lightweight IRC-like server for coordination:

```
#arconaut-general    — Broadcast channel for all agents
#arconaut-handoffs   — Session handoff messages
#agent-<name>        — Per-agent private channel
```

**Why IRC:**
- Simple text protocol, easy to parse
- Existing client ecosystem (weechat, irssi, ZNC)
- Agents can join/leave channels dynamically
- Human can monitor all agent activity in IRC client

### 6.5 Window Model

**Multiple terminal windows:**
```bash
arconaut --agent implementer --window    # New Ghostty window
arconaut --agent reviewer --window       # Another window
arconaut --session debug-2026-06-07      # Named session
```

**Same window, multiple panes:**
- Split-pane view in single TUI
- Left: conversation, Right: context/metrics
- Or: Top: primary agent, Bottom: assistant model output

### 6.6 Brief-Based Dispatch (Claudex Pattern)

For coordinated multi-agent tasks:

```rust
pub struct Brief {
    pub id: String,
    pub title: String,
    pub index: String,         // Shared context (INDEX.md)
    pub assignments: Vec<AgentAssignment>,
}

pub struct AgentAssignment {
    pub callsign: String,
    pub lane: String,          // Scope constraint
    pub deliverables: Vec<PathBuf>,
}
```

**Dispatch flow:**
1. User writes brief + index
2. System renders per-agent prompt: brief + index + lane constraints
3. Dispatches to agent windows/sessions
4. Agents report completion to inbox
5. User (or mayor agent) coordinates integration

---

## 7. Scaffolding Layer

### 7.1 Tools

See §4.2 for built-in tools. Additional tool categories:

**File tools:**
- `read` — Read file/dir with offset/limit
- `write` — Create/overwrite
- `edit` — Surgical replacement
- `replace` — Regex-based replacement

**Search tools:**
- `grep` — Regex content search
- `find` — Glob file discovery

**Execution tools:**
- `bash` — Shell command
- `eval` — Quick computation (Python/Rust expression)

**Agent tools:**
- `agent` — Spawn subagent
- `ask_user` — Interactive question

**Meta tools:**
- `think` — Explicit reasoning (visible in TUI)
- `skill` — Load skill file
- `report_tool_issue` — Automated QA

### 7.2 Skills

See §4.4 for lazy loading. Skill file format:

```markdown
---
name: rust-refactor
description: Refactor Rust code following project conventions
disable-model-invocation: false
---

# Rust Refactoring Skill

When refactoring Rust code:
1. Run `cargo check` before and after
2. Prefer `if let` over `match` for single variants
3. Use `?` instead of `match` for error propagation
4. ...
```

### 7.3 Attached Embeddings

Integration with neurotic_library:

```rust
pub struct CorpusSearch {
    pub index_path: PathBuf,
    pub embedding_model: String,
}

impl Tool for CorpusSearch {
    fn name(&self) -> &str { "corpus_search" }
    async fn call(&self, args: Value) -> Result<ToolResult, ToolError> {
        // Search neurotic_library corpus
        // Return top-K papers/snippets with metadata
    }
}
```

**Future:** Central embedding search across all project documents, skills, and audit logs.

### 7.4 Assistant Model

**Secondary model that the primary can query.**

```rust
pub struct AssistantModel {
    pub provider: Box<dyn ChatProvider>,
    pub trigger_events: Vec<TriggerEvent>,
    pub proactive: bool,  // Can reach out to primary
}

pub enum TriggerEvent {
    OnToolFailure { tool_name: String },
    OnCompaction,
    OnMaxStepsWarning,
    OnUserRequest { pattern: Regex },
    Periodic { interval: Duration },
}
```

**Use cases:**
- Primary gets stuck → Assistant suggests approach
- Post-compaction → Assistant summarizes what was lost
- Tool failure → Assistant diagnoses root cause
- Periodic → Assistant reports on background analysis

**Bidirectional triggers:**
- Primary → Assistant: `assistant_query` tool call
- Assistant → Primary: Inbox message with priority flag

---

## 8. Context & Memory

### 8.1 Variable Storage

Three-level hierarchy:

```rust
pub enum VariableScope {
    System,   // ~/.config/arconaut/vars.toml — API keys, defaults
    Project,  // ./.arconaut/vars.toml — project-specific
    Session,  // In-memory, per-session — temp state
}

pub struct VariableStore {
    system: TomlTable,
    project: TomlTable,
    session: HashMap<String, Value>,
}
```

**Usage in system prompt:**
```
Environment variables:
- API_KEY: {var:system.api_key}
- PROJECT_NAME: {var:project.name}
- TEMP_DIR: {var:session.temp_dir}
```

### 8.2 Interactive Compaction

When context approaches limit:

1. **Auto-compact** (system-driven): Summarize oldest messages automatically
2. **Interactive compact** (user-driven): `/compact` slash command
3. **Model-driven compact**: Agent requests compaction with focus instruction

**Reorientation prompt:**
```
Context has been compacted. Here is what you were doing:
[Summary of active task, recent decisions, open questions]

Continue from where you left off.
```

### 8.3 Token Management

```rust
pub struct TokenBudget {
    pub max_context: usize,
    pub trigger_ratio: f32,      // 0.8 = compact at 80%
    pub reserved_size: usize,    // Headroom for tool results
    pub auto_remove_tool_output: bool,  // Remove old tool results
    pub keep_recent_results: usize,     // N most recent results preserved
}
```

**Auto-remove:**
- After N turns, replace verbose tool results with "[Output removed to save context]"
- Keep recent N results always
- Critical results (errors, user approvals) never removed

---

## 9. Audit & Telemetry

### 9.1 Append-Only Audit Log

**EVERY event is logged.** No exceptions.

```rust
pub struct AuditEvent {
    pub timestamp: DateTime<Utc>,
    pub session_id: String,
    pub agent_name: String,
    pub event_type: EventType,
    pub payload: JsonValue,
}

pub enum EventType {
    TurnBegin,
    TurnEnd,
    StepBegin,
    StepEnd,
    MessageAppend,
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
    SteerInput,
    StatusUpdate,
    PlanModeToggle,
    AfkModeToggle,
    YoloToggle,
    SessionCreate,
    SessionEnd,
}
```

**Storage:**
- Partitioned by session: `~/.local/share/arconaut/audit/{session_id}/events.jsonl`
- Append-only: Never modify, only append
- High-fidelity: Complete verbatim history
- Scannable: Agents can read audit log via `read` tool

### 9.2 Metrics Interface

**Always-on metrics:**
- Session clock (elapsed time)
- Token usage (current / max / ratio)
- Step count (current turn / total)
- Tool call count (per tool)
- API cost (if provider reports it)

**User/model definable metrics:**
```rust
pub struct CustomMetric {
    pub name: String,
    pub value: f64,
    pub max: Option<f64>,  // For progress bars
    pub format: MetricFormat,
}

pub enum MetricFormat {
    Percentage,
    Count,
    Duration,
    Bytes,
    Custom(String),  // Format string
}
```

**Progress hooks:**
```rust
// Agent reports progress: "X/Y complete"
pub fn report_progress(name: &str, current: usize, total: usize) {
    // Updates metrics panel
    // Other agents can poll or receive push notification
}
```

### 9.3 Timestamps

**All messages have timestamps.** Default visible in TUI.

```rust
pub struct Message {
    pub role: Role,
    pub content: Vec<ContentPart>,
    pub timestamp: DateTime<Utc>,
    pub metadata: MessageMetadata,
}
```

**Display format:**
- Recent: `14:32:07`
- Older: `Jun 07 14:32`
- Hover: Full ISO `2026-06-07T14:32:07Z`

---

## 10. Eval Integration

### 10.1 mojave Interface

Native integration with `~/projects/mojave/` eval infrastructure:

```rust
pub struct EvalInterface {
    pub eval_core: eval_core::Client,
    pub orchestrator: eval_orchestrator::Client,
    pub perturbation: perturbation_engine::Client,
}
```

**Eval tools:**
- `eval_run` — Run a trial against a benchmark
- `eval_compare` — Compare two model/agent configurations
- `eval_mutate` — Apply mutation testing
- `eval_report` — Generate evaluation report

### 10.2 Built-in Benchmarks

- **Code completion:** HumanEval, MBPP, SWE-bench-lite
- **Tool use:** Custom harness measuring tool selection accuracy
- **Context management:** Stress tests for compaction quality
- **Multi-agent:** Coordination tasks, handoff fidelity

### 10.3 Regression Testing

```bash
arconaut eval --suite nightly    # Run full suite
arconaut eval --suite quick      # Smoke tests only
arconaut eval --compare v1 v2    # A/B comparison
```

---

## 11. Visual Design Language

### 11.1 Caves of Qud Aesthetic

**Color palette (dark mode, default):**
```
Background:      #0a0a0f (near-black, slightly blue)
Surface:         #12121a (panels, cards)
Border:          #2a2a3a (subtle, slightly warm)
Text Primary:    #d4c5a9 (aged parchment)
Text Secondary:  #8a7f6b (muted gold)
Accent:          #c4a35a (qud gold)
Success:         #5a8f5a (muted green)
Warning:         #c4a35a (gold)
Error:           #a34a4a (muted red)
Info:            #6a8aaa (muted blue)

Special:
- Mutation glow: #8a6ac4 (purple shimmer)
- Chrome shimmer: #a0a0b0 (metallic)
- Psionic pulse: #c45a8a (pink pulse)
```

**Typography:**
- Primary: JetBrains Mono or similar
- ASCII art headers using box drawing + block elements
- Braille sparklines for metrics

### 11.2 ASCII Art on Startup

```
    ▓█████  ██▀███   ▄████▄   ▒█████   ███▄    █  ▄▄▄      ▄████▄   ██ ▄█▀
    ▓█   ▀ ▓██ ▒ ██▒▒██▀ ▀█  ▒██▒  ██▒ ██ ▀█   █ ▒████▄   ▒██▀ ▀█   ██▄█▒ 
    ▒███   ▓██ ░▄█ ▒▒▓█    ▄ ▒██░  ██▒▓██  ▀█ ██▒▒██  ▀█▄ ▒▓█    ▄ ▓███▄░ 
    ▒▓█  ▄ ▒██▀▀█▄  ▒▓▓▄ ▄██▒▒██   ██░▓██▒  ▐▌██▒░██▄▄▄▄██▒▓▓▄ ▄██▒▓██ █▄ 
    ░▒████▒░██▓ ▒██▒▒ ▓███▀ ░░ ████▓▒░▒██░   ▓██░ ▓█   ▓██▒ ▓███▀ ░▒██▒ █▄
    ░░ ▒░ ░░ ▒▓ ░▒▓░░ ░▒ ▒  ░░ ▒░▒░▒░ ░ ▒░   ▒ ▒  ▒▒   ▓▒█░ ░▒ ▒  ░▒ ▒▒ ▓▒
     ░ ░  ░  ░▒ ░ ▒░  ░  ▒     ░ ▒ ▒░ ░ ░░   ░ ▒░  ▒   ▒▒ ░  ░  ▒   ░ ▒ ▒░
       ░     ░░   ░ ░        ░ ░ ░ ▒     ░   ░ ░   ░   ▒   ░        ░ ░ ░ ▒ 
       ░  ░   ░     ░ ░          ░ ░           ░       ░  ░░ ░          ░ ░  
                    ░                                      ░                  
    ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
     AGENT: {name}  MODEL: {model}  SESSION: {session}  
    ━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━
```

### 11.3 Light Mode

Auto-detected via mode 2031. Palette inverts:
```
Background:      #f5f0e8 (warm white)
Surface:         #e8e0d4 (parchment)
Border:          #c4b8a8 (warm gray)
Text Primary:    #2a2520 (dark brown)
Text Secondary:  #6a6050 (medium brown)
Accent:          #8a7020 (dark gold)
```

---

## 12. Implementation Phases

### Phase 0: Foundation (Week 1)
- Workspace setup, CI, test harness
- Core types: Message, ContentPart, Tool, ToolResult
- LLM provider abstraction (Anthropic first)
- Basic context manager
- Simple REPL (no TUI yet)

### Phase 1: Soul (Week 2-3)
- Turn/step lifecycle
- Tool registry + built-in tools (read, write, edit, bash, grep)
- Dynamic injection framework
- Hook engine
- Deduplication
- Compaction engine

### Phase 2: TUI (Week 3-4)
- ratatui integration
- Conversation pane with timestamps
- Input line with slash completion
- Context bar, metrics panel
- Ghostty optimizations (Kitty keyboard, graphics, mode 2031)
- Caves of Qud styling

### Phase 3: Scaffolding (Week 4-5)
- Lazy skill loader
- Variable storage (system/project/session)
- Document/report list
- PDF generation from markdown
- Utilities bin framework

### Phase 4: Multi-Agent (Week 5-7)
- Named agents with persisted context
- Named sessions with audit partitions
- gRPC inbox
- Local IRC server
- Brief-based dispatch
- Multiple window support

### Phase 5: Advanced (Week 7-8)
- Assistant model
- Off-pulse intervention
- MCP integration
- Neurotic library corpus search
- mojave eval integration
- OAuth auth flow

### Phase 6: Polish (Week 8-10)
- Performance optimization
- Comprehensive tests
- Documentation
- User hand-written system prompt integration
- ASCII art polish

---

## Appendix A: Configuration Schema

```toml
[core]
model = "claude-sonnet-4-6"
max_steps_per_turn = 50
compaction_trigger_ratio = 0.8
reserved_context_size = 4096

[ui]
theme = "qud-dark"  # or "qud-light", "minimal"
timestamps = "visible"  # visible | hover | hidden
show_thinking = true
show_tool_calls = true

[auth.anthropic]
type = "api_key"  # or "oauth"
# key loaded from var:system.anthropic_api_key

[agents.default]
mode = "implement"
model = "claude-sonnet-4-6"

[agents.reviewer]
mode = "review"
model = "claude-haiku-4"

[skills]
user_dir = "~/.config/arconaut/skills"
project_dir = "./.arconaut/skills"

[eval]
mojave_path = "~/projects/mojave"
benchmarks = ["humaneval", "mbpp", "swe-bench-lite"]

[metrics]
always_on = ["session_clock", "token_usage", "step_count"]
```

## Appendix B: Slash Commands

| Command | Description |
|---------|-------------|
| `/compact` | Manually trigger context compaction |
| `/plan` | Toggle plan mode |
| `/yolo` | Toggle yolo (auto-approve) mode |
| `/afk` | Toggle AFK mode |
| `/skill:<name>` | Invoke a skill |
| `/agent:<name>` | Switch to named agent |
| `/session:<name>` | Switch to named session |
| `/eval` | Run evaluation suite |
| `/report` | Generate report from current session |
| `/vars` | Show variable store |
| `/help` | Show help |

## Appendix C: URL Schemes

| Scheme | Purpose | Example |
|--------|---------|---------|
| `skill://` | Skill instructions | `skill://rust-refactor` |
| `rule://` | Project rules | `rule://no-panics` |
| `memory://` | Session memory | `memory://last-compaction` |
| `agent://` | Agent output | `agent://ALPHA/output` |
| `artifact://` | Tool artifacts | `artifact://bash-123` |
| `local://` | Shared content | `local://plan.md` |
| `doc://` | Documents/reports | `doc://architecture-v1` |
| `eval://` | Eval results | `eval://nightly-2026-06-07` |
