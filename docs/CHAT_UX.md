# Chat ergonomics baseline

The operator adopted this direction on2026-10-07: **chat-first, tools-secondary**, with a capable composer, discoverable commands and a readable conversation. This is the baseline, not ornamental polish. Powerful Lua/JSON/CLI paths remain available; ordinary interaction should not require memorizing them.

## Composer: delivered first editing slice

Enter sends. Alt-Enter or Ctrl-J inserts a newline. Bracketed paste stays literal and never submits itself, invokes a shortcut, or dispatches a command.

| Key | Behavior |
| --- | --- |
| Up / Down | Move between explicit newline-delimited draft lines, maintaining the desired terminal-cell column across short lines. On the first/last logical line, browse history. |
| Ctrl-P / Ctrl-N | Explicit older/newer history, even inside a multiline draft. Returning to the current draft restores its original cursor as well as its text. |
| Left / Right | Move between UTF-8 codepoint boundaries. Application-mode arrow sequences are also supported. |
| Home / End, Ctrl-A / Ctrl-E | Start/end of the current logical line. |
| Ctrl-Home / Ctrl-End | Start/end of the entire draft (when the terminal sends the corresponding modified-key sequences). |
| Alt-B / Alt-F, Ctrl-Left / Ctrl-Right | Move backward/forward through tokens separated by ASCII whitespace. Option-as-Meta/terminal key mapping may be needed for Alt shortcuts on macOS. |
| Ctrl-W, Alt-Backspace | Delete the preceding token. |
| Alt-D | Delete through the next token. |
| Ctrl-K | Kill to line end; at line end, kill the newline. |
| Ctrl-U | Clear the draft. |
| Ctrl-Y | Yank the last deletion/clear. This is one transient kill buffer, not a persistent kill ring or general undo. An oversized yank is rejected atomically. |
| Ctrl-C | Request stop and clear pending queue; preserve the current unsent draft. |
| Ctrl-Q | Stop and exit, persisting the current draft/cursor. |

Vertical motion uses the existing renderer's terminal-cell rules, including wide characters, combining marks, pasted tabs and escaped invalid bytes. This is codepoint editing, not full grapheme-cluster editing. Wrapped visual rows are not separate editing lines yet. History selection is a draft preview; it is never submitted merely by navigation.

## Command discovery: delivered

Tab completes a slash-command name at the end of the token. A unique argument-taking command adds a space; ambiguity expands only the common prefix. Completion never submits. The composer hint shows matching names or the selected command's syntax and purpose.

`/help`, `/keys`, `/queue` and `/cancel` are immediate local TUI controls, even while work is active. Other session/model/context commands keep their owning worker's turn-boundary semantics. `/clear` clears display only, not model context. `/queue` distinguishes pending prompts from recovered drafts, whose submission remains explicit.

## Next separate UX slices

1. Wrapped visual-row navigation and a more generous adaptive composer viewport.
2. External-editor escape hatch with terminal restoration, draft retention and explicit submission after return.
3. Searchable keyboard command palette and human-friendly session/model selection.
4. Distinct conversation blocks, bounded collapsible tool output, unobtrusive truthful activity, search, unread/follow-tail controls.

Keep useful expert paths and truthful work ownership. A polished display must not pretend a stored steer was consumed, a stopped request settled remote effects, a tool completion accepted a unit, or a source commit activated a running executable.
