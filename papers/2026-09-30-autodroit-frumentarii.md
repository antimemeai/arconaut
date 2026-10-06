# Autodroit: evidence-driven improvement of the harness

2026-09-30. Parent reconnaissance alongside the standing-state, programmable
workflow, and systems-runtime frumentarii. Scope: mechanisms for operator/model
experiments on Arconaut itself during work. This is research evidence and proposed
questions, not an adopted improvement protocol or a product implementation.

Read current operator intent and Blackbird. Browsed primary papers and inspected
the pinned source below. Downloaded PDFs and source archives were not executed;
no model, training, benchmark, or mutation run was performed. Direct archive
comparisons establish retained reference bytes, not behavioral correctness.

## 1. Autoresearch: a deliberately legible experiment loop

Karpathy's reference separates editable training code from fixed preparation
and evaluation, uses a time-bounded run, and compares a validation metric. The
operator edits the research instructions. This supplies a comprehensible split
between experiment purpose, intervention, execution, and evidence.
[README](https://github.com/karpathy/autoresearch/blob/228791fb499afffb54b46200aca536f79142f117/README.md).

Observed `program.md` specifies a baseline run, candidate commits, a TSV of
keep/discard/crash outcomes, and rollback of unsuccessful edits. It is an
instruction-driven loop, not a runtime providing audit completeness, hot policy
replacement, or an independent result verifier. The experiment log is deliberately
untracked. Its training/evaluation setup remains specific to that task.
[Experiment instructions](https://github.com/karpathy/autoresearch/blob/228791fb499afffb54b46200aca536f79142f117/program.md).

Inference: this legibility is useful for autodroit. Arconaut needs its own
meaningful outcomes for orchestration, ergonomics, collaboration, and continuity.
A smaller score or a longer uninterrupted run cannot answer all of those questions.
The source's branch/reset instructions are historical reference; they do not
authorize resetting this repository or importing its approval conventions.

## 2. GEPA: programmable candidates, feedback, and interchangeable search

Observed current GEPA exposes an evaluator separately from the object being
optimized. Candidates may be text or named text components; dataset examples
remain opaque to the API. The evaluator can return a score and diagnostic data.
This is an interface for optimizing real code and policies as well as prompts.
[Public API](https://github.com/gepa-ai/gepa/blob/3f160c295000dd31db3d438c3d17553c23cc5f81/src/gepa/optimize_anything.py),
[task representation](https://github.com/gepa-ai/gepa/blob/3f160c295000dd31db3d438c3d17553c23cc5f81/src/gepa/oa/task.py).

Observed the evaluation service exposes both direct Python calls and HTTP
operations over the same evaluator/budget. It assigns example identities,
records per-evaluation results and summaries, and combines train/validation
examples in the agent-visible pool. Optional test examples are excluded from
that pool. Evaluator-supplied diagnostics and cost populate the record; this
does not itself capture everything the evaluated program did.
[Evaluation service](https://github.com/gepa-ai/gepa/blob/3f160c295000dd31db3d438c3d17553c23cc5f81/src/gepa/oa/eval_server.py).

Observed configuration permits different engines, including GEPA, autoresearch,
meta-harness, and best-of-N. Evaluation and proposer budgets are different;
temporary engine workspaces can disappear unless explicitly persisted.
Engine interchangeability has real limits: named multi-component candidates
are accepted only by the GEPA engine, while other engines reject them. These
contracts help distinguish composability from a list of interchangeable names.
[Configuration](https://github.com/gepa-ai/gepa/blob/3f160c295000dd31db3d438c3d17553c23cc5f81/src/gepa/oa/config.py),
[candidate conversion](https://github.com/gepa-ai/gepa/blob/3f160c295000dd31db3d438c3d17553c23cc5f81/src/gepa/oa/task.py).

The original paper studies reflective prompt evolution using execution feedback
and Pareto selection. Its evidence does not establish current engine equivalence,
live runtime mutation, or continuity after replacing a compiled harness.
[Paper, v2](https://arxiv.org/abs/2507.19457v2). The acquired PDF was read for the
optimization/evaluation separation and generalization limits; its quantitative
results are not used as forecasts for Arconaut.

Inference: a human and model can share a programmatic experiment surface without
all orchestration being forced into one search algorithm. Preserve the candidate,
effective runtime, task, evaluator, observations, and resource cost together.
If changing the candidate also changes the judge, the experiment has changed
meaning; that needs explicit treatment rather than an unexplained score comparison.

## 3. Self-Harness: proposed code edits tied to observed failure mechanisms

The paper studies fixed models improving their surrounding harness from task
traces. It separates failure analysis, bounded candidate proposals, and evaluation.
Its proposer receives evidence from one task split; a second split participates
in promotion without being supplied as proposal evidence. These are benchmark
experiments, not demonstrations of preserving a currently inhabited process
through recompilation. Repeated selection against the second split also makes
it a selection surface rather than a pristine final test of generalization.
[Paper, v3](https://arxiv.org/abs/2606.09498v3).

Observed the public acceptance code requires comparable repeat identifiers and
denominators, with improvement on at least one split and no drop on the others.
That check alone does not establish identical case identities or sound graders.
The workflow also reevaluates merged candidates instead of assuming separately
accepted changes combine successfully. Both are concrete experiment mechanisms;
neither is proof that improvements generalize or that runtime continuity holds.
[Acceptance](https://github.com/qzzqzzb/Self-Harness/blob/2720dbb3f52283684f4b85a1065d642df1779dd8/acceptance/scripts/run_acceptance_gate.py),
[merge evaluation](https://github.com/qzzqzzb/Self-Harness/blob/2720dbb3f52283684f4b85a1065d642df1779dd8/workflow/scripts/run_self_harness_loop.py).

Inference: diagnosis should identify the mechanism behind a failure and name the
surface being changed. Independent outcomes matter more than a model's praise
of its revision. Human participation can improve the hypothesis, interpretation,
and experimental design; it need not become command-by-command authorization.

## Questions for the integrated research design

1. What can an autodroit experiment change during real work: turn policy, workflow,
   context construction, compaction, executable skills, providers, audit code,
   or the native owner itself? Which change needs migration or replacement?
2. What does the model inhabit before and after the intervention: the same live
   process, a migrated state, a resumed computation, or a new process with restored
   context? Record those as different treatments.
3. Which outcome directly answers the proposed improvement? Examples include
   successful repair after a tool error, responsive peer communication, complete
   context lineage, and preserved pending work across controller replacement.
4. Can both operator and model inspect the candidate, effective loaded definitions,
   audit, and experiment outcome through the same useful programmatic operations?
5. How do rejected or reversed edits affect running jobs and later context? Undoing
   source code alone cannot undo an external action already performed.
6. How does experimental evidence remain interpretable when the harness itself
   records and evaluates its behavior? Changes to measurement are changes to the
   experiment, with their own lineage and comparability question.

These questions call for direct behavioral oracles and appropriate comparative
experiments. Blackbird's single-layer rule applies; an additional receipt system
whose only purpose is to certify the first would not solve the underlying problem.
Mutation-based challenges belong on the fleet. No experiment has been run here.

## Acquired material and limits

The exact source archive catalog is
[2026-09-30-reference-acquisition.json](2026-09-30-reference-acquisition.json).
PDF identities, hashes, and local ignored paths are in
[2026-09-30-literature-acquisition.json](2026-09-30-literature-acquisition.json).
The other frumentarii reports examine Prime/Continual Harness research, persistent
execution, audit data, and runtime evolution. This pass establishes mechanisms
worth studying and their known limits; it does not measure Arconaut performance,
choose an evaluator, or accept a self-modification architecture.
