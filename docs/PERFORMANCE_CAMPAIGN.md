# Performance campaign: every action is a workload

Operator2026-10-07: profile autodev and persist its compute/memory/I/O evidence.
Generalize what the harness does, study excellent programs doing that same kind
of work, and derive performance/reliability improvements from actual mechanisms.
This is a measurement and comparative engineering campaign, not a new library
selection or an assurance gate on unrelated development.

## Capture first

Use [autodev collection](AUTODEV_PROFILING.md) and the existing
[finite native profilers](PROFILING.md). A capture must retain build/executable,
process identity, clocks/units, cadence, workload/run, observation status and raw
measurements. Keep private traces in ignored context, with source/configuration
and useful public summaries in Git. Missing data stays unavailable, not zero.

Continuous OS counters characterize each observed process and storage growth.
Finite stack/CPU/allocation/system windows identify mechanisms. The current W0
lane began before continuous capture was attached: retain that start gap. It used
a release snapshot, so complete allocation instrumentation is not claimed;
future profiled lanes use the refreshed optimized/symbolized profile generation,
whose profile-only signing enables the available allocation backend.
That attempt is stopped;520 resource observations plus complete CPU/system/stack
windows are retained. New autodev commands use `scripts/autodev-profiled`, with
automatic counters and finite stack samples. Allocation, network-byte and native
action attribution remain separate coverage, not implied by that wrapper.

External observations cannot precisely attribute every native action or every
short-lived child. Add a separately scoped native action-span unit: identities
join to existing audit request/call/effect/turn records; monotonic wall and CPU
bounds, bytes processed, allocation/copy work and outcome are observations.
Do not duplicate retained payloads to obtain metrics or send telemetry services
by default. Keep expensive detailed allocation tracing in finite development
windows; measure disabled/enabled costs on the same workload.

## Initial action map and reference questions

| Harness action | Workload/cost to isolate | Reference mechanisms to study | Reliability question |
| --- | --- | --- | --- |
| Read and frame streams | bytes per syscall, framing/UTF-8 progress, peak buffered bytes | Existing libvterm/parser corpus; bounded C stream parsers | Fragmented/malformed input, EOF and capacity cannot strand state. |
| Parse/build/serialize JSON | cycles or CPU ns per byte, allocations, copies, escaped/binary edge cases | [yyjson](https://github.com/ibireme/yyjson), [simdjson](https://github.com/simdjson/simdjson) | Preserve exact required representation and detect errors; benchmark equivalent APIs/data. |
| Retain audit/effect records | bytes written per useful byte, sync cadence, append latency/tails | [SQLite WAL](https://www.sqlite.org/wal.html), existing Restate journal corpus | Acknowledgment/crash boundaries and uncertain effects remain truthful; throughput cannot erase durability. |
| Replay and project context | indexed lookup/scanned bytes, history scaling, materialization/copy counts | Existing retained-state corpus; indexed logs and streaming projections | Original provenance, revision mismatch and recovery fences remain intact. |
| Assemble provider request | repeat materialization, time/byte, transient/retained heap | Streaming serializers, scatter/gather output patterns | Exact audited request and effect linkage survive streaming and failure. |
| Buffer/channel/dispatch work | throughput, wakeups, backlog, enqueue-to-start latency | [Aeron](https://github.com/aeron-io/aeron), bounded C queues; existing process/poll source | Ordering, backpressure, cancellation and overload have explicit policies. Study source patterns, do not adopt another runtime. |
| Spawn/collect programs | startup, resource attribution, output drainage, leaked descendants | Existing Child/poll machinery and OS process APIs | Timeout, partial output, escaped/remote effects and reaping are distinct facts. |
| Await provider/remote services | waiting vs active CPU; retries, actual response bytes/usage | Existing transport source, qualified native retry path | Transport loss cannot trigger replay of uncertain tool effects. Inference/provider work is not local CPU time. |
| Compose/diff/paint TUI | touched cells, emitted bytes, allocation/work per update, latency distribution | Existing native renderer/notcurses/libvterm studies | Backpressure, Unicode footprints, resize and restoration remain correct. |
| Allocate/retain payloads | resident/physical footprint, peak transient bytes, live-set growth | Existing waste audit; region/pool/shared immutable payload patterns | Lifetime correctness and bounded growth survive error/cancellation. |
| Schedule/settle workflow tasks | work per transition, timer/queue scaling, fairness/tail latency | Workflow matrix's Temporal/Restate/Windmill source seams | Completion, join, restart and side-effect uncertainty have honest state. |

These are comparative reference candidates, not established winners for our
workloads. Third-party performance claims are hypotheses until reproduced with
matched input, hardware, representation, correctness and durability requirements.
JVM/JS/Python source may teach patterns; it does not select those runtimes.

## First engineering orders

P0: automatic persistent capture on each new autodev launch, process-tree and
profiler-cost separation, one finite real-run sample/trace, retained failures.
P1: native action attribution spanning retained request/call/effect IDs; direct
oracle checks timing/byte provenance, short-lived child resource settlement and
low-cost disabled path. Define exact coverage before promising every action.
P2: extract representative work shapes from real runs without credentials or
private payload publication; compare owned machinery to source-studied reference
algorithms on those shapes, including crash/backpressure/error cases.
P3: improve the largest measured local cost as a whole unit, retain before/after
raw evidence and the semantic tradeoff. No leaderboard chasing or blanket full
suite/tribunal loops. Fixed allowance, at most two hardening layers, no mutants
on the laptop. Speed, memory, I/O amplification and reliability are separate axes.
