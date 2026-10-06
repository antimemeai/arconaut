# Arconaut first core specification

2026-10-01. First-core specification; adversarial review findings resolved. It develops the
operator's [foundation decisions](FOUNDATION.md), [scenarios](BEHAVIOR.md) and
[starting sketch](STARTING_DESIGN.md). The first core includes CLM. The
[research readiness assessment](../papers/2026-10-01-design-readiness.md) supplies
the mechanism evidence and remaining qualification subjects. Nothing here claims
an implemented or qualified runtime; external runtime/library adoption remains open.
Review and resolution: [admission/context](../papers/2026-10-01-core-context-review.md)
and [code/resource custody](../papers/2026-10-01-core-lifetime-review.md).

## Purpose and scope

The completion claim is real development of Arconaut inside Arconaut, through a
usable terminal conversation and ordinary tools, with programmable turns and
context, original audit, native replacement and outpost refit. One provider and
one operator are sufficient initially. Identities and operations must already
accommodate independently working colleagues; a globally alternating chat loop or
parent/child agent tree is not the foundation model.

C++ owns execution and continuity mechanisms. Lua programs define tools, workflows,
turns, context assembly, transformations and interface behavior. The model can
write and modify either during normal work. Autodroit supplies an experimental
working style, not additional editing authority. Routine operations use standing
operator authority without per-call approval.

Shared kernels/databases/computation are consumed services. The core neither
governs them nor pauses them during refit. No legacy implementation is selected.
The default native mechanism proposed here is an owned narrow generation loader;
RCC++ remains a comparator or integration candidate. Actual adoption follows
qualification and operator discussion of concrete benefit/cost.

## Runtime ownership

An independent C++ custodian owns the admitted-operation ledger, original audit
writer, harness-owned child identity/status collection and execution admission.
Replaceable participant runtimes hold Lua VMs, working context programs and model
interaction policy. The terminal is a client. An outpost is an independently
runnable participant runtime using a retained executable and its own provider
access; it must not depend on the candidate executable being rebuilt.

The custodian has one owner for ledger transitions and publication. Blocking work
and independently running participants can proceed concurrently. A Lua VM has one
execution owner; background completions enqueue values for that owner rather than
calling Lua concurrently. No presentation loop owns overall execution.

Operations pass through owned boundaries for identity/capture. Expert programs may
use ordinary OS interfaces, but the audit cannot invent unobserved internal effects.
Any extension that bypasses a capture boundary must expose that coverage limit.
This is an observation contract, not a sandbox or an approval workflow.

## Identities and time

Distinct types represent environment, participant, conversation, workflow, operation
attempt, process incarnation, context revision, definition generation and audit
stream/sequence. Handles carry an owning environment/incarnation and are checked
at use. A recycled PID, file descriptor or Lua registry slot is not a durable ID.

Record wall-clock observations and local monotonic time with their process
incarnation. Deadlines and elapsed intervals use the appropriate monotonic domain;
do not compare unrelated monotonic clocks across processes/restarts as one timeline.
Audit sequence orders publication to its writer. Explicit causal links and per-stream
offsets express other known order. Receipt order is not global physical execution order.
Pause does not stop external deadlines or wall time.

## Operation lifecycle

Each logical invocation may have several distinct attempts. Every external dispatch
has its own attempt ID and retained input; retry never reuses an attempt as though
the earlier effect did not happen. Supported operations expose start/observe/await,
input where meaningful, cancellation, and explicit final disposition.

| Local phase | Meaning and allowed transition |
| --- | --- |
| Prepared | Input, actor, environment, generation and relevant context are resolved; no dispatch yet |
| Admitted | The attempt/input record has crossed the specified durable audit boundary; it may be dispatched once |
| Running | Dispatch occurred or may have occurred; observations and cancellation requests are separately recorded |
| Settling | Execution stopped or interruption requested, but callbacks, streams or owned resources still need disposition |
| Terminal | Local execution/callback/stream obligations have concluded; outcome is success, failure, cancellation, or explicitly unknown |

