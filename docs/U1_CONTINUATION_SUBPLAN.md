# U1 selected-chain continuation

Operator sequencing correction, 2026-10-02: remaining advanced U1 work is deferred
until after the first useful coding session. Bootstrap U2/U3 use the existing
single-journal RetainedState path; earlier requirements here to wait for the complete
RetainedEnvironment owner are superseded by [the current plan](IMPLEMENTATION_PLAN.md).
Keep this work and its evidence; no completion or continuation qualification is claimed.

2026-10-01. Concrete next refinement of retained-state U1, using the implemented
EnvironmentHead permanent authority and bounded original-byte inspection. This
has completed independent pressure testing with the resolutions below and is ready
for direct red cases and implementation. Whole U1 remains unfinished.

Current implementation: exact maintenance codecs and complete RAM proposal ownership
have direct Mac/Linux qualification and resolved independent code/oracle review
(see the [resolution](../papers/2026-10-02-u1-continuation-prerequisites-resolution.md)).
Selected-chain ownership/replay is being implemented. The first construction stage
has a single accumulated owner, namespace-qualified reservations, historical source
routing and a three-segment fixture, with [resolved independent review](../papers/2026-10-02-u1-environment-construction-kimi-final-rereview.md).
Mac debug/release/ASan+UBSan and focused analysis pass. Initial capture/index/query
paths now preserve complete non-attempt packets, classify chunks before ordinary
replay and distinguish packet positions from semantic records; code review is
underway. Attempt-related captures and nonempty checked lists still refuse Unsupported
until the complete custody collector is integrated. Entropy and live candidate/head
publication remain unfinished.
Format decoding or this construction stage alone is not a continuation.

## Purpose and ownership

Extend a full or damaged journal without destroying originals, repeating an old
effect, losing an in-memory uncertain identity, or forking competing writers.
`RetainedEnvironment` becomes the ordinary U2/U3 consumer API. It owns one
EnvironmentHead for its entire lifetime and therefore one permanent environment
writer lease. Root RetainedState/FramedJournal remain internal segment machinery.
There is no separate audit log, approval step, storage library, background database,
or automatic rotation. Explicit continuation is a normal operator/model operation.

The head owner supplies its stable borrowed directory to segment objects. Keep old
FramedJournal objects/descriptors alive for source routing and original inspection;
keep only one accumulated semantic snapshot, not a full copy per historical segment.
A borrowed directory facade, owned alongside each segment if needed by existing
factory shape, never acquires/replaces the permanent authority lease. All allocation
for a candidate snapshot, segment vector and publication is done before I/O that
can publish a new head. Last publication is a nonallocating owner/cache swap.

Journal filenames are derived from their 16-byte ID, `journal.<32 lowercase hex>`.
An environment has a configured bound on segment count and retained indexes. These
are visible capacity refusals; increasing a configured bound remains normal model/
operator programmability. Root create takes an explicit immutable root header;
open takes the same expected declaration and never infers a new environment from
missing metadata. Initial head selector publication occurs only once the root journal is prepared and
synchronized; authority acquisition precedes root creation. Ordinary work waits for both. Partial creation files remain evidence.

## Environment owner implementation refinement (review resolved; implementation underway)

Independent [owner review](../papers/2026-10-02-u1-environment-owner-plan-kimi-review.md),
[rereview](../papers/2026-10-02-u1-environment-owner-plan-kimi-rereview.md),
[final focused review](../papers/2026-10-02-u1-environment-owner-plan-kimi-final-rereview.md)
and [correction](../papers/2026-10-02-u1-environment-owner-plan-kimi-correction.md)
resolve its findings. Initial Head prepare/publication phases have direct red/green
cases and [resolved code/oracle review](../papers/2026-10-02-u1-head-preparation-kimi-rereview.md).
Mac debug/release/ASan+UBSan and focused analysis pass. Linux qualification is pending:
Neuroses SSH resets before tests. The whole environment owner remains unfinished.

This implements the whole continuation invariant above; the steps below are internal
construction order, not separate acceptance of a weaker environment. U2/U3 use
RetainedEnvironment, not the standalone root-only factories. Keep the existing root
suite as direct regression evidence for its semantic machinery while extending that
machinery once, rather than writing a second admission validator.

### API and ownership

RetainedEnvironment create/open take an owned JournalDirectory, exact root header,
explicit root JournalCapacity and EnvironmentBounds. EnvironmentBounds contains
JournalLimits framing ceilings, JournalCapacity per-segment scan/index ceilings,
size_t max_segments and max_history_entries, and u64 max_capture_bytes. All bounds
are validated against actual vector/native/host types; impossible values refuse
before file I/O. Creation validates the exact root through the existing immutable
header encoder (including nonzero issuer namespace), root-without-predecessor rules,
configured framing ceilings and root capacity before acquiring authority or publishing
anything. Invalid root input therefore cannot manufacture an environment that open
would reject. Use full sync on the declared current hosts. Header/segment names
are derived internally; ordinary callers cannot nominate an unrelated filename.

Expose submit(single RetainedEvent), submit_proposal(expected cursor, source views,
event views), dispatch, attempt, source, issue<T>, committed_facts, provisional
originals, state/cursor/report, selected-journal original-range inspection,
confirm_recovery and reconcile. submit_proposal returns an owned result containing
the resulting cursor and ordered Submissions (existing/record/evidence) for events;
prepare the result storage before append, so acknowledgement cannot fail on result
allocation. Uncertain event records are descriptor-derived predicted provenance,
not claims that those original physical records were acknowledged or even written.
Their evidence discriminant must accompany every query/result; they never confer
source/parent/effect authority. Its cursor is the current active cursor; existing is an explicit proposal
flag, with optional original capture descriptor for provenance. Original event record
references are separate from the active cursor, including for a source-only capture. An exact purpose1 captured ordinary proposal returns its original event records
with Uncertain evidence and zero writes/dispatch; exact individual resubmission
uses the same full event/dependency comparison. A source-only captured proposal has
an empty event list but still returns existing proposal provenance/current cursor.
A mixed proposal containing both an exact uncertain identity and new events is
refused Conflict without admitting either part: do not promote its uncertain part or
silently renumber/split the submitted original. Retain its outer rejected proposal
through the ordinary diagnostic path with reason Conflict, subject to actual audit/
capacity failure; it is not a changed-identity claim when the event itself is equal.
Submit the new work separately with fresh identities. Changed identity reuse still
retains the corresponding identity conflict rather than treating it as exact. The same
rule covers additional/changed sources around equal uncertain events: only an exact
captured whole proposal or an exact individual event is a no-write uncertain result.
Do not confuse equal event bodies with an equal full source/event proposal.

