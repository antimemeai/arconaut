# TUI chat-shell discovery and local controls

Operator requests continued development and a nicer TUI/chat experience. Management first observed live core running Linux checks, then external_unknown at05:02:42UTC. Read-only retained local/remote inspection found interrupted ASan coding after passed debug/release and ASan context-budget; no matching remote check process observed. Core resumed in a fresh explicitly Root-authored context with original whole-unit and hardening bounds, no replay, no full-matrix rerun. Prior session/effects retained. This UI unit does not qualify the core candidate.

## Purpose and grounded consequences

Existing Composer, TerminalUI and main command dispatch are the source basis, along with the existing terminal PTY oracle. The input decoder already distinguishes pasted bytes from keyboard actions, maintains UTF-8 cursor boundaries and draft/history, and sends queued prompts only at boundaries. Keep those contracts rather than replace the terminal stack or adopt a dependency.

Command names were hidden in a flat main-only help string; help itself was dispatched to the worker and therefore queued while busy. Commands now have shared discovery metadata for completion, contextual hints and grouped help. Actual session/model/context commands still execute through their existing owning worker paths; this pass does not redesign those APIs or mutate a live provider request.

## Delivered behavior

- Tab completes a single slash-command token at the end of the draft. Unique argument-taking names add a space; ambiguous names expand only to common prefix. Exact /draft wins over longer /drafts on a subsequent Tab.
- Tab in a paste stays literal. Tab with arguments, normal text, multiline text or a cursor inside a token never rewrites that draft. No completion submits work.
- Composer hint shows matching commands or exact syntax/description; ordinary hint distinguishes Enter send from Enter queue.
- TUI /help and /keys run locally and immediately. /queue reports pending work separately from recovered unsent drafts. /cancel requests existing native cancellation and clears pending queue, rather than claiming immediate quiescence.
- /clear is explicitly display-only, not context deletion. Local output/clear returns display to bottom; original cancellation/draft persistence and recovered-draft explicit submit semantics remain.
- Plain /help shares the guide; TUI-only commands are labelled as such. No new dependencies or provider calls are needed for local controls.

## Scope and bounds

Started05:02UTC; whole allowance45minutes, hardening at most15minutes inside it, exactly two layers. Layer1 completion oracle first failed on the old Composer, then terminal unit and actual PTY passed. Layer2 one source recheck found exact /draft versus /drafts completion precedence, fixed with direct case; affected terminal and PTY passed again at05:06UTC. No third review/certification layer. Existing PTY exercises paste, resize, cancellation, queue nonexecution, persisted draft/history, restart and mascot layout. New cases directly exercise completion and local help/queue during a living shell, and slash stop. `git diff --check` clean for owned source.

Build used separate build/ui-shell release configuration to avoid primary build-directory collisions. Main core source remained outside UI ownership. Source publication and native activation are separate: this report records direct checks, not actual activation.

## Next useful UI units (not delivered here)

1. A real keyboard-navigable command palette and human-friendly session/model pickers, without making raw Lua/JSON paths disappear.
2. Multiline composer semantics: vertical cursor navigation inside a draft, history only at a deliberate edge/shortcut, word movement/deletion, compact paste summaries and optional external editor.
3. Conversation readability: distinct message blocks, bounded collapsible tool output, clear activity versus failures, follow-tail/unread indicators and transcript search.
4. Session resume/new/switch ergonomics with unsent draft preservation and explicit per-session work ownership.

Those should be separate whole behavior units with direct terminal observations, not one generic visual overhaul or unrelated assurance campaign.
