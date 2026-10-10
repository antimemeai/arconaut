# Raw event transport: publication, retention and replay

2026-10-10. Source study and design recommendation for discussion. No product
implementation, reference execution, benchmark, source build, dependency adoption,
provider call or full suite was performed by this research lane. The root lane's
separate primitive probes, if run, are separate evidence.

## Recommendation and its exact scope

Recommend a **preallocated, named shared-memory byte lane for each producer
execution context**, with one complete-record publication cursor and one
disk-acknowledged reclamation cursor. The separate recorder reads without
removing bytes. It returns space only after preserving the corresponding raw
prefix on disk under the recorder's specified flush contract. The tertiary
process reads the preserved prefix and performs every measurement transform.

This is an owned adaptation of Lamport's 1983 FIFO publication discipline, with
removal delayed until disk acknowledgment. It is not an existing implementation
we have qualified, and the source study does not demonstrate an overhead number.
The recommendation follows from retention and work accounting: a pipe makes
reads destructive, while the shared lane provides both transport and retained
replay bytes without another producer queue or a per-event syscall. Compare
against a raw pipe in the direct primitive experiment before implementing it.

The guarantee is precise: **complete published records survive either producer
or recorder death, or both together, while the host and named memory object
remain available; reclaimed records have met the recorder's disk-preservation
contract.** An observation acquired before publication has an interruption gap.
A host crash before disk preservation can lose the pending RAM prefix. Calling
either boundary unconditionally lossless would be false. Closing those gaps
requires a stronger acquisition/effect or storage contract, not a renamed queue.

## The older mechanisms actually followed

