# Runtime, refit, and recording ownership: adversarial design review

2026-09-30. Cross-unit review of `docs/BEHAVIOR.md` and `docs/DESIGN.md` under
Blackbird. Reviewed the integrated candidate, not a certification of this
reviewer's workflow study. Read the runtime and audit studies where necessary
to distinguish omitted integration from unresolved feasibility. No code, model,
provider, reference product, or fault experiment was executed. Only this report
was written.

Current artifact: [foundation design discussion](2026-09-30-foundation-design-discussion.md).
The operator clarified the sequence after this review: develop, review, and
discuss the brief, then write the specification in pen from the ground up.
The former `docs/DESIGN.md` is preserved as that discussion paper; no architecture
or exact contract was adopted. The [working brief](../docs/BEHAVIOR.md) likewise
remains for discussion. Original snapshot names and line locations below remain
historical; their navigation links now lead to the preserved discussion sections.

Initial verdict: **revise two lifecycle contracts before treating the design
review as resolved**. The parent subsequently integrated both corrections;
the follow-up inspection below finds them resolved at the design-contract level.
The proposed component boundary remains plausible. Neither finding required
selecting a different language, adding an approval layer, or solving all
acknowledged owner-replacement/platform questions now.

Locations below refer to the reviewed draft: DESIGN's recommendation begins at
line 10, its recording contract at line 301, and refit protocol at lines 343–365.
Later edits may move the lines. The section and quoted operation identify the
affected contract independently of numbering.

## R1 — High: changing the conversation-owner ledger does not fence former-owner actions

