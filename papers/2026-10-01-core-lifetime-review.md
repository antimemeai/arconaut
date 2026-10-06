# First-core review: code and resource custody

2026-10-01. Independent adversarial review of `docs/CORE_DESIGN.md`, before
implementation planning. Scope is one invariant: existing work retains the code,
resources and observation duties it needs through definition changes, interruption,
native retirement and exclusive outpost refit.

Read workspace/project instructions and Blackbird, FOUNDATION, BEHAVIOR,
STARTING_DESIGN, and the acquired-source native-reload, Lua-embedding and
refit-custody grounding reports. No acquired program ran. This review does not
require choosing runtime versions or specifying every binding before planning.

## Judgment

The proposed ownership split is plausible and avoids the major false claims in
the references: retaining a library does not preserve destroyed objects; Lua
continuations are not saved native stacks; a descriptor is not child parentage;
transport closure is not remote cancellation. Five behavior gaps below should be
resolved before this specification guides a plan. They require short contract
clarifications, not another architecture or a broad research pass.

Line numbers refer to the reviewed draft.

## F1 — Pending activation can deadlock the work it is waiting for

**High.** Lines 190–195 stop newly starting affected work once a change is pending,
without distinguishing new top-level turns/workflows from child operations of a
currently running affected workflow. A running coding turn receives a provider
response after activation becomes pending; its next step needs to launch a tool
and then another provider request. If those are newly starting affected work, the
gate blocks them while activation waits for that same turn to conclude.

Define the gate's unit. Normal pending activation should close admission of new
affected root work while existing roots continue to admit descendants under their
retained generation until conclusion. Their descendants must inherit that custody
and not independently select the candidate. Interrupt-and-apply-now is different:
stop ordinary descendant effects as well while allowing settlement/control duties.
Do not permit a new root to masquerade as a descendant to defeat the boundary.

Direct oracle: stage a differing tool definition while an old turn awaits a
provider. Its subsequent tool and request complete with old bindings; another
affected root waits; publication occurs after the original root concludes.

## F2 — The refit stream rule excludes preserved standing workers

**High.** Lines 243–255 say a still-active local stream obligation blocks refit,
but eligible workers must remain alive, retain their state and resume later.
A correctly stopped standing child normally keeps its stdout/stderr open. The
custodian still has a stream observation obligation, and buffered bytes can remain
even though the worker is no longer progressing. Requiring stream termination
here makes preservation impossible or invites closing pipes to pass the check.

Distinguish ordinary active-main settlement from paused-standing custody. Ordinary
commands/provider clients must settle their local obligations. A preserved worker
may retain open streams and queued passive observations in surviving custody;
those do not count as active main execution. Its pause contract must separately
exclude progressing owned callbacks/requests/children, retain required native/Lua
state, and define observation of already buffered bytes. Unknown remote effects
stay unknown and do not become cancelable by pausing a client.

Direct oracle: refit a worker with nonempty buffered output and open pipes; observe
its stopped progress, exact surviving bytes, live original heap and successful
resume without manufactured EOF or replacement worker identity.

## F3 — Exclusive return lacks the outbound quiescence condition

**High.** Lines 257–269 require main settlement before handing authority to the
outpost, but return only validates a successor, transfers data and advances its
epoch. The outpost may still have an active provider call, queued tool dispatch,
or unsaved context edit. Fencing future requests does not stop an already admitted
external action, and transferring a context snapshot does not transfer its pending
writer or continuations. The successor could start the same conversation while
the old outpost continues influencing it.

Apply the same exclusive-handoff prerequisite in both directions: close new root
admission on the departing owner; settle its ordinary conversation work or keep
return pending; reconcile/publish its context and queued input; fence queued stale
dispatch at the actual supported effect boundary; then publish new authority.
Separately identified peer work may continue, but must not silently inherit the
exclusive conversation. Unknown external outcomes remain retained obligations.

Direct oracle: request return during an outpost provider request and during a
queued tool dispatch. No successor action starts before supported local settlement;
the old epoch cannot dispatch after handoff; accumulated context/input appears once.

## F4 — A failed native migration has no disposition for the old state

**Medium, material.** Lines 207–214 forbid publishing a failed migration and reject
general native rollback, but do not say whether migration may mutate old state,
move owned handles or execute effects before publication. A checked failure after
such a move can leave the still-effective generation broken. An isolated checking
worker does not cure a later actual migration that mutates the live owner.

Choose a narrow default: preparation/checking constructs independent candidate
state, does not consume old custody and does not dispatch migration effects; the
defined activation transition transfers ownership only once the candidate is ready.
For an intentionally effectful or destructive migration, record the actual effects
and failure and stop affected admission when old state cannot be shown usable.
Do not silently resume the old generation or relabel the failure as rollback.
This needs an outcome contract, not an implementation of arbitrary transactions.

