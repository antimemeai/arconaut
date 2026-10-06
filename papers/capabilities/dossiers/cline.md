# cline

Migrated shared SDK exposes programmable model/tool turn seams, configured heterogeneous delegation, persisted team mailboxes/runs and integrity-checked context projection; continuity and audit remain bounded.

Role: interactive CLI/editor coding agent and shared SDK. Runtime: TypeScript, JavaScript.

Pinned source: [https://github.com/cline/cline](https://github.com/cline/cline); revision/version `9fe17595de3b0c980d6c8291f7c42bf0b8cc94a3`.

Node/Bun shared AgentRuntime over Core SessionRuntime; provider async streams, actual shell children and host/SDK extensions. Current monorepo differs materially from old recursive extension-only Task architecture.

Core owns session/conversation/team state, host routing and managed MCP; shell can detach from runtime. Consumes providers/MCP, with hub/cloud surfaces present but bounded study focuses local CLI/SDK.

Inspection: Selected source excerpts trace exposed entry/loop/request/action/result/state boundaries and relevant capability mechanisms; listed tests are read as source only.

Limits of this study: No acquired code executed, dependencies installed or live providers invoked. Claims concern this pinned snapshot; untraced catalog tools and dependency internals are not inferred.

## Actions

### read_files

Surface: model tool.

Input: Files/path aliases with start_line/end_line

Result: Per-file content/error; cline:// oversized-result cache reads

Lifecycle: Parallel reads; bounded outputs

Authority: Model tool policy and image capability

Evidence: [e13](#evidence-e13), [e16](#evidence-e16).

### search_codebase

Surface: model tool.

Input: Query/path search schema

Result: Per-query match result

Lifecycle: Parallel native executor requests with output/time bounds; detailed regex executor internals not traced

Authority: Model enabled tool policy

Evidence: [e41](#evidence-e41).

### editor / apply_patch

Surface: model tools.

Input: {path,old_text,new_text,insert_line?} or patch input

Result: Success/error and bounded edit result

Lifecycle: Awaited stateful no-auto-retry

Authority: Model mutation/host approval policy

Evidence: [e15](#evidence-e15), [e17](#evidence-e17).

### run_commands

Surface: model tool.

Input: One/multiple normalized commands, cwd from session

Result: Bounded output/errors; execution/PID log handle under detachment

Lifecycle: Shell children deadline/abort kill; host detach lets process continue

Authority: Model tool policy; operator execution controller

Evidence: [e14](#evidence-e14), [e18](#evidence-e18), [e19](#evidence-e19), [e20](#evidence-e20).

### skills / ask_question / submit_and_exit

Surface: model tools.

Input: Skill name / question input / final result

Result: Skill instructions / host answer / terminal completion

Lifecycle: Awaited callback; submit closes successful run

Authority: Model with configured skill/host callbacks

Evidence: [e1](#evidence-e1), [e6](#evidence-e6), [e40](#evidence-e40).

### spawn_agent / configured agent tools

Surface: model tools.

Input: Task/role and configured child inputs

Result: Child result/events

Lifecycle: Delegated runs; configured child provider and scoped tools

Authority: Model when feature enabled; configured policy not universal inheritance

Evidence: [e21](#evidence-e21), [e22](#evidence-e22).

### team_spawn_teammate / team_shutdown_teammate

Surface: model tools.

Input: Agent ID, role prompt/runtime spec / target reason

Result: Member status/lifecycle events

Lifecycle: Lead manages persistent team; running respawn forbidden

Authority: Lead-only spawn; feature-gated team

Evidence: [e23](#evidence-e23), [e26](#evidence-e26).

### team_run_task / team_cancel_run / team_list_runs / team_await_runs

Surface: model tools.

Input: Agent/task/run IDs plus message/options

Result: Run ID, state/error and result summary

Lifecycle: Queued/active run lifecycle; idle-only per teammate

Authority: Enabled team requester; management subset scoped

Evidence: [e23](#evidence-e23), [e27](#evidence-e27), [e22](#evidence-e22).

### team_send_message / team_broadcast / team_read_mailbox

Surface: model tools.

Input: Recipient/subject/body/task ID; unreadOnly

Result: Message receipt IDs / delivered count / mailbox bodies

Lifecycle: Notification on next model boundary; no automatic idle wake traced

Authority: Registered team sender; broadcast excludes lead

Evidence: [e24](#evidence-e24), [e25](#evidence-e25), [e26](#evidence-e26), [e11](#evidence-e11).

### team_task / team_mission_log / outcome catalog

Surface: model tools.

Input: Task action or mission summary/evidence/nextAction; outcome fragments/reviews

Result: Task/log/outcome IDs/status

Lifecycle: Shared team coordination; full outcome state machine not traced

Authority: Feature-gated model team tools

Evidence: [e23](#evidence-e23), [e24](#evidence-e24).

### prepareTurn / beforeModel / beforeTool hooks

Surface: programmable API.

Input: Executable handler receives snapshot/request or call

Result: Replacement context/tools/options/input/policy/stop

Lifecycle: Before provider or tool; hook context flush after tool results

Authority: Installed extension/SDK program, not blanket model-native control

Evidence: [e11](#evidence-e11), [e12](#evidence-e12), [e6](#evidence-e6).

### compactCurrentSession

Surface: operator/programmatic API.

Input: Current idle session, configured compaction

Result: Counts/state update

Lifecycle: Rejects running turn; persisted projection independent of canonical history

Authority: Host/operator manual action

Evidence: [e29](#evidence-e29), [e31](#evidence-e31), [e32](#evidence-e32).

### restartWithCurrentMessages

Surface: programmable API.

Input: Mutated config plus canonical history and valid compaction state

Result: Same conversation with replacement runtime

Lifecycle: Stops current session, serializes startup generation

Authority: Host config path; no executable rebuild transfer

Evidence: [e33](#evidence-e33).

## Capabilities

### filesystem

**I — Files** (source): Plural file reads, editing/patch executors and cache URI reads; model tool set/provider routing can vary.

Evidence: [e13](#evidence-e13), [e15](#evidence-e15), [e16](#evidence-e16), [e17](#evidence-e17).

### processes

**I — OS programs** (source): Spawned shell with progress, process tree abort/deadline and verified-identity detachment logs. Detached processes outlive a run; logs capped/retained24h.

Evidence: [e18](#evidence-e18), [e19](#evidence-e19), [e20](#evidence-e20).

### code-actions

**S — Code actions** (source): Native shell and programmable SDK tools/hooks execute code; no exposed persistent model language kernel established in inspected default tool definitions.

Evidence: [e14](#evidence-e14), [e18](#evidence-e18), [e11](#evidence-e11), [e12](#evidence-e12).

### persistent-kernel

**? — Kernel** (inspection-limit): No Python/JS standing interpreter established in traced CLI/SDK/native executor paths; shell process launch is not credited as a kernel.

### standing-database

**S — Standing DB** (source): Session service uses internal indexed persistence and shell/custom tools can consume databases, but inspected builtins expose no dedicated model standing-DB contract.

Evidence: [e42](#evidence-e42), [e18](#evidence-e18).

### workflow-programming

**I — Workflows** (source): Actual executable SDK prepareTurn/model/tool lifecycle hooks plus configured agents and team run/task tools; no dedicated model source-code workflow tool established.

Evidence: [e11](#evidence-e11), [e12](#evidence-e12), [e21](#evidence-e21), [e22](#evidence-e22), [e23](#evidence-e23).

### multi-model

**I — Models** (source): Configured child definitions carry own provider/model and tools. Team messaging independently traced; default teammate builder inherits parent connection, so arbitrary per-spawn heterogeneous teams not inferred.

Evidence: [e21](#evidence-e21), [e22](#evidence-e22), [e23](#evidence-e23).

### live-collaboration

**L — Peer chat** (source): Real shared team mailbox/task/run system; busy recipient notification consumed before model and body read explicitly. Broadcast skips lead; idle mail read at next routed task, not IRC-style wake/delivery body immediately.

Evidence: [e24](#evidence-e24), [e25](#evidence-e25), [e26](#evidence-e26), [e27](#evidence-e27), [e11](#evidence-e11).

### concurrent-work

**I — Concurrency** (source): Adjacent parallel tool calls overlap with sequential barriers; separate teammate runs and event/persisted state exist.

Evidence: [e8](#evidence-e8), [e22](#evidence-e22), [e27](#evidence-e27).

### steering-interrupt

**I — Steer/interrupt** (source): Pending operator input interrupts provider request while tools finish; whole-run external abort distinct. Team notification is a next-boundary mailbox hint.

Evidence: [e10](#evidence-e10), [e11](#evidence-e11), [e25](#evidence-e25), [e26](#evidence-e26).

### turn-redefinition

**I — Turn program** (source): prepareTurn and beforeModel rewrite messages/tools/options, hooks alter tool policy/input/stop; terminal completion policy governs yield. Model direct agency narrower than SDK host hooks.

Evidence: [e4](#evidence-e4), [e6](#evidence-e6), [e11](#evidence-e11), [e12](#evidence-e12).

### compaction

**I — Compaction** (source): Canonical-prefix fingerprint and separate working-context artifact guard resume projection; manual idle compaction versus automatic incremental summary. Basic/agentic/custom strategy bodies only partially traced.

Evidence: [e29](#evidence-e29), [e30](#evidence-e30), [e31](#evidence-e31), [e32](#evidence-e32).

### context-repair

**S — Repair** (source): Canonical transcript remains separately accessible and stale compacted prefix rejected. Manual fresh compaction, restore and host projection can reconstruct context; no dedicated model repair action traced.

Evidence: [e10](#evidence-e10), [e29](#evidence-e29), [e31](#evidence-e31), [e37](#evidence-e37), [e36](#evidence-e36).

### original-audit

**L — Original audit** (source): Canonical messages plus selected hook audit events; documented request ID excludes hidden retries. Shell output/logs bounded and expire; saved chat is not comprehensive original audit.

Evidence: [e34](#evidence-e34), [e35](#evidence-e35), [e20](#evidence-e20), [e36](#evidence-e36).

### audit-query

**S — Audit query** (source): Conversation history API, team mailbox/mission log and hook JSONL are separate inspection surfaces, not unified audit query engine.

Evidence: [e24](#evidence-e24), [e34](#evidence-e34), [e37](#evidence-e37).

### hot-change

**L — Hot change** (source): Per-request beforeModel and shell description reflect changes; host model/mode restart preserves same history using stop/start barrier. No general default after affected workflow transaction traced.

Evidence: [e11](#evidence-e11), [e14](#evidence-e14), [e33](#evidence-e33).

### rebuild-continuity

**L — Rebuild continuity** (source): Config restarts re-seed messages; persisted team recovery gives task instruction to inspect workspace/avoid duplication. This is neither unresolved-effect exact continuation nor quiescent executable outpost refit.

Evidence: [e22](#evidence-e22), [e28](#evidence-e28), [e33](#evidence-e33).

### remote-services

**I — Remote** (source): Provider model API and enabled remote/local MCP manager consume services; core host manages MCP shutdown. Wider hub/cloud implementation exists but not fully traced here.

Evidence: [e7](#evidence-e7), [e38](#evidence-e38).

### self-improvement

**? — Self-improve** (inspection-limit): Generic editing, hooks/configuration and skill loading do not establish model-governed harness candidate evaluation/promotion in inspected paths.

### complaints

**? — Complaints** (inspection-limit): No native model frustration report with agent-state capture established in inspected builtins/team/native hooks. Mission log is ordinary progress coordination.

### authority

**I — Authority** (source): Yolo selects local backend; runtime tool policy/hook overrides and host approval can enable/disable/ask. Child scoped tools/configured policy behavior must be reviewed per deployment.

Evidence: [e1](#evidence-e1), [e12](#evidence-e12), [e21](#evidence-e21).

### evaluation

**S — Evaluation** (source): Inspected configured-agent/lifecycle/manual-compaction tests assert boundaries using mocked SessionRuntime/session manager, not live model/process correctness. Native source contains bounded retry/loop behavior.

Evidence: [e4](#evidence-e4), [e8](#evidence-e8), [e31](#evidence-e31).

### time-order

**L — Time/order** (source): Parallel tool group results ordered, mailbox IDs monotonic per runtime and wall-clock sentAt; audit timestamps wall clock and detached PID/start-token differentiates reuse. No global causal order established.

Evidence: [e8](#evidence-e8), [e25](#evidence-e25), [e19](#evidence-e19), [e34](#evidence-e34).

## Inspected test oracles

- [quarantine/cline/sdk/packages/core/src/runtime/orchestration/runtime-builder.configured-agent-execution.test.ts](../../../quarantine/cline/sdk/packages/core/src/runtime/orchestration/runtime-builder.configured-agent-execution.test.ts): Configured child provider/skills/tool scoping Oracle: Mock SessionRuntime constructor/run; asserts selected provider/model/maxIterations, scoped tools and callback propagation; not live heterogeneous collaboration. Read, **not executed**.
- [quarantine/cline/sdk/packages/core/src/extensions/tools/team/multi-agent.lifecycle.test.ts](../../../quarantine/cline/sdk/packages/core/src/extensions/tools/team/multi-agent.lifecycle.test.ts): Shutdown of active queued teammate run Oracle: Fake SessionRuntime promise rejects on abort; runtime classification/event assertions do not test actual provider cancellation. Read, **not executed**.
- [quarantine/cline/apps/cli/src/runtime/interactive/session-runtime.test.ts](../../../quarantine/cline/apps/cli/src/runtime/interactive/session-runtime.test.ts): Manual compaction while running Oracle: Mock manager returns running; expects rejection and compactor/persistence not called. Source-level oracle for host gate only. Read, **not executed**.

## Useful mechanisms

- Runtime programming is actually wired through model/tool request boundaries.
- Canonical transcript and prefix-validated compaction artifact prevent blindly reusing stale summaries.
- Team mailboxes/task/run IDs and persistence establish collaboration beyond provider selection.
- Stateful native mutation tools are explicitly non-retryable.

## Material limits

- Detailed search/outcome state machines, remote hub/cloud and all SDK extensions outside trace scope.
- JetBrains source is unavailable; unpopulated evals/cline-bench submodule not inferred.
- Mailbox delivery is notification/poll or next task enrichment, not immediate general live peer wake.
- Bounded detached logs and surfaced request IDs do not meet full original audit.

## Arconaut design questions

- Require explicit message delivery semantics for busy, idle and recovering peers.
- Which canonical evidence should repair context after prefix mismatch rather than simply recomputing summary?
- Keep detached daemons outside quiescence ownership while pausing genuinely harness-owned standing work.
- Define model-native access to hook/workflow configuration without inheriting all host privileges.

## Evidence

### Evidence e1

[quarantine/cline/apps/cli/src/runtime/run-agent.ts:163–187](../../../quarantine/cline/apps/cli/src/runtime/run-agent.ts#L163): CLI supplies approval/question/submit callbacks, local backend in yolo, runtime hooks and session manager.

### Evidence e2

[quarantine/cline/apps/cli/src/runtime/run-agent.ts:220–246](../../../quarantine/cline/apps/cli/src/runtime/run-agent.ts#L220): SIGINT/SIGTERM abort active session and cleanup stops it.

### Evidence e3

[quarantine/cline/apps/cli/src/runtime/run-agent.ts:278–346](../../../quarantine/cline/apps/cli/src/runtime/run-agent.ts#L278): CLI starts Core session with config/hooks/team events then uses completed start result or sends prompt.

### Evidence e4

[quarantine/cline/sdk/packages/core/src/runtime/orchestration/session-runtime-orchestrator.ts:995–1087](../../../quarantine/cline/sdk/packages/core/src/runtime/orchestration/session-runtime-orchestrator.ts#L995): Per-run runtime is built with canonical history, tools, model, hooks and prepareTurn, then run/continued; startup abort forwarded at run-started.

### Evidence e5

[quarantine/cline/sdk/packages/agents/src/agent-runtime.ts:830–878](../../../quarantine/cline/sdk/packages/agents/src/agent-runtime.ts#L830): Turn loop invokes provider request with retry and treats interrupted empty turn separately.

### Evidence e6

[quarantine/cline/sdk/packages/agents/src/agent-runtime.ts:951–990](../../../quarantine/cline/sdk/packages/agents/src/agent-runtime.ts#L951): Tool results appended and emitted; terminal tool can finish run.

### Evidence e7

[quarantine/cline/sdk/packages/agents/src/agent-runtime.ts:2035–2053](../../../quarantine/cline/sdk/packages/agents/src/agent-runtime.ts#L2035): Actual provider model.stream(request) receives prepared request.

### Evidence e8

[quarantine/cline/sdk/packages/agents/src/agent-runtime.ts:2285–2322](../../../quarantine/cline/sdk/packages/agents/src/agent-runtime.ts#L2285): Only adjacent parallel tools overlap; sequential calls fence groups and Promise.all retains order.

### Evidence e9

[quarantine/cline/sdk/packages/agents/src/agent-runtime.ts:2485–2540](../../../quarantine/cline/sdk/packages/agents/src/agent-runtime.ts#L2485): Actual tool execution receives session/agent/conversation/run/call identity, abort signal, snapshot and update callback; errors normalized.

### Evidence e10

[quarantine/cline/sdk/packages/agents/src/agent-runtime.ts:631–675](../../../quarantine/cline/sdk/packages/agents/src/agent-runtime.ts#L631): Pending user message aborts model request only; external abort aborts whole run, restore resets state while retaining model/tools/hooks.

### Evidence e11

[quarantine/cline/sdk/packages/agents/src/agent-runtime.ts:1635–1669](../../../quarantine/cline/sdk/packages/agents/src/agent-runtime.ts#L1635): Pending messages then prepareTurn and beforeModel hooks can rewrite messages/tools/options and stop.

### Evidence e12

[quarantine/cline/sdk/packages/agents/src/agent-runtime.ts:2378–2443](../../../quarantine/cline/sdk/packages/agents/src/agent-runtime.ts#L2378): beforeTool may change input/policy/context or skip; disabled/approval policy applied to dispatch.

### Evidence e13

[quarantine/cline/sdk/packages/core/src/extensions/tools/definitions.ts:274–327](../../../quarantine/cline/sdk/packages/core/src/extensions/tools/definitions.ts#L274): read_files validates plural/ranged inputs and parallelizes independent read requests.

### Evidence e14

[quarantine/cline/sdk/packages/core/src/extensions/tools/definitions.ts:516–541](../../../quarantine/cline/sdk/packages/core/src/extensions/tools/definitions.ts#L516): run_commands normalizes commands, invokes shell executor, is explicitly non-retryable; request-time shell description getter.

### Evidence e15

[quarantine/cline/sdk/packages/core/src/extensions/tools/definitions.ts:656–737](../../../quarantine/cline/sdk/packages/core/src/extensions/tools/definitions.ts#L656): apply_patch and editor validate mutation args and call timed executors without auto-retry.

### Evidence e16

[quarantine/cline/sdk/packages/core/src/extensions/tools/executors/file-read.ts:216–254](../../../quarantine/cline/sdk/packages/core/src/extensions/tools/executors/file-read.ts#L216): Read routes cline:// cache or resolves/stats real files with signal/size/model image limits.

### Evidence e17

[quarantine/cline/sdk/packages/core/src/extensions/tools/executors/editor.ts:141–179](../../../quarantine/cline/sdk/packages/core/src/extensions/tools/executors/editor.ts#L141): Editor creates directories/files and reads content for unique replacement.

### Evidence e18

[quarantine/cline/sdk/packages/core/src/extensions/tools/executors/bash.ts:717–760](../../../quarantine/cline/sdk/packages/core/src/extensions/tools/executors/bash.ts#L717): Shell spawns actual child process, captures bounded rolling output, gives execution identity and tracks detach support.

### Evidence e19

[quarantine/cline/sdk/packages/core/src/extensions/tools/executors/bash.ts:801–879](../../../quarantine/cline/sdk/packages/core/src/extensions/tools/executors/bash.ts#L801): Abort/deadline kill process tree; verified PID/start-token supports detach to log, unref and releasing timeout/abort binding.

### Evidence e20

[quarantine/cline/sdk/packages/core/src/extensions/tools/executors/bash.ts:39–46](../../../quarantine/cline/sdk/packages/core/src/extensions/tools/executors/bash.ts#L39): Detached logs capped10MiB and default24h retention; process-progress batching48ms.

### Evidence e21

[quarantine/cline/sdk/packages/core/src/runtime/orchestration/runtime-builder.ts:630–674](../../../quarantine/cline/sdk/packages/core/src/runtime/orchestration/runtime-builder.ts#L630): Configured agent tools select child provider/model and scope tools/skills.

### Evidence e22

[quarantine/cline/sdk/packages/core/src/runtime/orchestration/runtime-builder.ts:685–809](../../../quarantine/cline/sdk/packages/core/src/runtime/orchestration/runtime-builder.ts#L685): Enabled teams are instantiated/bootstrap wired, persist state on team events, hydrate/recover active runs, unlock lead tools.

### Evidence e23

[quarantine/cline/sdk/packages/core/src/extensions/tools/team/team-tools.ts:195–249](../../../quarantine/cline/sdk/packages/core/src/extensions/tools/team/team-tools.ts#L195): Exact team native catalog; spawning lead-only, child tools exclude spawn and inherit delegated config.

### Evidence e24

[quarantine/cline/sdk/packages/core/src/extensions/tools/team/team-tools.ts:633–725](../../../quarantine/cline/sdk/packages/core/src/extensions/tools/team/team-tools.ts#L633): Model team_send_message/broadcast/read_mailbox/mission_log dispatch actual runtime; receipt IDs and mark-read behavior.

### Evidence e25

[quarantine/cline/sdk/packages/core/src/extensions/tools/team/multi-agent.ts:1493–1556](../../../quarantine/cline/sdk/packages/core/src/extensions/tools/team/multi-agent.ts#L1493): Mailboxes store complete body; running teammates receive pending notification, broadcasts skip sender and lead.

### Evidence e26

[quarantine/cline/sdk/packages/core/src/extensions/tools/team/multi-agent.ts:853–885](../../../quarantine/cline/sdk/packages/core/src/extensions/tools/team/multi-agent.ts#L853): Busy teammate cannot respawn; pending mailbox notification consumed by runtime callback;10minute request timeout.

### Evidence e27

[quarantine/cline/sdk/packages/core/src/extensions/tools/team/multi-agent.ts:1028–1110](../../../quarantine/cline/sdk/packages/core/src/extensions/tools/team/multi-agent.ts#L1028): Teammate run must be idle, includes unread mail, runs/continues and reports result/cancel/failure.

### Evidence e28

[quarantine/cline/sdk/packages/core/src/extensions/tools/team/multi-agent.ts:181–185](../../../quarantine/cline/sdk/packages/core/src/extensions/tools/team/multi-agent.ts#L181): Recovered interrupted run uses fresh instruction to inspect workspace and avoid duplicated effects, not exact resumption of uncertain tool.

### Evidence e29

[quarantine/cline/sdk/packages/core/src/extensions/context/compaction.ts:745–811](../../../quarantine/cline/sdk/packages/core/src/extensions/context/compaction.ts#L745): Compaction projects valid saved state and canonical tail before model, saves new state separately; manual fresh summary differs from incremental auto.

### Evidence e30

[quarantine/cline/sdk/packages/core/src/session/models/session-compaction.ts:82–103](../../../quarantine/cline/sdk/packages/core/src/session/models/session-compaction.ts#L82): Source hash excludes volatile message ID/time but includes role/content/durable metadata and model metrics.

### Evidence e31

[quarantine/cline/sdk/packages/core/src/session/models/session-compaction.ts:140–197](../../../quarantine/cline/sdk/packages/core/src/session/models/session-compaction.ts#L140): Compaction stores source count/hash/last key; invalid source prefix refuses projection, valid returns compacted messages plus canonical tail.

### Evidence e32

[quarantine/cline/apps/cli/src/runtime/interactive/session-runtime.ts:696–759](../../../quarantine/cline/apps/cli/src/runtime/interactive/session-runtime.ts#L696): Manual compact rejects running current turn and persists compaction artifact when valid.

### Evidence e33

[quarantine/cline/apps/cli/src/runtime/interactive/session-runtime.ts:440–502](../../../quarantine/cline/apps/cli/src/runtime/interactive/session-runtime.ts#L440): Model/mode config restart stops current session then seeds canonical messages/compaction, with startup generation barrier.

### Evidence e34

[quarantine/cline/sdk/packages/core/src/hooks/hook-file-hooks.ts:629–689](../../../quarantine/cline/sdk/packages/core/src/hooks/hook-file-hooks.ts#L629): Hook audit JSONL captures selected run/tool inputs/results with wall timestamp; not all provider transport/process events.

### Evidence e35

[quarantine/cline/sdk/ARCHITECTURE.md:73–77](../../../quarantine/cline/sdk/ARCHITECTURE.md#L73): Documented request ID covers final surfaced step, not hidden HTTP retries.

### Evidence e36

[quarantine/cline/sdk/ARCHITECTURE.md:581–601](../../../quarantine/cline/sdk/ARCHITECTURE.md#L581): Documents canonical history separate from compaction artifact, while older already-compacted imports cannot recover omitted originals.

### Evidence e37

[quarantine/cline/sdk/packages/core/src/session/stores/conversation-store.ts:19–88](../../../quarantine/cline/sdk/packages/core/src/session/stores/conversation-store.ts#L19): Conversation API retrieves/appends/replaces/restores messages, preserving display-only entries separately from runtime projection.

### Evidence e38

[quarantine/cline/sdk/packages/core/src/runtime/orchestration/runtime-builder.ts:206–310](../../../quarantine/cline/sdk/packages/core/src/runtime/orchestration/runtime-builder.ts#L206): MCP settings/plugins register clients, load enabled tools, isolate errors, dispose manager on shutdown.

### Evidence e39

[quarantine/cline/README.md:125–131](../../../quarantine/cline/README.md#L125): CLI/SDK/editor present; JetBrains implementation not open source.

### Evidence e40

[quarantine/cline/sdk/packages/core/src/extensions/tools/definitions.ts:775–870](../../../quarantine/cline/sdk/packages/core/src/extensions/tools/definitions.ts#L775): Named model skills, ask_question and submit_and_exit definitions provide skill-load, host interaction and terminal completion.

### Evidence e41

[quarantine/cline/sdk/packages/core/src/extensions/tools/definitions.ts:371–411](../../../quarantine/cline/sdk/packages/core/src/extensions/tools/definitions.ts#L371): search_codebase validates regex query inputs and invokes native executor via parallel Promise.all with output/time bounds.

### Evidence e42

[quarantine/cline/sdk/packages/core/src/session/services/session-service.ts:33–45](../../../quarantine/cline/sdk/packages/core/src/session/services/session-service.ts#L33): Core internal SQL store indexes session metadata and artifact paths; this is not a model-native standing database tool.

