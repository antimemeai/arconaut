# oh-my-pi

Broad model-native evaluation, programmable tooling, addressable live-agent messaging and grievance reporting; lifecycle ownership and bounded nested-call records materially differ from Arconaut fabric/refit/audit goals.

Role: interactive coding agent with persistent evaluation and local agent messaging. Runtime: TypeScript, JavaScript, Python, Rust native supporting package.

Pinned source: [https://github.com/can1357/oh-my-pi](https://github.com/can1357/oh-my-pi); revision/version `8b25ad4a05625dde65df41d057756b4815f4837c`.

Bun/TS agent loop; Python child kernels, JS subprocess or Worker evaluation, external shell/services. Runtime pause gates and cooperative message queues coexist with JS host event loop.

Owns session tree, jobs, kernels, browser/computer and local MCP lifecycles; root async manager may be shared with descendants. Consumes model/remote MCP/memory backends but not a general shared compute fabric.

Inspection: Selected source excerpts trace exposed entry/loop/request/action/result/state boundaries and relevant capability mechanisms; listed tests are read as source only.

Limits of this study: No acquired code executed, dependencies installed or live providers invoked. Claims concern this pinned snapshot; untraced catalog tools and dependency internals are not inferred.

## Actions

### read

Surface: model tool.

Input: {path}; local files, selectors, internal URLs, SQLite ?q=

Result: Rendered content and source metadata/paging

Lifecycle: Awaited; selector output can truncate

Authority: Model subject to tool tier/session routing

Evidence: [e9](#evidence-e9), [e10](#evidence-e10), [e45](#evidence-e45), [e46](#evidence-e46).

### write

Surface: model tool.

Input: {path,content?}; disk or handler-owned URL

Result: Bytes/result metadata, LSP diagnostics or device result

Lifecycle: Awaited; whole replacement or device-specific mutation

Authority: Model write/plan gate; coordination-specific exception

Evidence: [e11](#evidence-e11).

### bash

Surface: model tool.

Input: {command,cwd?,timeout?,pty?}; optional service name/readiness/background

Result: Text/exit or jobId/service name/PID/state

Lifecycle: Foreground, auto-background on steer; owned service/job lifecycle

Authority: Model execute approval and plan policy

Evidence: [e12](#evidence-e12), [e13](#evidence-e13).

### eval

Surface: model tool.

Input: {language:py|js,code,title?,timeout?,reset?}

Result: Output/artifact, kernel state or ongoing job handle

Lifecycle: Persistent per-session interpreter; backgroundable and cancellable

Authority: Model code under owned interpreter/tool bridge policy

Evidence: [e14](#evidence-e14), [e15](#evidence-e15), [e16](#evidence-e16), [e17](#evidence-e17), [e19](#evidence-e19), [e21](#evidence-e21).

### eval tools bridge

Surface: programmable API.

Input: Code invokes exposed tool/helper with structured arguments

Result: Tool result value/status or error

Lifecycle: Awaited calls, helpers support budget/workpool/status/cancel; direct-only operations rejected

Authority: Model-supplied code with validated session tools

Evidence: [e22](#evidence-e22), [e23](#evidence-e23).

### write agent://<id>

Surface: model tool.

Input: Message body and scoped recipient; broadcast through send API

Result: Delivery receipt/status, not peer response

Lifecycle: Busy injection/queue; idle wake or plan append; bounded mailbox

Authority: Visible/session-scoped peers; advisors read-only

Evidence: [e24](#evidence-e24), [e25](#evidence-e25), [e26](#evidence-e26), [e27](#evidence-e27), [e28](#evidence-e28), [e11](#evidence-e11).

### context_notes / new_context

Surface: model tools.

Input: Notebook text or {} fresh-window request

Result: Current notes/write receipt or requested:true

Lifecycle: Experimental gate; notebook persisted, rollover consumed at paired-result turn boundary

Authority: Model bounded notebook/current branch

Evidence: [e31](#evidence-e31), [e32](#evidence-e32), [e33](#evidence-e33).

### read history://current/full

Surface: model tool.

Input: Bound current full-history URI

Result: Entry IDs/raw full branch rendering

Lifecycle: Read-only current branch; flag required

Authority: Model experimental context-management access

Evidence: [e29](#evidence-e29), [e30](#evidence-e30).

### checkpoint / rewind

Surface: model tools.

Input: Goal / nonempty investigation report

Result: Checkpoint state / rewound report

Lifecycle: Session consumes direct result and branches with summary, retaining completed sibling tool pairs

Authority: Model tools settings-gated, eval bridge rejected

Evidence: [e39](#evidence-e39), [e40](#evidence-e40), [e8](#evidence-e8), [e22](#evidence-e22).

### manage_skill / learn

Surface: model tools.

Input: Skill create/update/delete fields; memory/context/optional skill

Result: Skill path/discovery update or backend memory result

Lifecycle: Authored disk/memory mutation persists independently of turn

Authority: Model gated autolearn/backend; cannot override shipped skill name

Evidence: [e47](#evidence-e47), [e48](#evidence-e48).

### write xd://report_issue

Surface: model tool.

Input: Tool name and human-readable issue content

Result: Acknowledgement and dispatch metadata

Lifecycle: Consent-aware SQLite record and asynchronous optional remote push

Authority: Model can complain; operator consent governs QA collection

Evidence: [e51](#evidence-e51), [e52](#evidence-e52), [e53](#evidence-e53).

### /reload-plugins

Surface: operator command.

Input: No arguments

Result: Refreshed discovery caches/MCP state

Lifecycle: Cache refresh/reconnect; full executable refit untraced

Authority: Operator TUI command

Evidence: [e41](#evidence-e41), [e42](#evidence-e42).

### AgentSession extension context

Surface: programmable API.

Input: Handlers/commands, branch/compact/ephemeral turn/timers

Result: Session/continuation events and data

Lifecycle: Start prompt overrides, session lifecycle APIs; abort and idle wait

Authority: Installed extension program; not all APIs directly model-native

Evidence: [e54](#evidence-e54), [e55](#evidence-e55).

### builtin registry catalog

Surface: model tools.

Input: Other named schemas: security_scan,edit,ast_grep,ast_edit,ask,debug,ida,github,glob,grep,find,lsp,task,wait,todo,web_search,memory_edit,retain,recall,reflect

Result: Per-tool results; only registry/gates traced for this residual catalog

Lifecycle: Availability filtered by settings/agent kind/recursion

Authority: Model schema visibility; detailed implementations not inferred

Evidence: [e7](#evidence-e7), [e8](#evidence-e8).

## Capabilities

### filesystem

**I — Files** (source): Local and URI file operations, structured archive/SQLite routes and LSP/editor writes are wired. Residual search/edit catalog exists but detailed edit engine not traced.

Evidence: [e9](#evidence-e9), [e10](#evidence-e10), [e11](#evidence-e11).

### processes

**I — OS programs** (source): Owned foreground/background shell jobs and standing services expose handles/readiness; steering can background without killing.

Evidence: [e12](#evidence-e12), [e13](#evidence-e13).

### code-actions

**I — Code actions** (source): Model eval executes Python or JavaScript, invoking validated native tools and program helpers.

Evidence: [e14](#evidence-e14), [e16](#evidence-e16), [e22](#evidence-e22), [e23](#evidence-e23).

### persistent-kernel

**I — Kernel** (source): Per-session owned Python process and persistent JS subprocess/Worker. Uncertain cells are not replayed; JS deadline kill loses variables.

Evidence: [e17](#evidence-e17), [e18](#evidence-e18), [e19](#evidence-e19), [e20](#evidence-e20), [e21](#evidence-e21).

### standing-database

**I — Standing DB** (source): Model SQLite selectors read/query disk databases and write routes invoke row mutation; harness grievance DB is a separate internal store. These are local consumers, not shared fabric control.

Evidence: [e10](#evidence-e10), [e11](#evidence-e11), [e45](#evidence-e45), [e46](#evidence-e46), [e52](#evidence-e52).

### workflow-programming

**I — Workflows** (source): Real eval scripts compose tools/budget/workpool/completion helpers; extension APIs supply ephemeral turns/timers/branching. Full helper scheduler internals outside excerpt scope.

Evidence: [e22](#evidence-e22), [e55](#evidence-e55).

### multi-model

**S — Models** (source): SDK exposes provider/model and role-based compaction candidate routing; peer bus can link sessions. Per-task model selection not traced here, so no blanket concurrent heterogeneous execution guarantee.

Evidence: [e1](#evidence-e1), [e34](#evidence-e34), [e24](#evidence-e24).

### live-collaboration

**I — Peer chat** (source): Process-global agent bus delivers scoped live messages, revives parked recipients and injects busy peer interrupts/parent steering; mailbox cap100, no cross-host transport proved.

Evidence: [e24](#evidence-e24), [e25](#evidence-e25), [e26](#evidence-e26), [e27](#evidence-e27), [e28](#evidence-e28).

### concurrent-work

**I — Concurrency** (source): Tool completions can be concurrent but transcript pairs flush in source order; async shell/eval jobs remain live while model handles steering.

Evidence: [e6](#evidence-e6), [e13](#evidence-e13), [e15](#evidence-e15).

### steering-interrupt

**I — Steer/interrupt** (source): Native asides/steering/follow-up routes plus pure-wait interrupt and cooperative backgrounding; external abort differs from message steering.

Evidence: [e4](#evidence-e4), [e5](#evidence-e5), [e13](#evidence-e13), [e27](#evidence-e27).

### turn-redefinition

**I — Turn program** (source): Request context synchronization, prompt replacement and yield continuation plus programmable ephemeral turns. Installed extension agency is broader than direct model tool exposure.

Evidence: [e1](#evidence-e1), [e2](#evidence-e2), [e4](#evidence-e4), [e54](#evidence-e54), [e55](#evidence-e55).

### compaction

**I — Compaction** (source): Distinct remote, bitmap snapcompact, handoff, shake and summary maintenance paths; persisted summary/retained entry/method metadata. Manual eligibility differs from auto methods.

Evidence: [e33](#evidence-e33), [e34](#evidence-e34), [e35](#evidence-e35), [e36](#evidence-e36), [e37](#evidence-e37).

### context-repair

**S — Repair** (source): Model can retrieve original branch entries, maintain notebook/request fresh window and checkpoint/rewind exploration. These primitives can repair lost working facts; dedicated automatic corruption diagnosis not traced.

Evidence: [e29](#evidence-e29), [e30](#evidence-e30), [e31](#evidence-e31), [e33](#evidence-e33), [e40](#evidence-e40).

### original-audit

**L — Original audit** (source): Session boundaries/messages persisted and full reset history retained, with explicit disk failure recovery. Eval bridge records summarized status rather than a direct toolResult for every nested call; no complete request/process/OS audit proof.

Evidence: [e22](#evidence-e22), [e37](#evidence-e37), [e38](#evidence-e38).

### audit-query

**S — Audit query** (source): Entry-ID full-history renderer and model read path enable branch inspection; SQLite grievances queryable via storage. Not a unified all-event audit query contract.

Evidence: [e29](#evidence-e29), [e30](#evidence-e30), [e45](#evidence-e45), [e52](#evidence-e52).

### hot-change

**L — Hot change** (source): Plugin refresh clears selected discovery caches/reconnects MCP; reload() instead aborts/reopens session. No demonstrated global default after-affected-workflow activation or full hook module refit.

Evidence: [e41](#evidence-e41), [e42](#evidence-e42), [e44](#evidence-e44), [e54](#evidence-e54).

### rebuild-continuity

**L — Rebuild continuity** (source): Session reload and disposal terminate owned work; idle JSON-safe Python snapshots exclude full process state, JS kill loses VM. Quiescent outpost recompile/reinhabitation absent from traced paths.

Evidence: [e18](#evidence-e18), [e21](#evidence-e21), [e43](#evidence-e43), [e44](#evidence-e44).

### remote-services

**S — Remote** (source): Provider streaming, external compaction routes and refreshed MCP are service consumption surfaces; local kernels/services remain owned.

Evidence: [e1](#evidence-e1), [e3](#evidence-e3), [e34](#evidence-e34), [e41](#evidence-e41), [e43](#evidence-e43).

### self-improvement

**S — Self-improve** (source): Dedicated model skill/memory modification changes procedural material; not governed harness compilation/candidate testing/promoting autoresearch.

Evidence: [e47](#evidence-e47), [e48](#evidence-e48), [e55](#evidence-e55).

### complaints

**L — Complaints** (source): Model issue-report device records grievance model/version/tool/report/time with consent and optional remote push. Not full agent-state capture, and Noted can precede durable acceptance.

Evidence: [e51](#evidence-e51), [e52](#evidence-e52), [e53](#evidence-e53).

### authority

**I — Authority** (source): Default yolo/autoapprove with explicit deny/tier rules; plan/device/coordination routing remains nuanced. Not a guaranteed permission-free harness.

Evidence: [e49](#evidence-e49), [e50](#evidence-e50), [e11](#evidence-e11), [e51](#evidence-e51).

### evaluation

**S — Evaluation** (source): Workflow tests probe actual worker script result/goal budget and fake kernel tests probe no-replay state. Eval judgment helper dispatch exists; broad scientific harness benchmarking not established.

Evidence: [e22](#evidence-e22), [e17](#evidence-e17).

### time-order

**L — Time/order** (source): Ordered persisted tool pairs differ from completion event order; kernel idle budget pauses restart window and uses wall time. No comprehensive causal clock/audit ordering contract.

Evidence: [e6](#evidence-e6), [e56](#evidence-e56).

## Inspected test oracles

- [quarantine/oh-my-pi/packages/coding-agent/test/tools/irc.test.ts](../../../quarantine/oh-my-pi/packages/coding-agent/test/tools/irc.test.ts): Delivery receipts, parked revival and busy session injection Oracle: Stub bus delivery and real AgentSession with mock prompt/forced streaming assert peer interrupt and parent steer queues; does not prove live provider consumes concurrent messages. Read, **not executed**.
- [quarantine/oh-my-pi/packages/coding-agent/test/eval/kernel-session-registry.test.ts](../../../quarantine/oh-my-pi/packages/coding-agent/test/eval/kernel-session-registry.test.ts): Dead uncertain cells and next execution Oracle: Fake kernel records original effect once, failing uncertain cell is not replayed; next call starts fresh. No external interpreter crash conformance. Read, **not executed**.
- [quarantine/oh-my-pi/packages/coding-agent/test/core/js-workflow-helpers.test.ts](../../../quarantine/oh-my-pi/packages/coding-agent/test/core/js-workflow-helpers.test.ts): Code helper phases and goal budget Oracle: executeJs worker tests assert logged phase/goal helper results with stub session tools; not executed in this study. Read, **not executed**.

## Useful mechanisms

- Actual live peer delivery and distinct parent steering rather than merely selecting providers.
- Persistent language kernels refuse replay of uncertain side effects.
- Explicit context maintenance boundaries retain source history and pair sibling results through rewind.
- Model issue device is a useful rageshake precursor.

## Material limits

- Study does not fully trace every large builtin tool or helper scheduler; residual catalog is labeled.
- Host remains TS/Bun and owns kernel/service lifecycles.
- No complete original audit or executable refit protocol demonstrated.
- Complaint acknowledgement is weaker than durable capture and stores no comprehensive state bundle.

## Arconaut design questions

- Separate message-as-steering from external abort and define preserved work when applying configuration now.
- Can eval and tool APIs consume shared kernel/job services without owning their lifecycle?
- What exact immutable event payload preserves nested calls, provider requests and complaint state for repair/evaluation?
- Use a tested quiescence/outpost refit instead of overloading plugin or session reload.

## Evidence

### Evidence e1

[quarantine/oh-my-pi/packages/coding-agent/src/sdk.ts:4222–4308](../../../quarantine/oh-my-pi/packages/coding-agent/src/sdk.ts#L4222): SDK wires Agent with model/tools/context transformations, provider payload/response hooks and streaming request function.

### Evidence e2

[quarantine/oh-my-pi/packages/agent/src/agent-loop.ts:1280–1329](../../../quarantine/oh-my-pi/packages/agent/src/agent-loop.ts#L1280): Loop yields, waits on process pause gate and synchronizes context before model calls.

### Evidence e3

[quarantine/oh-my-pi/packages/agent/src/agent-loop.ts:1404–1427](../../../quarantine/oh-my-pi/packages/agent/src/agent-loop.ts#L1404): Assistant streaming is invoked with live context and provider options.

### Evidence e4

[quarantine/oh-my-pi/packages/agent/src/agent-loop.ts:1619–1753](../../../quarantine/oh-my-pi/packages/agent/src/agent-loop.ts#L1619): Tool results enter active context; steering/asides/follow-up and onBeforeYield govern continuation.

### Evidence e5

[quarantine/oh-my-pi/packages/agent/src/agent-loop.ts:3038–3100](../../../quarantine/oh-my-pi/packages/agent/src/agent-loop.ts#L3038): Pure waits interrupt promptly; side-effecting tools receive external abort plus cooperative steering rather than unconditional steering abort.

### Evidence e6

[quarantine/oh-my-pi/packages/agent/src/agent-loop.ts:3217–3275](../../../quarantine/oh-my-pi/packages/agent/src/agent-loop.ts#L3217): Concurrent completion events differ from source-ordered persisted tool result pairs; results carry IDs and wall timestamps.

### Evidence e7

[quarantine/oh-my-pi/packages/coding-agent/src/tools/index.ts:561–604](../../../quarantine/oh-my-pi/packages/coding-agent/src/tools/index.ts#L561): Builtin registry exposes named native tools; hidden think/yield/goal are separate.

### Evidence e8

[quarantine/oh-my-pi/packages/coding-agent/src/tools/index.ts:768–802](../../../quarantine/oh-my-pi/packages/coding-agent/src/tools/index.ts#L768): Settings/recursion gates decide which experimental context, memory, checkpoint, wait and task tools enter sessions.

### Evidence e9

[quarantine/oh-my-pi/packages/coding-agent/src/tools/read.ts:672–676](../../../quarantine/oh-my-pi/packages/coding-agent/src/tools/read.ts#L672): Read accepts one path including inline selectors and internal URI/URL routes.

### Evidence e10

[quarantine/oh-my-pi/packages/coding-agent/src/tools/read.ts:1624–1679](../../../quarantine/oh-my-pi/packages/coding-agent/src/tools/read.ts#L1624): Filesystem pipeline resolves archives and SQLite selectors to actual readSqlite dispatch.

### Evidence e11

[quarantine/oh-my-pi/packages/coding-agent/src/tools/write.ts:777–938](../../../quarantine/oh-my-pi/packages/coding-agent/src/tools/write.ts#L777): Write dispatches internal handlers, archive and SQLite row writers, or filesystem replacement through plan/approval checks and LSP/editor bridge.

### Evidence e12

[quarantine/oh-my-pi/packages/coding-agent/src/tools/bash.ts:1049–1085](../../../quarantine/oh-my-pi/packages/coding-agent/src/tools/bash.ts#L1049): Service mode starts owned service with readiness configuration and returns name/state/PID/readiness timing.

### Evidence e13

[quarantine/oh-my-pi/packages/coding-agent/src/tools/bash.ts:1125–1190](../../../quarantine/oh-my-pi/packages/coding-agent/src/tools/bash.ts#L1125): Managed shell jobs can race completion/timeout/steering, promoting ongoing process to background; external abort cancels.

### Evidence e14

[quarantine/oh-my-pi/packages/coding-agent/src/tools/eval.ts:93–145](../../../quarantine/oh-my-pi/packages/coding-agent/src/tools/eval.ts#L93): Eval schema exposes language, code, title, timeout and reset; availability filters language choice.

### Evidence e15

[quarantine/oh-my-pi/packages/coding-agent/src/tools/eval.ts:603–706](../../../quarantine/oh-my-pi/packages/coding-agent/src/tools/eval.ts#L603): Eval owns async job records and can background on steering without terminating cell; external abort cancels.

### Evidence e16

[quarantine/oh-my-pi/packages/coding-agent/src/tools/eval.ts:855–952](../../../quarantine/oh-my-pi/packages/coding-agent/src/tools/eval.ts#L855): Eval streams tail output into artifact sink and invokes backend with session/owner identity, runtime budget and reset.

### Evidence e17

[quarantine/oh-my-pi/packages/coding-agent/src/eval/kernel-session-registry.ts:359–474](../../../quarantine/oh-my-pi/packages/coding-agent/src/eval/kernel-session-registry.ts#L359): Kernel lifecycle is owner-refcounted; dead/uncertain failed cells are never replayed automatically.

### Evidence e18

[quarantine/oh-my-pi/packages/coding-agent/src/eval/py/kernel.ts:277–386](../../../quarantine/oh-my-pi/packages/coding-agent/src/eval/py/kernel.ts#L277): Python kernel uses spawned interpreter; namespace snapshot is JSON-safe and idle-only, not complete live process serialization.

### Evidence e19

[quarantine/oh-my-pi/packages/coding-agent/src/eval/js/context-manager.ts:597–641](../../../quarantine/oh-my-pi/packages/coding-agent/src/eval/js/context-manager.ts#L597): JS evaluator reuses alive session workers keyed by session; new sessions retain owner IDs and pending runs.

### Evidence e20

[quarantine/oh-my-pi/packages/coding-agent/src/eval/js/context-manager.ts:974–1009](../../../quarantine/oh-my-pi/packages/coding-agent/src/eval/js/context-manager.ts#L974): JS eval prefers subprocess, falls back to Bun Worker; no claim of a native non-JS agent scheduler.

### Evidence e21

[quarantine/oh-my-pi/packages/coding-agent/src/eval/js/executor.ts:81–88](../../../quarantine/oh-my-pi/packages/coding-agent/src/eval/js/executor.ts#L81): JS forced deadline kill resets VM and loses preceding variables.

### Evidence e22

[quarantine/oh-my-pi/packages/coding-agent/src/eval/js/tool-bridge.ts:211–346](../../../quarantine/oh-my-pi/packages/coding-agent/src/eval/js/tool-bridge.ts#L211): Eval composition dispatches helper operations and validated tools; direct-only checkpoint/rewind are rejected, bridged todo explicitly persisted.

### Evidence e23

[quarantine/oh-my-pi/packages/coding-agent/src/eval/py/tool-bridge.ts:135–205](../../../quarantine/oh-my-pi/packages/coding-agent/src/eval/py/tool-bridge.ts#L135): Python tool bridge is token-authenticated loopback HTTP and dispatches through shared session bridge.

### Evidence e24

[quarantine/oh-my-pi/packages/coding-agent/src/irc/messaging.ts:45–102](../../../quarantine/oh-my-pi/packages/coding-agent/src/irc/messaging.ts#L45): Send targets visible agents via process-global bus and returns delivery receipts; broadcast is per-recipient delivery.

### Evidence e25

[quarantine/oh-my-pi/packages/coding-agent/src/irc/bus.ts:22–25](../../../quarantine/oh-my-pi/packages/coding-agent/src/irc/bus.ts#L22): Mailbox capacity is 100 in process-global bus.

### Evidence e26

[quarantine/oh-my-pi/packages/coding-agent/src/irc/bus.ts:97–179](../../../quarantine/oh-my-pi/packages/coding-agent/src/irc/bus.ts#L97): Recipients can be unavailable/read-only; parked live hosts are revived, pending waiter or session receives message, failure can queue.

### Evidence e27

[quarantine/oh-my-pi/packages/coding-agent/src/session/irc-bridge.ts:174–242](../../../quarantine/oh-my-pi/packages/coding-agent/src/session/irc-bridge.ts#L174): Busy parent-to-child messaging steers; peer messages queue interrupt, idle plan appends without wake, other idle messages wake session.

### Evidence e28

[quarantine/oh-my-pi/packages/coding-agent/src/internal-urls/agent-protocol.ts:98–130](../../../quarantine/oh-my-pi/packages/coding-agent/src/internal-urls/agent-protocol.ts#L98): Model-facing write agent:// target validates sender scope then sends body and returns delivery status.

### Evidence e29

[quarantine/oh-my-pi/packages/coding-agent/src/internal-urls/history-protocol.ts:260–282](../../../quarantine/oh-my-pi/packages/coding-agent/src/internal-urls/history-protocol.ts#L260): Full branch rendering includes entry IDs and raw entry data.

### Evidence e30

[quarantine/oh-my-pi/packages/coding-agent/src/internal-urls/history-protocol.ts:343–365](../../../quarantine/oh-my-pi/packages/coding-agent/src/internal-urls/history-protocol.ts#L343): history://current/full requires experimental context flag and bound current session, avoiding unsafe fallback.

### Evidence e31

[quarantine/oh-my-pi/packages/coding-agent/src/tools/context-notes.ts:102–141](../../../quarantine/oh-my-pi/packages/coding-agent/src/tools/context-notes.ts#L102): Model notebook replacement checks byte cap/current branch owner, appends custom entry and flushes.

### Evidence e32

[quarantine/oh-my-pi/packages/coding-agent/src/tools/context-notes.ts:149–179](../../../quarantine/oh-my-pi/packages/coding-agent/src/tools/context-notes.ts#L149): new_context returns turn-local rollover request, rather than directly replacing active history.

### Evidence e33

[quarantine/oh-my-pi/packages/coding-agent/src/session/session-maintenance.ts:2581–2634](../../../quarantine/oh-my-pi/packages/coding-agent/src/session/session-maintenance.ts#L2581): Maintenance consumes model new_context at paired-result boundary, persists turn before rollover and fences abort/disposal.

### Evidence e34

[quarantine/oh-my-pi/packages/coding-agent/src/session/session-maintenance.ts:1164–1220](../../../quarantine/oh-my-pi/packages/coding-agent/src/session/session-maintenance.ts#L1164): Manual compaction resolves supported remote/snapcompact/soft method and provider route fallback.

### Evidence e35

[quarantine/oh-my-pi/packages/coding-agent/src/session/session-maintenance.ts:4275–4343](../../../quarantine/oh-my-pi/packages/coding-agent/src/session/session-maintenance.ts#L4275): Automatic maintenance wires shake, deferred handoff, snapcompact and remote/context-full dispatch.

### Evidence e36

[quarantine/oh-my-pi/packages/coding-agent/src/session/session-maintenance.ts:2382–2421](../../../quarantine/oh-my-pi/packages/coding-agent/src/session/session-maintenance.ts#L2382): Compaction commits summary, retained entry boundary, method/preserve metadata then rebuilds active message projection.

### Evidence e37

[quarantine/oh-my-pi/packages/coding-agent/src/session/session-manager.ts:2986–3032](../../../quarantine/oh-my-pi/packages/coding-agent/src/session/session-manager.ts#L2986): Compaction/reset/custom boundaries are stored; reset contract preserves full transcript on disk.

### Evidence e38

[quarantine/oh-my-pi/packages/coding-agent/src/session/session-manager.ts:1420–1464](../../../quarantine/oh-my-pi/packages/coding-agent/src/session/session-manager.ts#L1420): Storage append distinguishes sync page-cache publication from async confirmation; failures latch and future entries retry rewrite.

### Evidence e39

[quarantine/oh-my-pi/packages/coding-agent/src/tools/checkpoint.ts:1–131](../../../quarantine/oh-my-pi/packages/coding-agent/src/tools/checkpoint.ts#L1): checkpoint/rewind tools expose exploration goal and report, enforce active checkpoint and completion constraints.

### Evidence e40

[quarantine/oh-my-pi/packages/coding-agent/src/session/agent-session.ts:9840–9918](../../../quarantine/oh-my-pi/packages/coding-agent/src/session/agent-session.ts#L9840): Direct rewind result triggers branchWithSummary; completed sibling tool pairs are reparented to remain visible.

### Evidence e41

[quarantine/oh-my-pi/packages/coding-agent/src/slash-commands/builtin-marketplace.ts:25–41](../../../quarantine/oh-my-pi/packages/coding-agent/src/slash-commands/builtin-marketplace.ts#L25): Plugin refresh clears discovery/skills/command caches and reconnects MCP; source does not prove all hook code reimport.

### Evidence e42

[quarantine/oh-my-pi/packages/coding-agent/src/slash-commands/builtin-marketplace.ts:553–565](../../../quarantine/oh-my-pi/packages/coding-agent/src/slash-commands/builtin-marketplace.ts#L553): Operator /reload-plugins command dispatches runtime refresh.

### Evidence e43

[quarantine/oh-my-pi/packages/coding-agent/src/session/agent-session.ts:5390–5433](../../../quarantine/oh-my-pi/packages/coding-agent/src/session/agent-session.ts#L5390): Session disposal aborts agent/retries/compaction, drains, disposes owned jobs/kernels/browser/computer/MCP; not pause-and-refit.

### Evidence e44

[quarantine/oh-my-pi/packages/coding-agent/src/session/agent-session.ts:10585–10683](../../../quarantine/oh-my-pi/packages/coding-agent/src/session/agent-session.ts#L10585): AgentSession.reload switches same on-disk session, aborting/disconnecting old session; reload naming is not executable replacement.

### Evidence e45

[quarantine/oh-my-pi/packages/coding-agent/src/tools/sqlite-reader.ts:24–61](../../../quarantine/oh-my-pi/packages/coding-agent/src/tools/sqlite-reader.ts#L24): SQLite reads open connection and enable query_only plus busy timeout.

### Evidence e46

[quarantine/oh-my-pi/packages/coding-agent/src/tools/sqlite-reader.ts:561–581](../../../quarantine/oh-my-pi/packages/coding-agent/src/tools/sqlite-reader.ts#L561): SQLite selectors support raw q SQL query, model-visible standing disk data distinct from kernel namespace.

### Evidence e47

[quarantine/oh-my-pi/packages/coding-agent/src/tools/manage-skill.ts:16–100](../../../quarantine/oh-my-pi/packages/coding-agent/src/tools/manage-skill.ts#L16): Model tool create/update/delete manages authored procedural skill files, refreshes discovery, prevents overriding shipped skill name.

### Evidence e48

[quarantine/oh-my-pi/packages/coding-agent/src/tools/learn.ts:49–110](../../../quarantine/oh-my-pi/packages/coding-agent/src/tools/learn.ts#L49): Learn is backend-gated and can write scoped long-term memory; not a harness candidate evaluation loop.

### Evidence e49

[quarantine/oh-my-pi/packages/coding-agent/src/tools/approval.ts:53–82](../../../quarantine/oh-my-pi/packages/coding-agent/src/tools/approval.ts#L53): Configured default approval is yolo, explicit autoapprove forces it; missing tool context asks.

### Evidence e50

[quarantine/oh-my-pi/packages/coding-agent/src/tools/approval.ts:188–238](../../../quarantine/oh-my-pi/packages/coding-agent/src/tools/approval.ts#L188): Policy resolves tool tier/per-tool rules and explicit denial despite permissive mode.

### Evidence e51

[quarantine/oh-my-pi/packages/coding-agent/src/internal-urls/xd-protocol.ts:78–108](../../../quarantine/oh-my-pi/packages/coding-agent/src/internal-urls/xd-protocol.ts#L78): write xd://report_issue dispatches real complaint device and retains inner tool approval tier.

### Evidence e52

[quarantine/oh-my-pi/packages/coding-agent/src/tools/report-tool-issue.ts:248–281](../../../quarantine/oh-my-pi/packages/coding-agent/src/tools/report-tool-issue.ts#L248): SQLite grievance schema stores model/version/tool/report/time and push/error status, not full agent snapshot.

### Evidence e53

[quarantine/oh-my-pi/packages/coding-agent/src/tools/report-tool-issue.ts:591–630](../../../quarantine/oh-my-pi/packages/coding-agent/src/tools/report-tool-issue.ts#L591): Complaint consent controls record/flush; acknowledgement can precede consent resolution so Noted is not durable-record oracle.

### Evidence e54

[quarantine/oh-my-pi/packages/coding-agent/src/session/agent-session.ts:7370–7396](../../../quarantine/oh-my-pi/packages/coding-agent/src/session/agent-session.ts#L7370): Before-agent-start extension may replace system prompt/add messages under policy freshness fence.

### Evidence e55

[quarantine/oh-my-pi/packages/coding-agent/src/session/agent-session.ts:7718–7783](../../../quarantine/oh-my-pi/packages/coding-agent/src/session/agent-session.ts#L7718): Extension context exposes branch/tree/compact/ephemeral turn/timers/session switching and idle/abort APIs.

### Evidence e56

[quarantine/oh-my-pi/packages/coding-agent/src/eval/idle-timeout.ts:1–90](../../../quarantine/oh-my-pi/packages/coding-agent/src/eval/idle-timeout.ts#L1): Runtime watchdog uses wall time/setTimeout; nested pauses restart fresh timeout window on final resume.