**Locations:** [DESIGN.md:96](2026-09-30-foundation-design-discussion.md#core-data-and-time) expected
revisions; [DESIGN.md:303](2026-09-30-foundation-design-discussion.md#recording-and-effects) dispatch;
[DESIGN.md:353](2026-09-30-foundation-design-discussion.md#refit-as-a-quiescent-operation) ownership transfer
to outpost; DESIGN.md:359–365 candidate import and return;
[BEHAVIOR.md:196](../docs/BEHAVIOR.md#5-refit-and-return) one active owner.

The design gives the service an authoritative conversation-owner ledger and
transfers its epoch, but does not require operation dispatch to validate the
current execution owner. Expected revisions prevent two competing ledger writes;
they do not stop the former owner from issuing an otherwise valid operation.
The return path also does not say when the outpost stops initiating work for
the handed-off conversation or how its final context tail is included.

**Concrete failure:** while the candidate starts, the outpost has a chat request
in flight. Step 5 imports an outpost context snapshot; step 6 transfers ownership
to the successor. The old request then returns a tool call. The outpost's normal
controller submits it through the shared operation interface. With no required
owner-epoch check at admission/dispatch, both controllers can act for the same
conversation. Alternatively the successor starts from the earlier snapshot and
misses a message the outpost accepted between import and transfer. The ownership
ledger can look internally consistent throughout.

Provider drivers may be controller modules or separate workers (DESIGN.md:49).
Consequently, checking an epoch once when a request is prepared is not enough:
a queued or previously authorized dispatcher can outlive that check. A supported
dispatch already committed before handoff needs a settled/adopted disposition;
it cannot simply fall out of the ownership model.

**Required correction:** define ownership transfer as an execution boundary,
not just a ledger update. Every conversation-derived admission and dispatch
must carry and validate its execution-owner epoch. Serialize transfer with the
dispatch gate and account for previously authorized but not-yet-dispatched work.
For return, stop the outpost's new conversation effects, settle its locally active
conversation requests/commands or give them an explicit separate disposition,
and identify the final transferred context/message cursor before successor
activation. Independently identified peers may keep working.

Accept late observations under their original operation/epoch and retain their
bytes; do not discard them as stale. What a former owner loses is authority to
start a new continuation/effect for that conversation. Remote outcome uncertainty
remains recorded uncertainty, not a reason to claim the remote provider stopped.
If an old result is to influence the successor, it enters through an explicit
retained observation that the new controller can select.

This is already partly present in the audit study: its refit section explicitly
says the owner rejects commands from an old controller epoch. Carry that rule
and its dispatch/return consequences into the integrated design rather than
leave the studies to supply different strengths of the same invariant.

**Direct oracle:** use controlled provider/program endpoints with independent
effect counters. Delay an old-owner completion and an old queued dispatch until
after ownership transfers in both directions. Confirm old-origin bytes remain
retrievable, stale continuation submissions create no new external effects,
accepted transition-time messages reach the designated new owner once, and
the successor can act. Checking only the ledger epoch or a handoff-success
response cannot expose the failure.

## R2 — Moderate: the closed main admission gate has no complete scope or return transition

**Locations:** [DESIGN.md:44](2026-09-30-foundation-design-discussion.md#responsibilities-and-physical-lifetimes)
shared service ownership; [DESIGN.md:343](2026-09-30-foundation-design-discussion.md#refit-as-a-quiescent-operation)
close main admission; DESIGN.md:347 pause every managed standing host;
DESIGN.md:355–365 outpost build, candidate start/reattachment, return/resume;
[BEHAVIOR.md:183](../docs/BEHAVIOR.md#5-refit-and-return) stop main admission.

The service owns the resident resources, but the protocol does not identify
which resource/operation cohort is being refitted. It closes main admission,
then requires outpost chat/build, candidate startup, reattachment, and resume.
Step 6 never explicitly reopens main admission. The audit study does explicitly
name this reopening; the integrated protocol omits it.

**Concrete failures permitted by this gap:**

- An implementation closes the shared service's effect gate and pauses every
  managed standing host, including a locally hosted outpost. The independent
  process exists but cannot perform its build/chat role through the supported
  interface.
- An implementation exempts the whole service to let the outpost work. A queued
  main command or a resume request then starts during refit, violating the
  operator's no-active-main-work rule.
- A correct implementation keeps main admission closed and reaches step 6.
  The new controller is ready and standing work resumes, but subsequent ordinary
  requests are still rejected. An implicit early reopening creates the opposite
  failure: new work starts before ownership/import/attachments are ready.

“Main” is a meaningful product distinction, but physical service ownership or
host identity alone does not define it: the service remains shared and outposts
can be local. A concrete lifecycle needs a small explicit scope rule.

**Required correction:** identify the main refit cohort and its admission epoch
before quiescence. Close new main workload admission/starts/resumes while keeping
observation, incoming-message retention, and the necessary refit control
operations available. Preserve an explicit disposition for previously admitted
but unstarted main work—held, settled, or drained before refit entry. Outpost
build/chat effects and candidate startup belong to the independent refit scope;
they must not reopen ordinary main work or enter its pause inventory by accident.

Define the return transition explicitly: successor working data and resource
attachments accepted, ownership transferred under R1, then resume/reopen in a
specified order. Record individual outcomes as the draft already requires.
If a subset cannot resume, state which resources remain paused and whether new
main work is admitted; do not silently equate partial recovery with full return.
This is a gate/state contract, not a remote scheduler or new permission system.

**Direct oracle:** arrange a queued main command, a preserved standing host,
an outpost build/chat fixture, and an independently managed daemon. During
refit, inspect actual endpoint activity: main work does not start and the host
remains alive/paused; the outpost and daemon remain useful. Before successor
activation, normal main submissions cannot run; after the declared reopening,
one can run. Exercise candidate-start and reconnect failure. Do not infer
gate correctness merely from its displayed state.

## Examined concerns that are not findings

- **Local cancellation versus remote uncertainty:** DESIGN.md:83–87 and
  343–346 already distinguish a locally closed stream from an unknown remote
  outcome and reject cancellation-requested as settlement. Refit need not
  prove that an unobservable remote provider stopped. The concrete local
  settlement/dispatch boundary needs R1/R2; no all-world cancellation claim
  should be added.
- **Preservation and pause:** the design requires actual paused/live instances,
  rejects recreation as preservation, calls out arbitrary descendant limits,
  and keeps platform pause mechanisms as explicit feasibility work. Demanding
  completed cgroup/macOS implementation in this candidate review would be
  misplaced. The direct oracle should use real process incarnation/state and
  endpoints, as the runtime study proposes, rather than a pause flag alone.
- **Owner failure and replacement:** DESIGN.md:331–335 and 374–378 correctly
  separate an owner crash/upgrade from ordinary controller refit. They do not
  promise a dead owner can preserve descriptors or pause children. An explicit
  initial limitation is offered; this is not a hidden solved-architecture claim.
- **Recording feasibility:** record-before-dispatch does not claim atomicity
  with an external effect. Attempt-open crash uncertainty and the read-to-commit
  tail are stated. Record-before-publication governs ordinary observations;
  unaudited emergency control is explicitly degraded. SQLite/VFS durability and
  bounded-load behavior are named feasibility obligations, not demonstrated
  results. These should remain part of the eventual operational experiment.
- **Capture scope:** final driver request/response bytes, program streams, and
  mediated core actions are named; hidden provider state and uninstrumented
  subprocess syscalls are excluded. This is an honest observable boundary,
  not a claim to capture all external activity. Full captured originals remain
  independent of projections and compaction.
- **Oracles and Blackbird:** BEHAVIOR's emitted-byte, actual-state, and actual
  subsequent-action observations attack real faults directly. No extra receipt
  or test-certification layer is needed. The handoff/gate interleavings above
  extend the relevant direct scenarios. No mutants ran; any mutation work
  remains fleet-only. The candidate is explicitly neither executed feasibility
  evidence nor an implementation plan.

## Follow-up inspection of the integrated corrections

Re-read the revised DESIGN on 2026-09-30 after the parent reported integration.
The original findings above are retained so the correction remains understandable.

**R1 resolved at contract level.** Revised DESIGN.md:109–115 requires every
supported conversation action/continuation to carry its epoch and rejects retired
epochs at admission and actual dispatch. Transfer closes the outgoing owner's
admission and settles admitted dispatches/requests; it explicitly says a ledger
change cannot revoke an already authorized external action. Late observations
remain attributed without restoring continuation authority. Revised refit step 6
(lines 395–404) closes outpost conversation admission, settles its local work,
reconciles pending messages/final additions, and transfers through that gate.
Lines 406–410 bound the first refit to a local independent outpost sharing the
surviving local owner; no unsupported remote fencing claim is implied.

**R2 resolved at contract level.** Revised refit step 1 (lines 370–379) identifies
the main cohort independently of physical resource-service ownership, closes its
effect gate including queued starts/resumes, and preserves ingress, observation,
refit/emergency control, outpost build/chat, and candidate start/attachment.
Step 2 pauses only that cohort's standing hosts. Step 6 now explicitly reopens
main admission after successor ownership, working state, and attachments are
accepted, then resumes standing resources with individual outcome reporting.
This supplies the previously missing scope and return transition.

No residual material hole was found in these corrections. The proposed direct
interleaving oracles remain necessary feasibility work; no execution has verified
the gate or handoff. This conclusion does not certify workflow crash recovery,
other design units, or unimplemented runtime/platform mechanisms.

## Status and navigation check after the discussion move

On 2026-09-30, checked the moved discussion paper's owner-epoch paragraph and
refit protocol only. Both reviewed corrections survive: epoch enforcement at
admission/dispatch with outgoing-work settlement and late-observation attribution;
and the explicit refit cohort, closed-gate exceptions, outpost return settlement,
successor acceptance, reopening, and individual resume outcomes. This was a
bounded preservation/navigation check, not a new architecture review or adoption
of the candidate. Updated links while retaining the original reviewed names and
locations. No further research, experiment, or implementation was performed.
