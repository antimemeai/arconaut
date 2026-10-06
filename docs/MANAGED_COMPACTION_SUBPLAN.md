# Managed compaction and campaign

**B1 complete:** arconaut-uaa.9 closed after full scoped implementation and serious
campaign. Final actual results: papers/2026-10-06-compaction-campaign.md. Historical
subplan/checkpoint notes below retain the design and fault-resolution sequence.

2026-10-06. Operator: start managed compaction immediately for a serious campaign
today. B1 of AUTODEV_QUEUE is the implementation scope; its necessary inspection
and repair controls are included. Automatic thresholds/model-limit policy (B2),
complete branching, and general workflow-error recovery remain separate work.

## Purpose and current mechanism

The A1–A6 Arco self-development run reached roughly 207k provider input tokens.
ContextStore currently retains immutable originals and CAS full-view edits;
CodingEngine validates request call/result linkage, and Lua can directly edit.
Restore appends an absent original, which is insufficient for reliable batch
repair of archived call/result groups. Build on the existing audit, not a separate
compaction store. CORE_DESIGN, TESTING_PLAN, CLM source study and capabilities
synthesis are grounding; Arco reads their relevant sections and actual sources.

## Required behavior

Provide operator and model/Lua surfaces for explicit summarize, select, archive
presentation, and restore. Arco chooses API shapes after studying current code;
write them here before implementing. Choices require no approval prompts. The
model can author transformations, inspect what was changed and repair losses.
No automatic destructive policy or guessed tokenizer/model limit in this unit.

Every managed transformation records reason/mode, recoverable source (model/Lua
program or explicit input), input revision, selected entry identities, candidate,
acceptance/rejection and output revision. A rejection changes no live context.
Publish after durable audit commitment; stale base is rejected, never silently
rebased. New summaries have their own identity and ancestry; never falsify an
original entry's identity or erase its bytes. Keep generic CLM agency available.

Default activation is after the current turn/affected workflow finishes. A model
request made as a tool must not publish while its current function call lacks a
result. Specify staging, successful completion, failure/interruption cancellation,
and RRC ordering. Resume uses the accepted compacted view and retains the mission
and continuation note. Concurrent/overlapping proposals have explicit outcomes.
Operator commands use the same semantics, without a special approval mechanism.

Managed candidates must preserve valid function-call/result ordering and pairing,
reject duplicate/dangling IDs, and handle groups atomically for select/archive/
restore. Preserve active operator instruction/mission and explicitly retained
system/developer items; opaque provider data remains exact and repairable. Do not
invent provider-specific rewriting rules from generic item names. Original and
archive inspection is bounded/paged so repair does not dump the entire old context
back into the next request. Archive means remove from presentation, not audit.
Batch repair can restore selected originals in valid original order without
resurrecting already-executed effects or replaying provider requests.

A summary's semantic accuracy cannot be certified by structural checks. Expose
its coverage/source identities, allow model repair, and exercise actual fact-
retention oracles in the campaign. Name both what shrank and what was lost.

## Implementation sequence

1. Read actual context/coding/main/Lua APIs, relevant design/source study; refine
   API, activation and repair details here. Write direct failing oracles.
2. Implement one coherent core publication/ancestry/repair mechanism with replay
   compatibility. Add usable operator + Lua/model tools and source-retained
   transformations, bounded inspection. No new dependency.
3. Affected checks, rebuilt release/arco, quiet RRC and verify new behavior in
   replacement. Independent Kimi core review: protocol/CAS/atomicity/repair.
4. Integrate valid findings; execute the campaign below, independent Kimi review
   of campaign oracle weaknesses at a useful boundary. Final relevant Mac and
   Neuroses checks, docs/queue/journal/bead updates, honest result report.

## Campaign and direct evidence

Use isolated test sessions. Historical context/autodev/session is evidence:
never modify it. An intact copied native audit or deterministic representative
large-history fixture may be used for replay/scale tests. Keep captures in
context/compaction-campaign and results in papers/2026-10-06-compaction-campaign.md.

- Exact direct red/green cases: stale proposal, duplicate/unknown entry IDs,
  dangling/reordered/partial call-result group, incompatible mixed selection,
  empty/no-op/full archive, mandatory retained instruction, Unicode/opaque data,
  failed/interrupted transform, stage-before-tool-result and RRC ordering.
