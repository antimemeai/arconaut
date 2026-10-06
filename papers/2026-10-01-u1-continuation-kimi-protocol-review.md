# Independent review: `docs/U1_CONTINUATION_SUBPLAN.md` chain invariant and wire rules

Scope inspected: the subplan (all 200 lines), `docs/U1_ENVIRONMENT_HEAD_SUBPLAN.md`, `docs/U1_RETAINED_STATE_SUBPLAN.md`, `include/arconaut/journal.hpp`, `include/arconaut/retained_events.hpp`, `include/arconaut/journal_writer.hpp`. I did not read papers/history and did not touch code. The design's spine (permanent lease before inspection, selected-prefix-only replay, no orphan authority, pre-I/O allocation, nonallocating publication swap, staged recovery before custody reconciliation, no synthetic terminal disposition) is consistent with both implemented specs. Findings below.

## Blocker 1 — RecoveryChoice fixed-size arithmetic is wrong (line 163)

"Fixed body is 100 bytes plus 20 per checked attempt." Summing the declared fields:

- predecessor journal16 + commit-sequence u64 + end u64 = 32
- diagnostic offset u64 + available end u64 = 16 (running 48)
- old max_payload u32 + max_batch u32 = 8 (56)
- old max_file u64 + max_records u64 = 16 (72)
- new max_file u64 + max_records u64 = 16 (88)
- error-present u16 + error-code u16 + detail i64 = 12 (100)
- **checked-attempt count u32 = 4 → 104**

The count field itself is omitted from the claimed fixed size. **Fixed body is 104 bytes + 20/attempt.** A decoder written to "100" accepts a 4-byte-short body and misreads the count as entry data. Per-entry arithmetic (16+1+1+1+1=20) is correct. Fix the number and pin it with a fixed-vector red test (finding 4 below).

## Blocker 2 — The choice does not declare its required provisional captures, so "missing capture marker" is undetectable on selected replay

Lines 198–200 state the right requirement ("A selected candidate lacking its required capture marker must not open for normal work… cross-checked… not inferred from chunk existence") but the RecoveryChoice body (lines 154–162) contains **no field naming which captures are required**. The required set is determined at continuation time from RAM proposals (the uncertain set not authoritatively recovered). On reopen after crash that RAM is gone by definition (line 84: "never claim later recovery knows bytes that neither storage nor surviving memory contains"). Therefore replay cannot recompute the required set from the predecessor chain — the information exists nowhere on disk except the candidate itself. Without a declared list, "required capture missing" is indistinguishable from "no capture was required," and a candidate whose capture batches were torn away after its choice batch committed would open with silently lost uncertain identities — exactly the "no lost original RAM sources" invariant the subplan sets out to protect. This is the gap the task brief anticipated.

**Smallest correction (no new receipt system, reuses existing primitives):** add to the RecoveryChoice body, after the checked-attempt entries:

- `required-capture count u32`, then per entry `origin journal16 + original first physical sequence u64` (24 bytes each).

Rules: count bounded by remaining body length before allocation (same rule as checked attempts); entries unique and canonically ordered (by journal bytes, then sequence) for deterministic encoding; each entry must match exactly one ProvisionalCapture marker (kind 13, purpose=1) inside the segment's validated prefix, with equal origin/sequence and a marker whose reassembled ARPROP01 proposal carries the unresolved identity; replay rejects missing, extra, or mismatched markers. New fixed body becomes **108 bytes + 20·attempts + 24·captures**. Keep the choice's dependency envelope empty (these are body fields, not source dependencies, so the "no source dependencies" rule is untouched). This is one count plus a 24-byte tuple list — the minimal authoritative declaration, and it makes the cross-check in line 199 actually implementable. It also fixes a second latent hole: without a declared set, nothing bounds *extra* captures either.

## Non-blocking defects and amendments

3. **RAM proposal lifetime endpoint is ambiguous (lines 81–82).** "Clear this temporary RAM proposal only after clean refusal or successful publication." "Publication" must mean acknowledged `EnvironmentHead.replace` completion (rename + directory sync + cache swap per head spec lines 56–60), not merely candidate-journal sync. Otherwise a crash after candidate sync but before head replace leaves the uncertain bytes only in an orphan file that confers no authority (line 128) while RAM was already cleared — a lost original. One-word amendment: "successful head publication acknowledged by EnvironmentHead.replace."

4. **Ordering rule missing between captures and ordinary batches in one segment.** Line 125 ("Any uncertain publication closes normal work") plus the reopen rules imply a segment carrying provisional captures never admits ordinary events afterward, but no replay rule enforces capture-before-ordinary ordering. Add: within a non-root segment, all ProvisionalCapture markers precede its first ordinary semantic batch; a segment violating this is corrupt. Otherwise "uncertain originals do not satisfy ordinary dependencies" (line 91) depends on unwritten ordering discipline.

