# First-core testing plan

2026-10-02 sequencing correction: test the current bootstrap behavior as it is added.
The [implementation plan](IMPLEMENTATION_PLAN.md) now targets a useful coding session
before advanced journal continuation, standing-worker recovery and refit. The full
families below remain later requirements; their exhaustive completion and another
plan review do not gate starting bootstrap CLM, tools or the provider loop.

2026-10-01. Adversarial plan review findings resolved; derived from the reviewed
[core specification](CORE_DESIGN.md). The [implementation plan](IMPLEMENTATION_PLAN.md)
assigns each test family to its owning conceptual unit. This is a plan, not a record
of executed tests or qualified dependencies.
Reviews and resolution: [oracle review](../papers/2026-10-01-plan-oracle-review.md)
and [execution review](../papers/2026-10-01-plan-execution-review.md).

## Purpose and evidence

The claim is useful self-development inside Arconaut with truthful original capture,
editable/evolvable context, safe code custody and continuity through refit. A successful
chat demonstration cannot establish the failure contracts. Each unit starts with a
written sub-plan and failing direct examples, then implementation, green checks and
adversarial code review. Resolve findings before its dependent unit uses the result.

Expected values come from specified transitions, independently known producer bytes,
external dispatch counters and deliberately seeded task requirements. Small reference
models represent only the invariant being checked; they must not reproduce production
storage/parser/scheduler code. Generators vary inputs and scheduling, rather than
deciding their own expected results. Sanitizers complement these behavioral oracles.
Coverage can reveal missing reachability; it is not an acceptance criterion.

Deterministic simulation uses injected storage, clock and completion boundaries, with
explicit schedules and seeds. OS/process/PTY and transport conformance then exercise
real seams; passing a simulated device is not filesystem qualification. A bounded
state explorer enumerates short admission/publication/handoff histories against allowed
transitions. No JVM specification tool is required. Expand a model only when a concrete
fault merits it; do not build a second harness whose correctness becomes the project.

## Qualification profile

The first host is this operator's macOS arm64 machine. Record compiler, standard
library, build mode, filesystem/sync policy, runtime/linkage and terminal profile with
results. Available tools are not qualified tools. Establish debug and optimized builds
with explicit warnings and no unnoticed profile fallback. ASan/UBSan exercise memory,
undefined behavior and unload/continuation paths; a separate TSan profile exercises
concurrent publication, completion queues and capture. Unsupported instrumentation is
reported rather than labeled passing. Linux fleet jobs can supply additional Clang/
libc++ profiles; MSan requires a compatible fully instrumented dependency graph and
is conditional on that actual graph. Do not infer Mac custody semantics from Linux.

Build both successful and rejected API-shaped cases: wrong environment/incarnation,
stale authority, invalid lengths, exhausted allocations, unsupported controls and
nonstring Lua errors. Runtime exception mode and C symbol linkage are qualified
together before the embedding depends on them. No proposed testing library is adopted
by this plan; use a small owned assertion/runner seam initially and discuss dependencies
if their concrete benefit warrants adoption.

## Direct oracle families