Dedup comparison precedes active-profile size refusal. Compare individual typed events
against retained full bodies/dependencies; compare full proposals using bounded
canonical encoding under independently configured framing ceilings and the applicable
capture/proposal byte ceiling, then exact originals. A smaller new active payload
limit cannot make an old exact captured submission fail before lookup. Only a genuinely
new append is checked/encoded under the new active header/profile. The current cursor
in a dedup result is unchanged; original predicted references remain separately typed
as Uncertain provenance. Within one ordinary proposal, exact repeated event identities
use their first matching record/evidence rather than creating two semantic facts;
result order still follows input order. Whole proposal originals retain all positions.
Physical frames retain the submitted positions, including exact duplicate event frames,
as the accepted root append does; semantic application publishes only the first matching
fact. Do not suppress/renumber those physical frames: provisional prediction remains
first_sequence + source_count + event_ordinal under the unchanged ARPROP01 layout.
Stale uncaptured proposals retain ordinary rejected-input evidence as in the root owner.

For a current-cursor, source-free proposal whose events are all exact ordinary
existing identities, return an existing proposal with unchanged cursor and no writes;
perform that lookup before encoding under the active profile or demanding ordinary
write readiness. Recovered-pending/uncertain evidence remains explicitly typed;
this pure dedup query is available in any physical state while the head is healthy,
no continuation is latched and no transaction is underway. It never confirms recovery
or permits dispatch. A stale uncaptured
cursor still takes the rejection path. Per-event `existing` in a genuinely new
proposal is distinct from the whole-proposal flag: new sources or any new event
require preserving all submitted physical positions. Never filter existing entries
from a mixed new proposal, because that would renumber its retained originals.

continue_journal takes explicit candidate JournalLimits/JournalCapacity, configured
bounds (which may be increased), CustodyVerifier and injected JournalEntropy.
JournalEntropy fills an owned 24-byte array; NativeJournalEntropy calls getentropy
without allocation, with no time/PID fallback. Snapshot/segment/configuration changes
are published only after complete candidate/head acknowledgement. Failed continuation
leaves ordinary work closed, surviving proposal RAM intact and original/candidate
files inspectable; healthy known-head failure permits explicit fresh-candidate retry,
uncertain head publication requires reopen. No automatic retry/rotation is added.
Latch continuation_required at the start of continuation separately from the temporary
reentrancy guard; it remains set on failure and gates ordinary mutations, dispatch,
confirm/reconcile and issue. An internally Recovered journal cannot bypass that latch.
A successful candidate/head/cache publication clears it; a fresh open independently
establishes actual selection. Unhealthy head reports Poisoned; the known healthy head
with a failed continuation remains Blocked. Startup recovery has its distinct staged
state and can reconcile only a completely valid selected chain, with no damaged tail
or missing capture set. These are composed views/latches over the existing physical
JournalWriterState, not a second independently advancing enum. Pure input validation
occurs before any candidate storage I/O.

RetainedEnvironment owns EnvironmentHead before its internal semantic owner, so the
head/directory outlive every borrowed segment descriptor. Internally reuse one
RetainedState semantic owner through a forwarding directory facade borrowing
EnvironmentHead.directory(). The facade forwards storage operations without owning
or acquiring the permanent lease. RetainedState gains owned historical FramedJournal
objects, plus its active FramedJournal, under that same stable facade. A narrow
friend relationship allows the environment constructor/replay/continuation to stage
its one Snapshot and validate maintenance records; it is not an ordinary maintenance
append API. Root-only factories still reject predecessor/maintenance events.

Factor internal EnvironmentHead preparation from initial selector publication.
The core exposes prepare/publish_initial phases for the environment owner and direct
phase oracles; neither phase is an ordinary model binding or admission API.
Preparation allocates/owns the directory, exclusively creates and locks authority,
checks absence of head, and writes/synchronizes the exact root declaration there,
without publishing a selector. RetainedEnvironment owns this unhealthy prepared
head before creating/synchronizing its root through the borrowing facade under that
lease; only then publish/synchronize the initial selector and mark head healthy.
A prepared flag permits exactly one initial publication operation; its Busy guard
blocks recursive publication/replacement. Recheck actual head absence immediately
before publication so an unexpected selector is preserved/refused, never overwritten.
Failure consumes that initial operation and leaves the head unhealthy/lease held for
diagnosis until owner destruction; no second initial branch is published on it.
A second creator fails at authority acquisition before touching any root journal.
Public EnvironmentHead.create composes the same preparation/selector phases and
keeps its prior head-only behavior. A crash/failure between authority and selector
leaves concrete partial-creation files; open refuses missing selector rather than
inventing one. On any root/head failure the semantic owner and all descriptors are
destroyed before head/directory. A consuming factory must not destroy a directory
while root borrows it. Open obtains a complete head before any borrowed segment.

