# gepa

Named-component candidate evolution, reflective feedback and independent test scoring are useful autodroit mechanisms; selection authority and evaluator trust must stay explicit.

Role: reflective program optimization framework. Runtime: Python.

Pinned source: [https://github.com/gepa-ai/gepa](https://github.com/gepa-ai/gepa); revision/version `3f160c295000dd31db3d438c3d17553c23cc5f81`.

In-process candidate engine with adapters, selection/evaluation callbacks and saved state; alternate optimize-anything engine can supervise Claude Code subprocesses.

Owns candidate selection/budget/checkpoint, consumes user evaluator and model/agent implementation.

Inspection: Read adapter/evaluation protocol, acceptance criteria, actual batch selection/dedup/rejection path, main checkpoint loop, state save, held-out scoring and autoresearch subprocess watchdog.

Limits of this study: Not all adapters/engine variants inspected; no model/evaluator invocation, benchmark reproduction or complete filesystem isolation audit.

## Actions

### GEPAAdapter.evaluate / make_reflective_dataset / propose_new_texts

Surface: programming API.

Input: Named component candidate and dataset

Result: Scores/outputs/optional trajectories and proposed text components

Lifecycle: Caller-supplied evaluation and reflection lifecycle.

Authority: Adapter/model and evaluator authority

Evidence: [adapter](#evidence-adapter).

### GEPAEngine.run / selection_strategy.select

Surface: optimizer API.

Input: Seed/validation data, strategy programs and stop callbacks

Result: Candidate pool/Pareto and rejection/acceptance events

Lifecycle: Checkpointed iterations; custom selection may permit exploration.

Authority: Caller/strategy controls optimizer

Evidence: [selection](#evidence-selection), [loop](#evidence-loop).

### optimize_anything

Surface: optimization API.

Input: Seed, evaluator, objective, engine/budgets and held-out test set

Result: Best candidate/score, pool/metadata and independent test scores

Lifecycle: Engine-specific; held-out eval outside budget.

Authority: Caller evaluator and external-agent posture

Evidence: [testsplit](#evidence-testsplit), [budget](#evidence-budget), [subprocess](#evidence-subprocess).

## Capabilities

### filesystem

**S — Files** (source): Framework writes candidate/checkpoint records and external-agent engine materializes workspace; not a universal model file-tool interface.

Evidence: [save](#evidence-save), [subprocess](#evidence-subprocess).

### processes

**S — OS programs** (source): Alternate autoresearch engine supervises actual Claude subprocess with session ID, budget and watchdog; inherited agent tool execution not claimed as GEPA implementation.

Evidence: [subprocess](#evidence-subprocess).

### code-actions

**S — Code actions** (source): Candidate text can be executable code through user adapter/evaluator; generic engine does not itself provide a persistent kernel.

Evidence: [adapter](#evidence-adapter).

### persistent-kernel

**— — Kernel** (role): persistent_kernel: this reference supplies reflective program optimization framework; an agent policy, model interface and context lifecycle are outside its supplied role.

### standing-database

**? — Standing DB** (inspection scope): Structured candidate state and eval ledger exist, but no independent standing query service for agent work was established in inspected engine/state/budget paths.

### workflow-programming

**S — Workflows** (source): Adapters, proposer, acceptance and selection strategies are programmable runtime interfaces; custom selection may override acceptance.

Evidence: [adapter](#evidence-adapter), [selection](#evidence-selection).

### multi-model

**S — Models** (source): Proposer/evaluator models are separately supplied/configured; not an independent live collaboration bus.

Evidence: [adapter](#evidence-adapter), [subprocess](#evidence-subprocess).

### live-collaboration

**— — Peer chat** (role): live_collaboration: this reference supplies reflective program optimization framework; an agent policy, model interface and context lifecycle are outside its supplied role.

### concurrent-work

**S — Concurrency** (source): Batch proposal/evaluation interfaces support grouped work and an independent external process; concurrency scheduling is adapter/engine-specific.

Evidence: [adapter](#evidence-adapter), [budget](#evidence-budget).

### steering-interrupt

**L — Steer/interrupt** (source): Subprocess watchdog can terminate wedged/budget-exhausted external agent; no general operator steering to busy candidate/model work in inspected paths.

Evidence: [subprocess](#evidence-subprocess).

### turn-redefinition

**S — Turn program** (source): Caller can replace selection/proposal/evaluation programs; this is optimizer policy, not model hot-redefinition of its live conversation turn.

Evidence: [selection](#evidence-selection).

### compaction

**— — Compaction** (role): compaction: this reference supplies reflective program optimization framework; an agent policy, model interface and context lifecycle are outside its supplied role.

### context-repair

**— — Repair** (role): context_repair: this reference supplies reflective program optimization framework; an agent policy, model interface and context lifecycle are outside its supplied role.

### original-audit

**L — Original audit** (source): Optional trajectories and candidate/log callbacks depend on adapter capture; score-only evaluations and caller-defined traces cannot guarantee all original IO.

Evidence: [adapter](#evidence-adapter), [selection](#evidence-selection).

### audit-query

**S — Audit query** (source): Candidate state, iteration records and callbacks are program accessible; not all original provider/OS IO.

Evidence: [selection](#evidence-selection), [save](#evidence-save).

### hot-change

**S — Hot change** (source): Strategies/adapters/candidates are caller-programmable, but adopted candidate is a future evaluation artifact, not after-turn hot replacement of current harness.

Evidence: [selection](#evidence-selection), [adapter](#evidence-adapter).

### rebuild-continuity

**L — Rebuild continuity** (source): Pickled candidate state excludes runtime hooks and external agent uses transcript session resume; no executable refit/context/ongoing-work transfer.

Evidence: [save](#evidence-save), [subprocess](#evidence-subprocess).

### remote-services

**— — Remote** (role): remote_services: this reference supplies reflective program optimization framework; an agent policy, model interface and context lifecycle are outside its supplied role.

### self-improvement

**S — Self-improve** (source): Executable reflect/propose/evaluate/select loop can optimize prompts/programs/harness components through supplied adapters; no automatic safe activation in current harness.

Evidence: [adapter](#evidence-adapter), [selection](#evidence-selection), [loop](#evidence-loop).

### complaints

**— — Complaints** (role): complaints: this reference supplies reflective program optimization framework; an agent policy, model interface and context lifecycle are outside its supplied role.

### authority

**S — Authority** (source): Strategy owns selection, evaluator owns score semantics, engine owns budget/watchdog and supplied subprocess permission posture.

Evidence: [selection](#evidence-selection), [budget](#evidence-budget), [subprocess](#evidence-subprocess).

### evaluation

**S — Evaluation** (source): Independent evaluator supplies scores, subsample selection then validation/test split; exceptions score zero, objective validity remains caller responsibility.

Evidence: [adapter](#evidence-adapter), [testsplit](#evidence-testsplit).

### time-order

**S — Time/order** (source): Eval ledger has wall timestamps; external-agent watchdog uses monotonic elapsed time, without durable paused-time policy.

Evidence: [budget](#evidence-budget), [subprocess](#evidence-subprocess).

## Inspected test oracles

- [quarantine/gepa/tests/test_acceptance_criterion.py](../../../quarantine/gepa/tests/test_acceptance_criterion.py): Comparison rule for scored proposals. Oracle: Known vectors distinguish strict improvement, equality, regression and 0.0001 gain; tests do not validate the external evaluator or generalization. Read, **not executed**.
- [quarantine/gepa/tests/test_parallel_proposals.py](../../../quarantine/gepa/tests/test_parallel_proposals.py): Batch sampling parent/minibatch relation. Oracle: Mock selectors/samplers verify same-parent shape and differing minibatches; not actual parallel provider/task execution. Read, **not executed**.

## Useful mechanisms

- Selection, acceptance, evaluator, reflection and budgets have distinct programmable owners.
- Unselected proposals receive explicit reasons; optional held-out scoring stays outside optimization input.

## Material limits

- Custom selection can override acceptance; a strict criterion is not an inviolable gate by itself.
- Optimizing score may exploit an invalid evaluator; saved candidate records are not original all-IO audit.

## Arconaut design questions

- Which experiment governance decisions belong to the model, and which independent machine invariants cannot be overridden?
- How does self-improvement keep selected/rejected/failed candidates observable without conflating metrics with correctness?

## Evidence

### Evidence adapter

[quarantine/gepa/src/gepa/core/adapter.py:16–120](../../../quarantine/gepa/src/gepa/core/adapter.py#L16): Adapter supplies candidate execution, scores, optional trajectories and proposer component edits; evaluator outputs are opaque to engine.

### Evidence accept

[quarantine/gepa/src/gepa/strategies/acceptance.py:12–66](../../../quarantine/gepa/src/gepa/strategies/acceptance.py#L12): Built-in acceptance compares summed subsample scores, with strict or lateral-move criterion.

### Evidence selection

[quarantine/gepa/src/gepa/core/engine.py:566–646](../../../quarantine/gepa/src/gepa/core/engine.py#L566): Selection strategy is authority and may surface criterion-rejected proposals; selected input identities/dedup and truthful rejection callbacks enforced.

### Evidence loop

[quarantine/gepa/src/gepa/core/engine.py:964–1007](../../../quarantine/gepa/src/gepa/core/engine.py#L964): Engine saves adapter/candidate state before iteration and stops according to callback policy.

### Evidence save

[quarantine/gepa/src/gepa/core/state.py:405–441](../../../quarantine/gepa/src/gepa/core/state.py#L405): Checkpoint pickles state excluding runtime hooks, optionally clouds callable data; image/live process continuity not promised.

### Evidence budget

[quarantine/gepa/src/gepa/oa/budget.py:1–87](../../../quarantine/gepa/src/gepa/oa/budget.py#L1): Eval call ledger is thread-safe; proposer dollars have separate owner/cap, not same budget.

### Evidence subprocess

[quarantine/gepa/src/gepa/oa/engines/autoresearch.py:641–690](../../../quarantine/gepa/src/gepa/oa/engines/autoresearch.py#L641): Claude subprocess pins session/resume, remaining USD cap and watchdog on monotonic no-eval time; permissions depend on sandbox mode.

### Evidence testsplit

[quarantine/gepa/src/gepa/optimize_anything.py:402–429](../../../quarantine/gepa/src/gepa/optimize_anything.py#L402): Held-out tests evaluated directly outside engine budget/server; exceptions map to zero scores.

### Evidence test

[quarantine/gepa/tests/test_acceptance_criterion.py:44–87](../../../quarantine/gepa/tests/test_acceptance_criterion.py#L44): Known score vectors exercise strict/equal/worse/marginal acceptance.

