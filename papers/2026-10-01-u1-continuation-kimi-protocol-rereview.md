# Re-review: `docs/U1_CONTINUATION_SUBPLAN.md` (current, post-correction)

Same scope as before: the subplan, `U1_ENVIRONMENT_HEAD_SUBPLAN.md`, `U1_RETAINED_STATE_SUBPLAN.md`, `include/arconaut/journal.hpp`, `retained_events.hpp`, `journal_writer.hpp`. All prior findings are verified against the current text.

## Verification of the corrections

**Arithmetic — now correct.** Fixed body: predecessor triple 32 + diagnostic pair 16 + old limits 8 + old capacities 16 + new capacities 16 + error triple 12 + checked count 4 + capture count 4 = **108**. Checked entry 16+1+1+1+1 = **20**. Descriptor 16+8+8+2+2 = **36**. "108 + 20·attempts + 36·captures" (line 165) checks out field by field.

**Required-capture set — Blocker 2 resolved.** The descriptor list (lines 163–167) makes the required set authoritative on-disk selection input, replay-recomputable after RAM loss, with missing/extra/mismatch rejection (lines 204–206, 232–233). Descriptors carrying length and purpose lets purpose-2 rejected proposals be required without seeding uncertain identity (lines 230–231). This is declaration inside the existing choice record, not a receipt layer — the resolution text (line 233) says exactly that and it is accurate.

**Forward checked-admission references** now have explicit validation order: parse the full capture set first, missing admission = replay rejection (lines 169–170). Resolved.

**RAM lifetime** now ends at acknowledged `EnvironmentHead.replace` including directory sync and cache swap (lines 80–82, 240–241). Resolved, and matches head-spec publication order (temp write → sync → rename → directory sync → cache swap).

**Canonical sorting** for checked entries (line 168) and **capture-before-ordinary ordering** (line 235: "All capture markers precede the first ordinary semantic batch") are in. **In-place same-lock staging** (lines 208–217) correctly forbids a second flock on a live segment and keeps the corrupt-header inspection path raw/bounded under the permanent lease without authority. **Fresh-ID retry only while head selection is healthy and still selects the known predecessor** (lines 219–223) composes correctly with the head spec's reread-before-replace and close-on-uncertainty rules. **u64→configured-type conversion check** (lines 237–238) resolves the capacity-type concern.

**The two corrected histories in the resolution section are right.** (1) A complete committed batch surviving an unacknowledged sync is recoverable per retained spec lines 79–83 ("Complete surviving batches can be discovered after an unacknowledged sync/write") — it is not prefix loss; the mismatch oracle must use a missing commit record or semantic invalidity inside an otherwise complete batch. (2) Reservation bodies carry counter only (`IssuerReservationEvent` in `retained_events.hpp:29-32`); namespace lives in the 112-byte header. Equal counter in a fresh namespace is the designed non-reuse mechanism (retained spec lines 244–247), not changed identity; the 128-bit ID differs because the namespace half differs. Both stand.

## Verdict

**No remaining design blockers. The protocol is ready for red/oracle work and code, contingent on the five wording amendments below** — all are rule completions, no new fields, no new mechanism, no receipt system.

## Remaining amendments (small, precise)