**Research Unix V7, 1979.** Read the complete acquired `pipe.c`, including
`pipe`, `readp` and `writep`. The writer waits for space, wakes a reader after
progress and returns EPIPE/SIGPIPE if the other end has gone. A read advances
the file offset; catching up resets the buffer. This is a compact process-to-
process stream with explicit finite-capacity progress, not disk preservation.
The inode lock, on-device backing and 4096-byte historical limit are not our
implementation. [Historical source](https://www.tuhs.org/cgi-bin/utree.pl?file=V7/usr/sys/sys/pipe.c).

**Lamport, Specifying Concurrent Program Modules, TOPLAS 5(2), April 1983.**
Read section 3.4, Figures 5 and 6, and the surrounding publication/progress
reasoning in the acquired complete paper. The element becomes visible when
TAIL advances after all its bits have been written; HEAD advances after
consumption. PUT cannot return without publication. Finite storage cannot
guarantee that PUT returns without consumption. The paper uses abstract atomic
actions and unbounded integers, discusses realistic modulo arithmetic, and
does not supply modern C++ memory ordering or process-death recovery.
[Author's paper](https://lamport.azurewebsites.net/pubs/spec.pdf).

**Bernstein's qmail 1.03 (1998) and daemontools 0.76 (2001).** These are later
than the spirit guide's date, but small older C programs, not metric frameworks.
Followed an actual producer through its logger, rather than stopping at an API:

1. `qmail-send.c` emits delivery and failure messages through `log1`, `log2`,
   `log3` and `logsafe`; `qsutil.c` binds its `substdio` writer to descriptor 0.
2. `qmail-start.c` optionally creates `pi0`, moves its write end to descriptor 1,
   gives the logger the read end as descriptor 0, then copies descriptor 1 to
   the future `qmail-send` descriptor 0 before connecting the delivery channels.
3. `qsutil.c` calls `substdio_putsflush`; `substdo.c` calls the supplied `write`
   operation, advances through short writes and retries EINTR.
4. In the separately supervised topology, daemontools `svscan.c:start` creates
   a pipe, moves its write/read ends into the service/logger supervisors, and
   retains both descriptors in the scanning parent across child restarts.
5. `multilog.c:flushread` first flushes its output buffers, then reads up to
   1024 bytes into private input storage. `doit` parses lines, selects outputs,
   optionally adds a timestamp and feeds each 512-byte `cyclog` buffer.
6. `buffer_put.c:allwrite` handles positive short writes and EINTR;
   `multilog.c:c_write` retries errors, writes the file and advances its byte
   position. `fullcurrent` fsyncs at rotation; `c_quit` flushes and fsyncs at
   orderly exit. The normal input-read path does not fsync each event.

Read the complete small setup, utility and buffering files and the complete
multilog implementation; qmail-send study was its emission call sites, not a
review of the entire mail system. [qmail archive](https://cr.yp.to/software/qmail-1.03.tar.gz),
[daemontools archive](https://cr.yp.to/daemontools/daemontools-0.76.tar.gz).

The architecture moves storage work into a separate process and keeps unread
pipe data through logger restart. It does **not** cover logger death after a
read. Its reader owns private unpreserved bytes. Qmail's logging helpers ignore
the write return, and `logsafe` changes the message. Multilog parses, filters,
adds timestamps/newlines, runs processors and deletes old archives. None of
those writer/recorder transformations or deletion policies are adopted.

**s6 2.15.1.0 and skalibs 2.15.1.0, inspected as a modern continuation of this
specific older mechanism.** `s6-svscan.c:check` creates and retains peer pipe
ends; `start_iter` maps them into the service and logger. `s6-log.c:normal_stdin`
fills a private buffer; `getchunk` accumulates and edits lines; `script_run`
selects and formats outputs; `logdir_write` writes the log. Skalibs `fd_read`
and `fd_write` retry EINTR, `buffer_fill` advances its cursor, and
`bufalloc_flush` advances its private progress through `allreadwrite`.
The `-b` mode prevents further reads while output is unflushed; it limits
read-ahead, but still has a post-read crash window. The default permits further
private accumulation. Studied those complete functions and their helper chain,
not the entire supervision suite. [Author's logging contract](https://skarnet.org/software/s6/s6-log.html),
[author's supervision overview](https://skarnet.org/software/s6/overview.html).

## Pipe contract and the counterexample that decides retention

A plain pipe is the simplest useful baseline. Emit a single framed raw record
in one write no larger than the descriptor's actual PIPE_BUF if several writers
share it; do not split such a record across independent writes. Larger writes
can interleave. Nonblocking writes no larger than PIPE_BUF are complete or
fail for insufficient space; larger nonblocking writes can be partial. EINTR
and EPIPE need explicit handling. A pipe is a byte stream: a reader assembles
length-framed records across arbitrary read boundaries. [POSIX write](https://pubs.opengroup.org/onlinepubs/9799919799/functions/write.html),
[POSIX read](https://pubs.opengroup.org/onlinepubs/9799919799/functions/read.html).

Consider the complete execution: producer writes event E successfully; recorder
reads E; recorder is killed before appending it; supervisor retains both pipe
ends and restarts the recorder. The pipe is empty. E was consumed and exists
only in the dead recorder's memory. Retaining descriptors does not recover it.
This counterexample follows directly from the source paths above.

Giving a pipe the requested recorder-death guarantee requires keeping E outside
the recorder until acknowledgment, plus replay after reconnection and an identity
for recovery. Retention only in the producer fails when it dies too. A separate
retained queue therefore reintroduces the shared storage machinery and adds
pipe syscalls/copies. SOCK_SEQPACKET preserves message boundaries but likewise
consumes received messages; it does not fix this retention problem.

## Byte lane ownership and publication

Each lane has a stable setup identity, capacity N, raw byte region, aligned
native 64-bit cursors P and D, and an external discovery entry. N is a power
of two chosen from measured burst and disk-acknowledgment backlog needs, not
a universal arbitrary byte ceiling. P and D occupy separate qualified cache
lines. Cursor arithmetic and framing are transport mechanics, not metric
accumulators. The record format is the capture owner's separate design input.

- **P**, written only by the producer, is the complete published byte prefix.
- **D**, written only by the recorder, is the prefix safe to overwrite because
  its raw records have met the disk-preservation contract.
- **R**, recorder-private, is its current read/append progress. R grants no
  reusable space and can be lost on recorder death.
- The bytes from D through P remain unchanged. A recorder may borrow them
  directly for writes because the producer cannot overwrite that range.

One producer means one non-reentrant execution context. Merely assigning a
lane to a process or thread is insufficient if a signal handler can interrupt
that thread's emission. No inherited child or new thread reuses another
execution context's lane. Lane binding happens during setup, not in a dynamic
metric registry or lazy first-emission registration.

Before entering a product critical region or starting an effect, the producer
reserves enough free bytes for that region's **entire bounded event burst**,
including success, failure and terminal paths. It acquire-loads D while no
product lock is held, checks its own P against capacity and obtains byte credit.
Within the region it copies raw fields into the prepaid bytes and release-stores
the new P only after a complete logical event is present. No wait, allocation,
lookup, user callback or syscall belongs inside that product critical region.
At its natural exit, outside owned locks, notification adds one atomic exchange
per complete publication burst and possibly one nonblocking send, as specified
below. Unused reservation bytes do not become a published hole. Reserving a
whole asynchronous workflow across a yield would introduce overlapping holes;
reserve one synchronous no-wait region or continuation instead.

At a physical wrap, copy to the ring end and then its beginning; there are at
most two physical spans for the complete byte record. Do not insert padding
events, per-slot fragmentation records or metric summaries. For discontiguous
source fields there can still be several field stores/copies: "two spans" is
not a dishonest claim that the whole capture has only two instructions.

The recorder acquire-loads P, reads only complete published bytes, writes them
unchanged and tracks partial write progress privately. After its flush succeeds
for a complete-record prefix, it release-stores D. The producer acquire-loads
that D before reusing the bytes. Release/acquire protects both publication
before reading and completion of reading before overwrite. `volatile` and a
wakeup alone do not provide either ordering. The Linux circular-buffer account
is useful corroboration for these two directions, but its kernel primitives
are not userspace C++ source to import. [Kernel authors' account](https://www.kernel.org/doc/Documentation/circular-buffers.txt).

An ordinary cached D may be used while already granted space suffices; refresh
it only to obtain more credit. That reduces cross-core loads without changing
the guarantee. Recorder R can advance ahead of D across grouped appends; the
producer must never use R to decide that space is free.

## Fullness, oversized events and waiting

When credit is unavailable, stop admission before the effect and wait or leave
the owner parked at a safe boundary. Preserve the pending source and all
published bytes. Do not reset D/P, disable instrumentation, aggregate, sample,
truncate or resume merely because the recorder died. A finite lane cannot accept
an indefinite event stream while the recorder or disk is stopped.

Every source site needs an audited bound on the event bytes emitted in a
no-wait critical region. If the owner cannot supply that bound, the proposed
site is incomplete: move the admission boundary or restructure the owner.
Blocking inside a product lock after the effect is not a backpressure design.

A known-size event larger than N cannot be reserved. On a safe admission slow
path, establish a new lane whose capacity accommodates the **whole** event,
preserve its discovery/setup entry, then enable capture. Do not grow a mapped
lane that a recorder is reading. Keep the old lane until drained. This is a
costly exceptional path to measure, not a secretly cheap hot-path allocation.
If size becomes known only after acquiring a bounded read, reserve the maximum
possible returned bytes before that read. For an inherently streaming source,
each actually returned source chunk is its own observed raw event; do not split
a preexisting logical event into independently publishable fragments and call
the result complete. Unbounded arbitrary custom payloads need an explicit
acquisition/ownership design; this report does not silently impose a maximum.

## Selected notification: armed exchange and a byte doorbell

Select an **armed atomic exchange plus a nonblocking byte doorbell**, not
continuous spinning or a periodic idle timer. Retain the unconditional
one-byte-per-burst doorbell as the simple comparison implementation: it costs
one producer syscall for every natural publication burst. The selected armed
handshake costs one producer RMW for every such burst and sends only when the
recorder has armed that lane for sleep. A natural burst ends at the existing
safe owner boundary; it is not a timer, record-count accumulator, artificial
batch delay or metric aggregation. A lone emission is a burst of one.

Use a native lock-free `armed` word for each lane, initialized before admission.
The recorder writes true with exchange; the sole producer writes false with
exchange. There is no initial plain-load shortcut on the producer. Both
exchanges use acquire/release for the stated first implementation. The producer
finishes all release-publications P for the burst, then always exchanges
`armed` to false. If the previous value was true, it sends one doorbell byte.
Do this notification after releasing product locks; report that exit latency
as part of the event's recorder lag. Published bytes are immediately available
to an already active recorder even before notification.

The recorder's idle transition is exact:

1. Finish its current available-byte drain and required disk acknowledgment;
   pending raw append/flush work is not idle.
2. Drain old doorbell bytes until the nonblocking receive reports no data.
3. Exchange each registered lane's armed word to true, then acquire-recheck
   each lane's published P against its private consumed R.
4. If any lane is ready, continue draining. If all are empty, wait on the
   doorbell and the independent process/control events. **Do not drain the
   doorbell after this final readiness check and before waiting.**
5. On any wake, repeat the availability checks. A byte says check channels;
   it is not a metric record, event count or acknowledgment.

The proof has two cases for each lane's exchange modification order. If the
producer's false exchange precedes the recorder's true exchange, the acquire
part of the recorder exchange receives that release, possibly through a chain
of RMWs, and its subsequent P check sees that completed publication or a later
one. If the recorder's true exchange precedes the producer's false exchange,
the producer observes true and sends a byte. If another producer exchange
intervenes and returns true first, that earlier exchange supplies the notice.
Thus a live completing producer cannot both be unseen by the final P check
and suppress the corresponding notice. This relies on actual RMW modification
order and release/acquire, not a hope that loads will soon notice stores.
[C++ draft atomic ordering/RMW rules](https://eel.is/c++draft/atomics.order).

Linux's `waitqueue_active` source gives the classical opposing-store/load
counterexample and requires a full barrier for its conditional wake shortcut.
It is corroboration for the danger; kernel wait queues are not imported.
The exchange handshake above is our explicit synchronization design and needs
its own direct weak-memory/interleaving checks before implementation.
[Pinned Linux v6.12 wait.h, lines 87–121](https://github.com/torvalds/linux/blob/v6.12/include/linux/wait.h#L87).

Choose an `AF_UNIX`, `SOCK_STREAM` socketpair for the one-byte doorbell. This
has the same small stream role as a pipe and permits signal suppression without
changing Blackbird's process-wide SIGPIPE disposition. The supported Linux and
installed macOS SDK provide `MSG_NOSIGNAL` and `MSG_DONTWAIT`; use a one-byte
send with those flags after setup has also established nonblocking, close-on-
exec endpoints. Check the target runtime directly. Apple's documented
`SO_NOSIGPIPE` is an available setup alternative on macOS; it returns EPIPE
instead of raising SIGPIPE. Do not silently change all child processes' signal
behavior to make one notifier safe. [POSIX send](https://pubs.opengroup.org/onlinepubs/9799919799/functions/send.html),
[Apple socket option contract](https://developer.apple.com/library/archive/documentation/System/Conceptual/ManPages_iPhoneOS/man2/setsockopt.2.html).

The launcher retains **both** socket endpoints without reading notices and
passes the sending endpoint to the producer process and receiving endpoint to
the sole recorder. Multiple lanes in that producer process share the doorbell;
each still has its own armed word. The recorder waits at a one-byte receive
threshold. Keeping both endpoints prevents recorder restart from destroying
pending notices or turning the producer's unchanged descriptor into EPIPE.
Replacing only the recorder obtains that same receiving endpoint from the
launcher, scans all retained lanes before sleep, and does not reset P or D.

A one-byte send succeeds only with return 1. EAGAIN/EWOULDBLOCK means an
unread notice already fills the healthy doorbell, so it can be coalesced safely:
the final-check ordering above prevents the consumer draining that notice and
sleeping without checking the new P. The metric bytes remain in the lane.
Retain the raw failed-send outcome in the prebound transport-failure lane;
publishing that failure never recursively invokes the failed notifier. Its
pending bytes are found on the already-required channel scan. Zero progress
and other errors, including EINTR, EPIPE,
ECONNRESET or a bad descriptor, are not evidence that a notice is pending.
Preserve their raw outcome, keep the original published bytes, stop further
owner admission at a safe boundary and ask the independent launcher control
channel to repair/rebind the notifier. If that control notification also fails,
preserve its failure too and remain fenced; do not advance D or disable capture.
Failure-lane space must be included in admission credit so a hard-error path
does not need allocation or recursive emission. Exhaustion is an admission
fence, not permission to replace failures with a counter.

Producer death between publishing P and completing its exchange/send is a
separate interruption case: no two-call protocol makes that interval atomic.
The recorder's wait set therefore includes an independent producer-process
death watch established before admission (Linux pidfd readiness; macOS kqueue
process exit watch), and scans that owner's retained P on death. A retained
socket writer endpoint intentionally prevents relying on EOF for this signal.
Linux v6.12 `pidfs.c:pidfd_poll` registers its wait queue before checking the
stable pid object's exited/empty thread-group state; no PIDFD_THREAD is used
when waiting for process-wide producer termination. Apple's EVFILT_PROC with
NOTE_EXIT supplies process exit notification; registration errors are checked
before admitting any producer work. These are platform mechanisms, not claims
that every kernel version implements the same lifecycle contract.
[Linux v6.12 pidfd poll source](https://github.com/torvalds/linux/blob/v6.12/fs/pidfs.c#L95),
[Apple kqueue contract](https://developer.apple.com/library/archive/documentation/System/Conceptual/ManPages_iPhoneOS/man2/kqueue.2.html).
Registration must handle an already-exited owner without missing its final
prefix; death and stale PID reuse are direct lifecycle checks. All notifier
holders dying together destroys the doorbell, not the named raw lanes: on
restart rebuild the notifier, discover and drain old lanes, and bind new owner
epochs before allowing new emission. A live producer whose notifier holders
died remains fenced until an explicit valid rebind; dead-recorder detection
alone does not grant new credit.

The selected producer path therefore adds **one acq_rel RMW per natural burst,
plus one nonblocking send on each armed wake**. On x86 a locked exchange has
ordering/cache-line cost; an ARM target may use a single atomic swap or an
exclusive-load/store retry sequence. Qualify the actual emitted instructions.
There is no periodic idle delay or silently dedicated 100% recorder core.
Measure contention, isolated wake latency and burst cost alongside disk lag.
The root's plain load/store/copy timing is a lower-cost baseline, not the
selected complete path's performance result.

## Named memory, death and restart

Keep the named object linked while any published byte can require recovery.
POSIX specifies that named shared-memory data persists until unlink and loss of
all references, including across process termination; reboot persistence is
unspecified. The shm namespace need not be enumerable as ordinary files.
[POSIX shm_open](https://pubs.opengroup.org/onlinepubs/9799919799/functions/shm_open.html),
[POSIX shm_unlink](https://pubs.opengroup.org/onlinepubs/9799919799/functions/shm_unlink.html).

An external setup process preserves a small discovery manifest containing each
exact leading-slash shm name, epoch, lane identity, capacity and capture/schema
binding **before** admitting its producer. Thus a new recorder can reopen lanes
when both old producer and recorder have died. Do not unlink immediately after
mapping and depend only on their descriptors. Do not use PID alone as identity,
reuse an old lane for a new owner, or truncate an existing object on restart.
Manifest/object creation interruption is handled as setup recovery; emission
was not enabled until the completed binding was preserved.

Only one recorder can acknowledge a lane. Enforce this in the external process
control plane, not a Blackbird product lock. A new recorder reconciles the disk
prefix with D and P before resuming reads. The case "flush completed, recorder
died before publishing D" needs either exact transport-prefix reconciliation
against stored lane/offset identity or explicitly preserved duplicate copies for
tertiary deduplication. It is not permission to acknowledge an unmatched suffix.
The storage owner's design must choose the exact recovery rule.

P advances only after full event capture. Producer death during a two-span copy
therefore exposes the earlier complete prefix and possibly unpublished tail
bytes. Those tail bytes are not a complete event and do not prove that every
already acquired fact was preserved. Recorder death after reading or short
writing does not free anything; replay starts from the reconciled retained
prefix. Disk-full or flush failure leaves D stationary and eventually stops
producer admission. Host crash removes any assumed RAM guarantee.

Retirement is external: drain through the final published prefix, verify the
owner cannot publish again, preserve the final transport state, then remove the
discovery entry and unlink. No TTL or dead-owner detection alone authorizes
discarding retained records. Forked children receive fresh bindings or cannot
emit until setup; they do not become a second writer by inheriting a handle.

## Atomic and integer contracts: qualified targets, not portable magic

The C++ working draft only **recommends** that lock-free operations be address-
free and independent of per-process state. It describes why that supports
shared mappings; it does not give an unconditional portable IPC guarantee.
Compiler atomic builtins can fall back to external runtime routines. Require
the exact deployed compiler/ABI to generate lock-free, address-free operations
on the aligned 64-bit cursor representation. [C++ draft atomics.lockfree/5](https://eel.is/c++draft/atomics.lockfree),
[GCC atomic builtin contract](https://gcc.gnu.org/onlinedocs/gcc/_005f_005fatomic-Builtins.html).

For macOS ARM64 and the supported Linux target, qualification must check type
size/alignment and compile-time lock-free capability, inspect the actual
load/store assembly and linked symbols for runtime fallback, and exercise
cross-process release/acquire publication and reclamation through different
virtual addresses. Repeat reopen/remap after the last producer/recorder exit.
Do not claim that is_lock_free alone proves IPC, and do not use C++ atomic
wait/notify as though it automatically wakes another process. A plain native
integer cursor accessed solely through the qualified atomic load/store ABI
avoids importing mutex/condition-variable state into the shared header.

POSIX additionally warns that the state of synchronization objects such as
mutexes, semaphores, barriers and condition variables becomes undefined when
their last mapped region disappears. A persistent shm name does not itself
qualify a persistent synchronization object. [POSIX mmap](https://pubs.opengroup.org/onlinepubs/9799919799/functions/mmap.html).

Keep physical wrap and integer wrap distinct. Address with P & (N - 1), but do
not silently wrap the absolute 64-bit byte cursor within one epoch. Before a
reservation could overflow, stop at a safe admission boundary, establish a new
epoch/lane and retain the old one until drained. Never reset P or D to zero in
the same identity. Use checked differences and lengths with N below 2^63 and
assert 0 <= P - D <= N; corrupted or incompatible control state stops admission.
Near-UINT64_MAX initialization belongs in the direct oracle, not a centuries-
away argument for leaving overflow unspecified.

## Work accounting and direct falsification

| Path | Producer work | Retention consequence |
| --- | --- | --- |
| Prepaid shared lane | Raw facts/clock reads required by site; field stores or raw copy; wrap arithmetic; one release P store per logical event | One retained copy, no reuse before disk D |
| Selected burst notification | One acq_rel armed exchange per natural burst outside product locks; one nonblocking send when it returns true | No idle spinning/timer; original bytes retained independently of notice |
| Obtaining lane credit | One acquire D load plus bounds checks, outside product locks | Can stop admission; no silent loss |
| Plain one-record pipe | Framing/raw stores; write syscall; OS copy and reader scheduling | Unread bytes retained, consumed bytes not replayable |
| Pipe plus crash-safe retention | Retained raw copy, pipe write, acknowledgments and replay | Adds transport to the retained-store mechanism |
| Unconditional comparison notification | One nonblocking one-byte send per natural burst | Simpler control baseline, syscall on every burst |
| Full/oversized lane | Safe-boundary wait or externally prepared larger lane | Exceptional cost; no cheap-path claim |

Before source implementation, test the transport state machine independently
of measurement semantics: complete publication, no overwrite before D,
monotone disk prefix and one producer/recorder owner. Test a tiny capacity with
every wrap offset, header/body split, zero and maximum payload, checked overflow,
bursts and slow/stopped recorder. Feed independently generated arbitrary bytes
and compare raw originals with preserved records; derive no oracle from the
emitter's own encoder. Inject partial disk writes, EINTR, zero-progress writes,
full disk and failed flush through the recorder's direct harness.

Kill the producer before/during/after publication, and the recorder before
read, after read, during append, after append, after flush and before/after D.
Kill both and reconstruct discovery. Test double-recorder refusal, PID reuse,
fork inheritance, interrupted setup/retirement and no progress until valid
disk acknowledgment. A SIGKILL test is not a host power-loss test. Recorder
storage qualification must separately falsify its actual crash durability.

For notification, force both exchange orderings, several bursts between arm
and final P check, notice-queue saturation, wake-before-wait, spurious wakes,
EINTR/zero/closed-peer errors, recorder replacement while the launcher retains
endpoints, and death at each publication/exchange/send instruction boundary.
Directly test that wrong drain-after-final-check ordering permits the lost-
wakeup counterexample, while the selected ordering does not. Check the cold
single-event wake and the active-recorder no-send path separately; wake tests
must use an independently known record sequence and a bounded failure deadline,
not a sleep that silently papers over the ordering race.

Measure producer latency including tail stalls, allocations, copies, cursor
loads/stores, clock reads, syscalls and wakeups; measure recorder CPU/drain rate,
disk lag and backlog simultaneously. Compare pipe and retained byte lane under
the same workload and disk contract. No metric aggregator, registry, callback,
formatter or transformer enters either transport candidate.

## Acquisitions and exact restoration

All new material is study-only under ignored
`quarantine/raw-transport-2026-10-10/`. Archives and downloaded HTML remain
intact; extracted source is separate. No nested Git metadata or filesystem
detritus was found in these release archives. Existing V7 and Lamport acquisitions
remain at the paths and hashes in the earlier study and old-Unix manifest.
Browser access to the POSIX pages returned 403, but direct author-site curl
downloads succeeded and were read locally. Browser access to the multilog
manual returned 502; the complete acquired source was the reference actually
used. Direct pidfd man-page downloads timed out; the pinned kernel source's
complete poll function and the primary author page retrieved by browser search
supplied the lifecycle contract instead. Those failed download jobs were
terminated, with no reference process/build left running. These are acquisition
limits, not substituted claims.

| Archive/HTML in archives/ | Source/version URL | SHA-256 |
| --- | --- | --- |
| daemontools-0.76.tar.gz | https://cr.yp.to/daemontools/daemontools-0.76.tar.gz | a55535012b2be7a52dcd9eccabb9a198b13be50d0384143bd3b32b8710df4c1f |
| qmail-1.03.tar.gz | https://cr.yp.to/software/qmail-1.03.tar.gz | 21ed6c562cbb55092a66197c35c8222b84115d1acab0854fdb1ad1f301626f88 |
| s6-2.15.1.0.tar.gz | https://skarnet.org/software/s6/s6-2.15.1.0.tar.gz | eab9c46e22b66b16135f9a05ec68a0ea287d9060b84d10defaaa2caad158ab52 |
| skalibs-2.15.1.0.tar.gz | https://skarnet.org/software/skalibs/skalibs-2.15.1.0.tar.gz | f9c905e74935c6fe911c7e344e3e89d5fbd2014c1a04650b524b15ce9b5635d1 |
| posix2024-write.html | https://pubs.opengroup.org/onlinepubs/9799919799/functions/write.html | 1b45e6a71f8bc9bff702da5633d42c88fd927c1d0efc5bafe7a939d23eb66e6a |
| posix2024-read.html | https://pubs.opengroup.org/onlinepubs/9799919799/functions/read.html | af724071475054a17dde265a540bc39bd6e5d06da2373729141280adb8fdd744 |
| posix2024-shm_open.html | https://pubs.opengroup.org/onlinepubs/9799919799/functions/shm_open.html | 63714034ef7a3eaa5d7858f791d0a22329b4182b1bb5bab66f85dc8d03b4255e |
| posix2024-shm_unlink.html | https://pubs.opengroup.org/onlinepubs/9799919799/functions/shm_unlink.html | 62c11e8361fe36d2fef8d82f2daa07bc23cf551a22ad3d5ec874e4b14163110b |
| posix2024-mmap.html | https://pubs.opengroup.org/onlinepubs/9799919799/functions/mmap.html | 6e78132f706b5ab3041414cbc98f597f12c5f851679841a9723b36b0fbe66414 |
| cppdraft-atomics-lockfree.html | https://eel.is/c++draft/atomics.lockfree (moving draft, acquired 2026-10-10) | 9e1c2232dc4cc24b91b9d174adbf6ec13accd21d466ce6b82c7654201115de54 |
| cppdraft-atomics-order.html | https://eel.is/c++draft/atomics.order (moving draft, acquired 2026-10-10) | cb5189ec62b0448cf9fb803bd8db57455827065de4768d6c227d2944c11a153b |
| linux-v6.12-wait.h | https://raw.githubusercontent.com/torvalds/linux/v6.12/include/linux/wait.h | 0433279c86b2f34d1a5dce39365f176a571ce8a44d935a542e90c3506b4fe627 |
| linux-v6.12-pidfs.c | https://raw.githubusercontent.com/torvalds/linux/v6.12/fs/pidfs.c | fae3fa65abd35998c65d111b73f4acd7aa3b2ecf6bea2a18f573dd93860e3334 |
| posix2024-send.html | https://pubs.opengroup.org/onlinepubs/9799919799/functions/send.html | c653e5014b769cb0ba2cd8f106437bf714cc9fbbb7c04b2cf4dd7d6ed6a5d922 |
| apple-setsockopt.html | https://developer.apple.com/library/archive/documentation/System/Conceptual/ManPages_iPhoneOS/man2/setsockopt.2.html | 2a215a3b64f031cdeb9ec93ebfc16854738131ee68ce16314d5ebd5b6d0512ae |
| apple-kqueue.html | https://developer.apple.com/library/archive/documentation/System/Conceptual/ManPages_iPhoneOS/man2/kqueue.2.html | 89581e15d32ad4f9303241a39d5d8e63fd0fd88e4ea446a0d630c958d65c73ef |

Restore from the repository root: create the `archives` and `extracted`
directories under the path above; `curl -fL URL -o archives/FILENAME` for each
row, verify `shasum -a 256 archives/FILENAME`, then extract each tar archive
with `tar -xzf archives/FILENAME -C extracted`. Daemontools' source root is
`extracted/admin/daemontools-0.76/src`; the other source roots are
`extracted/qmail-1.03`, `extracted/s6-2.15.1.0/src`, and
`extracted/skalibs-2.15.1.0/src`. Preserve the archives and strip any nested
metadata/detritus from future extracted replacements. A changed upstream HTML
hash is not exact restoration; the acquisition-day bytes remain the study pin.
