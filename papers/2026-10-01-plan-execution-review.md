# First-core plan review: ordering and lifecycle evidence

2026-10-01. Independent adversarial review of `docs/IMPLEMENTATION_PLAN.md` and
`docs/TESTING_PLAN.md`, following the resolved core-lifetime specification review.
No product code, imported program or test ran. Scope is conceptual ownership,
dependency order and direct evidence for Lua/native/refit/control and terminal/IPC
integration. This review does not select dependencies or demand general research.

## Judgment

The short path U0 → U1 → U2 → U3 → U4 → U6 is coherent. U0 can start with owned
primitives and actual installed compiler tooling; it needs neither an unselected
Lua runtime nor provider/transport choices. One narrow IPC acceptance gap should
be added to the plan before execution. It does not require a new unit or changing
the order.

## P1 — IPC has an implementation owner but insufficient wire-boundary tests

**Medium; acceptance gap in U3.** IMPLEMENTATION_PLAN lines 25–27, 38 and 64–68
correctly give local versioned, bounded custodian IPC to U3 rather than letting
each client invent a transport. TESTING_PLAN's T3 checks process custody, duplicate
effects and stale epochs, but does not explicitly exercise malformed/fragmented
IPC frames. Its decoder fuzz list at lines 148–150 names journal, context, provider
and input decoders; the central IPC decoder is absent. The general invalid-length
API examples do not establish correct behavior when length/version/identity fields
arrive incrementally over a real socket.

Without a direct wire oracle, the in-process effect tests could pass while client
framing produces extra invocations, unbounded allocation, parser desynchronization,
or unintended dispatch after a connection drops halfway through a frame. The TUI,
participant and outpost all depend on this same seam.

Add to U3/T3: real local client fixtures fragment headers/payloads, coalesce several
frames, send unsupported versions/oversized lengths/malformed authorship or epoch,
and disconnect during incomplete request and reply. Count external effects and
assert that invalid/incomplete requests do not dispatch, accepted requests retain
their identities across reply loss, and message limits actually bound allocation/
buffering. Include this decoder in the bounded fuzz targets with the same rejection
and dispatch predicates. Do not add a second protocol test framework. Exact framing
details remain in U3's bounded sub-plan.

## Ordering and ownership reviewed as sound

- U1 owns durable admission and retained semantic decisions; U3 implements the
  actual effect/IPC/process seam against them. Injected effects in U1 are suitable
  for durability cuts, and the real U3 counters subsequently challenge integration.
  The plan explicitly forbids splitting custody/admission between incompatible owners.
- U2 can establish revisions/CAS, branching and request bindings without a live
  provider or Lua. U4 then supplies real model-programmable execution; U6 checks
  the final provider adapter against those same source/continuation contracts.
  This avoids treating a fake codec as real provider qualification.
- U4 owns error/yield/rooted continuation/module/generation behavior together.
  T4 attacks allocating preparation, native versus Lua control transfer, GC,
  finalization and pending-root/descendant progress. U5 adds actual compiled code
  pins and unload checks; delayed callbacks are actually invoked rather than
  trusting the pin counter's own report.
- U5 may proceed independently of U6/U7 after U4. Its comparison against acquired
  RCC++/cr/Godot/JENOVA evidence is explicit. The owned-loader proposal is not a
  silent dependency adoption, and failed/destructive migrations have different
  expected outcomes.
- U7 uses the shared U3 protocol and qualified Lua behavior. PTY tests, real
  submission identities, queue/interrupt semantics, restoration and surviving
  core work cover materially different faults from render screenshots. Terminal
  profiles are appropriately chosen before that unit, not guessed here.
- U8 requires the independent usable outpost and provider/client first. Its
  ownership scope includes drain, pause, handoff, failed build/start, return and
  the same actual worker heap. T8 checks open-pipe preservation, owner fencing,
  model-initiated controls, failed settlement and a continuing second consumer.
  Custodian-image replacement remains separately qualified and may visibly block;
  the plan does not claim arbitrary live parentage/VM migration.
- U1 establishes local complaint/control access; U9 completes real external sink
  reconciliation and experiment ergonomics. The milestone requires both actual
  sink inspection and continuing source work, rather than merely a filed-looking
  complaint or a chat-only prototype.

## Evidence limits are appropriately explicit

Simulator schedules, real process crashes and stronger persistence-fault claims
are distinguished. Qualification does not infer Mac process semantics from Linux
or success from unsupported instrumentation. Runtime/provider/transport adoption
is discussed before its dependent unit; ordinary owned work need not await those
choices. Fleet mutation remains fleet-only and cannot be claimed completed when
unavailable. No additional library, general survey or formal ceremony is required
to resolve this review.

## Rereview

2026-10-01. P1 resolved by focused reread. T3–T5 now requires real local IPC clients
to fragment/coalesce frames, send unsupported versions/oversized lengths/invalid
authorship and epochs, and disconnect mid-request or after admission before reply.
Its oracle independently counts effects, forbids dispatch for incomplete/invalid
requests, retains accepted invocation identity through reply loss, and checks actual
allocation/buffering limits. The bounded fuzz target list includes custodian IPC.
U3's qualification text assigns those same checks to the owning implementation
unit. No extra harness or ordering change was introduced.

No remaining plan finding in this review's scope. Owned U0 can proceed after the
root closes plan review and writes its bounded sub-plan; runtime qualification,
direct red/green checks and code review remain required during execution.