- Native replay: accepted changes persist, rejected edits do not, originals and
  ancestry remain exact, repairing a removed/edited group restores valid order;
  no repeated side effects or provider calls. Exercise crash gaps using existing
  fault tools where practical; do not claim power-loss evidence from process exit.
- Deterministic mixed sequences with an independent simple expected model: compact,
  select, archive, restore, append, reopen, stale-base rejection. Assert exact
  expected selected identities and original bytes after each step. Aim for >=100
  varied transitions if cheap; quantity never substitutes for the oracle.
- Live Arco exercises: carry mission/code-task/sentinels through a real summary
  and a distinct selection/archive path, RRC, then recover a deliberately omitted
  fact from originals and continue a real code task. Compare against predeclared
  expected facts/constraints; capture false summaries and repair, not merely a
  model's assertion it remembers. No need to resend 207k tokens just to create load.
- Scale: representative long history + large tool bytes; measure actual serialized
  input before/after, publication/replay/request assembly elapsed and retained
  audit growth. Report limits and workloads; byte reduction is not token accuracy
  or spend savings. Audit is expected to grow as the view shrinks.
- UI/plain usable commands: inspect candidates/results/originals, request compaction,
  resume and repair; meaningful actual-process/PTY oracle for new terminal flows.

Use C++ diagnostics and relevant debug/release/ASan+UBSan checks; Neuroses portable
qualification at a coherent snapshot after fixes. Mutation remains deferred,
no laptop mutants. Prefer existing test targets and failure tools, one direct
mechanism per fault class. No generic proof/receipt subsystem.

## Independent review and boundaries

Use existing kimi-colleague skill/scripts/kimi-review with narrow concrete briefs.
Inspect final assistant report AND child status; retain completed reports in papers.
One bounded retry on failed transport, exact captured session for followup.
Kimi challenges code and oracle separately at useful checkpoints, never per edit.
Do not wait for a new plan-review gate. No coding delegation is required: Arco
implements itself. Production C++/Lua only; existing POSIX launcher remains.
No new library/service adoption, Git staging/commit/push, mutations, neighbor or
quarantine changes. Shared services remain independently governed. Keep broader
milestone acceptance open. Finish this implementation and its campaign rather
than stopping at a demonstration or a long design document.

## Concrete B1 API (implementation decision, 2026-10-06)

`ContextStore::manage(proposal)` / model `context_manage {proposal}` / Lua
`arco.manage(proposal)` / operator `/compact JSON` share one explicit proposal:
`{base, mode: summarize|select|archive|restore, ids:[entry IDs], reason, source,
summary:{role:"developer",content:...}}`. Summary is required only for summarize;
source is recoverable explicit input/program description (the enclosing Lua source
is already audited). Select keeps selected entries; archive removes them; summarize
replaces selected entries at their first position with a fresh immutable synthetic
original with `ancestry` selected IDs in its entry metadata. Restore replaces edited
presentations with exact originals and merges absent entries by capture order.
Selected call/result intervals must be complete; ordering is validated independently
at publication. Unknown/duplicate IDs, stale bases, invalid mode/source/reason and
mandatory loss are audited rejections. All live system/developer entries and the
latest operator user message are retained exactly. Generic edit/append remain
available and deliberately are not restricted to this menu.

The engine marks workflow begin/end. A managed tool stages at submission, recording
proposal plus the exact input snapshot and ID. One pending proposal per workflow;
overlap rejects. It does not alter the current tool exchange or future requests in
that workflow. Successful workflow completion settles append-only continuation:
only engine/context appends may advance the pending expected revision; generic edits
invalidate it. Publication records submitted base, settlement base and preserves
all appended suffix entries (including the proposing tool and its results). This
is an explicit audited continuation operation, not silent stale-CAS rebasing.
Failed/interrupted workflows cancel; restart validation happens after settlement.
Crash/reopen does not activate orphan staged work: retained stages are evidence,
not executable obligations. Operator idle commands publish immediately at their
already-quiescent command boundary. No speculative provider calls or effects replay.

