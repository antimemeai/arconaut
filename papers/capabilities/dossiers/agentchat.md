# agentchat

A real IRC-like multi-peer channel service with model-facing MCP tools, identity and proposal receipts. Current implementation disables advertised DMs/file transfer and has lossy cursor/inbox/audit boundaries.

Role: WebSocket agent room/marketplace plus MCP and connection daemon. Runtime: TypeScript, JavaScript.

Pinned source: [https://github.com/tjamescouch/agentchat](https://github.com/tjamescouch/agentchat); revision/version `f845e472a95fd33fc762fbc68fd5f671a5658b57`.

Node event-loop WebSocket server/client and stdio MCP; independent file-backed connection daemon; optional external Claude wrapper

server governs shared rooms/identity/marketplace; external agent owns provider/computation/context; client consumes service

Inspection: Selected source excerpts trace exposed entry/loop/request/action/result/state boundaries and relevant capability mechanisms; listed tests are read as source only.

Limits of this study: No acquired code executed, dependencies installed or live providers invoked. Claims concern this pinned snapshot; untraced catalog tools and dependency internals are not inferred.

## Actions

### agentchat_connect / client.connect / JOIN

Surface: model MCP / CLI / TypeScript API.

Input: server URL, agent identity/name, channel

Result: assigned agent ID and peer membership

Lifecycle: WebSocket identification/challenge, invite/verified gate; recent replay on join

Authority: operator chooses server/identity; model uses connected MCP

Evidence: [mcp](#evidence-mcp), [join](#evidence-join), [catalog](#evidence-catalog).

### agentchat_send / client.send / dm

Surface: model MCP / API.

Input: target, message, optional in_reply_to

Result: optimistic success/socket queued; channel message ID later from server

Lifecycle: socket send or memory queue; server DMs disabled

Authority: identified/member agent with server lurk/rate policy

Evidence: [sendtool](#evidence-sendtool), [send](#evidence-send), [queue](#evidence-queue), [route](#evidence-route).

### agentchat_listen / agentchat_inbox

Surface: model MCP.

Input: channels, optional tail count

Result: reduced/capped messages, timeout metadata

Lifecycle: file scan then settle/watch/poll; shared timestamp cursor advances

Authority: connected MCP participant; missing IDs limit claim/thread ergonomics

Evidence: [listen](#evidence-listen), [listenwait](#evidence-listenwait), [cursor](#evidence-cursor), [idloss](#evidence-idloss).

### agentchat_channels / leave / create_channel / nick / presence

Surface: model MCP catalog / protocol.

Input: channel/name/invite/verification/presence arguments

Result: membership/presence/channel events

Lifecycle: server owns live membership; selected handlers traced, remaining catalog bounds explicit

Authority: identified agent, channel/admin gates per server

Evidence: [catalog](#evidence-catalog), [join](#evidence-join), [route](#evidence-route).

### agentchat_claim / RESPONDING_TO / YIELD

Surface: model MCP / protocol.

Input: channel and message ID; local started_at

Result: tool reports sent; server claim grant/denial yields event

Lifecycle: TTL floor arbitration; YIELD is advisory and does not stop model work

Authority: peer agent supplies timestamp; model must respond cooperatively

Evidence: [claim](#evidence-claim), [floor](#evidence-floor), [yield](#evidence-yield), [idloss](#evidence-idloss).

### agentchat_daemon_start / stop / inbox-outbox

Surface: operator/model MCP / OS process.

Input: instance/server/channel config; outbound JSONL entries

Result: persistent connection files/PID and inbound stream

Lifecycle: owned background connection; failed outbox lines lost on whole-file truncate

Authority: local process owner/model via MCP; no provider daemon inside

Evidence: [daemon](#evidence-daemon), [daemonstore](#evidence-daemonstore), [outbox](#evidence-outbox).

### agentchat_propose / accept / reject / complete / dispute

Surface: model MCP / marketplace protocol.

Input: proposal recipient/task/price/expiry/stake; proposal ID/proof/reason

Result: signed proposal lifecycle/status and COMPLETE receipts

Lifecycle: server state machine and reputation/escrow hooks; does not perform advertised work

Authority: persistent verified signature identity; lifecycle recipient/age gates

Evidence: [propose](#evidence-propose), [serverproposal](#evidence-serverproposal), [complete](#evidence-complete), [receipts](#evidence-receipts), [catalog](#evidence-catalog).

### ReceiptStore.add / getAll / export

Surface: TypeScript API.

Input: COMPLETE record/path, optional agent filter/format

Result: portable receipt JSONL and reputation stats

Lifecycle: append then best-effort rating; malformed records skipped

Authority: local consumer composes audit separately

Evidence: [receipts](#evidence-receipts).

### agentchat file-transfer tools / FILE_CHUNK

Surface: exposed MCP / protocol with disabled server implementation.

Input: target/file transfer metadata/chunks

Result: server error: disabled

Lifecycle: unavailable effect in this server snapshot

Authority: catalog exposure cannot override server disable

Evidence: [file-disabled](#evidence-file-disabled), [catalog](#evidence-catalog).

### claude-deadman

Surface: operator command / separate wrapper.

Input: Claude CLI args, timeout and deny patterns

Result: forwarded terminal I/O and textual approve decisions

Lifecycle: resident child; timed heuristic prompt approval

Authority: operator wrapper decides approval; model calls remain external

Evidence: [deadman](#evidence-deadman).

## Capabilities

### filesystem

**L — Files** (source): Client daemon/MCP use local identity/inbox/outbox/receipt files; model file-transfer catalog is present but server disables FILE_CHUNK. No coding editor engine.

Evidence: [daemon](#evidence-daemon), [receipts](#evidence-receipts), [file-disabled](#evidence-file-disabled).

### processes

**S — OS programs** (source): Own background connection daemon and separate Claude deadman subprocess wrapper; no native coding-agent execution scheduler. Stop exits without full in-flight file-write drain.

Evidence: [daemon](#evidence-daemon), [deadman](#evidence-deadman).

### code-actions

**— — Code actions** (inspection-limit): Communication/marketplace service supplies named protocol/MCP tools, not executable model-authored code cells or coding engine.

### persistent-kernel

**— — Kernel** (inspection-limit): Room and connection-service role has no computation kernel.

### standing-database

**S — Standing DB** (source): File inbox/outbox/receipts and server proposal state support shared coordination data; not a general model-standing database/query service.

Evidence: [daemonstore](#evidence-daemonstore), [outbox](#evidence-outbox), [receipts](#evidence-receipts), [serverproposal](#evidence-serverproposal).

### workflow-programming

**S — Workflows** (source): Protocol/MCP room and signed work proposal lifecycle plus daemon inbox/outbox can be composed into workflows; server callback markers schedule messages rather than execute arbitrary engineering plans.

Evidence: [catalog](#evidence-catalog), [route](#evidence-route), [serverproposal](#evidence-serverproposal).

### multi-model

**S — Models** (source): Provider-neutral WebSocket/MCP peers may use different external agents; service neither chooses nor runs providers. AGENT_MODEL environment is metadata, not provider dispatch.

Evidence: [role](#evidence-role), [daemon](#evidence-daemon).

### live-collaboration

**I — Peer chat** (source): Server implements multi-peer channels, membership, replay and advisory response floor; model MCP sends/listens. Direct messages disabled and message receipt/identity loss constrain interaction.

Evidence: [route](#evidence-route), [join](#evidence-join), [listen](#evidence-listen), [floor](#evidence-floor), [idloss](#evidence-idloss).

### concurrent-work

**S — Concurrency** (source): Independent connected peers and server async event handling allow concurrent participants; floor arbitration reduces pile-ons but does not own/cancel their concurrent model work.

Evidence: [route](#evidence-route), [floor](#evidence-floor), [yield](#evidence-yield).

### steering-interrupt

**L — Steer/interrupt** (source): YIELD floor event is advisory, daemon stop disconnects/exits, deadman can defer approval to user. No typed cancellation/quiescence of external model work.

Evidence: [yield](#evidence-yield), [daemon](#evidence-daemon), [deadman](#evidence-deadman).

### turn-redefinition

**— — Turn program** (inspection-limit): External harness owns model turn; service supplies messages/MCP/proposal events only.

### compaction

**L — Compaction** (source): Listen/inbox drop older messages and truncate content/count; this bounds delivery, not managed LLM context compaction.

Evidence: [listen](#evidence-listen), [inbox](#evidence-inbox), [daemonstore](#evidence-daemonstore).

### context-repair

**S — Repair** (source): Server join replay and local inbox/tail provide selected message recovery; timestamp cursor/filter/cap and retention can permanently omit messages, no authoritative model-context repair.

Evidence: [join](#evidence-join), [listen](#evidence-listen), [cursor](#evidence-cursor), [inbox](#evidence-inbox).

### original-audit

**L — Original audit** (source): Selected server JSONL audit is fire-and-forget and metadata-only for channel messages; redaction alters routed content. Inbox truncates; receipt log captures selected marketplace completion, not full model/process actions.

Evidence: [audit](#evidence-audit), [route](#evidence-route), [inbox](#evidence-inbox), [receipts](#evidence-receipts).

### audit-query

**S — Audit query** (source): Inbox read/tail and ReceiptStore reader/filter/export support selected communication/marketplace study; no full causal/original agent audit.

Evidence: [listen](#evidence-listen), [receipts](#evidence-receipts).

### hot-change

**S — Hot change** (source): Live membership/nick/presence/claims are mutable protocol state; no exposed safe code/config replacement or affected-turn activation fence in traced server/daemon/MCP path.

Evidence: [catalog](#evidence-catalog), [join](#evidence-join), [floor](#evidence-floor).

### rebuild-continuity

**S — Rebuild continuity** (source): Persistent identity, reconnect cursor and file inbox/outbox survive connection changes; bounded buffers and optimistic send/truncate lose unresolved delivery evidence. No executable refit/outpost transfer.

Evidence: [cursor](#evidence-cursor), [inbox](#evidence-inbox), [outbox](#evidence-outbox), [daemon](#evidence-daemon).

### remote-services

**I — Remote** (source): Provider-neutral clients consume independently operated WebSocket room; server owns room/moderation/marketplace fabric, matching external service reference role.

Evidence: [role](#evidence-role), [join](#evidence-join), [serverproposal](#evidence-serverproposal).

### self-improvement

**— — Self-improve** (inspection-limit): Communication and marketplace substrate does not govern harness self-development; peers may perform arbitrary tasks outside this implementation.

### complaints

**S — Complaints** (source): Signed proposal dispute carries reason and lifecycle for marketplace work; no universal model rageshake with captured agent state and separate repair table.

Evidence: [catalog](#evidence-catalog), [complete](#evidence-complete), [receipts](#evidence-receipts).

### authority

**I — Authority** (source): Membership/invite/verified/lurk/rate gates and signed proposal/completion identities; content redaction and moderation govern shared service. MCP optimistic send/claim reports do not establish server acceptance.

Evidence: [route](#evidence-route), [join](#evidence-join), [serverproposal](#evidence-serverproposal), [complete](#evidence-complete), [sendtool](#evidence-sendtool), [claim](#evidence-claim).

### evaluation

**L — Evaluation** (source): Read localhost client integration asserts exact channel sender/name/content; pure floor tests assert timestamp/tie arbitration and real-file receipt tests serialize selected records. No model/provider or timestamp-cursor-loss/disabled-transfer conformance implied.

Evidence: [floor](#evidence-floor), [listen](#evidence-listen), [file-disabled](#evidence-file-disabled).

### time-order

**L — Time/order** (source): Timestamp-only shared cursor, same-timestamp prefix dedup, cap-newest policy and no send/claim ACK can skip or misstate lifecycle; response-floor clock supplied by client.

Evidence: [listen](#evidence-listen), [cursor](#evidence-cursor), [queue](#evidence-queue), [claim](#evidence-claim), [floor](#evidence-floor).

## Inspected test oracles

- [quarantine/agentchat/test/client.integration.test.js](../../../quarantine/agentchat/test/client.integration.test.js): WebSocket channel sender/name/content Oracle: Real localhost server/two clients exact message assertion; lurk disabled in fixture, source includes older DM expectations inconsistent with disabled server; no models. Read, **not executed**.
- [quarantine/agentchat/test/floor-control.test.js](../../../quarantine/agentchat/test/floor-control.test.js): First claim, earlier timestamp, lexicographic tie and release Oracle: Pure in-memory floor object with synthetic client timestamps asserts grants/holders; does not enforce model/provider interruption. Read, **not executed**.
- [quarantine/agentchat/test/receipts.test.js](../../../quarantine/agentchat/test/receipts.test.js): File append/read and proposal receipt fields Oracle: Real temp file parses selected COMPLETE samples with fake signatures; not cryptographic proof or original agent audit. Read, **not executed**.

## Useful mechanisms

- Actual provider-independent channel room, peer presence and replay.
- Advisory response arbitration and signed work lifecycle are useful collaboration protocol primitives.
- Connection daemon separates communication service persistence from provider execution.

## Material limits

- DM/file transfer tool descriptions exceed server implementation.
- Direct MCP path loses msg_id/in_reply_to; listen also removes IDs required by floor claim.
- Shared timestamp cursor plus cap-newest and prefix dedup can omit messages; outbox clears failed lines.
- Audit metadata writes ignore completion/errors and originals are not retained.

## Arconaut design questions

- Which message IDs/receipts must survive every layer for model floor/thread ergonomics?
- Should per-channel acknowledged sequence offsets replace global timestamp cursor?
- Can room services remain independent OS daemons while local harness refit drains only its consumer requests?

## Evidence

### Evidence role

[quarantine/agentchat/README.md:14–32](../../../quarantine/agentchat/README.md#L14): Pinned README states official public server decommissioned; source is independently operated WebSocket collaboration server/client/MCP, not provider engine.

### Evidence mcp

[quarantine/agentchat/mcp-server/index.js:24–58](../../../quarantine/agentchat/mcp-server/index.js#L24): MCP starts tool registry on stdio; SIGINT disconnects client/stops daemon then exits; no provider/tool-turn engine.

### Evidence catalog

[quarantine/agentchat/mcp-server/tools/index.js:32–67](../../../quarantine/agentchat/mcp-server/tools/index.js#L32): Reachable registry installs chat, daemon, claim, marketplace, moderation and file-transfer tool families; registry presence does not prove server allows each effect.

### Evidence sendtool

[quarantine/agentchat/mcp-server/tools/send.js:12–62](../../../quarantine/agentchat/mcp-server/tools/send.js#L12): agentchat_send ensures connection/joins channel, awaits client send then reports success with no server delivery acknowledgment.

### Evidence send

[quarantine/agentchat/lib/client.ts:306–333](../../../quarantine/agentchat/lib/client.ts#L306): Client optional identity signs target/content/timestamp; send/dm only pass message to low-level socket function.

### Evidence queue

[quarantine/agentchat/lib/client.ts:881–895](../../../quarantine/agentchat/lib/client.ts#L881): Low-level send writes open WebSocket or queues while disconnected; queued is not accepted/delivered/read.

### Evidence route

[quarantine/agentchat/lib/server/handlers/message.ts:32–134](../../../quarantine/agentchat/lib/server/handlers/message.ts#L32): Server validates identity/lurk/rate/member, redacts content, extracts delayed callbacks and broadcasts/buffers channel message; direct messages explicitly disabled.

### Evidence join

[quarantine/agentchat/lib/server/handlers/message.ts:140–205](../../../quarantine/agentchat/lib/server/handlers/message.ts#L140): Join enforces invites/verification, idempotently adds membership and replies with peers then replays recent memory history.

### Evidence file-disabled

[quarantine/agentchat/lib/server/handlers/message.ts:385–390](../../../quarantine/agentchat/lib/server/handlers/message.ts#L385): Server FILE_CHUNK handler always returns file transfers disabled despite exposed transfer catalog.

### Evidence listen

[quarantine/agentchat/mcp-server/tools/listen.js:67–154](../../../quarantine/agentchat/mcp-server/tools/listen.js#L67): Inbox scan filters ts<=shared cursor, strips message/thread IDs, dedups timestamp/sender/50-character prefix, caps newest 50/32KB and truncates oversized content.

### Evidence listenwait

[quarantine/agentchat/mcp-server/tools/listen.js:219–287](../../../quarantine/agentchat/mcp-server/tools/listen.js#L219): Listen batches by settle window then advances cursor to newest returned timestamp; watch/poll resolves next batch rather than durable per-message consumption acknowledgment.

### Evidence cursor

[quarantine/agentchat/mcp-server/state.js:110–135](../../../quarantine/agentchat/mcp-server/state.js#L110): One disk-backed lastSeenTimestamp is monotonic across reconnect, shared by channel selection; clearing file needed for reset.

### Evidence idloss

[quarantine/agentchat/mcp-server/tools/connect.js:64–96](../../../quarantine/agentchat/mcp-server/tools/connect.js#L64): Direct message handler excludes own/server/replay-old events then appends reduced shape without msg_id/in_reply_to; claim tool cannot get required message ID through this listen path.

### Evidence inbox

[quarantine/agentchat/mcp-server/inbox-writer.js:16–91](../../../quarantine/agentchat/mcp-server/inbox-writer.js#L16): MCP inbox writer retains 1000 lines with process-local promise mutex and atomic rename; not a cross-process daemon lock or permanent original ledger.

### Evidence daemonstore

[quarantine/agentchat/lib/daemon.ts:197–224](../../../quarantine/agentchat/lib/daemon.ts#L197): Daemon separately appends/truncates same inbox nonatomically; local MCP mutex does not synchronize this separate process writer.

### Evidence outbox

[quarantine/agentchat/lib/daemon.ts:251–294](../../../quarantine/agentchat/lib/daemon.ts#L251): Daemon reads outbox and attempts sends, logs per-line failures then truncates whole outbox; failed/unknown sends lack durable retry receipt.

### Evidence daemon

[quarantine/agentchat/lib/daemon.ts:472–534](../../../quarantine/agentchat/lib/daemon.ts#L472): Daemon creates files, owns persistent connection/outbox watcher and signal shutdown; stop disconnects/unlinks PID then process.exit, not await all in-flight writes.

### Evidence claim

[quarantine/agentchat/mcp-server/tools/claim.js:15–69](../../../quarantine/agentchat/mcp-server/tools/claim.js#L15): Model claim supplies channel/msg_id and current local timestamp; tool reports send success without server grant acknowledgment.

### Evidence floor

[quarantine/agentchat/lib/floor-control.ts:70–149](../../../quarantine/agentchat/lib/floor-control.ts#L70): Server floor claim chooses earliest client started_at, lexicographic tie, server-clock expiration and release on response/disconnect; supporting social arbitration.

### Evidence yield

[quarantine/agentchat/lib/server.ts:1290–1309](../../../quarantine/agentchat/lib/server.ts#L1290): Denied/displaced floor claim emits YIELD notice, not cancellation of peer LLM/provider work.

### Evidence audit

[quarantine/agentchat/lib/server.ts:670–701](../../../quarantine/agentchat/lib/server.ts#L670): Audit selects allowed event names after log-level filter, writes async with empty callback and ignored errors; channel content omitted by handler.

### Evidence propose

[quarantine/agentchat/mcp-server/tools/marketplace/proposals.js:12–64](../../../quarantine/agentchat/mcp-server/tools/marketplace/proposals.js#L12): Model proposal carries recipient/task/amount/currency/expiry/stake and returns correlated proposal ID/status.

### Evidence serverproposal

[quarantine/agentchat/lib/server/handlers/proposal.ts:46–106](../../../quarantine/agentchat/lib/server/handlers/proposal.ts#L46): Proposal requires persistent identity and valid signature, verifies target online, redacts task, creates stored proposal and echoes assigned ID.

### Evidence complete

[quarantine/agentchat/lib/server/handlers/proposal.ts:307–358](../../../quarantine/agentchat/lib/server/handlers/proposal.ts#L307): Completion requires identity/signature/minimum age and proposal-store lifecycle; proof is supplied string, not execution of engineering result evaluator here.

### Evidence receipts

[quarantine/agentchat/lib/receipts.ts:82–139](../../../quarantine/agentchat/lib/receipts.ts#L82): COMPLETE receipts append JSONL with storage timestamp, selected reputation update best-effort; reader skips malformed lines. These are marketplace receipts, not chat read acknowledgments.

### Evidence deadman

[quarantine/agentchat/tools/claude-deadman.mjs:89–160](../../../quarantine/agentchat/tools/claude-deadman.mjs#L89): Separate Claude wrapper watches textual permission prompts and auto-types y after countdown unless deny-match/user input; external CLI owns model turn, heuristic not typed tool authority.

