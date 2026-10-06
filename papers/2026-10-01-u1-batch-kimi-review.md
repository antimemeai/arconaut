# Independent review: U1 physical batch writer (`FramedJournal`) and its oracles

**Scope inspected (read-only; nothing executed):** `docs/U1_RETAINED_STATE_SUBPLAN.md`, `include/arconaut/journal_writer.hpp`, `src/journal_writer.cpp`, `tests/journal_batch_test.cpp`, `tests/journal_crash_test.cpp`, plus the supporting interfaces `include/arconaut/journal.hpp`, `src/journal.cpp`, `include/arconaut/journal_storage.hpp`, `src/journal_storage.cpp`, `include/arconaut/foundation.hpp`, and the supplied `context/u1-batch-crash-debug.log`. Codec/native layers were treated as previously reviewed and only read for contract verification. I did not build or run anything; the log showing 7/7 passing on arm64 Mac debug is reporter-supplied context, not my execution.

## Summary

The batch-preparation, bounded-arithmetic, short/error-I/O handling, allocation staging, poison/block failure closure, staged recovery, and crash-test structure are substantially correct. I independently re-derived every constant in both test files (offsets 147/153/179/187/199/203/238/270/294, sequence 4, `{203,1}` capacity boundary, the `fail_after=130` partial-write walk, the mask-matrix expectations in `nonprefix_survival_and_corruption`) and all match the documented 112/32/56-byte layout and the code's behavior. I found **no high-severity implementation defect** in the writer. Findings are one minor misclassification, one dead/untested failure path, and several concrete oracle gaps where a real regression would pass the current suites.

## Findings

### F1 (minor, in-component): oversized length field misclassified as `capacity`, not format corruption
- **Location:** `src/journal_writer.cpp:327-330` (mirrors `src/journal.cpp:212-214`).
- **Trigger:** a frame whose 4-byte length field exceeds `max_payload` on disk (bit damage or hostile bytes).
- **Consequence:** the recovery report's cause is `ErrorCode::capacity`, which the sub-plan (docs line 77) reserves for configured-limit closure; format damage should report torn/checksum/sequence/format. A damaged file is reported as if it merely exceeded a configured bound, misleading continuation/operator triage. Both paths still block, so this is reporting-only.
- **Fix:** map "length field exceeds declared limit during scan of on-disk bytes" to `corrupt` (or `invalid_range` documented as format cause) in `scan()`; keep `capacity` only for configured-limit admission rejection. A scan test injecting an oversized length field and asserting the report code would pin this.

### F2 (test gap): the zero-progress write branch is dead code in the oracle
- **Location:** path under test `src/journal_writer.cpp:199-201`; harness flag `tests/journal_batch_test.cpp:44,66-68` (`StorageState::zero_progress`) is **never set** by any test.
- **Trigger:** an adapter returning `success(0)` from `write_at`.
- **Consequence:** the spec-required "zero progress is an error" behavior (docs line 101) has no oracle; deleting or inverting the `== 0` check (infinite-loop / silent-divergence regression) passes all suites.
- **Fix:** one test setting `zero_progress = true` before `append`, expecting `ErrorCode::io` and `poisoned` state.

### F3 (test gap): multi-frame batches are never exercised
- **Location:** all tests use `const std::array drafts{JournalDraft{...}}` — exactly one draft (`tests/journal_batch_test.cpp:131`, `tests/journal_crash_test.cpp:134`).
- **Untested behavior:** commit count field ≠ 1 (`journal_writer.cpp:258`), multi-frame CRC chaining across >1 preceding frame (`:259`, scan `:359-360,380`), `batch_first` consistency across multiple frames (scan `:349-351`), and the accumulated batch-byte budget (`append:228-232`, scan `:352`).
- **Consequence:** a regression that hardcodes count=1, mis-chains the incremental CRC for ≥2 frames, or miscomputes the running batch size is invisible.
- **Fix:** a 2–3 draft batch with independently computed expected commit payload bytes (first, count=2/3, CRC over both frames), plus a batch that exactly reaches and one that exceeds `max_batch_bytes` expecting `capacity` with no write (state stays `live`, byte-identical file).

### F4 (test gap): staged-but-uncommitted pending records are never asserted
- **Location:** `tests/journal_batch_test.cpp:186-204` (cut loop) and crash test `:193`.
- **Trigger:** cuts in `[235, 294)` leave a fully decoded, CRC-valid source frame in `pending_` with a missing/torn commit; `scan()` retains it (`journal_writer.cpp:376-380`) and `pending_records()`/`read_payload` expose it (header `journal_writer.hpp:76-78`, spec docs lines 76-78).
- **Consequence:** the tests assert only `staged_records().empty()` and the `incomplete` code; a regression that drops `pending_` (losing the spec-required inspectable pending bytes) still passes.
- **Fix:** for cuts ≥ 238 assert `pending_records().size() == 1` and successful `read_payload` of the pending record returning `original`.