`context_inspect {query}` / `arco.inspect(query)` / `/inspect JSON` provide bounded
original/history inspection: kind, offset, limit and optional entry ID. Byte ranges
of serialized exact source are returned hex (safe for UTF-8 split points), total
bytes and next offset; default 4096, maximum 65536 bytes. Identity index is available
as kind `index`, records include chronology and ancestry. History is retained context
packets, not reconstructed successful-only edits. Existing unbounded generic original
surface remains compatible; managed repair never requires dumping all originals.

First coherent unit: core store plus direct malformed/CAS/group/ancestry/repair and
staging tests; engine/operator integration and campaign follow without a new gate.

Prequalification refinements: all live user messages are mandatory, not only the
latest, so a continuation prompt cannot unpin the earlier mission. Assistant-role
summaries are supported and recommended for facts; developer-role summaries are
instruction-like and thus pinned. Select automatically carries the open current
tool interval into settlement, in addition to appended outputs. Exact restore first
removes selected presentations then merges originals, repairing reordered groups.
Model `context_manage` may omit base to explicitly bind the invocation-time snapshot:
the resolved concrete proposal is audited. A supplied base always remains strict CAS.
This convenience is necessary because a provider's function-call append advances the
revision after a context_view result. Lua/store/operator proposals retain mandatory
base and can read-and-submit within one operation. No stale supplied base is rebased.

## Initial boundary / restart continuation

Core Kimi review run-tkhnw446 completed wrapper0/child0; unchanged final report is
papers/2026-10-06-compaction-core-kimi-review.md. Findings1/2 independently found
and fixed during its run: model omitted-base explicit invocation binding, plus
select preserves current open interval. Added actual scripted provider response ->
function call -> tool -> result -> settlement oracle, not only direct Lua/store.
Finding4 fixed: failure always clears RAM workflow/pending; when audit is blocked
it cannot record cancellation and says audit unavailable. Finding5 adds explicit
capture index/revision in bounded index. Finding3 clarification: all ContextStore
appends, including Lua `arco.append` (the standard workflow appends tool results
through that API), are explicit append-only continuation operations; generic full
view edits invalidate. Appended malformed protocol intentionally rejects settlement;
append is not authorized to bypass protocol. Do not whitelist provider origin names
and break the actual governing Lua workflow. The appended source/origin and exact
suffix are retained in native packets. This is intentional CLM agency, not an
implicit transformation of the staged selection. Review gaps in larger paging,
restore mixes and campaign scale remain campaign work.

Next turn after first rebuilt quiet RRC: verify new tool/API replacement behavior;
continue campaign rather than finalizing. Build a simple independent deterministic
expected-view model and >=100 varied transitions (mixed summary/select/archive/
restore/append/stale/reopen/cancel), include physical reopen, unknown/invalid range
inspection and byte roundtrip. Add representative history measurement capturing
exact serialized input bytes/audit growth/publication/replay/request latency.
Run actual live model-authored summary and distinct archive/selection, predeclare
retained facts and deliberately omit then recover one fact with bounded originals.
Continue actual C++ code task after compaction and another RRC; preserve mission,
constraints and protocol groups. Campaign oracle Kimi review is still required.
Final Mac debug/release/ASan diagnostics and Neuroses run only after fixes on one
coherent source snapshot. Do not edit concurrently with final qualification. Update
journal/queue/bead, bd backup, final results papers/2026-10-06-compaction-campaign.md.
Only close arconaut-uaa.9 after full campaign; broader acceptance stays open.

## Operator correction on source use and colleagues

Before continuing campaign, read context/compaction-campaign/operator-steering.md.
Primary paper/refimpl/test reading must actively inform or challenge this design
and its oracles; our study summaries alone do not satisfy the research step.
Colleagues are Kimi, MiMo, a separate ChatGPT context, or Claude, using actually
configured interfaces and independent review contexts. Existing specific operator
exceptions prevent another plan gate or premature mutation campaign.

## Primary-source-driven campaign unit (after operator correction)

Firsthand CLM paper §§3/4.1/4.2, original harness/edit gate/evolution; contrasting
gptme master-original compaction/resume and actual stale-summary test; letta-code
actual summary transcript parity tests read. Concrete design re-evaluation and
source-to-oracle consequences: papers/2026-10-06-compaction-source-consequences.md.
No missing acquisition: existing originals suffice, never executed or changed.

