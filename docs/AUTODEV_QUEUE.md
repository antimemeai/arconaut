# Arco autodevelopment queue

2026-10-03. Operator authorized a substantial feature queue, occasional independent
Kimi sanity checks/reviews, and autonomous implementation through Arco. This is
execution steering under CORE_DESIGN and the milestone-one course correction,
not a replacement architecture or another approval gate.

## Starting point and priorities

C++/Lua coding, native tools, CLM edits/repair, audit, terminal streaming,
persisted settings/identity and actual restart/resume/continue work today.
Arco has performed a real C++ edit, build, test, restart and continuation.
That does not establish daily-development readiness. First make ordinary work
pleasant and sustained context manageable, then expand programmability and peers.

Reference grounding: CORE_DESIGN.md, USING_ARCO.md, TESTING_PLAN.md,
RRC_SUBPLAN.md and the source-linked papers/capabilities/SYNTHESIS.md. Consult
the relevant dossier/reference before implementing a behavior borrowed from it.
References remain study-only. C++/Lua are production; Python is development-only.
No new library adoption or mutation campaign is authorized here. The operator
subsequently authorized checkpoint commits and pushes; see the publication rule below.

## Run now: everyday development

These are ordered conceptual units. The first run implements A1–A6, reviewing
at the boundaries below; the remaining queue is the next work, not a claim that
all of it can be safely implemented in this one run. Status starts queued;
Arco updates this document with the actual result and relevant evidence.

| ID | Feature | Concrete completion condition | Status |
| --- | --- | --- | --- |
| A1 | File reads by range | Optional line or byte range with precisely documented indexing, invalid-range errors and EOF behavior; existing full reads compatible; original bytes retained; direct boundary/binary tests. | complete; resumed range verification, pair review and Mac/Linux checks green |
| A2 | Bounded process results | Optional model-facing output budget with explicit omitted-byte metadata and retrieval from retained audit chunks/originals without rerunning the command or adding an output store; original stdout/stderr chunks remain audited; valid encoding and failure/timeout status preserved. | complete; resumed bounded exec/audit suffix verification and pair checks green |
| A3 | Context and request size visibility | `/context` or a focused command and Lua API report entries and serialized byte counts; provider usage displayed when actually returned; token estimates explicitly estimates; no credential exposure. | complete; resumed counts/usage verified; pair review and Mac/Linux green |
| A4 | Persistent terminal drafts/history | Recover unsent composer and sent-prompt history across ordinary exit/RRC; queued submissions restore only as drafts requiring explicit submission, behind automatic RRC continuation; existing 1 MiB draft/128-entry history bounds, actual PTY reopen oracle. | complete; replacement PTY reopen/RRC verified; pair checks green |
| A5 | Session discovery | List local sessions with last activity/configuration and explicit resume command; tolerate damaged entries without silently repairing audits; stable session selection. | complete; live replacement discovery, final review and Mac/Linux checks green |
| A6 | Comfortable long activity | Clearly visible turn/tool elapsed time, cancellation and completion; long output remains scrollable, composer usable during work, resize keeps cursor/layout valid; targeted PTY scenarios. | complete; replacement comfort PTY verified; final review and Mac/Linux checks green |

A1/A2 should retain results through the existing audit/context path. Do not add
a second output durability subsystem. A3 can report bytes without pretending to
have a tokenizer. A4 drafts are UI state, not automatically submitted context.
Session listing must not contend for or mutate live session journals.

### A1–A6 result

All six implemented and exercised through compiled replacement images. Final
focused terminal/PTY/session_store/RRC/coding checks pass 5/5 on Mac debug,
release and ASan+UBSan, and on Neuroses in the same three profiles. Earlier A1/A2
and A3/A4 affected cases and independent pair reviews are recorded in JOURNAL.md.
Completed review reports are in papers/2026-10-03-a1-a2-kimi-review.md,
papers/2026-10-03-a3-a4-kimi-review.md and papers/2026-10-03-a5-a6-kimi-review.md
(with the final narrow followup alongside). Failed/intermediate checks and review
timeouts remain recorded; they are not counted as qualification.