Rejected preparation/validation and undispatched abandoned admission remain in the
audit. Admission is not proof of dispatch. A crash between dispatch and its receipt
leaves uncertainty; recovery does not automatically repeat the effect.

Supported effectful workflow steps durably retain a decision/checkpoint, its planned
logical invocation identities and continuation disposition before dispatch. Repeated
submission of the same invocation/attempt and identical input returns existing state;
different input is a conflict. A deliberate retry creates a recorded new attempt
under that invocation. Recovery resumes from the retained decision, not a new action
identity invented by rerunning the Lua frame. Arbitrary VM stacks are not serialized:
lost uncheckpointed execution requires reconciliation or an explicit recorded restart,
with prior effects exposed. The core cannot deduplicate independently invented actions.

The effect boundary also applies to later stdin writes, signals, messages and additional
provider requests. Record each intended effect before issuing it, fence its authority
at dispatch, and retain actual partial-write/delivery outcomes separately. An admitted
process does not authorize unaudited later input.

For a process, child exit and each captured stream's EOF/error are separate facts.
Descendants retaining a pipe may keep it open after the original child exits.
Cancellation request, signal delivery, observed stop, exit and settled cancellation
are distinct. For a provider, transport closure does not prove server-side cancellation.
Record local cleanup and external outcome separately; terminal-unknown releases
local resources only after their actual settlement, and never asserts that remote
work was undone. Unsupported controls return an explicit error.

## Original audit and durability

Original captured bytes include model request/response representations at the final
observable transport boundary, process streams/input, participant communication,
file operations through the core, submitted/loaded definitions, context changes,
failed/rejected attempts and refit/experiment activity. Parsed results, displays,
search indexes and active context are derived data. Captured originals are never
replaced by summaries, placeholders or deletion markers.

The proposed initial store is a single-writer, framed append journal. Keep payload
chunks inline initially, avoiding a second blob-publication transaction. Frames
carry format version, sequence, kind, payload length and an accidental-corruption
checksum; bounded chunks carry stream identity/offsets and explicit logical endings.
Large observations are chunked rather than silently clipped. Queries address
retained bytes by journal identity and sequence/range; indexes are rebuildable.
This does not claim protection against deliberate tampering.

The writer handles short writes/EINTR. Appended means bytes submitted to storage,
not authoritative publication. A batch contains its source frames and semantic
transitions, followed by a commit frame identifying their sequence range and checksum.
Committed means that complete batch, including its commit frame, crossed the selected
successful sync boundary. Only then may the single owner publish ledger/context-head/
authority changes or acknowledge them to callers. Durably acknowledged means that
acknowledgement was returned; acknowledgement delivery is not inferable from disk.
The format and sync profile are qualified in the storage implementation unit.

Recovery can discover complete unacknowledged batches; their presence does not prove
the caller received an acknowledgement, nor whether an admitted effect was dispatched.
Incomplete batches remain diagnostic originals, never authoritative transitions.
All ordinary context consumers use committed sources; provisional bytes may be displayed
with their status but may not silently enter published context. A later committed batch
may depend only on earlier committed material or sources in that same atomic batch.
On the initial Mac profile, stronger power-loss
claims require the selected full-sync primitive and filesystem/device fault domain;
no silent weaker fallback is permitted under that claim. Before external dispatch,
the attempt and exact input must be durably acknowledged. Batching observations is
allowed with an explicit committed cursor; presentation before commit is provisional.
Context publication may reference only committed originals. New journal/segment
names require the directory durability specified by the supported backend.

Recovery preserves the acknowledged committed prefix under the declared fault
model, exposes torn/uncommitted tails and interior corruption, and never silently
skips missing material while declaring the audit complete. Interrupted appends and
sync failures have explicit unknown durability. Preserve damaged available bytes
for diagnosis; new admission stays closed until a trustworthy continuation is chosen.
Writer startup must reconcile live operations rather than treating the journal as
evidence of exclusive authority over an already living worker.