**A1 — Descriptor↔marker "match" is undefined (line 167).** "Match exactly one complete ProvisionalCapture marker" must be pinned as exact equality of all four fields (origin, first physical sequence, proposal length, purpose) with the marker body, plus: reassembled chunk bytes length == declared length, envelope chunk references ordered, same-journal, and each capture's chunks+marker retained contiguously without interleaving with another capture's chunks (mirroring retained spec line 186's no-interleaving rule). Without the contiguity clause, two interleaved partial captures each with a marker would pass field equality while violating the retention discipline the format inherited.

**A2 — Captures outside a continuation context are not forbidden.** "Choice in a root or later batch is invalid" (lines 181–182) constrains the choice, but nothing forbids a ProvisionalCapture marker in a root segment, or in a non-root segment *before* its choice. Add: capture markers are legal only in a non-root segment, after that segment's choice, before its first ordinary batch. Otherwise a root could carry captures that no choice declares, defeating the cross-check from the other direction.

**A3 — Cross-chain capture dedup is implicit.** Line 194–195 suppresses duplicate uncertain entries only for *authoritative* predecessor recovery. For an identity already completely captured in an intact earlier segment of the selected chain, the rules only imply dedup via "repeated different bytes conflict" (line 202). State explicitly: an exact same-bytes prior capture in the selected chain suppresses re-capture (the descriptor set must not re-declare it); changed bytes for the same identity retain an identity-conflict, never a second uncertain entry. This keeps the derived uncertain index a function of the chain alone.

**A4 — Diagnostic range bounds incomplete (lines 175–176).** Add `available_end <= actual predecessor file extent` and `diagnostic_offset >= 112` (or the first frame boundary) so the declared range cannot describe bytes outside the physical original. `predecessor.end <= available_end` alone permits an available_end beyond the file.

**A5 — Descriptor canonical ordering is referenced but never defined.** Line 236 says descriptors "use the canonical ordering defined above," but line 168 defines ordering only for checked entries (ascending attempt-ID bytes). Define descriptor order as ascending (origin journal bytes, then first physical sequence) so the encoding is deterministic for fixed-vector oracles.

## Standing caveat (unchanged from first review)

Line 190's `expected.sequence + 1 == original first physical sequence` is verified only against the cursor semantics in retained spec lines 202–204, not against the ARPROP01 layout source, which I have not read. Pin the actual cursor convention (last-committed sequence vs batch-first) with one fixed-vector oracle before green.

## Red/oracle histories (adjusted per the resolutions)

1. **Fixed vectors:** choice with 0/0 → body exactly 108; 1 attempt + 1 descriptor → 164; every truncation crossing the count fields and each descriptor boundary must reject before allocation; non-canonical entry or descriptor order must reject.
2. **Declared-set cross-check (the Blocker-2 oracle):** choice declares {J1, seq 7, len L, purpose 1}; candidate has chunks but marker torn after last commit boundary; head selects candidate → open refuses normal work, originals inspectable, identity neither ordinary nor uncertain. Variants: extra undeclared marker → reject; descriptor length ≠ marker length → reject; marker in root or preceding the choice → reject (A2).
3. **Prefix-mismatch (corrected):** child header predecessor end E where J1's validated prefix ends at E′ < E because the batch at (E′, E] lacks its commit record or contains a semantically invalid event inside complete framing → reject, no older-branch fallback, pending bytes inspectable only. (Not: fully valid committed surviving batch — that recovers.)
4. **Namespace (corrected):** committed reservation counter 5 under namespace N1; continuation draws N2 → reservation counter 5 commits and returns an ID whose namespace half is N2; assert the full 128-bit IDs differ and no conflict is retained; then changed reuse of the *full* old ID → conflict.
5. **Retry-while-healthy:** injected failure at candidate file sync → RAM proposals retained, orphan file inert, fresh entropy candidate succeeds while head still selects predecessor; injected failure after rename → no second branch, reopen establishes actual selection, staging requires custody reconciliation before any work.
6. **No old redispatch:** admitted+open attempt A, SIGKILL at adapter entry; continuation with CustodyVerifier success for A recorded as checked entry (opened=1, observed phase, prior evidence); reopen; resubmission returns Existing/Uncertain; fake effect count stays ≤1; the choice grants no dispatch permission by decoding alone.
7. **Entropy script:** zero ID, ID equal to a predecessor header journal, namespace equal to any intact predecessor namespace, 32 collisions → Conflict with no fallback; getentropy error → actual error; each refusal leaves no issued identity and at most the exclusively-created evidence file.

**Efficiency/ownership assessment:** one permanent lease, borrowed directory facades without lock authority, single accumulated snapshot with old descriptors kept only for source routing, preflight allocation before publishable I/O, nonallocating final swap, in-place rescan under the held descriptor — all consistent with the reviewed primitives and with no duplicated persistence or receipt machinery. The descriptor set is the minimal declaration that makes the capture cross-check computable after RAM loss; nothing smaller carries the information.