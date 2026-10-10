# Instrumentation: proposed hooks and metric emission

2026-10-10. **Proposal for operator discussion and approval. No product
implementation is authorized by this document.** Source census is of
`command-jobs` at `e85a845340d600b40b07b255040bf57d1b4e51d1`.

Instrumentation emits system and product metrics. Operations and evals consume
them. This proposal provides the hooks, acquisition surfaces, producer machinery
and native consumption interface; it selects no evaluation questions, scoring,
campaigns or operations policy.

A hook exposes a fact where its owner knows it. An instrument defines a measured
quantity. Instrumentation acquires observations through hooks or owner snapshots,
calculates the declared measurements and emits them. An observation is not
automatically a metric. Audit retains authoritative evidence of what occurs,
including unsuccessful work; metric selection and aggregation do not filter it.

## Study and its consequences

The research is split into four complementary reports:

- [Product/harness census](../papers/2026-10-10-instrumentation-product-surfaces.md):
  60 surface rows, actual routes and owners, and NullClaw, Kimi, Gemini CLI and
  Docker Agent source and test mechanisms.
- [System census](../papers/2026-10-10-instrumentation-system-surfaces.md): ten
  mechanical capabilities, nine system domains, current native implementation,
  SQLite and TigerBeetle, and native trace/profiler mechanisms.
- [Numeric/emission literature](../papers/2026-10-10-instrumentation-literature.md):
  eleven semantic classes, OTel, Prometheus, OpenMetrics, Monarch, distribution
  literature, HDR recorders and wrk2; their guarantees and failure policies.
- [Native hook mechanisms](../papers/2026-10-10-instrumentation-native-mechanisms.md):
  DTrace, LTTng-UST, Perfetto and Tracy, enablement, lifetime and critical paths.

Reference study selects no dependency. Archives, pins and restoration are in
[QUARANTINE.md](../QUARANTINE.md); acquired literature has restoration commands
in the reports. The reports distinguish mechanisms read from behavior tested.
No acquired implementation was built or executed for this study.

The sources support several useful patterns: discoverable typed semantic sites;
source-side filtering and aggregation; separate intervals, snapshots and flows;
owner-maintained resource accounting; coherent reader snapshots; and tests that
exercise actual production routes with an injected recorder. Their drop policies,
numeric narrowing, behavioral hook powers, estimates presented as reported usage,
and duplicate measurement paths are not adopted. These are engineering lessons,
not claims that an external implementation already satisfies Blackbird.

## Architecture proposed for discussion

```text
semantic owner / native resource owner / platform acquisition adapter
    -> typed hook fact, explicit interval, or coherent snapshot
    -> registered instrumentation producer and declared quantity definition
    -> typed measurements and declared aggregate streams
    -> native cursor/collection interface; optional external emission adapter
    -> operations and eval consumers
```

Propose an owned C++ core, available in release, with a Lua definition and
recording surface on the owning interpreter. Core quantities use native typed
instruments; hooks can also supply immutable facts to programmable projections.
Use stable handles bound during registration, avoiding hot-path string lookup
and generic object construction. A catalog exposes hook schema, source site,
capabilities, owner/concurrency domain and semantic version, independently of
which producers are enabled. Quantity definitions are separately discoverable.

Keep three axes explicit:

1. **Hook mechanics:** how and where evidence can be acquired safely.
2. **Measurement semantics:** what the emitted quantity means and how it combines.
3. **System domains:** the concrete owners and routes needing those capabilities.

These axes overlap. A queue owner uses occurrence, state, transfer and snapshot
hooks; it can feed event counts, occupancy, size and residence distributions.
There is no claim that each function has exactly one hook type.

## Hook capabilities and emission design for each