5. **Canonical ordering unspecified for checked-attempt entries.** "Entries are unique" (line 164) but unordered, so two encoders can produce different bytes for the same reconciliation set, weakening direct byte-vector oracles. Require ascending attempt-ID byte order (same determinism the head spec demands for its "exactly one legal encoding").

6. **`old max_records`/`max_file` types.** Wire u64 vs `std::size_t max_records` in `journal_writer.hpp:10` — fine, but the encoder must reject values unrepresentable in the owner type on 32-bit, and replay's "new capacities must equal the candidate owner" (line 172) must compare against the *configured* owner profile, not re-derive limits from the numbers it is checking.

7. **Checked entries referencing provisionally captured admissions.** Line 164 permits a checked attempt to "refer to … provisionally captured admissions," but the choice is sequence 1 in the first batch, ahead of those captures. Validating attempt IDs therefore requires the full declared-capture set (Blocker 2 fix) replayed first; state that validation order explicitly so a dangling reference is a replay rejection, not an unresolved forward reference.

## What checks out (no change needed)

- Head-seam contract: candidate/header/limits validation and complete choice+capture+sync before `EnvironmentHead.replace` matches head spec lines 42–45 exactly; unacknowledged rename → reopen reads actual head, orphan candidate confers no authority, no rollback (lines 62–66).
- Namespace/counter: current-namespace reservation indexing (lines 107–109) matches retained-state spec lines 244–249; cross-environment collision declared probabilistic.
- Prefix truthfulness: child header predecessor fields vs selected committed prefix (lines 42–48) matches header layout (`journal.hpp` JournalPredecessor) and "no scanning past damaged material" (retained spec line 73).
- No fabricated disposition: RecoveryChoice records observed phase/disposition and prior-evidence class only; line 166–167 explicitly defers actual resource state to U3's CustodyVerifier, consistent with retained spec lines 93, 211, 234–236. Empty checked set legal — good.
- Rejected-proposal (purpose=2) exemption from cursor equality (lines 191–195) correctly prevents stale-cursor caller input from becoming provisional ordinary operations.
- Entropy rules (lines 98–105): 32-candidate bound, Conflict on exhaustion, no clock/PID fallback, exclusive-creation orphan refusal — all grounded in the reviewed head primitive.

## Direct spec-derived red histories

1. **Wire length vector:** choice with 0 attempts, 0 captures → encoded body exactly 108 bytes (104 fixed + count); with 1 attempt → 128; feed every truncation 100..127 to the decoder expecting rejection of misaligned counts. Catches Blocker 1.
2. **Missing declared capture:** predecessor J1 commits a batch; sync result unknown; RAM proposal P (expected cursor seq 6, first frame seq 7) retained. Continuation writes choice declaring capture {J1,7}, chunk batches, then injected failure before the marker; crash; force head to select candidate (scripted rename). Reopen must refuse normal work with declared-set-vs-prefix mismatch, expose originals via `read_original_range`, and must not admit P as ordinary or uncertain. Without the Blocker-2 field this history is inexpressible — that is the red test proving the correction is needed.
3. **RAM lifetime:** candidate fully synced, injected rename failure at head replace. Assert RAM proposal P still retained (line 82), old head still authoritative, candidate file orphan with no admission; then clean retry publication succeeds; only then P cleared. Duplicate resubmission of P's identity afterward returns Existing/Uncertain with zero writes and zero fake-adapter effects.
4. **Prefix mismatch / no old-branch selection:** child header predecessor end = E, but J1's validated committed prefix is E′ < E with complete pending batch at (E′, E] → open rejects; bytes in (E′, E] remain inspectable, never replayed; no fallback to an older intact branch.
5. **Namespace:** historical reservation (N1, counter 5) committed; continuation issues namespace N2 → reservation (N2,5) must commit and return a distinct identity; resubmission of (N1,5) bytes → conflict; scripted entropy returning a predecessor namespace or zero ID → Conflict with no exclusive file created beyond the refused attempt; 32 collisions → Conflict; getentropy error → actual Io, no fallback bytes.
6. **No old redispatch across crash:** admit+open attempt A in J1, SIGKILL after adapter-entry before effect file write (retained spec lines 303–310); continuation with CustodyVerifier scripted to succeed for A; reopen; resubmit A → Existing/Uncertain; assert effect-file record count stays ≤1 and dispatch permission is never regranted by recovery, choice replay, or capture replay.
7. **SIGKILL across final head publication:** kill after rename, before directory sync. Reopen must select the new identity (head spec lines 66–68), stage the candidate chain, keep admission closed until custody reconciliation, and never manufacture a root/default selection.

## Limitations

I did not read the ARPROP01 layout source (referenced as existing), the CustodyVerifier interface, FramedJournal implementation bodies, or the papers; the ProvisionalCapture cursor arithmetic ("expected.sequence+1 = original first sequence") is verified only against the cursor semantics stated in the retained spec (line 202–204), not the code. If ARPROP01's expected cursor denotes batch-first rather than last-committed sequence, the +1 rule needs restating — worth one oracle pinning the actual convention.