Scope limits: full-file/process collection still has the existing 16 MiB capacity;
request/usage summaries are process-local; UI snapshots do not promise exact
keystroke recovery across arbitrary crash/power loss. Save-before-dispatch avoids
replay but can lose pending UI work in the save/dispatch crash gap. Discovery is
nonrecursive derived metadata; legacy config needs ordinary reopen, and failed
snapshot updates can leave stale config. UTF-8 editing is codepoint-level, not full
grapheme support; supported terminal minimum remains 12 columns by 10 rows.
Existing arbitrary Lua workflow-error linkage repair and unfinished file/process
recovery remain broader defects, not resolved by these features. Core milestone
and operator acceptance remain open. B1 managed compaction is now complete with
explicit choices, retained originals, revision/source audit and protocol-safe repair.
Next recommendation: the operator-sequenced source-grounded useful-work pilot;
no automatic destructive policy implied by byte visibility.

## Next: sustained work and context programming

| ID | Feature | Completion condition | Status |
| --- | --- | --- | --- |
| B1 | Managed compaction | Explicit model/operator choices: summarize, select, archive presentation, restore; transform source and input/output revisions audited; preserve system/tool protocol and recover originals. | complete; explicit APIs + serious independent/live/scale/repair/productive-RRC campaign, final Mac/Neuroses qualification and concrete reviews integrated. arconaut-uaa.9 closed; papers/2026-10-06-compaction-campaign.md |
| B2 | Context budget policy | Configurable trigger and target, provider/model limit source explicit; model can refuse/revise a destructive proposal; edits activate at safe boundaries. | queued |
| B3 | Context inspection/repair UI | Navigate current entries and originals, show edit/removal history, restore selected spans and record repair reason; usable after over-compaction. | queued |
| B4 | Workflow failure repair | Failed custom Lua program cannot strand ordinary requests behind unpaired function calls; retain failure and expose an explicit repair path without replaying effects. | queued |
| B5 | Large-context performance | Measure actual replay/assembly latency on retained sessions; remove demonstrated repeated scanning/copying while preserving exact audit/request semantics. | queued |
| B6 | Branch and merge context | Fork selected context with ancestry; explicitly import peer findings, preserve source IDs/protocol; no implicit conflation of conversation identities. | queued |

## Then: programmable tools and workflows

| ID | Feature | Completion condition | Status |
| --- | --- | --- | --- |
| C1 | Lua tool registry | Model/operator define schemas and implementations dynamically; calls and definitions audited; native effects still admitted through core. | queued |
| C2 | Program modules | Audited module loading, named configuration and source/version inspection; reload at turn/workflow boundary; failed candidates keep working generation. | G4 candidate015836b; native activation/use pending |
| C3 | Expressive workflow control | Compose sequential/branch/retry/join programs with explicit cancellation and uncertainty semantics; no automatic retry of unknown side effects. | queued |
| C4 | Hot configuration | Inspect queued versus effective model/tool/workflow settings; default turn/workflow conclusion activation; interrupt-and-apply operation. | G4 candidate staged request defaults + combined inspection; workflow-selector/atomic interrupt-apply deferred |
| C5 | Shared service clients | Connect to external DB/kernel/service through audited tools, retain client/result identities; lifecycle belongs to service owner. | queued |
| C6 | Skill discovery | Small metadata inventory and on-demand source loading, project/operator precedence explicit; no massive instruction dump. | queued |

## Autodroit and operational diagnosis

