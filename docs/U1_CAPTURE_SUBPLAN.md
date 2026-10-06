# U1 captured originals and uncertain identities

Operator sequencing correction, 2026-10-02: remaining advanced U1 work is deferred
until after the first useful coding session. Bootstrap U2/U3 use the existing
single-journal RetainedState path; earlier requirements here to wait for the complete
RetainedEnvironment owner are superseded by [the current plan](IMPLEMENTATION_PLAN.md).
Keep this work and its evidence; no completion or continuation qualification is claimed.

2026-10-02. Implementation refinement of U1_CONTINUATION_SUBPLAN, not a
separate acceptance boundary. Status: independent review/rereview resolved; ready for direct red and implementation.
The [original review](../papers/2026-10-02-u1-capture-plan-kimi-review.md) and
[resolved rereview](../papers/2026-10-02-u1-capture-plan-kimi-rereview.md) preserve
the independent findings and withdrawal of retrospective promotion.

## Purpose and scope

Finish selected-chain maintenance replay and the uncertain-query/admission boundary
before live continuation writes candidates. A captured packet proves what was
submitted, not that its operations or sources were admitted. Preserve the exact
ARPROP01 bytes, retain unresolved identities, prevent captured chunks from becoming
ordinary source authority, and validate each recovery choice against the complete
prior nonterminal-attempt set. Existing native framing, proposal/event codecs,
semantic apply and permanent writer authority remain the machinery. No dependency
or wire format is added. Current initial code handles complete non-attempt packets, capture-only chunks,
uncertain indexes and exact packet/event queries, with Mac changed checks passing
and independent code review underway. Attempt-related purpose1 packets and nonempty
checked lists still refuse Unsupported; the complete collector is unimplemented.
Live entropy/restage/publication, bad-header diagnostics,
emergency control and combined crash qualification remain required later in U1.

## One snapshot and owned originals

Add ProvisionalOriginal {marker RecordReference, capture descriptor, owned packet
bytes} and snapshot vectors provisional_originals and uncertain_facts. An uncertain
fact has original predicted RecordReference, full decoded RetainedEvent and Uncertain
evidence. Snapshot swap/copies include both vectors. Count facts + ordinary_sources
+ uncertain_facts + provisional_originals against max_history_entries with checked
subtraction; aggregate owned packet bytes against max_capture_bytes before reserve
or concatenation. Validate configured counts against each actual vector max_size.
Temporary packet parsing and candidate snapshots obey the same bounds; no persistent
second decoded-proposal cache. Borrowed provisional_originals() lasts until mutation,
including confirmation; unlike committed_facts(), its contents remain expressly
provisional. Confirmation changes maintenance facts to Recovered but never promotes
uncertain facts or packet contents to ordinary authority.

## Validate maintenance before ordinary replay

For each non-root selected segment, decode and validate its unique sequence1 choice
using existing header/range/capacity rules. Inspect the selected physical records
before shared ordinary replay. Starting immediately after choice, source chunks and
capture markers form complete contiguous groups, excluding intervening commit frames
(which are absent from the physical-record index). Every marker references all and
only the preceding group, in physical order, in its own journal. Groups may cross
source-only commits. No semantic event, unmatched source or other capture may
interleave a group. A marker cannot have zero chunks. A group's length equals its
descriptor exactly; marker descriptors equal the required set, with no missing,
extra, repeated, inherited or differently sized/purposed capture. Emission order
need not equal descriptor sort order; required descriptors remain canonically sorted
by the existing codec. All markers precede the first ordinary semantic record.
An extra/later choice or marker refuses activation, rather than being ordinary data.
After the complete required set, ordinary source-only batches are legal. An
incomplete required group never becomes ordinary sources or a partial capture.

Stage capture_only references bounded by actual selected physical record count.
Before any ordinary replay, classify every complete maintenance chunk as capture-only;
then replay with those references excluded and maintenance_end equal to the last
validated marker sequence (or1 if no captures). Any source in that maintenance
region which is not part of a validated group is a rejection. Every semantic
record in sequences2..maintenance_end must be a validated capture marker; any
ordinary semantic record anywhere in this region refuses activation, even between
choice and the first group or between complete groups. Choice/capture facts
enter staged facts only after all required packets and checked attempts validate.
A malformed required set refuses the environment; a subsequent ordinary invalid
batch follows the existing active-prefix versus historical-boundary behavior.
No maintenance transaction is published partially. These rules apply even when
choice and a complete capture share one physical commit batch.

