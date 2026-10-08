# 300 startup rerun after performance wrap-up — 2026-10-08

All 300 starts succeeded and exited zero: 100 fresh, 100 heavy, 100 weird.
The published Release/OFF binary was measured through the ordinary
`scripts/blackbird` launcher, with no executable override or profiling.
Source commit: `b9d263184719c229b1611f83e596a36607db64de`.
Binary SHA256: `47282d5c978dbed9c4a7eb6e01a1aa0c30e810910b63a1f91e011ea882a88d99`.

The unchanged `scripts/benchmark-startup` driver ran serial round robin with a
110×35 PTY and Ghostty image path. Every process received a private APFS clone of
the exact converted heavy/weird seeds from the [previous trial](2026-10-08-startup-300.md),
or a fresh empty directory. Cloning is excluded from timing. Ordinary macOS
caches were used; no flush or idle-host requirement. No provider/network calls.

Timing begins on the parent monotonic clock before launcher spawn. The first
endpoint is the complete TUI frame; the second is the successful audited Lua
`workflow_registry` query checking the `ultracode` definition. The latter is a
local command endpoint. Command response separately measures input to its result.
Percentiles use the driver's existing nearest-rank rule. All samples and outliers
are retained, with no retries or rejected observations.

## First complete TUI frame from spawn (ms)

| Scenario | p30 | p50 | p70 | p90 | max |
|---|---:|---:|---:|---:|---:|
| fresh | 92.553 | 98.661 | 102.938 | 111.610 | 310.207 |
| heavy | 74.783 | 82.269 | 85.707 | 93.355 | 177.303 |
| weird | 74.356 | 82.493 | 87.009 | 90.645 | 200.557 |

## Audited local command result from spawn (ms)

| Scenario | p30 | p50 | p70 | p90 | max |
|---|---:|---:|---:|---:|---:|
| fresh | 124.930 | 132.075 | 135.920 | 147.324 | 345.029 |
| heavy | 108.853 | 114.894 | 120.770 | 127.635 | 224.769 |
| weird | 108.266 | 117.778 | 122.710 | 127.800 | 241.685 |

## Local command response from input (ms)

| Scenario | p30 | p50 | p70 | p90 | max |
|---|---:|---:|---:|---:|---:|
| fresh | 32.514 | 33.336 | 34.023 | 35.214 | 49.942 |
| heavy | 32.688 | 33.850 | 34.992 | 36.899 | 47.455 |
| weird | 33.735 | 34.798 | 36.131 | 37.993 | 49.998 |

## Reading this run

First-frame medians rose from 89.345/69.607/69.956 ms in the previous trial to
98.661/82.269/82.493 ms for fresh/heavy/weird. Audited-command medians from spawn
rose from 128.247/109.158/111.151 ms to 132.075/114.894/117.778 ms.
Median command response after input fell from 38.990/39.101/40.523 ms to
33.336/33.850/34.798 ms. These are observations from two separate host runs;
they do not establish a causal change from the implementation.

This run started at 09:25:55 UTC and finished before the host sample at 09:26:44 UTC.
Host load averages before were 5.56/9.79/13.80 and after 5.17/9.05/13.29;
the previous trial's end was 3.03/3.47/5.14. Load averages alone do not attribute
latency. The worst first frame was fresh trial 61 at 310.207 ms; its audited
command finished at 345.029 ms. Heavy's worst frame was 177.303 ms and weird's
200.557 ms. The current trial does not supply an across-the-board sub-100 ms
startup result.

Public [numeric results](../docs/measurements/startup-300-wrapup-2026-10-08.json)
contain metadata, summaries and every sample. Private raw terminal transcripts,
run log and cloned seeds remain in ignored
`context/startup-300-wrapup-2026-10-08/`. The prior trial is unchanged.