| ID | Feature | Completion condition | Status |
| --- | --- | --- | --- |
| D1 | Autodev control surface | View/edit queue and running objective, pause/resume and steer work at boundaries; persist direction through RRC; model retains substantial planning agency. | queued |
| D2 | Experiment workflow | Record hypothesis, baseline, candidate, actual measurements and promotion/rollback; model-governed policy tunable from manual to autoresearch. | queued |
| D3 | Model rageshake | Complaint captures context/revision, program, model, active effects and relevant audit references; creates bead and delivers to an external DB table when that service is selected; offline complaint retained without obligating immediate repair. | queued |
| D4 | Audit exploration | Query/search actual retained effects, original bytes, context revisions and program/request lineage; one retention authority, derived indexes rebuildable. | queued |
| D5 | Failed image fallback | Explicitly selected prior runnable executable after failed boot; retain restart intent/diagnostics without restart storm or uncertain provider retry. | queued |
| D6 | Crash recovery expansion | Study unfinished file/process effects and implement bounded explicit reconciliation; unknown remains unknown; never invent successful completion. | queued |

## Peers, remote work and deeper runtime evolution

| ID | Feature | Completion condition | Status |
| --- | --- | --- | --- |
| E1 | Kimi colleague from Arco | Existing subscription CLI, independent review sessions with exact resume IDs, raw captures and inspected final results; practical steering without spawning ceremonial tribunals. | queued |
| E2 | Multi-provider ergonomics | Provider capability/configuration interface; concrete auth/protocol qualification before use; default remains current OpenAI subscription scaffold. | queued |
| E3 | Local/remote outposts | Selected context goes to independently runnable peer, message return to main chat, visible identity and audit; main refit and concurrent peer roles distinguished. | queued |
| E4 | IRC-style live collaboration | Multiple models/operator share addressable discussion and work lanes; per-peer context and tool/result identities, explicit interruption/routing. | queued |
| E5 | SSH-native work | Tools execute where files live with retained remote host/path identity, clear disconnect outcomes and no local/remote path confusion. | queued |
| E6 | Native hot reload | Start from source-grounded ABI/retirement/migration design; compare selected mechanism candidates; no library adoption by implication; RRC remains useful fallback. | queued |

## Execution and occasional independent review

For each whole unit: read actual source and relevant design; short written subplan;
direct meaningful red oracle; implement; green affected checks; journal and update
status. Native changes build release/arco, then invoke restart with changes,
checks, next unit and outstanding findings. After resume, verify the new behavior
from the new process before proceeding. Lua-only changes use next-turn reload.

Kimi checkpoints: initial queue sanity check, after A1/A2, after A3/A4, and at
completion of A5/A6. Earlier check only when a concrete invariant/architecture
question warrants it. Use installed kimi-colleague skill and scripts/kimi-review;
provide bounded source/fault-class briefs, inspect actual final and child status,
preserve completed reviews in papers, integrate valid findings. A failed review
is a failed review, not approval: one bounded retry, record it, continue unrelated
work. No compulsory Kimi review per edit or another operator approval cycle.

Run affected debug/release checks; sanitizers for native lifetime/buffer changes;
Neuroses for portable changes at meaningful batch boundaries. Avoid full-suite
ritual after every small edit. Mutations remain deferred. Do not edit quarantine,
neighbor projects, credentials or preexisting unrelated source. Commit and push
completed work/checkpoints under the publication rule below.

If a new dependency/service decision is needed, put that feature in blocked state
with a concrete explanation and continue independent queued units. Do not make
its adoption implicitly. At A1–A6 completion report actual achieved behavior,
remaining defects and first recommended next unit; milestone acceptance belongs
to the operator. During the run keep logs/session inspectable and update the
journal. Back up beads at meaningful checkpoints and at completion.

## Operator sequence update — 2026-10-06

**B1 managed compaction (complete) → useful-work evolution pilot → giga.** B1 .9
is closed after real live summary, exact bounded repair, productive native coding,
inhabited RRC and final Mac/portable qualification. The three-lane
frumentarii survey is complete; read
[papers/frumentarii-2026-10-06/SYNTHESIS.md](../papers/frumentarii-2026-10-06/SYNTHESIS.md)
and its source-grounded reports before planning the next pilot.

