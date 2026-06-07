# Arconaut Comprehensive Code Review

**Date:** 2026-06-07  
**Lines reviewed:** ~12,749  
**Test count:** 103 (all passing)  
**Clippy:** Clean

---

## Executive Summary

The codebase is well-structured, thoroughly tested, and follows Rust idioms. The crate separation (core → machine → agent → tui → cli) is clean. No critical security vulnerabilities or data-loss bugs were found. The main categories of findings are:

1. **Anthropic API message conversion bug** — tool results may be sent as empty messages
2. **Chat scroll direction inverted** in TUI
3. **Several async/blocking mixups** in tools
4. **Missing wiring** — Bus, Inbox, Dispatcher exist but aren't connected to the runtime
5. **Soul vs Agent mismatch** — Soul is agent-agnostic; CLI doesn't configure it per-agent

---

## 🔴 Critical / High Priority

### 1. AnthropicMessage drops ToolResult content (`arconaut-machine/src/anthropic.rs`)

**Problem:** The `From<Message> for AnthropicMessage` implementation only extracts `ContentPart::Text`:

```rust
let content = msg.content.into_iter()
    .filter_map(|part| match part {
        ContentPart::Text { text } => Some(text),
        _ => None,
    })
    .collect::<Vec<_>>()
    .join("");
```

When a `Message::tool_result()` is converted, the `ContentPart::ToolResult` is dropped, producing an empty `"user"` message. The Anthropic API receives empty content after tool execution.

**Impact:** Tool results with non-text content (or any content wrapped in `ToolResult`) become invisible to the model.

**Fix:** Handle `ToolResult` in the conversion:
```rust
ContentPart::ToolResult { tool_result } => {
    let text = tool_result.output.iter()
        .filter_map(|p| p.as_text())
        .collect::<Vec<_>>()
        .join("");
    Some(text)
}
```

---

### 2. TUI chat scroll is inverted (`arconaut-tui/src/app.rs`)

**Problem:** Pressing Up increases `scroll_offset`, and `render_chat_pane` skips that many lines from the *start* of the message list. Since messages are rendered oldest-first, increasing skip shows *newer* messages. This is backwards.

```rust
// Current (wrong)
KeyCode::Up => self.scroll_offset = self.scroll_offset.saturating_add(1),
KeyCode::Down => self.scroll_offset = self.scroll_offset.saturating_sub(1),
```

**Fix:** Swap the directions, or implement bottom-anchored scrolling.

---

## 🟡 Medium Priority

### 3. BashTool safety check is incomplete (`arconaut-machine/src/tools.rs`)

