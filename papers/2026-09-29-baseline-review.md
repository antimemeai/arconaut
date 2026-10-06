# Adversarial review of the restart baseline

2026-09-29. Scope: `docs/RESTART.md`, README, AGENTS, and QUARANTINE,
checked against Blackbird and the independent architecture assessment. Build
claims were checked against the oracle report; they were not independently rerun.
This reviews the assessment's synthesis and proposed research direction, not an
unwritten product specification. No product code or instruction file was changed
by this reviewer.

The factual synthesis is sound: it does not claim the old harness works, mistake
pilot/reactor for the CLI runtime, bless a provider/editor/storage choice, or
confuse compilation with behavioral conformance. Candidate reuse remains
conditional. Source restoration preserves the original archives/checkouts and
names exact selected reference revisions. The following corrections would make
the research direction more precise.

## Findings

**R1 — Effect outcome must be allowed to remain unknown after interruption.**
`docs/RESTART.md:89-90` proposes that every executed effect has its request,
result, and error state, and model retry does not silently duplicate or erase an
effect. The intent is appropriate, but the first sentence can be read as requiring
known outcomes for effects the harness cannot atomically observe. Consider a
shell command that changes a remote resource, followed by a process crash before
its result is stored. A request log alone cannot determine whether retry is safe.
The research report already explicitly distinguishes unknown external effects
from completed ones; the synthesis should preserve that boundary.

Suggested disposition: amend the invariant and recovery experiment to require
attributable requests plus explicit completed/failed/cancelled/unknown outcomes
according to the eventual contract, with no blind replay of uncertain effects.
Do not silently promise exactly-once execution for arbitrary shell or remote
operations. This is a specification question, not a reason to add a transactional
storage stack before research.

**R2 — Two research rows combine independent conceptual units and lose their
deciding oracles.** `docs/RESTART.md:125` combines editor-interface selection,
anchor validity, context reduction, and hosted caching. Paired edit cases can
choose an editing interface, but do not alone establish a long-session context
policy's instruction retention or total cache-adjusted cost. The next row
(`:126`) combines behavioral intervention, session recovery, retrieval, embeddings,
and predictors; labeled stalled traces cannot decide storage or retrieval quality.
This conflicts with Blackbird's instruction to delegate whole conceptual units,
not unrelated leftovers. The independent research report has already separated
these questions more usefully.

Suggested disposition: keep editing/reference validity separate from context
selection/reduction; keep intervention discrimination separate from memory
retrieval. Leave durable recovery with the existing conversation/effects/recovery
study. Treat prefetch as conditional on a measured latency problem. Each resulting
study should have a single decision and oracle; this does not require launching
all of them or constructing a larger program before operator priorities are known.

**R3 — Trace replay cannot establish the benefit of an intervention.**
`docs/RESTART.md:126` says to replay labeled tasks and check useful intervention
against false interruptions. Replay can establish when a detector would trigger,
including false triggers on valid polling/retries. Once an intervention changes
context or stops work, future model actions are no longer the recorded trajectory;
the same replay cannot establish that the intervention improves completion or
recovery. The research report states this limit, but the condensed plan omits it.

Suggested disposition: describe replay as an observation-only discrimination
screen. If the screen and operator needs justify an intervention, evaluate its
effect separately on comparable tasks with a declared model/workload budget.
Keep false-trigger accuracy distinct from task benefit; avoid selecting a rule
because it merely classifies the historical trace labels well.

**R4 — “Historical authority only” is an ambiguous disposition name.**
`docs/RESTART.md:68` gives that label to June plans, old instructions, CI, and
unsupported claims. Its explanatory text correctly denies them roadmap authority,
as do current AGENTS and README. The label nevertheless appears to preserve a
kind of authority that the column is meant to remove.

Suggested disposition: use “Historical evidence only.” This is a small clarity
correction, not a finding that the actual current instructions are contradictory.

## Review bounds

No additional factual correction is requested to the architecture summary or
retention table. In particular, it is correct to preserve implementation while
retiring its authority: physical deletion is not necessary to establish a fresh
baseline. README and AGENTS clearly establish current versus historical status;
QUARANTINE distinguishes portable source history from the later assessment
documents and gives concrete restoration commands. Operator workflow, supported
providers, UI, and fleet scope remain open without being replaced by inherited
labels. The design-before-plan-before-product-code ordering is consistent with
Blackbird.

Parent synthesis owns disposition of these findings. This review does not approve
an implementation plan or establish product readiness.

## Owner disposition — 2026-09-30

All four findings were accepted and integrated into `docs/RESTART.md`:

- R1: the proposed effect invariant now explicitly distinguishes unknown outcomes
  after a crash and requires a retry/reconciliation policy that respects them.
- R2: editing, context reduction, behavioral assistance, and retrieval memory now
  have separate study questions and deciding oracles; durable recovery remains
  with conversation/effects. Optional studies remain conditional on actual need.
- R3: trace replay is limited to detector discrimination; intervention benefit
  requires comparable task executions because subsequent actions change.
- R4: the disposition label is now “Historical evidence only.”

The reports and baseline remain an assessment and research direction. A renewed
product specification and its implementation plan still require their own review.
