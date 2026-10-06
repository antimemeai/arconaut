# U1: retained originals and effect admission

Operator sequencing correction, 2026-10-02: remaining advanced U1 work is deferred
until after the first useful coding session. Bootstrap U2/U3 use the existing
single-journal RetainedState path; earlier requirements here to wait for the complete
RetainedEnvironment owner are superseded by [the current plan](IMPLEMENTATION_PLAN.md).
Keep this work and its evidence; no completion or continuation qualification is claimed.

2026-10-01. Working sub-plan for the next whole unit in IMPLEMENTATION_PLAN.
U0 code review must resolve before this unit uses its foundation. This sub-plan
receives independent Kimi review before its red cases and product implementation.
No storage library is adopted. The purpose is a single retained truth for later
CLM, workflows, effects, complaints and refit, not a second logging framework.
[Environment-head refinement](U1_ENVIRONMENT_HEAD_SUBPLAN.md) supplies permanent
writer authority and durable selection; [selected-chain continuation](U1_CONTINUATION_SUBPLAN.md)
is the next reviewed refinement before its implementation.

## Grounding and scope

Read CORE_DESIGN's operation lifecycle and original-audit contracts, TESTING_PLAN
T1/T3, the audit-state and refit-custody studies, actual LevelDB log writer/reader
and installed Apple SDK fsync/fcntl manuals. LevelDB's flush/sync separation and
sync-error write closure are useful mechanisms; its corruption salvage policy is
inappropriate for a complete original audit. Own framing, codecs and recovery.
No quarantined source is imported. Source grounding is collected in
[refit custody](../papers/2026-10-01-refit-custody-grounding.md).

Initial supported storage is one restricted local directory and one append-only
journal file, with a single writer and bounded in-memory indexes. There is no
automatic rotation in this first unit: the configured journal limit closes new
admission visibly, retaining every available byte. U1 owns an explicit linked
continuation operation for capacity or damaged pending tails. This is available
to operator/model recovery code, with no new approval gate. Inline source chunks
avoid a blob transaction.
Defaults: 64 KiB payload/frame, 1 MiB encoded batch, 512 MiB journal; callers may
set smaller limits for direct boundary cases. No unchecked length drives allocation.
Configured batch limits must accommodate a maximum-size source frame plus its
32-byte header and 56-byte commit: max_payload <= max_batch_bytes - 88.

Qualification covers modeled persistence faults and actual process crash on the
named Mac/Linux profiles. Successful full-sync calls are observed; power-loss
survival on a particular filesystem/device is not claimed without that experiment.
This weaker release statement is explicit, not a replacement of a failed test.

## Concrete retained format

All integers are little-endian, encoded field by field; never memcpy C++ structs.
CRC32C uses reflected polynomial 0x82f63b78, initial/final XOR 0xffffffff.
The independent known vector `123456789` yields 0xe3069283. CRC detects accidental
damage; it is not authentication or a substitute for retained payload bytes.

The 112-byte file header consists of magic `ARCOJ001` (8), environment ID (16),
journal ID (16), issuer namespace u64, predecessor journal ID (16), predecessor
validated commit sequence u64, predecessor validated end offset u64, format
version u32=1, max frame payload u32, max batch bytes u32, flags u32 (root=1,
continuation=2), 12 reserved zero bytes, and CRC32C u32 over its first 108 bytes.
Flags must equal exactly 1 or 2; zero, combined or unknown bits reject open.
Root predecessor fields are zero. A continuation links the named predecessor's
validated prefix; its own frame sequences start at 1 and queries carry journal ID.
Environment identity is unchanged across that chain. The environment/journal
identity and bounds must match the opener's declared profile. A truncated or
invalid header never becomes a fresh empty journal. Existing damaged files stay
intact. Native new-file creation uses exclusive creation and restrictive mode.

Each frame has a 32-byte header: magic `ARJ1` (4), version u16=1, kind u16,
sequence u64, batch-first-sequence u64, payload length u32, CRC32C u32.
The CRC covers header bytes 0..27 followed by exact payload bytes. Kinds are
source=1, semantic=2, commit=65535; unknown version/kind rejects the continuation.
Sequence starts at 1 and advances for every frame, including commits, without wrap.

