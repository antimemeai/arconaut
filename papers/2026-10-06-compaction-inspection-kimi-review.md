# Independent review: revision-bound context inspection

Scope inspected: `docs/MANAGED_COMPACTION_SUBPLAN.md` (final section), `src/context.cpp` (`inspect`, reopen replay, `append`/`edit`/`reject_managed`/`finish_workflow` head handling), `tests/context_test.cpp` (new guard cases, lines 194–232), `src/tools.cpp:254` (description), `src/coding.cpp` (`inspect_op` bridge 297–397, tool dispatch 804–805, `operation` error boundary 516–616).

## What checks out

- **Guard contract** (`src/context.cpp:541-542`): optional `revision` compared by exact equality against `head_` before `string_field(query, "kind")` and before any serialization (`dump_json` at 574). `inspect` is `const` and calls no `log_.record`, so no context/audit mutation on the conflict path. Tests at `tests/context_test.cpp:216,230-231` assert journal cursor and view are unchanged after conflict — oracle matches implementation.
- **Malformed revision**: non-string revision falls into `string_field` (`src/context.cpp:24-30`) → `ErrorCode::corrupt`; tested with `Json{true}` (`context_test.cpp:209-216`). Omission is unguarded and backwards compatible (`query.find("revision")` short-circuit).
- **Reopen/replay consistency**: replay (`context.cpp:94-128`) only advances `head_` on accepted packets and validates `observed == head_` per packet, so a reopened store's `head_` equals the live head the guard compares against. Publication paths (`append` 154-192, `edit` 224-227, managed 532-537) and pending-expected advance (187-188) update `head_` atomically after `log_.record`; no path publishes without recording or records without updating `head_` on acceptance.
- **Cancellation/staging**: staged workflow manage and `finish_workflow(false)` (`context_test.cpp:187-192`) leave `head_` and history untouched; guard identity is unaffected. Rejected `finish_workflow` throws before mutation.
- **Error propagation**: tool path — `inspect` errors are caught in the `Boundary::dispatch` (`coding.cpp:571-580`) and returned as `error_json` results (usable by the model), disposition failure recorded. Lua path — `bridge` catch at `coding.cpp:379-388` converts to `error_name(code)` and `lua_error`, so `arco.inspect` conflicts surface as named, catchable Lua errors.
- **Conservatism**: guard fires even when `originals`/`index` content is unchanged (head moved by an append that didn't touch the queried entry), matching the stated "conservative" contract.

## Defect found (correctness gap, limited to `kind=history`)

**`src/context.cpp:541` vs `reject_managed` (322-339) and rejected `edit` (199, 220-223).**

- **Trigger**: caller pages `context_inspect` with `kind:"history"` bound to revision R. Between pages, a rejected `context_manage` (stale-base or pending-overlap) or a rejected/stale `context_edit` occurs. Both append a packet to `history_` and to the audit journal **without advancing `head_`** (`reject_managed` has no `head_` swap; `edit` only swaps when `publishes`).
- **Consequence**: the serialized history payload and `total_bytes` change, but a guarded page request with revision R still passes the equality check and returns bytes from the *new* history. The caller silently concatenates pages from two different history snapshots — exactly the torn-pagination outcome the guard exists to prevent ("callers must restart on stale"). For `originals`/`index` kinds this cannot happen (rejections touch only `history_`), so the gap is history-specific, but history is one of the three advertised kinds.
- **Fix**: for `kind:"history"`, guard against the latest history entry's revision (e.g. `string_field(history_.back(), "revision")` when history is non-empty) instead of `head_`, or return a dedicated `history_revision` in responses and guard on that. Alternatively record a `history_length`/`history_cursor` in each page and compare. The per-kind guard selection must also be honored in the tests (add a stale-history-after-rejection case; current tests at `context_test.cpp:222-232` only exercise append, which does advance `head_`).

## Improvements (not correctness failures)

- **`src/tools.cpp:254`**: description typo `"default4096"` (missing space). Cosmetic, but this string is model-facing documentation; fix to `"default 4096"`.
- The description does not state the conflict-on-stale semantics ("strict current-head guard" is close but doesn't name the conflict error or restart requirement). Adding "returns conflict error on stale revision; restart pagination from the returned revision" would make the contract discoverable to model callers without reading the subplan.
- Guard precedence: conflict fires before `limit`/`offset` validation (`invalid_range`). This is consistent with "before serialization" and arguably correct, but means a caller cannot distinguish "stale" from "stale and malformed page request". No change required; noted for completeness.
- Tests do not cover a stale guard where `kind` itself is invalid alongside a stale revision (guard wins → conflict, not invalid_range); behavior is defensible but untested.

## Limitations

I did not re-verify B1 mechanics, `Json::find` behavior on non-object queries (both branches — throw or skip — converge on a thrown error here, so immaterial), or rerun any build/tests (read-only review; relied on reported green runs and source/test inspection). The source-inspiration files (`gptme/.../resume.py`, `tests/test_auto_compact.py`) were treated as study-only per instructions and not consulted for conformance.