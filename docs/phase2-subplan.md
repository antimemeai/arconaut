# Phase 2 Sub-Plan: TUI

**Issue:** arconaut-wr9

---

## Sub-Task 1: Message Passing Architecture

**What:** mpsc channel bridge between Soul task and TUI task.

**Deliverables:**
- `SoulCommand` enum (UserInput, TerminalInput, SlashCommand)
- `TuiEvent` enum (NewMessage, TokenStream, ToolCallStarted, ToolCallFinished, MetricsUpdate, TerminalOutput)
- `App` struct in arconaut-cli that spawns both tasks and bridges channels

**Tests:**
- Commands round-trip through channel
- Events serialize/deserialize correctly

---

## Sub-Task 2: Persistent Shell

**What:** Spawned bash process with piped stdio. Background task reads output.

**Deliverables:**
- `PersistentShell` struct in arconaut-agent
- Spawns `tokio::process::Command::new("bash")` with piped stdin/stdout/stderr
- Background task drains stdout/stderr, forwards via mpsc
- `terminal_send` tool writes to stdin
- Output appended to Soul context as `[TERM] ...` system messages

**Tests:**
- Shell starts and accepts input
- Output is captured and forwarded
- State persists between sends (cd test)

---

## Sub-Task 3: TUI Event Loop

**What:** ratatui + crossterm event loop running in a tokio task.

**Deliverables:**
- `TuiApp` struct managing ratatui terminal state
- Crossterm event polling (keyboard, resize, mouse)
- 60 FPS render loop with frame time budgeting
- Graceful shutdown on Ctrl+C or /quit

**Tests:**
- Event parsing (key presses, resize)
- Frame render produces valid ratatui `Frame`

---

## Sub-Task 4: Widgets

**What:** All UI components.

**Deliverables:**
- `HeaderWidget` — agent name, model, session clock
- `ChatPane` — scrollable message list with timestamps, role icons
- `TerminalPane` — vt100 emulator rendering live shell output
- `InputLine` — sticky bottom input with focus indicator
- `ContextBar` — token ratio, plan mode, yolo mode
- `MetricsPanel` — step count, latency, token usage

**Tests:**
- Each widget renders without panic on empty state
- ChatPane renders messages with correct alignment
- TerminalPane renders vt100 screen state

---

## Sub-Task 5: Layout & Interaction

**What:** Dual-pane layout with adjustable split and focus switching.

**Deliverables:**
- Vertical split layout (left chat, right terminal)
- Adjustable split via drag or keyboard shortcuts
- `Ctrl+1` / `Ctrl+2` switches pane focus
- Input context-aware prefix (`[chat] >` vs `[term] $`)
- Scroll in each pane independent of input

**Tests:**
- Split ratio changes on resize
- Focus switch updates input prefix

---

## Sub-Task 6: Ghostty Optimizations

**What:** Terminal protocol enhancements for Ghostty.

**Deliverables:**
- Kitty Keyboard Protocol via crosstement flags
- Mode 2031 theme detection (query + parse response)
- OSC 133 semantic zone emission around prompts/output
- Synchronized output mode 2026 around frame renders

**Tests:**
- Mode 2031 query sends correct OSC sequence
- OSC 133 sequences emitted at correct boundaries

---

## Sub-Task 7: Caves of Qud Styling

**What:** Color palette and visual design.

**Deliverables:**
- `Theme` struct with dark/light variants
- Dark palette: warm brown background, amber accents
- Light palette: parchment background, dark gold accents
- Auto-switch on mode 2031 detection
- Border styles, role-based message colors

**Tests:**
- Theme produces valid ratatui `Color` values
- Dark/light themes are distinct

---

## Dependency Changes

**Add to workspace:**
- `ratatui = "0.29"` — TUI framework
- `crossterm = "0.28"` — terminal input/output
- `vt100 = "0.16"` — terminal emulation

**Add to arconaut-tui:**
- `ratatui`, `crossterm`, `vt100`, `arconaut-core`, `arconaut-agent`

**Add to arconaut-cli:**
- `ratatui`, `crossterm`, `arconaut-tui`, `arconaut-agent`

---

## Order of Attack

1. Sub-Task 1 (message passing) + Sub-Task 2 (persistent shell)
2. Sub-Task 3 (event loop) + Sub-Task 7 (styling)
3. Sub-Task 4 (widgets) — ChatPane first, then TerminalPane
4. Sub-Task 5 (layout + interaction)
5. Sub-Task 6 (Ghostty optimizations)
