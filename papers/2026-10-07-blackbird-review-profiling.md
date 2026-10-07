# Blackbird native profiling: adversarial review, one round

2026-10-07. Scope: `scripts/profile`, `tooling/probes/render_workload.cpp`,
profiling additions in `CMakeLists.txt`/`CMakePresets.json`, and the profiling guide.
Read-only review except this report. No provider calls, profiler launches, operator
process signals, or product edits. Prior renderer implementation is outside scope.

## Findings

### P2 — Failed profiler exercises discard the fixture's counters and stderr

`scripts/profile:124–135`: the `finally` block terminates/reaps the owned fixture
and retrieves stdout/stderr, but the files are written only after `capture()`
returns normally. A profiler exception propagates past those writes. The failed
capture retains its profiler log and capture metadata, but drops the native
workload's final report, actual elapsed time, and any failure explanation.
This matters for failed Instruments captures: distinguishing a healthy fixture
from a fixture failure becomes unnecessarily difficult.

Direct reproduction: import the script without invoking its entry point; substitute
an owned fake child returning `{"frames":17}` and `native stderr`; substitute
`capture()` with a function creating its capture directory then raising a profiler
failure. `main()` reaps the child but neither workload file exists. No actual
process or profiler was started.

Remediation: establish the capture directory before starting the fixture/capture;
retain child output and return status inside the cleanup path, including failure
and interruption. Preserve the original profiler error rather than masking it with
artifact-write errors. Do not convert the profiler failure into success.

### P2 — Interrupted captures record a contradictory terminal state

`scripts/profile:73–83`: `KeyboardInterrupt` bypasses the failure handler but runs
`finally`. The metadata gains `finished_unix` while retaining `status: recording`.
An operator's normal Ctrl-C therefore leaves a finished capture marked active.

Direct reproduction: invoke `capture()` with process-existence and Git-query
lookups stubbed and inject `KeyboardInterrupt` from its profiler runner. The
retained record is `status: recording` with `finished_unix` present. No real target
was signaled and no profiler started.

Remediation: mark interruption explicitly and re-raise it; retain owned fixture
artifacts through the same cleanup path as profiler failures. Capture of an
existing PID must continue to leave that harness untouched.

## Bounded observations, not additional blockers

- `scripts/profile build` uses the local macOS-only profile preset and compiler
  path. The Linux CPU backend does not imply that this build command works on
  Linux. State that boundary explicitly or add a deliberately configured Linux
  profile; avoid a guessed compiler fallback.
- Workload counters describe the fixture's entire lifetime, including profiler
  initialization and finalization, rather than exclusively the requested sampling
  interval. Label that distinction when presenting throughput or byte totals.
- The motion case intentionally advances every iteration and changes its frame
  number every iteration. It is a maximum-rate component workload, not the idle
  harness's actual animation cadence. The guide already correctly excludes
  input-to-photon latency and ordinary operator duty cycle.
- Python timeout kills/reaps the direct profiler process. Descendant or Instruments
  service cleanup was not established by this read-only review; do not claim
  process-tree cleanup from `subprocess.run(timeout=...)` alone.

## Positive evidence and limits

The profiling preset retains Release optimization, adds `-g` and frame pointers
through the existing interface target, and does not link profiler code into the
production runtime. Separate build directories keep profile flags isolated.
Apple symbol generation runs after the build; the owner's reported real
symbolized `sample` capture is independent of this source review.

The native fixture uses actual ChatView/grid/painter/sprite implementations,
retains packet byte counts without writing terminal packets, and declares that
scope in its JSON. It holds references only until their owning object is next
mutated: rows remain live through grid composition, and the painter packet is
counted before commit. No additional lifetime or arithmetic defect found in this
new fixture. Duration is bounded to 300 seconds; cooperative signal handling uses
`volatile sig_atomic_t`, and the wrapper has a terminate/kill/reap path for the
owned fixture. These are bounds on this direct process, not a general critical
systems guarantee.

Actual Instruments CPU/allocation success was not established by this review.
The owner's ongoing serial experiment must be reported according to its real
outcome. No second review round is requested; fix the two findings and perform
one targeted failure/interruption recheck.
