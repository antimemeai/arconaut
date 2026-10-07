# Explicit debug build gate: focused code review

2026-10-07. One source review bounded to three minutes, after implementation
ready. Scope: current changes in CMake configuration/presets, local timing header,
main-session and terminal-worker gates, and `docs/DEBUG_BUILD.md`. Inspected
existing sink implementation and timing call sites only to assess the changed
build boundary. No unrelated candidate review or new test campaign.

## F1 — Medium: two instrumentation fixtures remain in normal builds

`CMakeLists.txt:208–212` unconditionally declares the `instrumentation_fault`
executable and `bytes_fuzz_probe` object library. Neither is `EXCLUDE_FROM_ALL`.
Consequently `BLACKBIRD_DEBUG=OFF` still puts both into the ordinary default
build graph. The first contains deliberate memory/overflow/race defects for
sanitizer capability qualification; the second is an LLVM fuzzer entry-point
fixture. These are instrumentation machinery, not product correctness tests.
Their continued compilation contradicts the operator's explicit requirement
that instrumentation/performance fixtures be entirely excluded normal builds.

Minimal fix: include both declarations/link setup in a `BLACKBIRD_DEBUG` gate.
The existing fuzz executable's instrumentation-mode condition can remain nested
inside it. Keep ordinary correctness tests and compiler-contract checks as-is.
Direct recheck: normal target graph omits both fixture targets; explicit debug
and fuzz target graphs retain their respective capability fixtures. No further
review or broader suite is required by this finding.

## Other inspected paths

No additional actionable finding established within this scope and allowance.

- Default switch and release preset explicitly select OFF; debug, ASan, TSan,
  fuzz and profile presets explicitly select ON. A profiler/sanitizer setting
  combined with OFF fails configuration rather than silently enabling machinery.
- `src/local_timing.cpp` is conditionally absent. The OFF header branch has no
  clock/sink/metric-buffer/TLS declaration, only the specified empty inline
  facade. Instrumentation-only byte/linkage work is guarded by `enabled()`.
- Main's timing-session construction and worker sink transfer/unbinding are
  preprocessor-gated. The ON sink still allocates only with a nonempty runtime
  `BLACKBIRD_LOCAL_TIMING` opt-in; it retains its bounded buffer and explicit
  shutdown persistence behavior. Worker join ordering preserves the borrowed
  sink's lifetime across terminal execution.
- Renderer/history profile targets and the local-timing test are gated. Required
  audit, normal tool/UI durations and correctness tests remain ordinary product
  behavior, as the written scope specifies.

This report is source-review evidence, not a claim that either configuration
has been built or its runtime/symbol checks executed by this reviewer.
