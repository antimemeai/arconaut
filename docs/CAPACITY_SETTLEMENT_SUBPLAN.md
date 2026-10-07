# .14.3 next unit: protected settlement budget

10a8de4 provides a declared successor destination, not exhaustion recovery. Actual
quiet replacement continued: running native PID47430 started17:15:26, release/arco
built17:08:12. Ordinary resumed request worked. No old namespace activation/settlement
is thereby established. A large working-context proposal now returned staged under
pcall; finish this ordinary workflow to apply it before implementation, without a
ceremonial native restart.

## Concrete native consequences

* FramedJournal append computes exact transaction bytes:56 commit plus each frame
  header/payload; byte/record cap rejection is prewrite. This is where normal writes
  must leave protected room, rather than an estimated request-only warning.
* CodingEngine operation currently submits Decision/Invocation/Admission separately;
  all three contain the input. Dispatch writes AttemptOpen before effect and receipt
  after callback, even when callback errors. Allowing new admission until the final
  byte cannot retain that receipt or the terminal observation reliably.
* AuditLog originals are source+metadata transactions; provider capture and process
  output observers append while effects are live. A source-budget refusal must stop
  capture/provider or child, retain earlier partials, classify dispatched provider/
  process as unknown and never return a complete response or retry capacity failure.
* LocalTools child.collect bounds accumulated output16MiB but streams originals
  first. Observer throw unwinds child/retains its partial; callback failure can occur
  after process effects. File.proposed capture precedes write; do not generalize that
  all capacity errors establish nonexecution.
* Context append serializes a full next view and fresh originals; managed cancellation
  records the pending proposal. Interrupt linkage appends a full view plus fixed
  error outputs. A small constant terminal margin does not cover these. Reserve must
  account for current/proposed context and pending management BEFORE publishing each
  normal mutation. Stop rather than promising settlement for unlimited Lua outputs.
* Lua can nest operations inside a live outer Lua/tool attempt. Reserve terminal room
  for every outstanding admitted depth, not just the latest provider. Budget state
  must be exception-safe; no Lua/runtime reentry can silently reset protected room.

## Implementation direction and direct fault classes

Own a workflow-scoped budget at the retained/native write seam. Normal append must
fit its exact physical batch AND protected byte/record floor; narrowly scoped
settlement writes may consume that floor, never admit/dispatch fresh effects. Compute
context cancellation/linkage requirement from actual serialized prospective state;
include bounded active-attempt receipts/error observations and pending proposal.
Reject insufficient budget before admission, preferably atomic decision/invocation/
admission publication rather than leaving partial triples. Fixed terminal/error
records, bounded admitted nesting and explicit rejection of oversized maintenance
are necessary before promising a finite allowance. Do not choose a blind larger cap.

On normal exhaustion: stop new work, cancel live capture/child promptly, preserve
previously retained originals, expose any received-but-unretained fragment explicitly
rather than saying all originals survived, settle uncertain attempts conservatively,
cancel staged context, preserve call/output protocol linkage and permit explicit
successor action. Read errors/nonlive writer remain audit failures, not retryable
capacity. Settlement writes must still obey physical limits and publication rules.
The successor seed is declared/selected context, NOT imported admissions or proof
that the source completed. Live handoff and bounded old-original resolver are later
seams; do not conflate them with a write-floor primitive.

Tests should force byte and record exhaustion independently: pre-admission zero
adapter calls; streaming partial abort with no tool dispatch or capacity retry;
process effects/partial retained/unknown and responsive cleanup; nested outstanding
attempts; full-context/pending-management cancellation; exact last permitted normal
batch; insufficient reserve/refused oversized mutation; native write/sync failure
remaining unknown. Independent review after one coherent implementation, then only
affected Mac/Linux checks and native replacement. .14.3 remains active meanwhile.

### Implementation slice chosen (before editing)

Implement the native retained-owner budget primitive first, NOT enable an guessed
workflow allowance. A nonnestable RAII scope holds byte/physical-record credits.
Every append path (including issuer and rejected-proposal diagnostics) leaves these
credits untouched. Only validated adapter receipts and terminal observations can
spend them, within their exact physical cost; dispatch automatically routes receipts
through this seam. Successful publication debits credits; uncertain write/sync
failures retain existing poisoned/pending semantics. Scope release only removes a
RAM constraint, never edits originals or grants recovered effects permission.
Caller must size/replenish obligations before admissions; this primitive alone does
not guarantee context cancellation or all admitted attempts fit. Direct simulation
will exhaust normal source writes inside an actual dispatch and settle its unknown
receipt/terminal within held room. Byte/record floor, nesting, oversize, duplicate,
rejection diagnostic and sync-unknown cases are the bounded claims of this slice.
Workflow policy, atomic triple admission and context/capture integration follow.

## Next dependency: atomic operation admission (2026-10-06)

Protected-credit native activation actually observed: PID55593 start17:59:52 after
release/arco17:52:09, ordinary resumed provider request and local tool successful.
Managed revision9c814082f6003dff7d59000000000000 replaced the selected109 presentations;
current56 entries vs prior115, input177839 vs346305 bytes (different boundaries, not
performance/policy efficacy). Immutable originals remain, physical audit449447243
bytes87423669 remaining. Old536870704 audit unchanged; settlement still unestablished.

Choose atomic operation admission before broad protected-maintenance policy because
operation currently publishes Decision, Invocation and Admission in three independent
transactions. Prewrite refusal can leave a committed decision/invocation but no attempt.
This violates the intended new-admission unit even with the native normal-write floor.
Existing FramedJournal multi-draft transactions and RetainedState append already apply
ordered dependencies to one candidate and publish only acknowledged complete batches;
use that owned seam, no new library or journal format. Acquired Codex logical fork vs
physical base and measured growth/replay findings remain in HISTORY_CAPACITY_SUBPLAN;
this slice addresses admission, not segmentation or speculative reserve configuration.

