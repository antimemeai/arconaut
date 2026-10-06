# nanobot

A multi-channel agent with checkpointed interruption/recovery, programmable host hooks and bounded model runtime self-control.

Role: persistent multi-channel Python agent. Runtime: Python.

Pinned source: [https://github.com/HKUDS/nanobot](https://github.com/HKUDS/nanobot); revision/version `d3d70f20655bce5d3c7a017bfbe803efe09a2438`.

Asyncio per-session admission/queues, immutable admitted provider runtime, shared hook/tool runner and managed process sessions.

Owns local session/memory/child/task state and process lifetimes; consumes providers/MCP/channel services, not a shared compute governor.

Inspection: Read entry/runner actual provider/tool/checkpoint flow, runtime control, process/session persistence, compaction, subagents, tools, channel reload and stop/reload tests.

Limits of this study: No source executed. Dream implementation, all channel/provider adapters and deployment restart orchestration not exhaustively traced; no universal absence claims.

## Actions

### Filesystem / search / apply_patch discovery

Surface: model tools.

Input: paths, lines/text/patch/search parameters

Result: content, changed file/error

Lifecycle: tool classes scoped by loader; per-request read state

Authority: model allowlisted FS/workspace/sandbox scope

Evidence: [loader](#evidence-loader), [fs](#evidence-fs), [write](#evidence-write).

### exec sessions / stdin / poll / terminate

Surface: model tools.

Input: command, cwd, timeout, existing session handle/characters

Result: bounded output, done/exit/time/handle

Lifecycle: owned ongoing process across actions; deadlines checked on poll; owner-scoped cleanup

Authority: model tool; process-tree option and host policy

Evidence: [process](#evidence-process), [poll](#evidence-poll), [kill](#evidence-kill), [owner](#evidence-owner).

### my runtime inspection/changes

Surface: model tool.

Input: allowlisted snapshot key/set value or model_preset

Result: snapshot or validated next-turn setting/error

Lifecycle: admitted request runtime immutable; modification default disabled

Authority: model with allow_set; core machinery deliberately blocked

Evidence: [my](#evidence-my), [myset](#evidence-myset), [runtime](#evidence-runtime).

### spawn and child result routing

Surface: model/host APIs.

Input: task, label, session origin, admitted runtime

Result: background task and parent notification

Lifecycle: semaphore admitted independent run; result injects before next request

Authority: model/host child tools limited by scope

Evidence: [child](#evidence-child), [announce](#evidence-announce), [runner](#evidence-runner).

### Compact / stop / checkpoint recovery

Surface: operator/host APIs.

Input: session/current work/custom summary

Result: summary boundary or completed partial context

Lifecycle: explicit stop differs from shutdown; compaction captures append boundary

Authority: operator/host runner

Evidence: [summary](#evidence-summary), [compact](#evidence-compact), [stop](#evidence-stop).

### search_sessions/read_session

Surface: model tools.

Input: session key/handle and literal query

Result: bounded visible message excerpts

Lifecycle: persisted session retrieval; current active session excluded

Authority: model with access store scope

Evidence: [readsession](#evidence-readsession).

### Host hooks and tool plugins/channel enable-disable

Surface: program API/operator feature control.

Input: Python hooks/entry-point classes or feature action

Result: modified requests/tool results; live channel clients

Lifecycle: loop hook phases, cached discovery and channel-specific reload

Authority: host programmer; model runtime control does not replace hooks

Evidence: [loader](#evidence-loader), [runner](#evidence-runner), [channel](#evidence-channel).

## Capabilities

### filesystem

**I — Files** (source): Scoped read/write/search/patch class discovery; actual write replaces text; request-local state.

Evidence: [fs](#evidence-fs), [write](#evidence-write), [loader](#evidence-loader).

### processes

**I — OS programs** (source): Ongoing owned piped program sessions, stdin/poll/terminate; decoded/truncated streams and kill timeout limits.

Evidence: [process](#evidence-process), [poll](#evidence-poll), [kill](#evidence-kill), [owner](#evidence-owner).

### code-actions

**S — Code actions** (source): Commands can compose programs through shell; Python host hooks/plugins are executable. No traced persistent model-authored kernel API.

Evidence: [loader](#evidence-loader), [runner](#evidence-runner), [process](#evidence-process).

### persistent-kernel

**? — Kernel** (inspection scope): Ongoing process sessions can run interpreters but no specific shared persistent namespace/checkpoint contract traced.

### standing-database

**L — Standing DB** (source): Owned structured sessions/cursors and Markdown/JSONL memories; model session snippets are narrow, no general standing SQL research table interface in inspected paths.

Evidence: [memory](#evidence-memory), [readsession](#evidence-readsession).

### workflow-programming

**S — Workflows** (source): AgentRunSpec injection/continuation/checkpoint callbacks and hook phases are programmable host machinery, plus scoped tool plugins.

Evidence: [runner](#evidence-runner), [loader](#evidence-loader), [loop](#evidence-loop).

### multi-model

**I — Models** (source): Immutable per-admission provider runtime, default/session presets and independently captured child runtime; no selection-to-collaboration inference.

Evidence: [runtime](#evidence-runtime), [myset](#evidence-myset), [child](#evidence-child).

### live-collaboration

**S — Peer chat** (source): Child completion bus into busy-parent next-request snapshots; peer-addressed ongoing room not established.

Evidence: [announce](#evidence-announce), [runner](#evidence-runner).

### concurrent-work

**I — Concurrency** (source): Independent child semaphore/task tracking and safe tool batch dispatch; multiple owned exec sessions across actions.

Evidence: [child](#evidence-child), [runner](#evidence-runner), [process](#evidence-process).

### steering-interrupt

**L — Steer/interrupt** (source): Queued injections before each provider request and explicit stop preserves partial results. Kill attempts bounded with suppressed failure/timeout, not guaranteed globally settled work.

Evidence: [runner](#evidence-runner), [stop](#evidence-stop), [kill](#evidence-kill).

### turn-redefinition

**S — Turn program** (source): Host hook/injection/continuation phases alter running loop; MyTool cannot replace core runner/hooks.

Evidence: [runner](#evidence-runner), [my](#evidence-my), [loop](#evidence-loop).

### compaction

**I — Compaction** (source): Managed summary replay boundary retains raw message transcript, captures concurrent append boundary and emits phase outcomes; old provider state cleared.

Evidence: [summary](#evidence-summary), [compact](#evidence-compact), [idlecompact](#evidence-idlecompact).

### context-repair

**S — Repair** (source): Retained prefix and checkpoint restore support host reconstruction; model read_session visible excerpts exclude active session and tool stream.

Evidence: [summary](#evidence-summary), [stop](#evidence-stop), [readsession](#evidence-readsession).

### original-audit

**L — Original audit** (source): Retained transcript is already normalized result data; bounded decoded process IO loses originals. Atomic replacement default not fsynced; not captured all original provider/program attempts.

Evidence: [runner](#evidence-runner), [process](#evidence-process), [poll](#evidence-poll), [persist](#evidence-persist).

### audit-query

**L — Audit query** (source): Model persisted visible user/assistant session retrieval is bounded and excludes active session; full original captured IO not available.

Evidence: [readsession](#evidence-readsession), [persist](#evidence-persist).

### hot-change

**I — Hot change** (source): Future-turn config/model admissions and live channel start/stop; cached tool discovery not unrestricted live replacement.

Evidence: [runtime](#evidence-runtime), [myset](#evidence-myset), [channel](#evidence-channel), [loader](#evidence-loader).

### rebuild-continuity

**S — Rebuild continuity** (source): Shutdown retains recovery checkpoint and persisted session; no outpost/native executable replacement or preservation of owned active process handles traced.

Evidence: [stop](#evidence-stop), [persist](#evidence-persist).

### remote-services

**S — Remote** (source): Tool loader admits MCP/channel services and hot channel clients; lower remote work settlement and shared-service orchestration outside inspected core.

Evidence: [loader](#evidence-loader), [channel](#evidence-channel).

### self-improvement

**? — Self-improve** (inspection scope): Memory Git/Dream groundwork inspected, but candidate executable-policy change/evaluate/promote machinery not traced; generic edits insufficient.

### complaints

**? — Complaints** (inspection scope): Inspected native tool/core paths do not establish universal model state-capturing grievance and separate complaint table.

### authority

**L — Authority** (source): Filesystem/workspace and scoped tool authority; model self-control enabled inspection but mutation false by default, core runner/hooks blocked.

Evidence: [fs](#evidence-fs), [my](#evidence-my), [myset](#evidence-myset).

### evaluation

**S — Evaluation** (source): Read literal stop checkpoint and event-gated channel start/stop oracles; mocks cannot establish actual provider/program settlement or crash persistence.

Evidence: [stoptest](#evidence-stoptest), [reloadtest](#evidence-reloadtest).

### time-order

**S — Time/order** (source): Owned exec deadlines/elapsed use monotonic; idle archival uses civil timestamps and compaction identity/boundary, no paused cross-process clock contract.

Evidence: [process](#evidence-process), [poll](#evidence-poll), [idlecompact](#evidence-idlecompact), [compact](#evidence-compact).

## Inspected test oracles

- [quarantine/nanobot/tests/agent/test_stop_preserves_context.py](../../../quarantine/nanobot/tests/agent/test_stop_preserves_context.py): partial work preservation on cancellation Oracle: Exact user/assistant/tool sequence and checkpoint clearing/save invoked; mocked persistence/provider, no crash durability proof. Read, **not executed**.
- [quarantine/nanobot/tests/channels/test_channel_manager_hot_reload.py](../../../quarantine/nanobot/tests/channels/test_channel_manager_hot_reload.py): live owned-channel start/stop Oracle: Wait for start event, exact stopped/removal/no restart; fake channel, not all adapter replacement. Read, **not executed**.

## Useful mechanisms

- Explicit stop/shutdown recovery distinction with direct checkpoint-content oracle.
- Immutable admitted provider runtime separates active from future-turn changes.
- Failed owner cleanup reinserts handle custody rather than forgetting work.

## Material limits

- MyTool mutation opt-in/allowlisted; core hooks/runtime not model-replaceable through it.
- Selective bounded transformed transcripts and IO cannot serve comprehensive audit.
- Native refit/outpost and peer room not established.

## Arconaut design questions

- Specify checkpoint treatment for explicit interrupt, failed workflow and replacement shutdown separately.
- Make refit require observed settlement rather than a sent kill/cancel signal.
- Preserve model program agency while staging definition changes after affected workflows conclude.

## Evidence

### Evidence loop

[quarantine/nanobot/nanobot/agent/loop.py:2155–2193](../../../quarantine/nanobot/nanobot/agent/loop.py#L2155): Actual loop runs shared agent engine and records messages/provider compaction/checkpoints and continuation.

### Evidence runner

[quarantine/nanobot/nanobot/agent/runner.py:432–549](../../../quarantine/nanobot/nanobot/agent/runner.py#L432): Finite inbox snapshot before model request, hooks, assistant checkpoint then batched tool execution and normalized results.

### Evidence runtime

[quarantine/nanobot/nanobot/agent/loop.py:520–605](../../../quarantine/nanobot/nanobot/agent/loop.py#L520): Config invalidation and default/session model choices apply to immutable future-turn admissions.

### Evidence my

[quarantine/nanobot/nanobot/agent/tools/self.py:30–104](../../../quarantine/nanobot/nanobot/agent/tools/self.py#L30): MyTool enabled but modification default false, narrow adapter blocks core runner/session/hook/authority replacement.

### Evidence myset

[quarantine/nanobot/nanobot/agent/tools/self.py:500–555](../../../quarantine/nanobot/nanobot/agent/tools/self.py#L500): Session model preset selected for next turn; direct instance-wide model/window changes refused during active session.

### Evidence loader

[quarantine/nanobot/nanobot/agent/tools/loader.py:36–111](../../../quarantine/nanobot/nanobot/agent/tools/loader.py#L36): Cached built-in package/entry-point tool discovery, scoped create/enabled checks and native name precedence.

### Evidence fs

[quarantine/nanobot/nanobot/agent/tools/filesystem.py:35–104](../../../quarantine/nanobot/nanobot/agent/tools/filesystem.py#L35): Native FS tool class permissions separate read/write allowed paths with request-scoped file state.

### Evidence write

[quarantine/nanobot/nanobot/agent/tools/filesystem.py:550–589](../../../quarantine/nanobot/nanobot/agent/tools/filesystem.py#L550): Native write_file resolves write path then replaces file text and clears read state.

### Evidence process

[quarantine/nanobot/nanobot/agent/tools/exec_session.py:118–161](../../../quarantine/nanobot/nanobot/agent/tools/exec_session.py#L118): Owned process sessions use monotonic start/deadline, separate bounded decoded streams; UTF-8 errors replaced.

### Evidence poll

[quarantine/nanobot/nanobot/agent/tools/exec_session.py:185–262](../../../quarantine/nanobot/nanobot/agent/tools/exec_session.py#L185): Polling yields output, timeout kills, checks exit, drains output with limits; kill waits on stream tasks with suppressed timeout.

### Evidence kill

[quarantine/nanobot/nanobot/agent/tools/shell.py:711–763](../../../quarantine/nanobot/nanobot/agent/tools/shell.py#L711): Root/process-group or Windows job-tree kill and bounded awaited root exit; suppressed timeout/errors limit guaranteed settlement.

### Evidence owner

[quarantine/nanobot/nanobot/agent/tools/exec_session.py:407–432](../../../quarantine/nanobot/nanobot/agent/tools/exec_session.py#L407): Owner-scoped termination reinserts sessions on kill exceptions, avoiding loss of failed handle custody.

### Evidence stop

[quarantine/nanobot/nanobot/agent/loop.py:1579–1608](../../../quarantine/nanobot/nanobot/agent/loop.py#L1579): Explicit stop restores runtime checkpoint completed results; gateway shutdown keeps durable checkpoint untouched.

### Evidence summary

[quarantine/nanobot/nanobot/session/manager.py:219–279](../../../quarantine/nanobot/nanobot/session/manager.py#L219): Summary checkpoint changes replay boundary while retaining message transcript, legal tool-result start.

### Evidence compact

[quarantine/nanobot/nanobot/agent/memory.py:1241–1319](../../../quarantine/nanobot/nanobot/agent/memory.py#L1241): Compaction serializes captured boundary, preserves concurrent appends, clears provider replay state, emits phase outcomes.

### Evidence idlecompact

[quarantine/nanobot/nanobot/agent/autocompact.py:63–110](../../../quarantine/nanobot/nanobot/agent/autocompact.py#L63): Idle archival skips active keys and emits failure handling; TTL uses civil wall time.

### Evidence persist

[quarantine/nanobot/nanobot/session/manager.py:1207–1235](../../../quarantine/nanobot/nanobot/session/manager.py#L1207): Session JSONL full-state atomic replacement, optional fsync default false and checkpoint overlay removed after save.

### Evidence readsession

[quarantine/nanobot/nanobot/agent/tools/sessions.py:164–228](../../../quarantine/nanobot/nanobot/agent/tools/sessions.py#L164): Model reads bounded visible user/assistant sessions with snippets, excludes active session key; no raw tool/provider stream query.

### Evidence child

[quarantine/nanobot/nanobot/agent/subagent.py:139–166](../../../quarantine/nanobot/nanobot/agent/subagent.py#L139): Each child captures explicit runtime; semaphore and task maps plus isolated owned exec manager.

### Evidence announce

[quarantine/nanobot/nanobot/agent/subagent.py:527–569](../../../quarantine/nanobot/nanobot/agent/subagent.py#L527): Child completion routed to parent pending injection queue via system bus rather than competing session task.

### Evidence memory

[quarantine/nanobot/nanobot/agent/memory.py:59–90](../../../quarantine/nanobot/nanobot/agent/memory.py#L59): Standing memory is Markdown/JSONL with cursor and Git tracking, not an exposed SQL research store.

### Evidence channel

[quarantine/nanobot/nanobot/channels/manager.py:444–501](../../../quarantine/nanobot/nanobot/channels/manager.py#L444): Hot channel actions reload config and start/stop owned channel clients; always-enabled feature requires restart.

### Evidence stoptest

[quarantine/nanobot/tests/agent/test_stop_preserves_context.py:46–116](../../../quarantine/nanobot/tests/agent/test_stop_preserves_context.py#L46): Injected cancellation oracle checks exact roles/tool result recovery, checkpoint cleared and save invoked; fake IO does not establish actual crash durability.

### Evidence reloadtest

[quarantine/nanobot/tests/channels/test_channel_manager_hot_reload.py:113–147](../../../quarantine/nanobot/tests/channels/test_channel_manager_hot_reload.py#L113): Gated fake channel start/stop oracle asserts no restart required and channel removed/stopped.

