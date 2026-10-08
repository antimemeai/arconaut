# Heavy startup: doing less work, deliberately

Study requested after the 300-start launcher measurements. Production source:
d1c0dcf. No production code, installed executable, recovery policy or dependency
changed by this study. Development probes were extended to isolate archive cost.

## Finding

The expensive case is archive-heavy, not especially live-context-heavy. It holds
262,144 bytes of live content in 64 entries, plus 640 rejected context edits with
512 KiB candidate strings. Those rejected candidates never publish a new live
view. Startup nevertheless reads their bytes three times, checksums them repeatedly,
allocates/copies them and scans their JSON. The current representation forces work
that a normal reopen does not need to perform.

The existing research already identifies the right architecture: a saved current
state, a bounded recovery suffix and independently addressable history. The missing
part is delivery through the actual startup consumers. Neither cold payload handles
nor the optional physical scan checkpoint implements that architecture.

## Controlled experiment on the current native libraries

Added `live` to the existing development fixture generator: the same 64 entries as
`heavy`, without the 640 rejected edits. Its entire 611,608-byte audit is byte-for-byte
identical to the initial prefix of the heavy fixture. Thus this comparison preserves
the current context and initial identities, rather than comparing unrelated sessions.

Five fresh native probe processes per case, alternating live/heavy. Each receives
a private APFS clone of an immutable seed; cloning is outside the clock. Normal
Release, DEBUG=OFF, existing native libraries. No model/network request: the existing
deterministic local provider verifies native request admission and known settlement.
All ten processes exit successfully. One-minute host load was 4.69–5.03, versus
11–78 during the earlier launcher study. Ordinary OS cache, no cold-cache claim.

| Observation | Same live context, no rejected archive | Same live context + rejected archive |
| --- | ---: | ---: |
| Audit size | 611,608 bytes | 336,327,448 bytes |
| Facts at open | 2 | 642 |
| Serialized provider input | 302,241 bytes | 302,241 bytes |
| Journal reads through prompt preparation | 1,834,440 bytes | 1,008,981,960 bytes |
| Median open/replay/recovery wall time | 9.20 ms | 464.58 ms |
| Median context-constructor wall time | 4.49 ms | 248.31 ms |
| Median cumulative usable prompt time | 13.60 ms | 748.75 ms |
| Median cumulative audited provider admission | 68.32 ms | 817.07 ms |
| Median cumulative process CPU at admission | 30.94 ms | 642.46 ms |
| Median sampled RSS at prompt | 5.91 MB | 8.98 MB |

These are five-run diagnostic medians, not a replacement p90 study or an achieved
startup target. Phase medians do not necessarily sum to the median of totals.
The clock starts inside probe main: launcher, dynamic loading and TUI are excluded.
The provider path includes request construction and required admission durability.
The old TUI-local-command endpoint did not measure provider admission.

Read counts are successful bytes returned by JournalFile reads, including page-cache
hits, not physical disk traffic. The probe's extra 112-byte header inspection is
outside that counter. CPU is cumulative process CPU, not just one function's self
time. RSS is sampled residency, not peak heap. All samples and qualifications:
[numeric data](../docs/measurements/heavy-startup-components-2026-10-07.json).
Private seeds/raw output: `context/heavy-startup-study-2026-10-07/`.

This is strong evidence that historical work dominates this fixture. Removing the
archive was an experimental control, not a proposed deletion or migration strategy.

## Current path, read from source

1. `src/main.cpp:174` opens **RetainedState** directly with scan hints disabled by
   default. The application measured here does not enter RetainedEnvironment.
   The environment path separately replays predecessor segments and must also be
   addressed; changing only that path would miss today's launcher.
2. `FramedJournal::scan` in `src/journal_writer.cpp:363` reads all frames, checks
   individual frame CRCs and checks the batch CRC. It collects historical record
   descriptors. It reserves a buffer for the allowed maximum payload, but reserved
   address space must not be confused with touched/resident memory.
