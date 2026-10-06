# Independent Kimi final rigor rereview

2026-10-01. Direct CLI subscription session, read-only Read/Grep/Glob profile.
Capture: `context/kimi-review/run-shhgj81g`; session
`session_26abf263-6a3b-4b7f-a2b0-18f949732307`. CLI exit 0.
The response below is retained unchanged; owner dispositions are separate.

# Final rereview — residual resolutions

Inspected (read-only): `scripts/kimi-review`, `scripts/rigor`, `.clang-tidy`, and the owner's `context/rigor-final-review-cases.log` (treated as owner-reported evidence, not my execution; the code paths below were traced independently).

## Residual 1: preset/profile pin drift — RESOLVED
`scripts/rigor:49-60` parses `CMakePresets.json` before any version probe, requires both `CMAKE_CXX_COMPILER` (pinned LLVM clang++) and `CMAKE_MAKE_PROGRAM` (pinned ninja) to be declared exactly equal to the profile pins, raises on any mismatched declaration in any preset (so a child preset overriding the compiler is also caught — the loop inspects every preset, not just `base`), and fails closed when either key is nowhere declared ("presets do not declare the qualified compiler and build driver"). Version probes then run against the same pinned paths. The demonstrated RED case ("pin mismatch: base: CMAKE_MAKE_PROGRAM") matches the code path.

## Residual 2: owned directory symlinks escape enumeration — RESOLVED
`scripts/rigor:90-92`: at every walked directory (after exclusion pruning at root), any remaining folder entry that `is_symlink()` raises "owned directory symlinks need an explicit source policy". Excluded roots (`build/`, `context/`, etc.) are pruned before the check, so symlinks inside them don't cause false failures; symlinked source *files* are still enumerated and fail closed downstream (clang-format/missing-from-db). A broken symlink named `*.cpp` would reach clang-format and error — also fail-closed. Correct policy choice: reject rather than silently follow (no loop risk) or silently skip.

## Residual 3: post-spawn OSError leaks the Kimi child — RESOLVED
`scripts/kimi-review:80-96` extracts `stop_child()` (SIG_IGN escalation guard, killpg TERM, 10 s grace, killpg KILL, final reap, `ProcessLookupError` tolerated at each step). It runs on the timeout/interrupt path (line 112) and unconditionally in `finally` (line 146), so the OSError path (line 141, exit 127) now terminates and reaps the process group; `child_exit_code` is recorded from the actual reaped child in `finally` (lines 147-148). Owner's log shows exactly this shape: `exit_code 127`, `child_exit_code -15`, `status failed`, error "injected post-spawn wait failure". Pre-spawn failures are safe (`child is None` → no-op), normal completion sets `child = None` before `finally` so no double-kill, and handlers are restored afterward (lines 150-151).

## Header filter and extension widening — sound
`.clang-tidy:10` `HeaderFilterRegex: '.*'` removes the old three-root restriction; system-header suppression still comes from clang-tidy's default (no `--system-headers`), so this widens only non-system (owned) headers — matching the demonstrated RED diagnostic in a header outside the old roots. `scripts/rigor:18-19` adds usual unit spellings (`.C`, `.c++`, `.cppm`, `.ixx`) and header spellings (`.hh`, `.hxx`, `.h++`, `.inl`, `.ipp`); the new spellings flow through the same db-missing and orphan-header gates, since both now key off `CPP_UNITS`/`CPP_HEADERS`.

## Introduced defects
None consequential found. Specifically checked:

- `verify_tools`' `found` set aggregates across presets, so in theory the two pins could be split across unrelated presets and pass; with the real inheritance structure (`base` declares both, all others inherit) this is not exploitable by drift, only by deliberate restructuring. Not worth changing.
- `stop_child()` catches only `ProcessLookupError` from killpg; an `EPERM` would propagate out of `finally` and skip `result.json`. killpg targets the wrapper's own session child, so EPERM is not a realistic path. Accepted.
- `.c` and uppercase `.H` spellings remain unenumerated; the project is declared CXX-only (`project(... LANGUAGES CXX)`), so this is coverage consistent with the declared profile, not a silent gap within it.
- The cases log transparently retains an earlier AssertionError from a corrected expected literal (line 19 annotation) — honest capture, no action.
- xcrun remains as documented: a host SDK locator whose output is printed via `run()` and used only for the two ad-hoc tidy probes; no cross-host SDK claim is made anywhere I read. Accepted as stated.

## Bottom line
All three residuals from my previous handoff are resolved with direct, fail-closed implementations, and the new child-custody helper is correct on the normal, timeout, interrupt, pre-spawn, and post-spawn-OSError paths as traced. No new consequential defect introduced by these changes. Limits: read-only review; I did not execute `qualify()`, the fault cases, or the Kimi wrapper, and the `context/rigor-final-review-cases.log` outcomes are the owner's execution evidence, consistent with — but not verified by — my code reading.
