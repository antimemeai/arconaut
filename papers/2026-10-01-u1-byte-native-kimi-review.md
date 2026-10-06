# U1 byte/native independent code review

Unchanged assistant replies; read-only reasoning, no reviewer execution. This is an early component review, not complete U1 acceptance.

## Independent review — U1 byte-format codec and native storage adapter

Scope inspected (read-only, no execution): `docs/U1_RETAINED_STATE_SUBPLAN.md`, `include/arconaut/journal.hpp`, `include/arconaut/journal_storage.hpp`, `include/arconaut/foundation.hpp`, `src/journal.cpp`, `src/journal_storage.cpp`, `tests/journal_codec_test.cpp`, `tests/journal_native_test.cpp`. All conclusions below are from code reasoning, not from owner-observed runs.

### What checks out (verified against spec, not just round trips)

- CRC32C: reflected poly `0x82f63b78`, init/final XOR, and incremental chaining `crc32c(payload, crc32c(header))` is algebraically correct for the xor-in/xor-out form (src/journal.cpp:64-73, 183, 217). Tests use independent vectors (0xe3069283, RFC 3720 B.4 set) and a fixed expected frame CRC `0x9debf28c`, not encoder/decoder round trips (tests/journal_codec_test.cpp:41-54, 129-131).
- Header layout, offsets, endianness, reserved-zero enforcement, flags ∈ {1,2} exact-match rejection, root-predecessor-must-be-zero, CRC-over-108, nonzero identity enforcement, and "truncated/invalid never becomes fresh journal" all conform (src/journal.cpp:75-152; spec lines 41-52). Damage loops cover every byte offset and every truncation length.
- Frame header layout, kind/version rejection, `length > max_payload` → capacity before any sizing, `length > available` → incomplete, `sequence/batch_first` nonzero and ordering checks, no allocation on decode, borrowed-payload lifetime documented (src/journal.cpp:190-227; include/arconaut/journal.hpp:44).
- Native: off_t/ssize_t bound check before syscalls (journal_storage.cpp:22-27), O_NOFOLLOW|O_CLOEXEC|O_NONBLOCK + openat-under-directory-fd, component validation (no `/`, NUL, `.`, `..`), fstat regular-file/dir + euid + `&0077==0` restriction, exclusive create → conflict, EWOULDBLOCK → busy, ENOTSUP → unsupported, EINTR → interrupted, errno preserved in detail, no close-retry after EINTR, directory move semantics with exchange(-1) and self-move guard are all sound. Mac full sync is F_FULLFSYNC with no fsync fallback (journal_storage.cpp:28-44), matching spec line 130. The fork contention fixture correctly uses an independent open in the child (not the inherited OFD) and async-safe `_exit` codes (tests/journal_native_test.cpp:69-91).

### Findings

**1. `create_exclusive` performs no durability step before success — possible spec nonconformance.**
Subplan line 128-129, in the native-storage paragraph: "Creation syncs the file and its parent directory before success." `open_file` (src/journal_storage.cpp:152-185) opens, fstats, and returns; neither `synchronize` on the new descriptor nor `synchronize_directory` is invoked. Trigger: crash/power-loss between `create_exclusive` success and a later orchestrated sync; on some filesystems the directory entry itself can be lost, so an "exclusively created" journal can vanish even though creation reported success. Consequence: either the native layer violates the stated contract, or the sentence silently delegates durability to the (not yet implemented) continuation writer — the subplan's continuation paragraph (lines 110-112) does describe a directory sync, so this may be intentional layering, but then the native paragraph sentence is misleading. Fix: either fsync the new file + directory fd inside `create_exclusive`/`open_file` on the create path, or amend the spec sentence to place creation durability in the writer. Flag for owner decision; I treat it as a conformance ambiguity, not proven data loss on a claimed profile.

**2. Asymmetric predecessor consistency check admits an impossible header.**
src/journal.cpp:42-50 rejects `validated_sequence == 0 && validated_end_offset != 112`, but not the mirror case `validated_sequence != 0 && validated_end_offset == 112`. Direct case: `JournalHeader{env, journal, ns, limits, JournalPredecessor{prior, 5, 112}}` — a validated prefix of 5 frames cannot end at offset 112 (the header alone occupies 112 bytes; minimum frame is 32). `encode_journal_header` accepts it, and `decode_journal_header` re-validates via the same `validate_header` (line 148), so the impossible header round-trips and would enter any future chain validation as "intact predecessor." A tighter invariant available at this layer is `validated_end_offset >= 112 + 32 * validated_sequence` (checked arithmetic). Consequence: a corrupt-but-CRC-repaired or erroneously constructed continuation header passes structural validation, weakening the "invalid predecessor headers block continuation" guarantee (spec line 114-116). Fix: extend `validate_header` with the mirror case and optionally the minimum-extent bound.

**3. `valid_limits` does not relate `max_payload` to `max_batch_bytes`.**
src/journal.cpp:28-30 enforces only lower bounds (≥24, ≥88). A header declaring `max_payload = 2 MiB, max_batch_bytes = 1 MiB` encodes and decodes cleanly, and `encode_journal_frame` then permits a single frame whose payload alone exceeds the declared batch ceiling (spec line 26: "1 MiB encoded batch" as a bound; line 44-45 header carries both). Trigger: any profile configuring inconsistent limits (codec accepts them, so a peer file can declare them). Consequence: the batch bound cannot be enforced at frame granularity later without special-casing. Fix: require `max_payload <= max_batch_bytes` in `valid_limits`.

**4. `NativeJournalFile` is neither copyable nor movable, though the spec says move-only.**
include/arconaut/journal_storage.hpp:24-39 declares a destructor and deleted copy operations, which suppresses implicit move; subplan line 120 specifies "move-only `NativeJournalFile : Storage`". No functional break today because it is always held via `unique_ptr<JournalFile>`, but the type does not satisfy the stated contract and cannot be returned/moved by value. Fix: add defaulted move ctor/assignment (with descriptor exchange to keep single-ownership close semantics).

**5. Oracle gaps (minor, within the reviewed test scope).**
- Symlink rejection (O_NOFOLLOW → ELOOP → io) is implemented (journal_storage.cpp:121, 162) but has no test case; spec line 132 commits to exercising real filesystem errors, and a symlinked journal path is the canonical attack this flag defends.
- `lock_writer` on a `read_only` descriptor, and `read_at` short-read/partial-count behavior against a truncated file, are untested; the short-read contract matters because the (future) append/read loops rely on adapter counts.
- The codec tests are otherwise strong (every-length cuts, every-offset damage, fixed CRC oracles, concatenated-stream consumption at tests/journal_codec_test.cpp:157-160).

### Non-findings considered and rejected

- Returning `interrupted`/short counts instead of retrying inside `read_at`/`write_at`/`synchronize` is correct layering: spec line 99 assigns EINTR/short-write looping to the framing writer; retrying fsync after EINTR would be unsafe.
- Linux ignoring `SyncStrength` (fsync for both) matches spec line 131.
- O_NONBLOCK left on the descriptor is inert for regular files and is the correct FIFO guard.
- `decode_journal_header` rejecting buffers >112 bytes (journal.cpp:109-111) is a strict but coherent exact-slice contract; the future reader can pass exactly 112.
- `flock` lock released implicitly by descriptor close (including on process death) is the intended advisory-lock lifetime; the same-process second-descriptor contention test is valid on both declared profiles.

Deliberately absent downstream mechanisms (batch commit/recovery/replay, continuation orchestration, issuer ledger, emergency capture) were not assessed as defects, per the review brief.