Root creation prepares/syncs its journal before initial selector publication; no ordinary
work is returned before both succeed. Creation failures preserve partial originals.
Open first obtains EnvironmentHead's permanent lease, then inspects the selected
headers under it. Read header bytes with bounded short-read/interruption handling;
validate immutable declaration, namespace, filename/environment and configured
framing ceilings before allocating/opening each FramedJournal scan. Selected issuer
namespaces must be nonzero and pairwise distinct across all selected headers;
repeated namespaces refuse the chain, just as repeated journal IDs do. Follow exactly
the selected links with cycle/count/generation checks; then stage root to active.
No orphan discovery, fallback branch or source read through the wrong descriptor.

### One bounded semantic snapshot

Snapshot carries ordinary committed facts/sources, a separate uncertain-fact index,
owned provisional originals (marker reference, descriptor, exact ARPROP01 bytes),
and active issuer_namespace (u64) alongside counter (u64). Pending reservation burn
accounting is an explicit pair(namespace,counter), initialized from the selected active
header; entering each next selected unique namespace resets the snapshot counter
and updates its namespace before applying that segment's reservations.
The exact max_history_entries count is facts.size + ordinary_sources.size +
uncertain_facts.size + provisional_originals.size. Maintenance choice/capture facts
count in facts; captured source chunks remain physical inspection metadata and are
not ordinary_sources. Each event body/dependency vector is bounded by its validated
origin framing limit; total canonical provisional packet bytes across the snapshot
must not exceed max_capture_bytes. Physical metadata remains independently bounded
per segment and by max_segments. Temporary decode/candidate copies obey the same
bounds; checked addition precedes allocation. There is no unbounded extra index or
snapshot per historical segment. Ordinary appends preflight both active physical
policy and remaining snapshot budget, including diagnostic capture expansion.

Extend existing semantic apply/existing once. Identity dedup examines ordinary and
uncertain indexes before dependency admission: exact uncertain reuse is Existing/
Uncertain with no publication, changed reuse is a retained conflict; uncertain parents
and ProvisionalCapture original proposal sources cannot satisfy ordinary dependencies. An ordinary
parent/source lookup sees only the selected committed prefix. Maintenance validators
place choice/capture facts only after the required marker set and checked references
are all validated. Their source chunks never become ordinary sources.


During maintenance replay, stage a bounded capture_only reference set derived from
validated ProvisionalCapture marker dependencies and their contiguous source groups.
It is temporary per-segment state bounded by configured physical records, not another
persisted index. Validate the whole required set before applying ordinary batches;
filter those references out before constructing ordinary_sources or checking ordinary
dependencies. Chunk/marker groups can span source-only commit batches. Marker absence
or malformed/interleaved groups closes the owner; unmarked chunks are inspection
material, never speculative ordinary sources. Persisted marker facts already retain
the reference set for later source classification; do not duplicate it unboundedly.
Existing acknowledged root RejectedSubmission/IdentityConflict diagnostic sources
remain ordinary retained bytes under the accepted root contract: referencing bytes
never admits the rejected operation. The capture-only rule concerns the new
ProvisionalCapture maintenance groups, including purpose2 (rejected proposal), whose
original identities/sources do not acquire authority through capture. A purpose2
marker itself grants no ordinary admission regardless of its parsed content.

Reservation keys include their containing journal's namespace; all other existing
identity keys retain exact typed-ID semantics across the chain. Reset the active
issuer counter when entering a new unique namespace; old reservations stay facts,
but cannot advance or suppress new reservations. Burn a decoded uncommitted pending
reservation only for its own active namespace. Route source reads by original journal
ID to its retained descriptor, with existing membership and cached-frame-CRC checks.

### Replay, continuation and publication

Replay each selected historical boundary exactly, batch-atomically. Carry the one
staged snapshot forward; invalidate a whole semantically bad batch, never publish
half its facts. A selected historical boundary that cannot be semantically reached
refuses the chain; an active invalid/damaged tail retains a diagnostic prefix with
ordinary work closed. Choice and contiguous capture parsing runs before ordinary
semantic batches; missing/extra/interleaved markers refuse activation. Parse capture
bytes using their origin header's validated limits and full event/dependency equality;
only purpose1 creates uncertain identities. Match exactly recovered authoritative
ordinary records to suppress duplicate uncertain entries; inherited ancestor captures
are not re-declared. Checked admissions may resolve only after every required capture
has been parsed; do not manufacture observed terminal state from custody verification.

For a live continuation retain the acknowledged semantic snapshot and owned pending
proposal while closing/reentrancy-fencing the entire operation. Discard stale prepared
semantic staging before restaging each existing descriptor; reapply historical selected
boundaries. Fresh semantic replay stays in a local unpublished Snapshot until fully
validated. Reentrant mutations, effect dispatch and state-mutating source reads return
Busy; immutable committed_facts continues to expose the prior snapshot, never half
rebuilt indexes. Read queries with owned result objects can return Busy during this
operation. Environment state is closed for the whole transaction even if an internal
physical object is momentarily Live/Recovered. Raw-range inspection remains available
under the existing lease/descriptor, as the physical reader is state-nonmutating;
it reports observed extent/bytes and cannot publish or mutate semantic staging.
It never validates a partially scanned frame or reacquires/releases the writer lock. The reconstructed prefix must contain acknowledged facts/sources with
exact record/event/checksum equality (evidence can become recovered); changed/lost
acknowledged semantics refuse even if physical CRCs pass. Synchronize selected files,
verify actual nonterminal committed/provisional attempts, and preflight all candidate
metadata/choice/capture/result bytes before candidate creation. Missing/corrupt selected
headers refuse ordinary open; a separate bounded raw inspection helper obtains
EnvironmentHead's permanent lease and reads the selected named file without declaring
its header/namespace/history valid. This diagnostic helper grants no continuation
or effect authority and does not substitute an older branch. Preserve pending
originals until acknowledged head replacement and nonallocating cache/owner swap.