3. `RetainedState::replay` in `src/retained_state.cpp:564` reads the semantic frames
   again. `read_payload` verifies each frame again and erases its prefix from the
   vector, shifting the payload. `Decoder::blob` copies the application payload;
   replay then replaces that owned payload with a cold disk reader. This saves
   persistent residency while still paying transient decode/copy costs.
4. `ContextStore` in `src/context.cpp:133` walks all context application records.
   Every cold reader rereads the journal header and entire selected frame, verifies
   it and strips prefixes. `read_text` constructs a string. JSON projection avoids
   retaining the candidate field but still scans it to validate its syntax. Only
   after reading the record does the constructor learn it was rejected.
5. Session/configuration/identity consumers independently walk retained facts.
   This fixture has little program history, so their history-scaling risk is not
   well exercised here. Recovery also enumerates historical admissions to find
   unresolved work; real long-running sessions need a separate record-count case.

The measured reads are exactly consistent with two archive passes during core
restoration (672,618,832 bytes) followed by a third context pass. Header/commit
differences explain why this is not exactly three times the file size. The cold
reader also removes prefixes with vector erase; this adds memory movement without
adding useful information. Fix those copies where touched during the real change,
but do not substitute copy/CRC/SIMD tuning for eliminating the historical passes.

The memory improvement is real and worth preserving. Historical application blobs
are no longer all permanently resident. Non-application event blobs are still eager;
this rejected-edit fixture does not establish memory scaling for old provider/tool
inputs, decisions and observations.

## Recommendation

Ordinary startup should perform work proportional to current state, the required
unresolved-effect state, a bounded recent suffix and a small number of index pages.
Archive byte count must disappear from that equation. Growing live input still has
real serialization cost. Growing unresolved work still has real recovery cost.

Use a versioned, compact checkpoint containing current session/program settings,
live context/order, issuer reservation high-water marks and unresolved-operation
linkage, bound to one committed audit position. For modest live state, a contiguous
binary checkpoint is a reasonable first representation; serializing 256 KiB of useful
state is defensible. Serializing every historical fact is not. Do not require a
generic persistent object store to publish that first complete state.

Historical originals, exact duplicate comparisons and settled-operation queries
use checked disk locators. Keep historical indexes pageable and query errors
fallible. A bound on resident overlay/cache memory is intentional; a permanent
zero-cache rule is not a performance principle. Small useful caches should be
measured rather than prohibited.

Checkpoint after useful bounded amounts of tail work during operation, with an
optional final checkpoint on orderly exit. Recovery after a crash must work from
the previous durable generation plus its complete subsequent suffix. Checkpointing
only on clean exit would miss the important failure case. Tail limits need to
account for both record count and bytes, and preserve settlement capacity.

Publish referenced state before its root, preserve a previous complete generation,
and reconstruct uncertainty before allowing effects. Verify the state/index pages
actually read. Full archive verification remains an explicit operation and old
payloads are checked when fetched. Fast reopen cannot simultaneously reread every
archived byte; checksums are not a way around that fact. The existing plan already
declares this fault model. Exact originals and uncertain effects remain preserved.

### Correct the implementation sequence

The existing DURABLE_STATE_PLAN is directionally sound, but its first milestone
put a broad persistent index ahead of any delivered saved-state restoration. The
merged index still stores RAM-vector ordinals. Its presence does not make startup
independent of those vectors, and it is not enabled by the measured launcher.

Its reported 1,600-key test wrote 48,873,472 bytes. Source confirms that every
`put` copies/writes a full 16 KiB path immediately, even if several keys from one
logical update hit the same pages. There is no reclamation yet. This is not the
reason today's launcher is slow, but would be a poor default maintenance path.

Use a transaction/batch with writable dirty pages, publish once per batch and bulk
build a legacy index once. Add reclamation with explicit reader/previous-root
lifetime. LMDB's `mdb_page_touch` returns immediately for an already writable page;
copy-on-write does not require writing a fresh path for every individual key.
Batching may require changing today's per-put immutable-root API: rollback and
intermediate-root lifetimes must remain explicit. Do not break callers' old-root
expectations merely to reduce write counts.

