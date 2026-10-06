# Arconaut working brief and scenarios

2026-09-30. Proposed behavior for the reconstruction, derived from the operator's
[stated intent](FOUNDATION.md) and the [fresh research](../papers/2026-09-30-frumentarii-synthesis.md).
This takes the brief far enough for review and discussion. The proposed behavior
below makes the questions concrete; it is not the specification in pen, implemented
behavior, or an implementation plan. The operator has since selected C++ with Lua
(2026-10-01); see the [starting design](STARTING_DESIGN.md) and FOUNDATION for
that decision and its still-open reload/runtime choices. The operator's sequence
is explicit: develop/review/discuss the brief, then specify from the ground up.
The subsequent [core specification](CORE_DESIGN.md) now supplies reviewed
ground-up contracts; this brief preserves their discussion/scenario provenance.
Distinguish direct operator requirements in FOUNDATION from proposed semantics here.

## Purpose and working environment

Arconaut is an expert operator's programmable coding environment. Human and model
participants can do ordinary repository work, compose workflows, communicate
while other work runs, inspect standing state, and improve the environment itself.
The operator can act directly through ordinary programs and interfaces; the model
has access to the same operations without needing a human to approve each call.
Standing operator configuration governs access and execution. Visibility and
intervention support directing work, with almost no routine approval ceremony.

The operator confirmed the premise during discussion. Programmability is available
at all times, and the model has substantial agency to configure and modify the
programs governing its work under normal operation. Autodroit ranges from a couple
of manual experiments to full model-governed autoresearch with operator steering.
It is tunable experimental behavior; ordinary model agency does not depend on
entering that mode.

Kernels, databases, and similar computational services can be shared by many
Arconauts. Arconaut is their consumer/customer. Their governance, control plane,
and scaling belong to the eventual meta-project fabric. A local service is still
a service; it is not automatically resident harness state. Client/workload handles
and service observations are part of Arconaut's work, while the service owns its
actual computation and lifecycle.

The harness supports both a useful default coding workflow and replacement of
that workflow. Neither a fixed provider/tool cycle, a static DAG, one language
kernel, nor a parent/child agent tree defines all possible work. A terminal is a
client of the environment; its event loop does not own the entire harness.

Outposts fit this purpose as separate working environments or capability services.
They may be local or remote, have another provider, or expose operations without
an active model. The present brief includes their identity, context, communication,
and refit roles without specifying a separate remote-deployment product.

## First milestone: build Arconaut in Arconaut

The operator set the first milestone: get to the point where we can develop
Arconaut inside Arconaut **as soon as possible**. This prioritizes an early useful
self-development loop. The ground-up specification and subsequent plan should
identify the smallest coherent foundation that can carry that work.

The operator agreed on 2026-09-30 to this completion scenario:

1. Conduct real Arconaut development through its own operator/model conversation:
   inspect and edit source, execute programs, observe output, and run relevant checks.
2. The model changes programs governing its work. Changes activate after the current
   turn or affected workflows conclude, with interrupt-and-apply-now available.
3. A compiled change goes through outpost refit: quiesce the main harness, build,
   activate, and resume development with conversation continuity. A failed build
   leaves operator and model able to diagnose and recover.
4. Original audit material survives throughout. The model can retrieve evidence,
   repair context, and file a rageshake with captured state and tracked follow-up.
5. CLM is in the first core (operator decision, 2026-10-01): the model directly edits
   its live context, defines and uses its own transformations, repairs a lossy edit
   from originals, and evolves those programs. Context revisions and exact final
   provider inputs remain independently observable alongside the original audit.

Any harness-owned standing work obeys the pause/preservation requirement; shared
computational services remain outside it. The milestone can consume available
services without first implementing fabric governance or scaling.

This settles the milestone completion scenario. Detailed interfaces, architecture,
and implementation order remain for the ground-up specification and reviewed plan.
Evaluate proposed foundation work by how it advances this milestone while supporting
the intended later composition.

## Common semantics

**Identity and attribution.** Distinguish people, model participants, conversations,
workflows, operation attempts, contexts, environments, and process incarnations.
A logical ongoing-work handle must say which live incarnation currently serves it.
Results carry their workspace/host identity. A path on one server is not implicitly
a path on another. Changing a participant's model or provider affects future
requests at an explicit boundary; it does not rewrite already-submitted work.

**Time and order.** Retain wall-clock observations for study and monotonic elapsed
time within a process incarnation. Causal links and source sequence establish
order where known. A collector's receipt order is useful but is not a global
physical execution order. Pausing a program does not pause wall time or external
deadlines. Paused time and active time must be distinguishable when measuring work.

