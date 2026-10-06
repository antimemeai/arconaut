# Rereview: implemented multi-batch rejection correction and added oracles

Inspected read-only: final `retain_rejection` and the candidate-conflict/retry-pair changes in `src/retained_state.cpp`, amended sub-plan paragraph (`docs/U1_RETAINED_STATE_SUBPLAN.md:176-189`), and the new oracles in `tests/retained_state_test.cpp` (`large_rejection`, `rejection_partial_failures`, `rejection_preflight_limits`, `torn_reservation_burn`, `transition_matrix`, `retry_and_in_batch_conflict`) and `tests/retained_state_crash_test.cpp` (`rejection_crash_case` + `ChunkCrashFile`).

## Prior findings and required refinements: resolution status

- **B2 correction (all four amendments):** implemented and pinned. Marker-fits-one-frame bound with explicit preflight failure and no gate change (doc 183-185; `rejection_preflight_limits` verifies zero writes, unchanged bytes, live gate under a 64/512 profile). Synchronous non-interleaved retention (doc 185-186; guaranteed by the sequential custodian since all group/marker appends occur inside the failing `append_impl` call). Orphan chunks readable but semantically inert, no invented marker, torn marker under normal uncertainty (doc 187-189; verified by the crash case). Both envelope kinds share the form (doc 186-187; code path at `retained_state.cpp:577-585`).
- **Within-batch changed identity:** `existing(candidate, …)` at line 478; `retry_and_in_batch_conflict` pins a retained `IdentityConflictEvent` with `disputed_kind==decision`. Resolved.
- **Fresh retry decision per invocation:** `prior_retry` check at 234-240; tested for reuse rejection, fresh-decision acceptance, legitimate sharing of one decision across *different* invocations, and identical-retry dedup without writes. Resolved.
- **Torn commit-less reservation burn:** `torn_reservation_burn` writes a raw CRC-valid commit-less reservation-77 frame, asserts counter 77 burned, `incomplete` problem, blocked state, and issue refusal. Resolved.
- **Observation/receipt matrix:** every previously flagged branch now has an oracle, including undispatched-terminal disposition split, settling regression, post-terminal rejection, identical-terminal dedup (no writes), repeated running retained (writes increase), and error-receipt-after-terminal preserving terminal success. Resolved.
- **B1:** remains withdrawn; the sweep's resubmission oracle is intact.

## Multi-batch accounting: independently recomputed, correct

- Group splitting (595-610): 3×1056+56=3224 ≤ 4096, 4th chunk (936) overflows → groups {3,1}; references 3/4/5, commit 6, chunk 7, commit 8, marker 9, commit 10 — matches pinned refs 3/4/5/7 and cursor {10, 5598}. I recomputed offsets from the format: 112 + 1066 (first complaint batch) + 3224 + 992 = 5394; marker payload 8+4×24+12=116, batch 204 → 5598. Exact.
- Budgets: sequence-overflow check counts chunks + groups + 2 (correct, commits included); record check `chunks.size() >= remaining` is the correct off-by-one for chunks + 1 marker; `total_bytes = source_bytes + marker_bytes + 56×groups` matches the journal's own per-batch arithmetic, so preflight can't pass what `FramedJournal::append` would reject.
- Marker size is preflighted via encode with placeholder sequences (587); final sequences are encoded by `append_impl`'s own encode pass, and size is sequence-independent — sound.
- Partial-failure semantics: first-group allocation failure leaves the gate live (nothing retained); later prefix failure sets `recording_failed_` → blocked; marker failure sets it unconditionally; I/O failures poison the journal regardless. Caller receives the retention error, never a false `conflict`. `rejection_partial_failures` covers sync failure on group 1, group 2, marker, and a real operator-new failure armed after group 1's sync.
- Crash cut is genuine: `ChunkCrashFile::synchronize` SIGKILLs on the second *armed* full sync (group 2, after real F_FULLFSYNC-class sync of the native file); parent requires the signal, extent 5394, cursor {8, 5394}, clean recovery, complaint-only facts, chunk lengths 1024/1024/1024/904, empty custody set, and subsequent normal append growth.

## New finding (confirmed, minor severity)

**N1. The legacy `limit - 4` proposal cap still governs, rejecting marker-representable proposals — code contradicts the amended doc's representability rule.**
- **Location:** `src/retained_state.cpp:530-547` (`add` budget caps total proposal at `max_batch_bytes - 4`) versus doc lines 183-185, which define unrepresentability solely by the marker fitting one frame.
- **Trigger:** an `append()` batch of many individually legal small events where an early event conflicts and total encoded bytes exceed 4092 (1024/4096 profile) — e.g. 50 events of ~84 encoded bytes with event 2 a changed-identity duplicate. The per-event `add_size` checks stop at the conflicting event, but `encoded` spans all 50, so `retain_rejection` builds a ~4448-byte proposal, `add` fails, and the caller gets `capacity` with no retention — even though the marker would need only ~5 dependencies and is representable under the stated rule.
- **Consequence:** no corruption — explicit error, zero writes, gate stays live, originals unchanged — but retainable conflict evidence is discarded and the documented bound is wrong.
- **Fix:** chunk first, then derive the cap from marker dependency capacity (`floor((max_payload − envelope)/24)` chunks), letting marker-encode failure be the sole preflight bound; or amend the doc to state the `max_batch_bytes − 4` proposal cap explicitly. Add a red case at whichever bound is chosen.

## Bounded verdict

Within the declared scope (root journal only; no continuation, namespace-entropy chain, emergency controls, or U3 custody; process-crash rather than power-loss evidence), the implemented correction resolves B2 and every refinement from the previous round, with accurate multi-batch accounting, correct partial-failure gating, and genuinely independent byte/count/crash oracles. The only remaining defect is N1, a doc/code bound mismatch with explicit-failure (non-corrupting) behavior. **The root component can be considered correct within these bounds once N1 is resolved either by lifting the cap or by documenting it; no blockers remain.** Whole-U1 acceptance remains open as stated.
