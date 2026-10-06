# Tool timeout continuation

2026-10-06 operator correction: a tool deadline must lead to another attempt or
approach, not terminate the coding session. Current native Child uses interrupted
for both elapsed deadline and explicit cancellation; CodingEngine propagates
interrupted out of Lua/turn. Distinguish deadline as io/ETIMEDOUT using the existing
error representation; expose timed_out and unknown effect outcome on exec result.
Existing native child owner kills/reaps its process group on unwind before result
returns (grounding: src/native_process.hpp plus acquired POSIX wait/waitpid and
Darwin kill/wait manuals under papers/platform/refit-custody-2026-10-01).

Preserve output_ref and retained partial chunks; default Lua appends the tool result
and asks the model for its next step. Model may choose a new attempt with a larger
timeout, inspect partial output, split the task, or choose a different approach.
Never automatically repeat the timed-out command: prior external effects can remain.
Explicit operator cancellation stays interrupted and terminates the turn. Provider
transport errors remain distinct and are not retried automatically by this change.

Direct red: actual native timed-out exec returns inspectable result without throwing;
actual default Lua/provider loop sees timeout result, chooses a different exec,
completes three provider steps, and independently counted original effect occurs
once. Existing cancellation, audit partial readback and child cleanup checks stay.
Then implement, affected debug/release/sanitizer and process/PTY checks, independent
narrow Kimi code/oracle review, integrate findings, rebuild and resume campaign.
