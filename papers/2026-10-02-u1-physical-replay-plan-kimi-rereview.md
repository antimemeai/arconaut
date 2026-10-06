## Re-review: Physical replay implementation refinement (docs/U1_CONTINUATION_SUBPLAN.md lines 44–111)

I re-read the revised section against journal_writer.hpp, journal_writer.cpp (scan/append/confirm_recovery/resume_after_reconciliation/read_payload/read_original_range), and retained_state.cpp (append_impl/source/rebuild/confirm_recovery). Verifying each prior finding against the new text:

**B1 — resolved.** Line 68 pins cached wireCRC participation in `PhysicalJournalRecord` equality/containment (so `read_payload`'s full-record containment check at journal_writer.cpp:454–457 has defined semantics); lines 69–71 pin append populating from the just-encoded frame header pre-write (closing the never-scanned acknowledged-tail gap at journal_writer.cpp:250–253/289–291) and scan populating from the validated frame header; lines 71–74 pin RetainedState replacing the pre-append placeholder source records (retained_state.cpp:458–461) with the actual appended tail of `physical_records()` before `committed_.swap` (retained_state.cpp:522), nonallocating — feasible since `prepared_->sources` is already sized/reserved and the struct stays trivially assignable. Line 74's "failed prepared sources remain uncertain" matches the existing evidence handling. No residual placeholder path remains.

**B2 — resolved.** Line 49 pins the legal restage states (live/recovery-pending/recovered/blocked/poisoned, none granting writes while staging); lines 93–95 pin flag set in the successful selection call, survival across restage/confirmation, and the `resume_after_reconciliation` check; line 94–95 pins confirmation leaving flagged journals Blocked/inspection-only even with a clean report (an explicit, now-pinned change to the unconditional state set at journal_writer.cpp:440–443); lines 95–97 pin that restaging a flagged journal never clears the flag and the environment must reapply the exact selected boundary before semantic publication.

**Other pins — verified consistent.**
- Precedence (lines 78–83): acknowledged (sequence,end_offset) comparison runs even when scan stopped at damage, Corrupt ahead of the suffix diagnostic, empty-acknowledged + damaged first submission accepts 0/112, hard scan/alloc error leaves staging closed. Matches scan()'s report-and-succeed structure (journal_writer.cpp:311–397) and makes the override explicit.
- Cursor reset to 0/112 and fresh `available_end` from fresh extent before scan (lines 47–48) names the field, closing the stale-bound hazard (journal_writer.cpp:145, 315).
- Interrupted budget (8 consecutive, progress reset) mirrors the existing read_original_range discipline (journal_writer.cpp:493–497, 508–526); extent retries bounded likewise. Consistent.
- Reentrant restage/mutation → Busy matches the `in_transaction_` guard pattern (retained_state.cpp:59–68, 399–401).
- Line 111 (noncommit records consume max_records; commits consume sequence only) matches scan()'s capacity check placement (journal_writer.cpp:372) and append's `drafts.size()` accounting (journal_writer.cpp:214).
- Direct-oracle list (lines 99–111) now includes the five suggested histories: freshly appended never-scanned acknowledged sources, semantic-owner source reads after metadata publication, empty-vs-nonempty acknowledged damage, restage after prior semantic batch rejection, plus the never-resumes and CRC-rewrite cases.

### Remaining issues

None blocking. Non-blocking nits only:

1. Typos: "to0/112" (line 47), "from112" (line 48), "Accept0/112" (line 86) — missing spaces.
2. Worth one clause when the next layer lands: restage from `recovered` must also discard any rebuilt semantic `prepared_` snapshot (retained_state.cpp:307–309) so a stale snapshot cannot be confirmed against restaged indexes. This is next-layer semantic-owner territory and the physical Busy pin already blocks mid-restage confirmation, so not a defect here — just don't lose it at integration.

### Disposition

**Ready for direct red cases and code.** The section now pins the CRC metadata lifetime/population/equality seam, the no-resume state-machine interaction, comparison precedence, boundary operands, I/O retry budgets, and reentrancy closure at the granularity the existing code enforces. No acceptance of unimplemented selected-chain integration is implied or claimed, and the CRC32C fault-domain limitation is restated in the text.
