# Running command background/foreground: Blackbird integration map

2026-10-09. Read-only product investigation. This report grounds the command-job
design in the current implementation; it proposes interfaces and direct fault
oracles, not an adopted implementation. The operator's current objective is
complete running-command background/foreground support through literature,
reference-source study, design, implementation and rigorous static/dynamic tests.
Steering and trajectory work are subsequent concerns.

Read workspace/project AGENTS and BLACKBIRD, workspace JEV, repository README and
current journal, and bounded-hardening instructions. No reference implementation,
account/provider transport, dependency, or product code was executed or adopted.
Line ranges below refer to the checkout read for this report.

## Present path and the invariant it cannot yet express

`blackbird.call("exec", args)` and provider tool calls reach `CodingEngine::call`,
which unconditionally wraps the body in `operation` (`src/coding.cpp:1746-1749`).
The ordinary turn program blocks on each call, then appends its result as one
function-call output (`programs/turn.lua:1-24`). A returned result must close that
provider protocol interval, but it must not claim the underlying command ended if
the command was backgrounded. These are two distinct lifetimes.

The host needs a stable logical command identity that survives every wait,
background, foreground, inspection and cancel operation. Command launch is an
effect admitted and dispatched exactly once. Foreground/background change the
relationship between a waiter and that command, not the launch identity or its
external effects. Completion belongs to the command launch attempt, even when no
foreground waiter remains. Each wait/control call can separately finish and have
its own admission/result linkage.

The words foreground/background also have two potential meanings that must be
explicit in the design: (1) whether the agent/operator waits and sees output for a
running command, or (2) POSIX controlling-terminal foreground process groups,
interactive stdin, stop/continue and PTY ownership. Current exec supplies pipes,
closes stdin immediately, and has no PTY or controlling-terminal operations.
Do not silently imply shell `fg`/`bg` semantics from a wait/attach implementation.

## Source map

