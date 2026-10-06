## Review: U1 physical replay implementation refinement (docs/U1_CONTINUATION_SUBPLAN.md §"Selected-chain replay / Physical replay implementation refinement", lines 44–82) against include/arconaut/journal_writer.hpp, src/journal_writer.cpp, src/retained_state.cpp

Scope note honored: this layer pins same-FD restage + validated-prefix APIs only; I do not treat the absent selected-chain owner integration as a defect, and no tamper-resistance claim is imputed.

### Blocking findings

**B1 — Cached-CRC field semantics and population points are not pinned; current code paths would carry a placeholder into the containment key.**
- Locations: `PhysicalJournalRecord` (journal_writer.hpp:16–25, defaulted `operator==`), containment check in `read_payload` (journal_writer.cpp:454–457), record construction in `append` (journal_writer.cpp:250–253, 289–291), source-record construction in `RetainedState::append_impl` (retained_state.cpp:458–461), committed at retained_state.cpp:522.
- Trigger: doc line 53 caches the wire CRC "in its physical metadata," i.e. a new field on `PhysicalJournalRecord`. That struct's equality is the containment key for `read_payload`, and `RetainedState::append_impl` builds `candidate.sources` records **before** `journal_->append`, so any CRC populated by the writer at publication cannot be present in the semantic owner's copies.
- Consequences if left unpinned: (a) freshly appended (never scanned) records have no "existing validated wire CRC," so a same-session restage of the acknowledged vector has an undefined comparison operand — the doc's headline "CRC-valid source rewrite refused against the cached acknowledged frame checksum" fails for exactly the records most likely to be restaged (this session's unacked-at-crash tail); (b) if CRC joins `operator==`, semantic-owner copies become permanently unequal to journal records and every later `read_payload`/`source()` returns `stale_handle`; if it is excluded from equality, the restage comparison must say so explicitly or implementers will diff the whole struct and get false Corrupt or false accept.
- Fix: pin three things in the doc — (1) append populates the cached CRC from the frame header it just encoded at the same nonallocating publication point as the rest of the record (journal_writer.cpp:289); (2) scan populates it from the decoded frame; (3) whether the field participates in `PhysicalJournalRecord` equality/`read_payload` containment, with the doc line 80–82 "copy actual physical source metadata after successful append and before the nonallocating semantic publication" made an explicit replacement of retained_state.cpp:458–461 + 522 ordering (copy the appended tail of `physical_records()` into `prepared_->sources` after `written` succeeds, before `committed_.swap`). The doc currently states the destination ordering but not that the source construction at 458–461 must be superseded, nor the append-path population.

**B2 — The permanent no-resume flag's interaction with the existing state machine is unpinned.**
- Locations: `confirm_recovery` (journal_writer.cpp:426–444) unconditionally sets `recovered`/`blocked`; `resume_after_reconciliation` (445–451) gates only on `recovered` + clean report.
- Trigger: doc lines 70–72 — historical selection "permanently prevents resuming that segment for writing, even when its selected boundary equals physical EOF." A historical segment whose selected boundary equals EOF and whose scan is clean will, through the existing calls, reach `recovered` and then `live` unless the flag blocks the transition.
- Consequence: an old segment becomes writable again, forking the chain — the exact outcome the flag exists to prevent; the environment-owner selection check is the next layer and is not a substitute for the physical object refusing.
- Fix: pin that the flag is set at selection time (same call that moves suffix staged→pending), is checked inside `resume_after_reconciliation` (and any restage entry), survives `confirm_recovery`, and that a flagged journal's terminal state is inspection-only regardless of `recovery_.clean()`. Also pin which state restage is legal from ("immediately closes ordinary writes" implies `live`→`recovery_pending`; say whether `recovered`/`blocked` may restage) — currently unnamed.

### Non-blocking observations

- **Comparison precedence**: scan() reports damage via `recovery_.problem` and returns success (journal_writer.cpp:311–397). Pin that the acknowledged-prefix comparison runs on the partial staged vector even when scan stopped at damage, mismatch → Corrupt overrides the scan diagnostic, and the empty-acknowledged + damaged-first-batch case is a clean 0/112 acceptance, not Corrupt. Doc lines 56–59 imply this; making the precedence explicit costs one sentence.
- **Boundary operand**: pin the comparison cursor explicitly as `(sequence, end_offset)` equality against the acknowledged last record's `(sequence, payload_end+56)` — journal_writer.cpp:367 sets cursor from the commit frame, so this is derivable, but "same actual validated commit boundary" should name the two integers.
- **Capacity**: three live vectors (`records_` old acknowledged + `staged_` + `pending_`) each up to `max_records` during restage; all three are pre-reserved (journal_writer.cpp:62–64), so the "no allocation during selection move" claim holds, matching the existing pattern at journal_writer.cpp:419–421. No action.
- **Extent recheck**: restage must reset `recovery_.available_end` from the fresh `file_->extent()` before scan (open does this at journal_writer.cpp:145; scan's loop bound is `available_end` at 315). Doc line 46 "independently bounded extent" covers it; name the field to prevent an implementer scanning to the stale bound.
- Line 47 "scans from112" — typo (missing space).

### Suggested direct red histories (additive to doc lines 74–79)

1. Restage where the acknowledged vector's tail was appended **this session** (never scanned): catches missing append-path CRC population (B1).
2. Post-restage `confirm_recovery`, then `read_payload`/`source()` on a committed source record held by the semantic owner: catches the placeholder-CRC containment regression (B1).
3. Historical selection with selected boundary == physical EOF, then `resume_after_reconciliation`: must refuse `live` (B2).
4. Empty acknowledged vector + damaged first batch → accepted as 0/112; nonempty acknowledged vector + damaged first batch → Corrupt (precedence pair).
5. Restage after a prior `reject_recovery_batch`: acknowledged boundary taken from `records_` last record, not from `cursor_`/file EOF.

### Disposition

**Changes requested.** B1 and B2 are pin-level omissions that would produce divergent or unsafe implementations of exactly the seam this section exists to freeze (CRC metadata propagation/equality, and the no-resume guarantee). Both are one-paragraph doc additions, not redesign; everything else in the refinement is consistent with the implemented scan/append/confirm/reject machinery and the stated fault domain. Once those two pins and the precedence sentence land, this section is ready for the direct red cases it lists plus the five above.
