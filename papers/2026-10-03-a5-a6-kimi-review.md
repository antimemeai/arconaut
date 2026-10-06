Independent review complete (read-only; no test execution performed — test/PTY behavior assessed by reading the scripts only).

## Scope inspected
- `src/session.cpp` (`save_session_info`, `list_sessions`), `src/main.cpp` (arg parser, discovery path, callbacks, error path), `src/coding.cpp` (`CodingEngine::operation` clock/callback), `src/terminal.cpp` (`trim_terminal_transcript`, `TerminalUI::run`), `tests/session_store_test.cpp`, `tests/terminal_test.cpp` (grep-level), `scripts/check-terminal-pty` comfort fixture.

## Blockers
None found.

## Verified correct against the focus classes
- **Listing never mutates/locks live audit** (`session.cpp:53-124`): only `stat` + read of `session-info.json`; discovery returns before `create_directories`/journal open (`main.cpp:115-118`). `write_file` is mkstemp+rename (`tools.cpp:24+`), so a concurrent lister can't read a torn snapshot; failures degrade to `"damaged"` (session.cpp:97-99).
- **Schema/capacity/quoting**: version pin, `validate` bounds, 1MB read cap, identity hex length-32 checks, single-quote shell escaping including `'"'"'` (session.cpp:107-110), canonicalized path with non-canonical fallback. Explicit `configuration_source` disclaimer covers the legacy-missing-snapshot limitation.
- **Callback after terminal observation** (`coding.cpp:596-612`): callback fires only after the terminal `AttemptObservationEvent` submit; clock starts at operation entry (`coding.cpp:509`). `unknown`/`failed`/`completed` labels map from disposition correctly, including exec non-zero-exit and interrupted/io/incomplete/external_unknown → unknown for provider/exec.
- **Failure label in UI**: worker posts `Kind::failure` on exceptions and complete-handler prints "Turn failed"/"Turn cancelled" (`terminal.cpp:420-468`); `ui.failed()` wired in `main.cpp:341`.
- **Trim audit preservation**: display-only; bounded, no-LF fallback cut, UTF-8 continuation skip (`terminal.cpp:117-130`). Tests exist (`terminal_test.cpp:48-54`).
- **PTY oracle not stale**: `wait_frame` checks fresh frames for `Turn completed`/`Turn cancelled`/`Turn failed` and anchored LINE markers (`check-terminal-pty:274-291`), resize cursor bounds checked.

## Refinements (non-blocking)
1. **`terminal.cpp:510-511` — anchored scroll drifts when trim removes more lines than arrive.** `scroll += lines.size() - previous_lines` only when the count grows. Correct compensation is `G - T` (grown minus trimmed); when `trim_terminal_transcript` deletes more front lines than were appended (e.g. one giant trimmed line while scrolled up), `lines.size() <= previous_lines`, no adjustment occurs, and the anchored viewport slides. Fix: track the pre/post-trim transcript size or have trim report lines removed and subtract from `scroll` before clamping.
2. **`coding.cpp:606-609` — cancelled non-exec/provider tools label "failed".** `interrupted` for fs/lua-side operations maps to `failure`, so a user Ctrl-C during a file write reports "failed" rather than "stopped; outcome unknown". Cosmetic but misattributes cancellation.
3. **`session.cpp:54` — `root` field echoes the non-canonical requested path** (only `lexically_normal`), while entry paths are weakly-canonicalized; mixed forms in one document. Consider canonicalizing root too (fallback on error).
4. **`main.cpp:78`** — a discovery root whose name starts with `--` cannot be passed positionally; negligible edge, document or accept `=`-form.
5. **`session.cpp:102-103`** — `last_activity_unix_seconds` is audit mtime (derived, as permitted); if `stat` fails, the field is JSON-null with no status flag — consumers should tolerate null; consider an explicit `"unavailable"`.

No faults found in summary schema, damaged-entry handling, or operation lifecycle timing.
