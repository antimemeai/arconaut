## Review: exec deadline vs cancellation (3 bounded reads only)

### Blocker: ETIMEDOUT/exit-status collision misclassifies real failures as timeouts

**`src/native_process.hpp:218-219`** — a non-zero child exit is surfaced as `fail(ErrorCode::io, WEXITSTATUS(status))`, i.e. the exit status is stored in `Error::detail`.

**`src/coding.cpp:589-590`** — timeout detection is `error->code == io && error->detail == ETIMEDOUT`. On Linux `ETIMEDOUT == 110`, a valid exit status. So:

- **Trigger:** any `exec` whose process exits with status 110 (no deadline involved).
- **Consequence:** the result gets `timed_out:true`, `effect_outcome:"unknown"`, the operator message says "timed out; outcome unknown" (line 625), and — worse — line 605-610 forces `AttemptDisposition::unknown` instead of `failure`. The model is told the effect is unknown when the command deterministically failed, inviting a spurious retry of a completed operation.
- **Fix:** distinguish the two sources. Either tag deadline expiry with a distinct ErrorCode (e.g. a dedicated `deadline` code or a sentinel detail outside the 0–255 exit-status range, such as `-ETIMEDOUT`), or have `collect` report timeout via a separate channel (out-param / distinct exception) rather than overloading `ErrorCode::io` detail. Note line 219 already uses `-1` for the signaled case, so negative-detail sentinels are consistent with existing style.

### Non-blocking observations

1. **Disposition for genuine non-zero exits** (`coding.cpp:605-610`): exec `io` errors that are actually exit-status failures (per hpp:218) are recorded as `AttemptDisposition::unknown`, although the outcome is known (process ran and failed). The `!boundary.error` branch at 611-613 correctly marks exit_code≠0 as failure, but the throw-path equivalent never gets `failure`. Pre-existing shape, but the timeout change makes this path more traveled. Consider mapping `io` with detail in [0,255] from exec to `failure`.

2. **Cancellation ordering is correct**: `wait()` (hpp:230-231) and the waitpid loop (hpp:201-202) check `cancelled()` before the deadline checks (hpp:210, 235), so operator cancellation always yields `ErrorCode::interrupted`, which throws at `coding.cpp:637-640` and never gets `timed_out` markers. Intent preserved.

3. **Cleanup-before-result is sound**: on deadline `fail` at hpp:211, `pid` is still >0, so `~Child` (hpp:65-73) SIGKILLs the process group and reaps before the error propagates to `coding.cpp`. Caveat I cannot verify within scope: `kill(-pid, ...)` assumes the child was placed in its own process group (setpgid/setsid at spawn, outside the read ranges); if not, this kills the caller's group. Flagging as an assumption to confirm, not a confirmed fault.

4. **Partial output lost on timeout**: `take_partial()` (hpp:223) exists but `coding.cpp` timeout path returns only `output_ref` (of the attempt, not child output) plus markers; buffered partial stdout is discarded. If the model is expected to "choose a new approach" with evidence, consider including partial output or a real output ref in the timed-out result.

### Scope/limits
Inspected exactly the three requested ranges; spawn/setpgid code, `read()`, `error_json`, `Error::detail` type, and the journal disposition semantics were not read, so items 3 (process-group assumption) and the detail-type width in the blocker are inferred from the visible code. No tests claimed.
