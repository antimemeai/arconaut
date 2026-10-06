# ai-scientist-v2

Branching experiment-code search with hyperparameter/ablation/multi-seed stages and model interpretation of measured outputs.

Role: scientific experiment code-search workflow. Runtime: Python.

Pinned source: [https://github.com/SakanaAI/AI-Scientist-v2](https://github.com/SakanaAI/AI-Scientist-v2); revision/version `96bd51617cfdbb494a9fc283af00fe090edfae48`.

Stage manager drives ProcessPoolExecutor candidate search; each worker drafts/debugs/improves code and uses resettable multiprocessing interpreter; later plotting/writeup/review calls.

Owns experiment process workspaces, tree journals/checkpoints and local GPU assignment; consumes model providers. Not a shared computation-service consumer interface.

Inspection: launcher, manager stage loop/checkpoint, node worker dispatch/code execution/provider adaptation, result/metric parsing, summary and protocol assertions

Limits of this study: No first-party tests found in source inventory. Runtime assertions are inspected but unexecuted; full plotting/writeup/idea-generation internals were not exhaustively traced. No models/code/GPU ran.

## Actions

### AgentManager.run / ParallelAgent.step

Surface: workflow API.

Input: Idea/config/workspace, execution and step callbacks.

Result: Candidate journals, stage transitions/checkpoints and results.

Lifecycle: Stage/substage loop; process-pool parallel candidates and seed evaluation after completion.

Authority: Operator config/model budgets; not live user-authored turn graph.

Evidence: [e1](#evidence-e1), [e2](#evidence-e2), [e6](#evidence-e6), [e7](#evidence-e7).

### _draft / _debug / _improve / hyperparameter / ablation / seed node

Surface: internal programmable primitives.

Input: Parent node, task stage and generated specific experiment idea.

Result: Node with plan/code, parent identity, measurements and feedback.

Lifecycle: Worker-local generation followed by reset interpreter execution.

Authority: Model controls candidate code; host chooses branch/action and GPU assignment.

Evidence: [e3](#evidence-e3), [e4](#evidence-e4), [e5](#evidence-e5).

### Interpreter.run / cleanup_session

Surface: execution API.

Input: Python source and reset_session boolean.

Result: ExecutionResult(term_out,exec_time,exc_type,exc_info,exc_stack).

Lifecycle: Normal search resets each run; optional reuse has timeout limitation; direct TERM/KILL cleanup.

Authority: Host child OS authority; ready/finished assertions are protocol checks.

Evidence: [e8](#evidence-e8), [e9](#evidence-e9), [e10](#evidence-e10).

### parse_exec_result / metrics parser / paper review

Surface: evaluation primitives.

Input: Code/results, generated numpy data and review FunctionSpec/PDF.

Result: Bug/metric summary and stored review text/image reports.

Lifecycle: Metrics parser is generated and executed, then model interprets values; final review separate from candidate execution.

Authority: Configured evaluator model, no fixed independent correctness oracle.

Evidence: [e3](#evidence-e3), [e16](#evidence-e16), [e17](#evidence-e17).

## Capabilities

### filesystem

**I — Files** (source): Per-worker workspaces save generated program/data/plots; journals and idea/checkpoint files capture candidates. Shared scientific provenance for arbitrary external IO is not supplied.

Evidence: [e1](#evidence-e1), [e4](#evidence-e4), [e8](#evidence-e8), [e11](#evidence-e11), [e13](#evidence-e13).

### processes

**L — OS programs** (source): Process-pool node workers own reset interpreter children; timeout logs at future boundary, direct interpreter cleanup cannot prove whole descendant-tree quiescence and GPU bookkeeping is released even after wait timeout.

Evidence: [e4](#evidence-e4), [e7](#evidence-e7), [e9](#evidence-e9), [e10](#evidence-e10).

### code-actions

**I — Code actions** (source): Model generates complete candidate plan/code; parent determines draft/debug/improve/hyperparameter/ablation action and executes fresh candidate. No model-native arbitrary shell/schema dispatch engine is implied.

Evidence: [e3](#evidence-e3), [e5](#evidence-e5), [e8](#evidence-e8).

### persistent-kernel

**L — Kernel** (source): Interpreter can reuse global_scope with reset_session=False, but actual search calls True then cleanup. Timeout asserts reset mode, so optional interactive namespace is not the normal search or shared standing kernel contract.

Evidence: [e5](#evidence-e5), [e8](#evidence-e8), [e9](#evidence-e9), [e10](#evidence-e10).

### standing-database

**? — Standing DB** (inspection): Not established in the explicitly inspected launcher, manager stage loop/checkpoint, node worker dispatch/code execution/provider adaptation, result/metric parsing, summary and protocol assertions; no universal absence claim.

### workflow-programming

**L — Workflows** (source): Model-generated scientific substages and candidate programs run within fixed stage/search manager, callbacks and budgets. This is research-plan flexibility rather than hot arbitrary turn-program redefinition.

Evidence: [e2](#evidence-e2), [e3](#evidence-e3), [e5](#evidence-e5), [e6](#evidence-e6).

### multi-model

**I — Models** (source): Separate configured code/feedback/summary and paper-review models plus provider adaptation are explicit. These are distinct pipeline roles, not live peer room.

Evidence: [e3](#evidence-e3), [e6](#evidence-e6), [e14](#evidence-e14), [e17](#evidence-e17).

### live-collaboration

**L — Peer chat** (source): Branch workers consume parent/journal summaries and aggregate results; inspected search supplies no addressed live peer messaging.

Evidence: [e4](#evidence-e4), [e6](#evidence-e6), [e11](#evidence-e11).

### concurrent-work

**I — Concurrency** (source): Process-pool futures evaluate candidates concurrently, with local GPU allocation and sequential collection. Model/worker timeout semantics and actual cancellation differ.

Evidence: [e4](#evidence-e4), [e7](#evidence-e7).

### steering-interrupt

**L — Steer/interrupt** (source): Interpreter timeout sends SIGINT then direct process cleanup; future wait timeout only logs in inspected step. No general operator/provider live steering contract is established.

Evidence: [e7](#evidence-e7), [e9](#evidence-e9), [e10](#evidence-e10).

### turn-redefinition

**? — Turn program** (inspection): Not established in the explicitly inspected launcher, manager stage loop/checkpoint, node worker dispatch/code execution/provider adaptation, result/metric parsing, summary and protocol assertions; no universal absence claim.

### compaction

**S — Compaction** (source): Node stdout projection truncates and journal summary supplies reduced scientific history to workers. These are task-memory projections, not raw-archived conversation transformations.

Evidence: [e6](#evidence-e6), [e11](#evidence-e11), [e12](#evidence-e12).

### context-repair

**L — Repair** (source): Debugging repairs experiment code from output/ancestry; no recovery of original omitted/corrupted conversational context is traced.

Evidence: [e3](#evidence-e3), [e5](#evidence-e5), [e11](#evidence-e11).

### original-audit

**L — Original audit** (source): Candidate tree retains code/results/ancestry/UUID/time plus checkpoint and delivered review files. Query wrapper discards provider metadata and stdout is projected/truncated; no complete original wire/OS/action audit.

Evidence: [e11](#evidence-e11), [e12](#evidence-e12), [e13](#evidence-e13), [e14](#evidence-e14), [e15](#evidence-e15).

### audit-query

**S — Audit query** (source): Journal node identities/parents support result lookup/selection and projection to report; no generic immutable audit query service is established.

Evidence: [e6](#evidence-e6), [e7](#evidence-e7), [e11](#evidence-e11).

### hot-change

**L — Hot change** (source): Candidate/plot/metric code is generated and run during search; this changes research programs, not resident harness code or live stage-engine implementation.

Evidence: [e3](#evidence-e3), [e5](#evidence-e5), [e16](#evidence-e16).

### rebuild-continuity

**L — Rebuild continuity** (source): Tree/phase/config checkpoint is partial task state; no active worker/model-stack continuity or harness executable refit is established.

Evidence: [e13](#evidence-e13), [e9](#evidence-e9).

### remote-services

**I — Remote** (source): OpenAI/Anthropic backend requests are consumed with role models; experiment computation is owned local process/GPU execution in inspected main path.

Evidence: [e4](#evidence-e4), [e14](#evidence-e14), [e15](#evidence-e15).

### self-improvement

**S — Self-improve** (source): Draft/debug/improve/hyperparameter/ablation/multi-seed candidate loop is an autoresearch pattern over experiment code. It does not improve its own harness, and metrics/evaluation parsing are model-generated.

Evidence: [e2](#evidence-e2), [e3](#evidence-e3), [e5](#evidence-e5), [e16](#evidence-e16).

### complaints

**? — Complaints** (inspection): Not established in the explicitly inspected launcher, manager stage loop/checkpoint, node worker dispatch/code execution/provider adaptation, result/metric parsing, summary and protocol assertions; no universal absence claim.

### authority

**L — Authority** (source): Generated code executes with worker host OS authority in process workdir; no true isolation or command approval gate occurs in traced interpreter. Function schemas constrain result shape, not code effects.

Evidence: [e4](#evidence-e4), [e8](#evidence-e8), [e15](#evidence-e15).

### evaluation

**L — Evaluation** (source): Actual experiment/metric-parsing programs run, seed evaluation reuses parent parser, and model feedback marks bugs/metrics. This is stronger than pure review prose but evaluator/parser can share model errors; final paper reviewer is artifact assessment, not independent test suite.

Evidence: [e2](#evidence-e2), [e3](#evidence-e3), [e16](#evidence-e16), [e17](#evidence-e17).

### time-order

**L — Time/order** (source): Node UUID/parent/step/time and ready/finished/EOF protocol express local search order. No exact-generation owner ticket or append-only global causal event audit appears.

Evidence: [e7](#evidence-e7), [e8](#evidence-e8), [e10](#evidence-e10), [e11](#evidence-e11).

## Inspected test oracles

- [quarantine/ai-scientist-v2/ai_scientist/treesearch/interpreter.py](../../../quarantine/ai-scientist-v2/ai_scientist/treesearch/interpreter.py): Runtime protocol assertion and unsupported interactive timeout Oracle: These are runtime assertions, not a test suite: ready/finished tuple labels and reset_session-on-timeout are asserted. They catch protocol misuse but no independent process-tree or scientific oracle is supplied; unexecuted. Read, **not executed**.
- [quarantine/ai-scientist-v2/ai_scientist/treesearch/backend/backend_openai.py](../../../quarantine/ai-scientist-v2/ai_scientist/treesearch/backend/backend_openai.py): Structured model response shape drift Oracle: Runtime assertions require function call presence/name and JSON parse. They check shape only, not source truth, metric extraction accuracy or researcher/reviewer independence; unexecuted. Read, **not executed**.

## Useful mechanisms

- Explicit candidate ancestry and separate draft/debug/improve/ablation/seed evaluation stages.
- Measures actual generated-program outputs rather than only grading paper prose.

## Material limits

- Generation and evaluation/parser model errors can correlate; no independent suite found.
- Future timeout is not worker cancellation, direct-child cleanup is not descendant quiescence.
- Search normal path resets kernels; checkpoints do not preserve provider/process continuity.

## Arconaut design questions

- Can autodroit reuse this candidate tree while requiring immutable evaluator/oracle outside the candidate control?
- What exact authority/work identity must be held until timed-out compute actually stops?

## Evidence

### Evidence e1

[quarantine/ai-scientist-v2/launch_scientist_bfts.py:244–263](../../../quarantine/ai-scientist-v2/launch_scientist_bfts.py#L244): Launcher preserves idea JSON, edits experiment config and invokes BFTS search before copying experiment results.

### Evidence e2

[quarantine/ai-scientist-v2/ai_scientist/treesearch/agent_manager.py:692–751](../../../quarantine/ai-scientist-v2/ai_scientist/treesearch/agent_manager.py#L692): Stage/substage loop creates agents, runs steps and transitions based on stage completion, then multi-seed evaluation of best node.

### Evidence e3

[quarantine/ai-scientist-v2/ai_scientist/treesearch/parallel_agent.py:658–714](../../../quarantine/ai-scientist-v2/ai_scientist/treesearch/parallel_agent.py#L658): Model produces plan+code with repair feedback on extraction; result feedback model sees code/output, marks buggy from model flag or actual exception.

### Evidence e4

[quarantine/ai-scientist-v2/ai_scientist/treesearch/parallel_agent.py:1410–1466](../../../quarantine/ai-scientist-v2/ai_scientist/treesearch/parallel_agent.py#L1410): Worker creates process-specific workspace/MinimalAgent/Interpreter and GPU environment assignment.

### Evidence e5

[quarantine/ai-scientist-v2/ai_scientist/treesearch/parallel_agent.py:1490–1527](../../../quarantine/ai-scientist-v2/ai_scientist/treesearch/parallel_agent.py#L1490): Node dispatch selects debug/hyperparameter/ablation/improve, runs generated child code with reset_session=True and cleans interpreter.

### Evidence e6

[quarantine/ai-scientist-v2/ai_scientist/treesearch/parallel_agent.py:2053–2079](../../../quarantine/ai-scientist-v2/ai_scientist/treesearch/parallel_agent.py#L2053): Node selection serializes parents and optionally generates journal memory summary for child prompts.

### Evidence e7

[quarantine/ai-scientist-v2/ai_scientist/treesearch/parallel_agent.py:2124–2190](../../../quarantine/ai-scientist-v2/ai_scientist/treesearch/parallel_agent.py#L2124): Process-pool futures return node records into journal; timeout logs but finally releases local GPU assignment without cancelling future here.

### Evidence e8

[quarantine/ai-scientist-v2/ai_scientist/treesearch/interpreter.py:130–161](../../../quarantine/ai-scientist-v2/ai_scientist/treesearch/interpreter.py#L130): Interpreter child executes code against global_scope and writes ready/finished/EOF, with traceback and KeyboardInterrupt relabeled TimeoutError.

### Evidence e9

[quarantine/ai-scientist-v2/ai_scientist/treesearch/interpreter.py:196–238](../../../quarantine/ai-scientist-v2/ai_scientist/treesearch/interpreter.py#L196): run defaults to reset_session=True; optional False reuses process only after first spawn; cleanup TERM/KILL targets direct child.

### Evidence e10

[quarantine/ai-scientist-v2/ai_scientist/treesearch/interpreter.py:250–293](../../../quarantine/ai-scientist-v2/ai_scientist/treesearch/interpreter.py#L250): Protocol asserts ready/finished states; timeout asserts reset mode, repeatedly sends SIGINT then direct-child cleanup after grace.

### Evidence e11

[quarantine/ai-scientist-v2/ai_scientist/treesearch/journal.py:45–117](../../../quarantine/ai-scientist-v2/ai_scientist/treesearch/journal.py#L45): UUID/time/parent node stores plan/code, execution output/exception/metrics/plots and feedback.

### Evidence e12

[quarantine/ai-scientist-v2/ai_scientist/treesearch/journal.py:159–189](../../../quarantine/ai-scientist-v2/ai_scientist/treesearch/journal.py#L159): Node projection absorbs execution facts; term_out property truncates stored stdout for prompts.

### Evidence e13

[quarantine/ai-scientist-v2/ai_scientist/treesearch/agent_manager.py:249–272](../../../quarantine/ai-scientist-v2/ai_scientist/treesearch/agent_manager.py#L249): Stage checkpoint overwrites pickle containing journals/history/config/current stage; not ongoing process stack.

### Evidence e14

[quarantine/ai-scientist-v2/ai_scientist/treesearch/backend/__init__.py:19–77](../../../quarantine/ai-scientist-v2/ai_scientist/treesearch/backend/__init__.py#L19): Query compiles prompt and picks OpenAI/Anthropic backend/model-specific request options; returns output only.

### Evidence e15

[quarantine/ai-scientist-v2/ai_scientist/treesearch/backend/backend_openai.py:31–88](../../../quarantine/ai-scientist-v2/ai_scientist/treesearch/backend/backend_openai.py#L31): SDK request forces selected function spec; asserts function name/presence then parses JSON, with usage/metadata returned to backend wrapper.

### Evidence e16

[quarantine/ai-scientist-v2/ai_scientist/treesearch/parallel_agent.py:1534–1638](../../../quarantine/ai-scientist-v2/ai_scientist/treesearch/parallel_agent.py#L1534): Model writes metrics parsing code against generated numpy artifacts; that code runs and another structured model response interprets printed metrics.

### Evidence e17

[quarantine/ai-scientist-v2/launch_scientist_bfts.py:304–319](../../../quarantine/ai-scientist-v2/launch_scientist_bfts.py#L304): Optional final paper/text/image review reads PDF and writes reviewer outputs; not a completion gate for earlier experiment executions.

