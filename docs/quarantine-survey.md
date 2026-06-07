# Quarantine Survey: AI Coding Agent Architectures

## Executive Summary

24 repos surveyed across 5 categories. The landscape splits into three tiers:

1. **Production vendor CLIs** (codex, gemini-cli, kimi-cli, qwen-code) — hardened, sandboxed, multi-modal
2. **Research/agentic frameworks** (Pi, SWE-agent, Plandex, OpenHands, Devika) — novel loop architectures
3. **IDE/extensions** (Cline, Roo Code, Continue) — human-in-the-loop, diff-review UX

Key architectural axes: language (Rust vs Python vs TS), loop style (ReAct vs streaming vs pipeline),
sandboxing (OS-level vs Docker vs none), TUI framework (ratatui vs Ink vs custom), and extensibility (MCP vs custom).

---

## Category 1: Official Vendor CLI Agents

### OpenAI Codex
- **Language**: Rust (2024 edition), ~100+ crates, Bazel + Cargo
- **TUI**: ratatui — full alternate-screen, markdown, diff, voice (WebRTC)
- **Loop**: Thread-based event loop with embedded app-server (Unix socket / WebSocket)
- **Sandbox**: Seatbelt (macOS) / Landlock + bwrap (Linux) / Restricted token (Windows)
- **Tools**: Shell (zsh fork), file ops, web search, agent jobs, multi-agent spawn/wait
- **Extensibility**: MCP, plugins, skills
- **Key insight**: Enterprise-grade sandboxing maturity. The Rust architecture includes full rollout tracing, state DB, telemetry, multi-agent job orchestration.
- **Key files**: `codex-rs/tui/src/main.rs`, `codex-rs/core/src/codex_thread.rs`, `codex-rs/core/src/session.rs`

### Gemini CLI (Google)
- **Language**: TypeScript/Node, npm workspaces, esbuild
- **TUI**: Ink (React for terminals)
- **Loop**: Scheduler-driven batch execution + Turn streaming
- **Sandbox**: Docker/Podman
- **Tools**: Read/write, edit, ls, grep, glob, shell, web, memory, MCP
- **Extensibility**: MCP, skills, extensions
- **Key insight**: Native Gemini integration — Google Search grounding, 1M token context, thought streaming, auto-memory patches.
- **Key files**: `packages/core/src/scheduler/scheduler.ts`, `packages/core/src/core/turn.ts`

### Kimi CLI (Moonshot)
- **Language**: Python 3.12+, uv workspace, Typer, prompt-toolkit + Rich
- **Loop**: "Soul" architecture — turn → step loop with Flow mode and Plan mode
- **Sandbox**: Shell approval + yolo mode
- **Tools**: Shell, file, web, search, ask-user, subagent spawn, background tasks
- **Extensibility**: MCP, hooks, plugins, skills, subagent "labor market"
- **Key insight**: Multi-provider from ground up via `kosong` abstraction. Unique plan mode (read-only research with persistent plan files). Zsh shell-mode integration.
- **Key files**: `src/kimi_cli/soul/kimisoul.py`, `src/kimi_cli/soul/agent.py`, `src/kimi_cli/llm.py`

### Qwen Code (Alibaba)
- **Language**: TypeScript/Node, npm workspaces, esbuild
- **TUI**: Ink (React)
- **Loop**: Inherited from Gemini CLI + enhanced scheduler with OpenTelemetry spans
- **Sandbox**: Docker/Podman
- **Tools**: File ops, shell, grep, cron, todo, LSP, skill, subagent
- **Extensibility**: MCP, skills, subagents, SDKs (TS/Python/Java), daemon mode, chat channels (Telegram/WeChat/DingTalk/Feishu)
- **Key insight**: Broad provider support + Alibaba Cloud integration. Tree-sitter WASM for code parsing.
- **Key files**: `packages/core/src/core/coreToolScheduler.ts`, `packages/core/src/agents/runtime/agent-interactive.ts`

---

## Category 2: Pi Family

### pi (Original)
- **Language**: TypeScript/Node, ~96k LoC
- **TUI**: Custom differential renderer (not Bubble Tea/ratatui/Ink)
- **Loop**: Event-driven async turn loop with hooks
- **Tools**: 7 built-in (read, write, edit, bash, grep, find, ls)
- **Key insight**: **Lazy skill architecture** — skills listed by metadata only in system prompt; model `read`s full content on demand. Keeps prompt sub-1K tokens regardless of skill corpus size.
- **Key files**: `packages/agent/src/agent-loop.ts`, `packages/agent/src/harness/agent-harness.ts`, `packages/coding-agent/src/core/system-prompt.ts`

