# One narrow review: autodev profiling launcher

Scope: `scripts/autodev-profiled`, exclusively its launched-child lifetime,
signal forwarding, return status and persistence. Collector implementation was
not rereviewed. This is one review with owner remediation/direct checks, not a
second certification round.

The initial shell wrapper used `wait` followed by `kill -0` to distinguish an
interrupted wait. A reused PID could keep that loop alive after the owned child
was reaped, and signals during collector wait could produce an apparent failed
wait without observing the collector's real termination. The owner replaced it
within this review with stdlib Python `Popen` ownership. The replacement removes
the PID-existence loop and lets signals target the still-owned process; ordinary
negative subprocess signal status is mapped to shell-style `128 + signal`.

Two concrete findings on the Python replacement were returned to the owner:

1. **Telemetry failures can orphan an already-launched manager.** The manager
   starts before `collector.log` is opened and before `launcher.json` is written.
   Either failure unwinds to the outer exception handler without waiting the
   launched child. Open/preflight the capture files before launch, and guard later
   recording failures so the wrapper still waits its child and preserves its real
   exit status. This does not require promising writes to a full filesystem or
   terminating a workload because a profiler failed.
2. **Launch outcome is not persisted.** `launcher.json` records PIDs/hash/start
   time only; manager and collector return codes and finished time remain solely
   transient state. Write terminal launch metadata after the waits, best-effort
   with stderr explaining any persistence failure. Preserve the manager's actual
   return code even when collector recording fails. A failed prelaunch capture
   should remain distinguishable from a launched command with failed profiling.

No dependency adoption or product-code change is proposed. Root owns direct
checks for command exit status, signal forwarding and retained launch/capture
status, then fixes these findings within the existing bounded remediation layer.
There is no follow-up review requirement.