Recording failure closes normal admission and reports the failure. Already admitted
producers follow bounded buffering, backpressure and supported pause/termination
disposition; if bytes are lost, the gap and its limits are reported on recovery.
There is no lossless-capture promise after arbitrary storage exhaustion. Emergency
stop/detach controls remain usable and identify any interval that could not be recorded.
Capture secret-bearing originals under deliberately restricted local storage; ordinary
views may transform presentation while retaining the original and transformation.

## CLM as a core data operation

Every participant has a working context with immutable published revisions and an
editable local representation. Entries retain structure: role/presentation labels,
content blocks, tool call/result linkage, attachments, environment attribution and
source references where supplied. Extensible metadata and model-authored synthetic
material are allowed. Editing presentation cannot rewrite original provenance.
Opaque provider-specific blocks must not be flattened into reconstructed text.

The model and operator can select, reorder, replace, remove and restore context,
and write reusable arbitrary transformations in Lua or ordinary programs. The
operation set is not limited to predefined compaction strategies. A candidate
edit includes its base revision. Validate its representation, record the candidate
and result, then publish only if the base is still current. Conflicts preserve the
candidate and identify the current revision; no silent last-writer overwrite.
Invalid/missing/partial external files are observable rejected edits, not new context.

The editable file is a versioned workspace view. Its base is the revision actually
exported to the editing operation, retained separately from mutable file contents;
never infer it from the head at readback or a watcher event. Core file operations publish at
their explicit edit boundary. For ordinary external editors/commands, reconcile
after their completion and before the next provider assembly; do not pretend a
watcher observes every intermediate write. A failed reconciliation does not silently
authorize a new model call with unexpected context. The workflow chooses repair,
explicit reuse of the last published revision, or another visible disposition.

Ordinary context editing takes effect within the running program before subsequent
requests. Replacing its governing program follows generation activation below.
Requests already dispatched retain their actual immutable inputs. An edit cannot
remove the operation ledger's outstanding obligations or undo prior effects.

Responses and tool results belong to the originating request's continuation branch,
revision and generation. That branch retains structural call/result obligations apart
from editable presentation. Incorporation into a working head is an explicit context
operation with its own base/CAS and retained outcome. Conflict preserves both branch
and candidate; it neither drops the completion nor silently rebases onto a newer head.
The workflow selects, forks or incorporates branches and concurrent messages before
assembling another supported request. Context selection cannot settle real effects.

Provider assembly binds a request to a context revision, assembly-program generation,
model/provider configuration and tool definitions. Record both the structured source
and final serialized request after adapter transformations, with retry attempt identity.
Unsupported roles/blocks or protocol-invalid tool linkage fail visibly unless an
explicit recorded transformation maps them. Token/budget decisions are program policy;
repair may grow context. Never silently prune to fit.

Repair retrieves retained originals/prior revisions and publishes another revision
with its actual sources and output, keeping the flawed revision. Each colleague
has its own lineage; sharing selected context creates explicit copy/branch/source
relations rather than aliasing every participant's mutable file.

Context-management evolution is available in the first core: write and use a
transformation, inspect resulting context/request/task outcome, revise the program,
and compare recorded runs. Retain candidate/effective programs, task/evaluator inputs,
results, observed cost and selection rationale. Changing the judge is a separate
experimental change. Do not equate bytes removed, edit count or a reused selection
set with improvement/generalization. Weight training and specialized serving are
later consumed capabilities, not prerequisites for this programmatic evolution.

## Definition and native generations

A generation is a retained source/artifact/configuration snapshot and resolved
dependencies, not the latest bytes in the checkout. Candidate preparation, checking,
pending publication, activation, supersession and retirement have distinct outcomes.
Running turns/workflows retain their governing generation through conclusion.
Late dynamic imports through the supported loader resolve within that generation.