| Capability | Hook and acquisition proposal | Emission and lifecycle proposal |
| --- | --- | --- |
| Occurrence/outcome | Concrete payload at a named boundary, including rejection/failure/unknown. Request and response are distinct facts; preparation, local send, receipt, decode and publication stay distinct. | Registered occurrence counts and quantities derived from that fact. Capture once even if multiple definitions/readers use it. No universal success boolean or recounting the same fact through aliases. |
| Synchronous interval | Explicit begin/end around one computation or syscall phase; native scope helper where appropriate. Terminal knowledge and measured endpoints are explicit. | Per-occurrence elapsed-duration observation, optionally thread CPU on the same thread, plus outcome. C++ unwinding and Lua protected-call/error boundaries need their own matching paths. An exception does not manufacture successful completion. |
| Asynchronous interval | Owner-held activity handle with explicit start, phase and terminal/abandonment observations. Survives queueing, thread transfer and caller return. | Queue, execution, drain and settlement durations remain separate distributions. No subtraction of unrelated threads' CPU clocks. Activation changes preserve existing handles or explicitly end their coverage. |
| State/publication | Old/new state or revision observed at its authority; proposed, staged, committed, activated, duplicated and refused are distinct. | Transition counts, state populations and residence measurements with defined endpoints. Commit emits after publication. Replaying prior state produces recovery-work measurements, not new product transitions. |
| Quantity/work delta | Exact transferred/processed amount from the loop or effect that performs it; partial progress survives later error. | Add nonnegative work deltas or signed occupancy deltas to the corresponding instrument; optionally record batch-size distribution. Requested, read, queued, retained, returned and terminal-written bytes are separate quantities. |
| Ownership/resource change | Acquire/release/resize at the actual resource owner; optionally maintain current and high-water state there. | Live population, allocation churn, COW detach and high-water measurements. Shared physical storage counts once at its allocator owner; logical presentation bytes and RSS have separate definitions. Release paths include error/cancel/shutdown. |
| Flow/queue/handoff | Sender and receiver occurrences linked by transfer identity, queue instance and owner epoch; capture enqueue/dequeue, withdrawal, backpressure and actual delivery. | Stage counts/byte quantities, coherent queue population and residence distributions. Submission is not delivery; cancellation request is not acknowledgement. Identities link evidence without automatically becoming aggregate labels. |
| Passive snapshot/resource sample | Owner publishes one coherent snapshot, or a platform adapter acquires one bundle with clock bracket, method, scope and availability. Collectors never invoke effects to obtain a gauge. | Absolute observations or interval-resource measurements. One physical acquisition supplies all requested fields and readers of that acquisition; independent scheduled acquisitions are explicitly different samples. Missing, stale, unsupported and zero remain distinct. |
| Programmable domain phase/value | Registered native/Lua quantity definition; explicit recording and opaque mark/region handles owned by the program generation. Optional projections consume typed hook facts on a safe owner domain. | Custom quantities, intervals and distributions under the same units, numeric, provenance and retention rules. Recursion and parallel regions do not pair by latest matching string name. Instrumentation receives measurement capabilities, not product effect authority. |
| Instrumentation lifecycle/status | Definition, activation, retirement, collection, reader and adapter outcomes; status remains inspectable at its owner even when delivery fails. | Expose configuration/coverage boundaries, invalid definitions, clock status and delivery/collection disposition directly. Do not recursively emit through a failing emitter. This reports interface outcomes; it is not a second system to certify the first. |

Low-level hooks prepare small fixed native observations, or update an
owner-maintained accumulator with a specified concurrency contract. They do not
call an external receiver or arbitrary Lua under a product lock, in an allocator,
or from a signal handler. Larger projections use pinned immutable source facts
or published snapshots outside critical sections. A queue/buffer is permitted
only with an approved capacity and retention/failure contract.

An interest check acquires a handle to the active definition generation.
Preparation and recording use that same handle. Detaching prevents new entries;
retirement waits for in-flight calls and the defined treatment of open intervals.
A callback timeout cannot authorize destroying its owner while it is executing.
Borrowed payloads are valid only during their documented scope; asynchronous
transfer pins existing immutable storage or explicitly owns the required bytes.
Detach/retirement never waits under product, resource, emitter or collection
locks needed by in-flight producers, or inside the retiring instrument's own
invocation. Shutdown prevents new entries, joins/settles outside those locks,
then retires storage. Collection/removal follows an explicit lock order or an
owner-thread message protocol.

