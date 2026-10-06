# Independent review: EnvironmentHead prepare/publish_initial split

## Scope inspected
- `include/arconaut/environment_head.hpp` (full, 64 lines)
- `src/environment_head.cpp` (full, 350 lines), focused on `prepare` (183–220), `create` (221–232), `publish_initial` (233–258), and their interaction with the unchanged `open` (259–312) / `replace` (313–349) and shared `write_selector`/`Transaction`
- `tests/environment_head_test.cpp` `prepared_creation_cases` (294–391) and `native_prepared_creation` (739–792), plus the in-memory/native fakes they depend on (127–293, 599–680)
- Contract: `docs/U1_CONTINUATION_SUBPLAN.md` "Environment owner implementation refinement" (lines 40–160) and related publication-order statements (161–170, 562–566)

## Verdict
**No blocking scoped defects found.** The split implements the written phase contract, and the direct oracles are genuinely sensitive to it. The full `RetainedEnvironment`/chain owner remains unimplemented and is **not** accepted here; this verdict covers only the head phase split and its direct tests. Below are verified contract points followed by non-blocking gaps/observations.

## Verified against contract (code → oracle)
1. **Authority before root, preparation owns directory** — `prepare` validates input and encodes the root header *before* any storage call (`environment_head.cpp:186–191`), then creates/locks `authority` (195–201), checks head absence (202–207), writes/syncs the declaration (208–214), returns an unhealthy owner holding the lease. Oracle: test lines 296–309 (authority locked, no head, second creator conflict with `files.size()==1`), 369–371 (invalid root → zero calls/files).
2. **Borrowed directory outlives segments** — member order `directory_` then `authority_` (`environment_head.hpp:55–57`) destroys authority before directory; test resets `journal` before `prepared` (328, 353–355, 753–754).
3. **One initial operation** — `initial_prepared_` cleared before the transaction body (240); failure at any of cuts 1–4 leaves it consumed and unhealthy via `Transaction` (`attempted` set at 171, dtor 88–92); second call returns `audit_unavailable` with provably zero I/O (test 347–349 compares `calls.size()`).
4. **Busy/no-retry reentry** — `in_transaction_` guard (234) blocks recursive `publish_initial`/`replace` from the storage callback (test 313–321); `interrupted` retry budget (29–35) is bounded, not retry-after-failure.
5. **Actual head absence recheck immediately before publication** — `publish_initial` re-opens `head` read-only (244–248) right before `write_selector`; foreign selector preserved and refused `conflict` with exactly one `open:head` call and bytes intact (test 374–379). A directory/other error (`!= {io, ENOENT}`) propagates (247–248).
6. **Final acknowledgement ordering** — temp create/write/sync → rename → dirsync (160–181), only then `healthy_`/`published` set nonallocating (252–254). Cut semantics match: head absent for cuts 1–3, present-but-unacknowledged (unhealthy, reopen establishes selector) for cut 4 (test 334–365). Post-ack noalloc is proven by arming `allocation_cut` inside the successful dirsync (test 383–390) — no allocation occurs between dirsync return and `publish_initial` return.
7. **Invalid strength correctable before the operation** — strength checked (238–239) *before* flag consumption (240).
8. **Late-head protection on composed `create`** — pre-existing `head` refuses at prepare with bytes preserved (test 393–397), matching prior head-only behavior; `scripted_cases` and `native_cases` regression matrices (including replace cuts, SIGKILL cuts, lock/read/sync interruption budgets) are unchanged and green per the task statement.
9. **Native cross-process exclusion and crash truthfulness** — `contender` (631–640) proves the flock excludes a second process *before the root exists* (744), and after root append (750). SIGKILL before rename → open returns `io` (no selector inferred from the orphan root, 777–778); kill after rename/dirsync → open sees generation 1/root active (780–782); original root bytes inspected independently without activation (786–790).
10. **`replace`/`open` unchanged** — guards, poisoning rules, generation overflow, wrong-environment/conflict discrimination, and the temp-collision-keeps-healthy behavior (519–525) all still hold under the expanded matrix.

## Non-blocking gaps and observations (not scoped defects)

1. **No allocation-cut sweep for `publish_initial`'s failure path.** The 0..96 bad_alloc sweep (test 469–491) covers only `replace`; publish's only allocation case is the success-path noalloc proof (383–390). A bad_alloc thrown by `create_exclusive`/temp write during publish is handled correctly by reading (catch at 255–257 + `Transaction` poison since `attempted` is set at 171), but no direct red case pins it. File: tests/environment_head_test.cpp:294–391.

2. **Strength-correction path untested.** The ordering at `environment_head.cpp:238–240` (invalid strength does not consume `initial_prepared_`) is correct but has no direct oracle: no test calls `publish_initial(invalid)` then `publish_initial(full)` and expects success.

3. **Repeated publish on a healthy owner asserts the error code only, not zero I/O** (test 327). The failed-then-repeated case does assert zero calls (347–349), so the risk is cosmetic.

4. **Absence-recheck error propagation untested.** Neither prepare (206) nor publish (247) has a case where `open_existing("head")` fails with a non-ENOENT error (e.g. EACCES) to pin that it propagates rather than being treated as absent. Behavior is correct by inspection.

5. **Stale temp file after cuts 1–2** leaves `head.<root-hex>` in the directory. It cannot cause a wrong publication: a fresh `prepare` in the same directory first fails on the existing `authority`, and any scenario that cleared `authority` but not the temp would fail `create_exclusive` with `attempted==false` (healthy already false for an initial publish), preserving prior `create` behavior. Noted only as diagnostic residue, consistent with "partial creation files remain evidence."

6. **No power-loss durability claim** is made or tested (no dirsync after authority creation in `prepare`, matching prior `create` behavior); this is consistent with the stated scope ("No power-loss or real U3 custody claim") and with the contract's crash-consistency scope being process death, not power loss.

## Limitations
- Read-only review; I did not re-run the matrix, so debug-green is taken from the task statement plus oracle sensitivity analysis.
- `open`/`replace` were checked only for non-regression induced by the split (shared `write_selector`, `Transaction`, member order), not re-reviewed wholesale.
- The full `RetainedEnvironment` owner, facade, replay, entropy, and custody layers referenced by the subplan are unimplemented; nothing here accepts or reviews them.
