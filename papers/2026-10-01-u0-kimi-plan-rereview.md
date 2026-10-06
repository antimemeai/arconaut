# U0 Kimi plan rereview

Unchanged assistant reply; read-only review, not execution evidence.

# Rereview — docs/U0_FOUNDATION_SUBPLAN.md + tests/foundation_test.cpp (post-integration)

Verified the red artifact: `context/u0-red-missing-foundation.log` records the expected `fatal error: 'arconaut/foundation.hpp' file not found` — a genuine missing-header red, not a synthetic pass.

## Integration check of prior findings — all substantively resolved

- **F1 stale release:** Sub-plan line 29 now specifies full-domain release validation; test lines 105–111 execute release(H1) after reuse, expect `stale_handle`, and confirm H2 still validates. Resolved.
- **F2 registry identity:** Sub-plan lines 30–33 now state caller-supplied identity with U1 issuer obligation and honest "does not detect forged reuse" scope; test lines 101–104 reject cross-registry handles both directions. Resolved without demanding minting.
- **F3 native clock:** Test lines 227–234 bracket monotonic samples with independent `steady_clock` reads, repeated after real work; wall independently bracketed. Resolved (one residual in R3 below).
- **F4 rvalue view:** Sub-plan line 75 now names the deleted-function diagnostic. See R1 — artifact not yet present.
- **F5 deadline equality/zero:** Sub-plan lines 53–54 now pick the values (equality → due, zero-duration → due at start); test lines 182–184, 187 match. Resolved.
- **F6 unsigned elapsed:** Sub-plan line 55 mandates checked unsigned differences; test lines 180, 189–191 exercise the full-range and negative-crossing boundaries. Resolved (UBSan will police the implementation).
- **F7 TSan:** Test `immutable_reads` (lines 299–316) is a genuine shared-immutable-reader case with independently checkable totals (2×1000×(255+7)=262000 ✓). Period assertion upgraded to `static_assert` in every build (sub-plan line 62). Resolved.
- **Mutation deferral:** Now recorded as an operator decision at sub-plan lines 100–102. No objection.

## Consequential unresolved findings

### R1 — Promised compiler-negative cases do not exist as artifacts
Sub-plan lines 75 and 91 promise compiler-negative cases for wall/monotonic implicit mixing, the temporary-buffer view (deleted-function diagnostic), and discarded `Result` (required warning profile), "with real target flags and required diagnostic classes." `tests/` contains only `foundation_test.cpp`; no negative probes or driver exist. The only type-separation evidence in the runtime file is line 69's `!is_convertible_v<ParticipantId, ConversationId>` — which a wrong implementation still passes if both tags implicitly convert to a *common* type (e.g., a shared byte-array base or `bool`).
**Consequence:** The entire "fail compilation" row can ship green on assertion text alone; a too-permissive conversion operator or missing `[[nodiscard]]` enforcement goes undetected. Also note: any negative probe authored now would "pass red" for the wrong reason (missing header masks the intended diagnostic), so these cases are only meaningful post-implementation.
**Correction:** Add, at green time, small negative `.cpp` probes compiled with the real target flags, keyed to the named diagnostic classes (deleted function, nodiscard warning-as-error, no viable conversion), plus runtime static_asserts that IDs/instants are not convertible to or from their byte/count representations.

### R2 — Out-of-range slot in a forged handle has no oracle
Sub-plan line 27 promises the registry "validates that whole domain before exposing a valid slot," and the test tampers with environment/incarnation/registry/generation — but never with **slot**. No case hands a capacity-1 registry a handle whose domain and generation are valid but whose slot is, say, 7.
**Trigger:** Hand-crafted handle `{valid domain, slot=UINT32_MAX or 1, current generation}` passed to `validate`/`release`.
**Consequence:** An implementation that indexes its slot array after only domain/generation checks reads out of bounds — UB on the exact adversarial input class (stale/forged handles) this unit exists to fence. UBSan may catch it incidentally, but there is no defined-error oracle, and a capacity-1 array of one struct with trailing padding can even read "in bounds."
**Correction:** Add a case: forged handle with valid domain, out-of-range slot must reject with a defined error (e.g., `stale_handle`/`invalid_range`), distinct from success and from UB.

### R3 — Wall-bracket inconclusive detection is narrower than the sub-plan's promise
Sub-plan line 83 promises a concurrent wall adjustment is "reported as inconclusive, not hidden." Test lines 235–239 detect only a *net backward* step across the whole sample (`wall_after < wall_before`). A backward step that lands between the observation and `wall_after` (net still ≥ `wall_before`) produces a hard `CHECK` failure mislabeled as a test failure; a step back-then-forward can also pass with an invalid bracket.
**Consequence:** Rare but real flaky red that an owner may "fix" by weakening the bracket, and the promised inconclusive-reporting path is only partially implemented.
**Correction:** Treat *any* wall-bracket violation (either side) as the inconclusive path — retry once with a fresh bracket, then report inconclusive — rather than only the `wall_after < wall_before` shape. Keep monotonic brackets as hard failures.

## Minor (record, no blocking change)

- **Generation arithmetic baked as exactly +1** (test line 109). Reasonable and spec-derived, but the sub-plan text says only "invalidates before reuse." Either write "generation advances by exactly one per release/reuse" into the sub-plan or accept this as the recorded interpretation.
- **Allocation-failure injection requires heap allocation** (test lines 161–168 with `CHECK(!fail_allocation.load())`). This makes "create/copy perform at least one heap allocation" a test-enforced requirement the sub-plan never states; a legitimate small-buffer-optimized `ByteBuffer::copy` of 4 bytes would fail. Either record "these types allocate from the heap" in the sub-plan or scope the injector. The global `operator new` override also intercepts `jthread`/iostream allocations; the immediate-consumption checks currently confine this correctly.
- Sub-plan line 89–90's "registry reuse histories with specification-derived expected state" is met only thinly: the 200-round loop (lines 124–128) checks staleness after release but not generation monotonicity or slot-recycling shape against an independent model. Acceptable as bounded; consider asserting `generation` strictly increases across rounds (spec-derived, cheap).

## Scope confirmations
- No journal/provider/Lua machinery or fixture-qualification demands appear or are needed; injected storage/effect seams remain observation-only with U1 obligations deferred (sub-plan lines 64–69).
- Opaque identity equality vs. minting is now cleanly separated (sub-plan lines 20–22, 30–33); the test never asserts uniqueness minting.
- Red-phase artifact is authentic; runtime cases are well-formed, totals/brackets independently computed, and no oracle derives expected state from the registry's own answers.

**Limitations:** No production header/implementation exists to inspect; I reviewed only the sub-plan, the one test file, and the red log. Compiler-negative behavior (R1) cannot be assessed until the probes and the header exist.