**Operations.** Human, model, and programmatic clients discover and invoke the same
operations. Results include useful values and references; ongoing work has a handle
that can be inspected, awaited, communicated with, interrupted, or detached where
the operation supports it. Unsupported actions produce understandable errors.
Starting work, sending a message, and requesting cancellation are distinct from
completion, consumption, and settled cancellation. An effect whose outcome is
unknown remains unknown; automatic retry must not silently duplicate it.

**Definitions and applicability.** Effective code, configuration, workflow policy,
tool definitions, and context construction have inspectable versions. A changed
definition declares where it applies. By default it is pending until the affected
current turn/workflows conclude; an explicit interrupt-and-apply-now action brings
it forward. Migration and executable replacement have their own transitions. An existing request keeps
its captured input and governing definitions. Arbitrary live stacks are not claimed
to migrate just because a new function has been loaded.

**Context and service state.** A participant's active context is constructed local
data. Its captured audit and references are distinct from the state owned by kernels,
databases, and other consumed services. Context construction/compaction cannot silently
delete source material or reset a shared service. Referencing external data and giving
the model that data are distinct; useful operations retrieve it as needed. Service
observations retain their source/version where available; other consumers can change
state while this Arconaut is absent. The harness's ledger is authoritative for its
own admitted work, not for the global state of the computational fabric.

**Observation and audit.** Retain original captured requests, responses, messages,
program input/output, changes, transformation inputs/outputs, and experiment
material, including failures and abandoned work. Derived query views and active
context are replaceable projections. Define the capture boundary precisely:
unobserved provider internals and arbitrary external-world activity are not invented.
No silent clipping, coalescing, redaction, or compaction can stand in for retaining
original captured material. Secret-bearing originals need deliberate storage and
presentation handling. Recording loss is an operational failure with an explicit
disposition, rather than a successful run with a vague completeness claim.

The operator explicitly identified context repair and the accumulated research
dataset as audit purposes. The model can search/query original evidence, inspect
context transformation history, and recover omitted or misstated material into a
new working context. Useful repair operations are part of ordinary model agency.
Retain the flawed context and actual repair as study material; derived indexes,
annotations, and longitudinal datasets reference original observations.

**Model rageshake.** The operator requests a complaint operation available everywhere
in Arconaut. A model encountering frustration, a bug, or another problem can submit
a short report. The harness captures available agent state and relevant evidence,
opens a bead with that material, and tracks the complaint in a separate database
table elsewhere. The bead, tracking row and retained evidence share a complaint
identity. `LLMmmshake` is an illustrative name, not a selected command interface.

Capture can include active context/recent inputs, context transformation history,
effective and pending program/configuration versions, workflow/task state, current
operation handles/results, available runtime diagnostics, and audit references.
Detailed capture is automatic; the reporter does not need to assemble the state
bundle manually or investigate/fix in that moment. Retained snapshots identify
their observed state/time/cursor; they do not pretend shared service state was frozen.
The tracking table supports follow-up and longitudinal study. Repair should be
pursued at the first suitable opportunity, with its outcome tracked.

The operation remains readily reachable in different working modes and when the
normal workflow/context is faulty. Exact availability/failure and bead/table delivery
semantics need specification; accepted capture is distinguishable from successful
creation of both records. No new approval step is implied. Reporting and follow-up
are independent of the reporter's immediate task.

## 1. Ordinary repository work

**Situation.** The operator asks a model to understand a project, change one coherent
behavior, and run relevant checks. One command is slow, a check fails, and the
operator gives additional direction before the task completes.

**Required behavior.** The participant can read and edit files, run ordinary commands
with working directory and environment, consume output while it arrives, and inspect
exit status or signal outcomes. Long work does not prevent operator messages or
control operations from reaching the harness. The default workflow supports useful
continuation after tool results and failures, while retaining the operator's latest
direction. Interrupted or failed work is inspectable rather than erased. Normal
execution uses standing authority. Clients can reconnect without starting a second
copy of an existing operation.

When that work exposes a harness problem, the model can rageshake and continue
its task without taking on an immediate repair obligation. The complaint has enough
captured state and original evidence for later triage/repair.

**Direct observations.** Use a controlled repository with a concrete behavioral
oracle, a known output-producing program, and a directed operator message. Observe
the actual changed behavior, output bytes/outcome, message visibility, and whether
the intended follow-up action follows the message. A rendered terminal alone is
insufficient evidence of command completion or provider/tool correctness.

