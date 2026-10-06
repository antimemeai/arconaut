## Withdrawal confirmed — plain-Live oracle exists

I read `tests/journal_batch_test.cpp:185–226` and `main` at 574–582. Confirmed exactly as you stated:

- **`commitment()` runs first in `main`** (line 576).
- **Plain-Live inspection, lines 190–194:** immediately after successful `FramedJournal::create` (writer state `live`, only the synced 112-byte header on disk), `read_original_range(0, 64)` succeeds, bytes are compared exactly against the raw storage model (`directory.state->submitted[0..64)`), and `writer->state() == JournalWriterState::live` is asserted *after* the read — so both exact-byte content and non-interference are oracled in the Live state. Range is in-bounds (64 ≤ 112).
- Additionally, this same function oracles a **Recovered**-state inspection at lines 220–222: `read_original_range(144, 3)` returns the exact original source payload (`{0, 255, 10}`) with `recovered` state asserted unchanged both before and after — complementing the Recovered/Blocked oracles in `failures_and_cuts` (lines 318–322).

**I withdraw the "missing plain-Live oracle" finding.** My earlier read covered only `failures_and_cuts` and the RetainedState legs and did not include `commitment()`; that was a scope error on my side, not a test gap. Per your instruction, no duplicate Live case should be added.

## Final scoped verdict

- **Production code** (`read_original_range` in `src/journal_writer.cpp:487–531`, declarations in `journal_writer.hpp:44–50,76–77`, forwarding in `retained_state.hpp:69–73`): unchanged since my prior review; no findings. Bounds, checked subtraction, exact-size allocation, EINTR budgets (7 tolerated/8th error, progress reset, 8-call extent bound), short-read accumulation, zero-progress/impossible-count/OOM mapping, and absence of any state/authority/reset side effect all remain as previously verified.
- **All four prior oracle gaps: closed.** (1) non-interrupt extent `io` (mock `extent_failure` + oracle at 299–301); (2) exact 64-byte accept vs 65 reject (280–282, 286); (3) Live/Recovered/Blocked inspection with exact bytes and unchanged states (190–194, 220–222, 318–322); (4) two-byte progress + eight interruptions → `interrupted` with discarded partial copy and unchanged state (296–297, 310). Pre-existing 7-between-progress and 8-at-start oracles intact.
- **`pending_source_inspection`** (retained_state_test.cpp:388–407): exact pending source bytes at offset 144 readable while `source()` stays `audit_unavailable` and authoritative facts are empty, in both Poisoned and reopened RecoveryPending legs; non-interference re-checked post-read. The only residual is a cosmetic nit (no explicit `recovery_pending` state assertion in the reopened leg), which does not affect oracle validity.

**No remaining confirmed issues in scope.** Given the reported Mac debug/release/ASan+UBSan and Neuroses all-3 profile passes on this exact source, the bounded original-byte inspection code and oracles are approved as-is from this review.