| Family / owning unit | Faults and interleavings | Direct oracle and acceptance |
| --- | --- | --- |
| T0: identity/time/build / U0 | PID/FD/registry reuse, stale handles, clock jumps/restarts, unsupported instrumentation | Foreign-incarnation handles reject; local deadlines use their monotonic domain; cross-incarnation arithmetic rejects; wall changes do not alter elapsed intervals; intended warnings/profile actually apply |
| T1: journal/admission / U1 | Short write/EINTR, allocation/storage exhaustion, failed sync, death at each commit/publication/ack/dispatch boundary, corrupt/torn tails and interiors | Independently known bytes and allowed committed state survive within the declared fault model; provisional material never publishes context; actual dispatch count obeys admission; incomplete/corrupt material cannot silently produce a complete claim |
| T2: context/continuations / U2 | Concurrent edits, delayed response/tool result, editor exported from an old base, malformed/partial file, opaque blocks, repair that grows input | CAS preserves both candidates; completion remains on its originating branch; actual next structured/final request carries selected obligations and concurrent edits exactly; flawed revision and originals remain retrievable |
| T3: operations/custody / U3 | Exit before EOF, pipe-filling output, partial stdin, descendants holding pipes, lost handle reply, participant death, cancel racing completion, stale epoch queued dispatch | Controlled child/peer counts actual effects and emits known streams; exit/EOF/stop differ; same retained invocation is not relaunched; changed-input duplicate rejects; explicit retry remains distinguishable; settlement reflects actual local resources |
| T4: Lua/generations / U4 | Allocation in setup, native exception vs Lua error/yield, nonstring error, coroutine GC, finalizer failure/resurrection, load side effects, late imports, pending activation while old root needs children | Protected preparation returns correct error/disposition; owned continuation resumes with exact values; old definitions remain through root conclusion; new roots wait while old descendants progress; no callback enters retired code or dead VM |
| T5: native replacement / U5 | Failed compile/load/check/migration, stale queued callback, suspended Lua continuation, finalizer/destructor/thread pins, retained-generation limit, destructive migration failure | Old counters/resources remain intact on independent preparation failure; each live obligation pins its code; unload occurs after last old return; exhausted retention refuses activation; destructive failure blocks rather than resumes invalid old state |
| T6: provider/coding loop / U6 | Tool-only response, multiple linked calls, malformed/truncated stream, unexpected block, rejected request, transport failure after send, expiry/config change, operator message mid-request | Credential-free server captures actual path/headers/body and counts requests; second request preserves specified calls/results; exact post-adapter inputs match capture; unknown remote outcome is not cancelled/retried by assumption; incoming steering reaches explicitly selected next input |
| T7: terminal/client / U7 | Byte-fragmented UTF-8/escapes, ambiguous Escape, paste/control content, resize, disconnect/reconnect, draft restore, interrupt vs queued submit, client crash | PTY fixtures observe intended input submission once, queue/interrupt semantics, edits/repair/final-input inspection; known controls cannot alter layout through output text; display/edit behavior meets declared terminal profile; core work continues on disconnect |
| T8: outpost/refit / U8 | Model-initiated control, unpausable worker, open buffered pipes, failed build/start, handoff ack loss, stale owner, active outpost call at return, reattach failure | Actual no-progress/alive status and original worker incarnation/heap survive pause/resume; known bytes survive; only current owner dispatches; no main ordinary work during refit; return waits for outpost settlement; retained outpost remains usable on build/start failure |
| T9: complaints/autodroit / U9 | One sink unavailable, timeout after accepted write, duplicate delivery, missing low-memory state, context/program/evaluator changes during a trial | Same complaint links captured state and both independently inspected sinks; ambiguous delivery remains explicit/reconciles by actual sink contract; reporter continues; experiment records actual effective candidate and seeded task outcome, including failed/reversed trials |

### T1: durability and recovery boundary

Specify the journal byte format, length/sequence bounds, batch commit checksum, segment
rotation and sync backend in U1's sub-plan. Enumerate crash cuts before and after every
source frame, commit frame, sync, in-memory publication, acknowledgement and external
dispatch. Model admissible surviving writes and sync failures, including reordered/partial
unsynchronized writes and directory/segment publication under the chosen backend.
Exercise a surviving later commit with missing earlier source, complete unsynchronized
batches, and acknowledged earlier batches followed by damaged pending material.
Require all batch sources/dependencies before trusting its transition. Prefix-only
models must explicitly restrict their fault claim; no APFS ordering is guessed.
Do not equate a valid
frame with acknowledgement delivery. Recovered committed but unacknowledged admission
may have dispatched: it must reconcile rather than automatically replay. Subsequent
stdin, signals and messages receive the same intended-effect/partial-outcome treatment.

Exercise real subprocess kills and actual local sync error paths in addition to the
model. SIGKILL is a process-crash test, not a power-loss experiment. Any stronger host
power-loss claim needs an appropriate disposable device/VM fault experiment and named
filesystem/device scope; absent that evidence, report the weaker claim honestly.
Writer startup with an existing live controlled worker must not create a competing
reader/controller. Buffer saturation must close admission and show actual loss bounds;
an explicit gap is not recovery of missing bytes. Emergency control remains usable.

### T2: CLM and recovery from an incorrect context

Seed a task with a known indispensable requirement and exact original evidence. Remove
or misstate it in a published revision, retrieve the original, publish repair and check
the subsequent request and task outcome. Preserve the bad revision and transformation.
The initial deterministic participant proves mechanism conformance; the eventual real
model exercise assesses whether the operations are useful to a model, not a universal
repair-success claim.

Hold a completion from R0 while R1/R2 edits race. Inspect its originating branch,
CAS conflict and deliberate incorporation, not just valid JSON. Export an R0 file,
publish R1, finish the old edit: its base must still be R0. Vary Unicode, binary
attachments, provider-specific blocks and malformed linkage. Fuzz representation
decoders with explicit rejection/lossless accepted-case predicates. Roundtrip alone
does not prove meaningful preservation: compare specified roles, IDs and byte content.

### T3–T5: effects, Lua and native lifetime

A controlled process keeps a private monotonic counter and emits numbered byte payloads
over independently observed fixture channels. It can exit while another owned fixture
holds its pipe, block on input, and acknowledge park/stop. Observe process/stream state
directly; elapsed quiet time is never a completion oracle. Kill a participant after
dispatch before handle delivery and recover the retained decision. The external count
must remain one; duplicate fresh IDs do not count as recovery.