For rageshake, supply a controlled operation failure. A short complaint should
produce a bead and separate tracking row linked to the actual captured context,
configuration and operation evidence. Observe the reporter continuing the task,
then the complaint's subsequent repair/follow-up outcome. This is a future behavioral
oracle, not an issue or test run performed during the brief discussion.

## 2. A live room

**Situation.** The operator and three differently configured model participants work
on related tasks. A program streams output; participants arrive, leave, exchange
questions, and change priorities. One participant has a separate working environment.

**Required behavior.** Communication topology is independent of process supervision.
Peers can address one another or a room without being arranged into a fixed tree.
Messages can be accepted while a target is working. The interface distinguishes
accepted, delivered, included in a subsequent model input, and acted upon; it does
not promise that a running provider stream can consume a newly arrived message.
Each participant's context and request are separate. Scheduling policy can decide
whether to continue, interrupt, or start another request after the message.

Peers share observations and artifacts with their origin intact. Simultaneous
workspace edits must remain observable; the default coding workflow can use isolated
worktrees and ordinary version-control integration without making isolation the
only supported collaboration mode. No universal automatic merge or race-free shared
filesystem is assumed. A remote peer can receive selected context and communicate
back; packing a context does not imply ongoing synchronized copies.

**Direct observations.** Track known message identities through acceptance, context
construction, and the subsequent request; observe actual priority changes and output
availability while other work continues. Exercise simultaneous edits and inspect the
real filesystem/version-control outcome. Verify origin attribution for the separate
environment. Successful final reports alone do not establish live communication.

## 3. Redefine the turn during work

**Situation.** A serial workflow becomes a workflow that consults two models, receives
a review, and waits for operator input. A program and provider request were already
admitted under the earlier definitions.

**Required behavior.** A turn is programmable execution policy: initiation, participant
and context selection, operations, branching, waiting, continuation, and stopping.
Both operator and model can inspect and change it through shared operations. The
activation boundary is inspectable. Already-submitted requests retain their inputs;
already-started operations retain their declared execution semantics. By default
the current turn or affected workflows continue with their existing definitions
until conclusion, then the pending change governs subsequent work. An intermediate
provider response, tool result, or scheduling decision is not automatically that
conclusion; the running turn/workflow defines it.

Provide a combined interrupt-and-apply-now action to bring pending changes forward
without waiting for natural conclusion, analogous to immediately submitting queued
prompts. Control+Enter is an example, not a chosen binding. The same operation is
available to human and model, with pending/interrupting/applied status. Its scope is
the affected Arconaut turn/workflow/client activity, not global fabric services.
Actual transition/checkpoint and overlapping-workflow semantics need specification;
an interruption request alone cannot be reported as applied. Replacing a suspended
continuation or stateful workflow requires an explicit migration or restart disposition. Supported
step decisions need recoverable identity, continuation state, and planned action
identities before dispatch; a crash must not recompute earlier work under new
identities and silently duplicate its effects. Arbitrary lost frames require explicit
reconciliation rather than being replayed as though no effect happened.

Loading a new definition and rebuilding a controller are distinct operations.
Workflow code can launch programs, call models, exchange messages, branch, loop,
race, and handle errors. A blocking user workflow cannot prevent independent control
of the environment or overwrite its audit. The harness reports failures in terms
useful for repair, including the operation, governing revision, and retained inputs.

**Direct observations.** Arrange a known old/new policy disagreement. With a change
pending, observe the current turn/affected workflows completing under the old
definitions and subsequent work using the new ones. Separately exercise interrupt
and apply now: inspect the actual transition, retained request inputs/bindings,
partial effects and new effective revision. Include interruption that has not yet
settled and another consumer continuing to use a shared service. A reload or
interruption acknowledgement alone is not the oracle.

## 4. Managed compaction with standing resources

**Situation.** A participant has a large history, repository references, useful kernel
values, a standing database, and a program handle. The operator or model requests a
different context strategy and then continues work that needs earlier information.

**Required behavior.** Compaction is a named operation with an input context revision,
strategy, actual transformation inputs/outputs, and a new context revision. Strategies
may rebuild selection, externalize large data with useful retrieval, summarize,
fork, or combine these. The design must specify which strategies it initially offers;
this list is the scope of the inquiry rather than a promise to ship them all.

Retain the originals independently of the result. Do not silently prune kernel
values, remove standing database data, destroy a program, or change another
participant's context. A running request continues from its submitted input. A
prepared new context needs an explicit publication boundary; concurrent context
changes cannot overwrite each other unnoticed. Failure leaves the earlier usable
context and the failed attempt available. After compaction, the model can discover
and retrieve retained material through useful operations.