| Integration seam | Current behavior and exact source | Consequence |
| --- | --- | --- |
| Native tool owner | `include/blackbird/tools.hpp:10-24`; per-call `LocalTools` constructed at `src/coding.cpp:2070-2087` | Neither object lifetime spans an asynchronous command. Observer lambdas capture the synchronous body by reference. They cannot be retained by a detached worker. |
| Exec validation/launch | `src/tools.cpp:214-251` validates timeout/output size/argv, constructs stack `Child`, launches shell or argv, and closes stdin | Owned job manager must validate before any effect and hold `Child` past initial wait return. Empty argv's documented fallback needs explicit preservation or explicit interface change. |
| Exec blocking/output | `src/tools.cpp:253-272` collects to a 16 MiB buffer, returns output and mandatory exit_code; `output_max_bytes` limits presentation only | A wait deadline cannot reuse the process lifetime timeout: current timeout throws, destroys Child and kills its group. Running responses need typed nonterminal state and no fabricated exit_code. |
| Pipe/process custody | `src/native_process.hpp:59-91,116-206` owns pipe FDs, spawns with POSIX_SPAWN_SETPGROUP, increments owned_children | Child is noncopyable and currently not movable; ownership can remain behind a stable owning pointer. No detached thread may outlive its callbacks/engine. |
| Exit/EOF distinction | `src/native_process.hpp:239-267,293-330` drains streams before observing leader with waitid(WNOWAIT) | Preserve pipe EOF, leader exit, descendant cleanup, and effect certainty separately. A leader can exit while descendants keep output FDs open. EOF can precede leader exit. |
| Destructor custody | `src/native_process.hpp:73-91` closes FDs, kills group, reaps leader, checks group disappearance | Background return through this destructor would terminate the command. Never use recycled PIDs/PGIDs: WNOWAIT pins leader until cleanup. Destructor comment says collect reaps, but current collect actually leaves leader unreaped. |
| Cancellation | `src/native_process.hpp:94-95,245-258,277-292`; `src/coding.cpp:1081-1082,1376-1380`; `src/main.cpp:392-395` | Current cancellation is one turn-scoped bool. A retained background job must have independent sticky stop state; UI resets cancellation for each turn (`terminal.cpp:1315`) and plain interface resets it (`main.cpp:991`). |
| Admission | `src/coding.cpp:1112-1147` atomically appends decision/invocation/attempt binding | Preserve this record-before-effect mechanism. Do not call manager.start and afterward synthesize a launch admission. |
| Dispatch | `src/retained_state.cpp:1772-1821` writes AttemptOpen before EffectBoundary, then AdapterReceipt | Receipt means adapter returned; it is independent of terminal observation. This permits a launch adapter to transfer custody successfully while command remains open. Repeated dispatch of an open attempt is refused. |
| Settlement | `src/coding.cpp:1239-1386` always captures one result original and writes terminal observation after body returns | Must be refactored or deliberately separated for persistent jobs. Returning a running result today falsely settles launch, and the exec exit_code dereferences at 1305-1307 and 2085-2086 fail. |
| Output identity | `src/coding.cpp:1241-1246,2070-2081` uses launch attempt as output_ref and chunk metadata.attempt | Retain this stable relation across foreground calls. A foreground wait's own attempt must not become the command's output_ref. |
| Output retrieval | `src/coding.cpp:1831-1866`; presentation at `src/tools.cpp:172-177` | Existing retrieval concatenates every retained chunk matching attempt, scanning full audit and reconstructing full output even for tiny ranges. Add bounded job/output discovery without conflating running empty output with an unknown ID. |
| Lua/tool schemas | `src/tools.cpp:788-840`; Lua convenience APIs `src/coding.cpp:377-390` | Current exec promises timeout cleanup and turn cancellation; schema has no job/control/wait state. Update descriptions and native/Lua interfaces together. JSON remains only provider/operator boundaries. |
| Task binding | `src/coding.cpp:1075-1080,1149-1170,1345-1354`; `src/tools.cpp:807-812` | Existing TaskRun stack ends when operation returns. Job activity must remain bound to original command and must not mark task done or stopped merely because waiter detached. |
| Task presentation | `src/terminal.cpp:1179-1194`; `src/task_view.cpp:30-58` | TUI recognizes only running/waiting as live. Inventing a background phase without updating this logic removes the live run badge. Use explicit attachment metadata with running phase or update declared live-phase semantics. |
| Busy TUI admission | `src/terminal.cpp:1308-1355,1899-1910,1958-2007` | UI worker serializes turns. All nonlocal commands while busy enter ordinary queue. A background request queued behind exec cannot release exec; a native thread-safe control mailbox is required. |
| Engine thread ownership | `src/terminal.cpp:1322-1323,1507-1508`; `src/main.cpp:953-967` | UI only touches engine during idle. Do not call operator_call from busy UI thread. Workers may queue immutable chunks/status; only engine owner may retain effects. |
| Operation UI stack | `src/terminal.cpp:1462-1482` | Synchronous nested operations are LIFO. Asynchronous command terminal events must carry job IDs and not pop whatever current provider operation is on the stack. |
| Process UI bytes | `src/terminal.cpp:1116-1138,1454-1455` coalesces adjacent process chunks without identity | Concurrent jobs cannot share unattributed output. Add job-ID-bearing presentation, and define whether background output appears inline or only through chosen attachment. Audit originals remain exact either way. |
| Busy/idle draining | `src/terminal.cpp:1507-1508`; `src/main.cpp:953-967,979-995`; provider binding `src/coding.cpp:1595-1619` | Background capture must be drained during later inference/tool waits as well as idle. A quiet provider produces no callbacks for arbitrary periods. Plain getline has no idle pump. Bounded memory cannot rely on future user input. |
| Engine shutdown | `src/main.cpp:397-405`; `src/participants.cpp:595-609` | Job workers must shut down and retain final observations before UI destruction on normal, error, quit, session-switch, and restart paths. No output callback may reference a destroyed UI. |
| Session switch/refit | `src/coding.cpp:2137-2144`; `src/main.cpp:644-686`; `src/session.cpp:206-220` | Switch currently refuses active participants only. Restart additionally refuses unresolved retained attempts. Extend switch/refit to active jobs and unsettled terminal retention; keep external shared services outside this governor. |
| Backstop/quiescence | `include/blackbird/process_lifetime.hpp:4-11`; `src/coding.cpp:2164-2178`; context repair `src/coding.cpp:1868-1870` | owned_children and sticky uncontained_exec prevent claiming local quiescence after arbitrary exec. Backgrounding does not authorize clearing this fence. |
| Crash recovery | `src/coding.cpp:962-1049`; `src/retained_state.cpp:1889-1909` | Recovery verifier allows provider/colleague/participant operations and deliberately refuses unresolved exec. An ordinary crash cannot reattach an unauthenticated PID or redispatch launch. Specify command abandoned/unknown behavior and continued recovery fences explicitly. |
| Checkpoint/reopen | `src/retained_state.cpp:2043-2102,2139-2152`; `src/main.cpp:373-384` | Checkpoint includes unresolved attempts and causal closure, but compact coverage marker requires no unresolved work. Long-lived jobs can keep recovery on full path. Preserve command state/originals without pretending settled compact coverage. |
| Binary persistence | `include/blackbird/retained_events.hpp:136-189`; `src/packet.cpp:371-399` | Owned Value and BBM2 native packets already express running/settling/terminal observations. Append-only packet word IDs cannot be reordered. New retained job records can use native application packets or deliberate typed event additions. No production JSON state/IPC. |