Candidate entropy's 16-byte journal ID AND little-endian u64 issuer namespace must
both be nonzero and absent from every selected header, with at most 32 samples;
exclusive creation refuses existing orphan names. Candidate choice is semantic seq1;
write/sync all declared captures before head replacement. After final directory sync,
move old active into the pre-reserved historical vector, install new active/snapshot,
clear RAM proposal and publish configured bounds/profile without allocation. Old
attempts stay recovered/nondispatchable; only fresh explicit admissions can run.
A reopened chain requires explicit confirmation/custody reconciliation before ordinary
work. A candidate/head failure cannot grant permission from partly prepared indexes.

### Concrete first three-segment oracle

Use validated immutable headers with the same framing limits 1024/4096, distinct
journal IDs/namespaces, root policy 4096/8, first child 8192/16 and second child 16384/32.
Independent ceilings cover all three and max_history_entries is varied at the exact
boundary. Binary originals are 00 ff 0a. A decision has six 16-byte IDs, one planned
invocation and that 3-byte continuation; with one 24-byte source dependency its payload
is 155 bytes (8-byte envelope+24+96+4+16+4+3).

| Segment operations | Exact cursor | Snapshot entries |
| --- | --- | --- |
| Root: source1 + decision2 + commit3 | seq3/end390 | decision1+source1=2 |
| Root: reservation4(counter1) + commit5 | seq5/end494 | facts2+sources1=3 |
| First child: choice1 + commit2, no captures/checked attempts | seq2/end316 | facts3+sources1=4 |
| First child: source3 + invocation4 referencing root decision/source + commit5 | seq5/end526 | facts4+sources2=6 |
| First child: reservation6(counter1 in fresh namespace) + commit7 | seq7/end630 | facts5+sources2=7 |
| Second child: choice1 + commit2 | seq2/end316 | facts6+sources2=8 |
| Second child: source3 + admission4 referencing the prior invocation/source + commit5 | seq5/end526 | facts7+sources3=10 |

The two counter1 reservations must yield different full IDs and remain distinct facts.
All three binary sources must read through their owning descriptors before/after
reopen. At history budget 10 an AttemptOpen would require entry 11 and must refuse
before actual dispatch; with sufficient budget the explicit fresh attempt dispatches
once and retains its separate adapter receipt. Reopen requires real supplied custody
verification and never redispatches that recovered attempt. Exact tuples here derive
from specified envelope/frame/commit bytes, not the implementation's own cursor helper.
This is the first useful integrated oracle, alongside the required failure/capture
histories; it does not substitute a happy path for whole continuation qualification.

Construction order: implement bounded shared semantic/index accounting and namespace
keys; complete create/open/replay with source routing; connect ordinary proposal
results/dedup; implement live restage, entropy, captures and head publication; qualify
native process-death/head cuts and full continuation cases. Direct red cases from the
selected-chain/capacity sections govern every step. Independent code review must
resolve the complete owner before U2 relies on it. Emergency controls remain a further
required U1 component; this refinement does not silently omit them or accept whole U1.

## Selected-chain replay

### Physical replay implementation refinement (implemented, qualified, review resolved)

The independent [plan review](../papers/2026-10-02-u1-physical-replay-plan-kimi-review.md)
and [resolved rereview](../papers/2026-10-02-u1-physical-replay-plan-kimi-rereview.md)
pin checksum metadata publication and permanent historical write fencing. Direct
red compilation preceded implementation. The [code review](../papers/2026-10-02-u1-physical-replay-code-kimi-review.md),
[rereview](../papers/2026-10-02-u1-physical-replay-code-kimi-rereview.md) and
[final correction](../papers/2026-10-02-u1-physical-replay-code-kimi-final-rereview.md)
have no open scoped findings. Direct changed Mac debug/release/ASan+UBSan and
Neuroses three-profile checks pass; [resolutions](../papers/2026-10-02-u1-physical-replay-resolution.md)
name the fault/host scope. This is physical-layer acceptance only. The future
semantic owner must discard a stale prepared snapshot before any restaging operation.

`FramedJournal` supplies in-place restaging and exact historical-prefix selection
under its existing descriptor/lock. Restaging immediately closes ordinary writes,
rechecks the exact immutable header and independently bounded extent, clears the
fresh staging/pending buffers, resets its staged cursor to 0/112, updates
recovery.available_end from the fresh extent, and scans from 112 without reacquiring
a file lock. Restage is allowed from live, recovery-pending, recovered, blocked
or poisoned; none grants writes while staging. Header/frame/payload reads use an
eight-consecutive-Interrupted budget reset by real progress; extent retries have
the same bound. Zero/impossible read counts and native errors retain their concrete
failure. A reentrant restage or mutation while restaging returns Busy, so a storage
callback cannot confirm or change partly staged indexes. Payload validation also
returns Busy during restage because a failed read can change writer state. A
payload failure never downgrades an unknown-write Poisoned state to Blocked;
restage is the explicit operation that replaces that state through fresh recovery.
After confirmation, clear the old staging vector left by the records/staging swap;
only the acknowledged records and pending inspection suffix remain populated.
Keep the previously acknowledged physical record vector during staging. Its last
record defines the acknowledged commit boundary (last semantic/source sequence+1;
last payload end+56). Empty acknowledged records mean boundary 0/112, not the
unconfirmed cursor of a freshly opened recovery-pending file.

