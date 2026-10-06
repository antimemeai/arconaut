## Independent review — RAM pending-proposal ownership (selected-chain continuation)

Scope inspected: the supplied excerpts of `retained_proposal.{hpp,cpp}`, `retained_state.{hpp,cpp}` (`append_impl`, `retain_rejection`), the named tests, and the `FramedJournal::append` lifecycle needed to judge the Blocked-vs-Poisoned correction. Read-only; no files were modified.

### Verdict

The correction is sound and the core ownership invariants hold. I found **one real pre-validation defect** and several **oracle gaps**. No evidence that RAM bytes are treated as source authority or that reopen synthesizes the packet (`tests/retained_state_test.cpp`, `pending_proposal_owns_originals`, final reopen block explicitly asserts `!state->pending_proposal()`).

### What checks out

- **Codec** (`src/retained_proposal.cpp`): exact ARPROP01 layout (magic, 16B journal, LE sequence/end, count+length-prefixed blobs); `ProposalReader::blobs` bounds count against `remaining()/4` and lengths against actual bytes; `decode` enforces `bytes.size() > max_bytes → capacity`, trailing bytes → corrupt, bad magic/cursor fields preserved verbatim (stale `{UINT64_MAX, UINT64_MAX}` round-trips without gaining authority — header comment correctly states "no semantic/admission authority"). Decoded blobs own storage independent of input (verified by the mutable-wire test). Encoder pre-computes size against the caller bound, never from input counts; `bad_alloc` → controlled `allocation` on both paths. No checksum/permission layer, as specified.
- **Ordinary capture** (`append_impl`): `pending_proposal_.emplace` precedes the first storage write (`journal_->append`); on write failure it is reset **only** when `state() != poisoned`, i.e. unknown/poisoned writes keep the packet, clean pre-write refusals (Blocked/extent) clear it. Success clears it. This matches the corrected contract in `pending_proposal_clean_outcomes` (Blocked + stale-cursor Blocked cases assert `!pending_proposal()` and zero writes).
- **Rejection capture** (`retain_rejection`): outer packet emplaced before any inner chunk write; inner `append_impl(..., diagnostic=true)` calls never emplace/reset `pending_proposal_` (the `!diagnostic` guards), so chunks cannot overwrite the outer packet. Grouped failure: `recording_failed_ = prefix_retained`; packet reset only when `!prefix_retained && state() != poisoned` — so a pre-first-group Blocked refusal clears it, any committed group preserves the full OUTER packet on every later failure (Poisoned or Blocked), and marker success clears it. Stale/wrong `expected` cursor is preserved inside the rejected packet (`rejected_wrong_cursor_owns_caller_packet`). Dependency sequence assignment (per-group `sequence++` plus `++sequence` for each intermediate commit) matches `FramedJournal::append` numbering (frames `first..first+n-1`, commit `first+n`).
- **Together path**: `marker` is encoded before dependency sequences are patched, but `append_impl` re-encodes `retained[0]` itself; the stale `marker` is used only for byte-size arithmetic, which is sequence-independent. Not a defect.

### Defect

**D1 — `retain_rejection` capacity pre-check under-counts frames** (`src/retained_state.cpp`, in `retain_rejection`, the block after `total_bytes` is computed):
```cpp
if (chunks.size() >= remaining ||
    total_bytes > capacity_.max_file_bytes - current.end_offset)
```
`frames = chunks.size() + groups.size() + 2` was computed immediately above but never used here. The record-capacity gate should be `frames > remaining`. Trigger: `capacity_.max_records - physical_records().size() == chunks.size() + 1` (or any value in `[chunks.size()+1, frames-1]`) with a multi-group rejected proposal — the pre-check passes, one or more groups commit, and a later group/marker append fails `capacity` mid-sequence. Consequence: a refusal that could have been clean and no-I/O becomes a partial grouped commit (`recording_failed_ = true`, blocked state, retained packet) — precisely the outcome the correction was designed to reserve for genuine unknown writes. Red expectation: construct capacity so `remaining == frames - 1`, assert `ErrorCode::capacity`, zero writes (`storage->writes` unchanged), `state() == live`, and (per the corrected contract) `!pending_proposal()` since no group committed. Fix: replace `chunks.size() >= remaining` with `frames > remaining`.

### Oracle gaps

- **G1 — Blocked mid-group with a committed prefix is untested.** `rejection_partial_failures` exercises only Poisoned (`fail_sync_call` 3–5) and allocation (`fail == 6`) paths. No case fails a *pre-write* extent check after group 1 committed; expected red: `state() == blocked` (via `recording_failed_`), `pending_proposal()` retained with the full OUTER packet, group-1 sources readable.
- **G2 — Marker-failure-after-all-groups state assertion.** `fail_sync_call == 5` poisons the marker write, but the test only asserts `state() != live` and packet contents; it never pins `poisoned` vs `blocked`, leaving the `recording_failed_ = true` (marker branch, `retained_state.cpp` final `else`) vs poisoned distinction unobserved.
- **G3 — `allocation_before_publication` excerpt was not included** in the material provided; I could not verify that `allocate_after_sync_call` actually proves "no allocation after sync" in `FramedJournal::append`'s metadata publication (the `records_.push_back` reserve comment). The mechanism (`allocation_cut.store(0)` post-sync in `MemoryFile::synchronize`) would catch any post-sync allocation, but I have not seen the assertions. Treat as unverified, not failed.
- **G4 — No direct test that a subsequent ordinary `append` cannot clobber a retained rejected packet.** It holds structurally (post-failure state is non-live → `audit_unavailable` gate in `append_impl`), but no red exercises an `append` attempt while a `rejected_proposal` packet is pending to assert byte-equality before/after.
- **G5 — Codec:** the boundary `count == remaining()/4` with a trailing zero-length blob is only implicitly covered by the empty-blobs case; the corrupt-permutation offsets `{0,23,40,44,51,55}` don't mutate the events-count field (51 is the sources region per the expected layout — events count sits at 51 for the single-source packet, so this is covered; the *events length* field at 55 is covered). Adequate, but a two-blob truncation at the second length prefix would strengthen it.

### Out-of-scope confirmations

Per instructions, I did not treat selected-chain persistence/replay/entropy, `CustodyVerifier` (U3), or the maintenance codecs as reviewed or complete; nothing in the inspected code grants the RAM packet replay or permission authority — it is exposed read-only via `pending_proposal()` with an explicit "not committed sources or permission" contract.

**Bottom line:** approved-in-scope with D1 to fix (one-line, `frames > remaining`) and G1/G2 red tests to add before the grouped-diagnostic path can be called fully oracled; G3 requires reading `allocation_before_publication` to close.
