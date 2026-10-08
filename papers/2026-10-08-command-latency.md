# Local-command latency and one redundant UI save

First delivery for arconaut-04z, after prior request/context/capture changes. This unit does not claim the broader under100ms readiness goal is achieved.

## Actual path and fix

Terminal input and worker completion already wake a notification/stdin poll. The busy40ms timeout supports animation, and the idle1000ms timeout is not a mandatory command wait. No polling timeout was blindly reduced.

An ordinary final-byte Enter submission saves UI state before starting its worker, then formerly saved the unchanged state again at the end of the same input batch. The final-byte case now skips that duplicate serialization/rename/fsync. The flag resets for every later input byte, so trailing input after Enter still persists. Pre-dispatch save/failure gating and final exit save remain.

## Bounded probes

Added tooling/probes/command_latency.py: private fresh sessions through the actual scripts/blackbird launcher, explicit executable selection,110x35 PTY, first synchronized frame, audited workflow_registry call followed by a marker absent from input source, and completed-turn endpoint. No provider calls. It bounds each attempt and output size, discards test sessions, and retains timings/binary hashes/host load in JSON. Input tails are restricted to bounded ASCII text to avoid accidentally submitting more work. Query failure yields no success marker.

Added macOS-only opt-in dyld shim tooling/probes/ui_state_syncs.c to count UI-state fsync calls. This is not linked or loaded in production. Protected system shells strip dyld injection, so count measurements explicitly use the native binary path (--direct) rather than pretending launcher injection worked. Zero counts with a requested shim are a failed measurement.

Baseline is the copied pre-change Release binary, candidate is current Release/OFF. Ordinary cache, fresh sessions only, no cold/idle-host claim; alternating pair order. Measurement binaries have different paths, sample sizes are small and latency comparisons must not be treated as certified causal speedups.

## Results

Actual launcher:10samples per binary,20/20successful exits. Load around1.24..1.76 at one minute.

| Endpoint ms | Baseline p50/p90 | Candidate p50/p90 |
| --- | ---: | ---: |
| First frame |99.226 /105.609|99.910 /104.920|
| Submit to successful local marker |36.418 /37.810|37.084 /40.217|
| Submit to completed turn |37.660 /39.369|38.861 /41.963|
| Spawn to local marker |136.299 /143.425|138.037 /146.591|

Baseline max first frame250.781ms; candidate max108.216ms. These samples do **not** demonstrate a latency improvement; command p90 was higher in the candidate sample. No samples discarded, no host-scheduling attribution invented.

Three separate launcher attempts deliberately waited one second after first frame, then submitted a command plus trailing draft. All completed and preserved the draft. Submit-to-marker max42.501ms; submit-to-complete max48.031ms, showing no compulsory one-second input wait. Total process CPU across all phases max46.172ms; this is not an isolated idle-CPU measurement.

Native fsync-count diagnostic: four samples each. Every normal baseline run had4 UI-state fsync calls; every candidate run had3. With trailing input, two samples each had4 in both binaries and all drafts were preserved. This directly verifies elimination of one redundant save only when the last input byte already persisted the current state. It does not measure journal sync reductions.

Raw summaries/identities:
- docs/measurements/2026-10-08-command-latency.json
- docs/measurements/2026-10-08-idle-input-latency.json
- docs/measurements/2026-10-08-ui-state-syncs.json
- docs/measurements/2026-10-08-ui-state-trailing-syncs.json

## Checks and remaining scope

Release build/release/blackbird built. Direct PTY probes and terminal test pass. One recheck covered source flag lifetime, real normal/trailing persistence counts and existing composer/editor/state behavior through terminal test. git diff --check passed. No third assurance layer or prior storage/capture/context re-review.

arconaut-04z remains in progress: phase attribution for spawn/restore/input/worker/admission/render, heavier supported fixtures, isolated idle CPU, and application versus host outlier diagnosis remain. The startup goal remains a measured performance target, not a scheduling guarantee. No claim that tiny fsync savings explain historical broad startup latency.

Unit allowance40minutes; hardening at most20minutes, initial implementation/direct checks then one recheck. No external provider/network request or persistent profiler.
