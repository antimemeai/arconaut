# Independent review: docs/U1_CAPTURE_SUBPLAN.md

## Scope inspected

- `docs/U1_CAPTURE_SUBPLAN.md` (full), `docs/U1_CONTINUATION_SUBPLAN.md` (full), `docs/U1_RETAINED_STATE_SUBPLAN.md` (full; uncertainty/custody/diagnostic-retention clauses).
- `include/arconaut/retained_events.hpp`, `include/arconaut/retained_proposal.hpp`, `src/retained_events.cpp`, `src/retained_proposal.cpp`, `src/retained_state.cpp`, `src/retained_environment.cpp`, and commit-frame handling in `src/journal_writer.cpp`.

## Literal/oracle verification (all pass)

- ARPROP01 packet 214 = 48 + (4+3 source) + (4+155 event) ✓; decision payload 155 = 8 envelope + 24 dep + 96 IDs + 4 count + 16 invocation + 4+3 continuation ✓ (matches codec in `src/retained_events.cpp:179-191` and proposal layout in `src/retained_proposal.cpp`).
- Choice body fixed 108 (48+8+16+16+12+4+4) and payload 152 = 8 envelope + 108 + 36 descriptor ✓ against `encode_body(RecoveryChoiceEvent)` (`src/retained_events.cpp:248-278`).
- Marker payload 68 = 8 envelope + 24 dep + 36 capture body ✓ (`decode_capture`, 16+8+8+2+2).
- end352 = 112 + (32+152) + 56 ✓; 352+246+100+56 = 754 ✓ (source frame 32+214, marker frame 32+68, commit 56).
- Entry count 4 (choice+marker facts, 1 provisional original, 1 uncertain fact; chunk is capture-only, 0 ordinary sources); budget 3 refuses, byte budget 213 refuses / 214 succeeds ✓ consistent with the subplan's counting rule and `max_capture_bytes` aggregate.
- Commit frames are absent from the physical-record index (`src/journal_writer.cpp:394-408` — only non-commit frames enter `staged_`), so the "contiguous excluding commits" claim is accurate.
- Descriptor canonical order (origin bytes, first_sequence, purpose) and checked-attempt order match `capture_less`/`valid_choice` (`src/retained_events.cpp:140-175`); strict `less` also rejects same-(origin,sequence,purpose) duplicates with different lengths, so the codec backstops the subplan's "no repeated/differently sized" rule.

## Findings

### 1. Material — maintenance-region semantic records other than choice/markers are not expressly refused

- **Location:** U1_CAPTURE_SUBPLAN.md lines 40-56; mechanism at `src/retained_state.cpp:381-384` (`sequence <= maintenance_end` silently skips semantic records).
- **Trigger:** A selected segment whose physical records are: choice(seq1), an *ordinary* semantic event (seq2, e.g. a decision), then a complete valid chunk group and marker (seq3..N). The subplan's stated rejections cover: semantic events *interleaving a group* (line 42), unmatched *sources* in the region (line 55), and extra/later choices or markers (line 48). A non-marker, non-choice semantic record sitting *between the choice and the first group* (or between two groups) matches none of those clauses.
- **Consequence:** With `maintenance_end = last marker sequence`, shared replay would silently drop that semantic record (`sequence <= maintenance_end`) instead of refusing activation — silent history loss in the selected chain, contradicting the whole-continuation invariant that acknowledged semantics are reproduced exactly or refused.
- **Minimal correction:** Add one sentence: the entire maintenance region (sequences 2..maintenance_end) must consist solely of validated group chunk sources and validated marker semantic records; any other semantic record (or any record) in that region refuses activation, before ordinary replay.

### 2. Moderate — later-segment authoritative duplicate of an earlier uncertain identity is unspecified