A nonempty batch has one or more source/semantic frames followed by a commit.
Commit payload is 24 bytes: first sequence u64, non-commit frame count u64,
CRC32C over all preceding encoded frames in this batch u32, reserved u32=0.
Commit's sequence and batch ID must agree with the contiguous preceding frames.
Semantic frames begin with schema version u16=1, event kind u16, dependency
count u32, then that many references (journal ID 16 + source sequence u64), then
the event-specific body. Bound count by payload length before allocation. During
replay, check each reference against an earlier authoritative source or a source
in this batch, before publishing the batch. Dependencies may cross predecessor
journals in the validated chain, never a different environment or unrelated file.
A commit whose required earlier source is missing cannot publish anything.
No scanning past damaged material to call a later frame authoritative.

The journal API returns committed records and a cursor; indexes rebuild solely
from these records. A recovery report separately exposes available pending bytes,
first invalid offset and cause (torn, checksum, sequence, format, semantic conflict).
Original bytes are readable from storage even when their batch is not authoritative.
Complete surviving batches can be discovered after an unacknowledged sync/write.
All reopened batches are labelled recovered, with acknowledgement delivery unknown
(there is no on-disk bit that can distinguish a delivered acknowledgement). Recovery
first stages them without publishing, performs a new successful selected sync,
then rebuilds indexes with that explicit recovered status.
The recovery sync uses the same declared strength as live commits (full on Mac),
never a weaker silent substitution.
This makes surviving bytes authoritative now, not proof that the earlier sync/ack
occurred. Until this
step succeeds admission stays closed even on a structurally clean file. Open
attempts enter the reconciliation set, and can never acquire a fresh dispatch
permission merely by reopening or resubmitting. Their IDs are discoverable in the
staged reconciliation set even before recovery publication. Normal append on a
clean file resumes only after recovery sync and explicit owner reconciliation;
U3 supplies actual living-resource evidence rather than inferring custody from disk.

## Writer, publication and failure

Before the first write, validate lengths, sequence budget, source dependencies,
semantic transitions and index capacity; prepare all allocating publication state.
An allocation error at preparation leaves storage and indexes unchanged. There
is no allocating publication step after sync succeeds. Append loops handle short
writes and EINTR; zero progress or an impossible adapter count is an error.
Offsets/size arithmetic is checked before issuing storage calls.

Implementation layers within this one owning unit: `FramedJournal` owns one
physical file, bounded frame indexes, batch append and structural recovery.
`RetainedState` privately owns that journal and the semantic ledger, validates
event bodies/dependencies before append and before recovery publication, prepares
all ledger allocations before writes, and swaps prepared state after successful
sync. Physical frame inspection is explicitly not an authoritative context/ledger
query; only RetainedState supplies that consumer API. Keeping framing separate
does not create a second writer, persistence system or independent admission gate.

Write every source/transition frame, then commit, then the declared sync. Publish
indexes/ledger state and return success only after that sync succeeds. There is
no external dispatch inside the framing writer. Any partial append, interrupted
or failed sync has unknown durability, poisons that writer and closes admission.
It must not retry the same batch as if nothing was written. A reopened file with
an incomplete/damaged tail permits inspection but no append/admission on that
file. U1's continuation creates a fresh exclusively created journal whose header
links the validated predecessor prefix and whose first batch records the recovery
choice, old diagnostic range/limits and reconciled attempt dispositions. Sync the
available predecessor, then the new header/batch, then its parent directory before
publishing the new active writer. Any failure leaves admission closed. Preserve
old pending bytes and rebuilt committed dedup state; never truncate originals or
silently repair the file. Invalid/missing predecessor headers or an untrustworthy
acknowledged prefix block continuation as a complete history; expose that loss
instead of guessing the chain. Capacity-only rejection has no partial write and
does not poison/corrupt the journal; the same explicit continuation can extend it.