## Measurement contracts

Every quantity definition states its semantic version, unit, numeric domain,
observation population, acquisition point/method, origin and reset/incarnation,
allowed dimensions, aggregation algebra, time semantics and coverage. A mapping
from hook facts to quantities has a version too. Changing meaning under an
unchanged definition is rejected; display/export names may be aliases of one
native measurement.

The numeric study separates the following semantic capabilities. Bundles and
provenance qualify other classes rather than forcing a mutually exclusive enum.

| Semantic class | Contract and example |
| --- | --- |
| Event occurrence | One occurrence at one named boundary: failed launch attempts, not all failures anywhere in an outer operation. |
| Nonnegative quantity delta | Newly performed amount: bytes actually read, including bytes preceding an error. |
| Signed occupancy delta | Change in owned population: queue insertion/removal or allocation resize/free. |
| Absolute monotonic quantity | Source-owned cumulative count with incarnation/reset: importing it is observation, not adding the whole count each time. |
| Absolute additive population | Current owned total: summing disjoint queues can be valid; summing repeated snapshots is not. |
| Non-additive current state | Current mode, ratio, limit or value, with freshness; arbitrary gauges do not have a sum operation. |
| Per-occurrence value/distribution | One duration/size/amount, with its instance population. A later histogram is a representation of these values. |
| Coherent multi-value observation | Related count/sum/buckets or resource fields from one stated cut; individually atomic loads alone do not establish coherence. |
| Interval/resource sample | CPU/pressure/profile evidence over a specified interval, acquisition method, availability and sampler timing. |
| Externally reported quantity | Provider/tool-declared usage with provenance; separately named from estimates, billing or locally observed work. |
| Milestone/progress/state | An explicit milestone or categorical transition; last success and current state cannot reveal all failures or intervening transitions. |

Preserve native signed/unsigned integers and Decimal where their semantics are
exact; do not convert counts to double. Declare floating rounding and distribution
approximation. Overflow is an explicit integrity failure, never silent wrap or
saturation presented as exact. Duration recording accepts a duration with an
explicit physical clock domain and unit.

Definitions constrain input domains as well as representation: sign/range,
permitted Decimal scale and unit conversion, and behavior for invalid values.
Native Number rejects non-finite doubles; unavailable/invalid/overflow is a typed
status, never NaN as a sentinel or a zero observation. Native collection,
emission and persistence preserve these types through owned binary packets.
External adapters must expose any representational loss at their boundary.
Invalid, unrepresentable and out-of-range inputs have distinct failure outcomes;
malformed external evidence is retained without forcing it into a normal Number.

Version identity alone does not establish reproducible calculation. Retain the
definition/mapping artifact, configuration and numeric policy. Reconstruction
uses captured inputs and its declared order/state, including any acquired clock
or sampling decisions, rather than current configuration or fresh platform reads.
When deterministic reconstruction is insufficient, retain the actual produced
measurement in the same audit custody. Do not promise exact replay from a version
string and an input locator alone.

One acquisition may yield several measurements. Each output needs stable identity
within that acquisition and definition/mapping generation; a reader position must
address those outputs as well as its source cut. An event cursor alone cannot
express that only some expanded measurements have been consumed. Aliases and
multiple readers do not create new physical acquisitions or duplicate native
measurements.
Conversely, one measurement may depend on several source facts; coverage must
identify that relationship rather than assume one event per output.

