# Reopen behavior and command phases

User asks to finish finite remaining performance work, without certification or
performance claims. This lane owns reopen behavior and local phase attribution.
Plan: make large unsupported/missing-state fallback explicit before full scan;
keep small-session automatic rebuild and preserve uncertainty fences. Record
native startup/restore, first render, input-save and worker timings in the existing
opt-in fixed debug sink, then probe fresh/heavy launcher and native paths, plus
isolated idle CPU. No provider calls; no stream measurement expansion.
Direct checks: missing/bad derived state refusal vs explicit rebuild, predecessor
refusal, recovered context/settings and no replay in existing selected recovery
cases. Measure observed delays without attributing unmeasured OS causes. 90 minute
allowance total, initial work plus one recheck within 25 minutes; no campaign.

## Delivered behavior

The launcher automatically allows full replay only for audits up to 64 MiB. The
limit is checked against the already locked descriptor, before scanning the full
history. Valid saved state can still reopen a larger session. Missing/damaged
accelerators or unsupported semantic coverage require `--rebuild-session` above
the threshold. The explicit slow path remains subject to the existing 512 MiB /
200000-record capacity and existing uncertainty fences. It grants no retry.

Small sessions keep automatic recovery. Station/restart histories use full
recovery; scheduling and pending-turn continuation retain their explicit resume
rules. Unfinished valid provider calls become unknown without replay; unknown or
invalid other custody refuses new work with an inspection instruction. Linked
predecessors are explicitly refused. Authoritative corruption remains separate
from a lost derived accelerator. See docs/SESSION_REOPEN.md for the finite matrix.

`--expire-diagnostics` operates on one selected existing session directory. It
runs at most 128 cleanup passes, then reports completion or a rerun instruction.
Each pass retains the store's 128-entry /16-removal bound. It does not open/create
an authoritative audit or instantiate the coding engine.

## Concrete waste removed

Session discovery metadata previously serialized and fsynced the same
session-info.json after every ordinary command. save_session_info now compares
its existing bounded content and skips an identical write. Missing, changed or
unreadable metadata is still regenerated. The session test deliberately ages the
file timestamp, calls the same publication again, and checks the timestamp stays;
its existing damaged-file case still exercises replacement. No aggregate speed
change is attributed to this fix.

## Observed phases

Existing opt-in debug timing now records CLOCK_MONOTONIC start timestamps for
startup initialization/restoration, first terminal frame, input/save, worker
dispatch, operation admission, worker execution and rendering. The fixed-capacity
sink serializes pushes from the UI and worker; output is still written only at
shutdown. Release/OFF retains no timing sink or clocks.

Three launcher attempts each used private fresh sessions and copies of the
settled 324 MiB heavy fixture, ordinary cache. Each waited about 2 seconds after the
first frame before a successful audited local workflow_registry query. No provider
calls. These runs precede the discovery-metadata write skip. One-minute host load
was about 6.2–8.4, with five-minute load around 10.

| Observed phase ms | Fresh range | Heavy range |
| --- | ---: | ---: |
| Process spawn to native timing entry |14.020–23.186|14.636–21.660|
| Native startup initialization |61.984–68.292|29.966–37.262|
| Reopen inside initialization |not applicable|12.204–16.907|
| Input/save before dispatch |0.662–1.262|0.829–1.075|
| Worker scheduling |0.026–0.038|0.034–0.046|
| Native operation admission |3.814–5.067|3.789–5.752|
| Worker command execution |34.484–38.311|37.628–40.653|
| Eight retained append batches inside worker |33.613–37.277|36.303–39.287|

First frame was 76.357–92.071 ms fresh and 49.199–64.710 ms heavy. Submit to successful
local marker was 34.876–38.985 ms fresh and 38.214–41.250 ms heavy. Native thread CPU
inside the eight append batches was 0.591–1.449 ms, while those batches consumed most
worker wall time. The source synchronizes their durable boundaries. These spans
do not separate blocked I/O from runnable scheduling delay. Changing notification
poll timeouts would not remove the observed append work; dispatch and rendering
were small in these attempts.

Three direct-native attempts after the discovery write skip ran alongside the
selected saved-state source checks. They are retained as observations with that
contention, not a controlled comparison. Frame 124.527–749.614 ms; marker 69.600–
76.688 ms. The 749.614 ms frame included 619.694 ms before the native timing entry and
128.962 ms native initialization. Another 421.398 ms frame included 413.678 ms inside
initialization. No OS cause is assigned. Their worker 65.561–76.147 ms included eight
append batches 65.123–75.468 ms but 0.940–1.210 ms measured thread CPU.

Native process accounting during the isolated idle interval was 0.017–0.106 ms CPU
across 1.957–1.979 seconds. The shell launcher remains a waiting parent, so the probe
selects its native child; it does not report shell CPU as native idle CPU. macOS
proc_pid_rusage reports Mach ticks. The initial unit assumption was corrected
using mach_timebase_info 125/3, and the earlier raw deltas were converted with that
factor. Apple's [Recount source documentation](https://github.com/apple-oss-distributions/xnu/blob/main/doc/observability/recount.md)
identifies the Mach-time accounting; the [kernel fill path](https://github.com/apple-oss-distributions/xnu/blob/main/osfmk/kern/bsd_kern.c)
assigns task totals to ri_user_time/ri_system_time. A busy-process diagnostic exposed
the scale error; this is accounting precision, not a zero-CPU assertion.

The first timing-probe version also assumed Python time.monotonic shared the
native clock origin. In this execution environment it does not. The probe now
samples time.clock_gettime(CLOCK_MONOTONIC) for cross-process phase linkage and
keeps its original monotonic durations for local endpoint intervals.

## Direct checks

Selected profile saved_state_test, session_store_test and local_timing_test pass.
New reopen cases cover a byte-for-byte unchanged audit after bounded refusal,
missing/bad derived state, unsupported station coverage and compact reopen under
a restrictive replay policy. Existing selected cases restore context/originals,
settings, accepted suffix and pending-operation custody.

The real CLI on a private 324 MiB heavy copy with saved roots removed refused the
ordinary launch and accepted `--rebuild-session`, restoring its identity/model/
effort/workflow. The diagnostics CLI removed 300 expired files, kept one fresh
file, and created no audit. Help advertises both new flags.

Raw records: docs/measurements/2026-10-08-command-phases-{fresh,heavy,native}.json,
2026-10-08-command-phase-summary.json and 2026-10-08-reopen-cli.json. The launcher
probes record executable hashes and host load. Root owns final Release publication
and the actual published-launcher smoke; no further timing campaign is requested.
