# Independent Kimi U0 sub-plan and oracle review

Direct CLI read-only session. Capture: `context/kimi-review/run-ea75cues`.
CLI exit 0. Original response follows unchanged. Owner resolutions are separate.

# Independent adversarial review — docs/U0_FOUNDATION_SUBPLAN.md

Reviewed against CORE_DESIGN.md "Identities and time" (lines 52–64), TESTING_PLAN.md T0 (line 55) and IMPLEMENTATION_PLAN.md U0 (lines 37, 54–57). Overall the sub-plan is well grounded: it correctly refuses global-uniqueness minting claims (lines 20–22), separates wall/monotonic domains, and defers journal/admission/Lua machinery to later units as instructed. Findings below are concrete faults or gaps, ordered by severity.

## Findings

### F1 — Stale-handle release path is unspecified; an ABA-resurrecting implementation passes all listed cases
**Location:** U0_FOUNDATION_SUBPLAN.md lines 27–28, 71–72.
**Gap:** "Release invalidates the generation before reuse" and the table require that a *released* slot's old generation rejects and that wrong generation *validation* rejects — but nothing specifies that `release(stale_handle)` must be rejected without touching the slot's current live generation.
**Trigger:** Acquire handle H1 (slot 0, gen 1) → release(H1) → acquire H2 (slot 0, gen 2) → call `release(H1)`.
**Consequence:** An implementation whose release path checks only the slot (or blindly bumps the generation) lets a stale handle kill the current occupant H2 — the exact "stale handle / registry reuse" fault T0 (TESTING_PLAN.md line 55) names. Every listed green case still passes because none exercises release-by-superseded-handle.
**Correction:** Add to the spec: release validates the full handle domain including the *current* generation; a mismatched generation returns a distinct stale-generation error and leaves the live generation and slot untouched. Add the H1/H2 sequence above as a direct case.

### F2 — "Registry identity must be fresh" has no specified mechanism, contradicting the no-minting scope
**Location:** lines 20–22 vs. line 29.
**Gap:** U0 explicitly disclaims minting globally unique IDs, yet requires registry identity to "be fresh for a new registry, including within the same process." It never says whether identity is caller-supplied (in which case "freshness" is unenforceable and the requirement is vacuous) or constructor-minted (in which case a minting mechanism — e.g. process-local counter combined with incarnation — must be specified, without claiming cross-process/global uniqueness).
**Trigger:** Construct two registries in one process with the same environment/incarnation and the same caller-supplied identity bytes.
**Consequence:** Handles from two distinct registries validate interchangeably (registry-domain check passes), silently mixing ownership domains — the very failure the handle domain exists to prevent. Or, if the implementer invents an unspecified random mint, the "no new claims" scope is quietly exceeded.
**Correction:** State explicitly: either (a) identity is caller-supplied and same-process distinctness is a caller obligation checked where possible, or (b) the constructor derives freshness from a process-local monotonically increasing counter (no global-uniqueness claim). Add a direct case: two registries in one process must reject each other's handles via the registry-identity field.

### F3 — Native-clock "nondecreasing" oracle passes a frozen/wrong clock
**Location:** line 76.
**Gap:** "Consecutive samples have nondecreasing monotonic values" is satisfied by an implementation that caches one sample and returns it forever. The sub-plan (line 84) correctly forbids assuming the timer advances within a fixed duration, but the compensating check is missing.
**Trigger:** A wiring bug returns a stored initial `steady_clock` value for all monotonic observations.
**Consequence:** All native-clock cases pass while every deadline based on the native clock is permanently "not due" (or permanently due), and this wrong implementation only surfaces downstream in U1+.
**Correction:** Bracket the monotonic sample with independent `steady_clock` observations (before ≤ sample ≤ after), mirroring the wall bracket already specified; this needs no timing assumption. Keep the wall bracket tolerant of a wall-clock step between samples (retry-with-fresh-bracket once, then report) or document that a wall adjustment mid-case is an inconclusive run, not a failure — otherwise the case is flaky under exactly the wall jumps T0 cares about.

