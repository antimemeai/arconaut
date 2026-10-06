# continue

Pinned final/read-only Continue includes CLI and editor agents with provider/tool loops, MCP, background shells, configurable policies and beta delegated subagents; traced child/global state and destructive CLI compaction limit isolation and original audit.

Role: coding agent and editor client. Runtime: TypeScript, JavaScript.

Pinned source: [https://github.com/continuedev/continue](https://github.com/continuedev/continue); revision/version `5522c6f44ca0ac3528b37244818fbfa39b5af470`.

Node CLI with Ink/React UI and service container; separate editor GUI/core messaging path. Async provider loop launches approved tool promises in parallel.

Harness owns session files, globals and shell/background jobs; consumes configured providers, MCP/HTTP and editor host capabilities. No independent shared kernel fabric traced.

Inspection: Selected source excerpts trace exposed entry/loop/request/action/result/state boundaries and relevant capability mechanisms; listed tests are read as source only.

Limits of this study: No acquired code executed, dependencies installed or live providers invoked. Claims concern this pinned snapshot; untraced catalog tools and dependency internals are not inferred.

## Actions

### Read

Surface: model tool.

Input: filepath

Result: UTF-8 text or size/security/error

Lifecycle: awaited; records read marker

Authority: model, exposed-policy check

Evidence: [e7](#evidence-e7), [e9](#evidence-e9).

### Write / Edit / MultiEdit

Surface: model tools.

Input: filepath/content or edit schema; model capability chooses editing tool

Result: file mutation/diff or errors

Lifecycle: preprocess preview then approved operation; Write traced, editing internals not traced

Authority: model under configured policy

Evidence: [e7](#evidence-e7), [e10](#evidence-e10), [e11](#evidence-e11).

### Bash

Surface: model tool.

Input: command, optional timeout

Result: truncated stdout/stderr, error or background Job ID

Lifecycle: spawns owned shell; silence timeout resets on output; background signal transfers ownership

Authority: TUI ask/headless allow subject security evaluator

Evidence: [e12](#evidence-e12), [e13](#evidence-e13), [e14](#evidence-e14), [e30](#evidence-e30).

### CheckBackgroundJob

Surface: model tool.

Input: job_id

Result: JSON status, output, timestamps, exit/error

Lifecycle: polls existing job; not independently resumable execution

Authority: model; base registry and default allow

Evidence: [e7](#evidence-e7), [e15](#evidence-e15), [e30](#evidence-e30).

### Subagent

Surface: beta model tool.

Input: description,prompt,subagent_name

Result: streaming output + completed/failed task metadata

Lifecycle: awaits same loop with selected model; fresh child controller; restores globals finally

Authority: model when beta enabled; executor temporarily globally allows tools

Evidence: [e7](#evidence-e7), [e16](#evidence-e16), [e17](#evidence-e17), [e18](#evidence-e18), [e19](#evidence-e19).

### MCP adapted tool.run

Surface: extension model tools.

Input: server-advertised schema and args

Result: JSON MCP content or error

Lifecycle: awaited through consumed MCP service

Authority: policy-filtered model; service/container own connection

Evidence: [e8](#evidence-e8).

### compactChatHistory / automatic compaction

Surface: internal/operator mechanism.

Input: history,model/API,system tokens

Result: summary replacement history and index

Lifecycle: separate model request; successful auto path persists replacement

Authority: harness/operator; not native model original-restoration tool

Evidence: [e20](#evidence-e20), [e21](#evidence-e21).

### enqueueMessage / handleInterrupt / serve /message

Surface: operator/API.

Input: prompt/images or empty interrupt request

Result: queued next turn or controller abort

Lifecycle: FIFO after response; serve owns response controller; does not prove child shell cancellation

Authority: operator/remote client

Evidence: [e24](#evidence-e24), [e25](#evidence-e25), [e26](#evidence-e26), [e27](#evidence-e27).

### ConfigService.reload / updateConfigPath

Surface: programmable service API.

Input: configuration path and initialization state

Result: loaded config and dependent model/MCP reload

Lifecycle: no affected-workflow activation fence traced; active loop still uses passed model/API

Authority: host configuration service

Evidence: [e2](#evidence-e2), [e3](#evidence-e3), [e28](#evidence-e28).

### ReportFailure

Surface: conditional model tool.

Input: errorMessage, CLI --id

Result: Failure reported acknowledgement or API/auth error

Lifecycle: posts FAILED and marks task complete

Authority: model, allow default; remote agent authorization required

Evidence: [e29](#evidence-e29), [e30](#evidence-e30).

### legacy slash command.run

Surface: editor workflow API.

Input: command input,history,LLM,IDE,fetch,abort context

Result: async generated assistant chunks/prompt log

Lifecycle: core generator invoked instead of normal model stream when legacy metadata present

Authority: configured command author/operator; not arbitrary model turn redefinition

Evidence: [e31](#evidence-e31), [e34](#evidence-e34).

### tools/call / callClientTool

Surface: editor operation bridge.

Input: selected tool function arguments/ID

Result: ContextItems/error/MCP UI state

Lifecycle: client/core dispatch; results recorded before streamResponseAfterToolCall

Authority: editor policies approve; host supplies IDE/file/process authority

Evidence: [e32](#evidence-e32), [e33](#evidence-e33), [e35](#evidence-e35).

## Capabilities

### filesystem

**I — Files** (source): CLI Read/Write actual filesystem operations, request-gated editing catalog, plus separate editor tool bridge. Output/read limits and security checks apply.

Evidence: [e7](#evidence-e7), [e9](#evidence-e9), [e11](#evidence-e11), [e33](#evidence-e33).

### processes

**I — OS programs** (source): Bash owns child shells, has no-output timeout and foreground-to-background job transfer with status polling. Tool dispatcher passes no provider abort signal to Bash.

Evidence: [e8](#evidence-e8), [e12](#evidence-e12), [e13](#evidence-e13), [e14](#evidence-e14), [e15](#evidence-e15).

### code-actions

**L — Code actions** (source): Native CLI file editing and Bash are code-action substrate; separate editor bridge supplies host tools. No language kernel or bounded structural-code oracle established by traced operations.

Evidence: [e7](#evidence-e7), [e11](#evidence-e11), [e33](#evidence-e33).

### persistent-kernel

**? — Kernel** (inspection-limit): No persistent interpreter/kernel established in inspected CLI loop, shell, child executor or editor bridge.

### standing-database

**? — Standing DB** (inspection-limit): Inspected service container/history/job state is not an independently shared database execution/query contract.

### workflow-programming

**S — Workflows** (source): Configured MCP functions and editor legacy slash async generators provide programmable composition. CLI beta child agents reuse its fixed streaming loop.

Evidence: [e8](#evidence-e8), [e18](#evidence-e18), [e34](#evidence-e34), [e35](#evidence-e35).

### multi-model

**I — Models** (source): Child configuration selects its own model/API for Subagent and core editor selects role chat model. Distinguish from live peer collaboration.

Evidence: [e17](#evidence-e17), [e19](#evidence-e19), [e34](#evidence-e34).

### live-collaboration

**L — Peer chat** (source): Beta delegated child work streams results to one parent; no peer mailbox/IRC bus traced, and executor mutates global state instead of isolating each participant.

Evidence: [e17](#evidence-e17), [e18](#evidence-e18), [e19](#evidence-e19).

### concurrent-work

**L — Concurrency** (source): Approved CLI tool promises start during subsequent sequential permission checks, then join in original output order. Parallel Subagent calls can overlap global overrides; inspected unit tests mock executor and do not validate isolation.

Evidence: [e5](#evidence-e5), [e6](#evidence-e6), [e18](#evidence-e18), [e37](#evidence-e37).

### steering-interrupt

**L — Steer/interrupt** (source): Operator queue runs after response and local/remote abort controls provider/compaction. Fresh child controller and Bash context have no parent signal propagation in inspected dispatch, so provider interruption does not establish quiescence.

Evidence: [e8](#evidence-e8), [e17](#evidence-e17), [e19](#evidence-e19), [e25](#evidence-e25), [e26](#evidence-e26), [e27](#evidence-e27).

### turn-redefinition

**S — Turn program** (source): Loop recomputes tools/system per iteration; editor legacy command generator replaces normal stream. No model-owned custom settlement/request scheduler or arbitrary hot turn program traced.

Evidence: [e2](#evidence-e2), [e34](#evidence-e34).

### compaction

**I — Compaction** (source): Automatic and explicit summary creation replaces history; continuation can add continue after compaction. This is one summary mechanism, not interchangeable managed projections.

Evidence: [e2](#evidence-e2), [e20](#evidence-e20), [e21](#evidence-e21), [e36](#evidence-e36).

### context-repair

**L — Repair** (source): Errors retain pre-compaction history in memory; successful replacement overwrites session JSON. No model tool for original fetch/append-only restore identified in these paths.

Evidence: [e20](#evidence-e20), [e21](#evidence-e21), [e23](#evidence-e23).

### original-audit

**L — Original audit** (source): Session writes current history omitting system messages; compaction replaces history and shell outputs truncate. Tool timing and editor prompt devdata are partial records, not original event/transport/effect retention.

Evidence: [e8](#evidence-e8), [e14](#evidence-e14), [e21](#evidence-e21), [e22](#evidence-e22), [e23](#evidence-e23), [e31](#evidence-e31).

### audit-query

**S — Audit query** (source): Session JSON/history and background status are inspectable records; no cross-event immutable audit query API established.

Evidence: [e15](#evidence-e15), [e23](#evidence-e23).

### hot-change

**L — Hot change** (source): Config service reloads model/MCP and each loop step refreshes tools/system; active request loop retains its passed model/API. No deferred affected-workflow fence or atomic cross-service activation traced.

Evidence: [e2](#evidence-e2), [e3](#evidence-e3), [e28](#evidence-e28).

### rebuild-continuity

**? — Rebuild continuity** (inspection-limit): No outpost handoff, quiescent executable rebuild or preserved uncertain-action identity in inspected session/config lifecycle.

### remote-services

**I — Remote** (source): Consumes provider APIs and MCP; serve mode queues remote chat with per-response interruption. Server owns agent state and shell jobs, rather than an independent shared compute fabric.

Evidence: [e3](#evidence-e3), [e8](#evidence-e8), [e27](#evidence-e27), [e35](#evidence-e35).

### self-improvement

**? — Self-improve** (inspection-limit): Model file/Bash tools can edit programs, but no model-governed trial/measurement/adoption loop or self-refit safety protocol established.

### complaints

**L — Complaints** (source): ReportFailure is exposed only with agent --id and remote auth/API; it marks task FAILED/complete and sends error text. It is not a nonfatal agent-state grievance table.

Evidence: [e29](#evidence-e29), [e30](#evidence-e30).

### authority

**L — Authority** (source): Configurable ask/allow/exclude with headless wildcard allow; plan allows Bash. Beta subagent globally overwrites permissions to allow all; parallel tool launch makes that boundary materially weak for isolation.

Evidence: [e5](#evidence-e5), [e18](#evidence-e18), [e30](#evidence-e30).

### evaluation

**S — Evaluation** (source): Read tests assert mocked continuation/child formatting and simple real-shell outputs when executed; none establish child global isolation, compaction semantics or complete audit. Tests not run.

Evidence: [e36](#evidence-e36), [e37](#evidence-e37), [e38](#evidence-e38).

### time-order

**L — Time/order** (source): Queue timestamps, per-dispatch Date.now duration, and original-index result ordering coexist with parallel completion and in-memory state; no durable global causal clock traced.

Evidence: [e6](#evidence-e6), [e8](#evidence-e8), [e24](#evidence-e24).

## Inspected test oracles

- [quarantine/continue/extensions/cli/src/stream/streamChatResponse.autoContinuation.test.ts](../../../quarantine/continue/extensions/cli/src/stream/streamChatResponse.autoContinuation.test.ts): Automatic continuation after mocked compaction Oracle: Asserts continue insertion and provider call count using fake provider and compaction; does not verify semantic summaries/original retention. Read, **not executed**.
- [quarantine/continue/extensions/cli/src/tools/subagent.test.ts](../../../quarantine/continue/extensions/cli/src/tools/subagent.test.ts): Child adapter schema and output plumbing Oracle: Mocks executor/services; asserts prompt,parent ID, callback and task formatting, not executor concurrency/permission isolation. Read, **not executed**.
- [quarantine/continue/extensions/cli/src/tools/runTerminalCommand.test.ts](../../../quarantine/continue/extensions/cli/src/tools/runTerminalCommand.test.ts): Basic platform shell execution Oracle: When run would assert echo/cwd; inspected tests do not exercise cancellation/background ownership. Not executed. Read, **not executed**.

## Useful mechanisms

- Actual CheckBackgroundJob handle and transfer of existing shell process avoid launching a replacement job.
- Request-boundary system/tools refresh and programmable editor legacy async commands are concrete extension mechanisms.
- Structured tool result IDs and stable final ordering preserve provider-call pairing under concurrent completion.

## Material limits

- Pinned README states final 2.0 release and read-only repository; conclusions describe this snapshot.
- Beta child executor temporarily overwrites global permission/system/history behavior while generic dispatcher permits concurrent tool execution; isolation is unproven and structurally fragile.
- Successful CLI compaction saves replacement history; system messages and untruncated shell/provider originals are not durably retained by these paths.
- Provider abort is not evidence of Bash/child quiescence; global services can reload while active loop holds older model/API.
- ReportFailure closes task status and is remote-auth dependent, unlike nonfatal state-bearing rageshake.

## Arconaut design questions

- Make each arconaut participant a scoped consumer of provider/context/authority services; which exact state must never be a mutable process-global override?
- Use immutable originals plus separately versioned projections so models can repair compaction through an ordinary authorized API.
- What ownership handles and acknowledgements prove every harness-owned active shell/provider is quiescent before arcorefit?
- Separate nonfatal LLMmmshake filing from task failure/completion and record complaint state in an external database.

## Evidence

### Evidence e1

[quarantine/continue/README.md:17–31](../../../quarantine/continue/README.md#L17): Pinned README describes final 2.0 release and repository read-only maintenance status; CLI and editor hosts remain distinct source paths.

### Evidence e2

[quarantine/continue/extensions/cli/src/stream/streamChatResponse.ts:443–583](../../../quarantine/continue/extensions/cli/src/stream/streamChatResponse.ts#L443): CLI loop refreshes history/system/tools per iteration, compacts around request/tool execution, observes provider abort and can insert continue after compaction.

### Evidence e3

[quarantine/continue/extensions/cli/src/stream/streamChatResponse.ts:259–285](../../../quarantine/continue/extensions/cli/src/stream/streamChatResponse.ts#L259): Provider request uses passed model/API, assembled OpenAI history, tools, completion options and retry abort signal.

### Evidence e4

[quarantine/continue/extensions/cli/src/stream/handleToolCalls.ts:94–163](../../../quarantine/continue/extensions/cli/src/stream/handleToolCalls.ts#L94): Assistant call IDs persist before preprocessing and execution; failed preprocessing and rejected headless work have explicit result/state handling.

### Evidence e5

[quarantine/continue/extensions/cli/src/stream/streamChatResponse.helpers.ts:469–638](../../../quarantine/continue/extensions/cli/src/stream/streamChatResponse.helpers.ts#L469): Each permission check is sequential, but approved tools immediately launch into promises; Promise.all joins execution, so later permission checks can overlap running tools.

### Evidence e6

[quarantine/continue/extensions/cli/src/stream/streamChatResponse.helpers.ts:638–648](../../../quarantine/continue/extensions/cli/src/stream/streamChatResponse.helpers.ts#L638): Final tool results are assembled in original call order, distinct from completion/event ordering.

### Evidence e7

[quarantine/continue/extensions/cli/src/tools/index.tsx:64–149](../../../quarantine/continue/extensions/cli/src/tools/index.tsx#L64): Actual dynamic tool registry includes CheckBackgroundJob, model-capability editing choice, optional beta Subagent and MCP; several allBuiltIns catalog entries are not automatically request tools.

### Evidence e8

[quarantine/continue/extensions/cli/src/tools/index.tsx:192–275](../../../quarantine/continue/extensions/cli/src/tools/index.tsx#L192): MCP tool schemas adapt to Continue tool.run; dispatcher passes tool IDs/count, records timing/telemetry and returns or propagates results/errors.

### Evidence e9

[quarantine/continue/extensions/cli/src/tools/readFile.ts:35–124](../../../quarantine/continue/extensions/cli/src/tools/readFile.ts#L35): Read preprocess checks sensitive file concerns; actual UTF-8 read resolves path, divides output limits for parallel calls and marks read file.

### Evidence e10

[quarantine/continue/extensions/cli/src/tools/writeFile.ts:31–86](../../../quarantine/continue/extensions/cli/src/tools/writeFile.ts#L31): Write validates path/content and previews existing-file diff.

### Evidence e11

[quarantine/continue/extensions/cli/src/tools/writeFile.ts:120–158](../../../quarantine/continue/extensions/cli/src/tools/writeFile.ts#L120): Write creates parent directories and overwrites file, returning a diff for existing content.

### Evidence e12

[quarantine/continue/extensions/cli/src/tools/runTerminalCommand.ts:118–213](../../../quarantine/continue/extensions/cli/src/tools/runTerminalCommand.ts#L118): Bash validates command/timeout, evaluates command-security policy and spawns platform shell with per-batch output limits.

### Evidence e13

[quarantine/continue/extensions/cli/src/tools/runTerminalCommand.ts:225–285](../../../quarantine/continue/extensions/cli/src/tools/runTerminalCommand.ts#L225): Foreground shell can transfer the same child to background job ownership and return job ID; silence timer kills child after no-output timeout.

### Evidence e14

[quarantine/continue/extensions/cli/src/tools/runTerminalCommand.ts:302–377](../../../quarantine/continue/extensions/cli/src/tools/runTerminalCommand.ts#L302): Output resets timeout and streams history; close/error clears timer, output is truncated, and nonzero exit rejects only when stderr exists.

### Evidence e15

[quarantine/continue/extensions/cli/src/tools/checkBackgroundJob.ts:1–67](../../../quarantine/continue/extensions/cli/src/tools/checkBackgroundJob.ts#L1): CheckBackgroundJob returns JSON identity/status/output/timing/exit/error; missing ID reports available jobs.

### Evidence e16

[quarantine/continue/extensions/cli/src/subagent/index.ts:3–27](../../../quarantine/continue/extensions/cli/src/subagent/index.ts#L3): Subagent native schema takes description, prompt and subagent_name; metadata placeholder is replaced by actual dynamic implementation.

### Evidence e17

[quarantine/continue/extensions/cli/src/tools/subagent.ts:23–112](../../../quarantine/continue/extensions/cli/src/tools/subagent.ts#L23): Beta tool resolves configured agent/model, requires parent session, creates new AbortController, streams child output and returns completion metadata.

### Evidence e18

[quarantine/continue/extensions/cli/src/subagent/executor.ts:57–122](../../../quarantine/continue/extensions/cli/src/subagent/executor.ts#L57): Child executor temporarily replaces global permission service with wildcard allow and global system/history methods, using a local initial prompt.

### Evidence e19

[quarantine/continue/extensions/cli/src/subagent/executor.ts:125–199](../../../quarantine/continue/extensions/cli/src/subagent/executor.ts#L125): Escape can abort child; same streaming loop uses selected child model/API and finally restores global functions/permissions; parentSessionId is not used in this path.

### Evidence e20

[quarantine/continue/extensions/cli/src/compaction.ts:91–155](../../../quarantine/continue/extensions/cli/src/compaction.ts#L91): Compaction makes a separate model summary request and returns a short replacement history rather than immutable context edits.

### Evidence e21

[quarantine/continue/extensions/cli/src/stream/streamChatResponse.autoCompaction.ts:174–216](../../../quarantine/continue/extensions/cli/src/stream/streamChatResponse.autoCompaction.ts#L174): Successful automatic compaction calls updateSessionHistory with replacement history; failure retains current history.

### Evidence e22

[quarantine/continue/extensions/cli/src/session.ts:210–235](../../../quarantine/continue/extensions/cli/src/session.ts#L210): Persisted snapshot omits system messages and normalizes user editor state.

### Evidence e23

[quarantine/continue/core/util/history.ts:111–134](../../../quarantine/continue/core/util/history.ts#L111): History manager overwrites session JSON with current history and selected metadata; not an append-only original event store.

### Evidence e24

[quarantine/continue/extensions/cli/src/stream/messageQueue.ts:1–61](../../../quarantine/continue/extensions/cli/src/stream/messageQueue.ts#L1): FIFO in-memory queue holds message/images/timestamp and emits changes.

### Evidence e25

[quarantine/continue/extensions/cli/src/ui/hooks/useChat.ts:355–378](../../../quarantine/continue/extensions/cli/src/ui/hooks/useChat.ts#L355): Queued messages dispatch after response finally and a UI delay.

### Evidence e26

[quarantine/continue/extensions/cli/src/ui/hooks/useChat.ts:686–735](../../../quarantine/continue/extensions/cli/src/ui/hooks/useChat.ts#L686): Interrupt sends empty remote /message request or aborts local compaction/provider controller; partial non-tool assistant can be removed from visible history.

### Evidence e27

[quarantine/continue/extensions/cli/src/commands/serve.ts:448–482](../../../quarantine/continue/extensions/cli/src/commands/serve.ts#L448): Remote serve consumes queued messages one at a time, writes user history and creates per-response abort controller.

### Evidence e28

[quarantine/continue/extensions/cli/src/services/ConfigService.ts:347–403](../../../quarantine/continue/extensions/cli/src/services/ConfigService.ts#L347): Config reload/switch updates dependent model and MCP services without an inspected active-workflow fence.

### Evidence e29

[quarantine/continue/extensions/cli/src/tools/reportFailure.ts:8–89](../../../quarantine/continue/extensions/cli/src/tools/reportFailure.ts#L8): ReportFailure requires --id and API/auth, sets remote agent FAILED/error and marks metadata complete, unlike a nonfatal state-bearing complaint.

### Evidence e30

[quarantine/continue/extensions/cli/src/permissions/defaultPolicies.ts:7–58](../../../quarantine/continue/extensions/cli/src/permissions/defaultPolicies.ts#L7): TUI defaults ask for writes/Bash/MCP; headless defaults wildcard allow; plan excludes file writes but allows Bash.

### Evidence e31

[quarantine/continue/gui/src/redux/thunks/streamNormalInput.ts:174–256](../../../quarantine/continue/gui/src/redux/thunks/streamNormalInput.ts#L174): Editor compiles/prunes context before bridge stream, attaches provider prompt/completion devdata, and passes legacy slash metadata.

### Evidence e32

[quarantine/continue/gui/src/redux/thunks/streamNormalInput.ts:284–359](../../../quarantine/continue/gui/src/redux/thunks/streamNormalInput.ts#L284): Editor tool calls preprocess and evaluate policies, including parallel approved readonly work while other calls await approval.

### Evidence e33

[quarantine/continue/gui/src/redux/thunks/callToolById.ts:59–146](../../../quarantine/continue/gui/src/redux/thunks/callToolById.ts#L59): Editor routes client tools or core tools/call and stores context results/errors before continuation thunk.

### Evidence e34

[quarantine/continue/core/llm/streamChat.ts:25–120](../../../quarantine/continue/core/llm/streamChat.ts#L25): Core selects chat role model; legacy slash command generators receive IDE/LLM/history/abort/fetch capabilities, otherwise normal streamChat executes.

### Evidence e35

[quarantine/continue/core/tools/callTool.ts:70–109](../../../quarantine/continue/core/tools/callTool.ts#L70): Core supports HTTP and MCP tool routing with input coercion, timeout and returned error context.

### Evidence e36

[quarantine/continue/extensions/cli/src/stream/streamChatResponse.autoContinuation.test.ts:135–197](../../../quarantine/continue/extensions/cli/src/stream/streamChatResponse.autoContinuation.test.ts#L135): Mocked compaction/provider test asserts inserted continue and repeated provider call, not semantic summary completeness or original retention.

### Evidence e37

[quarantine/continue/extensions/cli/src/tools/subagent.test.ts:83–118](../../../quarantine/continue/extensions/cli/src/tools/subagent.test.ts#L83): Mocked child executor test checks inputs, streamed callback and completion formatting; does not exercise global state isolation.

### Evidence e38

[quarantine/continue/extensions/cli/src/tools/runTerminalCommand.test.ts:11–44](../../../quarantine/continue/extensions/cli/src/tools/runTerminalCommand.test.ts#L11): Shell tests assert actual echo output and platform-shaped cwd when run; no background/cancel/concurrency oracle in this read portion.

