# A6 — visible long work and anchored scrolling

Grounding: TerminalUI's one turn worker, main status/error emitter, CodingEngine's
admitted operation start/terminal observation, existing bounded transcript/composer
and PTY cancellation/resize oracles. No new library/async architecture needed.

Expose turn elapsed and current native operation elapsed separately. Operation
completion fires after retained terminal observation; show named success/failure
and elapsed time in transcript/status, preserve cancellation's unknown semantics.
UI final turn labels Completed/Cancelled/Failed with elapsed time, not generic
Ready hiding outcome. Main error path explicitly reports UI failure. Pure Lua work
continues to show turn time without falsely showing a completed tool as active.

Keep viewport anchored when output arrives while scrolled up (same-width wrapped
line delta adjusts end-relative scroll). PgDn to bottom resumes follow. Clamp after
resize/retention trimming; composer/cursor share existing wcwidth layout. Bound
no-newline output too, with UTF8-safe cut and explicit display-trim notice; original
audit is untouched. Persist UI only on input/queue transitions, not timer redraws.

Targeted actual PTY fixture: burst of numbered lines then delayed tail while user
scrolls up; verify earlier viewport lines stay while tail arrives, PgDn returns to
new tail. Assert turn/tool timing, native completion and cancelled/failed outcomes,
composer typed during work survives resize with final cursor within screen bounds.
Existing native stop/reap and RRC/draft tests remain meaningful. Direct red before
implementation, release/debug/ASan affected checks, focused diagnostics and Neuroses
at A5/A6 boundary, then narrow independent read-only Kimi final review.
