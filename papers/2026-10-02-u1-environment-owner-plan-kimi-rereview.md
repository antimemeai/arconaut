# Rereview: revised Environment owner refinement + three-segment oracle (docs/U1_CONTINUATION_SUBPLAN.md:40–263)

Verified the revised text against the actual codec (`src/retained_events.cpp`), `JournalHeader`/`JournalPredecessor` fields, and the retained-state machinery. All four prior findings are addressed; the oracle tuples check out exactly; one new genuine gap and two editorial notes follow.

## Prior findings — resolution check

**F1 (lease window) — resolved.** Lines 109–121 now factor preparation as allocate + `create_exclusive("authority")` + lock + head-absence check + write/sync the exact root declaration, with no selector publication; root is created/synced under that held lease; a second creator "fails at authority acquisition before touching any root journal." This matches the existing primitives (src/environment_head.cpp:195–216) split at the right boundary, public `create` composes the phases, and open's existing `read_selector` ENOENT failure (src/environment_head.cpp:137–140) gives the promised concrete partial-creation refusal. No contradiction remains.

**F2 (capture-chunk classification) — resolved.** Lines 160–174 name the operand: a temporary per-segment `capture_only` reference set derived from validated ProvisionalCapture marker dependencies and their contiguous source groups, bounded by configured physical records, fully validated before ordinary batches, filtered before `ordinary_sources` construction and dependency checks, with multi-batch source-only groups covered and persisted marker facts retaining the reference set for later classification (bounded by `max_history_entries`). The carve-out for existing acknowledged root RejectedSubmission/IdentityConflict diagnostic sources (lines 169–171) preserves the accepted root contract — those bytes stay ordinary sources and referencing them admits nothing — so there is no silent semantic change. The two rejection paths are now coherent: RetainedEnvironment's own rejections use the ordinary RejectedSubmission diagnostic path (lines 75–80); purpose2 ProvisionalCapture covers continuation-declared captures, and "only purpose1 creates uncertain identities" (line 192) correctly scopes the exact-captured-proposal/existing-Uncertain rule to purpose1.

**F3 (namespace state) — resolved.** Lines 134–139 add Snapshot `active issuer_namespace (u64) alongside counter (u64)` and an explicit pending `pair(namespace,counter)` initialized from the selected active header, with reset/namespace update on entering each next selected unique namespace before applying that segment's reservations. This is exactly what `apply()`'s monotonic check (src/retained_state.cpp:191–194) and the pending-burn scan (src/retained_state.cpp:378) need.

**F4 (refusal class) — resolved.** Lines 73–77: mixed exact-uncertain/new proposals are refused Conflict, retained via the ordinary diagnostic path with reason Conflict subject to real audit/capacity failure, explicitly not a changed-identity claim when event bytes are equal. Lines 64–66 pin predicted records as descriptor-derived Uncertain provenance with no source/parent/effect authority. Lines 205–208 qualify raw-range inspection as state-nonmutating under the existing lease/descriptor, never validating partially scanned frames or reacquiring the lock — consistent with the physical contract's Busy rule for mutating payload validation. The new `continuation_required` latch (lines 90–97) closes the internal-Recovered/reconcile bypass and states the resulting state mapping (Poisoned vs Blocked) and clearing conditions; it coexists correctly with "healthy known-head failure permits explicit fresh-candidate retry" since the latch gates ordinary work, not `continue_journal`.

## Oracle verification (lines 228–255)

All byte counts derive from the actual codec and check out exactly:

- Envelope = schema2+kind2+depcount4 = 8; each dependency 24 (src/retained_events.cpp:375–381). Decision = 8+24+6×16+4+16+(4+3) = **155** ✓ (encode at :179–191).
- Root: 112 + (32+3) + (32+155) + 56 = **390, seq3** ✓; +reservation (payload 16, frame 48) + 56 = **494, seq5** ✓.
- RecoveryChoice payload (empty checked/captures, no problem): 8+16+8+8+8+8+8+16+16+12+4+4 = 116 → frame 148 → 112+148+56 = **316, seq2** ✓ for both children (encode at :248–277; dependencies empty satisfies the :364–365 constraint).
- Invocation = 8+24+16×3+(4+3) = 87 → 316+35+119+56 = **526, seq5** ✓; reservation → **630, seq7** ✓; AttemptAdmission = same 87 → second child **526, seq5** ✓.
- Snapshot entry counts per the line-140 formula (choice facts count in `facts`; commits are not entries): 2, 3, 4, 6, 7, 8, **10** ✓; AttemptOpen as entry 11 at budget 10 refuses pre-dispatch, matching the submit-open-before-`boundary.dispatch` order in `dispatch` (src/retained_state.cpp:770–778) ✓.
- Capacity ceilings consistent: root 5 physical records/494 bytes ≤ 8/4096; children 7/630 ≤ 16/8192 and 5/526 ≤ 32/16384; all payloads ≤ 1024, batches ≤ 4096 ✓.
- The two counter1 reservations exercise exactly the F3 semantics: reset on entering the fresh namespace admits the second counter1; namespace-qualified keys keep them distinct facts ✓.

## New finding (minor, genuine): issuer-namespace uniqueness is assumed but never validated

Lines 138 and 177–179 reset the counter when "entering a new unique namespace" and rely on namespace-qualified reservation keys so "old reservations… cannot advance or suppress new reservations." But no stated check enforces that a candidate/selected header's `issuer_namespace` differs from all selected ancestors. Header validation (lines 126–129) checks declaration/namespace/filename/environment/framing and links; entropy checking (line 219) covers the journal **ID**, not the namespace.

- Trigger: a continuation candidate created (via the injected-entropy path with a programmatically chosen or mistakenly reused namespace) whose `issuer_namespace` equals an ancestor's.
- Consequence: either the reset fires and a fresh reservation `(N,1)` exactly matches the ancestor's reservation fact — dedup treats it as Existing and suppresses the new reservation, violating the stated invariant — or the reset is skipped for a non-unique namespace and the carried-forward `snapshot.counter` makes the new reservation conflict, also suppressing it. The oracle only covers the distinct-namespace case, so this ambiguity would ship untested.
- Minimal correction: one sentence in the open/continuation validation requiring pairwise-distinct issuer namespaces across the selected chain (refuse the candidate/chain on collision), or explicitly define same-namespace continuation as counter continuation without reset and reconcile it with the "cannot suppress" invariant.

## Editorial notes (not blocking)

- Line 153–154's "original proposal sources cannot satisfy ordinary dependencies" reads categorically, while lines 169–174 scope the capture-only rule to ProvisionalCapture groups and keep root RejectedSubmission chunks ordinary. Reading line 154 as "ProvisionalCapture original proposal sources" would remove the only remaining surface tension; the carve-out paragraph makes the intent clear enough that this is wording, not contradiction.
- The latch's state names (lines 94–95) are environment-level mappings; worth one clause noting they compose with, not replace, the internal `JournalWriterState` so implementers don't add a parallel state enum that can disagree with the physical one.

## Verdict

All four prior findings are verifiably resolved in the current text; the oracle's tuples, counts, and boundary-refusal arithmetic are exact against the real codec and contract constants. The latch addition is coherent with retry/reopen semantics. One new small gap (namespace-uniqueness validation) and two editorial clarifications remain; nothing else contradicts the existing accepted machinery. The section is otherwise ready to govern implementation, still without accepting the owner/U1 as a whole.
