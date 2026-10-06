# Arco evolution campaign: what to use next

2026-10-06. Operator discussion and execution-order input. Three frumentarii acquired
and studied primary papers, original implementation and test source. Their reports
cover empirical methods, evolving harnesses and orchestration. No reference programs
or tests were run, reported benchmark gains reproduced, dependencies adopted, or
product changes made by the research lanes. Source inspection supports mechanism
claims; published performance remains the authors' evidence.

Our next move should be a small useful-work evolution pilot after managed compaction
and before giga. Let it improve Arco's audit/complaint and workflow surfaces while
learning which changes help later development. Do not build a general evolutionary
platform first. The operator approved this ordering; integrations and HUD discussion
follows settling the scope, not another full implementation gate.

## Mechanisms and Arco fit

| Sources / approach | Actual mechanism worth studying | What to use in Arco | Limitation that changes the design |
| --- | --- | --- | --- |
| autoresearch | Baseline, bounded edit/run/keep, retain failed trials | Small Lua/context or C++ performance experiments | Changing candidate can falsify its metric; include compile/RRC overhead |
| DGM, FunSearch | Executable archive, parent diversity, stepping stones / behavioral signatures | Retain candidates independently of activation; outcomes as a vector | Selection routines contain source-level traps; paper is not oracle |
| SICA | Improving agent reads history and edits a copy; code ancestry differs from newest notes | Audit explorer exposing exact old attempts and programs | Small benchmark subsets; ranking actually used need not match described utility |
| ShinkaEvolve, HGM | Allocate trials to parent/model, novelty and later descendant productivity | Measure costs and future improvement ability before adaptive allocation | Lucky early outcomes and mutable statistics can corrupt selection |
| AlphaEvolve | Cheap-to-expensive evaluation cascade followed by actual deployment | Compile/direct changed-behavior check, then meaningful useful work | Passing a smoke check is not product qualification; implementation not released here |
| ModularRSI | Contrast success/failure under same task and executable bundle; localized edits and integration | Diagnose recurrent mechanism; preserve complete source/helper/context identity | A task ID is insufficient; combined modules need an interaction exercise |
| CLM | Editable local context, growth/repair, evolved editing skills and paired outcomes | Ordinary Lua context policy with original recovery | Structural fit/shrink gate does not establish semantic recall |
| Self-Harness, latest multitask harness paper | Candidate lifecycle, reusable notes, trace-driven changes | Durable experiment notes and actual effective program identity | Repeatedly used heldout becomes selection data; newest multitask source remains a lead |
| HELIX | Executable component recomposition with actual dependency bindings | Useful Lua composition points when real workflows need them | Portfolio oracle coverage is not an executable selection policy; fixture omissions documented |
| STOP | Evolve the program that allocates/proposes improvements | Autodroit campaign itself remains programmable by the model | Score downstream delivered work, not existence of a new campaign program |
| Exo, Ouroboros | Durable replacement outcomes, reboot/pause/continuation and campaign state | Model-readable build/activation results and actual quiet RRC | Delay is not quiescence; smoke literals are not proof of self-evolution |
| TTHE, Red Queen GM | Online proxy search / evolving reviewers against independent anchors | Separate proposal failure from bad selection; revision changed judges | Source unavailable in this pass; proxy/executable health is not correctness |
| AdaMAST | Failure-triggered advisory diagnosis and evidence-linked records | Model rageshake with observations, suspected cause and exact audit references | Reflection format and agreement do not certify correctness; avoid blocking rituals |
| Agent Mail, Station | Durable acceptance, idempotent transport, distinct committed/provisional work | Peer steering and future feed admission with explicit dispositions | Receipt does not imply model-request inclusion/action; unknown shell effects cannot replay |
| Inspect AI | Exact reconstruction, scoped caches, linear-work and divergent-lineage tests | Audit explorer first; optimize measured replay/storage without summarizing originals | Distinct occurrences remain distinct causal events; selective logging defaults do not fit capture-all |
| METR, Rethinking Evaluation | Real work/review effort, matched-feedback attempts and fresh outcomes | Separate useful output, operator capacity, latency and all-provider resources | Additional attempts, task selection, hardware changes and evaluator reuse confound improvement |
| SEAL | Generated self-edits rewarded after actual weight adaptation | External adaptation capability only if later wanted | Hosted subscription harness edits do not give weight-training capability |

The [empirical report](empirical/report.md), [harness report](harness/report.md) and
[orchestration report](orchestration/report.md) contain primary links, actual inspected
functions/tests, source pins, negative results, proposed oracles and reading limits.

## What the literature changes

