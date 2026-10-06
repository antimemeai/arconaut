# zeroclaw

Real model pipeline tool and peer-directed recipient turns; config-generation boundaries and live plugin config are useful, while terminal cancellation can precede worker shutdown.

Role: native persistent Rust agent with control plane. Runtime: Rust.

Pinned source: [https://github.com/zeroclaw-labs/zeroclaw](https://github.com/zeroclaw-labs/zeroclaw); revision/version `83f0ff3805ad2f2535d63db2fa85a72c02f5dd05`.

Current split crates runtime: Tokio turn engine with mutable request hooks, concurrent tools, named agent/delegate tasks, WASM tool plugins and configurable memory.

Owns task control plane, sessions/processes and runtime workspace; consumes providers/MCP/plugins/remote channels. This fabric ownership exceeds Arconaut role.

Inspection: Traced current crate paths (src reexports): shell/drains/kill guard, turn hooks/steering, pipelines, peer admission/delivery, delegate cancellation, live config and persistence/overflow.

Limits of this study: Source only; no acquired code executed. SOP full lifecycle, all memory/provider/plugin backends, every RPC refresh and archive retention not exhaustively traced.

## Actions

### shell

Surface: model tool.

Input: command/cwd

Result: capped stdout/stderr, status/error

Lifecycle: own process group, root wait success; timeout KILL request/drop rather than awaited settlement

Authority: configured security/runtime workspace

Evidence: [shell](#evidence-shell), [guard](#evidence-guard).

### execute_pipeline

Surface: model tool.

Input: ordered tool/args, interpolation, parallel, all/last

Result: ordered step outputs or first failure

Lifecycle: full prevalidation, serial dependence or parallel task set; no effect rollback

Authority: unrestricted principal and global/caller allowlists

Evidence: [pipeline](#evidence-pipeline), [parallel](#evidence-parallel), [surface](#evidence-surface).

### send_message_to_peer

Surface: model tool.

Input: configured target/channel/message

Result: accepted inbox task ID or external delivery result

Lifecycle: durable admission precedes separate recipient turn; not resident room injection

Authority: resolved peer set, sender principal and target admission

Evidence: [tools](#evidence-tools), [peer-admit](#evidence-peer-admit), [peer-turn](#evidence-peer-turn), [oneshot](#evidence-oneshot).

### delegate check/await/list/cancel

Surface: model tool.

Input: named target/prompt/task ID

Result: provider result or visible task status/output

Lifecycle: sync/parallel/background configured branches; terminal cancellation may precede shutdown

Authority: originator-chain visibility; target model/profile/authority

Evidence: [delegate](#evidence-delegate), [terminal-before-cancel](#evidence-terminal-before-cancel), [visibility](#evidence-visibility), [legacy-cancel](#evidence-legacy-cancel).

### Model/prompt/request/tool hooks

Surface: extension API.

Input: native handler and mutable messages/model/name/arguments

Result: rewritten request or cancel

Lifecycle: per request hooks; panic retains prior values, no arbitrary whole-loop replacement

Authority: registered host extension handlers

Evidence: [hook](#evidence-hook), [prompt-hook](#evidence-prompt-hook).

### Live generation/plugin configuration

Surface: operator/host API.

Input: canonical supported config change

Result: next-turn route/limits generation, per-call plugin values

Lifecycle: turn generation stable; plugin live value can change per action; ACP/WS may stay construction-pinned

Authority: host live config authority, feature-gated component registration

Evidence: [generation](#evidence-generation), [plugin-wire](#evidence-plugin-wire), [plugin-test](#evidence-plugin-test).

### Knowledge graph action family

Surface: model tool.

Input: capture/search/relate/lessons/graph metadata

Result: stored/retrieved expertise and graph data

Lifecycle: standing service tool dispatcher; backend persistence not established here

Authority: model knowledge tool scope

Evidence: [knowledge](#evidence-knowledge).

## Capabilities

### filesystem

**S — Files** (source): Shell ordinary programs and runtime tool registration; file operations not all traced.

Evidence: [shell](#evidence-shell), [tools](#evidence-tools).

### processes

**I — OS programs** (source): Own process groups and concurrent bounded pipe drains; root-success wait distinct from destructive timeout.

Evidence: [shell](#evidence-shell), [guard](#evidence-guard).

### code-actions

**I — Code actions** (source): Model declarative pipeline combines registered tools and shell programs; not persistent Python namespace.

Evidence: [pipeline](#evidence-pipeline), [parallel](#evidence-parallel), [shell](#evidence-shell).

### persistent-kernel

**? — Kernel** (inspection scope): Not established beyond inspected turn, shell, pipeline, peer, config, delegate and persistence paths.

### standing-database

**S — Standing DB** (source): Model knowledge graph capture/search dispatcher and durable task control plane; general model SQL DB/complete storage backend not traced.

Evidence: [knowledge](#evidence-knowledge), [peer-admit](#evidence-peer-admit).

### workflow-programming

**I — Workflows** (source): Actual sequential interpolation/parallel pipeline and mutable request hooks; unrestricted principal required for pipeline.

Evidence: [pipeline](#evidence-pipeline), [parallel](#evidence-parallel), [surface](#evidence-surface), [hook](#evidence-hook).

### multi-model

**I — Models** (source): Named target model/provider calls and request-hook model choice; live route/limit generation coupled.

Evidence: [delegate](#evidence-delegate), [hook](#evidence-hook), [generation](#evidence-generation).

### live-collaboration

**L — Peer chat** (source): Actual configured peer messages launch separate attributed recipient turns; one-shot history differs from resident multi-peer room/busy delivery.

Evidence: [peer-admit](#evidence-peer-admit), [peer-turn](#evidence-peer-turn), [oneshot](#evidence-oneshot).

### concurrent-work

**I — Concurrency** (source): Pipeline parallel task set, detached recipient turns and configured background delegates.

Evidence: [parallel](#evidence-parallel), [peer-turn](#evidence-peer-turn), [terminal-before-cancel](#evidence-terminal-before-cancel).

### steering-interrupt

**L — Steer/interrupt** (source): Between-iteration steering and token select; durable terminal status can be written before token cancellation and no worker join, shell timeout reports killed before confirmed reap.

Evidence: [steer](#evidence-steer), [tool-cancel](#evidence-tool-cancel), [terminal-before-cancel](#evidence-terminal-before-cancel), [shell](#evidence-shell).

### turn-redefinition

**S — Turn program** (source): Complete model-request messages/model mutable and structured tool pipelines; host compiled loop still fixed, no proven model-owned hot turn replacement.

Evidence: [hook](#evidence-hook), [pipeline](#evidence-pipeline), [prompt-hook](#evidence-prompt-hook).

### compaction

**I — Compaction** (source): Reactive overflow drops whole turns preserving structural pairing and breadcrumb; request accounting limitations explicit.

Evidence: [trim](#evidence-trim).

### context-repair

**L — Repair** (source): Current projection saved and discarded output not duplicated; no retained-original repair operation established in these inspected paths.

Evidence: [save](#evidence-save), [truncate](#evidence-truncate), [trim](#evidence-trim).

### original-audit

**L — Original audit** (source): Provider failure event explicitly omits prompt/completion; current history overwritten and tool truncation discards originals. Task terminal ledger not full original IO audit.

Evidence: [failure-audit](#evidence-failure-audit), [save](#evidence-save), [truncate](#evidence-truncate), [peer-turn](#evidence-peer-turn).

### audit-query

**S — Audit query** (source): Model task checks expose visible durable lifecycle and output; raw originals lacking, knowledge search separate.

Evidence: [visibility](#evidence-visibility), [terminal-before-cancel](#evidence-terminal-before-cancel), [knowledge](#evidence-knowledge).

### hot-change

**I — Hot change** (source): Supported live sessions adopt coupled config generation at turn boundary; WASM tool live config tested per invocation; ACP/WS construction-pinned caveat.

Evidence: [generation](#evidence-generation), [plugin-wire](#evidence-plugin-wire), [plugin-test](#evidence-plugin-test).

### rebuild-continuity

**L — Rebuild continuity** (source): Task/control-plane identities and projection files do not establish compiled outpost refit with settled programs/provider calls.

Evidence: [peer-admit](#evidence-peer-admit), [terminal-before-cancel](#evidence-terminal-before-cancel), [save](#evidence-save).

### remote-services

**S — Remote** (source): Model provider/peer external channel consumers; full external standing-compute namespace service not established.

Evidence: [delegate](#evidence-delegate), [peer-turn](#evidence-peer-turn).

### self-improvement

**S — Self-improve** (source): Knowledge lessons extraction, mutable program hooks/pipelines can support learning; independent autoresearch promotion oracle not traced.

Evidence: [knowledge](#evidence-knowledge), [hook](#evidence-hook), [pipeline](#evidence-pipeline).

### complaints

**? — Complaints** (inspection scope): Not established beyond inspected turn, shell, pipeline, peer, config, delegate and persistence paths.

### authority

**I — Authority** (source): Principal/allowlist pipeline preflight and originator-chain task control; actual model agency configurable and policies broader than blanket command approval.

Evidence: [surface](#evidence-surface), [pipeline](#evidence-pipeline), [visibility](#evidence-visibility), [tools](#evidence-tools).

### evaluation

**S — Evaluation** (source): Real constructor/component before/after literal config oracle and exact durable-write-before-cancel token tests; neither full process/provider settlement proof.

Evidence: [plugin-test](#evidence-plugin-test), [cancel-test](#evidence-cancel-test).

### time-order

**S — Time/order** (source): Tokio elapsed deadlines and durable admitted task identity, turn config-generation boundaries; paused logical time not established.

Evidence: [shell](#evidence-shell), [generation](#evidence-generation), [peer-admit](#evidence-peer-admit).

## Inspected test oracles

- [quarantine/zeroclaw/crates/zeroclaw-runtime/src/agent/plugin_live_config.rs](../../../quarantine/zeroclaw/crates/zeroclaw-runtime/src/agent/plugin_live_config.rs): live plugin config wiring Oracle: Actual Agent constructor/component execution exact before/after values without registry rebuild; not run. Read, **not executed**.
- [quarantine/zeroclaw/crates/zeroclaw-runtime/src/tools/delegate.rs](../../../quarantine/zeroclaw/crates/zeroclaw-runtime/src/tools/delegate.rs): durable cancellation transition Oracle: Token remains uncancelled if store fails, changes only after terminal write; no actual worker liveness oracle; not run. Read, **not executed**.

## Useful mechanisms

- Coupled route/limits config generations adopted at supported turn boundary.
- Actual model pipeline interpreter with complete step preflight.
- Peer acceptance and task registration accurately distinguished from delivered final output.

## Material limits

- Durable Cancelled transition can precede cancellation signal and confirmed worker settlement.
- Peer tool recipient path is a fresh one-shot turn, not live resident room context.
- Trimming/current-history overwrites and bounded failure events cannot supply original audit.

## Arconaut design questions

- Retain config-generation coherence while applying changes after affected workflows conclude.
- Keep cancellation requested/draining/settled state separate from terminal result visibility.
- Let workflows hide intermediate blobs from active context while retaining originals for audit.

## Evidence

### Evidence guard

[quarantine/zeroclaw/crates/zeroclaw-runtime/src/tools/shell.rs:18–59](../../../quarantine/zeroclaw/crates/zeroclaw-runtime/src/tools/shell.rs#L18): Drop sends process group KILL best-effort; disarmed after root wait.

### Evidence shell

[quarantine/zeroclaw/crates/zeroclaw-runtime/src/tools/shell.rs:339–417](../../../quarantine/zeroclaw/crates/zeroclaw-runtime/src/tools/shell.rs#L339): Concurrent capped stdout/stderr drains; timeout start_kill then drains abort, no awaited root death in timeout branch.

### Evidence tools

[quarantine/zeroclaw/crates/zeroclaw-runtime/src/tools/mod.rs:1412–1432](../../../quarantine/zeroclaw/crates/zeroclaw-runtime/src/tools/mod.rs#L1412): Actual model spawn/peer/model routing/switch tool registration.

### Evidence pipeline

[quarantine/zeroclaw/crates/zeroclaw-tools/src/pipeline.rs:124–185](../../../quarantine/zeroclaw/crates/zeroclaw-tools/src/pipeline.rs#L124): Validates every step allowlist/registry before effects; sequential outputs interpolate into later arguments.

### Evidence parallel

[quarantine/zeroclaw/crates/zeroclaw-tools/src/pipeline.rs:188–254](../../../quarantine/zeroclaw/crates/zeroclaw-tools/src/pipeline.rs#L188): Parallel JoinSet tools sorted by input index; early failure drops pending tasks, not foreign effects rollback.

### Evidence surface

[quarantine/zeroclaw/crates/zeroclaw-tools/src/pipeline.rs:257–272](../../../quarantine/zeroclaw/crates/zeroclaw-tools/src/pipeline.rs#L257): Model pipeline requires unrestricted principal; all/last output modes.

### Evidence peer-admit

[quarantine/zeroclaw/crates/zeroclaw-runtime/src/tools/send_message_to_peer.rs:240–307](../../../quarantine/zeroclaw/crates/zeroclaw-runtime/src/tools/send_message_to_peer.rs#L240): Durable inbox task registration required before success/admitted recipient execution.

### Evidence peer-turn

[quarantine/zeroclaw/crates/zeroclaw-runtime/src/tools/send_message_to_peer.rs:318–380](../../../quarantine/zeroclaw/crates/zeroclaw-runtime/src/tools/send_message_to_peer.rs#L318): Spawn separate recipient turn with sender principal, capture cost context, settle task; success reports acceptance.

### Evidence oneshot

[quarantine/zeroclaw/crates/zeroclaw-runtime/src/agent/loop_.rs:3937–3952](../../../quarantine/zeroclaw/crates/zeroclaw-runtime/src/agent/loop_.rs#L3937): Recipient process_message builds one-shot system+user history, not shared resident IRC context.

### Evidence steer

[quarantine/zeroclaw/crates/zeroclaw-runtime/src/agent/turn/mod.rs:1292–1319](../../../quarantine/zeroclaw/crates/zeroclaw-runtime/src/agent/turn/mod.rs#L1292): Steering drained between iterations and appended; cancellation boundary check. Gate/Annotate policy branches still TODO.

### Evidence hook

[quarantine/zeroclaw/crates/zeroclaw-runtime/src/agent/turn/mod.rs:1458–1490](../../../quarantine/zeroclaw/crates/zeroclaw-runtime/src/agent/turn/mod.rs#L1458): Actual pre-LLM hook mutates complete request messages/model; suffix cannot be assumed for arbitrary transforms.

### Evidence prompt-hook

[quarantine/zeroclaw/crates/zeroclaw-runtime/src/hooks/runner.rs:294–350](../../../quarantine/zeroclaw/crates/zeroclaw-runtime/src/hooks/runner.rs#L294): Model/profile/prompt hooks return replacement or cancel; panic retains prior values.

### Evidence tool-cancel

[quarantine/zeroclaw/crates/zeroclaw-runtime/src/agent/tool_execution.rs:279–299](../../../quarantine/zeroclaw/crates/zeroclaw-runtime/src/agent/tool_execution.rs#L279): Token select drops active tool future; no universal external-effect settlement contract.

### Evidence delegate

[quarantine/zeroclaw/crates/zeroclaw-runtime/src/tools/delegate.rs:2880–2908](../../../quarantine/zeroclaw/crates/zeroclaw-runtime/src/tools/delegate.rs#L2880): Named non-agentic provider/model call bounded by await timeout, not provider-side termination.

### Evidence terminal-before-cancel

[quarantine/zeroclaw/crates/zeroclaw-runtime/src/tools/delegate.rs:2213–2231](../../../quarantine/zeroclaw/crates/zeroclaw-runtime/src/tools/delegate.rs#L2213): Durable terminal transition wins BEFORE cancellation token signalled; returned aborted means token existed, not completed worker.

### Evidence visibility

[quarantine/zeroclaw/crates/zeroclaw-runtime/src/tools/delegate.rs:2234–2256](../../../quarantine/zeroclaw/crates/zeroclaw-runtime/src/tools/delegate.rs#L2234): Delegate task visibility bound to originator chain; legacy terminal artifacts readable but not mutable.

### Evidence legacy-cancel

[quarantine/zeroclaw/crates/zeroclaw-runtime/src/tools/delegate.rs:4266–4285](../../../quarantine/zeroclaw/crates/zeroclaw-runtime/src/tools/delegate.rs#L4266): Legacy token.cancel then file status Cancelled and claims aborted, without task join.

### Evidence cancel-test

[quarantine/zeroclaw/crates/zeroclaw-runtime/src/tools/delegate.rs:5996–6045](../../../quarantine/zeroclaw/crates/zeroclaw-runtime/src/tools/delegate.rs#L5996): Exact token remains active on store failure and cancelled only after terminal write; does not prove worker termination.

### Evidence generation

[quarantine/zeroclaw/crates/zeroclaw-runtime/src/agent/agent.rs:1664–1688](../../../quarantine/zeroclaw/crates/zeroclaw-runtime/src/agent/agent.rs#L1664): Turn boundary config generation stable for route/limits; direct ACP/WS construction stays pinned until reconnect.

### Evidence plugin-wire

[quarantine/zeroclaw/crates/zeroclaw-runtime/src/tools/mod.rs:2429–2458](../../../quarantine/zeroclaw/crates/zeroclaw-runtime/src/tools/mod.rs#L2429): Actual feature-gated WASM plugin host receives live config handle.

### Evidence plugin-test

[quarantine/zeroclaw/crates/zeroclaw-runtime/src/agent/plugin_live_config.rs:258–315](../../../quarantine/zeroclaw/crates/zeroclaw-runtime/src/agent/plugin_live_config.rs#L258): Real Agent construction and plugin execution observe changed config via same tool instance, literal before/after oracle.

### Evidence trim

[quarantine/zeroclaw/crates/zeroclaw-runtime/src/agent/turn/context_recovery.rs:85–125](../../../quarantine/zeroclaw/crates/zeroclaw-runtime/src/agent/turn/context_recovery.rs#L85): Context overflow drops oldest whole turns, token count excludes schemas/images in reactive path; inserts breadcrumb.

### Evidence failure-audit

[quarantine/zeroclaw/crates/zeroclaw-runtime/src/agent/turn/context_recovery.rs:21–55](../../../quarantine/zeroclaw/crates/zeroclaw-runtime/src/agent/turn/context_recovery.rs#L21): Provider failure events sanitize error, explicitly capture no prompt/completion content.

### Evidence save

[quarantine/zeroclaw/crates/zeroclaw-runtime/src/agent/history.rs:599–612](../../../quarantine/zeroclaw/crates/zeroclaw-runtime/src/agent/history.rs#L599): Interactive JSON writes current projection directly, not append-only original archive.

### Evidence truncate

[quarantine/zeroclaw/crates/zeroclaw-runtime/src/agent/history.rs:100–141](../../../quarantine/zeroclaw/crates/zeroclaw-runtime/src/agent/history.rs#L100): Truncation reports loss without retaining second original copy.

### Evidence knowledge

[quarantine/zeroclaw/crates/zeroclaw-tools/src/knowledge_tool.rs:138–153](../../../quarantine/zeroclaw/crates/zeroclaw-tools/src/knowledge_tool.rs#L138): Model knowledge capture/search/relate/lessons/graph/interaction actions dispatcher; storage backend not traced.

