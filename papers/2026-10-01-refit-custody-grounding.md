# Refit custody: source grounding before design

2026-10-01. Research input, not an adopted process architecture or implementation
plan. Scope: the observation, operation and writer custody that must survive a
main-executable refit while Arconaut's own standing workers remain alive and
paused. C++ and Lua are selected; no persistence or supervision library is adopted.
No acquired program, build, test, benchmark or provider request was executed.

## Readiness judgment

**We have enough primary evidence to design the narrow refit contract. We do not
yet have evidence that a chosen implementation preserves it on macOS arm64.**
The source establishes useful mechanisms and their actual limits: preserving
processes and descriptors outside the replaced image; observable stopped-child
state; descriptor duplication between local processes; framed append recovery;
and explicit synchronization before acknowledging durable work. It does not
supply a complete custody-transfer protocol, portable arbitrary-tree suspension,
or automatic recovery of effects whose outcomes were not observed.

The [starting sketch](../docs/STARTING_DESIGN.md) correctly labels its independent
custodian as a proposal. The old [runtime study](2026-09-30-runtime-design-study.md)
and [audit study](2026-09-30-audit-state-design-study.md) remain useful source work;
their Lisp/Rust/Python and SQLite recommendations are not current selections.
The audit study's transaction arguments must be reconsidered for the sketch's
owned append stream and separately retained blobs. Studying SQLite WAL does not
establish the correctness of that different mechanism.

## Evidence acquired and actually inspected

| Evidence | Exact material inspected | What it establishes |
| --- | --- | --- |
| Existing s6 reference | `67254f0c147f9b9f71aabc7988e52bdaeb9576ab`; supervisor STOP/CONT, fdholder client/server dump, retrieve, and line-input logger paths | Independent Unix custody mechanisms, with non-atomic and observational limits |
| Newly acquired LevelDB reference | `7ee830d02b623e8ffe0b95d59a74db1e58da04c5`, upstream commit 2026-03-11; log writer/reader, POSIX writable file, DB write/recovery and relevant fault-test source | Actual append framing, torn-record handling, flush/sync distinction and unknown sync outcomes |
| POSIX.1-2024 Issue 8 | Official `exec`, `recvmsg`, `sendmsg`, `wait`, `kill`, `pipe`, `write` contracts, retained HTML | Normative image/descriptor/child/stream semantics; availability on any specific OS still requires qualification |
| Installed Apple SDK | Xcode macOS SDK 26.2, canonical name `macosx26.2`; `execve`, `wait`, `recv`, `fcntl`, `pipe`, `kill`, `kqueue`, `fsync`, `write` manuals retained verbatim | Target-platform documented primitives, including full sync and child-scoped exit-status observation |
| Current Linux primary documentation | Kernel cgroup-v2 freezer contract and man-pages 6.19 `execve`, `unix`, `pipe`, `fsync` | Linux-specific completion, descriptor sharing and directory durability obligations |

[Reference acquisition catalog](2026-10-01-refit-custody-acquisition.json) and
[platform-document catalog](2026-10-01-refit-custody-platform-acquisition.json)
record locations and hashes. LevelDB's intact ZIP is retained in shared archives;
152 extracted files were byte-compared directly with it and executable bits were
retained. No nested Git metadata or filesystem detritus occurred. Its dependencies
and full key-value machinery were not acquired as production components.
Web-tool fetches of some Open Group pages returned 403; ordinary unauthenticated
HTTP subsequently acquired the official pages successfully. The retained pages,
not failed fetches or search snippets, support the POSIX observations below.

## Process existence, resource custody and execution authority differ

