# restate-sdk-python

Python context APIs for journaled code, state, calls, waits and invocation identity

Role: durable workflow/service SDK. Runtime: Python, Rust native VM wrapper.

Pinned source: [https://github.com/restatedev/sdk-python](https://github.com/restatedev/sdk-python); revision/version `ad4ea7c0a60b5745681a809fe57656bec0939c38`.

Python ASGI service invocation runs through a protocol VM; async run actions or sync actions in a thread executor. Server owns durable storage/replay/scheduling.

Consumes Restate server and user external systems; SDK owns only invocation attempt/client resources, not an agent loop or shared service governor.

Inspection: context contracts and server_context run/replay/proposal/state/RPC/teardown; direct retry and disconnect tests

Limits of this study: Core protocol VM and server durability studied separately; SDK source alone does not certify settlement of arbitrary external effects.

## Actions

### ctx.run / ctx.run_typed(name, action, options, *args)

Surface: Programmable journaled action.

Input: Sync/async function, args/serde and retry overrides

Result: Serialized result or terminal failure via VM durable handle

Lifecycle: Recorded result replay bypasses closure; external effect precedes result proposal and may repeat if not durably settled

Authority: Developer/client programs execute arbitrary action; host credentials/OS authority

Evidence: [e1](#evidence-e1), [e2](#evidence-e2).

### ctx.get/set/state_keys/clear/clear_all

Surface: Object/workflow structured state.

Input: Key names and serialized values

Result: Durable contextual key/value state

Lifecycle: Server VM state/journal; no persistent Python locals across attempts

Authority: Context capabilities; server owns storage

Evidence: [e3](#evidence-e3).

### ctx.service_call/object_call/workflow_call and *_send

Surface: Typed durable RPC.

Input: Handler, serialized arg, object/workflow key, idempotency/scope/limit and optional delay

Result: Durable call future or send handle/invocation identity

Lifecycle: Server schedules/correlates calls; delayed/background send independent of local await

Authority: Calling handler/server service routing authority

Evidence: [e4](#evidence-e4).

### ctx.awakeable/promise/signal; resolve/reject

Surface: Durable external completion primitive.

Input: Opaque awakeable ID or invocation ID/name and payload

Result: Future/promise resolved result

Lifecycle: Server retains pending wait; does not keep Python process stack alive

Authority: Client/external signal sender with endpoint authority

Evidence: [e5](#evidence-e5), [e8](#evidence-e8).

### ctx.cancel_invocation / attach_invocation

Surface: Invocation control/follow-up.

Input: Invocation ID

Result: Cancellation command or future attached to result

Lifecycle: Protocol cancel does not prove arbitrary external thread/side effect stopped

Authority: Context invoker; server controls durable invocation

Evidence: [e5](#evidence-e5), [e6](#evidence-e6), [e9](#evidence-e9).

### ctx.time/random/uuid; request.attempt_finished_event

Surface: Replay-stable time/identity and attempt lifecycle.

Input: Context and invocation seed

Result: Journaled wall timestamp, deterministic RNG/UUID, local cleanup signal

Lifecycle: Stable per invocation replay; attempt signal closes/cancels client resources, no universal quiescence

Authority: Handler/client context

Evidence: [e8](#evidence-e8), [e9](#evidence-e9), [e7](#evidence-e7).

## Capabilities

### filesystem

**— — Files** (inspection): Outside this durable workflow/service SDK role: the reference supplies Python context APIs for journaled code, state, calls, waits and invocation identity rather than an agent execution engine.

### processes

**— — OS programs** (inspection): Outside this durable workflow/service SDK role: the reference supplies Python context APIs for journaled code, state, calls, waits and invocation identity rather than an agent execution engine.

### code-actions

**S — Code actions** (source): Arbitrary sync/async closures inside run compose external tools; durable serialized result rather than resident process memory.

Evidence: [e1](#evidence-e1), [e2](#evidence-e2).

### persistent-kernel

**— — Kernel** (inspection): Outside this durable workflow/service SDK role: the reference supplies Python context APIs for journaled code, state, calls, waits and invocation identity rather than an agent execution engine.

### standing-database

**S — Standing DB** (source): Context get/set/keys/clear exposes server-owned object/workflow key-value state; no generic SQL or agent-facing database supplied.

Evidence: [e3](#evidence-e3).

### workflow-programming

**S — Workflows** (source): Python async programs compose VM run/call/send/wait/state primitives; replay constraints and external idempotency remain caller responsibilities.

Evidence: [e1](#evidence-e1), [e3](#evidence-e3), [e4](#evidence-e4), [e5](#evidence-e5).

### multi-model

**— — Models** (inspection): Outside this durable workflow/service SDK role: the reference supplies Python context APIs for journaled code, state, calls, waits and invocation identity rather than an agent execution engine.

### live-collaboration

**S — Peer chat** (source): Addressed one-shot signal and external awakeable are communication primitives, not a live model-peer room.

Evidence: [e5](#evidence-e5), [e8](#evidence-e8).

### concurrent-work

**L — Concurrency** (source): Multiple durable call/send handles compose, but sync run executes in a thread; cancellation of its future cannot establish thread/remote effect settlement.

Evidence: [e2](#evidence-e2), [e4](#evidence-e4), [e6](#evidence-e6).

### steering-interrupt

**L — Steer/interrupt** (source): Cancel command, attempt-finished event and local task cancellation; inspected paths do not wait for arbitrary third-party activity to physically end.

Evidence: [e5](#evidence-e5), [e6](#evidence-e6), [e7](#evidence-e7), [e9](#evidence-e9).

### turn-redefinition

**— — Turn program** (inspection): Outside this durable workflow/service SDK role: the reference supplies Python context APIs for journaled code, state, calls, waits and invocation identity rather than an agent execution engine.

### compaction

**— — Compaction** (inspection): Outside this durable workflow/service SDK role: the reference supplies Python context APIs for journaled code, state, calls, waits and invocation identity rather than an agent execution engine.

### context-repair

**— — Repair** (inspection): Outside this durable workflow/service SDK role: the reference supplies Python context APIs for journaled code, state, calls, waits and invocation identity rather than an agent execution engine.

### original-audit

**L — Original audit** (source): Journal serializes chosen action results/state/calls and handler output/error, not every external/provider request, stdout or action internal effect.

Evidence: [e1](#evidence-e1), [e2](#evidence-e2), [e3](#evidence-e3), [e4](#evidence-e4).

### audit-query

**— — Audit query** (inspection): Outside this durable workflow/service SDK role: the reference supplies Python context APIs for journaled code, state, calls, waits and invocation identity rather than an agent execution engine.

### hot-change

**— — Hot change** (inspection): Outside this durable workflow/service SDK role: the reference supplies Python context APIs for journaled code, state, calls, waits and invocation identity rather than an agent execution engine.

### rebuild-continuity

**L — Rebuild continuity** (source): Service can replay recorded result via fresh handler attempt; SDK does not rebuild executable/transfer model context or guarantee changed code replay compatibility.

Evidence: [e1](#evidence-e1).

### remote-services

**S — Remote** (source): Consumes durable server and external function services with explicit IDs/keys/serde; shared governor remains separate.

Evidence: [e4](#evidence-e4), [e5](#evidence-e5).

### self-improvement

**— — Self-improve** (inspection): Outside this durable workflow/service SDK role: the reference supplies Python context APIs for journaled code, state, calls, waits and invocation identity rather than an agent execution engine.

### complaints

**— — Complaints** (inspection): Outside this durable workflow/service SDK role: the reference supplies Python context APIs for journaled code, state, calls, waits and invocation identity rather than an agent execution engine.

### authority

**S — Authority** (source): Developer-supplied actions execute under service process authority; no approval policy or host model grant machinery supplied.

Evidence: [e2](#evidence-e2).

### evaluation

**S — Evaluation** (source): Real server retry test counts attempts; disconnect fake protocol test verifies bounded no-hang behavior. Neither checks external-effect duplication crash window.

Evidence: [e10](#evidence-e10), [e11](#evidence-e11).

### time-order

**L — Time/order** (source): Journaled wall time/seeded identity, delayed sends and wall-clock retry-duration accounting; no monotonic external-effect deadline or refit pause clock.

Evidence: [e2](#evidence-e2), [e4](#evidence-e4), [e8](#evidence-e8).

## Inspected test oracles

- [quarantine/restate-sdk-python/tests/servercontext.py](../../../quarantine/restate-sdk-python/tests/servercontext.py): Retry-after versus terminal failure Oracle: Real server service test literal attempts==2; helper uses latest image, not pinned server. No action-success-before-journal crash oracle here. Read, **not executed**.
- [quarantine/restate-sdk-python/tests/disconnect_hotloop.py](../../../quarantine/restate-sdk-python/tests/disconnect_hotloop.py): Receive-channel disconnect hot loop/no-hang Oracle: Fake ASGI stream returns known event sequence; wait_for and literal disconnect checks bound liveness fault. Not run. Read, **not executed**.

## Useful mechanisms

- Explicit journal result versus local action execution and invocation/attempt identity.
- Protocol disconnect/retry tests attack actual lifecycle faults.

## Material limits

- Durability is external server composition; arbitrary side effects can occur before result settlement.
- Attempt-finished or canceled executor future is not physical external quiescence.

## Arconaut design questions

- Which external actions require idempotency/reconciliation before a durable result can safely support replay?
- Can refit account for executor threads and external calls rather than treating attempt-finished as enough?

## Evidence

### Evidence e1

[quarantine/restate-sdk-python/python/restate/server_context.py:916–984](../../../quarantine/restate-sdk-python/python/restate/server_context.py#L916): Run schedules action only when VM does not replay result; returns durable future handle.

### Evidence e2

[quarantine/restate-sdk-python/python/restate/server_context.py:846–912](../../../quarantine/restate-sdk-python/python/restate/server_context.py#L846): Async action or executor-thread action runs before success/failure proposal; serialized result, wall-clock attempt duration and retry overrides.

### Evidence e3

[quarantine/restate-sdk-python/python/restate/server_context.py:768–807](../../../quarantine/restate-sdk-python/python/restate/server_context.py#L768): VM state reads/writes, encoded values and request/attempt/idempotency identity.

### Evidence e4

[quarantine/restate-sdk-python/python/restate/server_context.py:993–1061](../../../quarantine/restate-sdk-python/python/restate/server_context.py#L993): Handler/serde discovery and delayed send with key/idempotency/scope/limit, underlying protocol VM dispatch.

### Evidence e5

[quarantine/restate-sdk-python/python/restate/server_context.py:1230–1277](../../../quarantine/restate-sdk-python/python/restate/server_context.py#L1230): Awakeable ID+future, promise, cancel and attach invocation primitives.

### Evidence e6

[quarantine/restate-sdk-python/python/restate/server_context.py:420–448](../../../quarantine/restate-sdk-python/python/restate/server_context.py#L420): Attempt task list cancels futures, clears list; no wait for underlying executor thread.

### Evidence e7

[quarantine/restate-sdk-python/python/restate/server_context.py:603–646](../../../quarantine/restate-sdk-python/python/restate/server_context.py#L603): Teardown flushes protocol output, waits input close, closes output; finished event then cancels tasks.

### Evidence e8

[quarantine/restate-sdk-python/python/restate/server_context.py:827–843](../../../quarantine/restate-sdk-python/python/restate/server_context.py#L827): Addressed signal and stable seeded UUID/random; time implemented as journaled wall-time action.

### Evidence e9

[quarantine/restate-sdk-python/python/restate/context.py:118–127](../../../quarantine/restate-sdk-python/python/restate/context.py#L118): Attempt-finished resource-cleanup contract, not guarantee arbitrary third-party call physically ended.

### Evidence e10

[quarantine/restate-sdk-python/tests/servercontext.py:85–112](../../../quarantine/restate-sdk-python/tests/servercontext.py#L85): Server integration retry-after/terminal-error test counts exactly two attempts; latest container image not pinned by test helper.

### Evidence e11

[quarantine/restate-sdk-python/tests/disconnect_hotloop.py:38–74](../../../quarantine/restate-sdk-python/tests/disconnect_hotloop.py#L38): Fake receive channel checks finite prompt disconnect result after drained queue; concrete no-hang oracle.

