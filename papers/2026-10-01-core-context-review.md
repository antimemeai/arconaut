# First-core review: admission, originals and live context

2026-10-01. Reviewed `docs/CORE_DESIGN.md` in its initial 315-line form against
workspace/project AGENTS, BLACKBIRD, FOUNDATION and BEHAVIOR. Read the CLM study,
audit-state study, refit-custody grounding and readiness assessment/review. No
acquired code/test, installer, credential or provider call was executed. Scope is
the operation/original/context/request invariant, not a runtime qualification.

**Judgment: the design is coherent, but three connected semantics need resolution
before deriving implementation units and red oracles.** The findings below require
small behavioral decisions, not another architecture survey or a second audit.

## 1. Define the recoverable publication boundary and its ledger coupling

`CORE_DESIGN` lines 108–123 distinguish appended, committed and durably acknowledged
material without saying which event makes a record committed. Context publication
requires committed originals; dispatch requires durable acknowledgement. The design
does not explicitly bind authoritative context-head/operation/owner transitions to
the same recoverable journal unit that explains them.

**Counterexample.** An observation is fully written and called committed, then used
to publish context; its frame or publication transition is not synchronized. A new
request durably records its own input and dispatches. After a crash, request bytes
survive but the original observation or context lineage does not. Alternatively a
handed-off owner acts before its owner transition has a recoverable disposition.
Both can meet a permissive reading of the current wording.

**Required decision.** Give appended/committed/acknowledged explicit meanings under
the selected fault profile. Define which semantic unit changes authoritative state,
when that change becomes usable, and its ordering with source records and returned
acknowledgements. Ordinary model/context consumers must use the retained publication
boundary, not provisional display observations. State what recovery can establish
about complete but unacknowledged frames, and treat open dispatch-capable attempts
conservatively. Do not infer precrash client acknowledgement from frame validity.
The format may be designed during the planned storage unit; these are its behavioral
requirements, not a demand for a separate receipt system.

Also make the outgoing-effect boundary encompass later stdin writes, signals and
actual additional provider requests, not only initial launch/request preparation.
Retained intended bytes and the observed partial-write/result disposition are
different facts. An admitted interactive process must not make later input unaudited.

**Direct red oracle.** Independently known observation bytes and ledger transitions;
inject failed sync/death between record, state publication, acknowledgement and
dispatch. Compare recovered originals, head/epoch/attempt state and actual external
dispatches. Unacknowledged bytes may survive; acknowledged source dependencies must
not disappear under the declared fault model. Show that provisional display bytes
cannot silently become ordinary context.

Grounding: audit study §§2–4 and 8; custody study's LevelDB write/sync/recovery
distinctions. Their SQLite recommendations are not dependency selections.

## 2. Recovery must reuse a recorded decision's action identities

`CORE_DESIGN` lines 66–81 gives each dispatch a distinct attempt and says a crash
does not automatically repeat effects. It does not specify recoverable workflow
decisions, continuation state, logical invocation identity, or deduplication of
submissions to the custodian. BEHAVIOR §3 expressly requires those semantics.

**Counterexample.** Lua issues a shell action, the custodian admits/dispatches it,
and the participant dies before learning its handle. Restarting the Lua step
creates a new invocation/attempt. Both attempts were dispatched only once, but the
same intended action was duplicated. Keeping old attempts unknown is insufficient
unless the resumed decision is connected to those attempts.

**Required decision.** Supported effectful workflow steps retain their decision,
continuation disposition and planned action identities before dispatch. A repeat
submission of the same invocation and input returns the existing state; different
input under that identity is a visible conflict. Explicit retry is a new attempt
under the logical invocation and preserves earlier uncertainty. Define the supported
restart/checkpoint boundary; an arbitrary lost Lua frame needs reconciliation or
explicit restart, not invented continuation. This does not require serializing
arbitrary VM stacks or an automatic durable-workflow language.

**Direct red oracle.** Kill the participant after dispatch but before delivery of
its handle/result; recover the supported step and inspect actual external dispatch
count and identities. Also test a repeated submission with changed bytes and a
deliberate retry of an unknown attempt. A coherent log with two differently named
commands is not the oracle.