### oh-my-pi (can1357 fork)
- **Language**: TypeScript (Bun) + Rust, ~378k TS + ~67k Rust
- **TUI**: Custom differential renderer, 80+ components
- **Loop**: Enhanced Pi loop with telemetry, harmony leak mitigation, intent tracing, append-only context mode
- **Tools**: 32 built-in, in-process hot path (no fork/exec) via ~27k LoC Rust
- **Key insight**: Batteries-included coding surface with native performance. Embedded bash (brush-shell), in-process ripgrep, tree-sitter AST, LSP + DAP, Hindsight memory (retain/recall/reflect), subagents with worktree isolation.
- **Key files**: `packages/agent/src/agent-loop.ts`, `crates/pi-natives/src/lib.rs`, `packages/coding-agent/src/edit/hashline/`

### pi-agent-rust (Zero-unsafe port)
- **Language**: Pure Rust, 2024 edition, `#![forbid(unsafe_code)]`, ~368k LoC
- **TUI**: `charmed_rust` (Bubble Tea architecture)
- **Loop**: Structured concurrency with `asupersync` runtime, `ToolEffects` bitflags for parallel scheduling
- **Tools**: 8 built-in, process-tree cleanup via `sysinfo`
- **Key insight**: **~21MB single static binary, <100ms startup**. Capability-gated QuickJS extensions with deterministic hostcall reactor mesh, two-stage exec enforcement, tamper-evident risk ledgers.
- **Key files**: `src/agent.rs`, `src/tools.rs`, `src/extensions.rs`, `src/interactive.rs`

---

## Category 3: Plan/Execute Agents

### Plandex
- **Language**: Go 1.23, gorilla/mux, Cobra CLI
- **Loop**: Two-stage (tell → build). Tell: stream LLM response, parse into file ops + bash. Build: queue per-path, execute structured edits.
- **Key insight**: **Cumulative diff review sandbox** — AI changes accumulate in a plan branch separate from working tree. Review, revise, reject individual files. Plan version control (branches, rewinding). Tree-sitter project maps.
- **Key files**: `app/server/model/plan/tell_exec.go`, `app/server/model/plan/build_exec.go`

### OpenHands
- **Language**: Python/FastAPI + React SPA
- **Loop**: External SDK (`openhands-sdk`). This repo is orchestration layer managing sandboxed conversations.
- **Sandbox**: Docker, local tmux, or remote workspace
- **Key insight**: Enterprise multi-user cloud deployment with RBAC, event callbacks (Slack/Jira/Linear), secret management, Microagents (triggered markdown prompts).
- **Key files**: `openhands/app_server/app_conversation/app_conversation_service.py`

### SWE-agent
- **Language**: Python, Pydantic v2, Jinja2, SWE-ReX sandbox
- **Loop**: ReAct-style: setup → step (query LLM → parse thought+action → execute → observe) → repeat
- **Tools**: YAML-configurable commands with pluggable parsers (thought_action, function_calling, XML, JSON, bash blocks)
- **Key insight**: **Agent-Computer Interface (ACI)** — designing tool interfaces that are easy for LMs to use. History processors for context window management. Retry loop with reviewers.
- **Key files**: `sweagent/agent/agents.py`, `sweagent/tools/tools.py`, `sweagent/tools/parsing.py`

### mini-swe-agent
- **Language**: Python, ~100 lines core logic
- **Loop**: `while True: query() → execute_actions()`
- **Tools**: **Bash only**. Stateless `subprocess.run` — no persistent shell.
- **Key insight**: Radical minimalism. With capable LMs, complex tool scaffolds are unnecessary. Bash-only + linear message history = >74% SWE-bench Verified. Positioned as baseline/hackable tool.
- **Key files**: `src/minisweagent/agents/default.py`, `src/minisweagent/config/default.yaml`

---

## Category 4: IDE/Extension Hybrids

### Cline
- **Language**: TypeScript/Bun, SDK monorepo
- **Loop**: Event-driven iterative loop with hooks (`beforeRun`, `beforeModel`, `beforeTool`, `afterTool`, `afterRun`)
- **Key insight**: **Multi-product ecosystem** — CLI (headless + interactive), Kanban (web-based parallel agent orchestration), scheduled agents (cron), multi-agent teams with coordinator/delegate. Shared SDK for custom agents.
- **Key files**: `sdk/packages/agents/src/agent-runtime.ts`, `sdk/packages/core/src/runtime/tools/subprocess-sandbox.ts`