Change only CodingEngine::operation to one ordered three-event append with fresh
issuer identities, same continuation/input bytes and relationships. Issuer reservations
remain individually durable (unused identity is allowed), not rolled back or replayed.
Batch refusal happens before dispatch; physical failure preserves uncertain originals
and prevents dispatch via existing append failure propagation. Per-journal batch limit
now applies to their sum; smaller custom max_batch settings may reject a formerly
individually fitting operation. Do not raise limits or dispatch partially. Bootstrap
32MiB payload/96MiB batch accommodates these three payloads under existing limits.

Direct native engine faults: hold byte room for issuers plus two old event transactions,
independently hold record room for issuers plus two facts, and impose a small batch
limit where each input event fits but their sum does not. Require zero new committed
Decision/Invocation/Admission and no file effect/provider call; budget unchanged and
writer live on prewrite refusal. Release hold/use smaller input: one real file write,
one related triple in the same physical batch. Establish failing old-code oracle
before edit. Existing journal partialwrite/sync tests stay settled: batch machinery
itself is unchanged. Consequential independent review of actual delta and affected
coding/native retry consumer checks, release build/actual replacement follow.

Not completion of .14.3: workflow obligations, protected context maintenance, bounded
output/cancellation loss accounting, live handoff and old-original resolver remain.

## Integrated workflow refinement (fresh root-authored working context)

Source reread: `turn` only links calls on interrupt, Runtime::eval captures arbitrary
Lua error before propagating, context append/edit and staging publish without a
prospective floor, and receipt protection alone does not protect operation.result.
Implement one turn-owned credit scope, refreshed before prospective context/pending
publication and admission. Bound admitted nesting at 16; each active slot covers
receipt, bounded error source+issuer+terminal, plus bounded Lua diagnostics. Context
obligation is derived from serialized prospective entries with actual open-call IDs
and pending proposal, including fresh output originals and cancellation packet.
Conservative framing allowances are explicit, not physical cap increases.

Private native maintenance capability covers cancellation/linkage and bounded error
sources/issuer writes, never fresh admission or adapter dispatch. Successful normal
results remain normal writes; refusal after dispatch settles a bounded unknown
terminal rather than losing the attempt. Capacity is sticky for the turn even if
Lua catches it: no new effects; cancellation closes protocol groups. Streaming
observer refusal aborts existing provider/child behavior, retains previous fragments
and records received-but-unretained byte count (not content/full retention). Oversized
Lua diagnostics are truncated with explicit lost-byte metadata. Physical write/sync
failures remain unavailable/unknown, not capacity recovery. Reserve establishment
failure before workflow mutations is a pre-execution stop. Successor/old resolver
are separate remaining work, not implied by this integration.

### Bounded layer1 repairs, 2026-10-07

Operator's BOUNDED_HARDENING governs this envelope: layer1 remediation then ONE
layer2 scoped recheck/fix, never a third reviewer or unchanged-host recertification.
Started03:28:55Z; layer1 bound03:58:55Z, layer2 bound04:13:55Z including waits.
Kimi transport failed before a review; no replacement consensus sought. Independent
ChatGPT concrete findings: physical packet limits and longer interruption modeling.
Now preflight encoded cancellation/linkage packets against unchanged payload/batch
limits using actual next entries, duplicate originals, proposal, framing and issuer.
Shared stop_outputs produces both actual linkage and the longer reserve representation.
Single-context-event batch fits follow journal's valid_limits(payload <= batch-88),
but cost is checked explicitly; tight valid profiles exercise both constraints.
2000-call actual interrupted turn consumes ordinary room down to protected floor and
closes every call with turn_interrupted. Payload boundary oracle first RED: published
call and file effect; corrected tight fixtures now GREEN reject publication/effect.
The first batch fixture was invalid (payload > batch-88), not a behavioral red; fixed.
Earlier interrupted text mismatch was conservatively padded by longest dummy IDs,
so no demonstrated old under-reservation trace is claimed for finding2; replaced by
exact larger serialization regardless. Pending proposal survives failed success
publication until cancellation, and floor covers both old and prospective state.
Existing Linux3profiles coding/context/retained_state pass completed before new fixes;
only changed behavior will be checked again. Layer2 is now scoped to these fixes and
integrated stop claim. Successor/old resolver are separate per explicit operator steer.

Layer2 returned one propagation finding: cancellation_budget could throw before
protect_workflow's refresh-only sticky catch. Actual Lua bridge Runtime::failed
already sets capacity_stopped, so strengthened pcall -> write fixture passed BEFORE
this repair; claimed Lua escape was not reproduced. Still centralize the sticky
catch over the entire native calculation so the owner does not rely on bridge
propagation. Same oracle rerun, no layer3 review. Layer2 done subject to final affected
coding checks and activation. No consensus/review of recheck sought.
Final affected coding all3 Mac and Linux(run-6vzxt08r) passed; existing other affected
passes stay settled. Source19a067e pushed. One native activation pending observation.

Scoped closure: one native activation observed main PID14481 started03:43:10Z
(binary03:37:51Z), ordinary resumed provider/tool response and live managed-summary
publication observed. No predecessor settlement inferred. .14.3 closes under ROOT's
bounded envelope; unattended rollover/backstop and targeted old-original resolution
are separately queued, not delivery claims of this unit. Physical caps unchanged.