Include recovery from an overzealous or misleading compaction: the model can find
the original constraint, decision, or result, inspect how it was transformed, and
construct a repaired context without requiring the whole audit in every request.
Record the repair's selected sources, input/output, and effective context revision.
An earlier flawed summary is retained rather than silently corrected in history.

Late responses/tool results retain their request's continuation branch and required
call/result structure. They are not blindly appended to a newer compacted context;
the next context explicitly selects or incorporates that branch. Ordinary request
admission checks supported provider structure, including messages arriving while
the old request completes.

**Direct observations.** Continue a task whose correct result needs known earlier
information and live state. Compare actual model input before/after and retained
original bytes, then inspect the database/kernel/program directly. Test a conflicting
context update and failed transformation. Token reduction alone is insufficient.

Also supply a compacted context that omits or misstates a known operator requirement.
Have the model retrieve the original and repair the working context, then continue
the real task. Observe the requirement's actual satisfaction and retained repair
lineage; a plausible new summary alone does not establish recovery.

## 5. Refit and return

**Situation.** Autodroit changes compiled harness code. The operator initiates the
provisional `arcorefit`, transfers the conversation to an outpost, observes the build,
and returns to the replacement harness.

**Explicit operator rules.** At refit the main harness has no active programs or
provider requests. Its own harness-managed standing programs remain alive and paused.
Shared kernels/databases and fabric-owned computation are outside that scope and
remain under their provider's governance. Independently deployed OS daemons are
also outside the pause scope.

**Required behavior.** Stop main admission, settle active work, and establish actual
pause of its own standing programs before entering refit. A pause request alone is insufficient.
If settlement/pause cannot be established, report that refit has not begun. A paused
resident program is distinct from actively executing work. The external daemon case
is not silently included in main-harness suspension. Nor is a consumed shared service:
Arconaut settles its client activity and retains work/service references; it does
not stop the underlying service or another consumer's work.

The independent outpost has the required conversation data and provider access;
operator focus and responsibility for the handed-off conversation move to it.
Outpost chat and compilation are outside the quiescent main harness. The outpost can
continue during a failed build or failed candidate startup. The return carries the
outpost's accumulated conversation. Reconnect and resume the same harness-owned standing programs;
recreating a remembered variable or relaunching a named command is not preservation.
Reattach service clients using retained workload/session references and observe
the provider's current state. Service failure, replacement, or another consumer's
mutation remains visible; the harness does not promise custody of an external heap.
Keep explicit version and audit attribution across handoff/build/return/resume.
One active owner serves the handed-off conversation at a time; other separately
identified peers may work concurrently.

**Direct observations.** Observe no active main requests or commands at entry; direct
paused/alive status of each controlled standing program; handed-off message ownership;
build output; executable version after return; identical owned-program state after resume;
and the complete retained record. Include failure to pause, build failure, startup
failure, and reconnect failure. This scenario imposes no migration of active requests.
Include a second consumer using a shared service during this Arconaut's refit:
that consumer and service must remain unaffected by this harness's pause scope.

## 6. Audit a messy run

**Situation.** Provider and program streams interleave with participant messages,
failure, interruption, context transformation, and a definition change. Storage is
slow or unavailable during part of the run.

**Required behavior.** Originals and causal/version attribution remain available
independently of views and context. Outgoing effects and incoming observations need
an explicit recording/admission contract. Bounded buffers, storage exhaustion, and
writer failure need a disposition that preserves captured data where possible and
prevents unrecorded normal continuation. State exactly where crash or external
producer loss can leave capture incomplete. A gap marker acknowledges a loss; it
does not replace missing originals or establish complete capture.

Queryable state answers current-work questions and supports analysis without becoming
the sole surviving record. Replaying retained input may help reproduce computation;
it does not authorize resending an unknown-outcome provider request or command.
Results from a recorded run are distinguishable from new external effects.

**Direct observations.** Controlled producers provide independently known input/output.
Compare their emitted material with the original captured bytes and actual dispatched
inputs; inject a storage interruption and inspect admission, exposure, backlog, and
recovery. Separately check causal/source ordering and reconstruction of governing
versions. A self-consistent log does not establish completeness.

## 7. One autodroit improvement

**Situation.** Human and model use retained study material to identify a real harness
defect or workflow limitation, change the responsible behavior, and assess whether
the change improves work. The intervention may change definitions, context policy,
workflow, or compiled code.

**Required behavior.** They can inspect original evidence, formulate a concrete
hypothesis, alter actual executable behavior, run comparable evaluations, and keep,
revise, or reverse the change. Record candidate lineage, effective definitions,
model/provider configuration, task conditions, actual results, and relevant costs.
Use a direct behavioral outcome appropriate to the hypothesis; the changed model's
praise, quantity of edits, token count, or a cleaner transcript is insufficient.

