# gptme

A useful combination of executable Python, runtime hook registration, continuing shell jobs, multi-model subagents and separately stored compacted views. Interpreter isolation and event custody have important limits.

Role: programmable Python coding/operator harness. Runtime: Python.

Pinned source: [https://github.com/gptme/gptme](https://github.com/gptme/gptme); revision/version `1f8d73638a25253fd0e1824c3d0a72ec93007582`.

Synchronous CLI model/tool loop; threaded and subprocess children; process-global IPython; context-local shell/hooks/log handles

Owns its interpreter, shell jobs and child lifecycle; consumes model providers. Shared external computational service leases and executable refit were not established.

Inspection: Read CLI loop/provider hook wiring, actual save/patch writes, Python namespace and output capture, shell promotion/interruption, child configuration/control/steering, hook registry, compaction view publication, transcript/recovery writes and direct source-test oracles.

Limits of this study: No acquired code executed. Server immediate-persistence path, every adapter, MCP/ACP transport, all compaction strategies and evaluation packages not exhaustively inspected.

## Actions

### save / append / patch

Surface: model tool.

Input: file path and model-authored content/edit blocks

Result: changed files, preview and error/result messages

Lifecycle: synchronous actual writes with pre/post hooks; no multi-operation atomic transaction

Authority: model tool under confirmation hooks

Evidence: [save](#evidence-save), [patch](#evidence-patch).

### python / register_function / register_hook

Surface: model tool / programmable host API.

Input: Python program; callable helper or lifecycle callback

Result: cell streams/value/messages, persistent variables and later hook effects

Lifecycle: in-process continuing singleton; hook snapshot per invocation; async hooks detached daemon threads

Authority: model executable code can import exposed host APIs; configured confirmation

Evidence: [python](#evidence-python), [cell](#evidence-cell), [capture](#evidence-capture), [registry](#evidence-registry), [registry-context](#evidence-registry-context).

### shell / output / wait / background completion

Surface: model tool.

Input: OS command, job ID and optional wait/timeout

Result: stdout/stderr/exit or ongoing promoted job handle

Lifecycle: soft timeout transfers original shell/worker; successor shell permits more work; kill request is distinct from settlement

Authority: model tool with configured policy

Evidence: [promotion](#evidence-promotion), [interrupt](#evidence-interrupt), [shell-test](#evidence-shell-test).

### subagent / subagent_wait / subagent_cancel / subagent_steer

Surface: model helper / orchestration API.

Input: child ID, prompt, model/profile/context selection; steer text

Result: child log/results/progress or cooperative cancellation acknowledgement

Lifecycle: thread/subprocess child; queue read at checkpoint; ACP weaker; cancellation marks cache before effects stop

Authority: orchestrator/model calls; child profile restricts loaded tools

Evidence: [child](#evidence-child), [cancel](#evidence-cancel), [steer](#evidence-steer), [checkpoint](#evidence-checkpoint).

### compact / create_view / switch_view / switch_to_master

Surface: context tool / programmable host API.

Input: context/log and budget/strategy; view name

Result: projected view plus retained main history and output references

Lifecycle: check pending calls, evaluate savings, publish view; dual-write following messages; reload current view

Authority: model/tool and programmable log/context mechanisms

Evidence: [pending](#evidence-pending), [publish](#evidence-publish), [views](#evidence-views), [reference](#evidence-reference), [dual-write](#evidence-dual-write).

### append / write(sync=True) / recover_messages

Surface: persistence primitive.

Input: message/file attachment; conversation and recovery log

Result: disk transcript/view, content hashes, sequence event/checkpoint or recovered state

Lifecycle: append; atomic rewrites and optional sync; checkpoint prunes old recovery mutations

Authority: host persistence API; normal file/program access can consume retained data

Evidence: [jsonl](#evidence-jsonl), [dual-write](#evidence-dual-write), [eventlog](#evidence-eventlog), [checkpoint-prune](#evidence-checkpoint-prune), [recovery-test](#evidence-recovery-test).

## Capabilities

### filesystem

**I — Files** (source): Model save/append/patch operations write actual files and expose hooks; relative path check plus intentional absolute paths, no transaction across tool calls.

Evidence: [save](#evidence-save), [patch](#evidence-patch).

### processes

**I — OS programs** (source): Continuing shell commands can be transferred to background job handles without rerunning them; cancellation escalation helper lacks final settlement proof.

Evidence: [promotion](#evidence-promotion), [interrupt](#evidence-interrupt), [shell-test](#evidence-shell-test).

### code-actions

**I — Code actions** (source): Model-authored Python executes arbitrary code, imports host APIs and uses registered helpers in a continuing interpreter. Fresh-container sandbox is a different execution contract.

Evidence: [python](#evidence-python), [cell](#evidence-cell).

### persistent-kernel

**L — Kernel** (source): IPython namespace persists but singleton and stdout/stderr capture are process-global. Context-local child registries do not establish per-agent kernel/output isolation or shared external-service ownership.

Evidence: [python](#evidence-python), [capture](#evidence-capture), [child](#evidence-child), [python-test](#evidence-python-test).

### standing-database

**? — Standing DB** (inspection scope): standing_database: not established beyond the inspected CLI/Python/shell/subagent/hook/history surfaces; no universal absence inferred.

### workflow-programming

**S — Workflows** (source): Executable Python can register functions and lifecycle hooks; native synchronous CLI loop remains host code, not a fully traced replaceable orchestration program.

Evidence: [python](#evidence-python), [registry](#evidence-registry), [loop](#evidence-loop).

### multi-model

**I — Models** (source): Selected main model and separately configured child model/profile/context allow different model choices. Provider choice alone does not establish live peer room.

Evidence: [step](#evidence-step), [child](#evidence-child).

### live-collaboration

**L — Peer chat** (source): Parent can steer active thread/subprocess colleague and receive results/progress. Inspected channel is orchestrator-to-child; ACP cannot accept steer and no general peer-addressed room traced.

Evidence: [steer](#evidence-steer), [checkpoint](#evidence-checkpoint), [cancel](#evidence-cancel).

### concurrent-work

**L — Concurrency** (source): Background shell and child threads/subprocesses run concurrently with handles and eventual results. Python singleton/capture complicate parallel Python; cache completion/cancellation is weaker than worker settlement.

Evidence: [promotion](#evidence-promotion), [child](#evidence-child), [cancel](#evidence-cancel), [capture](#evidence-capture).

### steering-interrupt

**L — Steer/interrupt** (source): Actual queued mid-turn steering consumed at STEP_PRE. Thread/ACP cancellation may return while work continues; subprocess path waits for root only. Shell SIGTERM escalation has no final wait here.

Evidence: [steer](#evidence-steer), [checkpoint](#evidence-checkpoint), [cancel](#evidence-cancel), [interrupt](#evidence-interrupt).

### turn-redefinition

**S — Turn program** (source): Runtime hooks affect step boundaries, generation context, tool use and continuation; same-process Python gives agency to call registry APIs. General arbitrary replacement of loop/turn scheduler not established.

Evidence: [loop](#evidence-loop), [generation](#evidence-generation), [tool](#evidence-tool), [registry-context](#evidence-registry-context), [cell](#evidence-cell).

### compaction

**I — Compaction** (source): Pending-call pairing gate; token/savings-driven projected trimming and separate compacted views preserve main messages. Multiple strategy internals remain outside inspection.

Evidence: [pending](#evidence-pending), [compact](#evidence-compact), [publish](#evidence-publish), [dual-write](#evidence-dual-write).

### context-repair

**I — Repair** (source): Can switch back to master, reload view plus original messages and retrieve indexed byte ranges for trimmed output. This is retained message repair, not restoration of events never captured.

Evidence: [views](#evidence-views), [reference](#evidence-reference), [view-test](#evidence-view-test), [reload-test](#evidence-reload-test).

### original-audit

**L — Original audit** (source): Message/file snapshots and recovery events are useful. CLI buffers entire step until tool completion; direct Python Message return omits captured streams; recovery checkpoints prune older events. Neither full raw provider/program IO nor all attempts retained by these paths.

Evidence: [step](#evidence-step), [cell](#evidence-cell), [eventlog](#evidence-eventlog), [checkpoint-prune](#evidence-checkpoint-prune), [dual-write](#evidence-dual-write).

### audit-query

**S — Audit query** (source): Master byte references, file-backed logs and switch-to-master expose retained messages for program/model file access. A unified query API over original provider/effect events is not established.

Evidence: [reference](#evidence-reference), [views](#evidence-views), [jsonl](#evidence-jsonl).

### hot-change

**S — Hot change** (source): Callable registry edits affect later hook invocation snapshots and Python helper state can change live. No default turn/workflow completion activation transaction or compiled refit handoff traced.

Evidence: [registry](#evidence-registry), [registry-context](#evidence-registry-context), [python](#evidence-python), [loop](#evidence-loop).

### rebuild-continuity

**L — Rebuild continuity** (source): Saved conversation and selected context view reload are concrete. Background job/child/interpreter preservation through executable replacement and independently authenticated outpost activation not established.

Evidence: [reload-test](#evidence-reload-test), [python](#evidence-python), [promotion](#evidence-promotion).

### remote-services

**? — Remote** (inspection scope): remote_services: not established beyond the inspected CLI/Python/shell/subagent/hook/history surfaces; no universal absence inferred.

### self-improvement

**? — Self-improve** (inspection scope): self_improvement: not established beyond the inspected CLI/Python/shell/subagent/hook/history surfaces; no universal absence inferred.

### complaints

**? — Complaints** (inspection scope): complaints: not established beyond the inspected CLI/Python/shell/subagent/hook/history surfaces; no universal absence inferred.

### authority

**L — Authority** (source): Autonomous TOOL_CONFIRM hook permits normal tool execution and model Python can modify host functions/hooks. Additional direct confirm paths, e.g. missing parent directory, exist; exact deployment policy depends on configured hooks.

Evidence: [confirm](#evidence-confirm), [cell](#evidence-cell), [registry-context](#evidence-registry-context), [save](#evidence-save).

### evaluation

**S — Evaluation** (source): Useful source oracles attack actual promotion liveness/output, child-parent state, view/master disk persistence, reload and transcript corruption recovery; no harness improvement promotion gate inspected.

Evidence: [shell-test](#evidence-shell-test), [child-test](#evidence-child-test), [view-test](#evidence-view-test), [reload-test](#evidence-reload-test), [recovery-test](#evidence-recovery-test), [hook-test](#evidence-hook-test).

### time-order

**L — Time/order** (source): Foreground budget uses monotonic elapsed time and recovery events sequence mutations. Sentinel freshness and cooldowns use wall-clock time; root/process result status is not causal effect settlement.

Evidence: [promotion](#evidence-promotion), [steer](#evidence-steer), [eventlog](#evidence-eventlog), [cancel](#evidence-cancel).

## Inspected test oracles

- [quarantine/gptme/tests/test_shell_foreground_promotion.py](../../../quarantine/gptme/tests/test_shell_foreground_promotion.py): Actual command ownership after soft deadline Oracle: Checks prompt return under one second, continuing job liveness, later original before/after output and exit zero. Does not prove kill settles descendants or restart continuity. Read, **not executed**.
- [quarantine/gptme/tests/test_subagent_parent_isolation.py](../../../quarantine/gptme/tests/test_subagent_parent_isolation.py): Child does not corrupt parent selected tool/hook state Oracle: Offline mock/echo drives real child chat/complete and subsequent real parent shell call. Does not test singleton IPython variables or concurrent global stdout capture. Read, **not executed**.
- [quarantine/gptme/tests/test_logmanager.py](../../../quarantine/gptme/tests/test_logmanager.py): Independent on-disk original retention and restored view Oracle: Read persisted main/view after append, fresh-load selected view and original master, switch-to-master reload. Does not restore uncaptured provider attempts or interpreter state. Read, **not executed**.
- [quarantine/gptme/tests/test_eventlog.py](../../../quarantine/gptme/tests/test_eventlog.py): Recovery when main transcript removed and explicit checkpoint pruning Oracle: Deletes primary file and compares literal recovered content; checkpoint expects exactly latest checkpoint plus five events, proving state recovery rather than permanent event history. Read, **not executed**.
- [quarantine/gptme/tests/test_tools_python.py](../../../quarantine/gptme/tests/test_tools_python.py): Persistent variables and captured ordinary result output Oracle: Literal prints/values and reused variable assertions; no simultaneous cell isolation or direct Message-return captured-output oracle. Read, **not executed**.
- [quarantine/gptme/tests/test_hooks_registry.py](../../../quarantine/gptme/tests/test_hooks_registry.py): Normal callback failure versus session-end signal Oracle: Failing hook still allows later hook; session-complete exception must propagate. No transaction/rollback of callback effects or async settlement oracle. Read, **not executed**.

## Useful mechanisms

- Executable Python plus callable hook registry is a concrete route to normal model agency.
- Separate master and compacted view with dual-write and reload supports context repair.
- Shell promotion transfers the running computation instead of restarting it, with a direct liveness/output oracle.

## Material limits

- Process-global IPython and sys.stdout capture leave parallel-agent state/output isolation unestablished.
- Immediate cancelled result cache is not worker/provider/effect settlement.
- CLI step buffering and checkpoint pruning limit original audit custody; no executable refit protocol traced.

## Arconaut design questions

- Expose editable decision/workflow programs directly while keeping effect custody in a smaller host boundary.
- Preserve separate original history and mutable context views, including failed transformations and repair provenance.
- Test simultaneous Python clients with distinct variable/output witnesses, continuing child work after cancellation, and executable replacement with no active host-owned effects.

## Evidence

### Evidence loop

[quarantine/gptme/gptme/chat.py:550–666](../../../quarantine/gptme/gptme/chat.py#L550): STEP_PRE, buffered step generation/tool outputs, append after step, continuation after mid-turn compaction, TURN_POST and final sync.

### Evidence step

[quarantine/gptme/gptme/chat.py:896–982](../../../quarantine/gptme/gptme/chat.py#L896): Selected model/tools and overflow recovery; post-generation hook results only debug-logged; CLI buffers tool effects before persisting assistant/results.

### Evidence generation

[quarantine/gptme/gptme/llm/__init__.py:377–414](../../../quarantine/gptme/gptme/llm/__init__.py#L377): GENERATION_PRE receives messages and yields extra generation context before selected provider call; provider-origin exception tagging follows hooks.

### Evidence registry

[quarantine/gptme/gptme/hooks/registry.py:190–319](../../../quarantine/gptme/gptme/hooks/registry.py#L190): Snapshot enabled hooks per invocation; asynchronous hooks use unjoined daemon threads; synchronous failures log and continue except session-complete signal.

### Evidence registry-context

[quarantine/gptme/gptme/hooks/registry.py:428–440](../../../quarantine/gptme/gptme/hooks/registry.py#L428): Registry is context-local and runtime set/get/registration APIs are callable.

### Evidence confirm

[quarantine/gptme/gptme/hooks/auto_confirm.py:21–44](../../../quarantine/gptme/gptme/hooks/auto_confirm.py#L21): Autonomous hook returns confirmed execution and registers as TOOL_CONFIRM.

### Evidence tool

[quarantine/gptme/gptme/tools/base.py:926–1046](../../../quarantine/gptme/gptme/tools/base.py#L926): Mutable ToolExecutePreData contains actual ToolUse; native-call generator results buffer and stamp only final result with call ID; current tool ContextVar encloses execution.

### Evidence python

[quarantine/gptme/gptme/tools/python.py:1–235](../../../quarantine/gptme/gptme/tools/python.py#L1): Module-global IPython singleton and host helper registry; functions pushed into the continuing interpreter.

### Evidence capture

[quarantine/gptme/gptme/tools/python.py:260–269](../../../quarantine/gptme/gptme/tools/python.py#L260): Capture replaces process-global sys.stdout and stderr then restores previous handles.

### Evidence cell

[quarantine/gptme/gptme/tools/python.py:271–409](../../../quarantine/gptme/gptme/tools/python.py#L271): Default run_cell persists namespace; isolated sandbox is distinct path; direct Message/list result returns early before captured streams are emitted; plot artifacts detected.

### Evidence save

[quarantine/gptme/gptme/tools/save.py:122–205](../../../quarantine/gptme/gptme/tools/save.py#L122): Actual file writes, relative-path confinement but intentional absolute paths, pre/post hooks and conditional parent directory creation.

### Evidence patch

[quarantine/gptme/gptme/tools/patch.py:322–379](../../../quarantine/gptme/gptme/tools/patch.py#L322): Read file, apply model-authored patch, write changed bytes and return success/error.

### Evidence promotion

[quarantine/gptme/gptme/tools/shell.py:2897–3060](../../../quarantine/gptme/gptme/tools/shell.py#L2897): POSIX soft foreground budget transfers the actual running shell/worker into a background job, creates replacement shell and asynchronously publishes eventual completion.

### Evidence interrupt

[quarantine/gptme/gptme/tools/shell.py:3266–3294](../../../quarantine/gptme/gptme/tools/shell.py#L3266): SIGINT group and root wait; escalation sends SIGTERM without a final wait/death proof in this helper.

### Evidence child

[quarantine/gptme/gptme/tools/subagent/execution.py:315–470](../../../quarantine/gptme/gptme/tools/subagent/execution.py#L315): Child thread initializes separate tools/hook registry and selected model/profile/context subset; these scoped registries do not make module-global Python interpreter per-child.

### Evidence cancel

[quarantine/gptme/gptme/tools/subagent/api.py:1206–1287](../../../quarantine/gptme/gptme/tools/subagent/api.py#L1206): Thread cancellation marks result immediately and requests next checkpoint; subprocess root terminate/wait then kill; ACP cache-mark-only branch has no cooperative checkpoint despite returned prose.

### Evidence steer

[quarantine/gptme/gptme/tools/subagent/api.py:1292–1431](../../../quarantine/gptme/gptme/tools/subagent/api.py#L1292): Live thread/subprocess steering writes prompt queue, rejects closed or exited recipient before/after; sentinel freshness compares wall-clock mtime/start; ACP steering rejected.

### Evidence checkpoint

[quarantine/gptme/gptme/tools/subagent/hooks.py:43–118](../../../quarantine/gptme/gptme/tools/subagent/hooks.py#L43): STEP_PRE drains cancellation and mid-turn steer; cancellation ends chat at cooperative boundary, steer becomes user input before next model step.

### Evidence jsonl

[quarantine/gptme/gptme/logmanager/manager.py:158–208](../../../quarantine/gptme/gptme/logmanager/manager.py#L158): Transcript appends or atomic rewrites through symlink with temp fsync, replacement and directory sync; no original provider wire event ledger.

### Evidence dual-write

[quarantine/gptme/gptme/logmanager/manager.py:542–672](../../../quarantine/gptme/gptme/logmanager/manager.py#L542): On compacted view, new messages append to both main and view; file attachment snapshots by hash; sync barrier available.

### Evidence views

[quarantine/gptme/gptme/logmanager/manager.py:875–934](../../../quarantine/gptme/gptme/logmanager/manager.py#L875): Creates separate view, saves prior state before switch, returns to master and persists selected view marker.

### Evidence compact

[quarantine/gptme/gptme/tools/autocompact/engine.py:332–404](../../../quarantine/gptme/gptme/tools/autocompact/engine.py#L332): Compaction computes projected copies and master byte-range index; original master remains available in this architecture.

### Evidence reference

[quarantine/gptme/gptme/tools/autocompact/engine.py:540–569](../../../quarantine/gptme/gptme/tools/autocompact/engine.py#L540): Truncated result is replaced in projection and receives indexed master byte-range reference when available.

### Evidence publish

[quarantine/gptme/gptme/tools/autocompact/hook.py:296–329](../../../quarantine/gptme/gptme/tools/autocompact/hook.py#L296): After actual savings check accepted trim creates/switches view; preserves main, retains failure latch rather than falsely resetting retries.

### Evidence pending

[quarantine/gptme/gptme/tools/autocompact/hook.py:57–112](../../../quarantine/gptme/gptme/tools/autocompact/hook.py#L57): Pending tool calls block compaction using call-ID matching or markdown result count; UI notices do not satisfy tool-result coverage.

### Evidence eventlog

[quarantine/gptme/gptme/logmanager/eventlog.py:47–122](../../../quarantine/gptme/gptme/logmanager/eventlog.py#L47): Recovery events cover appended messages, checkpoint, undo and edits with sequence and wall timestamp.

### Evidence checkpoint-prune

[quarantine/gptme/gptme/logmanager/eventlog.py:148–180](../../../quarantine/gptme/gptme/logmanager/eventlog.py#L148): Recovery log compaction removes all events before latest state checkpoint; recoverable current state is not complete event history.

### Evidence view-test

[quarantine/gptme/tests/test_logmanager.py:463–501](../../../quarantine/gptme/tests/test_logmanager.py#L463): Literal disk oracle retains main originals after compaction view/new input while view has summary/new input.

### Evidence reload-test

[quarantine/gptme/tests/test_logmanager.py:577–603](../../../quarantine/gptme/tests/test_logmanager.py#L577): Fresh LogManager reload retains selected compacted view and original master; switching back survives another load.

### Evidence child-test

[quarantine/gptme/tests/test_subagent_parent_isolation.py:1–97](../../../quarantine/gptme/tests/test_subagent_parent_isolation.py#L1): Offline echo child and real subsequent parent shell call assert parent tool format/hooks/cwd survive; does not test global Python/stdout isolation.

### Evidence shell-test

[quarantine/gptme/tests/test_shell_foreground_promotion.py:60–94](../../../quarantine/gptme/tests/test_shell_foreground_promotion.py#L60): Actual short-budget shell is still running after promotion, then completion contains before/after and zero exit; independent liveness/output assertions.

### Evidence recovery-test

[quarantine/gptme/tests/test_eventlog.py:563–600](../../../quarantine/gptme/tests/test_eventlog.py#L563): Delete primary transcript and recover exact messages from events; checkpoint oracle explicitly expects prior events removed.

### Evidence python-test

[quarantine/gptme/tests/test_tools_python.py:31–69](../../../quarantine/gptme/tests/test_tools_python.py#L31): Literal cell outputs, namespace persistence and print/value combination; no concurrent namespace/capture isolation oracle.

### Evidence hook-test

[quarantine/gptme/tests/test_hooks_registry.py:547–592](../../../quarantine/gptme/tests/test_hooks_registry.py#L547): Failing normal hook does not stop another hook; session-complete exception is deliberately propagated.

