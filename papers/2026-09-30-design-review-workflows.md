# Workflow, context, and computing review of the integrated candidate

2026-09-30. Independent cross-unit review of `docs/BEHAVIOR.md` and the original
`docs/DESIGN.md`, under Blackbird. The latter now lives in
[foundation design discussion](2026-09-30-foundation-design-discussion.md).
This reviews proposed integrated contracts; it does not certify the reviewer's
earlier runtime recommendation, select a shipping language, or report implemented
behavior. No product, provider, installer, or benchmark was run. Only this paper
was written.

The inspected design was being revised concurrently. Findings below refer to the
snapshot with SHA-256
`48da46419b93cd3bbe731c2683f9c25dffac6189244d2c3247c0afcc2af280c5`
and 482 lines. The behavioral brief's hash was
`22605b1e9b2301d934de68fe598fbd23543583c53c4ab26bf3e139f4c5b31ba4`.
These identify the text reviewed, not an extra approval or verification layer.

Read current AGENTS, BLACKBIRD, FOUNDATION, README, journal, both candidate
documents, the workflow and audit studies, and selected primary references.
The operator's latest model-free/capability-outpost example remains in scope;
it is not evidence that a Gemini integration works and does not justify expanding
the outpost design.

## Final disposition check

Re-read the revised discussion paper and working brief on 2026-09-30. All four
findings are addressed in the proposed text. This disposition concerns the
original failure cases; it neither establishes implementation feasibility nor
adopts the proposal as a specification.

| Finding | Disposition | Revised discussion passage |
| --- | --- | --- |
| 1. Late protocol continuation across compaction | Resolved in discussion text: every ordinary provider admission checks structural constraints; request-specific branches retain pending obligations and original bindings; intervening messages remain pending until a valid boundary; abandoning a branch does not silently cancel its work. | Context and managed compaction, lines 273–286 |
| 2. Decision/action recovery | Resolved in discussion text: durable decision identity, selected inputs, continuation, and stable planned-action identities precede admission/dispatch; recovery looks up those identities; unrecoverable arbitrary frames stop for reconciliation. | Executable turns and workflows, lines 171–185 |
| 3. Migration failure guarantee | Resolved in discussion text: preservation applies to isolated/transactional conversion of owned state; shared-resource side effects require recorded disposition and reconciliation, including an unusable old instance. | Hot change and actual applicability, line 207 and lines 218–226 |
| 4. Compiled versus replacement changes | Resolved: live compiled Lisp definitions use declared activation; executable replacement uses quiescent refit. | Autodroit, lines 482–483 |

The brief also carries the decision-recovery and late-response concerns. Both
documents now state the operator's actual sequence: develop, review, and discuss
the brief, then write the specification in pen from the ground up. The scholarly
candidate is discussion input; runtime/storage choices and exact contracts remain
open. I found no material residual in the four reviewed failure cases. Their
direct checks below remain proposed evidence if these mechanisms are adopted,
not checks that have already passed or a reason to skip the specification work.

## Original findings

The following retains the original snapshot locations and reasoning as history.
`docs/DESIGN.md` locations refer to the identified original snapshot, not an active
file. Current proposed remedies are in the discussion paper linked above.

### 1. High — Old requests have no defined protocol continuation after context replacement

**Location:** `docs/DESIGN.md`, snapshot lines 182–187, 223–226,
237–253; [BEHAVIOR](../docs/BEHAVIOR.md), lines 154–170.

**Observed contract:** A sent request retains its actual context and original
tool binding. Compaction publishes against an expected context base; later
messages stay pending. Only the *default* context policy is required to preserve
provider call/result structure. The destination of subsequent observations from
an old request is not specified.

**Concrete failure, inferred from that gap:** Request R starts from context A.
Compaction publishes B while R is running. R then returns a tool call; a peer
message M arrives; the tool produces its result. Appending observations to B in
arrival order can omit the call that authorizes the result, place M between a
call and its result, or drop R's useful result to avoid the conflict. Expected-base
publication protects concurrent edits to a context; it does not identify which
protocol continuation must survive those edits.

