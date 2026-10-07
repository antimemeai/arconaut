# Beads design check — 2026-10-07

One bounded two-minute adversarial design check of `docs/BEADS_DESIGN.md`
against the completed pinned-source study and current native operation boundary.
No implementation, new acquisition, database mutations or further review.

## D1 — Unknown write must be represented by the native terminal event

The design correctly requires uncertainty in both result and retained attempt.
The implementation order must explicitly include this integration seam:
`src/coding.cpp:1053–1063` defaults a successful body to
`AttemptDisposition::success` and restricts several unknown-error classifications
to `provider`/`exec` (or capacity). Returning `{unknown:true}` from a `beads` tool
body alone would therefore falsely record success; throwing ordinary I/O from
that body can falsely record failure. CLI nonzero after a SQL write is not a
definite rejection either.

B1 must either deliver read operations only or carry the observed Beads mutation
outcome into `CodingEngine::operation` terminal disposition explicitly. Avoid
mutating placeholders. Direct fake-process oracle: simulated write followed by
nonzero/timeout/malformed stdout yields unknown in BOTH tool result and retained
AttemptTerminal event. A pre-spawn failure must remain distinguishable.

## D2 — `--readonly` is not a blanket guarantee against CLI maintenance writes

The design's no automatic migration/governance promise is stronger than the
external CLI boundary can universally provide. Pinned upstream
`cmd/bd/main.go:475–481` calls `autoMigrateOnVersionBump` for all commands before
opening the nominal read-only store. `cmd/bd/version_tracking.go:143+` has no
`readonlyMode` guard: on detected version upgrade it can recover old Dolt dirs,
open the backend and update version state. Source comments explicitly describe
that behavior for read-only commands. Other maintenance hooks also mean read
command classification is not a general filesystem non-mutation capability.

B1 should bind the existing installed/known setup and check supported CLI version
before database probes; do not automatically upgrade bd or advertise strict
database-maintenance isolation. State that Blackbird initiates no migration,
but external bd retains its own lifecycle behavior. If strict read isolation is
required, it needs an independently defined boundary; do not assume `--readonly`
solves it. Existing operator-approved CLI use is not a reason to create another
approval gate or broaden B1 into a backend rewrite.

## Disposition

No additional concrete interface/performance defect found within the bound.
Child-only `BEADS_DIR`, controlled routing overrides, explicit IDs, lazy binding,
no startup/keystroke subprocesses, and independent bounded stdout/stderr directly
address the inspected fault classes. Read-path-only B1 is a coherent fallback
if mutation-disposition integration does not fit the original allowance.
