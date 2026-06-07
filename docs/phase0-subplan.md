# Phase 0 Sub-Plan: Workspace Setup and Core Types

**Issue:** arconaut-6s8  
**Status:** In Progress  
**Date:** 2026-06-07  

## What We're Building

The foundation everything else stands on. A Cargo workspace with 8 crates, core domain types, an LLM provider abstraction, a basic context manager, and a simple REPL that can hold a conversation with Claude.

## Why This Order

The Soul (Phase 1) needs types to operate on. The TUI (Phase 2) needs a Soul to render. Scaffolding (Phase 3) needs the type system to extend. Multi-agent (Phase 4) needs everything below it.

## Sub-Tasks

### 0.1 Workspace Skeleton
- Create `Cargo.toml` workspace root with 8 member crates
- Create each crate's `Cargo.toml` with minimal dependencies
- Set up shared workspace metadata (edition, rust-version, authors)
- Verify `cargo check --workspace` compiles empty crates

### 0.2 Core Types (`arconaut-core`)
Define the fundamental data structures:
- `Message` — role, content parts, timestamp, metadata
- `ContentPart` — text, image, tool_call, tool_result
- `Tool` trait — name, description, parameters schema, async call
- `ToolResult` — success/failure with typed output
- `ToolCall` — id, function name, arguments
- `Role` — user, assistant, system, tool

### 0.3 LLM Provider Abstraction (`arconaut-machine`)
- `ChatProvider` trait with async chat method
- `ChatRequest` / `ChatResponse` types
- `ModelCapability` enum (text, images, tool_use, thinking)
- Anthropic provider implementation using `reqwest` + `serde`
- Provider error classification (rate_limit, auth, context_overflow, etc.)

### 0.4 Context Manager (`arconaut-core`)
- `Context` struct: history vec, token count, max tokens
- `append_message`, `clear`, `token_count` methods
- Checkpoint mechanism (save/restore state snapshot)
- Simple token estimation (4 chars ≈ 1 token heuristic)

### 0.5 Simple REPL (`arconaut-cli`)
- Read user input from stdin
- Send to LLM via provider
- Print response
- Basic `/quit` command
- No TUI — plain text IO

## What We're NOT Building

- No compaction (Phase 1)
- No tool registry beyond trait definition (Phase 1)
- No deduplication (Phase 1)
- No hooks (Phase 1)
- No TUI (Phase 2)
- No skills (Phase 3)
- No multi-agent (Phase 4)
- No audit log (Phase 1 infrastructure, formalized in Phase 4)

## Dependencies

| Crate | External Deps | Internal Deps |
|-------|--------------|---------------|
| `arconaut-core` | `chrono`, `serde`, `serde_json` | — |
| `arconaut-machine` | `reqwest`, `tokio`, `async-trait`, `serde`, `serde_json` | `arconaut-core` |
| `arconaut-tui` | `ratatui`, `crossterm` | `arconaut-core`, `arconaut-machine` |
| `arconaut-agent` | `tokio`, `tonic` | `arconaut-core`, `arconaut-machine` |
| `arconaut-audit` | `serde_jsonl` | `arconaut-core` |
| `arconaut-eval` | — | `arconaut-core`, `arconaut-machine` |
| `arconaut-corpus` | — | `arconaut-core` |
| `arconaut-cli` | `clap`, `tokio` | all above |

## Risk: What Could Go Wrong

1. **Dependency version conflicts** — ratatui and crossterm versions must align. Mitigation: pin to specific versions in workspace root.
2. **LLM API changes** — Anthropic's API is stable but beta features move. Mitigation: abstract behind trait, only use GA features.
3. **Token estimation accuracy** — 4-char heuristic is wrong for non-English and code. Mitigation: use `tiktoken-rs` or `tokenizers` crate for real counting in Phase 1.
4. **Async complexity** — tokio runtime setup across crates. Mitigation: single runtime in `arconaut-cli`, everything else is `async` but runtime-agnostic.

## Completion Criteria

- `cargo check --workspace` passes with zero warnings
- `cargo test --workspace` passes (tests exist and pass)
- REPL can hold a 3-turn conversation with Claude
- All types are `Serialize` + `Deserialize` for future persistence
