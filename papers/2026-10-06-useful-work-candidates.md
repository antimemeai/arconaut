# Useful-work candidate/Lua unit (.14.1), 2026-10-06

Source checkpoint follows reconstruction/cpp-lua-2026-10-06 at 4589dd5;
B1 and common observability remain closed. Native tooling is C++ (`arco-candidate`),
model-governed workflow/helpers are Lua. No new dependencies/services. No primary
checkout switching. Source checkpoint is NOT actual activation evidence; live
post-RRC qualification/activation and bounded pilot remain next.

## Delivered source and direct outcomes

Bounded configurable pool (one serial slot default, pilot limit4 explicitly
configurable), FIFO overflow, Git branch/commit retention; per-slot active-run
locks and global state serialization. Other leased slots proceed concurrently.
Intent/state durable before effects, actual leader results/output retained,
timeout/failure unknown and no replay. ALL runs need observed stopped settlement
because leader exit cannot prove escaped/standing programs stopped. Dirty,
staged/unrecorded and alias/foreign checkouts refuse unsafe switching. Explicit
named checkpoints, artifact copies outside checkout, serial reuse and retirement
preserve source/results. Activation is separately linked to actual qualified
program/request evidence by caller, never inferred from a commit. Caller must
manage all programs through leases or explicitly inspect unmanaged users; this is
an ownership protocol, not magic process detection or command approval.

Direct native tests: initial missing executable red; clean/pool-full/dirty/busy/
unknown/no-checkpoint refusal; same-path serial reuse; source branch and copied
artifact survive retirement; primary unrelated changes preserved. Real slot
symlink red switched a DISPOSABLE test primary, fixed by symlink refusal + owned
linked-worktree membership. Real failed Git dispatch against a colliding branch
then explicit clean-prior-checkout abandonment succeeds without effect replay;
partial leftover directory is quarantined durably, bytes retained. Actual two-slot
runs proceed concurrently; excess request remains queued, no third slot allocated.
Actual timeout once-only marker remains once; release refuses until settlement.
Configured cap5/limit5 demonstrates limit4 is not an upstream concurrency claim.

Lua `useful_work.lua`: positional pinned audit-page/original helpers, explicit
JsonNumber conversion, bounded predicate search with unknown continuation, actual
serialized context/request/available-usage feedback and top3 contributor locators.
Narrow question/action/outcome and contrast records with references, baseline,
candidate, independently predeclared discriminator, measurement/disposition.
No forced compaction or utility judge. `useful_turn.lua` reads persistent local
operator steer at the affected boundary; helper and steer reads are audited.
Tests independently check lossless tagged watermark, reserved-key ergonomics,
missing-not-zero, bounded nonexhaustive search, distinct occurrences, contributor
sorting, steer, required contrast fields, and single unknown delivery (no retry).
Real successful/failed native-Lua traces motivating helpers remain at local audit
records14503/14514/15023 (see observability report); live helper comparison next.

## Qualification and narrow review

Final Mac debug 2/2 (8.34s), release2/2 (7.82s), ASan+UBSan2/2 (9.27s).
Final Neuroses debug/release/ASan+UBSan2/2 each:
`context/linux/run-9ebbnvcz`; earlier pre-quarantine-durability run-d42sosr7 is not
final qualification. Native formatting applied; targeted clang-tidy clean before
last quarantine-directory fsync/test addition (compiler diagnostics clean final).
Compilation caught a missing include and test initializer brace; not behavioral
passes. No B1/full-suite recertification.

One Kimi narrow review completed: run-j71avml7,
session_00979d9d-1688-49c4-86da-3a0decbb9533, wrapper0/child0. Actual unchanged
handoff in 2026-10-06-useful-work-candidates-kimi-review.md. Findings dispositions:
1. Recovery dead-end consequential: fixed explicit evidence-linked abandonment
   retaining clean prior worktree or quarantined partial bytes; direct tests pass.
2. Record-before-state crash window: proposed unsafe program launch cannot occur.
   `save` must publish/fsync state and return before Git/Child dispatch. An orphan
   intent is planning, NOT effect admission/observation; failed save never dispatches.
   Canonical state published as running/unknown BEFORE possible effect, so crashes
   after dispatch remain blocked. No O(history) startup scan adopted.
3. Nested pool: intentional ignored context/ may hold isolated linked checkouts;
   nested is not aliasing primary. Owned membership/symlink/primary equality gates
   protect switching. No requirement to prohibit ignored nested pools adopted.
4. Candidate status mismatch: fixed candidate running/unknown alongside slot.
No review-of-review/consensus sought. Remaining unknowns are explicit, not success.

Literature consequences and bounded allowances in
`docs/USEFUL_WORK_CANDIDATE_SUBPLAN.md`; interface in USEFUL_WORK_CANDIDATES.md.
Operator economy amendment CONTEXT_ECONOMY_EXPERIMENT read before design.
No speedup evidence claimed yet: actual finite matched repeated tasks across two
families and fresh transfer, separately recorded correctness/resources/task-level
uncertainty are remaining .14 work. All-provider review usage absent from capture
is unavailable (not zero). Raw private captures stay ignored.