## Candidate interface responsibilities

Names below describe required responsibilities, not final tool spelling.

1. **Command owner**: an engine/session-owned job registry holds Child, original
   launch attempt and invocation, immutable arguments, sticky command deadline,
   task binding, independent cancellation and ordered output references. State
   includes running, leader-exited/draining, cleanup/settling, and terminal plus
   outcome certainty. Attachment is a separate state. A successful launch result
   is not a successful command outcome.
2. **Launch/admit**: allocate stable ID before dispatch, retain complete invocation
   binding, reserve resources, open once, then spawn. Validate errors cannot spawn.
   A spawn/worker-creation failure after admission needs an honest outcome.
3. **Wait/foreground**: references existing job only; uses an independent wait
   deadline and cursor/output budget; returns snapshot/output offsets and either
   running or observed terminal result. It never reconstructs argv and relaunches.
4. **Background**: releases the currently selected foreground wait without setting
   cancellation or destroying custody. An explicit target/job epoch prevents a
   control issued for a finishing command from affecting a subsequent command.
   Busy operator request reaches native mailbox, not queued Lua/turn admission.
5. **Inspect/read**: discover jobs and their stable IDs, attachment, active deadline,
   task binding, observed output extent and retained terminal result. Retrieval
   identifies exact bytes and does not imply completion from an empty slice.
6. **Cancel**: requests stop for exact job; success means request accepted, not
   terminal cleanup completed or effects undone. Repeated stop is idempotent.
   Foreground cancellation policy and turn cancellation policy must be specified.
7. **Drain/settle**: worker I/O queues bounded immutable capture/status events;
   owner thread commits originals and terminal observation under launch attempt.
   Publish retained completion, not speculative terminal worker flags. Complete
   only once after required stream/custody observations. Capture failure requests
   stop, records bounded unknown outcome where possible, and never redispatches.
8. **Shutdown/recovery**: explicit teardown joins workers before owner/UI destruction.
   Restart/switch refuse live or unretained command settlement. Process death marks
   old custody lost; records alone do not identify a still-owned OS process. If
   cross-process continuity is desired, it requires a distinct supervisor protocol
   and its own identity/custody design, not PID storage.

The existing `operation` function may gain an explicit deferred-command result
path, leaving the launch attempt nonterminal while returning the provider tool
response. Alternatively launch control can be a short operation linked to a
separately admitted command effect. Either must keep one logical launch identity
and distinguish terminal waiter/control operations from the live command. Avoid
special-casing `state == running` without changing effect/result/task/UI semantics.

## Ownership alternatives under pressure

