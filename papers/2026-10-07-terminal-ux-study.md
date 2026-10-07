# Terminal harness study

2026-10-07. Purpose: make Arco a beautiful, responsive daily coding interface,
including discoverable slash commands. Operator explicitly wants colors, animation,
and useful information density first; advanced control comes later. No design SaaS.
This is a focused study of presentation and command machinery in ten references,
not a claim to have exhaustively reviewed every harness in quarantine.

## Findings used in the plan

Paths below are relative to each reference directory under `quarantine/`.
Existing snapshot provenance is in QUARANTINE.md and the known/discovered-agent
acquisition catalogs. New MiMo/Muse provenance is in
`2026-10-07-terminal-reference-acquisition.json`.

| Reference / revision | Primary material studied | Consequence for Arco |
| --- | --- | --- |
| Codex / `08e2b58b07b8a423d6577b66fb7756e980b53dbf` | `codex-rs/tui/src/bottom_pane/command_popup.rs`, `bottom_pane/chat_composer.rs`, `history_cell/messages.rs`, `shimmer.rs` | Canonical commands with searchable aliases, bounded popup with visible selection, typed message rendering, time-driven highlight sweep with color fallback. |
| Pi / `8ce69e9d2b171d173fe4b6b2b6256f1f4411e69d` | `packages/tui/src/autocomplete.ts`, `components/loader.ts`; coding-agent `src/modes/interactive/components/{user-message,assistant-message,tool-execution,footer}.ts` | Separate message/tool styling; tool-specific collapsed previews; argument completers; cached status inputs; small animated indicators with explicit start/stop. |
| Oh-my-pi / `8b25ad4a05625dde65df41d057756b4815f4837c` | `docs/slash-command-internals.md` | Command provenance and collision rules must remain explicit when Lua extensions arrive. Do not copy its large discovery ecosystem into this visual pass. |
| OpenCode / `2006259a02a87edf9e37f253cbddf3188309026b` | `packages/tui/src/component/prompt/autocomplete.tsx`, `routes/session/footer.tsx` | Name/description columns, selected-row contrast, scroll selection into view, compact operational footer. Completion can insert text rather than execute. |
| Crush / `76cc5c574e15072b15aaed0f4f843a5711fae0d9` | `internal/ui/completions/{completions,keys}.go`, `chat/{user,shell}.go` | Bounded list geometry; cached message render; collapsed shell output with explicit expansion and truncation counts. |
| Kimi CLI / `9ab1286b8fe4e6bcd116949a27ce5e0ac3389c82` | `src/kimi_cli/ui/shell/{prompt,slash}.py` | Match aliases while showing one canonical row; distinguish typed command token from other text; concise descriptions with selection detail. |
| Mistral Vibe / `7c19608af06f6c61d63f8f7a5c3430da73fba2ab` | `docs/adr/0012-two-phase-slash-command-execution.md`, `vibe/cli-rust/src/completion_manager.rs`, `agents.rs` | Explicit busy-command policy; avoid competing queue authorities; recalled history should not reopen suggestions without a real edit; delayed activity indicators avoid flashing on brief operations. |
| Gemini CLI / `c6bccb7ecbf6d8368d995455dd725ed34466faad` | `packages/cli/src/ui/hooks/useSlashCompletion.ts`, `components/SuggestionsDisplay.tsx`, `docs/cli/themes.md` | Shared command metadata supports nested/argument completion; visible list capped at eight; semantic color roles and clear selected-row contrast. |
| MiMo Code / `6babeb0b98f9b4818bddf04a4331edfee04dbf85` | `packages/cli/src/cli/cmd/tui/component/prompt/autocomplete-detect.ts`, corresponding tests; `context/visual.ts`, `component/spinner.tsx` | Vivid by default with independent motion switch. Its 80ms spinner has a static fallback. Tests distinguish Unicode string indexes from display columns and avoid URL-path triggers. |
| Muse Code SDK / `912061bb125b4bff60c9f3089a21cd23b53c6b4f` | `README.md`, `clients/sdk-ts/src/facade/turn-handle.ts`; official interactive documentation | Public SDK exposes typed turn outcomes; a lost host is unknown, not fabricated completion. User manual documents `/` filtering and immediate/deferred commands. **Host/TUI implementation is absent from this repository.** |

Muse's public source is its SDK/protocol, not its terminal renderer. The SDK README
explicitly says the real host binary is not part of the repository. We did not
substitute an unrelated Muse project or claim source evidence for its animations.
Official behavior reference: [Working with Muse Code](https://dev.meta.ai/docs/muse-code/interactive).
Xiaomi source identity was confirmed through its
[official project link](https://mimo.mi.com/docs/en-US/updates/feature/mimo-code).

## Actual Arco gap

`src/terminal.cpp` contains a useful command table, explicit modal palette,
completion hints, and composer editing. `/` alone does not open the palette.
The renderer re-wraps a flat transcript string and writes a whole frame; raw
operation/status messages share the conversation display. The composer reserves
three rows. Physical dimensions are clamped upward to minimum rows/columns,
which can draw outside a genuinely smaller terminal. `src/main.cpp` updates the
title after settings changes, so the problem is presentation, not an assumed
permanently stale model label.

The implementation already has enough foundation for the redesign. Build typed
presentation events at their producers, reuse composer/persistence behavior, and
give the renderer semantic spans and bounded caches. Do not reverse-engineer
event identity from bracketed prose or create a second audit log.

## Choices rather than imitation

- Arco keeps C++/Lua. None of React/Ink/OpenTUI/Ratatui/Charm/prompt-toolkit is
  adopted by this study, and no dependency is proposed.
- Animation belongs in the first visual slice. Time changes a small indicator or
  highlight, not the entire conversation. Do not copy per-character allocations
  from Codex's shimmer merely because the effect is attractive.
- Vibe's idle-only settings rule conflicts with our operator's turn/workflow
  boundary activation rule. Retain our semantics and display pending changes.
- Muse's `/clear` starts a fresh context; Arco's existing `/clear` clears display
  only. Retain Arco's meaning and make it obvious in command descriptions.
- A command menu is an interaction aid, not an approval dialog. Exact commands
  remain quick to submit; navigation alone produces no runtime action.
- The result should have strong visual character, not inherit a generic theme
  by copying whichever reference has the largest component library.

The concrete discussion plan is [TUI_PLAN.md](../docs/TUI_PLAN.md).
