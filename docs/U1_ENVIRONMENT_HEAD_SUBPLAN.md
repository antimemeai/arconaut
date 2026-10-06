# U1 environment authority and head selection

2026-10-01. One storage component for linked continuation. The semantic ledger
remains the source of facts. Head metadata selects which journal chain to read;
it cannot grant permission to execute an attempt.

## Grounding

Existing native storage qualifies restricted local directories, no-follow regular
files, nonblocking flock, binary positional I/O and explicit file/directory sync.
The primary [renameat manual](https://man7.org/linux/man-pages/man2/rename.2.html)
describes atomic replacement and unchanged open descriptors. The
[fsync manual](https://man7.org/linux/man-pages/man2/fsync.2.html) distinguishes
file synchronization from persistence of its directory entry. These support
write-temp/sync/rename/directory-sync, not publication at rename alone. Check the
installed Apple SDK declarations and actual Mac/Linux operations. No library is
adopted. SIGKILL qualifies process crash, not power loss. The profile is a trusted
restricted local directory, not NFS or defense against operator-owned programs
modifying the files concurrently.

## Permanent authority and selector

`EnvironmentHead` owns a directory and the open, locked file `authority` for its
entire lifetime. This immutable file is the exact 112-byte encoded root journal
header, including environment/root/namespace/limits. Only a root header is legal.
Never rename/unlink/recreate it or lock the mutable selector instead. Open locks
before reading either file. Create uses exclusive creation; incomplete creation
is retained and ordinary reopen fails closed. Explicit repair is recovery work.

The `head` selector is exactly 72 bytes, field-by-field little-endian:
`ARCHEAD1` magic (8), environment (16), root journal (16), active journal (16),
generation u64 (8), reserved u32=0 (4), CRC32C over bytes 0..67 (4).
IDs/generation are nonzero. Generation 1 selects root; later generations select
non-root. Reject unknown sizes/magic/bits/CRC, zero fields, wrong environment/root.
Reject CRC-valid generation/active mismatches as corrupt too.
Missing/corrupt head never falls back to root or scans for a convenient branch.

Create selects root/generation 1. Replace requires the exact expected selector,
a different non-root active ID and checked generation+1. UINT64_MAX refuses
without I/O and leaves health intact. Expected equality means decoded fields;
there is exactly one legal encoding (zero reserved bits and determined CRC). The continuation owner
must validate predecessor boundaries, distinct namespace, complete recovery-choice
and provisional capture, and sync the new journal BEFORE replace. This primitive
cannot adjudicate those semantics. U2/U3 consumers will use the complete environment
owner, never use this primitive to activate unvalidated journal segments.

Before replacement, reread the existing selector and require it equal the last
acknowledged selection. Missing, corrupt or changed on-disk head closes switching;
never reconstruct it. Read errors also close switching until a successful reopen.
Busy applies to every ordinary open; a separate untrusted diagnostic reader is
not part of this component.

Encode the next selector and form the bounded temporary filename
`head.<32 lowercase hex journal-ID bytes>` before mutation. Create exclusively,
write all 72 bytes with checked short/EINTR I/O, sync at requested strength,
rename over head in the same directory, then sync the directory at that strength.
Only then publish the new in-memory selector. Existing temporary files are neither
overwritten nor removed; collision is a refusal before any selector write, and
continuation can choose a fresh unused journal identity. Preserve evidence files.

After the first attempted selector write, any error or exception closes further
replace operations on this owner. Old in-memory selection remains the last
acknowledged value, explicitly unavailable for normal activation. Never roll back
an uncertain rename or publish a competing branch. Fresh open under the permanent
lock reads the actual selector, syncs its file and directory, and returns that
identity. In particular, after successful rename followed by directory-sync
failure or SIGKILL, same-host reopen observes the new identity, even though the
previous owner never acknowledged it. The environment owner must still stage journal recovery and reconcile
actual custody before work. Create/open success includes authority-file and
directory sync. Failed sync returns no owner. Noncopyable/nonmovable single-owner
object returned by unique_ptr, with mutation/reentry guard. Selector FD never
holds the permanent lock. Retry interrupted lock/read/write/sync operations up
to eight consecutive interruptions, resetting after progress. Short reads/writes
accumulate until the exact known size; zero before completion is Incomplete and
impossible counts are corrupt. Failure is never interpreted as fallback.

## Native replacement seam

Extend JournalDirectory with replace_file(from,to), default Unsupported so old
fake directories do not silently claim atomic replacement. Native accepts different
validated filename components, checks source and existing destination are restricted
regular files without following symlinks (absent destination allowed), then uses
renameat on the same directory descriptor. No copy/unlink fallback. Return actual
errors; mandatory directory sync stays explicit in the owner.

## Direct oracles and sequence

1. Independent Kimi protocol review and resolve findings before implementation.
   Original review is retained in papers; explicit resolutions below.
2. Red API cases: independently specified fixed vector, all truncations/trailing
   bytes, bad magic/CRC/reserved, environment/root mismatch, generation rules and
   exhaustion. No unchecked read or length-driven allocation.
3. Scripted byte-file owner oracles check lock-before-read and exact I/O order:
   temp write/sync, rename, directory sync, cache publication. Wrong expectations
   and invalid IDs cause no I/O. Short/EINTR, zero/impossible counts and faults at
   each boundary preserve old acknowledged cache and close transitions. Reopen
   reads actual disk bytes. Pre-I/O allocation failure leaves health intact;
   preexisting temporary files remain untouched.
4. Native actual creation/open/replacement, binary inspection, permissions and
   symlink rejection. Another-process writer exclusion before AND after selector
   replacement. SIGKILL directly before/after replacement: reopen selects exactly
   old/new identity, never a manufactured root default. No power-loss claim.
5. Mac debug/release/ASan+UBSan and changed-target Neuroses checks. Independent
   Kimi code/oracle review, resolve and rereview findings, journal results.

## Remaining full-environment integration

Implement bounded selected-chain replay, exact predecessor boundaries, original
cross-journal source/parent identities and permanent non-dispatchability of recovered
attempts. Select a fresh OS-entropy namespace against all intact predecessor
headers; preserve unresolved RAM submissions as uncertainty, not invented admission.
Missing/corrupt selected segments stay closed; orphan files have no authority.
Concrete recovery-choice/provisional formats need reviewed refinement before coding.
Pending-byte diagnostics and preallocated emergency controls also remain. This
component alone neither accepts U1 nor enables U2 dependencies.

## Protocol review resolutions

Kimi findings 1/2/3/4/6/7/9 are clarified above: CRC-valid mismatches, exhaustion,
after-rename failure observation, bounded interrupted I/O, reader-busy semantics,
existing-selector check, and collision Conflict with health intact. Short reads
are normal positional I/O and accumulate; only premature zero is incomplete.
Finding 8 requests raw expected-byte CAS, but the API accepts a typed selector
with no reserved/CRC fields: these are checked by the decoder and deterministic
encoder. Comparing decoded fields is exact legal-value equality; alternate invalid
encodings cannot enter this API. No redundant raw-byte token is needed. Finding 5
is an oracle obligation, not a defect: journal_native already calls full-strength
directory synchronization directly; rerun it alongside the new actual native
replacement tests. Installed Xcode SDK sys/stdio.h declares renameat; sys/random.h
declares getentropy. No sync-strength fallback is introduced.

Creation must also refuse an already present head before writing its initial
selector, including a head with no authority file. After exclusive authority
creation/lock, probe head: only ENOENT permits initial publication; other errors
or an existing file fail closed. Preserve existing head and any newly created
partial authority. A preparation allocation failure before any selector write
preserves switching health; actual head-read errors/corruption close it.
Interrupted native open/create operations surface as transient Interrupted;
only lock/read/write/sync/extent calls use the bounded retry loop. Replace checks
expected versus cache before rereading disk. Same-inode rename is rejected by the
native seam rather than treating renameat's no-op as replacement.
