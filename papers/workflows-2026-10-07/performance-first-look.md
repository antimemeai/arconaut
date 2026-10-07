# W1 autodev: first measured performance findings

Snapshot2026-10-07T12:58:52Z, ongoing run, not a completed benchmark. Raw analysis
is `context/workflows-core/performance/snapshot-analysis.json`; original resource
series and three finite stack windows remain beside it. Optimized profile harness,
including one five-second Instruments allocation attachment; these observations
are not an uninstrumented shipping-build memory qualification.

| Observation | Measured snapshot |
| --- | --- |
| Window and cadence | 785.3 seconds;781 observations at approximately1Hz |
| Harness CPU | 22.62 CPU-seconds;2.88% of one core averaged over window |
| Harness physical footprint | 83.47MiB current;153.16MiB sampled peak |
| Harness resident bytes | 183.22MiB current;240.55MiB sampled peak; distinct gauge |
| Logged provider calls | 42 completed;740.0 seconds cumulative;median8.99s,max80.08s |
| Request JSON | 43 requests;16,545 first bytes,487,646 latest;10,411,506 cumulative bytes |
| Retained audit | 75,092,662 logical bytes at latest storage observation |
| Harness disk writes | 284.30MiB OS cumulative byte counter |
| Collector CPU | 2.28 CPU-seconds;0.29% of one core averaged over window |
| Collector footprint | 12.28MiB at snapshot |
| Collector tick CPU | median2.11ms,p95 3.42ms; serialization/persistence excluded here |

Provider elapsed time includes transport/inference/response drainage, and can
overlap local CPU work; do not add these columns as disjoint phases. Child resource
fields and observed descendant CPU must not be summed together. Unsampled tools
remain absent, not free. Heavy profiler processes are outside the collector's own
CPU/footprint values. Two process observation errors occurred; retained per tick.

All three stack windows predominantly sample native `poll`, consistent with the
low average harness CPU and provider-call time. The later windows also catch
durability sync (`__fcntl`), retained-event lookup, vector relocation/snapshot
bookkeeping and allocation/free. These are specific candidates for action spans
and matched-workload study, not a complete CPU attribution or a demonstrated leak.

Context growth is the immediate behavioral finding. In `src/coding.cpp`, the
configured budget supplies an advisory request instruction, and managed proposals
activate after successful workflow completion. The current long Lua turn has not
crossed that boundary. A131072-byte advisory trigger is not a mid-workflow cap;
it cannot be described as having bounded the487KB request. Useful turn segmentation
and explicit continuation/compaction boundaries deserve their own order while
preserving operator-selected activation semantics and model-authored compaction.

Audit growth and disk counters justify measuring record categories and exact
application writes. Their ratio alone does not establish duplicated payloads or
physical write amplification: OS accounting and logical retained file size measure
different things. Preserve required audit provenance while investigating redundant
representation/materialization. Do not optimize away durability to improve a chart.

The allocation trace was recorded successfully but not interpreted into per-action
allocation totals here. Its raw trace remains private; native Instruments metadata
can include process environment, unlike the resource collector's argv/env-free
observations. Do not publish raw native traces or their full metadata by default.
