# Command jobs design/plan: adversarial engine review

2026-10-09. Reviewed `docs/COMMAND_JOBS.md` (137 lines) and
`docs/COMMAND_JOBS_SUBPLAN.md` (27 lines) against the current-source map and actual
engine/retention/UI paths. This is the requested pre-implementation design/plan
challenge, not implementation assurance. Product code unchanged; no provider
traffic, dependency adoption or tests executed.

The chosen ownership design A is coherent: original exec remains open; a returned
tool result closes a waiter without claiming process completion; the engine owns
custody and aggregate reserve across turns. The design already rules out PID
adoption/replay, UI mutation of RetainedState, attachment-induced cancellation,
deadline renewal and asynchronous LIFO operation-stack completion. The remaining
required corrections are below. Their oracles belong in subplan step 2.

## 1. Specify which terminal bytes can actually consume the reserve

**High; design lines 39-50, subplan step 4.** The design settles launch after
successful result retention, and reserves bounded terminal allowance, but current
reserved submission admits only AdapterReceipt and terminal AttemptObservation
(`src/retained_state.cpp:1152-1157`). `log_.original` sources/application records
consume ordinary capacity. Normal output/result source retention can therefore
fail precisely when terminal reserved capacity is the only room left. The phrase
successful result retention cannot be assumed to mean the ordinary result source
is protected. Capacity also has individual payload/batch limits; aggregate byte
and record reserve alone does not guarantee the bounded observation fits them.

**Correction:** declare exact terminal representation and recording order. Keep
full output in already retained chunk originals; terminal result must be bounded
metadata/exit evidence/locators, not a second complete stdout string. Either add a
deliberate reserved terminal-original facility, or encode the bounded terminal
result in the reserved observation itself and teach inspection to read it. When
normal result capture fails, retain an explicitly bounded unknown observation if
possible; otherwise preserve admission fence. Do not create a success locator for
an unretained source. State whether output capture refusal stops all owned jobs or
only affected job, and enforce that choice.

**Trigger/oracle:** job has retained prefix and then exits zero at ordinary
capacity exhaustion with reserve intact. Check actual source availability and
terminal bytes, not just returned status. Inject refusal after result source and
before terminal observation, plus max_payload/max_batch boundary cases. There is
one terminal observation or a retained unresolved fence; no missing locator,
fabricated complete output, or redispatch.

## 2. Make raw operator input part of admission-before-effect

**High; design lines 31-39, 74-89.** Model/Lua input/signal calls have admitted
attempts, but raw attached keyboard input is described as terminal relay through
a native mailbox. Without an explicit owner admission point, implementation can
let the worker write bytes before the audit records them. Ctrl-C is input with an
external signal effect through the PTY driver; raw relay is not exempt from the
owned interface's effect linkage. Resize has similar ordered control semantics.

**Correction:** UI queues an immutable exact-target control request; engine owner
retains its admission/input linkage before granting worker delivery. Receipt and
partial-delivery observation return through owner drain. Worker cannot treat
mailbox enqueue as permission to write an unaudited request. Exact chunking need
not match keypresses, but each admitted byte sequence and stream offset must be
defined. Resize coalescing records which requested state became deliverable.

Input delivery has partial writes: a request may write a prefix and then fail.
Distinguish accepted, observed applied prefix, delivered, and unknown. Repeated
delivery identity returns prior receipt/unknown and must not retry the prefix or
resignal. Identity/content conflicts compare target, operation and exact bytes,
not merely text. No cross-process replay follows the retained request.

**Trigger/oracle:** audit refusal before raw-input grant produces zero fixture
input bytes. Gate a worker write after N bytes, fail remaining delivery, repeat
same identity and then conflicting identity contents. Fixture receives prefix
once; duplicate signal counter increments once; retained evidence describes N or
unknown honestly. Ctrl-C also has exact retained input/control identity.

## 3. Bound delivery receipts and terminal retention debt, not only queues

**High; design lines 64-80, subplan steps 2-4.** Bounded input/event queues do not
bound deduplication receipts: a long-lived interactive command can receive an
arbitrary number of accepted input/signal delivery IDs while its current queue is
empty. Erasing receipts permits duplicate effects; retaining them unbounded
violates declared bounded memory. Similarly a completed worker whose terminal
retention failed remains a live debt even if its process is gone.

**Correction:** declare finite registry, queued-byte/event and receipt limits plus
refusal policy before effects. The runtime may use persisted receipt lookup or
ordered delivery sequences with a declared replay floor, but cannot silently
forget applied IDs. Bounded live-output history must not be the authoritative
input-dedupe ledger. Job active/pending counts include unretained terminal debt;
archive never discards that debt. State which collection limits have unit/time
meaning so red tests can calculate exact boundary values.

**Trigger/oracle:** fill dedupe capacity on one still-live command while draining
every input. Next new request is refused before bytes/signals; an old duplicate
still cannot repeat effects. A worker finishes with terminal retention failure;
restart, switch and archive remain refused, and its reserve remains owned.

## 4. Specify pump failure translation and transaction exclusion concretely

**High; design lines 113-117, subplan step 4.** Existing Lua interrupt hook calls
`engine.cancelled()` directly (`src/coding.cpp:525-530`). A callback that drains
can throw Error/bad_alloc while the Lua VM is in a C hook. Saying never throws
across a hook is the right invariant, but the plan needs an actual error route.
Silently returning false after a failed drain would continue effects without
retention. Root rejects refresh/dispatch under an active transaction; a pump
cannot reenter writer mutation from an arbitrary cancellation check.