**A: keep original exec attempt open until real command settlement.** Launch
adapter returns a running handle; the engine records a nonterminal launch result
and returns that handle to Lua/provider, while the command manager later retains
the actual command result and terminal observation under the same attempt. This
fits the existing observation state machine and preserves unresolved-effect
fences. It requires an explicit native deferred result contract; success of
dispatch/receipt is not command success. `operation.result` needs distinguishable
launch-response versus terminal-result semantics so output discovery/audit does
not mistake the first record for completion. TaskRun stack destructor must not
emit outcome unknown for deliberate handoff. Synchronous UI operation start/end
must still pair for the returned call without labeling command completed.

Advantages: one existing causal effect identity; generic unresolved/restart
rules remain useful; old output_ref remains the command attempt. Costs: engine
must own settlement beyond a turn; generic operation completion logic splits
into control-return and effect-terminal paths; live command checkpoints prevent
the current settled compact marker.

The principal failures to falsify are early terminality, duplicate terminal
observations when an attached wait races owner drain, reserve disappearing at
turn boundary, and mutation of a returned provider call output after completion.
Command completion is a new retained fact, not an edit to that earlier output.
The command manager must decide terminality centrally; waiters only consume it.

**B: process_start is terminal once spawn is observed; separate retained job
lifecycle represents running command.** The process_start operation's explicit
semantics are only that launch transferred into custody. It returns stable job_id
equal to or bound to the launch attempt. Asynchronous job admission/spawn/output/
exit/EOF/result records describe the still-live command independently. A terminal
success on process_start must never be described as command success, and ordinary
exec cannot quietly inherit the changed meaning.

Advantages: synchronous operation machinery remains simpler; launch/control
operations can all be ordinary short-lived operations; a separately modeled
job can express attachment/drain/custody states directly. Costs: job activity
becomes a second authoritative effect lifecycle outside unresolved_attempts;
restart, switch, refit, repair, backstop and saved-state coverage must all consult
it. The launch-to-lifecycle atomicity gap becomes critical. Persist pending job
state before spawn and treat crash before observed spawn publication as unknown,
not not-started. Terminal reserve is still required for asynchronous job records;
B does not remove that ownership/capacity problem.

The strongest objection to B is falsely passing existing generic quiescence
tests because process_start settled successfully while a command still runs.
If selected, the separate lifecycle must carry a recovery fence even when every
RetainedState attempt is terminal. The compact coverage marker cannot claim
settled-no-station while an active/unknown job is omitted. It must validate job
state, retain its source linkages, and prevent terminal job results from being
fabricated by a snapshot. Model-generated task state is not this lifecycle.

For the parent's selected first scope, jobs belong to one process lifetime;
restart/session switch are refused while active or settlement is unretained;
crash loses custody and leaves unknown, with no PID adoption and no launch replay.
A has the smaller semantic gap against today's unresolved-effect machinery.
Neither option gives cross-process custody, terminal raw attachment or PTY
behavior automatically. Those each require explicit implementation and direct
tests. The operator's optional PTY question remains pending; full raw attach
should keep one persistent collector and change foreground attachment only.

## Scrutiny of the proposed owner pump

The parent proposes A with an engine-owned aggregate SettlementScope and native
mailbox; owner drain runs during provider cancellation/stream callbacks, tool
cancellation callbacks, Lua instruction hook and idle UI. This is feasible only
with the following concrete invariants:

- Every credit refresh includes pending command terminal allowance. Current
  `refresh_settlement` replaces credit (`src/retained_state.cpp:1139-1149`), while
  settlement consumes it (`1394-1396`). A workflow refresh cannot overwrite live
  job reserves with context/depth alone. Avoid double-counting a launch as both
  synchronous depth and outstanding job. Reserve before spawn; release only after
  retained terminal outcome and actual custody cleanup.
- At next `turn`, reuse the engine scope instead of calling protect_settlement
  while a scope already exists. Job-only idle state, failed workflow cleanup and
  engine destructor also need scope ownership. Snapshot counts cannot discard
  jobs merely because their worker has finished but terminal retention is pending.
- Owner pump must check actual thread ownership and have a reentrancy guard.
  It must not invoke ordinary `operation`, call cancellation callbacks recursively,
  or run while root is in a transaction. Workers queue immutable bytes only.
