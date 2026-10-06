# U1 byte/native Kimi rereview

Unchanged reply; read-only reasoning, no reviewer execution. Owner dispositions belong in JOURNAL.md.

## Rereview of integrated fixes — F1–F5

Scope reread in full: `src/journal.cpp`, `src/journal_storage.cpp`, `include/arconaut/journal_storage.hpp`, `tests/journal_codec_test.cpp`, `tests/journal_native_test.cpp`, and the revised native-storage paragraph of `docs/U1_RETAINED_STATE_SUBPLAN.md` (lines 121–139). Read-only; all assessments are code reasoning, not executed runs.

### F1 (creation durability vs. publication) — resolved as a spec clarification, sound
Docs lines 130–133 now state: "Low-level exclusive creation returns an opened file, without a durability/publication promise. Journal initialization/continuation orchestration syncs its contents and parent directory before publishing that journal or returning durable initialization success." This is consistent with the continuation protocol (lines 112–114: sync predecessor, header/batch, then parent directory) and removes the contradictory adapter-level claim I flagged. The rationale is correct: an empty-file fsync inside `open_file` would neither cover the not-yet-written header/batch nor substitute for the ordered publication protocol. No code change was needed and none was made. The obligation now rests entirely on the unimplemented initialization/continuation owner; that is tracked as future U1 work, not a present defect.

### F2 (predecessor extent bound) — resolved correctly, arithmetic verified
`validate_header` (src/journal.cpp:43–53): the `||` chain short-circuits, so `validated_end_offset - journal_header_size` is only evaluated after `validated_end_offset < 112` is excluded — no underflow. Division by `journal_frame_header_size` (32) before comparing against the sequence avoids multiplication overflow entirely. Boundary cases by hand:
- seq=0, offset=112: passes the zero-sequence clause; `(0)/32 < 0` is false → accepted (empty validated prefix, correct).
- seq=17, offset=112: `0 < 17` → rejected (my original reported case, now covered by test at journal_codec_test.cpp:122–123).
- seq=1, offset=144: `32/32=1 < 1` false → accepted (tightest valid case).
- seq=UINT64_MAX, offset=UINT64_MAX: `(2⁶⁴−1−112)/32 < 2⁶⁴−1` true → rejected, no overflow (test at line 124–125).

The bound is also physically sound: every frame occupies ≥32 bytes, so any offset representable by a real `off_t` file passes when honest. Decode revalidates through the same function (journal.cpp:151), so encode and decode paths share the fix.

### F3 (payload/batch consistency) — resolved, with one derived floor worth noting
`valid_limits` (journal.cpp:28–30): `max_payload >= 24 && max_batch_bytes >= 88 && max_payload <= max_batch_bytes - 88`. The `&&` ordering guarantees `max_batch_bytes - 88` never underflows. The 88 constant derives correctly: 32-byte commit header + 24-byte commit payload + 32-byte minimal source frame header, so a payload of `max_batch_bytes - 88` fits exactly in a one-frame batch plus commit (e.g. `{64, 512}` still passes, defaults `65536/1048576` pass, `2097152/1048576` rejects — tested at line 119–120). Consequence to be aware of, not a defect: the effective minimum batch is now 112 (since payload ≥ 24 forces batch ≥ 24+88); a profile declaring `{24, 88}` that previously encoded now rejects. That is the correct semantics — an 88-byte batch cannot hold a 24-byte payload — and no spec default or test case violates it.

### F4 (move-only NativeJournalFile) — resolved, ownership lifecycle verified
Header declares noexcept move ctor/assignment (journal_storage.hpp:29–30); definitions (journal_storage.cpp:69–79) use `std::exchange(other.descriptor_, -1)`, close the replaced descriptor in assignment, and guard self-move. The destructor now guards `descriptor_ != -1` (line 65), which is required for the moved-from state and was added. The test (journal_native_test.cpp:110–114) moves out of the heap object via reference cast, verifies the moved-from object rejects `extent()` (fstat(−1) → EBADF → io), and verifies the destination's exact write/extent. Double-close analysis: moved-from object holds −1, so `move_owner`'s eventual destruction closes nothing; `moved`'s destructor closes the real descriptor exactly once. No leak or double-close path. Spec line 122 "move-only" is now satisfied.

### F5 (new native oracle cases) — mostly resolved; one portability concern

Resolved:
- Truncated-file short read (lines 99–105): truncate to 2 with the fd open, `read_at` returns exactly 2, bytes 0–1 match the original content, bytes 2–7 verified untouched (0xAA fill) — this correctly pins the "count returned, no over-write" adapter contract that the future framing reader depends on.
- Symlink (lines 106–109): `open_existing("alias")` through a real symlink rejects with io and exact `ELOOP` detail, directly exercising the O_NOFOLLOW path.
- These complement the pre-existing request-8/file-4 short read (line 64).

Concern — read-only `flock` contention assertion (journal_native_test.cpp:94–95):
```cpp
auto read_only = require(directory.open_existing("journal", FileAccess::read_only));
error_is(read_only->lock_writer(), ErrorCode::busy);
```
`lock_writer` issues `flock(fd, LOCK_EX | LOCK_NB)` on an O_RDONLY descriptor. On Linux, flock performs no access-mode check, so this contends and returns EWOULDBLOCK → busy as asserted. On XNU/BSD-derived kernels (the declared Mac profile), `flock(LOCK_EX)` on a descriptor not opened for writing fails with EBADF *before* contention is evaluated, which `native_error` maps to io, not busy — the `error_is(..., busy)` check would fail on that profile. Trigger: running this fixture on macOS. Consequence: either the test fails on Mac (false regression signal in a contention oracle), or, if it currently passes because the owner's observed run says otherwise, the behavior is still undocumented platform divergence in the adapter contract for "exclusive lock on read-only descriptor." Fix options: (a) in the fixture, accept `busy`-or-`io/EBADF` for the read-only descriptor and keep the strict `busy` assertion on a read-write descriptor (which line 67–68 already covers); or (b) document in the subplan that `lock_writer` requires a writable descriptor and have `lock_writer` itself pre-check access mode for a uniform error. I cannot execute to confirm which branch occurs on the named Mac profile; this is a code-reasoned portability flag, not an observed failure.

### Introduced-interaction scan (clean)
- The new `valid_limits` inequality flows into `decode_journal_frame` (journal.cpp:194): a file whose header declares the now-rejected `{payload > batch−88}` limits would fail decode with invalid_range — but such a header can no longer be encoded or decoded by `decode_journal_header` either, so open rejects before any frame decode. Consistent.
- Header tests at journal_codec_test.cpp:118–125 exercise encode only; decode coverage for the new bounds is by shared `validate_header`, and every-offset damage loops still apply. Adequate at this layer.
- Move additions did not alter `JournalFile`/`Storage` virtuals; `unique_ptr<JournalFile>` polymorphic destruction is unchanged.
- No new allocation paths; all new branches are noexcept-safe arithmetic.

### Verdict
F1–F4 are correctly and minimally resolved with direct boundary tests. F5's added oracles are correct except the read-only `flock(LOCK_EX|LOCK_NB)` busy assertion, which is Linux-correct but platform-fragile on the declared Mac profile (expected EBADF→io instead of busy). That is the single actionable item from this pass. Absent downstream mechanisms (batch/recovery/continuation/issuer/emergency) remain out of scope and were not assessed.
