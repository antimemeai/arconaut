# Confirmation review: `docs/U1_CONTINUATION_SUBPLAN.md` (final rule completions)

Verified the new "Second review rule completions" section (lines 254–289) against each amendment, and re-checked the amended text for interactions with the rest of the protocol and the two implemented specs.

## Each amendment landed correctly

**A1 — descriptor/marker match (lines 256–260).** Exact four-field equality (origin/first-sequence/length/purpose); marker references ordered by increasing physical sequence, same-candidate-journal only, concatenating to the exact declared length; chunks+marker contiguous except interleaved commit frames; captures cannot interleave with each other. This is exactly the inherited retention discipline of retained-spec line 186, restated for the candidate. Correct — and "except their commit frames" is the right carve-out since each source group is its own batch.

**A2 — capture legality window (lines 260–261).** Markers legal only in a non-root segment, after its unique choice, before any ordinary semantic batch. Combined with line 262 ("required set must equal the complete marker set, including rejected proposals"), the cross-check is now bidirectional: declared-but-missing and present-but-undeclared both reject. Closes the root-capture and pre-choice-capture holes.

**A3 — ancestor capture dedup (lines 270–274).** Exact same-bytes capture in an intact ancestor is inherited, not re-declared/re-captured; identity dedup spans committed and uncertain indexes; inconsistent duplicate discovered in replay rejects the batch/suffix rather than writing an unsolicited repair. Consistent with the no-scan-past-damage rule and with descriptors referring only to the immediate predecessor (line 166) — older-origin uncertainties are already represented by ancestor captures, so the origin scope is sound.

**A4 — physical range bounds (lines 276–278).** `available_end <= observed predecessor extent`, `diagnostic_offset >= 112`, and the damaged-header raw path cannot manufacture a valid choice. Correct; ties the declared range to the actual physical original. (Cosmetic: "diagnostic\n_offset" is split across the line wrap at 276–277.)

**A5 — canonical ordering (lines 264–268).** Descriptors sort by unsigned origin bytes, then first sequence, then purpose; checked entries by unsigned attempt-ID bytes; decoders reject noncanonical order/duplicates; encoders refuse rather than silently reorder. Deterministic fixed vectors are now pinnable. The purpose tertiary key is harmless (same origin+sequence with different purpose is already a conflict).

**Outer ARPROP01 ownership (lines 283–289).** The whole outer rejected proposal is prepared as owned RAM before the first source-group append; inner chunk appends neither replace it nor seed uncertain ordinary identities; it survives unknown group/marker/head outcomes; purpose-2 capture tolerates stale/foreign expected cursors. This closes the last "lost original RAM source" path: the diagnostic retention failure can no longer leave only an inner-group append while the caller's full proposal evaporates.

## Interaction re-check (no new contradictions found)

- Marker envelope references point to earlier committed source frames in the same candidate segment — legal under retained-spec dependency rules (lines 68–71: earlier authoritative source or same-batch source), and consistent with "never send an older source record to its newest file" (the chunks are new records; only bytes are copied).
- Staged-before-publication and forward-reference resolution order (lines 279–281) align with the retained spec's stage-then-sync-then-publish recovery discipline.
- The required-set equality rule plus the legality window plus ancestor inheritance together make the uncertain index a pure function of the selected chain — recomputable after RAM loss, which was the original Blocker 2 motivation.
- Empty-predecessor edge (prefix zero/112, line 43) remains consistent: no committed or uncertain events can originate in an empty predecessor, so the descriptor set is empty and the empty checked set is legal (line 179).

## Verdict

**Ready for red/oracle work and code. No remaining semantic design blockers, and no new format, library, receipt layer, or authorization gate is needed.** The RecoveryChoice/ProvisionalCapture pair with the declared descriptor set is the minimal on-disk declaration that makes missing-capture detection computable post-crash; everything else reuses the existing envelope, ARPROP01, CRC32C framing, and the reviewed head primitive.

Two items to carry into execution (not blockers):

1. **Pin the ARPROP01 cursor convention.** Line 190's `expected.sequence + 1 == original first physical sequence` is verified against retained-spec cursor semantics (lines 202–204) but not against the ARPROP01 layout source, which remains unread in this review scope. One fixed-vector oracle on an empty and a nonempty predecessor journal will settle whether the expected cursor denotes last-committed sequence or batch-first sequence; restate the rule if the latter.
2. **Owner implementation pending.** `RetainedEnvironment` (lease ownership, segment vector, accumulated snapshot, borrowed directory facades, in-place rescan, preflight/nonallocating publication) is specified only at design level here; the red cases should be written against this document's rules before the owner exists, per the established red-first discipline, including the seven histories from the previous round updated for the new legality window and ancestor-inheritance rules (marker in root, marker before choice, interleaved captures, re-declared ancestor capture, noncanonical descriptor order, available_end beyond extent, diagnostic_offset below 112 — each must reject).