- `Runtime::interrupt_hook` (`src/coding.cpp:525-530`) assumes cancelled callback
  returns bool. A new drain can fail with journal/IO/capacity/allocation errors.
  Do not throw C++ exceptions across Lua hook/C frames. Catch into Runtime failure
  state and raise Lua error with no owning automatic objects across the longjmp,
  following the bridge's existing pattern (`src/coding.cpp:510-523`).
- Pumping from a cancellation predicate changes its contract: callers currently
  use it before launch and in destructor-adjacent paths. Keep it bounded and
  nonblocking. Declared failure signaling cannot return false and silently continue
  effects after capture retention failed.
- Turn resets capacity_stopped and unretained_bytes (`2247-2248`); persistent job
  capture failure must stay in the job/engine owner and cannot be cleared by the
  next unrelated turn. Stop requests and terminal queue entries remain sticky.
- Provider request pins context head (`src/coding.cpp:1583,1741-1743`). Drain may
  append log/program facts; it must not inject/edit context behind that live request.
  Asynchronous completion cannot rewrite an already-returned function_call_output.
- A quiet provider must still call the periodic cancellation predicate or a new
  explicit tick. A custom CodingProvider implementation is not automatically
  cooperative; document and test its hook contract. Blocking file/tool operations
  without cancellation callbacks also need a declared capture backpressure policy.
- `/bg` on busy UI queues an exact-target release action; `/jobs` may read a locked
  immutable projection, but neither calls mutable engine APIs from the UI thread.
  `/fg` is a waiter against stable identity; finish/background races are decided by
  owner transition order, not a UI boolean sampled from a later turn.

Fixture/schema paths needing direct updates are `tests/tools_test.cpp`,
`tests/coding_test.cpp`, `tests/terminal_test.cpp`, a new job-owned native suite,
`tests/session_recovery_test.cpp` and `tests/participant_recovery_test.cpp` for
nonregression of existing fences, `tests/saved_state_test.cpp` for live closure,
`tests/context_budget_test.cpp`/retained-state reservation cases, and actual PTY
drivers modeled on `scripts/check-participants-pty` and `scripts/check-new-session-pty`.
Product declarations span `src/tools.cpp:788-840`, the Lua convenience surface,
terminal command/help tables (`src/terminal.cpp:35-116,437`), and user guide Lua/tool
sections. Static/dynamic test selection should follow changed fault classes;
these paths are integration targets, not a demand to rerun unrelated assurance.

## Capacity and time ownership

`CodingEngine::turn` creates a workflow SettlementScope (`src/coding.cpp:2251-2254`).
`protect_workflow` sizes extra credit by synchronous operation_depth
(`src/coding.cpp:1053-1066`). A background job can outlive both. Terminal recording
credit must follow outstanding owned jobs through workflow return and later turns.
Current root reservation is singular (`include/blackbird/retained_state.hpp:116-119`),
so independent per-job scopes are not an implementation shortcut. Account for
aggregate jobs and current workflow without double reservations or releasing a
job's credit on waiter return.

The collector's hard retained-output cap is separate from requested visible
output_max_bytes (`tools.cpp:214-215,256-270`). Specify spill-to-owned-storage,
backpressure or explicit stop-and-unknown when capture allowance is exhausted.
Silently truncating originals contradicts retained-output behavior. Every byte
must have a deliberate owner; avoid resident concatenated copies in Child,
queued captures, registry row, result, and full snapshot simultaneously.

Process timeout is a launch-relative lifetime policy; wait timeout is a presentation
boundary; background is neither timeout nor process stop. Repeated foregrounding
must not restart process lifetime clocks. PID/group custody persists across waits
until cleanup. Child uses steady_clock deadlines; keep that choice for duration.

## Direct red fault oracles

These are claims against actual behavior, not coverage targets. Existing fixtures
provide useful starting patterns: `tests/tools_test.cpp:21-78` validates merged
output/nonzero exits/binary clipping/no dispatch for invalid inputs;
`tests/coding_test.cpp:407-478` uses an external X marker to establish single
execution and exact retained output across reopen. Participant and TUI drivers
already use deterministic blocking transports and real PTYs. Add command-specific
oracles before implementation rather than merely extending generic no-crash cases.

