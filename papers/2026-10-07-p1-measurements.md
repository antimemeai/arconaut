# P1 measured local actions — 2026-10-07

Blackbird produced an opt-in timing primitive, append/preparation hooks and a
real history-scaling fixture. Four affected checks passed; independent Codex
review returned five concrete findings; the run applied fixes and direct rechecks.
The four-segment runner ended successfully before its deadline, without the
requested final commit/report marker. Root preserves this candidate and artifacts;
completion of a runner is not acceptance or activation.

Disabled spans were checked for zero clocks/allocations/output. Enabled private
JSONL files retained valid clocks, final status,2300 records and zero drops in
the four growing-history runs. Collection uses a fixed buffer and shutdown
persistence; a crash can lose buffered timing, not authoritative audit.

Optimized same-host fixture,100 samples per action, cumulative live entries
64+history. Timings are from the disabled-telemetry fixture; OS cache/scheduling
are uncontrolled. Append includes actual retained durability. Preparation is
context items/protocol validation/JSON, not provider wait or all startup.

| History appends | Action | Median wall µs | p95 wall µs | max wall µs |
| ---: | --- | ---: | ---: | ---: |
| 0 | append | 8190.00 | 10194.00 | 12430.00 |
| 0 | prepare_subset | 98.00 | 146.00 | 231.00 |
| 128 | append | 8417.00 | 10052.00 | 14187.00 |
| 128 | prepare_subset | 289.00 | 363.00 | 472.00 |
| 512 | append | 9750.00 | 10757.00 | 15083.00 |
| 512 | prepare_subset | 899.00 | 1501.00 | 2429.00 |
| 2048 | append | 9831.00 | 20288.00 | 23190.00 |
| 2048 | prepare_subset | 3003.00 | 3243.00 | 3300.00 |

Raw aggregate/samples/review and1223 OS observations remain in private context/
local-performance/performance. All221 observed process identities no longer
matched libproc at local reconciliation; unobserved escaped children/remote
effects are not inferred absent. Candidate inactive, no primary executable
replacement. Full benchmark and relevant portability qualification remain bounded
by their recorded scope rather than a new certification campaign.