External quantity adapters declare whether a field is a delta, cumulative report,
provisional value or revision/final replacement, and its request/source scope.
Repeated cumulative usage cannot be added as fresh usage on every stream chunk.
Missing or malformed reporting remains distinct from a reported zero. Aggregate
selection of final reports or explicit corrections is a declared producer rule;
no heuristic based merely on a smaller number selects replacement semantics.
Normalization, status reads and presentation do not re-record one report as new
usage. Nested/overlapping fields are not automatically additive, and attempt/report
scope distinguishes retries from repeated access to a final result.

Origin identity establishes the producer and its incarnation. Aggregate dimensions
describe a selected population. Event, attempt, job, transfer, revision and source
generation references provide association/evidence, including multi-input causal
relationships; they are not a one-parent data model and are not automatic labels.
The logical-clock/system identity design belongs to `arconaut-nls.9`. Physical
monotonic time measures duration; a logical-clock difference is not elapsed time.

No fixed label/cardinality ceiling is selected. Definitions select dimensions
deliberately and account for their population/cost. A projection that collapses
attribution must declare that fact. It cannot erase original audit evidence or
stand in for individual observations when those are required.

## Comprehensive current-owner census

The detailed product report lists the proposed occurrences and native sites for
60 product/harness surfaces; the system report supplies nine low-level domains.
The following union groups them by owner and includes source units and Lua
surfaces that need additional explicit treatment. Companion public headers belong
to the same owners. This is a census of the current candidate and known extension
boundaries, not a claim that all hooks exist or that future domains are complete.

In the table, O = occurrence, I = interval, S = state/publication, W = quantity,
R = resource, F = flow, P = passive sample, C = programmable. These are capabilities
from the preceding hook table, not dashboard categories.