Written unit: add a native isolated mixed-transition fixture to existing CTest
suite with independent logical capture-order/original-byte model, >=160 scheduled
varied steps (selection/archive/summary/restore/append/stale/stage cancel/reopen),
exact view/items and immutable originals after each step. Compare synthetic IDs
against returned revision only to bind generated identities; expected contents and
selection come from the independent model, not actual output. Add bounds and paged
byte-roundtrip cases. Direct red checks for malformed protocol must precede fixes:
malformed type/call IDs/parallel order and sparse edited-group repair, source-inspired
stale summaries and orphan stages. Capture representative history metrics separately.
Then actual live semantic/task campaign and independent Kimi oracle review remain.

Scale workload decision before implementation: isolated native fixture of 64 completed
provider-like tool turns, each 8192-byte result plus opaque reasoning/narrative and
one protected mission. No historical-session edits or provider traffic. Record native
file bytes and exact serialized input before/after explicit summary, wall latency for
publication and physical reopen, and owned request assembly+audit+mock response (not
network/inference latency). No token estimate/cost claim. Assert immutable exact
originals on reopen and no provider effects replay. Current native full-view packets
and in-RAM history copies may grow quadratically; measure that limit honestly, not
hide it behind favorable reduced-input bytes.

## Oracle checkpoint interruption and follow-through

Compound default120s exec exceeded deadline while ASan campaign ran; root cooperative
interruption retained, not a test failure. Oracle review run-mz4dltlh has substantive
final text but no result.json, wrapper/child absent: incomplete, unchanged text in
papers/2026-10-06-compaction-oracle-kimi-incomplete.md. No automatic replay. Run
individual checks with explicit600s; standalone review >=wrapper timeout+cleanup.
Never background reviewer in killable tool process group.

Valid oracle gaps to fix: provider boundary exact input before *and after* settlement;
paged expected serialization must derive wholly from independent model originals,
including total/next/EOF/invalid offset; persistent generic reorder then sparse
restore/reopen. Developer summary pinning direct case. Malformed representation
repair must be tested through real Lua/engine flow: explicit archive can remove an
untyped malformed entry by ID, so generic full-view reset is not the only repair.
A model request cannot assemble malformed presentation, but Lua-only workflow may
inspect/archive it without requesting the provider; no side-effect replay. Test that
flow rather than accept the incomplete review's unverified permanent-brick claim.
Actual review findings remain leads, not completed independent acceptance.

Completed standalone oracle review run-0yv51w07 wrapper0/child0 final inspected and
preserved unchanged papers/2026-10-06-compaction-oracle-kimi-review.md. Next narrow
fixes: malformed suffix must reject at settlement without changing appended view;
multiple engine tool rounds preserve pre-settlement context and full suffix; explicit
beyond-EOF inspection contract is clamped empty with next=total; recover rejected
candidate from bounded history after reopen; native reopen after reordered group
repair; scale bounded exact original repair. The independent160 logical model uses
role/reasoning data, not randomly malformed tool protocols: targeted protocol tests
are a different oracle, report that limit rather than inflate randomized coverage.

## Live campaign predeclaration (before first summary)

Independent expected facts/constraints fixed in context/compaction-campaign/live-oracle.json.
Plant an actual assistant source record, authored by current live model, with marker,
measurement and next useful C++ task facts plus a recovery code deliberately omitted
from summary. Author an actual managed summary of old live campaign context, retain
all live user/system/developer instructions and current call groups. Source original
ID captured to file for bounded repair; do not include omitted value in summary/RRC
note. Before repair, compare presented summary against predeclared retained facts and
verify omitted value absent via local oracle (do not read expected value into model).
After compact+RRC, recover exact omitted fact from bounded original ID, independently
compare source with declaration. A distinct archive/selection must also activate and
repair. Actual code task after first compact+RRC: optional bounded inspection revision
CAS guard, motivated by original gptme stale snapshot/test. Write red -> implement ->
green -> rebuild and another RRC, then verify commands/provider continuation. Final
Mac/Linux checks on coherent integrated image remain, no final early.

## Productive continuation: revision-bound inspection (after live compact+RRC)

