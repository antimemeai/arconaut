# Rereview: revised docs/U1_CAPTURE_SUBPLAN.md

Same scope as the prior review (revision at lines 4, 56-59, 75-77, 85-99, 101-110, 192-193; codecs and `src/retained_state.cpp` / `src/retained_environment.cpp` re-checked where relevant). Read-only; no execution.

## Prior findings — resolution assessment

### F1 (maintenance-region semantic records) — Resolved
Lines 56-59 now state: every semantic record in sequences 2..maintenance_end must be a validated capture marker, and any ordinary semantic record anywhere in that region — including between choice and the first group, or between complete groups — refuses activation. Combined with the unchanged clauses (unmatched sources in the region reject, line 55-56; extra/later choice or marker refuses, line 48; all markers precede the first ordinary semantic record, line 47), the region `(choice.seq, maintenance_end]` is now fully pinned before the `sequence <= maintenance_end` skip in shared replay (`src/retained_state.cpp:381-384`) can silently drop anything. The empty-range edge (no captures ⇒ maintenance_end = 1 ⇒ 2..1 vacuous) is coherent. No remaining gap.

### F3 (changed bytes vs existing uncertain identity) — Resolved
Lines 88-90 now explicitly reject the required capture and the selected segment on changed bytes/dependencies, retaining the first original record only for exact duplicates. This matches the accepted continuation rule ("an inconsistent duplicate capture discovered in replay rejects that batch/suffix", U1_CONTINUATION line 754-755). No ambiguity remains.

### F2 (later authoritative duplicate of an uncertain event) — Resolved as pinned; promotion recommendation withdrawn
The pin (lines 101-110): an ordinary semantic frame matching a still-uncertain event rejects its batch Conflict even when the body is equal; active suffix keeps its preceding prefix with work closed, historical selection including that batch refuses the chain; the uncertain entry and packet stay unchanged; U1 has no promotion operation.

I independently checked this choice against the whole contract and find it **consistent, not merely safe**:

1. **Unreachable by legitimate API use.** After a capture seeds an uncertain identity, the specified submission API cannot produce an ordinary committed frame for that event: exact individual resubmission returns Existing/Uncertain with zero writes (line 129-130; U1_CONTINUATION lines 87-89), and any mixed bulk input containing an uncertain identity refuses Conflict without admitting either part (lines 130-133; U1_CONTINUATION lines 91-99). So a legitimately generated selected chain can never contain the frame being rejected — the Conflict branch is reachable only from foreign/corrupt bytes or an API violation, which is exactly what the historical-boundary refusal exists for.
2. **The "good" case is already handled elsewhere.** If the original append was in fact committed in the predecessor before continuation, capture parsing sees it as an authoritative prior fact and suppresses the uncertain entry entirely (line 86-87) — so the F2 scenario cannot arise from an acknowledged-then-continued history either.
3. **Failure semantics match existing machinery.** Active-prefix-retained/work-closed versus historical-chain-refusal is precisely the current `reject_recovery_batch` behavior (`src/retained_state.cpp:409-415`) and the accepted "semantically bad batch invalidates atomically" rule.
4. Rejecting rather than promoting preserves the invariant that only fresh ordinary admission grants authority, and avoids the count/evidence divergence my original wording risked. My earlier suggestion to define a promotion/retirement path is withdrawn; the document's explicit statement (lines 109-110) that any future resolution operation needs its own contract is the correct boundary.

Oracle 4 (lines 192-193) now exercises both sides of this pin. Resolved.

## New clauses — assessment

- **Purpose1 maintenance-event and record-policy restriction (lines 75-77):** Consistent. Choice/capture bodies decode successfully at the codec layer (`src/retained_events.cpp` cases 12/13) but `RetainedState::apply` rejects them (`src/retained_state.cpp:202-205`), so no legitimately captured purpose1 packet could contain one — such a submission would have been retained as purpose2 diagnostic instead. Excluding/rejecting them in purpose1 therefore refuses no legitimate history. Requiring prospective count fit against the immediate predecessor's *declared* record policy (in addition to payload/batch framing fit, line 73) mirrors `append_impl`'s preflight and is bounded by already-validated declarations, not untrusted wire values.
- **Non-keyed suppression only at the exact predicted record (lines 90-92):** Correct and necessary. Non-keyed events (nonterminal observations) have no identity, so identity-style suppression would be unsound; positional comparison (predicted sequence = first_sequence + source_count + ordinal, in the origin journal, checked addition) is the only evidence that the original physical record was actually committed. Treating an equal observation at a different position as a separate event preserves the accepted "repeated nonterminal observations are separately received facts" rule (U1_RETAINED_STATE line 229-230).
- **Reservation query origin vs current namespace (lines 97-99):** Consistent with namespace-keyed reservation identity (`src/retained_state.cpp:31-42`) and the accepted rule that a same-counter reservation in a fresh namespace is a distinct identity, not a conflict (U1_CONTINUATION lines 729-733). An individually resubmitted old-namespace reservation is correctly treated under the *current* namespace's ordinary rules; offering provenance for the old packet only through full-packet equality (lines 114-121) avoids both counter burn and cross-namespace suppression. No authority leak: the full-packet path returns provenance/cursor only, with zero writes.

## Literals
Unchanged from the reviewed revision; the previously verified literals (packet 214 = 48+4+3+4+155; choice payload 152; marker payload 68; end 352/754; counts 4; budgets 3/213/214) still match the codecs and frame geometry.

## Result
All three prior findings are resolved, including withdrawal of my F2 promotion recommendation in favor of the document's rejection pin. The three new clauses check out against the contract and codecs. **No remaining or new material findings in this revision.**

Limitations: read-only review, no build/execution; assessment is of the plan text against the existing machinery and accepted continuation/capacity/retained-state contracts, not of an implementation, which does not yet exist.
