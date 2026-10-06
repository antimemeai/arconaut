# claude-code-unofficial

A rich but incomplete, modified unofficial source artifact: real query/tool/teammate code, gated or missing internals, and selective transcripts. Its behavior is not attributed to official Claude Code.

Role: unofficial modified TypeScript coding-agent source mirror. Runtime: TypeScript.

Pinned source: [https://github.com/codeaashu/claude-code](https://github.com/codeaashu/claude-code); revision/version `eec3692193a30bdedb9f31e033d93971aec73585`.

Bun/Node async generators and React/Ink; in-process teammate loops, shell children and remote-session client.

Owns local turn/task/transcript state; consumes model/MCP/remote sessions. Artifact authenticity and correspondence to official executable unverified.

Inspection: Read mirror provenance, actual query/dependency wiring, tool registry/dispatch/edit/shell, teammate mailbox and runner, compaction/transcript, configuration/permission and remote controls, plus web session test.

Limits of this study: No execution; authenticity unverified. WorkflowTool/REPLTool core modules referenced by gated registry absent in acquired tree; issue command is disabled stub. Many internal build gates and dependencies unavailable.

## Actions

### Read/Edit/Write/Glob/Grep/NotebookEdit

Surface: model tools.

Input: file paths, ranges, replacement text/search patterns

Result: content/patch/result/error

Lifecycle: tool rounds; Edit stale-read check before synchronous write

Authority: model through registry/permission function

Evidence: [registry](#evidence-registry), [edit](#evidence-edit).

### Bash / background task / TaskOutput / TaskStop

Surface: model tools.

Input: command, timeout, background flag/task ID

Result: progress, transformed merged output, task handles

Lifecycle: foreground awaited or background owned task; active-frame/child settlement not fully traced

Authority: model permitted shell; scope per-agent tasks

Evidence: [bash](#evidence-bash), [background](#evidence-background), [registry](#evidence-registry).

### Agent / team / SendMessage

Surface: model tools.

Input: prompt/model override, peer or * and message

Result: independent peer task, mailbox receipt or shutdown protocol

Lifecycle: peer works independently, returns idle and polls; busy delivery boundary not fully traced

Authority: model registry with feature/build gates; write failures can still report sent

Evidence: [peer](#evidence-peer), [idle](#evidence-idle), [send](#evidence-send), [mailbox](#evidence-mailbox).

### Compact / MCP refresh / queued steering

Surface: operator and host loop.

Input: custom summary instructions, queued agent-addressed messages, connected MCP tools

Result: summary/attachments/lineage, next-round input/schema

Lifecycle: context preparation each provider round; slash commands after turn

Authority: host callbacks and operator control, not arbitrary model-authored scheduler

Evidence: [loop](#evidence-loop), [compact](#evidence-compact), [lineage](#evidence-lineage), [queue](#evidence-queue).

### Config get/set

Surface: gated model tool.

Input: supported setting and optional value

Result: validated get/set result

Lifecycle: specific state updates immediate; query config snapshot and MCP next round differ

Authority: internal-only tool; writes ask permission

Evidence: [config](#evidence-config), [registry](#evidence-registry).

### Remote session message/interrupt/reconnect

Surface: client API.

Input: session identity, content, request ID

Result: send success, WS controls, reconnected transport

Lifecycle: HTTP + WS client; no direct OS execution in this path

Authority: program/operator authenticated remote session

Evidence: [remote](#evidence-remote).

## Capabilities

### filesystem

**I — Files** (source): Native edit/read/search/write/notebook tools; stale content guard synchronous within process, no claimed cross-process transaction.

Evidence: [registry](#evidence-registry), [edit](#evidence-edit).

### processes

**I — OS programs** (source): Streaming shell children with explicit/automatic background task handles; output merged/transformed and lifetime lower layer not fully traced.

Evidence: [bash](#evidence-bash), [background](#evidence-background).

### code-actions

**L — Code actions** (source): Shell composes OS programs. Gated REPL/workflow registries exist but core modules unavailable, so persistent code-action engine not established.

Evidence: [registry](#evidence-registry), [gates](#evidence-gates).

### persistent-kernel

**? — Kernel** (inspection scope): REPL registry branch and primitives exist but REPLTool core module absent; persistent namespace/lifecycle not established.

### standing-database

**? — Standing DB** (inspection scope): Inspected state is task/transcript/mailbox data; no model queryable standing research database traced.

### workflow-programming

**S — Workflows** (source): Host query dependencies and hooks, skill/agent tools and notification priorities compose execution; advertised gated WorkflowTool core unavailable.

Evidence: [deps](#evidence-deps), [registry](#evidence-registry), [queue](#evidence-queue), [gates](#evidence-gates).

### multi-model

**I — Models** (source): Main/fallback/advisor models and per-peer override in actual model/runner invocation.

Evidence: [provider](#evidence-provider), [peer](#evidence-peer).

### live-collaboration

**L — Peer chat** (source): Addressed peer mailboxes and independent persistent peer loops real; idle poll traced, busy delivery incompletely traced, swallowed write failures allow false sent acknowledgement.

Evidence: [send](#evidence-send), [mailbox](#evidence-mailbox), [peer](#evidence-peer), [idle](#evidence-idle), [poll](#evidence-poll).

### concurrent-work

**I — Concurrency** (source): Consecutive declared-safe tools concurrent, independent peer runs and background shell handles; not all effects safe merely by declaration.

Evidence: [dispatch](#evidence-dispatch), [peer](#evidence-peer), [background](#evidence-background).

### steering-interrupt

**L — Steer/interrupt** (source): Provider AbortSignal, post-tool abort terminals and remote interrupt requests; cancellation signal does not prove all program/provider activity settled.

Evidence: [provider](#evidence-provider), [continue](#evidence-continue), [remote](#evidence-remote).

### turn-redefinition

**S — Turn program** (source): Hooks can stop continuation, model deps injectable by host and contexts modified by tools; no traced model-replaceable full scheduling program.

Evidence: [deps](#evidence-deps), [dispatch](#evidence-dispatch), [continue](#evidence-continue).

### compaction

**I — Compaction** (source): Multiple projection/micro/auto summary stages, grouping retry truncation and recent-resource restoration; some branches build gated/unavailable.

Evidence: [loop](#evidence-loop), [compact](#evidence-compact), [lineage](#evidence-lineage).

### context-repair

**S — Repair** (source): Boundary previous UUID and transcript path support recovery; recent files re-read. No universal model repair operation or preserved transformation originals established.

Evidence: [lineage](#evidence-lineage), [compact](#evidence-compact), [append](#evidence-append).

### original-audit

**L — Original audit** (source): Selective JSONL excludes progress; fallback tombstones partial attempts. Internal best-effort successful response dump is parsed/delta-based, neither all attempts nor original bytes.

Evidence: [audit](#evidence-audit), [provider](#evidence-provider), [dump](#evidence-dump), [append](#evidence-append).

### audit-query

**L — Audit query** (source): Rendered text search omits thinking/unknown tool shapes; transcript path accessible but complete original audit unavailable.

Evidence: [search](#evidence-search), [lineage](#evidence-lineage).

### hot-change

**S — Hot change** (source): Config updates some state immediately, query entry snapshots config, tools refresh between model rounds; executable replacement unavailable.

Evidence: [config](#evidence-config), [loop](#evidence-loop), [queue](#evidence-queue).

### rebuild-continuity

**? — Rebuild continuity** (inspection scope): Inspected resume/transcript/remote paths do not establish outpost/refit executable rebuild continuity or quiescence.

### remote-services

**I — Remote** (source): MCP registry and remote-session authenticated message/permission/interrupt/reconnect client; consumer feature, not governor of shared kernels.

Evidence: [registry](#evidence-registry), [remote](#evidence-remote).

### self-improvement

**? — Self-improve** (inspection scope): Generic edits/skills/config are present; no independently evaluated executable self-rebuild/promotion program traced.

### complaints

**L — Complaints** (source): Inspected /issue command disabled stub; internal dump comments for /issue not an available universal model complaint/state-table flow.

Evidence: [issue](#evidence-issue), [dump](#evidence-dump).

### authority

**L — Authority** (source): Model tools subject to configurable permission modes. Even bypass retains certain ask/deny/interaction checks; config write itself asks.

Evidence: [authority](#evidence-authority), [config](#evidence-config).

### evaluation

**S — Evaluation** (source): Query dependency seam supports fakes; available web session tests target routing/count bookkeeping, not underlying real provider/process cancellation.

Evidence: [deps](#evidence-deps), [test](#evidence-test).

### time-order

**S — Time/order** (source): Query UUID chain/depth, 500ms idle poll and wall timestamps provide identities/order hints; pause/restart logical clock semantics unestablished.

Evidence: [loop](#evidence-loop), [poll](#evidence-poll), [send](#evidence-send).

## Inspected test oracles

- [quarantine/claude-code-unofficial/src/server/web/__tests__/session-manager.test.ts](../../../quarantine/claude-code-unofficial/src/server/web/__tests__/session-manager.test.ts): session cap and PTY/WebSocket routing Oracle: Exact count/cap/cleanup bookkeeping; mocks do not establish real process death or byte preservation. Read, **not executed**.

## Useful mechanisms

- Actual peer work-abort versus peer lifecycle-abort separation.
- Multiple managed context transformations with boundary/attachment handling.
- Concurrency-safe batching preserves input order when applying context modifiers.

## Material limits

- Unverified modified artifact must not serve as authoritative official implementation evidence.
- Mailbox write failures swallowed before success result; partial attempts/progress not complete audit.
- Gated REPL/workflow/issue implementation unavailable or stubbed in acquired source.

## Arconaut design questions

- Make durable message acceptance distinct from recipient observation and turn settlement.
- Record failed provider attempts and transformation inputs before changing active transcript.
- Specify model-program activation boundaries explicitly; next tool round and settled workflow are different.

## Evidence

### Evidence origin

[quarantine/claude-code-unofficial/README.md:3–15](../../../quarantine/claude-code-unofficial/README.md#L3): Mirror claims leaked source and preserved backup branch; attribution unverified and current branch modified.

### Evidence loop

[quarantine/claude-code-unofficial/src/query.ts:293–467](../../../quarantine/claude-code-unofficial/src/query.ts#L293): Actual query loop projects/compacts messages before provider step; config snapshot at user-turn entry.

### Evidence provider

[quarantine/claude-code-unofficial/src/query.ts:659–720](../../../quarantine/claude-code-unofficial/src/query.ts#L659): callModel passes tools, abort signal, model/advisor; fallback tombstones orphaned partial messages.

### Evidence deps

[quarantine/claude-code-unofficial/src/query/deps.ts:21–39](../../../quarantine/claude-code-unofficial/src/query/deps.ts#L21): Production model/microcompact/autocompact/UUID dependencies wired.

### Evidence dispatch

[quarantine/claude-code-unofficial/src/services/tools/toolOrchestration.ts:19–176](../../../quarantine/claude-code-unofficial/src/services/tools/toolOrchestration.ts#L19): Consecutive declared-safe calls concurrent with default cap10; unsafe serial; context modifiers reapplied in original block order.

### Evidence continue

[quarantine/claude-code-unofficial/src/query.ts:1484–1520](../../../quarantine/claude-code-unofficial/src/query.ts#L1484): Abort and hook-stop terminal conditions after tool results.

### Evidence queue

[quarantine/claude-code-unofficial/src/query.ts:1547–1670](../../../quarantine/claude-code-unofficial/src/query.ts#L1547): Mid-tool-round agent-addressed prompt/notification drain, deferred slash commands and MCP tool refresh.

### Evidence registry

[quarantine/claude-code-unofficial/src/tools.ts:193–250](../../../quarantine/claude-code-unofficial/src/tools.ts#L193): Native files/shell/agent/team/task/MCP tools; internal-only config/REPL and gated workflows.

### Evidence gates

[quarantine/claude-code-unofficial/src/tools.ts:104–134](../../../quarantine/claude-code-unofficial/src/tools.ts#L104): REPL/workflow/context-inspect branches reference gated core modules not available in this tree.

### Evidence edit

[quarantine/claude-code-unofficial/src/tools/FileEditTool/FileEditTool.ts:425–491](../../../quarantine/claude-code-unofficial/src/tools/FileEditTool/FileEditTool.ts#L425): Synchronous stale-read/content check then patch write; same-event-loop atomicity is not cross-process locking.

### Evidence bash

[quarantine/claude-code-unofficial/src/tools/BashTool/BashTool.tsx:624–711](../../../quarantine/claude-code-unofficial/src/tools/BashTool/BashTool.tsx#L624): Shell dispatch with agent-scoped task channel, streaming progress and merged stdout/stderr before result transformation.

### Evidence background

[quarantine/claude-code-unofficial/src/tools/BashTool/BashTool.tsx:923–1000](../../../quarantine/claude-code-unofficial/src/tools/BashTool/BashTool.tsx#L923): Foreground in-place backgrounding and explicit background task handle; assistant timer behind gate.

### Evidence send

[quarantine/claude-code-unofficial/src/tools/SendMessageTool/SendMessageTool.ts:149–265](../../../quarantine/claude-code-unofficial/src/tools/SendMessageTool/SendMessageTool.ts#L149): Addressed and broadcast messages await mailbox API then report sent; writes use wall timestamps.

### Evidence mailbox

[quarantine/claude-code-unofficial/src/utils/teammateMailbox.ts:84–190](../../../quarantine/claude-code-unofficial/src/utils/teammateMailbox.ts#L84): Locked read/modify/write JSON inbox; read errors become empty list and write errors logged/swallowed, so sent result does not guarantee persistence.

### Evidence peer

[quarantine/claude-code-unofficial/src/utils/swarm/inProcessRunner.ts:1169–1218](../../../quarantine/claude-code-unofficial/src/utils/swarm/inProcessRunner.ts#L1169): Independent runAgent loop with per-peer model override, retained context, separate work/lifecycle aborts.

### Evidence idle

[quarantine/claude-code-unofficial/src/utils/swarm/inProcessRunner.ts:1317–1367](../../../quarantine/claude-code-unofficial/src/utils/swarm/inProcessRunner.ts#L1317): Peer remains idle, sends notification and then polls next mailbox/task message; final response not automatically sent to lead.

### Evidence poll

[quarantine/claude-code-unofficial/src/utils/swarm/inProcessRunner.ts:690–765](../../../quarantine/claude-code-unofficial/src/utils/swarm/inProcessRunner.ts#L690): Idle loop takes pending operator messages and polls file mailbox at 500ms.

### Evidence compact

[quarantine/claude-code-unofficial/src/services/compact/compact.ts:440–555](../../../quarantine/claude-code-unofficial/src/services/compact/compact.ts#L440): Summary retries truncate oldest round groups; errors rejected; recent file/agent/plan attachments restored.

### Evidence lineage

[quarantine/claude-code-unofficial/src/services/compact/compact.ts:596–624](../../../quarantine/claude-code-unofficial/src/services/compact/compact.ts#L596): Compact boundary names previous UUID and summary references transcript path.

### Evidence audit

[quarantine/claude-code-unofficial/src/utils/sessionStorage.ts:121–184](../../../quarantine/claude-code-unofficial/src/utils/sessionStorage.ts#L121): Transcript excludes ephemeral progress; tombstone rewriting can remove partial messages.

### Evidence append

[quarantine/claude-code-unofficial/src/utils/sessionStorage.ts:606–677](../../../quarantine/claude-code-unofficial/src/utils/sessionStorage.ts#L606): Async queued JSONL append per file; append completion not fsync guarantee.

### Evidence dump

[quarantine/claude-code-unofficial/src/services/api/dumpPrompts.ts:90–225](../../../quarantine/claude-code-unofficial/src/services/api/dumpPrompts.ts#L90): Internal USER_TYPE-only prompt/response dump, deltas and successful responses, asynchronous best-effort errors ignored.

### Evidence search

[quarantine/claude-code-unofficial/src/utils/transcriptSearch.ts:24–79](../../../quarantine/claude-code-unofficial/src/utils/transcriptSearch.ts#L24): Rendered transcript search skips thinking, unknown tool shapes and model-only wrappers.

### Evidence config

[quarantine/claude-code-unofficial/src/tools/ConfigTool/ConfigTool.ts:67–179](../../../quarantine/claude-code-unofficial/src/tools/ConfigTool/ConfigTool.ts#L67): Internal config tool get/set; writes request permission; specific remote-control change updates AppState immediately.

### Evidence authority

[quarantine/claude-code-unofficial/src/utils/permissions/permissions.ts:1225–1280](../../../quarantine/claude-code-unofficial/src/utils/permissions/permissions.ts#L1225): Bypass still respects tool denials, content ask rules, required interactions and path safety checks.

### Evidence remote

[quarantine/claude-code-unofficial/src/remote/RemoteSessionManager.ts:219–323](../../../quarantine/claude-code-unofficial/src/remote/RemoteSessionManager.ts#L219): Remote message, permission response, interrupt control request, disconnect/reconnect; client signal is not remote settled-work confirmation.

### Evidence issue

[quarantine/claude-code-unofficial/src/commands/issue/index.js:1–3](../../../quarantine/claude-code-unofficial/src/commands/issue/index.js#L1): Acquired issue command is disabled stub.

### Evidence test

[quarantine/claude-code-unofficial/src/server/web/__tests__/session-manager.test.ts:49–136](../../../quarantine/claude-code-unofficial/src/server/web/__tests__/session-manager.test.ts#L49): Mock PTY/WebSocket tests assert session cap, forwarding call counts and cleanup bookkeeping, not true process/IO termination.