Question: can a caller assemble multiple pages from different context snapshots
without noticing? Original gptme resume.py snapshot comparison and its actual
concurrent-unlock stale-result test reject obsolete derived output. Apply the same
principle to read pagination: optional query `revision` must equal current head,
otherwise throw conflict before serialization; malformed non-string revision throws
corrupt. Omission remains an explicitly unguarded read, existing consumers unchanged.
No new historical snapshot materialization. Caller obtains revision from first page,
binds later pages, and restarts a fresh inspection on conflict. Conservative guard
also rejects after presentation-only changes even if selected original is unchanged.
No audit mutation from inspect success/rejection. Direct test all three kinds with
current revision exact equality; append invalidates old guard, compare cursor/head;
check wrong-type revision. Then implementation/schema help, green and independent
narrow review. Rebuild release plus affected tests and RRC; verify actual tool guard.

Independent inspection review run-yarf2yrz completed wrapper0/child0 and identified
real torn-history fault: rejected manage/edit grow history without moving view head.
Revise guard to bind **payload snapshot**: originals/index conservatively use head;
history uses latest history packet revision (empty history uses head). Response
`revision` is the matching per-kind cursor; add `context_revision` for view identity.
Callers must bind response revision, not guess from context view. Rejected proposals
must invalidate history pagination even though view remains exactly unchanged.
Direct red for rejection-only mutation, then fix and rerun. Original history packets
already carry unique revision and replay them; no new storage/ancestry mechanism.

## Live fault: bounded history cannot depend on aggregate JSON ceiling

Actual rebuilt-image Lua verification failed capacity at history inspection before
rejection probe; no saved completed result and no replay of uncertain manage.
Distinct archive absence/individual exact source recovery verified separately.
Full history serialization uses default16MiB JSON ceiling even for7byte query; live
native audit239MiB. Bounded output is not enough if aggregate source cannot be read.
Original gptme engine uses preserved master-log byte-range references: range reads
must not depend on entire master fitting one request/document. Our owned JSON
encoder already qualifies each context packet when audited. Inspect arrays as
virtual concatenation '[' + individually serialized records separated by ',' + ']';
retain only requested overlap and count total with checked size_t arithmetic. Do
not copy aggregate history/captures or raise global JSON safety limits. Index entries
likewise serialize individually; targeted original remains one object. Validation
of offset/limit before traversal. O(history bytes) CPU still, bounded extra memory
by one accepted record plus page, honest limit; no cache/threshold policy.
Direct red: existing64turn scale history>16MiB, independently concatenate original
native context packet payloads to expected JSON; compare sevenbyte pages at start,
midpoint, around16MiB, tail, beyondEOF and total/next. Existing small exact paged
original/index/history tests stay green. Implement -> green -> narrow independent
Kimi review/fix -> rebuild release+affected tests/RRC and repeat live guard once
qualified. This is a genuine campaign fault, not a benchmark or receipt test.

## Final qualification checkpoint, last native verification outstanding

Large history direct red fixed, Kimi qd_apwqg completed0/0 no correctness faults.
Incremental range cost O(history bytes)/page, memory one packet plus bounded page.
Actual plain8process operator command campaign passes; initial Ruby2.6 harness
method fault resumed retained source plant without replay. Mac and Neuroses relevant
11cases in debug/release/ASan+UBSan all pass on unchanged product source snapshot;
seven relevant TUs diagnostics clean. Native live exact two-entry restore staged;
latest release built, RRC next activates repair AND new history serialization code.
After resume verify exact restored chronological pair+all user originals, then actual
history sevenbyte bounded result and stale revision conflict after rejection-only
mutation, no view change. If green, final report/queue/bead closure .9 only. Do not
start dependent pilot or close broader acceptance until completing this verification.

## Scoped completion

Latest native replacement verified live restored pair exact/adjacent at14/15 and
all seven pinned entries equal immutable originals. Actual history7byte page from
104245469bytes succeeds; fresh guard equal, rejection-only mutation leaves view
unchanged and invalidates old history guard with conflict. First verification probe
compared lossless Lua JSON-number tables by identity and asserted incorrectly;
external_unknown/no result file retained, read-only diagnostic checks confirmed
repair, no replay of effects. Use encoded/value comparisons, not table identity.
Full actual campaign report papers/2026-10-06-compaction-campaign.md now final;
qualified snapshots/reviews/live continuity/operator commands complete. B1 .9 may
close; B2/general acceptance remain separate. No further product source changes.