U1 adds `JournalDirectory` with `create_exclusive`, `open_existing` and
`synchronize_directory`, plus move-only `NativeJournalFile : Storage` exposing
`lock_writer`. Native construction checks permissions/type; injected directory
operations let tests fail publication directly. Existing creation returns conflict,
lock contention returns busy, unsupported synchronization returns unsupported,
and other native failures return io with errno detail. These add conflict/busy
error codes in U1; the narrower U0 data-I/O interface is unchanged.
Native storage owns a move-only close-on-exec descriptor and an exclusive writer
lock, checks regular-file status and restrictive permissions, uses pread/pwrite,
checks off_t bounds, and propagates native error detail. Low-level exclusive
creation returns an opened file, without a durability/publication promise. Journal
initialization/continuation orchestration syncs its contents and parent directory
before publishing that journal or returning durable initialization success.
Mac full sync is F_FULLFSYNC, with no
fsync fallback under a full-sync request; data sync uses fsync. Linux uses fsync
for this local profile, including directory publication. Unsupported/error returns
remain errors. Tests exercise actual invalid/read-only descriptors and filesystem
errors as well as injected failures. Writer locking does not authorize taking
over control of an already living worker; U3 owns that reconciliation.

## Semantic state and identities

Semantic payloads have their own versioned bounded codecs, exact required fields
and no ignored trailing bytes. Retain source bytes and transitions atomically.
The ledger exposes checkpoint decisions, invocation/attempt input, local phase,
observed disposition and original sequence references, not a mutable shadow log.

The initial semantic body kinds are reservation=1, decision=2, invocation=3,
attempt-admitted=4, attempt-open=5, observation=6, retry=7, identity-conflict=8,
complaint=9, rejected-submission=10, and adapter-receipt=11. A reservation contains counter u64. A decision contains its ID,
actor, conversation, workflow, definition and context IDs (six 16-byte identities),
u32 planned-invocation count plus those IDs, and u32 continuation length plus bytes.
An invocation contains invocation/decision/definition IDs and length-prefixed exact
resolved input. Admission contains attempt/invocation/decision IDs and the exact
per-attempt resolved input. Open contains an attempt ID. Observation contains
attempt ID, phase u8 (running=1, settling=2, terminal=3), disposition u8 (none=0,
success=1, failure=2, cancellation=3, unknown=4), two reserved zero bytes, and
length-prefixed observation bytes. Only terminal has a non-none disposition.
Retry contains decision/invocation/new-attempt IDs. Conflict contains disputed
kind u16 (decision/invocation/admission), disputed ID and length-prefixed rejected
proposal bytes. Complaint contains complaint/actor IDs and length-prefixed detail.
Rejected-submission contains reason u16 (the declared ErrorCode value), reserved
u16=0 and signed detail i64. Rejection/conflict envelopes may reference source
chunks holding the exact rejected serialized proposal; conflict's empty inline
proposal then means concatenate its dependency chunks. This preserves a proposal
at the frame limit without requiring a larger conflict frame or clipping input.
If framing/dependency overhead prevents the complete rejection fitting one batch,
preflight all lengths, source references, record/sequence/file budgets, then retain
its ordered source chunks in bounded source-only batches followed by one marker
referencing all chunks. These batches confer no dispatch permission. Only the final
marker signifies complete retention of the rejection. A failure after any prefix
has been written closes ordinary admission and leaves the retained partial originals
explicit; it must not return conflict as though complete capture succeeded.
The marker must itself fit one frame (24 bytes per dependency plus its kind's
fixed body/envelope). An unrepresentable proposal fails preflight explicitly,
without writes or a change to admission health. Retention completes synchronously
without unrelated submissions interleaving between its chunks and marker. Both
conflict and generic rejection use this form. Replayed source-only chunks without
a marker remain readable originals; they confer no rejection semantics, and replay
does not invent a marker. A torn/pending marker follows normal recovery uncertainty.
Adapter receipt contains attempt ID, success u16 (0/1), reason u16 and signed
detail i64. Success requires zero reason/detail; failure carries an actual Error.
This receipt is independent of execution phase and may arrive after synchronous
settlement; it never regresses phase or treats a local return as terminal success.
All variable counts/lengths are bounded against remaining payload before allocation;
identities, counters and source-reference sequences are nonzero. Encoding rejects
invalid combinations, and decoding rejects unknown schemas/kinds and trailing bytes.
These codecs express retained facts; the owning ledger, not successful decoding,
validates dependencies, checkpoint membership and state transitions. Continuation
and later owning-unit events add explicitly specified kinds rather than accepting
opaque unknown semantic records.

