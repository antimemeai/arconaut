# Re-Review: Oracle Gap Resolution in tests/retained_events_test.cpp

I reread the updated test file in full. Production codec unchanged per your statement; I verified each claimed addition against the body contract in `src/retained_events.cpp` and the subplan layout. **All previously identified oracle gaps are resolved.** No execution performed; the pass claim is yours.

## Gap-by-gap verification

1. **Raw phase/disposition matrix (test 165–175)** — Correct. Byte offsets 24/25 are right (8-byte header + 16-byte attempt ID). The expectation predicate at 171–172 is exactly `valid_observation` (src 114–126): `(phase∈{1,2} ∧ disposition==0) ∨ (phase==3 ∧ disposition∈[1,4])`. This covers decode rejection of phase 0 and 4, disposition 5, and every invalid combination including terminal+none and running/settling+non-none — 30 cases derived from the contract, not from the implementation.

2. **Encode terminal+none rejection (176–181)** — Correct; exercises the `valid_observation` false path on encode (src 194–195).

3. **Encode disputed-kind=complaint rejection (182–186)** — Correct; exercises `valid_conflict`'s kind restriction (src 131–133).

4. **Encode all-zero disputed identity rejection (187–191)** — Correct; exercises the `nonzero` check (src 128–130).

5. **Decode zero disputed identity (192–194)** — Correct. `events[5]` has no dependencies, so the disputed identity spans bytes 10–26 (8 header + 2 disputed-kind); zeroing exactly that range must fail via `valid_conflict` inside `completed` (src 284). ✓

6. **Excessive dependency count (125–127)** — Correct. Setting count byte 4 to 255 yields count=255 > (40−8)/24 = 1, hitting the bound-before-allocation check (src 267). ✓

7. **Exact 16-byte reservation encode acceptance (72)** and **15-byte decode capacity rejection with `ErrorCode::capacity` assertion (73–74)** — Correct; both boundary sides of src 259–261 and the encoder limit are now pinned.

8. **Later mid-decode allocation failure (203–209)** — Correct. Decode of `{{reference}, decision()}` performs, in order: `dependencies.reserve` (allocation 1), `identities<InvocationId>::reserve` (allocation 2), continuation `blob()` (allocation 3). The countdown `store(2)` throws exactly on the third `operator new` (`fetch_sub` returns 0), so dependencies and planned IDs genuinely succeed before the blob fails, matching the comment. The error is caught by the single try/catch → `ErrorCode::allocation`, and the counter is reset to −1 before the subsequent CHECK so assertion formatting can't trip the cut. ✓ The one-shot countdown allocator (13–23) is sound for this single-threaded test.

Earlier coverage (prefix cuts for every kind, trailing-byte rejection, reserved-byte corruption at offset 26, zero sequence, zero identity, unknown schema/kind, first-allocation OOM on both entry points) is intact.

## Residual issues

None. The only carried observation is the cosmetic check-ordering note (src/retained_events.cpp:267–272 reports `corrupt` before `unsupported` for unknown-schema records with oversized counts), which you have explicitly retained; the contract mandates rejection only and no caller branches on the distinction. I agree it is not a defect.

## Out of scope / unfinished (unchanged, not defects)

RetainedState ledger, semantic replay/dependency authority, admission gating, issuer reservation/counter rebuild, chain continuation, and emergency controls remain unbuilt; successful decoding remains a wire fact only, as documented in `retained_events.hpp` lines 104–105.

## Inspection limits

- Read-only review of the diff-relevant test file against the codec and subplan; no build or test execution — the reported direct pass and running lane checks are your evidence.
- I verified allocator-failure ordering by hand-tracing allocation sites in `decode_retained_event`; if the codec later gains an intermediate allocation, the `store(2)` countdown's targeting would need re-derivation.
