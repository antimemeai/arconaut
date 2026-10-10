# Raw recorder: append, durable acknowledgement and recovery

2026-10-10. Research and proposed design, not implementation approval. This
report owns the separate raw disk recorder and its source-record reclamation
rule. Transport selection belongs to the companion study. No acquired code was
built or executed, no production source was changed, and no dependency is
proposed. Source checkpoints and restoration are below.

The useful primary reference is **djb's daemontools process/logging split**, read
end to end rather than inferred from the multilog interface. Its durability is
insufficient for this requirement. **Cedar group commit** explains sequential
batching and explicitly exposes the loss it permits. The existing Blackbird
journal provides a stronger owned physical starting point; SQLite and LevelDB
are comparison code for commit ordering, partial writes and failed flushes,
not metric architectures or proposed dependencies.

## Complete daemontools path and its limits

Acquired the intact official
[daemontools 0.76 archive](https://cr.yp.to/daemontools/daemontools-0.76.tar.gz)
and read `svscan.c`, `supervise.c`, `multilog.c`, `buffer.c`, `buffer_get.c`,
`buffer_put.c` and descriptor/locking helpers. Version0.76 uses the `buffer`
API, not the older `substdio` spelling. This report does not claim to have read
an absent `substdio` implementation. The code's old-fashioned C style and small
interfaces matter more than its 2001 release date.

`svscan.start` finds a service's `log/` directory, creates a pipe once for that
service and retains the descriptors. It starts separate `supervise` processes:
the service gets the write descriptor as stdout; the logger gets the read
descriptor as stdin. `supervise.trystart` forks/execs the run program, and its
event loop restarts it after death while the desired state remains up. Thus
restart management and pipe lifetime live outside both measured process and
logger. See the author's [service/log setup](https://cr.yp.to/daemontools/faq/create.html).

`multilog` reads through a 1024-byte input buffer, assembles a prefix of a line
and sends selected bytes to a 512-byte output buffer per destination. `c_write`
handles positive short writes and retries zero/error writes after reporting to
stderr and sleeping. `flushread` flushes output before acquiring another input
block. Rotation and orderly exit call `fsync`. These are separate boundaries:
flushing a userspace buffer is a write, not a durable flush. The complete
[author interface](https://cr.yp.to/daemontools/multilog.html) distinguishes
safely finished files from an interrupted current file.

What transfers: separate recorder process, retained transport lifetime outside
the logger, one disk writer, sequential byte delivery, retry rather than drop,
and upstream backpressure when the recorder cannot progress.

What does **not** transfer:

- A logger killed after `read` has sole ownership of read-ahead bytes in its
  address space. Restart cannot recover those bytes from the already-drained
  pipe. Its output buffers are also volatile. There is no durable per-record
  acknowledgement allowing the source to retain or replay them.
- Current-file writes are not synced per delivered prefix. OS/power failure can
  lose pipe contents and unsynced current-file bytes; orderly shutdown is not a
  crash protocol. Rotation has link/rename/unlink work without an explicit
  directory-fsync protocol in the source studied.
- Final-newline insertion, timestamp prefixes, filters, status truncation and
  replacement of a raw file by processor output alter or discard input. Its
  old-file pruning deliberately deletes history. None belongs in this recorder.
- Its synchronous processor can block raw recording. Tertiary processing must
  instead read an already durable audit prefix independently.

This is a good process reference and an unsuitable complete losslessness claim.

## What group commit buys, and what it does not

Read Hagmann's *Reimplementing the Cedar File System Using Logging and Group
Commit* (SOSP1987), especially5.3–5.5 and5.9, in the
[acquired paper](https://www.seltzer.com/margo/teaching/CS508.19/papers/hagmann87.pdf).
It batches updates into sequential log writes and distinguishes pages logged
from pages safe to reuse. It also allows roughly half a second of recent
metadata updates to disappear in a crash; callers can force the log. That
allowance is explicit and is not imported.

Our recorder can batch **unchanged records** to amortize writes/flushes. Proposed
initial policy: take a finite complete ready prefix, append it and its commit
frame, then flush immediately. Do not wait for a batch to fill, and do not add a
periodic half-second durability timer. Under load, events naturally join a
batch; under light load, a batch can contain one event. This is a proposed simple
policy, not a measured optimum. A bounded batch prevents an endlessly advancing
producer from postponing acknowledgement forever.

Group commit makes one flush cover many records. It does not make earlier
enqueue, read or `write` success mean durable storage.

## Existing Blackbird audit: real machinery, real gaps

Studied source at `e75d57c4bb0beebba0698b3e12b590e9a6de057b`:
[journal format](../include/blackbird/journal.hpp),
[encoder](../src/journal.cpp), [writer](../src/journal_writer.cpp),
[native storage](../src/journal_storage.cpp),
[retained events](../include/blackbird/retained_events.hpp) and
[environment](../src/retained_environment.cpp).

Current physical format has a112-byte stream header,32-byte frame headers and
a56-byte commit frame (32-byte header plus24-byte commit payload). The stream
header includes environment, journal identity, issuer namespace, limits and an
optional validated predecessor. Each frame carries version, kind, physical
sequence, first sequence of its batch, payload length and CRC32C. The commit
payload contains batch-first sequence, record count and checksum of the batch.
Raw payloads are preserved in the framing.

`FramedJournal.append` prepares storage/index allocations before writing,
requires actual extent to match its acknowledged cursor, loops on positive
short writes and retries `EINTR`, writes the complete batch and calls storage
sync before publishing records/cursor. Zero progress is an error. A partial
write or sync failure poisons the writer. It does not pretend the append failed
without possible disk effects.

Recovery scans bounded frames, checks sequence/batch/checksums, selects only
complete committed batches and leaves incomplete/rejected tails explicit.
`confirm_recovery` syncs file and directory before publication; a damaged tail
remains blocked. The predecessor mechanism can preserve the old damaged file
while a successor continues from an explicitly selected prefix.

These mechanisms are useful, but **the existing object is not the proposed
recorder already implemented**:

- It allocates and copies a frame and complete encoded batch. All of that must
  stay out of Blackbird's producer path. The recorder may initially use one
  reusable preallocated batch buffer; any further copy elimination needs an
  actual cost measurement.
- Bounded resident indexes/file size can refuse append. The recorder needs
  prepared segment rollover and a storage-failure stop; it cannot discard raw
  events when an existing application capacity is reached.
- `RetainedState` knows source and semantic/application records. Arbitrary raw
  metrics cannot simply be shoved into its semantic replay loop or disguised as
  existing application packets. The raw stream must be a registered member of
  the session's audit storage, with a raw record kind/reader, not a competing
  unregistered metrics log.
- A second process cannot write the current selected semantic file while its
  existing owner holds its writer lease. The proposed recorder owns a dedicated
  raw **audit stream**, not a second writer on that file. Moving every existing
  semantic audit write into this process is separate scope.

Registration must durably preserve stream/site descriptions before producers
emit against them. Audit readers must discover the stream from the session
audit set even if the producer dies before its next semantic checkpoint. A
semantic checkpoint referring to a metric record cannot establish preservation
by itself. Implementing this discovery and frame-kind extension needs explicit
design under the incomplete-audit issue; existing cross-journal source handles
are not automatically valid semantic dependencies on a raw stream.

## Concrete proposed recorder protocol

Transport must retain complete published source records until their disk
acknowledgement, in storage that survives producer and recorder process death
while the OS remains alive. A supervisor-owned shared mapping can do that.
A pipe with a producer-local retained copy survives recorder death but not
producer death; a pipe alone does not meet this rule. This report does not
select between the companion transport designs.

Each transport stream has a never-reused stream epoch and monotonic source
sequence; each record carries its complete raw envelope and bytes. Source
sequence is transport identity, not an aggregate metric or inferred total order.
Physical journal sequence is separate. Source records from different streams
may arrive in any order; the recorder must not invent an execution order from
its append order.

1. At setup, create/lease the recorder's stream, register it in audit storage,
   preserve decoding/site metadata and sync names/headers. Prepare rollover
   outside emission. Producer publication uses only an already provisioned
   channel; setup failure stops startup before sources acquire unrecordable work.
2. The recorder snapshots complete published prefixes. It checks only transport
   bounds, source epoch/sequence and publication completeness, not metric values.
   An unpublished partial slot remains the source owner's slot.
3. Copy the original source envelope/payload bytes unchanged into raw audit
   frames. Add physical framing and a commit frame in the recorder. CRC is
   **integrity framing computation**, not a measurement transform: producer
   computes no CRC; metric payload is never parsed, normalized, reformatted,
   compressed, filtered or aggregated. This specific pre-preservation integrity
   work is stated openly, not smuggled into a metric processing abstraction.
4. Write at the known next file offset. Handle short positive progress by moving
   only the output offset; retry interrupted calls. Do not restart a partial
   append from the old offset without recovery, and never acknowledge a zero,
   short, refused or errored append as complete.
5. After writing the batch and commit frame, complete the qualified native file
   flush. Complete required namespace/catalog flushes for newly created files.
   No durable acknowledgement precedes either applicable flush.
6. Publish the new disk cursor and each source stream's largest **contiguous**
   durable sequence. An acknowledgement carries the source epoch so a stale
   recorder or replaced channel cannot free another channel's storage.
   Producers may reuse only records at or below that acknowledgement. Recorder
   read position and page-cache append position never authorize reuse.
7. Independently notify tertiary readers of that durable prefix. They do not run
   inside the recorder and never own source-slot acknowledgement. Slow/crashed
   readers retain a disk cursor and catch up later. Disk retention remains
   explicit; the recorder does not delete raw data to maintain a short window.

**Reclamation invariant:** once a source record is published, either its exact
bytes remain in retained transport storage or a checked complete disk batch
containing those bytes has completed the durability/namespace protocol. There
may temporarily be both copies. There is never deliberately neither.

## Restart, ambiguous success and partial tails

The old recorder must be dead and its writer lease released before a replacement
acts. An acknowledgement is not a durable state journal of its own; reconstruct
the source watermarks from the audit prefix. The disk records carry the epoch
and source sequence needed for this reconstruction.

Restart scans the selected physical chain and validates complete batches without
transforming the raw measurements. It flushes the checked prefix again before
announcing new durable watermarks. A batch written/synced just before death may
be present although its acknowledgement never reached the source. Match its
source identities against still-retained records; do not append a duplicate.
If the same identity names different bytes, stop and preserve both inputs for
diagnosis. Source sequence alone is not permission to discard conflicting data.

If the final batch is incomplete or corrupt, preserve that file and its bad tail.
Select the last valid committed boundary; durably create/register a successor
whose predecessor names that boundary. Resume unacknowledged raw records from
retained transport in the successor. Do not silently skip an interior corrupt
frame, truncate original evidence or advance acknowledgement over a hole. If an
already acknowledged prefix is damaged, this is a storage integrity failure,
not a recoverable ordinary partial tail; source storage may already be reused.

If only a suffix of published source records survived OS failure, a later record
does not make the missing one recorded. Recovery reports a missing source range
when the setup/recovery metadata makes it knowable. It cannot reconstruct the
missing raw values from a counter or invented placeholder.

## Full disk and failed ingestion

`ENOSPC`, quota exhaustion, read-only storage, I/O failure, unsupported required
flush or corrupt prefix stops durable acknowledgement. Keep all outstanding
source slots and pause admission before more work needs slots. Never overwrite,
sample, aggregate or substitute an error event for the original record.
Preallocation reduces ordinary allocation failure; it does not cure a failed
device. No finite queue promises continued progress during an indefinite stall.

Reserve a separate control-event slot before each fallible ingest, write or sync
attempt. A failure slot is never reused or overwritten before its own durable
acknowledgement. On failed ingestion, fill that slot with the original stream/sequence,
failed stage, raw error code and observed progress. A supervisor retains it;
it bypasses the failing ordinary-data enqueue path. On append/flush failure,
the recorder similarly uses its preallocated supervisor control slot, not its
poisoned journal object and not a call back into normal instrumentation.

The control event is written to the registered audit stream when a usable
recorder/storage path is available. A preallocated audit emergency region can
make ordinary full-disk failure easier to report but cannot promise disk writes
through an actual device failure. If every disk path has failed, the event is
retained in memory and the failure remains visible; it is **not claimed to have
been preserved on disk**. Immediate durable reporting of total storage failure
requires an independent functioning storage destination, which is additional
scope. Do not introduce a recursive metrics-about-metrics log or pretend the
same failed disk becomes reliable when the payload is an error.

For repeated failures, every attempted ingest needs an available control slot.
Once none is available, stop attempting/admitting more work. Do not collapse
multiple failed attempts into one summary event. Retry attempts remain raw facts;
a retry cannot silently clear the original failure record.

## Exact crash claims

| Boundary/fault | What is preserved under this proposal |
| --- | --- |
| Producer dies before publication | Incomplete reservation is detectable; fields never published cannot be claimed as an acquired complete event |
| Producer dies after publication | Supervisor-owned complete source records remain for recorder drain while the OS survives |
| Recorder dies after drain, before flush | Source records have not been reclaimed; replacement replays them |
| Recorder dies after flush, before ack | Disk prefix supplies reconstructed ack; retained source slots supply byte comparison |
| OS/power loss before durable ack | Volatile pending records can disappear; no ordinary RAM/pipe/shared-memory design can claim all acquired events survived |
| OS/power loss after durable ack | Acked raw prefix survives subject to the specified OS/filesystem/device flush contract and namespace protocol |
| Actual media destruction or dishonest flush completion | A single local recorder cannot guarantee survival; independent durable storage is required for that fault class |

Thus this proposal supplies lossless live transfer and process-crash retention,
plus a precise durable disk boundary. It does **not** quietly redefine a RAM
publication as disk preservation. If the operator requires every acquired event
to survive host power loss before the recorder flush, the minimum architecture
must add persistent source storage or await durable acknowledgement before
calling acquisition complete. A durable fence at existing audited-effect
boundaries protects those effects, but does not magically preserve every
intermediate metric fact before the fence. This is a real specification choice,
not an implementation optimization to make without discussion.

## Native disk contracts and comparison code

Linux: use the existing `fsync` path first. `fdatasync` can flush required file
size metadata too, but adopting it is an optimization to measure. New file names
require parent-directory synchronization; file sync alone is insufficient.
Failures can be reported at flush after earlier writes succeeded. See the
[Linux syscall documentation](https://www.man7.org/linux/man-pages/man2/fsync.2.html).
The [kernel cache-control description](https://docs.kernel.org/block/writeback_cache_control.html)
explains that ordinary device completion can mean volatile cache and how flush/
FUA requests establish a stronger boundary. The claim relies on the configured
filesystem/device honoring those requests.

macOS: ordinary `fsync` is not the selected power-loss boundary; use
`fcntl(F_FULLFSYNC)` and report failure without silently downgrading. Apple
documents the distinction in [fsync](https://developer.apple.com/library/archive/documentation/System/Conceptual/ManPages_iPhoneOS/man2/fsync.2.html)
and [fcntl](https://developer.apple.com/library/archive/documentation/System/Conceptual/ManPages_iPhoneOS/man2/fcntl.2.html).
These are archived API documents, not empirical APFS/power-loss qualification.
Current Blackbird uses the same strength for directory synchronization; the
native qualification must directly establish the correct file/directory
protocol on its actual macOS filesystem before accepting this recorder. Do not
claim old HFS documentation established contemporary APFS behavior.

LevelDB pin `7ee830d02b623e8ffe0b95d59a74db1e58da04c5`:
[log writer](https://github.com/google/leveldb/blob/7ee830d02b623e8ffe0b95d59a74db1e58da04c5/db/log_writer.cc)
fragments into32KiB blocks with7-byte headers and CRC, while `Flush` only drains
the file's buffer. `DBImpl.Write` calls `Sync` only when requested and blocks
future writes after a sync error because success is ambiguous. `log_reader.cc`
can skip corrupt/partial records; that permissive salvage is not our complete
raw-history rule. `env_posix.cc` falls back from failed `F_FULLFSYNC` to a weaker
flush, and its buffered flush resets the buffer after error; our source retention
and strict strength rule must not copy those details.

SQLite pin `5af1b822f5da6f4ea72ead5060e4bc365ccbf209`:
[wal.c](https://github.com/sqlite/sqlite/blob/5af1b822f5da6f4ea72ead5060e4bc365ccbf209/src/wal.c)
documents header/frame/commit/checksum structure and `walFrames` performs the
configured sync before publishing its wal-index header. `os_unix.c` distinguishes
file/directory flush and also falls back from fullfsync. The official
[WAL description](https://www.sqlite.org/wal.html) distinguishes fully synced
commits from asynchronous commits that can disappear on OS/power failure.
There is also a distinct **powersafe overwrite** assumption: a later append may
rewrite a physical sector containing an already acknowledged batch's last bytes.
Flush-before-ack alone does not prove a torn later write cannot damage those
bytes. `walFrames` has a sector-padding path when that property is absent. Our
recorder must either qualify an explicit powersafe-overwrite storage contract
or isolate completed batches using a qualified recorder-side sector/padding
protocol. Padding is container framing; it cannot alter the source payload or
run in its emitter. Existing Blackbird framing has no arbitrary padding records,
so this needs a deliberate raw-family format rule, not trailing zero bytes
smuggled into an existing journal. Correct namespace sync, no arbitrary media
destruction, and devices honoring flush remain assumptions even with isolation.
CRC32C detects many corruptions; it does not prove detection of all adversarial
byte changes. See SQLite's explicit
[storage assumptions](https://www.sqlite.org/atomiccommit.html).
Borrow explicit boundaries and direct filesystem fault injection, not page
transforms, checkpoint machinery, the fallback or a database dependency.

## Direct verification plan for this conceptual unit

No suite or benchmark was run for the study. After approval, direct recorder
checks must use an independently specified sequence of exact input byte strings
and source identities; checking that the recorder can read its own output is
not a sufficient oracle.

1. Inject each positive short-write boundary, zero write, interrupted call and
   delayed write/flush failure. Disk prefix may advance only after a complete
   valid batch and successful required syncs; source reclamation never exceeds
   that prefix. Compare every retained payload byte to independent input.
2. Use a filesystem model where page-cache writes, sector persistence and
   namespace persistence are separate. At every append/commit/file-sync/
   directory-sync/ack step crash it; reorder unsynced writes and tear sectors,
   including a later append touching the acknowledged prefix's tail sector.
   Every acknowledged record must reappear unchanged; unacked records either
   remain retained on a live OS or are explicitly outside the host-crash claim.
3. Kill producer and recorder at real process boundaries, retaining the channel
   owner. Specifically kill after drain before append, after append before sync,
   and after sync before ack. Restart must neither omit nor double-append records.
   A killed userspace process is not a simulated power failure.
4. Inject partial/corrupt tails and interior corruption. Bad original files must
   remain unchanged; successor links select an exact validated boundary. Never
   acknowledge across damage. A conflicting replay identity must stop.
5. Exhaust data slots, control slots, index capacity, disk/quota capacity and
   segment rollover capacity. Observe admission stop and exact original/error
   retention. Failure-event publication must work when the normal ingest path is
   deliberately unusable; it must not recurse or report imaginary durability.
6. Pause the transformer indefinitely. Producer acknowledgement must depend
   only on recorder flush, and the transformer must see no unflushed prefix.
7. Measure separate recorder copy bytes/record, writes/batch, syncs/batch, drain
   throughput and durable lag. Report source storage occupancy and actual source
   backpressure alongside throughput. Optimize only a demonstrated cost.

One source qualification campaign after implementation is appropriate; a full
product suite for this research text is not.

## Acquisition and restoration

New source lives only under ignored `quarantine/raw-recorder-2026-10-10/`.
The intact daemontools archive is retained; no nested Git metadata, generated
builds or filesystem detritus was acquired. Its SHA-256 is
`a55535012b2be7a52dcd9eccabb9a198b13be50d0384143bd3b32b8710df4c1f`.

Restore from repository root with an absent destination:

```sh
mkdir -p quarantine/raw-recorder-2026-10-10
curl -fL https://cr.yp.to/daemontools/daemontools-0.76.tar.gz -o quarantine/raw-recorder-2026-10-10/daemontools-0.76.tar.gz
tar -xzf quarantine/raw-recorder-2026-10-10/daemontools-0.76.tar.gz -C quarantine/raw-recorder-2026-10-10
```

Verify the archive hash before extraction. Source-path identities, acquired
documentation hashes and restoration commands are recorded in the ignored
`quarantine/raw-recorder-2026-10-10/MANIFEST.json`; source-function paths above
identify exactly what was studied. Existing LevelDB restoration is in
[its acquisition catalog](2026-10-01-refit-custody-acquisition.json); SQLite
pin/archive/restore are in [storage references](storage-performance-2026-10-07/references.json).
Cedar paper identity/restoration is in
[the preceding old-systems study](2026-10-10-instrumentation-raw-emission-study.md).
The shared acquisition owner can index this new material in QUARANTINE.md.

Linux syscall source was read from the intact official
[man-pages6.19 archive](https://cdn.kernel.org/pub/linux/docs/man-pages/man-pages-6.19.tar.xz),
SHA-256 `88a7c42ad2e03d8b96dc72d95e451f2d875ff0f43103a8eb8ac8242133bdcb05`.
Only `man/man2/fsync.2`, `write.2` and `close.2` were extracted/read; the full
source archive remains unchanged. Restore by downloading that URL into
`quarantine/raw-recorder-2026-10-10/man-pages-6.19.tar.xz`, checking its hash and
running:

```sh
tar -xf quarantine/raw-recorder-2026-10-10/man-pages-6.19.tar.xz -C quarantine/raw-recorder-2026-10-10 man-pages-6.19/man/man2/fsync.2 man-pages-6.19/man/man2/write.2 man-pages-6.19/man/man2/close.2
```

Direct man7 download timed out; the publisher archive closes that acquisition
gap. Apple archived API pages and djb author HTML were downloaded intact with
their hashes in the manifest. Browser acquisition of djb pages failed while
direct acquisition succeeded. No unavailable paper or unread source is counted.
