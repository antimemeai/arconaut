# Saved-state delivery: current outcome, 2026-10-08

Operator authorized Astra's integrated current-state/suffix proposal. Native
implementation ran in one reused slot for the declared 25-minute allowance.
Candidate `candidate/saved-state-delivery-2026-10-08`, tip `c06615e`, is pushed,
clean, and inactive. Master and installed runtime have no candidate production code.

Implemented: compact current context/configuration and unresolved linkage,
two checksummed durable saved-state slots, paired sorted disk locators with batched
publication, lazy context archive restoration, and a checked physical suffix seam.
Native context restoration consumes the saved projection. Native semantic recovery
still scans the full prefix; the suffix seam is deliberately not activated.

Direct scoped saved-state, catalog, physical-resume and context tests passed.
Cold-history recheck exposed damaged-prefix inspection regression; the owner fixed
it and reran that failed check successfully. Those results do not demonstrate
integrated semantic suffix recovery. The owner retained its full progress and exact
continuation on the candidate under papers/saved-state-delivery-2026-10-08/.

Heavy fixture diagnostic: prompt339.268ms, local audited provider admission397.796ms,
673,841,790 audit/read bytes through admission, identical302,241-byte input,
known-success settlement. This is one diagnostic, not a matched percentile study.
It still reads the rejected archive twice. The ZERO rejected-body-read target and
under100ms target are unmet. Do not substitute it for the previous100-start results.
The1600-key catalog now writes153,760bytes, versus48,873,472 in the old per-key COW
experiment; this is the measured bulk-publication improvement, not startup success.

## Preliminary concrete-code review finding (unresolved)

ArchiveCatalog entries checksum their key/locator but not their ordinal/address.
Moving an intact96-byte entry or block preserves its checksum and can corrupt
sorted lookup, returning false absence. Bind ordinal inside checksum coverage and
validate it on every selected read; bound count to its encoded width. A direct
relocated-entry corruption case must fail rather than return absence. This finding
was delivered at the next native turn, but the hard deadline ended that turn before
an edit. No fix or complete independent code review is claimed.

## Exact remaining implementation

Select a paired root/catalog under the journal writer lease before scanning.
Restore compact issuer/unresolved semantic state and replay only the suffix.
Replace resident fact/source dependency resolvers with checked disk payload reads;
use bounded tail state, preserving duplicate/transition/unknown-effect behavior.
Migrate current consumers that still traverse committed_facts, including RRC and
station recovery, and page original/history queries instead of loading all history.
Bound ongoing tail maintenance and failed-publication storage; preserve settlement
capacity. Only then activate native RetainedState suffix recovery and run the
independent forbidden-read oracle through a real local provider admission.
RetainedEnvironment multi-segment behavior remains the existing full-replay fallback.

Root's dev-only read oracle is committed on master: it refuses audit reads crossing
any of the640 rejected-body ranges in the heavy fixture. Baseline fails with one
attempt and audit_unavailable. No candidate success is claimed. Once integrated,
repeat matched100-start fresh/heavy/unusual workloads using normal Release/OFF
binaries; do not time the deliberately fault-injecting oracle as ordinary startup.

The native manager returned unknown at its hard deadline. Root inspected the
process table: no managed group26085 or checkout programs/builds remained. Native
slot was explicitly settled and checkpointed; the board records blocked/incomplete
and releases its active reservation. A leased inactive checkout is retained for
continuation. Collector persisted687 process-tree observations; raw artifacts stay
under context/saved-state-delivery-2026-10-08/. No automatic deadline extension,
blind request retry, new review campaign, or partial-success marker.
