# Integrated design review: audit, state, and executable control

2026-09-30. Independent review of the parent's candidate `docs/BEHAVIOR.md` and
`docs/DESIGN.md`, against FOUNDATION and the three design studies. Locations
below refer to the candidate read during this review. The reviewer authored the
audit/state study; that study is source input, not something this review certifies.
No source archive verification, new survey, execution, or feasibility experiment
was performed. Only this review file was written.

**Final disposition: all three findings are resolved in the discussion text.**
The current artifact is the
[foundation design discussion](2026-09-30-foundation-design-discussion.md), moved
from `docs/DESIGN.md` after the operator clarified the phase: develop, review, and
discuss the brief, then write the specification in pen from the ground up.
BEHAVIOR now reflects that sequence. This review resolves its stated textual
findings; it does not approve a specification, select a runtime, or establish
implemented behavior. Original locations and the intermediate dispositions remain
below as snapshot history, not current unresolved claims.

## 1. P2 — Query results and live audit watches lack a complete, finite capture contract

**Locations:** DESIGN lines 246–282, 291–305; BEHAVIOR lines 219–228.
[Audit and query state](2026-09-30-foundation-design-discussion.md#original-audit-operational-ledger-and-derived-study-state),
[recording](2026-09-30-foundation-design-discussion.md#recording-and-effects).

**Failure:** an operator/model runs a query against a derived view, receives its
result, and uses it to change the harness. Later the view definition changes or
the projection is rebuilt. The integrated design names expressive SQL, raw
inspection, applied cursors, and original participant ingress/egress, but does
not explicitly assign capture of the query request and actual returned result
to an operation path. Recording a SQL string and database cursor does not recover
what an old formatter, view definition, or model-derived annotation actually
returned. Letting analysis return directly to a client also bypasses the stated
durable-before-publication rule.

A naive application of that rule creates another failure: a live audit tail emits
records; its own delivery is recorded; the new delivery record is emitted and
recorded again indefinitely. Both problems arise from the integrated standing
query surface, not from unobservable external-world activity.

**Fix:** explicitly make query/inspection requests, failures, and actual emitted
result representations part of the shared recorded operation path. Retain their
view/derivation identity and finite source cursor; exact immutable byte references
can avoid duplicate payload storage. Specify that a watch's own delivery
bookkeeping remains available to finite historical queries but does not feed
back into that watch automatically. Internal append/persistence steps must not
recursively generate participant actions. State which component performs capture
before returning results from the separate derived database. This is original
interaction evidence, not another system of audit certificates.

**Second-pass disposition:** revised DESIGN lines 301–309 explicitly capture
actual results and their cursor/derivation before exposure. That resolves the
query-capture omission. Its watch wording excludes each watch's own delivery
bookkeeping, but two watches can still feed one another if other watches' delivery
records enter their automatic feeds: A observes B's delivery, emits delivery A1;
B observes A1, emits B1; this repeats without new workload. Clarify that the
automatic workload feed excludes audit-observation/delivery bookkeeping globally,
including other watches. Keep those records available through bounded historical
queries/explicit finite cursors. The earlier source study's own-watch-only
suggestion had this same weakness; it is not sufficient merely because the study
recommended it.

**Final disposition: resolved in discussion.** The current discussion's lines
341–352 retain query requests and actual result representations before exposure,
with cursor/derivation identity. Every default automatic workload feed now
excludes the entire audit-observation/delivery-bookkeeping category, including
other watches; those records remain retained and explicitly queryable at a finite
source cursor. This closes both self-recursion and mutual-watch recursion without
discarding originals. No runtime test is implied.

## 2. P2 — Asynchronous study views need an explicit exclusion from authoritative decisions

**Locations:** DESIGN lines 96–98, 143–147, 163–170, 266–282.
[Controller decisions](2026-09-30-foundation-design-discussion.md#executable-turns-and-workflows),
[activation boundaries](2026-09-30-foundation-design-discussion.md#hot-change-and-actual-applicability),
[ledger and projections](2026-09-30-foundation-design-discussion.md#original-audit-operational-ledger-and-derived-study-state).

**Failure:** a definition/context/owner transition is durably committed, while the
derived database remains behind. The next controller decision obtains what it
believes is current state from that database and prepares a new request using the
old selection. The candidate says a decision receives explicit current state,
and that a changed policy applies at its next declared boundary, but only requires
the projection to store an applied cursor. It does not say that the cursor is
returned/enforced or that operational admission reads the authoritative ledger.
An expected revision on a later write is insufficient when the stale read produces
an otherwise valid new operation rather than a conflicting overwrite.

**Fix:** require the controller's current-state decision snapshot and normal
admission validation to use authoritative ledger state at a named boundary.
Return the applied cursor and derivation version on every derived-view response;
a caller requiring a newer cursor waits or receives an explicit unavailable/behind
result. A study query may intentionally read history, but it cannot silently
become authoritative current context, definition selection, or conversation
ownership. Preserve captured old tool bindings for requests that really were
admitted earlier; do not conflate that deliberate historical binding with a new
request accidentally built from a stale projection.

**Second-pass disposition: resolved in design.** Revised DESIGN lines 307–309
requires authoritative-ledger decisions and explicit applied cursors, waiting,
or not-yet-materialized results. This closes the stale-view ambiguity. It remains
an implementation contract to check later, not a claim of executed verification.

**Final check:** the same correction remains in the current discussion at lines
350–352. Finding 2 remains resolved as a discussion-level contract.

## 3. P2 — The allowed controller-local provider driver needs an independent control contract

**Locations:** DESIGN lines 46–49, 61–66, 129–133, 156–159, 301–305, 343–350;
BEHAVIOR lines 81–88 and 137–141.
[Physical lifetimes](2026-09-30-foundation-design-discussion.md#responsibilities-and-physical-lifetimes),
[refit settlement](2026-09-30-foundation-design-discussion.md#refit-as-a-quiescent-operation).

**Failure:** request R is active through a provider driver implemented as an SBCL
controller module. The programmable policy then blocks or the controller's
ordinary request-processing path stops making progress. The independent service
can record a cancellation request, but the candidate gives it no stated control
handle for R's transport or supervised driver. The only code able to stop/read
the request may be the same blocked control path. Accepting a control operation
does not settle R, and refit cannot use that acceptance as evidence of zero
active main requests.

This does **not** mean controller modules are inherently unsuitable, or that
cancellation always must succeed. The missing contract is how either permitted
driver placement preserves independent control and truthful local request state.
“Provider packaging remains untested” does not decide which component owns this
obligation.

**Fix:** every admitted provider request must register an identified control and
observation path that progresses independently of its participant's policy, or
declare a specific unsupported control with an explicit disposition. Name who
owns the local transport lifecycle and who observes its closure. A separately
supervised driver or independent controller I/O/control lane are possible
mechanisms; this review selects neither. Closing the local transport can settle
local activity while leaving the remote outcome unknown. Any forced controller
termination fallback must expose that broader effect rather than masquerade as
cancellation of R alone. Driver output must still pass through durable capture
before policy/tool execution sees it. This adds no per-call approval mechanism.

**Second-pass disposition: resolved in design.** Revised DESIGN's responsibility
table and lines 68–75 require an independent registered transport lifecycle handle
before dispatch, control without a responsive SBCL/Python policy frame, independent
capture/local completion, and explicit separation of local closure from remote
outcome. Actual transport feasibility remains open as stated; that uncertainty
is not an unresolved version of this contract finding.

**Final check:** independent registered transport control remains in the current
discussion at lines 72–79. Finding 3 remains resolved as a discussion-level
contract. The autodroit wording also now distinguishes live compiled Lisp policy
activation from controller/native executable replacement through quiescent refit.

## Findings not raised

The candidate faithfully preserves the operator's self-improvement principle,
hot turn semantics, compaction choices, live peers, shared expressive workflows,
model ergonomics, and near-zero routine approval friction. It clearly labels
SBCL/Rust/SQLite as recommendations and does not claim superiority established
by benchmarks. The narrow compiled owner is a reasoned candidate compatible
with the conditional compiled-component preference, not a silent adoption of
the inherited Rust partition.

The audit correctly retains failed and abandoned work, separates originals from
views, treats credentials as retained protected originals with explicit views,
and distinguishes acknowledged capture from an unknowable crash tail. Unknown
external effects are not blindly retried. No finding demands syscall omniscience,
a tamper-proof sandbox for arbitrary programs, or a second layer of receipts.

Tool binding across an in-flight request is explicitly protected, including an
unavailable-old-version outcome; the design does not silently reinterpret an
old call under a new schema. Refit correctly uses settled requests and preserved
paused programs. Owner replacement and platform pause feasibility are acknowledged
open obligations, not findings merely because experiments have not happened.

Autodroit keeps effective candidate/evaluator/configuration lineage and failed
trials, accounts for adaptive selection, and treats changes to measurement as
interventions. Source revert is not misrepresented as reversing live state or
external effects. No unsupported generalization/adoption claim was found in
those sections. The three findings above concern integration contracts needed
to make those otherwise sound requirements compose.
