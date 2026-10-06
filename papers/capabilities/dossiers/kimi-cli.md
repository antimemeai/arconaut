# kimi-cli

Python coding harness with programmable prompt-flow graphs, schema tools, root-managed background jobs and truly restored subagents, rotating compaction history and wire events.

Role: coding agent. Runtime: Python.

Pinned source: [https://github.com/MoonshotAI/kimi-cli](https://github.com/MoonshotAI/kimi-cli); revision/version `9ab1286b8fe4e6bcd116949a27ce5e0ac3389c82`.

Asyncio KimiSoul with in-repo Kosong streaming/tool scheduler and KAOS backend; background Bash detached worker, background agents in current event loop

Owns local commands/task workers/context files; consumes model/MCP/KAOS services. Persisted task spec is not shared compute control plane.

Inspection: Entry run/cancel wiring, soul phases, actual Kosong generation and Kimi provider SDK, tool hooks/dispatch/results, foreground/background custody, child restore/model/role guards, compaction/rotation, wire logger, model reload, feedback and tests.

Limits of this study: Source read only, reference not executed. Alternative provider/remote KAOS transports, all media/search/question executors, ACP/web UI, flow parser and full hook scheduler not exhaustively traced. Source only, no reference run.

## Actions

### ReadFile / ReadMediaFile / Glob / Grep

Surface: model tools.

Input: path/line_offset/n_lines; media path; pattern/directory; regex/search

Result: file/media/search results

Lifecycle: awaited KAOS reads; default schemas, not every media/search executor traced

Authority: Model; file tools plus configurable workspace scope.

Evidence: [e1](#evidence-e1), [e39](#evidence-e39).

### WriteFile / StrReplaceFile

Surface: model tools.

Input: path/content/mode append-or-overwrite; path/edit {old,new,replace_all} or list

Result: filesystem change/diff/error

Lifecycle: awaited; plan mode restricts writes and approval policy applies

Authority: Model; yolo/afk autoapprove, plan protections remain.

Evidence: [e39](#evidence-e39), [e40](#evidence-e40), [e37](#evidence-e37), [e38](#evidence-e38).

### Shell

Surface: model tool.

Input: command, timeout, run_in_background, description

Result: foreground clipped output/exit or task ID/full log pointer

Lifecycle: foreground fresh shell; background detached worker with control store

Authority: Model; approval unless autoapproved; local host authority.

Evidence: [e11](#evidence-e11), [e12](#evidence-e12), [e13](#evidence-e13), [e16](#evidence-e16), [e18](#evidence-e18).

### TaskList / TaskOutput / TaskStop

Surface: model tools.

Input: active_only/limit; task_id/block/timeout; task_id/reason

Result: persistent status/output path/preview; stop status

Lifecycle: root-only; background process control or asyncio agent cancellation

Authority: Root model; no child background-management access.

Evidence: [e20](#evidence-e20), [e21](#evidence-e21), [e22](#evidence-e22).

### Agent

Surface: model tool.

Input: description/prompt/subagent_type/model/resume/run_in_background

Result: agent_id/task_id, text/summary, resumed/type metadata

Lifecycle: new or truly context-restored child; simultaneous resume forbidden; background asyncio

Authority: Model root; per-child alias/spec; role limits and saved launch data.

Evidence: [e23](#evidence-e23), [e24](#evidence-e24), [e25](#evidence-e25), [e19](#evidence-e19).

### SetTodoList / AskUserQuestion

Surface: model tools.

Input: todo list; user question choices

Result: displayed task state or user answer

Lifecycle: default catalog; individual executors not deeply traced

Authority: Model; afk behavior/authority set by runtime.

Evidence: [e1](#evidence-e1), [e37](#evidence-e37).

### EnterPlanMode / ExitPlanMode

Surface: model tools.

Input: plan entry/exit decisions

Result: runtime plan mode/status and controlled plan file writes

Lifecycle: tool-execution mode changes; not redefining turn program

Authority: Model; default operations and file protections.

Evidence: [e1](#evidence-e1), [e8](#evidence-e8), [e39](#evidence-e39).

### SearchWeb / FetchURL

Surface: model tools.

Input: query/limit/include_content; URL

Result: search/document text

Lifecycle: consumed network services, catalog and native schemas inspected; full service implementation not audited

Authority: Model; provider/config credentials.

Evidence: [e1](#evidence-e1).

### SendDMail

Surface: optional model tool.

Input: message/checkpoint_id

Result: signal reverting future context with message

Lifecycle: installed only explicit custom agent catalog; rotates context but external file effects remain

Authority: Model optional; disabled default.

Evidence: [e45](#evidence-e45), [e2](#evidence-e2), [e26](#evidence-e26), [e1](#evidence-e1).

### FlowRunner / /flow skill / ralph_loop

Surface: workflow program API.

Input: graph begin/end/task/decision nodes and labelled edges, move cap

Result: serial node turns/choice transitions

Lifecycle: same soul history across prompt stages; invalid decision retry not counted as valid move

Authority: Host/operator flow skill; model selects branches through <choice>.

Evidence: [e33](#evidence-e33).

### steer / run cancel event

Surface: operator/program API.

Input: follow-up content; cancellation event

Result: queued user injection/wire SteerInput or RunCancelled

Lifecycle: steer between steps and before stop; cancel awaits soul/futures

Authority: Operator/wire client; native model peer-message bus not provided.

Evidence: [e10](#evidence-e10), [e2](#evidence-e2), [e46](#evidence-e46), [e4](#evidence-e4).

### /compact [focus] / context checkpoint,revert

Surface: operator/runtime API.

Input: manual focus; stored checkpoint ID

Result: summary+tail and active task reminder; rotated full previous context files

Lifecycle: compaction separately calls model; checkpoint rotates/rebuilds effective data

Authority: Operator/runtime; model file reads can access history as files, no dedicated repair transaction.

Evidence: [e27](#evidence-e27), [e28](#evidence-e28), [e29](#evidence-e29), [e26](#evidence-e26).

### /model / reload / session switch

Surface: operator commands.

Input: model/thinking/session selection

Result: config save and same-session runtime reconstruction

Lifecycle: background work explicitly preserved; no refit pause/quiescence

Authority: Operator; program reload, not compiled executable handoff.

Evidence: [e34](#evidence-e34), [e35](#evidence-e35).

### /feedback

Surface: operator command.

Input: feedback text under managed provider or GitHub fallback

Result: remote feedback with session ID/version/OS/model

Lifecycle: user prompt/HTTP submit; no captured-state bundle

Authority: Operator; no default model rageshake tool.

Evidence: [e36](#evidence-e36), [e1](#evidence-e1).

### custom agent YAML / native tool imports / hooks / MCP

Surface: extension APIs.

Input: agent prompt/tool paths/subagent specs; hook command/callback; MCP config

Result: native tool descriptors/results and before-tool block

Lifecycle: runtime composition/deferred loading; hook execution around actions; no affected-workflow activation transaction traced

Authority: Program/operator defines extension, model may edit files but not hot-install full loop.

Evidence: [e1](#evidence-e1), [e7](#evidence-e7), [e41](#evidence-e41).

### WireFile.iter_records / Context.restore

Surface: supporting study/persistence APIs.

Input: JSONL path

Result: normalized wire events/messages/checkpoints

Lifecycle: wire merged append and context rotation, malformed data may be skipped

Authority: Program/operator; not general model SQL audit API.

Evidence: [e26](#evidence-e26), [e30](#evidence-e30), [e31](#evidence-e31).

## Capabilities

### filesystem

**I — Files** (source): Native KAOS file/search/media/write/replace with plan/workspace checks; shell expands host access.

Evidence: [e1](#evidence-e1), [e39](#evidence-e39), [e40](#evidence-e40), [e13](#evidence-e13).

### processes

**L — OS programs** (source): Fresh foreground shell kills direct child without reaping/group ownership; background separate worker has group TERM/KILL/wait and file output.

Evidence: [e13](#evidence-e13), [e14](#evidence-e14), [e15](#evidence-e15), [e16](#evidence-e16), [e17](#evidence-e17), [e18](#evidence-e18).

### code-actions

**I — Code actions** (source): Shell general command execution and task-backed background code, not resident kernel.

Evidence: [e11](#evidence-e11), [e12](#evidence-e12), [e18](#evidence-e18).

### persistent-kernel

**? — Kernel** (source): No resident notebook/Python namespace in inspected default tools, KAOS or background worker.

### standing-database

**S — Standing DB** (source): Persistent file task/context stores support standing work; no native general shared-DB query client.

Evidence: [e18](#evidence-e18), [e19](#evidence-e19), [e26](#evidence-e26).

### workflow-programming

**I — Workflows** (source): FlowRunner actual graph prompt/decision transitions, serial stages with model choice; native tools/hooks/agent composition. Invalid choice may repeat without valid-move cap progressing.

Evidence: [e33](#evidence-e33), [e1](#evidence-e1), [e7](#evidence-e7).

### multi-model

**I — Models** (source): Agent model alias override or saved effective/spec model clones child LLM.

Evidence: [e24](#evidence-e24), [e25](#evidence-e25).

### live-collaboration

**L — Peer chat** (source): Concurrent parent/children and TaskOutput feedback exist; resume forbidden while running, no send-to-running-colleague/peer bus in native catalog.

Evidence: [e1](#evidence-e1), [e19](#evidence-e19), [e22](#evidence-e22), [e25](#evidence-e25).

### concurrent-work

**I — Concurrency** (source): Tools start during model generation, background shell workers and in-process agent tasks with stable IDs.

Evidence: [e4](#evidence-e4), [e5](#evidence-e5), [e16](#evidence-e16), [e19](#evidence-e19).

### steering-interrupt

**L — Steer/interrupt** (source): Between-step steers and cancel-and-await run/futures implemented; foreground shell direct-child kill, background status may mark killed before cleanup.

Evidence: [e10](#evidence-e10), [e46](#evidence-e46), [e4](#evidence-e4), [e13](#evidence-e13), [e14](#evidence-e14), [e20](#evidence-e20).

### turn-redefinition

**S — Turn program** (source): Prompt-flow graphs and hook programs expressive; underlying soul step/continuation still host-defined, no model live replacement.

Evidence: [e33](#evidence-e33), [e2](#evidence-e2), [e7](#evidence-e7).

### compaction

**I — Compaction** (source): Auto/manual custom focus summary, preserved tail, begin/end events, rotating original context file and active task snapshot.

Evidence: [e27](#evidence-e27), [e28](#evidence-e28), [e29](#evidence-e29), [e26](#evidence-e26), [e44](#evidence-e44).

### context-repair

**S — Repair** (source): Rotated normalized context files and checkpoint reversion support restoration; optional D-Mail conveys later state. No default model managed original-context repair transaction.

Evidence: [e26](#evidence-e26), [e45](#evidence-e45), [e1](#evidence-e1).

### original-audit

**L — Original audit** (source): Merged wire JSONL and rotating contexts valuable; raw provider request/HTTP/chunk order and clipped foreground output omitted, recorder failures logged only.

Evidence: [e30](#evidence-e30), [e31](#evidence-e31), [e32](#evidence-e32), [e5](#evidence-e5), [e6](#evidence-e6), [e9](#evidence-e9).

### audit-query

**S — Audit query** (source): Wire iter_records/Context.restore and ordinary ReadFile permit study of persisted normalized logs; no model DB audit tables.

Evidence: [e31](#evidence-e31), [e26](#evidence-e26), [e1](#evidence-e1).

### hot-change

**L — Hot change** (source): /model reconstructs same session preserving background tasks; no general after-turn/affected-workflow change transaction.

Evidence: [e34](#evidence-e34), [e35](#evidence-e35).

### rebuild-continuity

**L — Rebuild continuity** (source): Real saved child/context restore and runtime Reload are data continuity; detached Bash can persist, agents remain current asyncio tasks. Reload intentionally keeps work active rather than refit quiescence.

Evidence: [e23](#evidence-e23), [e25](#evidence-e25), [e26](#evidence-e26), [e35](#evidence-e35), [e16](#evidence-e16), [e19](#evidence-e19).

### remote-services

**I — Remote** (source): Provider SDK adapter, MCP client and KAOS boundary consume services; no shared fabric governor.

Evidence: [e6](#evidence-e6), [e41](#evidence-e41), [e13](#evidence-e13).

### self-improvement

**S — Self-improve** (source): Flow ralph/task graph plus editable files/hooks are useful search programs; no governed harness autoresearch/refit lifecycle traced.

Evidence: [e33](#evidence-e33), [e39](#evidence-e39), [e7](#evidence-e7).

### complaints

**L — Complaints** (source): Operator /feedback attaches a few identifiers/version/OS/model, not model-issued full captured-state bead; no native complaint default.

Evidence: [e36](#evidence-e36), [e1](#evidence-e1).

### authority

**I — Authority** (source): Yolo/afk remove tool approvals; hooks/plan restrictions and root-only task access persist. Explore README prompt constraint not treated as physical readonly.

Evidence: [e37](#evidence-e37), [e38](#evidence-e38), [e7](#evidence-e7), [e21](#evidence-e21), [e39](#evidence-e39).

### evaluation

**I — Evaluation** (source): Mock soul/real storage resume test, real controlled sleep worker status test and compaction prompt oracle; not descendant custody/provider correctness.

Evidence: [e42](#evidence-e42), [e43](#evidence-e43), [e44](#evidence-e44).

### time-order

**L — Time/order** (source): Checkpoints/task IDs/wire wall timestamps and awaited cancellation provide order primitives; merged events and early killed status not complete causal original audit.

Evidence: [e26](#evidence-e26), [e31](#evidence-e31), [e20](#evidence-e20), [e46](#evidence-e46).

## Inspected test oracles

- [quarantine/kimi-cli/tests/core/test_subagent_resume_e2e.py](../../../quarantine/kimi-cli/tests/core/test_subagent_resume_e2e.py): Resume identity/history and running-state guard Oracle: Mock soul/provider but actual runner/context/store verifies same ID, growing assistant history, type preservation and active-instance rejection. Read, **not executed**.
- [quarantine/kimi-cli/tests/background/test_worker.py](../../../quarantine/kimi-cli/tests/background/test_worker.py): Controlled stop and deadlines Oracle: Real sleep worker writes terminal killed/timeout status and reason; no independently probed surviving descendants. Read, **not executed**.
- [quarantine/kimi-cli/tests/core/test_simple_compaction.py](../../../quarantine/kimi-cli/tests/core/test_simple_compaction.py): Summary focus and tail projection Oracle: Deterministic custom instruction/tail assertions; no semantic original recovery or vendor response oracle. Read, **not executed**.

## Useful mechanisms

- First-class task output/path/stop identity and child context restore.
- Flow graph branching with model choice and same conversation.
- Compaction rotates previous context and reinjects active work snapshot.

## Material limits

- Alternative provider/remote KAOS transports, all media/search/question executors, ACP/web UI, flow parser and full hook scheduler not exhaustively traced. Source only, no reference run.

## Arconaut design questions

- Can foreground commands use the stronger background process-group/reap contract?
- Can live child messages/steers exist without waiting for idle resume?
- Can original capture retain raw output/provider boundaries before presentation merging and clipping?
- Can after-workflow hot change and quiescent refit separate from keep-running runtime reload?

## Evidence

### Evidence e1

[quarantine/kimi-cli/src/kimi_cli/agents/default/agent.yaml:1–36](../../../quarantine/kimi-cli/src/kimi_cli/agents/default/agent.yaml#L1): Default native action catalog and named subagents; Think/SendDMail disabled here.

### Evidence e2

[quarantine/kimi-cli/src/kimi_cli/soul/kimisoul.py:1010–1110](../../../quarantine/kimi-cli/src/kimi_cli/soul/kimisoul.py#L1010): Compaction/checkpoint/step/outcome/steer phases; D-Mail revert and next-step steering.

### Evidence e3

[quarantine/kimi-cli/src/kimi_cli/soul/kimisoul.py:1199–1218](../../../quarantine/kimi-cli/src/kimi_cli/soul/kimisoul.py#L1199): kosong.step receives current prompt/toolset/effective history and wire callbacks.

### Evidence e4

[quarantine/kimi-cli/packages/kosong/src/kosong/__init__.py:104–184](../../../quarantine/kimi-cli/packages/kosong/src/kosong/__init__.py#L104): Step starts tools as calls complete during generation; cancels and awaits tracked futures on model failure/cancellation.

### Evidence e5

[quarantine/kimi-cli/packages/kosong/src/kosong/_generate.py:52–85](../../../quarantine/kimi-cli/packages/kosong/src/kosong/_generate.py#L52): Actual provider.generate and streamed-part assembly/tool-call callback; normalized messages, not raw HTTP wire.

### Evidence e6

[quarantine/kimi-cli/packages/kosong/src/kosong/chat_provider/kimi.py:146–200](../../../quarantine/kimi-cli/packages/kosong/src/kosong/chat_provider/kimi.py#L146): Native provider converts history/tools and uses async OpenAI SDK stream/raw-response API.

### Evidence e7

[quarantine/kimi-cli/src/kimi_cli/soul/toolset.py:456–506](../../../quarantine/kimi-cli/src/kimi_cli/soul/toolset.py#L456): PreToolUse hook may block; native tool coroutine awaited and cancellation propagated.

### Evidence e8

[quarantine/kimi-cli/src/kimi_cli/soul/kimisoul.py:1256–1347](../../../quarantine/kimi-cli/src/kimi_cli/soul/kimisoul.py#L1256): Awaits tool results then grows history and stops on no calls/rejection/repetition.

### Evidence e9

[quarantine/kimi-cli/src/kimi_cli/soul/kimisoul.py:1389–1410](../../../quarantine/kimi-cli/src/kimi_cli/soul/kimisoul.py#L1389): Normalized assistant/tool context appended after results complete.

### Evidence e10

[quarantine/kimi-cli/src/kimi_cli/soul/kimisoul.py:622–654](../../../quarantine/kimi-cli/src/kimi_cli/soul/kimisoul.py#L622): Steer queue injects follow-up user messages; /btw intercepted elsewhere.

### Evidence e11

[quarantine/kimi-cli/src/kimi_cli/tools/shell/__init__.py:26–56](../../../quarantine/kimi-cli/src/kimi_cli/tools/shell/__init__.py#L26): Shell command/timeout/background/description parameter contract.

### Evidence e12

[quarantine/kimi-cli/src/kimi_cli/tools/shell/__init__.py:81–142](../../../quarantine/kimi-cli/src/kimi_cli/tools/shell/__init__.py#L81): Shell approval and bounded output builder with success/error exit interpretation.

### Evidence e13

[quarantine/kimi-cli/src/kimi_cli/tools/shell/__init__.py:221–264](../../../quarantine/kimi-cli/src/kimi_cli/tools/shell/__init__.py#L221): Fresh shell via KAOS, timeout only wraps reading pipes; cancellation/timeout calls process.kill, no wait there.

### Evidence e14

[quarantine/kimi-cli/packages/kaos/src/kaos/local.py:39–64](../../../quarantine/kimi-cli/packages/kaos/src/kaos/local.py#L39): Local process.kill kills direct child without awaiting reap or descendants.

### Evidence e15

[quarantine/kimi-cli/packages/kaos/src/kaos/local.py:165–177](../../../quarantine/kimi-cli/packages/kaos/src/kaos/local.py#L165): Foreground local subprocess has no new process session/group.

### Evidence e16

[quarantine/kimi-cli/src/kimi_cli/background/manager.py:135–147](../../../quarantine/kimi-cli/src/kimi_cli/background/manager.py#L135): Bash task worker is detached process with new OS session.

### Evidence e17

[quarantine/kimi-cli/src/kimi_cli/background/worker.py:87–119](../../../quarantine/kimi-cli/src/kimi_cli/background/worker.py#L87): Worker control loop sends group TERM then KILL after grace.

### Evidence e18

[quarantine/kimi-cli/src/kimi_cli/background/worker.py:129–173](../../../quarantine/kimi-cli/src/kimi_cli/background/worker.py#L129): Background shell stdout/stderr file and process group; actual wait/timeout/TERM/KILL/reap.

### Evidence e19

[quarantine/kimi-cli/src/kimi_cli/background/manager.py:209–270](../../../quarantine/kimi-cli/src/kimi_cli/background/manager.py#L209): Background agents instead asyncio tasks in current parent; persistent task spec and explicit agent/model/prompt.

### Evidence e20

[quarantine/kimi-cli/src/kimi_cli/background/manager.py:347–403](../../../quarantine/kimi-cli/src/kimi_cli/background/manager.py#L347): TaskStop agent marks killed before task cancellation cleanup; Bash sends persisted control and best-effort signal.

### Evidence e21

[quarantine/kimi-cli/src/kimi_cli/tools/background/__init__.py:111–161](../../../quarantine/kimi-cli/src/kimi_cli/tools/background/__init__.py#L111): TaskList/TaskOutput/TaskStop schemas, background access root-only.

### Evidence e22

[quarantine/kimi-cli/src/kimi_cli/tools/background/__init__.py:214–265](../../../quarantine/kimi-cli/src/kimi_cli/tools/background/__init__.py#L214): TaskOutput can block with timeout then returns status, preview path/size and full log ReadFile hint.

### Evidence e23

[quarantine/kimi-cli/src/kimi_cli/subagents/core.py:36–85](../../../quarantine/kimi-cli/src/kimi_cli/subagents/core.py#L36): Real child context restore and persisted prompt reuse; prompt snapshot and new soul.

### Evidence e24

[quarantine/kimi-cli/src/kimi_cli/subagents/builder.py:12–42](../../../quarantine/kimi-cli/src/kimi_cli/subagents/builder.py#L12): Cloned child LLM model override/effective/spec fallback.

### Evidence e25

[quarantine/kimi-cli/src/kimi_cli/tools/agent/__init__.py:174–236](../../../quarantine/kimi-cli/src/kimi_cli/tools/agent/__init__.py#L174): Reject simultaneous resume, mark running_background before async dispatch and retain launch-model metadata.

### Evidence e26

[quarantine/kimi-cli/src/kimi_cli/soul/context.py:135–242](../../../quarantine/kimi-cli/src/kimi_cli/soul/context.py#L135): Revert/clear rotate old file then reconstruct/clear effective messages; normalized append JSONL.

### Evidence e27

[quarantine/kimi-cli/src/kimi_cli/soul/compaction.py:114–147](../../../quarantine/kimi-cli/src/kimi_cli/soul/compaction.py#L114): Summary via separate no-tools model step, excludes thinking and returns summary plus preserved tail.

### Evidence e28

[quarantine/kimi-cli/src/kimi_cli/soul/compaction.py:154–198](../../../quarantine/kimi-cli/src/kimi_cli/soul/compaction.py#L154): Preserves last configured user-message groups and serializes prior history for summary.

### Evidence e29

[quarantine/kimi-cli/src/kimi_cli/soul/kimisoul.py:1540–1595](../../../quarantine/kimi-cli/src/kimi_cli/soul/kimisoul.py#L1540): Compaction emits begin, rotates/clears context, writes summary/checkpoint and retains active task snapshot.

### Evidence e30

[quarantine/kimi-cli/src/kimi_cli/wire/__init__.py:23–64](../../../quarantine/kimi-cli/src/kimi_cli/wire/__init__.py#L23): Wire recorder consumes merged complete-message queue; recorder flush failure only logged.

### Evidence e31

[quarantine/kimi-cli/src/kimi_cli/wire/file.py:108–145](../../../quarantine/kimi-cli/src/kimi_cli/wire/file.py#L108): JSONL timestamps/protocol metadata, malformed records skipped, append not fsync transaction.

### Evidence e32

[quarantine/kimi-cli/src/kimi_cli/tools/utils.py:54–124](../../../quarantine/kimi-cli/src/kimi_cli/tools/utils.py#L54): Output builder clips chars and per-line size before native result/wire record.

### Evidence e33

[quarantine/kimi-cli/src/kimi_cli/soul/kimisoul.py:1849–1927](../../../quarantine/kimi-cli/src/kimi_cli/soul/kimisoul.py#L1849): FlowRunner node/decision execution, bounded valid moves, invalid decision repeats same node.

### Evidence e34

[quarantine/kimi-cli/src/kimi_cli/ui/shell/slash.py:284–312](../../../quarantine/kimi-cli/src/kimi_cli/ui/shell/slash.py#L284): Model operator command saves config then reloads same session.

### Evidence e35

[quarantine/kimi-cli/src/kimi_cli/cli/__init__.py:679–712](../../../quarantine/kimi-cli/src/kimi_cli/cli/__init__.py#L679): Reload/vis/web explicitly preserve background tasks rather than stopping them; ordinary exit shuts down.

### Evidence e36

[quarantine/kimi-cli/src/kimi_cli/ui/shell/slash.py:444–501](../../../quarantine/kimi-cli/src/kimi_cli/ui/shell/slash.py#L444): Operator feedback includes session ID/text/version/OS/model; no state bundle/native model complaint.

### Evidence e37

[quarantine/kimi-cli/src/kimi_cli/soul/approval.py:172–179](../../../quarantine/kimi-cli/src/kimi_cli/soul/approval.py#L172): Afk or yolo implies automatic tool approval.

### Evidence e38

[quarantine/kimi-cli/src/kimi_cli/soul/approval.py:238–247](../../../quarantine/kimi-cli/src/kimi_cli/soul/approval.py#L238): Autoapproval path bypasses operator request after trace metadata setup.

### Evidence e39

[quarantine/kimi-cli/src/kimi_cli/tools/file/write.py:86–143](../../../quarantine/kimi-cli/src/kimi_cli/tools/file/write.py#L86): KAOS file path validation/plan protection and append/overwrite preparation.

### Evidence e40

[quarantine/kimi-cli/src/kimi_cli/tools/file/replace.py:95–147](../../../quarantine/kimi-cli/src/kimi_cli/tools/file/replace.py#L95): Multiple old/new edits prepared and no-op rejected before write approval.

### Evidence e41

[quarantine/kimi-cli/src/kimi_cli/soul/toolset.py:932–968](../../../quarantine/kimi-cli/src/kimi_cli/soul/toolset.py#L932): MCP request approval, client call timeout and converts is_error results.

### Evidence e42

[quarantine/kimi-cli/tests/core/test_subagent_resume_e2e.py:82–149](../../../quarantine/kimi-cli/tests/core/test_subagent_resume_e2e.py#L82): Mock soul but real runner/store/context; same ID resumes and assistant context accumulates.

### Evidence e43

[quarantine/kimi-cli/tests/background/test_worker.py:43–82](../../../quarantine/kimi-cli/tests/background/test_worker.py#L43): Real sleep worker controlled stop; checks stored terminal status/reason, not descendants separately.

### Evidence e44

[quarantine/kimi-cli/tests/core/test_simple_compaction.py:131–150](../../../quarantine/kimi-cli/tests/core/test_simple_compaction.py#L131): Compaction custom focus included in model prompt; no semantic preservation oracle.

### Evidence e45

[quarantine/kimi-cli/src/kimi_cli/tools/dmail/__init__.py:12–38](../../../quarantine/kimi-cli/src/kimi_cli/tools/dmail/__init__.py#L12): Optional SendDMail implemented checkpoint signal, disabled default catalog.

### Evidence e46

[quarantine/kimi-cli/src/kimi_cli/soul/__init__.py:202–236](../../../quarantine/kimi-cli/src/kimi_cli/soul/__init__.py#L202): Run wrapper races cancel event, cancels and awaits soul; wire recorder/notification lifecycle.

