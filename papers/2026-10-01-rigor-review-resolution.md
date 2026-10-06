# Rigor setup: review resolutions and achieved scope

2026-10-01. Owner integration of the independent Kimi reviews. The original
[review](2026-10-01-kimi-rigor-review.md), [rereview](2026-10-01-kimi-rigor-rereview.md)
and [final rereview](2026-10-01-kimi-rigor-final-rereview.md) retain Kimi's replies
unchanged. All were direct CLI turns in the same explicitly captured subscription
session, with only Read/Grep/Glob available. Kimi read and traced code; the owner
executed the direct cases. Neither role is represented as the other's evidence.

| Finding | Resolution | Direct evidence |
| --- | --- | --- |
| F1: future owned source roots skipped | Enumerate all owned roots, prune only research/capture/generated roots | An unregistered source in a new root fails the actual compile-database gate |
| F2: headers formatted but never analyzed | Require real compiler dependencies from analyzed translation units | An orphan owned header fails the gate |
| F3: CMake/CTest/Ninja attribution incomplete | Pin and verify exact executable paths/versions; require qualified compiler in the actual database | A foreign compiler entry is rejected; actual Apple compiler configuration fails with the explicit LLVM pin diagnostic |
| F4: fenced example headings selected as sections | Fence-aware heading enumeration and boundaries | Fenced-only heading fails; a real section with fenced duplicate selects the actual section |
| F5: a wrong-model Jev response accepted | Retain raw response/actual model, then fail attribution | Valid answers with a stubbed wrong model exit 1 with `model_mismatch_failed` and unchanged response |
| F6: basename exemptions could hide new source | Exempt only the exact deliberate defect probe; compile fuzz object in every profile | A same-basename source elsewhere fails; fuzz source appears in the actual debug database and analysis |
| Preset/profile pins could drift | Check declarations against profile before tool version probes | Changed Ninja profile pin fails explicitly before execution |
| Directory symlinks silently skipped | Reject owned directory symlinks before enumeration | A linked owned source directory fails explicitly |
| Post-spawn Kimi wait failure could leak child | One cleanup helper runs in `finally`, terminates process group and reaps | Injected wait `OSError` after actual process spawn yields wrapper 127, child −15, no surviving PID |

Owner additionally widened the header diagnostic filter to all non-system headers
and enumerated ordinary C++ module/header filename spellings. An actual unchecked
optional in a header outside the original source roots produces the required tidy
diagnostic and nonzero status. Excluded roots are still excluded; imported source
does not become owned implementation. The Apple SDK remains an actual-host locator
through xcrun, whose path is printed, not a portable pinned distribution.

The full five-profile qualification succeeded in
`context/rigor-qualification-final.log`: direct clean fixtures, actual ASan heap
overrun, UBSan overflow, TSan race, bounded property fuzzing, format/tidy/analyzer
negative diagnostics, and fresh LuaLS annotation-error reporting. Subsequent
enumeration/pin/header corrections passed `scripts/rigor check debug` in
`context/rigor-review-final-check.log` and focused direct fault cases in
`context/rigor-final-review-cases.log`. No changed native target/instrumentation
required repeating the already-passing sanitizer profiles.

The fault-case log retains an owner AssertionError caused by an incorrect expected
literal for CMake's diagnostic; the corrected literal then passed. This was an
oracle expectation error, not a compiler-gate failure. The full qualification's
initial LLVM 21 ASan deadlock, LuaLS empty-table encoding and missing default SDK
were likewise investigated before accepting the actual working configuration;
the original failure captures remain available.

Kimi's final response marks all findings resolved and reports no consequential
introduced defect within its read-only scope. It accepts the declared SDK locator
and current preset inheritance. That is independent code review, not a guarantee
against every future source-tree or profile change.

Local tool capability is qualified. Fleet mutation/Linux-only tooling, Lua 5.5
production embedding and all future agent behavioral oracles remain unqualified.
No mutants ran locally, no acquired EDG build was executed, and no production
dependency or agent runtime was implemented. Operational commands and boundaries
are in [tooling/README](../tooling/README.md).
