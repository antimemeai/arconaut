## Verification of the four oracle closures

I read the current oracles in `tests/journal_batch_test.cpp:53–137, 250–343` and `tests/retained_state_test.cpp:157–165, 388–407`. Production reader unchanged (confirmed scope). Results:

### Gap 1 — non-interrupt extent error → **CLOSED**
`MemoryFile::extent` (journal_batch_test.cpp:121–128) now has `extent_failure` returning `{ErrorCode::io, 5}` *before* the interrupt branch, so it exercises the non-retryable path at `src/journal_writer.cpp:498–500`. Oracle at lines 299–301 asserts `ErrorCode::io` and resets the flag; unchanged writer state is covered by the collective assertion at line 310 (`cut->state() == original_state`). Correct both sides.

### Gap 2 — exact 64 accept / 65 reject → **CLOSED**
Line 280–282: `read_original_range(0, 64)` (== max_payload 64 from `header().limits{64,512}`, line 176) succeeds with byte-exact comparison against `before[0..64)`; `before` is ≥112 bytes so the range is in-bounds. Line 286 still rejects 65. An off-by-one (`>` vs `>=`) in the production guard is now caught.

### Gap 4 — two-byte progress then eight interruptions → **CLOSED**
Lines 296–297: with `max_read = 2` and `interrupts_between_reads = 8`, `read_original_range(0, 3)` makes a 2-byte successful read (mock re-arms `interrupt_reads = 8` at line 95), then hits 8 consecutive interruptions and returns `ErrorCode::interrupted`. The failed `Result` carries no partial copy, and the unchanged-state assertion at line 310 covers this case. The pre-existing 7-between-progress (line 268, chunked reconstruction) and 8-at-start (lines 294–295) oracles are intact. Mock bookkeeping is sound: the 8 interrupts are consumed by the failing call, so subsequent cases start clean.

### Gap 3 — plain Live/Recovered/Blocked → **PARTIALLY CLOSED — one confirmed remaining issue**
- **Recovered** and **Blocked**: closed. Lines 318–322 read `(0, 64)` after `confirm_recovery()` with byte-exact comparison, then assert the state is exactly `recovered` (end==112) or `blocked` (end>112) — so both the bytes and the no-state-change property are oracled per iteration.
- **Live**: still not oracled anywhere. After the successful `resume_after_reconciliation()` at line 326 the loop advances without any read; the next live writer (`partial`, lines 330–332) is deliberately poisoned by `fail_after = 130` before its read at line 338. On the RetainedState side, every inspection is in `recovery_pending`, `poisoned`, or synthesized-`blocked` (the receipt-failure case at retained_state_test.cpp:500 is physically live but the public `state()` reports blocked, and no read happens before the failure injection).
  - **Consequence:** the doc claim (U1_RETAINED_STATE_SUBPLAN.md line 318) "available in every writer state" still lacks a plain-`live` oracle. The reader has no state branching, so risk is negligible, but the stated closure is inaccurate as written.
  - **Fix (one line each):** after line 326's successful resume, add `CHECK(require(cut->read_original_range(0, 64)).bytes == std::vector<std::byte>(before.begin(), before.begin() + 64));` and `CHECK(cut->state() == JournalWriterState::live);`.

### `pending_source_inspection` (retained_state_test.cpp:388–407, wired into `main` at 903)
Logic verified: offset 144 = 112-byte header + 32-byte frame header, `original` is 3 bytes (line 165) so it sits exactly at the source-frame payload; `fail_sync` append fails with `io` leaving bytes submitted and the writer poisoned. The oracle asserts exact bytes at 144 while `source()` stays `audit_unavailable` and `committed_facts()` is empty, in both the poisoned writer (state asserted, line 398) and the reopened pending writer (lines 404–406), with `source()` re-checked *after* inspection (line 400) proving non-interference. One nit (not a fault): the reopened leg never asserts `state() == JournalWriterState::recovery_pending` or state-unchanged-after-read — the empty facts + `audit_unavailable` imply it, but an explicit assertion would match the rigor of the other legs.

### Summary
Gaps 1, 2, 4 are fully closed with correct oracles and no collateral damage to existing cases. **One remaining confirmed issue: plain-`live`-state inspection is claimed but still has no oracle** — add the two lines after the successful resume (journal_batch_test.cpp:326). Optionally assert `recovery_pending` explicitly in the reopened leg of `pending_source_inspection`. No production-code concerns; the pending rerun of these direct oracles is the right remaining step.