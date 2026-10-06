# goose

Extensible Rust agent with a concrete optional composable state machine, dynamic MCP tools, independent multi-provider delegation, recipes/scheduling and bounded cross-session orchestration.

Role: coding agent. Runtime: Rust, TypeScript.

Pinned source: [https://github.com/aaif-goose/goose](https://github.com/aaif-goose/goose); revision/version `bab8ff641039c9cd3331121cd84a5c6045f365ca`.

Rust multithreaded Tokio CLI/core; optional Deno/pctx code-mode runs on blocking thread; MCP local stdio/builtin and remote services; desktop UI outside core trace.

Consumes provider/MCP tools; owns sessions/SQLite conversation and optional schedule/child lifecycle; pctx kernel internals external dependency.

Inspection: CLI entry; old stream loop and optional operation pipeline; provider/action/result effect flow; native file/shell and code-mode wrapper, delegate/orchestrator, recipes/scheduler, compaction retention, recall/telemetry and selected source oracles.

Limits of this study: Source read only, reference not executed. Optional code-mode dependency runtime internals, desktop UI and all extensions/providers not read. State-machine path is opt-in; default older loop inspected in core request/dispatch/cancellation segments. Full original event logging, executable refit and governed self-improvement not traced.

## Actions

### write / edit / shell / tree / read_image

Surface: model tools.

Input: Structured file replacement/write, shell command/timeout, tree path or image path

Result: File outcomes, structured stdout/stderr/exit/truncation metadata or image

Lifecycle: Awaited actions; shell emits live notifications, cancel kills immediate child and drains streams for 500ms

Authority: Goose mode/inspectors, enabled developer extension, session working directory

Evidence: [e10](#evidence-e10), [e11](#evidence-e11), [e30](#evidence-e30), [e31](#evidence-e31), [e36](#evidence-e36).

### list_functions / get_function_details / execute_typescript / execute_bash

Surface: model tools.

Input: Function filters or TypeScript/code+disclosure/tool graph; bash command

Result: Schemas or code outcome Markdown; nested extension tool results

Lifecycle: pctx code-mode calls on blocking thread; extension timeout/cancel propagated to callback token

Authority: Enabled code-mode extension; underlying pctx implementation not in inspected snapshot

Evidence: [e12](#evidence-e12), [e13](#evidence-e13), [e36](#evidence-e36).

### load / delegate

Surface: model tools.

Input: Sources/named recipe/agent or instructions; provider/model/context/extensions, async; task ID and peek/cancel

Result: Loaded instructions or independent child result/background task status

Lifecycle: Separate session/context; async handle supports wait/peek/cancel; live notifications; no nested delegation

Authority: Parent model may choose provider/model; children forced Auto because approval forwarding unavailable

Evidence: [e14](#evidence-e14), [e15](#evidence-e15), [e16](#evidence-e16), [e33](#evidence-e33), [e36](#evidence-e36).

### orchestrator__list_sessions / view_session / start_agent / send_message / interrupt_agent

Surface: model tools.

Input: Session filters/id, working directory/name, message text

Result: Sessions, summaries/endpoints, new session ID, reply text, cancellation acknowledgement

Lifecycle: Messaging actually drives target reply and awaits it; busy target rejected rather than queued

Authority: Target authorization checked; start uses parent provider/model

Evidence: [e17](#evidence-e17), [e18](#evidence-e18).

### scheduler__manage_schedule

Surface: model tool.

Input: action=list/create/run_now/pause/resume/remove/update/inspect; recipe/schedule parameters

Result: Schedule record or produced session inspection

Lifecycle: Optional configured scheduler; recurring recipe lifecycle

Authority: Model-enabled platform tool; scheduler owns schedule execution

Evidence: [e20](#evidence-e20), [e21](#evidence-e21).

### extensionmanager__search_available_extensions / manage_extensions / list_resources / read_resource

Surface: model tools.

Input: Available extension name enable/disable, resource URI

Result: Discovered extension/tools/resource content or errors

Lifecycle: Current manager can change connected extensions; schema-hash invalidates code-mode cache

Authority: Parent user sessions allowed; subagents denied; cannot disable manager itself

Evidence: [e12](#evidence-e12), [e28](#evidence-e28), [e29](#evidence-e29), [e34](#evidence-e34).

### chatrecall__chatrecall

Surface: model tool.

Input: query/date filters or session_id

Result: Messages grouped by session or bounded first/last three

Lifecycle: Saved-history search, not raw original-event replay

Authority: Model with extension; stored conversation visibility boundaries

Evidence: [e25](#evidence-e25), [e26](#evidence-e26).

### Agent.reply / StateMachine.step / apply / run / Operation / Inference

Surface: embedding program API.

Input: Message/session/cancel, ordered operations/effect handler/provider

Result: Event stream or one applicable step/effects; persisted conversation continuation

Lifecycle: Explicit optional machine; concrete host pipeline remains compiled/internal

Authority: Embedding author can compose/redefine pipelines; not model hot replacement

Evidence: [e2](#evidence-e2), [e4](#evidence-e4), [e5](#evidence-e5), [e6](#evidence-e6), [e9](#evidence-e9).

### /compact / /clear / recipe commands and YAML/JSON recipes

Surface: operator/program surfaces.

Input: Conversation transformation or parameterized recipe/source

Result: Summary/replaced history or configured independent run

Lifecycle: Managed summary includes completed tool-pair constraint; declarative model/extension/output config

Authority: Operator and model delegation source loading; recipe does not redefine compiled turn protocol

Evidence: [e19](#evidence-e19), [e23](#evidence-e23), [e24](#evidence-e24).

### analyze / todo__todo_write / summarize__summarize; optional apps catalog

Surface: model tool catalogs.

Input: Code structure path/mode; full TODO replacement; paths/question/extensions for summary; feature-dependent apps schemas

Result: Structure, persisted TODO or model summary; app tool internals not individually traced

Lifecycle: Registered native/feature-gated extension actions; generated/plugin catalogs remain individually discoverable

Authority: Enabled/feature flags and mode; representative catalog inspection rather than full media/app implementation trace

Evidence: [e36](#evidence-e36), [e37](#evidence-e37), [e38](#evidence-e38), [e39](#evidence-e39).

## Capabilities

### filesystem

**I — Files** (source): Native write/edit/tree/image and shell provide local files; exact unique text edits via structured params.

Evidence: [e10](#evidence-e10).

### processes

**L — OS programs** (source): Streamed awaited shell command with timeout/cancel and output artifacts; immediate-child kill and no writable PTY/background shell handle in traced developer surface.

Evidence: [e10](#evidence-e10), [e11](#evidence-e11).

### code-actions

**I — Code actions** (source): Feature-dependent pctx TypeScript/bash code-mode invokes current extension callbacks; wrapper execution/cancellation traced.

Evidence: [e12](#evidence-e12), [e13](#evidence-e13).

### persistent-kernel

**? — Kernel** (source): pctx CodeMode wrapper/schema cache inspected; dependency runtime implementation/namespace survival not present in this bounded trace. Cached callbacks do not establish a persistent notebook kernel.

### standing-database

**S — Standing DB** (source): SQLite session storage and chat-history querying are standing internal data primitives, not arbitrary shared external research database action.

Evidence: [e25](#evidence-e25), [e26](#evidence-e26).

### workflow-programming

**I — Workflows** (source): Public Operation/Inference/StateMachine pipeline composition plus parameterized recipes, retries/subrecipes and optional scheduling; concrete Goose operations internal.

Evidence: [e4](#evidence-e4), [e5](#evidence-e5), [e6](#evidence-e6), [e19](#evidence-e19), [e20](#evidence-e20), [e21](#evidence-e21).

### multi-model

**I — Models** (source): Delegate supports provider/model overrides and own context; orchestrator start separately inherits parent selection.

Evidence: [e14](#evidence-e14), [e16](#evidence-e16), [e18](#evidence-e18).

### live-collaboration

**L — Peer chat** (source): Orchestrator exposes session inspection, send and interrupt; sending awaits target turn and rejects busy recipients. Independent delegates explicitly lack peer coordination.

Evidence: [e15](#evidence-e15), [e17](#evidence-e17).

### concurrent-work

**I — Concurrency** (source): Tool result/notification streams selected together; async delegation has separate handles and can overlap; no shared-write arbitration established.

Evidence: [e8](#evidence-e8), [e14](#evidence-e14), [e15](#evidence-e15).

### steering-interrupt

**I — Steer/interrupt** (source): Between-turn FIFO steering, provider/tool cancellation and scripted cancellation-resume tests; peer send is not queued busy steering.

Evidence: [e3](#evidence-e3), [e8](#evidence-e8), [e17](#evidence-e17), [e22](#evidence-e22), [e32](#evidence-e32).

### turn-redefinition

**L — Turn program** (source): Embedding authors can supply ordered Operation/Inference implementations; shipped concrete host steps are internal and opt-in machine disabled by default. Model tool/recipe changes do not replace turn kernel.

Evidence: [e4](#evidence-e4), [e5](#evidence-e5), [e6](#evidence-e6), [e19](#evidence-e19).

### compaction

**I — Compaction** (source): Automatic/manual summary, pair-result token accounting, incomplete-pair guard and persisted visibility changes retain original message content.

Evidence: [e23](#evidence-e23), [e24](#evidence-e24), [e25](#evidence-e25).

### context-repair

**S — Repair** (source): Saved message visibility graph and chatrecall bounded history reads allow recovery composition; no model-operated precise context lineage repair traced.

Evidence: [e25](#evidence-e25), [e26](#evidence-e26).

### original-audit

**L — Original audit** (source): Stored conversations retain pre-compaction content, but optional telemetry and reconstructed finish reasons explicitly limit original wire evidence; all program/provider stream events not established.

Evidence: [e25](#evidence-e25), [e27](#evidence-e27).

### audit-query

**L — Audit query** (source): Model chatrecall searches saved messages and bounded session endpoints; it is not complete original-audit query.

Evidence: [e25](#evidence-e25), [e26](#evidence-e26), [e27](#evidence-e27).

### hot-change

**L — Hot change** (source): Parent model can enable/disable configured extensions and schema-hash refresh code-mode callbacks; subagent restrictions and compiled machine remain. No executable refit continuity traced.

Evidence: [e12](#evidence-e12), [e28](#evidence-e28), [e29](#evidence-e29), [e34](#evidence-e34).

### rebuild-continuity

**? — Rebuild continuity** (source-inspection): Not established in the inspected entry, model loop, action dispatch and state paths; no universal absence claim.

### remote-services

**I — Remote** (source): Remote MCP streamable HTTP client implementation and provider/extension dispatch consume outside services; ownership varies with stdio/builtin clients.

Evidence: [e7](#evidence-e7), [e28](#evidence-e28), [e35](#evidence-e35).

### self-improvement

**S — Self-improve** (source): Writable source/recipes and model extension reconfiguration support operator-composed improvement; measured governed executable autoresearch not established.

Evidence: [e10](#evidence-e10), [e19](#evidence-e19), [e28](#evidence-e28).

### complaints

**? — Complaints** (source-inspection): Not established in the inspected entry, model loop, action dispatch and state paths; no universal absence claim.

### authority

**I — Authority** (source): Auto default with Ask/SmartApprove/Chat alternatives; inspector executable decisions persisted. Delegates forced Auto due absent action-required forwarding.

Evidence: [e16](#evidence-e16), [e30](#evidence-e30), [e31](#evidence-e31).

### evaluation

**S — Evaluation** (source): Specific scripted provider and extension routing oracles read; no provider-performance result or runtime conformance inferred.

Evidence: [e32](#evidence-e32), [e33](#evidence-e33), [e34](#evidence-e34).

### time-order

**I — Time/order** (source): Live event generated IDs and durable message/effect boundaries; steering FIFO; background Instant state. No total distributed audit order guarantee.

Evidence: [e2](#evidence-e2), [e9](#evidence-e9), [e14](#evidence-e14), [e22](#evidence-e22).

## Inspected test oracles

- [quarantine/goose/crates/goose/src/agents/state_machine/tests/steering_lifecycle.rs](../../../quarantine/goose/crates/goose/src/agents/state_machine/tests/steering_lifecycle.rs): Steer ordering and cancellation ownership Oracle: Scripted held provider responses verify both FIFO steers in next request, compaction system includes redirected guidance, cancel retains queue and resume delivers exactly once. Read, **not executed**.
- [quarantine/goose/crates/goose/src/agents/platform_extensions/summon.rs](../../../quarantine/goose/crates/goose/src/agents/platform_extensions/summon.rs): Child live notification ordering Oracle: Repeated synthetic stream test asserts three child-tagged notifications arrive before delegate result; notification pipeline oracle, not child/provider execution test. Read, **not executed**.
- [quarantine/goose/crates/goose/src/agents/platform_extensions/ext_manager.rs](../../../quarantine/goose/crates/goose/src/agents/platform_extensions/ext_manager.rs): Dynamic extension actor restrictions Oracle: Calls management directly with persisted user/subagent session types and checks enabled manager state changes only for user. Read, **not executed**.

## Useful mechanisms

- Reusable first-applicable-step/effects protocol can be composed independently of UI.
- Compaction preserves old stored message content through visibility metadata.
- Model extension discovery and reconfiguration have actual dispatch.
- Live delegate notification and queued-steering tests assert meaningful ordering.

## Material limits

- Optional code-mode dependency runtime internals, desktop UI and all extensions/providers not read. State-machine path is opt-in; default older loop inspected in core request/dispatch/cancellation segments. Full original event logging, executable refit and governed self-improvement not traced.

## Arconaut design questions

- Can Arconaut expose a programmable turn definition to the model without shipping an immutable host pipeline?
- Should busy peer messages queue with durable receipts instead of reject?
- How should externally owned processes and shared kernels expose cancellable handles beyond bounded shell calls?
- What original provider/program event loss would a trace replay oracle detect beyond saved conversation?
- How should async delegation authority remain explicit when approval forwarding is unsupported?

## Evidence

### Evidence e1

[quarantine/goose/crates/goose-cli/src/main.rs:19–56](../../../quarantine/goose/crates/goose-cli/src/main.rs#L19): CLI owns a multithreaded Tokio runtime on dedicated main thread.

### Evidence e2

[quarantine/goose/crates/goose/src/agents/agent.rs:2079–2174](../../../quarantine/goose/crates/goose/src/agents/agent.rs#L2079): Reply supplies live event identities and explicit opt-in state-machine versus existing loop choice.

### Evidence e3

[quarantine/goose/crates/goose/src/agents/agent.rs:2720–2785](../../../quarantine/goose/crates/goose/src/agents/agent.rs#L2720): Existing loop requests provider stream and cancellation-selects streamed events.

### Evidence e4

[quarantine/goose/crates/goose-agent/src/machine.rs:15–155](../../../quarantine/goose/crates/goose-agent/src/machine.rs#L15): Public composable operations/inference pipeline with first applicable effect step and cancellation protocol.

### Evidence e5

[quarantine/goose/crates/goose/src/agents/state_machine/mod.rs:1–76](../../../quarantine/goose/crates/goose/src/agents/state_machine/mod.rs#L1): Generic machine exported; concrete Goose operations internal and feature flag false by default.

### Evidence e6

[quarantine/goose/crates/goose/src/agents/agent.rs:1646–1768](../../../quarantine/goose/crates/goose/src/agents/agent.rs#L1646): Actual ordered pipeline installs compaction, approval, skills, recipes, tool execution, retries and inference.

### Evidence e7

[quarantine/goose/crates/goose/src/agents/state_machine/ops_llm.rs:119–168](../../../quarantine/goose/crates/goose/src/agents/state_machine/ops_llm.rs#L119): Provider adapter prepares toolshim context, dispatches stream and records advertised schemas on assistant requests.

### Evidence e8

[quarantine/goose/crates/goose/src/agents/state_machine/ops_toolcalling.rs:897–985](../../../quarantine/goose/crates/goose/src/agents/state_machine/ops_toolcalling.rs#L897): Pending executable tool futures/notifications selected together; IDs attach result metadata.

### Evidence e9

[quarantine/goose/crates/goose/src/agents/state_machine/session.rs:30–133](../../../quarantine/goose/crates/goose/src/agents/state_machine/session.rs#L30): Effects persisted before confirmation events; compaction history-replacement events after storage.

### Evidence e10

[quarantine/goose/crates/goose/src/agents/platform_extensions/developer/mod.rs:108–254](../../../quarantine/goose/crates/goose/src/agents/platform_extensions/developer/mod.rs#L108): Native write/edit/shell/tree/read_image catalog and dispatcher using session/cwd.

### Evidence e11

[quarantine/goose/crates/goose/src/agents/platform_extensions/developer/shell.rs:554–665](../../../quarantine/goose/crates/goose/src/agents/platform_extensions/developer/shell.rs#L554): Shell awaits process with timeout/cancel then bounded output drain; kills immediate child, no process-tree proof.

### Evidence e12

[quarantine/goose/crates/goose/src/agents/platform_extensions/code_execution.rs:116–187](../../../quarantine/goose/crates/goose/src/agents/platform_extensions/code_execution.rs#L116): Code-mode interface cached by current callback-schema hash and registry routes named extension tools.

### Evidence e13

[quarantine/goose/crates/goose/src/agents/platform_extensions/code_execution.rs:215–355](../../../quarantine/goose/crates/goose/src/agents/platform_extensions/code_execution.rs#L215): execute_bash/execute_typescript invoke pctx interface on blocking-thread Tokio runtime; cancellation token drains nested callbacks with bounded grace.

### Evidence e14

[quarantine/goose/crates/goose/src/agents/platform_extensions/summon.rs:79–106](../../../quarantine/goose/crates/goose/src/agents/platform_extensions/summon.rs#L79): Delegate accepts provider/model/context/extensions plus async; background tasks have handles/cancel/notification state.

### Evidence e15

[quarantine/goose/crates/goose/src/agents/platform_extensions/summon.rs:700–795](../../../quarantine/goose/crates/goose/src/agents/platform_extensions/summon.rs#L700): Model load and delegate interfaces expose source discovery, task wait/cancel/peek; isolated delegation does not coordinate.

### Evidence e16

[quarantine/goose/crates/goose/src/agents/platform_extensions/summon.rs:1345–1420](../../../quarantine/goose/crates/goose/src/agents/platform_extensions/summon.rs#L1345): Actual delegation rejects nested delegation and configures Auto mode because parent approval forwarding unavailable.

### Evidence e17

[quarantine/goose/crates/goose/src/agents/platform_extensions/orchestrator.rs:502–636](../../../quarantine/goose/crates/goose/src/agents/platform_extensions/orchestrator.rs#L502): Cross-session messaging authorizes, rejects busy recipient, runs reply to completion and propagates parent cancellation; separate interrupt.

### Evidence e18

[quarantine/goose/crates/goose/src/agents/platform_extensions/orchestrator.rs:82–106](../../../quarantine/goose/crates/goose/src/agents/platform_extensions/orchestrator.rs#L82): start_agent inherits orchestrator provider/model; per-start model tier unimplemented.

### Evidence e19

[quarantine/goose/crates/goose/src/recipe/mod.rs:40–130](../../../quarantine/goose/crates/goose/src/recipe/mod.rs#L40): Recipe structured instructions, extensions, parameters, settings/model, output schema, retries and sub-recipes.

### Evidence e20

[quarantine/goose/crates/goose/src/agents/schedule_tool.rs:76–120](../../../quarantine/goose/crates/goose/src/agents/schedule_tool.rs#L76): Actual scheduler action dispatcher for create/list/run_now/pause/resume/remove/update/inspect results.

### Evidence e21

[quarantine/goose/crates/goose/src/agents/platform_extensions/scheduler.rs:24–75](../../../quarantine/goose/crates/goose/src/agents/platform_extensions/scheduler.rs#L24): Scheduler native surface appears only when a scheduler implementation exists.

### Evidence e22

[quarantine/goose/crates/goose/src/agents/state_machine/ops_steer.rs:1–78](../../../quarantine/goose/crates/goose/src/agents/state_machine/ops_steer.rs#L1): FIFO queue applied between model/tool turns with prompt hooks and persisted message effects.

### Evidence e23

[quarantine/goose/crates/goose/src/agents/state_machine/ops_compaction.rs:51–88](../../../quarantine/goose/crates/goose/src/agents/state_machine/ops_compaction.rs#L51): Compaction checks incomplete tool pair and counts unreported result tokens.

### Evidence e24

[quarantine/goose/crates/goose/src/agents/state_machine/ops_compaction.rs:178–235](../../../quarantine/goose/crates/goose/src/agents/state_machine/ops_compaction.rs#L178): Operator compact/clear and model summary compacted conversation replacement effects.

### Evidence e25

[quarantine/goose/crates/goose/src/session/session_manager.rs:2016–2070](../../../quarantine/goose/crates/goose/src/session/session_manager.rs#L2016): Compaction storage transaction updates visibility of old message IDs and inserts new summaries; content retained.

### Evidence e26

[quarantine/goose/crates/goose/src/agents/platform_extensions/chatrecall.rs:265–291](../../../quarantine/goose/crates/goose/src/agents/platform_extensions/chatrecall.rs#L265): Model chatrecall searches past chat/date filters or loads bounded session endpoints.

### Evidence e27

[quarantine/goose/crates/goose/src/agents/gen_ai_telemetry.rs:10–43](../../../quarantine/goose/crates/goose/src/agents/gen_ai_telemetry.rs#L10): Message content capture is optional; reconstructed output finish reason is not original provider finish reason.

### Evidence e28

[quarantine/goose/crates/goose/src/agents/platform_extensions/ext_manager.rs:150–232](../../../quarantine/goose/crates/goose/src/agents/platform_extensions/ext_manager.rs#L150): Model enable/disable dispatch; subagents cannot manage and manager cannot disable itself.

### Evidence e29

[quarantine/goose/crates/goose/src/agents/platform_extensions/ext_manager.rs:299–350](../../../quarantine/goose/crates/goose/src/agents/platform_extensions/ext_manager.rs#L299): Model extension search/manage tools advertised.

### Evidence e30

[quarantine/goose/crates/goose/src/agents/state_machine/ops_tool_approval.rs:44–155](../../../quarantine/goose/crates/goose/src/agents/state_machine/ops_tool_approval.rs#L44): Goose-mode inspectors persist executable decisions/permission changes and approval request effects.

### Evidence e31

[quarantine/goose/crates/goose-provider-types/src/goose_mode.rs:22–32](../../../quarantine/goose/crates/goose-provider-types/src/goose_mode.rs#L22): Auto default, Approve, SmartApprove and Chat authority modes.

### Evidence e32

[quarantine/goose/crates/goose/src/agents/state_machine/tests/steering_lifecycle.rs:8–124](../../../quarantine/goose/crates/goose/src/agents/state_machine/tests/steering_lifecycle.rs#L8): FIFO/compaction/cancel-resume scripted provider tests assert exact subsequent inputs and once-only steer.

### Evidence e33

[quarantine/goose/crates/goose/src/agents/platform_extensions/summon.rs:3552–3609](../../../quarantine/goose/crates/goose/src/agents/platform_extensions/summon.rs#L3552): Notification ordering test checks child events precede final result with preserved child ID.

### Evidence e34

[quarantine/goose/crates/goose/src/agents/platform_extensions/ext_manager.rs:629–655](../../../quarantine/goose/crates/goose/src/agents/platform_extensions/ext_manager.rs#L629): Direct management test refuses subagent enable/disable while user can alter developer extension.

### Evidence e35

[quarantine/goose/crates/goose/src/agents/extension_manager/streamable_http.rs:611–638](../../../quarantine/goose/crates/goose/src/agents/extension_manager/streamable_http.rs#L611): Actual remote streamable HTTP transport connection and OAuth fallback.

### Evidence e36

[quarantine/goose/crates/goose/src/agents/platform_extensions/mod.rs:36–220](../../../quarantine/goose/crates/goose/src/agents/platform_extensions/mod.rs#L36): Native extension registration plus exact unprefixed-tool and feature flags.

### Evidence e37

[quarantine/goose/crates/goose/src/agents/platform_extensions/todo.rs:106–166](../../../quarantine/goose/crates/goose/src/agents/platform_extensions/todo.rs#L106): todo_write catalog and dispatch; complete content replacement.

### Evidence e38

[quarantine/goose/crates/goose/src/agents/platform_extensions/analyze/mod.rs:217–240](../../../quarantine/goose/crates/goose/src/agents/platform_extensions/analyze/mod.rs#L217): analyze source catalog for tree-sitter code structure.

### Evidence e39

[quarantine/goose/crates/goose/src/agents/platform_extensions/summarize.rs:66–84](../../../quarantine/goose/crates/goose/src/agents/platform_extensions/summarize.rs#L66): summarize file/directory/question action catalog.

