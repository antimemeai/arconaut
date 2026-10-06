# agent-laboratory

Role-based research phases refine experiment code with execution feedback and model reward before report review.

Role: phased scientific-research coding workflow. Runtime: Python.

Pinned source: [https://github.com/SamuelSchmidgall/AgentLaboratory](https://github.com/SamuelSchmidgall/AgentLaboratory); revision/version `d9017d90e329112d2a80b7712f37ee9094d2cd27`.

Synchronous role/phase loop, model text commands, fresh multiprocessing child for each generated-code execution.

Owns report/code/phase-object snapshots and code children; consumes model SDKs/search. No shared kernel/database service is present in traced execution.

Inspection: phase scheduler/model dispatch/command parser, code worker and timeout, history expiry, object snapshots and review/human feedback

Limits of this study: No first-party test suite found in inspected file inventory; application UI/AgentRxiv service not fully traced. Source only, no provider/code/PDF execution.

## Actions

### LaboratoryWorkflow.perform_research / set_model / save_state

Surface: operator/library API.

Input: Research topic, phase map/limits, role notes and optional human-loop flags.

Result: Phase products/status/stats and overwritten PaperN.pkl snapshot.

Lifecycle: Sequential phases with repeat/refinement backedges; model activation at subtask boundary.

Authority: Host configures roles/providers and trusted snapshot serialization.

Evidence: [e1](#evidence-e1), [e2](#evidence-e2).

### REPLACE / EDIT

Surface: model text commands.

Input: Entire fenced Python candidate or inclusive line range/new lines.

Result: Accepted candidate/output/reward or repair/rollback feedback.

Lifecycle: Executes dataset+candidate in fresh child before acceptance; failed text candidate reverted, external side effects not rolled back.

Authority: Child host OS authority; no true process sandbox in traced runner.

Evidence: [e5](#evidence-e5), [e6](#evidence-e6), [e8](#evidence-e8), [e9](#evidence-e9).

### execute_code

Surface: execution API.

Input: Python source and timeout.

Result: Captured stdout/traceback or timeout error string.

Lifecycle: Fresh multiprocessing child; direct terminate after join timeout; no durable handle.

Authority: Operator/runtime host account; substring prohibitions are not comprehensive isolation.

Evidence: [e9](#evidence-e9).

### get_score / ReviewersAgent.inference / report_refinement

Surface: evaluation/workflow API.

Input: Plan/code/output or plan/report and configured reward/reviewer model.

Result: Model score/persona reviews and loop-repeat decision.

Lifecycle: Synchronous model calls; fixed override may end review regardless of findings.

Authority: Authored model judgement over artifact text, not independent rerun oracle.

Evidence: [e7](#evidence-e7), [e10](#evidence-e10), [e11](#evidence-e11).

### human_in_loop

Surface: operator interaction.

Input: Phase product and user Y/N plus rejection notes.

Result: Accept or reset histories/repeat phase with notes.

Lifecycle: Phase boundary only; not live provider steering.

Authority: Explicit operator feedback selects repeat.

Evidence: [e12](#evidence-e12).

## Capabilities

### filesystem

**I — Files** (source): Generated experiment code/output/report files and overwritten pickle workflow snapshots are owned by host; arbitrary generated Python can access host files with child OS authority.

Evidence: [e1](#evidence-e1), [e9](#evidence-e9), [e13](#evidence-e13).

### processes

**L — OS programs** (source): Fresh child per code execution; timeout terminates direct process. No durable job ID, process-group descendant proof or shared external execution service appears.

Evidence: [e9](#evidence-e9).

### code-actions

**I — Code actions** (source): Fenced REPLACE and EDIT inclusive-line commands form actual executable coding actions; candidate code runs before acceptance, repairs have bounded retries and rollback only covers text candidate.

Evidence: [e5](#evidence-e5), [e6](#evidence-e6), [e8](#evidence-e8).

### persistent-kernel

**L — Kernel** (source): Each execution starts fresh globals/process; persistent model/workflow objects and dataset_code string are not a persistent scientific namespace.

Evidence: [e9](#evidence-e9).

### standing-database

**? — Standing DB** (inspection): Not established in the explicitly inspected phase scheduler/model dispatch/command parser, code worker and timeout, history expiry, object snapshots and review/human feedback; no universal absence claim.

### workflow-programming

**L — Workflows** (source): Fixed phase scheduler and configurable per-phase model/notes/human feedback steer research; editing experiment programs does not expose arbitrary turn/workflow graph programming.

Evidence: [e1](#evidence-e1), [e2](#evidence-e2), [e12](#evidence-e12).

### multi-model

**I — Models** (source): Per-phase model map selects provider/model at subtask boundary; role agents and reviewers may change model through host. Three reviewer personas are same configured model, not independent peers.

Evidence: [e1](#evidence-e1), [e2](#evidence-e2), [e4](#evidence-e4), [e11](#evidence-e11).

### live-collaboration

**L — Peer chat** (source): Role prompts exchange shared attributes and human phase feedback in the authored workflow; inspected paths provide no addressed asynchronous peer messages or busy-target delivery.

Evidence: [e1](#evidence-e1), [e3](#evidence-e3), [e12](#evidence-e12).

### concurrent-work

**? — Concurrency** (inspection): Not established in the explicitly inspected phase scheduler/model dispatch/command parser, code worker and timeout, history expiry, object snapshots and review/human feedback; no universal absence claim.

### steering-interrupt

**L — Steer/interrupt** (source): Human rejection repeats phase with new notes after phase product; code execution has timeout. No in-flight provider abort/steer contract was traced.

Evidence: [e9](#evidence-e9), [e12](#evidence-e12).

### turn-redefinition

**? — Turn program** (inspection): Not established in the explicitly inspected phase scheduler/model dispatch/command parser, code worker and timeout, history expiry, object snapshots and review/human feedback; no universal absence claim.

### compaction

**L — Compaction** (source): Context history expires selected feedback or drops oldest at max length; prior content is not archived by that transformation.

Evidence: [e3](#evidence-e3).

### context-repair

**L — Repair** (source): Phase rejection clears role histories and adds feedback; experiment repair edits code. Neither operation retrieves originals to repair lost active conversation.

Evidence: [e8](#evidence-e8), [e12](#evidence-e12).

### original-audit

**L — Original audit** (source): Phase objects, code/output logs and latest workflow pickle preserve selected state. Expired history and overwritten files/snapshot lose originals; no all-IO audit.

Evidence: [e1](#evidence-e1), [e3](#evidence-e3), [e13](#evidence-e13).

### audit-query

**? — Audit query** (inspection): Not established in the explicitly inspected phase scheduler/model dispatch/command parser, code worker and timeout, history expiry, object snapshots and review/human feedback; no universal absence claim.

### hot-change

**L — Hot change** (source): Per-phase model updates and candidate experiment code take effect within workflow; no harness executable refit or arbitrary hot engine reload is shown.

Evidence: [e2](#evidence-e2), [e8](#evidence-e8).

### rebuild-continuity

**L — Rebuild continuity** (source): Latest pickled phase state is a checkpoint primitive; inspected saves do not supply a resume loader, active-call/process continuity, executable rebuild or re-inhabitation.

Evidence: [e1](#evidence-e1), [e2](#evidence-e2).

### remote-services

**I — Remote** (source): Direct model SDK requests consume chosen providers; local code children are owned. Search roles exist separately, no remote shared kernel control plane inferred.

Evidence: [e4](#evidence-e4).

### self-improvement

**? — Self-improve** (inspection): Not established in the explicitly inspected phase scheduler/model dispatch/command parser, code worker and timeout, history expiry, object snapshots and review/human feedback; no universal absence claim.

### complaints

**? — Complaints** (inspection): Not established in the explicitly inspected phase scheduler/model dispatch/command parser, code worker and timeout, history expiry, object snapshots and review/human feedback; no universal absence claim.

### authority

**L — Authority** (source): Generated code runs under child host authority with substring filters for exit/pubmed; optional human review is phase-level and direct model SDK branches modify process environment credentials.

Evidence: [e4](#evidence-e4), [e9](#evidence-e9), [e12](#evidence-e12).

### evaluation

**L — Evaluation** (source): Code acceptance checks error substring; reward is parsed LLM score over plan/code/output, three report reviewers are persona calls to same model. This evaluates research artifacts, not independent scientific truth or harness self-improvement.

Evidence: [e6](#evidence-e6), [e7](#evidence-e7), [e11](#evidence-e11).

### time-order

**L — Time/order** (source): Sequential phase/step/history and process join timeout establish local order; snapshot lacks immutable run/attempt event identities.

Evidence: [e2](#evidence-e2), [e3](#evidence-e3), [e9](#evidence-e9).

## Inspected test oracles

No relevant populated test source was recorded in this inspection; capability claims remain source/document traces.


## Useful mechanisms

- Concrete staged research loop and experiment candidate rollback/feedback mechanics.
- Configurable model roles and human phase review are visible rather than marketing-only.

## Material limits

- Text rollback does not undo generated program effects.
- Same-model reward/persona review is not independent evaluation.
- Fresh process execution, destructive history expiry and overwritten pickle are weak continuity/audit.

## Arconaut design questions

- How can research-program candidate evaluation avoid conflating no traceback with validity?
- What independent task oracle and retained original execution evidence should govern autoresearch?

## Evidence

### Evidence e1

[quarantine/agent-laboratory/ai_lab_repo.py:60–113](../../../quarantine/agent-laboratory/ai_lab_repo.py#L60): Fixed phases map to model names and role agents; save_state overwrites one pickled workflow object per paper.

### Evidence e2

[quarantine/agent-laboratory/ai_lab_repo.py:139–204](../../../quarantine/agent-laboratory/ai_lab_repo.py#L139): Research traverses phase/subtask loops, changes models at boundaries, preserves phase_status and can recursively reset experiment phases after refinement.

### Evidence e3

[quarantine/agent-laboratory/agents.py:204–277](../../../quarantine/agent-laboratory/agents.py#L204): Agent rebuilds prompt from phase/context/history/notes, calls query_model, tracks response and drops expired/old history entries at bounded length.

### Evidence e4

[quarantine/agent-laboratory/inference.py:35–103](../../../quarantine/agent-laboratory/inference.py#L35): Provider branches construct SDK requests from system/prompt, retry, select OpenAI/Gemini/Anthropic and set global environment credentials.

### Evidence e5

[quarantine/agent-laboratory/mlesolver.py:55–82](../../../quarantine/agent-laboratory/mlesolver.py#L55): REPLACE parses fenced code and executes dataset+new code before accepting text as candidate.

### Evidence e6

[quarantine/agent-laboratory/mlesolver.py:99–138](../../../quarantine/agent-laboratory/mlesolver.py#L99): EDIT parses inclusive line range, splices candidate and executes dataset+new code; substring execution error is rejection oracle.

### Evidence e7

[quarantine/agent-laboratory/mlesolver.py:141–162](../../../quarantine/agent-laboratory/mlesolver.py#L141): Reward model reads plan/code/output and emits a SCORE float; validity is parsed model output rather than independently measured objective.

### Evidence e8

[quarantine/agent-laboratory/mlesolver.py:329–374](../../../quarantine/agent-laboratory/mlesolver.py#L329): Command dispatch tests edits, repairs up to fixed attempts and rolls back candidate code on failure; successful code gets model score.

### Evidence e9

[quarantine/agent-laboratory/tools.py:292–325](../../../quarantine/agent-laboratory/tools.py#L292): Code executes in fresh multiprocessing globals, captured stdout queue; timeout terminates direct process, not proven descendant group; simple substring filters are no sandbox.

### Evidence e10

[quarantine/agent-laboratory/ai_lab_repo.py:207–237](../../../quarantine/agent-laboratory/ai_lab_repo.py#L207): Report refinement reads three reviewer outputs and either bounded override or model decision returns to planning; optional human branch has distinct behavior.

### Evidence e11

[quarantine/agent-laboratory/agents.py:184–201](../../../quarantine/agent-laboratory/agents.py#L184): Three reviewers are sequential persona prompts to same configured model over plan/report.

### Evidence e12

[quarantine/agent-laboratory/ai_lab_repo.py:547–574](../../../quarantine/agent-laboratory/ai_lab_repo.py#L547): Human phase rejection resets agents, records notes and repeats phase; no live provider interrupt handle supplied.

### Evidence e13

[quarantine/agent-laboratory/ai_lab_repo.py:319–336](../../../quarantine/agent-laboratory/ai_lab_repo.py#L319): Best experiment code/output written to deliverable files after solver iterations; selecting best score is model-reward based.

