# trae-agent

Tool-driven coding benchmark/CLI harness with configurable native tools, normalized trajectories and optional code index; completion interception and ownership gaps matter.

Role: coding agent. Runtime: Python.

Pinned source: [https://github.com/bytedance/trae-agent](https://github.com/bytedance/trae-agent); revision/version `e839e559ac61bdd0e057c375dd1dee391fee797d`.

Asyncio step orchestration with synchronous provider SDK requests; optional local shared Bash or Docker tool routing

Owns shell/processes, optional Docker, local code index; consumes provider/MCP services.

Inspection: Entry interactive loop, BaseAgent/Trae dispatch/results/completion, concrete OpenAI request/history, Bash/editor/JSON/thought/CKG/MCP lifecycles, Docker and recorder/test oracles.

Limits of this study: Source read only, reference not executed. Source only; no SDK/Docker/MCP dependency internals executed. Local Bash state claim contradicted by subshell. Cancellation cleanup and mixed completion batches require discriminating experiments.

## Actions

### bash

Surface: model tool.

Input: command and restart

Result: stdout/stderr/error code, restart text

Lifecycle: single owned shell; commands subshell, timeout needs restart; no job IDs

Authority: automatic local OS/Docker authority

Evidence: [e6](#evidence-e6), [e7](#evidence-e7), [e8](#evidence-e8).

### str_replace_based_edit_tool

Surface: model tool.

Input: command=view/create/str_replace/insert, absolute path, range/text/line

Result: numbered content/snippet/errors

Lifecycle: awaited edit; unique replacement; no undo

Authority: no approval; absolute path check not workspace containment

Evidence: [e9](#evidence-e9), [e10](#evidence-e10).

### json_edit_tool

Surface: model tool.

Input: operation=view/set/add/remove,file_path,json_path,value,pretty_print

Result: JSON selected content/update counts/error

Lifecycle: awaited file read/write

Authority: automatic files

Evidence: [e11](#evidence-e11).

### sequentialthinking

Surface: model tool.

Input: thought,next_thought_needed,thought_number,total_thoughts and revision/branch fields

Result: thought count/status/branches

Lifecycle: in-memory thought store; not running branch work

Authority: model records own thoughts

Evidence: [e12](#evidence-e12).

### task_done

Surface: model tool/completion sentinel.

Input: no args

Result: agent final_result and optional nonempty patch check

Lifecycle: completion detected before executing entire returned batch

Authority: model ends task; not behavioral verification

Evidence: [e2](#evidence-e2), [e3](#evidence-e3).

### ckg

Surface: optional model tool.

Input: command=search_function/search_class/search_class_method,path,identifier,print_body

Result: indexed bodies,file/line locations

Lifecycle: SQLite index cached by tool and snapshot hash; global expiry

Authority: configured tool, owns index lifecycle

Evidence: [e13](#evidence-e13), [e14](#evidence-e14), [e15](#evidence-e15).

### MCP discovered tool names

Surface: model extension.

Input: configured allow-listed server and native tool schema

Result: first text result only/error

Lifecycle: async remote call, cleanup after task

Authority: operator supplies allowed servers; no per-call approvals traced

Evidence: [e16](#evidence-e16), [e17](#evidence-e17).

### new_task / execute_task / set_chat_history

Surface: host API.

Input: task,project/issue/base_commit/must_patch/patch_path; messages

Result: AgentExecution and steps; explicit history setter

Lifecycle: fixed step loop; provider history remains unless host resets

Authority: host subclasses/configures agent

Evidence: [e1](#evidence-e1), [e26](#evidence-e26), [e27](#evidence-e27), [e19](#evidence-e19).

### ToolExecutor.parallel_tool_call / sequential_tool_call

Surface: host API.

Input: ToolCall list

Result: paired ToolResult list

Lifecycle: gather locally; serialized Docker; shared Bash not invocation-locked

Authority: host config parallel flag

Evidence: [e4](#evidence-e4), [e5](#evidence-e5), [e7](#evidence-e7), [e18](#evidence-e18).

### run / interactive

Surface: operator CLI.

Input: task/config/provider/model/project/step limits

Result: CLI updates and trajectory/patch file

Lifecycle: tasks awaited then next input; Ctrl-C

Authority: operator

Evidence: [e23](#evidence-e23), [e1](#evidence-e1).

### TrajectoryRecorder / get_trajectory_path

Surface: host API.

Input: normalized interactions/steps and path

Result: rewritten JSON trajectory

Lifecycle: incremental snapshot, write errors warning only

Authority: host/operator reads; not model query API

Evidence: [e21](#evidence-e21), [e22](#evidence-e22).

### CKGDatabase.update

Surface: host primitive.

Input: codebase directory

Result: reconstructed local index

Lifecycle: local owned SQLite; not consumer fabric service

Authority: host

Evidence: [e15](#evidence-e15).

## Capabilities

### filesystem

**I — Files** (source): Native text/JSONPath editing plus Bash; absolute paths not contained to workspace.

Evidence: [e9](#evidence-e9), [e10](#evidence-e10), [e11](#evidence-e11).

### processes

**L — OS programs** (source): Local Bash shell has restart/timeout/close but commands subshell; no native job handles and no shared-session concurrency lock.

Evidence: [e6](#evidence-e6), [e7](#evidence-e7), [e8](#evidence-e8).

### code-actions

**I — Code actions** (source): Arbitrary Bash execution plus precise file/JSONPath operations and index search.

Evidence: [e7](#evidence-e7), [e9](#evidence-e9), [e11](#evidence-e11), [e13](#evidence-e13).

### persistent-kernel

**L — Kernel** (source): Shell process persists, but per-command subshell loses shell mutations; no resident language kernel traced.

Evidence: [e7](#evidence-e7), [e8](#evidence-e8).

### standing-database

**L — Standing DB** (source): Model-facing ckg is local cached/expiring code index, not shared general standing DB.

Evidence: [e13](#evidence-e13), [e14](#evidence-e14), [e15](#evidence-e15).

### workflow-programming

**S — Workflows** (source): Subclass BaseAgent and supply Tool/config; native thought branches only record metadata, not workflow programs.

Evidence: [e26](#evidence-e26), [e12](#evidence-e12), [e1](#evidence-e1).

### multi-model

**S — Models** (source): Selected provider/model config per agent; host can instantiate different agents, no peer coordination.

Evidence: [e26](#evidence-e26), [e19](#evidence-e19).

### live-collaboration

**? — Peer chat** (source): No peer agent bus/message protocol in inspected loop/registry.

### concurrent-work

**L — Concurrency** (source): Local gather for tool batch; Docker serializes. Shared Bash process has no call serialization.

Evidence: [e4](#evidence-e4), [e5](#evidence-e5), [e7](#evidence-e7), [e18](#evidence-e18).

### steering-interrupt

**L — Steer/interrupt** (source): Interactive input only between awaited tasks; synchronous provider blocks event loop. CancelledError can bypass post-finally tool/MCP cleanup.

Evidence: [e23](#evidence-e23), [e19](#evidence-e19), [e1](#evidence-e1).

### turn-redefinition

**S — Turn program** (source): Host subclass hooks fixed step architecture; no model hot turn replacement mechanism.

Evidence: [e1](#evidence-e1), [e2](#evidence-e2), [e26](#evidence-e26).

### compaction

**? — Compaction** (source): No managed compactor in inspected BaseAgent/OpenAI accumulated history.

### context-repair

**S — Repair** (source): Explicit set_chat_history and trajectory reads give host history replacement; no model repair operation.

Evidence: [e19](#evidence-e19), [e20](#evidence-e20), [e22](#evidence-e22).

### original-audit

**L — Original audit** (source): Trajectory captures normalized completed replies and delta inputs, not full accumulated provider request or raw retries/events. Rewrites can fail silently.

Evidence: [e19](#evidence-e19), [e20](#evidence-e20), [e21](#evidence-e21), [e22](#evidence-e22).

### audit-query

**S — Audit query** (source): JSON trajectory and recorder path host read; no native model audit query.

Evidence: [e22](#evidence-e22).

### hot-change

**? — Hot change** (source): Captured config/tool map and no atomic after-turn/workflow activation in inspected paths.

### rebuild-continuity

**? — Rebuild continuity** (source): No outpost transfer or quiescent executable re-inhabitation established.

### remote-services

**I — Remote** (source): Provider and allowed MCP clients, optional owned Docker deployment.

Evidence: [e16](#evidence-e16), [e19](#evidence-e19), [e26](#evidence-e26).

### self-improvement

**S — Self-improve** (source): Agent can edit/code/test through tools, but no controlled harness improvement experiment.

Evidence: [e7](#evidence-e7), [e9](#evidence-e9).

### complaints

**? — Complaints** (source): No model state-capturing complaint artifact traced.

### authority

**I — Authority** (source): Automatic configured tools local or Docker; allow-listed MCP discovery. No routine command confirmation traced.

Evidence: [e26](#evidence-e26), [e16](#evidence-e16), [e5](#evidence-e5).

### evaluation

**L — Evaluation** (source): Source tests exercise shell outputs and nonempty patch acceptance; completion not task correctness, mixed batch untested.

Evidence: [e24](#evidence-e24), [e25](#evidence-e25), [e3](#evidence-e3).

### time-order

**L — Time/order** (source): Step numbers/order and shell deadlines; gather has shared resource races; wall timestamps and mutable JSON not total audit.

Evidence: [e1](#evidence-e1), [e4](#evidence-e4), [e7](#evidence-e7), [e21](#evidence-e21), [e22](#evidence-e22).

## Inspected test oracles

- [quarantine/trae-agent/tests/tools/test_bash_tool.py](../../../quarantine/trae-agent/tests/tools/test_bash_tool.py): Echo, invalid command and explicit shell restart Oracle: Checks concrete output/nonzero error/new echo; does not assert cd/env persistence, deadline cleanup or concurrent call isolation. Read, **not executed**.
- [quarantine/trae-agent/tests/agent/test_trae_agent.py](../../../quarantine/trae-agent/tests/agent/test_trae_agent.py): Patch filtering/completion Oracle: Empty test-only patch rejected, mocked nonempty patch accepted; no semantic correctness or combined task_done/tool call oracle. Read, **not executed**.

## Useful mechanisms

- Explicit normalized action/result IDs across provider implementations.
- JSONPath editor and cached symbol index add expressive targeted actions.

## Material limits

- Source only; no SDK/Docker/MCP dependency internals executed. Local Bash state claim contradicted by subshell. Cancellation cleanup and mixed completion batches require discriminating experiments.

## Arconaut design questions

- Should completion acknowledge every co-issued action or forbid mixed completion batches?
- Can the harness prove owned processes are stopped after async cancellation, including descendants?
- What distinguishes a persistent shell process from persistent shell state and a persistent compute service?

## Evidence

### Evidence e1

[quarantine/trae-agent/trae_agent/agent/base_agent.py:147–200](../../../quarantine/trae-agent/trae_agent/agent/base_agent.py#L147): Async task loop calls synchronous provider; docker stop finally but tool/MCP cleanup after finally can be bypassed by cancellation.

### Evidence e2

[quarantine/trae-agent/trae_agent/agent/base_agent.py:209–244](../../../quarantine/trae-agent/trae_agent/agent/base_agent.py#L209): Completion checked before dispatch; finalize overrides error state to completed.

### Evidence e3

[quarantine/trae-agent/trae_agent/agent/trae_agent.py:228–249](../../../quarantine/trae-agent/trae_agent/agent/trae_agent.py#L228): Any task_done call denotes completion, must_patch checks non-test patch only, not correctness.

### Evidence e4

[quarantine/trae-agent/trae_agent/agent/base_agent.py:314–352](../../../quarantine/trae-agent/trae_agent/agent/base_agent.py#L314): Config parallel option dispatches gather versus sequential then paired results.

### Evidence e5

[quarantine/trae-agent/trae_agent/tools/base.py:182–244](../../../quarantine/trae-agent/trae_agent/tools/base.py#L182): Normalized cached name map executes tool with standardized result/error and asyncio.gather.

### Evidence e6

[quarantine/trae-agent/trae_agent/tools/bash_tool.py:19–85](../../../quarantine/trae-agent/trae_agent/tools/bash_tool.py#L19): Owned persistent Bash shell; direct-process terminate/kill+communicate deadlines, not process-group kill.

### Evidence e7

[quarantine/trae-agent/trae_agent/tools/bash_tool.py:87–159](../../../quarantine/trae-agent/trae_agent/tools/bash_tool.py#L87): Each command wrapped in subshell; private buffers polled until sentinel, timeout marks unusable without immediate kill; no invocation lock.

### Evidence e8

[quarantine/trae-agent/trae_agent/tools/bash_tool.py:177–246](../../../quarantine/trae-agent/trae_agent/tools/bash_tool.py#L177): bash schema command/restart, description promises state persistence and raw background shell usage.

### Evidence e9

[quarantine/trae-agent/trae_agent/tools/edit_tool.py:101–151](../../../quarantine/trae-agent/trae_agent/tools/edit_tool.py#L101): Text editor dispatch view/create/str_replace/insert with absolute path and existence checks.

### Evidence e10

[quarantine/trae-agent/trae_agent/tools/edit_tool.py:197–290](../../../quarantine/trae-agent/trae_agent/tools/edit_tool.py#L197): Unique replacement and insert write files; undo explicitly removed.

### Evidence e11

[quarantine/trae-agent/trae_agent/tools/json_edit_tool.py:56–153](../../../quarantine/trae-agent/trae_agent/tools/json_edit_tool.py#L56): JSONPath view/set/add/remove with absolute file_path and value/pretty-print.

### Evidence e12

[quarantine/trae-agent/trae_agent/tools/sequential_thinking_tool.py:278–319](../../../quarantine/trae-agent/trae_agent/tools/sequential_thinking_tool.py#L278): Thought/revision/branch records held in memory, returns counts and branch IDs; not executable workflow.

### Evidence e13

[quarantine/trae-agent/trae_agent/tools/ckg_tool.py:50–133](../../../quarantine/trae-agent/trae_agent/tools/ckg_tool.py#L50): Optional ckg tool searches function/class/class_method cached code index.

### Evidence e14

[quarantine/trae-agent/trae_agent/tools/ckg/ckg_database.py:107–119](../../../quarantine/trae-agent/trae_agent/tools/ckg/ckg_database.py#L107): Old code-index DB files deleted after expiry.

### Evidence e15

[quarantine/trae-agent/trae_agent/tools/ckg/ckg_database.py:148–203](../../../quarantine/trae-agent/trae_agent/tools/ckg/ckg_database.py#L148): Snapshot-hash SQLite index owned locally; updates reconstruct; no general standing data API.

### Evidence e16

[quarantine/trae-agent/trae_agent/agent/trae_agent.py:65–99](../../../quarantine/trae-agent/trae_agent/agent/trae_agent.py#L65): Allowed MCP server discovery extends model tool list; failures/cancellations cleaned and skipped.

### Evidence e17

[quarantine/trae-agent/trae_agent/tools/mcp_tool.py:48–58](../../../quarantine/trae-agent/trae_agent/tools/mcp_tool.py#L48): MCP projects first text content only; isError path lacks nonzero error_code.

### Evidence e18

[quarantine/trae-agent/trae_agent/tools/docker_tool_executor.py:70–75](../../../quarantine/trae-agent/trae_agent/tools/docker_tool_executor.py#L70): Docker parallel execution deliberately sequential.

### Evidence e19

[quarantine/trae-agent/trae_agent/utils/llm_clients/openai_compatible_base.py:94–147](../../../quarantine/trae-agent/trae_agent/utils/llm_clients/openai_compatible_base.py#L94): Synchronous provider call uses accumulated message_history, retries, current schemas.

### Evidence e20

[quarantine/trae-agent/trae_agent/utils/llm_clients/openai_compatible_base.py:149–215](../../../quarantine/trae-agent/trae_agent/utils/llm_clients/openai_compatible_base.py#L149): Normalized response and accumulated assistant history; trajectory recorder sees passed delta messages, not full transport history.

### Evidence e21

[quarantine/trae-agent/trae_agent/utils/trajectory_recorder.py:77–128](../../../quarantine/trae-agent/trae_agent/utils/trajectory_recorder.py#L77): Normalized response/content/usage/calls plus passed messages recorded after successful response.

### Evidence e22

[quarantine/trae-agent/trae_agent/utils/trajectory_recorder.py:220–260](../../../quarantine/trae-agent/trae_agent/utils/trajectory_recorder.py#L220): Trajectory rewritten in place; save failures swallowed; serialized tool results omit name.

### Evidence e23

[quarantine/trae-agent/trae_agent/cli.py:516–592](../../../quarantine/trae-agent/trae_agent/cli.py#L516): Simple interactive loop runs tasks then accepts next input; KeyboardInterrupt handling, not live steering.

### Evidence e24

[quarantine/trae-agent/tests/tools/test_bash_tool.py:28–59](../../../quarantine/trae-agent/tests/tools/test_bash_tool.py#L28): Actual shell tests check echo/error/restart, no cd/variable persistence or parallel sharing oracle.

### Evidence e25

[quarantine/trae-agent/tests/agent/test_trae_agent.py:78–98](../../../quarantine/trae-agent/tests/agent/test_trae_agent.py#L78): Patch filtering and mocked nonempty completion oracle; no task_done mixed batch assertion.

### Evidence e26

[quarantine/trae-agent/trae_agent/agent/base_agent.py:29–81](../../../quarantine/trae-agent/trae_agent/agent/base_agent.py#L29): Configuration selects tool classes and local/Docker caller; initial creation clears old CKG.

### Evidence e27

[quarantine/trae-agent/trae_agent/agent/trae_agent.py:102–153](../../../quarantine/trae-agent/trae_agent/agent/trae_agent.py#L102): New task clears task messages but provider history reset not here; tool_names only applied if tool list empty.

### Evidence e28

[quarantine/trae-agent/trae_agent/tools/__init__.py:27–34](../../../quarantine/trae-agent/trae_agent/tools/__init__.py#L27): Native registry bash, text/json editors, thought, task_done, optional ckg.

