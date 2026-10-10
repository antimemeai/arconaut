# Instrumentation: raw emission, recording and transformation

2026-10-10. Proposed design for discussion and approval; product source unchanged.

Blackbird emits raw metric events into the audit log. A separate recorder preserves
the raw bytes on disk. A tertiary process transforms the preserved records.
Operations and evals consume metrics. No writer-side transforms or metric framework.

Read the concrete artifacts:

* [Design](INSTRUMENTATION_RAW_DESIGN.md): raw layout, retained byte channels,
  publication/reclaim, wakeups, capacity, audit recorder and exact crash boundaries.
* [Raw hook/body census](../papers/2026-10-10-raw-instrumentation-surfaces.md):
  all60 product surfaces, nine system domains and per-class emission design.
* [Implementation and verification plan](INSTRUMENTATION_PLAN.md): direct static,
  byte/order, fault, process-death, owner-lock/cancellation and performance oracles.
* [Transport study](../papers/2026-10-10-raw-transport-design-study.md) and
  [recorder study](../papers/2026-10-10-raw-recorder-design-study.md): complete
  reference paths, mechanisms, limits and restoration.
* [Executed primitive experiments](../papers/2026-10-10-raw-emission-probes.md):
  macOS/Linux costs and five recorder-death cuts, with explicit limits.
* [Single adversarial review](../papers/2026-10-10-raw-instrumentation-design-review.md).

The [producer-transform proposal](../papers/2026-10-10-instrumentation-producer-transforms-superseded.md)
is superseded. OTel/Prometheus no longer guide emission. No dependency adopted.

One essential operator contract remains: whether losslessness includes pending
RAM at host/power failure, requiring a stronger return/acquisition boundary than
async handoff. No crash-loss allowance is selected. Product implementation awaits
the explicitly required discussion and approval. nls.2 remains open; logical
clocks nls.9 and audit completeness nls.10 are separate unfinished work. Evals,
multiplayer and steering/trajectory implementation remain deferred.
