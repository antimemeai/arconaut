# self-harness

Bounded hook edits, candidate queues and explicit train/heldout promotion support a real harness improvement experiment, without live native refit.

Role: candidate harness optimization workflow. Runtime: Python.

Pinned source: [https://github.com/qzzqzzb/Self-Harness](https://github.com/qzzqzzb/Self-Harness); revision/version `2720dbb3f52283684f4b85a1065d642df1779dd8`.

CLI orchestration evaluates separate candidate harness surfaces in Harbor/DeepAgents environment, with diagnosis/proposal and acceptance phases.

Owns experimental candidate/branch records; consumes task evaluator/model/backend rather than supplying full live operator harness.

Inspection: Read workflow candidate lifecycle/branch finalization, materialization, hook edit policy, acceptance metric validation and backend execution boundary.

Limits of this study: No Harbor/model execution or results reproduction; backend dependencies and full task-identity/oracle integrity not audited.

## Actions

### run_self_harness_loop

Surface: workflow CLI.

Input: Eval config, work dir, proposal/diagnosis sources and candidate cap

Result: Persisted queue, baseline, candidate evaluations and branch state

Lifecycle: Resumes pending candidate workflow; sequential evaluation/finalization.

Authority: Operator/program launch

Evidence: [queue](#evidence-queue), [branch](#evidence-branch).

### apply_candidate_values / materialize_candidate

Surface: candidate API.

Input: Virtual hook edit, declared surfaces and output dir

Result: Executable candidate files and manifest

Lifecycle: Separate candidate artifact before evaluation.

Authority: Bounded edit menu

Evidence: [hooks](#evidence-hooks), [materialize](#evidence-materialize).

### run_acceptance_gate

Surface: evaluation CLI/API.

Input: Baseline/candidate split repeated metrics

Result: Accepted/rejected reason with split means/deltas

Lifecycle: Evaluation selection; does not deploy current harness.

Authority: Independent workflow gate

Evidence: [gate](#evidence-gate), [metric](#evidence-metric).

### HarborSandbox.aexecute

Surface: supplied backend API.

Input: Command and timeout

Result: Output/exit result or timeout 124

Lifecycle: Awaited task-environment operation; remote cleanup unproven.

Authority: Task environment authority

Evidence: [backend](#evidence-backend).

## Capabilities

### filesystem

**S — Files** (source): Candidate materialization writes declared relative files; task backend provides file operations via external Harbor boundary.

Evidence: [materialize](#evidence-materialize), [backend](#evidence-backend).

### processes

**S — OS programs** (source): HarborSandbox command API waits asynchronously and reports timeout; process lifetime beyond await belongs to environment.

Evidence: [backend](#evidence-backend).

### code-actions

**S — Code actions** (source): Editable Python hook programs shape prompts/tools/subagents/middleware/runtime policy, rather than only prose.

Evidence: [hooks](#evidence-hooks).

### persistent-kernel

**— — Kernel** (role): persistent_kernel: this reference supplies candidate harness optimization workflow; an agent policy, model interface and context lifecycle are outside its supplied role.

### standing-database

**— — Standing DB** (role): standing_database: this reference supplies candidate harness optimization workflow; an agent policy, model interface and context lifecycle are outside its supplied role.

### workflow-programming

**S — Workflows** (source): Explicit persisted candidate queue and evaluation/accept/finalize pipeline are executable.

Evidence: [queue](#evidence-queue), [branch](#evidence-branch).

### multi-model

**— — Models** (role): multi_model: this reference supplies candidate harness optimization workflow; an agent policy, model interface and context lifecycle are outside its supplied role.

### live-collaboration

**— — Peer chat** (role): live_collaboration: this reference supplies candidate harness optimization workflow; an agent policy, model interface and context lifecycle are outside its supplied role.

### concurrent-work

**— — Concurrency** (role): concurrent_work: this reference supplies candidate harness optimization workflow; an agent policy, model interface and context lifecycle are outside its supplied role.

### steering-interrupt

**L — Steer/interrupt** (source): Timeout wraps environment await and editable interrupt hook exists; no traced busy operator steering loop or confirmation that remote command terminates.

Evidence: [backend](#evidence-backend), [hooks](#evidence-hooks).

### turn-redefinition

**— — Turn program** (role): turn_redefinition: this reference supplies candidate harness optimization workflow; an agent policy, model interface and context lifecycle are outside its supplied role.

### compaction

**— — Compaction** (role): compaction: this reference supplies candidate harness optimization workflow; an agent policy, model interface and context lifecycle are outside its supplied role.

### context-repair

**— — Repair** (role): context_repair: this reference supplies candidate harness optimization workflow; an agent policy, model interface and context lifecycle are outside its supplied role.

### original-audit

**L — Original audit** (source): Candidate/diagnosis/evaluation/acceptance records are useful experimental provenance, not full original provider/program/transformation IO capture.

Evidence: [queue](#evidence-queue), [role](#evidence-role).

### audit-query

**S — Audit query** (source): Branch/queue/acceptance JSON is programmatically accessible; this is an experimental corpus, not universal agent audit.

Evidence: [queue](#evidence-queue), [gate](#evidence-gate).

### hot-change

**L — Hot change** (source): Accepted candidates become child branch records after evaluation; no after-current-turn live definition activation established.

Evidence: [branch](#evidence-branch).

### rebuild-continuity

**? — Rebuild continuity** (inspection scope): No compiled executable/outpost handoff or provider/context re-inhabitation path established in candidate/evaluation/branch workflow.

### remote-services

**— — Remote** (role): remote_services: this reference supplies candidate harness optimization workflow; an agent policy, model interface and context lifecycle are outside its supplied role.

### self-improvement

**S — Self-improve** (source): Dedicated diagnose/propose/materialize/evaluate/promote flow for bounded harness hooks; evaluated branch creation, not in-situ core refit.

Evidence: [queue](#evidence-queue), [hooks](#evidence-hooks), [gate](#evidence-gate).

### complaints

**— — Complaints** (role): complaints: this reference supplies candidate harness optimization workflow; an agent policy, model interface and context lifecycle are outside its supplied role.

### authority

**S — Authority** (source): Hook aliases and declared surfaces bound changes; actual task authority comes from backend and mutable permission hooks.

Evidence: [hooks](#evidence-hooks), [materialize](#evidence-materialize).

### evaluation

**S — Evaluation** (source): Gate validates repeated split metrics and rejects regression in any mean; evaluator/task identity integrity is externally supplied.

Evidence: [gate](#evidence-gate), [metric](#evidence-metric).

### time-order

**S — Time/order** (source): Candidates record evaluated_at wall time; per-command async timeout exists, not durable agent causal/pause clock.

Evidence: [queue](#evidence-queue), [backend](#evidence-backend).

## Inspected test oracles

No relevant populated test source was recorded in this inspection; capability claims remain source/document traces.


## Useful mechanisms

- Harness edit surfaces include runtime/tool policy, with evaluation and promotion recorded separately.
- Per-split acceptance prevents aggregate gain from hiding a mean drop in one split.

## Material limits

- Mean pass-rate gain with two default repeats is not statistical confidence or proof of task identity equality.
- Promotion to branch records does not establish safe in-place activation, shared-service boundary or native rebuild continuity.

## Arconaut design questions

- Which mutable programs are candidate surfaces, and which independent invariant oracles survive their replacement?
- How should model-governed experiment selection expose uncertainty and generalization while permitting operator steering?

## Evidence

### Evidence queue

[quarantine/self-harness/workflow/scripts/run_self_harness_loop.py:437–497](../../../quarantine/self-harness/workflow/scripts/run_self_harness_loop.py#L437): Pending candidates are materialized, evaluated, gated and persisted before merge-group finalization.

### Evidence branch

[quarantine/self-harness/workflow/scripts/run_self_harness_loop.py:501–545](../../../quarantine/self-harness/workflow/scripts/run_self_harness_loop.py#L501): Groups wait for pending evaluations then create child branch for accepted candidate, not hot-activate current harness.

### Evidence hooks

[quarantine/self-harness/proposer/src/self_harness_proposer/hooks.py:9–111](../../../quarantine/self-harness/proposer/src/self_harness_proposer/hooks.py#L9): Hook menu covers prompts, subagents, tools, middleware and runtime controls; one virtual hook change required.

### Evidence materialize

[quarantine/self-harness/proposer/src/self_harness_proposer/materialize.py:79–114](../../../quarantine/self-harness/proposer/src/self_harness_proposer/materialize.py#L79): Candidate files are bounded to declared safe relative surfaces and changed values recorded.

### Evidence gate

[quarantine/self-harness/acceptance/scripts/run_acceptance_gate.py:77–130](../../../quarantine/self-harness/acceptance/scripts/run_acceptance_gate.py#L77): Accept when no split mean pass rate decreases and at least one improves; default train/heldout and two repeats.

### Evidence metric

[quarantine/self-harness/acceptance/scripts/run_acceptance_gate.py:140–188](../../../quarantine/self-harness/acceptance/scripts/run_acceptance_gate.py#L140): Validate repeat cardinality/uniqueness and passed/total ranges, and compare repeat denominators.

### Evidence backend

[quarantine/self-harness/eval/harness_workspace/self_harness_harbor/backend_bridge.py:33–91](../../../quarantine/self-harness/eval/harness_workspace/self_harness_harbor/backend_bridge.py#L33): Backend executes task-environment commands asynchronously with timeout result 124; timeout of await is not proof remote process died.

### Evidence role

[quarantine/self-harness/README.md:16–22](../../../quarantine/self-harness/README.md#L16): Authored loop holds weights/evaluator fixed while proposing bounded harness edits and promoting candidates.

