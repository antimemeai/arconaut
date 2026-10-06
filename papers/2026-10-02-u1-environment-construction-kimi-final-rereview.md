# Final focused rereview — round 3

Scope inspected: the new mixed-proposal case in `tests/retained_environment_test.cpp:160-180` and the new subplan clause at `docs/U1_CONTINUATION_SUBPLAN.md:117-127`. No production code changed; F1-F4 dispositions stand (resolved/withdrawn). Read-only; no execution.

## Mixed committed-existing + new case — verified correct, oracle is sound

Hand-checking the test against the implementation:

- **Setup state:** after the first `[event,event]` proposal, cursor is seq3/end326 with one semantic fact (complaint20 at seq1); the all-existing query leaves this untouched. Each complaint frame is 79 bytes (32 header + 47 payload), commit 56 — consistent with 112 + 2·79 + 56 = 326.
- **Staging:** `first = 3 + 0 + 1 = 4`. `events[0]` (complaint20) matches committed → `existing=true` with the *original* record seq1 (first-match semantics, subplan line 103 of the accepted root rule). `events[1]` (complaint21) is new, ordinal 1 → predicted record seq5. Whole `existing=false` because not all events match (`src/retained_environment.cpp:355-356`) — correctly distinguishing per-event flags from the whole-proposal no-write claim.
- **Append:** not short-circuited; `append_impl` writes seq4 (duplicate complaint20 frame), seq5 (complaint21), commit seq6 → end 326 + 79 + 79 + 56 = 540. Test expects cursor 6/540, frames at offsets 326/405 with sequences 4/5, `batch_first=4`, and exact decoded bodies. All arithmetic and ordinal predictions check out.
- **Semantic outcome:** exactly one new fact (total 2); the physical duplicate seq4 frame is retained and directly inspected via `read_original_range` + `decode_journal_frame`, which is the correct oracle for "never filter existing entries from a mixed proposal — that would renumber retained originals" (subplan :126-127). This closes the round-2 gap #2 with semantic, positional, and byte-exact assertions rather than a round-trip.

## Subplan clause vs. code — consistent

The pinned clause (:117-127: pure dedup query available in any physical state while head healthy, no continuation latched, no transaction underway; never confirms recovery or permits dispatch; stale cursor still rejected) matches the code exactly: gates are `busy_` (`:311`), head health + `continuation_required_` (`:322-324`), and the short-circuit (`:351-359`) precedes the write-readiness state check (`:360`) and any encoding. It performs no mutation, so it cannot confirm recovery or grant dispatch; its typed evidence (`recovered_pending`) is preserved. This pins the breadth question I raised in round 2 item 1 — resolved as designed, and the documented claim now covers the code's actual behavior.

## Remaining items

No material defects found in these changes. Carried minor oracle gaps only:

1. Three-segment sufficient-budget fresh `AttemptOpen` dispatching exactly once with retained adapter receipt (still exercised only root-only; the chain tests the budget-10 refusal).
2. Direct assertion that the two counter1 reservations in namespaces 3/13 are retained as distinct facts with different full IDs (currently aggregate-count only; ns15/counter1 issuance is pinned separately).

Both are oracle-strength items on already-reviewed behavior, not defects. Unchanged scope statement: live continuation/restage/entropy, capture/checked-state validation, raw bad-header helper, emergency controls, and Linux qualification remain unfinished owning-unit obligations; this rereview accepts nothing beyond the construction paths reviewed.
