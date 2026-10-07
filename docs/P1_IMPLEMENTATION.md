# P1 local native timing (arconaut-fsp)

Bound: one primitive, retained append and context/provider preparation; real small
history-scaling fixture. Absolute deadline 1791381225 (25 minutes, no reset).
One independent Codex review attempt <=90 seconds; one findings-driven recheck.
No journal format, policy or durability changes. No upstream builds/tests/install.

Research: PERFORMANCE_CAMPAIGN and W1 first-look distinguish poll/provider wait
from active snapshot-copy/lookup/sync work. Waste audit findings 4-7 identify input
duplication, stream sync cadence, context materialization and forward metadata copy;
these remain targets, not changes in this measurement unit. Existing scripts/profile
uses bounded native sample/xctrace windows; tooling/probes/render_workload is the
existing native workload mechanism. Study-only local quarantine/notcurses/src/lib/
stats.c counts timing, bytes and failed writeouts separately; LevelDB db/db_impl.cc
pairs Env::NowMicros with compaction bytes and subtracts separate waiting work.
Influence: explicit named boundaries, measured bytes, failure/drop counters, no
payload capture or new audit identities. No reference code copied.

Design: opt-in BLACKBIRD_LOCAL_TIMING path, exclusive private JSONL, fixed-capacity
records buffered during actions and written only at session shutdown. Monotonic
wall and CLOCK_THREAD_CPUTIME_ID nanoseconds. Provider boundary is wall-only
(includes callbacks, NOT pure remote inference or disjoint from callback spans).
Null sink: no clocks, files, telemetry allocations or formatting. Single owning
thread; no async collectors. Buffer overflow counts drops; open/write/clock failures
are explicit status, never affect audit acknowledgment. Shutdown file I/O can block;
not in local action spans. Crash may lose buffered telemetry, not retained audit.
Source/action, existing revision/attempt where available; byte/history gauges are
observed inputs, not allocated/copied bytes. No all-allocation, short-child or TUI
presentation claims. IDs are reused; content is not duplicated.

Oracles before code: injectable deterministic clocks verify deltas, wall-only
boundary and failure behavior; disabled scope performs zero clock calls and creates
no output; bounded overflow/status; real optimized retained append and ContextStore
items + protocol validation + JSON serialization at growing history under unchanged
journal semantics, raw samples and nearest-rank p50/p95/p99/max. Compare enabled
and disabled matched fixture runs and primitive overhead. Relevant retained/context/
coding checks only. Highest observed cost becomes a specific next fix. Byteaudit
1v0/02g/3x2 remain backlog.
