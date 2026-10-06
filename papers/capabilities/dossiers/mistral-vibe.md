# mistral-vibe

Dual coding harness with typed concurrent tools, real task child loops, projected compaction and hot configuration; experimental native protocol and replayable TypeScript orchestration give distinct design leads.

Role: coding agent. Runtime: Python, Rust, TypeScript in embedded V8.

Pinned source: [https://github.com/mistralai/mistral-vibe](https://github.com/mistralai/mistral-vibe); revision/version `7c19608af06f6c61d63f8f7a5c3430da73fba2ab`.

Default Python AgentLoop/app-server scheduler; experimental Rust pure state machine/Python hosts; local managed terminal readers and alternate native CLI surfaces

Owns selected shell sessions/file state and harness sessions; provider/tool-IO/MCP/connector/hosted teleport clients. Rust core emits actions; Python drivers own external effects.

Inspection: Default entry/app-server/turn/request/dispatch/results/continuation, native file and both shell variants, child create/cancel, projected compaction/log/rewind/reload, teleport; bounded unified Engine/driver/model result/code-mode dispatch and configured child receipts; relevant source tests.

Limits of this study: Source read only, reference not executed. Both implementations substantial: default Python branch traced more completely; unified app-server host complete cancellation/resource/recovery machinery, connector/MCP/network executors, native Rust CLI/frontend, all code-mode builtins and multi-model child binding details not exhaustively audited. Bare unified defaults disable compaction/subagents/background, application config may enable. No reference execution.

## Actions

### read_file / write_file / edit / grep

Surface: model tools.

Input: absolute file_path/range/content/replacements; search

Result: typed file/change/search results

Lifecycle: awaited native tools or consumed IO backend

Authority: Model profile/tool selection and permissions; full grep executor not deeply traced.

Evidence: [e15](#evidence-e15), [e16](#evidence-e16), [e17](#evidence-e17), [e7](#evidence-e7), [e11](#evidence-e11), [e13](#evidence-e13).

### bash (legacy fresh process)

Surface: model tool.

Input: command/timeout

Result: clipped captured stdout/stderr/exit

Lifecycle: fresh shell or IO terminal, timeout then finally group kill/reap

Authority: Model; permission decision/bypass.

Evidence: [e18](#evidence-e18), [e19](#evidence-e19), [e38](#evidence-e38).

### bash (managed variant)

Surface: model tool.

Input: command/background/cwd/env/shell/timeout_seconds/hard_timeout

Result: session ID/status/log path

Lifecycle: persistent terminal job; soft timeout hands over ongoing process, hard abort kills

Authority: Model when selected available variant; variants do not imply every profile default.

Evidence: [e14](#evidence-e14), [e20](#evidence-e20), [e22](#evidence-e22), [e24](#evidence-e24), [e25](#evidence-e25).

### bash_output / bash_stdin / bash_sessions / bash_log_file

Surface: managed model tools.

Input: session_id/cursor/wait/max_bytes; text/control/base64; list/inspect/kill/reset; log read/write/append

Result: cursor output/status/path; bytes written; session metadata

Lifecycle: ongoing terminal handles and log access; kill joins readers

Authority: Model selected tool family; log write means these logs are mutable, not immutable audit.

Evidence: [e21](#evidence-e21), [e23](#evidence-e23), [e13](#evidence-e13).

### task

Surface: model tool (legacy).

Input: agent/task

Result: response/turns_used/completed plus streamed child progress

Lifecycle: child independent loop inside awaited parent task; cancel interrupts and awaits child

Authority: Model with profile permissions, parent-shared grants.

Evidence: [e26](#evidence-e26), [e27](#evidence-e27), [e28](#evidence-e28), [e29](#evidence-e29).

### skill / todo / ask_user_question / exit_plan_mode / web_fetch / web_search

Surface: model tools.

Input: skill name, todo action read/write, question, plan acceptance, URL/query

Result: prompt/task/user/plan/network results

Lifecycle: native discovered catalog; individual network/question executors not exhaustively traced

Authority: Model under permission/profile and prompt invocation controls.

Evidence: [e11](#evidence-e11), [e12](#evidence-e12), [e7](#evidence-e7), [e4](#evidence-e4).

### run_typescript

Surface: experimental unified model tool.

Input: inline async main TypeScript and discovered tools namespace

Result: value/error/pending tool effects resumed through operation receipts

Lifecycle: new isolated V8 evaluation/replay at pending effects, bounded effects/ops/heap/watchdog

Authority: Model configured unified harness; ambient OS/network unavailable inside VM, provided tool handlers supply effects.

Evidence: [e46](#evidence-e46), [e47](#evidence-e47), [e48](#evidence-e48), [e49](#evidence-e49), [e50](#evidence-e50), [e53](#evidence-e53).

### subagent.spawn / subagent.list / subagent.wait / subagent.send_message / subagent.interrupt / subagent.stop

Surface: configured unified tools.

Input: agentName/type/message/timeout and target child

Result: durable child receipt/state/turn generation/status

Lifecycle: send admission can steer running/start idle; operation-key ack and watcher

Authority: Model when feature configured; no assertion bare host exposes it by default.

Evidence: [e51](#evidence-e51), [e52](#evidence-e52), [e53](#evidence-e53).

### Engine create/restore/apply/inspect/checkpoint

Surface: native programmable API.

Input: config/history/checkpoint/input_id/command/determinism

Result: transition/actions/observation/checkpoint or rejection

Lifecycle: Rust serialized state advancement; Python host drives external action completion

Authority: Program author/runtime; checkpoint alone does not own provider/process termination.

Evidence: [e41](#evidence-e41), [e42](#evidence-e42), [e43](#evidence-e43), [e44](#evidence-e44), [e45](#evidence-e45).

### middleware/hooks/custom Python tools

Surface: extension API.

Input: before-turn/tool/hooks; BaseTool modules in user/project paths

Result: stop/continue/rewrite/tool result

Lifecycle: Python programs construction/reload; sys.modules cache limits code refresh

Authority: Operator/program; model editing source does not automatically replace turn.

Evidence: [e4](#evidence-e4), [e11](#evidence-e11), [e12](#evidence-e12), [e35](#evidence-e35), [e36](#evidence-e36).

### compact / rewind

Surface: operator/runtime API.

Input: extra summary instruction; prior user index/restore_files/inplace

Result: appended context boundary or fork/truncated history/file restoration errors

Lifecycle: summary snapshot then append; effective model projection; inplace rewind destructive

Authority: Operator/runtime; no native model original-audit restoration transaction traced.

Evidence: [e30](#evidence-e30), [e31](#evidence-e31), [e34](#evidence-e34).

### reload_with_initial_messages / switch_agent / config update

Surface: runtime/operator APIs.

Input: new profile/config/hook reload

Result: atomically swapped managers/prompt/backend

Lifecycle: generation guards stale prepare; live active-turn swap possible, old transport deferred close

Authority: Operator/client; not default after-affected-workflow transaction.

Evidence: [e35](#evidence-e35), [e36](#evidence-e36), [e37](#evidence-e37).

### teleport

Surface: operator/service action.

Input: prompt, hosted project/conversation IDs and packed/summary context

Result: hosted workflow URL

Lifecycle: checks/potentially pushes Git then starts hosted workflow; no main executable continuity

Authority: Operator; Git push requires service approval branch.

Evidence: [e39](#evidence-e39), [e40](#evidence-e40).

### SessionLogger.save_interaction

Surface: supporting persistence API.

Input: normalized history/stats/tools/profile

Result: message JSONL/current metadata

Lifecycle: serialized append or fsynced atomic rewrite, nonsystem only

Authority: Runtime/program; not comprehensive original event log.

Evidence: [e32](#evidence-e32), [e33](#evidence-e33).

## Capabilities

### filesystem

**I — Files** (source): Native typed filesystem/edit tools with permission hooks, delegated IO; checkpointer supports selected file rewind.

Evidence: [e15](#evidence-e15), [e16](#evidence-e16), [e17](#evidence-e17), [e34](#evidence-e34).

### processes

**I — OS programs** (source): Legacy shell group kill/reap; managed terminal IDs/output/stdin/control/kill/soft handoff and reader manifests; transport-dependent behavior.

Evidence: [e18](#evidence-e18), [e19](#evidence-e19), [e20](#evidence-e20), [e21](#evidence-e21), [e22](#evidence-e22), [e23](#evidence-e23), [e24](#evidence-e24), [e25](#evidence-e25).

### code-actions

**I — Code actions** (source): General Bash; experimental Rust dispatches replayable isolated TypeScript/V8 orchestrated external tools. Separate feature scopes.

Evidence: [e18](#evidence-e18), [e46](#evidence-e46), [e47](#evidence-e47), [e48](#evidence-e48), [e49](#evidence-e49), [e50](#evidence-e50).

### persistent-kernel

**L — Kernel** (source): Persistent terminal process with stdin could host interpreter by composition; no native managed Python notebook namespace in traced paths. V8 evaluates replay rather than continuous kernel.

Evidence: [e21](#evidence-e21), [e24](#evidence-e24), [e48](#evidence-e48), [e49](#evidence-e49).

### standing-database

**S — Standing DB** (source): Session/checkpoint/task receipts and file logs persistent state; generic shared SQL/kernel service client not established.

Evidence: [e32](#evidence-e32), [e41](#evidence-e41), [e51](#evidence-e51).

### workflow-programming

**I — Workflows** (source): Python middleware/hooks/custom tools; configured unified model TypeScript supports async external-tool programs with replay state/IDs.

Evidence: [e4](#evidence-e4), [e11](#evidence-e11), [e12](#evidence-e12), [e47](#evidence-e47), [e48](#evidence-e48), [e49](#evidence-e49).

### multi-model

**I — Models** (source): Copied child config plus selected agent profile, multiple backend adapters/compaction model; not provider selection alone claimed live peers.

Evidence: [e29](#evidence-e29), [e10](#evidence-e10), [e30](#evidence-e30).

### live-collaboration

**L — Peer chat** (source): Legacy child runs during parent task with streamed progress; configured unified send steers running child/starts idle via receipts. Parent-child scope, not unrestricted authenticated IRC peers.

Evidence: [e27](#evidence-e27), [e28](#evidence-e28), [e51](#evidence-e51), [e52](#evidence-e52), [e53](#evidence-e53).

### concurrent-work

**I — Concurrency** (source): Python concurrent tool batch and child loop; managed background terminals, runtime directive tasks and separately gated title generation.

Evidence: [e6](#evidence-e6), [e27](#evidence-e27), [e24](#evidence-e24), [e42](#evidence-e42), [e4](#evidence-e4).

### steering-interrupt

**L — Steer/interrupt** (source): Legacy cancellation cancels/awaits tool batch, closes provider stream and child interrupt wait; GeneratorExit path weaker; unified core interrupt is protocol action, host custody not fully audited.

Evidence: [e6](#evidence-e6), [e10](#evidence-e10), [e28](#evidence-e28), [e41](#evidence-e41), [e42](#evidence-e42).

### turn-redefinition

**S — Turn program** (source): Middleware/hooks and TypeScript effects configure sophisticated programs; native core turn protocol still authored/compiled, no model live loop replacement.

Evidence: [e4](#evidence-e4), [e47](#evidence-e47), [e48](#evidence-e48), [e41](#evidence-e41).

### compaction

**I — Compaction** (source): Default Python appends boundary with prior user/summary and selects effective projection, retaining previous messages; configured Rust supports checkpointed compaction state.

Evidence: [e30](#evidence-e30), [e31](#evidence-e31), [e54](#evidence-e54), [e56](#evidence-e56), [e57](#evidence-e57).

### context-repair

**L — Repair** (source): Prior normalized history retained after compaction, operator fork/rewind/file restore; inplace rewind destroys current old log. No model managed original-history repair traced.

Evidence: [e30](#evidence-e30), [e31](#evidence-e31), [e32](#evidence-e32), [e34](#evidence-e34).

### original-audit

**L — Original audit** (source): Normalized nonsystem messages and some persisted typed results, but clipping precedes capture, logger can rewrite and provider raw payload/chunks omitted. Unified transitions/checkpoints are state receipts, not all original external evidence.

Evidence: [e18](#evidence-e18), [e32](#evidence-e32), [e33](#evidence-e33), [e34](#evidence-e34), [e44](#evidence-e44), [e45](#evidence-e45), [e58](#evidence-e58).

### audit-query

**S — Audit query** (source): Session message/log file and Engine.inspect/checkpoint primitives; model terminal log read can inspect but also mutate its log.

Evidence: [e21](#evidence-e21), [e32](#evidence-e32), [e41](#evidence-e41).

### hot-change

**L — Hot change** (source): Generation-based off-loop preparation and synchronous swap protect partial/stale config; active turn may adopt swapped objects and defer old backend close. Cached Python modules limit source hot reload.

Evidence: [e35](#evidence-e35), [e36](#evidence-e36), [e37](#evidence-e37), [e12](#evidence-e12).

### rebuild-continuity

**S — Rebuild continuity** (source): Native checkpoint+explicit input cursor restores pending compaction/projection and state protocol; runtime data resume exists, no quiescent own provider/process outpost/recompile handoff proved.

Evidence: [e41](#evidence-e41), [e45](#evidence-e45), [e56](#evidence-e56), [e57](#evidence-e57), [e42](#evidence-e42).

### remote-services

**I — Remote** (source): Mistral/alternate backends, MCP/connector contexts, consumed tool IO and hosted teleport workflow.

Evidence: [e10](#evidence-e10), [e7](#evidence-e7), [e39](#evidence-e39), [e40](#evidence-e40).

### self-improvement

**S — Self-improve** (source): Editable owned programs, hooks and replayable TypeScript orchestrator support search; no governed harness autodroit research/refit loop traced.

Evidence: [e15](#evidence-e15), [e11](#evidence-e11), [e47](#evidence-e47), [e48](#evidence-e48).

### complaints

**? — Complaints** (source): No state-rich model complaint/bead operation in inspected native catalog/loop/protocol; operator/telemetry events are not that contract.

### authority

**I — Authority** (source): Bypass execution, per-tool/profile grants, shared child permissions, sandbox no ambient effect access; caller handlers supply actual authority.

Evidence: [e38](#evidence-e38), [e29](#evidence-e29), [e46](#evidence-e46), [e43](#evidence-e43).

### evaluation

**I — Evaluation** (source): Concrete compaction append/focus oracle, hook-binding mock and native restored/live checkpoint/projected result comparisons; pure-state equality not external custody proof.

Evidence: [e54](#evidence-e54), [e55](#evidence-e55), [e56](#evidence-e56), [e57](#evidence-e57), [e58](#evidence-e58).

### time-order

**I — Time/order** (source): Unified core explicit sequenced input IDs/action/operation IDs/determinism, plus runtime child-generation receipt admission; original external byte audit still limited.

Evidence: [e45](#evidence-e45), [e49](#evidence-e49), [e51](#evidence-e51), [e41](#evidence-e41).

## Inspected test oracles

- [quarantine/mistral-vibe/tests/core/compaction/test_compaction_manager.py](../../../quarantine/mistral-vibe/tests/core/compaction/test_compaction_manager.py): Compaction success/retention Oracle: Mock completion verifies old messages remain and boundary/prior users appended; semantic summary correctness not established. Read, **not executed**.
- [quarantine/mistral-vibe/tests/agent_loop/test_hook_reload.py](../../../quarantine/mistral-vibe/tests/agent_loop/test_hook_reload.py): Hook reload binding Oracle: Mock loader verifies hook manager/diagnostics/runtime policy, not affected-workflow timing/refit. Read, **not executed**.
- [quarantine/mistral-vibe/harness/core/src/tests/compaction_and_reconfiguration.rs](../../../quarantine/mistral-vibe/harness/core/src/tests/compaction_and_reconfiguration.rs): Pending compaction restoration Oracle: Pure state actual checkpoint compares live/restored next context and atomic bad-reconfiguration rejection, not external provider/process custody. Read, **not executed**.
- [quarantine/mistral-vibe/harness/core/src/tests/programmatic_tools.rs](../../../quarantine/mistral-vibe/harness/core/src/tests/programmatic_tools.rs): Replay/projection boundaries Oracle: Live/restored result/checkpoint equality and private output exclusion; intentionally projected intermediary data is not original audit. Read, **not executed**.

## Useful mechanisms

- Compaction boundary/projection preserves prior normalized messages.
- Model-programmable replayable TypeScript effects routed by stable IDs separate from state-machine implementation.
- Managed shell soft timeout transfers an ongoing handle with cursor/stdin/kill access.
- Config preparation generation and synchronous commit reveal useful partial-update protections.

## Material limits

- Both implementations substantial: default Python branch traced more completely; unified app-server host complete cancellation/resource/recovery machinery, connector/MCP/network executors, native Rust CLI/frontend, all code-mode builtins and multi-model child binding details not exhaustively audited. Bare unified defaults disable compaction/subagents/background, application config may enable. No reference execution.

## Arconaut design questions

- Can complete original effects be captured before projection/clipping and protected against log rewrite?
- Can native state replay/refit be joined to external effect custody without reissuing uncertain work?
- Can change transaction default to whole affected workflow end while keeping strong stale-generation guards?
- How should parent-child message receipts generalize to shared multi-model peers without owning shared compute?

## Evidence

### Evidence e1

[quarantine/mistral-vibe/vibe/cli/cli.py:228–292](../../../quarantine/mistral-vibe/vibe/cli/cli.py#L228): Interactive CLI creates LocalHarness and app-server client.

### Evidence e2

[quarantine/mistral-vibe/vibe/_experimental_harness.py:80–135](../../../quarantine/mistral-vibe/vibe/_experimental_harness.py#L80): Default legacy or rollout/explicit unified selection; native host module factory.

### Evidence e3

[quarantine/mistral-vibe/vibe/app_server/local.py:74–112](../../../quarantine/mistral-vibe/vibe/app_server/local.py#L74): Local app-server transport in-process; cannot mix harness backends on one LocalHarnessHost.

### Evidence e4

[quarantine/mistral-vibe/vibe/core/agent_loop/_loop.py:2050–2136](../../../quarantine/mistral-vibe/vibe/core/agent_loop/_loop.py#L2050): Python conversation middleware/model/tool continuation, overflow recovery, per-step save, pending injections and clear requests.

### Evidence e5

[quarantine/mistral-vibe/vibe/core/agent_loop/_loop.py:2464–2488](../../../quarantine/mistral-vibe/vibe/core/agent_loop/_loop.py#L2464): Provider generation followed by parsed/resolved tool calls.

### Evidence e6

[quarantine/mistral-vibe/vibe/core/agent_loop/_loop.py:2610–2657](../../../quarantine/mistral-vibe/vibe/core/agent_loop/_loop.py#L2610): Concurrent tool tasks; cancellation cancels and awaits; GeneratorExit cancels tasks but does not directly gather same way.

### Evidence e7

[quarantine/mistral-vibe/vibe/core/agent_loop/_loop.py:2828–2904](../../../quarantine/mistral-vibe/vibe/core/agent_loop/_loop.py#L2828): Native invocation context with subagent/terminal/sampling/hooks/permissions; serializes typed results.

### Evidence e8

[quarantine/mistral-vibe/vibe/core/agent_loop/_loop.py:3013–3043](../../../quarantine/mistral-vibe/vibe/core/agent_loop/_loop.py#L3013): Normalized tool message plus persisted result appended, host telemetry sent.

### Evidence e9

[quarantine/mistral-vibe/vibe/core/agent_loop/_loop.py:3231–3277](../../../quarantine/mistral-vibe/vibe/core/agent_loop/_loop.py#L3231): Streaming request consumes projected history/tools, model config and backend metadata.

### Evidence e10

[quarantine/mistral-vibe/vibe/core/llm/backend/mistral.py:527–578](../../../quarantine/mistral-vibe/vibe/core/llm/backend/mistral.py#L527): Actual Mistral stream_async conversion and async-with stream closure.

### Evidence e11

[quarantine/mistral-vibe/vibe/core/tools/manager.py:145–166](../../../quarantine/mistral-vibe/vibe/core/tools/manager.py#L145): Tools discovered in configured/project/user paths.

### Evidence e12

[quarantine/mistral-vibe/vibe/core/tools/manager.py:198–237](../../../quarantine/mistral-vibe/vibe/core/tools/manager.py#L198): Imported Python modules cached in sys.modules and expose concrete BaseTool subclasses; config reload alone need not reload code.

### Evidence e13

[quarantine/mistral-vibe/vibe/core/tools/manager.py:310–338](../../../quarantine/mistral-vibe/vibe/core/tools/manager.py#L310): Availability/profile enabled and disabled filters.

### Evidence e14

[quarantine/mistral-vibe/vibe/core/tools/manager.py:394–434](../../../quarantine/mistral-vibe/vibe/core/tools/manager.py#L394): Tool variants selected by availability, priority and discovery order.

### Evidence e15

[quarantine/mistral-vibe/vibe/core/tools/builtins/write_file.py:114–138](../../../quarantine/mistral-vibe/vibe/core/tools/builtins/write_file.py#L114): Actual write delegation/result and size/path preparation.

### Evidence e16

[quarantine/mistral-vibe/vibe/core/tools/builtins/read_file.py:59–82](../../../quarantine/mistral-vibe/vibe/core/tools/builtins/read_file.py#L59): Native file read parameters and typed result shape.

### Evidence e17

[quarantine/mistral-vibe/vibe/core/tools/builtins/edit.py:40–64](../../../quarantine/mistral-vibe/vibe/core/tools/builtins/edit.py#L40): Native replacement schema and edit result.

### Evidence e18

[quarantine/mistral-vibe/vibe/core/tools/builtins/bash.py:747–818](../../../quarantine/mistral-vibe/vibe/core/tools/builtins/bash.py#L747): Legacy fresh-shell or consumed terminal IO; bounded result and finally kill_async_subprocess.

### Evidence e19

[quarantine/mistral-vibe/vibe/core/utils/async_subprocess.py:13–63](../../../quarantine/mistral-vibe/vibe/core/utils/async_subprocess.py#L13): Force kill process group/direct child per platform then await reap.

### Evidence e20

[quarantine/mistral-vibe/vibe/core/tools/builtins/experimental_bash.py:1283–1309](../../../quarantine/mistral-vibe/vibe/core/tools/builtins/experimental_bash.py#L1283): Managed shell command/background/hard-or-soft timeout/cwd/env/shell schemas.

### Evidence e21

[quarantine/mistral-vibe/vibe/core/tools/builtins/experimental_bash.py:1331–1440](../../../quarantine/mistral-vibe/vibe/core/tools/builtins/experimental_bash.py#L1331): Output cursor, stdin control/raw bytes, sessions list/inspect/kill/reset and log read/write/append schemas.

### Evidence e22

[quarantine/mistral-vibe/vibe/core/tools/builtins/experimental_bash.py:584–633](../../../quarantine/mistral-vibe/vibe/core/tools/builtins/experimental_bash.py#L584): Managed terminal OS backend starts, stable ID/manifest/log and daemon reader thread.

### Evidence e23

[quarantine/mistral-vibe/vibe/core/tools/builtins/experimental_bash.py:722–747](../../../quarantine/mistral-vibe/vibe/core/tools/builtins/experimental_bash.py#L722): Managed kill updates status, terminates, restores running on failure, joins readers and saves manifest.

### Evidence e24

[quarantine/mistral-vibe/vibe/core/tools/builtins/experimental_bash.py:1849–1921](../../../quarantine/mistral-vibe/vibe/core/tools/builtins/experimental_bash.py#L1849): Foreground owns kill-on-abort scope; soft timeout returns live background handle, hard timeout kills.

### Evidence e25

[quarantine/mistral-vibe/vibe/core/tools/builtins/managed_shell/_posix.py:116–157](../../../quarantine/mistral-vibe/vibe/core/tools/builtins/managed_shell/_posix.py#L116): Managed terminal creates new session and group termination; persistent process, not Python namespace.

### Evidence e26

[quarantine/mistral-vibe/vibe/core/subagents.py:16–31](../../../quarantine/mistral-vibe/vibe/core/subagents.py#L16): Legacy task args only agent/task, returns response/turn count/completed.

### Evidence e27

[quarantine/mistral-vibe/vibe/app_server/_sessions.py:315–354](../../../quarantine/mistral-vibe/vibe/app_server/_sessions.py#L315): Legacy Task creates independent child runtime and persists/links child before start with failure cleanup.

### Evidence e28

[quarantine/mistral-vibe/vibe/app_server/_sessions.py:361–390](../../../quarantine/mistral-vibe/vibe/app_server/_sessions.py#L361): Child turn launched/awaited; cancelled/closed task interrupts child and awaits completion.

### Evidence e29

[quarantine/mistral-vibe/vibe/app_server/_runtime.py:1000–1048](../../../quarantine/mistral-vibe/vibe/app_server/_runtime.py#L1000): Child has copied orchestrator, selected agent profile, own session lease and shared permissions.

### Evidence e30

[quarantine/mistral-vibe/vibe/core/compaction/manager.py:89–110](../../../quarantine/mistral-vibe/vibe/core/compaction/manager.py#L89): Compaction appends boundary/envelope only after summary, preserves old normalized history and saves.

### Evidence e31

[quarantine/mistral-vibe/vibe/core/compaction/context.py:144–161](../../../quarantine/mistral-vibe/vibe/core/compaction/context.py#L144): Model context selects last compaction boundary plus preceding system prompts.

### Evidence e32

[quarantine/mistral-vibe/vibe/core/session/session_logger.py:508–556](../../../quarantine/mistral-vibe/vibe/core/session/session_logger.py#L508): Normalized nonsystem message append when fingerprint boundary unchanged, otherwise current log rewrite.

### Evidence e33

[quarantine/mistral-vibe/vibe/core/session/session_logger.py:413–429](../../../quarantine/mistral-vibe/vibe/core/session/session_logger.py#L413): Rewrite uses temp/fsync/replace, not append-only original history.

### Evidence e34

[quarantine/mistral-vibe/vibe/core/rewind/manager.py:86–120](../../../quarantine/mistral-vibe/vibe/core/rewind/manager.py#L86): Rewind optionally restores files; fork preserves prior log, inplace drops rewound turns and overwrites.

### Evidence e35

[quarantine/mistral-vibe/vibe/core/agent_loop/_loop.py:3815–3867](../../../quarantine/mistral-vibe/vibe/core/agent_loop/_loop.py#L3815): Config reload builds managers off-loop, generation rejects stale preparations and commits synchronously.

### Evidence e36

[quarantine/mistral-vibe/vibe/core/agent_loop/_loop.py:3906–3940](../../../quarantine/mistral-vibe/vibe/core/agent_loop/_loop.py#L3906): Live commit swaps backend/tools/skills/system/hooks; old backend close deferred until active turn ends.

### Evidence e37

[quarantine/mistral-vibe/vibe/core/agent_loop/_loop.py:1480–1506](../../../quarantine/mistral-vibe/vibe/core/agent_loop/_loop.py#L1480): Backend close deferral explicitly assumes subagent inside parent turn; detached child would not be protected.

### Evidence e38

[quarantine/mistral-vibe/vibe/core/agent_loop/_loop.py:2940–2950](../../../quarantine/mistral-vibe/vibe/core/agent_loop/_loop.py#L2940): Bypass permissions immediately grants execution.

### Evidence e39

[quarantine/mistral-vibe/vibe/core/teleport/teleport.py:106–163](../../../quarantine/mistral-vibe/vibe/core/teleport/teleport.py#L106): Git branch/commit checked/push approval then hosted workflow start with packed context; returns URL.

### Evidence e40

[quarantine/mistral-vibe/vibe/core/teleport/orchestrator.py:57–103](../../../quarantine/mistral-vibe/vibe/core/teleport/orchestrator.py#L57): Builds/summarizes supplied message context before teleport service.

### Evidence e41

[quarantine/mistral-vibe/harness/core/src/engine.rs:60–99](../../../quarantine/mistral-vibe/harness/core/src/engine.rs#L60): Rust native session create/restore/apply/inspect/checkpoint; owns state machine, external actions supplied by runtime.

### Evidence e42

[quarantine/mistral-vibe/harness/runtimes/python/python/mistralai_vibe_local_harness/runtime.py:272–313](../../../quarantine/mistral-vibe/harness/runtimes/python/python/mistralai_vibe_local_harness/runtime.py#L272): Simple reference driver schedules actions on asyncio and awaits first completions; external custody not owned by Rust state alone.

### Evidence e43

[quarantine/mistral-vibe/harness/runtimes/python/python/mistralai_vibe_local_harness/runtime.py:333–365](../../../quarantine/mistral-vibe/harness/runtimes/python/python/mistralai_vibe_local_harness/runtime.py#L333): Driver routes emitted completion/hook/native/provided action to Python handlers then applies result event.

### Evidence e44

[quarantine/mistral-vibe/harness/runtimes/python/python/mistralai_vibe_local_harness/runtime.py:448–512](../../../quarantine/mistral-vibe/harness/runtimes/python/python/mistralai_vibe_local_harness/runtime.py#L448): Python model handler stream assembled and explicit action-ID success/failure applied.

### Evidence e45

[quarantine/mistral-vibe/harness/runtimes/python/python/mistralai_vibe_local_harness/runtime.py:575–598](../../../quarantine/mistral-vibe/harness/runtimes/python/python/mistralai_vibe_local_harness/runtime.py#L575): Input ID sequence plus explicit time/random determinism envelope and matching transition check.

### Evidence e46

[quarantine/mistral-vibe/harness/core/src/core/features/programmatic_tool_calling/tools.rs:19–43](../../../quarantine/mistral-vibe/harness/core/src/core/features/programmatic_tool_calling/tools.rs#L19): run_typescript native code-action contract describes isolated replayable orchestration; docs part implemented tool definition.

### Evidence e47

[quarantine/mistral-vibe/harness/core/src/core/features/programmatic_tool_calling/tools.rs:137–150](../../../quarantine/mistral-vibe/harness/core/src/core/features/programmatic_tool_calling/tools.rs#L137): run_typescript dispatch enters owned execution, not merely advertised schema.

### Evidence e48

[quarantine/mistral-vibe/harness/core/src/core/features/programmatic_tool_calling/execution.rs:28–48](../../../quarantine/mistral-vibe/harness/core/src/core/features/programmatic_tool_calling/execution.rs#L28): Starts program with inline code, partial evaluation state and explicit determinism.

### Evidence e49

[quarantine/mistral-vibe/harness/core/src/core/features/programmatic_tool_calling/execution.rs:134–177](../../../quarantine/mistral-vibe/harness/core/src/core/features/programmatic_tool_calling/execution.rs#L134): V8 evaluation returns pending tool effects routed with stable operation/action IDs and hooks.

### Evidence e50

[quarantine/mistral-vibe/harness/core/src/core/features/programmatic_tool_calling/code_mode/v8.rs:80–132](../../../quarantine/mistral-vibe/harness/core/src/core/features/programmatic_tool_calling/code_mode/v8.rs#L80): Owned V8 heap-limit termination guard/watchdog and joined thread lifetime.

### Evidence e51

[quarantine/mistral-vibe/harness/runtimes/python/python/mistralai_vibe_local_harness/vibe/_subagents/_controller.py:936–1033](../../../quarantine/mistral-vibe/harness/runtimes/python/python/mistralai_vibe_local_harness/vibe/_subagents/_controller.py#L936): Configured unified send uses durable receipt, child-generation operation key/admission/acknowledge and watcher; can steer running or start idle child.

### Evidence e52

[quarantine/mistral-vibe/harness/runtimes/python/python/mistralai_vibe_local_harness/vibe/_host.py:525–556](../../../quarantine/mistral-vibe/harness/runtimes/python/python/mistralai_vibe_local_harness/vibe/_host.py#L525): Unified subagent controller only enabled when configured runtime feature; not universal default.

### Evidence e53

[quarantine/mistral-vibe/harness/runtimes/python/python/mistralai_vibe_local_harness/vibe/_host.py:4376–4392](../../../quarantine/mistral-vibe/harness/runtimes/python/python/mistralai_vibe_local_harness/vibe/_host.py#L4376): Bare native-host defaults disable subagents/background/compaction, enable bounded programmatic tools; application may configure later.

### Evidence e54

[quarantine/mistral-vibe/tests/core/compaction/test_compaction_manager.py:113–140](../../../quarantine/mistral-vibe/tests/core/compaction/test_compaction_manager.py#L113): Mock summary oracle verifies previous messages plus appended boundary/user preservation.

### Evidence e55

[quarantine/mistral-vibe/tests/agent_loop/test_hook_reload.py:17–40](../../../quarantine/mistral-vibe/tests/agent_loop/test_hook_reload.py#L17): Mock hook loader tests bound managers/diagnostics after reload, not full workflow/refit.

### Evidence e56

[quarantine/mistral-vibe/harness/core/src/tests/compaction_and_reconfiguration.rs:127–153](../../../quarantine/mistral-vibe/harness/core/src/tests/compaction_and_reconfiguration.rs#L127): Manual compaction checkpoint while completion pending; later compares restored/live projection.

### Evidence e57

[quarantine/mistral-vibe/harness/core/src/tests/compaction_and_reconfiguration.rs:202–224](../../../quarantine/mistral-vibe/harness/core/src/tests/compaction_and_reconfiguration.rs#L202): Actual pure-state oracle compares live/restored checkpoint and next model messages.

### Evidence e58

[quarantine/mistral-vibe/harness/core/src/tests/programmatic_tools.rs:983–1004](../../../quarantine/mistral-vibe/harness/core/src/tests/programmatic_tools.rs#L983): Restored programmatic tool projection stable and excludes private/intermediary/image material; checkpoint equality.

