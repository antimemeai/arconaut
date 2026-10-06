# restate

durable invocation, state, RPC/wait and administrative pause/replay/deployment/query machinery

Role: distributed durable execution service/runtime. Runtime: Rust.

Pinned source: [https://github.com/restatedev/restate](https://github.com/restatedev/restate); revision/version `2180b55410f6512f8534012167aa536116d75c8a`.

Ingress/API, replicated log/partition processors, durable journal/state/outbox, service invoker HTTP protocol and SQL introspection. User code runs separately in SDK service deployments.

Owns scheduling/storage/replay/service control plane. Arconaut would consume this boundary, not govern its shared fleet. User service/provider/OS effects remain external.

Inspection: ingress call/send/visibility, v4 invoker journal replay/completion proposal, state-machine journal/state/outbox, pause/cancel/resume/deployment fencing, query schema/retention and direct lifecycle/idempotency/fencing tests

Limits of this study: Not a full replication/fleet correctness audit; no server launch/test occurred. External SDK closures and semantic replay compatibility are not certified by protocol compatibility.

## Actions

### Ingress service/object/workflow call and send

Surface: Public HTTP/service routing.

Input: Target handler/key, body/headers/idempotency and optional send delay

Result: Call result or202 invocation ID/execution time/Accepted|PreviouslyAccepted

Lifecycle: Server invocation outlives submitting client; call delay disallowed; sends recorded/scheduled

Authority: Public handler schema at ingress; service fleet owns work

Evidence: [e1](#evidence-e1), [e2](#evidence-e2).

### SDK v4 run completion/journal replay

Surface: Durable service protocol.

Input: Invocation attempt, stored journal/state and SDK serialized result proposal

Result: Replay frames, completion notifications and exact journal entries

Lifecycle: Fresh service attempt replays recorded results; arbitrary external action precedes settlement

Authority: Registered SDK deployment, VM protocol and partition service

Evidence: [e3](#evidence-e3), [e4](#evidence-e4), [e5](#evidence-e5).

### State and CallCommand/OneWayCallCommand

Surface: Durable state/RPC action dispatch.

Input: Keyed state entry or target/payload/retention/idempotency

Result: Stored state, outbox invocation and correlated invocation-ID result

Lifecycle: Partition state-machine scope; external systems beyond Restate are not in transaction

Authority: Service handler context; runtime owns state/log/outbox

Evidence: [e5](#evidence-e5), [e6](#evidence-e6), [e7](#evidence-e7).

### PauseInvocation / ResumeInvocation with deployment patch

Surface: Administrative invocation lifecycle.

Input: Invocation ID; optional pinned/latest deployment and run_at

Result: Accepted/Paused/NotRunning or resumed invocation/error

Lifecycle: Journal retained; abort current attempt, fence stragglers, replay later; protocol compatibility only

Authority: Admin/server control, not consumer-wide service shutdown

Evidence: [e8](#evidence-e8), [e9](#evidence-e9), [e11](#evidence-e11), [e12](#evidence-e12).

### CancelInvocation

Surface: Durable cancellation control.

Input: Invocation ID

Result: Appended for active waits; Done for queued/scheduled; AlreadyCompleted/NotFound

Lifecycle: Active cancellation is a durable signal, not guarantee all external activity stopped

Authority: Runtime admin/SDK call authority

Evidence: [e10](#evidence-e10).

### POST /query

Surface: SQL introspection API.

Input: Query string and Accept JSON/Arrow

Result: Streaming rows, raw journal bytes/decoded entries/events/state views

Lifecycle: Retained platform data only; availability/rate limits; journal purge reduces history

Authority: Admin endpoint/program access

Evidence: [e13](#evidence-e13), [e14](#evidence-e14), [e15](#evidence-e15).

### PurgeJournal completed invocation

Surface: Retention/admin operation.

Input: Completed invocation ID

Result: Dropped journal, retained completion with zero journal length

Lifecycle: Active invocation rejected; irreversible original journal deletion by retention/admin

Authority: Service administrator/governor

Evidence: [e16](#evidence-e16).

## Capabilities

### filesystem

**— — Files** (inspection): Outside this distributed durable execution service/runtime role: the reference supplies durable invocation, state, RPC/wait and administrative pause/replay/deployment/query machinery rather than an agent execution engine.

### processes

**— — OS programs** (inspection): Outside this distributed durable execution service/runtime role: the reference supplies durable invocation, state, RPC/wait and administrative pause/replay/deployment/query machinery rather than an agent execution engine.

### code-actions

**— — Code actions** (inspection): Outside this distributed durable execution service/runtime role: the reference supplies durable invocation, state, RPC/wait and administrative pause/replay/deployment/query machinery rather than an agent execution engine.

### persistent-kernel

**— — Kernel** (inspection): Outside this distributed durable execution service/runtime role: the reference supplies durable invocation, state, RPC/wait and administrative pause/replay/deployment/query machinery rather than an agent execution engine.

### standing-database

**I — Standing DB** (source): Keyed service state plus SQL introspection of system/service records; is this service’s state platform, not a resident agent DB.

Evidence: [e6](#evidence-e6), [e13](#evidence-e13).

### workflow-programming

**S — Workflows** (source): External SDK programs consume durable state/RPC/replay/wait primitives; runtime does not supply a model agent turn program.

Evidence: [e3](#evidence-e3), [e5](#evidence-e5), [e7](#evidence-e7).

### multi-model

**— — Models** (inspection): Outside this distributed durable execution service/runtime role: the reference supplies durable invocation, state, RPC/wait and administrative pause/replay/deployment/query machinery rather than an agent execution engine.

### live-collaboration

**— — Peer chat** (inspection): Outside this distributed durable execution service/runtime role: the reference supplies durable invocation, state, RPC/wait and administrative pause/replay/deployment/query machinery rather than an agent execution engine.

### concurrent-work

**I — Concurrency** (source): Call/send invocations with separate IDs, outbox and deferred scheduling compose concurrent service work.

Evidence: [e2](#evidence-e2), [e7](#evidence-e7).

### steering-interrupt

**L — Steer/interrupt** (source): Pause/abort with internal fencing and explicit cancellation signals; Appended is not Done, and runtime cannot prove arbitrary external action physically settled.

Evidence: [e8](#evidence-e8), [e9](#evidence-e9), [e10](#evidence-e10).

### turn-redefinition

**— — Turn program** (inspection): Outside this distributed durable execution service/runtime role: the reference supplies durable invocation, state, RPC/wait and administrative pause/replay/deployment/query machinery rather than an agent execution engine.

### compaction

**— — Compaction** (inspection): Outside this distributed durable execution service/runtime role: the reference supplies durable invocation, state, RPC/wait and administrative pause/replay/deployment/query machinery rather than an agent execution engine.

### context-repair

**— — Repair** (inspection): Outside this distributed durable execution service/runtime role: the reference supplies durable invocation, state, RPC/wait and administrative pause/replay/deployment/query machinery rather than an agent execution engine.

### original-audit

**L — Original audit** (source): Raw journal command/completion plus ordered lifecycle events retained/queryable; closure-internal IO/provider envelopes/OS effects outside protocol are not captured; completed journal can be purged.

Evidence: [e4](#evidence-e4), [e5](#evidence-e5), [e14](#evidence-e14), [e15](#evidence-e15), [e16](#evidence-e16).

### audit-query

**I — Audit query** (source): POST/query streams JSON/Arrow over retained journal/state; raw/version2 decoded entries and event index link are actual query schema.

Evidence: [e13](#evidence-e13), [e14](#evidence-e14), [e15](#evidence-e15).

### hot-change

**L — Hot change** (source): Explicit resume can patch pinned deployment at log position after protocol validation. It does not hot-patch resident stacks or validate changed program’s semantic replay compatibility.

Evidence: [e11](#evidence-e11), [e12](#evidence-e12).

### rebuild-continuity

**L — Rebuild continuity** (source): Fresh service attempt replays journal and explicit resume can repoint deployment; no harness/model context rebuild/outpost transfer and no atomic external-effect guarantee.

Evidence: [e3](#evidence-e3), [e11](#evidence-e11), [e12](#evidence-e12).

### remote-services

**I — Remote** (source): Runtime invokes separately deployed SDK endpoints with journal/state start and result protocol; this is a service governor external to an Arconaut consumer.

Evidence: [e2](#evidence-e2), [e3](#evidence-e3).

### self-improvement

**— — Self-improve** (inspection): Outside this distributed durable execution service/runtime role: the reference supplies durable invocation, state, RPC/wait and administrative pause/replay/deployment/query machinery rather than an agent execution engine.

### complaints

**— — Complaints** (inspection): Outside this distributed durable execution service/runtime role: the reference supplies durable invocation, state, RPC/wait and administrative pause/replay/deployment/query machinery rather than an agent execution engine.

### authority

**L — Authority** (source): Public ingress rejects private handlers; admin lifecycle/query controls operate platform state. No model/operator approval policy or permissions for arbitrary handler OS effects supplied.

Evidence: [e1](#evidence-e1), [e13](#evidence-e13), [e10](#evidence-e10).

### evaluation

**I — Evaluation** (source): Direct state-machine and committed-log oracles test pause/abort/state, protocol replacement rejection, literal retained output and stale-attempt fencing. No claim full distributed or arbitrary external-effect conformance.

Evidence: [e17](#evidence-e17), [e18](#evidence-e18), [e19](#evidence-e19), [e20](#evidence-e20).

### time-order

**I — Time/order** (source): Exact journal index, record-created append timestamp and after-journal event index provide platform causal order; external effects/time are outside captured protocol.

Evidence: [e5](#evidence-e5), [e14](#evidence-e14), [e15](#evidence-e15).

## Inspected test oracles

- [quarantine/restate/crates/worker/src/partition/state_machine/lifecycle/manual_pause.rs](../../../quarantine/restate/crates/worker/src/partition/state_machine/lifecycle/manual_pause.rs): Pause transitions and running-attempt abort Oracle: State-machine environment asserts exact reply/action and persistent Paused state; does not assert third-party OS work ended. Read, **not executed**.
- [quarantine/restate/crates/worker/src/partition/state_machine/lifecycle/manual_resume.rs](../../../quarantine/restate/crates/worker/src/partition/state_machine/lifecycle/manual_resume.rs): Deployment patch compatibility Oracle: Literal protocol-incompatible/unknown/unpinned errors; protocol oracle does not validate changed workflow semantic replay. Read, **not executed**.
- [quarantine/restate/crates/worker/src/partition/state_machine/tests/idempotency.rs](../../../quarantine/restate/crates/worker/src/partition/state_machine/tests/idempotency.rs): Stored result on completed keyed invocation Oracle: Known stored123 result expected for repeated idempotency key; scoped to state machine, not external payment/model request exactly once. Read, **not executed**.
- [quarantine/restate/crates/worker/src/partition/leadership/mod.rs](../../../quarantine/restate/crates/worker/src/partition/leadership/mod.rs): Write-time stale attempt fencing Oracle: Distinguishable current/stale effects with pause/resume; reads committed log and asserts only accepted current effects. Direct fault oracle, not physical external cancellation. Read, **not executed**.

## Useful mechanisms

- Durable replay, result identity, state/outbox and explicit invocation lifecycle are separated in source.
- Current-attempt fencing tested by reading actual committed log against distinguishable stale/current effects.
- Query exposes raw journal plus causal event links rather than only aggregate logs.

## Material limits

- External closure/provider/OS effects are not atomically settled or fully audited by durable protocol.
- Deployment protocol compatibility does not prove semantic journal replay compatibility.
- Cancellation appended/pause accepted is not physical external quiescence; retained journal can be purged.

## Arconaut design questions

- Which client operations can consume durable service identity without transferring shared governor responsibilities into Arconaut?
- What replay compatibility, idempotency/reconciliation and outstanding-effect accounting must compiled refit enforce beyond protocol versions?
- Can original core audit retain consumed service journal references even after external retention deletes them?

## Evidence

### Evidence e1

[quarantine/restate/crates/ingress-http/src/handler/service_handler.rs:130–154](../../../quarantine/restate/crates/ingress-http/src/handler/service_handler.rs#L130): Latest target resolution rejects private handler at public ingress.

### Evidence e2

[quarantine/restate/crates/ingress-http/src/handler/service_handler.rs:284–368](../../../quarantine/restate/crates/ingress-http/src/handler/service_handler.rs#L284): Call versus send routing, retained ID/idempotency, delayed schedule and202 acceptance/prior acceptance response.

### Evidence e3

[quarantine/restate/crates/invoker-impl/src/invocation_task/service_protocol_runner_v4.rs:307–375](../../../quarantine/restate/crates/invoker-impl/src/invocation_task/service_protocol_runner_v4.rs#L307): Attempt starts with state/seed/journal metadata then streams stored journal replay from transaction.

### Evidence e4

[quarantine/restate/crates/invoker-impl/src/invocation_task/service_protocol_runner_v4.rs:998–1037](../../../quarantine/restate/crates/invoker-impl/src/invocation_task/service_protocol_runner_v4.rs#L998): SDK run result decoded into raw notification proposal; this follows external closure execution, not atomic third-party settlement.

### Evidence e5

[quarantine/restate/crates/worker/src/partition/state_machine/entries/mod.rs:355–414](../../../quarantine/restate/crates/worker/src/partition/state_machine/entries/mod.rs#L355): Apply notification, append raw journal at exact index/deterministic record timestamp, update invocation status.

### Evidence e6

[quarantine/restate/crates/worker/src/partition/state_machine/entries/set_state_command.rs:35–52](../../../quarantine/restate/crates/worker/src/partition/state_machine/entries/set_state_command.rs#L35): User state mutation written through same state-machine context for keyed service target.

### Evidence e7

[quarantine/restate/crates/worker/src/partition/state_machine/entries/call_commands.rs:96–151](../../../quarantine/restate/crates/worker/src/partition/state_machine/entries/call_commands.rs#L96): Durable call request enqueued into outbox, invocation-ID completion correlated back to caller.

### Evidence e8

[quarantine/restate/crates/worker/src/partition/state_machine/lifecycle/manual_pause.rs:27–97](../../../quarantine/restate/crates/worker/src/partition/state_machine/lifecycle/manual_pause.rs#L27): Manual pause preserves journal, aborts running attempt; suspended pause drops wait set for replay recomputation.

### Evidence e9

[quarantine/restate/crates/worker/src/partition/leadership/leader_state.rs:840–866](../../../quarantine/restate/crates/worker/src/partition/leadership/leader_state.rs#L840): Write-time current-attempt fencing token prevents stale effects entering durable log, not external effect settlement.

### Evidence e10

[quarantine/restate/crates/worker/src/partition/state_machine/lifecycle/cancel.rs:65–115](../../../quarantine/restate/crates/worker/src/partition/state_machine/lifecycle/cancel.rs#L65): Cancel signal appended for invoked/suspended/paused; queued/scheduled terminated; response distinguishes Appended/Done/completed/missing.

### Evidence e11

[quarantine/restate/crates/worker/src/partition/state_machine/lifecycle/manual_resume.rs:68–101](../../../quarantine/restate/crates/worker/src/partition/state_machine/lifecycle/manual_resume.rs#L68): Replacement deployment must exist/support pinned protocol version; semantic handler program equivalence is not checked here.

### Evidence e12

[quarantine/restate/crates/worker/src/partition/state_machine/lifecycle/manual_resume.rs:187–227](../../../quarantine/restate/crates/worker/src/partition/state_machine/lifecycle/manual_resume.rs#L187): Deployment patch resolved at command log position, pinned ID updated and paused/suspended invocation resumed.

### Evidence e13

[quarantine/restate/crates/admin/src/rest_api/query.rs:103–166](../../../quarantine/restate/crates/admin/src/rest_api/query.rs#L103): POST/query executes SQL, JSON or Arrow streaming results; availability/errors/rate limit handled.

### Evidence e14

[quarantine/restate/crates/storage-query-datafusion/src/journal/schema.rs:19–72](../../../quarantine/restate/crates/storage-query-datafusion/src/journal/schema.rs#L19): Journal query exposes indices/targets/raw binary/version2 JSON/append timestamps.

### Evidence e15

[quarantine/restate/crates/storage-query-datafusion/src/journal_events/schema.rs:17–35](../../../quarantine/restate/crates/storage-query-datafusion/src/journal_events/schema.rs#L17): Events carry invocation ID, after-journal index, timestamp/type/event JSON.

### Evidence e16

[quarantine/restate/crates/worker/src/partition/state_machine/lifecycle/purge_journal.rs:39–83](../../../quarantine/restate/crates/worker/src/partition/state_machine/lifecycle/purge_journal.rs#L39): Only completed invocation journal can be dropped; result status survives with length/commands reset.

### Evidence e17

[quarantine/restate/crates/worker/src/partition/state_machine/lifecycle/manual_pause.rs:129–159](../../../quarantine/restate/crates/worker/src/partition/state_machine/lifecycle/manual_pause.rs#L129): Pause state-machine test checks Accepted+abort action and stored Paused status.

### Evidence e18

[quarantine/restate/crates/worker/src/partition/state_machine/lifecycle/manual_resume.rs:355–415](../../../quarantine/restate/crates/worker/src/partition/state_machine/lifecycle/manual_resume.rs#L355): Literal protocol-incompatible/unknown/unpinned replacement errors.

### Evidence e19

[quarantine/restate/crates/worker/src/partition/state_machine/tests/idempotency.rs:142–165](../../../quarantine/restate/crates/worker/src/partition/state_machine/tests/idempotency.rs#L142): Completed idempotent call produces stored literal123 response; this range does not independently count actual external effects.

### Evidence e20

[quarantine/restate/crates/worker/src/partition/leadership/mod.rs:1259–1343](../../../quarantine/restate/crates/worker/src/partition/leadership/mod.rs#L1259): Controlled attempts/pause/stragglers test reads committed log and asserts only current attempt effects appended.