The degree of automation is tunable: a few operator/model-directed experiments,
partially automated trials, or a full model-governed hypothesis/experiment/keep-or-
reverse loop. Operator steering can change aims, priorities, or experimental direction
throughout. Routine model configuration/modification remains available outside
autodroit; the mode does not create a new approval step for each intervention.

Keep exploratory trials distinct from independent final evaluation where applicable.
Repeated adaptive selection on the same cases spends their independence. Changes to
the evaluator or recording boundary are themselves interventions. Ordinary work can
continue around experiments under explicit ownership/version boundaries; compiled
refit follows scenario 5. Experiments inherit standing operator authority without a
new approval ritual. Fleet-only mutation rules remain current doctrine.

**Direct observations.** Choose a known defect with a direct outcome oracle, compare
the real before/after behavior under named conditions, and inspect the exact effective
candidate. Preserve failed trials. Check recovery/reversal of the actual behavior,
not just a source-file revert. Assess costs only in relation to successful work.

## Design handoff

These seven scenarios form one design problem. A proposed substrate must explain
how their identities, resources, change boundaries, and audit compose. One convenient
serial test cannot justify the whole architecture. Model ergonomics crosses every
scenario: useful discovery, understandable errors, handles, inspection, bounded
presentation with original retrieval, and expressive composition.

## Discussion agenda

The central proposal is a programmable working environment: model participants
and the operator can change how work proceeds while retaining useful context,
access to computational services, and study material. The default coding experience must
remain good enough to use daily. Expressive composition should make unusual work
practical rather than require rebuilding the product for each experiment.

Five questions deserve discussion before specification:

1. **What does the model inhabit?** The operator agreed on an always-programmable
   environment with substantial normal-operation model agency and tunable autodroit
   up to model-governed autoresearch. The first milestone is building Arconaut
   inside Arconaut as soon as possible. Its exact bootstrap scope remains to be
   discussed. Python is excluded from production by the operator (2026-10-01).
   Live compiled Lisp and native refit remain candidate
   mechanisms; the working experience is the primary question.
2. **What stays alive?** The operator settled the service ownership boundary:
   Arconaut consumes kernels/databases/computation; the eventual fabric governs
   them and provides control-plane/scaling. Refit pauses only Arconaut-owned work
   and preserves client references. Shared services continue under their own
   lifetimes. The exact client/reconnect and internal harness-lifetime contracts
   belong in the ground-up specification.
3. **When does a turn change?** The operator chose default activation after current
   turn/affected workflows conclude, with a combined interrupt-and-apply-now action.
   Current work retains its definitions through conclusion. Exact transition,
   overlapping-workflow and checkpoint semantics need specification; the sample
   Control+Enter gesture does not select a UI binding.
4. **What is the audit's observation boundary?** Original mediated model/program
   exchanges, context/definition changes, and experiments are concrete. Queryability,
   protected originals, failure behavior, and unobserved internal effects need
   deliberate meaning rather than an undifferentiated promise to capture everything.
5. **How does the model use the workbench well?** The operator agreed with useful
   discovery/composition/inspection/repair and added universal model rageshake: a
   bead with automatic agent-state capture and a separate complaint-tracking table,
   no immediate fix obligation for the reporter, and timely repair follow-up. Shared
   service operations, retrieval and workflow composition need direct task evidence;
   human/model access should preserve the same underlying lifecycle meanings.

These are agenda items, not requests for five immediate answers or implementation
gates. The [review record](../papers/2026-09-30-brief-review.md) preserves concrete
faults and revisions. The remaining feasibility questions stay discussion/research
input until the specification gives them a precise job.

The [foundation design discussion](../papers/2026-09-30-foundation-design-discussion.md)
explores one coherent candidate and alternatives. It is research/discussion input,
not the next authoritative specification. Runtime/storage choices and the exact
contracts remain open. Reviews found concrete interaction faults worth carrying
into discussion: stale owners dispatching work, decision recovery duplicating effects,
late tool results crossing compaction, and migration side effects that cannot be
undone by keeping the old object.

Discuss the brief's scope, representative work, and difficult choices with the
operator. Then write the specification in pen from the ground up: entities and
identities, lifetimes and ownership, time/order, state/effects and observations,
then the expressive agent/workflow semantics they support. Study feasibility where
it changes those decisions. Review the integrated specification before planning
implementation or choosing adaptations from quarantine. Old phases and file layouts
cannot decide the new architecture.