### F4 — "View from a temporary owned buffer is prohibited" lacks its compiler-negative case
**Location:** line 42 vs. the case table lines 66–78.
**Gap:** The prohibition is stated, but the "Type separation" compiler-negative row covers only participant/conversation and wall/monotonic mixing. How the prohibition is enforced (deleted rvalue overload, etc.) and its diagnostic class are unspecified.
**Trigger:** `auto s = make_span(OwnedBuffer{...});` compiles and the test suite stays green.
**Consequence:** Dangling-span UB in every downstream unit that takes a view of a temporary, undetected by any listed case.
**Correction:** Add a compiler-negative case for obtaining a span/view from an rvalue buffer, with the required diagnostic class under the real target flags, alongside the existing mixing cases.

### F5 — "due-at-equality and zero duration are specified" but the specified values are not in the sub-plan
**Location:** lines 49–50, 74.
**Gap:** The sub-plan is the contract the owner implements and the reviewer attacks; deferring the actual truth values to unnamed later text means the green case can encode either convention.
**Trigger:** A reviewer reads "due at equality" as `now > due` vs `now >= due`.
**Consequence:** A boundary-off-by-one implementation (deadline fires one nanosecond early/late) passes the written oracle; downstream U1 admission timeouts inherit the ambiguity.
**Correction:** State in the table: observation at `now == due` yields due = true (or false — pick one and record it), and zero-duration deadline is due immediately at its start instant.

### F6 — UINT64_MAX-elapsed case needs a mandated arithmetic route, not just an expected value
**Location:** lines 47, 74 ("INT64_MIN to INT64_MAX elapsed is UINT64_MAX").
**Note:** The expected value is mathematically correct (2⁶⁴−1 fits uint64 exactly), but a naive `to - from` in int64 is signed-overflow UB; under the required UBSan run this case goes red even in a numerically correct build, and under optimization it can silently miscompute.
**Correction:** Specify that elapsed arithmetic is performed in unsigned/widened arithmetic (e.g., `uint64(to) - uint64(from)` after the ordering check), so the oracle is both defined and sanitizer-clean.

### F7 — TSan run is vacuous as scoped
**Location:** line 81 vs. line 30 ("No … concurrent dereference is owned by this unit").
**Note:** With a single-owner registry and no concurrency claim, a separate TSan build exercises nothing. Either name one concrete shared-access probe TSan is expected to validate (e.g., concurrent reads of immutable ID/byte values) or drop the TSan requirement from U0 rather than reporting a meaningless green.

### Minor
- Line 56: native clock "asserts its supported period/representation" — an `assert` vanishes under NDEBUG optimized builds (which the plan also runs). Make this a checked precondition with an explicit error/abort, or state the optimized build's behavior.

## Proposed specification-derived adversarial cases (small set)

1. **Stale release (F1):** acquire H1 → release(H1) → acquire H2 same slot → `release(H1)` returns the stale-generation error; H2 still validates; slot generation unchanged.
2. **Cross-registry swap (F2):** two registries, same process, same environment/incarnation; each registry's handle rejected by the other with the registry-identity error, distinct from the generation error.
3. **Generation-boundary construction (line 33):** construct a registry with initial generation UINT64_MAX; acquire then release retires the slot permanently; subsequent acquire returns capacity/retired error — no wrap to generation 0 or 1, no billions of iterations.
4. **Frozen native clock (F3):** bracket native monotonic sample with independent `steady_clock` reads; also take two native samples around a real (unmeasured-duration) workload and require nondecreasing *and* within outer brackets.
5. **Deadline boundary (F5):** scripted clock stepped to exactly `due-1ns`, `due`, `due+1ns`; assert the one recorded truth value at each point; zero-duration deadline asserted due at its start instant.
6. **Wall-step decoupling (existing line 75, sharpened):** inject wall jump backward *and* forward *between* two monotonic observations; elapsed stays exactly 25ns and due status flips only on monotonic inputs — including a wall observation that is numerically inside the monotonic deadline's range, which must never be accepted as an elapsed/deadline input (compile-negative).

## What I did not find (scope confirmation)
- No premature demand for journal/provider/Lua machinery; injected storage/effect seams (lines 58–62) are correctly minimal and defer admission/epoch/dedup to U1+.
- Identity equality is treated as opaque byte equality with a caller/issuer freshness obligation (lines 20–22) — correctly not confused with guaranteed minting, except for the F2 registry-identity gap noted above.
- CORE_DESIGN line 64 ("Pause does not stop external deadlines") is faithfully re-expressed at line 52.

**Limitations:** I reviewed only the four named documents; I did not inspect any implementation (none exists yet) and did not verify the LLVM 23.1.2/libc++ profile claims against installed tooling.