- **Location:** U1_CAPTURE_SUBPLAN.md lines 80-83 ("If an authoritative *prior* fact has the same identity…omit"; existing-uncertain matching). U1_CONTINUATION_SUBPLAN.md line 251 only covers suppression against *prior* recovered records.
- **Trigger:** Event E is captured purpose1/uncertain in segment 1's choice (its original append was never acknowledged). After continuation, the caller resubmits E fresh and it is committed authoritatively in segment 2. On reopen, replay parses segment 1's capture first (creating the uncertain entry), then replays segment 2's ordinary commit of E.
- **Consequence:** The plan does not pin the outcome: does ordinary replay's extended identity dedup treat the uncertain entry as "existing" and skip the authoritative frame (losing the authoritative record/evidence upgrade), apply it as a second fact (two entries, double-counted against `max_history_entries`, and divergent `existing` results), or conflict? U1_CONTINUATION line 210-212 ("identity dedup examines ordinary and uncertain indexes…exact uncertain reuse is Existing/Uncertain with no publication") reads as *skipping* the authoritative frame, which would leave the better-evidenced record unpublished — an evidence/custody downgrade.
- **Minimal correction:** State explicitly that during selected-chain replay, an exact (identity + full event + dependencies) authoritative record retiring a prior uncertain entry is applied as the authoritative fact and the uncertain entry is dropped (or retained but never suppresses the authoritative record); pin the resulting entry count and query evidence. A changed-body match remains a rejection.

### 3. Minor — disposition of "same uncertain identity, different bytes" is ambiguous in this document

- **Location:** U1_CAPTURE_SUBPLAN.md lines 82-83: "An existing uncertain identity must also match exactly; retain its first original record, never a second identity with different bytes."
- **Trigger:** A second purpose1 capture (in its own segment) whose event shares an identity with an existing uncertain fact but has different body/dependency bytes.
- **Consequence:** Read alone, the sentence could be implemented as "silently keep the first, accept the capture," whereas the accepted continuation rule (line 754-755: "an inconsistent duplicate capture discovered in replay rejects that batch/suffix") requires rejection. Divergent implementations either accept contradictory retained identities or refuse a valid chain.
- **Minimal correction:** Mirror the continuation language here: same identity with different bytes rejects the capture/segment during replay validation; "retain its first original record" applies only to exact duplicates (which are inherited/not re-declared).

## Non-findings checked and cleared

- Packet-provenance vs first-semantic precedence (lines 90-116) is internally consistent and matches the continuation subplan's resolved dedup rules, including pure all-existing queries, duplicate ordinals, and "existing=true/Uncertain per original position even when authoritatively known."
- purpose1 vs purpose2 distinction is preserved: purpose2 never seeds uncertain identities, permits stale/foreign cursors, and is consistent with what the existing diagnostic path can actually produce (`retain_rejection` only captures successfully-encoded events, so "origin-limit encoded events" is attainable; oversize source *blobs* inside a purpose2 packet remain possible and are only chunk-bounded, which the subplan permits).
- Reservation namespace handling (origin namespace for captured reservations; new namespace cannot suppress origin ones) matches the namespace-keyed identity rule in `src/retained_state.cpp:31-42,160-171`.
- Snapshot counting, checked subtraction, `max_capture_bytes` aggregation, and per-vector `max_size` validation align with the continuation subplan's exact accounting definition and the existing `has_room`/`validate` shape.
- Checked-attempt collector rules (chronological merge across segments/input positions, authoritative terminal settles, uncertain terminal cannot settle, evidence-class normalization with the original byte retained for study) do not contradict the wire codec's `CheckedAttempt` validation or U3 custody limits; reconcile including provisional admissions after confirmation matches the accepted "no synthetic terminal" rule.
- No direct oracle duplicates an existing proof layer; the stated construction order (classify/packet/index → lookups/diagnostics → checked-set/custody, with explicit refusal until integrated) is consistent with the current `ErrorCode::unsupported` refusal in `load_selected` (`src/retained_environment.cpp:275-276`).

## Limitations

- I did not execute or build anything (read-only review); oracle arithmetic was verified by hand against the codecs.
- I did not re-review the already-resolved continuation/capacity/physical-replay sections except where the capture subplan interacts with them; U2/U3 worker-custody claims, entropy, and head publication are explicitly out of this refinement's scope and were not re-litigated.
- The unimplemented pieces (uncertain index, provisional originals, capture validation) were assessed only against their specified contracts and the existing machinery they extend.