Cache each frame's EXISTING validated wire CRC in its physical metadata, including
source metadata copied into the semantic owner. This is the current integrity
field, not a new checksum/receipt system. Fresh staged records must contain the
entire acknowledged vector with identical journal/kind/sequence/batch/offset/length
and cached frame CRC, at the same actual validated commit boundary. Changed or
shortened acknowledged history returns Corrupt and keeps work closed; header
mismatch/native failure returns its concrete error. CRC collisions are outside
this CRC32C fault domain; no malicious-tamper/cryptographic-history claim is made.
The cached checksum participates in PhysicalJournalRecord equality/containment.
Append sets it from the header of its just-encoded frame before any write, and
publishes that prepared metadata with the acknowledged record. Scan sets it from
the validated frame header. After successful physical append, RetainedState replaces
the incoming placeholder source metadata with the actual appended source records
from physical_records(), before committed_.swap; this copy cannot allocate. Failed
prepared sources remain uncertain and never become ordinary source dependencies.
Semantic replay additionally compares the saved acknowledged facts. Allocation
failure while staging leaves the owner closed with originals and RAM proposals
intact, and does not grant permission from either old or partly staged indexes.
Compare the acknowledged (sequence,end_offset) against the actual scanned commit
boundary after scan, including when scan stopped at damage. Missing/changed
acknowledged prefix returns Corrupt ahead of the suffix scan diagnostic. An empty
acknowledged vector with a damaged first submission still accepts prefix 0/112;
the damaged suffix remains explicit. A hard scan/allocation error leaves staging
closed rather than declaring successful comparison.

Historical selection is legal only during recovery-pending staging and only for
its own journal. Accept 0/112 or a boundary derived from the final record of a
fully scanned commit batch; reject mid-batch, mismatched end, unavailable prefix
or a different journal without changing selection. Move later staged metadata
to pending inspection without allocation/truncation. Keep actual suffix diagnostic
information. A historical-selection flag permanently prevents resuming that
segment for writing, even when its selected boundary equals physical EOF; only
the independently selected active journal can be resumed by the environment owner.
The flag is set in the successful selection call, survives restage/confirmation,
and is checked in resume_after_reconciliation. Confirmation leaves flagged journals
Blocked/inspection-only even when their report is clean. Restaging a flagged journal
may recheck originals but never clears the flag or resumes writing; the environment
must still reapply its exact selected historical boundary before semantic publication.

Direct oracles: live-to-staged same-descriptor writer exclusion; uncertain append
with a complete recovered batch vs every partial tail; exact empty/first/later
commit boundaries vs offsets/sequences in a batch; valid later suffix retained
but unpublished; CRC-valid source rewrite with unchanged geometry refused against
the cached acknowledged frame checksum; shorter acknowledged history, changed
header, header/read/extent/OOM failures; selected historical segment never resumes.
The source owner copies the actual physical source metadata after successful append
and before its nonallocating semantic publication, so cached CRC cannot remain a
placeholder. Full selected-chain ownership/semantic replay remains the next layer.
Also exercise freshly appended (never scanned) acknowledged sources, semantic-owner
source reads after checksum metadata publication, empty-vs-nonempty acknowledged
damage, and restage after prior semantic batch rejection. Noncommit source/semantic
records consume max_records; commits consume additional sequence numbers only.

Open obtains the permanent lease before inspecting selection/history. Follow the
selected active header's predecessor links back to the declared root, with bounded
segment count. Reject cycles, repeated journal IDs/namespaces, wrong environment,
missing/corrupt headers, root with a predecessor, a non-root without one, or chain
length inconsistent with head generation. Do not use orphan files as history or
choose an older branch when the selected file is bad.

Replay root to head. For each historical segment, its child's header names exactly
the selected committed prefix: zero/112 for an empty journal, or an actual complete
validated commit boundary. Derive every candidate boundary from fully validated
staged batch records and commit framing, not caller offsets. A child cannot split
or skip a batch. Restrict historical semantic replay to that exact prefix even if
additional complete or pending bytes survive in the original file. Those bytes
remain inspectable, not published by scanning past the declared boundary.

Carry original source references and event identities with their original journal
IDs through the accumulated snapshot. Route source reads to the owning segment
and recheck membership/CRC there. The current semantic owner must never send an
older source record to its newest file. All reopened facts have recovered evidence;
old admission/open permissions never become live merely through a new segment.
Source dependencies and parent validation use the accumulated committed prefix,
not an unrelated journal or a provisional original.

Stage the whole selected chain before publication. Synchronize the selected files
and directory at the declared strength. Confirm recovered semantic facts only
when every selected prefix is valid. Actual nonterminal attempt custody is verified
through the existing CustodyVerifier; successful verification does not regrant old
dispatch permission. If the active tail is damaged, expose the validated prefix
and original ranges but keep ordinary work closed until explicit continuation.

## Capacity declaration versus configured bounds (review resolved; owner implementation pending)

The 112-byte header declares payload/batch framing limits, not file/record policy.
Do not change that accepted format to recover a declaration from bytes that never
contained it. Environment create/open supplies an explicit root JournalCapacity;
continuation supplies an explicit new JournalCapacity. The first child choice's
old capacities must equal that supplied root declaration. Each later choice's old
capacities must equal the immediately preceding segment's validated new declaration.
The selected active segment's policy is the last validated new declaration, or the
explicit root declaration for a root-only environment. A different root declaration
on reopen cannot silently reinterpret an existing linked history. In a root-only
environment no persisted capacity anchor exists: the caller's explicit declaration
is trusted configuration, checked against independent bounds and selected-prefix
fit. Refusal of a different historical root declaration is possible only once the
first choice has persisted that declaration; do not claim otherwise or infer a
missing anchor. Explicit root-only policy changes are configuration inputs.