The most useful counterweight is yesterday's revision of
[Rethinking Evaluation](https://arxiv.org/abs/2607.12227v4). Its coding experiments
find fixed-harness extra attempts competitive or better than harness evolution;
mean heldout performance stays flat. That is not a universal rejection: long-horizon
games show better results. It makes an equally funded retry baseline essential.

[ModularRSI](https://arxiv.org/abs/2609.14857v1) offers a practical diagnosis pattern:
compare contrasting traces, localize recurring causes, change a coherent program,
and exercise the combination. Its actual K-roll implementation captures the first
complete selected bundle and reuses it, instead of treating changing composition
as stochastic evidence about one harness. Root also inspected the exact bundle
and execution-gated review fixtures. These are useful counterexamples, not code
to import or a new requirement for every reviewer to emit a particular tag.

[METR's later study-design update](https://metr.org/blog/2026-02-24-uplift-update/)
shows why perceived speed and task timers become unreliable with selection and
concurrent agents. For this operator, reclaimed attention and work now possible
are valid outcomes even when a task takes longer. Report those separately.

This is how Blackbird should become recognizable: actual relevant sources affect
the mechanism/oracle; a discriminating test rejects a tempting shallow shortcut;
the agent changes strategy after failure, accepts valid criticism, repairs context
from originals and finishes useful work. Reading counts, rigor declarations and
mandatory prose checkpoints cannot substitute for these behaviors.

## Slated pilot: after B1, before giga

Start with three connected deliverables, sized as whole conceptual units:

1. **Audit explorer and evidence-linked complaint.** Bounded inspection of actual
   request/context/program/attempt/output; local rageshake captures state references
   and creates a complaint without requiring immediate repair. The DB sink remains
   a consumer interface, not an Arco-governed service. Decide integrations/HUD before
   choosing a new dependency or external sink.
2. **Contrastive diagnosis as a Lua workflow.** While doing real Arco work, inspect
   useful successful and failed traces, name a mechanism and falsifying observation,
   and optionally consult a separately contextualized colleague. Recurring Kimi
   failure switches to configured MiMo or ChatGPT; it does not strand work. Keep
   actual completed/failed review outcomes, valid and false findings.
3. **A bounded comparison that leaves useful improvements.** Compare the preceding
   harness with extra attempts, an evolving Lua/context policy and, when the change
   calls for it, a native RRC candidate. Match starting task/source/settings and
   feedback opportunity; record actual aggregate budgets. Use fresh task variants
   and a subsequent useful change to assess transfer. A small pilot establishes
   feasibility and counterexamples, not statistical recursive acceleration.

Establish audit/complaint instrumentation as a common capability before comparison;
all arms get the same inspection tools and relevant source access. Otherwise the
experiment confounds the policy with better observability. Declare the edit surface
and colleague policy before task assignment. Native refit and provider fallback
remain recorded treatments: selecting those only after seeing outcomes cannot
establish their causal advantage. Adaptive choices remain useful operationally;
report them as adaptive work, or evaluate that whole policy on fresh assignments.

Direct outcomes: independently specified finished behavior; exact source-derived
constraint recall after compaction; retained partial-output recovery after timeout;
once-only externally counted effects across RRC; actual selected executable/program
and provider-request identity. Use source identity to make comparison interpretable,
not to restrict ordinary configurability or impose operator command approvals.

Keep accepted work, elapsed latency, operator active/review/rescue time, available
usage from every participant, build/test resources, failures/reverts and remaining
unknowns separate. Missing usage is unavailable, not zero. Selection is a model
policy with operator steer; an archived candidate need not be activated. A failed
fresh final case can become development evidence, but ceases to be unseen thereafter.

A one-off correctness fix can be kept because it fixes independently witnessed
behavior. It need not wait for an uplift experiment. Conversely, shipping a useful
feature does not by itself establish that the process producing it accelerated.

## Certification-loop constraint

The operator requires guards against certification doom loops. Scope each
check/review to a consequential fault and a decision; reuse passed qualification
unless relevant changes or contrary evidence justify reopening it. No reviews
of reviews, unanimous reviewer approval or recursive evidence layers. Configure
qualification/retry budgets up front; repeated no-change or inconclusive checks
end that approach, prompting a different direct mechanism or separately tracked
work. Budget exhaustion does not permit activation of a known defective candidate.
Finish qualified units and proceed; don't grow their acceptance scope indefinitely.
Use ordinary audit records, not another certification subsystem. Detailed campaign
requirements are in docs/AUTODEV_QUEUE.md; enforcement is not implemented here.

## Current product evidence and remaining discussion

B1 managed compaction is still in progress. Core, mixed native and scale oracles
and scoped reviews pass; the first live summary has published. Its RRC hit busy
because a pending restart was not consumed by a manual ordinary --once launch.
Root resumed through explicit --resume-continue/--resume-once, preserving history;
actual live continuation now verifies retained literals and deliberate omission.
Bounded-original repair, useful C++ work, distinct selection/archive and final
portable qualification remain. No campaign-complete claim.

The real retained campaign has also exposed expensive replay, reinforcing the
utility of the audit explorer/performance work. A sampled actual resume process
was executing ContextStore reconstruction, SessionStore resume and retained-state
rebuild. That is an observation, not a measured causal speedup or license to drop
originals. Compiler/provider/inference/host time must remain distinguishable.

Before giga, discuss integrations and HUD: which ordinary programs/services/models
are the first consumers, how a colleague is invoked, and which current/pending
context/program/campaign/attempt facts the operator needs at a glance. A status
must distinguish queued, running, unknown, failed and activated. We have not
selected a UI framework, DB, provider adapter or optimizer dependency through this
survey.

## Acquisitions and open leads

The lanes acquired 20 pinned source archives representing 18 distinct repository
revisions; DGM and ModularRSI were independently studied in two lanes. Three
manifests retain source URLs/pins/hashes/archives/restoration and reading limits:
[empirical manifest](empirical/MANIFEST.md), [harness manifest](harness/manifest.json),
[orchestration manifest](orchestration/manifest.json). Existing autoresearch, CLM,
Self-Harness, GPTMe, Python mail and Station material was reused. Original archives
remain intact; extracted instructions have no present authority.

Primary PDFs and HTML snapshots are acquired locally. Paper-only leads include
TTHE, Red Queen GM, the latest multitask harness, complete compiler-team orchestration
and the scaling-study runner. HELIX's 101 unsafe absolute fixture symlinks were
omitted from extraction and preserved in the source archive; reproduction limits
are explicit. No operator-paid acquisition blocks this design discussion.
