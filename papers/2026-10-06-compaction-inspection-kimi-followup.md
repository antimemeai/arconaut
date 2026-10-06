# Follow-up review: torn-history fix verification

Scope inspected: `src/context.cpp:540-597` (`inspect`), `tests/context_test.cpp:194-249`, `src/tools.cpp:254`. Cross-checked packet/replay invariants at `src/context.cpp:94-128`, `reject_managed` (322-339), `edit` (199-223).

## Verdict: the defect is resolved

- **Guard now per-kind** (`context.cpp:542-546`): for `kind=="history"` with nonempty history the snapshot is `history_.back()`'s `revision`, otherwise `head_`. A rejection (`reject_managed` or rejected `edit`) appends a history packet with a fresh revision, so any previously issued history page revision now fails equality → `conflict`, before serialization and with no audit mutation (`inspect` remains `const`, no `log_.record`). This closes the torn-history hole I reported.
- **Invariant holds**: every history packet — append, edit (accepted or not), managed (accepted or rejected) — carries a string `revision` equal to its record identity, enforced on replay (`context.cpp:100-102`), so `string_field(history_.back(), "revision")` can never throw on a store that passed replay, and snapshot values are stable across reopen.
- **Response binding**: `revision` returns the snapshot (line 595), `context_revision` reports the view head (596) — additive field, backwards compatible; description updated accurately and the `default 4096` typo is fixed.
- **Tests are valid oracles**: the rejection-red case (`context_test.cpp:202-218`) captures the real first-page history revision, forces a stale-base rejection, asserts view unchanged, and asserts the subsequent guarded inspect throws `conflict` with the journal cursor unmoved. Append invalidation still binds the actual per-kind first-page revision (220-224) and asserts no audit/context mutation on conflict (247-248). Wrong-type revision still → `corrupt` (226-233).

## New-fault hunt (contract / error / paging / replay)

- **Empty-history edge**: `history_` empty + `kind:"history"` falls back to `head_` (initially `""`); a supplied `""` revision passes and returns a consistent `[]` page. No fault.
- **Error precedence shift**: `kind` is now parsed before the guard (line 541). Non-string `kind` with a stale revision → `corrupt` rather than `conflict`; unknown kind with matching revision → `invalid_range` after the guard. Both are defensible and precede serialization; no contract violation.
- **Kind asymmetry is intentional, not a gap**: `originals`/`index` data cannot change without `head_` advancing (rejections touch only `history_`; staging mutates only `pending_`, which is not serialized by `inspect`), so head-guarding those kinds remains tear-free.
- **Paging**: `total_bytes`/`next` computed from the same snapshot the guard binds; `context_revision` addition doesn't alter paging arithmetic. Replay rebuilds `history_` identically, so a mid-pagination restart re-derives the same snapshot revisions.
- **Documented semantics caveat** (not a fault): callers must now use the page-returned `revision`, not `context_revision`/`context_view` head, to guard history pages — after a trailing rejection these differ. `tools.cpp:254` states the per-kind snapshot guard and restart requirement, which is the correct guidance.

No consequential contract, error-propagation, paging, or replay fault introduced.

## Limitations

Read-only inspection only; I did not rerun the debug/campaign suites (relied on your reported green). I inspected the diff-relevant regions plus the replay/record invariants they depend on, not the full files. The rejection-red test uses a stale-base rejection; the `pending-overlap` rejection path shares the same `reject_managed` code, so it is covered by construction rather than by a dedicated test. The empty-history fallback branch is untested but traced manually.