Separately configure independent JournalLimits framing ceilings (max_payload and
max_batch_bytes as u32), maximum segment-file bytes (u64, at most INT64_MAX),
noncommit physical records per segment (size_t), selected segment count (size_t)
and accumulated indexed history (size_t). Both policy-fit checks and physical-index
bounds count source/semantic records only; commits consume sequence numbers, as
pinned above. Every selected root/child header's framing limits must fit the
configured ceilings before FramedJournal allocation or scan, including a continuation
that changes limits. Each choice's old_limits must equal its immediate predecessor's
immutable header limits, including the root header for the first child; its containing
new header carries the new framing limits. This restates the existing wire/replay
old_limits rule at the actual allocation boundary. These configured
bounds control physical scans, reservations and snapshot allocations. Validate
wire capacities and all conversions against them before accepting a declaration.
Any value not representable in its actual host operand type returns Capacity;
never truncate, clamp, wrap or narrow before comparing. The current host qualification
is 64-bit Mac/Linux; this rule does not claim 32-bit qualification. In particular,
wire max_records (u64) must fit size_t before use, and configured file bounds must
fit the native offset domain. All declared framing limits are validated against
the configured framing ceilings before their use in per-frame scratch sizing.
Validated header limits may narrow scratch allocation within those ceilings;
unchecked wire values never choose allocations, and capacity declarations never
choose configured reserve budgets or loosen independent bounds. Admission also
checks the active segment's validated policy, which can be smaller than the configured
scan bound. A caller requesting larger continuation policy must provide sufficiently
large configured bounds before any candidate I/O. Reopening with insufficient bounds
returns Capacity rather than inferring a reduced history or choosing an older branch.

A selected committed prefix must fit its segment's declared file/record policy.
Damaged/unselected suffix bytes remain inspection material, bounded by the independent
scan limits, and cannot inflate that segment's declared authoritative capacity. A
continuation's choice retains the actual observed available end, including a suffix;
this can exceed the predecessor's declared append policy without authorizing that
suffix. Older source dependencies retain their segment identity/checksum and their
original immutable framing limits; a larger active profile cannot reinterpret them.

The single accumulated snapshot is bounded independently of one segment's record
limit: otherwise continuation merely moves a full index into another full index.
Define the exact counted entries (ordinary facts/sources and provisional indexes)
in the owner API before its red cases. Snapshot growth and capture-byte ownership
must be checked before allocation/publication; capacity refusal preserves originals
and surviving pending proposals. Increasing those configured limits is normal operator/
model configuration, not an approval step or an untrusted-file-driven allocation.

Required direct history: root, first child and second child with successively increased
file/record policies, parents and source dependencies spanning all three, successful
reopen under sufficient independent bounds, Capacity under insufficient bounds, and
wrong supplied root in a linked history/mismatched second choice-old capacities
refused. Add mismatched old_limits, framing limits exceeding configured ceilings
refused before allocation/I/O, enlarged framing within ceilings accepted, and a
prefix exactly at its noncommit-record limit despite interleaved commits. Check exact
active-policy admission boundaries independently of the larger scan profile; a valid
later historical suffix remains unpublished and inspectable. Root-only reopening
must use its explicit declaration, not a nonexistent choice/header capacity field.
The independent [capacity review](../papers/2026-10-02-u1-capacity-refinement-kimi-review.md)
and [resolved rereview](../papers/2026-10-02-u1-capacity-refinement-kimi-rereview.md)
confirm these rules. The owner API and ordinary snapshot accounting have a resolved refinement and
initial implementation above. The [capture refinement](U1_CAPTURE_SUBPLAN.md)
completes provisional accounting and custody contracts; its review is resolved.

passing physical tests alone do not establish selected-chain owner acceptance.
The accepted wire/header formats are unchanged.

## Live continuation and uncertainty

Continuation first closes ordinary admission/reentrant dispatch for the entire
operation. If the current owner has a known acknowledged cursor, fresh physical/
semantic reconstruction must still contain that exact acknowledged prefix. A
shorter or changed prefix is declared history loss; do not silently continue as
complete history. Synchronize the available predecessor before selecting its
validated prefix. U3's actual custody check must succeed for every nonterminal
attempt; no synthetic terminal result is manufactured from the check.

The root now retains uncertain owned event bodies in prepared state and a bounded
owned serialized proposal before the first write: expected cursor, exact source
bytes and exact encoded events. The linked owner must preserve that surviving
packet through continuation. The existing ARPROP01 layout has an owned pure codec;
no new receipt/checksum layer. Clear this temporary RAM proposal only
after clean refusal or successful head publication acknowledged by
EnvironmentHead.replace (including directory sync and the prepared cache swap). Unknown writes keep it. Diagnostic
source-group failures preserve any RAM proposal needed to describe the failed
submission. A process crash may lose RAM; never claim later recovery knows bytes
that neither storage nor surviving memory contains.

If fresh predecessor replay already recovers an exact uncertain event authoritatively,
do not duplicate it as a new ordinary fact. Otherwise retain its original submitted
proposal in the new file as a provisional capture, not a retrospective admission.
Keep a separate derived uncertain identity index. An exact resubmission returns
Existing/Uncertain without writing or dispatching; changed reuse retains a conflict.
Uncertain originals do not satisfy ordinary parent/source dependencies and do not
appear as ordinary committed Admission/Decision facts. The authoritative fact is
that these bytes were captured as an unresolved earlier submission. Later ordinary
work can use fresh explicit decisions/invocations/attempts.

## Candidate identity and publication

Read 24 OS-entropy bytes using getentropy (native max 256): 16-byte journal ID and
little-endian u64 issuer namespace. Reject zero/repeated IDs or namespace against
EVERY intact selected predecessor header. Exclusive filename creation also refuses
an existing orphan. Try at most 32 candidates, then return Conflict; native entropy
errors return actual error, never a clock/PID fallback. Linux getentropy manual and
installed Xcode sys/random.h ground this owned call. Cross-environment collision
remains probabilistic; within this intact environment counter restart cannot reuse
an older issued ID. Missing headers block that nonreuse claim.

New namespace starts counter zero. Reservation dedup/burn/replay is keyed to the
current segment namespace: historical reservations remain facts but cannot suppress
or advance a reservation in a distinct namespace. All other identity dedup remains
across the selected chain. A failed namespace/candidate publication returns no
issued identity and no new dispatch permission.