| Fault class | Independent direct oracle |
| --- | --- |
| Return destroys process | Fixture writes START, blocks on owned IPC, then writes END and exits known code. Background return leaves it alive; foreground references same PID/job, exact marker contains STARTEND once. |
| Redispatch during foreground | Fixture increments external launch counter exactly once. Arbitrarily many waits/bg/fg/read/cancel repeat controls never increment it. Admitted/open launch count is exactly one. |
| Early settlement | Retained root.attempt(original_launch) remains nonterminal after background and wait timeout; waiter operation may be terminal. After release there is exactly one terminal command observation linked to correct retained result. |
| Wrong target at completion boundary | Deterministic schedule delivers background/cancel for job A while A ends and B begins. B continues; stale request returns stale/finished response, never targets B implicitly. |
| Output loss/duplication | IPC-gated byte sequence includes NUL, invalid UTF-8 and split multibyte sequences. Concatenated retained chunks equal exact generated bytes once across many detach/reattach boundaries; slices/cursors have declared offsets. |
| Concurrent output misattribution | Two fixtures emit distinct deterministic binary streams interleaved by gates. Per-job retained bytes remain exact; displayed lines identify selected owner; provider stack is unchanged by other job completion. |
| Leader versus stream lifetime | Leader exits while same-group descendant holds/writes pipe; inverse fixture closes stdout then continues. Completion follows declared leader/drain/cleanup policy, never EOF-only or leader-only inference. |
| Wait timeout versus process timeout | Wait expires while fixture stays alive. Repeated waits do not extend launch lifetime deadline. Explicit command timeout requests cleanup and reports effect certainty truthfully. |
| Cancellation reset | Background A, start later turn resetting engine.cancelled, stop A, then another turn reset. A's sticky job stop persists; unrelated B is unaffected. |
| Retention failure/capacity | Inject capture refusal/terminal refusal at existing journal fault boundary. No byte marked retained without actual source; command stops appropriately; launch never replayed; bounded unknown observation or preserved recovery fence remains. |
| No owner pump while busy | Produce beyond queue capacity while fake provider blocks without callbacks. Native owner drain/backpressure policy behaves as specified, memory stays deliberately bounded, UI can still background foreground wait. |
| Task versus runtime | Bound command stays live in task pane after background; task status/version does not change from execution. Completion and cancel update only runtime for same launch; task archive removes derived badge safely. |
| Teardown lifetimes | Normal quit, cancellation, error, restart attempt, session switch and engine destruction each leave no owned worker/FD/zombie; callbacks cannot reach destroyed UI. Check real process status and FD counts, not just return code. |
| Crash and replay | Crash at admission/open/spawn/output/settlement boundaries, reopen journal, assert launch marker count stays one, old PID is never adopted/signalled by identity alone, unknown custody remains explicit and unsafe admission stays fenced. |
| Busy TUI command path | Real PTY with blocked exec; operator background action returns useful control status promptly, admits no extra model turn, leaves command alive, composer responsive; foreground operation later waits same command and receives same terminal result. |
| Static lifetime/thread errors | Changed native units under required lint; ASan/UBSan for callback/FD ownership and bounds; TSan where qualified for native mailbox, shutdown and completion races. These catch separate fault classes from semantic model tests. |

Use an owned small deterministic command-job model for operation sequences and
completion races, with a fake clock/child adapter only where that makes the real
state transitions observable. Exercise actual POSIX process behavior with real
fixture subprocesses on qualified Mac/Linux profiles. The model must assert single
launch, independent attachment, stable deadlines, nonterminal return, ordered
output and terminality exactly once. OS tests must attack group/EOF/signal/reap
assumptions the model cannot validate. If PTY semantics are selected, add actual
tcgetpgrp/stdin/stop-continue/window-size oracles; pipe tests cannot qualify them.

Keep the operator's bounded hardening instructions: one remediation layer and one
recheck/fixes, with direct affected checks and declared allowance. Mutation work,
if used, runs on the fleet only; no laptop mutants. The present report performs no
tests because it changes no product behavior and claims no implementation outcome.