I would scope the next unit around one end-to-end demonstration: ordinary reopen
of this heavy fixture restores complete current state and admits the same local
request without reading rejected candidate bodies. Implement the saved reducer
and migrate startup consumers alongside the minimum disk queries they require.
Keep the existing full replay as independent fallback/oracle. Historical lookup,
ID uniqueness and unresolved-effect behavior are requirements of this unit; they
cannot be deferred behind a fast-looking prompt. Broad optional indexing and
low-level tuning should not be prerequisites to seeing that integrated result.

This is a proposed sequencing correction for discussion, not a silent replacement
of the accepted plan or a claim that the implementation is complete.

### The next related issue after reopen

`ContextStore::append_impl` records the entire resulting `entries` array on each
append, alongside newly captured originals. Repeated accepted appends therefore
can write the accumulated context repeatedly. A subsequent format evolution should
record small context changes referring to stable original IDs, with occasional
current-state checkpoints. Keep exact submitted candidates and provider requests
as archival evidence, separate from the small fields needed to reduce state. This
reduces future archive growth and lets suffix replay avoid inspecting large inert
payloads. It is a separate change from reopening existing archives correctly.

## Reference mechanisms actually used

Re-read pinned local reference source, not just the previous study:

- **Aeron**, `RecordingLog.java:1749–1777`: selects a matching snapshot and begins
  replay at its log position. This is the direct model for saved state plus suffix.
  [Pinned source](https://github.com/aeron-io/aeron/blob/ad4baf8dcd5aed31e5a60a89940748e9408baa12/aeron-cluster/src/main/java/io/aeron/cluster/RecordingLog.java).
- **LMDB**, `mdb.c:4868`, `5012`, `3019`: alternating generation metadata, root
  selection and transaction-local writable pages. Use durable publication and
  batched dirty-page handling, not one new persistent tree path per logical key.
  [Pinned source](https://github.com/LMDB/lmdb/blob/700e10f91a65fae69520301926fb9819f16d292f/libraries/liblmdb/mdb.c).
- **SQLite WAL**, official documentation re-read: checkpoint size controls the
  remaining log work; selected page reads avoid scanning all values. Sync ordering
  matters. We retain the audit separately rather than adopting WAL recycling as
  historical deletion. [WAL](https://www.sqlite.org/wal.html),
  [format and recovery](https://www.sqlite.org/walformat.html).

These mechanisms inform an owned implementation; no dependency adoption is proposed.
Prior corpus and restoration commands remain in
[the storage study](storage-performance-2026-10-07/STUDY.md).

## Evidence needed from the implementation

Use one direct generated-history comparison: full replay versus saved state plus
suffix, checking current context, settings, issuer progression, unresolved effects
and exact historical retrieval. Inject interruptions at publication boundaries;
restoration must select a complete state and preserve the subsequent tail.

Then hold live state/tail constant and independently grow archive bytes and record
count. Measure actual read bytes/replayed records as well as latency; this prevents
a warm page cache from hiding full scans. Include a realistic settled tool/provider
history, a crash with an unresolved operation, and larger live input. Report first
complete TUI frame and actual audited local provider admission separately. Repeat
the 100-start distributions after the integrated change, recording host load.

Sub-100 ms remains plausible as a target worth pursuing, not established by this
experiment. No implementation can promise that wall-clock bound under arbitrary
scheduler starvation. The structural success condition is stronger than a lucky
timing: archived rejected payloads are not read during routine startup at all.

## Reproduction

Build only the two development targets; ordinary production builds stay unchanged:

```sh
cmake --build build/release --target startup_scenarios startup_loading_probe -j2
build/release/startup_scenarios live context/new-study/live-seed
build/release/startup_scenarios heavy context/new-study/heavy-seed
```

For each trial create a new owner-only directory and APFS-clone the corresponding
seed files, then run `build/release/startup_loading_probe open TRIAL_DIRECTORY`.
The probe writes and settles its own local request; never run it against operator
sessions or reuse a mutated trial as the next seed. The existing benchmark helper's
`clone_session` performs the same cloning used here. Alternate cases and preserve
stdout, stderr, exit status and host load. Delete only completed private trial copies.