Preflight candidate header, choice/provisional records, exact chunk/dependency sizes,
sequence/record/file budgets and every cache allocation before creating candidate
files. Unrepresentable capture returns Capacity with old evidence retained; caller
may request a larger explicitly declared continuation profile. First batch retains
the recovery choice: predecessor cursor, diagnostic available range/error, old/new
limits and the actual checked attempt states. Unresolved proposal bytes use bounded
source-only groups followed by a provisional marker referencing ALL ordered chunks,
like large rejected-proposal retention. A marker alone signifies complete capture;
orphan groups do not invent semantic facts. Never publish head until all required
choice/capture markers and the candidate journal/file/directory syncs succeed.

Then EnvironmentHead.replace performs temp-selector sync/rename/directory sync.
Any uncertain publication closes normal work. Keep old/candidate files and RAM;
never undo rename, dispatch from an unacknowledged candidate, or publish another
branch on the same owner. Fresh open reads actual head and stages that selected
chain with explicit custody. Candidate files not selected by head confer no authority.

## Reviewed design questions and execution order

The RecoveryChoice and ProvisionalCapture layouts below received independent
review before implementation. The review pressure-tested
whether observed prior attempt states plus successful CustodyVerifier constitute a
sufficient retained choice without asserting synthetic disposition; how diagnostic
partial captures preserve their original proposal; and how RAM/index capacity is
bounded without refusing every useful continuation at a full predecessor limit.
The resolutions below govern implementation; do not improvise different formats.

Implement the exact maintenance codecs first, with hand-derived byte vectors,
every truncation, field/order negatives and allocation cuts. Root RetainedState
must reject these maintenance events: decoding is not permission to append a
recovery choice outside the selected-chain owner. Then factor the existing exact
ARPROP01 encoder/parser and own complete ordinary/outer rejected proposals before
the first storage write. Connect physical selected-prefix rescan and the accumulated
semantic owner, entropy selection, provisional indexes and head publication as
one continuation invariant. These are implementation steps within U1, not separate
acceptance of the linked chain.

The pure proposal codec takes an explicit byte bound, encodes the existing
ARPROP01 layout unchanged and parses owned source/event byte vectors. It checks
all counts/lengths against actual remaining bytes before allocation, accepts exact
binary originals (including empty blobs), rejects trailing bytes/zero journal IDs,
and does not reinterpret a rejected proposal's expected cursor as current history.
The root holds at most one pending proposal: ordinary work closes after uncertainty.
An acknowledged ordinary append clears its temporary proposal after nonallocating
cache publication; a failed diagnostic append holds the whole outer packet through
any source-group/marker failure. Inner diagnostic appends cannot replace or clear
that packet. Clean pre-I/O refusal clears it; unknown writes or incomplete grouped
capture keep it even if no complete event bytes reached disk. Reopening after process
loss starts without that RAM evidence. A borrowed diagnostic accessor allows the
future environment owner to capture these originals; it grants no source/admission
authority and its borrow ends at the next mutation.

Direct oracles must include multi-segment parents/sources, same counter in fresh
namespace, scripted entropy zero/collision/error/exhaustion, exact prefix mismatch
and missing header, orphan candidate/head uncertainty, no old redispatch before or
after crash, incomplete source memory surviving a failed append, provisional duplicate
versus conflict, semantic invalidity inside otherwise complete batches, metadata
failure at every publication boundary, permanent competing-writer exclusion, and
actual SIGKILL across final head publication. No power-loss/live-U3-worker claim.

## Proposed wire and replay rules for review

Extend retained event schema1 with RecoveryChoice=12 and ProvisionalCapture=13.
These use the existing source-dependency envelope and explicit nonzero typed IDs.
Integers remain little-endian; no struct memcpy or extra integrity/receipt system.

RecoveryChoice has no source dependencies and is semantic sequence1 in the first
candidate batch. Body: predecessor journal16/commit-sequence u64/end u64;
diagnostic offset u64/available end u64; old max_payload u32/max_batch u32;
old max_file u64/max_records u64; new max_file u64/max_records u64;
error-present u16 (0/1)/error-code u16/detail i64 (absent requires code/detail zero);
checked-attempt count u32; then each checked entry is attempt16/opened u8 (0/1)/
observed phase u8 (0 none, 1 running, 2 settling, 3 terminal)/disposition u8 (0 none,
1 success, 2 failure, 3 cancellation, 4 unknown)/prior evidence u8 (0 live,
1 recovered-pending, 2 recovered, 3 uncertain). After checked entries: required-capture count u32 and that many descriptors
(origin journal16/first physical sequence u64/proposal length u64/purpose u16/
reserved u16=0). Fixed body is 108 bytes plus 20 per checked attempt and 36 per
required capture. Descriptors are unique, nonzero, refer to the immediate predecessor
and match exactly one complete ProvisionalCapture marker in this selected segment. Count is bounded by actual body length before allocation;
entries are unique and ordered by ascending attempt-ID bytes, and refer to retained
or provisionally captured admissions. Validate these references only after the whole
required capture set has been parsed; a missing admission is a replay rejection.
The choice states which prior attempts and observations custody checked successfully;
it does not assert that a prior observed phase is the actual new resource state or
manufacture a terminal disposition. Verify this distinction against U3 ownership.

Choice predecessor must equal its containing header's predecessor, range must obey
predecessor.end <= available_end and diagnostic_offset <= available_end, old limits
must equal the predecessor header, new capacities must equal the candidate owner,
and all checked entries must match the actual pre-continuation reconciliation set.
An empty set is legal. New payload/batch limits are in the candidate header and
are validated there. Old/new capacity numbers are storage/index policy captured for
study; they do not themselves drive allocation on replay. Sequence1 and exactly one
choice per non-root segment are environmental replay rules; choice in a root or
later batch is invalid. Generic event format decoding alone grants no activation.

