# restate-sdk-rust

Rust typed service/object/workflow, journaled closure, durable call/wait and ingress APIs

Role: durable workflow/service SDK. Runtime: Rust.

Pinned source: [https://github.com/restatedev/sdk-rust](https://github.com/restatedev/sdk-rust); revision/version `5cd9a3d24d7ddc21f29c8c43074d3d67ca67c163`.

Compiled Tokio service handlers and typed client/context futures around external protocol VM; optional tunneled endpoint transport.

Consumes Restate server/durable state and external effect services; SDK hosts a service endpoint, not an agent/model loop or service governor.

Inspection: context traits/retry rules, RunFuture state machine, durable error interception and real fake-server ingress/tunnel lifecycle tests

Limits of this study: Shared-core/server settlement and full fleet recovery studied separately. No compiled reference or tests ran.

## Actions

### #[service]/#[object]/#[workflow] + #[handler]; Endpoint.bind

Surface: Typed compiled handler/discovery.

Input: Annotated Rust methods/context/input/output

Result: Service endpoint and typed generated clients

Lifecycle: Compiled deployment; durable runtime separate; object serial/exclusive key contract

Authority: Developer/service process; no model modification/approval loop

Evidence: [e4](#evidence-e4), [e5](#evidence-e5), [e8](#evidence-e8).

### ctx.run(closure).name(...).retry_policy(...).await

Surface: Programmable journaled action.

Input: FnOnce async closure with serialized output and retry policy

Result: Recorded typed result/terminal failure

Lifecycle: VM ExecuteRun→closure→propose completion→await; immediate-await restriction; no external atomicity guarantee

Authority: Service caller code/credentials; context operations forbidden inside run

Evidence: [e1](#evidence-e1), [e2](#evidence-e2), [e3](#evidence-e3).

### service_client/object_client/workflow_client request.call/send

Surface: Typed durable RPC/send.

Input: Target method/payload/key; idempotency/scope/delay

Result: Durable result future or invocation/send identity

Lifecycle: Server records/schedules retries; virtual-object waits can deadlock cycles

Authority: Caller/server routing; server owns durable work

Evidence: [e4](#evidence-e4), [e5](#evidence-e5).

### ctx.signal / invocation_handle(id).signal(name).resolve

Surface: Addressed durable one-shot communication.

Input: Invocation ID/name and serialized value

Result: Durable future completion

Lifecycle: Survives host attempt via server; not a bidirectional peer room

Authority: Program/server access to target invocation

Evidence: [e6](#evidence-e6).

### IngressClient.call/send and invocation_handle

Surface: External typed service API.

Input: Ingress URI, request, idempotency key

Result: Decoded result plus invocation ID/follow-up handle

Lifecycle: Request transport is replaceable; server work identity distinct from local client

Authority: External program with ingress authority

Evidence: [e8](#evidence-e8), [e10](#evidence-e10).

### Tunnel drain/overlapping replacement and close

Surface: Endpoint transport lifecycle.

Input: Cloud drain/control and connection sessions

Result: Replacement session and reaped old/new drivers

Lifecycle: Connection handover; not application executable rebuild or unresolved-effect settlement

Authority: Endpoint/Cloud transport owner

Evidence: [e9](#evidence-e9).

### ctx.get/get_keys/set/clear/clear_all

Surface: Typed contextual state API.

Input: Object/workflow key and serialized value

Result: Server-owned typed KV state/read futures

Lifecycle: Object state across invocations; workflow state per run; attempts rebuild locals

Authority: Context type constrains readable/writable scope; server owns storage

Evidence: [e11](#evidence-e11).

## Capabilities

### filesystem

**— — Files** (inspection): Outside this durable workflow/service SDK role: the reference supplies Rust typed service/object/workflow, journaled closure, durable call/wait and ingress APIs rather than an agent execution engine.

### processes

**— — OS programs** (inspection): Outside this durable workflow/service SDK role: the reference supplies Rust typed service/object/workflow, journaled closure, durable call/wait and ingress APIs rather than an agent execution engine.

### code-actions

**S — Code actions** (source): Developer Rust closures compose arbitrary external computations and journal chosen result; no model code-action evaluator.

Evidence: [e1](#evidence-e1), [e2](#evidence-e2).

### persistent-kernel

**— — Kernel** (inspection): Outside this durable workflow/service SDK role: the reference supplies Rust typed service/object/workflow, journaled closure, durable call/wait and ingress APIs rather than an agent execution engine.

### standing-database

**S — Standing DB** (source): Typed server-owned object/workflow get/get_keys/set/clear state; workflow run is writer, other handlers read. No model SQL dispatcher or persistent Rust stack.

Evidence: [e11](#evidence-e11).

### workflow-programming

**L — Workflows** (source): Typed async durable programs and generated clients compose; run must be immediately awaited and cannot include context operations to preserve deterministic replay.

Evidence: [e2](#evidence-e2), [e4](#evidence-e4).

### multi-model

**— — Models** (inspection): Outside this durable workflow/service SDK role: the reference supplies Rust typed service/object/workflow, journaled closure, durable call/wait and ingress APIs rather than an agent execution engine.

### live-collaboration

**S — Peer chat** (source): Invocation-addressed one-shot signal primitive can compose participant communication; no live model room.

Evidence: [e6](#evidence-e6).

### concurrent-work

**L — Concurrency** (source): Durable sends/calls and signal/select primitives compose, but virtual-object key seriality can deadlock cycles and run immediate-await bounds composition.

Evidence: [e2](#evidence-e2), [e5](#evidence-e5), [e6](#evidence-e6).

### steering-interrupt

**L — Steer/interrupt** (source): Run state handles VM cancellation; transport drain reaps sockets/tasks. Neither proves user external effect/remote request physically canceled.

Evidence: [e1](#evidence-e1), [e9](#evidence-e9).

### turn-redefinition

**— — Turn program** (inspection): Outside this durable workflow/service SDK role: the reference supplies Rust typed service/object/workflow, journaled closure, durable call/wait and ingress APIs rather than an agent execution engine.

### compaction

**— — Compaction** (inspection): Outside this durable workflow/service SDK role: the reference supplies Rust typed service/object/workflow, journaled closure, durable call/wait and ingress APIs rather than an agent execution engine.

### context-repair

**— — Repair** (inspection): Outside this durable workflow/service SDK role: the reference supplies Rust typed service/object/workflow, journaled closure, durable call/wait and ingress APIs rather than an agent execution engine.

### original-audit

**L — Original audit** (source): Journal chosen serialized result and failure; does not capture every closure-internal/provider/OS IO or semantic original context.

Evidence: [e1](#evidence-e1), [e2](#evidence-e2).

### audit-query

**— — Audit query** (inspection): Outside this durable workflow/service SDK role: the reference supplies Rust typed service/object/workflow, journaled closure, durable call/wait and ingress APIs rather than an agent execution engine.

### hot-change

**— — Hot change** (inspection): Outside this durable workflow/service SDK role: the reference supplies Rust typed service/object/workflow, journaled closure, durable call/wait and ingress APIs rather than an agent execution engine.

### rebuild-continuity

**L — Rebuild continuity** (source): Replay contract supports new service attempts and tunnel handover supports transport replacement; no agent context/executable rebuild continuity mechanism established.

Evidence: [e1](#evidence-e1), [e2](#evidence-e2), [e9](#evidence-e9).

### remote-services

**S — Remote** (source): Typed server client and replaceable ingress consume external durable service; no governor responsibility assumed.

Evidence: [e4](#evidence-e4), [e8](#evidence-e8), [e10](#evidence-e10).

### self-improvement

**— — Self-improve** (inspection): Outside this durable workflow/service SDK role: the reference supplies Rust typed service/object/workflow, journaled closure, durable call/wait and ingress APIs rather than an agent execution engine.

### complaints

**— — Complaints** (inspection): Outside this durable workflow/service SDK role: the reference supplies Rust typed service/object/workflow, journaled closure, durable call/wait and ingress APIs rather than an agent execution engine.

### authority

**S — Authority** (source): Developer supplied closures execute under service process authority; SDK has no model approval/policy program.

Evidence: [e1](#evidence-e1), [e2](#evidence-e2).

### evaluation

**S — Evaluation** (source): Captured ingress requests and fake HTTP2 drain/close tests attack wire/lifetime faults; no arbitrary effect crash-settlement oracle inspected.

Evidence: [e9](#evidence-e9), [e10](#evidence-e10).

### time-order

**L — Time/order** (source): Stable invocation/key order and retry policy; docs explicitly warn actual attempts/duration may exceed limits due to closure/result settlement gap.

Evidence: [e3](#evidence-e3), [e5](#evidence-e5).

## Inspected test oracles

- [quarantine/restate-sdk-rust/tests/tunnel_handover.rs](../../../quarantine/restate-sdk-rust/tests/tunnel_handover.rs): HTTP2 overlap/drain/close transport lifetime Oracle: Fake Cloud sessions require drain acceptance/distinct IDs and await owned driver completion within timeout; transport liveness, not harness rebuild. Read, **not executed**.
- [quarantine/restate-sdk-rust/tests/ingress_client.rs](../../../quarantine/restate-sdk-rust/tests/ingress_client.rs): Typed ingress wire requests and retained responses Oracle: Deterministic capture transport permits literal route/header/body/error oracles; server durability/external side effects remain separate. Read, **not executed**.

## Useful mechanisms

- Explicit state machine separates closure execution, result proposal and replay wait.
- Typed invocation identity and direct fake transport/drain lifecycle oracles.

## Material limits

- External side effect and journal completion are not one atomic transaction.
- Immediate-await restriction and serial object cycles limit composition; connection handover is not self-refit.

## Arconaut design questions

- How should replay compatibility and side-effect settlement be represented when replacing a compiled workflow program?
- Can service/tunnel identity remain independent of a refitting consumer while all that consumer’s active work is accounted for?

## Evidence

### Evidence e1

[quarantine/restate-sdk-rust/src/endpoint/context.rs:1016–1118](../../../quarantine/restate-sdk-rust/src/endpoint/context.rs#L1016): Run future executes closure only on VM ExecuteRun, proposes serialized result then waits VM result; cancellation branch explicit.

### Evidence e2

[quarantine/restate-sdk-rust/src/context/mod.rs:898–973](../../../quarantine/restate-sdk-rust/src/context/mod.rs#L898): Journaled result contract, no nested context use in run, and immediate-await restriction to avoid replay nondeterminism.

### Evidence e3

[quarantine/restate-sdk-rust/src/context/run.rs:28–115](../../../quarantine/restate-sdk-rust/src/context/run.rs#L28): Named run/retry options; actual attempts/duration may exceed configured because closure executes before server receives result.

### Evidence e4

[quarantine/restate-sdk-rust/src/context/mod.rs:413–471](../../../quarantine/restate-sdk-rust/src/context/mod.rs#L413): Typed service/object/workflow call keys, idempotency scope and server-owned journal/retry contract.

### Evidence e5

[quarantine/restate-sdk-rust/src/context/mod.rs:566–608](../../../quarantine/restate-sdk-rust/src/context/mod.rs#L566): Virtual-object serial ordering and explicit cyclic deadlock limits.

### Evidence e6

[quarantine/restate-sdk-rust/src/context/mod.rs:830–867](../../../quarantine/restate-sdk-rust/src/context/mod.rs#L830): Named signal targets invocation ID/name; one-shot durable promise, composable select.

### Evidence e7

[quarantine/restate-sdk-rust/src/endpoint/futures/durable_future_impl.rs:25–59](../../../quarantine/restate-sdk-rust/src/endpoint/futures/durable_future_impl.rs#L25): VM/internal future error marks context failed and returns Pending for handler state interception.

### Evidence e8

[quarantine/restate-sdk-rust/README.md:58–86](../../../quarantine/restate-sdk-rust/README.md#L58): Typed ingress client call and returned invocation handle contract.

### Evidence e9

[quarantine/restate-sdk-rust/tests/tunnel_handover.rs:24–111](../../../quarantine/restate-sdk-rust/tests/tunnel_handover.rs#L24): Fake Cloud HTTP2 session verifies distinct IDs/drain protocol and owned socket/driver task completion.

### Evidence e10

[quarantine/restate-sdk-rust/tests/ingress_client.rs:38–98](../../../quarantine/restate-sdk-rust/tests/ingress_client.rs#L38): Capture fake transport records exact requests and queued responses; no live server dependency in this oracle.

### Evidence e11

[quarantine/restate-sdk-rust/src/context/mod.rs:1067–1148](../../../quarantine/restate-sdk-rust/src/context/mod.rs#L1067): Typed get/get_keys/set/clear traits forward to context; only objects/workflows carry writable KV scope.

