# TUI Intermission — Quarantine Reference Index

Date: 2026-05-19

## Purpose

Reference library of TUI implementations for studying advanced techniques relevant
to rocket_surgeon's dual-interface (TUI + structured protocol) debugger. These repos
are shallow clones (`--depth 1`) in `quarantine/` (gitignored).

---

## Newly Cloned Repos (this session)

### 1. BugStalker — `quarantine/BugStalker` (12 MB)

- **URL:** https://github.com/godzie44/BugStalker
- **What:** Modern Rust debugger for Linux x86-64 with TUI, built on ratatui
- **Why it matters:** The single most directly relevant reference. A real debugger
  with a TUI — breakpoints, stepping, watch expressions, multi-thread support.
  Uses ratatui for its TUI mode and supports switching between console and TUI at
  runtime. Also implements DAP (Debug Adapter Protocol), which parallels our
  dual-interface goal.
- **Study targets:** TUI/console mode switching, debugger state rendering, DAP
  integration, how they layout source/disassembly/watch/stack panes

### 2. bottom — `quarantine/bottom` (9.4 MB)

- **URL:** https://github.com/ClementTsang/bottom
- **What:** Cross-platform system/process monitor with customizable widget layout
- **Why it matters:** Best-in-class ratatui charting. Sparklines, time-series
  graphs, CPU/GPU temperature plots, configurable widget placement. Shows how to
  build a dense, information-rich dashboard with real-time data.
- **Study targets:** Chart/sparkline widgets, customizable layout system (user
  config-driven widget placement), real-time data rendering, GPU temp monitoring

### 3. nnd — `quarantine/nnd` (3.5 MB)

- **URL:** https://github.com/al13n321/nnd
- **What:** From-scratch TUI debugger for Linux (not GDB/LLDB-based), written in Rust
- **Why it matters:** Built from scratch rather than wrapping GDB — exactly our
  approach. Single 6 MB binary, no dependencies. Implements its own DWARF parsing,
  process control, TUI rendering. The architecture of a ground-up debugger is
  invaluable reference.
- **Study targets:** From-scratch debugger architecture, DWARF handling, process
  control, how a non-wrapper debugger structures its TUI, pretty-printer system for
  Rust and C++ types

### 4. helix — `quarantine/helix` (16 MB)

- **URL:** https://github.com/helix-editor/helix
- **What:** Post-modern modal text editor written in Rust
- **Why it matters:** Sophisticated compositor/event system architecture. Layers are
  managed by a Compositor that renders Components in stack order — popups over
  editors, file pickers over popups. This is the pattern we need for overlaying
  surgical intervention dialogs over the debugger view.
- **Study targets:** `helix-term/src/ui/` compositor, Component trait, event
  dispatch system, how layers/popups/overlays compose, LSP integration architecture

### 5. zellij — `quarantine/zellij` (56 MB)

- **URL:** https://github.com/zellij-org/zellij
- **What:** Terminal workspace/multiplexer with WASM plugin system
- **Why it matters:** Plugin architecture via WebAssembly — plugins in any language
  that compiles to WASM, sandboxed, with defined APIs. Also demonstrates floating
  and stacked panes, complex multi-pane layout management, and true multiplayer
  collaboration. The WASM plugin model is relevant if we consider extensibility.
- **Study targets:** WASM plugin system, pane layout engine (floating, stacked,
  tiled), IPC between panes, multi-session architecture

### 6. yazi — `quarantine/yazi` (7.5 MB)

- **URL:** https://github.com/sxyazi/yazi
- **What:** Blazing fast file manager with async I/O and image previews
- **Why it matters:** Best example of async I/O architecture in a TUI — all file
  ops run on background threads, UI never freezes. Also demonstrates Sixel/Kitty
  graphics protocol integration for inline image previews. Lua plugin system for
  extensibility.
- **Study targets:** Async I/O architecture (non-blocking everything), Sixel/Kitty
  graphics protocol usage, three-pane layout, Lua plugin system, preview system

### 7. gitui — `quarantine/gitui` (71 MB)

- **URL:** https://github.com/gitui-org/gitui
- **What:** Fast terminal UI for git, written in Rust with ratatui
- **Why it matters:** The `asyncgit` crate is a masterclass in putting long-running
  operations on a thread pool with crossbeam-channel notification. Five-panel
  interactive layout with keyboard-driven navigation. Shows how to keep TUI
  responsive while background operations complete.
- **Study targets:** `asyncgit/` crate architecture (thread pool + channel
  notification), component architecture, keyboard navigation system, diff rendering

### 8. csvlens — `quarantine/csvlens` (36 MB)

- **URL:** https://github.com/YS-L/csvlens
- **What:** Terminal CSV viewer (`less` for CSV files), built with ratatui
- **Why it matters:** Data exploration TUI — relevant to how we display tensor data,
  activation tables, weight matrices. Shows tabular data rendering with search,
  regex filtering, column selection, scrolling through large datasets.
- **Study targets:** Table/grid rendering at scale, search/filter UX, large dataset
  scrolling, column-aware navigation

### 9. nviwatch — `quarantine/nviwatch` (2.5 MB)