This is an actual provider constraint, not merely a preferred transcript shape.
Anthropic documents that client tool results must answer the matching call in
the immediately following message, with result blocks preceding text; pending
server-tool continuations can impose further restrictions. Other providers need
their own rules. [Primary protocol documentation, consulted 2026-09-30](https://platform.claude.com/docs/en/agents-and-tools/tool-use/handle-tool-calls).

**Fix:** Give every request's later response/tool obligations an explicit
association with its request and context branch. The next request must either
preserve a provider-valid continuation, explicitly combine/rebase it, or record
that continuation's abandonment and replan. Provider-bound request admission
should validate structural obligations for all ordinary context strategies,
including user-defined strategies; raw protocol experiments can remain expressible
with their real failures. Do not silently repair by losing pending messages or
original observations, serialize every participant, or prohibit compaction while
work is in flight.

**Direct check:** Start R on A; publish B; return R's tool call/result with M
arriving between them; construct the next actual provider input. Check request
association, required ordering/pairing, M's inclusion status, and retained originals.
Repeat with a custom transform. The oracle is the concrete provider protocol and
known message identities, not successful summarization or token reduction.

### 2. High — Workflow decision recovery is disconnected from operation admission

**Location:** `docs/DESIGN.md`, snapshot lines 133–137, 160–164,
223–226, 328–337.

**Observed contract:** A decision receives explicit state and selected events,
then returns actions and continuation state. An operation is durably admitted
and can be looked up by the same client submission identity. External uncertainty
does not authorize replay. The design does not define the recovery relationship
between a decision, its action identities, selected messages, and continuation.

**Concrete failure, inferred from that gap:** A decision submits action A.
The service durably admits and executes it. Before preserving the new continuation
or A's client submission identity, the controller fails. Recovery sees the old
controller state and runs the decision again, producing A-prime with a fresh
identity. Per-submission deduplication correctly treats A-prime as new and repeats
the effect. This can duplicate an effect whose outcome is fully known; it is
distinct from uncertainty after an external attempt opens. Conversely, persisting
the continuation first can skip an action never admitted.

**Fix:** Define a recoverable controller-decision unit: identity, base state and
selected event identities, governing policy, resulting state/continuation, stable
identities for emitted actions, and their admission dispositions. Recovery must
reconcile those identities before advancing or dispatching again. Partial admission
must be representable. If a continuation cannot be recovered, retain an explicit
stopped/reconciliation-required condition instead of rerunning arbitrary code to
rebuild its stack. This need not require deterministic workflow replay or a new
durable-execution framework; a compact ledger contract can serve the existing
explicit decision boundary. Ordinary resident host frames remain ordinary live
frames, without invented crash survival.

**Direct check:** Fail the controller after one of two decision actions is
admitted but before its local continuation update. Recover against the authoritative
ledger. Independently count real external actions, inspect stable identities and
selected-message disposition, and verify that no automatic replay manufactures
another submission. Exercise the opposite boundary before any action admission.

The audit reviewer confirmed this is separate from their dispatch-owner epoch
and refit-cohort findings.

### 3. Medium — Failed state conversion cannot generally promise a usable old instance

**Location:** `docs/DESIGN.md`, snapshot lines 186 and 197–201;
[workflow study](2026-09-30-workflow-design-study.md), lines 254–266.

**Observed contract:** A migration prepares separate replacement state; failure
leaves the old instance usable. The design correctly denies transactional semantics
to arbitrary Lisp loading, but does not give the migration guarantee equivalent
limits. The underlying workflow study explicitly notes that arbitrary migration
side effects cannot be undone by moving an entry point back.

**Concrete failure:** Conversion prepares a new Lisp object, alters a shared
database schema or a persistent kernel value, then raises an exception. The old
Lisp object is unchanged but its external resource assumptions no longer hold.
Retaining its pointer does not make it usable. A shallow copy can similarly
retain aliases to mutable old state.

**Fix:** Limit the old-instance guarantee to a supported conversion whose owned
state is isolated or whose actual affected resources participate in a suitable
transaction. Account explicitly for shared references and external effects.
Unrestricted conversion remains possible, but failure must expose partial effects
and the actual recoverability of the old instance. A migration requiring repair
is not a successful automatic rollback. This clarification adds neither per-call
approval nor a ban on arbitrary programming.

**Direct check:** Have conversion change a shared resource and then fail. Inspect
the resource and old instance directly. The result must report the partial change
and necessary disposition, rather than promising that an unchanged heap object
means restored operation.

### 4. Low — “Compiled changes use quiescent refit” contradicts live compiled policy

**Location:** `docs/DESIGN.md`, snapshot line 439, against lines
166–170 and the hot-change table.

The candidate deliberately selects compiled Lisp policy with live compile/load
and declared activation. Its autodroit section then assigns compiled changes
generally to refit. SBCL compilation is not a synonym for replacing its OS
process: its manual describes a compiler-only implementation, while the inspected
SLY backend compiles and loads new code in the running Lisp.
[SBCL manual](https://www.sbcl.org/manual/#Compiler_002donly-Implementation),
[pinned SLY SBCL backend](https://github.com/joaotavora/sly/blob/3ffa216d0818972f7a7fea38a566a6b570349f3b/slynk/backend/sbcl.lisp#L761).

**Fix:** Say changes requiring executable/runtime replacement use quiescent
refit. Live compiled definitions use their declared activation/migration boundaries.
The distinction is already present elsewhere; this needs wording reconciliation,
not another experiment.

## Acknowledged research, not additional defects

The following unresolved work is material but already honestly represented as
feasibility work rather than implemented capability:

- **Language and model ergonomics.** The model can edit actual Lisp policy;
  Python is an ordinary workflow binding and kernel option, not a hidden owner of
  the systems core. Shared operations, concise familiar actions, useful handles,
  errors, and source inspection make this coherent. No evidence yet establishes
  superior model composition/repair in Lisp versus Python. The proposed real-task
  checks are necessary; protocol fixtures cannot settle model skill.
- **Retained hosts.** Long-lived Lisp frames belong in retained hosts just as
  Python frames do. The candidate does not promise that arbitrary suspended stacks
  migrate into a replacement main controller. Standing hosts stay alive paused
  during refit; independently managed daemons are the uninterrupted-work option.
  These statements preserve the latest operator constraint rather than reviving
  earlier active-job migration scenarios.
- **Binding retention through helper dependencies.** Fresh namespaces and explicit
  dependencies are a useful proposed mechanism, not yet demonstrated isolation.
  The live-load feasibility check should include an old tool calling a helper
  whose definition changes. SBCL's pinned `%coerce-callable-to-fun` returns a supplied
  function object directly but resolves a symbol through its current function
  binding. Retaining only a top-level tool reference therefore does not establish
  the semantics of all later indirect calls. Specify intentional late-bound
  dependencies and keep the rest consistent with the advertised generation.
  [Pinned source](https://github.com/sbcl/sbcl/blob/e0b6f381c2abb096fc2c4b00d1caead9b6f968be/src/code/fdefinition.lisp#L185).
- **Ordinary computing and owner failure.** Meaningful byte streams, input,
  environments, exit/signal observations, independent control, and process identity
  remain requirements. The candidate explicitly leaves actual macOS/Linux pause,
  descriptor custody, child-wait authority, and owner replacement for direct
  investigation. It does not claim that a signal-sent flag or transferred descriptor
  establishes full preservation. This is bounded open work, not evidence of a
  working process subsystem.
- **Outpost breadth.** Capability-only environments and independently attributed
  peers are represented without requiring every outpost to contain a model. The
  example neither compels a special search subsystem nor proves a provider feature.
  No further outpost architecture is needed for this review.

The original review requested contract-level corrections to findings 1–3 and
direct wording correction to finding 4. Those corrections are now recorded above.
A larger workflow framework, universal migration machinery, or another approval
ceremony would not substitute for these specific boundaries. The next phase
remains discussion of the brief followed by a fresh specification, not
implementation planning from this research candidate.
