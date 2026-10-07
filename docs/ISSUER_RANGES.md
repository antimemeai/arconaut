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
