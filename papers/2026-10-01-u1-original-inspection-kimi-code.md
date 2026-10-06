## Independent review: bounded original-byte inspection (U1 refinement)

**Scope inspected:** `docs/U1_RETAINED_STATE_SUBPLAN.md` (lines 312–346 refinement + review resolutions); `include/arconaut/journal_writer.hpp:44–50, 76–77`; `src/journal_writer.cpp:487–531` (new `read_original_range`); `include/arconaut/retained_state.hpp:69–73`; `tests/journal_batch_test.cpp:53–96, 245–305`; `tests/retained_state_test.cpp:484–510, 718–738`; mock oracles (`MemoryFile::read_at`/`extent`, journal_batch_test.cpp:76–126).

### Implementation verdict — no blocking faults

Verified against each review axis in `src/journal_writer.cpp:487–531`:

- **Bounds:** length gated on `header_.limits.max_payload` before any I/O (489); extent fetched then `offset > size || length > size - offset` uses checked subtraction with no wrap (501); `offset + done` in the read loop cannot overflow since `offset + done < extent` (511). `UINT64_MAX` offset and EOF+1 cases are oracled (test 270–271).
- **Allocation:** exact checked size, inside `try`, `bad_alloc` → `ErrorCode::allocation` (504–530); OOM oracle with `allocation_cut = 0` passes and preserves state (test 286–290).
- **EINTR:** read loop tolerates 7 consecutive interruptions, returns the actual `interrupted` error on the 8th, resets on progress (508–525) — matches doc lines 341–343 and the `interrupt_reads = 8` / `interrupts_between_reads = 7` oracles (test 280–281, 256–257). Extent retry uses the same 8-call bound (493–497); both failure (8 interrupts) and success-after-retry (1 interrupt) sides oracled (test 282–285).
- **Exact original bytes:** chunked reconstruction (offsets 0..end step 11, max_read=2 forcing short reads) compared byte-for-byte against `submitted` (test 258–267); poisoned-writer range (112,18) matched against raw storage (test 300–305); torn-reservation full-file match (retained_state_test.cpp:730–732).
- **Error/non-interference:** zero progress → `incomplete`, impossible count → `io` (518–523), both oracled; no write to `state_`, `recovery_`, or any record index on any path; storage unchanged (`directory.state->submitted == before`, test 267–268, 290).
- **No authority/reset:** reader never touches admission/recovery; RetainedState forwarding is unconditional (retained_state.hpp:70–73) and works under synthesized blocked (`recording_failed_` && physical live) without clearing the gate — oracled including that `attempt()` still reports `reconciliation_required` and redispatch stays `audit_unavailable` (retained_state_test.cpp:500–509). Raw bytes stay unvalidated: `source()`/membership rules untouched; the pending-frame CRC/`source()` path remains separately gated.

### Oracle gaps (non-blocking, concrete)

1. **Extent hard-failure path untested** — `src/journal_writer.cpp:498–500`. `MemoryFile::extent` (journal_batch_test.cpp:120–126) can only inject `interrupted`; a non-interrupt extent error (e.g. `io`) is never exercised. Trigger: adapter `extent()` fails with `io`. Consequence: a regression mapping it to the wrong code, or retrying non-interrupt errors, would pass green. Fix: add a mock `extent_failure` flag and `error_is(cut->read_original_range(0, 1), ErrorCode::io)` with state unchanged.

2. **Accept-side payload boundary untested** — line 489 uses `length > max_payload`; only the reject side (`(0, 65)` with max 64, test 272) is oracled. An off-by-one to `>=` would pass everything. Concrete input: `read_original_range(0, 64)` on the ≥112-byte cut file must succeed with 64 bytes.

3. **Writer-state coverage asymmetry** — doc (line 318) claims availability "in every writer state," but FramedJournal-level inspection is oracled only in `recovery_pending` and `poisoned`; RetainedState-level in `recovery_pending` and synthesized-`blocked`/`poisoned`. No case inspects a plain `live` or `recovered` journal (e.g. immediately after the successful `resume_after_reconciliation()` at test 288). The reader has no state branching, so risk is minimal, but one `live`-state read would make the claim direct.

4. **Mid-loop interruption-budget exhaustion** — only 8-consecutive-from-start (test 280) and 7-between-progress (257) are oracled; progress followed by 8 consecutive interruptions (partial buffer discarded, error, state preserved) is a distinct path through the same counter reset at line 525. Minor; same code, but a cheap explicit case.

### Notes

- Retried-extent success correctly returns the *later* extent (test 284–285 asserts byte content after re-fetch).
- Range checks correctly precede any read attempt: `(112, 1)` at extent 112 yields `invalid_range` even with `zero_read` armed (test 274–275), confirming check ordering.
- TOCTOU (extent vs. concurrent operator writes) is explicitly disclaimed in the struct comment (journal_writer.hpp:44) and doc lines 327–328; observations are not presented as atomic snapshots or validated sources anywhere in the oracles.

No faults requiring changes before integration; the four oracle additions above would close the remaining coverage seams.