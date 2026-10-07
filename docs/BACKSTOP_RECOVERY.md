# Backstop recovery: step back, assess, pivot, continue

Status: operating design with a first cooperative native implementation (G1).
`--backstop MISSION_JSON_FILE --once PROMPT` opts into an independent model
assessment after a failed workflow under the same native lifetime and session
custody. See [BACKSTOP_OPERATING](BACKSTOP_OPERATING.md) for the delivered interface.
This is NOT unattended crash recovery: a reopened engine has no backstop ticket,
and arbitrary exec makes containment unavailable for that native lifetime. Recovery
refuses exec/restart and provider requests during the selected action. At most two
pivots and a two-minute native deadline produce either a changed artifact observation
or a retained blocked/paused outcome. Exact-byte artifact observation is not a
semantic correctness judgment. Broader supervisor containment below remains design,
not delivered behavior. Operator2026-10-06/07 requests a mode for “got stuck,
nothing in flight, back up, assess, pivot, and drive on.”
The active capacity workflow remains its own coherent implementation unit; this
backstop consumes its interfaces and extends recovery beyond a network retry.

## Purpose and placement

An independent supervisor should make a failed campaign recoverable without a
Codex colleague manually writing another handoff. The model owns assessment and
choice of approach. Native machinery owns process lifetime, session exclusion,
recorded dispositions and the authority to resume execution. No ordinary command
approval or global correctness judge is introduced.

Put lifecycle recovery outside the failed turn and its exhausted journal. A fresh,
small recovery session is an early use of the outpost idea; it can initially be
another invocation of the existing native Arco executable. No new model provider,
plugin framework or runtime is needed. External construction tooling can supply
the first supervisor; the eventual product must not require evotools or Codex
acting as an attentive operator beyond today's explicit auth scaffolding.

Do not make the process monitor an LLM. Do not ask the failing main workflow to
allocate its own rescue after it can no longer write. Keep the recovery contract
small enough that the monitor can survive the failure it is meant to handle.

## Two different questions

1. Is this harness locally quiescent: no active provider transport, local tool
   children or outstanding callbacks, with exclusive handoff/session authority?
2. Which past effects have known outcomes, and which remain uncertain?

A terminated provider request can still have unknown billing or remote completion.
A reaped command may already have modified a file or submitted remote work.
Quiescence allows recovery; it does not convert either into success or nonexecution.
An intentionally detached/shared OS service is outside harness lifetime. An escaped
or untracked child is not evidence of quiescence. Expose unavailable observations.

The successor must not inherit permission to redispatch predecessor attempts.
Unknown effects remain attached to their exact attempt and affected resource. The
model can inspect actual current artifacts and reconcile them, choose a genuinely
new operation, or leave that resource untouched and do independent useful work.
Native rules enforce no automatic replay; a blanket halt on every unknown effect
would defeat the purpose of the backstop.

## Recovery cycle

**Detect → quiesce → preserve → assess → pivot → act → observe.**

Detect structured workflow failures (capacity, nontransient provider failure,
workflow exception, damaged working presentation), explicit model/operator “stuck”
requests, or an abandoned campaign. Silence alone is not failure: a long compiler
or provider request may be healthy. A watchdog needs operation kind, deadline,
heartbeat semantics and configured intervention policy before cancelling work.
Explicit operator pause/cancellation stays paused; the backstop must not undo it.
Normal successful workflow exit is not an automatic recovery trigger.

Quiesce by stopping admissions and requesting cancellation through existing native
paths. Wait for active callbacks and tracked child groups to finish/reap. Record
unknowns as unknowns. If native cooperation fails, a supervisor must have tracked
process containment to terminate and verify local quiescence. Parent PID exit,
expired heartbeat, a freed journal lock, or exit0 alone is insufficient. The
current Child implementation reaps its leader but does not expose a complete
persistent descendant registry; crash/escaped-child recovery is therefore a real
implementation prerequisite, not a promise satisfied by parsing ps output.