### Roo Code
- **Language**: TypeScript/pnpm, Turbo monorepo
- **Loop**: Recursive LLM-request loop (`recursivelyMakeClineRequests`, ~4.6K lines)
- **Key insight**: **Modes architecture** — Code, Architect, Ask, Debug, Custom. Each mode changes system prompt + available tools. Git-based checkpoints for every change. Context condensing when window exceeded.
- **Key files**: `src/core/task/Task.ts`, `src/core/tools/BaseTool.ts`, `src/services/mcp/McpHub.ts`

### Continue
- **Language**: TypeScript/npm
- **Loop**: Message-handler based (not recursive). User-driven chat; tools via slash commands.
- **Key insight**: **Privacy-first, source-controlled AI checks in CI**. `.continue/checks/` — markdown-defined agents running as GitHub status checks. 40+ providers. Enterprise on-prem.
- **Key files**: `core/core.ts`, `core/llm/index.ts`, `core/tools/callTool.ts`

### Tabby
- **Language**: Rust + TypeScript
- **Not an agent**: Self-hosted completion server + RAG-based Answer Engine. No recursive tool loop.
- **Key insight**: Single Rust binary, no DBMS dependency, on-premises. Tantivy indexing + LSP snippets for context-aware completion.
- **Key files**: `crates/tabby/src/serve.rs`, `ee/tabby-webserver/src/service/answer.rs`

---

## Category 5: Other Notable Agents

### Goose (Block → Linux Foundation)
- **Language**: Rust, ~20 crates, Tokio, `rmcp` MCP SDK
- **Loop**: Streaming reply with `ReplyContext`, compaction at threshold, max 1,000 turns
- **Key insight**: **MCP-first architecture** — everything is an MCP extension. 70+ extensions. "Recipes" for preconfigured workflows.
- **Key files**: `crates/goose/src/agents/agent.rs`, `crates/goose/src/agents/extension_manager.rs`

### OpenCode
- **Language**: TypeScript/Bun, **Effect.ts** functional architecture
- **Loop**: Effect-based streaming processor with `updateToolCall`/`completeToolCall`, doom loop detection
- **Key insight**: Heavy use of typed effects, layered services, Zod schemas. 75+ providers via Vercel AI SDK. LSP integration. Built-in web search.
- **Key files**: `packages/opencode/src/session/processor.ts`, `packages/opencode/src/tool/registry.ts`

### OpenDev
- **Language**: Rust, 2024 edition, ratatui TUI
- **Loop**: ReAct loop with parallel read-only tools, doom-loop detection
- **Key insight**: **Compound AI system** — 5 independent workflow slots (Normal, Thinking, Compact, Critique, VLM), each bound to different model/provider. **4.3ms startup, 9.4MB RAM, 18MB binary.**
- **Key files**: `crates/opendev-agents/src/react_loop/mod.rs`, `crates/opendev-agents/src/main_agent.rs`

### Trae Agent (ByteDance)
- **Language**: Python, Rich TUI
- **Loop**: Simple step loop with trajectory recording
- **Key insight**: Research-friendly — every interaction recorded to JSON. "Lakeview" summarization. Docker sandbox.
- **Key files**: `trae_agent/agent/base_agent.py`, `trae_agent/utils/trajectory_recorder.py`

### Claude Code (unofficial leak)
- **Language**: TypeScript/Bun, React + Ink TUI, ~512K lines
- **Loop**: `QueryEngine` streaming tool loop (~46K lines) with permission checks, sub-agent spawning, team coordination
- **Key insight**: Production-grade React+Ink terminal UI. ~40 typed tools. Multi-agent swarms. Skill system. Permission modes (default/plan/bypass/auto).
- **Key files**: `src/QueryEngine.ts`, `src/Tool.ts`, `src/tools.ts`, `src/commands.ts`

### GPT-Engineer (OG)
- **Language**: Python, LangChain
- **Loop**: Pipeline steps (gen → entrypoint → exec → improve)
- **Key insight**: Preprompt-driven greenfield generation from single prompt file. Benchmarking infrastructure.
- **Key files**: `gpt_engineer/core/default/steps.py`, `gpt_engineer/core/ai.py`

### MetaGPT
- **Language**: Python, asyncio
- **Loop**: Role _observe → _think → _act cycle
- **Key insight**: Software company simulation — PM, Architect, Engineer, QA roles with SOPs. `Code = SOP(Team)`.
- **Key files**: `metagpt/roles/role.py`, `metagpt/team.py`

### Devika
- **Language**: Python/Flask, Playwright
- **Loop**: Pipeline orchestrator (Decision → Planner → Researcher → Coder → Runner → Patcher)
- **Key insight**: Devin-style autonomous agent with browser integration and multi-step planning.
- **Key files**: `src/agents/agent.py`, `src/agents/planner/planner.py`

