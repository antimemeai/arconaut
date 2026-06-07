# Phase 2 Conformance Specification

**Issue:** arconaut-wr9

---

## 1. Message Passing

| # | Assertion | Test | Oracle |
|---|-----------|------|--------|
| 1.1 | `SoulCommand::UserInput` round-trips through channel | `messaging::tests::user_input_roundtrip` | Gold |
| 1.2 | `TuiEvent::NewMessage` round-trips through channel | `messaging::tests::new_message_roundtrip` | Gold |
| 1.3 | Multiple events queue in order | `messaging::tests::event_ordering` | Gold |

---

## 2. Persistent Shell

| # | Assertion | Test | Oracle |
|---|-----------|------|--------|
| 2.1 | Shell starts and `echo hello` returns "hello\n" | `shell::tests::echo_works` | Gold |
| 2.2 | State persists between sends (`cd /tmp; pwd`) | `shell::tests::state_persists` | Gold |
| 2.3 | Output is forwarded via channel | `shell::tests::output_forwarded` | Gold |
| 2.4 | `terminal_send` tool exists in registry | `shell::tests::tool_registered` | Gold |
| 2.5 | Multi-line input works | `shell::tests::multiline_input` | Gold |

---

## 3. TUI Event Loop

| # | Assertion | Test | Oracle |
|---|-----------|------|--------|
| 3.1 | Crossterm events parsed into internal enum | `event_loop::tests::key_event_parsed` | Gold |
| 3.2 | Resize event updates terminal size | `event_loop::tests::resize_updates_size` | Silver |
| 3.3 | Frame render completes without panic | `event_loop::tests::render_frame` | Gold |

---

## 4. Widgets

| # | Assertion | Test | Oracle |
|---|-----------|------|--------|
| 4.1 | `HeaderWidget` renders on empty state | `widgets::tests::header_empty` | Gold |
| 4.2 | `ChatPane` renders messages with timestamps | `widgets::tests::chat_with_messages` | Gold |
| 4.3 | `TerminalPane` renders vt100 screen state | `widgets::tests::terminal_renders` | Gold |
| 4.4 | `InputLine` renders with focus prefix | `widgets::tests::input_focus` | Gold |
| 4.5 | `ContextBar` renders token ratio | `widgets::tests::context_bar` | Gold |

---

## 5. Layout & Interaction

| # | Assertion | Test | Oracle |
|---|-----------|------|--------|
| 5.1 | Split ratio is adjustable | `layout::tests::split_adjustable` | Silver |
| 5.2 | Focus switch updates active pane | `layout::tests::focus_switch` | Gold |
| 5.3 | Input prefix reflects focus | `layout::tests::input_prefix` | Gold |
| 5.4 | Chat scroll independent of terminal | `layout::tests::independent_scroll` | Silver |

---

## 6. Ghostty Optimizations

| # | Assertion | Test | Oracle |
|---|-----------|------|--------|
| 6.1 | Kitty keyboard flags pushed on startup | `ghostty::tests::keyboard_flags` | Silver |
| 6.2 | Mode 2031 query emitted | `ghostty::tests::mode_2031_query` | Silver |
| 6.3 | OSC 133 A emitted before prompt | `ghostty::tests::osc133_prompt` | Silver |
| 6.4 | Synchronized output brackets frame | `ghostty::tests::sync_output` | Silver |

---

## 7. Styling

| # | Assertion | Test | Oracle |
|---|-----------|------|--------|
| 7.1 | Dark theme has distinct palette | `style::tests::dark_theme` | Gold |
| 7.2 | Light theme has distinct palette | `style::tests::light_theme` | Gold |
| 7.3 | Theme produces valid ratatui Colors | `style::tests::valid_colors` | Gold |

---

## Fault Model

| Fault | Expected Behavior |
|-------|-------------------|
| Shell process dies | `TuiEvent::TerminalOutput("[shell exited]")`, attempt restart on next `terminal_send` |
| Terminal pane resize to 0 width | No panic, render empty |
| VT100 parse error on malformed ANSI | Silently drop sequence, continue parsing |
| Crossterm event parse failure | Log error, continue event loop |
| Mode 2031 query timeout | Default to dark theme |

---

## Oracle Tiers

- **Gold:** Deterministic, no external deps, run in CI
- **Silver:** Requires filesystem, process spawning, or timing-sensitive checks
- **Bronze:** Manual/integration tests, run on-demand

All Gold tests must pass. Silver tests should pass but may be flaky.
