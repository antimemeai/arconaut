# Native current-state recovery: design and implementation plan

2026-10-07. Status: written proposal for the remaining implementation, grounded
in inspected code; not independently reviewed, implemented, or activated.
No new worker is launched by this document. It replaces the broad durable-state
mission and optional smaller-delivery language as the execution direction.

## The goal and the planning correction

Ordinary reopen restores current state at a committed boundary, replays only the
subsequent tail, and reads archived history when asked. Holding current state and
tail constant, archive payload volume must not govern startup memory or I/O.
The operator's target is below100ms to an actual audited local provider request,
not the appearance of a prompt. Live input size and unresolved work are independent
costs; report them rather than promising constant work for arbitrary live state.

Earlier work has a real research basis in [STUDY](../papers/storage-performance-2026-10-07/STUDY.md).
However, it did not turn the complete state schema and reader migration into an
implementation plan. R1 substituted physical scan hints. R2 explicitly allowed
cold ownership without checkpoint recovery. That was Root's sequencing error,
not a worker independently refusing an otherwise complete instruction.

R2d32a764 builds and changes native readers; it is an inactive prerequisite.
On its matched335.8MB fixture, request-ready RSS353MB→6–7MB, but median warm
readiness198ms→245ms and reads672MB→1007MB. It still reconstructs history.
Its one reviewer attempt timed out; that gap remains. Existing issuer/projection
changes are prerequisites, not completion of this milestone.

## Sources that determine the design

These are mechanisms actually inspected, not names collected after designing.
Full pins, acquisition/restoration and literature are in the study and its manifest.

| Source and inspected location | Decision for Blackbird |
| --- | --- |
| LMDB700e10f91a65 `mdb.c:1354–1386`, `mdb_env_write_meta`, `mdb_env_pick_meta`; `mdb_page_split:10692` | Select a small generation root after referenced data is durable; update index pages by copying changed paths instead of rewriting the historical prefix. |
| Aeronad4baf8dcd5a `RecordingLog.java:1694–1775` | Snapshot state and replay boundary are one recovery decision. Starting replay at the saved log position is the actual optimization. |
| SQLite5af1b822f5da `walIndexRecover`, `walCheckpoint`, `sqlite3WalReadFrame:3740` | Outstanding tail recovery, explicitly ordered publication, selected fallible reads. A transient index is not authority. |
| Bitcask2010 paper; d84c8d913713 `bitcask_nifs.c:140–149`, `bitcask_fileops.erl:405–440` | Keep values on disk and retain exact locations; invalid accelerators fall back. Avoid its history-sized resident keydir. |
| TigerBeetlec95d7a53a3d0 `superblock.zig`, `grid.zig:75–103` | Bind references to address/content and generation; keep read ownership and cache policy explicit. Do not import replica/quorum machinery. |

The plan is our synthesis, not a dependency adoption. References stay study-only.
Our fault model and single-writer integration differ from those programs.

## One owner and three representations

The selected RetainedEnvironment/RetainedState owner coordinates publication.
Nobody independently publishes a context checkpoint that can disagree with the
effect ledger or issuer. One implementation lane owns this entire milestone;
the steps below are dependencies within it, not separate competing owners.

1. **Current state:** current session/configuration/context, unresolved-operation
   closure and recovery fences. Its size follows useful live state.
2. **Recovery tail:** physical and semantic records after a saved committed cursor.
   Preserve incomplete/uncertain suffixes and the existing recovery decisions.
3. **Archive index:** paged metadata pointing at immutable audit bytes. Its disk
   size may follow history; startup must not load its entire contents.

Proposed owned index: one fixed-page copy-on-write search tree with typed keys,
checked location values and ordered range iteration. Start with16KiB pages for
the first measurement; this is a tuning choice, not a measured optimum. Read
only traversal pages through bounded pread buffers. No mmap requirement, global
resident key map, LSM/compaction framework, generic database API or imported library.
Splits/path copying and root changes follow the inspected LMDB mechanism, with
our own simpler byte encoding and no borrowed pointers to mutable pages.

Index key families: semantic identity (including namespace-qualified reservations
and terminal observations), semantic ordinal, source journal/sequence, latest
attempt state, last admission per invocation, retry decision/invocation, context
revision, original ID, capture ordinal/group, RRC origin, and segment identity.
Values identify validated records or compact transition summaries; large payloads
remain in the audit. Exact duplicate checks fetch exact bytes, not just hashes.
New uncheckpointed changes use a bounded overlay over the published index.

## Complete saved state and the queries it replaces

Do not serialize Snapshot::facts or a list of every PhysicalJournalRecord.
The following is the required schema coverage before enabling the fast path.

