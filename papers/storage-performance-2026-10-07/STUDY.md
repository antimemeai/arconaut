# Lean durable state: storage and messaging systems study

2026-10-07. Operator asks to leave the AI-harness frame and study extreme
performance programs handling analogous persistence, history, recovery and memory.
This is source-grounded design input, not dependency adoption or a delivered
performance claim. Five immutable source archives acquired; existing LevelDB and
official Git format documentation also studied. No imported programs/tests run.

The useful target is structural: with current working state and recovery tail
held constant, adding archived history should barely affect reopen latency or
private memory. Faster full-history parsing cannot establish that property.

## What Blackbird actually does today

In candidate35413f3, `FramedJournal::open` scans the entire journal. `scan` reads
frame payloads and validates frame/batch checksums and commit boundaries.
`RetainedState::open` then calls `rebuild`/`replay`, reading semantic payloads again,
decoding them, checking identities/dependencies/transitions, and appending decoded
events to `Snapshot::facts`. `Decoder::blob` allocates and copies blob bytes.
`ApplicationRecordEvent::payload` owns those bytes through `ImmutableBytes`.
`ContextStore` walks every retained context application record, validates JSON and
publication linkage, and retains shared payload references in `history_`.

Projection avoids constructing unused candidate JSON trees, not loading their
underlying bytes. Thus the335,783,054-byte fixture, including335,544,320 bytes of
rejected proposal contents, still produces354,304,000-byte median process peak RSS.
No precise heap attribution was performed; the identifiable retained payloads
dominate, while shared context references do not create another whole byte copy.

Matched warm releaseOFF medians: replay160.891ms; context restoration19.705ms;
usable prompt180.675ms; engine/request admission85.965ms; cumulative audited
readiness265.961ms. Live provider input37bytes. This is neither cold startup nor
large-live-context evidence. The allocator candidate's interaction is unmeasured.

Current API `committed_facts()` exposes an in-memory span of all decoded history.
Changing only its storage allocator will preserve the wrong startup behavior.
Consumers requiring session settings, live context, unresolved effects or one old
record need corresponding queries, not a resident reconstruction of everything.

## Reference mechanisms, inspected in code

| Program | Actual mechanism | Transfer to Blackbird | Limit we must respect |
| --- | --- | --- | --- |
| LMDB | Copy-on-write database roots, alternating meta pages; borrowed mapped values | Durable root publication; stable views instead of allocating every old value | Mapping does not bound touched pages; reader lifetime constrains reclamation |
| SQLite WAL | Persistent current database plus recoverable WAL and transient frame index | Separate current state, recovery suffix and derived lookup structures | It still scans the surviving WAL on recovery; checkpointing controls that work |
| TigerBeetle | Superblock root, checksummed block references, checkpoint plus WAL suffix, configured cache | Explicit memory/I/O budgets; references bind address and contents; bounded recovery | Multi-GiB replica sizing and distributed repair are unsuitable to import wholesale |
| Aeron Cluster | Snapshots bound to a log position; replay begins at that position | Restore current state, replay only later transitions; archive remains separately addressable | Snapshot/log agreement and complete service state matter; language/runtime not selected |
| Bitcask | Append-only immutable files; key→file/offset/size metadata; checked hint files | Historical values stay on disk; exact references and sequential append | Resident keydir scales with key count; merely shrinking values leaves a history-sized index |
| Git commit graph | Binary chunk directory, fixed-width records and positional references | Compact paged history metadata without per-entry object graphs | Index remains proportional to history on disk; checksum existence does not imply full startup verification |

