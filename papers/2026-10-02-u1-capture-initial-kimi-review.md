# Independent review: initial U1 capture/index/query implementation

## Scope inspected

- Specs: `docs/U1_CAPTURE_SUBPLAN.md` (resolved), `docs/U1_CONTINUATION_SUBPLAN.md`
- Code: `include/arconaut/retained_environment.hpp`, `include/arconaut/retained_state.hpp`, `src/retained_environment.cpp` (686 lines), `src/retained_state.cpp` (982 lines)
- Oracles: `tests/retained_environment_test.cpp` (817 lines); cross-checked `select_recovered_prefix`/`reject_recovery_batch` semantics in `src/journal_writer.cpp:502-590` where findings depended on them.

Read-only review; no execution. I treat FramedJournal/codec/head layers as accepted per their resolved reviews. The staged Unsupported refusals (attempt-related purpose1 captures at `src/retained_environment.cpp:224-228`; nonempty `checked_attempts` at `:271-272`) are present as declared and I do not count the pending collector/entropy/restage/publication/bad-header/emergency/Linux scope as defects.

## Verified correct against the pinned contract

- **Group classification**: contiguous source-then-marker scan (`replay_maintenance`, `src/retained_environment.cpp:278-338`) rejects zero-chunk markers (`:282`), misreferenced/out-of-order/other-journal chunks (`:315`), missing markers, extra markers (required-set `find` + `seen` dedup `:294-296`), inherited redeclaration (`:299-301`), later choice/marker in the suffix (`:341-357`), and ordinary semantic records between choice and first group or between groups (absorbed then rejected via dependency-count `:297`, or refused at `:292`). Stray sources cannot leak past a group undetected.
- **Packet bounds/origin/limits**: origin-namespace reservation keys, origin `max_payload` decode (`stage_original:213`), single-batch origin framing fit (`:194-209`), declared-record-policy counts (`:191-192`), expected-cursor/first-sequence overflow (`:187-189`, `:206-209`), aggregate `max_capture_bytes` with checked subtraction before `reserve` (`:302-312`), purpose2 exclusion from identity seeding (`:185`, `:216`).
- **Index accounting**: `has_room` counts facts+sources+uncertain+originals with checked subtraction (`src/retained_state.cpp:217-229`); uncertain entries and originals each pre-checked (`stage_original:254-260`); choice+markers counted together (`replay_maintenance:367`). Pinned budget-3-refuses/4-succeeds and 213/214 byte tests match the code paths.
- **Query semantics**: whole-packet equality precedes state/profile gates (`submit_proposal:568` vs `:610`); per-position Uncertain results including duplicate ordinals (`:416-423`); source-only capture returns empty events (`existing_proposal` naturally); individual exact dedup prefers authoritative then uncertain (`retained_state.cpp:772-781`, `submit:773-781`); zero cursor movement pinned and code paths are write-free; changed identity falls through to retained IdentityConflict; mixed uncertain/new refuses Conflict with the **outer** ARPROP01 retained (`retained_state.cpp:577-587` + test `:269-283`); uncertain entries never satisfy parents/sources (`apply:240-253`, `retained_state.cpp:270-290` — pinned by the invocation Conflict at test `:284-292`); capture chunks never enter `ordinary_sources` (`replay:400-402` + `source()` only scans `committed_`/`visible().sources`, pinned stale_handle at test `:228`, `:365-366`); confirmation never promotes uncertain (`retained_state.cpp:920-922` only touches `facts`).
- **Batch replay**: per-batch candidate copy/swap is atomic (`replay:395-449`); historical invalid batch → `reject_recovery_batch` mutates the fenced prefix and is caught by the cursor-unchanged check (`retained_environment.cpp:538-540` → Corrupt, pinned); active invalid tail → blocked prefix (pinned `SemanticFault::active`); ordinary replay of a still-uncertain event rejects Conflict even with equal body (`apply:240-241`).

## Findings

No critical correctness defects found in the scoped initial paths. Residual items to resolve before relying on them:

**F1 (low, latent): position-blind non-keyed uncertain matching in ordinary replay vs position-scoped capture suppression.**
`existing_uncertain` (`src/retained_state.cpp:204`) matches non-keyed events by full-body equality *anywhere* in the uncertain index, so `apply()` (`:240`) rejects an ordinary replay of an equal non-keyed event regardless of position; but capture suppression of non-keyed events in `stage_original` (`src/retained_environment.cpp:236-241`) is position-scoped to the predicted record, and the uncertain-duplicate `continue` is gated on `has_identity` (`:249`). Today unreachable (all capture-admissible events are keyed; attempt-related refuse Unsupported), but when the collector lands and non-terminal `AttemptObservationEvent` packets become admissible, suppression and replay-conflict will use inconsistent positional semantics. Minimal correction: decide one rule (spec line 96 favors per-position for non-keyed) and make `apply()`'s uncertain check position-aware, or document the asymmetry. Red test: capture a packet with a non-terminal observation, then ordinary-replay an equal observation at a *different* sequence — pin the intended outcome.

**F2 (low): dedup-ordering deviation for oversized bulk input.**
`submit_proposal` refuses `sources+events > bounds_.segment.max_records` (`src/retained_environment.cpp:564-566`) *before* the exact-capture lookup at `:568`. Spec requires dedup to precede refusal. Currently unreachable in practice (a captured packet had to fit origin policy ≤ configured bounds), but a caller re-opening with larger configured bounds than a peer and resubmitting an over-configured-bounds bulk input gets Capacity instead of Conflict/Existing. Minimal correction: move the exact-capture lookup ahead of the count pre-check, or pin Capacity-first as intended with a test.

**F3 (oracle gap — spec oracle 2 mostly unimplemented).** Only missing-marker (`missing_capture`) and interrupted-region negatives exist. Missing direct reds, each cheap to add to the existing fixture style:
- extra marker beyond the required set; marker not declared in the choice;
- misreferenced marker (wrong order, unknown sequence, chunk from another journal);
- descriptor `proposal_length` mismatch vs concatenated chunks (both directions);
- origin mismatch (marker `origin` ≠ immediate predecessor);
- sequence overflow (`expected.sequence == UINT64_MAX` in packet bytes);
- malformed packet and trailing bytes after a valid ARPROP01 parse;
- origin-narrow limits (event exceeding origin `max_payload` but fitting the child);
- purpose2 with stale/foreign cursor accepted as capture yet seeding no uncertain identity and no `existing_proposal` match.
Trigger/consequence if any regresses: silent ordinary-source authority or partial-capture acceptance. Each maps to one existing check (`replay_maintenance:282-327`, `stage_original:181-228`); the tests pin them against deletion.

**F4 (oracle gap — spec oracle 3 incomplete).** No red tests for: changed identity *inside a required capture* (same keyed ID, changed bytes vs existing uncertain/authoritative → environment refusal, `stage_original:243-245`, `:250-252`); authoritative suppression (event recovered authoritatively elsewhere → no uncertain entry, later `submit` returns Recovered, not Uncertain); duplicate keyed positions inside one captured packet producing one uncertain fact with per-ordinal query results; exact captured-packet query under a narrower active profile (analogue of `dedup_before_narrowed_profile` for the capture path); mixed proposal with *added/changed sources* around equal uncertain events.

**F5 (oracle gap — spec oracle 5 absent).** No allocation-fault injection at snapshot/packet/result ownership boundaries (`replay_maintenance:312` reserve, `stage_original:256/261` pushes, `submit_proposal:574` result reserve, `load_selected:431/462`), and no selected-marker payload corruption case proving open refuses without partial publication or source leakage. The code's capacity-before-allocation ordering is correct as written, but nothing pins it.

## Limitations

I did not execute the tests or fault-inject; judgments about `FramedJournal` (commit frames absent from `staged_records`, `available_end` surviving `select_recovered_prefix` — verified at `src/journal_writer.cpp:502-540`, which does keep full observed extent, so the historical-suffix choice check at `retained_environment.cpp:517` is sound) rest on the previously resolved physical-layer reviews. Original review reports were not modified.
