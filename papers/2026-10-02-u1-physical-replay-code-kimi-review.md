# Independent review: U1 physical restage / historical-prefix implementation

Files read in full or in relevant part: `docs/U1_CONTINUATION_SUBPLAN.md` (contract, lines 42–120 and resolved pins), `include/arconaut/journal_writer.hpp`, `src/journal_writer.cpp` (all 682 lines), `src/retained_state.cpp` (`append_impl` and recovery/source paths, lines ~300–540, 790–810), `tests/journal_batch_test.cpp` (all), `tests/journal_native_test.cpp` (`restage_keeps_native_lease`), `tests/retained_state_test.cpp` (source-read and `no_allocation_after_acknowledged_sync` regions).

## What checks out (verified against contract, no defect)

- **Acknowledged-prefix containment with damaged suffix**: `restage()` runs the acknowledged comparison (`src/journal_writer.cpp:484-498`) unconditionally after `scan(true)`, including when scan stopped at damage; empty `records_` correctly means boundary 0/112 rather than the unconfirmed cursor. The exact-source rewrite test (`tests/journal_batch_test.cpp:660-709`) genuinely isolates the cached-CRC refusal (fresh scan accepts the rewrite, acknowledged writer rejects), and the truncation loop covers every damaged/shortened suffix 112–202.
- **CRC population/routing**: append caches the just-encoded frame CRC pre-write (`journal_writer.cpp:269-274`), scan caches the validated wire CRC (`405-408`), defaulted `operator==` (`journal_writer.hpp:25`) makes it participate in containment, and `read_payload` re-verifies it (`623`). `RetainedState::append_impl` replaces placeholder source metadata from `physical_records()` after append, before the nonallocating swap (`retained_state.cpp:524-528`), and `no_allocation_after_acknowledged_sync` cuts allocation exactly in that window.
- **Historical fencing**: flag set only in successful selection (`538`), survives restage, blocks `confirm_recovery` promotion (`586`) and `resume_after_reconciliation` (`594`); both single- and cross-restage permanence are tested, including the native flock lease test.
- **Reentrancy**: all six mutating entry points check `in_restage_` before touching state, and the callback oracle (`journal_batch_test.cpp:822-838`) exercises all of them mid-restage.
- **Hard error vs diagnostic**: read/`interrupted`-exhaustion/extent errors propagate to `refuse` → blocked; `incomplete`/corrupt structure stays a diagnostic; both pinned by tests. Interruption budgets (7 retries + final failure = 8 attempts, reset on progress) match the contract and are asserted via call counts.

## Genuine defects / oracle gaps

### O1 — Medium — test oracle gap: mid-batch prefix selection is unreachable in every test
`journal_writer.cpp:516-519` (batch-final-record guard in `select_recovered_prefix`) and the symmetric guard at `489-490` in `restage()` are never exercised, because every selection/restage test uses single-record batches (1 source + commit per append), so adjacent `staged_` records always have distinct `batch_first`. Deleting the guard would pass the entire suite while accepting a batch-splitting prefix.
- **Trigger**: a multi-record batch journal; a child header naming the commit boundary derived from the *first* record of a two-record batch.
- **Consequence of the untested wrong behavior**: `select_recovered_prefix` accepts a mid-batch prefix, splitting a commit batch — exactly what contract line 94-95 forbids.
- **Direct red oracle**: with `header().limits` permitting it, append `{drafts[0], drafts[0]}` in one batch (records seq1@112, seq2@147, commit@182–238), then a second single-record batch. Open, then `select_recovered_prefix({journal, 2, 203})` (commit_boundary of record 1: seq 1+1=2, end 144+3+56=203) must return `invalid_range` with `staged_records()`, `pending_records()` and cursor unchanged. An implementation without the `batch_first` skip accepts it. Also apply the same shape to the restage guard at 489-490.

### D1 — Low — `read_payload` failure downgrades `poisoned` to `blocked`
`src/journal_writer.cpp:613` and `:625` set `state_ = JournalWriterState::blocked` unconditionally on read/CRC failure. On a `poisoned` journal (a write of unknown outcome may be on disk), a subsequent failing `read_payload` (e.g. inspecting a pending record after a torn append) silently reclassifies it `blocked`.
- **Trigger**: poison an append (e.g. `fail_after` mid-write), then `read_payload` a staged/pending record whose bytes were torn or corrupted.
- **Consequence**: the "unknown write may have landed" distinction is lost. Today both states fence writes and permit restage, so immediate impact is small, but any future consumer treating `blocked` as a known-clean-closed state would be wrong, and the poisoning evidence in `recovery_` is overwritten.
- **Fix**: keep poisoned sticky — only transition to `blocked` from `live`/`recovered`/`recovery_pending`, e.g. `if (state_ != JournalWriterState::poisoned) state_ = blocked;`.
- **Red oracle**: poison via `fail_after = 130` append failure; corrupt a byte in the first (committed) frame; call `read_payload` on a pending record; assert `state() == poisoned`.

### O2 — Low — oracle gap: `select_recovered_prefix` prepend ordering into a non-empty `pending_` is untested
`journal_writer.cpp:531` inserts the deselected staged suffix at `pending_.begin()`. All selection tests select against journals whose `pending_` is empty (clean two-batch file). An implementation that appended instead of prepending (misordering pending inspection records) would pass everything.
- **Red oracle**: two committed batches plus a partial third-batch tail (uncommitted record in `pending_`); select prefix `{2, 203}`; assert `pending_records()` sequences are strictly ascending (`[3, <tail seq>]`) and `staged_records().size() == 1`.

### D2 — Low — `confirm_recovery` leaves the old acknowledged vector visible as `staged_records()`
`journal_writer.cpp:585` (`records_.swap(staged_)`) leaves the pre-confirm acknowledged records in `staged_` until the next restage. After a selection shortened the prefix, `staged_records()` then returns the *old, longer* acknowledged vector while `physical_records()` returns the selected prefix — a misleading inspection surface for the future semantic owner, which the contract says must compare against the actual acknowledged boundary.
- **Trigger**: restage → select a proper prefix → confirm → inspect `staged_records()`.
- **Fix**: `staged_.clear()` after the swap (trivial, nonallocating), or document/pin the post-confirm contents. **Red oracle**: after the sequence above, assert `staged_records().empty()`.

## Scope-separated observations (missing whole-owner integration, not scoped defects)

- `RetainedState::source` (`retained_state.cpp:792-804`) routes every committed source read to the single `journal_`. This is correct only because one segment exists; per contract line 137 ("route source reads to the owning segment; never send an older source record to its newest file"), multi-segment routing belongs to the unimplemented selected-chain `RetainedEnvironment` layer. No fault in this scope, but the function has no journal-mismatch guard beyond record equality, so the future owner must not reuse it unchanged across segments.
- Semantic replay restriction to the selected prefix, predecessor-chain validation, and head publication are correctly absent here; nothing in the physical layer claims them.

## Limitations

Review is static only (no builds run; debug-pass and host-qualification claims taken as stated). I did not audit `encode_journal_frame`/`decode_journal_frame` internals beyond the offset-24/28 length/CRC layout consistency that the hand-derived byte oracles already pin, nor the retained-state codec regions outside the append/recovery paths. `tests/retained_state_test.cpp` was read selectively around source-read and allocation-cut oracles; post-reopen source reads with scan-populated CRCs appear covered (e.g. lines 400-404), but I did not trace all 96 `source()` call sites.
