# Instrumentation research: numeric measurements, emission and aggregation

Research/design input, 2026-10-10. Nothing here selects a dependency, instrument
API, retention policy, exporter, metric catalog, evaluator or implementation.
The operator requires discussion and approval before implementation. The earlier
operational-metadata draft is rejected and is not a foundation for this design.

The later [integrated proposal](../docs/INSTRUMENTATION.md) and
[numeric design review](2026-10-10-instrumentation-numeric-design-review.md)
clarify the retention recommendation: every acquired measurement observation
and failed acquisition outcome enters authoritative custody. The historical-
reproduction conditions discussed below are research alternatives, not an
exception allowing acquired evidence to be omitted.

Instrumentation emits metrics. Operations and evals consume them. A consumer may
request a collection, select dimensions, or calculate an analysis from emitted
metrics; this does not make it the producer of the underlying measurements.

This report owns the numeric-measurement part of the study. It classifies
measurement semantics, independent of subsystem domains and probe mechanisms.
The separate system inventory must map these classes to concrete source surfaces.
Native probes, event hooks, Lua interfaces and sampling adapters are mechanisms;
provider execution, storage, UI and command jobs are domains. Neither list can
stand in for the measurement classification.

## What the primary sources establish

OpenTelemetry's event model, aggregate stream and backend time series are
different layers. Its API distinguishes inline recordings from collection-time
observations, and distinguishes additive quantities from non-additive values.
Callbacks have lifetime and concurrency requirements: registered callbacks must
be removable when registration permits it, must not run indefinitely, and may
be evaluated independently for different readers. This supports a producer-side
measurement capability with independently configured acquisition, rather than
putting metric extraction inside an evaluator. [Metrics API](https://github.com/open-telemetry/opentelemetry-specification/blob/ef25ddb7d2ab36a194412c1c3bf71ae5900afdec/specification/metrics/api.md)

The data model identifies additive streams using origin, scope, name, units,
point type and temporality; individual dimensions distinguish series. It
requires one logical writer per stream. Cumulative windows repeat their start;
delta windows advance it. Restart/reset information and gaps affect rate
interpretation. A gauge value and an additive absolute quantity need not have
the same merge semantics despite both being represented by numbers. [Metrics
data model](https://github.com/open-telemetry/opentelemetry-specification/blob/ef25ddb7d2ab36a194412c1c3bf71ae5900afdec/specification/metrics/data-model.md)

The SDK separates instruments, aggregation and readers. Two readers must not
consume each other's delta state. Its synchronous cardinality-overflow mechanism
preserves each measurement in exactly one aggregate, but collapses dimensional
attribution. Its push exporter failure contract discards a failed batch. The
first is a useful aggregation mechanism; neither permits losing Blackbird's
authoritative audit evidence. [Metrics SDK](https://github.com/open-telemetry/opentelemetry-specification/blob/ef25ddb7d2ab36a194412c1c3bf71ae5900afdec/specification/metrics/sdk.md)

Historical OTEP discussions explain why the verb matters: add a delta, record an
occurrence, observe an absolute quantity. They also explicitly discuss
interdependent measurements recorded together and the danger of argument-order
interfaces. These are historical design arguments, not current API authority.
[OTEP 0003](https://github.com/open-telemetry/opentelemetry-specification/blob/ef25ddb7d2ab36a194412c1c3bf71ae5900afdec/oteps/metrics/0003-measure-metric-type.md),
[OTEP 0098](https://github.com/open-telemetry/opentelemetry-specification/blob/ef25ddb7d2ab36a194412c1c3bf71ae5900afdec/oteps/metrics/0098-metric-instruments-explained.md)

Prometheus recommends instrumentation in every subsystem, including failure
paths, queueing, thread pools, caches and library boundaries. Its producer
library model separates a registered collector from the bridge that exposes
collected samples. It also warns that label combinations multiply series cost.
These are useful coverage and ownership patterns, not an instruction to select
Prometheus or turn the application into an HTTP server. [Instrumentation](https://prometheus.io/docs/practices/instrumentation/),
[Writing client libraries](https://prometheus.io/docs/instrumenting/writing_clientlibs/)

OpenMetrics 2.0 is explicitly a release candidate/experimental specification at
the time of reading. It covers numbers, distributions, enum/boolean state and
information families, and warns that an ingestor may reduce numbers to float64.
Its snapshot semantics are a wire-format choice, not a sufficient model for
exact per-occurrence consumption. [OpenMetrics 2.0](https://prometheus.io/docs/specs/om/open_metrics_spec_2_0/)

Monarch makes target and metric schemas explicit and treats distributions as
first-class values. It uses exemplars to connect aggregate evidence to specific
traces. Its availability-oriented collection deliberately rejects some delayed
writes and can return partial results. Those choices must be identified as
different from the operator's complete-audit requirement; Google scale is not
authority to inherit its loss policy. Read: sections 2–4, especially 3.2 and
4.3. [Adams et al., 2020](https://www.vldb.org/pvldb/vol13/p3181-adams.pdf)

Resource coverage should start with the resource inventory and questions about
utilization, saturation and errors. A low interval-average utilization does not
exclude burst saturation. User-facing boundaries additionally need traffic,
failure and duration facts. These frameworks suggest producer coverage, not
product success scores or evaluation criteria. [Gregg, USE method](https://www.brendangregg.com/usemethod.html),
[Google SRE Workbook, Monitoring](https://sre.google/workbook/monitoring/)

## Measurement classes and candidate emission contracts

The following are proposed semantic classes, not a claim that Blackbird already
provides them. Some can share an implementation after their semantics are fixed.
Synchronous/asynchronous below describes when measurement is acquired; it does
not describe whether the underlying product operation uses asynchronous I/O.

| Class | What the producer knows | Proposed hook/emission shape | Interpretation hazards |
| --- | --- | --- | --- |
| Event occurrence | A specific transition, decision, rejection or failure happened | Typed event hook; increment a registered occurrence counter from that fact | Count at one named boundary; admission, dispatch and completion are different populations |
| Non-negative quantity delta | This operation transferred/allocated/processed N units | Typed counter increment linked to its source fact | A byte count is not an event count; one failed operation can still transfer bytes |
| Signed occupancy delta | This transition acquired or released N units | Typed additive delta, emitted at the actual ownership transition | Release must occur on all terminal paths; replay must not acquire twice |
| Absolute monotonic quantity | An external/source-owned counter currently reads N | Observation callback or source sample, with source incarnation/reset identity | N is an absolute count, not another increment; restart cannot be inferred solely from a smaller value |
| Absolute additive population | A pool/queue/process currently owns N units | Owner snapshot callback or immutable committed snapshot | Summing repeated reads over time double-counts; spatial sums require disjoint populations |
| Non-additive current state | A value such as temperature, ratio, configuration limit or mode is current | Gauge/state observation with observation time and availability | Last value is not necessarily still valid; arbitrary gauges cannot be spatially summed |
| Per-occurrence value | One operation/batch produced a size, duration, amount or other value | Distribution-recording instrument, preserving the occurrence unit and relevant source reference | Histograms summarize distributions; they do not preserve arbitrary correlations or all individual values |
| Multi-value observation | Several related values describe one occurrence/snapshot | Typed observation bundle with one source position/sample interval | Separate atomic loads can produce an impossible vector; count, sum and buckets must cover the same population |
| Interval/resource sample | CPU/resource/pressure/profile evidence was observed over an interval | Platform adapter or periodic sampling hook; emit interval, method, value and availability together | Sampling is not complete event history; delayed sampling and observer stalls are facts, not zeros |
| External reported quantity | A provider/tool/source declared a numeric result | Adapter emits the reported quantity and its provenance separately from locally measured quantities | Reported usage is neither observed billing nor locally measured work; absence is not zero |
| Milestone/progress/state transition | A milestone was reached or a lifecycle state changed | Event hook plus optional state/last-milestone projection | A last-success timestamp does not count failures; a state snapshot does not reveal intervening transitions |

The classification separates additive meaning from numeric representation.
An active-request count can be emitted from transitions or observed from the
owner, but those are two acquisition methods for one defined quantity. They
must not be added together. A distribution of queue sizes is different again
from the current total queue size. Historical OTEP 0098 is particularly useful
on this distinction; Finagle's counter/gauge/stat API offers a compact concrete
example without supplying our ownership or persistence model. [Finagle Util
statistics basics](https://twitter.github.io/util/guide/util-stats/basics.html)

## Candidate owned design

These recommendations are engineering inferences from the study and Blackbird's
requirements. They remain discussion input.

Use typed source hooks and typed instrument definitions together. A hook names
a real system boundary and exposes its immutable fact or owner snapshot. An
instrument defines a quantity with a unit, numeric domain, observation unit,
allowed dimensions, acquisition semantics, origin and schema version. The
producer instrumentation maps a hook fact into the instrument's measurements.
Consumers acquire those emissions or derived aggregate streams without owning
the hook or calling the underlying product operation again.

This is more specific than `emit(name, arbitrary_object)`. That generic design
cannot prevent a cumulative source count being added as a delta, a timeout
becoming a success latency, or an integer count becoming a rounded floating
point value. A native typed interface can coexist with a Lua registration and
recording surface, but Lua must obey the same schema and ownership rules.

Prefer reusable descriptors plus stable handles, not string lookup, heap
construction and dimension interning on every hot-path recording. A descriptor
conflict should be an explicit registration failure. Instrument names should
not encode session/request IDs. Definitions and changes need durable version
identity sufficient to interpret old emissions after refit or rebuild.

The native numeric type already supports signed/unsigned 64-bit integers,
double and exact Decimal (`include/blackbird/value.hpp`). Preserve those
representations at owned boundaries. Exact event/count quantities should not
be forced through double. Checked overflow must not silently wrap, reset or
claim that a saturated value is exact. Floating aggregation must state its
rounding behavior; mathematical associativity is not bitwise associativity.
Duration helpers should accept a duration type and convert units explicitly.

Keep origin identity distinct from aggregate dimensions. Process/harness
incarnation and instrument schema establish who wrote a stream. Provider,
operation class and outcome may be dimensions. Per-attempt IDs and payload
locators belong to evidence/context links unless an explicitly selected
high-cardinality view needs them. No magic default cardinality number is
proposed: determine actual dimensions, their populations and costs.

A cardinality policy may deliberately project fewer dimensions or route
additional combinations into an overflow aggregate, but then the result must
say attribution was collapsed. It is still unsuitable as the only evidence
for a consumer requiring individual contexts. Such projection must not change
or erase complete audit records. Gauge overflow cannot generally be implemented
by summing unrelated non-additive last values.

### Acquisition and temporality

Provide independently understandable producer surfaces for inline recordings,
collection-time owner observations, and external-source adapters. One expensive
resource read should produce a bundle rather than being independently repeated
for each requested metric. Registered callbacks require explicit removal,
owner lifetime and in-flight completion semantics. A timeout cannot safely
destroy a callback's owner while its C++ code is still executing.

Do not invoke arbitrary Lua on a participant worker or collector thread. An
observation that requires Lua must execute on its owning interpreter domain
and publish its result through the agreed native boundary. Registration
ownership must survive or retire cleanly at program replacement/refit.

Cumulative collection is a useful candidate for stable additive instruments:
a missed acquisition does not discard the total accumulated between successful
reads. Delta collection is useful for interval merging, but requires a cursor
per reader/exporter and a clear acknowledgement/retry rule. `collect()` must
not globally reset state and make a second consumer miss data. A cumulative
sample cannot reconstruct the placement of observations inside a missed
interval; retaining exact totals does not imply exact temporal resolution.

Push and pull are acquisition/delivery policies. An operator/eval requesting a
snapshot causes producer callbacks to emit measurements; the caller still
consumes them. Scheduled export is another adapter. A remote delivery retry
requires idempotent batch/position identity or receiver deduplication if the
contract promises no double counting. Session/program completion and shutdown
need explicit final collection/lifecycle behavior, not an assumption that a
future scrape will arrive. Prometheus's Pushgateway documents the stale-series
problem when producer and cache lifetimes diverge. [When to use the
Pushgateway](https://prometheus.io/docs/practices/pushing/)

### Authoritative evidence and consumer availability

Do not select a duplicate raw-metrics journal merely because telemetry systems
usually have a collector. Identify how each quantity is obtained:

- Counts/outcomes/sizes fully determined by complete audited facts can be
  emitted and reconstructed from those facts and their versioned mapping.
- Required facts such as worker start times or queue-entry times that are
  currently absent must be captured at their source; an aggregate cannot
  recover them later.
- Ephemeral resource readings, sampler lateness and external reported values
  require retention of the acquired evidence when exact historical consumption
  is a requirement. Polling later cannot reconstruct a past temperature or RSS.
- Histograms, rates and dimension projections are derived representations;
  their approximation/window policies do not weaken source-audit completeness.

Which observations require exact historical consumption, and whether to attach
their facts to existing audit events or introduce new audited measurement events,
is a design decision for discussion. No second persistence system is selected.

The operator's audit requirement remains unconditional for audited work and
observations, including failures. Metrics export availability is a separate
contract. A stalled consumer can fall behind an authoritative retained prefix;
that is different from a producer discarding evidence. Optional aggregation or
remote export failure must not be disguised as zero, a successful collection,
or a new permission to lose audit. OTel's nonblocking/loss policy is a source
tradeoff, not Blackbird's accepted failure model. [Performance and blocking
principles](https://github.com/open-telemetry/opentelemetry-specification/blob/ef25ddb7d2ab36a194412c1c3bf71ae5900afdec/specification/performance.md)

Logical-clock causality and physical durations have separate jobs. The audit
logical-clock upgrade must supply causal event positions; local monotonic
timestamps still supply elapsed duration. A logical-clock difference is not
nanoseconds. Cross-host physical aggregation cannot invent synchronized start
times. An emission's source event/cut and physical measurement interval must
remain distinguishable.

## Distributions, sampling and coordinated omission

Prometheus explains why averaging per-instance quantiles is invalid and why
histograms can be merged when their representation is compatible. Its current
guidance favors native histograms for its ecosystem, but that does not select
our format. The quantity being estimated, error model, merge behavior and
source population matter before an algorithm name. [Histograms and
summaries](https://prometheus.io/docs/practices/histograms/)

| Representation candidate | What is exact/approximate | Fit and unresolved cost |
| --- | --- | --- |
| Exact observation sequence/frequency map | Values and multiplicities remain exact within their numeric domain | Arbitrary later grouping is possible if context is retained; memory/storage cost grows with population or distinct values |
| Explicit fixed buckets | Bucket counts are exact for chosen boundaries; quantiles between boundaries are estimates | Simple owned implementation and threshold interpretation; choosing boundaries too early limits resolution |
| Base-2 exponential histogram | Counts preserved while value resolution is quantized and can be coarsened | Compatible downscaling/merge candidate; zero handling, extreme ranges and resolution policy require a reviewed spec |
| HDR-style fixed-range quantization | Integer counts with configured value quantization and range | Predictable preallocated storage; out-of-range/capping policy must be explicit, and capped values are not exact observations |
| DDSketch-style logarithmic bins | Relative-value error for supported quantiles; fixed-size collapse can invalidate that guarantee in collapsed regions | Broad-range merging; disclose mapping, collapse and which quantiles remain covered |
| KLL-style sketch | Probabilistic additive rank error | Strong rank guarantee and mergeable stream summaries; rank error is not a relative-value tail guarantee |
| t-digest-style centroid summary | Tail-oriented approximation rather than exact original values | Compact distribution estimate; independently qualify order/merge behavior and the precise guarantee before selection |

DDSketch's paper explicitly distinguishes rank error from relative value error;
its Proposition 4 qualifies when bounded collapsing stores retain accuracy.
KLL gives additive rank-error guarantees with stated probability, not a promise
of small value error on a heavy tail. The t-digest paper motivates centroid
summaries with greater tail accuracy. These are different contracts, not
interchangeable implementations of an exact percentile API. Read DDSketch
sections 1–3, KLL sections 1–2 and t-digest sections 1–2. [Masson et al.,
2019](https://arxiv.org/abs/1908.10693), [Karnin et al.,
2016](https://arxiv.org/abs/1603.05346), [Dunning and Ertl,
2019](https://arxiv.org/abs/1902.04023)

Prefer exposing a mergeable distribution with observation count, numeric sum
when meaningful, interval/population and approximation parameters over emitting
only a few precomputed percentiles. Exact integer counts are a candidate
baseline; a floating sum is not thereby exact. No sketch is selected here.

Latency must name its starting/ending boundary. Readiness-to-terminal,
queue-entry-to-dispatch, dispatch-to-first-byte, first-byte-to-last-byte and
foreground-wait duration describe different quantities. Timeout/cancellation
has a factual terminal duration, but does not reveal when the abandoned remote
operation would have completed. Keep these outcomes/populations explicit.

Gil Tene's coordinated-omission example shows how response-driven measurement
can stop generating observations during a stall and report an artificially
healthy distribution. HdrHistogram's correction API generates synthetic values
using an expected interval; those are a model, not newly observed requests.
wrk2 instead records latency from the planned send time and also tracks actual
send latency. The owned design should retain intended readiness/schedule time
when one exists and sampler lateness/gaps. Never synthesize observations into
an observed-request count without a separately named model and its assumptions.
[Tene, 2012](https://qconsf.com/sf2012/dl/qcon-sanfran-2012/slides/GilTene_HowNotToMeasureLatency.pdf),
[HdrHistogram correction description](https://github.com/HdrHistogram/HdrHistogram/blob/de84b0a7de2378abfc405da503bf4898e84ea98e/README.md#corrected-vs-raw-value-recording-calls),
[wrk2 source](https://github.com/giltene/wrk2/blob/44a94c17d8e6a0bac8559b53da76848e430cb7a7/src/wrk.c#L484)

Sampling policy must say what population was sampled, by which method, and what
an estimate means. Sampling expensive diagnostic exemplars need not sample the
counter or histogram observations they reference. Periodic gauges describe
observed states, not all intervening states; a sample-average is not automatically
a time-weighted average. Exact transition hooks can support an occupancy integral
when their physical timing evidence is retained. No evaluation sampling policy
or load-generator campaign is selected.

## Numeric reference code findings

The code was read, not built or executed. Source archives and extraction
restoration are in `quarantine/instrumentation-2026-10-10/numeric/MANIFEST.md`.
Quarantine is study-only.

HdrHistogram Java `Recorder` gives readers exclusive sampled histograms until
explicit recycling. Its active/inactive swap and writer-phase completion make
an interval snapshot safe from unfinished recordings. Allocation, recycling,
locking and lifetime are observable costs; its quantization is not arbitrary
exact-value retention. [Recorder.java](https://github.com/HdrHistogram/HdrHistogram/blob/de84b0a7de2378abfc405da503bf4898e84ea98e/src/main/java/org/HdrHistogram/Recorder.java)

The C port exposes separate ordinary/atomic recording forms. Its sampler swaps
the active pointer, flips the writer phase and waits for writers before returning
the old histogram. Source caveat: `sample_and_recycle` does not inspect the
replacement `hdr_init` return; destruction assumes external lifetime discipline.
Those preconditions and failure gaps cannot be inherited blindly. Its corrected
recording loop adds synthetic values, and its current tests compare concurrent
recording to a serial expected histogram and exercise overflow. [Interval
recorder](https://github.com/HdrHistogram/HdrHistogram_c/blob/8885476fc83fa362fec2fb2b5e9cd9544514976e/src/hdr_interval_recorder.c),
[Writer/reader phaser](https://github.com/HdrHistogram/HdrHistogram_c/blob/8885476fc83fa362fec2fb2b5e9cd9544514976e/src/hdr_writer_reader_phaser.c)

Prometheus Go's histogram updates bucket/sum fields before publishing its
completion count; collection flips hot/cold state and waits for completion.
An atomic counter alone would not make a count/sum/bucket vector coherent. Its
counter splits integer and floating paths, then exports their sum as float64;
the source explicitly flags precision/overflow considerations. Learn the
snapshot invariant, not its numeric reduction. [histogram.go](https://github.com/prometheus/client_golang/blob/a4329cfb379b666a2d895da149cb2c98df181e01/prometheus/histogram.go),
[counter.go](https://github.com/prometheus/client_golang/blob/a4329cfb379b666a2d895da149cb2c98df181e01/prometheus/counter.go)

wrk2's `response_complete` increments per-response counters but histogram
recording is conditional on all-response mode or the final response in a batch.
Its expected origin uses the batch's completed-response count at send time to
avoid giving pipelined responses an invalid future origin. This is a concrete
reason to declare the observation unit: a batch distribution and a per-response
counter need not describe the same population. This finding qualifies the
README's broad statements; it does not qualify wrk2 as Blackbird's test harness.

For an owned initial implementation, a simple synchronized coherent accumulator
or one owner-thread accumulator is a legitimate baseline. Do not select a
phaser, lock-free map or per-thread sharding until measured call rates and
contention justify it. Any later optimization must preserve the same emission,
numeric, snapshot and lifetime semantics.

## Direct fault classes for the eventual approved design

These are proposed oracles, not tests run during research and not an evaluation
suite. The plan should choose direct checks after the semantics are approved.

1. A known typed fact sequence yields exact event/delta/population counts and
   bucket memberships, including rejection, cancellation, uncertainty and
   partial transfer. Each semantic boundary has its own population.
2. Two readers with different schedules/windows consume one known source prefix
   without stealing/resetting each other's data. Retry, duplicate delivery and
   source-incarnation changes produce the specified result.
3. A deterministic writer/collection interleaving checks coherent bundle
   snapshots against a serial model; race sanitizers attack actual concurrent
   implementation, not the serial oracle.
4. Registration/removal/refit/shutdown interleavings verify no callback outlives
   its owner, no borrowed payload escapes its scope, and no callback executes
   on the wrong Lua domain. Consumer stalls do not erase authoritative facts.
5. Native round trips cover 2^53 boundaries, signed/unsigned extrema, Decimal,
   finite floats, non-finite rejection/status, checked sum/count overflow and
   explicit unit conversion. External adapter precision limits remain visible.
6. Distribution merge is compared to a simple exact reference population for
   boundaries, zero, extreme values, heavy tails, singleton populations and
   merge order. Approximation checks target the selected error contract.
7. A scheduled sequence with a controlled stall verifies readiness/queue/service
   distinctions and sampler gaps. Timeout does not invent remote completion;
   synthetic correction remains distinguishable from observed facts.
8. Failure injection at collection, aggregation, allocation and export checks
   the chosen audit/emission contract directly. No drop counter is accepted as
   proof that missing required evidence was retained.
9. Overhead measurements distinguish disabled-hook cost, enabled recording,
   descriptor/dimension binding, collection and delivery. Report actual work,
   allocation/copy/I/O costs; import no benchmark number from another runtime.

## Decisions to take to the operator

- Approve/adjust the semantic classes and their mapping to the concrete system
  inventory, before choosing names and a finite built-in metric catalog.
- Choose which exact observation history consumers can acquire versus only
  aggregates, using existing audited facts wherever sufficient. Decide how
  newly required ephemeral facts join the authoritative audit.
- Choose initial distribution representation/error contract and whether exact
  value/context acquisition remains available for selected surfaces.
- Choose default temporality, independent-reader behavior, consumer cursors and
  the acquisition/delivery forms actually needed now. Do not infer an HTTP
  endpoint, Prometheus/OTLP dependency or mandatory remote collector.
- Fix registration/lifetime/refit/concurrency behavior, numeric domains/units,
  schema evolution, cardinality projection and overflow/integrity failure.
- Coordinate the logical-clock and complete-audit beads with this instrumentation
  work; neither a metrics projection nor the previous parent-pointer draft is
  a substitute for those system repairs.

## Acquired literature and restoration

All acquisitions/readings are dated 2026-10-10. PDFs are ignored by repository
policy. Versions below were identified from the document title pages. Text was
read using `pdftotext -layout`; cited source sections and relevant code are
described above. No credentials, provider probes, dependency installation,
product build, dynamic test, metric consumer/evaluator execution or source
implementation was performed.

| Local PDF in `papers/pdfs/` | Source/version | SHA-256 |
| --- | --- | --- |
| `2020-monarch-adams.pdf` | [PVLDB 13(12), 2020](https://www.vldb.org/pvldb/vol13/p3181-adams.pdf) | `4e7f37dcdfd20ffc2564d969ce41dca275495e22ac78d4ac3f0ffc026f4f8c9f` |
| `2019-ddsketch-masson.pdf` | [arXiv 1908.10693v1](https://arxiv.org/pdf/1908.10693v1) | `caa10bac2d15ca1a9274a31b12c83276f57498d3e9c24a6855e60e79e947d0ce` |
| `2016-kll-optimal-quantile-approximation.pdf` | [arXiv 1603.05346v2](https://arxiv.org/pdf/1603.05346v2) | `61f80573ed5484faf45e8296d558f6bfd8b0da237e7720b5c2f7a60e6861a09b` |
| `2019-tdigest-dunning-ertl.pdf` | [arXiv 1902.04023v1](https://arxiv.org/pdf/1902.04023v1) | `1cb5e25348399f666178a8a66dc74f0aa9ab7791c08a5f79c975732c77a1b86e` |
| `2012-tene-how-not-to-measure-latency.pdf` | [QCon SF 2012 author slides](https://qconsf.com/sf2012/dl/qcon-sanfran-2012/slides/GilTene_HowNotToMeasureLatency.pdf) | `237139f5add84f5d198c7b765952efe065a77ce218a092f2128465ee44230d1e` |

Restore from the repository root:

```sh
mkdir -p papers/pdfs
curl -fL https://www.vldb.org/pvldb/vol13/p3181-adams.pdf -o papers/pdfs/2020-monarch-adams.pdf
curl -fL https://arxiv.org/pdf/1908.10693v1 -o papers/pdfs/2019-ddsketch-masson.pdf
curl -fL https://arxiv.org/pdf/1603.05346v2 -o papers/pdfs/2016-kll-optimal-quantile-approximation.pdf
curl -fL https://arxiv.org/pdf/1902.04023v1 -o papers/pdfs/2019-tdigest-dunning-ertl.pdf
curl -fL https://qconsf.com/sf2012/dl/qcon-sanfran-2012/slides/GilTene_HowNotToMeasureLatency.pdf -o papers/pdfs/2012-tene-how-not-to-measure-latency.pdf
```

Online documentation was read on the same date. Source links pin the
OpenTelemetry specification to `ef25ddb7d2ab36a194412c1c3bf71ae5900afdec`
(2026-10-09), Prometheus Go client to
`a4329cfb379b666a2d895da149cb2c98df181e01` (2026-10-08), HdrHistogram Java to
`de84b0a7de2378abfc405da503bf4898e84ea98e` (2024-05-30), its C port to
`8885476fc83fa362fec2fb2b5e9cd9544514976e` (2026-10-02), and wrk2 to
`44a94c17d8e6a0bac8559b53da76848e430cb7a7` (2019-09-24). Reference source age
does not confer production adoption; these are mechanism studies.