POSIX `exec` replaces the address space and terminates the other threads without
calling their destructors/cleanup handlers. PID, parent relationship and eligible
open descriptors survive; memory mappings and live C++/Lua stacks do not.
Thus direct `exec` can preserve parentage and prepared descriptors, but cannot
preserve a standing Lua continuation inside the replaced image. The successor's
early failure can close the descriptors it alone inherited. These are reasons
to consider surviving custody, not a proof that a particular multi-process layout
is necessary. [POSIX exec](https://pubs.opengroup.org/onlinepubs/9799919799/functions/exec.html),
[Apple execve](https://developer.apple.com/library/archive/documentation/System/Conceptual/ManPages_iPhoneOS/man2/execve.2.html).

On local Unix sockets, SCM_RIGHTS transfers references to open-file descriptions.
Linux documents equivalence to duplication into another process: descriptor
numbers need not match, and truncation or descriptor limits can close excess
received descriptors. The Apple SDK `recv(2)` independently documents SCM_RIGHTS
and `MSG_CTRUNC`. Sharing a reference does not duplicate a pipe's unread contents
into independent streams: two active readers can consume different portions.
Transferring a socket also does not serialize its parser, buffered plaintext,
TLS objects or application protocol state. **Handle survival is only part of
stream survival.** [Linux unix](https://man7.org/linux/man-pages/man7/unix.7.html),
[POSIX recvmsg](https://pubs.opengroup.org/onlinepubs/9799919799/functions/recvmsg.html).

s6's separate logger illustrates observation lifetime, not original-byte capture.
The inspected `getchunk`/`normal_stdin` paths find newline or NUL delimiters,
split at a configured line limit and pass delimiter-free strings to the logging
script; `process_partial_line` turns the buffered tail into a line. This contract
cannot stand in for Arconaut's byte offsets, partial records and untouched originals.
[Logger input](https://github.com/skarnet/s6/blob/67254f0c147f9b9f71aabc7988e52bdaeb9576ab/src/daemontools-extras/s6-log.c#L1061).

The inspected s6 `transferdump` client gets descriptors from the source, connects
to the destination and sets the dump. Server `do_getdump` leaves the source store
intact. `do_setdump_data` installs received batches and acknowledges intermediate
batches; there is no whole-transfer rollback or exclusive-controller handoff in
these paths. A failure can leave the destination with some installed descriptors.
It is useful physical-custody prior art, **not an action-authority transfer**.
[Transfer client](https://github.com/skarnet/s6/blob/67254f0c147f9b9f71aabc7988e52bdaeb9576ab/src/fdholder/s6-fdholder-transferdump.c#L64),
[server dump paths](https://github.com/skarnet/s6/blob/67254f0c147f9b9f71aabc7988e52bdaeb9576ab/src/fdholder/s6-fdholderd.c#L352).

POSIX/Apple wait APIs report a caller's children. Moving a descriptor to another
process does not move that parent relationship or make `waitpid` valid there.
The local Apple kqueue manual likewise makes `NOTE_EXITSTATUS` child-only;
general exit notification is not replacement for parent-owned status collection.
An update to the custodian must therefore state what survives as the parent or
how the promised lifecycle observation changes. No parentage-transfer mechanism
was established by this study. [POSIX wait](https://pubs.opengroup.org/onlinepubs/9799919799/functions/wait.html),
[Apple wait](https://developer.apple.com/library/archive/documentation/System/Conceptual/ManPages_iPhoneOS/man2/wait.2.html).

## Pause has a scope and an observed completion

s6 `killp`/`killP` sends SIGSTOP and sets `flagpaused` immediately without checking
`kill`'s result. That flag describes control intent, not an adequate pause oracle.
POSIX successful process-group `kill` requires permission for at least one target;
it does not establish that every intended member stopped. The Apple SDK describes
different group-error detail, another reason not to infer a portable tree-wide
acknowledgement from return zero. Process groups also are not arbitrary descendant
trees. [Supervisor paths](https://github.com/skarnet/s6/blob/67254f0c147f9b9f71aabc7988e52bdaeb9576ab/src/supervision/s6-supervise.c#L252),
[POSIX kill](https://pubs.opengroup.org/onlinepubs/9799919799/functions/kill.html).

For an owned worker child, `waitpid(..., WUNTRACED)` plus `WIFSTOPPED` supplies an
observed stop. It supplies neither an enumeration of arbitrary grandchildren nor
a stable membership boundary while unrelated code forks or changes groups.
A cooperative workflow checkpoint can carry a stronger application meaning,
but it must account for its own active callbacks, requests, timers and children;
a reply named `paused` alone proves nothing beyond the program's stated contract.
Neither mechanism pauses remote services, shared files or elapsed time.

Linux cgroup-v2 `cgroup.freeze=1` includes descendant cgroups and has asynchronous
completion in `cgroup.events:frozen=1`. Frozen tasks may still be killed or moved;
moving out makes them runnable. Available delegation and membership discipline
are therefore part of its qualification. This is useful fleet evidence, **not a
macOS suspension implementation**. No robust macOS arbitrary-process-tree pause
primitive was established here. That remains a consequential scope/design choice,
not permission to pretend the Linux mechanism is portable.
[Kernel freezer contract](https://www.kernel.org/doc/html/latest/admin-guide/cgroup-v2.html#core-interface-files).

Refit consequently requires a known set of harness-owned standing workers and
an actual pause condition for that set. Workers retain the code and native objects
their live Lua stacks require. Settled ordinary programs and provider requests
are separate prerequisites. A paused client may resume to changed external state
or expired connections/deadlines. Consumed services and independent daemons are
outside this pause scope, as the operator explicitly required.

## Append persistence: what mature source teaches and what it does not

LevelDB's log format has CRC-protected physical records and explicit complete or
first/middle/last fragments within block boundaries. Its writer emits header and
payload, then calls `Flush`, not `Sync`. Its reader returns only completed logical
records; truncated trailing headers, payloads or fragmented records become EOF.
Interior checksum/length/fragment errors can be reported and skipped with later
resynchronization. DB recovery always checks checksums but can continue after
corruption when `paranoid_checks` is false. **That salvage policy must not become
silent loss in an original audit.** A valid later record does not certify that
every earlier observation survived.
[Format](https://github.com/google/leveldb/blob/7ee830d02b623e8ffe0b95d59a74db1e58da04c5/doc/log_format.md),
[writer](https://github.com/google/leveldb/blob/7ee830d02b623e8ffe0b95d59a74db1e58da04c5/db/log_writer.cc#L91),
[reader](https://github.com/google/leveldb/blob/7ee830d02b623e8ffe0b95d59a74db1e58da04c5/db/log_reader.cc#L149),
[recovery](https://github.com/google/leveldb/blob/7ee830d02b623e8ffe0b95d59a74db1e58da04c5/db/db_impl.cc#L384).

In its actual DB write path, a serialized batch is added to the log, optional
`Sync` completes, then the batch enters the memtable. A sync error blocks later
writes because the just-written record may or may not survive reopening.
The source explicitly treats this as indeterminate, rather than erasing it or
retrying as if absent. An owned audit needs its own acknowledged durability
boundary; one complete frame, a successful `write`, `Flush`, and a successful
durability operation are different facts. O_APPEND controls placement and
PIPE_BUF controls specified pipe interleaving, neither supplies a multi-write
durable record transaction.
[DB write](https://github.com/google/leveldb/blob/7ee830d02b623e8ffe0b95d59a74db1e58da04c5/db/db_impl.cc#L1206),
[POSIX write](https://pubs.opengroup.org/onlinepubs/9799919799/functions/write.html).

The POSIX file implementation loops over short writes, retries EINTR, separates
Flush from Sync, and syncs relevant directory state for manifest publication.
On Apple it attempts F_FULLFSYNC and can fall back to weaker fsync behavior.
The installed Apple fsync/fcntl manuals distinguish flushing to a drive from
asking its cache to reach persistent storage; the current SDK also describes
F_BARRIERFSYNC as ordering rather than durability. A silent fallback must not
support a stronger durability claim than the actual successful primitive.
Linux documentation separately requires directory synchronization for durable
directory entries. Actual filesystem/device failures remain outside a mere
successful API-call demonstration.
[POSIX file implementation](https://github.com/google/leveldb/blob/7ee830d02b623e8ffe0b95d59a74db1e58da04c5/util/env_posix.cc#L334),
[Apple fsync](https://developer.apple.com/library/archive/documentation/System/Conceptual/ManPages_iPhoneOS/man2/fsync.2.html),
[Linux fsync](https://man7.org/linux/man-pages/man2/fsync.2.html).

For Arconaut's proposed separate blobs, persistence design must bind each
acknowledged event to complete reachable original bytes. A durable event pointing
to an incomplete or unpersisted blob is not complete capture. The reverse order
can leave an orphan blob; that is different from missing acknowledged evidence.
Whether bytes share a logical frame, whether multiple frames form an explicit
commit, and how segment/blob names become durable are specification questions.
LevelDB batches demonstrate one particular recovery boundary, not a transaction
for an independently designed record/blob pair. No storage dependency or framing
format is selected here.

The inspected log tests directly assert torn-tail omission, fragment errors and
checksum failures. `WriteSyncError` asserts later writes remain blocked. The
fault-injection environment tracks synchronized file positions, truncates
unsynchronized data and removes unsynchronized new names, then reopens the DB.
Those are valuable fault models, not actual power-failure qualification. Its
`VAL_EXPECT_NO_ERROR` helper propagates a missing/error read rather than accepting
it: the loop exits with that non-OK status, checked by its caller. Conversely,
the post-sync missing-value branch accepts any read error, so its oracle does not
distinguish absence from unrelated read failures. Arconaut needs its own exact
acknowledged-record/byte oracle rather than copying those predicates wholesale.
[Log tests](https://github.com/google/leveldb/blob/7ee830d02b623e8ffe0b95d59a74db1e58da04c5/db/log_test.cc#L384),
[sync-error test](https://github.com/google/leveldb/blob/7ee830d02b623e8ffe0b95d59a74db1e58da04c5/db/db_test.cc#L1818),
[fault model and oracle](https://github.com/google/leveldb/blob/7ee830d02b623e8ffe0b95d59a74db1e58da04c5/db/fault_injection_test.cc#L416).

## Custody transition obligations for the specification

This is an invariant checklist for discussion, not an engineered protocol:

1. **One current effect authority for the handed-off conversation.** Duplicating
   descriptors or losing a connection does not revoke the earlier owner's ability
   to act. Old requests/callbacks must be rejected or made incapable of dispatch
   at the actual effect boundary before the new owner dispatches. An epoch number
   written in a file is ineffective if both processes can bypass its enforcement.
2. **At least one live physical custodian of every promised standing resource
   during ordinary refit.** Confirm receiving custody before surrendering the last
   needed descriptor. Child parentage/status collection and native worker code
   ownership are separate from descriptor ownership. Whole-host failure cannot
   preserve these live heaps and is a different fault class.
3. **Observation and writer ownership have explicit boundaries.** For each stream,
   transfer the committed cursor, any consumed but uncommitted bytes, parser state
   where needed, and responsibility for the next read. Do not have competing readers
   silently split the stream. Build/outpost output and conversation continue under
   an identified observer/writer while the main executable is absent.
4. **Durable admission precedes dispatch; durable captured bytes precede normal
   publication.** A dispatch and a local append cannot be atomic with an arbitrary
   external service. After an open attempt loses its observer, the result remains
   unknown unless reconciliation establishes it. Duplicate submission identity
   alone does not make an external effect idempotent.
5. **Recovery never interprets missing completion as permission to repeat.**
   A failure after receiver acquisition but before acknowledgement can leave both
   holding resources. A commit whose acknowledgement was lost may have survived.
   A dead last owner can lose stream endpoints even while a worker is alive.
   Recovery must inspect the retained facts and actual living state, preserve
   uncertainty, and stop affected normal dispatch until ownership is resolved.
6. **Failure of candidate build/start leaves the outpost usable.** Writer/storage
   failure is a different condition: ordinary effects stop; finite buffering,
   producer pause and emergency unaudited controls need explicit dispositions.
   A retained pipe can delay EOF; lost readers can cause SIGPIPE/EPIPE. Keeping
   arbitrary extra copies is therefore not a free general recovery strategy.

POSIX sendmsg expressly does not guarantee delivery on successful return. The
handoff's application acknowledgement must describe actual receipt/custody and
its durability meaning, not an OS send result. Streams have finite capacity;
pause does not settle remote work already dispatched.
[POSIX sendmsg](https://pubs.opengroup.org/onlinepubs/9799919799/functions/sendmsg.html),
[Linux pipe](https://man7.org/linux/man-pages/man7/pipe.7.html).

## What remains before and after settling design

**Design choices now have sufficient grounding:** define the standing-worker
scope; choose its pause completion evidence; state who retains parentage, capture
and writer duties; define action-authority transfer; define audit commit/recovery
and blob publication; separate ordinary main refit from custodian replacement.
These must be in the reviewed specification before its implementation plan.

**Missing mechanism evidence:** no inspected reference implements that whole
combined handoff; no portable arbitrary-tree pause proof was found; no owned
append/blob format exists yet; macOS atomic parentage reassignment was not
established. Those are not gaps cured by adding another agent to the zoo. Narrow
the contract or study the exact selected mechanism where the design requires it.

**Runtime qualification after reviewed design/plan:** on macOS arm64, use an owned
fixture whose worker carries live state and records independent progress. Refit
through failed startup and verify the same living worker, paused progress and
exact stream bytes after resume; perturb each handoff boundary with process death
and lost acknowledgements. Exercise append short writes, torn tails, failed sync,
interior corruption, disk exhaustion and missing blob/segment names. The direct
oracle is the independently known admitted attempts and acknowledged byte sequence,
not successful parsing or a coherent surviving log. Linux freezer access and
membership need their own fleet qualification. No such experiment ran here.

## Ready manifest/restoration entry for the owning lane

- Name: `leveldb`; repository: `https://github.com/google/leveldb`.
- Revision: `7ee830d02b623e8ffe0b95d59a74db1e58da04c5`.
- Project extraction: `quarantine/leveldb` (152 files, 992405 bytes).
- Intact archive relative to Arconaut:
  `../quarantine_proj/archives/arconaut-refit-custody-2026-10-01/leveldb-7ee830d02b62.zip`.
- Download:
  `https://codeload.github.com/google/leveldb/zip/7ee830d02b623e8ffe0b95d59a74db1e58da04c5`.
- Archive SHA-256:
  `cf6fd7b51903cac54be415a5cf59572b30dfcce41011821c071f187fd713efb4`.
- Restore from project root with `unzip` into an otherwise absent destination,
  then rename extracted `leveldb-7ee830d02b623e8ffe0b95d59a74db1e58da04c5` to
  `quarantine/leveldb`. Strip nested Git metadata/detritus if upstream archive
  content changes; retain the archive unchanged. Verify retained files against
  the archive as the acquisition catalog describes.

```sh
mkdir -p quarantine ../quarantine_proj/archives/arconaut-refit-custody-2026-10-01
curl --fail --location https://codeload.github.com/google/leveldb/zip/7ee830d02b623e8ffe0b95d59a74db1e58da04c5 --output ../quarantine_proj/archives/arconaut-refit-custody-2026-10-01/leveldb-7ee830d02b62.zip
unzip ../quarantine_proj/archives/arconaut-refit-custody-2026-10-01/leveldb-7ee830d02b62.zip -d quarantine
mv quarantine/leveldb-7ee830d02b623e8ffe0b95d59a74db1e58da04c5 quarantine/leveldb
```

- Platform documents are project literature, catalogued under
  `papers/platform/refit-custody-2026-10-01/`; official POSIX URLs permit refetch,
  while installed Apple manuals must come from the stated SDK or be marked a
  different revision. Root owns shared manifest, journal and bead updates.
