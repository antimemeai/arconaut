# openclaw

A large extensible agent gateway with executable code actions, addressed session steering, configurable context engines and explicit ownership of parked continuations. These are scoped mechanisms, not a complete original audit or compiled refit protocol.

Role: persistent multi-channel agent gateway. Runtime: TypeScript, JavaScript.

Pinned source: [https://github.com/openclaw/openclaw](https://github.com/openclaw/openclaw); revision/version `646c42b4c630d1c01910eda96172f2a1da020224`.

Node host, native worker-backed JS cells, embedded agent sessions and owned process supervisors

Gateway owns agent/session/process policy and plugin lifetimes; consumes model, ACP, MCP and paired-node services, and can own sandbox containers

Inspection: Read executable cell dispatch/worker/owner cleanup, exposed file/process actions, session spawning and busy-target delivery, context-engine request assembly, transcript branching/compaction and bounded history/search diagnostics; inspect continuation and compaction tests.

Limits of this study: No reference execution. This is a bounded trace of a very large pinned tree; not all channels, sandbox/remote backends, plugins, database retention or restart transactions were traced. No assumed feature credit from adjacent products or archived instructions.

## Actions

### read/list/edit/write/apply_patch

Surface: model tools.

Input: path, range or patch under selected workspace

Result: bounded file content, edits or errors

Lifecycle: ordinary tool execution; host or sandbox bridge

Authority: model/program subject to standing effective workspace and read-only policy

Evidence: [files](#evidence-files).

### exec + process follow-up

Surface: model tools.

Input: command/env/workdir/PTY/yield; session ID and poll/log/input/kill action

Result: exit outcome or live process handle; later output/input/termination request

Lifecycle: background keeps supervisor owner after tool observer ends; final settlement callback precedes notification

Authority: model/program permitted process scope; borrowed policy does not borrow ownership

Evidence: [process](#evidence-process), [kill](#evidence-kill), [settlement](#evidence-settlement), [scope](#evidence-scope).

### code-mode exec/wait

Surface: model code action.

Input: JS source; executor and bounded catalog/namespace config; parked run handle

Result: incremental output, final JSON or waiting continuation and saved-result descriptors

Lifecycle: fresh namespace per new exec, same live cell across waits; close joins/disposes and retains failed cleanup

Authority: model executes enabled nested tools under ordinary policy/approvals

Evidence: [execution](#evidence-execution), [executor](#evidence-executor), [cell](#evidence-cell), [owner](#evidence-owner), [guidance](#evidence-guidance).

### results.save/load/delete

Surface: code-action API.

Input: JSON value or scoped result ID

Result: full stored JSON or bounded descriptor/error

Lifecycle: run/catalog lifetime, 64 entry and byte cap; abort/restriction clears store

Authority: same admitted run/catalog; old cell cannot adopt replacement store

Evidence: [results](#evidence-results).

### sessions_spawn

Surface: model tool.

Input: task/model/thinking/agent/run-or-session/fork-or-isolated/timeout/cleanup

Result: accepted child identity or explicit forbidden/error

Lifecycle: ACP or embedded session launched with captured parent identity; acceptance is not completion

Authority: model under configured spawn and inherited capability boundary

Evidence: [spawn](#evidence-spawn), [spawn-dispatch](#evidence-spawn-dispatch).

### sessions_send steer/notify/followup

Surface: model tool.

Input: target session incarnation, addressed text and delivery mode

Result: queued notification or accepted/steered run identity; later settlement distinct

Lifecycle: notify does not start run; active steer has receiver custody; absent active steer errors

Authority: model/operator under session visibility and exact caller/target mutation authority

Evidence: [delivery](#evidence-delivery), [custody](#evidence-custody), [notification](#evidence-notification).

### sessions_history + sessions_search

Surface: model tools.

Input: visible session, original message anchor, paging/includeTools or query

Result: bounded/redacted history, snippets/IDs, indexing/excluded archive warnings

Lifecycle: gateway query with captured session access; not complete wire audit

Authority: model under scoped session visibility

Evidence: [history](#evidence-history), [history-api](#evidence-history-api), [search](#evidence-search), [search-dispatch](#evidence-search-dispatch).

### context engine assemble

Surface: programmable extension API.

Input: active messages/tools/model/token budget/read fence

Result: selected messages, prompt authority and system addition

Lifecycle: actual request path adopts selection; exception falls back to pipeline

Authority: host configured extension; not proven arbitrary model-installed turn loop

Evidence: [context](#evidence-context).

### plugin/channel refresh

Surface: host lifecycle.

Input: pending plugin refresh or channel reload generation

Result: cleanup/refresh outcome or deferred/reloaded channels

Lifecycle: cell lease prevents premature plugin release; channel lease/timeout escape can reload while other work remains

Authority: host/operator control plane

Evidence: [owner](#evidence-owner), [reload](#evidence-reload).

## Capabilities

### filesystem

**I — Files** (source): Exposed host/sandbox read/list/edit/write/patch tools; writes depend on effective workspace policy.

Evidence: [files](#evidence-files).

### processes

**I — OS programs** (source): Exec yields owned process handles; process follow-up supports inspection/input/termination request. Background effects outlive a completed tool observer deliberately.

Evidence: [process](#evidence-process), [kill](#evidence-kill), [settlement](#evidence-settlement), [scope](#evidence-scope).

### code-actions

**I — Code actions** (source): Model JS composes discovered tools and parallel calls with exec/wait; Node or plugin executors own parked continuation and bridge calls.

Evidence: [execution](#evidence-execution), [executor](#evidence-executor), [cell](#evidence-cell), [owner](#evidence-owner), [guidance](#evidence-guidance).

### persistent-kernel

**L — Kernel** (source): A single parked JS cell preserves its live context across waits, but a new exec creates a fresh namespace; no serialized VM image or shared persistent computational kernel established.

Evidence: [cell](#evidence-cell), [continuation](#evidence-continuation), [results](#evidence-results).

### standing-database

**S — Standing DB** (source): Bounded JSON result references and scoped transcript query/search are actual structured state, but do not establish a writable shared research SQL database.

Evidence: [results](#evidence-results), [history-api](#evidence-history-api), [search-dispatch](#evidence-search-dispatch).

### workflow-programming

**S — Workflows** (source): Executable JS can compose tools/conditional discovery and child work; conditional swarm surface is advertised. Durable arbitrary workflow replay was not traced.

Evidence: [execution](#evidence-execution), [guidance](#evidence-guidance).

### multi-model

**I — Models** (source): Spawn accepts and forwards child model/thinking overrides alongside isolated/fork context and agent identity.

Evidence: [spawn](#evidence-spawn), [spawn-dispatch](#evidence-spawn-dispatch).

### live-collaboration

**I — Peer chat** (source): Addressed active-session steering has incarnation checks, original-input recording and receiver-owned accepted-message custody; notification and new-turn/spawn admission are separate operations.

Evidence: [delivery](#evidence-delivery), [custody](#evidence-custody), [notification](#evidence-notification), [spawn-dispatch](#evidence-spawn-dispatch).

### concurrent-work

**I — Concurrency** (source): Owned background programs, independently spawned sessions and concurrent bridged code calls have handles and settlement paths. No universal external-effect quiescence claimed.

Evidence: [process](#evidence-process), [spawn-dispatch](#evidence-spawn-dispatch), [owner](#evidence-owner).

### steering-interrupt

**L — Steer/interrupt** (source): Busy-target steering acceptance and later input settlement are distinct. Code-action close joins its retained continuation; process kill acknowledges only termination request and background exec survives observer abort.

Evidence: [delivery](#evidence-delivery), [custody](#evidence-custody), [owner](#evidence-owner), [process](#evidence-process), [kill](#evidence-kill).

### turn-redefinition

**I — Turn program** (source): Configured context engine participates in actual model request construction and can replace active messages/add system material with explicit budget/read fence. This is a host extension surface, not proven unrestricted model runtime reprogramming.

Evidence: [context](#evidence-context).

### compaction

**I — Compaction** (source): Separate compaction/branch events survive store reload and produce a projected context; rewrite stages side entries with retained ancestry. Test oracle validates these exact persistence/projection edges.

Evidence: [rewrite](#evidence-rewrite), [compact-test](#evidence-compact-test).

### context-repair

**S — Repair** (source): Original message anchors, scoped history/search and transcript ancestry provide repair ingredients. Tool presentation is redacted/bounded and universal retrieval of all precompaction originals was not traced.

Evidence: [rewrite](#evidence-rewrite), [history](#evidence-history), [history-api](#evidence-history-api), [search](#evidence-search), [search-dispatch](#evidence-search-dispatch).

### original-audit

**L — Original audit** (source): Transcript lineage/compaction records and process settlement exist. Optional provider diagnostics are Anthropic-only and redacted; code output/results and history are bounded. This is not retention of all original provider/program/failure IO.

Evidence: [diagnostic](#evidence-diagnostic), [results](#evidence-results), [guidance](#evidence-guidance), [history](#evidence-history), [settlement](#evidence-settlement).

### audit-query

**L — Audit query** (source): sessions_history and sessions_search expose scoped transcript retrieval, anchors, tools option and indexing state; search is user/assistant snippets, history redacted/clipped, not original wire corpus.

Evidence: [history](#evidence-history), [history-api](#evidence-history-api), [search](#evidence-search), [search-dispatch](#evidence-search-dispatch).

### hot-change

**L — Hot change** (source): Code-mode owners hold plugin refresh until physical continuation cleanup succeeds. Channel reload has explicit proceed-while-active escape paths, not a universal settled-turn activation rule.

Evidence: [owner](#evidence-owner), [pool](#evidence-pool), [reload](#evidence-reload), [owner-test](#evidence-owner-test).

### rebuild-continuity

**L — Rebuild continuity** (source): Transcript ancestry/branch restore and reload ownership are useful. Live cell has no serializable VM image; channel reload may proceed active. No outpost-auth/chat executable rebuild and re-inhabitation protocol traced.

Evidence: [rewrite](#evidence-rewrite), [continuation](#evidence-continuation), [reload](#evidence-reload).

### remote-services

**I — Remote** (source): Actual ACP child dispatch and remote-workspace skill read bridge consume independent services; conditional code-mode MCP/nodes guidance is separately marked as advertised. Remote cancellation/reconnect guarantees were not traced.

Evidence: [spawn-dispatch](#evidence-spawn-dispatch), [workspace-skill-read](#evidence-workspace-skill-read), [guidance](#evidence-guidance).

### self-improvement

**? — Self-improve** (inspection scope): self_improvement: not established beyond the inspected code-mode, embedded context, session-tool and process lifecycles; this is not a universal absence claim.

### complaints

**? — Complaints** (inspection scope): complaints: not established beyond the inspected code-mode, embedded context, session-tool and process lifecycles; this is not a universal absence claim.

### authority

**I — Authority** (source): Workspace/read-only and exec policy apply to native/nested code actions; process scopes distinguish borrowed policy from ownership. Ordinary model edits are available under standing policy; full config/turn authority not inferred.

Evidence: [files](#evidence-files), [guidance](#evidence-guidance), [scope](#evidence-scope).

### evaluation

**S — Evaluation** (source): Inspected tests discriminate late cleanup, revoked dispatch and refresh release ordering, plus persisted compaction/projection; mock completion/continuation do not prove foreign process termination or semantic summary fidelity.

Evidence: [owner-test](#evidence-owner-test), [compact-test](#evidence-compact-test).

### time-order

**L — Time/order** (source): Monotonic performance.now budgets govern cell preparation/execution; parked expiration and channel reload elapsed budgets use Date.now. Session incarnation/read fences/receiver admission distinguish causal identity from wall time.

Evidence: [execution](#evidence-execution), [cell](#evidence-cell), [expiry](#evidence-expiry), [reload](#evidence-reload), [delivery](#evidence-delivery), [context](#evidence-context).

## Inspected test oracles

- [quarantine/openclaw/src/agents/code-mode-state.test.ts](../../../quarantine/openclaw/src/agents/code-mode-state.test.ts): late continuation ownership, revoked dispatch and refresh release Oracle: Controlled promises assert authority revoked immediately, late continuation disposed once, refresh withheld until disposal, stale execution rejected. No real foreign-effect termination oracle. Read, **not executed**.
- [quarantine/openclaw/src/agents/sessions/session-manager-provenance-compaction.test.ts](../../../quarantine/openclaw/src/agents/sessions/session-manager-provenance-compaction.test.ts): compaction and branch projection survive store reload Oracle: SQLite-backed test asserts persisted compaction and branch summary and projected final user message; summarizer mocked, summary factual accuracy and all original wire retention not asserted. Read, **not executed**.

## Useful mechanisms

- Explicit separation of code-action observer from live continuation custody and plugin refresh.
- Addressed steering distinguishes input admission, incarnation, original transcript commit and receiving owner.
- Configurable context assembly and staged transcript side branches are useful projection mechanisms.

## Material limits

- New code action does not inherit a previous namespace; live continuation cannot serialize its VM.
- Channel reload can proceed with active work, conflicting with strict refit quiescence.
- Provider diagnostics and history surfaces redact/clip; full original audit and model state-captured complaint were not established.

## Arconaut design questions

- Can Arconaut represent a pending tool/process owner independently from a short-lived model/tool observer while still enforcing strict refit settlement?
- How do accepted, committed and consumed peer inputs survive sender completion and recipient reincarnation?
- Use explicit pending generations and settled-turn activation without the proceed-active reload escape; keep shared services outside harness quiescence.

## Evidence

### Evidence files

[quarantine/openclaw/src/agents/core-coding-tools.ts:244–415](../../../quarantine/openclaw/src/agents/core-coding-tools.ts#L244): Constructs host/sandbox read/list/edit/write/patch and exec/process tools under effective workspace/read-only policy.

### Evidence process

[quarantine/openclaw/src/agents/bash-tools.exec-run.ts:576–718](../../../quarantine/openclaw/src/agents/bash-tools.exec-run.ts#L576): Exec admits process with supervisor settlement callback, returns ongoing session handle on yield; tool abort deliberately leaves background owner active and suppresses obsolete updates.

### Evidence kill

[quarantine/openclaw/src/agents/bash-tools.process.ts:578–599](../../../quarantine/openclaw/src/agents/bash-tools.process.ts#L578): Kill returns Termination requested after supervisor cancellation admission, not confirmed physical exit.

### Evidence settlement

[quarantine/openclaw/src/agents/bash-tools.exec-settlement.ts:8–69](../../../quarantine/openclaw/src/agents/bash-tools.exec-settlement.ts#L8): Exit settlement preserves observed process outcome, settles task callback before notification, and retains exact owner through failure correction.

### Evidence scope

[quarantine/openclaw/src/agents/agent-tools.ts:158–178](../../../quarantine/openclaw/src/agents/agent-tools.ts#L158): One-shot CLI scope cleanup joins both supervisor cleanup and exec finalization; borrowed policy does not transfer process ownership.

### Evidence execution

[quarantine/openclaw/src/agents/code-mode-execution.ts:58–176](../../../quarantine/openclaw/src/agents/code-mode-execution.ts#L58): Exec captures catalog, replay identity, run owner and bounded output; uses monotonic preparation budget and retains waiting continuations.

### Evidence executor

[quarantine/openclaw/src/agents/code-mode-executor.ts:17–76](../../../quarantine/openclaw/src/agents/code-mode-executor.ts#L17): Dispatches Node or plugin executor; resumes a retained continuation rather than serializing an arbitrary stack.

### Evidence cell

[quarantine/openclaw/src/agents/code-mode-node.worker.ts:220–295](../../../quarantine/openclaw/src/agents/code-mode-node.worker.ts#L220): Each new exec creates a fresh node:vm context; current cell keeps pending bridge requests and a monotonic deadline.

### Evidence continuation

[quarantine/openclaw/src/agents/code-mode-node.ts:221–250](../../../quarantine/openclaw/src/agents/code-mode-node.ts#L221): Continuation retains its live pool; explicitly no serialized VM image. Disposal closes pool, resumed ownership transfers.

### Evidence pool

[quarantine/openclaw/src/agents/code-mode-node.ts:83–115](../../../quarantine/openclaw/src/agents/code-mode-node.ts#L83): Pool owner stays in retiring set while native task close is pending or fails; plugin release aggregates failures.

### Evidence owner

[quarantine/openclaw/src/agents/code-mode-state.ts:84–259](../../../quarantine/openclaw/src/agents/code-mode-state.ts#L84): Parked cell holds plugin refresh lease. Close revokes dispatch immediately, joins late executions/disposals and retains failed cleanup before releasing lease.

### Evidence expiry

[quarantine/openclaw/src/agents/code-mode-state.ts:262–283](../../../quarantine/openclaw/src/agents/code-mode-state.ts#L262): One timer owns expiration of parked continuations even without a subsequent caller; expiry uses wall Date.now.

### Evidence results

[quarantine/openclaw/src/agents/code-mode-results.ts:12–117](../../../quarantine/openclaw/src/agents/code-mode-results.ts#L12): JSON results live in a run/catalog-bound WeakMap store, maximum 64 entries and byte allowance; invalidation closes/clears and capacity failure is explicit.

### Evidence guidance

[quarantine/openclaw/src/agents/code-mode.ts:142–189](../../../quarantine/openclaw/src/agents/code-mode.ts#L142): Advertises programmatic tools/discovery, conditional swarm/MCP/nodes and results APIs; nested calls retain normal policy/approvals, output/errors are bounded, guest modules unavailable.

### Evidence spawn

[quarantine/openclaw/src/agents/tools/sessions-spawn-tool.ts:446–555](../../../quarantine/openclaw/src/agents/tools/sessions-spawn-tool.ts#L446): Model spawn arguments select child model/thinking, isolated/fork context, run/session mode and cleanup, with ACP feature/authority constraints.

### Evidence spawn-dispatch

[quarantine/openclaw/src/agents/tools/sessions-spawn-tool.ts:588–661](../../../quarantine/openclaw/src/agents/tools/sessions-spawn-tool.ts#L588): Actual execution dispatches ACP or embedded subagent, inherits exact parent identity and records accepted spawn facts.

### Evidence delivery

[quarantine/openclaw/src/agents/tools/sessions-send-tool.delivery.ts:153–275](../../../quarantine/openclaw/src/agents/tools/sessions-send-tool.delivery.ts#L153): Steer requires current active incarnation; receiving runtime owns original input commit, queued acceptance returns steered run ID and unsupported commit-wait has explicit best-effort fallback.

### Evidence custody

[quarantine/openclaw/src/agents/tools/sessions-send-tool.steering.ts:1–70](../../../quarantine/openclaw/src/agents/tools/sessions-send-tool.steering.ts#L1): Accepted steering transfers mutation custody to receiver until settlement, distinct from sender completion and queue admission.

### Evidence notification

[quarantine/openclaw/src/agents/tools/sessions-send-tool.delivery.ts:48–94](../../../quarantine/openclaw/src/agents/tools/sessions-send-tool.delivery.ts#L48): Notify is process-durable queue admission, explicitly runStarted false, not a new recipient turn.

### Evidence context

[quarantine/openclaw/src/agents/embedded-agent-runner/run/attempt-history-prepare.ts:172–246](../../../quarantine/openclaw/src/agents/embedded-agent-runner/run/attempt-history-prepare.ts#L172): Actual request path invokes configured context engine with token/model/tool/read-fence data, adopts assembled messages and system addition; failure falls back to pipeline messages.

### Evidence rewrite

[quarantine/openclaw/src/agents/sessions/session-manager.ts:130–202](../../../quarantine/openclaw/src/agents/sessions/session-manager.ts#L130): Transcript rewrite prepares off-store, appends side entries retaining ancestry and adopts memory at outer publication edge; no arbitrary live stack migration.

### Evidence compact-test

[quarantine/openclaw/src/agents/sessions/session-manager-provenance-compaction.test.ts:28–121](../../../quarantine/openclaw/src/agents/sessions/session-manager-provenance-compaction.test.ts#L28): Test appends model-produced compaction and branch summary, reloads persisted store and asserts summary/branch presence and projected final message; model completion is mocked.

### Evidence history

[quarantine/openclaw/src/agents/tools/sessions-history-tool.ts:108–186](../../../quarantine/openclaw/src/agents/tools/sessions-history-tool.ts#L108): History tool redacts and clips text to 4000 characters, removes details/usage/cost; global returned data is capped at 80 KiB.

### Evidence history-api

[quarantine/openclaw/src/agents/tools/sessions-history-tool.ts:361–434](../../../quarantine/openclaw/src/agents/tools/sessions-history-tool.ts#L361): Exposed history operation selects current/other visible sessions, original message anchors and optional tool messages under session authority.

### Evidence search

[quarantine/openclaw/src/agents/tools/sessions-search-tool.ts:40–89](../../../quarantine/openclaw/src/agents/tools/sessions-search-tool.ts#L40): Search exposes user/assistant snippets and message/session identity, bounded query/hits/snippets and incomplete-index warnings.

### Evidence search-dispatch

[quarantine/openclaw/src/agents/tools/sessions-search-tool.ts:505–539](../../../quarantine/openclaw/src/agents/tools/sessions-search-tool.ts#L505): Search executes gateway sessions.search under per-session admission, preserving incomplete indexing and excluded archive indicators.

### Evidence diagnostic

[quarantine/openclaw/src/agents/anthropic-payload-log.ts:97–171](../../../quarantine/openclaw/src/agents/anthropic-payload-log.ts#L97): Anthropic-only optional diagnostics save a redacted request digest/payload and selected usage/error; explicitly do not retain original secret-bearing payload.

### Evidence reload

[quarantine/openclaw/src/gateway/server-reload-active-work.ts:85–143](../../../quarantine/openclaw/src/gateway/server-reload-active-work.ts#L85): Channel reload initially defers for active work but can proceed on lifecycle lease demand or elapsed timeout with work still active.

### Evidence owner-test

[quarantine/openclaw/src/agents/code-mode-state.test.ts:60–143](../../../quarantine/openclaw/src/agents/code-mode-state.test.ts#L60): Controlled continuation oracle checks immediate revocation, late continuation disposal, withheld refresh until cleanup and rejected stale dispatch; not a foreign-effect process test.

### Evidence workspace-skill-read

[quarantine/openclaw/src/agents/core-coding-tools.ts:67–143](../../../quarantine/openclaw/src/agents/core-coding-tools.ts#L67): Wraps reads of selected remote workspace skill paths through workspace access/skill resources, enforcing selected roots and abort checks while leaving ordinary reads local.

