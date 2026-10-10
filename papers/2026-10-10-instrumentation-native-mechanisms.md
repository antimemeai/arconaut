# Native hook and emission mechanisms — 2026-10-10

Research input, not an adopted specification. Instrumentation emits metrics;
operations and evals consume them. No product code or reference implementation
was executed. Sources were read; useful source archives and literature acquired.

## DTrace: programmable observation at the source

Read Cantrill, Shapiro and Leventhal, *Dynamic Instrumentation of Production
Systems*, USENIX2004, including probe/provider architecture, execution control
blocks, handler safety, aggregation and buffer handling. The paper separates
probe discovery from selected predicates/actions and supports aggregation near
the source. It also distinguishes semantic probes from arbitrary function probes.
Its runtime safeguards depend on its restricted execution model; arbitrary Lua
or native callbacks do not inherit them. Its buffer drops are reported but that
is not Blackbird's audit completeness contract.

[Paper](https://static.usenix.org/publications/library/proceedings/usenix04/tech/general/full_papers/cantrill/cantrill.pdf),
local ignored `papers/instrumentation-2026-10-10/cantrill-dtrace-2004.pdf`; read
via `pdftotext -layout`, working text in context/instrumentation-research/dtrace.txt.
Restore with curl from that paper URL. The www.usenix.org legacy PDF endpoint
returned403 during acquisition; the static.usenix.org primary endpoint worked.

Consequence for our proposal: provide discoverable semantic hooks and separately
versioned instrumentation definitions selecting hooks, predicates, projections
and accumulation. Don't predeclare every eventual product metric or define the
hook API as a handful of fixed counters. Keep arbitrary probe handlers off
allocator/lock/exception-critical paths. A native slow callback cannot be made
safe by swallowing its exception, and an unbounded scripting handler needs a
real execution contract rather than a promise.

## LTTng-UST: typed sites and registration lifetime

Snapshot5d11feb7dbd25862da163880f89acb6a6d88754c:
`include/lttng/tracepoint.h:60–77` checks enablement around typed calls;
`:192–221` protects probe-list access with read-side lifetime management.
The [official tracepoint manual](https://lttng.org/man/3/tracepoint/v2.9/)
explains why preparing arguments after one enablement check and independently
rechecking later can expose uninitialized arguments when enablement races.
This is a concurrency/lifetime pattern, not selection of LTTng or its RCU library.

Consequence: a hook interest check should yield an activation handle pinning the
selected producer definitions. Preparation and publication use that same handle.
Removal must retire definitions after in-flight hooks/intervals finish. Define
whether disable drains existing scopes or terminates collection with an explicit
coverage boundary. Don't claim ordinary C++ flag checks have DTrace's zero
machine-code disabled cost.

## Perfetto: separate observation forms and writer ownership

Snapshot977bd034d59e01de0d4d202a2431c36c6e0e5a74:
`docs/instrumentation/track-events.md:20–36` distinguishes intervals, numeric
snapshots and cross-track flow; `:176–190` documents lazy payload preparation
and possible callback invocation once per active trace session. Interceptor docs
state that callbacks can run concurrently on arbitrary threads. The buffer
design identifies writer sequences and reports missing/invalid packet loss.
Its trace-oriented terminology and loss policy are not metric/audit semantics
for Blackbird.

Sources: [track events](https://perfetto.dev/docs/instrumentation/track-events),
[interceptors](https://perfetto.dev/docs/instrumentation/interceptors),
[buffer design](https://github.com/google/perfetto/blob/977bd034d59e01de0d4d202a2431c36c6e0e5a74/docs/design-docs/trace-buffer.md).

Consequence: one physical hook hit is captured once as an observation, regardless
of how many instrumentation definitions/readers use it. Producer calculations
must not duplicate a measurement merely because two consumers are attached.
Thread-local lexical scope is insufficient for jobs and participant flows that
outlive calls; those need explicit ownership and phase references. Snapshot,
interval and handoff observation types stay distinct. No consumer callback runs
inside a product lock or publishes product mutations.

## Tracy: richer native instrumentation than timing spans

Snapshotd55cee060180ad386aaa0b289880f5889cc5407d:
`public/tracy/Tracy.hpp:25–117` compiles disabled operations away;
`:178–250` exposes lexical zones, plots, synchronization and allocation/free
operations. Source inspection shows separate integer/float plot forms and source
location metadata. These are valuable surface examples, not a guarantee about
our owned implementation's overhead or a proposal to integrate a profiler.

[Header](https://github.com/wolfpld/tracy/blob/d55cee060180ad386aaa0b289880f5889cc5407d/public/tracy/Tracy.hpp).

Consequence: include ownership/custody, lock wait/hold, current numeric state and
memory accounting in the inventory. Preserve integer values and source/version
identity. Keep optional detailed allocation/stack sampling distinct from semantic
product instrumentation; no global allocator rewrite is selected.

## Existing Blackbird and additional counterexamples

`include/blackbird/local_timing.hpp` already has LocalSpan, a debug-only fixed
8192-record LocalTimingSink, CPU/local durations, linkage and explicit shutdown
persistence. It is relevant timing machinery but absent in release and not a
runtime metrics emission interface. Generalization requires actual interfaces,
not renaming the existing sink or turning UI status callbacks into probes.

Quarantined phux `crates/phux-perf/src/histogram.rs:96–120` records integer samples
and scoped timings into fixed atomic buckets. Its `record` updates bucket,
count, sum, min and max separately: a reader needs a specified snapshot/epoch
contract if it requires coherent equality between components. Percentiles are
approximate bucket estimates, not exact observations. `pi`'s instrumented-storage
decorator is expressly test-only and records attempted commits; it demonstrates
an independent direct oracle, not proof of production hook integration.

## Proposed mechanical contracts to carry into the synthesis

1. Hook observations are typed, read-only facts at named/versioned sites. Product
   code owns the facts; instrumentation owns their metric interpretation.
2. Instrumentation definitions are separate from hooks and consumers. Definitions
   declare measurement meanings, units, dimensions, acquisition/aggregation and
   coverage. Ops/evals consume emitted measurements/aggregate streams.
3. Fast critical hooks only perform owned bounded native operations; richer
   projections use owner-safe snapshots or retained immutable inputs. Neither
   external exporter I/O nor arbitrary scripting runs while a product lock is held.
4. Activation, shutdown, disable, refit and reader attachment have explicit
   boundaries. In-flight definitions and borrowed data cannot outlive their owners.
5. A registry-wide scalar callback bus is insufficient for sampled gauges,
   interval matching, queue residence, ownership accounting and mergeable
   distributions. Each class gets a fitting acquisition and emission contract.
6. Metric evidence that already exists in complete audit is referenced/replayed,
   not copied to a second authoritative history. Ephemeral readings need an
   explicit retention decision when exact reproduction is required. Reference
   drop/overwrite defaults do not authorize audit loss.

Snapshots/restoration: QUARANTINE.md and ignored
quarantine/instrumentation-2026-10-10/MANIFEST.md. Archives retained intact;
extracted references have nested Git metadata and filesystem detritus removed.
No dependency selected. No implementation or performance claim established.
