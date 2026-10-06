## Scope inspected

- `docs/U1_RETAINED_STATE_SUBPLAN.md` lines 312–337 (the full "Bounded original-byte inspection refinement" section)
- `include/arconaut/journal_writer.hpp` (full), `src/journal_writer.cpp` (`read_exact` 169–188, `scan` 302–424, `read_payload` 452–485, state transitions 426–451)
- `include/arconaut/retained_state.hpp` (full), `src/retained_state.cpp` (`source` 793–805, gating via `state()`/`recording_failed_`)
- `include/arconaut/foundation.hpp` ErrorCode list

## Findings

No blocking design fault. The refinement is implementable as specified, and its key invariants are correctly anchored against the existing code:

- **State non-interference is well-targeted.** Existing `read_payload` (journal_writer.cpp:465–468, 475–477) poisons `state_`/`recovery_` on any read/decode failure; the refinement correctly mandates the new reader *not* share that failure path and never touch publication/admission state. As long as the implementation does not reuse `read_payload` or its error handling, the "no accidental gate reset/publication" requirement holds. `RetainedState::source` (retained_state.cpp:799–803) keeps pending/uncertain sources closed via `audit_unavailable`, and the doc explicitly preserves that (`source()` keeps its rules) — forwarding must bypass `source()` entirely and call `journal_->read_original_range` unconditionally, which the doc's "available in every writer state" covers.
- **Bounding is sound.** `length <= header.limits.max_payload` before allocation, checked `extent - offset` subtraction, exact-size allocation with `bad_alloc` → `Allocation`, short-read accumulation, zero-progress → `Incomplete`, over-read → `Io` all match the existing checked primitives (`read_exact` lines 179–184) and available ErrorCodes.

Specification gaps the implementer/oracles will hit (all underspecification, not design errors):

1. **Interrupted-retry cap vs. shared `read_exact` (journal_writer.cpp:174).** Existing `read_exact` retries `interrupted` *unboundedly* and is shared by `scan()` (lines 321, 336) and `read_payload` (line 463). The doc mandates "at most eight consecutive interrupted calls" for the new reader but does not say whether this cap is applied by modifying `read_exact` (which would introduce a brand-new failure mode into recovery `scan()` during `open`, changing existing recovery oracles) or by a separate bounded helper used only by `read_original_range`. The refinement should state explicitly: new helper (or parameterized retry budget), leaving `read_exact`'s semantics for scan/recovery untouched — otherwise previously-succeeding opens can newly fail after 9 EINTRs.
2. **Unspecified error codes.** (a) After the 8-consecutive-interruption budget is exhausted, the returned code is unstated (`Interrupted` pass-through, `Io`, or `Incomplete`?). (b) Range violations (`offset > extent`, `length > extent - offset`, `length > max_payload`) have no mandated code — `invalid_range`, `capacity`, and `overflow` are all plausible existing codes. Oracles "exercising every length cut" cannot assert expected errors without this.
3. **RetainedState-level blocked coverage.** The oracle list covers "read after Poisoned/Blocked" at the journal level but not the RetainedState-synthesized `blocked` state (`recording_failed_ && live`, retained_state.hpp:79–83). Since public forwarding must not consult `state()`, an oracle should pin that forwarding still succeeds while `recording_failed_` is set — this is the cheapest place an implementer could accidentally reuse the `state() != live → audit_unavailable` gate pattern used by `append_impl`/`dispatch`.

Non-issues confirmed: `read_original_range`/`ObservedJournalBytes` are new declarations (not yet in the headers), so no signature conflict; the empty-range-at-EOF case terminates naturally (zero-length span skips the read loop); extent re-check vs. append races is moot under RetainedState's single sequential custodian, and external-writer non-atomicity is explicitly disclaimed (lines 327–328); chunked reads of larger originals via repeated bounded calls compose correctly with the per-call `max_payload` cap.

## Verdict

Sufficient to implement after resolving the three specification gaps above (retry-cap scoping, exact error codes, RetainedState-level blocked oracle). No library, approval, receipt, or replay machinery is implied or required. No files were mutated and no commands were run.