The pilot produces an audit explorer/evidence-linked rageshake and a contrastive
Lua diagnosis workflow, then compares actual useful work against equally funded
fixed-harness retries. Native RRC participates when the hypothesis needs a native
change; no general optimizer platform first. Preserve fresh subsequent tasks,
unknown outcomes and all provider/build/operator effort. Product correctness and
acceleration are separate claims. This sequence does not launch the pilot before
B1 completes or authorize dependencies through reference acquisition.

Next design discussion: integrations and HUD, after settling this pilot's scope.
Colleagues: Kimi, configured MiMo, independent ChatGPT, Claude, Grok as available;
operator explicitly directs MiMo/ChatGPT fallback for persistent Kimi failure.
Never count timed-out/missing reviews as completed or stall on one provider.

B1 live checkpoint: actual summary+RRC retained six predeclared literals, omitted
fact recovered exactly by bounded native original. Actual productive C++ inspection
revision task red/green/review integrated; review caught/fixed rejection-only torn
history, per-kind cursor now returned. release/arco rebuilt, release4/4 and ASan4/4
pass. Distinct closed call/result archive staged for end-of-workflow/RRC, then exact
batch repair, real operator commands and final Mac/Neuroses qualification remain.
No B1 closure or downstream pilot start yet.

## Guards against certification loops

Operator requirement2026-10-06: prevent repeated qualification/review from
consuming a campaign without delivered work. This applies Blackbird's one direct
mechanism per claim/fault class; it is a campaign behavior requirement, not a new
certifier or approval gate. Scheduler enforcement remains implementation work.

- Before a check, name the concrete failure it can expose and how its result
  changes the next action. A check whose only purpose is certifying another
  check of the same kind is omitted. Replace an inadequate oracle rather than
  adding recursive validation.
- Once required affected checks pass and consequential findings are resolved,
  finish/activate the unit and proceed. Reopen qualification only for relevant
  source/input/environment changes, new contrary evidence or a previously
  untested consequential fault class. Reviewer wording preferences or requests
  for broader confidence do not reopen it.
- Review is occasional and scoped. Inspect findings; reproduce relevant faults
  directly, fix valid ones, and explain rejected ones. Do not seek reviewer
  consensus, unanimous approval, or reviews of reviews. A follow-up may address
  the concrete fix; unrelated questions become separately prioritized work.
- Establish configurable time/resource/attempt bounds for qualification and
  review before the unit, not retrospectively after an unproductive run.
  Timeouts permit a bounded retry or different approach within that allowance.
  Failed reviewer access does not certify success or block unrelated progress.
- Repeated no-change checks, new reviewers for the same settled question, or
  repeated inconclusive attempts trigger a visible stop to that approach.
  State the missing evidence, change the mechanism/scope, or defer the unit and
  continue independent productive work. Do not silently reset the allowance.
- Real new defects remain real: never force activation merely because the budget
  expired. Keep the candidate inactive or revert, retain evidence and resume
  useful work on another unit. A recurring defect can warrant a revised plan
  rather than indefinitely extending the current qualification run.

Use existing attempt/audit records to show delivered work, qualification time,
repeated attempts and why a settled check was reopened. No model judge, scoring
service or mandatory justification essay on every tool call is required. These
controls must preserve required correctness checks and model planning agency.

## Interstitial candidate management — queued

Bead arconaut-uaa.14.1, under the post-compaction/pre-giga pilot; waits for B1.
Git branches/commits retain candidate ancestry, with an experiment record linking
the effective source/Lua/context/configuration, hypothesis, attempts, outcomes
and build artifacts. Candidates do not each require a worktree. Serial runs
reuse an isolated checkout after recording current work. Concurrent runs lease
a worktree from a configurable bounded pool; excess requests queue. Never switch
a checkout while programs/builds use it. Preserve unrelated working changes.

