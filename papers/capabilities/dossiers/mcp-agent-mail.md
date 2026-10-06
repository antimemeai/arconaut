# mcp-agent-mail

Addressed persistent mail with authenticated identities, per-recipient read/ack, cross-project contacts and advisory file reservations.

Role: coordination service. Runtime: Python.

Pinned source: [https://github.com/Dicklesworthstone/mcp_agent_mail](https://github.com/Dicklesworthstone/mcp_agent_mail); revision/version `3fad5ec672869f81d2ca0a4c525dde004ac96f45`.

Async FastMCP HTTP service; SQLite/SQLAlchemy sessions, worker-thread Git filesystem operations and awaited commit queue. It does not run the participating agents.

Owns mail identities/index/archive and advisory reservations. Client coding agents and their programs/provider requests remain separately owned.

Inspection: registration/authentication, send/store/notification, inbox/ack/search, advisory reservation, macro, settings and selected test assertions

Limits of this study: No client agent turn/provider loop or commercial Companion implementation was inspected; partial filesystem/Git crash recovery is not proven.

## Actions

### register_agent / macro_start_session

Surface: external model MCP tool or operator/client API.

Input: Project identity, external program/model, optional name/token/task and reservation/inbox options.

Result: Durable identity/profile plus registration token; macro returns reservations and mail.

Lifecycle: Register or update persistent identity; reconnect authentication binds current MCP session; does not start model work.

Authority: Bearer/JWT endpoint access plus token/session binding for an existing identity.

Evidence: [e1](#evidence-e1), [e2](#evidence-e2), [e14](#evidence-e14), [e17](#evidence-e17).

### send_message

Surface: external model MCP tool.

Input: Project/sender token, to/cc/bcc or broadcast, subject/body, thread/topic, attachments, importance/ack.

Result: Delivery list/count, native message IDs or contact/archive/conflict errors.

Lifecycle: DB then awaited archive; blocked contact never queues body; signals are hints; recipient busy status is not consulted.

Authority: Authenticated sender, contact policy and visible recipient scope; external clients govern when mail enters their context.

Evidence: [e2](#evidence-e2), [e3](#evidence-e3), [e4](#evidence-e4), [e5](#evidence-e5), [e6](#evidence-e6), [e7](#evidence-e7).

### fetch_inbox / acknowledge_message

Surface: external model MCP tool.

Input: Recipient/token, limit/time/topic/urgent/unread filters; explicit message ID for ack.

Result: Recent metadata/optional bodies and per-recipient read/ack timestamps.

Lifecycle: Polling leaves unread state intact; explicit ack is idempotent receipt, not execution completion.

Authority: Authenticated recipient and visible addressed message.

Evidence: [e8](#evidence-e8), [e9](#evidence-e9).

### search_messages

Surface: external model MCP query.

Input: Authenticated project identity, FTS5 query and capped result limit.

Result: Ranked visible message metadata; parameterized LIKE fallback or empty invalid-query result.

Lifecycle: Queries persistent index without replaying an agent; source originals separately accessible in archive.

Authority: Sender-or-recipient visibility predicate on both query branches.

Evidence: [e10](#evidence-e10), [e11](#evidence-e11).

### file_reservation_paths

Surface: external model MCP tool.

Input: Identity token, project-relative paths/globs, exclusive flag, TTL/reason.

Result: Granted reservation IDs/expiry/reused marker plus overlapping holders and advisory warning.

Lifecycle: May grant while conflicting; renew/release are separately named lifecycle tools; new rows compensated on archive error.

Authority: Intent signals; code write access is not fenced, optional precommit guard acts at commit.

Evidence: [e12](#evidence-e12), [e13](#evidence-e13).

### summarize_thread

Surface: external model MCP tool.

Input: Thread IDs, per-thread limit, optional examples/LLM mode/model.

Result: Heuristic points/actions/mentions plus optional model refinement.

Lifecycle: On-demand derived digest; preserves stored mail rather than compacting a client conversation.

Authority: Authenticated visible mail and server-configured LLM availability.

Evidence: [e15](#evidence-e15).

### clear_settings_cache

Surface: service programmatic API.

Input: No arguments.

Result: Cache cleared; next get_settings builds new Settings.

Lifecycle: Explicit next-access refresh; captured server objects need their own recreation.

Authority: Service-side code, not an exposed arbitrary model turn rewrite.

Evidence: [e16](#evidence-e16).

## Capabilities

### filesystem

**S — Files** (source): Writes identity/message/reservation archive files and processed attachments; advisory code-path intent is not arbitrary client filesystem execution.

Evidence: [e5](#evidence-e5), [e12](#evidence-e12), [e13](#evidence-e13).

### processes

**— — OS programs** (inspection): Outside this coordination service role: the reference supplies Addressed persistent mail with authenticated identities, per-recipient read/ack, cross-project contacts and advisory file reservations. rather than an agent execution engine.

### code-actions

**— — Code actions** (inspection): Outside this coordination service role: the reference supplies Addressed persistent mail with authenticated identities, per-recipient read/ack, cross-project contacts and advisory file reservations. rather than an agent execution engine.

### persistent-kernel

**— — Kernel** (inspection): Outside this coordination service role: the reference supplies Addressed persistent mail with authenticated identities, per-recipient read/ack, cross-project contacts and advisory file reservations. rather than an agent execution engine.

### standing-database

**I — Standing DB** (source): Owns persistent SQLite messages/recipient state/identities/reservations plus scoped FTS query. This is mail service data, not generic SQL access or a client kernel DB.

Evidence: [e4](#evidence-e4), [e8](#evidence-e8), [e10](#evidence-e10), [e12](#evidence-e12).

### workflow-programming

**S — Workflows** (source): Composable MCP primitives and session macro automate identity/reservation/mail preparation; caller owns arbitrary workflow/agent execution.

Evidence: [e14](#evidence-e14).

### multi-model

**S — Models** (source): External program/model identity metadata and addressed peers permit heterogeneous consumers; service does not orchestrate peer provider requests.

Evidence: [e1](#evidence-e1), [e3](#evidence-e3).

### live-collaboration

**S — Peer chat** (source): Addressed and broadcast mail plus persistent threads/inboxes supply asynchronous collaboration. Delivery does not schedule or interrupt a busy model.

Evidence: [e3](#evidence-e3), [e7](#evidence-e7), [e8](#evidence-e8).

### concurrent-work

**S — Concurrency** (source): Independent external agents coordinate through per-project archive serialization and awaited commit queue; mail does not settle their concurrent OS effects.

Evidence: [e4](#evidence-e4), [e6](#evidence-e6).

### steering-interrupt

**S — Steer/interrupt** (source): Recipient polling and best-effort debounced file signal can inform a client; no active provider cancellation, busy mailbox injection or acceptance/completion protocol is supplied.

Evidence: [e7](#evidence-e7), [e8](#evidence-e8).

### turn-redefinition

**— — Turn program** (inspection): Outside this coordination service role: the reference supplies Addressed persistent mail with authenticated identities, per-recipient read/ack, cross-project contacts and advisory file reservations. rather than an agent execution engine.

### compaction

**S — Compaction** (source): On-demand bounded thread digests/refinement summarize mail without discarding stored messages; client context compaction is separate.

Evidence: [e15](#evidence-e15).

### context-repair

**S — Repair** (source): Persistent authenticated inbox/search can reacquire omitted coordination data. No reconstruction of an external client original provider context is implemented.

Evidence: [e8](#evidence-e8), [e10](#evidence-e10), [e11](#evidence-e11).

### original-audit

**L — Original audit** (source): Git archive/SQLite retain mail records, not every client/provider action or original submitted byte. Images/body stripping/BCC projections transform records; DB/Git compensation is best effort, not a crash-atomic shared commit.

Evidence: [e4](#evidence-e4), [e5](#evidence-e5), [e6](#evidence-e6).

### audit-query

**I — Audit query** (source): Authenticated recipient/sender FTS5, LIKE fallback and explicit inbox filters query persistent mail. This scope excludes agent execution traces/provider originals.

Evidence: [e8](#evidence-e8), [e10](#evidence-e10), [e11](#evidence-e11).

### hot-change

**S — Hot change** (source): Explicit settings-cache reset rebuilds Settings on next access; no full live server reload, active-agent refit or automatic captured-object update follows.

Evidence: [e16](#evidence-e16).

### rebuild-continuity

**— — Rebuild continuity** (inspection): Outside this coordination service role: the reference supplies Addressed persistent mail with authenticated identities, per-recipient read/ack, cross-project contacts and advisory file reservations. rather than an agent execution engine.

### remote-services

**I — Remote** (source): HTTP MCP exposes the separately running mail service to external agents; bearer/JWT layer and per-identity authentication differ.

Evidence: [e2](#evidence-e2), [e17](#evidence-e17).

### self-improvement

**— — Self-improve** (inspection): Outside this coordination service role: the reference supplies Addressed persistent mail with authenticated identities, per-recipient read/ack, cross-project contacts and advisory file reservations. rather than an agent execution engine.

### complaints

**— — Complaints** (inspection): Outside this coordination service role: the reference supplies Addressed persistent mail with authenticated identities, per-recipient read/ack, cross-project contacts and advisory file reservations. rather than an agent execution engine.

### authority

**I — Authority** (source): Token/session identity, contact-policy/send refusal, scoped visible search and BCC projections are real boundaries. Code reservations grant conflicting intent rather than revoking filesystem authority.

Evidence: [e2](#evidence-e2), [e3](#evidence-e3), [e5](#evidence-e5), [e10](#evidence-e10), [e12](#evidence-e12), [e17](#evidence-e17).

### evaluation

**— — Evaluation** (inspection): Outside this coordination service role: the reference supplies Addressed persistent mail with authenticated identities, per-recipient read/ack, cross-project contacts and advisory file reservations. rather than an agent execution engine.

### time-order

**L — Time/order** (source): Created/read/ack wall-clock timestamps, since filtering, lease expiry and signal debounce provide mail ordering/time operations; no global model-work or physical effect order.

Evidence: [e7](#evidence-e7), [e8](#evidence-e8), [e9](#evidence-e9), [e12](#evidence-e12).

## Inspected test oracles

- [quarantine/mcp-agent-mail/tests/test_messaging_semantics.py](../../../quarantine/mcp-agent-mail/tests/test_messaging_semantics.py): Archive failure after DB commit. Oracle: Read test1209–1252: monkeypatched bundle raises, asserts actual Message and MessageRecipient row counts are both zero. Good compensation oracle; fails before partial file writes and does not model crash/fsync or compensation failure. Read, **not executed**.
- [quarantine/mcp-agent-mail/tests/test_message_delivery_regression.py](../../../quarantine/mcp-agent-mail/tests/test_message_delivery_regression.py): Repeated read/ack and self-addressed mail. Oracle: Read test110–185 and825–872: exact prior read/ack timestamps are retained; native ID/subject/self-inbox assertions. They validate logical receipt, not model execution or notification reliability. Read, **not executed**.
- [quarantine/mcp-agent-mail/tests/test_contact_policy.py](../../../quarantine/mcp-agent-mail/tests/test_contact_policy.py): Contact refusal/overlap/cross-project delivery. Oracle: Read1–170: block_all raises specific error, overlapping reservations allow intended contact and addressed cross-project payload appears. Same MCP client can bind several identities; not evidence every remote client has that identity. Read, **not executed**.

## Useful mechanisms

- Separates durable message from transient signal and explicit recipient receipt.
- Source filters both primary and fallback searches by actual visibility.
- Awaited Git queue and DB compensation are concrete rather than only an enqueue acknowledgement.
- Reservation response discloses conflict and code-path advisory mode.

## Material limits

- Mail receipt is not completion or interruption of recipient work.
- Archive body/attachments are transformed; no client provider original audit.
- DB/Git/filesystem publication is non-atomic and compensation is best effort.
- Conflicting exclusive code reservations still grant; external guard cannot fence all writes.
- Settings reset is not executable reload/refit; commercial Companion behavior remains unavailable.

## Arconaut design questions

- Can a consumer mailbox distinguish stored, offered, incorporated, and acted-on messages without confusing provider quiescence with mail delivery?
- How should audit retain the original submitted body/attachments alongside canonical readable projections?
- Should model/operator coordination avoid exclusive-intent labels unless the filesystem effects are actually fenced?

## Evidence

### Evidence e1

[quarantine/mcp-agent-mail/src/mcp_agent_mail/app.py:5988–6028](../../../quarantine/mcp-agent-mail/src/mcp_agent_mail/app.py#L5988): register_agent accepts project/program/model/name/task/attachment metadata and registration token; profiles describe external agents rather than creating a provider loop.

### Evidence e2

[quarantine/mcp-agent-mail/src/mcp_agent_mail/app.py:5231–5306](../../../quarantine/mcp-agent-mail/src/mcp_agent_mail/app.py#L5231): Identity operations require an authenticated session or constant-time registration-token match; tokenless legacy cleanup and configured window rebind are special cases.

### Evidence e3

[quarantine/mcp-agent-mail/src/mcp_agent_mail/app.py:7132–7240](../../../quarantine/mcp-agent-mail/src/mcp_agent_mail/app.py#L7132): send_message accepts addressed/broadcast Markdown, attachments, thread/topic/ack and sender token; blocked automatic contact does not queue message body and requires a new send.

### Evidence e4

[quarantine/mcp-agent-mail/src/mcp_agent_mail/app.py:5585–5730](../../../quarantine/mcp-agent-mail/src/mcp_agent_mail/app.py#L5585): Processed message commits in DB before Git bundle; archive exception triggers best-effort DB compensation. Notification after archive lock is best effort and is not model interruption.

### Evidence e5

[quarantine/mcp-agent-mail/src/mcp_agent_mail/storage.py:1551–1688](../../../quarantine/mcp-agent-mail/src/mcp_agent_mail/storage.py#L1551): Canonical/outbox and recipient inbox copies carry JSON frontmatter; recipient BCC is redacted and body is stripped, attachments processed; writing files/digest precedes awaited Git commit.

### Evidence e6

[quarantine/mcp-agent-mail/src/mcp_agent_mail/storage.py:169–213](../../../quarantine/mcp-agent-mail/src/mcp_agent_mail/storage.py#L169): Commit queue enqueue waits for request future; absent/stopped/full processor falls back to direct commit, so ordinary completion does not just enqueue archive publication.

### Evidence e7

[quarantine/mcp-agent-mail/src/mcp_agent_mail/storage.py:3618–3683](../../../quarantine/mcp-agent-mail/src/mcp_agent_mail/storage.py#L3618): Debounced per-agent signal file overwrites one notification projection; errors return false and do not fail mail delivery.

### Evidence e8

[quarantine/mcp-agent-mail/src/mcp_agent_mail/app.py:9584–9696](../../../quarantine/mcp-agent-mail/src/mcp_agent_mail/app.py#L9584): fetch_inbox authenticates recipient, filters/caps recent mail and does not mark read/ack; it clears best-effort notification signal after polling.

### Evidence e9

[quarantine/mcp-agent-mail/src/mcp_agent_mail/app.py:9913–9995](../../../quarantine/mcp-agent-mail/src/mcp_agent_mail/app.py#L9913): acknowledge_message checks visibility and stores recipient read and ack timestamps separately; it represents receipt, not task execution.

### Evidence e10

[quarantine/mcp-agent-mail/src/mcp_agent_mail/app.py:10621–10752](../../../quarantine/mcp-agent-mail/src/mcp_agent_mail/app.py#L10621): Authenticated scoped FTS search limits sender/recipient visibility and returns ranked subject/body matches; unsupported/syntax queries are handled separately.

### Evidence e11

[quarantine/mcp-agent-mail/src/mcp_agent_mail/app.py:10752–10836](../../../quarantine/mcp-agent-mail/src/mcp_agent_mail/app.py#L10752): FTS failure falls back to parameterized AND-term LIKE query over visible messages, with limit and creation-time ordering; returned records contain metadata.

### Evidence e12

[quarantine/mcp-agent-mail/src/mcp_agent_mail/app.py:11620–11749](../../../quarantine/mcp-agent-mail/src/mcp_agent_mail/app.py#L11620): Overlapping exclusive reservation conflicts are reported while reservations are still granted; DB commit then archive with compensation for new rows only; code-path advisory warning is explicit.

### Evidence e13

[quarantine/mcp-agent-mail/src/mcp_agent_mail/app.py:11440–11524](../../../quarantine/mcp-agent-mail/src/mcp_agent_mail/app.py#L11440): Reservation API accepts paths/globs, TTL, exclusive intent and identity token; server archive enforcement and code precommit guard are distinct. Sub-minute TTL actually warns rather than enforcing the docstring minimum.

### Evidence e14

[quarantine/mcp-agent-mail/src/mcp_agent_mail/app.py:10002–10087](../../../quarantine/mcp-agent-mail/src/mcp_agent_mail/app.py#L10002): macro_start_session authenticates existing identity, ensures project/registers new identity, invokes reservation tool and fetches inbox; result includes token, agent, reservations and mail.

### Evidence e15

[quarantine/mcp-agent-mail/src/mcp_agent_mail/app.py:10950–11038](../../../quarantine/mcp-agent-mail/src/mcp_agent_mail/app.py#L10950): Thread digest combines heuristic points/actions and optional enabled LLM refinement; only compact derived thread summaries reach refinement; failure falls back without replacing stored messages.

### Evidence e16

[quarantine/mcp-agent-mail/src/mcp_agent_mail/config.py:607–632](../../../quarantine/mcp-agent-mail/src/mcp_agent_mail/config.py#L607): Explicit settings accessor cache reset reloads configuration next access; no automatic full server/transport/client reconfiguration is demonstrated.

### Evidence e17

[quarantine/mcp-agent-mail/src/mcp_agent_mail/http.py:668–722](../../../quarantine/mcp-agent-mail/src/mcp_agent_mail/http.py#L668): HTTP middleware checks static bearer in constant time, optional local bypass or inner JWT delegation; endpoint auth is distinct from agent identity.

