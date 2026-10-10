# Unapproved initial instrumentation sketch

Historical input from before the operator required comprehensive study and
design discussion/approval. None of the API, sink, limits, synchronous-delivery
or lifecycle schema choices below is selected. This is not an implementation
specification. The current scope is research and proposed design only.

# Instrumentation

Instrumentation emits system/product metrics. Operations and evals consume
metrics. This unit implements the hooks and emission/consumption interfaces;
evaluation definitions, scoring, steering and trajectory UI remain deferred.
Audit completeness (nls.10) and logical-clock system ordering (nls.9) are separate
work. Metrics neither replace audit nor claim to repair those deficiencies.

## Grounding and design

The [OpenTelemetry metrics API](https://opentelemetry.io/docs/specs/otel/metrics/api/)
separates instrument definitions and measurement recording from consumers and
aggregation. The [Prometheus instrumentation guidance](https://prometheus.io/docs/practices/instrumentation/)
grounds useful boundary measurements and warns that labels multiply time series.
Quarantined `nullclaw/src/observability.zig` distinguishes lifecycle events and
numeric instruments behind a consumer interface; its no-op/multiple-observer
paths inform disabled and fan-out behavior. OpenClaw's diagnostic event machinery
checks listener interest and isolates delivery. These are study material; no
library, SDK, source import or external schema is adopted.

Owned native instruments have a name, kind, unit, description and declared
attribute keys. Counter measurements are nonnegative increments; histogram
measurements are nonnegative individual observations; gauges report current
values and may be negative. There is no aggregation in the emission API.
Integers retain their native 64-bit precision. Decimal arbitrary-precision
numbers are outside this metric interface. Definitions are immutable; reusing a
name with different semantics fails before registration.

Native consumers subscribe to the common measurement surface. Delivery is
synchronous on the emitting thread, outside registry/subscription locks. Workers
can emit concurrently; a consumer must synchronize its own state. Measurement
views are valid during the callback; consumers can make an owned native value
when they need to retain a measurement. Subscription removal prevents future
delivery starts, while callbacks already entering delivery may finish. Callbacks
must not call product controls; they consume metrics. Nested metric emission
from callbacks is suppressed and counted to prevent feedback loops. A throwing
consumer is counted and cannot prevent delivery to other consumers or alter
execution outcomes. A slow consumer adds its execution time; no unbounded queue
or claim of nonblocking export is introduced.

Without consumers, hooks perform an interest check before reading the clock or
constructing measurement payloads. Built-in definitions are discoverable. Lua
and native callers can define custom instruments and record measurements; Lua
custom names use `custom.` to distinguish them from built-in instruments.
Lua inputs cross directly as native values, and these calls retain the ordinary
operation audit. Operations/evals receive measurements through subscriptions or
the optional owned binary stream consumer; neither emits metrics here.

Attributes describe measurement dimensions. Correlation carries exact existing
identities separately: actor/conversation/workflow/generation/revision/attempt,
run/request/job where available. These references attribute measurements; they
do not select a new causal data model or logical clock. Timing uses local
monotonic nanoseconds for durations. A process/emitter identity separates streams.
No physical clock measurement is presented as system causality.

## Hook coverage and semantics

* Lifecycle started/completed/duration instruments cover operation entry/exit,
  turn/program/workflow, context mutation/checkpoint, audit append, command worker,
  participant request, session open/recovery and operator entry. Completion labels
  distinguish returned/threw and explicit domain outcomes where available.
* Provider request and decoded-response instruments are separate. Stream bytes
  are counted at receive callbacks; standard reported input/output token counters
  are emitted only when present as valid nonnegative integers. Missing usage is
  not zero. Transport failure produces lifecycle failure, not a fabricated response.
* Command received and retained output bytes are distinct measurements. Controls
  expose the action and job identity; foreground wait and worker lifetime have
  separate lifecycle boundaries.
* Participant measurements occur on the worker for each actual request, with
  run/request attribution; queue capture behavior remains an audit issue.
* Context hooks distinguish append/edit/manage/restore/checkpoint. Rejected
  changes get a rejected outcome. Audit payload measurements count successful
  payload storage and exclude framing/index overhead; they do not imply complete
  audit coverage.

The optional `--metrics-directory DIR` consumer creates one private binary stream
per session instance. Frames contain owned native BBM2 measurement packets, with
an explicit format header and length. File export is a consumer of metrics. Its
failure is observable in instrumentation status; it does not change effect
outcomes. It is not an audit store and has no implicit retention policy. The
directory option follows session switching/restart. No network exporter selected.

## Plan, oracles and allowance

Start2026-10-10 10:28:52UTC; whole unit90minutes through11:58:52UTC. At most25min
two-layer hardening inside that unit; name its actual start before review/recheck.
Six build workers, two lint workers; no provider probes, evals or local mutants.

Implement instruments/subscriptions/native stream, then hook existing owners,
then Lua/CLI surfaces. Direct tests inspect actual native measurements from real
engine/provider/command/participant/context paths. Compare exact increments,
identities, distinct request/response records, custom native/Lua values, absent
usage, retained binary round trips and independent consumer delivery. Test
conflicting definitions, invalid records, subscriber throw/removal/reentrancy,
concurrent worker emissions, disabled hooks, and failed/unknown operations.
Static review attacks hook placement, borrowed lifetimes, lock reentrancy,
consumer failure isolation and request/response semantics. Affected release,
ASAN/UBSAN, TSAN and Linux checks attack independent runtime fault classes. The
ordinary clean-staged-tree full debug/C++/Lua commit gate remains mandatory.
