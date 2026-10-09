# Command jobs code review

2026-10-09. Independent, read-only review of the command identity/custody unit.
The designated Kimi provider failed before reviewing; this report occupies that
same single review slot. No build, test, provider call, product edit or subordinate
review was performed. Findings come from concrete source paths against
`docs/COMMAND_JOBS.md`, its subplan and the platform/harness/integration studies.
Line numbers refer to the implementation read during this review; fixes may move
them. The owner integrates findings within the existing hardening allowance.

## 1. High: blocked input prevents an explicit continuation signal

**Location:** `src/command_jobs.cpp:442-480`, `783-787`.

**Trigger:** Stop an established private PTY child, then admit input larger than
its kernel input capacity, followed by `signal: continue`. Input and signals share
`job.input`; the worker examines only its front delivery. Once the input write
returns EAGAIN, that delivery remains at the front. The child is stopped and
cannot read the bytes needed to unblock it, while the continuation is behind
that delivery.

**Consequence:** The accepted continuation never reaches the child. The original
lifetime eventually stops it instead. Interrupt/terminate signals can be blocked
in the same way; the separate sticky `op: stop` remains available. Foregrounding
does not resolve the deadlock because it deliberately does not continue a child.
This defeats explicit control of the same owned process under input pressure.

**Fix:** Let admitted signals progress independently of a blocked input write,
while preserving ordered input bytes, signal identity deduplication and pinned
child targeting. Keep stop/background available under pressure.

**Direct red case:** Use a raw private PTY fixture whose resumed child reads and
records an exact large input. Observe SIGSTOP, queue more than PTY kernel capacity,
then admit a continuation identity. Require the same PID to resume and record the
exact ordered bytes before the original lifetime expires. Repeating that identity
must not send a second signal or repeat writes. Existing separate STOP/CONT and
input-pressure cases do not exercise this combined trigger.

## 2. High: failed command-control result capture does not stop owned work

**Location:** `src/coding.cpp:1431-1440`, with control effects at `1968-1977` and
turn failure cleanup at `2579-2599`.

**Trigger:** A `process` input or signal is admitted and its native control is
granted. Retaining its ordinary `operation.result` then fails with capacity.
The catch sets `capacity_stopped_`, but it sets the sticky command failure and
calls `capture_failed` only for `persistent_command`, which is true solely for
the original `exec` operation.

**Consequence:** The controlled job remains running, and queued input may still
be written after result capture has failed. If the child stays quiet,
`poll_commands()` sees no output capture to fail and never stops it. Workflow
failure cleanup also leaves it alive. This violates the selected retention-failure
behavior: stop owned work, then retain a bounded unknown outcome or preserve a
fence. Existing background-job custody on unrelated ordinary workflow errors does
not justify continuing after this actual command-control retention failure.

**Fix:** Apply the sticky command-retention failure and stop policy to failed
result captures for command controls too. Do not fabricate an original locator;
preserve the original launch's reserved terminal debt until settlement.

**Direct red case:** Drive a quiet PTY fixture through an admitted input control
with the journal at an ordinary-capacity edge chosen so admission fits but result
capture does not. Require owned stop/reap, one bounded unknown launch terminal or
the explicitly retained fence, and no continuing gated child effects. The existing
producer-output refusal case exercises a different capture path.

## 3. Medium: complete request storage bypasses the input identity byte bound

**Location:** `src/command_jobs.cpp:754-795`, `803-826`; busy operator adapter at
`src/terminal.cpp:2144-2151`.

**Trigger:** An input request contains small or empty `bytes` plus a large extra
field, or a resize/signal request contains a large `bytes`/extra field. The full
arguments Value is retained in `Delivery::request`; the only history byte charge
is `delivery.bytes().size()` for `op == input`. Resize and signal have no payload
byte charge. The UI mailbox likewise bounds input's `bytes` alone and accepts
other request objects without a complete-payload bound.

**Consequence:** The declared 16MiB retained input identity payload does not bound
the actual resident request payload. Thousands of distinct controls can retain
large unused fields, and the mailbox can retain dozens before native admission.
Immutable sharing prevents a duplicate copy but does not bound those originals'
resident lifetime. This is a resource-bound bypass, not a proposal for an arbitrary
RSS ceiling.

**Fix:** Validate each operation's complete request shape and reject irrelevant
fields before mailbox/delivery retention, or bound and account the complete
retained request payload. Preserve exact admitted requests in audit and retain
enough native identity content to detect conflicting reuse.

**Direct red case:** Repeatedly submit input with empty bytes and large unused
payloads, and resize with large `bytes`, through native and mailbox paths.
Require refusal before the declared resident payload allowance is exceeded or
effects are granted. Check accepted identity retries and conflicts still behave
as specified.

## 4. High: station idle never pumps background command capture

**Location:** `src/main.cpp:890-940`.

**Trigger:** A station event workflow launches a zero-yield command and returns.
No second event arrives. Unlike TUI idle and the plain-input loop, the station
loop has no `engine.poll_commands()` call between events or while paused.

**Consequence:** The worker's captured output is not retained, and terminal
observations are not settled while the station remains idle. A producer exceeding
the 1MiB pending queue stalls under backpressure and may be terminated at its
original deadline instead of completing normally. Even a quiet completed child
keeps unsettled debt until another workflow happens to pump the engine. This is
the missing native execution-owner safe point for an already supported frontend.

**Fix:** Pump commands at a bounded station owner-safe point outside retained
transactions, including idle/paused iterations. Route retention failure through
the existing station blocked/pause behavior rather than admitting later effects.

**Direct red case:** One station event starts a controlled background producer;
send no second event. Require exact output exceeding 1MiB and one launch terminal
to be retained while station is idle, before timeout. Repeat with paused event
intake: pausing intake must not abandon already owned command collection.

## Limits

This report establishes source-derived defects and suitable direct oracles, not
executed reproduction results. It does not claim arbitrary descendant containment,
cross-process custody, full-screen terminal emulation, race freedom or general
repository correctness. No review of another review or new assurance layer was
added.