Changes normally activate after every currently affected turn/workflow concludes.
New affected root turns/workflows wait once activation is pending, preventing an endless
stream of new old-generation work from starving publication. Already admitted roots
continue to admit their descendants under retained old definitions until conclusion;
the custodian checks actual root ancestry, not an unchecked supplied label. Unrelated work proceeds.
Applicability/dependencies are explicit. Interrupt-and-apply-now closes affected
root and ordinary descendant admission, interrupts/settles its active work, then activates; inability to settle
leaves the candidate pending with reasons. This is timing, not permission to edit.

Lua candidates use distinct environments/module caches. Declaration construction
is separated from effectful execution; a failed load must not be mislabeled as an
atomic rollback of arbitrary top-level side effects. Ephemeral code evaluation is
an ordinary identified operation with its own snapshot, not implicit replacement
of the parent's governing program.

The proposed initial native boundary is a versioned C-compatible function table
with checked handles, byte spans/lengths, explicit result/error values and ownership
rules. No C++ exception or allocation ownership crosses accidentally. Lua-visible
callbacks enter stable foundation trampolines; native continuations/objects/threads
hold explicit generation custody. Candidate modules have distinct artifact paths
and explicit state migration. Active objects are not destroyed while old work uses
them. Interface changes outside the supported versioned boundary require refit.

A failed compile/check/load/migration does not publish the candidate. Native faults
are process faults, not recoverable Lua errors; no signal/longjmp rollback of heap
state or external effects is promised. Prepublication checking in an isolated worker
can contain checking faults but cannot prove fault-free future execution.

Default migration preparation builds independent candidate state without mutating or
consuming old custody or dispatching external effects. Once ready, the defined activation
transition assigns candidate ownership; failure beforehand leaves old state usable.
An explicitly destructive/effectful migration records its actual actions and failures.
If old state is no longer known usable, affected admission remains blocked with that
disposition; keeping the old module mapped is not rollback or permission to resume.

Retirement requires no active calls, callbacks, coroutine continuation, Lua finalizer,
native object destructor, worker thread or external queued obligation that can enter
the generation. Cleanup may execute old code. Unload occurs from surviving code
after returning from the last such call, not from its own destructor/finalizer.
Expose pinned generations and reasons; bound retained generations by refusing further
activation at a configured limit rather than unloading live code. A standing unit can
pin an old generation until completion or an explicit supported migration.

## Lua embedding

Qualify the selected Lua build's C++ exception mode and exported symbol linkage
together. Preparation that allocates must run behind a protected boundary. Native
business-call exception conversion must not encompass Lua API operations, whose
private control transfer implements errors and yields. No noexcept path may let
such transfer escape. Preserve nonstring errors, traceback where available and
allocation-failure diagnostics through a minimal native reporting path.

Coroutines are rooted VM objects; continuation state is owned data/operation state,
not a vanished C++ stack local. Validate binding shapes rather than accepting incidental
numeric/string coercions. Async buffers outlive the call through native owners.
Finalizers perform local cleanup; they neither settle a live provider/process nor
govern a consumed service. VM close and warnings are observable operations, including
failed cleanup/resurrection. The [Lua source study](../papers/2026-10-01-lua-embedding-grounding.md)
defines the relevant mechanism limits and upstream oracle examples.

## Outpost refit

Refit uses the states open, draining, pausing, handed-off, building, candidate-ready,
returning and open again, with explicit failed-build/start paths. Stop admitting
new main work, settle active main programs/provider client operations, then confirm
its known standing workers paused. Historical unknown effects remain obligations;
they are not automatically retried or asserted cancelled. A still-active local
ordinary request/callback/stream obligation blocks refit. Preserved standing workers
may retain open capture pipes and buffered passive observations in surviving custody;
those are not active main execution and must not be forced to EOF. Their pause contract
excludes progressing owned callbacks, requests and child work, and defines who captures
already-buffered bytes while paused.

Refit and interrupt requests are custodian-owned control operations. They return an
accepted/pending identity rather than requiring the initiating tool continuation to
remain alive until completion. The caller must conclude, transfer or explicitly end
that continuation before its root can settle. Observation, settlement, complaint and
minimal transition controls remain available when ordinary admission is closed; this
exception does not admit arbitrary new ordinary effects during quiescence.