| Domain and source owners | Capabilities | Proposed acquired facts and emitted quantities |
| --- | --- | --- |
| Startup, ingress, session and launch handoff: `main.cpp`, `session.cpp`; CLI wrappers `auth_main.cpp`, `colleague_main.cpp` | O/I/S/F/P | Interface selection, open/reopen readiness, submitted/refused input, settings effective revisions, attachment lifetime, restart token scheduling/consumption, process handoff outcome. Wrapper entry and downstream requests are distinct. |
| Turn/program/workflow/module/tool registry: `coding.cpp`, `workflows.cpp` | O/I/S/F/C | Turn admission/execution/settlement, pinned source, Lua create/load/call/teardown, definition validation/staging/activation, workflow selection/deferred invocation, cache hit and custom phases. Hooks belong at the affected owner, including direct Lua APIs. |
| Native operation boundary: `coding.cpp` | O/I/S/W | Preadmission refusal, retained admission, effect dispatch, observed outcome, result retention and final settlement. Each stage is counted independently; a normal wrapper return is not success. |
| Context/managed transformation: `context.cpp` | O/I/S/W/P | Append/edit/restore, inspection, staged/committed/refused proposal, conflict, original repair, protocol placeholder, budget crossing, logical compaction and successor seed. Staged `accepted=false` is not rejection; unknown placeholders are not executed tools. |
| Tasks and activity: `tasks.cpp`, task bindings in `coding.cpp` | O/S/W/P/F | Parsed batch, duplicate receipt/conflict, actual changes and state population, task/activity binding and badge transfer. Authored `done` is not a correctness judgment; command exit does not complete a task. |
| Provider assembly, retries, raw stream and response: `coding.cpp`, `openai.cpp`, `stream_capture.hpp` | O/I/S/W/F | Request preparation/local send, raw receipt, decode, externally reported usage/model, response import and context publication. Receipt measurements precede later import failure. Retry policy selection, actual wait and attempts remain separate. Raw bytes, decoded content and terminal output have distinct first-observation marks. |
| Direct colleagues: `colleague.cpp`, `colleague_main.cpp` | O/I/S/W/F | Profile/context selection, admission, dispatch, raw/decoded response, reported quantities, normalized result and CLI exit/unknown/refusal. Normalization cannot erase response receipt. |
| Participants: `participants.cpp` | O/I/S/W/R/F/P | Run/worker lifecycle, actual per-request execution, capture receipt/admission/retention, direction accepted/dedup/dequeued/dispatched, await/join/cancel/seal/archive, queue occupancy and drain delay. Worker time is acquired on the worker; owner drain is a different phase. |
| Provider auth/accounts/device flow: `provider_auth.cpp`, `provider_oauth.cpp`, `auth_main.cpp` | O/I/S/F/P | Account/config revision, credential resolution and refresh stage, login/device begin/poll/slowdown/expiry, selection/conflict, logout/revocation disposition. No credential values or account names as metric labels. |
| File tools: `tools.cpp` | O/I/S/W/F | Actual source/read bytes, retained originals, range returned, edit-match refusal, proposed/write bytes, short writes, sync/close/rename and publication outcome. Presented range is not whole-source size. |
| Native child adapter and custody: `native_process.hpp`, `process_lifetime.hpp` | O/I/S/W/R/F/P | Spawn attempt/outcome, send/read/EOF, leader exit, signal attempt/result, cleanup and reap; child counts/descriptors. Used by providers/auth/Beads/editor as well as tools. Concurrent global child CPU deltas do not prove per-child usage. |
| Command execution and private PTY: `command_jobs.cpp`, `command_job_runner.cpp` | O/I/S/W/R/F/P | Bootstrap/launch/ready, running/stopped/continued, leader exit, descendant-tail drain, reap and durable terminal settlement, stdin/control delivery, signal/ioctl outcome, deadlines/escalation. Output stages and backpressure remain distinct; PID/job/queue links are evidence context. |
| App foreground wait and keyboard route: `command_jobs.cpp`, `terminal.cpp` | O/I/S/F | Wait epoch acquire/release and reason, background request, input route and actual write, resize delivery. App background releases a wait; it neither sends SIGSTOP nor relaunches. It is distinct from the child's POSIX foreground process group. |
| Operator/UI queue, persistence and terminal custody: `terminal.cpp` | O/I/S/W/R/F/P | Composer submission versus draft, pending prompt admission/dequeue/refusal, presentation queue, saved UI state, outer raw terminal acquire/restore, editor handoff, resize handling. Observe signal handling on the owner after self-pipe wakeup; no ordinary metric callback in the signal handler. |
| Rendering/presentation: `chat_view.cpp`, `task_view.cpp`, `sprite.cpp`, `presentation.cpp` | O/I/S/W/P | View projection/layout, history trim, task projection, sprite generation/graphics command preparation, native value formatting and terminal transport. Preparation is not terminal acceptance; terminal-written is not operator-seen. Byte/node work should come from existing loops. |
| Native values/shared sequence/codec/bridge: `value.cpp`, `shared_sequence.hpp`, `packet.cpp`, Lua bridges in `coding.cpp` | O/I/W/R/P | Share/detach/clone/resize, radix node/leaf allocation, exact numeric domain, encode/decode/project work and errors, native/Lua conversion. Separate logical bytes, unique allocated bytes, copies and visited/materialized nodes; no extra traversal just to count work. |
| Native identity/handles/byte buffers: `foundation.hpp`, `foundation.cpp` | O/S/W/R/P | Registry admission, slot acquire/release/retirement, validation by actual native reason, live/capacity state, identity exhaustion and requested/copied/refused buffer bytes. Wrong environment/incarnation/registry and stale slot/generation remain distinct. Slot retirement is not a new acquisition or counter reset. |
| External JSON boundary: `json.cpp`, external adapters | O/I/W | Parse/format/import/export stage, byte/node work, invalid UTF-8/protocol/capacity result. JSON remains an external protocol representation; hooks and metric persistence use native values/packets. |
| Journal physical work/durability: `journal.cpp`, `journal_writer.cpp`, `journal_storage.cpp`, `journal_checkpoint.cpp` | O/I/S/W/R/P | Frame encoding, append/write progress, sync, tail scan/repair, checkpoint write/rename/directory sync and authoritative publication. Bytes submitted, actually written and durability acknowledgement are separate. |
| Retained logical state, projection and recovery: `retained_state.cpp`, `retained_events.cpp`, `retained_environment.cpp`, `retained_proposal.cpp`, `saved_state.cpp`, `environment_head.cpp`, `recovery_index.cpp`, `archive_catalog.cpp` | O/I/S/W/R/P | Proposal/admission/commit, settlement capacity, replay/reconcile, live projection, index/catalog/archive lookup, cold reads, resident retirement and checkpoint/reopen work. No remote-effect replay or duplicate live-effect counts. Shared retained payloads are not multiplied per reference. |
| Audit/variables/read-only observation: `audit.cpp`, `observations.cpp` | O/I/W/F/P/C | Cursor/pin/page query, rows/bytes actual return and error, original lookup, declared variable sample, clock bracket and Git observation. Measurement collection must not recursively query/instrument itself. Existing `variable.sample` is a durable domain sample, not an allocator-safe hook or metric registry. |
| Diagnostic storage: `diagnostics.cpp` | O/I/S/W/P | Diagnostic store/read/purge attempt, actual scanned/removed files, short-write/read/permission/stale outcome and bytes. Existing expiring provider originals are an audit bug; labeling this storage diagnostic does not authorize required evidence deletion. |
| Beads and decision-model adapters: `beads.cpp`, `decision_models.cpp` | O/I/S/W/F | Validation, preparation/send, child transport, raw/parsed response, reported usage, mutation-possible/unknown/refused disposition. Decision-model product calls are ordinary instrumented work; they are not eval consumption. |
| Candidate/evolution/complaints: `candidate.cpp`, complaint path in `coding.cpp` | O/I/S/F/C | Source archive, lease/queue, exact checkpoint/run/qualification/activation transition, locally retained complaint versus attempted delivery. Producer exposes observed facts; consumer assigns significance. |
| Station and backstop: `station.cpp`, `backstop.cpp` | O/I/S/F/P/C | Feed validation/dedup/admission, pause/resume/steer effective transition, workflow dispatch, lineage claim, assessment/pivot/action/artifact and bound/no-progress. Feed receipt is not context inclusion; backstop's artifact predicate is not a general success score. |
| Lua orchestration/packages/maintenance: `programs/orchestration.lua`, `packages.lua`, `maintenance.lua`, `turn.lua` | O/I/S/W/F/P/C | Explicit participant attempt/branch/join/pause, package configure/receive/HUD preparation/include/event preparation, maintenance item/match work, loop/phase observations. Package counters currently describe distinct function boundaries: HUD generated is not HUD displayed. Native admission remains owned by station/engine. |
| Existing useful-work scripts: `programs/useful_work.lua`, `useful_turn.lua`, `economy_pilot.lua` | C | Inventory existing source reads, feedback, record/contrast preparation and caller-defined phases without running or endorsing the old campaign. Ad hoc date subprocess timing and caller-recorded usage must not become authoritative native timing/usage. Evaluation design/execution remains deferred. |
| Clocks, platform resources, native synchronization and measurement control: `foundation.cpp`, `local_timing.cpp`, owned mutex/worker sites | O/I/S/W/R/P | Clock domain/regression/availability, thread CPU where valid, process CPU/RSS/FD/thread samples with platform scope, selected lock wait/hold, definition/reader/retirement status. Debug LocalSpan is relevant machinery but compiles out in release and has a fixed lossy sink; it is not the proposed production interface. |

