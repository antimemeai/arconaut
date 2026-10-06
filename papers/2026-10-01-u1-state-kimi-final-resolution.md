# Nit withdrawn — the arithmetic is exact; final verdict

## The residual nit was a false positive; I withdraw it

You're right, and my suggested "fix" would have introduced a 4-byte overrun. Re-reading 553-559: the second clause `size > limit - 4 - bytes` is exactly `size + 4 + bytes > limit`, evaluated *before* `size += 4 + bytes`. The `- 4` accounts for the current entry's own length prefix at check time, so the accepted proposal bound is precisely `limit`, inclusive — there is no `(limit-4, limit]` rejection window. No production change is warranted.

## Independent recomputation of `rejection_exact_marker_bound`

- **Bound:** empty `RejectedSubmissionEvent` marker = 8 (schema/kind/count) + 12 (body) = 20; `max_references = (1024−20)/24 = 41`; `limit = min(max_file_bytes, 41×1024) = 41984`.
- **Exactly-at-bound case:** 43 complaints, 42×978 + 688: proposal = 48 + 43×4 + 41076 + 688 = **41984** exactly → accepted by `add` (final size == limit, not over). Chunks: 41984 = 41×1024 exactly → 41 full chunks, marker payload = 8 + 41×24 + 12 = 1004 ≤ 1024 — the marker fits with 20 bytes to spare. Consistent with the claimed retention.
- **Sequence/cursor:** initial complaint batch ends at seq 2, offset 1178 (112 + 1010 + 56, recomputed). Chunk groups: 13 groups of 3 (4 sequences each) + 1 group of 2 (3 sequences): 2 + 52 + 3 = 57 consumed; marker 58, commit 59 → cursor sequence **59** ✓. Offsets: 1178 + 13×3224 + (56 + 2×1056) = 45258; marker batch = 32 + 1004 + 56 = 1092 → end **46350** ✓. Both pinned values reproduce from the format, not from the implementation's output.
- **One past bound:** 689-byte last detail → proposal 41985 → `48+4+…` exceeds `limit` at `add` → `capacity` before reserve/copy, zero writes, facts/bytes unchanged, gate live ✓ — matching the doc's explicit-preflight-failure contract.
- **Gate semantics:** retained-conflict case leaves the writer `live` (rejection is recorded, admission healthy); the failure case also `live` — both as specified.

This is the correct boundary pair: the largest representable proposal retained, the smallest unrepresentable one refused, with the refusal occurring before any allocation of proposal bytes beyond what already existed.

## Final bounded verdict

All findings across this review chain now stand as: **B1 withdrawn** (no escaping-allocation path; resubmission sweep pins it), **B2 resolved** (multi-batch chunked retention with exact sequence/commit accounting, partial-failure gating, orphan-chunk replay semantics, real SIGKILL cut between chunk batches and marker), **N1 resolved** (bound lifted to the true marker-derived limit), **nit withdrawn** (checked arithmetic is exact at the bound). No outstanding confirmed defects in scope; no new realizable trigger found in the bound-lift arithmetic, the group/together split, record/sequence/file preflights, or the failure-gating transitions.

**The root RetainedState component can be considered correct within the declared bounds**: single root journal, synchronous sequential custodian with reentrant callbacks, process-crash (not power-loss) evidence on the named profiles, pending final lane confirmations as captured. Whole U1 remains unaccepted — linked continuation, namespace entropy/new-chain non-reuse, public pending-byte diagnostics, and emergency controls are outstanding, and CustodyVerifier remains the U3 seam. Nothing in this verdict speaks to those unfinished units.
