# gemini-cli

Gemini CLI implements hook-programmable requests/continuations, concurrent tool scheduling, cancellable shell/background handles and local/A2A delegation. Conversation JSONL and protocol replay provide partial originals/continuity but not complete audit or quiescent executable refit.

Role: coding agent CLI and agent protocol. Runtime: TypeScript, JavaScript.

Pinned source: [https://github.com/google-gemini/gemini-cli](https://github.com/google-gemini/gemini-cli); revision/version `c6bccb7ecbf6d8368d995455dd725ed34466faad`.

Node provider/chat loop and event-driven Scheduler; CLI has traditional and config-gated AgentSession host paths; tool/service children include PTY processes.

Owns local session JSONL, tool registries, hooks and shell/background lifecycle; consumes Gemini content generators, MCP and A2A agents. Local child tools are scoped/cloned rather than independent fabric consumers.

Inspection: Selected source excerpts trace exposed entry/loop/request/action/result/state boundaries and relevant capability mechanisms; listed tests are read as source only.

Limits of this study: No acquired code executed, dependencies installed or live providers invoked. Claims concern this pinned snapshot; untraced catalog tools and dependency internals are not inferred.

## Actions

### read_file

Surface: model tool.

Input: file_path,start_line,end_line

Result: processed text/media or typed path/read error

Lifecycle: awaited workspace filesystem-service read

Authority: model; workspace access validation and policy

Evidence: [e10](#evidence-e10), [e11](#evidence-e11), [e12](#evidence-e12).

### write_file / replace

Surface: model tools.

Input: file_path/content or replacement declaration

Result: write/diff or typed error

Lifecycle: write path lock and abort checked; replace catalog/name traced, internals untraced

Authority: model; scheduler/policy/confirmation plus workspace validation

Evidence: [e8](#evidence-e8), [e10](#evidence-e10), [e11](#evidence-e11), [e13](#evidence-e13), [e14](#evidence-e14).

### run_shell_command

Surface: model tool.

Input: command,is_background,optional wait_for_previous/additional permissions

Result: stdout/stderr/events, PGID/PID or background handle

Lifecycle: shared signal reaches shell; promotion moves existing process to service ownership; PTY abort escalates group kill

Authority: model, approval/sandbox execution options

Evidence: [e8](#evidence-e8), [e9](#evidence-e9), [e11](#evidence-e11), [e15](#evidence-e15), [e16](#evidence-e16), [e17](#evidence-e17), [e18](#evidence-e18).

### list_background_processes / read_background_output

Surface: model tools.

Input: empty list request or pid/delay/output limits

Result: session PID/status/exit list or bounded log tail

Lifecycle: polls owned background job; delay path ignores abort; log may be removed

Authority: current-session model only

Evidence: [e10](#evidence-e10), [e19](#evidence-e19), [e20](#evidence-e20).

### invoke_agent

Surface: model tool.

Input: agent_name,prompt

Result: streamed child activity + tool result

Lifecycle: local/remote/browser routing; feature-gated session mode propagates abort; local nested agents excluded

Authority: model/configured agent registry and derived confirmation bus

Evidence: [e21](#evidence-e21), [e22](#evidence-e22), [e23](#evidence-e23).

### RemoteSubagentSession.send / getSessionState / abort

Surface: agent protocol API.

Input: text content and seeded contextId/taskId

Result: streamId,event deltas,result,updated context/task handles

Lifecycle: one active send, reusable process-map A2A conversation state; updates/actions unused

Authority: harness/remote service authentication, not native peer messaging tool

Evidence: [e24](#evidence-e24), [e25](#evidence-e25), [e26](#evidence-e26), [e27](#evidence-e27).

### HookSystem.registerHook / setHookEnabled

Surface: host programming API.

Input: command/runtime action,event,matcher,sequential/source

Result: hook output,modified request/tools,stop/retry/context decisions

Lifecycle: synchronous loop boundaries; runtime timeout cooperative; installed command plumbing separately supplied

Authority: host/operator or code extension; no native model registration tool established

Evidence: [e5](#evidence-e5), [e7](#evidence-e7), [e34](#evidence-e34), [e35](#evidence-e35).

### tryCompressChat / ChatCompressionService.compress

Surface: internal/operator mechanism.

Input: history,model,force,prompt_id,abortSignal

Result: verified state snapshot + retained tail or truncation/failure status

Lifecycle: summary and verification requests; active history replaced while recorder retained

Authority: harness/operator; self-correction summary prompt is not post hoc native repair

Evidence: [e28](#evidence-e28), [e29](#evidence-e29).

### AgentSession.stream

Surface: programmable protocol API.

Input: eventId/streamId cursor

Result: agent_start through agent_end events

Lifecycle: subscribe before replay; reject invalid or not-yet-started cursor; in-memory protocol state

Authority: host protocol consumers

Evidence: [e33](#evidence-e33).

### AgentsRefreshed → setTools/updateSystemInstruction

Surface: hot configuration mechanism.

Input: refreshed registry event

Result: new active tool/system definitions

Lifecycle: immediate active-client refresh; no affected-workflow deferred fence traced

Authority: host registry/configuration

Evidence: [e36](#evidence-e36).

### /bug

Surface: operator command.

Input: description

Result: chat-history file + prefilled GitHub issue URL

Lifecycle: exports then opens browser; does not itself file issue

Authority: operator; no model grievance action or external table

Evidence: [e37](#evidence-e37).

## Capabilities

### filesystem

**I — Files** (source): Workspace-validated native reads/writes, same-path write locking, typed results and native replacement catalog.

Evidence: [e10](#evidence-e10), [e12](#evidence-e12), [e13](#evidence-e13), [e14](#evidence-e14).

### processes

**I — OS programs** (source): Shell propagates combined abort, streams output, background promotion returns PID and polls session logs; PTY cancellation escalates process-group kill.

Evidence: [e15](#evidence-e15), [e16](#evidence-e16), [e18](#evidence-e18), [e19](#evidence-e19), [e20](#evidence-e20).

### code-actions

**S — Code actions** (source): File replacement/write, Bash-like shell and configurable agent/MCP tools supply code operations; no independently shared compiler/kernel contract established.

Evidence: [e10](#evidence-e10), [e11](#evidence-e11), [e22](#evidence-e22).

### persistent-kernel

**? — Kernel** (inspection-limit): Persistent interpreter namespace not established in inspected shell/service/agent loop paths.

### standing-database

**? — Standing DB** (inspection-limit): Task tracker/tool catalog and conversation JSONL are not a shared queryable computation/database contract in inspected paths.

### workflow-programming

**I — Workflows** (source): Actual before/after agent/model/tool and tool-selection hook wiring can inject/rewrite/filter/block/retry. Runtime action API is programmable; cooperative cancellation limit matters.

Evidence: [e5](#evidence-e5), [e7](#evidence-e7), [e9](#evidence-e9), [e34](#evidence-e34), [e35](#evidence-e35).

### multi-model

**I — Models** (source): Per-request model config/hook model rewrite and definition-driven local/remote agents expose real model routing, without implying shared-peer dialogue.

Evidence: [e6](#evidence-e6), [e7](#evidence-e7), [e21](#evidence-e21), [e22](#evidence-e22).

### live-collaboration

**L — Peer chat** (source): Delegated local/A2A work has streamed observability and reusable context/task handles, but remote protocol rejects active-stream send and ignores update/action/elicitations; local child excludes nested agent tools.

Evidence: [e23](#evidence-e23), [e24](#evidence-e24), [e25](#evidence-e25), [e27](#evidence-e27).

### concurrent-work

**I — Concurrency** (source): Scheduler runs contiguous parallelizable tool groups; edit/write/topic serialize and wait_for_previous gives model ordering control. Child registry isolates cloned tool state.

Evidence: [e8](#evidence-e8), [e23](#evidence-e23).

### steering-interrupt

**I — Steer/interrupt** (source): Signal crosses provider/turn/scheduler/native shell and remote session; tool events carry cancellation. Remote active-message steering is limited; delayed background-log read ignores signal.

Evidence: [e3](#evidence-e3), [e6](#evidence-e6), [e9](#evidence-e9), [e18](#evidence-e18), [e20](#evidence-e20), [e24](#evidence-e24), [e26](#evidence-e26).

### turn-redefinition

**S — Turn program** (source): Hooks rewrite request model/config/contents/tools and AfterAgent controls halt/retry/context clear with bounded recursive continuation. Turn orchestration remains fixed host implementation.

Evidence: [e5](#evidence-e5), [e7](#evidence-e7), [e34](#evidence-e34).

### compaction

**I — Compaction** (source): Separate summary plus model verification requests use original inputs when fitting; new active history preserves recording object/file and distinguishes content truncation.

Evidence: [e28](#evidence-e28), [e29](#evidence-e29).

### context-repair

**L — Repair** (source): Compaction verifier can correct its own summary before activation and originals remain in recorder path; no native model original-fetch/restore protocol established. Model critique is not independent correctness oracle.

Evidence: [e28](#evidence-e28), [e29](#evidence-e29), [e30](#evidence-e30), [e31](#evidence-e31).

### original-audit

**L — Original audit** (source): Append-only same-ID conversation versions and retained recorder across compression preserve selected conversation originals. ENOSPC disables writes, foreground temp artifacts are cleaned, and full transport/effect audit is not established.

Evidence: [e17](#evidence-e17), [e29](#evidence-e29), [e30](#evidence-e30), [e31](#evidence-e31), [e32](#evidence-e32).

### audit-query

**S — Audit query** (source): Conversation JSONL materialization plus event-cursor replay and background log polling are usable primitives. In-memory protocol events are not a complete durable global audit/query API.

Evidence: [e20](#evidence-e20), [e30](#evidence-e30), [e31](#evidence-e31), [e33](#evidence-e33).

### hot-change

**L — Hot change** (source): Runtime hook registration/toggle and agent refresh update active client definitions immediately. No deferred affected-workflow activation or compilation handoff traced.

Evidence: [e34](#evidence-e34), [e36](#evidence-e36).

### rebuild-continuity

**L — Rebuild continuity** (source): Protocol event replay and reusable remote IDs support reconnect composition but rely on object/static state; do not establish quiescent executable refit or uncertain-effect continuity.

Evidence: [e27](#evidence-e27), [e33](#evidence-e33).

### remote-services

**I — Remote** (source): Consumes Gemini content generator, configured MCP catalog and authenticated A2A remote agents; shell/service state still owned by local harness.

Evidence: [e7](#evidence-e7), [e10](#evidence-e10), [e22](#evidence-e22), [e25](#evidence-e25), [e26](#evidence-e26).

### self-improvement

**? — Self-improve** (inspection-limit): Programmable hooks and file/process actions do not demonstrate model-governed measured improvement or self-recompile/reinhabit protocol.

### complaints

**L — Complaints** (source): Operator /bug packages active chat plus issue URL/browser opening; no native model complaint carrying full agent state into independently tracked table.

Evidence: [e37](#evidence-e37).

### authority

**I — Authority** (source): Scheduler confirmation context, derived child bus/cloned registry, workspace path validation and shell sandbox options are traced. Operator-free configurations are possible only within configured policy; policy internals not exhaustively audited.

Evidence: [e9](#evidence-e9), [e12](#evidence-e12), [e15](#evidence-e15), [e23](#evidence-e23).

### evaluation

**S — Evaluation** (source): Read source tests cover runtime hook registration/input, hook-stop nonexecution and Unicode-safe history collapse. Mocks/pure transforms do not prove live summary adequacy or OS cancellation convergence; not executed.

Evidence: [e38](#evidence-e38), [e39](#evidence-e39), [e40](#evidence-e40).

### time-order

**L — Time/order** (source): UUID conversation IDs/timestamps, call IDs and protocol cursor/stream lifecycle support ordering; no cross-service durable causal clock or exactly-once effect log established.

Evidence: [e3](#evidence-e3), [e31](#evidence-e31), [e33](#evidence-e33).

## Inspected test oracles

- [quarantine/gemini-cli/packages/core/src/hooks/runtimeHooks.test.ts](../../../quarantine/gemini-cli/packages/core/src/hooks/runtimeHooks.test.ts): Runtime registration and event action input Oracle: Uses mocked action and asserts registry source/input/output; no uncooperative timeout or full turn integration oracle. Read, **not executed**.
- [quarantine/gemini-cli/packages/core/src/scheduler/scheduler_hooks.test.ts](../../../quarantine/gemini-cli/packages/core/src/scheduler/scheduler_hooks.test.ts): Stopped-hook prevents operation Oracle: Asserts error and mocked executor never invoked; not OS side-effect audit. Read, **not executed**.
- [quarantine/gemini-cli/packages/core/src/context/chatCompressionService.test.ts](../../../quarantine/gemini-cli/packages/core/src/context/chatCompressionService.test.ts): Unicode-safe old-tool-output collapse Oracle: Pure fabricated history asserts no replacement/unpaired characters and UTF-8 roundtrip; not semantic state-snapshot quality. Read, **not executed**.

## Useful mechanisms

- Hooks alter actual request/continuation lifecycle rather than only appending prompts.
- Model wait_for_previous plus serial edit/topic classes make tool-order authority explicit.
- Append-only message versions and recorder retained through compaction are stronger than overwriting a transcript.
- A2A context/task handles persist across same-process delegated invocations and aborts; actual remote consumer protocol.

## Material limits

- Traditional CLI and feature-gated AgentSession scopes remain distinct; source replay does not establish default host use everywhere.
- Remote protocol is sequential delegation, rejects sends while active and ignores update/action/elicitations; local nested delegation excluded.
- Self-verifying summaries have no independent semantic-loss oracle.
- Audit is selected conversation data; ENOSPC disables persistence, temp/output cleanup and external effects bound originals.
- Runtime hook timeout only requests cooperative abort; active definitions can refresh immediately; no refit quiescence/standing-pause contract.

## Arconaut design questions

- How can Arconaut expose programmable request/settlement boundaries with model authority while preserving scoped services and explicit activation versions?
- Retain immutable provider/tool/context originals and attach projection versions; let models inspect and repair without relying only on summary self-critique.
- Generalize remote context/task IDs into an outpost consumer protocol with live messaging and durable checkpoint—not a process-static map.
- What verified quiescence barrier distinguishes cancellable requests from arbitrary hooks or standing/background owned computation?

## Evidence

### Evidence e1

[quarantine/gemini-cli/packages/cli/src/nonInteractiveCli.ts:72–85](../../../quarantine/gemini-cli/packages/cli/src/nonInteractiveCli.ts#L72): Noninteractive host chooses ADK/AgentSession branch via configuration; traditional loop remains separately wired.

### Evidence e2

[quarantine/gemini-cli/packages/cli/src/nonInteractiveCli.ts:312–338](../../../quarantine/gemini-cli/packages/cli/src/nonInteractiveCli.ts#L312): Traditional CLI sends current parts through GeminiClient with common abort signal and bounded turn counter.

### Evidence e3

[quarantine/gemini-cli/packages/cli/src/nonInteractiveCli.ts:482–510](../../../quarantine/gemini-cli/packages/cli/src/nonInteractiveCli.ts#L482): Native calls schedule with same abort signal; structured streaming results carry timestamp/call ID/status.

### Evidence e4

[quarantine/gemini-cli/packages/core/src/agent/legacy-agent-session.ts:174–305](../../../quarantine/gemini-cli/packages/core/src/agent/legacy-agent-session.ts#L174): Session loop sends provider work, collects calls, handles errors/abort, nudges empty post-tool answers, schedules operations and emits paired results.

### Evidence e5

[quarantine/gemini-cli/packages/core/src/core/client.ts:924–1047](../../../quarantine/gemini-cli/packages/core/src/core/client.ts#L924): BeforeAgent injects/blocks/stops; AfterAgent can clear context, halt or recursively request bounded continuation with hook state reset.

### Evidence e6

[quarantine/gemini-cli/packages/core/src/core/turn.ts:269–319](../../../quarantine/gemini-cli/packages/core/src/core/turn.ts#L269): Turn calls GeminiChat with model config, request/history override and signal; transforms retries/cancellation/hook responses into events.

### Evidence e7

[quarantine/gemini-cli/packages/core/src/core/geminiChat.ts:992–1088](../../../quarantine/gemini-cli/packages/core/src/core/geminiChat.ts#L992): BeforeModel modifies model/config/contents, can synthesize blocked response, and BeforeToolSelection modifies tools before actual content generator stream.

### Evidence e8

[quarantine/gemini-cli/packages/core/src/scheduler/scheduler.ts:456–594](../../../quarantine/gemini-cli/packages/core/src/scheduler/scheduler.ts#L456): Contiguous parallelizable requests validate and execute together; edit/write/topic serialize and wait_for_previous overrides; terminal calls finalize.

### Evidence e9

[quarantine/gemini-cli/packages/core/src/scheduler/tool-executor.ts:63–155](../../../quarantine/gemini-cli/packages/core/src/scheduler/tool-executor.ts#L63): Executor passes signal, shell config, PID callback and live output through hook-wrapped tool invocation; abort results become cancellation.

### Evidence e10

[quarantine/gemini-cli/packages/core/src/config/config.ts:4009–4123](../../../quarantine/gemini-cli/packages/core/src/config/config.ts#L4009): Config registers native file/search/shell/background/web/MCP/skill/plan/task/agent tools with configuration-dependent availability.

### Evidence e11

[quarantine/gemini-cli/packages/core/src/tools/definitions/base-declarations.ts:51–65](../../../quarantine/gemini-cli/packages/core/src/tools/definitions/base-declarations.ts#L51): Native file/process names are read_file, run_shell_command, write_file and replace.

### Evidence e12

[quarantine/gemini-cli/packages/core/src/tools/read-file.ts:123–169](../../../quarantine/gemini-cli/packages/core/src/tools/read-file.ts#L123): Read validates real workspace path and returns processed file content/range or typed error.

### Evidence e13

[quarantine/gemini-cli/packages/core/src/tools/write-file.ts:371–428](../../../quarantine/gemini-cli/packages/core/src/tools/write-file.ts#L371): Write validates path and serializes same-path writers with cancellable lock before apply.

### Evidence e14

[quarantine/gemini-cli/packages/core/src/tools/write-file.ts:459–494](../../../quarantine/gemini-cli/packages/core/src/tools/write-file.ts#L459): Write creates directories, normalizes line endings, writes through filesystem service and produces diff.

### Evidence e15

[quarantine/gemini-cli/packages/core/src/tools/shell.ts:681–735](../../../quarantine/gemini-cli/packages/core/src/tools/shell.ts#L681): Shell streams text/binary events, uses combined controller and sandbox/session execution options.

### Evidence e16

[quarantine/gemini-cli/packages/core/src/tools/shell.ts:764–825](../../../quarantine/gemini-cli/packages/core/src/tools/shell.ts#L764): Background timer promotes existing PID and returns initial output after race; foreground awaits result.

### Evidence e17

[quarantine/gemini-cli/packages/core/src/tools/shell.ts:1158–1182](../../../quarantine/gemini-cli/packages/core/src/tools/shell.ts#L1158): Background ownership transfers temp directory to execution service; foreground finally removes temp artifacts and abort listeners.

### Evidence e18

[quarantine/gemini-cli/packages/core/src/services/shellExecutionService.ts:1700–1714](../../../quarantine/gemini-cli/packages/core/src/services/shellExecutionService.ts#L1700): PTY abort invokes escalating process-group kill and returns process identity/result handle.

### Evidence e19

[quarantine/gemini-cli/packages/core/src/tools/shellBackgroundTools.ts:45–89](../../../quarantine/gemini-cli/packages/core/src/tools/shellBackgroundTools.ts#L45): list_background_processes enumerates current-session command PID/status/exit metadata.

### Evidence e20

[quarantine/gemini-cli/packages/core/src/tools/shellBackgroundTools.ts:133–185](../../../quarantine/gemini-cli/packages/core/src/tools/shellBackgroundTools.ts#L133): read_background_output checks PID session ownership and reads bounded log tail with O_NOFOLLOW; delay ignores abort in this path.

### Evidence e21

[quarantine/gemini-cli/packages/core/src/agents/agent-tool.ts:43–108](../../../quarantine/gemini-cli/packages/core/src/agents/agent-tool.ts#L43): invoke_agent schema resolves agent_name and comprehensive prompt into registry-specific inputs.

### Evidence e22

[quarantine/gemini-cli/packages/core/src/agents/agent-tool.ts:156–238](../../../quarantine/gemini-cli/packages/core/src/agents/agent-tool.ts#L156): Delegate selects browser/local/remote and feature-gated session invocation, propagates signal and live output through actual invocation.

### Evidence e23

[quarantine/gemini-cli/packages/core/src/agents/local-executor.ts:174–224](../../../quarantine/gemini-cli/packages/core/src/agents/local-executor.ts#L174): Local child clones tools into isolated registry and derived confirmation bus; nested agent tools and update_topic are excluded.

### Evidence e24

[quarantine/gemini-cli/packages/core/src/agents/remote-subagent-protocol.ts:94–146](../../../quarantine/gemini-cli/packages/core/src/agents/remote-subagent-protocol.ts#L94): Remote protocol exposes events and context/task state; rejects message send while stream active and ignores update/action/elicitations.

### Evidence e25

[quarantine/gemini-cli/packages/core/src/agents/remote-subagent-protocol.ts:211–265](../../../quarantine/gemini-cli/packages/core/src/agents/remote-subagent-protocol.ts#L211): A2A stream consumes context/task IDs, updates them from responses and emits streamed deltas/progress.

### Evidence e26

[quarantine/gemini-cli/packages/core/src/agents/remote-session-invocation.ts:117–175](../../../quarantine/gemini-cli/packages/core/src/agents/remote-session-invocation.ts#L117): Session invocation seeds prior A2A state, wires parent abort, observes progress and detects empty-result abort.

### Evidence e27

[quarantine/gemini-cli/packages/core/src/agents/remote-session-invocation.ts:242–250](../../../quarantine/gemini-cli/packages/core/src/agents/remote-session-invocation.ts#L242): A2A context/task state is retained in process-static invocation map even on abort/error, not a durable executable handoff.

### Evidence e28

[quarantine/gemini-cli/packages/core/src/context/chatCompressionService.ts:576–654](../../../quarantine/gemini-cli/packages/core/src/context/chatCompressionService.ts#L576): Compression prefers original history when it fits and makes separate initial summary plus model verification/self-correction requests.

### Evidence e29

[quarantine/gemini-cli/packages/core/src/core/client.ts:1220–1265](../../../quarantine/gemini-cli/packages/core/src/core/client.ts#L1220): Compression creates a new active chat while carrying recording conversation/file; truncation updates current history without resetting failure flag.

### Evidence e30

[quarantine/gemini-cli/packages/core/src/services/chatRecordingService.ts:563–633](../../../quarantine/gemini-cli/packages/core/src/services/chatRecordingService.ts#L563): Normal record append is JSONL; ENOSPC disables persistence and unreadable-file recovery attempts backup then atomic rewrite.

### Evidence e31

[quarantine/gemini-cli/packages/core/src/services/chatRecordingService.ts:650–714](../../../quarantine/gemini-cli/packages/core/src/services/chatRecordingService.ts#L650): Full same-ID updates append to log but latest overwrite in-memory materialization; messages get durable UUID/time and selected thoughts/tokens.

### Evidence e32

[quarantine/gemini-cli/packages/core/src/services/chatRecordingService.ts:725–808](../../../quarantine/gemini-cli/packages/core/src/services/chatRecordingService.ts#L725): Synthetic messages, thought summaries/tokens and enriched tool calls are recorded; not all provider transport/OS effects.

### Evidence e33

[quarantine/gemini-cli/packages/core/src/agent/agent-session.ts:123–223](../../../quarantine/gemini-cli/packages/core/src/agent/agent-session.ts#L123): Wrapper replays event cursor/stream boundaries and listens for new events; unknown/pre-agent-start resume cursors reject. Protocol event history is object state.

### Evidence e34

[quarantine/gemini-cli/packages/core/src/hooks/hookSystem.ts:175–216](../../../quarantine/gemini-cli/packages/core/src/hooks/hookSystem.ts#L175): Host hook API supports enable/disable and runtime registration by event/matcher/sequencing.

### Evidence e35

[quarantine/gemini-cli/packages/core/src/hooks/hookRunner.ts:252–301](../../../quarantine/gemini-cli/packages/core/src/hooks/hookRunner.ts#L252): Runtime hook action runs against timeout race and cooperative abort controller; timeout/error requests abort, not forcibly terminating JS action.

### Evidence e36

[quarantine/gemini-cli/packages/core/src/config/config.ts:4222–4232](../../../quarantine/gemini-cli/packages/core/src/config/config.ts#L4222): Agent refresh event immediately updates active client tools/system if initialized.

### Evidence e37

[quarantine/gemini-cli/packages/cli/src/ui/commands/bugCommand.ts:80–125](../../../quarantine/gemini-cli/packages/cli/src/ui/commands/bugCommand.ts#L80): Operator /bug exports active chat history and opens prefilled GitHub URL; no native model complaint/database submission traced.

### Evidence e38

[quarantine/gemini-cli/packages/core/src/hooks/runtimeHooks.test.ts:47–96](../../../quarantine/gemini-cli/packages/core/src/hooks/runtimeHooks.test.ts#L47): Runtime hook test asserts registration/callback input/finalOutput using mock action.

### Evidence e39

[quarantine/gemini-cli/packages/core/src/scheduler/scheduler_hooks.test.ts:143–159](../../../quarantine/gemini-cli/packages/core/src/scheduler/scheduler_hooks.test.ts#L143): Hook stop test asserts error and nonexecution of mocked tool.

### Evidence e40

[quarantine/gemini-cli/packages/core/src/context/chatCompressionService.test.ts:946–997](../../../quarantine/gemini-cli/packages/core/src/context/chatCompressionService.test.ts#L946): Pure collapse test checks emoji/surrogate/UTF-8 integrity and recent output preservation; not semantic adequacy of model summary.

