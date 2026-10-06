# N1 verification: resolved, with one residual 4-byte nit

## N1 correction confirmed correct

I re-derived the new bound arithmetic at `src/retained_state.cpp:526-632`:

- **Empty-marker measurement via the real encoder** (537-542): the marker is constructed with the *actual* selected body (`RejectedSubmissionEvent` or `IdentityConflictEvent`), so the fixed-envelope constant is not duplicated and cannot drift from the codec. `empty_marker.size() ≤ chunk_limit` is guaranteed by successful encode, so the subtraction at 542 cannot underflow. `max_references = (chunk_limit − empty_size)/24` is exact (24 = 16-byte journal + 8-byte sequence per reference).
- **Cap consistency (the N1 question):** `limit = min(max_file_bytes, max_references × chunk_limit)` and `add` caps the proposal at `limit`. A proposal of `p ≤ max_references × chunk_limit` bytes yields `ceil(p/chunk_limit) ≤ max_references` chunks, so the final marker encode at 599 is provably size-safe (only allocation can fail). The doc's representability rule (marker fits one frame, doc lines 183-184) and the code bound now coincide — my N1 trigger (50-event batch, ~4.4 KB proposal, conflicting second event) is now retained: `max_references = (1024−20)/24 = 41`, cap ≈ 41 KB. **N1 is resolved.**
- **Overflow safety:** `max_references ≤ (2³²−1)/24 ≈ 1.78×10⁸`, times `chunk_limit ≤ 2³²−1` stays far below 2⁶⁴. `limit < 48` preflight handles degenerate profiles (e.g. 64-byte payload → `max_references = 1`, `limit = 64`). The reserve at 571 is bounded by `limit` before any copy, as claimed. Record/sequence/file preflights (623-632) are unchanged and still exact.
- **Unchanged downstream math:** group splitting, intervening-commit sequence accounting, `together` fast path, and partial-failure gating (`prefix_retained`/`recording_failed_`) are byte-identical to the version I verified last round.

## New oracle verified by independent recomputation

`rejection_exceeds_single_batch`: fifth complaint detail 73 → encoded 8+16+16+4+73 = 117; proposal = 48 + 5×4 + (4×978 + 117) = **4097** (one byte past the old single-batch cap — genuinely minimal over-the-boundary). Chunks: 4×1024 + 1 → 5 references ≤ 41. Groups {3,2}: 3224 then 56+1056+33=1145. Sequences: 1/commit 2, chunks 3-5/commit 6, chunks 7-8/commit 9, marker 10/commit 11 → cursor **11**. End: 1178 + 3224 + 1145 = 5547; marker payload 8+5×24+12 = 140, batch 228 → **5775**. Both pinned values (771-772) match the format arithmetic exactly, and `proposal[44]==5` pins the event-count field. `rejection_preflight_limits` still fails explicitly with zero writes and a live gate — now via the derived cap (`limit=64`) rather than marker encode; the observable contract and the doc's "unrepresentable fails preflight without writes or gate change" both still hold.

## Residual nit (not a blocker)

`add` at 553-559 still uses `limit − 4` rather than `limit`. The −4 was legacy per-entry length accounting, but the length prefixes are already counted inside `size` (`size += 4 + bytes`), so a proposal in the window `(limit − 4, limit]` — realizable with a crafted multi-event batch whose total lands exactly there — is rejected with `capacity` despite being marker-representable. Same explicit-failure/no-write/gate-live behavior as N1, so no evidence loss beyond a 4-byte window. Fix: drop the `− 4` from both comparisons, or document the window. One-line change; suggest a boundary test at exactly `limit` and `limit + 1` when touched.

## Verdict

N1 resolves as claimed; the qualification chain (red log before correction, golden 3976-byte field-built proposal, tiny-marker refusal, partial sync/heap failures, native between-chunks-and-marker SIGKILL, plus the retry-sharing/dedup pins) is behaviorally anchored rather than round-trip. With the 4-byte `add` nit either fixed or documented, and pending final analysis/build runs passing, **the root component is correct within the declared bounds**. Whole U1 remains unaccepted: continuation, namespace entropy, raw-byte diagnostics, and emergency controls outstanding, as stated.