Direct oracle: reject migration after constructing candidate state and verify the
old counter/resource owner remains intact. Separately inject failure after an
explicit destructive step and verify blocked/unknown disposition rather than
ordinary execution with invalid custody or duplicate resource ownership.

## F5 — Refit/control calls can wait on their own completion

**Medium.** The model/operator share operations, but lines 193–195 and 243–248 do
not separate an initiating control operation from the work it asks to settle.
If `arcorefit` is awaited as an ordinary tool in a live turn, refit waits for that
turn, and the turn waits for refit. Interrupt-and-apply-now has the same issue when
invoked by the affected Lua program. Closing main admission can also suppress the
completion/status or control duties required to finish the transition.

Make transition requests custodian-owned control activity: report pending/accepted
identity without requiring the calling continuation to remain alive until return,
or explicitly transfer/end that continuation before the transition. Exempt the
minimum settlement, observation, complaint/control and transition duties from
ordinary-work admission closure. This is a scheduling distinction and must not
allow arbitrary new ordinary effects during quiescence.

Direct oracle: request refit from the model's own tool call and interruption from
the affected program; both remain inspectable and reach a real boundary without
the initiating continuation deadlocking their prerequisite.

## Native mechanism selection and nonfindings

The owned generation-loader proposal is compatible with the operator's C++ plus
RCC++/Godot/**similar** direction: it implements similar live compilation/loading
while retaining the qualification/adoption decision. It is not permission to skip
the direct RCC++ comparison. The grounding gives a concrete reason for proposing
owned machinery: RCC++ replaces/destroys objects without an affected-work census,
retains modules without bounded unload, and its signal recovery does not supply
safe C++/Lua rollback. Godot editor reload and JENOVA's documented platform/state
limits are correctly not presented as a qualified Mac runtime answer.

The Lua owner-thread rule, protected allocating preparation, limited business-call
exception conversion, rooted coroutine/owned continuation data, finalizer custody
and unload-after-return requirements are appropriately stated. Actual runtime
unwind, low-memory and retirement tests belong in qualification, not additional
specification ceremony. No defect was found in the explicit refusal to pause
consumed services, claim arbitrary-tree suspension, transfer wait rights through
descriptor passing, or recreate standing heaps under remembered names.

The independently retained outpost executable/provider access meets the failed
candidate availability requirement. Dependence on the surviving custodian is
explicit; this does not claim survival of whole-host or custodian failure. Complaint
sink uncertainty and TUI disconnect behavior are sound, subject to the control
admission clarification in F5. Explicit inability to settle is an acceptable visible
pending outcome; the design need not promise forcibly recovering arbitrary native
code or transplanting live Lua stacks.

## Rereview

2026-10-01. Reread the integrated specification, including operation decisions,
later effect fencing, context incorporation, generation activation and both
handoff directions together. All five findings are resolved at design scope:

| Finding | Integrated contract | Disposition |
| --- | --- | --- |
| F1 | Pending changes gate new affected roots; existing roots admit descendants under retained definitions; the custodian checks ancestry. Interrupt closes ordinary descendant effects separately. | Resolved, CORE_DESIGN lines 228–235. |
| F2 | Ordinary active obligations block refit; paused-standing pipes and passive buffered observations remain in surviving custody without manufactured EOF. Pause excludes progressing owned work. | Resolved, lines 290–299. |
| F3 | Return closes outpost root/descendant admission, settles local work, reconciles context/input and fences queued stale effects before publishing successor authority. Failed settlement keeps return pending. | Resolved, lines 323–331, with effect-boundary fencing at 92–95. |
| F4 | Default preparation preserves old custody and avoids effects; destructive migration failure blocks affected admission when old state is not known usable. | Resolved, lines 256–261. |
| F5 | Custodian-owned control requests return accepted/pending identity; their initiator must conclude/transfer/end its continuation. Settlement, observation, complaint and minimal controls survive admission closure. | Resolved, lines 301–306. |

No remaining defect was found in this invariant. The added supported-step
checkpoint/invocation identities also prevent recovery from treating a lost Lua
frame as permission to invent replacement action identities. Unknown remote
outcomes remain separate from settled local custody throughout return and refit.

The concrete activation ownership transition, real stopped-worker membership,
dispatch fencing and Lua unwind/retirement behavior still require direct
qualification in the reviewed implementation/test plan. Those are implementation
evidence obligations already stated by the specification, not unresolved design
findings or claims of an implemented runtime.
