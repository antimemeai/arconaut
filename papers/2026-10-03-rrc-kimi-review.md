# Independent Review: Restart/Resume/Continue (RRC) Scope

**Inspected:** `docs/RRC_SUBPLAN.md`, `src/session.cpp`, `include/arconaut/session.hpp`, `scripts/arco`, `src/main.cpp` (full), plus targeted verification in `src/coding.cpp` (turn/request/restart handling, recovery), `src/context.cpp` (origin persistence), `src/tools.cpp` (exec child lifecycle), `programs/turn.lua`, and `src/terminal.cpp` (worker join/cancel reset).

## Verified correct against the plan

- **Settings persistence/validation** — `SessionStore::save` (session.cpp:77-87) validates (non-empty model/workflow, no NULs, effort ∈ {low,medium,high,xhigh}) before recording to the program channel; invalid CLI `/effort`, `/model`, `--effort` values throw before any record is written. CLI overrides are merged into loaded settings and saved *before* engine activation (main.cpp:179-198). `/workflow` and startup workflow are read-checked (main.cpp:187, 259) before persistence, absolute-normalized.
- **Restart scheduling** — restart tool (coding.cpp:674-680) only stages `restart_note_`; `request()` refuses `busy` while staged (coding.cpp:604-605); `turn.lua:20` ends the turn after the current batch; `turn()` validates protocol at batch end (coding.cpp:753-754) and resets the note on any error/interruption (coding.cpp:756, 781). Durable intent is committed in `perform` only after a successful `engine.turn` via `take_restart_note()` (main.cpp:289-293), which exchanges the note out before persistence (coding.hpp:46), so a failed `session_store.restart` discards rather than replays a stale request.
- **Single injection / consumption** — `resume()` dedups via `origin = "rrc:"+token` against committed context records (session.cpp:112-123; origin persisted in the append packet, context.cpp:157), injects at most once, then records `state:"consumed"` before any provider request. Ordinary reopen (`resume_requested` false) never touches the intent (main.cpp:192).
- **Pending-linkage and duplicate-pending blocks** — session.cpp:91-99 throw `busy` on an existing pending intent or any admitted attempt lacking a terminal observation.
- **Launcher** — `scripts/arco` waits for full process exit (`if "$arco_executable" "$@"`), only loops on status 75, and restarts with `--session … --resume-continue` (never replaying the original `--once` prompt; `--once` → `--resume-once`, `--plain` preserved). Old image fully exits before the new one starts.
- **Synchronous native ops / recovery refusal** — exec collects and reaps via `Child::collect` before returning (tools.cpp:158-172); `recover_coding_session`'s custody verifier fails reconciliation for any unresolved attempt whose operation metadata isn't `provider` (coding.cpp:446-449), aborting startup with the "fresh --session" hint.
- **Worker lifecycle** — TUI `start` joins any prior worker and resets `cancelled` (terminal.cpp:253-257), so no stale worker/cancel state leaks across turns.
- **Identity stability** — `session_identity` (session.cpp:36-62) reuses the recorded identity, else migrates from the latest `DecisionEvent`, else issues and records once.

## Findings

1. **Low — consumed intent is unrecoverable on clean turn failure (not just crashes).** main.cpp:192 consumes the intent durably, then main.cpp:307-311 (`one` mode) rethrows any turn error and exits 1. A transient provider/network error on the resume turn permanently retires the pending intent; the note survives only as the injected `continue` user message in context. The plan sanctions this for *crashes* ("crash after consumption does not retry"), but a clean, immediately-retryable provider failure takes the same path. Consider deferring the `consumed` record until the provider request is actually issued, or re-pending on clean `Error` before the first request.

2. **Low — `validate()` does not bound model/workflow length** (session.cpp:28-34) while restart notes are capped at 65536 (session.cpp:89). A hostile/corrupt settings record with a megabyte-long model string is accepted and re-persisted. Add a sane length cap for symmetry.

3. **Cosmetic — `safe()` escapes only the 0xC2 byte of a C1-control UTF-8 pair** (main.cpp:30-35); the following 0x80–0x9F byte is emitted raw to the terminal. Escape both bytes or the pair.

## Limitations

- I did not re-verify `RetainedState` reconciliation internals, journal durability primitives, or the test suite; the demo/audit evidence cited in the task was taken as scoped confirmation only.
- Resume consumption ordering (finding 1) is a judgment call against plan wording; all other checked behaviors matched the plan literally.