| Saved section | Fields and native consumers |
| --- | --- |
| Boundary | Format/reducer version; environment, selected head generation, active journal/header identity; sequence/end offset/commit anchor; index root; total semantic/physical counts and capacity accounting. FramedJournal and RetainedEnvironment use it. |
| Issuer | Active namespace and durable reserved upper bound. RAM allocation cursor/unused IDs are never resurrected. Tail reservations advance the bound before issuing IDs. |
| Session | Participant/conversation/workflow identity, latest settings, latest RRC token/state/note, injection origin linkage. SessionStore/session_identity/resume stop scanning old program/context records. |
| Effective programs | Program config/revision, tool definitions/revision, context budget/revision. Restore only the durably effective generation; staged proposals do not become effective because a process restarted. CodingEngine's constructor consumes this section. |
| Context | Current revision and exact live entries/order; relevant origin/group metadata; roots for originals, capture order and history. ContextStore restores this view directly, never parses rejected historical candidates to rediscover it. |
| Effects | Unresolved attempts with admission/open/latest observation/receipt, evidence and reconciliation requirement; invocation/decision linkage and exact input/continuation references. Custody checks load only this closure. Settled attempt summaries stay disk-indexed for later transitions/retries. |
| Recovery chain | Selected predecessors and maintenance/capture requirements; uncertain evidence and provisional originals where durable. Historical segment catalog is paged. No cached state grants old attempts permission to dispatch again. |
| History/display | Persistent semantic ordinal and display/original locators. Audit inspection, transcript window, complaints, backstop selection and station configuration use bounded queries, not a whole-history span. |

All14 RetainedKind cases need a documented reduction: reservations update the
issuer; decisions/invocations/admissions/open/observations/retries/receipts update
dependency and attempt indexes; conflicts/complaints/rejections remain indexed
history; recovery choices/captures preserve chain requirements; application
records update the appropriate typed projection and remain exact in history.
Nonterminal observation lookup and terminal duplicate identity are different keys.

Existing source-dependency existence and duplicate/effect-transition predicates
remain unchanged. Replace their reverse scans with indexed queries. Preserve
semantic ordinals that exclude deduplicated facts; physical sequence is a different
quantity. RAM-only proposals, Lua stack, child handles, settlement scope and pending
tool calls are not revived from a file. Recreate settlement credit through existing
recovery before new work; do not treat saved credit as permission/capacity.

Required migration census from candidate d32a764: `committed_facts()` callers in
main, session, audit, context, coding, backstop and station, plus RetainedState's
existing/find_event/source/attempt/dispatch/reconcile paths. Replace ordinary
startup and hot queries. Full-history traversal remains an explicit paged iterator
or forensic replay; it cannot hide behind a noexcept borrowed span.
RetainedEnvironment currently opens and replays EVERY predecessor segment. Merely
optimizing RetainedState::open leaves that production path unchanged and fails.

## Publication and recovery sequence

Fault model: local single writer; crash/torn writes, short/failing I/O, missing or
accidentally corrupt selected data. Normal fast reopen trusts a correctly published
reduction; it does not authenticate hostile replacement or reread every old byte.
CRC checks bytes actually read. Complete archive verification is an explicit path.

1. At a sequential-owner boundary, select a fully acknowledged commit. No prepared
   transaction is included. Unresolved effects may exist and must be represented.
2. Encode current-state sections and changed index pages to derived storage using
   checked little-endian fields; never persist C++ layouts/pointers. Bind page
   references to generation/address/length/checksum. Validate bounds before allocating.
3. Synchronize referenced derived data after the authoritative journal commit is
   durable. Encode the root with section locations/checksums and that exact cursor.
4. Publish into the alternate root slot through existing JournalDirectory seams:
   complete write, file sync, replacement, directory sync. Keep the previous root
   and its referenced pages. Incomplete publication cannot destroy the older view.
5. Reopen under the writer lease. Read the selected environment head and both roots;
   check identity/version/current sections and the commit anchor. Select the latest
   usable generation. Unknown versions or invalid roots use explicit full recovery.
6. Open FramedJournal AT that committed boundary, retaining counters and paged
   historical lookups. Scan/check physical frames and semantically apply ONLY the
   suffix through the same transition rules used by ordinary appends.
7. Restore native session/context/configuration from the saved projection plus
   tail. Keep ordinary admission closed until confirmation and custody reconciliation
   complete. Damaged/uncertain suffixes use existing inspect/reconcile behavior.

No new audit format is required for the first version: roots/indexes are derived
sidecars. Existing journals remain unchanged. Legacy first-open/full fallback may
scan history once and publish a usable root; distinguish it from routine reopen.
An older root is usable only with its entire subsequent tail, never by ignoring
newer reservations/effects. Selected index/read errors never mean an absent record.