The owning API prepares a batch against an explicit expected journal cursor; all
source drafts precede its semantic events and receive deterministic references.
A stale cursor rejects the preparation. Resubmission by identity compares exact
resolved fields and dependencies, returning its existing state without writing.
The owner caches facts/sources derived from the journal, prepares a complete
candidate cache before I/O, and publishes with a nonallocating swap after sync.
If I/O becomes uncertain, its prepared identities remain discoverable as uncertain
RAM state while admission is closed. Reopen rebuilds staged facts before new sync,
and admitted/open attempts remain nondispatchable after reconciliation. An injected
custody verifier supplies U3's actual reconciliation evidence; disk never grants it.
Dispatch retains open and consumes permission before calling the effect adapter.
Its report separates the actual adapter result from success/failure retaining the
receipt; adapter failure never manufactures terminal settlement. Reentrant dispatch
uses an owned input snapshot so an adapter's callback cannot invalidate its bytes.

A decision records actor, conversation/workflow, resolved definition/context,
planned invocation IDs and continuation bytes before admitting its effects.
Submission of the same decision/invocation/attempt with identical resolved fields
returns existing state. Reusing any identity for changed input/actor/parent/version
is a retained conflict while recording is healthy; it cannot overwrite the first
fact or dispatch. A new attempt under an invocation requires an explicit retained
retry decision. Fresh unrelated IDs never pretend to deduplicate a prior action.
Each additional retry of one invocation requires a fresh decision: the same
(decision, invocation) pair cannot authorize another attempt. One decision may
plan retries of different invocations. Repeated nonterminal observations have no
event identity and remain separately received facts. Within-batch changed identity
uses the same conflict kind as changed identity against already committed facts.

Retain attempt-open before dispatch; dispatch is permitted once only in the live
owner, and the permission is consumed before calling the effect seam. Adapter
failure may mean an external effect occurred; retain local observations separately.
Recovery treats open/admitted attempts as requiring reconciliation and cannot
automatically dispatch them. Terminal observations have explicit success, failure,
cancellation or unknown, with stream/resource settlement completed by U3 before
it supplies terminal state. Later input/signals/messages are new admitted effects.

The retained issuer reserves a monotonic nonzero 64-bit allocation counter in a
committed semantic record before returning a new typed 128-bit identity. Its bytes
combine a retained nonzero 64-bit issuer namespace with that counter. Counter is
shared across all issued tags and never wraps. Rebuild consumes the maximum counter
over committed/recovered reservations and fully decoded CRC-valid pending reservation
frames; uncertain reservations are burned, never reused. A conformant issuer returns
an identity only after successful sync; recovery still must not assume delivery from
that fact. New linked journals receive a nonzero OS-entropy namespace checked against
every namespace in the intact predecessor-header chain, so counter restart cannot
reuse an older identity even if pending counter bytes are damaged. Collision outside
the retained environment is probabilistic and declared. Missing/corrupt chain headers
block this same-environment non-reuse claim; no silent random replacement of identity.
Failed/uncertain reservation never returns a usable identity.
Import/recovery does not reinterpret caller-supplied identical bytes as new IDs.

## Recording failure, complaints and emergency controls

All ordinary semantic/effect admission shares the journal health gate.
In-process recovery after incomplete receipt recording uses the explicit linked
continuation path; until that path is implemented, restart/reopen/reconciliation is
the implemented route. It never silently clears the failed recording interval.

Retain complaint text and available state references in that journal when healthy;
delivery to real beads/external DB remains U9. If unavailable, a preallocated
bounded in-memory emergency slot records control/complaint detail and explicitly
marks it provisional/unrecorded. Overflow rejects new detail and reports that
loss; it does not silently clip originals or claim complete capture. Process
death may lose this RAM material. Recovery can report uncertainty, not invent it.

Emergency stop/detach reaches the injected control seam even when persistence
fails, labels any unaudited interval, and reports the actual control result.
U3 connects it to real owned processes. This is degraded control, not permission
for ordinary unrecorded execution or a second persistence system. Bounded producer
buffer/pause custody is implemented with the real producers in U3.

## Direct execution and review

