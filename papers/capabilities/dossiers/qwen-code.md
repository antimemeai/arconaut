# qwen-code

Independently evolved Qwen Code snapshot has actual code/workflow programs, model messaging and team routing, projected context and optional commit-fenced managed state. Its freshness, mailbox retention, best-effort reload and declared-domain boundaries prevent treating these as full audit or executable refit.

Role: coding agent and programmable local workflow/team runtime. Runtime: TypeScript, JavaScript.

Pinned source: [https://github.com/QwenLM/qwen-code](https://github.com/QwenLM/qwen-code); revision/version `310f4ba3ab954eb858b500c6aee3551cc564ee0a`.

Node multi-host LlmClient and shared tool scheduler; fresh separate QuickJS host for exec; node:vm workflow orchestration and local AgentHeadless workers; shell processes and managed/legacy session writers.

Consumes providers, MCP and optional wrapped execution environments; owns local code-mode/workflow compute, teams/mailboxes/background registries and chosen session authority. Consumer/store abstractions exist but do not establish an independent shared kernel fabric.

Inspection: Selected source excerpts trace exposed entry/loop/request/action/result/state boundaries and relevant capability mechanisms; listed tests are read as source only.

Limits of this study: No acquired code executed, dependencies installed or live providers invoked. Claims concern this pinned snapshot; untraced catalog tools and dependency internals are not inferred.

## Actions

### read_file / write_file / edit

Surface: model tools.

Input: file_path/range/pages/content or edit fields

Result: content/media,diff/error

Lifecycle: read cache tracks actual residency; write checks prior read/runtime version/abort; edit name/catalog only

Authority: model scheduler policy/runtime filesystem context

Evidence: [e10](#evidence-e10), [e14](#evidence-e14), [e15](#evidence-e15), [e16](#evidence-e16).

### run_shell_command

Surface: model tool.

Input: command,is_background,timeout

Result: foreground results or task ID/PID/output file

Lifecycle: owned process registration; background settlement awaits output flush; orphan registration requests abort

Authority: model under scheduler execution policy; task_stop catalog available

Evidence: [e8](#evidence-e8), [e9](#evidence-e9), [e12](#evidence-e12), [e17](#evidence-e17).

### exec

Surface: mode-gated model program.

Input: JavaScript using tools/text/media helpers

Result: bounded values/text/media + nested results

Lifecycle: separate fresh QuickJS host each call; framed dispatch/CPU/memory/wall limits; cancels child/nested work

Authority: model via audited runtime only; no direct host I/O/import

Evidence: [e11](#evidence-e11), [e18](#evidence-e18), [e19](#evidence-e19), [e20](#evidence-e20), [e21](#evidence-e21).

### tool_search / tool_call

Surface: model discovery/bridge tools.

Input: deferred name/search/arguments

Result: schemas/result through registry

Lifecycle: lazy registration; bridge must dispatch via scheduler, not direct run

Authority: model declared tools and policy

Evidence: [e12](#evidence-e12).

### workflow

Surface: enabled model program/operator saved workflow.

Input: exactly script/scriptPath/name,args,resumeFromRunId,run_in_background

Result: runId/scriptPath/journalPath; phase/results/error

Lifecycle: bounded real AgentHeadless dispatch through node:vm orchestration; journal replay longest unchanged prefix; background only TUI/completion channel

Authority: model configurable workflows; saved scope/name-only gates; description requires operator requested orchestration

Evidence: [e22](#evidence-e22), [e23](#evidence-e23), [e24](#evidence-e24), [e25](#evidence-e25), [e26](#evidence-e26), [e27](#evidence-e27), [e28](#evidence-e28), [e29](#evidence-e29).

### agent

Surface: model delegation tool.

Input: configured agent prompt/model/background options

Result: registered agent ID,lineage/output + result/notification

Lifecycle: background registry records abort/controller and continuation reason; selected-model teammate route separately tested

Authority: model/configured child/tool policy; production workflow denies selected backchannels

Evidence: [e12](#evidence-e12), [e27](#evidence-e27), [e38](#evidence-e38), [e53](#evidence-e53).

### send_message

Surface: model messaging tool.

Input: task_id or to,message,summary

Result: queued/resumed/continued/peer sent or error; no inline answer

Lifecycle: running task at tool-round boundary; completed resident continuation vs cold revival; peer recipients can hold messages

Authority: default ask; peer text explicitly carries no sender user authority

Evidence: [e12](#evidence-e12), [e30](#evidence-e30), [e31](#evidence-e31), [e32](#evidence-e32), [e33](#evidence-e33).

### team_create / team_delete / team_plan_approval

Surface: experimental model control catalog.

Input: team identity/lifecycle/plan fields

Result: team configuration and controls

Lifecycle: enabled registry only; detailed create/delete internals not traced; actual delivery/route below

Authority: config flag; leader-only shutdown omitted from child registry

Evidence: [e13](#evidence-e13), [e53](#evidence-e53).

### TeamManager.sendMessage / mailbox.consumeUnread

Surface: team runtime primitive.

Input: recipient,text,sender,summary

Result: queued/provenance envelope or mailbox batch

Lifecycle: bounded peer queues drain idle; leader durable mailbox marks-read; five-minute read retention

Authority: teammate identity scope; explicit shutdown response reservation

Evidence: [e34](#evidence-e34), [e35](#evidence-e35), [e36](#evidence-e36), [e37](#evidence-e37).

### LocalManagedSessionAuthority.open / commit

Surface: programmable state authority.

Input: journal/lease,expected proof,command ID/digest/sequence,declared event/actors

Result: committed receipt/events or conflict/uncommitted-tail refusal

Lifecycle: serialized transaction marker before application, failed append poisons writer; cold proof checks

Authority: declared actors/session fences under configured managed-mode binding

Evidence: [e39](#evidence-e39), [e40](#evidence-e40), [e41](#evidence-e41), [e42](#evidence-e42).

### recorder close / sealForHandoff

Surface: owned persistence lifecycle.

Input: flush/activation stop/lease proof

Result: sealed writer position or integrity failure

Lifecycle: pins committed prefix; does not prove active provider/shell pause nor compiled core exchange

Authority: harness writer owner, successor must match proof

Evidence: [e40](#evidence-e40), [e44](#evidence-e44).

### tryCompressChat / compression snapshot / compress-fast substrate

Surface: context mechanism.

Input: history,instructions,signal; fast path named in client

Result: new projection/completed IDs and cache invalidation

Lifecycle: automatic/manual compaction append snapshot and preserve transcript; exact summarizer internals not fully traced

Authority: harness/operator; no native original-context repair action established

Evidence: [e45](#evidence-e45), [e46](#evidence-e46), [e47](#evidence-e47).

### SkillManager filesystem refresh / listeners

Surface: hot definition mechanism.

Input: changed skill files/activation

Result: registry and tool validation refresh

Lifecycle: 150ms debounce; registry mutates before listeners, 30s cap may leave slow listener finishing later

Authority: host skill manager/model edits authorized files through ordinary tools

Evidence: [e48](#evidence-e48), [e49](#evidence-e49).

### SessionService.searchSessionContent

Surface: programmable query API.

Input: query,maxFiles,maxResults,signal

Result: text match snippets

Lifecycle: bounded scan with cancellation/yield; transcript-domain query

Authority: host/session consumers, not a full effect audit query

Evidence: [e50](#evidence-e50).

## Capabilities

### filesystem

**I — Files** (source): Native read/write runtime paths support media/ranges, residency-aware caching and prior-read/version checks; native edit catalog separately scoped.

Evidence: [e14](#evidence-e14), [e15](#evidence-e15), [e16](#evidence-e16).

### processes

**I — OS programs** (source): Foreground/background owned shell registry gives stable task/PID/output file; orphan register aborts and terminal status waits for output flush. Shell stop operation is cataloged but internals untraced.

Evidence: [e12](#evidence-e12), [e17](#evidence-e17).

### code-actions

**I — Code actions** (source): Fresh code-mode QuickJS programs compose registered tool calls through audited scheduler; direct file/process tool paths and node:vm workflow programs are distinct.

Evidence: [e14](#evidence-e14), [e16](#evidence-e16), [e18](#evidence-e18), [e19](#evidence-e19), [e20](#evidence-e20), [e23](#evidence-e23).

### persistent-kernel

**L — Kernel** (source): Code-mode creates new QuickJS runtime/context for each call and fresh-global test confirms no namespace continuity. Workflow VM can retain variables only for its run; no shared resident language kernel established.

Evidence: [e20](#evidence-e20), [e28](#evidence-e28), [e51](#evidence-e51).

### standing-database

**S — Standing DB** (source): Declared managed-session journal/authority and queryable session transcripts are standing state primitives, not model general SQL or independent shared computation fabric.

Evidence: [e39](#evidence-e39), [e40](#evidence-e40), [e41](#evidence-e41), [e50](#evidence-e50).

### workflow-programming

**I — Workflows** (source): Enabled native workflow executes model-authored/saved JavaScript with agent/parallel/pipeline/nested composition, stable run/journal IDs and bounded real production dispatch. Code-mode separately composes ordinary tools.

Evidence: [e18](#evidence-e18), [e22](#evidence-e22), [e23](#evidence-e23), [e24](#evidence-e24), [e25](#evidence-e25), [e26](#evidence-e26), [e27](#evidence-e27), [e28](#evidence-e28), [e29](#evidence-e29).

### multi-model

**I — Models** (source): Request model overrides and definition-driven child/team generator routes are wired. Mock route test checks separate provider view; no live quality guarantee inferred.

Evidence: [e1](#evidence-e1), [e2](#evidence-e2), [e6](#evidence-e6), [e38](#evidence-e38), [e53](#evidence-e53).

### live-collaboration

**I — Peer chat** (source): Model send_message routes running/paused/resident background tasks, teams and same-machine peer adapter. Team recipient queues drain idle with provenance/backpressure; peer transport internals untraced and receiver may hold messages.

Evidence: [e30](#evidence-e30), [e31](#evidence-e31), [e32](#evidence-e32), [e33](#evidence-e33), [e34](#evidence-e34), [e35](#evidence-e35), [e36](#evidence-e36).

### concurrent-work

**I — Concurrency** (source): Shared safe-kind predicate batches reads/read-only shells/agents and serializes unsafe calls/skill activation; workflow dispatch adds bounded concurrency and separate agent attempts.

Evidence: [e8](#evidence-e8), [e9](#evidence-e9), [e26](#evidence-e26), [e27](#evidence-e27).

### steering-interrupt

**I — Steer/interrupt** (source): Steer callback preserves input on abort and combines pending steer with Stop continuation; provider child signals, code host termination, task messages and background controller identities traced. Full all-program quiescence not established.

Evidence: [e4](#evidence-e4), [e5](#evidence-e5), [e7](#evidence-e7), [e19](#evidence-e19), [e33](#evidence-e33), [e38](#evidence-e38).

### turn-redefinition

**S — Turn program** (source): Message types, Stop-hook forced continuation/model override/steer callbacks plus code/workflow programs expose meaningful turn composition. Fixed host send loop remains; no arbitrary model-owned scheduler replacement established.

Evidence: [e3](#evidence-e3), [e4](#evidence-e4), [e5](#evidence-e5), [e18](#evidence-e18), [e23](#evidence-e23).

### compaction

**I — Compaction** (source): Auto/manual compression appends projection snapshot with tool/prompt identities, swaps active history and clears stale file-read/execution cache; fast rule-based client path exists but internals not covered here.

Evidence: [e45](#evidence-e45), [e46](#evidence-e46), [e47](#evidence-e47).

### context-repair

**S — Repair** (source): Original transcript vs appended compressed snapshot and explicit cache invalidation enable recomposition; re-read restores file contents rather than returning stale unchanged. No dedicated native model historical-context repair protocol traced.

Evidence: [e14](#evidence-e14), [e45](#evidence-e45), [e46](#evidence-e46), [e47](#evidence-e47), [e50](#evidence-e50).

### original-audit

**L — Original audit** (source): Managed declared events have sequence/digest/commit proof and raw conversation projections append rather than overwrite. Non-strict unsupported records can drop, write failure disables ordinary writes, mailboxes expire read messages; full provider/OS-effect capture not established.

Evidence: [e37](#evidence-e37), [e39](#evidence-e39), [e41](#evidence-e41), [e42](#evidence-e42), [e43](#evidence-e43), [e46](#evidence-e46).

### audit-query

**S — Audit query** (source): Managed readEvents/commit receipt cold reopen and bounded session-content search are actual query primitives; event domains and materialized transcript are narrower than all-effect audit.

Evidence: [e40](#evidence-e40), [e41](#evidence-e41), [e42](#evidence-e42), [e50](#evidence-e50), [e52](#evidence-e52).

### hot-change

**L — Hot change** (source): Saved workflow scripts load per call; skill watcher debounces and awaits best-effort derived refresh. Registry mutates before listeners and timeout can release a still-running listener; no affected-workflow versioned activation contract established.

Evidence: [e22](#evidence-e22), [e48](#evidence-e48), [e49](#evidence-e49).

### rebuild-continuity

**S — Rebuild continuity** (source): Managed recorder seals writer proof and cold successor validates it; workflow journal resumes unchanged-prefix agent results and background cold revival uses transcript. No compiled executable outpost/refit or owned-standing pause barrier traced.

Evidence: [e22](#evidence-e22), [e26](#evidence-e26), [e32](#evidence-e32), [e40](#evidence-e40), [e44](#evidence-e44), [e54](#evidence-e54), [e55](#evidence-e55).

### remote-services

**I — Remote** (source): Consumes configured content generators/compatible remote provider stream and environment-wrapped tools; managed authority can use abstract journal handle. Inspected code-mode/workflow/shell compute is locally harness owned.

Evidence: [e6](#evidence-e6), [e7](#evidence-e7), [e19](#evidence-e19), [e27](#evidence-e27), [e39](#evidence-e39), [e40](#evidence-e40).

### self-improvement

**S — Self-improve** (source): Model-authored/saved workflows plus measured dispatch token budgets and ordinary editing can compose experiments. No dedicated model-governed self-change acceptance/refit loop proven by inspected paths or README self-iteration claim.

Evidence: [e16](#evidence-e16), [e22](#evidence-e22), [e25](#evidence-e25), [e26](#evidence-e26), [e48](#evidence-e48).

### complaints

**? — Complaints** (inspection-limit): No nonfatal native agent-state grievance filing into independent database established in inspected tools/recording/workflow/team paths; report_findings catalog name is not treated as complaint.

### authority

**I — Authority** (source): Scheduler safety classification, declared managed actors/fences, prior-read/version checks, experimental leader-only registration and send_message default ask are explicit scopes. Peer text cannot inherit sender operator authority.

Evidence: [e8](#evidence-e8), [e13](#evidence-e13), [e15](#evidence-e15), [e16](#evidence-e16), [e30](#evidence-e30), [e31](#evidence-e31), [e41](#evidence-e41).

### evaluation

**S — Evaluation** (source): Read tests cover fresh guest globals/cancel, filesystem-backed commit/reopen/idempotence, mock provider route and SIGKILL journal replacement. Strong persistence oracle scopes do not prove real provider/OS exactly-once or complete context semantics; not run.

Evidence: [e51](#evidence-e51), [e52](#evidence-e52), [e53](#evidence-e53), [e54](#evidence-e54), [e55](#evidence-e55).

### time-order

**I — Time/order** (source): Declared managed events commit ordered sequence/digest prefix with actor fences; background exit timestamp captured before flush while status waits; mailbox timestamp/priority delivery separate clocks. No external effect global order inferred.

Evidence: [e17](#evidence-e17), [e34](#evidence-e34), [e36](#evidence-e36), [e41](#evidence-e41), [e42](#evidence-e42).

## Inspected test oracles

- [quarantine/qwen-code/packages/core/src/code-mode/code-mode.test.ts](../../../quarantine/qwen-code/packages/core/src/code-mode/code-mode.test.ts): Fresh guest globals and unfinished nested call cancellation Oracle: Uses actual host runner with mocked nested function; confirms no persisted global and abort event, not OS/provider termination convergence. Read, **not executed**.
- [quarantine/qwen-code/packages/core/src/managed-runtime/managed-session-authority.test.ts](../../../quarantine/qwen-code/packages/core/src/managed-runtime/managed-session-authority.test.ts): Durable input/wake transaction, cold reopen and idempotent command replay Oracle: Filesystem fixture asserts declared records/receipt counts before and after reopen; no external provider/process effect atomicity. Read, **not executed**.
- [quarantine/qwen-code/packages/core/src/agents/team/TeamManager.model-routing.test.ts](../../../quarantine/qwen-code/packages/core/src/agents/team/TeamManager.model-routing.test.ts): Dedicated teammate model/provider runtime view Oracle: Mocks content generator/core; asserts selected route and separation from leader, not live provider collaboration quality. Read, **not executed**.
- [quarantine/qwen-code/packages/core/src/agents/runtime/workflow-journal-restart.test.ts](../../../quarantine/qwen-code/packages/core/src/agents/runtime/workflow-journal-restart.test.ts): Kill around atomic journal rewrite Oracle: SIGKILL child before/after rename, load retained fake prefix/result/start records; attacks torn persistence, not uncertainty of real tool effects. Read, **not executed**.

## Useful mechanisms

- Two distinct model programming surfaces: audited ordinary-tool QuickJS programs and agent-oriented saved/resumable workflows.
- Actual live session/team messaging with explicit handles, provenance, receiver authority and bounded delivery rules.
- Managed journal explicitly fences actor/sequence/content and distinguishes committed prefix from torn tail; integration is actual optional config binding.
- Compaction cache invalidation understands that an unchanged-file placeholder is invalid once its original context is no longer resident.
- Background shell status reflects flushed output rather than announcing terminal state while disk bytes remain pending.

## Material limits

- Breadth is large; this study traces representative reachable mechanics, not every media/tool/goal/thread/host implementation. Managed mode and experimental teams are explicitly conditional.
- Code-mode is fresh per call, workflow globals per run; neither proves standing shared language kernel.
- Team peer messages wait for idle, task messages wait tool boundary, same-machine peer adapter may hold text; mailbox read records expire after five minutes.
- Managed commits cover declared records, not all transport/OS effects; non-strict dropped records and degraded writer are material completeness limits.
- Writer proof handoff and workflow prefix replay are supporting continuity; no quiescent compilation/outpost with owned standing pause traced.
- Skill registry mutates before best-effort listener refresh; timeout can leave partially refreshed consumers.

## Arconaut design questions

- Treat model programs, workflows and participant state as scoped consumers while deciding which primitives belong to fabric rather than Arconaut ownership.
- Can preserved original history plus residency markers make model-initiated context repair reproducible and auditable?
- Which declared event domains and uncertain effects must join the immutable audit rather than remaining sidecars, expiry mailboxes or best-effort records?
- Build Arconaut quiescence/refit proof above—not equivalent to—writer-seal commit proof and longest-prefix workflow replay.
- Keep operator-governed authority distinct from peer claims while making sustained multi-model messaging and model orchestration normal work.

## Evidence

### Evidence e1

[quarantine/qwen-code/packages/cli/src/nonInteractiveCli.ts:2524–2544](../../../quarantine/qwen-code/packages/cli/src/nonInteractiveCli.ts#L2524): CLI provider send includes message type/model override and optional goal permit under shared abort signal.

### Evidence e2

[quarantine/qwen-code/packages/cli/src/nonInteractiveCli.ts:2645–2676](../../../quarantine/qwen-code/packages/cli/src/nonInteractiveCli.ts#L2645): CLI actual shared tool batch returns response parts/termination and updates next-turn model override.

### Evidence e3

[quarantine/qwen-code/packages/core/src/core/client.ts:3410–3436](../../../quarantine/qwen-code/packages/core/src/core/client.ts#L3410): LlmClient differentiates user/retry/cron/notification/teammate/goal interaction starts and captures agent output.

### Evidence e4

[quarantine/qwen-code/packages/core/src/core/client.ts:4278–4315](../../../quarantine/qwen-code/packages/core/src/core/client.ts#L4278): Steer callback is consumed at boundaries under turn budget and abort guard; aborted accepted input can restore.

### Evidence e5

[quarantine/qwen-code/packages/core/src/core/client.ts:5270–5295](../../../quarantine/qwen-code/packages/core/src/core/client.ts#L5270): Blocking Stop continuation consumes pending steer and recursively requests bounded Hook turn with same model override.

### Evidence e6

[quarantine/qwen-code/packages/core/src/core/llm-chat.ts:5179–5199](../../../quarantine/qwen-code/packages/core/src/core/llm-chat.ts#L5179): Actual provider call uses current/override content generator with model/history/generation config and retry auth attribution.

### Evidence e7

[quarantine/qwen-code/packages/core/src/core/openaiContentGenerator/pipeline.ts:470–510](../../../quarantine/qwen-code/packages/core/src/core/openaiContentGenerator/pipeline.ts#L470): OpenAI-compatible stream uses child AbortController and actual SDK request; HTTP response metadata inspected before converted stream.

### Evidence e8

[quarantine/qwen-code/packages/core/src/core/coreToolScheduler.ts:1490–1525](../../../quarantine/qwen-code/packages/core/src/core/coreToolScheduler.ts#L1490): Shared concurrency predicate excludes skill activation, admits agents, and admits shell only when conservatively read-only.

### Evidence e9

[quarantine/qwen-code/packages/core/src/core/coreToolScheduler.ts:5039–5079](../../../quarantine/qwen-code/packages/core/src/core/coreToolScheduler.ts#L5039): Scheduler gates all calls ready and executes consecutive safe groups or sequential unsafe operations.

### Evidence e10

[quarantine/qwen-code/packages/core/src/tools/tool-names.ts:20–73](../../../quarantine/qwen-code/packages/core/src/tools/tool-names.ts#L20): Native catalog includes exec, file/shell, agents/messages/teams, tasks, workflows, memory and goals; names alone do not prove every implementation.

### Evidence e11

[quarantine/qwen-code/packages/core/src/config/config.ts:12176–12181](../../../quarantine/qwen-code/packages/core/src/config/config.ts#L12176): Exec is dynamically registered only in CodeModeOnly tool mode.

### Evidence e12

[quarantine/qwen-code/packages/core/src/config/config.ts:12348–12376](../../../quarantine/qwen-code/packages/core/src/config/config.ts#L12348): Actual registry wires tool_call/search, agent/list/task_stop and send_message.

### Evidence e13

[quarantine/qwen-code/packages/core/src/config/config.ts:12638–12661](../../../quarantine/qwen-code/packages/core/src/config/config.ts#L12638): Experimental team flag registers team_create/delete/plan approval; leader-only shutdown is excluded from child registries.

### Evidence e14

[quarantine/qwen-code/packages/core/src/tools/read-file.ts:133–230](../../../quarantine/qwen-code/packages/core/src/tools/read-file.ts#L133): Read has resident-history-aware file cache, excludes nested code-mode cache fast path and calls cancellable media/text processor.

### Evidence e15

[quarantine/qwen-code/packages/core/src/tools/write-file.ts:299–358](../../../quarantine/qwen-code/packages/core/src/tools/write-file.ts#L299): Write captures runtime version, checks shared-memory secrets and enforces prior-read before existing-content read.

### Evidence e16

[quarantine/qwen-code/packages/core/src/tools/write-file.ts:516–556](../../../quarantine/qwen-code/packages/core/src/tools/write-file.ts#L516): Final version decision can reject TOCTOU, creates directories after check and passes version/abort to actual runtime write.

### Evidence e17

[quarantine/qwen-code/packages/core/src/tools/shell.ts:4241–4326](../../../quarantine/qwen-code/packages/core/src/tools/shell.ts#L4241): Background shell registers stable ID/PID/output file, aborts on orphan registration, and marks terminal status only after output stream flush.

### Evidence e18

[quarantine/qwen-code/packages/core/src/tools/exec.ts:44–113](../../../quarantine/qwen-code/packages/core/src/tools/exec.ts#L44): Exec requires audited runtime and uses scheduler dispatch for nested calls; terminal-goal metadata gates further work.

### Evidence e19

[quarantine/qwen-code/packages/core/src/code-mode/host-client.ts:125–179](../../../quarantine/qwen-code/packages/core/src/code-mode/host-client.ts#L125): Code-mode spawns separate host with framed messages; tracks nested controllers and terminates child/nested work on abort or wall timeout.

### Evidence e20

[quarantine/qwen-code/packages/core/src/code-mode/host.ts:198–226](../../../quarantine/qwen-code/packages/core/src/code-mode/host.ts#L198): Each request constructs QuickJS runtime/context with memory/stack/CPU budget; not a resident kernel.

### Evidence e21

[quarantine/qwen-code/packages/core/src/tools/code-mode.ts:222–247](../../../quarantine/qwen-code/packages/core/src/tools/code-mode.ts#L222): Model-facing code-mode contract describes nested tool functions/helpers and fresh isolated runtime without direct filesystem/network/imports.

### Evidence e22

[quarantine/qwen-code/packages/core/src/tools/workflow/workflow.ts:112–191](../../../quarantine/qwen-code/packages/core/src/tools/workflow/workflow.ts#L112): Workflow accepts inline/path/name,args,resume run ID/background; saved script loaded per call; globals compose agents/phases/parallel/pipeline/nesting.

### Evidence e23

[quarantine/qwen-code/packages/core/src/tools/workflow/workflow.ts:535–629](../../../quarantine/qwen-code/packages/core/src/tools/workflow/workflow.ts#L535): Actual WorkflowRunner admission returns stable runId/script/journal handle for background or awaits completion for foreground.

### Evidence e24

[quarantine/qwen-code/packages/core/src/config/config.ts:12685–12688](../../../quarantine/qwen-code/packages/core/src/config/config.ts#L12685): Workflow tool actual registration is gated on workflows enabled.

### Evidence e25

[quarantine/qwen-code/packages/core/src/agents/runtime/workflow-runner.ts:519–540](../../../quarantine/qwen-code/packages/core/src/agents/runtime/workflow-runner.ts#L519): Runner selects production dispatch unless test injection; uses same controller and token budget with approval event bridge.

### Evidence e26

[quarantine/qwen-code/packages/core/src/agents/runtime/workflow-runner.ts:751–795](../../../quarantine/qwen-code/packages/core/src/agents/runtime/workflow-runner.ts#L751): Run handle links bounded dispatch scheduler, controller,budget,registry and orchestration with journal/resumeReplay.

### Evidence e27

[quarantine/qwen-code/packages/core/src/agents/runtime/workflow-orchestrator.ts:805–837](../../../quarantine/qwen-code/packages/core/src/agents/runtime/workflow-orchestrator.ts#L805): Production dispatch actually creates AgentHeadless with bounded turns/time and denied message/plan control backchannels.

### Evidence e28

[quarantine/qwen-code/packages/core/src/agents/runtime/workflow-sandbox.ts:1200–1244](../../../quarantine/qwen-code/packages/core/src/agents/runtime/workflow-sandbox.ts#L1200): Workflow runs node:vm context; globals constructed in guest realm, random/time disabled for replay determinism.

### Evidence e29

[quarantine/qwen-code/packages/core/src/tools/workflow/workflow.ts:1884–1899](../../../quarantine/qwen-code/packages/core/src/tools/workflow/workflow.ts#L1884): Workflow resume ID validates; background requires interactive TUI and completion channel.

### Evidence e30

[quarantine/qwen-code/packages/core/src/tools/send-message.ts:88–99](../../../quarantine/qwen-code/packages/core/src/tools/send-message.ts#L88): send_message is a privileged instruction sink with default ask, not inherited sender authority.

### Evidence e31

[quarantine/qwen-code/packages/core/src/tools/send-message.ts:117–201](../../../quarantine/qwen-code/packages/core/src/tools/send-message.ts#L117): Peer route calls same-machine peer transport, handles ambiguity/errors and marks received text as cross-session without sender user authority; transport internals untraced.

### Evidence e32

[quarantine/qwen-code/packages/core/src/tools/send-message.ts:272–338](../../../quarantine/qwen-code/packages/core/src/tools/send-message.ts#L272): Paused task can resume and completed resident task continues before transcript cold-revive fallback; restored sessions do not retain resident runtime.

### Evidence e33

[quarantine/qwen-code/packages/core/src/tools/send-message.ts:351–375](../../../quarantine/qwen-code/packages/core/src/tools/send-message.ts#L351): Running background messages queue at next tool-round boundary, with completion notification rather than inline reply.

### Evidence e34

[quarantine/qwen-code/packages/core/src/agents/team/TeamManager.ts:784–848](../../../quarantine/qwen-code/packages/core/src/agents/team/TeamManager.ts#L784): Teammate-to-leader actual mailbox write/timestamp and shutdown-response reservation; accepted approved shutdown can abort sender.

### Evidence e35

[quarantine/qwen-code/packages/core/src/agents/team/TeamManager.ts:855–895](../../../quarantine/qwen-code/packages/core/src/agents/team/TeamManager.ts#L855): Team peer messages queue with bounded backpressure and immediately flush only when recipient idle.

### Evidence e36

[quarantine/qwen-code/packages/core/src/agents/team/TeamManager.ts:2096–2147](../../../quarantine/qwen-code/packages/core/src/agents/team/TeamManager.ts#L2096): Delivery prioritizes queue and wraps provenance then enqueues within teammate AsyncLocalStorage identity.

### Evidence e37

[quarantine/qwen-code/packages/core/src/agents/team/mailbox.ts:198–259](../../../quarantine/qwen-code/packages/core/src/agents/team/mailbox.ts#L198): Inbox uses lock+atomic rewrite and marks reads; read messages older than five minutes compact out, unread stay.

### Evidence e38

[quarantine/qwen-code/packages/core/src/tools/agent/agent.ts:3522–3546](../../../quarantine/qwen-code/packages/core/src/tools/agent/agent.ts#L3522): Background agent registry records model/lineage/output/abort identity and continuation-block reason.

### Evidence e39

[quarantine/qwen-code/packages/core/src/config/config.ts:5005–5029](../../../quarantine/qwen-code/packages/core/src/config/config.ts#L5005): When managed mode chosen, config opens authority and binds recorder managed sink before activation under asserted writer lease.

### Evidence e40

[quarantine/qwen-code/packages/core/src/managed-runtime/managed-session-authority.ts:441–481](../../../quarantine/qwen-code/packages/core/src/managed-runtime/managed-session-authority.ts#L441): Cold open validates committed sequence/digest handoff proof and refuses uncommitted tail without exclusive repair.

### Evidence e41

[quarantine/qwen-code/packages/core/src/managed-runtime/managed-session-authority.ts:1740–1789](../../../quarantine/qwen-code/packages/core/src/managed-runtime/managed-session-authority.ts#L1740): Commit checks expected sequence, same-command content digest/idempotent receipt and session/actor fence.

### Evidence e42

[quarantine/qwen-code/packages/core/src/managed-runtime/managed-session-authority.ts:1880–1934](../../../quarantine/qwen-code/packages/core/src/managed-runtime/managed-session-authority.ts#L1880): Declared events and commit marker append as transaction before in-memory application; append failure poisons further writer and does not imply external effects atomically committed.

### Evidence e43

[quarantine/qwen-code/packages/core/src/services/chatRecordingService.ts:1565–1665](../../../quarantine/qwen-code/packages/core/src/services/chatRecordingService.ts#L1565): Managed/legacy record writes serialize; unsupported managed records can be dropped on non-strict path; write failure degrades recorder and later ordinary writes become no-ops.

### Evidence e44

[quarantine/qwen-code/packages/core/src/services/chatRecordingService.ts:2057–2117](../../../quarantine/qwen-code/packages/core/src/services/chatRecordingService.ts#L2057): Recorder close flushes and seals managed writer with commit proof after stopAdvancing; failed nonmanaged handoff retains lock; not executable compile/outpost.

### Evidence e45

[quarantine/qwen-code/packages/core/src/core/llm-chat.ts:2707–2759](../../../quarantine/qwen-code/packages/core/src/core/llm-chat.ts#L2707): Compression records snapshot/completed tool IDs then changes history, clears file-read cache and invalidates execution cache after successful compaction.

### Evidence e46

[quarantine/qwen-code/packages/core/src/services/chatRecordingService.ts:2849–2871](../../../quarantine/qwen-code/packages/core/src/services/chatRecordingService.ts#L2849): Compression appends distinct system record preserving projection/prompt IDs rather than rewriting transcript originals.

### Evidence e47

[quarantine/qwen-code/packages/core/src/core/client.ts:5670–5718](../../../quarantine/qwen-code/packages/core/src/core/client.ts#L5670): Manual compression carries completed IDs/start context and forces full IDE context; clears stale read placeholders.

### Evidence e48

[quarantine/qwen-code/packages/core/src/skills/skill-manager.ts:182–208](../../../quarantine/qwen-code/packages/core/src/skills/skill-manager.ts#L182): Skill activation mutates registry before parallel change listeners; hung listeners capped at 30s and can finish later—best effort activation consistency.

### Evidence e49

[quarantine/qwen-code/packages/core/src/skills/skill-manager.ts:1299–1329](../../../quarantine/qwen-code/packages/core/src/skills/skill-manager.ts#L1299): Filesystem skill watcher debounces refresh 150ms and updates watchers from refreshed cache.

### Evidence e50

[quarantine/qwen-code/packages/core/src/services/sessionService.ts:2427–2462](../../../quarantine/qwen-code/packages/core/src/services/sessionService.ts#L2427): Session content search has bounded file/result defaults, whitespace/case folding and cancellation/yield scanning.

### Evidence e51

[quarantine/qwen-code/packages/core/src/code-mode/code-mode.test.ts:895–919](../../../quarantine/qwen-code/packages/core/src/code-mode/code-mode.test.ts#L895): Real code-mode host test checks fresh globals and mock nested unawaited cancellation; not persistent state.

### Evidence e52

[quarantine/qwen-code/packages/core/src/managed-runtime/managed-session-authority.test.ts:214–288](../../../quarantine/qwen-code/packages/core/src/managed-runtime/managed-session-authority.test.ts#L214): Filesystem fixture test checks input/wake transaction, cold reopen and command-id replay receipts; not model/provider/OS effect exact-once.

### Evidence e53

[quarantine/qwen-code/packages/core/src/agents/team/TeamManager.model-routing.test.ts:218–244](../../../quarantine/qwen-code/packages/core/src/agents/team/TeamManager.model-routing.test.ts#L218): Mocked generator/core tests validate teammate dedicated provider route/runtimeView vs leader.

### Evidence e54

[quarantine/qwen-code/packages/core/src/agents/runtime/workflow-journal-restart.test.ts:28–44](../../../quarantine/qwen-code/packages/core/src/agents/runtime/workflow-journal-restart.test.ts#L28): Restart test prepares fake journal records and child writer to target kill before/after rename.

### Evidence e55

[quarantine/qwen-code/packages/core/src/agents/runtime/workflow-journal-restart.test.ts:67–103](../../../quarantine/qwen-code/packages/core/src/agents/runtime/workflow-journal-restart.test.ts#L67): SIGKILL and reload oracle checks retained prefix/result/start sets across atomic rename, not real provider side effects.