Source coverage here includes every current `src/*.cpp`, native helper header and
shipped Lua program. It does not imply per-function instrumentation: pure helpers
feed their owner's quantity without necessarily adding another hook boundary.
Global allocator interception, instruction tracing, every-lock wrapping and stack
profiling remain separately selectable diagnostic proposals, not mandatory rewrites.

Multiplayer, remote tool-using outposts, general connectors, live station UI
attachment, arbitrary concurrent main turns, full interrupt-and-apply-now,
external complaint sinks and native hot plugin reload have no fully implemented
owner yet. Their future owners need the same capability analysis, including
distributed request/response and flow identities. Blackbird measures client work
and facts reported by shared services; their control planes and internals belong
to their owners.

## Collection, aggregation and consumption

Propose independent readers over native measurements and coherent aggregate
snapshots. A read does not clear a global accumulator. Additive baseline quantities
can expose cumulative state with a stable origin; interval/delta views have their
own cursors and explicit source windows. A missed collection can preserve a total
without preserving where its occurrences fell within the missed interval.

Collection names its consistency scope. One owner's bundle is coherent; unrelated
owner snapshots are not falsely described as one globally simultaneous state.
Use owner-thread accumulation or simple synchronization as a correctness baseline.
Sharding or writer phases are later candidates if actual measured contention
warrants them. No lock-free or zero-overhead claim is made.

