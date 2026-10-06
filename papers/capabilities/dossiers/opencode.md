# opencode

A session-oriented coding agent with a durable normalized conversation/tool record, per-step model/tool resolution, JS/TS tool and plugin hooks, managed summaries and optionally concurrent child sessions.

Role: interactive coding agent. Runtime: TypeScript, JavaScript.

Pinned source: [https://github.com/sst/opencode.git](https://github.com/sst/opencode.git); revision/version `2006259a02a87edf9e37f253cbddf3188309026b`.

Bun/Effect process with instance-scoped session runners, AI SDK or feature-gated native provider streams, child shell/MCP processes and background session jobs.

Owns local sessions, tool settlement, runners and launched stdio children; consumes model providers and MCP/service APIs. Local child disposal is distinct from governing independent shared services.

Inspection: Read entry/loop/provider dispatch, tool settlement, shell execution, child resumption/background delivery, cancellation, compaction selection and pruning, message retrieval, plugin/config lifecycle, native schemas and relevant test excerpts. Source inspection only.

Limits of this study: Inspected excerpts rather than every file/branch. Default AI SDK transport internals, complete native-provider/GitLab workflow implementation and every optional tool dispatch were not traced. Experimental flags materially gate child background work and richer event emission. No reference code or tests executed.

## Actions

### SessionPrompt.prompt / loop

Surface: programmatic API.

Input: sessionID plus user parts, model/agent and optional noReply/tools overrides

Result: persisted user message and final assistant result

Lifecycle: persists then continues provider/tool steps; instance runner owns cancellation

Authority: server/program caller; tool rules merge agent and session policy

Evidence: [entry](#evidence-entry), [loop](#evidence-loop), [continuation](#evidence-continuation).

### shell

Surface: model tool.

Input: command; optional timeout in ms, workdir; description

Result: bounded streamed preview, final text, exit code and full-output file path when truncated

Lifecycle: awaited spawn races exit/timeout/abort; abort/timeout request termination; no public reusable shell process handle established

Authority: model under shell/external-directory rules and environment hook

Evidence: [shell-schema](#evidence-shell-schema), [shell](#evidence-shell), [tool-dispatch](#evidence-tool-dispatch).

### read

Surface: model tool.

Input: filePath; optional one-based offset and limit

Result: numbered text/directory entries or image/PDF attachment

Lifecycle: single read; media/binary and clipping rules apply

Authority: model with path permission checks; full read wrapper not fully inspected

Evidence: [read](#evidence-read), [registry](#evidence-registry).

### write

Surface: model tool.

Input: filePath, content

Result: write result, diff and diagnostics metadata

Lifecycle: permission then write, formatting and events

Authority: model; edit permission may ask operator

Evidence: [write-edit](#evidence-write-edit).

### edit

Surface: model tool.

Input: filePath, oldString, newString, optional replaceAll

Result: changed file and diff/diagnostic result

Lifecycle: per-file serialization; permission precedes write

Authority: model under edit policy

Evidence: [edit](#evidence-edit).

### glob / grep

Surface: model tools.

Input: glob: pattern/path; grep: pattern/path/include

Result: bounded file list or file/line/text search results

Lifecycle: awaited search-service query with cancellation signal

Authority: model under search/path permission rules

Evidence: [glob](#evidence-glob), [search](#evidence-search).

### apply_patch

Surface: model tool.

Input: patchText

Result: patch result; full mutation/result path not traced

Lifecycle: registered native tool parses hunks; schema/source boundary disclosed

Authority: model through common tool permission context

Evidence: [patch](#evidence-patch), [registry](#evidence-registry), [tool-dispatch](#evidence-tool-dispatch).

### task

Surface: model tool.

Input: description, prompt, subagent_type; optional task_id, command, background

Result: child task_id and terminal text, or running background status

Lifecycle: new/resumed child session; feature-gated background start/extend; terminal parent notification; cancellation propagates

Authority: model with task permission and child-agent restrictions

Evidence: [task](#evidence-task), [task-continuation](#evidence-task-continuation), [feature-flags](#evidence-feature-flags).

### todowrite

Surface: model tool.

Input: todos with content, status and priority

Result: session todo list as JSON

Lifecycle: awaited persistent session update

Authority: model under todowrite rule

Evidence: [todo](#evidence-todo).

### question

Surface: model tool.

Input: array of operator questions

Result: operator answers associated with the originating call

Lifecycle: waits for answer service

Authority: model can request operator input when this client/agent exposes the tool

Evidence: [question](#evidence-question), [registry](#evidence-registry).

### skill

Surface: model tool.

Input: name

Result: instruction text and sampled resource paths

Lifecycle: lookup/read; no standalone workflow execution inferred

Authority: model under skill permission

Evidence: [skill](#evidence-skill).

### webfetch / websearch

Surface: model tools.

Input: URL/format/timeout or query/search-provider settings

Result: fetch/search output; complete transport/error semantics not inspected

Lifecycle: registered provider-backed tools

Authority: model under per-tool permissions

Evidence: [web](#evidence-web), [websearch](#evidence-websearch), [registry](#evidence-registry).

### lsp / plan_exit

Surface: gated model tools.

Input: lsp: operation/filePath/line/character/query; plan_exit: empty arguments

Result: language-service result or operator plan-transition question; complete latter dispatch not traced

Lifecycle: conditional registration; optional behavior is not unconditional baseline

Authority: model under LSP policy; operator answers planning transition

Evidence: [optional-tools](#evidence-optional-tools), [plan](#evidence-plan), [registry](#evidence-registry), [feature-flags](#evidence-feature-flags).

### custom JS/TS and plugin tools; named hooks

Surface: extension/programmatic surface.

Input: tool schemas/execute implementations; named hook input and mutable output

Result: custom tool outputs with clipping; altered request/context/tool output

Lifecycle: instance-cached imports; named hooks awaited; event callbacks fire asynchronously; dispose callbacks on teardown

Authority: operator-authored extensions; model can edit files when filesystem policy permits, but automatic live reload not established

Evidence: [registry](#evidence-registry), [plugins](#evidence-plugins), [tool-dispatch](#evidence-tool-dispatch).

### MCP tools

Surface: model/extension surface.

Input: discovered server JSON schemas and named arguments

Result: MCP content converted into tool outputs/attachments

Lifecycle: request timeout; local stdio child lifetime tied to instance; remote servers consumed

Authority: model under merged tool permissions; server configured by operator/program

Evidence: [tool-dispatch](#evidence-tool-dispatch), [mcp-call](#evidence-mcp-call), [mcp-owner](#evidence-mcp-owner), [mcp-dispose](#evidence-mcp-dispose).

### messages / message / config.update

Surface: server programmatic APIs.

Input: session/message ID and optional limit/before; configuration patch

Result: stored messages/pagination or updated config

Lifecycle: retrieval does not restore context by itself; config update marks instance for disposal

Authority: operator/program API; not a dedicated model audit/repair tool

Evidence: [query](#evidence-query), [config](#evidence-config).

## Capabilities

### filesystem

**I — Files** (source): Read, write, edit, glob and grep are registered model operations with path checks, bounded output and edit policy.

Evidence: [read](#evidence-read), [write-edit](#evidence-write-edit), [edit](#evidence-edit), [glob](#evidence-glob), [search](#evidence-search).

### processes

**I — OS programs** (source): shell spawns and streams an OS command, records exit/overflow metadata and races exit, timeout and abort. Reusable process/PTY handles were not established.

Evidence: [shell](#evidence-shell), [shell-schema](#evidence-shell-schema).

### code-actions

**S — Code actions** (source): Model-written shell programs and custom JS/TS tool implementations can compose computation; the default provider interface still dispatches named JSON-schema tools rather than a persistent code-action interpreter.

Evidence: [shell](#evidence-shell), [registry](#evidence-registry), [tool-dispatch](#evidence-tool-dispatch).

### persistent-kernel

**? — Kernel** (inspection-limit): No model-addressable persistent interpreter was established in the inspected shell, tool registry and session loop. Instance/server memory and fresh shell spawns do not establish such a kernel.

### standing-database

**? — Standing DB** (inspection-limit): Internal session storage and todo updates are visible, but no model-queryable standing structured database contract was established in this loop/tool/MCP study. External MCP composition remains possible, not traced.

### workflow-programming

**S — Workflows** (source): Custom JS/TS tools, before/after execution hooks and message/compaction transformations provide executable composition. A replaceable first-class workflow scheduler was not established.

Evidence: [registry](#evidence-registry), [plugins](#evidence-plugins), [loop](#evidence-loop), [compact-transform](#evidence-compact-transform).

### multi-model

**I — Models** (source): The loop resolves the selected model each provider step; child agents may override the parent's model. This does not itself create live collaboration.

Evidence: [loop](#evidence-loop), [task](#evidence-task).

### live-collaboration

**L — Peer chat** (source): Experimental background tasks accept updates addressed by task_id while busy and later inject terminal notifications into the parent. Inspected wiring is parent-child session/task routing, not a general peer-room or arbitrary bidirectional live bus.

Evidence: [task-continuation](#evidence-task-continuation), [feature-flags](#evidence-feature-flags), [task-test](#evidence-task-test).

### concurrent-work

**I — Concurrency** (source): Feature-gated background child jobs expose task/session identity, extension and completion; independent job work can overlap the parent. Their lifetime is still linked to cancellation/instance scope.

Evidence: [task-continuation](#evidence-task-continuation), [cancel](#evidence-cancel), [feature-flags](#evidence-feature-flags).

### steering-interrupt

**I — Steer/interrupt** (source): New user turns become reminders on later loop steps; cancellation propagates through session descendants, provider AbortController and shell race/termination. Remote providers acknowledging cancellation was not validated.

Evidence: [loop](#evidence-loop), [cancel](#evidence-cancel), [provider](#evidence-provider), [shell](#evidence-shell).

### turn-redefinition

**S — Turn program** (source): Message transformations, compaction prompt hooks, tool replacement and per-step model/tool resolution can alter a continuation's inputs. The main while-loop's scheduling program is not shown as a hot-replaced turn definition.

Evidence: [loop](#evidence-loop), [plugins](#evidence-plugins), [registry](#evidence-registry), [compact-transform](#evidence-compact-transform).

### compaction

**I — Compaction** (source): Overflow/pending compaction yields a persisted tool-free summary and retained tail boundary. Pruning marks prior outputs; active-message filtering uses summaries/tail_start_id. Hook-supplied prompts/context are supported.

Evidence: [loop](#evidence-loop), [compact-transform](#evidence-compact-transform), [compact-prune](#evidence-compact-prune), [active-context](#evidence-active-context), [compact-test](#evidence-compact-test).

### context-repair

**S — Repair** (source): Original stored messages remain retrievable through server message APIs after active-context filtering; output-file paths support further inspection. No dedicated model repair/lineage operation is established, and clipped-output originals have an expiry.

Evidence: [query](#evidence-query), [compact-prune](#evidence-compact-prune), [active-context](#evidence-active-context), [clipped-output](#evidence-clipped-output).

### original-audit

**L — Original audit** (source): Durable normalized message/tool settlement is useful, but complete original provider/program/transformation capture is not established. Raw chunks are selectively enabled, richer events are gated and separate full-output files expire after seven days.

Evidence: [settlement](#evidence-settlement), [provider](#evidence-provider), [feature-flags](#evidence-feature-flags), [clipped-output](#evidence-clipped-output).

### audit-query

**S — Audit query** (source): Program callers can retrieve all/paged session messages or a message by ID; models can read/search known overflow files. This queries the retained conversation/output scope, not a demonstrated complete audit corpus.

Evidence: [query](#evidence-query), [shell](#evidence-shell), [read](#evidence-read), [search](#evidence-search).

### hot-change

**L — Hot change** (source): Tools/plugins are instance-cached and config.update marks the instance for disposal. Inspected code does not enforce Arconaut's default activation after affected turns/workflows conclude; disposal finalizers cancel owned work.

Evidence: [registry](#evidence-registry), [plugins](#evidence-plugins), [config](#evidence-config), [cancel](#evidence-cancel), [mcp-dispose](#evidence-mcp-dispose).

### rebuild-continuity

**L — Rebuild continuity** (source): Stored sessions support conversation retrieval, while instance teardown cancels runners and closes/kills owned MCP children. No executable rebuild/outpost handoff retaining unresolved owned work was established.

Evidence: [query](#evidence-query), [cancel](#evidence-cancel), [mcp-dispose](#evidence-mcp-dispose).

### remote-services

**I — Remote** (source): MCP tool calls consume service schemas/results; local stdio servers are explicitly launched and disposed by the instance. Remote transport reconnect/session-continuity details were outside inspected excerpts.

Evidence: [mcp-call](#evidence-mcp-call), [mcp-owner](#evidence-mcp-owner), [mcp-dispose](#evidence-mcp-dispose), [tool-dispatch](#evidence-tool-dispatch).

### self-improvement

**? — Self-improve** (inspection-limit): Normal file/shell access and custom plugins provide editing capability. The inspected loop, extension lifecycle and tests do not establish a dedicated model-governed candidate/evaluate/promote harness-improvement program.

### complaints

**? — Complaints** (inspection-limit): The native registry, question/todo and session APIs inspected do not establish a model-invokable complaint operation capturing agent state and follow-up. This is not a claim about all uninspected plugins.

### authority

**I — Authority** (source): Standing permission rules merge agent/session policy. Defaults allow most actions but ask on selected sensitive scopes; build and plan agents have different edit policies. Models act through these rules and can request operator answers.

Evidence: [permission](#evidence-permission), [tool-dispatch](#evidence-tool-dispatch), [task](#evidence-task), [question](#evidence-question).

### evaluation

**I — Evaluation** (source): Inspected tests directly assert busy-child update identity/completion ordering and compaction tail boundary. Mocked provider/prompt streams make these local orchestration oracles, not provider integration or semantic summary-fidelity validation.

Evidence: [task-test](#evidence-task-test), [compact-test](#evidence-compact-test).

### time-order

**S — Time/order** (source): Message/call IDs and stored wall timestamps support turn/tool ordering; Effect durations enforce shell/request deadlines. A specified monotonic, restart or paused-time contract was not established.

Evidence: [loop](#evidence-loop), [settlement](#evidence-settlement), [shell](#evidence-shell), [mcp-call](#evidence-mcp-call).

## Inspected test oracles

- [quarantine/opencode/packages/opencode/test/tool/task.test.ts](../../../quarantine/opencode/packages/opencode/test/tool/task.test.ts): Busy child task update and completion notification ordering. Oracle: Deferred mocked prompts prove task_id stability and that completion/notification uses the updated result; no live provider, room delivery or OS process cancellation is tested by this case. Read, **not executed**.
- [quarantine/opencode/packages/opencode/test/session/compaction.test.ts](../../../quarantine/opencode/packages/opencode/test/session/compaction.test.ts): Retained recent-turn boundary and overflow arithmetic. Oracle: Seeded history asserts tail_start_id for the second of three turns; canned LLM streams isolate orchestration and do not establish summary semantic fidelity. Read, **not executed**.

## Useful mechanisms

- Durable tool-call identity and explicit success/error settlement decouple tool execution from display.
- Per-step model/tool resolution plus named extension hooks provide several concrete intervention boundaries.
- Retained-tail summaries and logical output pruning preserve a recoverable stored conversation scope.
- Busy-child update handling has a concrete identity/order oracle rather than only a background-task claim.

## Material limits

- Useful experimental child/event behavior is gated and should not be assigned to every default deployment.
- Normalized conversation storage and expiring overflow files do not satisfy full original audit requirements.
- Instance disposal cancels owned work; config invalidation is not a quiescent executable refit.
- Some optional native tools were inspected at schema/registration boundary only; complete dispatch semantics remain outside this row.

## Arconaut design questions

- Which changes should resolve per provider step, and which must wait for the affected turn/workflow boundary?
- How should busy addressed participants receive steering without reducing live collaboration to parent-child terminal notifications?
- How can compaction lineage and every original provider/program output remain queryable independently of active context or output retention cleanup?
- Which local children belong to harness quiescence, and which independent services must survive refit without being governed by Arconaut?

## Evidence

### Evidence entry

[quarantine/opencode/packages/opencode/src/session/prompt.ts:1188–1206](../../../quarantine/opencode/packages/opencode/src/session/prompt.ts#L1188): prompt persists a user message, updates session permission overrides and starts the loop unless noReply.

### Evidence loop

[quarantine/opencode/packages/opencode/src/session/prompt.ts:1217–1430](../../../quarantine/opencode/packages/opencode/src/session/prompt.ts#L1217): The continuing loop filters compacted messages, handles pending tasks/compaction, resolves current model/agent/tools and transforms messages before processor.process.

### Evidence continuation

[quarantine/opencode/packages/opencode/src/session/prompt.ts:1432–1478](../../../quarantine/opencode/packages/opencode/src/session/prompt.ts#L1432): Processor results drive stop or compaction; interruption finalizes the assistant message and the session runner owns the loop.

### Evidence provider

[quarantine/opencode/packages/opencode/src/session/llm.ts:278–369](../../../quarantine/opencode/packages/opencode/src/session/llm.ts#L278): Default streamText gets prepared tools/messages, provider middleware and scoped AbortController; raw chunks are enabled specifically for Copilot.

### Evidence settlement

[quarantine/opencode/packages/opencode/src/session/processor.ts:157–249](../../../quarantine/opencode/packages/opencode/src/session/processor.ts#L157): Call IDs identify running tool parts; successful and failed results settle durable parts, completion Deferreds and timestamps.

### Evidence tool-dispatch

[quarantine/opencode/packages/opencode/src/session/tools.ts:45–155](../../../quarantine/opencode/packages/opencode/src/session/tools.ts#L45): Registry tools and MCP tools receive context, merged permission asks and before/after plugin hooks; tools are invoked through execute.

### Evidence registry

[quarantine/opencode/packages/opencode/src/tool/registry.ts:137–276](../../../quarantine/opencode/packages/opencode/src/tool/registry.ts#L137): Instance-cached custom JS/TS and plugin tool discovery is composed with built-ins; LSP, questions and plan tools have gates.

### Evidence shell-schema

[quarantine/opencode/packages/opencode/src/tool/shell/prompt.ts:22–33](../../../quarantine/opencode/packages/opencode/src/tool/shell/prompt.ts#L22): shell accepts command, optional positive timeout, workdir and description.

### Evidence shell

[quarantine/opencode/packages/opencode/src/tool/shell.ts:443–605](../../../quarantine/opencode/packages/opencode/src/tool/shell.ts#L443): Spawned shell output streams through bounded tails and overflow files; exit, abort and timeout race, with termination and exit metadata.

### Evidence task

[quarantine/opencode/packages/opencode/src/tool/task.ts:43–158](../../../quarantine/opencode/packages/opencode/src/tool/task.ts#L43): task accepts prompt, description, subagent_type, task_id, command and background; permission checks and child-session/model selection precede execution.

### Evidence task-continuation

[quarantine/opencode/packages/opencode/src/tool/task.ts:174–318](../../../quarantine/opencode/packages/opencode/src/tool/task.ts#L174): Child prompt execution returns its last text; busy background task extension, completion notifications, same task_id resumption and cancellation are wired but background requires an experimental flag.

### Evidence cancel

[quarantine/opencode/packages/opencode/src/session/run-state.ts:37–145](../../../quarantine/opencode/packages/opencode/src/session/run-state.ts#L37): Instance-scoped runners are canceled at disposal; explicit cancellation traverses descendant background jobs and startShell rejects a busy runner.

### Evidence compact-prune

[quarantine/opencode/packages/opencode/src/session/compaction.ts:253–300](../../../quarantine/opencode/packages/opencode/src/session/compaction.ts#L253): Pruning marks old completed tool outputs with compacted timestamps rather than deleting their stored output in this path.

### Evidence compact-transform

[quarantine/opencode/packages/opencode/src/session/compaction.ts:343–445](../../../quarantine/opencode/packages/opencode/src/session/compaction.ts#L343): Compaction plugins can alter prompt/context; selected history becomes a tool-free summarizing request and tail_start_id is persisted.

### Evidence active-context

[quarantine/opencode/packages/opencode/src/session/message-v2.ts:533–584](../../../quarantine/opencode/packages/opencode/src/session/message-v2.ts#L533): Completed summaries and retained tail boundaries control the filtered active message sequence.

### Evidence clipped-output

[quarantine/opencode/packages/opencode/src/tool/truncate.ts:10–73](../../../quarantine/opencode/packages/opencode/src/tool/truncate.ts#L10): Large outputs have separately written originals, with a seven-day cleanup policy and line/byte clipping defaults.

### Evidence plugins

[quarantine/opencode/packages/opencode/src/plugin/index.ts:249–299](../../../quarantine/opencode/packages/opencode/src/plugin/index.ts#L249): Configuration hooks run during instance initialization; event hooks and awaited named mutable-output hooks are available, with disposal callbacks.

### Evidence config

[quarantine/opencode/packages/opencode/src/server/routes/instance/httpapi/handlers/config.ts:18–21](../../../quarantine/opencode/packages/opencode/src/server/routes/instance/httpapi/handlers/config.ts#L18): Config API updates project configuration then marks the instance for disposal.

### Evidence query

[quarantine/opencode/packages/opencode/src/server/routes/instance/httpapi/handlers/session.ts:104–150](../../../quarantine/opencode/packages/opencode/src/server/routes/instance/httpapi/handlers/session.ts#L104): Session messages can be retrieved in full, paginated or by message ID through the server API.

### Evidence permission

[quarantine/opencode/packages/opencode/src/agent/agent.ts:101–158](../../../quarantine/opencode/packages/opencode/src/agent/agent.ts#L101): Default permissions allow most actions but ask for doom loops, external directories and environment files; primary/plan agents merge distinct restrictions.

### Evidence mcp-call

[quarantine/opencode/packages/opencode/src/mcp/index.ts:159–186](../../../quarantine/opencode/packages/opencode/src/mcp/index.ts#L159): MCP schemas become tools and client.callTool sends named arguments with a request timeout.

### Evidence mcp-owner

[quarantine/opencode/packages/opencode/src/mcp/index.ts:423–445](../../../quarantine/opencode/packages/opencode/src/mcp/index.ts#L423): Local MCP creates a stdio child using command, arguments, cwd and environment.

### Evidence mcp-dispose

[quarantine/opencode/packages/opencode/src/mcp/index.ts:551–583](../../../quarantine/opencode/packages/opencode/src/mcp/index.ts#L551): Clients are initialized concurrently and instance disposal terminates stdio descendants and closes connected clients.

### Evidence read

[quarantine/opencode/packages/opencode/src/tool/read.ts:285–360](../../../quarantine/opencode/packages/opencode/src/tool/read.ts#L285): Read returns directories, typed image/PDF attachments or numbered text with offset/limit and binary/truncation checks.

### Evidence write-edit

[quarantine/opencode/packages/opencode/src/tool/write.ts:20–83](../../../quarantine/opencode/packages/opencode/src/tool/write.ts#L20): write accepts content and filePath, asks edit permission with a diff, writes/format-checks and publishes changes.

### Evidence edit

[quarantine/opencode/packages/opencode/src/tool/edit.ts:35–120](../../../quarantine/opencode/packages/opencode/src/tool/edit.ts#L35): edit accepts oldString/newString/replaceAll, serializes file access and asks permission before writing.

### Evidence search

[quarantine/opencode/packages/opencode/src/tool/grep.ts:33–84](../../../quarantine/opencode/packages/opencode/src/tool/grep.ts#L33): grep validates roots/permissions and asks the search service for file/line/text results with abort.

### Evidence glob

[quarantine/opencode/packages/opencode/src/tool/glob.ts:28–79](../../../quarantine/opencode/packages/opencode/src/tool/glob.ts#L28): glob checks root permissions and returns bounded matching files with a truncation indication.

### Evidence question

[quarantine/opencode/packages/opencode/src/tool/question.ts:15–44](../../../quarantine/opencode/packages/opencode/src/tool/question.ts#L15): question awaits operator answers associated with session and tool IDs.

### Evidence todo

[quarantine/opencode/packages/opencode/src/tool/todo.ts:26–55](../../../quarantine/opencode/packages/opencode/src/tool/todo.ts#L26): todowrite updates session todos after permission checking and returns structured JSON.

### Evidence skill

[quarantine/opencode/packages/opencode/src/tool/skill.ts:15–68](../../../quarantine/opencode/packages/opencode/src/tool/skill.ts#L15): skill resolves named instruction material, asks permission and returns text plus sampled resource paths.

### Evidence optional-tools

[quarantine/opencode/packages/opencode/src/tool/lsp.ts:38–65](../../../quarantine/opencode/packages/opencode/src/tool/lsp.ts#L38): Gated lsp defines location/query operations and performs external-directory and lsp permission checks; dispatch beyond this excerpt was not traced.

### Evidence web

[quarantine/opencode/packages/opencode/src/tool/webfetch.ts:25–48](../../../quarantine/opencode/packages/opencode/src/tool/webfetch.ts#L25): webfetch declares URL, format and timeout and asks webfetch permission; the complete fetch/error path was not traced.

### Evidence websearch

[quarantine/opencode/packages/opencode/src/tool/websearch.ts:99–132](../../../quarantine/opencode/packages/opencode/src/tool/websearch.ts#L99): websearch selects a configured search provider and dispatches query/search settings; remote transport behavior was not traced.

### Evidence patch

[quarantine/opencode/packages/opencode/src/tool/apply_patch.ts:23–39](../../../quarantine/opencode/packages/opencode/src/tool/apply_patch.ts#L23): apply_patch takes nonempty patchText and begins parsing patch hunks; full mutation behavior was not traced.

### Evidence plan

[quarantine/opencode/packages/opencode/src/tool/plan.ts:16–35](../../../quarantine/opencode/packages/opencode/src/tool/plan.ts#L16): plan_exit has an empty argument schema and asks the operator about moving from planning to building.

### Evidence feature-flags

[quarantine/opencode/packages/opencode/src/effect/runtime-flags.ts:1–55](../../../quarantine/opencode/packages/opencode/src/effect/runtime-flags.ts#L1): Background subagents, LSP, plan mode and event-system features have experimental gates; native LLM has its own flag.

### Evidence task-test

[quarantine/opencode/packages/opencode/test/tool/task.test.ts:590–662](../../../quarantine/opencode/packages/opencode/test/tool/task.test.ts#L590): Mocked prompt responses and Deferreds test that a running child update keeps its task identity and completion/notification waits for the updated run.

### Evidence compact-test

[quarantine/opencode/packages/opencode/test/session/compaction.test.ts:944–968](../../../quarantine/opencode/packages/opencode/test/session/compaction.test.ts#L944): A seeded three-turn test asserts persisted tail_start_id points at the second user turn under a two-turn retention configuration.

