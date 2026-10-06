# Re-review: verification of O1/O2/D1/D2 resolutions

Re-read in full: `src/journal_writer.cpp` (690 lines, current) and `tests/journal_batch_test.cpp` changed regions (lines 590–957). Cross-checked the claimed red oracles against the actual guard code.

## Verdict: all four prior findings are genuinely resolved, with real oracles

### O1 (mid-batch selection guard) — Resolved, both directions now covered
- `selected_multirecord_batch_and_diagnostic` (`tests/journal_batch_test.cpp:663-692`) builds a real two-source batch (seq1@112, seq2@147, commit@182–238, end 238) plus a damaged tail, then asserts `select_recovered_prefix({journal, 2, 203})` — the commit boundary *inferred from the first record* — returns `invalid_range` with cursor/staged unchanged (lines 679-681). Without the `batch_first` skip at `src/journal_writer.cpp:516-519`, `commit_boundary(staged_[0])` = {2, 203} would match exactly, so this oracle is red against guard deletion. It also pins that a valid selection preserves the damage diagnostic offset (line 689, matching the `!recovery_.problem` condition at 535-537) and proves selection is nonallocating via `allocation_cut` (682-685).
- `restage_acknowledged_history` (`743-754`) adds the harder containment-evasion case: a combined two-source batch whose first source is byte-identical to the acknowledged record (asserted equal, including cached CRC, at line 751) but whose original commit boundary {2,203} no longer exists. Restage refuses `corrupt` (753). This exercises the mid-batch guard at `src/journal_writer.cpp:489-490` (`staged_[1].batch_first == staged_[0].batch_first`), which pure prefix-equality would not catch. Note the boundary check at 493-496 would also refuse (203 vs 238), so the guard-specific redness rests on the equality+ordering path — acceptable, since line 751 proves record-level containment passes and only the guards remain to refuse.

### O2 (pending ordering on selection) — Resolved
`selected_prefix_pending_order` (`756-778`): two committed batches plus a complete-but-uncommitted source seq5 (`fail_after = 329` leaves the frame but no commit), select {2,203}, assert `pending_records()` is exactly `[3, 5]` both before and after `confirm_recovery` (772-777). An append-instead-of-prepend at `journal_writer.cpp:531` would yield `[5, 3]` and fail. Allocation during selection is excluded by arming `allocation_cut = 0` across the call (767-769) — valid because both vectors were reserved to `max_records` pre-I/O and combined size is bounded by it.

### D1 (poisoned downgrade) — Resolved in code and oracle
`journal_writer.cpp:617-619` and `630-632` now keep `poisoned` sticky on both I/O and CRC read failure. `payload_failure_preserves_unknown_write` (`779-795`) poisons via `fail_after = 215` mid-append, then injects a hard read failure (789-790) and a payload-bit corruption (792-794), asserting `poisoned` after each. This is red against the old unconditional `state_ = blocked`. Residual cosmetic note (not a defect): `recovery_.problem`/`diagnostic_offset` are still overwritten on a poisoned journal's read failure; since the poisoned-append path never sets `recovery_.problem`, no evidence is actually destroyed, and state — the load-bearing fence — is preserved.

### D2 (stale staged vector after confirm) — Resolved
`journal_writer.cpp:586` clears `staged_` after the swap, and both `selected_replay_prefixes` (605) and `selected_prefix_pending_order` (775) assert `staged_records().empty()` post-confirm, red against the old swap-only behavior.

## New reentrancy oracle — verified correct

`journal_writer.cpp:603-605` checks `in_restage_` before membership, I/O, or any state write. The callback at `journal_batch_test.cpp:918-922` calls `read_payload` with an armed read failure mid-restage and requires `busy` — correctly targeting the mutation risk (the old path would have returned `io` *and* blocked the writer mid-scan). The flag-permanence redesign (`607-612`) is sound: restage → confirm *without* reselection must still yield `blocked` and refuse resume; a restage that cleared `historical_` would promote to `recovered` and let `resume_after_reconciliation` succeed, failing the oracle. This is strictly stronger than the previous reselection-based test.

## Residual gaps (minor, not blocking)

1. **Low — symmetric mid-batch guard test asymmetry**: in `restage_acknowledged_history`'s combined case, both the 489-490 guard *and* the 493-496 boundary mismatch fire. A guard-deletion mutant at 489-490 specifically would still be caught only if the boundary check were also absent — but since record equality passes and the boundary {2,203} genuinely doesn't exist in the combined file, the 493-496 check alone suffices there. A mutant deleting *only* 489-490 is caught by nothing if... actually it is caught: deleting 489-490 leaves 493-496, which refuses (238 ≠ 203). So 489-490 remains technically redundant-defensive rather than independently oracle-pinned. Acceptable as defense-in-depth; noted for completeness.
2. **Low — reentrant `read_original_range`** remains non-`busy`-guarded during restage (it is read-only and state-nonmutating, so this matches the written contract; the new contract text pins only state-mutating reads). No action needed unless a future storage callback can observe torn staging through it.

## Scope statement

Whole-owner integration observations from the prior review are unchanged: `RetainedState::source` single-journal routing (`src/retained_state.cpp:792-804`) and semantic prefix restriction remain correctly deferred to the unimplemented selected-chain layer; nothing in this physical component claims them. I did not re-audit unchanged regions beyond spot consistency, did not execute the suites (debug pass and Mac/Linux qualification taken as reported), and did not verify the "written physical contract" document text itself — only the code behavior it describes.