Readers can acquire retained observation-derived measurements by position, and
current samples/aggregates by collection. Push adapters and pull collections are
delivery choices, not different ownership of measurement. Retry must preserve
batch/source position identity when the delivery contract requires deduplication.
Final collection, detach, shutdown and source-incarnation/reset are explicit.

Proposed instrument failure behavior is explicit: invalid registration returns
an instrumentation error; a mapping failure retains its source fact and failure
and marks the affected quantity unavailable; a failed clock read leaves the
duration unavailable while an independently valid occurrence can still count.
Aggregate overflow invalidates the affected exact aggregate rather than silently
wrapping. These failures do not grant projections dispatch, retry or product-state
control. Mandatory audit-custody failure follows the system's agreed admission/
settlement behavior, which is a different responsibility from an exporter failure.

Propose preserving exact per-occurrence value/context acquisition from retained
evidence, with count/sum and explicitly configured fixed-bucket distributions as
the simplest optional aggregate baseline. Buckets are exact membership counts;
within-bucket quantiles are estimates. Exponential histograms, HDR, DDSketch, KLL
and t-digest have different error/range/merge contracts and remain alternatives
for discussion, not interchangeable exact-percentile implementations. Do not
average per-instance p99s. No arbitrary global bucket layout is selected.

Distribution merge requires matching quantity/unit/population and compatible
representation, bounds/resolution and window semantics. An explicit conversion
may coarsen resolution with declared loss; incompatible distributions are not
silently merged. Sum/count and buckets describe the same acquired population.
Representations state boundary inclusivity, underflow/overflow handling and
compatibility version. Rebinning declares whether it is exact or approximate;
merge also requires an identified population without duplicate observations.

Measure intended readiness/arrival when it exists, queue entry, execution start
and completion separately. Record sampler schedule/lateness/gaps. Never create
synthetic requests to hide coordinated omission in observed counts. Periodic
snapshots do not establish every intervening state or a time-weighted mean.

## Complete audit, retention and failure

`arconaut-nls.10` requires complete audit irrespective of whether work succeeds.
`arconaut-nls.9` owns logical-clock/system causality. Neither is repaired by a
metric hook, a dropped count, one parent ID or local timing. Existing capture
queue refusal, post-dispatch result-retention refusal and expiring stream
originals remain substantive audit defects.

Proposed retention rule: use the authoritative audit facts already sufficient
for a measurement and its versioned mapping; add newly acquired ephemeral
measurement evidence to that same authoritative custody. This includes acquired
resource samples and failed acquisition outcomes, not just quantities convenient
to reconstruct later. A past RSS sample, worker clock endpoint or provider-reported
quantity cannot be recovered by polling later. Instrumentation configuration/
definition changes and failures are also audited. Do not introduce a duplicate
raw-metrics journal by default.
Actual emission attempts/outcomes are audited as work through that custody;
recording a failed emission must not recursively call the same failing emitter.

