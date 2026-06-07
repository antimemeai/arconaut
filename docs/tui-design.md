# Phase 2 TUI Design

## Architecture: Message Passing (Option B)

```
┌─────────────────────────────────────────────────────────────┐
│                         CLI Process                         │
│  ┌──────────────┐              ┌─────────────────────────┐  │
│  │   Soul Task  │◄────────────►│        TUI Task         │  │
│  │              │  mpsc::chan  │                         │  │
│  │  run_turn()  │              │  ratatui event loop     │  │
│  │  registry    │              │  crossterm input        │  │
│  │  context     │              │  60 FPS render          │  │
│  └──────────────┘              └─────────────────────────┘  │
│         │                                │                  │
│         ▼                                ▼                  │
│    SoulCommand                      TuiEvent                │
└─────────────────────────────────────────────────────────────┘
```

**SoulCommand** (TUI → Soul):
- `UserInput(String)` — user message to LLM
- `TerminalInput(String)` — raw text to persistent shell
- `SlashCommand(String, Vec<String>)` — `/compact`, `/yolo`, etc.

**TuiEvent** (Soul → TUI):
- `NewMessage(Message)` — assistant response
- `TokenStream(String)` — streaming token (for live rendering)
- `ToolCallStarted(String)` — tool name
- `ToolCallFinished(String, ToolResult)` — tool result
- `MetricsUpdate(MetricsSnapshot)` — token usage, step count
- `TerminalOutput(String)` — shell stdout/stderr

## Layout

```
┌─────────────────────────────────────────────────────────────┐
│  AGENT: alpha    MODEL: claude-sonnet    00:04:12          │  ← Header
├──────────────────────────┬──────────────────────────────────┤
│                          │                                  │
│   Chat Pane (focus=1)    │   Terminal Pane (focus=2)      │
│                          │                                  │
│   > hello                │   $ ls                           │
│   hi there               │   Cargo.toml  src/               │
│   > read src/main.rs     │   $ cargo build                  │
│   [tool: read]           │   Compiling...                   │
│   fn main() { ... }      │   Finished dev [unoptimized]     │
│                          │                                  │
│                          │                                  │
├──────────────────────────┴──────────────────────────────────┤
│  [chat]  > _                                               │  ← Sticky Input
└─────────────────────────────────────────────────────────────┘
```

- **Sticky input** always at bottom. Context-aware: sends to focused pane.
- **Ctrl+1 / Ctrl+2** switches pane focus.
- **Tab** switches pane focus.
- Input prefix shows target: `[chat] >` or `[term] $`

## Persistent Terminal

Instead of `bash` tool spawning a new process per command, the LLM gets a `terminal_send` tool that writes to a **persistent bash session**.

```rust
// arconaut-agent/src/persistent_shell.rs
pub struct PersistentShell {
    stdin: tokio::process::ChildStdin,
    stdout_rx: mpsc::Receiver<String>,
}

impl PersistentShell {
    pub async fn new() -> Self;
    pub async fn send(&mut self, line: &str);
    pub fn output(&self) -> String; // accumulated buffer
}
```

Spawned via `tokio::process::Command::new("bash")` with piped stdio. A tokio task continuously reads stdout/stderr and:
1. Streams to TUI terminal pane via `TuiEvent::TerminalOutput`
2. Appends to Soul context as `Message::system("[TERM] ...")` so the LLM sees it

**Why this is powerful:**
- `cd`, `export`, background jobs persist between tool calls
- LLM sees full terminal history in context
- User can also focus terminal pane and type directly (raw shell access)
- No new dependency — `tokio::process` gives us stdin/stdout/stderr pipes

## Ghostty Optimizations

| Feature | Implementation | Phase 2 |
|---------|---------------|---------|
| Kitty Keyboard Protocol | crossterm `PushKeyboardEnhancementFlags` | ✅ |
| Mode 2031 (theme) | `\x1b]996\x07` query, parse response | ✅ |
| OSC 133 semantic zones | `SemanticZoneEmitter` struct | ✅ |
| Synchronized output | `\x1b[?2026h` / `\x1b[?2026l` around frames | ✅ |
| Kitty graphics | Skip for now (ASCII art only) | ❌ |

## Crate Boundaries

- `arconaut-tui`: ratatui widgets, theme, event handling, layout (library)
- `arconaut-cli`: application — spawns Soul task + TUI task, wires mpsc channels
- `arconaut-agent`: adds `PersistentShell` + `terminal_send` tool

## Dependencies

Re-add to workspace (justified):
- `ratatui` + `crossterm` — TUI framework (Phase 2 core)

No new deps beyond that. `tokio::process` covers the persistent shell.

## Open Questions

1. Should the terminal pane show a **live scrollback** (like a real terminal emulator) or just **captured output lines**?
2. Should `terminal_send` tool take a single line or multi-line input?
3. Do we want **pane resizing** (drag split) or fixed 50/50 split for Phase 2?