- **URL:** https://github.com/msminhas93/nviwatch
- **What:** NVIDIA GPU monitoring TUI in Rust
- **Why it matters:** Directly relevant to multi-GPU monitoring. Shows how to poll
  GPU metrics (temperature, utilization, memory, power) via NVML and render them in
  a TUI. Lowest memory footprint among GPU monitoring tools (~18 MB). Process
  management on GPU from TUI.
- **Study targets:** NVML integration in Rust, GPU metric polling, multi-GPU
  display layout, process management UX

### 10. ratatui-image — `quarantine/ratatui-image` (21 MB)

- **URL:** https://github.com/ratatui/ratatui-image
- **What:** Ratatui widget for rendering images via Sixel/Kitty/iTerm2 protocols
- **Why it matters:** If we want to render tensor heatmaps, attention matrices, or
  activation visualizations as actual images in the TUI, this is the widget that
  makes it possible. Handles protocol detection, fallback to halfblocks, and
  stateless (immediate-mode) rendering.
- **Study targets:** Graphics protocol negotiation (Sixel vs Kitty vs iTerm2 vs
  halfblock fallback), image widget architecture, how to integrate pixel graphics
  into a ratatui layout without blocking

---

## Previously in Quarantine (relevant to TUI work)

| Repo | Relevance |
|------|-----------|
| `ratatui` | The TUI framework itself — primary dependency |
| `textual` | Python TUI framework — alternative design philosophy reference |
| `py-spy` | Python profiler — flame graph visualization patterns |
| `trippy` | Network diagnostic TUI in Rust with ratatui — chart/map rendering |
| `taskwarrior-tui` | Task management TUI — ratatui app architecture |
| `rr` | Record-replay debugger — not TUI but debugger architecture |
| `scalene` | Python profiler — profiler UX patterns |
| `treescope` | ML tensor visualization — data display patterns |
| `transformer-debugger` | OpenAI's transformer debugger — debugger UX for ML |
| `perfetto` | Chrome trace format — trace visualization architecture |
| `debug-adapter-protocol` | DAP spec — protocol reference for IDE integration |

---

## Considered but NOT Cloned

### lazygit (Go)
- **URL:** https://github.com/jesseduffield/lazygit
- **Why not:** Written in Go (not Rust), and gitui already covers the "git TUI"
  pattern in Rust with ratatui. 63k stars but the Go codebase is less transferable.
  Could revisit for UX study only.

### ugdb (Rust)
- **URL:** https://github.com/ftilde/ugdb
- **Why not:** GDB wrapper approach — wraps GDB via its machine interface. We're
  building from scratch (closer to nnd's approach). Also uses `unsegen` rather than
  ratatui, limiting code transferability.

### inferno / flamegraph-rs
- **URL:** https://github.com/jonhoo/inferno
- **Why not:** Generates SVG flamegraphs, not terminal-based visualization. We
  already have py-spy for flame graph concepts. If we need Rust flamegraph
  generation, this is a library dependency candidate, not a TUI reference.

### samply
- **URL:** https://github.com/mstange/samply
- **Why not:** Uses Firefox Profiler as UI (opens browser), not a TUI. Interesting
  profiler architecture but not relevant to terminal rendering.

### nvtop / nvitop
- **URL:** https://github.com/Syllo/nvtop (C), https://github.com/XuehaiPan/nvitop (Python)
- **Why not:** nvtop is C/ncurses, nvitop is Python. nviwatch covers the same
  domain in Rust with ratatui, which is directly transferable.

### grafatui (Rust)
- **URL:** https://lib.rs/crates/grafatui
- **Why not:** Prometheus-specific visualization. Too narrow a use case — bottom
  already covers time-series charting better and more generally.

### tensorwatch (Microsoft)
- **URL:** https://github.com/microsoft/tensorwatch
- **Why not:** Python, Jupyter-based, not terminal TUI. We already have treescope
  for ML visualization patterns.

### r3bl_tui (Rust)
- **URL:** https://docs.rs/r3bl_tui
- **Why not:** Alternative TUI framework (not ratatui). Interesting composability
  model but we're committed to ratatui. Could study the docs for design ideas
  without cloning.

### tui-widgets (ratatui org)
- **URL:** https://github.com/ratatui/tui-widgets
- **Why not:** Collection of standalone widgets — useful but better consumed as
  crate dependencies when needed rather than reference code. Already have ratatui
  source for core widget study.

---

## Total Disk Impact

New clones: ~234 MB (shallow, `--depth 1`)

## Study Priority Order

For the TUI intermission, suggested study order based on relevance to rocket_surgeon:

1. **BugStalker** + **nnd** — Debugger TUI architecture (our core problem)
2. **helix** — Compositor/event/layer system (our rendering architecture)
3. **bottom** — Dashboard charting and real-time data (our data display)
4. **nviwatch** — GPU monitoring integration (our multi-GPU endgame)
5. **ratatui-image** — Pixel graphics in terminal (tensor visualization)
6. **yazi** — Async I/O patterns (our worker architecture)
7. **gitui** — Async operations + ratatui app patterns
8. **zellij** — Plugin system and pane layout (extensibility model)
9. **csvlens** — Tabular data exploration (tensor/activation tables)