Real local IPC clients fragment headers/payloads, coalesce frames, submit unsupported
versions, oversized lengths and invalid authorship/epochs, then disconnect mid-request
or after admission but before the reply. Invalid/incomplete requests dispatch nothing;
accepted invocation identity survives reply loss without redispatch. Independently
count effects and check configured allocation/buffering limits at this actual wire
boundary, not only the in-process interface.

Use allocation-fault injection at Lua setup/resume/error/finalizer boundaries; assert
actual error objects and continuation results. Isolate intentional native crash probes
in subprocesses. Retain a generation while each kind of callback/object/coroutine/thread
can enter it; release them in different orders and inspect the actual unload boundary
and resource owner. A generation pin counter agreeing with itself is insufficient:
invoke the delayed obligation and observe its defined output. No broad signal recovery
or fabricated C++ stack continuity is accepted. Refit tests retain the same actual
worker VM/heap, not a regenerated value labeled equivalent.

### T6–T8: boundary conformance and continuity

After first-provider selection, read its current primary protocol documentation and
retain bounded local fixtures of successful and rejected exchanges. Test final outgoing
bytes after every adapter transformation and retry, supported streaming events and
tool-result links. Transport cancellation is tested as local settlement plus the
remote outcome actually observable. A live model call is a bounded integration exercise
under ordinary configured authority; use fakes for repeatable adversarial schedules.

Separately compare predetermined server-emitted response body/observable representation
bytes to retained incoming originals and offsets before decoding. Vary fragmentation,
whitespace/unknown fields, malformed/truncated tails and capture failure mid-stream.
Compare the retained prefix and uncertainty to the actual observation boundary; correct
decoded tools do not establish raw capture fidelity. No network-packet/internal-provider
claim is implied, and transport chunk boundaries need not equal server write boundaries.

Declare terminal/locale/width/key encoding expectations before implementation. A PTY
drives fragmented input, large multiline paste, literal output controls and resize.
Inspect the submitted commands and terminal state, not screenshots alone. Recover
after ordinary exit/handled termination; after uncatchable client death a surviving
launcher or documented reset path must restore the terminal. Test that path explicitly.

Exercise handoff and return as the same exclusivity invariant. Record independently
counted effects on each owner, pause observations and bytes captured before/during/after
handoff. A second controlled consumer of a service continues working throughout refit;
its counter must advance. Pending controls initiated by the model must not wait for
their own tool frame. Inject lost replies, build failure, candidate startup failure and
failed reconnect. A visible pending/blocked transition is correct when its prerequisite
cannot be established; falsely claiming success is not.

## Running and reviewing the tests

Operator steering on 2026-10-01: postpone mutation campaigns until a useful working
Arconaut and a mature test suite exist. Mutation is not a prerequisite on the current
U0–U9 path. Retain direct behavioral, fault, conformance and sanitizer checks plus
independent code/oracle review. Future mutants use Runpod only; general Linux checks
use Neuroses over Tailscale. The operator handles Runpod node termination. This
explicit steering supersedes the earlier campaign timing below.

Run the owning unit's direct tests and relevant sanitizer/conformance profile during
development. At a changed cross-unit boundary, run the dependent scenario families.
Before milestone acceptance run the integrated self-development scenario in debug and
optimized builds, the applicable sanitizer lanes and the reviewed fault schedules.
Seeds and minimal counterexamples are retained as useful regressions; do not repeat
expensive unchanged tests after checks have passed without a new reason.

Fuzz journal/context/provider/IPC/input decoders and bounded transition schedules where
the properties above provide an oracle. Include invalid length/version/UTF-8 and
resource-exhaustion cases without unbounded allocations. Schedule fleet mutation once
direct oracles are mature for effect dedup, source commitment, CAS, code pins and owner
fencing. Mutants always run on the fleet, never this laptop. Review surviving relevant
mutants for missing behavioral discrimination; do not optimize a score or require tests
for equivalent changes. Fleet access is a separately available resource, not guessed
credentials; lack of a run remains explicit, never silently marked completed.

Code reviewers inspect the actual oracle and seams alongside code, challenge masking
fixtures, reproduce consequential failures and verify the corrected behavior. No
meta-test is added merely to certify another test. For each unit report the claim,
fault class, profile, observed result and remaining limitation in its journal/review.

## Milestone acceptance

Perform real source work through Arconaut: inspect/edit/run checks, change a governing
Lua tool/workflow with deferred and interrupted activation, write/use/revise a context
program and repair the seeded omission, activate a compiled native change, then change
foundation code through outpost refit. Include a failed build/start recovery, preservation
of an actual paused standing worker, original/request inspection, complaint delivery to
beads and the separate database, and one direct-outcome improvement/reversal experiment.
The operator must be able to keep working in that conversation afterward.

All required failure contracts need their direct checks and resolved code reviews.
Unsupported platform/provider/general-tree behavior remains outside the declared
profile. A live successful demonstration does not replace the failure evidence above.