**Correction:** a bounded owner-only pump uses reentrancy/transaction exclusion.
Drain failures become sticky native failure state. Lua hook catches and transfers
that state into Runtime::failure, then raises Lua error outside owning automatic
C++ objects, following existing bridge handling (`coding.cpp:510-523`). Provider/
tool cancellation callbacks also need a specified stop/error result rather than
an accidental exception contract. Shutdown can request worker stop even when no
new audit write succeeds. Tests include an injected allocation error, not solely
capacity. State how quiet/custom providers supply ticks or encounter backpressure.

**Trigger/oracle:** force retain failure from Lua instruction hook, provider quiet
poll, tool poll, and idle UI drain. Check error recorded/fenced, no subsequent
fixture effect, worker stoppability, original failure preserved, and no exception
crossing Lua C frames. Reentrant cancellation/pump schedule never mutates root in
transaction or loops recursively.

## 5. Specify aggregate reserve transitions before implementing registry

**High; design lines 45-50, subplan steps 3-4.** Current `turn` always creates a
local SettlementScope (`coding.cpp:2254`); `refresh_settlement` replaces credit
(`retained_state.cpp:1139-1149`), and reserved writes consume it (`1394-1396`).
Therefore a new live job must not just add a fixed credit once and rely on existing
workflow refreshes. They would overwrite it or try to acquire a second scope.

**Correction:** write the aggregate accounting transition in the implementation
note before the registry unit: scope exists iff workflow or job debt exists;
reserve before launch; live-job debt is included in every workflow refresh;
failed spawn/failed worker creation remove debt only after truthful terminal
retention; waiter release removes no debt; completion removes its debt only after
retention and custody cleanup; scope survives workflow failure and ends at full
quiescence. Do not double count an exec as depth plus job debt unintentionally.
Preserve sticky job failure independently of `turn` resets at 2247-2248. Reserve
arithmetic must reject overflow before launch.

Subplan step 3 can implement/test pure registry mechanics, but no functioning
product launch path should bypass step 4's admission/reserve contract even
temporarily. The complete invariant remains under one owner.

**Trigger/oracle:** background A, start and fail another workflow, launch B, finish
A while nested operation active, then finish B. Inspect reserve at each model
transition and actual terminal-retention ability under tight capacity. Final scope
release occurs exactly once after both jobs and workflow debt vanish. Thread
construction/spawn/capture/result/terminal refusal schedules are included.

## 6. Clarify Ctrl-C, attached wait cancellation and queued later work

**Medium; design lines 42-43, 82-89.** The existing synchronous exec contract says
operator cancellation stops the turn and destroys group custody. New raw attached
Ctrl-C goes through private PTY driver, while `/bg` releases wait without cancelling.
Missing is the behavior of ordinary `/cancel`/quit during pipe foreground, PTY
foreground, or unrelated later inference with several background jobs. Original
tool timeout remains command timeout, while yield expiry is a wait boundary.

**Correction:** give a compact control table: Ctrl-B/background, Ctrl-C raw,
ordinary turn cancel, explicit job stop, quit, and lifetime/yield/wait deadline for
pipe and PTY states. Declare which exact job is stopped and which wait merely
returns. Busy default background captures job+wait epoch at admission; it never
resolves against a later active slot. Completion consumes returned bytes once.
Raw attach quit has an explicit route even though keystrokes normally relay.
Do not inject a queued model turn just to perform native release/inspection.

**Trigger/oracle:** A backgrounds; B foregrounds; stale background/cancel request
for A arrives at B's start; then Ctrl-C/ordinary cancel under each attachment
state. Check selected PIDs, unchanged A/B deadlines, pending prompt queue, exact
wait result, and original launch terminal dispositions.

## 7. Cover all teardown paths before UI destruction

**High; design lines 16-20, subplan steps 4-5.** Design explicitly addresses normal
shutdown. Existing main needs a guard because UI is declared after engine;
ParticipantShutdown precedes UI destruction (`main.cpp:397-405`). Engine destructor
alone runs after UI and is too late for callbacks. A failed audit cannot prevent
process cleanup/join by throwing before worker stop.

**Correction:** install job shutdown/observer-disconnection guard before UI
destruction on every normal/error path, not just `/quit`. Terminal retention may
fail; cleanup and join must still run, and no terminal success is published without
retention. Switch/restart checks include pending terminal debt, not only worker
running flags. Reopen remains fenced for unresolved exec and does not modify the
existing recovery whitelist to mark exec safe just to make session reopening easy.

**Trigger/oracle:** throw from worker retention/display callback and force main
error after starting a background job; verify child cleanup/worker join precede
UI destruction, no callback reaches destroyed UI, and unknown/unresolved evidence
is retained or original fence persists. Normal `/new`/restart while active are
refused without spawn/reexec; settled jobs permit them. Crash schedules never
relaunch or adopt PID.

## Required plan adjustment

Before step 2, incorporate the seven concrete rules above and their model oracles.
Keep step 3 pure registry/OS mechanics until admission/reserve integrates in step
4; do not publish partial product behavior in between. The current implementation
allowance is declared and bounded, and unsafe partial work remains inactive.
This report adds no further assurance layer or unrelated hardening task.