**LMDB.** At pinned700e10f91a65, `mdb.c`1354–1386 defines two meta pages with
transaction IDs and database roots. `mdb_env_pick_meta`5012 selects a root by
transaction sequence. `mdb_env_write_meta` publishes after data handling, with
platform-specific durable writes; weak sync flags change the guarantees. `intro.doc`
describes values borrowed from the mapping and invalidated by transaction end.
The lesson is small root selection and explicit borrow lifetime, not a claim that
two unchecked pointers solve arbitrary corruption. [Source](https://github.com/LMDB/lmdb/blob/700e10f91a65fae69520301926fb9819f16d292f/libraries/liblmdb/mdb.c),
[borrow lifetime](https://github.com/LMDB/lmdb/blob/700e10f91a65fae69520301926fb9819f16d292f/libraries/liblmdb/intro.doc).

**SQLite.** At5af1b822f5da, `src/wal.c:1422` rebuilds the WAL index using a frame
buffer and checked frame scan; `sqlite3WalReadFrame:3740` reads a selected frame
by computed offset. `walCheckpoint:2281` observes reader boundaries, synchronizes
WAL before transferring pages, and synchronizes the database before completing
backfill. The transient index is not durable authority. It recovers the outstanding
WAL, not every historical transaction ever committed. We retain audit history,
so their WAL recycling cannot be copied literally. [Source](https://github.com/sqlite/sqlite/blob/5af1b822f5da6f4ea72ead5060e4bc365ccbf209/src/wal.c),
[recovery format](https://www.sqlite.org/walformat.html),
[checkpoint behavior](https://www.sqlite.org/wal.html).

**TigerBeetle.** Atc95d7a53a3d0, `src/vsr/superblock.zig` stages a sequence/parent
linked root, writes its copies, and selects a compatible quorum on open. Grid
references pair address and checksum; grid reads can verify a block without first
reading all others. `src/vsr/grid.zig:231` allocates configured cache/stash capacity.
Do not equate bounded allocation with small allocation: published hardware guidance
requires at least6GiB per replica. Our useful lessons are publication ordering,
content-bound references and explicitly sized working buffers, not its whole
replicated storage architecture or redundant-copy count. [Layout](https://github.com/tigerbeetle/tigerbeetle/blob/c95d7a53a3d044741409b03b2f334a8ed033fa3c/docs/internals/data_file.md),
[root code](https://github.com/tigerbeetle/tigerbeetle/blob/c95d7a53a3d044741409b03b2f334a8ed033fa3c/src/vsr/superblock.zig),
[hardware scope](https://docs.tigerbeetle.com/operating/hardware/).

**Aeron.** Atad4baf8dcd5a, `RecordingLog.java:1694` chooses matching snapshots
and the recorded log extent. At1763 replay starts from the chosen snapshot's log
position, otherwise the recording start. `addSnapshots` checks service/term/position
agreement. Thus state restoration and history retention are separate paths. We
study this mechanism despite its Java implementation; this selects no JVM runtime.
[Recovery code](https://github.com/aeron-io/aeron/blob/ad4baf8dcd5aed31e5a60a89940748e9408baa12/aeron-cluster/src/main/java/io/aeron/cluster/RecordingLog.java).

**Bitcask.** The2010 paper specifies one append writer, immutable closed files,
and a keydir storing location metadata. At d84c8d913713,
`c_src/bitcask_nifs.c:140` stores file ID/size/offset and key metadata, not value
contents. `src/bitcask_fileops.erl:405` distinguishes normal and recovery hint
loading; recovery checks the hint CRC and falls back to data scanning on invalidity.
For our mostly unique audit IDs, retaining every key in RAM would still scale
with history. Borrow disk references and hint validation, not that resident keydir.
[Paper](https://github.com/basho/bitcask/blob/d84c8d913713da8f02403431217405f84ee1ba22/doc/bitcask-intro.pdf),
[hint recovery](https://github.com/basho/bitcask/blob/d84c8d913713da8f02403431217405f84ee1ba22/src/bitcask_fileops.erl).

**Git/LevelDB.** Git's documented commit-graph format uses a chunk-offset directory,
sorted identifiers and positional parent references; it offers a concrete alternative
to thousands of individually allocated metadata objects. Existing pinned LevelDB
7ee830d02b62 `VersionSet::Recover`861 reads CURRENT and reconstructs manifest
metadata; it does not deserialize every value in every table. These reinforce the
separation of compact metadata from historical value storage. Neither by itself
provides constant reopen time for unbounded metadata. [Git format](https://git-scm.com/docs/commit-graph-format),
[LevelDB source](https://github.com/google/leveldb/blob/7ee830d02b623e8ffe0b95d59a74db1e58da04c5/db/version_set.cc).

## Proposed architecture: working state, recovery tail, archive

This is our synthesis/inference from those mechanisms, not an adopted library or
a promise of timing. Preserve complete history while changing how we consume it.

1. **Working state** is a compact durable root at an exact committed journal
   boundary. It contains current session/program generations and context revision,
   live item references/order, issuer high-water marks, unresolved operations and
   required reconciliation fences. Historical originals and settled operations are
   located through disk indexes. Store bytes once; roots reference them.
2. **Recovery tail** contains only committed transitions after that boundary plus
   any uncertain suffix. Reconstruct admission fences before allowing effects.
   Bound tail growth by bytes/operations through incremental checkpoint work during
   normal operation. A time-only checkpoint timer cannot bound a high-volume tail.
3. **Archive** stores immutable audit segments, exact original payloads and paged
   location indexes. Rejected edits remain inspectable without joining startup's
   working set. Indexed history/repair fetch only the requested ranges.

Normal reopen selects a published root, validates its required state sections,
replays its tail, restores the visible transcript window, then enables work when
custody/recovery permits. Provider-request assembly materializes only live input
when needed; its cost remains measured separately. Full forensic replay exists
as an explicit path and as fallback for an unusable root, with visible progress.

Memory scales with live working state, unresolved work and explicitly bounded
read/index/render buffers. Older payload volume should have essentially zero
effect on private heap. Growing history still consumes disk; fixed-width indexes
must be paged or demand-read rather than eagerly loaded into an O(history) map.
An enormous live request has unavoidable byte traffic; do not promise constant
time while pretending its serialization/network bytes disappeared.

### Publication and integrity are part of this same conceptual unit

- Root version, environment/session/journal identity, generation and exact committed
  sequence/offset must agree. Referenced state sections need size bounds and checksums;
  distinguish actual state bytes from historical locator entries.
- Write referenced data and make it durable before publishing its root. Retain
  an earlier valid generation until publication settles. Define interruption at
  every write/sync/publication boundary; rename alone is not a durability protocol.
- Bind the root to its checkpoint record/boundary. Recover later reservations and
  unresolved admissions; a stale root may neither reuse an ID nor erase an effect.
- Never infer that a missing child/provider process means an admitted action had
  no effect. Checkpoint loading preserves uncertainty and real custody reconciliation.
- A checksum protects the bytes it actually covers; it does not prove that a
  state reduction is semantically correct. Direct comparison against authoritative
  replay on generated histories supplies the reduction oracle.
- A fast reopen cannot simultaneously re-read and verify every archived byte.
  It trusts correctly published state under the declared storage fault model,
  verifies historical blocks when accessed, and offers explicit complete verification.
  Root hashes do not magically detect every latent change to an untouched archive.
  Crash recovery, accidental corruption and hostile replacement are different claims;
  CRC alone is not authentication. The exact guarantees remain a design decision.
- Corrupt/missing root or index takes an explicit safe rebuild/fallback path.
  An invalid index never changes authoritative state. Existing archives remain
  readable; conversion must not silently mutate operator history.

Do not transplant TigerBeetle's four-copy quorum or manufacture a multi-file database
framework before selecting the fault model. Start with the smallest publication
protocol satisfying our defined local single-writer semantics.

### Access and ownership

Start with offset/length references and bounded `pread` buffers for fallible archival
reads. Study/measure read-only mappings for sealed segments and compact indexes,
where they remove real copies. Do not map a mutable/truncatable file and pretend
its pointer remains valid across recovery/refit. Mapping lifetime, truncation,
page faults and borrowed-view invalidation need explicit rules. Rhizome's
`papers/cpp-rigor-stack.md` reinforces checked offset arithmetic, endian decoding,
legal C++ object lifetime, immutable publication and bounded borrowing.

Remove whole-prefix metadata copies on ordinary appends: stage changed/new entries
and publish the delta. Keep lookup tables for hot state, not all payload-bearing
facts. Stream capture should append bounded blocks and share one input/blob between
decision/invocation/admission links where their semantics permit it. The existing
durable ID ranges are the correct example of batching without weakened acknowledgement.
Do not defer required pre-effect admission durability to an eventual flush timer.

## Direct experiment and acceptance plan

These are proposed budgets, not measured achievements. Preserve the operator's
<100ms startup goal; report distributions and exact boundary, not a banner or best run.

| Question | Matched experiment / oracle |
| --- | --- |
| Does archive volume stop governing startup? | Same live state and tail;1MiB,336MiB, then multi-GiB archives. Count bytes actually read and allocations, plot startup/RSS versus archive size. Keep fixture size bounded on laptop. |
| Is large live state handled honestly? | Independently vary live context, original count, unresolved operations and visible transcript size. Report request serialization separately. |
| Does tail growth remain bounded? | Vary post-checkpoint bytes/records, including stalled checkpoint/error path; record writer backpressure and recovery cost. No hidden automatic budget reset. |
| Is the restored state identical? | Generated event histories; compare checkpoint+tail to full replay for current state, IDs, unresolved effects and exact historical reads. Check rejected/no-op edits explicitly. |
| Can publication crash safely? | Deterministic storage faults/torn writes at each root/data sync; old or new published state, never mixtures; no ID reuse or blind effect retry. |
| Are history reads trustworthy? | Corrupt/truncated/wrong-session/missing index blocks and archives; exact-read checksum failures and safe fallback, no out-of-range allocation. |
| Is the gain just cache or accounting? | Interleaved warm matched runs; separately identify actual cold/first-open conditions. Report CPU, wall, read bytes, syncs, allocated/copied bytes, peak private memory, mapped RSS and faults. |

Operator clarification: memory may be large when it buys useful capability or
measured speed. There is no arbitrary total-RSS ceiling or byte-minimalism target.
Each substantial allocation needs an intentional purpose, lifetime and benefit;
archived payload volume alone must not force residency. Measure useful caches and
live-state costs, and compare benefit against their memory and initialization work.
A mapped archive's address-space size is not
RSS; kernel page cache still consumes machine memory. Linux smaps separates anonymous,
file-backed and shared residency; macOS needs corresponding VM/resource observations.
[Kernel accounting](https://docs.kernel.org/filesystems/proc.html).

Do not add production profiling to meet these experiments: normal DEBUG=OFF,
explicit development builds/counters and finite external capture only. ReleaseOFF
matched latency is the comparison; instrumented byte counters establish actual work.
No imported benchmark execution, new dependency, repeated certification or mutants.

## Concrete course from here

The useful next product unit is **compact current-state recovery with historical
payloads addressed on disk**. Root publication, suffix recovery and hot-state
restoration belong to one owner/invariant; do not split writer and reader among
uncoordinated lanes. Define its complete state schema and fault model before coding.
First integrate/measure the already delivered allocator+projection changes as a
baseline, without another tribunal. That baseline is not the architecture's endpoint.

Reuse issues arconaut-n11 (startup) and arconaut-3x2 (state/history copying/indexing);
arconaut-1v0 (one input original) and arconaut-02g (capture batching) remain separate
useful units. One scoped review and direct findings recheck per actual changed unit,
finite25-minute implementation slices with honest incomplete checkpoints. This
study itself added no implementation or autodev launch.

Acquisition identities/restoration: [references.json](references.json),
[documents.json](documents.json). Source archives intact in shared quarantine_proj;
extracted Git metadata/detritus omitted. PDFs ignored, not committed.
