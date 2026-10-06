# paper-qa

A literature retrieval/evidence/answer agent whose callable vocabulary is research operations rather than arbitrary coding or shell.

Role: literature research agent and retrieval library. Runtime: Python.

Pinned source: [https://github.com/Future-House/paper-qa](https://github.com/Future-House/paper-qa); revision/version `57e89f7223b0960d5ee5ea048c69e3c47e088572`.

Async Aviary/LDP/custom tool-selector loop over document/index/session state; separate answer/summary/embedding provider calls.

Consumes model/embedding/literature APIs; owns local document index and transient question/session evidence, no owned scientific execution kernel.

Inspection: agent selection/loop/timeout, tool construction, evidence mutation/reset, persistent index recovery and selected assertions

Limits of this study: Aviary/LDP/LiteLLM inner provider transports remain dependencies; inspected loop and domain operations do not establish general coding-agent features. Tests not run.

## Actions

### agent_query / run_agent

Surface: library/operator API.

Input: Query, Docs, Settings, agent type/custom runner and callbacks.

Result: AnswerResponse(session,status), evidence/answer/cost.

Lifecycle: Async bounded rollout; timeout can trigger answer fallback with TRUNCATED status.

Authority: Caller configures models and tool vocabulary.

Evidence: [e1](#evidence-e1), [e2](#evidence-e2).

### paper_search

Surface: model tool.

Input: Query and optional publication-year interval.

Result: Search result/status; matching document texts enter Docs.

Lifecycle: Repeated query/year increments search offset; concurrency-safe discovery metadata.

Authority: Configured local corpus/search clients, not unrestricted filesystem search.

Evidence: [e3](#evidence-e3), [e5](#evidence-e5).

### gather_evidence / gen_answer

Surface: model tools.

Input: Specific question for evidence; current session for answer.

Result: Scored cited contexts and synthesized answer in PQASession.

Lifecycle: Evidence summaries are bounded parallel calls; shared question swapped/restored; answer models separate.

Authority: Configured summary/answer/embedding providers; authored synthesis is not source truth.

Evidence: [e6](#evidence-e6), [e7](#evidence-e7), [e8](#evidence-e8).

### reset / complete

Surface: model tools.

Input: Reset no arguments; complete has_successful_answer boolean.

Result: Cleared current contexts; terminal Certain/Unsure status and latest answer.

Lifecycle: Reset does not discard corpus; complete terminates using existing answer.

Authority: Model-declared certainty; no independent verification gate.

Evidence: [e9](#evidence-e9).

### get_directory_index / indexed per-file processing

Surface: support API.

Input: Paper directory/settings/manifest and rebuild flag.

Result: Persisted index/document identities and marked failures.

Lifecycle: Resumable per-file build; concurrent process locking is not supplied.

Authority: Operator supplied corpus/index serialization; historical material remains data.

Evidence: [e1](#evidence-e1), [e10](#evidence-e10), [e11](#evidence-e11).

## Capabilities

### filesystem

**L — Files** (source): Reads/indexes operator-supplied paper files and writes local persistent index objects; the model tool vocabulary is literature search/evidence/answer, not generic file editing.

Evidence: [e5](#evidence-e5), [e11](#evidence-e11).

### processes

**? — OS programs** (inspection): Not established in the explicitly inspected agent selection/loop/timeout, tool construction, evidence mutation/reset, persistent index recovery and selected assertions; no universal absence claim.

### code-actions

**? — Code actions** (inspection): Not established in the explicitly inspected agent selection/loop/timeout, tool construction, evidence mutation/reset, persistent index recovery and selected assertions; no universal absence claim.

### persistent-kernel

**? — Kernel** (inspection): Not established in the explicitly inspected agent selection/loop/timeout, tool construction, evidence mutation/reset, persistent index recovery and selected assertions; no universal absence claim.

### standing-database

**S — Standing DB** (source): Persistent searchable corpus/index and session citation contexts support standing literature memory. This is not a generic standing SQL database callable by the model.

Evidence: [e1](#evidence-e1), [e5](#evidence-e5), [e11](#evidence-e11).

### workflow-programming

**S — Workflows** (source): Operator can compose custom runner/tool selector, configured tools/models and per-action/environment callbacks. Domain reset/search/gather/generate sequence is programmable; no general live workflow scheduler is established.

Evidence: [e1](#evidence-e1), [e2](#evidence-e2), [e3](#evidence-e3).

### multi-model

**I — Models** (source): Agent selection, answer, evidence-summary and embedding model roles are configured separately. These are pipeline roles, not live independent peers.

Evidence: [e1](#evidence-e1), [e3](#evidence-e3), [e7](#evidence-e7), [e8](#evidence-e8).

### live-collaboration

**? — Peer chat** (inspection): Not established in the explicitly inspected agent selection/loop/timeout, tool construction, evidence mutation/reset, persistent index recovery and selected assertions; no universal absence claim.

### concurrent-work

**L — Concurrency** (source): Paper search metadata permits concurrency and evidence snippets are summarized with a bounded parallel request limit. gather_evidence mutates shared question and cannot run concurrently with itself; index lacks cross-process file locks.

Evidence: [e3](#evidence-e3), [e5](#evidence-e5), [e6](#evidence-e6), [e7](#evidence-e7), [e10](#evidence-e10).

### steering-interrupt

**L — Steer/interrupt** (source): Overall runner timeout and max timestep force truncated status and may issue one answer-generation fallback. No operator steer/pause handle is established in inspected runner.

Evidence: [e1](#evidence-e1), [e2](#evidence-e2).

### turn-redefinition

**S — Turn program** (source): Settings choose fake/Aviary/LDP or custom selector plus callbacks; this is per-run composition, not hot turn-program replacement.

Evidence: [e1](#evidence-e1), [e2](#evidence-e2), [e3](#evidence-e3).

### compaction

**S — Compaction** (source): Evidence retrieval reduces corpus to bounded summarized matches. This is domain retrieval compression, not archived conversational context compaction.

Evidence: [e7](#evidence-e7).

### context-repair

**S — Repair** (source): Model reset discards unsuitable current evidence, then can search/gather afresh; original indexed documents remain. It does not reconstruct lost/corrupted full conversation.

Evidence: [e5](#evidence-e5), [e6](#evidence-e6), [e9](#evidence-e9).

### original-audit

**L — Original audit** (source): Session stores token counts/tool-name batches, answer/citation contexts and callback opportunities; this is not every original request/result or execution audit.

Evidence: [e2](#evidence-e2), [e4](#evidence-e4), [e8](#evidence-e8).

### audit-query

**S — Audit query** (source): Tool-history name lookup and evidence/corpus retrieval expose selected research records, not a general complete audit query.

Evidence: [e4](#evidence-e4), [e5](#evidence-e5).

### hot-change

**? — Hot change** (inspection): Not established in the explicitly inspected agent selection/loop/timeout, tool construction, evidence mutation/reset, persistent index recovery and selected assertions; no universal absence claim.

### rebuild-continuity

**L — Rebuild continuity** (source): Index construction resumes saved per-file marks after crash; active agent/provider/turn state and executable rebuild continuity are outside that mechanism.

Evidence: [e1](#evidence-e1), [e11](#evidence-e11).

### remote-services

**I — Remote** (source): Search/index clients and distinct model/embedding calls are consumed through configured tools/Docs pipeline; no remote shell/kernel/service governance is implied.

Evidence: [e3](#evidence-e3), [e7](#evidence-e7), [e8](#evidence-e8).

### self-improvement

**? — Self-improve** (inspection): Not established in the explicitly inspected agent selection/loop/timeout, tool construction, evidence mutation/reset, persistent index recovery and selected assertions; no universal absence claim.

### complaints

**? — Complaints** (inspection): Not established in the explicitly inspected agent selection/loop/timeout, tool construction, evidence mutation/reset, persistent index recovery and selected assertions; no universal absence claim.

### authority

**L — Authority** (source): Configured tool set defines model action ceiling; model must call tools and reports its own answer sufficiency. No generic command-approval gate is present in traced domain dispatcher; provider/library policies remain external.

Evidence: [e2](#evidence-e2), [e3](#evidence-e3), [e9](#evidence-e9), [e10](#evidence-e10).

### evaluation

**L — Evaluation** (source): Model has_successful_answer is a declared sufficiency signal; truncation/timeout may still produce an answer. Inspected agent tests assert nonempty answer/evidence/cost, not independent semantic/scientific truth.

Evidence: [e1](#evidence-e1), [e9](#evidence-e9).

### time-order

**L — Time/order** (source): Loop accumulates action then observations; search pagination and index records maintain local progress. No durable complete causal event stream is established.

Evidence: [e2](#evidence-e2), [e4](#evidence-e4), [e5](#evidence-e5), [e11](#evidence-e11).

## Inspected test oracles

- [quarantine/paper-qa/tests/test_agents.py](../../../quarantine/paper-qa/tests/test_agents.py): Index crash resume and agent answer/accounting plumbing Oracle: Inspected lines 183–234 inject a crash after known file count, reopen index and assert all files indexed with fewer reparses than full rebuild. Lines 350–393 assert nonempty answer/context, question equality, tokens/cost and formatting under a debug prompt demanding tool order. This verifies mechanics, not correctness of the answer; provider fixtures/cassettes are not live validation. Not run. Read, **not executed**.

## Useful mechanisms

- Explicit separation of retrieval, evidence summary, answer and termination.
- Persistent corpus offers source-grounded recall and recoverable per-file indexing.

## Material limits

- Research tool vocabulary is not general scientific/coding execution.
- Model-declared answer sufficiency and nonempty-output tests are weak semantic oracles.
- Shared evidence mutation and unlocked index builds constrain concurrency.

## Arconaut design questions

- How should retrieved/summarized evidence remain distinct from original source bytes in managed context?
- Can specialist role/model decomposition expose clear source authority and independent oracles?

## Evidence

### Evidence e1

[quarantine/paper-qa/src/paperqa/agents/main.py:91–178](../../../quarantine/paper-qa/src/paperqa/agents/main.py#L91): run_agent chooses fake/Aviary/LDP runner; index is built once; timeout/truncation may force answer generation while retaining truncated/fail status.

### Evidence e2

[quarantine/paper-qa/src/paperqa/agents/main.py:275–323](../../../quarantine/paper-qa/src/paperqa/agents/main.py#L275): ToolSelectorLedger accumulates observations/actions, retries malformed model messages five times, awaits environment step and stops on complete or max timestep.

### Evidence e3

[quarantine/paper-qa/src/paperqa/agents/env.py:47–140](../../../quarantine/paper-qa/src/paperqa/agents/env.py#L47): Settings constructs named tools with concurrency metadata and separate answer/summary/embedding models; complete is ordered last in discovery.

### Evidence e4

[quarantine/paper-qa/src/paperqa/agents/tools.py:75–104](../../../quarantine/paper-qa/src/paperqa/agents/tools.py#L75): Session action recording counts tokens and only native tool names in tool_history; relevant contexts are score filtered.

### Evidence e5

[quarantine/paper-qa/src/paperqa/agents/tools.py:109–184](../../../quarantine/paper-qa/src/paperqa/agents/tools.py#L109): paper_search accepts query/year filters, permits search concurrency and tracks pagination by repeated query/year pair.

### Evidence e6

[quarantine/paper-qa/src/paperqa/agents/tools.py:255–281](../../../quarantine/paper-qa/src/paperqa/agents/tools.py#L255): gather_evidence temporarily replaces session question and restores it finally; this mutability prevents parallel calls of itself.

### Evidence e7

[quarantine/paper-qa/src/paperqa/docs.py:492–564](../../../quarantine/paper-qa/src/paperqa/docs.py#L492): Evidence retrieval obtains bounded matches and parallel LLM summaries under session identity using explicit concurrency limit.

### Evidence e8

[quarantine/paper-qa/src/paperqa/agents/tools.py:314–355](../../../quarantine/paper-qa/src/paperqa/agents/tools.py#L314): gen_answer calls Docs.aquery with current session plus distinct model roles and callbacks.

### Evidence e9

[quarantine/paper-qa/src/paperqa/agents/tools.py:389–440](../../../quarantine/paper-qa/src/paperqa/agents/tools.py#L389): reset clears current contexts; complete records model-reported successful-answer boolean and installs no-answer phrase if absent.

### Evidence e10

[quarantine/paper-qa/src/paperqa/agents/env.py:243–319](../../../quarantine/paper-qa/src/paperqa/agents/env.py#L243): Reset creates fresh query session; index building is excluded because it lacks file locks; ordinary prose is rejected as progress.

### Evidence e11

[quarantine/paper-qa/src/paperqa/agents/search.py:506–540](../../../quarantine/paper-qa/src/paperqa/agents/search.py#L506): Per-file index build uses semaphore and records failed documents/save state for resumable construction.

