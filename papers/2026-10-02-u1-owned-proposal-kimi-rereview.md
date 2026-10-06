## Final scoped verdict — revalidation complete

### D1: **withdrawn**
Verified against actual `src/journal_writer.cpp`:
- Line 214: the capacity gate is `drafts.size() > capacity_.max_records - records_.size()`.
- Lines 250/290: `prepared`/`records_` receive exactly one record per draft (source/semantic frames only); the commit frame is encoded into `encoded` but never recorded.
- Line 292: commits consume sequence numbers only.

So in `retain_rejection`, total non-commit records consumed = `chunks.size() + 1` (marker), and the existing `chunks.size() >= remaining` is exactly `chunks.size() + 1 > remaining` — correct. My proposed `frames > remaining` conflated sequence exhaustion (which legitimately includes commits, hence `frames = chunks + groups + 2` for the `UINT64_MAX` check) with record-index capacity, and would falsely refuse the legal exact fit. The new `rejection_exact_noncommit_capacity()` (tests/retained_state_test.cpp:989–1023) proves this empirically: max_records=6 → 1 old semantic + 4 chunk sources + 1 marker exactly exhaust non-commit capacity, returns `conflict` (refusal captured), cursor 10 / end 5598, storage fully synchronized; max_records=5 → clean `capacity` refusal with zero writes, unchanged bytes, live state, no pending packet. No production change needed; my apologies for the false positive.

### Oracle gaps: all closed as described, verified in source
- **G1/G2** (`rejection_partial_failures`, lines 933–987): fail 3/4/5 pin **Poisoned**; fail 6 (post-group-1 allocation) and fail 7 (`fail_extent_after_sync_call = 3`, pre-write extent failure after group 1) pin **Blocked** via the explicit state equality at lines 964–965; full OUTER packet retained with original expected cursor and encoded events; `source({journal,3})` readable for fail ≥ 4.
- **G4** (lines 977–982): a later ordinary `append` while the rejected packet is pending is refused `audit_unavailable` with zero additional writes and byte-identical pending packet.
- **G3** (`no_allocation_after_acknowledged_sync`, lines 1025–1039): global allocation cut armed at 0 inside successful sync 2, yet the append succeeds — proving post-sync metadata publication (the reserved `records_.push_back` loop) performs no allocation after acknowledged sync. Combined with the previously read `allocation_before_publication`, this closes the allocation-ordering oracle.
- **G5**: agreed and withdrawn as a requirement — the two-blob truncation would exercise the same `ProposalReader::take`/`blobs` bounds mechanism already covered by the every-cut single-source golden and the exact empty-blob framing; no uncovered fault.

### Unresolved findings
None in scope. Production lifetime/allocation/error semantics for RAM pending-proposal ownership are validated: capture strictly before first storage write, retention only on unknown (Poisoned) outcomes or after any committed diagnostic group, clean clearing on acknowledged writes / completed marker / clean no-I/O refusals, no reopen synthesis, codec boundedness and exact byte ownership with controlled errors, and stale expected cursors preserved without authority in rejected purpose2 packets.

### Limitations
Review was of the supplied production excerpts plus the cited test regions (lines 920–1069); I did not re-audit `submit`/`dispatch`/`reconcile` bodies, the maintenance codecs, or any selected-chain persistence/replay/entropy path — per scope, no acceptance claim is made or implied for those pending areas, and the RAM packet is treated strictly as surviving caller originals, not source authority.