Only a defined harness-owned worker set is eligible for preservation. Each worker
retains its Lua VM and required native objects/code, or talks to a surviving owner
over a compatible protocol. Child-stop observations or a qualified cooperative
checkpoint establish the chosen pause contract. No arbitrary-process-tree suspension
is promised on macOS. An unpausable workload blocks refit; the operator can arrange
uninterrupted work as independently governed daemons.

The custodian gates conversation execution with an owner epoch. Commit exclusive
handoff to the independently ready outpost before it admits conversation actions;
stale-epoch requests fail. The outpost continues conversation and compilation with
its own participant identity and original capture. Main is quiescent; outpost work
is separately identified. Physical descriptors, observation duties and exclusive
action authority are separate inventories. One writer/reader owner per captured
stream prevents competing readers from consuming different bytes.

The outpost can recover failed builds/startups without the candidate runtime. A
successor proves the expected artifact/protocol. Return first closes new outpost
conversation roots and ordinary descendant effects, settles its active local work,
reconciles context edits and queued inputs, and fences queued stale dispatch at the
effect boundary. Return stays pending if settlement fails. Separately identified peer
work can continue without inheriting this conversation. Then the successor receives accumulated conversation,
context revisions, pending messages and known/unknown obligations, then gets the new
owner epoch. Resume the same standing workers after reattachment; do not recreate
their heaps under old names. Refit does not pause shared services or other consumers.

Replacing the physical custodian is a separate protocol, not implicit in replacing
main. Descriptor passing does not move child parentage, wait rights or buffered
transport state. Preserve those duties in a surviving parent/observer or perform a
separately specified compatible image replacement. Until qualified, block custodian
replacement while promised live custody remains; do not claim it is generally hot
replaceable. This limitation does not make its source permanently immutable.

## Complaints and operator interface

Rageshake has one local complaint ID and automatically captures available context,
generations, pending changes, operation state, diagnostics and audit references.
It remains available from normal workflows, low-memory failure paths and the
outpost where possible. The complaint opens a bead and is tracked in an external
database table. Each sink's delivered/pending/failed/unknown outcome is recorded.
Retry must reconcile ambiguous prior delivery or use a sink's actual idempotency
contract; do not invent exactly-once behavior. Reporting does not oblige immediate
investigation. Follow-up remains tracked for the first suitable repair opportunity.

The baby TUI is an owned C++ client with Lua-configurable commands/layout: conversation,
multiline composer/history/paste, output detail and compact status. It exposes CLM
revision/edit/repair and final-input inspection, queued prompts, pending definitions,
interruption/application and refit attachment. Substantial editing can use the ordinary
editor. Bindings require supported terminal encodings and fallback gestures.

Client disconnection/rebuild does not cancel harness work. Reattach by conversation
and audit cursor; preserve submitted input identities so reconnection cannot submit
it twice. Unsubmitted drafts are client state with explicit save/restore behavior.
Specify the initial terminal profile, Unicode editing/display widths, fragmented
input, escape ambiguity, paste, resize and restoration. Raw control bytes are retained
in the audit and transformed for display rather than passed blindly into UI control.

## Acceptance and remaining qualification

The milestone exercise changes Arconaut source in its own conversation, runs checks,
publishes a Lua tool/workflow, writes/uses/evolves a context transformation, repairs
a seeded lossy context, activates a compiled native change, refits a foundation change,
recovers from failed build/start, resumes the same paused worker and delivers a
captured-state complaint to both sinks. Independently known emitted bytes/dispatches
and seeded task requirements supply the oracle; a coherent surviving log is insufficient.

Before corresponding implementation units depend on them, qualify: selected compiler/
standard library/instrumentation; Lua runtime/linkage/unwind; native loader retirement;
host pause/custody; journal durability/recovery and overload; initial provider/transport;
and terminal compatibility. Dependencies and runtime versions stay open until concrete
adoption discussion. Testing and implementation plans follow resolved design review.
