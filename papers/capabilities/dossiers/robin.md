# robin

Public orchestration for hypothesis/assay/candidate generation and multi-trajectory analysis, with proprietary hosted agent execution remaining external.

Role: scientific discovery pipeline and hosted-agent consumer. Runtime: Python.

Pinned source: [https://github.com/Future-House/robin](https://github.com/Future-House/robin); revision/version `4a5cce310f3bc7663a67117db88af43b84733ffe`.

Async local synthesis/ranking calls plus uploaded Edison literature/data-analysis tasks; notebook operator drives top-level stages.

Owns workflow/output files and consumed task IDs; Edison owns remote analysis/literature engines and their execution environments.

Inspection: Step pipeline/provider dispatch/remote handles, analysis consensus/ranking/results persistence, official availability boundary

Limits of this study: Hosted Edison internals and credentialed trajectories are unavailable. No standalone coding-agent loop or first-party tests were found in the inspected source file inventory; no code executed.

## Actions

### MultiTrajectoryRunner.add_step / run_pipeline

Surface: operator/library API.

Input: Step prompt/generator, input/output mappings, parallel count and StepConfig.

Result: results[step_id] with task IDs/responses/success rates and downloaded artifacts.

Lifecycle: Sequential stage barriers, parallel remote tasks within stage, final JSON save.

Authority: Configured remote credentials and local callback authority.

Evidence: [e2](#evidence-e2), [e3](#evidence-e3), [e4](#evidence-e4).

### call_platform / poll_for_task_completion / gather_results

Surface: hosted-service consumer API.

Input: Hypothesis/query map, hosted job name and Edison client; remote task_id.

Result: Responses or errors with submitted task identities/final statuses.

Lifecycle: Create then concurrent polling; local timeout does not terminate hosted work.

Authority: Paid hosted account; no source access to downstream agent.

Evidence: [e1](#evidence-e1), [e5](#evidence-e5).

### data_analysis

Surface: scientific workflow API.

Input: Data path, analysis type, goal and configuration.

Result: Parallel trajectory files, remote consensus outputs and interpreted results.

Lifecycle: Analysis stage then consensus stage consume file boundary; R runtime owned by Edison.

Authority: Remote compute customer; public wrapper never governs shared kernel.

Evidence: [e6](#evidence-e6), [e7](#evidence-e7).

### process_comparison_pair / run_comparisons

Surface: ranking API.

Input: Candidate pair IDs/dataframe, ranking/system prompts, LLM client, output path and concurrency bound.

Result: JSON comparison records and aggregate error/success reports/CSV.

Lifecycle: Bounded concurrent model requests, no external experiment execution in ranker.

Authority: Authored candidate reasoning and evaluator model; not a validated oracle.

Evidence: [e8](#evidence-e8), [e9](#evidence-e9).

## Capabilities

### filesystem

**I — Files** (source): Local pipeline writes JSON/reports/CSVs, uploads known inputs and downloads per-task outputs. No generic model-exposed file editor is supplied by these host functions.

Evidence: [e3](#evidence-e3), [e4](#evidence-e4).

### processes

**? — OS programs** (inspection): Not established in the explicitly inspected Step pipeline/provider dispatch/remote handles, analysis consensus/ranking/results persistence, official availability boundary; no universal absence claim.

### code-actions

**S — Code actions** (source): Data-analysis remote StepConfig declares Python/R environment and instructions; execution belongs to hosted Edison agent, whose code action implementation is unavailable.

Evidence: [e2](#evidence-e2), [e4](#evidence-e4), [e6](#evidence-e6).

### persistent-kernel

**L — Kernel** (documentation): Remote tasks can request R/Python analysis environments; lifetime/namespace persistence belongs to Edison and is not established by public wrapper. Notebook running Robin is operator execution, not an owned persistent model kernel.

Evidence: [e1](#evidence-e1), [e2](#evidence-e2), [e6](#evidence-e6).

### standing-database

**? — Standing DB** (inspection): Not established in the explicitly inspected Step pipeline/provider dispatch/remote handles, analysis consensus/ranking/results persistence, official availability boundary; no universal absence claim.

### workflow-programming

**I — Workflows** (source): Native Step arrays permit dynamic prompt generation from prior results, parallel task count, input/output maps and postprocess callbacks. Pipeline is sequential across steps, not a live arbitrary model-programmed turn.

Evidence: [e2](#evidence-e2), [e4](#evidence-e4).

### multi-model

**S — Models** (source): Configured direct LLM and multiple hosted agent roles separate synthesis/ranking from literature/analysis. Hosted model identities are opaque; selectable jobs are not proof of live multi-provider collaboration.

Evidence: [e7](#evidence-e7), [e8](#evidence-e8), [e10](#evidence-e10).

### live-collaboration

**L — Peer chat** (source): Consensus aggregates separate trajectory result files; no addressed live peer message mechanism is supplied in the inspected workflow.

Evidence: [e6](#evidence-e6).

### concurrent-work

**I — Concurrency** (source): Remote task batches and semaphore-bounded pairwise comparisons execute concurrently; step boundaries await results before next stage.

Evidence: [e4](#evidence-e4), [e5](#evidence-e5), [e9](#evidence-e9).

### steering-interrupt

**L — Steer/interrupt** (source): Overall timeout bounds local wait/polling and returns remote IDs; inspected timeout path does not cancel remote work or expose live steering. Stop waiting is not service quiescence.

Evidence: [e4](#evidence-e4), [e5](#evidence-e5).

### turn-redefinition

**? — Turn program** (inspection): Not established in the explicitly inspected Step pipeline/provider dispatch/remote handles, analysis consensus/ranking/results persistence, official availability boundary; no universal absence claim.

### compaction

**? — Compaction** (inspection): Not established in the explicitly inspected Step pipeline/provider dispatch/remote handles, analysis consensus/ranking/results persistence, official availability boundary; no universal absence claim.

### context-repair

**? — Repair** (inspection): Not established in the explicitly inspected Step pipeline/provider dispatch/remote handles, analysis consensus/ranking/results persistence, official availability boundary; no universal absence claim.

### original-audit

**L — Original audit** (source): Timestamped results contain task IDs/responses and success rate; downloads may fail with log-and-continue, and final save errors also merely log. This is not complete provider requests/remote execution IO.

Evidence: [e3](#evidence-e3), [e4](#evidence-e4), [e5](#evidence-e5).

### audit-query

**S — Audit query** (source): results[step_id] and remote task IDs permit stored-result inspection and Edison polling; no general local original-audit query layer is supplied.

Evidence: [e3](#evidence-e3), [e4](#evidence-e4), [e5](#evidence-e5).

### hot-change

**? — Hot change** (inspection): Not established in the explicitly inspected Step pipeline/provider dispatch/remote handles, analysis consensus/ranking/results persistence, official availability boundary; no universal absence claim.

### rebuild-continuity

**L — Rebuild continuity** (source): Independent remote task handles support service follow-up, while results remain process-local until final JSON. No automatic crash resume/executable refit continuity is established.

Evidence: [e3](#evidence-e3), [e5](#evidence-e5).

### remote-services

**I — Remote** (source): Public code consumes Edison create/upload/run-until-done/get/download contracts and independent chosen LLM calls. It does not contain proprietary analysis kernels or literature engines.

Evidence: [e1](#evidence-e1), [e4](#evidence-e4), [e5](#evidence-e5), [e7](#evidence-e7).

### self-improvement

**? — Self-improve** (inspection): Not established in the explicitly inspected Step pipeline/provider dispatch/remote handles, analysis consensus/ranking/results persistence, official availability boundary; no universal absence claim.

### complaints

**? — Complaints** (inspection): Not established in the explicitly inspected Step pipeline/provider dispatch/remote handles, analysis consensus/ranking/results persistence, official availability boundary; no universal absence claim.

### authority

**L — Authority** (source): Operator supplies provider/Edison credentials and workflow callback code. Public pipeline forwards task parameters; execution permission boundaries in hosted service are unavailable.

Evidence: [e1](#evidence-e1), [e2](#evidence-e2), [e4](#evidence-e4).

### evaluation

**L — Evaluation** (source): Pairwise LLM ranking scores proposed hypotheses and consensus aggregates analysis trajectories; task success_rate measures hosted status. Neither is independent scientific validity or measured experimental truth.

Evidence: [e4](#evidence-e4), [e6](#evidence-e6), [e8](#evidence-e8), [e9](#evidence-e9).

### time-order

**L — Time/order** (source): Steps have short UUID IDs and sequential dependencies; task IDs separate parallel trajectories; result filenames use wall-clock time. No durable write-ahead run/attempt causal ledger is supplied.

Evidence: [e2](#evidence-e2), [e3](#evidence-e3), [e4](#evidence-e4).

## Inspected test oracles

No relevant populated test source was recorded in this inspection; capability claims remain source/document traces.


## Useful mechanisms

- Step data-transfer and remote task identity are explicit and inspectable.
- Separates trajectory generation, consensus and hypothesis ranking as different workflows.

## Material limits

- Hosted scientific execution source is not public in this reference.
- Local timeout leaves remote work unresolved; final-only result save is weak crash continuity.
- Ranking authored proposals and remote success statuses do not validate science.

## Arconaut design questions

- How should consumed-service jobs be tracked/cancelled while harness-owned workflow pauses?
- What independent oracles distinguish useful consensus from correlated model agreement?

## Evidence

### Evidence e1

[quarantine/robin/README.md:5–11](../../../quarantine/robin/README.md#L5): Official public README requires purchased Edison access for data analysis while local hypothesis/experiment generation still uses chosen provider.

### Evidence e2

[quarantine/robin/robin/multitrajectory_runner.py:20–74](../../../quarantine/robin/robin/multitrajectory_runner.py#L20): Step/StepConfig declares prompt/generator, uploads/downloads, UID, parallel count, remote max steps/language/timeout and postprocess.

### Evidence e3

[quarantine/robin/robin/multitrajectory_runner.py:104–156](../../../quarantine/robin/robin/multitrajectory_runner.py#L104): Runner holds steps/results; timestamped JSON serializes remote response objects and logs save errors.

### Evidence e4

[quarantine/robin/robin/multitrajectory_runner.py:199–289](../../../quarantine/robin/robin/multitrajectory_runner.py#L199): Pipeline sequentially uploads, submits parallel remote tasks until done, stores task IDs/success rates, downloads outputs and postprocesses; save happens at overall end.

### Evidence e5

[quarantine/robin/robin/utils.py:31–128](../../../quarantine/robin/robin/utils.py#L31): Edison tasks are created then concurrently polled under overall timeout; timeout returns submitted IDs/statuses without remote cancellation.

### Evidence e6

[quarantine/robin/robin/analyses.py:17–83](../../../quarantine/robin/robin/analyses.py#L17): Analysis submits parallel R data-analysis trajectories then another remote consensus stage consuming produced flow-results files.

### Evidence e7

[quarantine/robin/robin/analyses.py:139–158](../../../quarantine/robin/robin/analyses.py#L139): Data interpretation is another direct LLM call parsed into four delimited fields; format error is logged.

### Evidence e8

[quarantine/robin/robin/utils.py:584–639](../../../quarantine/robin/robin/utils.py#L584): Pairwise hypothesis comparison uses authored candidate reasoning and asks LLM for JSON; this is preference/evaluation rather than experimental result measurement.

### Evidence e9

[quarantine/robin/robin/utils.py:698–741](../../../quarantine/robin/robin/utils.py#L698): Pair comparisons run under semaphore with configurable max concurrent requests and aggregate success/error records.

### Evidence e10

[quarantine/robin/robin/configuration.py:242–260](../../../quarantine/robin/robin/configuration.py#L242): Separate hosted agent roles can choose Crow/Falcon jobs; actual hosted model implementation is external.

