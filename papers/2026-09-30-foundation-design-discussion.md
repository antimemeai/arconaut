# Foundation design discussion: live control with independent resource ownership

2026-09-30. Reviewed research/discussion candidate, originally drafted as
docs/DESIGN.md. The operator subsequently clarified the phase: take the brief as
far as useful, review and discuss it, then write the specification in pen from the
ground up. This paper preserves a concrete proposal and its alternatives without
adopting architecture, language, exact contracts, or implementation sequence.
The [working brief](../docs/BEHAVIOR.md) defines the discussion problem;
[FOUNDATION](../docs/FOUNDATION.md) preserves the operator's original intent.
Mechanism feasibility and model ergonomics remain research obligations.

**Subsequent operator correction: the resident-computation ownership premise below
is superseded.** Kernels/databases/computation can be services shared by an army
of Arconauts. Arconaut is their consumer/customer; the eventual meta-project fabric
governs them and supplies control-plane/scaling. See the
[current scope decision](../docs/FOUNDATION.md#discussion-decision-arconaut-consumes-the-computational-fabric)
and [working brief](../docs/BEHAVIOR.md). The body is retained as a pre-clarification
research candidate, not a current architecture recommendation. Its local process
custody/child-wait questions apply only where a harness actually owns local work;
they are not prerequisites for reconnecting to externally governed services.
Client/workload identity, actual exchanges, reconnection, and shared-state observations
need specification instead. Earlier reviews resolved faults in that earlier candidate;
they do not approve the corrected scope or select its runtime.

The operator also superseded the candidate's next-decision activation default:
changes normally wait for current turn/affected workflows to conclude, with an
explicit combined interrupt-and-apply-now action for immediate transition. See the
[current activation decision](../docs/FOUNDATION.md#discussion-decision-defer-changes-until-current-work-concludes).
The hot-change table below is retained as the earlier proposal, not current intent.

## Recommendation and evidence

Use a **live compiled Common Lisp controller on SBCL**, a **small independent
Rust resource/audit service**, and **separate resident program hosts**. Offer
Python as an early workflow binding and persistent computational host alongside
ordinary commands and direct model tools. Keep those surfaces on shared operation
semantics. Use SQLite provisionally for original captured history and the small
operational ledger; derived analysis lives outside its critical recording path.

The language choices are provisional recommendations. Confidence is higher in
the ownership boundary: the process being rebuilt cannot exclusively own a
standing program whose actual resident state must survive that rebuild. The
operator's outpost/refit proposal makes compiled controller replacement practical
to investigate, while live compiled definitions make ordinary policy experiments
possible without a full refit for every edit.

This recommendation integrates three focused studies:

- [Runtime and replacement](../papers/2026-09-30-runtime-design-study.md): SBCL
  compile/load machinery, retained process ownership, observed-pause limits,
  competing substrates, and bounded feasibility checks.
- [Complete audit and standing state](../papers/2026-09-30-audit-state-design-study.md):
  recording/admission, originals versus views, fault behavior, uncertainty, and
  transactional storage.
- [Executable workflow semantics](../papers/2026-09-30-workflow-design-study.md):
  shared operations, hot applicability, live messages, context versions, and
  direct ergonomics/scenario oracles.

Their evidence is pinned source inspection and primary documentation. No reference
product, prototype harness, runtime benchmark, provider request, or model comparison
was executed. These studies support a candidate architecture, not a claim that it
already meets its contracts. The source-based studies, their limits, and original
literature remain available in papers.

## Responsibilities and physical lifetimes

| Component | Responsibility | Main-controller refit |
| --- | --- | --- |
| Resource/audit service | Own resident processes, terminals/pipes, input/output capture, operation admission, authoritative lifecycle and conversation-owner ledger, durable originals | Remains alive; preserves paused standing hosts and attachment endpoints |
| SBCL controller | Participant decision policies, context construction, compaction orchestration, provider/tool semantic bindings, workflow composition, interpretation of events | Quiesces, exports explicit working data, and is replaced |
| Resident program hosts | Actual heaps/frames of kernels and long-running workflows; languages chosen for the work | Remain alive and paused; resume as the same instances |
| Provider drivers | Translate versioned request/tool bindings; execute exchanges through independently controlled transport workers/handles | Main requests settle before refit; semantic definitions may live in controller, active transport cannot depend on a responsive policy frame |
| Clients | Operator interaction, CLI/scripts, model-facing operation bindings, inspection | Reconnect to service/controller; client lifetime does not define job lifetime |
| Refit outpost | Handed-off conversation, compilation/diagnosis, accumulated return data | Continues independently while the main controller is absent |
| Independent OS daemons | Operator-managed uninterrupted services | Outside harness pause scope |

The service has no turn loop, prompt-building policy, model routing preference,
experiment judge, or per-command approval business logic. Its stable boundary
concerns resources, recording, admission, and truthful observations. It can use
native threads/readiness mechanisms without putting every participant behind
one busy workflow or terminal client. Slow analysis and arbitrary user programs
run outside that critical path.

The controller is a systems program with live compiled definitions. Python is
an optional executable surface and host, not the owner of the entire control
plane. A Python kernel that blocks on foreign code must not prevent independent
inspection, message acceptance, pause, or refit preparation. Long-lived Lisp
workflows follow the same lifetime rule: their resident frames belong in retained
hosts if preservation through main replacement is promised.

Before dispatch, each provider request registers an independent transport lifecycle
handle with the owner. Closing/cancelling it cannot require a blocked SBCL policy
frame or Python workflow to cooperate. Protocol semantics may be controller code;
the active exchange has a controlled worker/transport boundary. The owner retains
capture and observed local completion independently. Closing that boundary settles
the local request without claiming the remote provider's computation was cancelled.
The exact HTTP/stream implementation remains a feasibility obligation; accepting
a control message alone cannot establish this independence.

This is initially a local arrangement. Environment identity and operation semantics
extend to separate hosts; a remote deployment stack, cluster scheduler, discovery
fabric, and universal distributed consensus protocol are outside this design slice.
Outposts are peers or capability environments, including potentially model-free
ones. Their breadth does not require designing each possible application now.

## Core data and time

Use distinct logical identities for participants, rooms/conversations, environments,
workflows, operation submissions, execution attempts, resident resource instances,
context revisions, definition generations, and process incarnations. Labels help
readability but never identify a recreated object as an existing one. A handle
names its environment and resource incarnation; reattachment and recreation have
different results.

An operation's lifecycle and knowledge about its external effects are separate.
For example, a local provider stream can close while the remote outcome remains
unknown. Retain both facts. Distinguish submission accepted, attempt started,
partial output, observed terminal outcome, cancellation requested, and unresolved
effect. Never turn a lost connection into an invented successful cancellation.

Each source stream carries sequence/byte offsets. The local journal has a durable
commit cursor; causal references connect events. Neither that cursor nor a wall
timestamp establishes physical order between concurrent external events. Record
wall-clock observations and monotonic observations with their process incarnation.
Comparisons identify the clock domain and host incarnation; a process-local elapsed
timer's origin is not assumed restored after executable restart.
Pause intervals remain explicit; wall time and external deadlines keep advancing.

State changes use expected revisions. Two editors changing the same definition,
context, or conversation owner cannot silently overwrite one another's unseen work.
Rejected/conflicting proposals remain useful study material when capture is healthy.

Conversation-owner epochs govern execution, not just labels. Every supported
conversation action/continuation carries its owner epoch; admission and actual
dispatch reject retired epochs. Ownership transfer first closes the outgoing
owner's admission and settles its admitted dispatches/requests. A recorded owner
change alone cannot revoke an already authorized external action. Late observations
remain attributed to their original attempts and can be retained without granting
the retired owner permission to continue the conversation.

## One operation surface, several useful interfaces

Provide shared semantic operations for discovery/description, invocation, inspection,
stream reading, waiting, messaging, lifecycle control, definition activation, context
transformation, and state transactions. A CLI, Lisp call, Python binding, or direct
model tool is another client of that surface. They share identity, arguments,
definition selection, environment, observed outcomes, and errors.

Common coding actions are readily available with concise examples. Deeper discovery
does not require loading every possible operation into each model request, but
ordinary work must not pay a schema-discovery round trip for every familiar call.
Descriptors explain effects, lifetime, arguments, results, and supported controls.
Large outputs have useful previews and a reference/cursor into retained originals.
An error identifies the failed stage, relevant object/argument, partial results,
and available diagnostic material.

Invocation returns an operation handle; synchronous conveniences await that same
operation. Repeating a client submission identity queries the same submission
when its content agrees. Deliberately invoking identical arguments again creates
a new operation. This is not result caching and does not promise exactly-once
external effects. A conflicting reuse of an identity is an explicit error.

Waiting is observation by default. Cancelling a waiting client does not terminate
the job being watched. A race returns named completed/pending members and the
condition that resolved it; pending work remains alive unless the caller selected
a loser disposition. Cancellation, pause, and resume are requested operations
whose observed outcomes are inspected separately. Ending a turn does not implicitly
kill standing work.

Control access follows standing operator configuration. The same operations are
available to the model and operator without a new approval step for each invocation.
Useful observations and interruption controls remain accessible while work runs.
This is an execution architecture, not a new security sandbox or a tamper-proof
claim about code that deliberately bypasses its supported interfaces.

## Executable turns and workflows

A turn program chooses initiation, participants/models, contexts, operations,
continuations, waits, branching, error handling, and stopping. Provide a useful
ordinary coding policy, then make its behavior inspectable and replaceable.
Workflow programs can loop, race, send messages, run programs, call models, wait
on external input, and invoke other workflows. They are not limited to static DAGs.

A controller decision receives explicit current state and selected event identities
and returns actions/continuation state. The interface is the change boundary, not
a prohibition on arbitrary computation. Long-running arbitrary workflow frames
belong in a resident host; short controller decisions must not hide standing state
that is promised to survive controller replacement.

Each supported decision has a durable identity, base state/context/policy versions,
and selected message/event identities. Commit its prepared continuation state and
stable planned-action identities before those actions can be admitted or dispatched.
Action attempts/results link back to that decision; recovery queries those identities
and resumes the committed state rather than recomputing the old decision under new
submission IDs. A conflicting base rejects publication before its planned effects.
Messages arriving after selection stay pending. Inclusion in a model input remains
an explicitly observed fact, not merely a decision-selection acknowledgement.

This is a recovery unit for supported step decisions, not automatic durable replay
of arbitrary Lisp/Python frames. Preparing the step's proposal is effect-free at
the core boundary; imperative shared-operation calls are individually admitted work
with their own identities and recorded effects. A host or policy without recoverable
continuation state stops for explicit reconciliation after loss. It is not restarted
from an earlier frame to recreate handles by repeating completed external actions.

Compiled Lisp policy is the initial controller implementation surface. Python is
an early ordinary-workflow binding and kernel option. Both can call the shared
operations; the controller's compile/load/inspect/activate operations are available
to human and model clients too. A model must be able to change the actual policy,
not merely send parameters to an inaccessible Lisp engine. Other executable hosts
can later implement explicit decision/state contracts when they add useful power.

A stuck policy may prevent its own participant's next decision; it cannot block
independent service control or make a pending change look active. Known harness
self-waits and wait cycles can be reported, but arbitrary program deadlock cannot
be universally diagnosed. Timeouts and inspection remain useful controls.

## Hot change and actual applicability

| Changed object | Initial activation boundary | Existing work |
| --- | --- | --- |
| Turn/continuation policy | Next explicit controller decision | Existing requests/operations retain admitted definitions; new decisions use new generation |
| Model/provider/options/context construction | Next request admission | Sent requests keep actual serialized input and captured configuration |
| Tool implementation or schema | New binding selection or explicitly versioned direct call | Calls returned from an earlier request use that request's original binding, or fail explicitly if unavailable |
| Named reusable function | Future lookup through its versioned entry point | Old frames/closures may retain old definitions |
| Stateful workflow/tool representation | Declared migration boundary | Supported isolated conversion prepares replacement state; failed publication preserves old owned state; arbitrary side effects need reconciliation |
| Context transformation | Publication against expected context base before a later request | A sent request keeps its context; newly arrived messages stay pending |
| Native executable | Quiescent refit | No active main request/program crosses refit; standing hosts stay alive paused |

Prepare new definitions in a separate generation/namespace and publish a new entry
point against the expected old generation at a declared boundary. Retain source,
load diagnostics, effective dependencies/configuration, and activation. An atomic
entry-point publication does not make arbitrary Lisp top-level side effects atomic.
Unrestricted live evaluation is available as programming, but its partial effects
and loaded definitions must be represented honestly.

CLOS redefinition and Lisp compile/load are useful mechanisms, not automatic
transactional state migration. The preserve-old-state guarantee applies to supported
isolated/transactional conversion of owned state: prepare new state without mutating
the committed instance before success. A migration/evaluation that changes a shared
database, kernel, or external world can fail after real effects; record those effects
and require explicit reconciliation or a failed/unusable disposition. An unchanged
old object alone does not undo them. Arbitrary suspended Python/Lisp stacks
do not become a new continuation because a module was reloaded. Unsupported
in-place change applies to future work or requires a designed yield/restart/refit.

Old binding retention matters. If request R advertised a tool returning 0–10,
and a new definition returns 0–100 before R responds, its call cannot silently
acquire the new meaning. Keep R's binding or return an explicit unavailable-version
error and let the current policy decide whether to replan. A source revision
label alone cannot identify which definition actually governed that execution.

## Rooms and live communication

A room is an addressing/history facility, not one shared model context or a
supervision tree. Participants can join/leave, talk to peers, and contribute while
other work runs. They retain their own context and control program. Messages
include origin environment and reply references where useful.

Acceptance means durable responsibility under the audit contract; a busy target
is not a rejection condition. Availability means the recipient controller can
observe the message. Inclusion names a subsequent context/request or decision
that incorporated it. Neither implies that the model understood or acted on it.
Actual action is a separate observation. Multi-recipient messages have per-recipient
facts, without pretending simultaneous consumption.

Each decision selects specific mailbox identities. Concurrent later arrivals stay
pending; no whole-queue clearing after context construction. A policy can use
the message in its next request, change its next action, or explicitly interrupt
and restart an active request. Newly arrived data cannot alter already-sent bytes.
Default receipt does not automatically cancel work.

The default coding workflow can use isolated worktrees for simultaneous changes
and ordinary version-control integration. Shared files and other coordination
styles remain expressible. Their real conflicts are observations to handle, not
an assumed universal automatic merge. Context transfer to a peer is a named
selection; later communication does not require continuously identical contexts.

## Context and managed compaction

A context revision is an ordered selection/transformation of retained material
for one participant. Record selected messages/artifacts/byte ranges, transformations,
definition versions, and the actual provider input produced. Kernel heaps, standing
databases, program instances, and the complete captured history live independently.

Initial proposed strategies are explicit selection/rebuilding, externalization
with useful retrieval, and model-produced summary. A context fork is useful for
experiments and peers. These strategies share transformation/publication semantics;
their detailed algorithms need evaluation on continued work. The default context
policy keeps provider-required call/result structure valid.

Every ordinary provider admission validates the supported adapter's structural
constraints, including tool-call/result obligations; validity is not only a default
policy preference. Explicit raw-protocol experiments identify themselves as such
and retain their actual input/outcomes, rather than silently disabling ordinary
request validation.

Each in-flight request retains its own continuation branch and pending protocol
obligations. If request R started from context A and compaction publishes B, R's
later response/tool calls/results stay linked to R's branch and original bindings.
Do not blindly append those results to B. A policy chooses the branch to continue
or constructs an explicit next context preserving required call/result structure.
Intervening peer messages stay pending until placed at a valid boundary. A discarded
branch retains originals and the disposition of its pending calls; selecting B does
not silently cancel them. No automatic semantic merge is assumed.

Transforming a named base produces a candidate revision, original inputs/output,
strategy and diagnostics. A model summarizer's request is ordinary recorded work.
Publish against the expected base before a later provider request. If that base
changed, retain the candidate as a branch, retry, or explicitly combine the new
material; do not overwrite intervening changes. Mailbox acceptance and context
publication do not themselves mark messages included.

Failure preserves the earlier usable revision and failed attempt. Returning to
an earlier revision affects subsequent input; it does not undo external effects
or delete later history. Compaction never implicitly prunes a kernel, resets a
database, destroys a program, or rewrites another participant's context. Retrieval
operations make externalized originals and still-live resources discoverable.

## Original audit, operational ledger, and derived study state

The audit service captures its actual mediated boundary: participant ingress/egress;
final provider request representation and actual received response bytes; program
arguments/environment/cwd, stdin/output, terminal input, observed lifecycle;
files/artifacts read or written through core operations; definition/configuration
changes and effective activations; context construction/compaction; and experiment
interventions/results. Capture failed, cancelled, rejected, and abandoned work too.

Original program output is bytes, including invalid text and partial lines. Retain
each stream's observed order separately from a merged display. Original provider
capture occurs at the final serialization/received-byte boundary exposed by the
selected driver, before parsing or model-facing transformation. Capturing an SDK
argument object alone is insufficient. Opaque SDK retries or transformations cannot
be advertised as fully observed exchanges; compliant drivers expose each attempt.

This boundary does not observe provider internals, arbitrary external activity,
or every file/network action inside an uninstrumented subprocess. A launched shell
does not grant a complete syscall history. Observed files/edits/artifacts and tool
inputs/outputs remain original study material; broader process tracing would need
an explicit feature and cost model. Coverage boundaries must stay visible.

Keep three distinct stores of meaning:

1. Original captured history: events and original payload chunks. Corrections
   append new facts; history survives context changes and derived-view rebuilds.
2. Authoritative operational ledger: live resource/effect states, revisions,
   admission and conversation-owner epochs. Its mutations commit with the event
   explaining them.
3. Derived research/query state: indexed views, annotations, summaries, and active
   context. Each identifies source cursor/revisions and derivation. Conclusions
   remain attributed claims, rather than replacements for observations.

SQLite is the first proposed local storage engine. Put event, original chunks,
and operational transition in one transaction. A single writer simplifies that
boundary; raw inspection uses bounded read transactions. Use a separate rebuildable
derived database for prolonged analysis, with its applied source cursor committed
alongside each projection change. SQL reading/querying can be expressive; operational
mutation uses versioned transactions rather than rewriting original history.

Query requests and actual returned results retain their source cursor/derivation
and immutable original representation before ordinary exposure. An audit
watch selects workload events while excluding bookkeeping for its own delivery
and other audit observations/deliveries from every default automatic workload feed.
All such bookkeeping remains retained and explicitly queryable at a finite source
cursor. Excluding only a watch's own delivery would allow two watches to feed each
other indefinitely, so the exclusion applies globally to that event category.
Thus observation stays inspectable without making every delivered audit event
recursively generate another event that the same subscription must deliver.
Operational decisions consult the authoritative ledger. A derived view identifies
its applied cursor; a caller needing a newer cursor waits or receives an explicit
not-yet-materialized result, rather than treating stale analytics as current state.

Propose WAL plus FULL synchronization, checking the actual VFS/filesystem and macOS
flush behavior. Pin an appropriate current SQLite build; do not assume a system
library or requested pragma supplies the needed contract. WAL storage is local,
not a shared network-filesystem database. Choose chunk/batch bounds from direct
capture/latency observations, not invented throughput guarantees. Consistent
database-aware backups are recovery material, not a second audit certificate.

Retain secret-bearing originals with reversible authenticated encryption of raw
payloads before they enter database pages/WAL, using a reviewed library. Ordinary
views may suppress credentials while referencing the retained original and their
transformation. Raw inspection remains an explicit shared operation under standing
authority, without a per-call approval prompt. Key rotation preserves old access;
key loss or deliberate omission is explicit loss of original accessibility. Exact
key handling is a focused design obligation, not a new approval system.

## Recording and effects

Normal execution follows **record before dispatch; record before publication**.
Admission commits the proposal/input and authoritative state before acknowledgement.
Each external attempt commits final resolved input and attempt-open before dispatch.
Captured bytes/outcomes become ordinary model/operator observations only after
durable capture. Stream batching is permitted; queuing is not durable publication.

A crash after attempt-open can leave an effect unstarted, partial, or complete.
Missing completion therefore means unknown outcome, not permission to replay.
Reconciliation or an operation-specific retry is a new recorded decision. A
durable local database cannot make an arbitrary external effect atomic with it.

Bounded queues apply backpressure. They never sample away original chunks or prune
failed experiments. High-water limits stop new admission; slow/full/unavailable
storage enters a visible recording-failure state. Preserve queued captured material
and pause supported producers where possible. Verify transaction state after a
failed commit rather than blindly retrying a statement.

Emergency pause/cancel/disconnect remains possible when recording is broken. Attempt
capture; if impossible, expose the action as unaudited and later append a clearly
retrospective record of what is known. This is degraded control, not normal unaudited
continuation. Do not terminate standing programs merely to solve recording/refit.
Automatic deletion of originals is not a capacity strategy under the stated intent.

There is a real interval between reading an OS/network byte and committing it.
A host crash can lose an unconfirmed tail. Non-replayable producers cannot fill
that gap afterward. The claim is retained originals for acknowledged capture and
truthful interrupted coverage, not zero-loss knowledge of every external byte.
A gap marker does not replace missing material. The direct completeness oracle
compares independently known emitted input/output against originals.

The service's own failure differs from controller replacement: retained children
may continue and attachment descriptors may disappear. Do not assert automatic
pause/preservation on service crash without an independent mechanism. Main refit
keeps that owner alive. Owner replacement needs separate resource/parentage/wait
and stream handoff evidence; transferring a descriptor alone is insufficient.

## Refit as a quiescent operation

The operator's rule is binding: no active main-harness programs/provider requests
at refit; standing programs stay alive paused; uninterrupted work belongs in
independent OS daemons. This protocol applies those rules:

1. Identify the main refit cohort: its controller, active requests/commands, and
   harness standing hosts whose preservation is required. Resource-service ownership
   alone does not make a process a member: the refit outpost and independent peer
   work have distinct attribution. Close this cohort's effect admission at a
   recorded epoch, including queued starts and resumes. Retain message ingress,
   observation and refit/emergency controls; allow independently owned outpost
   compilation/chat and candidate startup/reattachment. Finish or explicitly settle
   current commands/provider requests. Cancellation requested is not settled.
   Preserve any unknown external outcome as an obligation, rather than claiming
   the remote world has stopped.
2. Establish actual pause of every standing host in that cohort and preserve its identity,
   heap, terminals/pipes, and stream position. Ask cooperative hosts to reach idle
   boundaries where necessary, then observe the supported pause condition. Drain
   available output without inventing a stream-end. Failure means refit has not begun.
3. Export explicit main working data: participant/context/definition references,
   pending messages/obligations, and retained-resource inventory. Transfer the
   conversation execution-owner epoch to the independent outpost. Messages arriving
   during transition remain retained and assigned once.
4. Build/start a candidate while the outpost owns conversation and repair. Capture
   build inputs under the declared build contract, actual output/artifact, failures,
   and the returned conversation. No main provider stream or active program is
   migrated. A new window is an interface option, not an architectural dependency.
5. A candidate reports its actual artifact and protocol/data versions, imports the
   working data, and reattaches the same paused resources. Incompatible state,
   failed startup, or failed reconnect leaves outpost ownership and pause intact.
6. Close the outpost's admission for the handed-off conversation and settle its
   admitted conversation requests/program actions before return. Other separately
   identified peers or capability work are distinct. Reconcile pending messages
   and final outpost additions, then transfer ownership to the usable successor
   under the dispatch epoch gate. After successor ownership, working state and
   attachments are accepted, reopen main effect admission and reconnect operator
   clients, then explicitly resume preserved work. Record individual resume
   outcomes; partial failure is not reported as a
   completely successful return. Rollback/recovery uses real state and loaded
   versions; it does not undo outpost external effects.

The first proposed refit uses a local independent outpost and the surviving local
resource/audit owner for the dispatch/ownership boundary. Remote collaborating
peers have distinct execution identities. A future remote conversation handoff
would need its own cross-environment ownership/failure protocol; this design does
not imply that a local ledger change fences an arbitrary remote process.

Pause is an observed state. s6's inspected pause flag follows a sent signal and
is insufficient evidence. Linux cgroup freezing and macOS stopped-child observation
offer different mechanisms and limits; arbitrary descendant containment is not
assumed. A standing host whose whole pause scope cannot be established must report
that condition. No automatic termination or recreation stands in for the promised
pause/preservation. Exact supported macOS/Linux mechanisms require direct checks.

SBCL image saving is not the refit strategy: it does not preserve arbitrary live
threads or OS streams. Export explicit controller data and keep resident work in
its actual hosts. Updating the resource/audit service requires its own demonstrated
custodian/successor handoff; the proposed first refit contract does not pretend that
case is already solved. Every component remains within autodroit's eventual scope.

## Autodroit as ordinary research on executable behavior

Use the same operations and original audit for joint human/model improvement.
Name the failure/hypothesis, changed surface, baseline/candidate lineage, actual
effective definitions, task conditions, model configuration, evaluator, outcomes,
and relevant costs. Explore with ordinary programs and standing databases/kernels.
Retain rejected/abandoned interventions and their actual results.

Keep/reverse decisions use direct task outcomes appropriate to the hypothesis.
Reevaluate composed changes; individually useful edits need not combine usefully.
Repeated adaptive selection spends evaluation independence. A final independent
assessment, when needed for the claim, must be treated accordingly. Changing audit
or evaluator behavior is itself an intervention with comparability consequences.

Hot policy edits, including live compiled Lisp definitions, use declared activation;
controller/native executable replacement uses quiescent refit.
Reverting a source file is not reversing effective loaded code, migrated state,
or external actions. Inspect actual post-reversal behavior. No fixed optimizer,
automatic score threshold, training algorithm, or new approval ritual is imposed.
The autoresearch/GEPA/Self-Harness studies supply mechanisms and limitations, not
automatic evidence that an Arconaut improvement generalizes.

## Alternatives and unresolved decisions

| Choice | Proposed direction | Why / what could change it |
| --- | --- | --- |
| Main environment | SBCL live compiled Lisp | Compile/load/introspection fits in-situ policy edits; model repair/ergonomics and provider packaging remain untested |
| Resource/audit owner | Narrow Rust program | Explicit resource ownership and compiler checks suit this boundary; no inherited crate adopted and no measured Rust superiority claimed |
| Early executable workflow binding | Python plus direct tools/CLI | Useful current code-action ecosystem; must show actual model task success and semantic parity, not assume familiarity guarantees it |
| Original storage | Local SQLite transactional events/chunks/ledger | One publication/recovery boundary; actual capture load, durability settings and key handling need direct checks |
| Native fallback | Rebuildable Rust main with real hot policy surface | Refit makes it viable; rebuilding for every policy edit would miss live-change intent |
| Other systems alternatives | BEAM/OTP, Scheme, C++ | Meaningful concurrency/live programming mechanisms; no current Arconaut-specific result outweighs the proposed fit; C++ remains open to Rhizome evidence |

JS/TS and Go are not proposed core substrates, and JVM remains excluded. We do
not discard mechanisms learned from their reference implementations. Language
selection follows actual continuity, operational fit, model effectiveness, and
maintenance burden. SBCL's lack of an installed local executable is a current
feasibility limit, not a reason to pretend the relevant checks have passed.

Before an implementation plan, resolve these concrete obligations:

1. Live compiled policy loading/publication, failed-load effects, explicit state
   conversion, and model access to the actual responsible code.
2. Main replacement with verified pause and same-instance return on supported
   macOS/Linux arrangements, including failed build/startup/reconnect.
3. Owner replacement or an explicit first-version limitation, including child
   parentage/wait rights and ongoing capture descriptors.
4. Provider/stream serialization and packaging fit for the candidate control
   environment, using controlled fixtures before any live integration.
5. Direct audit completeness/admission/failure checks at bounded load, including
   original retrieval and protected-record/key accessibility.
6. Model ergonomics for real composition/repair tasks through Lisp, Python and
   direct operations; fixtures establish protocols but cannot establish model skill.

The [working brief](../docs/BEHAVIOR.md) supplies proposed direct scenario outcomes.
Discuss the brief and these alternatives before writing the authoritative
specification from basic entities/lifetimes/time/effects upward. Feasibility work
should answer questions that change those decisions. Only the later reviewed
specification leads to an implementation plan. The full June implementation remains
history/quarantine; selecting concrete adaptations comes after those contracts,
not from old phase names or a desire to keep its layout.