## Packet validation and uncertain identities

The descriptor origin is the immediate predecessor header. Reassemble and parse
with the existing bounded ARPROP01 decoder; reject trailing bytes or length/count
errors. Decode each event with that origin header's max_payload, not the newer
segment's profile. Purpose1 requires expected.journal == origin,
expected.sequence +1 == first_sequence without overflow, nonempty proposal, and
its prospective sources/events/commit fitting origin payload/batch framing limits.
Original event positions are first_sequence + source_count + input ordinal; every
addition is checked. Purpose1 events cannot themselves be choice/capture
maintenance events, which the ordinary owner cannot admit. Its source/event count
also fits the immediate predecessor declared record policy. Sources remain packet
bytes, never ordinary SourceReferences.
Purpose2 permits stale/foreign expected cursors and originally refused source sizes;
it still requires bounded packet syntax and origin-limit encoded events. It never
seeds uncertain identities, even if the bytes contain a well-formed admission.
Do not apply ordinary parent/source validation to provisional originals: a packet
may describe dependencies whose physical records were never committed.

For each purpose1 event, use the existing identity rules with the origin namespace
for reservations. If an authoritative prior fact has the same identity and full
event/dependencies, omit its uncertain entry; changed same identity rejects capture.
An existing uncertain identity must also match exactly: changed bytes/dependencies
reject the required capture and selected segment. Only exact duplicates retain their
first original semantic record. Suppressing a non-keyed event requires an exact
authoritative event at that predicted physical record; an earlier equal ordinary
observation at another position is a separate event, not identity dedup.
That positional suppression answers whether the *original physical submission*
was recovered. It differs deliberately from an exact individual uncertain query,
which compares full event bytes even for non-keyed events. Accordingly ordinary
replay of the same complete non-keyed event while it is still uncertain also
rejects its batch, even at a later position: the supported submission API would
have returned Existing/Uncertain without producing that later frame. This does
not deduplicate ordinary repeated nonterminal observations when neither is uncertain. Duplicate keyed positions
inside one packet do not create multiple uncertain facts. Non-keyed events retain
individual original positions for state study; individual exact uncertain lookup
compares the complete event, while ordinary non-keyed-event behavior stays unchanged.
Never turn uncertain parents, reservations or observations into ordinary apply facts.
A later new reservation uses its new namespace and cannot suppress an origin one.
Individual reservation queries use the current namespace; an exact old reservation
packet can be queried through full-packet equality without burning the new counter.

Ordinary replay is not the same operation as an idempotent submission query. An
ordinary semantic frame matching a still-uncertain event cannot be produced by the
specified submission API, which returns Existing/Uncertain without writing. Reject
that ordinary batch Conflict, even if the body is equal; never silently drop its
frame or retrospectively promote the unresolved submission. A changed-body match
also rejects. Apply the existing active suffix/historical exact-boundary distinction:
active retains its preceding prefix with work closed, historical selection which
includes that batch refuses the chain. The uncertain entry and original packet stay
unchanged. There is no authorized uncertainty-resolution/promotion operation in U1;
if one is later designed it needs an explicit new contract, not replay inference.

## Packet provenance versus semantic queries

submit_proposal first searches exact purpose1 packet equality, using the caller's
expected cursor, every source and every encoded event under the origin limits.
This precedes new active-profile/cursor/write-readiness refusal. On a full match,
return whole existing=true, capture descriptor, current active cursor, and one
existing=true/Uncertain result per original event position. Preserve input ordinals,
including physical duplicate positions and events also known authoritatively.
These results describe this old captured submission; they do not redefine semantic
fact provenance. A source-only exact packet returns empty event results plus capture.

By contrast submit(single) and ordinary event queries prefer the first authoritative
semantic record, then the first uncertain semantic record. An event already known
committed therefore remains Recovered even when it also occurs in an old captured
packet. Within an ordinary mixed old/new proposal preserve all physical frames and
return first semantic matches as the existing construction contract requires.