ProvisionalCapture body: origin journal16/original first physical sequence u64/
proposal-byte length u64/purpose u16 (ordinary submission=1, rejected proposal=2)/
reserved u16=0. Its envelope references every ordered new source chunk containing
one exact ARPROP01 proposal. Reassembled length must equal declared length; parse
all counts/lengths against available bytes and reject trailing material. For ordinary
submission, encoded expected journal must equal origin and expected.sequence+1 must
equal original first sequence. Derive original event record positions from that
first sequence and the proposal's source count. Only unresolved ordinary identities
enter the separate uncertain index; sources/parents in that index never satisfy
ordinary dependencies. Exact authoritative predecessor recovery suppresses duplicate
uncertain entries, with full encoded event/dependency equality, not just same ID.

A rejected-proposal capture records original caller input that may contain a stale
or wrong expected cursor, so that cursor need not equal its actual attempted
physical diagnostic source-group position. It never seeds ordinary uncertain
admissions. This distinction prevents failed diagnostic retention from turning
previously refused caller input into a provisional ordinary operation. Origin/first
physical sequence/purpose identify the capture; repeated different bytes conflict.
Only the final marker signifies complete capture; chunks without it stay originals.
A selected candidate lacking its required capture marker must not open for normal
work. The choice and required capture set must therefore be cross-checked during
candidate preparation and selected-chain replay, not inferred from chunk existence.

Live reconstruction must use an in-place physical rescan under the already-held
segment descriptor/lock (or a deliberately sequenced descriptor transfer under the
permanent environment lease); opening a competing descriptor and taking a second
flock on the same live segment is not a valid protocol. Save the acknowledged cursor
and owned RAM proposals before staging. Stage fresh indexes without publishing them,
then restrict replay to selected exact boundaries. Damage that removes acknowledged
history refuses continuation as complete history. A corrupt/missing selected header
may still be inspected through a bounded raw file path under the permanent lease,
but cannot provide a namespace or participate in authoritative replay. No older
selected segment is substituted for it.

Prepublication candidate write failures leave ordinary work closed and preserve
orphan bytes plus the original RAM proposals. Explicit continuation may retry with
a fresh candidate while EnvironmentHead is healthy and still selects the known
predecessor. After any uncertain head replacement, no second branch is allowed on
that owner; reopen/recovery must establish the actual selection first.

## First protocol review resolutions

The independent review read the initial 100-byte claim before its correction:
checked count adds 4 (104), required-capture count adds 4 (108). Current format is
108 fixed +20/checked attempt +36/required capture. Descriptors retain length and
purpose as well as origin/sequence: purpose2 rejected proposals also require
complete capture before publication, and must not seed ordinary uncertain identity.
Reject missing, extra or mismatched capture markers; the declared required set is
authoritative selection input, not a second receipt. Resolve all forward checked
admission references only after full captures. All capture markers precede the
first ordinary semantic batch; choice/captures themselves are maintenance records.
Checked entries and required descriptors use the canonical ordering defined above.
Wire u64 capacity fields are checked when converting to the actual configured owner
type; never derive an allocation limit from an untrusted value being validated.

The retained RAM proposal lifetime ends at acknowledged full head publication and
nonallocating cache swap, not at candidate-file sync. Candidate failure preserves
RAM/orphans and permits explicit fresh-ID retry only while the original head remains
known/healthy. Uncertain head publication requires reopen, not another branch.

Two suggested review histories need correction: a completely valid committed batch
that survives unacknowledged sync can be staged and recovered, so it does not by
itself establish a prefix mismatch. Use missing commit or semantic invalidity when
asserting a child references unavailable authoritative history. Reservation events
contain counter only; their namespace is the containing header. The same counter
is legal in a fresh namespace and is NOT a changed-identity conflict. Old 128-bit
caller-supplied IDs remain the exact old identity; tests must compare full IDs and
origin namespaces rather than reinterpret identical counter bodies as the same ID.

## Second review rule completions

Descriptor/marker match is exact origin/first-sequence/length/purpose equality.
Each marker's references are ordered by increasing physical sequence, refer only
to source frames in its own candidate journal, and concatenate to the exact
length. A capture's source groups and marker are contiguous except their commit
frames; captures cannot interleave. Capture markers are legal only in a non-root
segment, after its unique choice and before any ordinary semantic batch. The
required set must equal the complete marker set, including rejected proposals.

Canonical descriptors sort by unsigned origin journal bytes, then first sequence,
then purpose. Checked entries sort by unsigned attempt ID bytes. Decoders reject
noncanonical order or duplicates; encoders refuse invalid order instead of changing
caller originals silently. This pins deterministic byte vectors without another
token/receipt representation.

An exact proposal already fully captured in an intact ancestor is inherited and
not re-declared/re-captured in a later choice. Identity dedup spans both committed
and uncertain indexes. Changed ordinary reuse retains an identity conflict in live
submission; an inconsistent duplicate capture discovered in replay rejects that
batch/suffix, not an unsolicited repair write or second uncertain identity.

Choice available_end cannot exceed observed predecessor file extent, and diagnostic_offset must be at least the 112-byte header boundary. Inspection of a damaged
header uses the separate raw path and cannot manufacture a valid choice. All
choice/capture semantic checks are staged before publication. Choice does not
publish an incomplete forward-reference set; candidate selection validation waits
for all declared markers before resolving checked admissions.

For failed diagnostic retention, keep the entire OUTER rejected ARPROP01 proposal,
not merely an inner source group's serialized append. Source-only chunk appends
must not replace that RAM proposal or seed uncertain ordinary identities. Prepare
it before the first group; retain it through an unknown group/marker/head outcome.
Its purpose2 capture preserves exact caller originals even if the caller's expected
cursor is stale or refers to another journal. Failed ordinary append owns its
whole source/event proposal before first write, never just pointers to caller data.

An empty COMMITTED predecessor prefix (sequence0/end112) can still have an
unresolved first submission in RAM or a partial physical tail. Therefore required
captures need not be empty for that prefix. The final review's contrary edge-case
sentence is incorrect: use a first source+decision append that fails partway before
its commit as the direct red history. The selected prefix stays empty, while the
choice declares and preserves that exact uncertain proposal with zero redispatch.