Grounding: BEHAVIOR §3; audit study §3's logical operation versus dispatch attempt;
current generation rules already require explicit suspended-work disposition.

## 3. Late completions need a continuation branch, not blind append to the head

`CORE_DESIGN` lines 135–172 correctly gives edits base revisions/CAS, keeps immutable
requests and prevents edits erasing ledger obligations. It does not define where a
response and its tool-call/result obligations live when the request's context head
has been edited. BEHAVIOR §4 explicitly says late results retain their originating
continuation branch and are incorporated deliberately.

**Counterexample.** A request assembled from R0 remains active while R1 removes or
reorders old material. Its response issues a tool call; the result arrives while
another edit publishes R2. Automatically appending either completion to the latest
head can resurrect removed material, lose concurrent edits or orphan its call/result
structure. Provider validation may catch an orphan, but does not decide which
valid continuation/context the model is actually inhabiting.

**Required decision.** Attribute each completion to its originating request,
revision and generation. Retain the continuation's complete structural obligations
independently of editable presentation. Incorporating its observations into the
working head is an explicit context operation with base/CAS and retained outcome;
on conflict preserve the branch/candidate instead of dropping the result or silently
rebasing it. The workflow may select/fork/incorporate a branch and present a supported
provider representation; editing context cannot settle a real outstanding effect.
Messages arriving concurrently participate through the same explicit selection.

For ordinary editor/file reconciliation, the candidate's base must be the revision
the editing operation actually started from, not whichever head is current when
the file is read back. The existing CAS rule is good; clarify that its base is not
inferred from a refreshed workspace view or a watcher event.

**Direct red oracle.** Hold a response/result, publish two competing context edits,
release the completion and inspect branch identity, conflict outcome, actual next
request structure and exact bytes. No lost concurrent edit, no silent resurrection,
and no invented result. Separately edit an exported R0 file after R1 has published
and verify a visible conflict instead of attributing the old edit to R1.

Grounding: BEHAVIOR §4; CLM study's command-boundary mirror, role/tool losses and
snapshot-before-provider-rewrite gap.

## Strengths and bounds

The design already makes originals/context/final requests distinct, permits repair
that grows context, keeps current governing generations across ordinary data edits,
rejects silent invalid edits/pruning, and treats remote outcome as separate from
local settlement. No finding asks for approvals, forbids ordinary OS tools, gives
Arconaut ownership of shared services, adopts a library, or promises zero-loss
capture across arbitrary producer/storage failure. Those boundaries should remain.

## Focused reread after integration

2026-10-01. Reread the revised operation, audit, CLM and adjacent generation
contracts. **All three findings are resolved at specification scope; no remaining
finding in this review's invariant.**

- Finding 1: the journal now defines appended versus a synchronized committed
  batch, binds source frames and semantic transitions through its commit frame,
  and permits authoritative ledger/head/authority publication only afterward.
  Callers' receipt of acknowledgement remains separate from recovered valid bytes.
  Dependencies cannot silently outrun retained source publication, and provisional
  displays cannot enter ordinary context. Later input, signals, messages and actual
  requests inherit the effect boundary and preserve partial outcomes.
- Finding 2: supported steps retain decisions/checkpoints, planned invocation
  identities and continuation disposition before dispatch. Repeated identical
  submissions return existing state; conflicting input is visible; deliberate
  retries remain distinct attempts. Lost unsupported frames require reconciliation
  or explicit restart with earlier effects exposed, rather than invented recovery.
- Finding 3: completions retain request-origin branches and structural obligations;
  incorporation into a head is an explicit CAS operation. Conflicts retain branch
  and candidate. Concurrent messages are selected explicitly. External-edit bases
  are retained from actual export, independently of mutable file contents.

This is resolution of behavioral ambiguity, not evidence that an implementation
honors it. Commit-format recovery and the selected sync/fault profile still need
the planned direct storage oracle; workflow recovery/dedup and late-result/CAS
behavior still need their direct failure/interleaving oracles. Those are now
derivable implementation obligations rather than unresolved specification choices.
