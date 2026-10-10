# Instrumentation: raw emission, separate recording and transformation

2026-10-10. Current operator direction and revised research sketch.
Implementation still requires discussion and approval under BLACKBIRD.md.

## The operator's boundary

Blackbird emits raw metric events and gets them out of its main processes as
quickly as physically possible, losslessly. A separate recorder preserves the
raw records on disk. A tertiary process may transform the preserved records.
Operations and evals consume the resulting metrics; neither owns raw emission.

```text
Blackbird: capture raw event -> transport
    -> separate recorder: append raw event to disk
        -> tertiary process: transforms -> operations / eval consumers
```

Blackbird's instrumentation performs capture and transport only. No duration
subtraction, counts, sums, histograms, filtering, sampling of emitted events,
unit conversion, dimensional projection, normalization, compression, string
formatting, or programmable measurement processing happens on the writer path.
These restrictions also apply to a recorder before raw preservation: its job is
to stack raw records, not interpret them. Transport framing and publication are
necessary mechanics, not a license to introduce a measurement-processing layer.
Their actual instruction, byte-copy and synchronization costs must be accounted
for explicitly. "Zero moving parts" means the smallest capture/transport
machinery, not a literal claim that emitting a record costs no instructions.

Raw begin/end clock readings are emitted as separate events with the existing
activity identity. Duration is calculated in the tertiary process after disk
preservation. Existing product-owned state may be copied as an observation;
instrumentation adds no owner-maintained metric accumulators. A native quantity
keeps its type and original unit. Error, refusal, cancellation, unknown and
partial-progress facts are emitted as observed, before later processing can
reshape their presentation.

The rejected [earlier proposal](../papers/2026-10-10-instrumentation-producer-transforms-superseded.md)
put registered instruments, projections and aggregates in the producer. Its
source census remains research input; that architecture is superseded.
OpenTelemetry and Prometheus no longer guide this emission design. They are
historical study material, not dependencies or an intended runtime framework.

## Older primary references and what they contribute

[New study](../papers/2026-10-10-instrumentation-raw-emission-study.md): Multics
instrumentation (1969/1970), Research Unix processes/pipes (1974/1978 and V7
1979 source), Lampson's interfaces/background work/logging (1983), Lamport's
FIFO publication and progress contracts (1983), Berkeley trace collection and
analysis (1985), Cedar group commit (1987), and BSD raw trace records plus a
separate decoder (1988–1990 source).

These references explain mechanisms and limits, rather than supplying a ready
implementation. Multics' external observation channel is useful; its on-system
meters and overwriting history are not our emission model. BSD separates record
capture from decoding, but its traced-path allocation/file I/O and disabling
tracing on error fail this requirement. Cedar's tolerated pre-flush crash loss
is not imported. Lamport's abstract atomic actions are not modern C++ atomics.
No reference was built or executed, and no dependency was selected.

## Raw surfaces

The previous source study enumerated all 46 C++ source units and seven shipped
Lua programs, 60 product surfaces and nine system domains. The
[product census](../papers/2026-10-10-instrumentation-product-surfaces.md) and
[system census](../papers/2026-10-10-instrumentation-system-surfaces.md) locate the
actual owners and failure paths. Their producer-side aggregation recommendations
are superseded. The revised implementation plan must enumerate exact raw record
fields at those sites, rather than treating derived metric names as payloads.