First write red journal/ledger cases against missing production APIs. Expected
format bytes and decoded facts come from this layout and named histories, not
round trips alone. Exercise every byte-length cut in small batches, wrong length,
CRC/sequence/version/kind/reserved fields, valid later commit behind damaged earlier
source, duplicate and changed-input histories, sequence/issuer exhaustion and OOM.

A scripted storage retains separate submitted and synchronized byte sets and can
survive reordered/partial pending writes, not just a prefix. Enumerate cuts at
source/commit/sync/publication/ack/dispatch; known acknowledged earlier batches
survive, pending outcomes are limited to the specified uncertainty. Fake externally
counted effects stay exactly one for retry/restart with retained identities and zero
for conflicts or failed admission. A complete recovered open attempt never repeats.

Actual child processes use native files and die at relevant boundaries; another
process inspects exact committed bytes and state. Lock contention prevents two
writers. A sync-failed but complete batch returns recovered-pending on duplicate
submission before recovery sync, then recovered/reconciliation-required afterwards;
it never returns not-found/dispatch permission. Torn-tail continuation preserves old
bytes, indexes and issuer non-reuse, with exact header lineage and failure at every
new-file/directory publication point. Test complete unsynchronized reservations
as consumed and the next returned ID as distinct; no case pretends a conformant
issuer returned an ID before its required sync. These are process-crash cases,
not power failures. Exercise real sync
success/failure and directory publication on both declared host profiles.
Debug/release, ASan/UBSan and appropriate direct concurrency/resource cases precede
fresh independent Kimi code/oracle review; integrate findings and reread. Do not
run mutation campaigns or qualify unrelated cloud machinery on this route.

The execution-boundary crash case uses an independent native effect file containing
the exact attempt identity and resolved input. SIGKILL occurs either upon adapter
entry before that file is touched, or after its successful full sync but before
the adapter returns. The parent requires the actual signal, checks zero or one
exact effect records, reopens and reconciles the original attempt, and checks
resubmission leaves the effect file unchanged. Only a new retained retry decision
and new attempt may append another record. This qualifies process-crash dispatch
consumption, not power-loss survival or U3's future living-worker custody checks.

## Bounded original-byte inspection refinement

`FramedJournal::read_original_range(offset,length)` and RetainedState's forwarding
API return `ObservedJournalBytes {journal, offset, observed_extent, bytes}`. This
is an owned byte copy from current storage, explicitly unvalidated: not a semantic
fact, source reference, or permission. It is available in every writer state and
never changes recovery/publication/admission state. It includes headers, damaged
frames and incomplete tails; source() keeps its authoritative membership/CRC rules.
Before allocating, require length <= header.limits.max_payload, then obtain actual
extent and require offset <= extent and length <= extent-offset (checked subtraction,
no wrapping). Empty range at EOF is legal. Larger originals are read in successive
bounded chunks. Allocate the exact checked size, read positional bytes with short
I/O accumulation and at most eight consecutive interrupted calls, then return the
actual observed extent with the copy. Zero progress before completion is Incomplete,
impossible read counts are Io, allocation failure is Allocation. Preserve bytes and
writer state on every failure. This is an observation, not an atomic snapshot
against arbitrary concurrent operator-owned writes or a new persistence claim.

Direct oracles reconstruct a known incomplete batch byte-for-byte in bounded pieces,
check exact offsets and observed extent, and show no authoritative record or append
permission appears from inspection. Exercise every actual length cut beyond the
header, range overflow/EOF/payload bounds, short/interrupted reads, premature zero,
impossible counts, real heap failure and read after Poisoned/Blocked. Public root
forwarding must yield identical originals while source of pending bytes stays closed.
Independent Kimi refinement review precedes code; code/oracles are rereviewed after
green qualification. This small reader does not implement selected-chain replay.

Inspection review clarifications: use a separate bounded reader loop; leave existing
scan/read_payload interruption semantics unchanged. Range/payload violations return
InvalidRange before reading bytes; the eighth consecutive interruption returns the
actual Interrupted error, and progress resets this count. Extent interruption uses
the same eight-call bound. Include RetainedState's synthesized Blocked state after
recording_failed_ while its physical journal remains Live: inspection still works
and never resets the gate. Kimi found no blocking design flaw and approved
implementation once these three explicit gaps were resolved.
