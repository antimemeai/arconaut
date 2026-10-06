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
