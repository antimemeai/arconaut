Confirmed — I withdraw the residual note. Final correction below.

## Correction: withdrawal of the O1 residual "redundant-defensive" claim

**You are right, and my residual note #1 in the last review was wrong.** Tracing the actual operands in `src/journal_writer.cpp:484-497` for the combined-batch history in `restage_acknowledged_history` (`tests/journal_batch_test.cpp:743-754`):

- `records_` = the single acknowledged record (seq1, batch_first 1, payload 144–147), so `acknowledged = commit_boundary(records_.back())` = **{seq 2, end 203}** and `selected = records_.size() = 1`.
- `staged_` = the combined batch's two sources `[seq1@112 (batch_first 1), seq2@147 (batch_first 1)]`. Record equality at line 488 passes for the prefix (asserted byte-identical including CRC at test line 751).
- `actual = commit_boundary(staged_[selected - 1])` reads `staged_[0]` — the unchanged first source — giving **{seq 2, end 203}**, identical to `acknowledged`. The boundary check at 493-496 therefore *passes*, because the inferred boundary arithmetic is anchored at the last acknowledged record, not at the batch's true final record.

So the only thing refusing this history is the batch-finality guard at 489-490 (`selected < staged_.size() && staged_[1].batch_first == staged_[0].batch_first`, i.e. 1 == 1) — which correctly detects that the acknowledged record's inferred boundary is not a real commit boundary. Deleting only lines 489-490 would accept this rewritten history, and the combined-batch test **is** a direct red oracle for exactly that mutant. My earlier reasoning ("the 493-496 check alone suffices, 238 ≠ 203") erroneously assumed `actual` was computed from the last record of the combined batch; it is not. The guard is load-bearing, not redundant, and it is now properly oracle-pinned by the combined case (with `selected_multirecord_batch_and_diagnostic` pinning the symmetric guard in `select_recovered_prefix`). I explicitly withdraw the "redundant-defensive / guard-deletion blindness" statement. No mutation run was needed to see this; the operand trace alone proves it.

## Contract pins verified (docs/U1_CONTINUATION_SUBPLAN.md:42-122, now read)

All four behaviors from the fix round are explicitly pinned in the written physical contract:

1. **Busy payload reads during restage** — lines 61-62: "Payload validation also returns Busy during restage because a failed read can change writer state." Matches `journal_writer.cpp:603-605` (check before membership/I/O/state) and the callback oracle at test lines 918-922.
2. **Sticky Poisoned** — lines 63-64: "A payload failure never downgrades an unknown-write Poisoned state to Blocked; restage is the explicit operation that replaces that state through fresh recovery." Matches the guards at 617-619/630-632 and `payload_failure_preserves_unknown_write`.
3. **Cleared staging after confirm** — lines 65-66: "After confirmation, clear the old staging vector left by the records/staging swap; only the acknowledged records and pending inspection suffix remain populated." Matches `staged_.clear()` at 586 and the post-confirm emptiness assertions (tests 605, 775).
4. **Permanent no-resume flag** — lines 102-109: flag set in the successful selection call, survives restage/confirmation, checked in `resume_after_reconciliation`, "Restaging a flagged journal may recheck originals but never clears the flag or resumes writing." Matches code (538, 586-588, 595) and the strengthened no-reselection oracle (tests 607-612).

The contract also correctly pins the batch-finality rule my corrected analysis depends on: line 98-99, "Accept 0/112 or a boundary derived from the **final record** of a fully scanned commit batch; reject mid-batch."

## Final assessment

With the withdrawal above, no open findings remain in the scoped physical layer: all four prior findings (O1, O2, D1, D2) are resolved with genuine direct red oracles, the added reentrancy/flag-permanence oracles are sound and strictly stronger than their predecessors, and the written contract matches the code on every pinned point I checked. I did not execute anything; reported Mac debug/release/ASan+UBSan diagnostics, Neuroses all3 affected checks, and the Linux run are taken as operator-reported. Whole selected-chain ownership/U1 remains incomplete and nothing here broadens that acceptance.
