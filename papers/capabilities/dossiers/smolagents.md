# smolagents

Python code is a native agent action combining tools, managed agents and arbitrary authorized computation across retained executor variables.

Role: code-action/tool-calling agent framework. Runtime: Python.

Pinned source: [https://github.com/huggingface/smolagents](https://github.com/huggingface/smolagents); revision/version `c30b115286e000e98711fae5e85993547b73d826`.

Synchronous multi-step generators, provider adapters, local AST interpreter state, threaded tool calls and optional remote Jupyter executors.

Owns Python agent memory/state and default local or created remote executor; consumes model/tool/MCP dependencies. Docker executor creates/cleans its kernel/container, rather than leasing an existing shared kernel.

Inspection: MultiStepAgent run/finalize, CodeAgent/tool dispatch, local timeout/state, Docker kernel ownership, memory/export and source oracles

Limits of this study: No downloaded code/tests run; no full provider/MCP or isolation conformance assessment. AST restrictions are not treated as a security sandbox.

## Actions

### run / interrupt

Surface: operator/program agent API.

Input: task/images/additional_args/reset/max_steps/stream

Result: final answer or RunResult, streamed steps

Lifecycle: interrupt between steps; reset chat/monitor but preserves executor state

Authority: caller owns model/tool/executor configuration

Evidence: [e1](#evidence-e1), [e2](#evidence-e2).

### python_interpreter / final_answer

Surface: native model code action.

Input: parsed Python program with tool/agent calls and state variables

Result: CodeOutput/output/logs/final-answer signal or AgentExecutionError

Lifecycle: stateful per executor; logs/output capped; no atomic state rollback on failure

Authority: authorized imports/functions/tools; local AST interpreter not security sandbox

Evidence: [e3](#evidence-e3), [e6](#evidence-e6), [e7](#evidence-e7).

### execute_tool_call / process_tool_calls

Surface: model tool/managed-agent dispatcher.

Input: tool name/arguments possibly state reference

Result: typed native value/observation/error; media stored in agent state

Lifecycle: parallel future completion; retained order ID-sorted

Authority: known tools/agents, argument validation/sanitize native I/O

Evidence: [e4](#evidence-e4), [e5](#evidence-e5).

### managed_agent.__call__

Surface: model callable delegated agent.

Input: task and kwargs

Result: rendered report plus optional truncated summary

Lifecycle: synchronous agent run; separate model permitted; remote executor disallows

Authority: configured registered agent name, no peer room protocol

Evidence: [e5](#evidence-e5), [e9](#evidence-e9), [e13](#evidence-e13).

### LocalPythonExecutor.send_variables / send_tools / __call__

Surface: supplied code executor API.

Input: variables/tools and Python string

Result: CodeOutput with final flag/log/output

Lifecycle: resident state survives calls until object destroyed; timeout not hard kill

Authority: host functions/import whitelist configurable including broad authority

Evidence: [e6](#evidence-e6), [e7](#evidence-e7), [e8](#evidence-e8).

### DockerExecutor.run_code_raise_errors / cleanup

Surface: supplied remote executor lifecycle.

Input: Python code or cleanup

Result: Jupyter outputs/error; kernel/container identity

Lifecycle: creates own kernel, per-call websocket; cleanup stop/remove

Authority: operator config/host Docker authority; not shared-kernel consumer lease

Evidence: [e9](#evidence-e9), [e10](#evidence-e10).

### step_callbacks / final_answer_checks / get_full_steps / save

Surface: program customization/evaluation/inspection/export.

Input: callback/check functions, agent state or output directory

Result: mutations/check verdicts, step dicts, agent/tool code/config artifacts

Lifecycle: finalize each step, checks before finish; export ignores callbacks/checks, no heap checkpoint

Authority: operator-supplied host callbacks and final policy

Evidence: [e2](#evidence-e2), [e12](#evidence-e12), [e13](#evidence-e13), [e14](#evidence-e14), [e15](#evidence-e15).

## Capabilities

### filesystem

**S — Files** (source): Authorized imports/local tool callables can expose host filesystem; remote code uses created container kernel. No default general coding filesystem tool inferred.

Evidence: [e5](#evidence-e5), [e6](#evidence-e6), [e10](#evidence-e10).

### processes

**S — OS programs** (source): Authorized host functions/imports or remote kernel can start programs; only executor container ownership/cleanup traced, no native durable child registry.

Evidence: [e6](#evidence-e6), [e9](#evidence-e9), [e10](#evidence-e10).

### code-actions

**I — Code actions** (source): Native CodeAgent python_interpreter action parses Python and composes tool/managed-agent calls, variables and final_answer; JSON tool agent separate.

Evidence: [e3](#evidence-e3), [e5](#evidence-e5), [e6](#evidence-e6).

### persistent-kernel

**L — Kernel** (source): AST state persists across steps and conversation reset does not clear executor state; created remote Jupyter kernel persists until cleanup. No restart/shared multi-consumer lease or heap restore.

Evidence: [e1](#evidence-e1), [e6](#evidence-e6), [e9](#evidence-e9), [e10](#evidence-e10).

### standing-database

**S — Standing DB** (source): Resident memory/variables and pluggable tools can access DB; no general standing database supplied by these paths.

Evidence: [e5](#evidence-e5), [e6](#evidence-e6), [e12](#evidence-e12).

### workflow-programming

**I — Workflows** (source): Model Python code programs control flow and tools/managed agents; per-step callbacks and optional planning/retry/final-answer checks program the agent.

Evidence: [e2](#evidence-e2), [e3](#evidence-e3), [e5](#evidence-e5), [e13](#evidence-e13), [e15](#evidence-e15).

### multi-model

**I — Models** (source): Managed agent objects can use separate models and are callable as tools; remote-code path excludes managed agents.

Evidence: [e5](#evidence-e5), [e9](#evidence-e9), [e13](#evidence-e13).

### live-collaboration

**L — Peer chat** (source): Managed-agent task/report is synchronous delegation, with bounded report summaries; selected implementation no peer mail/room steering. Remote-code delegation explicitly restricted.

Evidence: [e5](#evidence-e5), [e9](#evidence-e9), [e13](#evidence-e13).

### concurrent-work

**I — Concurrency** (source): JSON tools/managed calls ThreadPoolExecutor runs independent calls, completion-order yields; no transaction settlement across external effects.

Evidence: [e4](#evidence-e4), [e5](#evidence-e5).

### steering-interrupt

**L — Steer/interrupt** (source): Interrupt switch checked between steps; timeout future cannot kill running Python thread and executor context waits shutdown. Tests do not prove deadline return or external effect stop.

Evidence: [e2](#evidence-e2), [e8](#evidence-e8), [e16](#evidence-e16).

### turn-redefinition

**S — Turn program** (source): Step callbacks receive mutable agent at finalize; planning/check policies and Python model actions configurable. No before-provider transform/hot turn-definition protocol traced.

Evidence: [e2](#evidence-e2), [e3](#evidence-e3), [e15](#evidence-e15).

### compaction

**L — Compaction** (source): summary_mode projects messages omitting model/planning content and managed reports truncate; not managed compaction event with archived context lineage.

Evidence: [e11](#evidence-e11), [e12](#evidence-e12), [e13](#evidence-e13).

### context-repair

**S — Repair** (source): Mutable memory full steps and callbacks can reconstruct/change model context; reset clears stored steps and no corruption-repair protocol.

Evidence: [e2](#evidence-e2), [e12](#evidence-e12), [e15](#evidence-e15).

### original-audit

**L — Original audit** (source): ActionStep includes model inputs/outputs/code and full-step export, but logs/output already truncated and reset deletes memory; model_output may be augmented closing tag, not untouched raw response.

Evidence: [e3](#evidence-e3), [e7](#evidence-e7), [e11](#evidence-e11), [e12](#evidence-e12).

### audit-query

**S — Audit query** (source): get_full_steps/get_succinct_steps and replay/return_full_code expose structured per-run inspection; not persistent everything-query DB.

Evidence: [e11](#evidence-e11), [e12](#evidence-e12).

### hot-change

**S — Hot change** (source): Python tool/agent dictionaries and finalize callback supply ordinary mutable machinery; executor tools sent at each run. No atomic live deployment/reload protocol established.

Evidence: [e1](#evidence-e1), [e5](#evidence-e5), [e15](#evidence-e15).

### rebuild-continuity

**L — Rebuild continuity** (source): Save/from_dict exports agent code/configuration; callbacks/checks explicitly ignored and resident memory/executor heap omitted. No outpost refit/active work drain.

Evidence: [e9](#evidence-e9), [e13](#evidence-e13), [e14](#evidence-e14).

### remote-services

**I — Remote** (source): Remote executor choices and provider/native tool objects consume services; Docker/Jupyter lifecycle is agent-owned here.

Evidence: [e3](#evidence-e3), [e5](#evidence-e5), [e9](#evidence-e9), [e10](#evidence-e10).

### self-improvement

**S — Self-improve** (source): Code actions/callbacks/checks/export are ingredients for experiments, not a wired candidate harness evaluation/adoption loop.

Evidence: [e2](#evidence-e2), [e3](#evidence-e3), [e13](#evidence-e13), [e14](#evidence-e14), [e15](#evidence-e15).

### complaints

**S — Complaints** (source): AgentError plus code/input/output/timing/step IDs provide diagnostics; no standing complaint table/state snapshot filing.

Evidence: [e2](#evidence-e2), [e11](#evidence-e11).

### authority

**L — Authority** (source): Allowed tools/imports/functions and argument validation; local executor explicitly not security sandbox, remote executor restrictions different. No routine approval pipeline seen in native code path.

Evidence: [e5](#evidence-e5), [e6](#evidence-e6), [e9](#evidence-e9).

### evaluation

**I — Evaluation** (source): Final-answer checks can inspect memory/agent state and force retry; timeout/AST/reset tests actual numerical/error/step assertions but no semantic validity of arbitrary check.

Evidence: [e2](#evidence-e2), [e16](#evidence-e16).

### time-order

**L — Time/order** (source): Step timings/numbers and tool IDs; threaded effects execute completion order while persisted observations sorted IDs; no durable effect ordering/settlement.

Evidence: [e2](#evidence-e2), [e4](#evidence-e4), [e11](#evidence-e11).

## Inspected test oracles

- [quarantine/smolagents/tests/test_local_python_executor.py](../../../quarantine/smolagents/tests/test_local_python_executor.py): AST state/tool protection and threaded timeout mechanics Oracle: Exact state/result/error tests plus timeout/thread join assertions. Timeout tests require eventual exception and thread completion, but do not measure prompt deadline nor prove killed worker/effects; source uses waiting executor context. Read, **not executed**.
- [quarantine/smolagents/tests/test_agents.py](../../../quarantine/smolagents/tests/test_agents.py): chat reset and final-answer validation retry Oracle: Fake models produce exact number 7.2904, assert memory step counts 3/5/3 with reset, rejecting check diagnostics and accepted final step count. Does not prove reset clears interpreter variables or arbitrary answer correctness. Read, **not executed**.

## Useful mechanisms

- Code combines tool calls/control flow/variables natively instead of one JSON action per operation.
- ActionStep retains rich request/code/result structure and checks/callbacks have actual agent access.
- Interpreter and remote kernel boundaries visible and local non-sandbox claim explicit.

## Material limits

- Thread future timeout is not a kill boundary and context-manager shutdown may delay exception until worker completion; tests lack hard latency/effect oracle.
- Conversation reset does not reset executor state; exported agent code/config does not checkpoint heap or callback policy.
- Owned remote kernels/cleanup and managed-agent restrictions differ from shared fabric consumers.

## Arconaut design questions

- How should logical turn reset, model-context reset and external computation-state reset be independent named operations?
- Can code actions retain uncapped original output and exact source/generated transforms while offering bounded model views?
- What executable refit barrier proves worker settlement beyond timeout exceptions?

## Evidence

### Evidence e1

[quarantine/smolagents/src/smolagents/agents.py:436–500](../../../quarantine/smolagents/src/smolagents/agents.py#L436): Run retains state/additional args and passes tools/variables to executor; reset clears conversation/monitor, not local executor state.

### Evidence e2

[quarantine/smolagents/src/smolagents/agents.py:540–625](../../../quarantine/smolagents/src/smolagents/agents.py#L540): Step loop checks interrupt only before step, optional planner; error/finalize callback then append; final-answer callbacks assert and agent retries on failure.

### Evidence e3

[quarantine/smolagents/src/smolagents/agents.py:1647–1754](../../../quarantine/smolagents/src/smolagents/agents.py#L1647): CodeAgent records model input/output, parses/fixes code action, invokes python_interpreter, records logs and truncated output observation.

### Evidence e4

[quarantine/smolagents/src/smolagents/agents.py:1361–1439](../../../quarantine/smolagents/src/smolagents/agents.py#L1361): JSON tools including managed agents execute in thread pool; futures yield completion order then retained calls/observations sorted by ID.

### Evidence e5

[quarantine/smolagents/src/smolagents/agents.py:1444–1502](../../../quarantine/smolagents/src/smolagents/agents.py#L1444): Tool-name validation, state-reference argument substitution, validation and sanitized native or managed-agent call; errors converted into agent execution diagnostics.

### Evidence e6

[quarantine/smolagents/src/smolagents/local_python_executor.py:1688–1768](../../../quarantine/smolagents/src/smolagents/local_python_executor.py#L1688): Local executor keeps state/custom tools/import permissions across calls, explicitly not security sandbox, configured print cap and timeout.

### Evidence e7

[quarantine/smolagents/src/smolagents/local_python_executor.py:1637–1669](../../../quarantine/smolagents/src/smolagents/local_python_executor.py#L1637): AST operations mutate live state; print capture truncated before return/errors; timeout wrapper around execution without transactional rollback.

### Evidence e8

[quarantine/smolagents/src/smolagents/local_python_executor.py:285–319](../../../quarantine/smolagents/src/smolagents/local_python_executor.py#L285): Timeout uses future.result inside ThreadPoolExecutor context; running thread cannot be force-killed. Context manager exit waits worker by ordinary executor semantics, so timeout exception alone does not establish prompt hard stop.

### Evidence e9

[quarantine/smolagents/src/smolagents/agents.py:1593–1620](../../../quarantine/smolagents/src/smolagents/agents.py#L1593): Executor selectable local/blaxel/e2b/modal/docker; cleanup called; managed agents explicitly unsupported with remote code execution.

### Evidence e10

[quarantine/smolagents/src/smolagents/remote_executors.py:623–704](../../../quarantine/smolagents/src/smolagents/remote_executors.py#L623): Docker executor builds/starts own container, new authenticated Jupyter kernel and websocket calls; cleanup stops/removes container.

### Evidence e11

[quarantine/smolagents/src/smolagents/memory.py:53–101](../../../quarantine/smolagents/src/smolagents/memory.py#L53): ActionStep stores inputs/output/code/tool calls/error/timing/state projection; serialization sanitizes values and images raw bytes, not universal transport/effect archive.

### Evidence e12

[quarantine/smolagents/src/smolagents/memory.py:214–250](../../../quarantine/smolagents/src/smolagents/memory.py#L214): Memory is resident step list, reset deletes it; full/succinct exported steps and pretty replay are inspection, not replaying effects.

### Evidence e13

[quarantine/smolagents/src/smolagents/agents.py:868–912](../../../quarantine/smolagents/src/smolagents/agents.py#L868): Managed agent renders task/report and optional bounded work summary; save exports tools/agent/prompts/UI/requirements code artifacts.

### Evidence e14

[quarantine/smolagents/src/smolagents/agents.py:970–1009](../../../quarantine/smolagents/src/smolagents/agents.py#L970): to_dict serializes configuration/tool code/provider configuration and ignores callbacks/final-answer checks; not live memory/kernel checkpoint.

### Evidence e15

[quarantine/smolagents/src/smolagents/agents.py:416–434](../../../quarantine/smolagents/src/smolagents/agents.py#L416): Step callback registry accepts per-step class callbacks; caller receives agent at finalize for normal-operation mutation.

### Evidence e16

[quarantine/smolagents/tests/test_local_python_executor.py:2180–2255](../../../quarantine/smolagents/tests/test_local_python_executor.py#L2180): Timeout tests assert eventual error and completion/thread finished within generous bound, not deadline latency/no post-timeout side effects.