Rejects `&&`, `;`, `|`, `` ` ``, `$()` but not:
- `||` (OR command chaining)
- `<( )` (process substitution)
- Newline-separated multi-line commands (bash `-c` executes all lines)

**Fix:** Also reject `||` and newlines.

---

### 4. GrepTool blocks the async runtime (`arconaut-machine/src/tools.rs`)

`GrepTool::walk_dir` uses `std::fs::read_dir` and `std::fs::read_to_string` inside an `async fn`. For large directories this blocks the async executor.

**Fix:** Use `tokio::fs` or wrap in `tokio::task::spawn_blocking`.

---

### 5. CompositeInjector only injects one system prompt (`arconaut-agent/src/injection.rs`)

`SystemPromptInjector` checks if a system message exists at head and skips if so. When composed, the second injector sees the first's system message and skips. This makes `CompositeInjector` with multiple `SystemPromptInjector`s effectively useless.

**Fix:** Either allow multiple system prompts, or have `CompositeInjector` merge them.

---

### 6. CompactionEngine destroys checkpoints (`arconaut-agent/src/compaction.rs`)

`compact()` calls `context.clear()`, which wipes all checkpoints. If the user reverts after compaction, all checkpoint IDs are invalidated.

**Fix:** Preserve checkpoints in `compact()`, or document the behavior.

---

### 7. TerminalSendTool has a racy 100ms sleep (`arconaut-agent/src/persistent_shell.rs`)

```rust
tokio::time::sleep(tokio::time::Duration::from_millis(100)).await;
let output = shell.take_buffer();
```

Slow commands (e.g., `cargo build`) may not produce output in 100ms.

**Fix:** Use a prompt-based approach or configurable timeout with incremental output.

---

### 8. InboxServer leaks subscriber channels (`arconaut-agent/src/inbox.rs`)

`stream_messages` inserts a new sender into `subscribers` on every call. If an agent reconnects, the old sender remains in the HashMap forever.

**Fix:** Remove old subscriber on new connection, or use weak references.

---

## 🟢 Low Priority / Nits

### 9. `estimate_tokens` is very rough (`arconaut-core/src/context.rs`)

`text_len / 4` works for ASCII but underestimates for CJK text (where tokens are ~1-2 chars). Consider using a proper tokenizer or `tiktoken-rs`.

---

### 10. `GrepTool` gitignore matching is naive (`arconaut-machine/src/tools.rs`)

String containment: `target/` matches `not-target/foo`. Should use `glob` or `ignore` crate.

---

### 11. `SkillLoader` frontmatter parser is fragile (`arconaut-machine/src/skills.rs`)

Won't handle quoted YAML values, multiline strings, or indented keys.

**Fix:** Use a real YAML parser (`serde_yaml`) or document the restriction.

---

### 12. `DocumentIndex::scan_dir` doesn't recurse (`arconaut-core/src/docs.rs`)

Only scans top-level files. The recursive version exists in `scan_dir_recursive` but isn't used.

---

### 13. `PdfGenerator` is single-page (`arconaut-core/src/pdf.rs`)

Long documents truncate at `y < 50.0`. No multi-page support.

---

### 14. `run_single_turn` is a stripped-down Soul (`arconaut-cli/src/main.rs`)

No compaction, injection, hooks, skills, or terminal bridge. The non-TUI mode is significantly less capable than TUI mode.

---

## Architecture Observations

### A. Soul doesn't know about Agents

`Soul` owns provider + registry + context, but has no concept of `Agent` name, mode, or session. The CLI parses `--agent` and `--mode` but only uses them for the prompt and doesn't configure the Soul differently.

**Suggestion:** Add agent context to Soul (system prompt based on mode, agent name in status).

### B. Bus / Inbox / Dispatcher are islands

All three exist and are tested, but none are:
- Started by the CLI
- Used by the Soul
- Connected to each other

**Suggestion:** Wire the Bus into the Soul so agents can whisper each other during turns. Start the gRPC inbox server alongside the TUI.

### C. No embedding integration

The user has a `neurotic_library` with SQLite+vec hybrid search for research corpora. Arconaut has no way to query it.

**Suggestion:** Add a `CorpusQueryTool` that shells out to `python scripts/wave_corpus/unified_query.py` or implements the query in Rust via `rusqlite` with the `sqlite-vec` extension.

---

## Test Coverage

| Crate | Tests | Gaps |
|-------|-------|------|
| arconaut-core | 14 | No `Context` overflow test, no `DocumentIndex` recursion test |
| arconaut-machine | 17 | No actual HTTP provider test (needs mock server), no `SkillTool` integration test |
| arconaut-agent | 33 | No `Soul` + `Bus` integration test, no `InboxClient`/`Server` full lifecycle test |
| arconaut-tui | 4 | Only Ghostty sequences; no widget rendering tests |
| arconaut-cli | 8 | No CLI arg parsing tests, no end-to-end test |

---

## Recommended Priority Order

1. Fix AnthropicMessage ToolResult conversion (critical correctness)
2. Fix TUI chat scroll direction (user-facing bug)
3. Wire Bus + Inbox into the runtime (finish Phase 4)
4. Add CorpusQueryTool for neurotic_library integration
5. Fix BashTool safety, GrepTool blocking, CompositeInjector behavior
6. Add agent-mode system prompts to Soul
7. Improve `run_single_turn` parity with TUI
