# Review: interactive coding slice

Inspected: `docs/INTERACTIVE_LOOP_SUBPLAN.md`, `docs/USING_ARCO.md`, `src/terminal.cpp`, `include/arconaut/terminal.hpp`, `src/main.cpp`, `src/coding.cpp`, `src/openai.cpp`, `src/native_process.hpp`, `src/tools.cpp`, `tests/terminal_test.cpp`, `tests/coding_test.cpp`, `tests/openai_test.cpp`, `scripts/check-terminal-pty`, plus `programs/turn.lua` (referenced by the presentation path).

## High-priority defects

**1. Idle Ctrl-C destroys the in-progress draft; the unit test enshrines the contract violation.**
- `src/terminal.cpp:132-136` (`Composer::feed`, `c == 3`): unconditionally `text_.clear(); cursor_ = 0;`. The main loop only pre-intercepts byte 0x03 when `busy` (`src/terminal.cpp:396`), so with no active turn the composer's cancel branch runs and wipes the draft.
- Contract (subplan/task): "Ctrl-C stops current turn/process/Lua and drops queue but **preserves an in-progress draft**." Behavior is currently draft-preserving when busy and draft-destroying when idle — inconsistent and contrary to contract.
- Worse, `tests/terminal_test.cpp:28-32` types `"draft"`, then asserts `feed(c, "\x03").action == cancel && c.text().empty()`. This oracle locks in the wrong behavior; the PTY script never exercises idle Ctrl-C, so nothing catches it. Fix: on cancel, keep `text_`/`cursor_` (reset only history navigation), act on `InputAction::cancel` uniformly in `run()`, and change the test to assert the draft survives.

**2. Pasted Ctrl-C bytes cancel a running turn.**
- `src/terminal.cpp:395-402`: every raw `0x03` byte is intercepted before the composer sees it, *including bytes inside a bracketed paste* (composer paste state is invisible to this check). Trigger: paste text containing a literal `^C` (e.g. copied terminal scrollback) while a turn runs. Consequence: the current turn/process/Lua is cancelled and the queue dropped by an input action the user never intended as an interrupt; the remainder of the paste then lands in the composer as editing bytes. Fix: drop the pre-interception and let `Composer::feed` (which inserts 0x03 literally while `pasted_`, `src/terminal.cpp:128-131`) be the sole classifier — the `InputAction::cancel` path at `src/terminal.cpp:409-413` already handles cancellation; this also removes the busy/idle asymmetry behind defect 1.

## Follow-up refinements

- **Preview-mismatch duplication** — `src/coding.cpp:628-635`: when final text doesn't start with the streamed preview, the code prints a warning *and then the full final text*, so the already-displayed preview prefix appears twice. The contract says "reconcile preview without duplication"; the flag-and-show-both behavior is defensible for diagnosis but should be a conscious choice, not an accident of falling through to the common display block.
- **Unhandled wedge path on non-interruption Lua errors** — `src/coding.cpp:661-685`: missing `function_call_output` settlement runs only for `ErrorCode::interrupted`. If the workflow errors (e.g. `error(...)` or a host `Error`) after `arco.request()` appended a `function_call` but before the matching `arco.append` of its output, every later turn fails `validate_protocol` with `conflict` (`src/coding.cpp:167-187`, `527-529`) until manual `/lua` or `/restore` repair. That matches "no automatic retry," but the docs (`docs/USING_ARCO.md:66-70`) describe only interruption recovery; consider documenting or extending the settle.
- **`present()` key collision** — `src/coding.cpp:626-627` and `src/openai.cpp:251-253`: items/deltas lacking `id`/`item_id` are keyed `""`, so a previewed no-id message can falsely reconcile against a different no-id final item, silently truncating its displayed text.
- **Poll-error spin** — `src/terminal.cpp:382-384`: `poll` returning <0 (e.g. persistent `EBADF`, not just `EINTR`) loops with no backoff or exit, busy-spinning the UI.
- **Test-oracle weaknesses (cancellation timing/cleanup)**:
  - `tests/coding_test.cpp:185-204`: the exec-cancellation oracle depends on a 200 ms wall-clock window against a 100 ms cancellation poll — on a loaded machine the margin is ~1 poll tick; it also never asserts the `sleep 30` child/group was actually killed and reaped (only the PTY script does, and only via `kill(pid, 0)`, which is pid-recycling-sensitive). The unit test could capture the child pid (e.g. via `echo $$` like the PTY script) and assert death.
  - `tests/coding_test.cpp:87-89` (`StoppedTool` second call) inspects `input[size-2]` rather than locating the `function_call_output` by `call_id`; a context shape change would make this pass/fail for the wrong reason.
  - `scripts/check-terminal-pty:67-78`: the queue-drop assertion only inspects bytes after `observed.clear()`; it is valid for execution-order reasons, but it never asserts the *composer draft* survived the busy Ctrl-C (the contract's other half), which is exactly where defect 1 lives.
  - No test exercises idle Ctrl-C, paste-containing-0x03 while busy, or `TerminalMode` restoration on an exception path (only clean `/quit`).
- **Composer escape-edge**: while `pasted_`, an ESC that isn't the prefix of `\x1b[201~` is inserted literally (`src/terminal.cpp:80-86`), embedding raw control bytes into the submitted prompt.

## What checks out

Threading contract holds: only the worker touches engine/context/audit; the terminal thread exchanges presentation/input via the mutexed queue (`src/terminal.cpp:227-276`, `260-269`). Stream originals precede presentation (`src/coding.cpp:573-582`); partial function calls never dispatch (`ResponsePreview` only surfaces `output_text.delta`; `completed_response` is authoritative, `src/openai.cpp:228-372`). Interruption records `unknown` disposition and always submits a terminal observation (`src/coding.cpp:502-518`), settles missing call outputs, and never retries. `argv` precedence matches contract (`src/tools.cpp:141-157`). Cancellation poll intervals are bounded (≤100 ms, `src/native_process.hpp:229-244`); teardown kills/reaps the child group (`src/native_process.hpp:65-74`). Effort, `--plain`, and process output/error surfacing are wired as specified. `scripts/check-terminal-pty`'s terminal-restoration comparison (PENDIN masking) and post-interruption session reopen are sound oracles.

Limitations: read-only review, no execution; journal/reconcile internals (`RetainedState::dispatch`) were inspected only at their call sites.