Retire checkouts without deleting recorded source/results or required artifacts.
Archive independently of promotion/activation; a source commit is not evidence
of a built, tested or running candidate. Activate through Lua reload or native
RRC, retaining conversation and audit continuity. Include qualification/retry
bounds above. This queues the manager, not a new worktree or Git publication.

B1 qualification now Mac+Neuroses debug/release/ASan11/11 each and sevenTU tidy
clean, unchanged product source snapshot. Actual8process operator cases pass.
Live bounded history16MiB aggregate-ceiling fault red/fix/green/Kimi resolved; exact
live archive batch restore staged. Last native RRC verifies repair+new range reader,
then finalize paper and close ONLY .9, dependent pilot not yet started.

**B1 .9 complete (2026-10-06):** latest native RRC verified exact live group repair
and seven pinned originals; actual104245469byte history bounded7byte read and
rejection-only revision conflict pass. Final actual report
[papers/2026-10-06-compaction-campaign.md](../papers/2026-10-06-compaction-campaign.md).
All final scoped qualification and serious campaign complete. Close only .9,
broader acceptance/B2 still open; useful-work pilot is the recommended next unit,
not begun by this campaign. Earlier pending checkpoints above are historical.

## Checkpoint commits and branch pushes — authorized

Operator2026-10-06 requires commits with pushes, even for branches not on main.
Preserve completed units and meaningful candidate checkpoints as source commits
with linked experiment evidence; push work/candidate branches without waiting
for promotion or integration. A pushed branch is archived work, not certification
or activation. No force-push, implicit main merge, credential/audit-secret upload
or ignored context/quarantine/build publication. Bounded worktree management
and checkout quiescence remain unchanged. Missing remotes need a concrete
destination; failed pushes remain visible and local history retained.

## Interstitial launched — 2026-10-06

Arconaut-uaa.14 is in_progress, fresh context/evolution-pilot/session using
gpt-6.1-sol medium. Audit/rageshake, programmable diagnosis/measurement, then
branch-based candidates with bounded worktree leases (.14.1) and bounded
comparison/transfer work. Earlier B1-incomplete checkpoints above are historical;
B1 is closed. Read current live beads and final compaction report. Use and update
the actual per-unit plan/status as work proceeds; no giga launch implied.

Interstitial unit1 source checkpoint: bounded audit_inspect + advisory rageshake
implemented (papers/2026-10-06-useful-work-observability.md). Direct binary/reopen,
timeout unknown lineage, failed delivery local retention and successful fake-bd
privacy oracles green. Completed narrow Kimi wrapper/child0 findings integrated;
final changed-source Mac+Neuroses debug/release/ASan audit+coding2/2 each. Native
quiet activation/live tool and bead verification next, not implied by source commit.
.14 remains in_progress; .14.1 and Lua diagnosis/comparison/transfer remain work.

## Operator correction: two hardening layers, never three

The2026-10-07 instruction supersedes open-ended tribunal/recheck language above.
[BOUNDED_HARDENING](BOUNDED_HARDENING.md) defines layer1 remediation/fix and
layer2 one recheck/fix, named direct oracles and an upfront allowance. No third
assurance layer, repeated full-suite recertification or silent budget reset.
Backstop/old-history ergonomics do not expand current capacity promotion scope.

## Giga is running — current operator sequence

Milestone one .3 and interstitial .14 are CLOSED/accepted; .14.3 scoped integrated
capacity stop is CLOSED after two-layer fixes/checks and actual native activation
inside25minutes. Earlier “not ready” and open-compaction/pilot status passages are
historical. Current execution is [GIGA_CAMPAIGN](GIGA_CAMPAIGN.md), epic
arconaut-7iy: recovery, programmability, heterogeneous colleagues first; then
orchestration, station, optional integration/feed and two encrypted independent peers.
Accepted units advance master promptly; no global certification gate.
