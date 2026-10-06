# Follow-up independent review: new prepare/publish_initial oracles

## Scope and method
Re-read the current `tests/environment_head_test.cpp` (880 lines, in full) and re-verified `src/environment_head.cpp` `prepare`/`create`/`publish_initial` (lines 183–258). **Production code is byte-identical to my prior review** in the scoped region; only tests changed. No execution, no edits. Scope remains the head construction phases; the unimplemented `RetainedEnvironment` owner is not accepted or reviewed.

## Prior gaps — resolution status

**Gap 1 (no allocation sweep for `publish_initial`'s failure path) — RESOLVED.**
New sweep at `tests/environment_head_test.cpp:425–462`: fresh `Script`/`prepare`/`FramedJournal` per cut, root+authority bytes captured before arming and verified unchanged after every cut (437–438); failure asserts `allocation` + `!healthy` + zero-I/O `audit_unavailable` no-retry (443–448); after owner destruction, ground truth comes from a fresh `open` keyed on actual head presence — healthy open when the rename landed, `io` when it did not (452–458). This correctly exercises the real fault classes:
- bad_alloc in the absence recheck (`environment_head.cpp:244`, `attempted==false`, no poison needed — owner was never healthy),
- bad_alloc in temp create/write/rename inside `write_selector` (`attempted==true` at line 171 → `Transaction` poisons, 88–92),
- bad_alloc thrown *after* a completed rename (e.g. inside the fake's `dirsync` call logging) → head exists, owner still correctly unhealthy/consumed, fresh open establishes the actual selector.

The 64-cut bound reaching success is plausible for this shorter path (the replace sweep needed 96) and the task states debug green.

**Gap 2 (invalid-then-corrected strength) — RESOLVED.**
Lines 397–405: `publish_initial(static_cast<SyncStrength>(99))` returns `invalid_range` with provably zero additional storage calls and `!healthy`, then `publish_initial(full)` succeeds — pinning the check-before-consume ordering at `environment_head.cpp:238–240` and that the flag survives invalid input.

**Gap 3 (repeat healthy publish zero I/O) — RESOLVED.**
Lines 330–332 now capture `published_calls` and assert `calls.size()` unchanged across the `audit_unavailable` repeat.

**Gap 4 (non-ENOENT absence-check errors) — RESOLVED for both phases.**
New `head_open_error` hook (`Script` field at test line 146, fake at 261–262) injects `{io, EACCES}`; both prepare-time (418–421) and publish-time (409–416) refusal assert **exact `Error` equality** with the injected value, `!healthy`, and no `head` file manufactured (423). This pins the propagation branches at `environment_head.cpp:206–207` and `247–248`.

## Do the new oracles test the real behavior (not themselves)?
Yes, with two benign characteristics worth recording:

1. **Cut indices are not stable production-site ordinals.** The global `operator new` also intercepts test-harness allocations (`calls.push_back` string growth, `std::map`/`std::string` inside the fakes), so a given cut number may fire inside fake bookkeeping rather than a production allocation site. Because the sweep runs exhaustive-until-success with independent fresh state per cut and checks outcomes (error code, health, no-retry, bytes, open ground truth) rather than cut positions, coverage is complete and the assertions remain valid; only the readability of "cut N = site X" is absent. Not a defect.

2. **The allocation-during-recheck sub-case is indistinguishable in outcome.** When the recheck allocation fails (`attempted==false`), the no-retry/`!healthy` assertions pass trivially because the owner was never healthy — the `Transaction` poison path and the pre-`attempted` path converge on the same observable state for an initial publication. The distinction only matters for `replace` (covered by the existing 96-cut sweep). Acceptable.

## New code/oracle defects in the added tests
None found. Specifically verified:
- Strength test (400–404): `strength_calls` captured after prepare, so the `invalid_range` call truly performs no storage I/O; the subsequent successful publish proves `initial_prepared_` was not consumed.
- `head_open_error` short-circuits before the map lookup (261–262), so the `!s->files.contains("head")` check at 423 is meaningful in the publish case (recheck attempted, no file created) and trivially true in the prepare case — harmless.
- Publish-case EACCES refusal occurs before `attempted` is set, leaving no `Transaction` poison — consistent with "failure consumes the operation" since the flag was already cleared at line 240; the consumed-flag retry behavior is separately pinned by the cut loop (347–353) and sweep (445–448).
- Sweep resets `allocation_cut` before any assertion allocation (`store(-1)` at 436 precedes `CHECK`s and `open`), so oracle failures can't be caused by a still-armed cut.

## Residual items (unchanged from prior review, explicitly out of scope)
- Stale `head.<hex>` temp after write/sync cuts remains diagnostic residue only; no path to a wrong publication (prior review item 5).
- No power-loss durability claim; process-death (SIGKILL) scope only, per contract.
- Full `RetainedEnvironment`/chain owner remains unimplemented and unaccepted.

## Verdict
All four prior gaps are resolved by direct, behavior-sensitive oracles. Production code is unchanged and has **no blocking defects** in the scoped prepare/publish_initial phases; the expanded test suite has **no concrete oracle defects**. Debug-green claim is consistent with my trace of the new cases; I did not execute anything and infer no Linux or whole-U1 acceptance.
