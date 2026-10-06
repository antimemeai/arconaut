# roo-code

Single-active mode-oriented task agent with real file/terminal actions, configurable tool/profile surfaces and non-destructive context projection; sequential delegation and disposal artifacts sharply limit collaboration/audit/refit claims.

Role: mode-based editor/CLI coding agent. Runtime: TypeScript, JavaScript.

Pinned source: [https://github.com/RooVetGit/Roo-Code](https://github.com/RooVetGit/Roo-Code); revision/version `b867ec9145750d0ae1ff7f02d35406e9bf2a0b16`.

VSCode/Node Task loop with provider stream and serial presentation, VSCode terminals or execa subprocesses; optional CLI host present.

Owns Task/message queue/editor context and task history/artifacts, releases task terminals on disposal; consumes providers and MCP, serial parent-child task handoff.

Inspection: Selected source excerpts trace exposed entry/loop/request/action/result/state boundaries and relevant capability mechanisms; listed tests are read as source only.

Limits of this study: No acquired code executed, dependencies installed or live providers invoked. Claims concern this pinned snapshot; untraced catalog tools and dependency internals are not inferred.

## Actions

### read_file

Surface: model tool.

Input: Path/files with line ranges

Result: Text with numbered lines or image/binary error

Lifecycle: Awaited read; common presentation serialization

Authority: Model read policy/ignore/current mode

Evidence: [e8](#evidence-e8), [e11](#evidence-e11), [e34](#evidence-e34).

### write_to_file / apply_diff / apply_patch / edit / search_replace / edit_file

Surface: model tools.

Input: File text, patch or edit-specific schema

Result: Write/diff result and diagnostics/context update

Lifecycle: Awaited editor mutation; rejection reverts staged changes

Authority: Mode/write protected/outside policy and common callbacks

Evidence: [e8](#evidence-e8), [e10](#evidence-e10), [e12](#evidence-e12), [e34](#evidence-e34).

### list_files / search_files / codebase_search

Surface: model tools.

Input: Native search/list arguments

Result: Matches/indexed results

Lifecycle: Wired catalog; individual search engine internals outside scope

Authority: Model mode/read policy

Evidence: [e8](#evidence-e8), [e10](#evidence-e10).

### execute_command

Surface: model tool.

Input: Command,cwd, model wait timeout plus operator configuration

Result: Exit/output or running status; artifact/execution ID

Lifecycle: Terminal/execa execution, foreground→background on model timer; operator timer cleanup contradiction

Authority: Model allowed/denied command rules and operator limits

Evidence: [e13](#evidence-e13), [e14](#evidence-e14), [e15](#evidence-e15), [e33](#evidence-e33).

### read_command_output

Surface: model tool.

Input: {artifact_id,search?,offset?,limit?}

Result: Paged/search content and artifact size

Lifecycle: Task-owned storage; removed at disposal

Authority: Model only validated cmd-* artifacts

Evidence: [e16](#evidence-e16), [e30](#evidence-e30), [e31](#evidence-e31).

### new_task

Surface: model tool.

Input: {mode,message,todos?}

Result: Child task ID

Lifecycle: Flush parent/dispose then sole-active child; return is delegation receipt

Authority: Mode/subtask approval; parent lineage

Evidence: [e17](#evidence-e17), [e18](#evidence-e18), [e5](#evidence-e5).

### switch_mode

Surface: model tool.

Input: {mode_slug,reason?}

Result: Mode-change receipt

Lifecycle: Persisted handler/profile switch then500ms settling delay

Authority: Model when allowed; custom mode/tool restrictions

Evidence: [e19](#evidence-e19), [e20](#evidence-e20), [e34](#evidence-e34).

### run_slash_command / skill

Surface: model tools.

Input: Command or skill name, optional args

Result: Loaded procedural instruction content

Lifecycle: Experimental slash gate; optional mode change; ordinary prompt composition

Authority: Model approval/skill special autoapproval

Evidence: [e21](#evidence-e21), [e34](#evidence-e34).

### use_mcp_tool / access_mcp_resource

Surface: model tools.

Input: Server/tool/object args or resource URI

Result: Service result/resource content

Lifecycle: Validated remote-service request; detailed transport lifecycle untraced

Authority: Model per-server/tool approvals

Evidence: [e8](#evidence-e8), [e35](#evidence-e35), [e33](#evidence-e33).

### ask_followup_question / update_todo_list / attempt_completion / generate_image

Surface: model tools.

Input: Native structured schemas

Result: Question/plan/completion/image results

Lifecycle: Catalog and actual named handlers; detailed image/completion bodies outside trace scope

Authority: Model mode/policy; host interactive callbacks

Evidence: [e8](#evidence-e8), [e10](#evidence-e10).

### customTool.execute

Surface: programmable API.

Input: Parsed args, {mode,task}

Result: Return value/error

Lifecycle: Awaited inside ordinary presentation loop; experimental registry

Authority: Installed tool code full Task access

Evidence: [e9](#evidence-e9).

### condenseContext / abortTask / processQueuedMessages

Surface: operator/programmatic API.

Input: Current Task, optional stored support prompt/operator messages

Result: Summary history / disposed task / submission

Lifecycle: Flush pairs before compaction; abort cancel/provider resource cleanup

Authority: Operator/host APIs, no native repair directive established

Evidence: [e23](#evidence-e23), [e24](#evidence-e24), [e29](#evidence-e29), [e30](#evidence-e30), [e32](#evidence-e32).

## Capabilities

### filesystem

**I — Files** (source): Native read/write/edit/search routed by common handler; writes use editor preview/commit and context tracking.

Evidence: [e8](#evidence-e8), [e11](#evidence-e11), [e12](#evidence-e12).

### processes

**L — OS programs** (source): Real terminal/execa commands with background transition and task artifacts. Both timer IDs cleared after race despite comment promising remaining user timeout; candidate defect, no execution proof.

Evidence: [e13](#evidence-e13), [e14](#evidence-e14), [e15](#evidence-e15).

### code-actions

**S — Code actions** (source): Shell commands and experimental custom executable tools support computation; slash tool returns prompt material rather than executing workflow program.

Evidence: [e9](#evidence-e9), [e14](#evidence-e14), [e21](#evidence-e21).

### persistent-kernel

**? — Kernel** (inspection-limit): No standing language interpreter established in traced Task/native-tool/terminal paths.

### standing-database

**? — Standing DB** (inspection-limit): No model-native standing DB/query contract established; shell/MCP integration alone not counted as turnkey database.

### workflow-programming

**S — Workflows** (source): Custom modes, procedural commands/skills, hierarchical new_task and custom Tool.execute compose workflow primitives; no general model-owned workflow program lifecycle traced.

Evidence: [e17](#evidence-e17), [e19](#evidence-e19), [e21](#evidence-e21), [e9](#evidence-e9).

### multi-model

**I — Models** (source): Mode switch can activate mode-specific provider profile and rebuild handler. Serial child/mode handoff, not simultaneous heterogeneous models.

Evidence: [e19](#evidence-e19), [e20](#evidence-e20), [e18](#evidence-e18).

### live-collaboration

**L — Peer chat** (source): new_task closes parent before child and enforces single-open invariant; no peer messaging/live team bus in inspected task orchestration.

Evidence: [e17](#evidence-e17), [e18](#evidence-e18), [e1](#evidence-e1).

### concurrent-work

**L — Concurrency** (source): Presentation lock executes tools sequentially although provider metadata requests parallelToolCalls; terminal command can remain background while loop proceeds.

Evidence: [e6](#evidence-e6), [e3](#evidence-e3), [e14](#evidence-e14).

### steering-interrupt

**I — Steer/interrupt** (source): Queued operator messages processed after explicit tool/compaction sites; abort disposes task and cancels current request. No universal immediate message injection into live model stream inferred.

Evidence: [e12](#evidence-e12), [e23](#evidence-e23), [e29](#evidence-e29), [e32](#evidence-e32).

### turn-redefinition

**L — Turn program** (source): Model changes mode/profile and prompt command; custom tool can access Task. Actual loop remains host Task/presentation stack, not user/model programmable turn algebra.

Evidence: [e2](#evidence-e2), [e19](#evidence-e19), [e20](#evidence-e20), [e21](#evidence-e21), [e9](#evidence-e9).

### compaction

**I — Compaction** (source): Non-destructive tagged originals with latest-summary provider projection; custom prompt and pre-condense pair flush; orphan parent cleanup on rewind.

Evidence: [e23](#evidence-e23), [e24](#evidence-e24), [e25](#evidence-e25), [e26](#evidence-e26).

### context-repair

**S — Repair** (source): Deleting a summary reactivates original history and tests directly probe it. Disk JSON corruption instead returns empty; no model corruption diagnosis/repair protocol established.

Evidence: [e26](#evidence-e26), [e27](#evidence-e27).

### original-audit

**L — Original audit** (source): Tagged transcript preserves condensed originals, but JSON rewrite/retry removal and cmd-* artifact deletion make it incomplete as immutable event audit.

Evidence: [e24](#evidence-e24), [e28](#evidence-e28), [e30](#evidence-e30), [e31](#evidence-e31).

### audit-query

**S — Audit query** (source): Read/search/paging command artifacts and disk API history provide partial retrieval; disposal removes artifact originals. No unified all-event query.

Evidence: [e16](#evidence-e16), [e27](#evidence-e27), [e31](#evidence-e31).

### hot-change

**L — Hot change** (source): Mode files watched and mode/provider updates apply without complete process restart;500ms settling delay is not affected-workflow transaction/quiescence.

Evidence: [e22](#evidence-e22), [e20](#evidence-e20), [e19](#evidence-e19).

### rebuild-continuity

**L — Rebuild continuity** (source): Saved task metadata/history permits handoff/reopen; parent disposal proceeds even failed flush, with stale-state warning. No executable outpost/recompile handoff or standing-work pause.

Evidence: [e18](#evidence-e18), [e29](#evidence-e29), [e30](#evidence-e30).

### remote-services

**I — Remote** (source): Native/dynamic MCP plus model provider requests; service consumption exists, shared computation fabric contract untraced.

Evidence: [e3](#evidence-e3), [e35](#evidence-e35).

### self-improvement

**? — Self-improve** (inspection-limit): Mode/custom tool/files edit agency not evidence of model-governed harness candidate evaluation/promotion in inspected paths.

### complaints

**? — Complaints** (inspection-limit): Tool error telemetry and human GitHub reporting do not establish model-state grievance/bead capture surface.

### authority

**I — Authority** (source): Explicit per-class autoapproval, command allow/deny and outside/protected file controls; substantial approval machinery can be configured.

Evidence: [e33](#evidence-e33), [e34](#evidence-e34), [e12](#evidence-e12).

### evaluation

**S — Evaluation** (source): Read regression tests directly test orphan projection restoration; command/delegation tests mock executor/provider and assert arguments, not live process/refit behavior.

Evidence: [e26](#evidence-e26), [e14](#evidence-e14), [e18](#evidence-e18).

### time-order

**L — Time/order** (source): Serial tool presentation, provider monotonic rate slot, summary lastTimestamp+1 and delay-based mode settling coexist with wall timestamps/artifact IDs. No global causal audit.

Evidence: [e2](#evidence-e2), [e6](#evidence-e6), [e24](#evidence-e24), [e19](#evidence-e19).

## Inspected test oracles

- [quarantine/roo-code/src/core/condense/__tests__/rewind-after-condense.spec.ts](../../../quarantine/roo-code/src/core/condense/__tests__/rewind-after-condense.spec.ts): Lost originals after deleting condensation summary Oracle: Pure synthetic arrays and cleanup/effective-history assertions recover3/5 original messages; validates projection functions, not disk/provider rollback. Read, **not executed**.
- [quarantine/roo-code/src/core/tools/__tests__/newTaskTool.spec.ts](../../../quarantine/roo-code/src/core/tools/__tests__/newTaskTool.spec.ts): Delegated message escaping/todos and receipt Oracle: Mocks VSCode/provider/modes; legacy adapter calls startSubtask spy, assertions prove argument transformation rather than sole-active lifecycle. Read, **not executed**.
- [quarantine/roo-code/src/core/tools/__tests__/executeCommandTool.spec.ts](../../../quarantine/roo-code/src/core/tools/__tests__/executeCommandTool.spec.ts): Command tool validation/dispatch Oracle: Terminal/execa/Task mocked; executeCommandInTerminal returns fixed result, so no oracle for observed dual-timeout cleanup contradiction. Read, **not executed**.

## Useful mechanisms

- Preserves condensed originals and reactivates orphan projection tags on rewind.
- Model mode/profile switching is a first-class action.
- Command output has explicit artifact paging/search before disposal.

## Material limits

- README records extension shutdown; pinned source remains studyable, future maintenance not inferred.
- Sequential task invariant prevents broad live multi-model team claims.
- Disposal deletes command originals and parent flush failure does not stop delegation.
- Dual-timeout comment contradicts finally cleanup; candidate needs discriminating runtime test in separate authorized environment.

## Arconaut design questions

- Do refit/context handoff fail closed when authoritative persistence fails?
- Define timeout ownership across foreground→background transitions without relying on comments.
- Keep raw output originals independently of interactive task disposal.
- Expose genuinely executable workflow/turn programming beyond instruction templates and500ms mode settling.

## Evidence

### Evidence e1

[quarantine/roo-code/src/core/webview/ClineProvider.ts:2536–2592](../../../quarantine/roo-code/src/core/webview/ClineProvider.ts#L2536): Provider creates task with settings/profile/custom modes and enforces single open top-level task.

### Evidence e2

[quarantine/roo-code/src/core/task/Task.ts:2430–2530](../../../quarantine/roo-code/src/core/task/Task.ts#L2430): Task agent outer loop and request stack continue with tool/no-tool feedback, abort/mistake limits and monotonic provider rate slot.

### Evidence e3

[quarantine/roo-code/src/core/task/Task.ts:4080–4179](../../../quarantine/roo-code/src/core/task/Task.ts#L4080): Effective history projection, current mode/native+MCP tool building and provider.createMessage; advertised parallelToolCalls not actual dispatch concurrency.

### Evidence e4

[quarantine/roo-code/src/core/task/Task.ts:2926–2953](../../../quarantine/roo-code/src/core/task/Task.ts#L2926): Streaming complete calls parsed with call IDs then presented.

### Evidence e5

[quarantine/roo-code/src/core/task/Task.ts:3410–3477](../../../quarantine/roo-code/src/core/task/Task.ts#L3410): new_task truncates later calls; assistant history saved before final tool dispatch; loop waits result readiness.

### Evidence e6

[quarantine/roo-code/src/core/assistant-message/presentAssistantMessage.ts:59–83](../../../quarantine/roo-code/src/core/assistant-message/presentAssistantMessage.ts#L59): Presentation lock serializes content blocks and marks results ready only when streaming exhausted.

### Evidence e7

[quarantine/roo-code/src/core/assistant-message/presentAssistantMessage.ts:446–489](../../../quarantine/roo-code/src/core/assistant-message/presentAssistantMessage.ts#L446): Native result deduplicated per call, approval feedback merged and images appended.

### Evidence e8

[quarantine/roo-code/src/core/assistant-message/presentAssistantMessage.ts:652–824](../../../quarantine/roo-code/src/core/assistant-message/presentAssistantMessage.ts#L652): Native tool names actually dispatch named handler classes through common callbacks.

### Evidence e9

[quarantine/roo-code/src/core/assistant-message/presentAssistantMessage.ts:830–872](../../../quarantine/roo-code/src/core/assistant-message/presentAssistantMessage.ts#L830): Experimental custom tool parses schema and executes with full Task/mode access; executable extension primitive.

### Evidence e10

[quarantine/roo-code/src/core/prompts/tools/native-tools/index.ts:42–71](../../../quarantine/roo-code/src/core/prompts/tools/native-tools/index.ts#L42): Native catalog includes files/edit/search/shell/output/task/mode/skill/slash/MCP/image/question/todo/completion.

### Evidence e11

[quarantine/roo-code/src/core/tools/ReadFileTool.ts:758–785](../../../quarantine/roo-code/src/core/tools/ReadFileTool.ts#L758): Actual file read and inclusive line-range extraction; binary/image path separate.

### Evidence e12

[quarantine/roo-code/src/core/tools/WriteToFileTool.ts:148–185](../../../quarantine/roo-code/src/core/tools/WriteToFileTool.ts#L148): Diff preview asks approval, saveChanges commits file and records context, result and queued-message processing.

### Evidence e13

[quarantine/roo-code/src/core/tools/ExecuteCommandTool.ts:197–222](../../../quarantine/roo-code/src/core/tools/ExecuteCommandTool.ts#L197): Creates task-scoped output interceptor and selects execa versus VSCode terminal provider.

### Evidence e14

[quarantine/roo-code/src/core/tools/ExecuteCommandTool.ts:362–434](../../../quarantine/roo-code/src/core/tools/ExecuteCommandTool.ts#L362): Actual runCommand; model timeout continues process to background, operator timeout aborts. Finally clears both timer IDs, contradicting comment that operator timer remains active after background transition.

### Evidence e15

[quarantine/roo-code/src/core/tools/ExecuteCommandTool.ts:447–474](../../../quarantine/roo-code/src/core/tools/ExecuteCommandTool.ts#L447): Completed output awaits asynchronous persistence callback; user feedback can leave command running.

### Evidence e16

[quarantine/roo-code/src/core/tools/ReadCommandOutputTool.ts:98–172](../../../quarantine/roo-code/src/core/tools/ReadCommandOutputTool.ts#L98): Artifact read validates ID/path/offset and implements paged/search output access.

### Evidence e17

[quarantine/roo-code/src/core/tools/NewTaskTool.ts:14–122](../../../quarantine/roo-code/src/core/tools/NewTaskTool.ts#L14): Model new_task validates mode/message/todos, approval then delegates parent and opens sole-active child.

### Evidence e18

[quarantine/roo-code/src/core/webview/ClineProvider.ts:2783–2879](../../../quarantine/roo-code/src/core/webview/ClineProvider.ts#L2783): Delegation flushes parent result pairs, logs failure but proceeds; disposes parent, switches mode/profile and creates child after metadata sequencing.

### Evidence e19

[quarantine/roo-code/src/core/tools/SwitchModeTool.ts:31–67](../../../quarantine/roo-code/src/core/tools/SwitchModeTool.ts#L31): Model mode switch validates and asks approval, applies shared mode handler then uses500ms delay before next tool.

### Evidence e20

[quarantine/roo-code/src/core/webview/ClineProvider.ts:1255–1377](../../../quarantine/roo-code/src/core/webview/ClineProvider.ts#L1255): Mode persistence before in-memory activation, optional per-mode profile; handler rebuild/sync on provider-model/profile changes.

### Evidence e21

[quarantine/roo-code/src/core/tools/RunSlashCommandTool.ts:22–133](../../../quarantine/roo-code/src/core/tools/RunSlashCommandTool.ts#L22): Experimental slash operation resolves command or skill, asks approval, optional mode switch then returns instruction content; not an executable workflow engine.

### Evidence e22

[quarantine/roo-code/src/core/config/CustomModesManager.ts:316–354](../../../quarantine/roo-code/src/core/config/CustomModesManager.ts#L316): VSCode watches .roomodes create/change/delete and refreshes merged modes/global state/cache without workflow transaction.

### Evidence e23

[quarantine/roo-code/src/core/task/Task.ts:1603–1648](../../../quarantine/roo-code/src/core/task/Task.ts#L1603): Manual condense flushes pending results first, supports custom summary instructions and current mode tools.

### Evidence e24

[quarantine/roo-code/src/core/condense/index.ts:446–490](../../../quarantine/roo-code/src/core/condense/index.ts#L446): Non-destructive fresh-start condensation tags originals and appends summary with ID/timestamp.

### Evidence e25

[quarantine/roo-code/src/core/condense/index.ts:539–561](../../../quarantine/roo-code/src/core/condense/index.ts#L539): Provider effective history starts latest summary, removes orphan tool result references.

### Evidence e26

[quarantine/roo-code/src/core/condense/index.ts:602–686](../../../quarantine/roo-code/src/core/condense/index.ts#L602): Deleting summary/marker reactivates originals; cleanup clears orphan projection parent tags.

### Evidence e27

[quarantine/roo-code/src/core/task-persistence/apiMessages.ts:40–70](../../../quarantine/roo-code/src/core/task-persistence/apiMessages.ts#L40): Disk history read returns array, corruption logs and returns empty rather than repairing originals.

### Evidence e28

[quarantine/roo-code/src/core/task-persistence/apiMessages.ts:109–120](../../../quarantine/roo-code/src/core/task-persistence/apiMessages.ts#L109): History persistence rewrites JSON via safeWriteJson; no complete append-only event audit contract.

### Evidence e29

[quarantine/roo-code/src/core/task/Task.ts:2212–2253](../../../quarantine/roo-code/src/core/task/Task.ts#L2212): Abort disposes task and cancels current provider request.

### Evidence e30

[quarantine/roo-code/src/core/task/Task.ts:2288–2325](../../../quarantine/roo-code/src/core/task/Task.ts#L2288): Disposal releases terminals, cleans command artifacts, file context/ignore resources and reverts unfinished editor changes.

### Evidence e31

[quarantine/roo-code/src/integrations/terminal/OutputInterceptor.ts:388–398](../../../quarantine/roo-code/src/integrations/terminal/OutputInterceptor.ts#L388): Artifact cleanup unlinks every cmd-* file in task output directory.

### Evidence e32

[quarantine/roo-code/src/core/task/Task.ts:4597–4617](../../../quarantine/roo-code/src/core/task/Task.ts#L4597): Queued operator messages dequeue and submit asynchronously at explicit processing sites.

### Evidence e33

[quarantine/roo-code/src/core/auto-approval/index.ts:47–129](../../../quarantine/roo-code/src/core/auto-approval/index.ts#L47): Absent/disabled autoapproval asks; command rules can approve/deny and MCP has per-server allow rules.

### Evidence e34

[quarantine/roo-code/src/core/auto-approval/index.ts:151–178](../../../quarantine/roo-code/src/core/auto-approval/index.ts#L151): Skills intentionally approved; mode/subtask/read/write gates separate, including outside/protected paths.

### Evidence e35

[quarantine/roo-code/src/core/tools/UseMcpToolTool.ts:33–77](../../../quarantine/roo-code/src/core/tools/UseMcpToolTool.ts#L33): MCP model call validates existence/resolved name, asks policy approval and invokes service execution.

### Evidence e36

[quarantine/roo-code/README.md:64–68](../../../quarantine/roo-code/README.md#L64): Pinned README documents extension shut down May15 and points to community fork; do not infer current maintenance.

