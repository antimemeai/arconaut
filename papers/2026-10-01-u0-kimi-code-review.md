# U0 independent Kimi code/oracle review

Unchanged assistant replies; read-only review, not execution evidence.

Let me check the downstream contract referenced for purpose, then finalize.

# Independent review: U0 foundation unit

I read `docs/U0_FOUNDATION_SUBPLAN.md` (the contract), `include/arconaut/foundation.hpp`, `src/foundation.cpp`, `tests/foundation_test.cpp`, `CMakeLists.txt`, `scripts/check-foundation-api`, and `scripts/check-linux`, with a skim of `docs/CORE_DESIGN.md` §identity/time/audit for downstream consistency. All findings below are from code reasoning; I executed nothing, so I cannot confirm the claimed Mac green runs or the Linux lane.

## Verdict

I found **no consequential contract violation, UB, overflow, allocation, move/lifetime, or native-time error** in the production code, and **no specification-derived oracle error** in the tests. Details of what I verified, then the only (low-severity) findings, in the runner scripts.

## Verified correct (attack reasoning)

- **ID/sequence**: `Id::from_bytes` preserves exact bytes, rejects all-zero (foundation.hpp:81-88). `AuditSequence` rejects 0 and `next()` rejects at `UINT64_MAX` without wrapping (foundation.cpp:47-58). Oracles at tests/foundation_test.cpp:76-88 match the spec table exactly.
- **Registry**: acquire skips retired (`generation==0`) slots; `validate` checks environment → incarnation → registry → slot/liveness/generation with distinct error codes; `release` validates the full handle first and advances generation exactly once, retiring at `UINT64_MAX` to 0 (foundation.hpp:178-219). Stale-release-cannot-invalidate-reused-slot is genuinely oracle-tested (test:122-128). Move transfers state and nulls the source domain; moved-from source returns `stale_registry` before touching (empty) slots — the deliberate use-after-move test at test:129-135 is safe because `domain_` nullopt short-circuits.
- **Time**: `elapsed` uses unsigned modular subtraction (INT64_MIN→INT64_MAX = UINT64_MAX correct, test:207). `Deadline::after` overflow path: `available = INT64_MAX − start` is computed unsigned, so negative starts (incl. INT64_MIN, `available` = `UINT64_MAX`) are exact; the negative-start branch's `distance_to_zero = -(start+1)+1` avoids negating INT64_MIN; I hand-checked every boundary the tests assert (−10+9=−1, −10+10=0, INT64_MIN+UINT64_MAX=INT64_MAX, INT64_MAX+1 rejects). `due()` equality-is-due and regression-before-start both enforced.
- **Native clock**: `native_nanoseconds` statically asserts integral signed rep ≤ 64-bit and exact nanosecond scaling (`Scale::den==1`) in every build via `foundation.cpp` always being compiled; the overflow guard `count > INT64_MAX/num` cannot itself overflow and the truncated-division lower bound cannot underflow. Wall and monotonic are sampled and bracketed independently in the test, and a wall step is thrown as an explicit "inconclusive… qualification not passed" failure — reported, not hidden, per contract.
- **Bytes**: `slice` uses subtraction-based bounds (no offset+length overflow); `ByteBuffer::copy` checks the configured limit before copying and converts `bad_alloc`. The allocation-failure oracles confirm the armed flag was consumed (test:187-195), so a pass can't be caused by the failure never firing. `view() const && = delete` matches the temporary-view prohibition.
- **Compile negatives**: each required diagnostic string ("no viable conversion", "invalid operands to binary expression", "no matching function for call to 'elapsed'", "call to deleted member function 'view'", "declared with 'nodiscard' attribute") is emitted only by the intended error on Clang, and the harness fails closed (returncode 0 *or* missing substring → RuntimeError). Flags come from the real target's compile_commands entry with `-o/-MF/-MT/-MQ/-c/-MD/-MMD` correctly stripped, keeping `-Werror` and `-std=` — so `discarded_result` cannot silently pass as a mere warning.

## Findings (all low severity)

1. **scripts/check-linux:55-56 — cleanup failure masks the real result.** The `finally` runs `ssh … rm -rf` with `check=True`. If the checks pass but the cleanup SSH hiccups, the whole lane reports failure (false red); if the checks failed *because* connectivity dropped, the cleanup `CalledProcessError` replaces the informative "Linux checks failed … inspect checks.log" error. Trigger: transient network failure during cleanup. Fix: wrap the cleanup in its own try/except and warn, never raise.

2. **scripts/check-linux:16-17,44-48 — lane scope is narrower than the Mac lane, and broader in build than in test.** (a) The Linux lane runs debug/release/ASan+UBSan only — no TSan — which matches the stated "ordinary lane" intent, but means Linux TSan coverage of `immutable_reads` rests solely on the Mac run; worth one line in the qualification record, not a defect. (b) `cmake --build` builds all default targets, including `instrumentation_fault` (a deliberate-fault probe) and `bytes_fuzz_probe`; I could not inspect `tooling/probes/*` (outside my read scope), so I cannot verify those probes can't fail the ordinary build or behave unexpectedly under `-Werror` on clang-18 — recommend either scoping the build to `foundation_test compiler_contract` or confirming the probes' Linux-green status is recorded.

3. **scripts/check-foundation-api:40-42 — pollutes the source tree.** It mkdirs `context/` at the repo root on every local ctest run (tempdir itself is cleaned, the parent is not). Cosmetic, but it creates an untracked artifact directory inside the qualified source tree that `scripts/check-linux` deliberately excludes from its bundle — fine as-is, just worth a `.gitignore` entry or use of the system tempdir.

## Review limits

- No execution: all green/red claims are code-level reasoning; I could not reproduce the compile-negative diagnostics against Clang 23.1.2/18.1.3.
- `tooling/probes/compiler_contract.cpp`, `instrumentation_fault.cpp`, `bytes_fuzz.cpp` are referenced by CMake but were not in my inspection scope; finding 2(b) flags the resulting blind spot for the Linux build-all step.
- CORE_DESIGN was skimmed for contract alignment (wall+monotonic-with-incarnation, no cross-process clock comparison, deadline semantics) and is consistent with what U0 implements; I did not audit the full document.

Nothing here blocks U1 depending on this foundation beyond the operator's planned Linux-lane qualification and noting finding 2(b).
