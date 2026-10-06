# finch

A small model-editable notebook analysis environment with full-notebook rerun after each code-cell edit.

Role: notebook-agent environment and evaluation composition. Runtime: Python, R.

Pinned source: [https://github.com/Future-House/finch](https://github.com/Future-House/finch); revision/version `aea66fdf2dd2be827727de50a73cae60dff59972`.

Aviary environment consumed by LDP rollout agent; local Jupyter kernel or Docker nbconvert notebook execution.

Owns temporary notebook workspaces/kernel/container lifecycle; generic provider loop is delegated to LDP/Aviary, storage can be GCS-backed.

Inspection: environment reset/step/action execution, notebook storage/kernel lifetime, rollout composition and notebook integration assertions

Limits of this study: Underlying LDP provider loop is a dependency rather than implemented in this directory; source only, no Docker/Jupyter/tests/models executed.

## Actions

### reset / step

Surface: environment API.

Input: Workspace/language/runtime configuration then ToolRequestMessage.

Result: Initial tool schemas plus notebook observation; step returns messages,reward delta,done,truncated=false.

Lifecycle: Episode owns local kernel/container; tool calls sequential and errors handled as observations.

Authority: Aviary/LDP caller, not standalone Finch-owned provider loop.

Evidence: [e1](#evidence-e1), [e2](#evidence-e2), [e6](#evidence-e6).

### edit_cell

Surface: model tool.

Input: contents code and optional idx; invalid numeric idx becomes append.

Result: Append/edit message followed by updated full notebook environment observation.

Lifecycle: Finally saves and reruns entire notebook; repeated effects can recur in local live namespace.

Authority: Arbitrary code under runtime authority, code-only cell surface.

Evidence: [e3](#evidence-e3), [e4](#evidence-e4), [e5](#evidence-e5).

### list_workdir / download_from_bucket

Surface: model tools.

Input: No arguments for recursive listing; configured optional bucket/path arguments.

Result: Directory JSON or downloaded workspace contents.

Lifecycle: Workspace scoped path; download capability exposed only when configured.

Authority: Configured environment/GCS credentials; not an agent-owned standing database.

Evidence: [e2](#evidence-e2), [e3](#evidence-e3).

### submit_answer

Surface: model tool.

Input: String/numeric/structured answer depending on subclass.

Result: Submitted answer text and done state.

Lifecycle: DataAnalysisEnv closes owned runtime immediately; base NBEnvironment only sets done.

Authority: Submission is not correctness validation.

Evidence: [e3](#evidence-e3), [e7](#evidence-e7).

## Capabilities

### filesystem

**I — Files** (source): Workspace copied into temporary directory by default; notebook/Markdown/analysis files saved, listed and copied at trajectory end. Cell code can perform file IO under runtime authority.

Evidence: [e1](#evidence-e1), [e3](#evidence-e3), [e4](#evidence-e4), [e6](#evidence-e6).

### processes

**L — OS programs** (source): Owns local Jupyter or Docker/container process and closes it at end; no general shell-process handle API is exposed beyond notebook code and fixed nbconvert command.

Evidence: [e1](#evidence-e1), [e4](#evidence-e4).

### code-actions

**I — Code actions** (source): Native edit_cell(contents,idx) modifies code only and reruns full notebook in finally. Local and Docker error behavior differs: local raises at first error; Docker allows cell errors.

Evidence: [e2](#evidence-e2), [e3](#evidence-e3), [e4](#evidence-e4), [e5](#evidence-e5).

### persistent-kernel

**L — Kernel** (source): Local AsyncKernelManager persists for environment episode, but every edit reruns every notebook cell into that live kernel. Docker nbconvert invokes notebook execution anew; neither is a shared standing service, and submit closes analysis kernel.

Evidence: [e1](#evidence-e1), [e3](#evidence-e3), [e4](#evidence-e4), [e7](#evidence-e7).

### standing-database

**? — Standing DB** (inspection): Not established in the explicitly inspected environment reset/step/action execution, notebook storage/kernel lifetime, rollout composition and notebook integration assertions; no universal absence claim.

### workflow-programming

**S — Workflows** (source): Model can edit notebook computation sequence; operator configures agent/evaluator/rollout callbacks. No general live workflow scheduler is supplied in this environment.

Evidence: [e3](#evidence-e3), [e6](#evidence-e6), [e8](#evidence-e8).

### multi-model

**? — Models** (inspection): Not established in the explicitly inspected environment reset/step/action execution, notebook storage/kernel lifetime, rollout composition and notebook integration assertions; no universal absence claim.

### live-collaboration

**? — Peer chat** (inspection): Not established in the explicitly inspected environment reset/step/action execution, notebook storage/kernel lifetime, rollout composition and notebook integration assertions; no universal absence claim.

### concurrent-work

**L — Concurrency** (source): Environment tool calls are deliberately concurrency=False and each notebook runs sequentially; independent rollouts belong to external LDP evaluator.

Evidence: [e2](#evidence-e2), [e5](#evidence-e5), [e8](#evidence-e8).

### steering-interrupt

**? — Steer/interrupt** (inspection): Not established in the explicitly inspected environment reset/step/action execution, notebook storage/kernel lifetime, rollout composition and notebook integration assertions; no universal absence claim.

### turn-redefinition

**? — Turn program** (inspection): Not established in the explicitly inspected environment reset/step/action execution, notebook storage/kernel lifetime, rollout composition and notebook integration assertions; no universal absence claim.

### compaction

**S — Compaction** (source): Composition requires hide_old_env_states when agent supports it; latest full notebook projection replaces redundant old environment observations. This is state projection, not archived managed conversational compaction.

Evidence: [e2](#evidence-e2), [e6](#evidence-e6).

### context-repair

**L — Repair** (source): Current notebook is reloadable and rendered as observation; previous overwritten notebook/output bytes are not preserved for repairing omitted or corrupted original conversation.

Evidence: [e1](#evidence-e1), [e4](#evidence-e4).

### original-audit

**L — Original audit** (source): Action strings, notebook outputs and external trajectory callback offer analysis records. Notebook is overwritten after execution and does not preserve each original version or complete provider/OS IO.

Evidence: [e1](#evidence-e1), [e2](#evidence-e2), [e3](#evidence-e3), [e4](#evidence-e4), [e8](#evidence-e8).

### audit-query

**? — Audit query** (inspection): Not established in the explicitly inspected environment reset/step/action execution, notebook storage/kernel lifetime, rollout composition and notebook integration assertions; no universal absence claim.

### hot-change

**L — Hot change** (source): Model code-cell edits take effect immediately through full rerun; this updates scientific program, not the harness or agent turn engine.

Evidence: [e3](#evidence-e3).

### rebuild-continuity

**? — Rebuild continuity** (inspection): Not established in the explicitly inspected environment reset/step/action execution, notebook storage/kernel lifetime, rollout composition and notebook integration assertions; no universal absence claim.

### remote-services

**S — Remote** (source): GCS-backed workspace and HTTP TaskDatasetClient/evaluator composition are remote-storage/environment boundaries. No consumed durable remote kernel protocol is established.

Evidence: [e8](#evidence-e8).

### self-improvement

**? — Self-improve** (inspection): Not established in the explicitly inspected environment reset/step/action execution, notebook storage/kernel lifetime, rollout composition and notebook integration assertions; no universal absence claim.

### complaints

**? — Complaints** (inspection): Not established in the explicitly inspected environment reset/step/action execution, notebook storage/kernel lifetime, rollout composition and notebook integration assertions; no universal absence claim.

### authority

**L — Authority** (source): Direct code executes with local kernel authority or mounted Docker workspace. No per-edit approval gate appears on traced action route; isolation depends on configured runtime.

Evidence: [e1](#evidence-e1), [e2](#evidence-e2), [e3](#evidence-e3), [e4](#evidence-e4).

### evaluation

**L — Evaluation** (source): NB tests assert executed literal text/plot replacement and normalized environment state; base submit merely ends episode and DataAnalysisEnv does not grade its stored eval_mode here. Evaluator is external dependency.

Evidence: [e3](#evidence-e3), [e7](#evidence-e7), [e8](#evidence-e8).

### time-order

**L — Time/order** (source): Tool calls and cell reruns are sequential; local Jupyter outputs match parent message IDs. No durable attempt/generation causal ledger is shown.

Evidence: [e2](#evidence-e2), [e5](#evidence-e5).

## Inspected test oracles

- [quarantine/finch/tests/test_nb_env.py](../../../quarantine/finch/tests/test_nb_env.py): Notebook execution/projection drift after replacing plot cell Oracle: Temporary workspace with real configured kernel/container asserts literal hello/Goodbye counts, plot/image disappearance after edit and EnvStateMessage text/image shapes. Docker/R cases skip when unavailable; no action-history/replay or side-effect-idempotence oracle. Tests not run. Read, **not executed**.

## Useful mechanisms

- Simple computation-as-edit surface and notebook-as-current-state ergonomics.
- Kernel/runtime ownership and episode termination are directly visible.

## Material limits

- Rerunning all cells can repeat external effects and accumulate local state.
- Overwritten notebook is a projection, not immutable original audit.
- Provider loop, training and evaluation machinery are external compositions.

## Arconaut design questions

- Should an edit rerun from a clean namespace, dependency suffix or explicit operator policy?
- How can shared kernel consumers expose namespace lifetime/authority without owning service teardown?

## Evidence

### Evidence e1

[quarantine/finch/src/fhda/notebook_env.py:39–105](../../../quarantine/finch/src/fhda/notebook_env.py#L39): Notebook file state can reload, local kernel starts in workdir or Docker mounts /workspace; close terminates owned execution.

### Evidence e2

[quarantine/finch/src/fhda/notebook_env.py:150–185](../../../quarantine/finch/src/fhda/notebook_env.py#L150): Tools are edit_cell/list_workdir/submit_answer plus optional bucket download; step invokes sequentially and returns notebook-state observation/reward/done.

### Evidence e3

[quarantine/finch/src/fhda/notebook_env.py:215–269](../../../quarantine/finch/src/fhda/notebook_env.py#L215): edit_cell appends/replaces code then finally saves/reruns notebook; submit_answer ends base episode without correctness evaluation.

### Evidence e4

[quarantine/finch/src/fhda/notebook_env.py:286–384](../../../quarantine/finch/src/fhda/notebook_env.py#L286): Docker runs nbconvert --execute --inplace --allow-errors, local path uses live kernel; state is overwritten/reloaded and temporary directory cleaned on close.

### Evidence e5

[quarantine/finch/src/fhda/utils.py:150–234](../../../quarantine/finch/src/fhda/utils.py#L150): Local rerun iterates cells, matches Jupyter message parent ID, records outputs and raises on error before continuing.

### Evidence e6

[quarantine/finch/src/expts/eval.py:154–191](../../../quarantine/finch/src/expts/eval.py#L154): Configured LDP agent consumes environment via RolloutManager; hide-old-env-state setting is required when offered; outputs copied on trajectory completion.

### Evidence e7

[quarantine/finch/src/fhda/data_analysis_env.py:65–79](../../../quarantine/finch/src/fhda/data_analysis_env.py#L65): DataAnalysisEnv submit marks done and immediately closes owned kernel/container.

### Evidence e8

[quarantine/finch/src/expts/eval.py:67–91](../../../quarantine/finch/src/expts/eval.py#L67): Evaluator callbacks record trajectories/workspaces and configured agent; benchmark apparatus is composition rather than automatic scientific verification.

