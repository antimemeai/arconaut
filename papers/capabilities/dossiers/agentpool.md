# agentpool

Programmable agents, message graphs, teams/chains and model-executed Python tools, with strong live object agency but cancellation, durable-original and safe activation limits.

Role: programmable multi-agent framework. Runtime: Python.

Pinned source: [https://github.com/phil65/agentpool](https://github.com/phil65/agentpool); revision/version `b6ddbea9cb66173c096942ac57397b636e0a2248`.

Async Python pool/native PydanticAI turn wrapper; optional owned external ACP/CLI agents; task/team concurrency and environment-backed processes.

Pool owns local managers/storage/skills/MCP connections and node lifetime; configured backends may be external. Resident debug object store belongs to framework, not independent shared fabric.

Inspection: Selected source excerpts trace exposed entry/loop/request/action/result/state boundaries and relevant capability mechanisms; listed tests are read as source only.

Limits of this study: No acquired code executed, dependencies installed or live providers invoked. Claims concern this pinned snapshot; untraced catalog tools and dependency internals are not inferred.

## Actions

### run / run_stream

Surface: operator and programmable API.

Input: prompts, session/parent/message IDs, deps, history, store_history

Result: ChatMessage or stream events including tool updates and StreamCompleteEvent

Lifecycle: awaited/streamed; queued prompts become subsequent turns

Authority: program/user invoke agents; native model tools dispatch inside PydanticAI loop

Evidence: [base](#evidence-base), [result](#evidence-result), [agentlet](#evidence-agentlet), [iterate](#evidence-iterate).

### connect_to / Team.run_iter / TeamRun.execute_iter

Surface: programmable workflow API.

Input: named source/targets, run/context/forward type, transforms/conditions, queued mode; team nodes

Result: forwarding events, per-member messages/errors, sequential chain or parallel team results

Lifecycle: in-process message graph and async tasks; no durable delivery receipt

Authority: program defines wiring; connected model responses drive subsequent models

Evidence: [talk](#evidence-talk), [team](#evidence-team), [chain](#evidence-chain).

### list_available_nodes / task / ask_<worker>

Surface: model tools.

Input: agent/team name, prompt, async_mode; worker config history flags

Result: result or background task ID and /tasks/<id>/output.md; worker reply

Lifecycle: sync delegated stream or retained in-process task writing internal FS

Authority: model selects configured/current pool nodes; tool confirmation/hook settings apply at outer dispatch

Evidence: [task](#evidence-task), [workers](#evidence-workers).

### start_process / get_process_output / wait_for_process / kill_process / release_process / list_processes

Surface: model tools.

Input: command,args,cwd,env,output_limit or process_id/filter

Result: process ID, output/status/truncation/exit code/error strings

Lifecycle: selected execution environment manages work; kill/release semantics delegated

Authority: model uses explicit or agent environment and tool authority

Evidence: [process](#evidence-process), [kill](#evidence-kill).

### read / write / edit / edit_batch / delete_path

Surface: model filesystem tools.

Input: path; encoding/line/limit; content/mode/overwrite; replacement tuples

Result: text/media, diffs/diagnostics, metadata or error text

Lifecycle: awaited filesystem backend reads/writes; replacements staged before write

Authority: configured filesystem/environment; overwrite explicit flag plus native permissions

Evidence: [fsread](#evidence-fsread), [fswrite](#evidence-fswrite), [fsedit](#evidence-fsedit).

### execute_tool

Surface: model Python composition tool.

Input: python_code containing async main and return; title

Result: return value or error; script/meta under codemode/scripts in internal FS

Lifecycle: fresh namespace per call; await main; no persistent external kernel

Authority: outer tool dispatch gate; inner FunctionTool callables bypass native per-tool wrapper hooks/confirmation

Evidence: [aggregate](#evidence-aggregate), [codemode](#evidence-codemode), [rawbindings](#evidence-rawbindings), [callable](#evidence-callable).

### execute_introspection

Surface: model development/debug tool.

Input: async main code,title using ctx,run_ctx,me,save,state

Result: string result/error, saved provider-local objects and stream progress

Lifecycle: exec in current harness process; arbitrary object state survives subsequent calls only

Authority: model when debug toolset enabled has direct live runtime object agency

Evidence: [debug](#evidence-debug).

### /compact / compact_conversation

Surface: operator command / programmable API.

Input: preset minimal/balanced/summarizing or custom pipeline/history

Result: before/after counts; substituted working history

Lifecycle: ordered async transforms and optional model summary; immediate history replacement

Authority: operator/program; introspection could call API; no separate activation barrier

Evidence: [pipeline](#evidence-pipeline), [compactcmd](#evidence-compactcmd), [compact](#evidence-compact).

### SQL filter_messages / replace_conversation_messages

Surface: storage API.

Input: SessionQuery or session ID/new ChatMessages

Result: filtered chat records or deleted/added counts

Lifecycle: per-message commit; replace deletes then reinserts across configured providers

Authority: program/storage; OpenCode-compatible compaction route calls destructive replacement

Evidence: [sql](#evidence-sql), [storage](#evidence-storage), [replace](#evidence-replace), [servercompact](#evidence-servercompact).

### interrupt / set_model / set_tool_confirmation_mode

Surface: operator and programmable API.

Input: agent/model/mode

Result: interrupt/state signals or adapter errors

Lifecycle: native task cancellation and immediate config assignment; ACP notification/local cancellation

Authority: program/user; no awaited global provider/process quiescence

Evidence: [model](#evidence-model), [interrupt](#evidence-interrupt), [confirm](#evidence-confirm), [acpcancel](#evidence-acpcancel).

### ACPAgent prompt / fork_session / set_model

Surface: adapter protocol API.

Input: session/prompt blocks,cwd,MCP servers,model

Result: protocol updates, session IDs and reconstructed ChatMessage

Lifecycle: owns ACP child process; model/action execution delegated to child

Authority: configured executable and remote permission handler/auto_approve

Evidence: [acpinit](#evidence-acpinit), [acpturn](#evidence-acpturn), [acpcancel](#evidence-acpcancel).

### add_agent / create_config / PATCH /config

Surface: program API / model schema tool / server API.

Input: agent instance; candidate YAML/JSON/TOML; Config fields

Result: registered initialized node; validation result; assigned config

Lifecycle: agent enters lifetime immediately; schema validation does not activate; PATCH assigns fields only

Authority: program adds nodes; model validates configurations or acts through introspection; server caller edits config

Evidence: [add](#evidence-add), [schema](#evidence-schema), [config](#evidence-config).

## Capabilities

### filesystem

**I — Files** (source): Configured FSSpec read/write/smart-edit tools return media/diffs/errors; append read failure can discard old content. Backend internals untraced.

Evidence: [fsread](#evidence-fsread), [fswrite](#evidence-fswrite), [fsedit](#evidence-fsedit).

### processes

**I — OS programs** (source): Explicit background start/output/wait/kill/release/list handles delegate to environment process manager; OS reaping/persistence reside in dependency.

Evidence: [process](#evidence-process), [kill](#evidence-kill).

### code-actions

**I — Code actions** (source): Model-authored async Python composes current tool callables; debug introspection executes against live runtime objects.

Evidence: [codemode](#evidence-codemode), [debug](#evidence-debug).

### persistent-kernel

**S — Kernel** (source): Debug toolset retains saved Python objects in current provider instance; ordinary codemode namespace is fresh each call. This is harness-resident state, not a shared independently governed kernel.

Evidence: [debug](#evidence-debug), [codemode](#evidence-codemode).

### standing-database

**S — Standing DB** (source): SQL provider supplies queryable conversation storage, not a general model-visible standing analytic database; live object introspection can compose services.

Evidence: [sql](#evidence-sql), [debug](#evidence-debug).

### workflow-programming

**I — Workflows** (source): Programmable transformed/conditional message connections, queued chains, concurrent teams, worker/subagent tools and Python codemode are real composition paths.

Evidence: [talk](#evidence-talk), [team](#evidence-team), [chain](#evidence-chain), [task](#evidence-task), [codemode](#evidence-codemode).

### multi-model

**I — Models** (source): Pool constructs separately configured typed agents; native agent has own model and teams/connections invoke member models, distinct from a provider selector.

Evidence: [pool](#evidence-pool), [agentlet](#evidence-agentlet), [team](#evidence-team), [talk](#evidence-talk).

### live-collaboration

**S — Peer chat** (source): Message graph connects independent agents and forwards responses/context; team jobs coexist. In-process queues and emitted flow events are not durable room/mail receipts.

Evidence: [talk](#evidence-talk), [team](#evidence-team).

### concurrent-work

**L — Concurrency** (source): Parallel team tasks and background subagents are implemented; unfinished team cancellation is not awaited and agent shared stream/cancel state has no traced per-run isolation barrier.

Evidence: [team](#evidence-team), [task](#evidence-task), [base](#evidence-base).

### steering-interrupt

**L — Steer/interrupt** (source): Post-tool injected context/queued next-turn prompts are wired. Native cancel and ACP notification do not await all active work/provider cessation.

Evidence: [inject](#evidence-inject), [base](#evidence-base), [model](#evidence-model), [acpcancel](#evidence-acpcancel).

### turn-redefinition

**S — Turn program** (source): Hooks, processors, runtime-object introspection and fresh agentlet construction enable extensive programmable changes; provider-loop internals reside in PydanticAI.

Evidence: [agentlet](#evidence-agentlet), [hooks](#evidence-hooks), [debug](#evidence-debug).

### compaction

**I — Compaction** (source): Composable ordered filtering/truncation/summary presets mutate history through command/API and server summary route; actual durable replacement is destructive.

Evidence: [pipeline](#evidence-pipeline), [truncate](#evidence-truncate), [summary](#evidence-summary), [compactcmd](#evidence-compactcmd), [servercompact](#evidence-servercompact).

### context-repair

**L — Repair** (source): Programs/models with introspection can inspect/set history or rebuild context; compaction creates replacement IDs and server path deletes stored originals, so recoverability is not assured.

Evidence: [debug](#evidence-debug), [compact](#evidence-compact), [replace](#evidence-replace), [servercompact](#evidence-servercompact).

### original-audit

**L — Original audit** (source): Successful final SQL records serialize model parts; history-disabled/partial native runs and stream/process events are not full originals. Server compaction deletes prior stored messages.

Evidence: [result](#evidence-result), [iterate](#evidence-iterate), [sql](#evidence-sql), [replace](#evidence-replace), [servercompact](#evidence-servercompact), [logs](#evidence-logs).

### audit-query

**L — Audit query** (source): SessionQuery filters SQL conversation records; bounded clearable memory logs are separate. No unified original causal event query traced.

Evidence: [sql](#evidence-sql), [storage](#evidence-storage), [logs](#evidence-logs).

### hot-change

**L — Hot change** (source): Native model assignment/live introspection/tool refresh are available, with one agentlet constructed per run; config PATCH merely assigns metadata. No affected-turn/workflow deferred activation contract.

Evidence: [model](#evidence-model), [agentlet](#evidence-agentlet), [codemode](#evidence-codemode), [debug](#evidence-debug), [config](#evidence-config).

### rebuild-continuity

**? — Rebuild continuity** (inspection-limit): No compiled harness handoff/rebuild/reinhabitation path established in native/ACP run, interruption, pool or configuration paths.

### remote-services

**S — Remote** (source): MCP/ACP and configured execution environments are connection surfaces; pool/ACP adapter own lifecycle managers/child processes. Independently governed shared computation is not imposed.

Evidence: [pool](#evidence-pool), [acpinit](#evidence-acpinit), [process](#evidence-process).

### self-improvement

**S — Self-improve** (source): Enabled debug tool lets model modify live ctx/me/pool and retain objects; schema validation and file/process tools support development. No governed experiment/rollback/refit loop traced.

Evidence: [debug](#evidence-debug), [schema](#evidence-schema), [fsedit](#evidence-fsedit), [process](#evidence-process).

### complaints

**? — Complaints** (inspection-limit): No model complaint with captured state plus external issue-table lifecycle established in inspected tool catalogs/storage paths; logs are not that contract.

### authority

**L — Authority** (source): Native never/per_tool/always-style delegated confirmation exists; pre-hooks can change args after confirmation and inner codemode raw callables do not receive native per-tool gates/hooks. ACP authority remains child/protocol specific.

Evidence: [confirm](#evidence-confirm), [hooks](#evidence-hooks), [rawbindings](#evidence-rawbindings), [callable](#evidence-callable), [acpcancel](#evidence-acpcancel).

### evaluation

**S — Evaluation** (source): Inspected tests cover simulated model graph routing/background cancellation, structural tool schemas and deterministic compaction transforms; no executed or real-provider/refit guarantee implied.

Evidence: [team](#evidence-team), [talk](#evidence-talk), [pipeline](#evidence-pipeline), [codemode](#evidence-codemode).

### time-order

**L — Time/order** (source): Session/run/message/parent IDs, timestamps, process elapsed time and task IDs exist; forwarded events precede actual target execution and there is no traced global durable sequence.

Evidence: [base](#evidence-base), [iterate](#evidence-iterate), [task](#evidence-task), [talk](#evidence-talk), [process](#evidence-process), [sql](#evidence-sql).

## Inspected test oracles

- [quarantine/agentpool/tests/teams/test_team_run.py](../../../quarantine/agentpool/tests/teams/test_team_run.py): Background chain execution, failure and cancellation Oracle: function_to_model delayed callbacks assert final member/name, busy/wait states and timing; cancellation asserts local busy cleared, not external provider/process quiescence. Read, **not executed**.
- [quarantine/agentpool/tests/messaging/test_connection_registry.py](../../../quarantine/agentpool/tests/messaging/test_connection_registry.py): Graph chain/broadcast event routing Oracle: TestModel agents assert count/source/target of flow events; these events emit before target processing and are not delivery acknowledgments. Read, **not executed**.
- [quarantine/agentpool/tests/messaging/test_compaction.py](../../../quarantine/agentpool/tests/messaging/test_compaction.py): Conditional ordered transforms/config building Oracle: Artificial Pydantic model messages assert retained counts/removed thinking/step types; some preset assertions only <= input count. No archive recovery or durable atomic replacement oracle. Read, **not executed**.
- [quarantine/agentpool/tests/test_codemode_provider.py](../../../quarantine/agentpool/tests/test_codemode_provider.py): Remote code-mode aggregation/schema registration Oracle: TestModel and simple callables assert one tool/name/descriptions/python_code schema; no composed execution, inner permission gates or external kernel continuity exercised in inspected tests. Read, **not executed**.

## Useful mechanisms

- Live ctx/me/pool introspection is concrete model agency over runtime programs.
- Separately configured agents, actual message wiring, parallel teams and async delegation provide broad composition.
- Compaction is an explicit ordered programmable pipeline rather than a single opaque summary toggle.

## Material limits

- Provider-loop internals, execution environment OS lifecycle and method_spawner internals are dependencies not audited here.
- Flow event emission is earlier than target work; cancellation does not await global cessation.
- Server compaction deletes previous durable conversation; successful final transcript persistence is not original full audit.
- Native wrapper confirmation/hooks apply to outer codemode tool, not inner raw FunctionTool calls.

## Arconaut design questions

- Can live object agency be retained while versioning changes for post-turn/workflow activation?
- How do consumer clients acquire stable external kernel/process IDs without inheriting service ownership?
- What immutable-original/context-view relationship prevents destructive compaction and supports model repair?
- Which cancellation acknowledgment proves provider and OS work absent before refit?

## Evidence

### Evidence pool

[quarantine/agentpool/src/agentpool/delegation/pool.py:99–148](../../../quarantine/agentpool/src/agentpool/delegation/pool.py#L99): Manifest creates typed agents, teams, shared storage/MCP/skills, jobs and process registry; pool owns these lifecycle managers.

### Evidence enter

[quarantine/agentpool/src/agentpool/delegation/pool.py:154–215](../../../quarantine/agentpool/src/agentpool/delegation/pool.py#L154): Pool enters managers and nodes, parallel-load optionally gathers; failed initialization cleans process manager and exit stack.

### Evidence add

[quarantine/agentpool/src/agentpool/delegation/pool.py:513–524](../../../quarantine/agentpool/src/agentpool/delegation/pool.py#L513): Programmatic add_agent enters its lifetime, attaches event handlers and shared MCP provider, then registers it.

### Evidence base

[quarantine/agentpool/src/agentpool/agents/base_agent.py:568–643](../../../quarantine/agentpool/src/agentpool/agents/base_agent.py#L568): run_stream initializes session, stores mutable cancellation/current-stream state, consumes queued prompts and loops subsequent queued turns.

### Evidence result

[quarantine/agentpool/src/agentpool/agents/base_agent.py:677–794](../../../quarantine/agentpool/src/agentpool/agents/base_agent.py#L677): One run consumes staged/pending context, emits user/result signals, applies hooks, stores final message and working history conditionally, then routes connections; errors emit failure.

### Evidence agentlet

[quarantine/agentpool/src/agentpool/agents/native_agent/agent.py:588–651](../../../quarantine/agentpool/src/agentpool/agents/native_agent/agent.py#L588): Each native run constructs a PydanticAI Agent with current model/settings/prompts/history processors and registers enabled wrapped tools.

### Evidence iterate

[quarantine/agentpool/src/agentpool/agents/native_agent/agent.py:653–743](../../../quarantine/agentpool/src/agentpool/agents/native_agent/agent.py#L653): Native wrapper iterates model/tool nodes, merges stream events and checks cancellation; cancelled result preserves text/usage but omits normal raw model messages.

### Evidence model

[quarantine/agentpool/src/agentpool/agents/native_agent/agent.py:768–781](../../../quarantine/agentpool/src/agentpool/agents/native_agent/agent.py#L768): set_model mutates model selection immediately; native interrupt cancels stream task without awaiting it.

### Evidence interrupt

[quarantine/agentpool/src/agentpool/agents/base_agent.py:969–982](../../../quarantine/agentpool/src/agentpool/agents/base_agent.py#L969): interrupt sets flag, awaits adapter interrupt method and emits signal, without a common provider/process quiescence barrier.

### Evidence confirm

[quarantine/agentpool/src/agentpool/agents/context.py:80–100](../../../quarantine/agentpool/src/agentpool/agents/context.py#L80): Native per_tool/never permits configured operations; other modes delegate confirmation to input provider.

### Evidence hooks

[quarantine/agentpool/src/agentpool/agents/native_agent/tool_wrapping.py:87–178](../../../quarantine/agentpool/src/agentpool/agents/native_agent/tool_wrapping.py#L87): Tool confirmation precedes pre-hook argument replacement; actual callable executes then post-hook context injection, with session_id None in hook events.

### Evidence inject

[quarantine/agentpool/src/agentpool/agents/native_agent/hook_manager.py:192–224](../../../quarantine/agentpool/src/agentpool/agents/native_agent/hook_manager.py#L192): Pending injected context is consumed and appended after a tool result.

### Evidence talk

[quarantine/agentpool/src/agentpool/talk/talk.py:198–349](../../../quarantine/agentpool/src/agentpool/talk/talk.py#L198): Connections filter/transform and queue or route run/context/forward messages; processed/forwarded events emit before target execution.

### Evidence team

[quarantine/agentpool/src/agentpool/delegation/team.py:85–169](../../../quarantine/agentpool/src/agentpool/delegation/team.py#L85): Team run_iter creates tasks for all members and queues results/errors; finally cancels unfinished tasks without awaiting them. Team store_history branch is pass.

### Evidence chain

[quarantine/agentpool/src/agentpool/delegation/teamrun.py:209–250](../../../quarantine/agentpool/src/agentpool/delegation/teamrun.py#L209): TeamRun wires queued pairwise connections, awaits first node and each triggered successor, disconnecting in finally.

### Evidence task

[quarantine/agentpool/src/agentpool_toolsets/builtin/subagent_tools.py:158–325](../../../quarantine/agentpool/src/agentpool_toolsets/builtin/subagent_tools.py#L158): Exposed list_available_nodes and task choose current pool agents/teams; background task returns identity/internal output path with in-process task retention.

### Evidence workers

[quarantine/agentpool/src/agentpool_toolsets/builtin/workers.py:22–121](../../../quarantine/agentpool/src/agentpool_toolsets/builtin/workers.py#L22): Configured worker tools ask_<name> can pass/reset history and await agent or team; native worker history restoration is finally protected.

### Evidence process

[quarantine/agentpool/src/agentpool_toolsets/builtin/execution_environment.py:34–185](../../../quarantine/agentpool/src/agentpool_toolsets/builtin/execution_environment.py#L34): Background process tools delegate start/output/wait to selected execution-environment process manager; identity, output truncation, elapsed time and exit code are formatted.

### Evidence kill

[quarantine/agentpool/src/agentpool_toolsets/builtin/execution_environment.py:204–266](../../../quarantine/agentpool/src/agentpool_toolsets/builtin/execution_environment.py#L204): kill/release/list delegate process lifecycle and emit status; dependency internals determine OS reaping and persistence.

### Evidence fsread

[quarantine/agentpool/src/agentpool_toolsets/fsspec_toolset/toolset.py:361–425](../../../quarantine/agentpool/src/agentpool_toolsets/fsspec_toolset/toolset.py#L361): File reads resolve environment path, read configured filesystem and return text or binary media with stream events.

### Evidence fswrite

[quarantine/agentpool/src/agentpool_toolsets/fsspec_toolset/toolset.py:499–577](../../../quarantine/agentpool/src/agentpool_toolsets/fsspec_toolset/toolset.py#L499): File writes enforce size/overwrite flag, append by read-rewrite and return diff/diagnostics; unreadable append source is ignored and replacement proceeds.

### Evidence fsedit

[quarantine/agentpool/src/agentpool_toolsets/fsspec_toolset/toolset.py:640–751](../../../quarantine/agentpool/src/agentpool_toolsets/fsspec_toolset/toolset.py#L640): edit delegates sequential smart-match replacements; matching failure returns before final file write.

### Evidence aggregate

[quarantine/agentpool/src/agentpool/resource_providers/aggregating.py:93–120](../../../quarantine/agentpool/src/agentpool/resource_providers/aggregating.py#L93): Aggregating provider refreshes tools and wraps them as one codemode tool when configured.

### Evidence codemode

[quarantine/agentpool/src/agentpool/resource_providers/codemode/provider.py:54–152](../../../quarantine/agentpool/src/agentpool/resource_providers/codemode/provider.py#L54): execute_tool runs model-authored async main in fresh exec namespace of tool callables; returns value/error, saves script/meta to internal memory filesystem, refreshes bindings each call.

### Evidence rawbindings

[quarantine/agentpool/src/agentpool/resource_providers/codemode/helpers.py:21–83](../../../quarantine/agentpool/src/agentpool/resource_providers/codemode/helpers.py#L21): Syntax/substrings validate program shape; generated namespace binds tool.get_callable directly, not native confirmation/hook wrappers.

### Evidence callable

[quarantine/agentpool/src/agentpool/tools/base.py:254–263](../../../quarantine/agentpool/src/agentpool/tools/base.py#L254): FunctionTool.get_callable returns original fn, making inner codemode calls distinct from native wrapped tool dispatch.

### Evidence debug

[quarantine/agentpool/src/agentpool_toolsets/builtin/debug.py:241–315](../../../quarantine/agentpool/src/agentpool_toolsets/builtin/debug.py#L241): execute_introspection exposes live ctx/run_ctx/me to exec and supports saved arbitrary objects across calls in provider-local namespace storage.

### Evidence logs

[quarantine/agentpool/src/agentpool_toolsets/builtin/debug.py:41–92](../../../quarantine/agentpool/src/agentpool_toolsets/builtin/debug.py#L41): Memory logging keeps newest1000 records by default and can be cleared; not durable or comprehensive original audit.

### Evidence pipeline

[quarantine/agentpool/src/agentpool/messaging/compaction.py:110–145](../../../quarantine/agentpool/src/agentpool/messaging/compaction.py#L110): Compaction pipelines compose ordered mutable steps with | and |=.

### Evidence truncate

[quarantine/agentpool/src/agentpool/messaging/compaction.py:320–353](../../../quarantine/agentpool/src/agentpool/messaging/compaction.py#L320): Tool-output compaction substitutes prefix-truncated content with notice.

### Evidence summary

[quarantine/agentpool/src/agentpool/messaging/compaction.py:511–551](../../../quarantine/agentpool/src/agentpool/messaging/compaction.py#L511): SummarizeOlderMessages uses separate model summarization and retains recent tail when message threshold exceeded.

### Evidence compact

[quarantine/agentpool/src/agentpool/messaging/compaction.py:645–697](../../../quarantine/agentpool/src/agentpool/messaging/compaction.py#L645): Compaction rebuilds ChatMessages from model parts with placeholder content/new IDs and replaces working history; no archive generated here.

### Evidence compactcmd

[quarantine/agentpool/src/agentpool_commands/pool.py:99–176](../../../quarantine/agentpool/src/agentpool_commands/pool.py#L99): /compact selects minimal/balanced/summarizing or configured pipeline and mutates current agent history; errors print.

### Evidence replace

[quarantine/agentpool/src/agentpool/storage/manager.py:529–603](../../../quarantine/agentpool/src/agentpool/storage/manager.py#L529): Stored-history replacement deletes session messages across providers then logs compacted records; per-provider deletion errors are swallowed/logged, no atomic multi-provider swap.

### Evidence servercompact

[quarantine/agentpool/src/agentpool_server/opencode_server/routes/session_routes.py:717–754](../../../quarantine/agentpool/src/agentpool_server/opencode_server/routes/session_routes.py#L717): OpenCode-compatible summary route actually replaces durable history with compacted history, clears session message list to summary and ignores compaction failure.

### Evidence sql

[quarantine/agentpool/src/agentpool_storage/sql_provider/sql_provider.py:104–142](../../../quarantine/agentpool/src/agentpool_storage/sql_provider/sql_provider.py#L104): SQL history query/filter and per-message committed records preserve serialized model messages and selected metadata; not all stream/process events.

### Evidence storage

[quarantine/agentpool/src/agentpool/storage/manager.py:208–267](../../../quarantine/agentpool/src/agentpool/storage/manager.py#L208): Storage routes configurable message/session logging and history-provider queries; session marked logged before providers complete. method_spawner dependency behavior untraced.

### Evidence acpinit

[quarantine/agentpool/src/agentpool/agents/acp_agent/acp_agent.py:329–380](../../../quarantine/agentpool/src/agentpool/agents/acp_agent/acp_agent.py#L329): ACP adapter owns a subprocess/stdio connection and creates remote session with configured MCP servers/capability filtering.

### Evidence acpturn

[quarantine/agentpool/src/agentpool/agents/acp_agent/acp_agent.py:435–537](../../../quarantine/agentpool/src/agentpool/agents/acp_agent/acp_agent.py#L435): ACP prompt runs as task, reconstructs streamed model parts and returns complete/partial ChatMessage; ephemeral run forks remote session.

### Evidence acpcancel

[quarantine/agentpool/src/agentpool/agents/acp_agent/acp_agent.py:544–571](../../../quarantine/agentpool/src/agentpool/agents/acp_agent/acp_agent.py#L544): ACP model changes delegate protocol; interrupt sends cancel notification then cancels local prompt/stream without awaiting remote completion.

### Evidence config

[quarantine/agentpool/src/agentpool_server/opencode_server/routes/config_routes.py:201–213](../../../quarantine/agentpool/src/agentpool_server/opencode_server/routes/config_routes.py#L201): Config PATCH only assigns fields to state.config and returns; it does not itself reconstruct agents or activate a turn program.

### Evidence schema

[quarantine/agentpool/src/agentpool_toolsets/config_creation.py:50–111](../../../quarantine/agentpool/src/agentpool_toolsets/config_creation.py#L50): create_config validates model-authored markup against schema and returns validation text, without persisting or activating configuration.