Audit completeness is unconditional. Which additional optional diagnostic probes
to activate is a collection decision; it does not permit discarding facts from
work that must already be audited or measurement evidence actually acquired.
Selecting a histogram or sampling expensive profiling detail does not sample the
authoritative work history. Historical measurement availability begins at the
explicit acquisition/definition activation boundary; it cannot invent evidence
from periods when that measurement was not acquired.

Local evidence custody, in-memory aggregation and external delivery have distinct
failure states. A stalled consumer may lag a retained prefix; aggregation can be
rebuilt where source facts suffice. Delivery failure must not look like zero or a
successful collection. Do not synchronously invoke a remote exporter on the effect
path. Do not silently erase required evidence to protect latency.

The exact critical-path custody bridge, reservation/admission rules, crash boundary
for new ephemeral observations and resource exhaustion behavior must be settled
with the audit repair before claiming durable complete emission at those sites.
This proposal does not pretend that an in-memory native buffer already supplies
that guarantee. Hooks, typed instruments and owner snapshots can be designed
independently; implementation units that need new custody cannot be qualified
without the corresponding repair. No global blocking policy, lossy default,
second persistence engine or fixed buffer/TTL is selected here.

## Registration and change activation

Process, session, worker and Lua-program registrations have explicit owners.
Cross-thread emitter handles follow worker shutdown/join, not a caller's stack
or UI subscription. Snapshot callbacks read published state on a safe owner
domain. Lua acquisition/projection executes on its interpreter domain, never an
arbitrary collector/participant thread, and cannot gain tool/provider effects
through an instrumentation-only interface.

Definition activation follows the existing conclusion-of-current-turn/affected-
workflow default. An eventual interrupt-and-apply operation must also settle
affected registrations and interval coverage. Historical emissions remain
interpretable by definition/source generation. Refit retires this harness's
registrations and client activity without stopping shared services. Detached
consumers do not destroy producer state or reset other readers.

## Discussion decisions and subsequent Blackbird work

The recommendation is typed release hooks plus owner-safe instruments, native
readers, exact numeric domains, explicit semantic/definition versions, and Lua
custom measurements under the same contracts. Please discuss/adjust:

1. Whether the capability/census union misses any intended domain or semantic
   boundary, and which detailed diagnostic classes should be initially enabled.
2. The useful historical observation and aggregate views; the concrete
   custody/crash/exhaustion behavior for newly acquired ephemeral evidence.
3. Initial distribution representation: exact source access plus optional fixed
   buckets is the proposed baseline; resolution/error and storage costs are open.
4. Which native stream/snapshot consumption forms and external adapters are needed
   first; no OTLP/Prometheus endpoint, remote collector or SDK is selected.
5. Registration/projection expressiveness and activation lifetime, including the
   eventual logical-clock context supplied by the system upgrade.

This proposal received [system-source review](../papers/2026-10-10-instrumentation-design-review.md)
and [numeric/emission review](../papers/2026-10-10-instrumentation-numeric-design-review.md);
integrated findings are recorded in the journal.
After discussion and approval: incorporate agreed changes, review any materially
changed design boundaries, produce and review the implementation plan, then
implement conceptual units under BLACKBIRD.md and
the existing two-layer hardening bound. Direct oracles must exercise real owners
and routes, not just manual calls to an emitter. The studies identify concrete
fault classes: preadmission rejection and post-effect failure; native and direct
Lua store routes; response-before-import-failure; partial transfer; real child
receipt/exit/reap; cancellation/dedup/recovery; two independent readers; coherent
concurrent snapshots; detach/refit with in-flight producers; numeric extrema,
overflow, unit conversion and distribution merge; disabled work and actual
allocation/copy/I/O costs. Static analysis and appropriate deterministic,
property/fault-injection and sanitizer checks belong to those units. Existing
tests and reference patterns are inputs, not qualification of unbuilt machinery.

No product code, evaluation campaign, provider request or runtime activation was
performed for this proposal. The rejected metadata overlay and
[retired initial sketch](../papers/2026-10-10-instrumentation-initial-sketch.md)
remain historical input only.
