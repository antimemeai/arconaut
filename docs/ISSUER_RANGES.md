# Durable issuer ranges

Bound: original 25 minutes, deadline 1791382309; layer 1 allocator implementation,
count/performance oracle and direct recovery/failure tests; layer 2 one independent
CLI code review (90 seconds), findings fixes and affected checks only. No third layer.

Source grounding: RetainedState reserve_identity/apply/submit, pending namespace
and replay recovery; study-only LevelDB db/version_set.cc LogAndApply records
next_file_number and last_sequence before descriptor Sync, Recover restores them
and MarkFileNumberUsed advances past surviving files. Blackbird differs: globally
unique namespace/counter IDs can escape without a file, so no reuse/reclaim of
unused numbers is permitted and acknowledgement must precede every returned range.

Mechanism: retain existing IssuerReservationEvent encoding; counter means durable
upper bound, not most recently issued identity. Reserve up to 1024 counters at once
through ordinary submit with unchanged journal Sync/capacity/settlement envelope.
A private volatile namespace/last-issued/upper-bound cursor dispenses the range.
1024 reduces the 14,610-identity census to 15 reservations (without reopen), while
burning at most 1023 unused counters per crash. This is a bound, not a measured claim.
Reopen never restores the volatile cursor. Unknown sync returns no identity; its
surviving reservation highwater is burned by existing pending/replay recovery.
Namespace or externally advanced highwater invalidates a cached range. Every issue,
including the fast path, checks live admission and transaction fences. Saturating
range width (remaining UINT64 space) preserves the final IDs without wraparound.

Ownership: only RetainedState allocator fields/function; no event format, audit ID,
replay-cache, context restoration, timing or stream capture changes. Admission and
capacity remain authority at range refill; allocating from already durable space
requires no new journal capacity, and cannot spend settlement credits.

Direct oracles planned: exact committed reservation and synchronization counts;
monotonic/disjoint IDs across exhaustion, reopen, crash and namespace changes;
no identity on failed refill/blocked admission; UINT64 final counters; real native
file-backed matched workload timing and record counts. Remaining capture cadence
is deliberately unchanged. Results and review status recorded below when observed.

## Renewed continuation (2026-10-07)
Authorized second unit, absolute deadline 1791386844 (25 minutes total including
build/provider/review waits), not a completion claim for the preserved first unit.
Layer 1: finish exact 14610-ID fake and actual native file-backed count oracle,
matched same-ID-work timing (range versus durable-per-ID reference mode), verify
existing recovery/failure/admission/tail tests and add missing scoped properties.
Layer 2: one authenticated allocator review <=90s, fix findings and affected direct
recheck only. No third assurance layer or unrelated suite. Preserve event encoding,
durable acknowledgement, protected credits and startup lazy-history code unchanged.
Native candidate built separately <=j2; primary profile harness is not replaced.
Normal candidate release OFF; any profile observation explicitly ON and per-process.
Measurements report reservations/sync deltas and wall time, not total copied bytes,
allocations, short children, or startup latency. Deadline gaps remain explicit.
Commit/push candidate branch and persist completion marker (partial if bounded out).

## Renewed delivery / observed results

Layer 1 added fake and real native-file exact-count oracles. For 14610 consecutive
issued IDs, observed/asserted 15 committed IssuerReservationEvent records and 15
file synchronize calls, durable highwater 15360, last issued counter 14610. Count
excludes journal creation and subsequent reopen. Native reopen resumes at 15361;
SIGKILL after first issued ID resumes at 1025; distinct namespaces encode distinct
full IDs. Existing failure/pending/reconciliation/external-advance/MAX-tail tests
remain relevant. Added full protected-credit cached exhaustion/refill oracle and
reentrant transaction fence oracle. Protected credits are unchanged on fastpath;
refill still refuses capacity and only succeeds after releasing protection.

Two failing legacy oracles were corrected explicitly, not suppressed: coding's
atomic admission refusal budget no longer leaves room for three unnecessary issuer
records; captured predecessor reservation counter1 remains captured, but duplicate
active reservation after first issue is counter1024. Both affected tests pass.

One authenticated Codex gpt-6.1-sol read-only review completed within the 90-second
invocation bound (session 01a116ea-f17b-7a02-b7c3-61312f942161). It found a real fence
hole: a synchronization callback could block admission during successful refill,
yet an ID escaped. Fixed by rechecking live state after durable submission before
publishing the cursor; test blocks during synchronize, asserts no ID and committed
highwater1024, then reopens at1025. No second review or third assurance layer.
Layer 2 built own release blackbird and affected fixtures <=j2; scoped ctest coding,
retained_state, retained_environment, retained_state_crash, issuer_ranges: 5/5 pass
(12.17s). No unrelated full-suite rerun. Primary operator binary untouched.

Final matched native timed interval (same14610 issue calls, Release DEBUG=OFF):
range1024 = 0.063761750s, 15 reservations/syncs; build-only width1 reference =
88.372044708s, 14610 reservations/syncs. Same native journal/capacity/encoding,
sole allocator constant changed via scripts/issuer-single-baseline. This isolates
range amortization including retained publication costs; it is NOT historical
binary, startup, application throughput, or per-fsync latency evidence. Earlier
pre-review observations were0.067859583s/81.791909042s, not the final-source pair.

External whole-process time -lp: range real0.10s/user0.00s/sys0.00s (rounded),
max RSS1949696bytes; width1 real88.59s/user11.03s/sys3.19s, max RSS233947136bytes.
Whole-process includes post-interval properties (different extra crash checks), so
CPU/memory are descriptive only. OS block input/output ops both0 do not measure
all I/O bytes. Baseline live one-second sample succeeded and stack is persisted;
range process too short for live sample. No new native timing instrumentation in
chosen OFF release; primary harness/traces remain unchanged. No allocation/copy
census, short-child coverage or startup latency claim. JSON report and evidence
hashes: docs/measurements/issuer-ranges-r2.json; raw logs under lane build/.

## Accepted master integration

The operator's wrap-up direction merged this completed unit through1abb5b7 and
published the local Release runtime02872af (DEBUG/profiling OFF). Affected integrated
checks passed. Earlier candidate measurements above remain observations of those
specific workloads/source versions; no new integrated latency claim is implied.
Full current-state/suffix recovery remains a separate unfinished milestone.
