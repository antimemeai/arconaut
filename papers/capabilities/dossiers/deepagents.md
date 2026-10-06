# deepagents

Programmable graph/middleware agent SDK with backend files/execute, local/forked and remote asynchronous subagents, event-based compaction and recovery paths.

Role: agent framework and SDK. Runtime: Python.

Pinned source: [https://github.com/langchain-ai/deepagents](https://github.com/langchain-ai/deepagents); revision/version `dd0e2f5366da3704585a372aa905734cafcec900`.

DeepAgents SDK assembles LangChain/LangGraph compiled graph and middleware; provider transport/tool-node scheduling external; optional LangGraph-hosted async colleagues

Default state files, owned optional LocalShell; caller-supplied StoreBackend/checkpointer/sandbox and remote colleagues consumed. Talon/Code/ACP are separate repository products.

Inspection: Primary SDK graph, model request wrappers, native actions/backend dispatch, local/fork/hosted child lifecycle, store/memory, automatic/manual compaction, large-result capture, interruption patch and tests.

Limits of this study: Source read only, reference not executed. Unvendored LangChain/LangGraph/provider SDK loop and scheduler not implementation-audited; separate Code/ACP/Talon products and examples not comprehensively traced. Claims scoped to primary SDK. No reference executed.

## Actions

### ls | read_file | glob | grep

Surface: model tools.

Input: absolute paths; offset/limit; glob/literal pattern/path and search output options

Result: directory/files/matching lines or backend errors

Lifecycle: sync/async backend adapter and projection

Authority: backend and optional filesystem permissions

Evidence: [e5](#evidence-e5), [e6](#evidence-e6).

### write_file | edit_file | delete

Surface: model tools.

Input: file_path/content; old/new/replace_all; file_path

Result: updated path/occurrences/error

Lifecycle: backend operation; call-ID paired messages

Authority: rules allow/deny/ask composed by middleware

Evidence: [e5](#evidence-e5), [e7](#evidence-e7), [e8](#evidence-e8), [e3](#evidence-e3).

### execute

Surface: model tool conditional on backend.

Input: shell command, optional timeout

Result: content plus exit_code/truncated artifact, possible capture file pointer

Lifecycle: awaited backend operation; no native persistent process handle

Authority: execution backend controls host/sandbox authority

Evidence: [e9](#evidence-e9), [e10](#evidence-e10).

### task

Surface: model tool.

Input: description/subagent_type from configured specs

Result: last AI text and selected state updates

Lifecycle: await child; isolated or effective-history fork; recursion guard

Authority: child model/tools/permissions supplied or inherited

Evidence: [e2](#evidence-e2), [e12](#evidence-e12), [e13](#evidence-e13).

### start_async_task

Surface: optional model tool.

Input: description/subagent_type mapped to hosted graph

Result: stable task/thread ID, run ID in state

Lifecycle: remote independent run; host caller consumes service

Authority: configured LangGraph deployment/auth

Evidence: [e14](#evidence-e14).

### check_async_task | list_async_tasks

Surface: optional model tools.

Input: task ID or status filter

Result: live status and successful thread result, tracked list

Lifecycle: poll current remote status

Authority: configured hosted client; only tracked tasks

Evidence: [e15](#evidence-e15), [e18](#evidence-e18).

### update_async_task

Surface: optional model tool.

Input: task_id/message

Result: same task ID/new run ID

Lifecycle: interrupt existing remote run and start follow-up on same thread

Authority: model steering of configured hosted colleague

Evidence: [e16](#evidence-e16).

### cancel_async_task

Surface: optional model tool.

Input: task_id

Result: local cancelled state after remote cancel API

Lifecycle: request acceptance not independently verified quiescence

Authority: model can request remote task stop

Evidence: [e17](#evidence-e17).

### compact_conversation

Surface: optional middleware model tool.

Input: no args

Result: summary event and paired confirmation

Lifecycle: explicit summarization middleware must be configured; main graph uses automatic compactor

Authority: model-initiated if caller adds tool middleware

Evidence: [e21](#evidence-e21), [e3](#evidence-e3).

### create_deep_agent / custom AgentMiddleware / CompiledSubAgent

Surface: programmatic API.

Input: model/tools/graph specs/backends/context/state schema/checkpointer/store

Result: compiled graph invoking external LangChain loop

Lifecycle: construction-time composition; mutable request hooks possible

Authority: operator/program authors expressive workflow

Evidence: [e1](#evidence-e1), [e4](#evidence-e4), [e28](#evidence-e28).

### StoreBackend / LocalShellBackend

Surface: supplied backend APIs.

Input: runtime namespace/store or cwd/env/command

Result: persistent files or command response

Lifecycle: cross-thread consumed store versus own local child process

Authority: host chooses storage/execution ownership

Evidence: [e11](#evidence-e11), [e10](#evidence-e10).

### read_file conversation_history paths

Surface: model recovery primitive.

Input: summary-supplied file path

Result: rendered earlier history if backend offload succeeded

Lifecycle: ordinary file read, not automatic restoration of original context

Authority: model may rehydrate selected detail

Evidence: [e20](#evidence-e20), [e7](#evidence-e7).

## Capabilities

### filesystem

**I — Files** (source): Backend-routed native list/read/write/edit/delete/glob/grep with permissions and standard results.

Evidence: [e5](#evidence-e5), [e6](#evidence-e6), [e7](#evidence-e7), [e8](#evidence-e8).

### processes

**L — OS programs** (source): Conditional execute uses backend; LocalShell one subprocess per action, clipped output and lost timeout partials; no stable job lifecycle.

Evidence: [e9](#evidence-e9), [e10](#evidence-e10).

### code-actions

**I — Code actions** (source): Command execution plus native file tools; exact authority and execution supplied backend.

Evidence: [e9](#evidence-e9), [e10](#evidence-e10), [e7](#evidence-e7).

### persistent-kernel

**? — Kernel** (source): No resident interpreter in primary SDK/LocalShell paths; external backend may supply services but not traced.

### standing-database

**S — Standing DB** (source): StoreBackend consumes supplied BaseStore with runtime namespaces persistent across threads; native file abstraction, not model SQL/query API.

Evidence: [e11](#evidence-e11).

### workflow-programming

**I — Workflows** (source): Program authors compose graph/middleware/runnables, custom tool and per-child specs; loop delegated to LangChain/LangGraph.

Evidence: [e1](#evidence-e1), [e2](#evidence-e2), [e4](#evidence-e4), [e28](#evidence-e28).

### multi-model

**I — Models** (source): Each declarative subagent may specify distinct model/profile and inherits defaults otherwise.

Evidence: [e2](#evidence-e2).

### live-collaboration

**L — Peer chat** (source): Hosted async tasks allow model check/update/cancel while running; parent-to-child interrupting follow-up, no peer-to-peer IRC bus traced. Local task awaited.

Evidence: [e14](#evidence-e14), [e15](#evidence-e15), [e16](#evidence-e16), [e17](#evidence-e17), [e12](#evidence-e12).

### concurrent-work

**I — Concurrency** (source): Hosted runs execute independently with stable thread/task and run identity; local graph concurrency depends on unvendored tool scheduler.

Evidence: [e14](#evidence-e14), [e16](#evidence-e16), [e4](#evidence-e4).

### steering-interrupt

**L — Steer/interrupt** (source): Remote update interrupt strategy and cancel API implemented; local cancelled status is not observed termination. Host HITL optional.

Evidence: [e16](#evidence-e16), [e17](#evidence-e17), [e3](#evidence-e3).

### turn-redefinition

**S — Turn program** (source): Middleware can alter request/response and graph built from supplied programs; model editing does not hot-install new turn graph.

Evidence: [e28](#evidence-e28), [e19](#evidence-e19), [e4](#evidence-e4).

### compaction

**I — Compaction** (source): Automatic summary events select effective history while retaining message channel; argument truncation, overflow recovery and optional model compact tool.

Evidence: [e19](#evidence-e19), [e21](#evidence-e21), [e25](#evidence-e25), [e26](#evidence-e26).

### context-repair

**L — Repair** (source): Model can reopen rendered offloaded conversation path; unmatched tool calls repaired. Offload nonfatal failures/media placeholders and pre-state tool eviction bound original recoverability.

Evidence: [e20](#evidence-e20), [e22](#evidence-e22), [e24](#evidence-e24), [e19](#evidence-e19).

### original-audit

**L — Original audit** (source): Message channel preservation through summary is real but not raw provider audit; pre-state result eviction, backend clipping, rendered history, optional checkpoints/callbacks.

Evidence: [e19](#evidence-e19), [e20](#evidence-e20), [e22](#evidence-e22), [e10](#evidence-e10), [e4](#evidence-e4).

### audit-query

**S — Audit query** (source): Native read_file enables offloaded rendered-history retrieval; checkpoint/store query semantics depend on host.

Evidence: [e20](#evidence-e20), [e11](#evidence-e11).

### hot-change

**L — Hot change** (source): Graph/middleware composition construction-time; memory caches initial loaded files. No default whole-turn/workflow activation transaction.

Evidence: [e23](#evidence-e23), [e28](#evidence-e28).

### rebuild-continuity

**S — Rebuild continuity** (source): Supplied checkpointer/store and external thread IDs can reconstitute graph data, but no quiescent executable handoff with owned process/provider custody.

Evidence: [e1](#evidence-e1), [e4](#evidence-e4), [e14](#evidence-e14).

### remote-services

**I — Remote** (source): Configurable model clients and hosted async deployments plus supplied storage/sandbox backends consumed.

Evidence: [e2](#evidence-e2), [e4](#evidence-e4), [e11](#evidence-e11), [e14](#evidence-e14).

### self-improvement

**S — Self-improve** (source): Model can edit memory/program files, framework can compose evaluation hooks, but no governed harness autoresearch loop in primary paths.

Evidence: [e7](#evidence-e7), [e23](#evidence-e23), [e1](#evidence-e1).

### complaints

**? — Complaints** (source): No model state-capturing complaint operation in inspected SDK native action set.

### authority

**I — Authority** (source): Filesystem permissions and optional HITL merge; local shell explicitly full host authority, supplied backend decides isolation.

Evidence: [e3](#evidence-e3), [e7](#evidence-e7), [e9](#evidence-e9), [e10](#evidence-e10).

### evaluation

**I — Evaluation** (source): Strong mocked compaction budgets/reduced-retry/request-immutability and remote ID/strategy tests; no live remote custody verified.

Evidence: [e25](#evidence-e25), [e26](#evidence-e26), [e27](#evidence-e27).

### time-order

**L — Time/order** (source): Summary cutoff/message identities, graph checkpoint channels and remote run IDs structure history; remote cancel and backend audit timing not total-order custody.

Evidence: [e19](#evidence-e19), [e14](#evidence-e14), [e16](#evidence-e16), [e17](#evidence-e17).

## Inspected test oracles

- [quarantine/deepagents/libs/deepagents/tests/unit_tests/middleware/test_compaction_recovery.py](../../../quarantine/deepagents/libs/deepagents/tests/unit_tests/middleware/test_compaction_recovery.py): Overflow reduction and persistence Oracle: Handler enforces budget, full bytes offloaded, original request unchanged, retry strictly smaller and bounded; mock counter/backend do not prove full vendor fidelity. Read, **not executed**.
- [quarantine/deepagents/libs/deepagents/tests/unit_tests/test_async_subagents.py](../../../quarantine/deepagents/libs/deepagents/tests/unit_tests/test_async_subagents.py): Stable colleague identity and update strategy Oracle: Mock asserts same thread/task, new run, multitask_strategy interrupt; remote resource termination/races remain external. Read, **not executed**.

## Useful mechanisms

- Compaction as an event/projection rather than destructive rewriting of every original message.
- Consumed persistent store and hosted work boundaries fit Arconaut consumer intent better than agent-owned shared services.
- Model-accessible hosted work has explicit IDs and follow-up/cancel operations.

## Material limits

- Unvendored LangChain/LangGraph/provider SDK loop and scheduler not implementation-audited; separate Code/ACP/Talon products and examples not comprehensively traced. Claims scoped to primary SDK. No reference executed.

## Arconaut design questions

- Can capture be independent of model projection, backend clipping and nonfatal offload failures?
- Can restart preserve caller task IDs while proving own activity quiescent and leaving shared deployments running?
- How should a model install altered workflows at the end of affected workflow rather than only constructing a new graph?

## Evidence

### Evidence e1

[quarantine/deepagents/libs/deepagents/deepagents/graph.py:272–292](../../../quarantine/deepagents/libs/deepagents/deepagents/graph.py#L272): create_deep_agent configurable model/tools/middleware/subagents/backend/checkpointer/store returns graph.

### Evidence e2

[quarantine/deepagents/libs/deepagents/deepagents/graph.py:639–700](../../../quarantine/deepagents/libs/deepagents/deepagents/graph.py#L639): Default StateBackend and per-child model/permission/default middleware resolution.

### Evidence e3

[quarantine/deepagents/libs/deepagents/deepagents/graph.py:863–923](../../../quarantine/deepagents/libs/deepagents/deepagents/graph.py#L863): Main filesystem/task/summarization/patch middleware and optional hosted async subagents, memory and HITL.

### Evidence e4

[quarantine/deepagents/libs/deepagents/deepagents/graph.py:961–983](../../../quarantine/deepagents/libs/deepagents/deepagents/graph.py#L961): LangChain create_agent delegated loop/provider dispatch, supplied stores/checkpointer and graph state; dependencies not vendored.

### Evidence e5

[quarantine/deepagents/libs/deepagents/deepagents/middleware/filesystem.py:1238–1345](../../../quarantine/deepagents/libs/deepagents/deepagents/middleware/filesystem.py#L1238): Native file/search schema inputs including offsets, unique edits, deletion, literal grep.

### Evidence e6

[quarantine/deepagents/libs/deepagents/deepagents/middleware/filesystem.py:1890–1906](../../../quarantine/deepagents/libs/deepagents/deepagents/middleware/filesystem.py#L1890): Built-in file and execute tool factories.

### Evidence e7

[quarantine/deepagents/libs/deepagents/deepagents/middleware/filesystem.py:2177–2218](../../../quarantine/deepagents/libs/deepagents/deepagents/middleware/filesystem.py#L2177): Write path validation/permissions and backend delegation with ToolMessage.

### Evidence e8

[quarantine/deepagents/libs/deepagents/deepagents/middleware/filesystem.py:2268–2312](../../../quarantine/deepagents/libs/deepagents/deepagents/middleware/filesystem.py#L2268): Edit backend API and replace_all; results/errors paired to call ID.

### Evidence e9

[quarantine/deepagents/libs/deepagents/deepagents/middleware/filesystem.py:3001–3095](../../../quarantine/deepagents/libs/deepagents/deepagents/middleware/filesystem.py#L3001): Execute capability check/timeouts, optional offloaded capture or backend execute; artifact exit status with message success even nonzero command.

### Evidence e10

[quarantine/deepagents/libs/deepagents/deepagents/backends/local_shell.py:298–360](../../../quarantine/deepagents/libs/deepagents/deepagents/backends/local_shell.py#L298): LocalShell fresh subprocess, explicit cwd/env, timeout, stdout/stderr combine then clip; timeout discards partial output.

### Evidence e11

[quarantine/deepagents/libs/deepagents/deepagents/backends/store.py:91–150](../../../quarantine/deepagents/libs/deepagents/deepagents/backends/store.py#L91): Persistent StoreBackend consumes supplied or execution-context BaseStore, caller namespaces across threads.

### Evidence e12

[quarantine/deepagents/libs/deepagents/deepagents/middleware/subagents.py:772–876](../../../quarantine/deepagents/libs/deepagents/deepagents/middleware/subagents.py#L772): Task isolated/fork contexts, awaited runnable invocation, recursion refusal and standardized return.

### Evidence e13

[quarantine/deepagents/libs/deepagents/deepagents/middleware/subagents.py:742–755](../../../quarantine/deepagents/libs/deepagents/deepagents/middleware/subagents.py#L742): Child final nonempty AI text projected to parent plus state updates, not complete child transcript.

### Evidence e14

[quarantine/deepagents/libs/deepagents/deepagents/middleware/async_subagents.py:245–339](../../../quarantine/deepagents/libs/deepagents/deepagents/middleware/async_subagents.py#L245): Hosted start creates thread/run and task record only after successful calls; partial-start cleanup not present.

### Evidence e15

[quarantine/deepagents/libs/deepagents/deepagents/middleware/async_subagents.py:407–473](../../../quarantine/deepagents/libs/deepagents/deepagents/middleware/async_subagents.py#L407): Check remote run, success thread values and paired status update.

### Evidence e16

[quarantine/deepagents/libs/deepagents/deepagents/middleware/async_subagents.py:482–567](../../../quarantine/deepagents/libs/deepagents/deepagents/middleware/async_subagents.py#L482): Update same task/thread, new run with multitask_strategy interrupt; stable task ID.

### Evidence e17

[quarantine/deepagents/libs/deepagents/deepagents/middleware/async_subagents.py:580–654](../../../quarantine/deepagents/libs/deepagents/deepagents/middleware/async_subagents.py#L580): Cancel remote request then local cancelled status; remote termination not acknowledged by observation.

### Evidence e18

[quarantine/deepagents/libs/deepagents/deepagents/middleware/async_subagents.py:729–804](../../../quarantine/deepagents/libs/deepagents/deepagents/middleware/async_subagents.py#L729): List tracked tasks including refreshed nonterminal status and filtering.

### Evidence e19

[quarantine/deepagents/libs/deepagents/deepagents/middleware/summarization.py:1487–1619](../../../quarantine/deepagents/libs/deepagents/deepagents/middleware/summarization.py#L1487): Effective history/argument truncation/summary event updates without destructive summary rewrite; one reduced overflow retry.

### Evidence e20

[quarantine/deepagents/libs/deepagents/deepagents/middleware/summarization.py:1227–1302](../../../quarantine/deepagents/libs/deepagents/deepagents/middleware/summarization.py#L1227): Offload rendered history to backend, warn/fail None nonfatal; not original wire audit.

### Evidence e21

[quarantine/deepagents/libs/deepagents/deepagents/middleware/summarization.py:2017–2100](../../../quarantine/deepagents/libs/deepagents/deepagents/middleware/summarization.py#L2017): Optional compact_conversation tool emits summary event, not automatically added in main graph.

### Evidence e22

[quarantine/deepagents/libs/deepagents/deepagents/middleware/filesystem.py:3591–3637](../../../quarantine/deepagents/libs/deepagents/deepagents/middleware/filesystem.py#L3591): Large tool content is processed/offloaded before addition to state; raw-state preservation has this boundary.

### Evidence e23

[quarantine/deepagents/libs/deepagents/deepagents/middleware/memory.py:279–311](../../../quarantine/deepagents/libs/deepagents/deepagents/middleware/memory.py#L279): Memory files read once when memory_contents absent, cached in state.

### Evidence e24

[quarantine/deepagents/libs/deepagents/deepagents/middleware/patch_tool_calls.py:33–52](../../../quarantine/deepagents/libs/deepagents/deepagents/middleware/patch_tool_calls.py#L33): Adds paired error results for unmatched/invalid calls and rewrites message list; cancellation factual status uncertain.

### Evidence e25

[quarantine/deepagents/libs/deepagents/tests/unit_tests/middleware/test_compaction_recovery.py:60–78](../../../quarantine/deepagents/libs/deepagents/tests/unit_tests/middleware/test_compaction_recovery.py#L60): Mocked input budget, offloaded full result bytes and original request unchanged oracle.

### Evidence e26

[quarantine/deepagents/libs/deepagents/tests/unit_tests/middleware/test_compaction_recovery.py:114–163](../../../quarantine/deepagents/libs/deepagents/tests/unit_tests/middleware/test_compaction_recovery.py#L114): Overflow retries smaller once; no-cutoff unchanged retry prohibited.

### Evidence e27

[quarantine/deepagents/libs/deepagents/tests/unit_tests/test_async_subagents.py:370–424](../../../quarantine/deepagents/libs/deepagents/tests/unit_tests/test_async_subagents.py#L370): Mock remote update asserts same task ID/new run/interruption strategy; no actual remote concurrency/custody.

### Evidence e28

[quarantine/deepagents/libs/deepagents/deepagents/graph.py:205–239](../../../quarantine/deepagents/libs/deepagents/deepagents/graph.py#L205): Name-based custom middleware replacement at graph construction, not runtime hot replacement.