Preserve the original session and the last readable prefix. Never truncate a full
audit, steal a live lease, discard dirty source, increase capacity blindly, or
replay an uncertain effect. “Back up” means preserve the evidence and step back in
working context; it does not automatically copy a 512MiB archive or reset Git.

Start a fresh recovery audit and present a concise packet:

- operator mission and current steer, with authority/provenance;
- cause, native error, failed attempt and exact original/session locators;
- confirmed local quiescence versus unavailable observations;
- committed work, dirty paths and active candidate/checkout users;
- known/unknown effects and affected resources;
- latest working map, retained next step and relevant evidence locations;
- previous recovery/pivot outcomes and available resource policy.

Maintain the working map and locators while the campaign is healthy, at useful
boundaries. A periodically replaced packet is a derived convenience, not the
original audit or authority on success. Snapshot failure is visible; stale fields
carry timestamps/identity. Preserve minimal lifecycle failure facts in the
supervisor's separate store if the old audit cannot accept them. If that store or
free disk is also unavailable, do not launch work pretending it can be audited.
A fresh journal fixes a per-session limit; it does not fix exhausted host storage.

Assessment is a model turn with a recovery-specific Lua program. Begin with bounded
inspection of relevant source, current artifacts and failure evidence. The model
chooses among repair-and-resume, selected-context successor, a different workflow
or model within installed capabilities, a distinct experiment, and deferring one
blocked resource while advancing another. No general confidence score qualifies
these choices. Specific effects require specific reconciliation.

The recovery program produces a small written pivot: what is retained, what is
uncertain, what changes, the next useful action and its observable outcome. The
native handoff records selected context/new identities and declared lineage using
the successor seam. Verified facts and author declarations stay separate. Use the
same operational session when recovery writes fit and its ordinary native checks
permit it; use a successor when the audit or presentation needs a fresh start.

Then perform useful work. A successful recovery requires the selected action to
run and produce its predeclared observation; starting another process, receiving
an assessment paragraph or posting a “recovered” status is not enough. The board
shows assessment, pivot, resumed work, blocked resource or paused state, alongside
actual observations and locators.

## Avoid a recovery doom loop

Use a configurable finite no-progress recovery budget; initial proposal is two
pivots for the same unresolved incident. The count resets only on a meaningful
outcome named by that pivot (for example, the failing build now completes or a
successor completes its first useful source/check task), not on token output or
process creation. A later recurrence remains linked to previous incidents.

Do not launch the same deterministic failure indefinitely under new session IDs.
At the limit, retain the complaint and assessment, choose independent useful work
if available, or become visibly blocked on the specific external condition.
Transient provider outages already have backoff/retry policies; an assessment need
not spend another model call every time an unavailable provider remains unavailable.
Recovery consumes configurable call/time/storage budgets and retains costs/failures.
These are operating controls, not a second certification campaign.

## First whole implementation unit

Build the quiescent-stop → recovery-session → selected pivot → useful successor
path for actual capacity/workflow errors, reusing protected workflow settlement and
seed/lineage work. Add a bounded typed failure/handoff record and process ownership
observations at the native/supervisor seam. Retain an explicit failure reason when
quiescence cannot be established. Initially trigger on observed failure or explicit
stuck request; autonomous silence intervention waits for real operation tracking.

Use direct fault oracles: full old audit unchanged plus fresh auditable useful work;
child still alive blocks operational handoff; expired heartbeat does not kill a
healthy command; unknown file effect is inspected rather than replayed; provider
partials never execute; dirty source survives; duplicate monitors cannot both
activate; crash during handoff leaves one recoverable intent; explicit pause stays
paused; repeated same failure reaches its configured no-progress bound; missing
packet/space reports unavailable rather than invented continuity. Complete these
as one integrated behavior, with consequential review and affected checks.

No additional promotion gate is created here. Existing arconaut-5x7 waits for the
current .14.3 capacity unit. Any deferred backstop scope must be plainly recorded
when that unit is accepted, rather than silently expanding it into an endless epic.

Source consequences and primary literature are in
[the backstop study](../papers/2026-10-07-backstop-recovery-study.md).