Checkpoint work is triggered by both tail bytes and records, not elapsed time
alone. Choose finite soft/hard thresholds from measured tail costs. At the hard
threshold, defer NEW ordinary work until checkpoint progress; retain enough
capacity to settle already admitted work. Bound one legal batch and remaining
settlement separately. This requires a direct stalled-publication/settlement test.
Do not weaken pre-effect durability or continue growing an unbounded tail.

Derived file growth must be recorded and capped using ordinary storage capacity;
on inability to publish, retain the prior root and bounded backpressure. Reclamation
must respect readers/previous generation. No unbounded append-only sidecar may be
silently called production-ready; reclamation is a separate measured follow-up
if it does not fit, with that limitation explicit.

## Dependency-ordered work and observable endpoints

| Step | Required integrated endpoint | Direct discriminator |
| --- | --- | --- |
| S1: indexed state queries | Same transitions and historical results without requiring resident fact/source vectors. One paged index, bounded tail overlay, exact duplicate/attempt/source queries. Existing full recovery populates it. | Generated known outcomes plus eager/indexed comparison, including rejected/no-op/duplicates, namespace changes, retry and terminal rules; root/path split/short-read failures. |
| S2: native current-state projection | Existing full replay maintains the saved-state schema; SessionStore, ContextStore, CodingEngine, custody and environment consumers restore directly from it. | Exact settings/config/revision/live entries/original repair/RRC single injection and unresolved linkage; no archive-wide JSON pass by constructors. |
| S3: checkpoint plus suffix reopen | Durable publication and native RetainedEnvironment open select that SAME projection/index and replay later records only; legacy fallback intact. | Exact full-replay equivalence at varied boundaries; measured startup reads exclude archived payloads; predecessor-chain and uncertain suffix cases. |
| S4: bounded maintenance and delivery | Ordinary operation keeps tail bounded, preserves settlement during failed checkpoint, and delivers a measured candidate usable through native application paths. | Faults at every data/root sync/replacement boundary; stalled checkpoints; matched native request-ready measurements; one scoped code review/fixes/direct recheck. |

S1/S2 alone are prerequisites, not successful startup delivery. Do not replace
S3 with another physical hint, cold-pointer experiment, cache tweak or helper.
The independent full replay remains the reference path; do not make both test
sides load the same proposed checkpoint and call equality evidence.

Execution periods remain bounded at25minutes TOTAL, including providers/builds/
review. Those bounds do not imply this architecture fits in one period. At expiry,
commit/push useful code and write exact unfinished step/next edit/build state.
Keep the same milestone; never mark full completion because the clock ran out.
Completion markers that halt the driver must distinguish prerequisite delivery
from milestone success. No implicit allowance reset or new launch is authorized
by this plan. Start application builds early and batch independent reads/tool work;
the last run recorded69 provider responses and990.297s of provider elapsed time.

No added review of reviews or third assurance layer. One bounded review of the
changed coherent path and its direct fixes/recheck; any timeout remains a gap.
The existing unreported R2 review is not approval. Further code/review allocation
and master integration remain explicit decisions, not document side effects.

## Measurement that decides whether we succeeded

Use matching normal Release/DEBUGOFF native applications and a deterministic
local provider with successful audited admission and known settlement. Keep
provider network latency out of local readiness; do not omit journal syncs.
Deliberate dev timing/resource capture remains opt-in, persisted privately.

- Fixed live state/tail: vary archived payload bytes independently of record count.
  Include the existing336MB workload. Separately vary historical record/segment
  counts to catch metadata trees or predecessor scans secretly loaded in full.
- Fixed archive: vary live input and unresolved closure; vary suffix bytes/records
  up to configured hard bounds. Report large-live serialization separately.
- Report readiness distributions, actual read bytes, semantic replay count, syncs,
  CPU, RSS/OS footprint and available allocation/copy accounting. Warm and actual
  cold/first-open conditions are separate; unavailable measurements stay unavailable.
- For fixed live state/tail, ordinary startup must show no full-prefix read/replay
  and no resident historical index proportional to record count. Small paged-index
  traversal may grow with tree height; do not falsely claim strict constant time.
- Below100ms remains a measured target on declared workloads, not an automatic
  consequence of checkpoints. If durability/live work dominates afterwards, report
  its exact spans and plan that work; do not trade away acknowledged persistence.

The direct correctness oracle is exact state/IDs/effects/history versus full replay
on histories with known intended outcomes. Crash outcomes are complete old-root+
tail or complete new-root+tail, never mixed sections, reused IDs or blind retry.
Only that integrated candidate counts as completion of this recovery milestone.