Exact individual uncertain resubmission returns Existing/Uncertain before dependency
or active-profile encoding, with no write or dispatch. For bulk input that is not an
exact captured packet, any uncertain identity mixed with new events, changed event
bytes, or added/changed sources refuses Conflict and retains the complete outer
rejected input through the existing diagnostic path when audit capacity permits.
A source-free current-cursor all-existing event query may include uncertain events
and returns their typed semantic references without mutation. No query confirms
recovery, alters evidence or opens dispatch. Busy/head-health/continuation-latch
gates retain their existing meaning. Retention failure returns the actual failure
and preserves the existing RAM-ownership rules, not a fabricated Conflict success.

## Checked attempts and current custody

Factor one bounded attempt-state collector used by choice validation, attempt queries
and reconcile. It sees authoritative and uncertain attempt-related events but never
uses uncertain entries for ordinary parent validation or dispatch. An uncertain
admission creates a provisional attempt for custody checking. Related uncertain
open/receipt/observation makes a known attempt uncertain; an uncertain terminal
observation cannot settle it. A true authoritative terminal observation settles
that known attempt; earlier provisional related records cannot undo that terminal.
Choose latest observations in original chronological order across selected segments
and input positions; do not reorder ordinary and uncertain histories by vector type.

The required set is every nonterminal committed/provisional admission at that
pre-continuation point, after all required captures have been parsed and before
candidate ordinary work. Validate exact set equality and opened/phase/disposition.
Unknown, missing, extra, duplicate or terminal entries refuse. Checked prior_evidence
is historical evidence at the time of the check: committed Live, RecoveredPending
and Recovered all normalize to the committed class when reopening; Uncertain must
match the actual provisional/uncertain class. Reopening cannot reconstruct which
committed evidence mode a previous process observed. Keep the original evidence
byte for study and never interpret it as new execution permission. Live preparation
will encode the actual observed evidence, not its normalized replay class.

Reconcile sends the complete current nonterminal set to CustodyVerifier, including
provisional admissions, after explicit recovery confirmation. Successful verification
never writes terminal state or promotes uncertain facts. Older recovered and uncertain
attempts remain nondispatchable; fresh ordinary admission is the only new permission.
No actual U3 worker-custody claim follows from the injected verifier tests here.

## Direct red oracles and execution order

1. Construct independently selected root/child history with prefix0/112 and a
   captured source+decision packet: packet length214 (=48+4+3+4+155); first_sequence1,
   original event sequence2. Choice with one descriptor payload152 gives seq2/end352;
   captured214-byte source seq3 plus marker dependency payload68 seq4 and commitseq5
   gives end352+246+100+56=754. Confirm exact original bytes and Uncertain event2;
   the chunk3 is not an ordinary source, decision is not an ordinary parent, and
   neither confirmation nor reconciliation grants dispatch. One choice+one marker+
   one original+one uncertain decision counts4; history budget3 refuses, byte213
   refuses and214 succeeds. These are literal wire/entry expectations, not a new
   round-trip/golden-dump system.
2. Source-only captures across multiple source-only commit groups; missing/extra/
   interleaved/misreferenced marker, bad descriptor length, origin mismatch, sequence
   overflow, malformed/trailing packet, origin-narrow limits and purpose2 stale cursor.
   Each case names the specific boundary it attacks; no duplicate proof layers.
3. Exact whole packet and individual uncertain queries before/after confirmation and
   a narrower active profile; duplicate input ordinals, authoritative+new captured
   packet, changed identity and mixed uncertain/new/source inputs. Pin zero cursor
   movement for pure queries and retained outer originals for conflicts.
4. Committed and provisional admissions with uncertain opens/observations: required
   set completeness, each checked field, historical evidence-class normalization,
   true versus uncertain terminal, purpose2 exclusion and recovered nondispatch.
   Include a later ordinary batch reusing an uncertain event: active batch rejection
   with unchanged uncertain entry; selected historical inclusion refuses the chain.
5. Allocation cuts at actual snapshot/packet/result ownership and selected-marker
   payload corruption; no partial publication, no ordinary-source leakage, old
   source routing remains intact. Run affected Mac profiles and current Neuroses
   profiles when accessible. Obtain independent code/oracle review and resolve it.

Implement classification/packet/index staging first, then shared uncertainty lookups
and diagnostic integration, then checked-set/custody collection. These are internal
steps of the same invariant. Until all are implemented, reject unsupported selected
maintenance explicitly; U2 cannot consume a partially integrated U1 owner.