| Surface | What Blackbird emits raw | What the tertiary process may derive |
| --- | --- | --- |
| Occurrence/outcome | Named boundary, existing operation identity, actual result/error/unknown | Counts, outcome populations and rates |
| Synchronous phase | Separate begin/end events, raw clock readings/domain, existing call identity | Durations and distributions |
| Asynchronous work | Queue/start/terminal events with existing job/worker/attempt identity | Queue, execution, drain and settlement durations |
| State/publication | Observed old/new state or revisions, exact publication/refusal boundary | Transition counts, residence and state populations |
| Transfer/work | Actual amount returned by the operation and its stage, including partial progress | Byte/work totals and throughput |
| Ownership/resource | Actual acquire/release/resize facts and existing owner identity | Live population, churn and high water |
| Flow/handoff | Enqueue/dequeue/delivery/cancel facts with existing transfer identity | Residence, delay, deduplication analysis |
| Resource sample | Unchanged acquired value, clock readings, method and acquisition failure | Deltas, resource intervals and summaries |
| Native/Lua custom event | Caller-supplied raw typed payload at an explicit site | Custom interpretations outside Blackbird |
| Transport/recorder failure | Raw failed-ingest/recording outcome through an independent failure path | Operations interpretation and diagnosis |

Provider request and response are distinct events. Raw externally reported usage
retains reporting/attempt identity and is not normalized or accumulated by
instrumentation. Repeated cumulative or revised reports are interpreted later.
Resource acquisition belongs outside the main processes wherever possible.
A source fact that only an owner can observe still needs a minimal owner hook.
Instrumentation-only scripting callbacks and a dynamic metric registry do not
belong in the Blackbird process.

## Capture and transport: candidates to study directly

Compare a raw-record pipe/stream to a preallocated shared-memory producer/recorder
channel. Prefer the simplest mechanism that meets the actual throughput, latency
and failure requirements. A pipe buys owned OS buffering and simple lifetime
behavior at syscall/copy cost. Shared memory can avoid a per-event syscall but
needs explicit publication, overwrite prevention, producer/recorder lifetime,
notification and reclamation. It is not selected just because it sounds faster.

The critical path may obtain facts unavailable later, copy raw fields into an
owned record and publish it. No generic objects, metric lookup, arbitrary callbacks
or downstream computation are added. Fixed site descriptions and decoding
metadata can be prepared outside event emission and preserved with the records.
A pointer into a soon-to-expire product buffer is not a lossless handoff.
Record layout must not leak padding or assume C++ object layout is a wire format.

A recorder drains records and performs sequential raw appends. It can group raw
records for disk writes without transforming them. A tertiary reader sees only
the preserved prefix; it cannot slow the writer through synchronous callbacks.
Raw records must enter audit storage under the operator's existing requirement;
this sketch does not select a second authoritative metrics journal or silently
move all current audit code into a daemon. The actual audit integration needs
design alongside nls.10 and causal identity alongside nls.9.

## Losslessness and disk preservation

Publication, transfer into recorder ownership, append and durable flush are
different boundaries. Define each before claiming losslessness. A buffer or OS
write returning success does not itself establish disk persistence.

The design must preserve every acquired record, including failed work and failed
ingestion. On ingest failure, write a failure event without recursively calling
the same failing emitter, and retain the original pending record. An error event
does not replace the failed record. If disk writes fail too, the design must retain
pending records and state the failure plainly; it cannot manufacture durability.

Finite transport storage cannot absorb an indefinitely stalled recorder. Work
must not overwrite/drop pending records or make metrics smaller by aggregating
on the writer. Study reservation and backpressure at safe owner boundaries,
short writes, recorder restart, process crashes and host crashes. Capacity,
acknowledgement/reclamation and full-buffer behavior remain mechanism decisions
for discussion. No lossy default or arbitrary capacity/TTL is selected.

## Performance and the next design checkpoint

Account for the producer's actual instructions, copies, allocations, locks,
atomic publication, clock reads, syscalls and wakeups. Separate normal emission
from full-buffer and failure handling. Evaluate producer latency, recorder drain
and disk-preservation lag together; a cheap enqueue alone does not demonstrate
fast lossless recording. Preserve raw identity/order in a direct byte-for-byte
oracle through slow-reader, burst, short-I/O and restart cases.

The next checkpoint must return the concrete record layouts, transport comparison,
recorder/audit disk boundary, failure/backpressure behavior and an implementation
plan. Then obtain operator approval before writing product code. No evaluation
campaign, benchmark, source implementation or full test suite was run for this
research/document correction.
