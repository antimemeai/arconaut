# stakpak-agent

Stakpak is a DevOps-oriented native agent with interactive/async CLI, server run coordination, local/remote tracked process tasks, optional child agents, skills/docs and standing autopilot schedules.

Role: coding agent. Runtime: Rust.

Pinned source: [https://github.com/stakpak/agent](https://github.com/stakpak/agent); revision/version `760cd2b5984d29c2d513bb15ca33e995fae45f17`.

Rust Tokio CLI/client and separate server session actors; native provider SDK; MCP server TaskManager owns local/SSH/background child processes; autopilot scheduler/gateway.

Consumes provider HTTP/MCP/SSH/API; owns checkpoints/session services, schedule DB, task manager and optional sandbox containers. Shared persistent sandbox server-owned, ephemeral session-owned.

Inspection: Traced both distinct CLI AgentClient completion/tool continuation and server agent-core actor -> native provider -> MCP result -> checkpoint/event continuation; inspected task lifecycle, subagent resume, router, scheduler reload, request trimming and source oracles.

Limits of this study: Source read only, reference not executed. References not executed. Warden network enforcement/secret substitution full implementation and other provider protocols not audited; no full security assurance inferred. Gateway dispatcher state read at ownership boundary, not complete channel transport conformance. Cancellation/no-progress risks are source inferences, not reproduced runtime faults.

## Actions

### view, str_replace, create, remove

Surface: model MCP tools.

Input: local or SSH path; line range/grep/glob/tree; exact old/new; file text; recursive; optional SSH auth

Result: text/tree/search max300 lines; mutation/errors and selected backup paths

Lifecycle: awaited; tools route local or SSH connection

Authority: profile/tool approval and sandbox/host boundary; uses operator OS/SSH authority

Evidence: [e20](#evidence-e20), [e26](#evidence-e26), [e27](#evidence-e27).

### run_command, run_remote_command

Surface: model MCP tools.

Input: command,description,timeout; remote/password/private_key_path

Result: formatted exit/output or error

Lifecycle: awaited synchronous service execution with optional timeout

Authority: approval/profile policy; local host or SSH connection

Evidence: [e21](#evidence-e21).

### run_command_task, run_remote_command_task

Surface: model MCP tools.

Input: command,description,timeout and optional remote auth

Result: task_id,status,start_time

Lifecycle: returns background task after bounded start check; manager owns process/remote connection

Authority: same tool policy; global manager not session-isolated

Evidence: [e21](#evidence-e21), [e22](#evidence-e22), [e32](#evidence-e32), [e34](#evidence-e34), [e35](#evidence-e35).

### get_all_tasks, get_task_details, wait_for_tasks, cancel_task

Surface: model MCP tools.

Input: task_id or whitespace task_ids; wait timeout

Result: status/output/duration/pause info; wait error or cancel result

Lifecycle: inspection/wait/terminate; cancelled task removed; no persisted process identity across manager death

Authority: task ID possession plus tool access; no session guard traced

Evidence: [e23](#evidence-e23), [e24](#evidence-e24), [e25](#evidence-e25), [e32](#evidence-e32), [e33](#evidence-e33).

### dynamic_subagent_task, resume_subagent_task

Surface: model MCP tools (flag enabled).

Input: instructions/context/tools/model/max_steps/sandbox; task_id decisions or new input

Result: background child task identity; paused checkpoint details; resumed new CLI invocation

Lifecycle: asynchronous subprocess; inspect/wait/cancel by task tools; checkpoint resume after paused/completed status

Authority: parent chooses child model/tools and approval input; launches same executable, not refit

Evidence: [e20](#evidence-e20), [e30](#evidence-e30), [e31](#evidence-e31).

### search_docs, load_skill

Surface: model MCP tools.

Input: keywords/search criteria; skill URI

Result: up to five docs results; local skill body/path or remote rulebook text

Lifecycle: awaited service/file read; load_skill local path restricted to configured directories

Authority: profile tools plus optional API; not model-owned persistent evidence DB

Evidence: [e36](#evidence-e36), [e38](#evidence-e38).

### generate_password, view_web_page

Surface: model MCP tools.

Input: length/no_symbols; HTTPS url

Result: generated password; converted webpage text or error

Lifecycle: awaited native utility/HTTP action

Authority: tools/profile; output password itself returned as text

Evidence: [e28](#evidence-e28).

### ask_user

Surface: model tool / TUI interception.

Input: question option structures

Result: interactive answer in TUI; INTERACTIVE_REQUIRED at actual server handler

Lifecycle: requires interactive path, cannot be assumed available to autopilot/headless

Authority: model asks, operator answers

Evidence: [e29](#evidence-e29).

### slack_read_messages, slack_read_replies, slack_send_message

Surface: optional model MCP tools.

Input: channel/thread/message request

Result: API content/error

Lifecycle: awaited external transport via configured AgentProvider

Authority: optional Slack family/profile and credentials; no inherent model peer routing

Evidence: [e20](#evidence-e20), [e48](#evidence-e48).

### run_agent; AgentCommand Steering, FollowUp, SwitchModel, Cancel, ResolveTool(s)

Surface: programmable API / server operator API.

Input: session/run identity; prompt, model or tool call decision

Result: typed run/turn/action events and messages/metadata

Lifecycle: steer between provider/tool phases, skip pending calls with result placeholders; follow-up after finish; no immediate cancellation during inference/approval wait

Authority: host/session API; stale run ID rejected

Evidence: [e7](#evidence-e7), [e10](#evidence-e10), [e12](#evidence-e12), [e13](#evidence-e13), [e14](#evidence-e14), [e15](#evidence-e15), [e16](#evidence-e16), [e17](#evidence-e17), [e18](#evidence-e18).

### CLI -a pause/resume checkpoints; interactive session/model/profile switching

Surface: operator commands.

Input: prompt/session/checkpoint plus decisions/input, chosen profile/model

Result: pause manifest/outcome or continued chat

Lifecycle: CLI distinct pipeline; interactive stream cancellation select; profile outer loop restarts session services

Authority: operator; ordinary model/API config settings

Evidence: [e2](#evidence-e2), [e3](#evidence-e3), [e4](#evidence-e4), [e5](#evidence-e5), [e6](#evidence-e6), [e55](#evidence-e55).

### autopilot schedule add/remove/enable/disable; cron/check scripts

Surface: operator / ordinary shell-program surface.

Input: name,cron,prompt/check,profile,max_turns,sandbox,notification route

Result: scheduled run/session/checkpoint status in DB

Lifecycle: long-lived scheduler reconciles changed config; skip prior running schedule; errors can bypass singleton check

Authority: operator can configure; model can use shell as composition, no native workflow code interpreter

Evidence: [e43](#evidence-e43), [e44](#evidence-e44), [e45](#evidence-e45).

### SSE event replay; latest/checkpoint messages API

Surface: operator / programmable client API.

Input: session and event cursor or saved checkpoint identity

Result: typed event envelopes, replay or gap requiring snapshot refresh; saved messages

Lifecycle: bounded in-memory replay; latest envelope rename-replaced

Authority: server API; no permanent complete event audit

Evidence: [e8](#evidence-e8), [e9](#evidence-e9), [e41](#evidence-e41), [e42](#evidence-e42), [e51](#evidence-e51).

### AgentHook / HookContext lifecycle

Surface: supplied host primitives.

Input: compiled async callback around inference/tool/request and state

Result: error/Continue/Skip/Abort; mutable CLI inference context; server hook snapshots

Lifecycle: host-selected hooks; server AgentHook message slice is immutable

Authority: program author; no runtime model-owned turn replacement

Evidence: [e6](#evidence-e6), [e49](#evidence-e49).

## Capabilities

### filesystem

**I — Files** (source): Native local/SSH view/grep/tree/str_replace/create/remove; bounded views and backup machinery. External tools selected by router/profile.

Evidence: [e20](#evidence-e20), [e26](#evidence-e26), [e27](#evidence-e27).

### processes

**I — OS programs** (source): Sync/background local+SSH command tasks with IDs/status/output/wait/cancel, null stdin/process groups; manager owns/terminates work.

Evidence: [e21](#evidence-e21), [e22](#evidence-e22), [e23](#evidence-e23), [e24](#evidence-e24), [e25](#evidence-e25), [e32](#evidence-e32), [e33](#evidence-e33), [e34](#evidence-e34), [e35](#evidence-e35).

### code-actions

**S — Code actions** (source): Shell tasks can run ordinary programs, subprocess subagents and check scripts; no persistent general-purpose kernel/code-action interpreter traced.

Evidence: [e21](#evidence-e21), [e30](#evidence-e30), [e44](#evidence-e44).

### persistent-kernel

**? — Kernel** (source): Not established in CLI/server loops or native MCP catalog; standing shell task is not a programmable interpreter namespace.

### standing-database

**L — Standing DB** (source): Session/checkpoint/scheduler DB state internal; README memory claim does not establish model standing DB and search_memory tool is commented out.

Evidence: [e37](#evidence-e37), [e41](#evidence-e41), [e44](#evidence-e44).

### workflow-programming

**L — Workflows** (source): Cron/check/prompt/profile workflows, background coordination and subprocess child checkpoint resume; hook primitives host compiled. No model-native program orchestration interpreter.

Evidence: [e24](#evidence-e24), [e30](#evidence-e30), [e31](#evidence-e31), [e43](#evidence-e43), [e44](#evidence-e44), [e45](#evidence-e45), [e49](#evidence-e49).

### multi-model

**I — Models** (source): Different per-profile/server models and explicit child model resolution; server SwitchModel applies later request boundary.

Evidence: [e10](#evidence-e10), [e15](#evidence-e15), [e30](#evidence-e30), [e55](#evidence-e55).

### live-collaboration

**S — Peer chat** (source): Child task/resume plus optional Slack read/send and gateway conversations can compose interaction; no model peer room/identity/broadcast/receipt protocol established.

Evidence: [e30](#evidence-e30), [e31](#evidence-e31), [e46](#evidence-e46), [e47](#evidence-e47), [e48](#evidence-e48).

### concurrent-work

**I — Concurrency** (source): Independent session actors, background process/subagent tasks and scheduled runs; one active run per session with stale-ID guards.

Evidence: [e7](#evidence-e7), [e16](#evidence-e16), [e17](#evidence-e17), [e21](#evidence-e21), [e22](#evidence-e22), [e30](#evidence-e30), [e44](#evidence-e44).

### steering-interrupt

**L — Steer/interrupt** (source): Server steering skips unstarted calls with paired placeholders; follow-up after finish; MCP calls cancel-select. Inference/retry sleep/approval recv do not promptly observe flipped cancellation token. Interactive CLI has separate stream cancel select.

Evidence: [e4](#evidence-e4), [e10](#evidence-e10), [e11](#evidence-e11), [e13](#evidence-e13), [e14](#evidence-e14), [e15](#evidence-e15), [e17](#evidence-e17), [e18](#evidence-e18).

### turn-redefinition

**S — Turn program** (source): Compiled lifecycle hooks and configurable profiles/tool approval/turn count; request/action/continuation structure stays compiled. CLI mutable context hook broader than immutable server hook.

Evidence: [e6](#evidence-e6), [e9](#evidence-e9), [e49](#evidence-e49).

### compaction

**L — Compaction** (source): Budget reducer trims request copies with stable metadata boundary and pairing cleanup; server overflow compactor is passthrough and repeats unchanged context with decremented turn count (source-inferred no-progress risk).

Evidence: [e9](#evidence-e9), [e10](#evidence-e10), [e11](#evidence-e11), [e39](#evidence-e39), [e40](#evidence-e40), [e54](#evidence-e54).

### context-repair

**S — Repair** (source): Original message vectors remain in checkpoint while request reducer trims copies; CLI resumes selected saved checkpoint. No model selective history-repair action or untrim override traced.

Evidence: [e3](#evidence-e3), [e8](#evidence-e8), [e9](#evidence-e9), [e31](#evidence-e31), [e39](#evidence-e39), [e41](#evidence-e41).

### original-audit

**L — Original audit** (source): Latest snapshots and bounded in-memory event ring; core text/reasoning deltas synthesized after full generate, nontext tool output omitted. Raw provider request/delta/everything journal not established.

Evidence: [e9](#evidence-e9), [e12](#evidence-e12), [e18](#evidence-e18), [e41](#evidence-e41), [e42](#evidence-e42).

### audit-query

**L — Audit query** (source): Cursor replay with explicit gap and snapshot/messages API plus scheduler histories/checkpoints; incomplete original audit and ring lost on server death.

Evidence: [e41](#evidence-e41), [e42](#evidence-e42), [e45](#evidence-e45), [e51](#evidence-e51).

### hot-change

**I — Hot change** (source): Scheduler config mtime/DB signal reconciliation preserves old state on parse/load error, rejects DB-path changes; CLI model changes/profile restart. No universal settled workflow activation transaction.

Evidence: [e43](#evidence-e43), [e45](#evidence-e45), [e53](#evidence-e53), [e55](#evidence-e55).

### rebuild-continuity

**S — Rebuild continuity** (source): Versioned latest checkpoint and child pause/resume persist context; server/manager work ownership and kill-on-drop require composition for refit. No outpost/quiescence executable handoff traced.

Evidence: [e8](#evidence-e8), [e9](#evidence-e9), [e31](#evidence-e31), [e35](#evidence-e35), [e41](#evidence-e41), [e55](#evidence-e55).

### remote-services

**I — Remote** (source): Provider HTTP/MCP/SSH/docs/rulebook/optional Slack gateway services; session host may share persistent sandbox at server scope.

Evidence: [e8](#evidence-e8), [e18](#evidence-e18), [e19](#evidence-e19), [e21](#evidence-e21), [e22](#evidence-e22), [e36](#evidence-e36), [e38](#evidence-e38), [e46](#evidence-e46), [e48](#evidence-e48).

### self-improvement

**? — Self-improve** (source): File/program/skill editing is available; measured model-governed harness change/rebuild/evaluation loop not established in inspected paths.

### complaints

**? — Complaints** (source): RunError and external notifications exist; model grievance with state capture and external complaint record not established.

### authority

**I — Authority** (source): ToolApprovalPolicy per profile/server; allowlist/autopilot controls, run-ID validation and optional shared/ephemeral sandbox. Full Warden/secret security guarantee not audited in this study.

Evidence: [e9](#evidence-e9), [e16](#evidence-e16), [e17](#evidence-e17), [e20](#evidence-e20), [e30](#evidence-e30), [e38](#evidence-e38).

### evaluation

**I — Evaluation** (source): Read concurrency guard, budget sanitation, ring gap and schedule reload tests. Drop child-kill named test lacks post-drop process liveness oracle.

Evidence: [e50](#evidence-e50), [e51](#evidence-e51), [e52](#evidence-e52), [e53](#evidence-e53), [e54](#evidence-e54).

### time-order

**S — Time/order** (source): UUID run/task IDs, request/turn counts, wall times, cron/durations and replay cursor; per-session counter allocated before ring lock is not a global durable causal order.

Evidence: [e7](#evidence-e7), [e10](#evidence-e10), [e32](#evidence-e32), [e42](#evidence-e42), [e43](#evidence-e43).

## Inspected test oracles

- [quarantine/stakpak-agent/libs/server/src/session_manager.rs](../../../quarantine/stakpak-agent/libs/server/src/session_manager.rs): Double start / stale run Oracle: Barrier two starts: exactly one success and one conflict; direct run-ID rejection tests. It does not prove cross-process coordination. Read, **not executed**.
- [quarantine/stakpak-agent/libs/server/src/event_log.rs](../../../quarantine/stakpak-agent/libs/server/src/event_log.rs): Replay gap and cursor scope Oracle: Sequential publications assert replay after cursor and ring-loss gap bounds; no restart durability or concurrent publisher ordering oracle. Read, **not executed**.
- [quarantine/stakpak-agent/libs/agent-core/tests/budget_context_reducer.rs](../../../quarantine/stakpak-agent/libs/agent-core/tests/budget_context_reducer.rs): Request trimming/pair validity Oracle: Checks user/system preserved and assistant/tool placeholders, pairing and trim boundary stability using estimator; not provider tokenizer correctness. Read, **not executed**.
- [quarantine/stakpak-agent/libs/shared/src/task_manager.rs](../../../quarantine/stakpak-agent/libs/shared/src/task_manager.rs): Drop ownership / child kill claims Oracle: Manager-exit flag checked; named child-kill test only asserts Running before drop then sleeps/cleans marker, no post-drop liveness assertion. Read, **not executed**.
- [quarantine/stakpak-agent/cli/src/commands/watch/commands/run.rs](../../../quarantine/stakpak-agent/cli/src/commands/watch/commands/run.rs): Failed reload retention Oracle: Injected config-read failure returns false and keeps registered count; runtime token and schedule reconciliation tests separate. Read, **not executed**.

## Useful mechanisms

- Run IDs and atomic Starting reservation make per-session ownership/races explicit.
- Concrete process task IDs, remote SSH handling and paused child checkpoints are useful lifecycle precedents.
- Request-copy trimming separates presentation budget from stored original message vector.
- Bounded replay reports missing events instead of silently promising completeness.
- Schedule reload rejects incompatible storage changes and retains prior config on load error.

## Material limits

- References not executed. Warden network enforcement/secret substitution full implementation and other provider protocols not audited; no full security assurance inferred. Gateway dispatcher state read at ownership boundary, not complete channel transport conformance. Cancellation/no-progress risks are source inferences, not reproduced runtime faults.

## Arconaut design questions

- Should cancellation wake every awaited phase and require acknowledged quiescence rather than setting a token?
- How should compaction prove progress before retry and preserve retrievable original material?
- Which process/task manager belongs in shared fabric, and which client identity/lease guards access?
- How should schedule activation coordinate ongoing workflows and model-selected runtime changes?
- What permanent raw audit and grievance capture must be added beyond replay ring/checkpoints?

## Evidence

### Evidence e1

[quarantine/stakpak-agent/cli/src/main.rs:263–277](../../../quarantine/stakpak-agent/cli/src/main.rs#L263): Tokio native CLI entry.

### Evidence e2

[quarantine/stakpak-agent/cli/src/commands/agent/run/mode_async.rs:487–544](../../../quarantine/stakpak-agent/cli/src/commands/agent/run/mode_async.rs#L487): Async CLI model request through AgentClient, accumulate usage and append assistant.

### Evidence e3

[quarantine/stakpak-agent/cli/src/commands/agent/run/mode_async.rs:602–725](../../../quarantine/stakpak-agent/cli/src/commands/agent/run/mode_async.rs#L602): CLI pending-approval pause manifest/checkpoint and sequential MCP execution with hour timeout.

### Evidence e4

[quarantine/stakpak-agent/cli/src/commands/agent/run/mode_interactive.rs:1398–1456](../../../quarantine/stakpak-agent/cli/src/commands/agent/run/mode_interactive.rs#L1398): Interactive CLI streams AgentClient reply and races stream processing against cancellation; calls cancel_stream.

### Evidence e5

[quarantine/stakpak-agent/libs/api/src/client/provider.rs:178–224](../../../quarantine/stakpak-agent/libs/api/src/client/provider.rs#L178): AgentClient hooks before/after request and creates saved session/checkpoint around completion.

### Evidence e6

[quarantine/stakpak-agent/libs/api/src/client/provider.rs:830–914](../../../quarantine/stakpak-agent/libs/api/src/client/provider.rs#L830): Mutable CLI inference hook prepares input, local stakai chat/chat_stream dispatch and output hook.

### Evidence e7

[quarantine/stakpak-agent/libs/server/src/session_actor.rs:43–80](../../../quarantine/stakpak-agent/libs/server/src/session_actor.rs#L43): Server creates command queue/cancellation handle and owned session task; marks finished afterward.

### Evidence e8

[quarantine/stakpak-agent/libs/server/src/session_actor.rs:94–166](../../../quarantine/stakpak-agent/libs/server/src/session_actor.rs#L94): Session loads latest checkpoint; persistent sandbox shared at server scope or owned ephemeral container; host tools fallback explicit.

### Evidence e9

[quarantine/stakpak-agent/libs/server/src/session_actor.rs:207–328](../../../quarantine/stakpak-agent/libs/server/src/session_actor.rs#L207): Server baseline/periodic/terminal checkpoints; actual run_agent wiring uses PassthroughCompactionEngine and BudgetAwareContextReducer.

### Evidence e10

[quarantine/stakpak-agent/libs/agent-core/src/agent.rs:66–201](../../../quarantine/stakpak-agent/libs/agent-core/src/agent.rs#L66): Core drains steering/model switches at boundary, request-copy context reduction and awaited inference; context overflow invokes compactor.

### Evidence e11

[quarantine/stakpak-agent/libs/agent-core/src/agent.rs:201–236](../../../quarantine/stakpak-agent/libs/agent-core/src/agent.rs#L201): Overflow compacts, subtracts turn count and restarts; retries sleep without cancellation select.

### Evidence e12

[quarantine/stakpak-agent/libs/agent-core/src/agent.rs:254–389](../../../quarantine/stakpak-agent/libs/agent-core/src/agent.rs#L254): Full inference result emits text/reasoning pseudo-deltas then assistant and proposed tool events; these are not raw provider stream chunks.

### Evidence e13

[quarantine/stakpak-agent/libs/agent-core/src/agent.rs:420–470](../../../quarantine/stakpak-agent/libs/agent-core/src/agent.rs#L420): Follow-up queue runs after no-tool completion; terminal messages/metadata returned.

### Evidence e14

[quarantine/stakpak-agent/libs/agent-core/src/agent.rs:490–659](../../../quarantine/stakpak-agent/libs/agent-core/src/agent.rs#L490): Sequential approval/action cycle keeps call pairing, appends cancelled/skipped placeholders and drains steering between actions.

### Evidence e15

[quarantine/stakpak-agent/libs/agent-core/src/agent.rs:659–707](../../../quarantine/stakpak-agent/libs/agent-core/src/agent.rs#L659): Approval wait blocks on command_rx.recv, with no cancellation-token select; commands include Steering, FollowUp, SwitchModel, Cancel.

### Evidence e16

[quarantine/stakpak-agent/libs/server/src/session_manager.rs:48–110](../../../quarantine/stakpak-agent/libs/server/src/session_manager.rs#L48): Atomic one-active-run per session with Starting reservation and stale-start handling.

### Evidence e17

[quarantine/stakpak-agent/libs/server/src/session_manager.rs:146–224](../../../quarantine/stakpak-agent/libs/server/src/session_manager.rs#L146): Commands/cancel validated by session/run ID; cancel_run only flips token.

### Evidence e18

[quarantine/stakpak-agent/libs/server/src/session_actor.rs:600–697](../../../quarantine/stakpak-agent/libs/server/src/session_actor.rs#L600): MCP metadata carries session/run/call IDs; tool request races token with response and sends cancellation notification; nontext output omitted.

### Evidence e19

[quarantine/stakpak-agent/libs/ai/src/providers/openai/provider.rs:489–535](../../../quarantine/stakpak-agent/libs/ai/src/providers/openai/provider.rs#L489): Native provider posts converted request for Responses or Chat Completions and parses response.

### Evidence e20

[quarantine/stakpak-agent/libs/mcp/server/src/lib.rs:225–278](../../../quarantine/stakpak-agent/libs/mcp/server/src/lib.rs#L225): Local/remote/combined router assembly; subagent and Slack tool families conditionally enabled.

### Evidence e21

[quarantine/stakpak-agent/libs/mcp/server/src/local_tools.rs:307–414](../../../quarantine/stakpak-agent/libs/mcp/server/src/local_tools.rs#L307): Synchronous local/SSH commands and local background task delegation into TaskManager.

### Evidence e22

[quarantine/stakpak-agent/libs/mcp/server/src/local_tools.rs:436–477](../../../quarantine/stakpak-agent/libs/mcp/server/src/local_tools.rs#L436): SSH background command creates remote connection and starts tracked task.

### Evidence e23

[quarantine/stakpak-agent/libs/mcp/server/src/local_tools.rs:492–518](../../../quarantine/stakpak-agent/libs/mcp/server/src/local_tools.rs#L492): Task list displays manager-global tasks and paused-task progress.

### Evidence e24

[quarantine/stakpak-agent/libs/mcp/server/src/local_tools.rs:606–688](../../../quarantine/stakpak-agent/libs/mcp/server/src/local_tools.rs#L606): cancel_task and wait_for_tasks operate task IDs with terminal statuses and optional timeout.

### Evidence e25

[quarantine/stakpak-agent/libs/mcp/server/src/local_tools.rs:703–788](../../../quarantine/stakpak-agent/libs/mcp/server/src/local_tools.rs#L703): get_task_details returns status/output/paused details for task ID.

### Evidence e26

[quarantine/stakpak-agent/libs/mcp/server/src/local_tools.rs:827–967](../../../quarantine/stakpak-agent/libs/mcp/server/src/local_tools.rs#L827): Filesystem view/search/tree and exact string replace/create route local or SSH path; view capped 300 lines.

### Evidence e27

[quarantine/stakpak-agent/libs/mcp/server/src/local_tools.rs:1121–1152](../../../quarantine/stakpak-agent/libs/mcp/server/src/local_tools.rs#L1121): remove routes recursive/local/remote operations.

### Evidence e28

[quarantine/stakpak-agent/libs/mcp/server/src/local_tools.rs:983–1038](../../../quarantine/stakpak-agent/libs/mcp/server/src/local_tools.rs#L983): Password generation and HTTPS webpage fetch are actual tools.

### Evidence e29

[quarantine/stakpak-agent/libs/mcp/server/src/local_tools.rs:3089–3104](../../../quarantine/stakpak-agent/libs/mcp/server/src/local_tools.rs#L3089): ask_user advertised but stub errors in server/headless; TUI intercept is required.

### Evidence e30

[quarantine/stakpak-agent/libs/mcp/server/src/subagent_tools.rs:169–270](../../../quarantine/stakpak-agent/libs/mcp/server/src/subagent_tools.rs#L169): Dynamic child tool resolves explicit/config/parent model and tools, launches CLI process as background task.

### Evidence e31

[quarantine/stakpak-agent/libs/mcp/server/src/subagent_tools.rs:333–430](../../../quarantine/stakpak-agent/libs/mcp/server/src/subagent_tools.rs#L333): Resume child reads paused/completed task checkpoint, builds a new CLI invocation and passes tool decisions or input.

### Evidence e32

[quarantine/stakpak-agent/libs/shared/src/task_manager.rs:278–303](../../../quarantine/stakpak-agent/libs/shared/src/task_manager.rs#L278): Task manager in-memory global HashMap and channels; no per-session task ownership field.

### Evidence e33

[quarantine/stakpak-agent/libs/shared/src/task_manager.rs:573–594](../../../quarantine/stakpak-agent/libs/shared/src/task_manager.rs#L573): Cancel removes task, sends cancel, kills local process group and aborts handle.

### Evidence e34

[quarantine/stakpak-agent/libs/shared/src/task_manager.rs:650–681](../../../quarantine/stakpak-agent/libs/shared/src/task_manager.rs#L650): Background subprocess uses null stdin/captured outputs and Unix own process group.

### Evidence e35

[quarantine/stakpak-agent/libs/shared/src/task_manager.rs:927–940](../../../quarantine/stakpak-agent/libs/shared/src/task_manager.rs#L927): Handle Drop signals shutdown that terminates owned tasks, not checkpoint-preserving pause.

### Evidence e36

[quarantine/stakpak-agent/libs/mcp/server/src/remote_tools.rs:274–311](../../../quarantine/stakpak-agent/libs/mcp/server/src/remote_tools.rs#L274): search_docs routed to AgentProvider, cap five, sanitize text.

### Evidence e37

[quarantine/stakpak-agent/libs/mcp/server/src/remote_tools.rs:319–343](../../../quarantine/stakpak-agent/libs/mcp/server/src/remote_tools.rs#L319): search_memory implementation is commented out, despite approval/README memory mentions.

### Evidence e38

[quarantine/stakpak-agent/libs/mcp/server/src/remote_tools.rs:352–406](../../../quarantine/stakpak-agent/libs/mcp/server/src/remote_tools.rs#L352): load_skill reads configured-directory local file or remote rulebook.

### Evidence e39

[quarantine/stakpak-agent/libs/agent-core/src/budget_context.rs:133–254](../../../quarantine/stakpak-agent/libs/agent-core/src/budget_context.rs#L133): Budget reducer sanitizes pairing, trims assistant/tool text on request copy, persists monotonic trim boundary metadata; user/system preserved.

### Evidence e40

[quarantine/stakpak-agent/libs/agent-core/src/compaction.rs:24–45](../../../quarantine/stakpak-agent/libs/agent-core/src/compaction.rs#L24): Passthrough compactor returns unchanged messages, equal whitespace-estimated counts and truncated false.

### Evidence e41

[quarantine/stakpak-agent/libs/server/src/checkpoint_store.rs:33–109](../../../quarantine/stakpak-agent/libs/server/src/checkpoint_store.rs#L33): Latest checkpoint atomically rename-replaced, with versioned message/metadata envelope.

### Evidence e42

[quarantine/stakpak-agent/libs/server/src/event_log.rs:35–151](../../../quarantine/stakpak-agent/libs/server/src/event_log.rs#L35): Server events kept bounded in-memory ring, per-session IDs/replay, explicit snapshot-required gap notices.

### Evidence e43

[quarantine/stakpak-agent/cli/src/commands/watch/commands/run.rs:435–503](../../../quarantine/stakpak-agent/cli/src/commands/watch/commands/run.rs#L435): Hot schedule reload preserves current state on load errors, rejects DB-path change and reconciles future jobs.

### Evidence e44

[quarantine/stakpak-agent/cli/src/commands/watch/commands/run.rs:550–583](../../../quarantine/stakpak-agent/cli/src/commands/watch/commands/run.rs#L550): Schedule singleton guard checks DB running state; DB check error proceeds anyway.

### Evidence e45

[quarantine/stakpak-agent/cli/src/commands/autopilot/mod.rs:2025–2080](../../../quarantine/stakpak-agent/cli/src/commands/autopilot/mod.rs#L2025): Operator schedule add/remove/enable/disable writes config and signals reload.

### Evidence e46

[quarantine/stakpak-agent/libs/gateway/src/router.rs:41–86](../../../quarantine/stakpak-agent/libs/gateway/src/router.rs#L41): DM/group/thread routing selects one conversation routing key; not independent model peer collaboration.

### Evidence e47

[quarantine/stakpak-agent/libs/gateway/src/dispatcher.rs:28–49](../../../quarantine/stakpak-agent/libs/gateway/src/dispatcher.rs#L28): Gateway active runs, pending input/approval and cursor maps are in-memory; crash recovery delegated to watch reconciler.

### Evidence e48

[quarantine/stakpak-agent/libs/mcp/server/src/integrations/slack.rs:82–170](../../../quarantine/stakpak-agent/libs/mcp/server/src/integrations/slack.rs#L82): Optional native Slack read channel/thread and send actions through AgentProvider.

### Evidence e49

[quarantine/stakpak-agent/libs/shared/src/hooks/mod.rs:79–122](../../../quarantine/stakpak-agent/libs/shared/src/hooks/mod.rs#L79): CLI hook lifecycle and Continue/Skip/Abort policy points; mutable HookContext is host program surface.

### Evidence e50

[quarantine/stakpak-agent/libs/server/src/session_manager.rs:236–275](../../../quarantine/stakpak-agent/libs/server/src/session_manager.rs#L236): Concurrent start test asserts exactly one success and one already-running conflict.

### Evidence e51

[quarantine/stakpak-agent/libs/server/src/event_log.rs:216–273](../../../quarantine/stakpak-agent/libs/server/src/event_log.rs#L216): Replay cursor and ring-overflow gap source oracles.

### Evidence e52

[quarantine/stakpak-agent/libs/shared/src/task_manager.rs:1331–1391](../../../quarantine/stakpak-agent/libs/shared/src/task_manager.rs#L1331): Drop test asserts manager exits; named child-kill test checks running before drop but has no post-drop liveness assertion.

### Evidence e53

[quarantine/stakpak-agent/cli/src/commands/watch/commands/run.rs:1849–1885](../../../quarantine/stakpak-agent/cli/src/commands/watch/commands/run.rs#L1849): Reload failure source oracle asserts false and unchanged registered schedule count.

### Evidence e54

[quarantine/stakpak-agent/libs/agent-core/tests/budget_context_reducer.rs:91–137](../../../quarantine/stakpak-agent/libs/agent-core/tests/budget_context_reducer.rs#L91): Trimming oracle preserves user/system and expects some assistant/tool placeholder; no provider-token correctness proof.

### Evidence e55

[quarantine/stakpak-agent/cli/src/commands/agent/run/mode_interactive.rs:1170–1218](../../../quarantine/stakpak-agent/cli/src/commands/agent/run/mode_interactive.rs#L1170): Profile switch validates new config then sends shutdown/restarts outer session, not executable refit.

