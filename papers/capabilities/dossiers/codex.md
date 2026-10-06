# codex

Native coding core with configurable tool plans, local/remote environments, PTY handles, code-mode composition, v1/v2 multi-agent APIs and managed local/remote compaction.

Role: coding agent. Runtime: Rust, JavaScript (model code in embedded V8).

Pinned source: [https://github.com/openai/codex](https://github.com/openai/codex); revision/version `08e2b58b07b8a423d6577b66fb7756e980b53dbf`.

Rust Tokio session submission loop/turn tasks, cancellable tool futures and native executor services; optional per-cell V8 runtimes behind code-mode session transport.

Owns local sessions, child agents, executors/terminal handles and optional code-mode worker; consumes provider/MCP/remote executor/code-mode services via explicit transports.

Inspection: Submission/turn/stream dispatch and continuation; executor filesystem/PTY interaction, optional code-mode cells, multi-agent wake modes/permissions, compaction/checkpoint/state activation, audit retention and direct tool tests.

Limits of this study: Source read only, reference not executed. Large feature-gated surface: row inventories native catalogs and representative dispatch mechanisms, not every connector-generated tool or entire provider/platform backend. Original inference trace explicitly omits raw stream deltas; selected rollout event persistence varies by history mode. No executable rebuild/self-improvement machinery traced.

## Actions

### exec_command; shell; shell_command

Surface: model tool.

Input: Command/cwd/environment/shell/login/tty/yield/output; profile-dependent schema

Result: Output/exit or ongoing session ID; errors/events

Lifecycle: Native local/remote exec; PTY or pipes; yielded ongoing work via write_stdin

Authority: Model; standing approval/sandbox/network policy per environment

Evidence: [e5](#evidence-e5), [e6](#evidence-e6), [e7](#evidence-e7), [e23](#evidence-e23).

### write_stdin

Surface: model tool.

Input: session_id, chars, yield_time_ms, max_output_tokens

Result: New output/exit/session status

Lifecycle: Poll or write ongoing process; Ctrl-C bytes possible; cancellation respects executor lifecycle

Authority: Model; stdin approval/policy can reject

Evidence: [e7](#evidence-e7).

### apply_patch

Surface: model tool.

Input: Freeform patch, selected environment

Result: Patch results/diffs/errors

Lifecycle: Verified file changes; streamed preview then execution

Authority: Model; selected filesystem approval/sandbox

Evidence: [e8](#evidence-e8).

### functions.exec; functions.wait (code mode)

Surface: model tool.

Input: Raw JS; pragma yield/output; cell_id/terminate

Result: Text/media/notifications or yielded cell ID and resumed results

Lifecycle: Fresh isolate per cell; serializable session store/load; tool promises awaited; optional runtime feature

Authority: Model; nested enabled tools inherit native authority, no direct FS/network in JS

Evidence: [e9](#evidence-e9), [e10](#evidence-e10), [e11](#evidence-e11).

### spawn_agent; send_message; followup_task; wait_agent; list_agents; interrupt_agent

Surface: model tool.

Input: Task/name/fork/model; canonical/relative target; message; timeout

Result: Agent path/ID, statuses/mailbox/result, interruption state

Lifecycle: V2 independently working agents; queue-only message versus wake-on-idle follow-up; bounded active capacity

Authority: Model; configured delegation/tree/depth/authority rules; direct JS nesting may exclude collab tools

Evidence: [e12](#evidence-e12), [e13](#evidence-e13), [e14](#evidence-e14), [e15](#evidence-e15).

### new_context

Surface: model tool.

Input: No args

Result: New-context request acknowledgement

Lifecycle: Requests fresh window without summarization; environment persists

Authority: Model if configured

Evidence: [e17](#evidence-e17).

### MCP/extension/dynamic/hosted tool catalogs; tool_search; list/read_mcp_resource; view_image; update_plan; request_user_input

Surface: model tool.

Input: Per-tool discovered schema/resource URI/plan/image/user prompt

Result: Native/remote typed tool results and UI responses

Lifecycle: Dispatch through finalized direct/deferred/nested plan; feature/model dependent

Authority: Model plus operator-configured extensions/services; catalog represents generated tools

Evidence: [e5](#evidence-e5), [e27](#evidence-e27).

### Op::TurnInput; Op::Interrupt; /compact; /feedback

Surface: operator command.

Input: Prompt/turn settings; cancellation; summary instructions; feedback/log choice

Result: Streamed events, compact checkpoint, feedback UI

Lifecycle: Submission loop independent of task; task cancellation versus background-terminal cleanup distinguished

Authority: Operator; /feedback not autonomous model complaint

Evidence: [e1](#evidence-e1), [e2](#evidence-e2), [e16](#evidence-e16), [e26](#evidence-e26).

### search_rollout_matches; active next-step settings update

Surface: programmable API.

Input: Search term/home/archive; validated model/settings/environments update

Result: Matching saved paths/snippets; Applied/Rejected/Unavailable

Lifecycle: Query retained rollout; next sampling step settings snapshot, old tool retains original step

Authority: Client/harness extension/operator authority

Evidence: [e18](#evidence-e18), [e22](#evidence-e22), [e4](#evidence-e4).

## Capabilities

### filesystem

**I — Files** (source): Native patch plus shell reads/search and selected local/remote executor filesystem; output/tool schemas feature-dependent.

Evidence: [e6](#evidence-e6), [e8](#evidence-e8).

### processes

**I — OS programs** (source): PTY/pipe exec, yielded session IDs and write_stdin polling/input with per-environment policy.

Evidence: [e6](#evidence-e6), [e7](#evidence-e7).

### code-actions

**I — Code actions** (source): Optional raw JS code-mode cells compose enabled tools with Promise orchestration, store/load and yield/wait.

Evidence: [e9](#evidence-e9), [e10](#evidence-e10), [e11](#evidence-e11).

### persistent-kernel

**L — Kernel** (source): Fresh V8 isolate each cell; serializable store/load shares values across session, not arbitrary persistent interpreter namespace or OS handles.

Evidence: [e10](#evidence-e10), [e11](#evidence-e11).

### standing-database

**S — Standing DB** (source): Serialized code-mode key/value store and local retained rollouts supply state primitives; internal thread DB is not established as a standing model query service.

Evidence: [e10](#evidence-e10), [e11](#evidence-e11), [e19](#evidence-e19), [e22](#evidence-e22).

### workflow-programming

**S — Workflows** (source): Model code-mode programs and extension request/stream hooks allow composition; unrestricted replaceable turn-scheduler program not established.

Evidence: [e9](#evidence-e9), [e11](#evidence-e11), [e27](#evidence-e27).

### multi-model

**I — Models** (source): Child model override and fork selection; provider/client abstraction with request-scoped metadata.

Evidence: [e12](#evidence-e12), [e27](#evidence-e27).

### live-collaboration

**I — Peer chat** (source): V2 addressed canonical task paths; send_message queues without waking, followup_task can wake idle; messages retain author/recipient and wake mode.

Evidence: [e12](#evidence-e12), [e13](#evidence-e13), [e14](#evidence-e14), [e15](#evidence-e15).

### concurrent-work

**I — Concurrency** (source): Independent child sessions, yielded processes/cells and selected parallel tool calls; per-step immutable tool advertising retained.

Evidence: [e4](#evidence-e4), [e7](#evidence-e7), [e9](#evidence-e9), [e12](#evidence-e12).

### steering-interrupt

**I — Steer/interrupt** (source): Independent submission loop handles interrupts; V2 messages deliberately cannot secretly interrupt, separate interrupt action; process/cell cancellation explicit.

Evidence: [e1](#evidence-e1), [e4](#evidence-e4), [e7](#evidence-e7), [e25](#evidence-e25).

### turn-redefinition

**S — Turn program** (source): Next-step model/settings/environment activation and compiled extension request/response contributors exist; arbitrary model replacement of native loop not traced.

Evidence: [e18](#evidence-e18), [e27](#evidence-e27).

### compaction

**I — Compaction** (source): Managed pre-turn/manual/mid-turn local/remote variants, checkpoint lineage and controlled initial-context reinjection; model fresh-window action distinct.

Evidence: [e2](#evidence-e2), [e16](#evidence-e16), [e17](#evidence-e17).

### context-repair

**S — Repair** (source): Saved rollout/query and retained-context checkpoints make originals partly retrievable; no model-native repair-originals operation traced.

Evidence: [e16](#evidence-e16), [e19](#evidence-e19), [e22](#evidence-e22).

### original-audit

**L — Original audit** (source): Optional trace captures request-attempt/tool/runtime/terminal/checkpoint payload references; inference explicitly summarizes completed items rather than raw stream deltas; selected persistence is not everything.

Evidence: [e19](#evidence-e19), [e20](#evidence-e20), [e21](#evidence-e21).

### audit-query

**I — Audit query** (source): Program API searches plain/compressed saved/archived rollouts; scope is retained rollout text, not complete transport/program audit.

Evidence: [e22](#evidence-e22).

### hot-change

**I — Hot change** (source): Next-step settings/environment changes revalidate authority and task identity; old tool call keeps advertising StepContext. Executable replacement not included.

Evidence: [e4](#evidence-e4), [e18](#evidence-e18).

### rebuild-continuity

**? — Rebuild continuity** (source): Thread/rollout resume and code-mode reconnect are present primitives; actual self-rebuild/quiescent handoff and re-inhabitation not traced in inspected lifecycle files.

### remote-services

**I — Remote** (source): Native executor abstraction supports remote paths/permission enforcement and code-mode session transport; consumes provider/MCP services.

Evidence: [e6](#evidence-e6), [e9](#evidence-e9).

### self-improvement

**? — Self-improve** (source): Generic file/code actions and extensions exist; dedicated change/evaluate/rebuild experiment governance not established in inspected turn/tool/config paths.

### complaints

**L — Complaints** (source): Operator /feedback sends logs; no model-invokable state-capturing complaint/database follow-up affordance traced.

Evidence: [e26](#evidence-e26).

### authority

**I — Authority** (source): Standing approval policy plus filesystem/network profiles; selected-environment login authority enforced; expert full-access/never configurations exist but managed constraints revalidated.

Evidence: [e6](#evidence-e6), [e18](#evidence-e18), [e23](#evidence-e23), [e24](#evidence-e24).

### evaluation

**I — Evaluation** (source): Specific handler authority and message non-interruption oracles inspected; mock/internal dispatch assertions do not validate all OS/provider backends.

Evidence: [e24](#evidence-e24), [e25](#evidence-e25).

### time-order

**I — Time/order** (source): Tool invocation Instant measurements, call/turn/cell IDs and retained step identity; these are local lifecycle ordering primitives rather than global audit completeness.

Evidence: [e4](#evidence-e4), [e9](#evidence-e9), [e18](#evidence-e18), [e20](#evidence-e20).

## Inspected test oracles

- [quarantine/codex/codex-rs/core/src/tools/handlers/unified_exec_tests.rs](../../../quarantine/codex/codex-rs/core/src/tools/handlers/unified_exec_tests.rs): Environment-specific command authority Oracle: Login request rejected based on selected environment even when turn policy allows login; actual handler invoked with fixture. Read, **not executed**.
- [quarantine/codex/codex-rs/core/src/tools/handlers/multi_agents_tests.rs](../../../quarantine/codex/codex-rs/core/src/tools/handlers/multi_agents_tests.rs): Accidental message interrupt Oracle: Unknown interrupt field yields model-facing error and captured worker Ops contain no interrupt; internal-thread fixture, no live model. Read, **not executed**.

## Useful mechanisms

- Immutable advertising-step retention avoids changing authority/tool definitions under pending calls.
- Queue-only message and trigger-turn task are deliberately separate operations.
- Optional trace separates original payload storage from reduced graph/UI.

## Material limits

- Large feature-gated surface: row inventories native catalogs and representative dispatch mechanisms, not every connector-generated tool or entire provider/platform backend. Original inference trace explicitly omits raw stream deltas; selected rollout event persistence varies by history mode. No executable rebuild/self-improvement machinery traced.

## Arconaut design questions

- Should our generic code action use a session kernel or fresh isolate with explicit serialized storage?
- How should Arconaut originals include every provider delta/failed attempt and unbounded program streams beyond this trace scope?
- What activation boundary preserves old workflows while applying model-authored turn-program changes?

## Evidence

### Evidence e1

[quarantine/codex/codex-rs/core/src/session/handlers.rs:422–506](../../../quarantine/codex/codex-rs/core/src/session/handlers.rs#L422): Independent submission loop processes interrupt/input while turn tasks exist.

### Evidence e2

[quarantine/codex/codex-rs/core/src/session/turn.rs:163–249](../../../quarantine/codex/codex-rs/core/src/session/turn.rs#L163): Turn creates model client session, runs pre-sampling compaction and preserves input even on compaction failure.

### Evidence e3

[quarantine/codex/codex-rs/core/src/session/turn.rs:2775–2823](../../../quarantine/codex/codex-rs/core/src/session/turn.rs#L2775): Completed stream items dispatch tool/output handling and set continuation need.

### Evidence e4

[quarantine/codex/codex-rs/core/src/tools/parallel.rs:44–164](../../../quarantine/codex/codex-rs/core/src/tools/parallel.rs#L44): Tool runtime retains advertising step; cancellable futures and parallel eligibility choose execution locks.

### Evidence e5

[quarantine/codex/codex-rs/core/src/tools/router.rs:47–181](../../../quarantine/codex/codex-rs/core/src/tools/router.rs#L47): Exposed tool plan supports direct/code-mode/deferred tools and distinct terminal/child capability checks.

### Evidence e6

[quarantine/codex/codex-rs/core/src/tools/handlers/unified_exec/exec_command.rs:155–245](../../../quarantine/codex/codex-rs/core/src/tools/handlers/unified_exec/exec_command.rs#L155): Exec routes through selected local/remote environment filesystem and permission profile.

### Evidence e7

[quarantine/codex/codex-rs/core/src/tools/handlers/unified_exec/write_stdin.rs:58–122](../../../quarantine/codex/codex-rs/core/src/tools/handlers/unified_exec/write_stdin.rs#L58): write_stdin uses ongoing process/session ID, yield/output limits, cancellation and approval errors.

### Evidence e8

[quarantine/codex/codex-rs/core/src/tools/handlers/apply_patch.rs:63–121](../../../quarantine/codex/codex-rs/core/src/tools/handlers/apply_patch.rs#L63): Patch handler supports selected environment filesystem and streamed argument parsing.

### Evidence e9

[quarantine/codex/codex-rs/core/src/tools/code_mode/execute_handler.rs:86–181](../../../quarantine/codex/codex-rs/core/src/tools/code_mode/execute_handler.rs#L86): Exec submits raw JS to code service, holds cell ID, yields/resumes and traces original cell request/runtime response.

### Evidence e10

[quarantine/codex/codex-rs/code-mode-runtime/src/runtime/mod.rs:175–235](../../../quarantine/codex/codex-rs/code-mode-runtime/src/runtime/mod.rs#L175): Each cell starts V8 isolate/context; receives serialized stored values and pending tool/timer state.

### Evidence e11

[quarantine/codex/codex-rs/code-mode-protocol/src/description.rs:19–46](../../../quarantine/codex/codex-rs/code-mode-protocol/src/description.rs#L19): Model surface describes fresh isolate, no direct OS/network, serializable store/load and Promise lifetime.

### Evidence e12

[quarantine/codex/codex-rs/core/src/tools/handlers/multi_agents_v2/spawn.rs:101–200](../../../quarantine/codex/codex-rs/core/src/tools/handlers/multi_agents_v2/spawn.rs#L101): Spawn resolves model/fork mode/depth and durable canonical agent path.

### Evidence e13

[quarantine/codex/codex-rs/core/src/tools/handlers/multi_agents_v2/send_message.rs:32–52](../../../quarantine/codex/codex-rs/core/src/tools/handlers/multi_agents_v2/send_message.rs#L32): send_message maps to QueueOnly addressed delivery.

### Evidence e14

[quarantine/codex/codex-rs/core/src/tools/handlers/multi_agents_v2/followup_task.rs:32–52](../../../quarantine/codex/codex-rs/core/src/tools/handlers/multi_agents_v2/followup_task.rs#L32): followup_task maps to TriggerTurn addressed delivery.

### Evidence e15

[quarantine/codex/codex-rs/core/src/agent/control/delivery.rs:11–43](../../../quarantine/codex/codex-rs/core/src/agent/control/delivery.rs#L11): Message mode preserves author/recipient and wake flag; messages and new tasks distinct.

### Evidence e16

[quarantine/codex/codex-rs/core/src/compact.rs:59–96](../../../quarantine/codex/codex-rs/core/src/compact.rs#L59): Manual/pre-turn compaction clears context baseline; mid-turn injects initial context before last user message; checkpoint metadata separate.

### Evidence e17

[quarantine/codex/codex-rs/core/src/tools/handlers/new_context_window.rs:14–48](../../../quarantine/codex/codex-rs/core/src/tools/handlers/new_context_window.rs#L14): Model new_context requests empty context window without summarizing and leaves environment state.

### Evidence e18

[quarantine/codex/codex-rs/core/src/session/step_activation.rs:297–378](../../../quarantine/codex/codex-rs/core/src/session/step_activation.rs#L297): Updates validate current task identity and managed authority, then install next-step settings/environments atomically.

### Evidence e19

[quarantine/codex/codex-rs/rollout/src/policy.rs:10–65](../../../quarantine/codex/codex-rs/rollout/src/policy.rs#L10): Durable rollout stores selected response items/context/compaction/inter-agent communications; unknown item classes excluded.

### Evidence e20

[quarantine/codex/codex-rs/rollout-trace/src/inference.rs:1–96](../../../quarantine/codex/codex-rs/rollout-trace/src/inference.rs#L1): Optional request-attempt trace; explicit non-delta completed/interrupted output summary, not raw streamed deltas.

### Evidence e21

[quarantine/codex/codex-rs/rollout-trace/src/payload.rs:9–49](../../../quarantine/codex/codex-rs/rollout-trace/src/payload.rs#L9): Trace references separate original request/results/runtime/terminal/compaction payload files.

### Evidence e22

[quarantine/codex/codex-rs/rollout/src/search.rs:25–100](../../../quarantine/codex/codex-rs/rollout/src/search.rs#L25): Program API searches saved/archived plain or compressed rollouts using rg or scanning fallback.

### Evidence e23

[quarantine/codex/codex-rs/core/src/tools/orchestrator.rs:125–173](../../../quarantine/codex/codex-rs/core/src/tools/orchestrator.rs#L125): Execution authority combines standing approval policy and filesystem/network sandbox profile.

### Evidence e24

[quarantine/codex/codex-rs/core/src/tools/handlers/unified_exec_tests.rs:356–395](../../../quarantine/codex/codex-rs/core/src/tools/handlers/unified_exec_tests.rs#L356): Oracle rejects login shell when selected environment forbids it even if turn allows it.

### Evidence e25

[quarantine/codex/codex-rs/core/src/tools/handlers/multi_agents_tests.rs:1858–1920](../../../quarantine/codex/codex-rs/core/src/tools/handlers/multi_agents_tests.rs#L1858): V2 send-message rejects interrupt argument and verifies no interrupt Op emitted.

### Evidence e26

[quarantine/codex/codex-rs/tui/src/slash_command.rs:69–95](../../../quarantine/codex/codex-rs/tui/src/slash_command.rs#L69): Feedback command sends logs to maintainers; not a model-state complaint tool.

### Evidence e27

[quarantine/codex/codex-rs/core/src/model_request.rs:12–46](../../../quarantine/codex/codex-rs/core/src/model_request.rs#L12): Extension contributors can modify request metadata/intercept response streams.