### Sweep
- **Language**: Python, FastAPI
- **Loop**: Issue → plan → PR pipeline
- **Key insight**: GitHub-native PR automation. XML-tagged prompts, regex parsing. Lexical search + vector DB.
- **Key files**: `sweepai/handlers/on_ticket.py`, `sweepai/core/sweep_bot.py`

---

## Cross-Cutting Architectural Patterns

### 1. The Three-Layer Stack
From rocket_surgeon TUI research and confirmed in practice:
- **Layer 1: Core Engine** — state transformation, no UI (Codex's `codex-rs/core`, Pi's `packages/agent`)
- **Layer 2: Machine Interface** — structured protocol, primary for LLM access (Codex protocol, Pi RPC, Kimi ACP)
- **Layer 3: TUI** — human interface, consumer of layer 2 (ratatui, Ink, custom differential)

### 2. Loop Architectures
| Pattern | Repos | When to use |
|---|---|---|
| **ReAct** | SWE-agent, OpenDev, Trae | Research, explicit reasoning steps |
| **Streaming event-driven** | Codex, Cline, Pi, Gemini, Qwen | Production, real-time UX |
| **Recursive turn** | Roo Code, Kimi Soul | Complex multi-step tasks with persistence |
| **Pipeline** | GPT-Engineer, Devika, Sweep | Deterministic workflows |
| **Message-handler** | Continue, OpenHands | Assisted/user-driven coding |
| **Minimal step** | mini-swe-agent | Baseline, hackable, proof-of-concept |

### 3. Tool System Patterns
| Pattern | Repos | Tradeoff |
|---|---|---|
| **MCP-first** | Goose, Codex, Gemini, Qwen, Cline, Roo | Standardized, discoverable, ecosystem |
| **Built-in + registry** | Pi, Kimi, SWE-agent, OpenDev | Tight integration, optimized UX |
| **Bash-only** | mini-swe-agent | Minimalism, maximum model agency |
| **YAML-configurable** | SWE-agent | Research flexibility |
| **Preprompt-driven** | GPT-Engineer | Simplicity, no schema overhead |

### 4. Sandboxing Strategies
| Strategy | Repos | Cost |
|---|---|---|
| **OS-level** (Seatbelt/Landlock) | Codex | High implementation cost, maximum security |
| **Container** (Docker/Podman) | Gemini, Qwen, OpenHands, Trae | Medium cost, standard isolation |
| **Process + approval** | Kimi, Pi, Cline | Low cost, user-mediated |
| **None** | mini-swe-agent, GPT-Engineer | Zero cost, trust model |

### 5. TUI Framework Choices
| Framework | Repos | Pros | Cons |
|---|---|---|---|
| **ratatui** (Rust) | Codex, OpenDev | Immediate-mode, <1ms/frame, zero-cost | Rust learning curve |
| **Ink** (React+TS) | Gemini, Qwen, Claude Code | Familiar React patterns, component reuse | Heavier, Node dependency |
| **Custom differential** | Pi, oh-my-pi | Optimized for agent output, minimal redraw | Maintenance burden |
| **Bubble Tea** | pi-agent-rust | Elm architecture, Go ecosystem | Less mature than ratatui |
| **prompt-toolkit + Rich** | Kimi | Python ecosystem, powerful | Slower, heavier deps |

---

## Strategic Observations for arconaut

### What works
1. **Rust + ratatui** is the performance ceiling for terminal agents (Codex, OpenDev, pi-agent-rust)
2. **Lazy skills** (Pi) keep prompt size constant — critical for context window efficiency
3. **MCP-first** (Goose) creates ecosystem network effects
4. **Bash-only baseline** (mini-swe-agent) proves complex tool systems aren't always necessary
5. **Three-layer architecture** (core → protocol → TUI) enables both human and LLM consumers
6. **In-process hot path** (oh-my-pi) eliminates fork/exec latency for search/shell

### What to avoid
1. **Python for the TUI** — Kimi's prompt-toolkit is the slowest startup in the vendor tier
2. **Monolithic TypeScript** — Claude Code at 512K lines shows the complexity ceiling
3. **External SDK dependencies** — OpenHands splitting agent loop into PyPI packages creates friction
4. **Over-scaffolding** — mini-swe-agent proves 100 lines can achieve >74% SWE-bench

### Open questions
1. Can we combine Pi's lazy skills with Codex's sandboxing and OpenDev's per-workflow model binding?
2. Is there a sweet spot between mini-swe-agent's minimalism and oh-my-pi's batteries-included approach?
3. How does the TUI research from rocket_surgeon (terminal graphics, input protocols) integrate with agent loop design?