### F5 (test gap): no allocation-failure (OOM) oracle despite the plan requiring it
- **Location:** docs `U1_RETAINED_STATE_SUBPLAN.md:99-100,211` require "An allocation error at preparation leaves storage and indexes unchanged" and OOM cases; neither test file injects allocation failure into `allocate`/`append`/`scan`.
- **Consequence:** the `catch (std::bad_alloc)` paths (`journal_writer.cpp:66-68, 297-299, 398-400`) are unverified; an exception escaping the pre-write preparation region, or an accidental allocation moved after the sync point, would not be caught by these suites (ASan/UBSan won't synthesize `bad_alloc` either).
- **Fix:** an allocator-injecting harness (e.g., a directory whose returned file plus a global new-handler counter, or `#include <new>` replacement in a dedicated TU) that fails the N-th allocation during `create`/`append` and asserts: error `allocation`, no write issued (`writes == 0` delta), file bytes and `physical_records()` unchanged, state still `live`.

### F6 (test gap): read-side short/EINTR handling and sync interruption are unexercised
- **Location:** `read_exact` retry/short loop `journal_writer.cpp:169-188`; `MemoryFile::read_at` always returns the full requested count and never returns `interrupted` (`tests/journal_batch_test.cpp:50-59`).
- **Consequence:** a regression in the read loop's accumulation or its zero-read→`incomplete` mapping passes silently; likewise EINTR from `synchronize` (correctly treated as unknown-durability → poison per docs line 116) has no oracle.
- **Fix:** a chunked/interrupting read path in `MemoryFile` mirroring `max_write`/`interrupt_writes`, and one sync-interrupted case asserting `poisoned`.

### F7 (test gap): open-time identity/header rejection and scan capacity closure lack oracles in this component's suites
- **Location:** `journal_writer.cpp:139-161` (short file → `incomplete`, oversize file → `capacity`, `wrong_environment`, header `conflict`) and `:372-375` (scan-side `max_records` exhaustion).
- **Consequence:** these are `FramedJournal`-level decisions (not codec/native behavior, which is covered elsewhere); a regression accepting a wrong-environment or limit-mismatched header, or scanning unbounded records past `max_records`, passes both assigned test files.
- **Fix:** four small negative `open` cases (mutated environment byte, mutated limits byte, truncated-to-64 file, file grown past `max_file_bytes`) and a two-batch journal reopened with `max_records=1` expecting a `capacity` recovery problem and `blocked` after `confirm_recovery`.

### F8 (test gap): crash suite never dies during journal creation/publication
- **Location:** `tests/journal_crash_test.cpp` arms the crash only around the second `append` (`:170-173`); the plan (docs line 224-225) requires "failure at every new-file/directory publication point".
- **Consequence:** crash-mid-header-write and crash-before/after `synchronize_directory` in `create` (`journal_writer.cpp:100-111`) are unobserved; e.g., a regression reordering file-sync/directory-sync in `create`, or treating a torn-header file as a fresh journal on the next `create` (must instead `conflict` via exclusive creation), would pass.
- **Fix:** crash points around `create`'s header write, file sync, and directory sync; afterwards assert the next `create_exclusive` returns `conflict` and `open` returns a decode error, never a fresh empty journal.

### F9 (oracle weakness, informational): `resume_after_reconciliation` gating not asserted for the cut==112 clean case
- The cut loop correctly asserts `audit_unavailable` for damaged cuts (`journal_batch_test.cpp:201-203`) but never asserts that the `end == 112` clean cut *does* permit resume; a regression blocking resume on clean files passes. Add one positive `resume_after_reconciliation()` assertion in that branch.

## Explicitly out-of-scope / not findings
- Continuation-journal creation, RetainedState semantic validation, issuer non-reuse, ledger swap, emergency slots: declared unfinished and not claimed by this component; their absence from `journal_writer.hpp` matches the task framing.
- Semantic-frame dependency replay: `FramedJournal` correctly treats semantic frames structurally only; physical indexes are not used as admission permission anywhere in the inspected code.
- Power-loss ordering/durability, APFS behavior: explicitly disclaimed by the plan; the crash test's own comment (`journal_crash_test.cpp:59-60`) states the same. Process-crash oracles are legitimate and correctly structured (SIGKILL verification at `:183`, the `_exit(98)` fallback correctly fails the parent if SIGKILL malfunctions, and the `point == after_return` check at `:172` converts a non-crashing bug into a child failure).
- `MemoryFile`-modeled non-prefix survival (mask matrix) is a documented model, correctly not claimed as a filesystem guarantee.

## Limits of this review
Single-writer sequential behavior only; no concurrency, no execution, no coverage data. Mac release/ASan and Neuroses lanes were not consulted beyond the supplied debug log. Where a path is noted as possibly covered by `journal_native`/`journal_codec` tests (F7 partially), those files were outside the assigned reading set and I did not verify their oracles.
