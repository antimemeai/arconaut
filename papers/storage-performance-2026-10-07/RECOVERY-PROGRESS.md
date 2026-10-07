# Native recovery work progress — 2026-10-07

Owner: candidate/durable-recovery-2026-10-07, seed fef64c1.
Period ends at Unix 1791408577; no extension. Candidate remains inactive.

## Short S1 subplan and bound

1. Implement owned 16KiB checked copy-on-write ordered index, typed fixed keys,
   fallible exact/range reads, root rollback, capped derived bytes. No resident keydir.
2. Wire index roots into RetainedState snapshots and route duplicate, dependency,
   transition and native historical lookup queries through it. Keep independent
   vector/full replay path as oracle while consumer migration is incomplete.
3. Generate split/update/range and fault outcomes; compare native indexed and full
   replay results, exact duplicates and attempt transitions. Start affected Release
   DEBUG=OFF application build early (started at beginning of period, <=j2).
4. Continue S2/S3/S4 only after their prerequisites are actually established.

Hardening: fix index encoding/bounds/read errors, duplicate identity and snapshot
rollback errors; direct index + retained-state/environment/crash/issuer/cold-history
checks. One scoped review of changed coherent path (including depended-on cold
payload equality/read ownership), findings fixes and affected recheck. Budget:
up to 17 minutes implementation/checks, 5 minutes review/fixes, final 3 minutes
handoff, all within the same absolute deadline. No third assurance layer.

## Source grounding

Read actual DURABLE_STATE_PLAN and supplied STUDY. Targeted pinned LMDB
`mdb.c:1354–1386,5012` shows alternating generation roots; Bitcask
`bitcask_nifs.c:140–149` shows location metadata rather than values. These guide
owned encoding and disk traversal, not imported execution or dependencies.
Aeron snapshot/boundary, SQLite checked selected reads and TigerBeetle addressed
checksums remain the written plan's publication inputs. No reference executed.

## Explicit status

- **S1: in progress**, not delivered: paged index and query migration starting.
- **S2: not started**: native saved current-state schema/constructor restoration.
- **S3: not started**: SAME root/state/index plus suffix-only environment reopen.
- **S4: not started**: bounded tail/publication/settlement and matched readiness.

Normal startup still replays all predecessors. No startup improvement claimed.
NEXT: add paged index encoding and native RetainedState query integration.

## First implementation/check checkpoint

Owned `recovery_index.hpp/.cpp`: 16KiB ordered COW tree, typed 40-byte keys,
checked LE scalar/address/generation/CRC page encoding, bounded traversal buffers,
exact and ordered range queries, immutable old roots, finite derived byte cap.
RetainedState attachment builds from authoritative full replay; snapshot owns root.
Duplicate identities (including namespace reservation and terminal observation),
latest observation, admission/invocation and retry/decision indexes, source lookup,
semantic ordinal and attempt-related chronological ranges now drive native queries
when explicitly attached. Apply/replay rollback restores the root. Audit inspection
uses fallible fact-count/ordinal queries rather than borrowed whole-history span.
Facade is shared with RetainedEnvironment. No new dependency/instrumentation.

Initial Release/OFF application build succeeded. Direct index generated 1600-key
permutation/splits/update/old-root/range/reopen/7-byte-I/O/read/write/corruption/cap
checks passed. Native indexed retained-state transitions/duplicates/retries/effects
passed; same oracle fixture without index passed. Indexed attachment is deliberately
excluded from the full-replay allocation assertion: attachment itself builds derived
pages, and the memory file fixture reallocates; it is not a startup measurement.
Cold-history native exact context/original/history/duplicates/lifetime/errors passed
with indexed fact and duplicate queries attached. Audit test passed. Environment
allocation-failure test initially segfaulted: inherited FramedJournal destructor
unconditionally dereferenced unattached file during allocation unwind. Null guard
and allocation error handling for unique→shared attachment fixed it; affected
native environment test then passed. One scoped independent review launched with
240s limit, including depended-on inherited cold path; result pending.

**S1 still incomplete**: index values temporarily name resident fact/source ordinals;
no bounded overlay, archive locator-only representation or full consumer census
migration yet. Explicit attachment is a prerequisite mode, not default fast reopen.
No index root publication, reclamation or current-state root exists. S2/S3/S4 remain
not started. NEXT: finish indexed native consumer/locator migration and replace
resident history vectors, before projection and suffix reopen.

## Affected native recheck and open limitations

Release/OFF recheck after destructor fix: retained_environment (including native
indexed pending/full confirmation/source/fact/dispatch path), retained_state_crash,
issuer_ranges and retained_state_indexed all passed (4/4, 5.95s total). Initial
retained_state/audit/cold_history/index checks passed as recorded above. Added
selected index corruption + failed derived write tests: duplicate/fact errors stay
errors, journal cursor/bytes unchanged, old root still supplies exact duplicate.
Indexed retained-state test passed with these cases.

The optional query mode presently writes copied paths per inserted key and retains
orphaned pages up to the supplied cap; cap exhaustion rejects new derivation before
journal admission. This is not S4 settlement backpressure or a reclamation policy.
1600 keys test produced bounded-tree disk work, not a native readiness measurement.
No provider readiness/read-byte/RSS result exists for this candidate; startup remains
full replay. No S1/S2 completion or overall completion is claimed.

## Review result and measured limitation

The single Kimi attempt did **not** provide a review: stderr reports provider
403/subscription access unavailable. Capture:
`context/kimi-review/run-9mr9qwdc` (private), one version event only, no substantive
findings and no final wrapper result file observed. No retry or replacement review.
Independent review remains missing, including inherited cold-history code. Candidate
stays inactive; tests do not imply integration or safety.

Only a prerequisite index fixture measurement exists: normal Release/OFF,
`/usr/bin/time -l recovery_index_test`: 1600 keys, 48,873,472 derived bytes (path-copy
write amplification is substantial), 0.13s real/0.08s user/0.01s sys, 118,226,944
max RSS and 117,850,544 peak footprint. This uses a memory-file fixture retaining
its bytes, not native startup/RSS or archive scaling evidence. Input/output blocks
reported zero, not actual journal read bytes. Internal multi-level split stress,
publication crashes and native request-ready measurements remain unperformed.
A failed write after copied-child completion/before new-parent publication also
preserves the old selected tree in the direct index test.

Both `build/durable-release/blackbird` and `build/release/blackbird` built <=j2,
Release and BLACKBIRD_DEBUG=OFF. No installed binary changed and no session restarted.

## Exact continuation edit (same milestone)

S1 remains the unfinished dependency. FIRST edit: change `RecoveryIndexEntry::value`
from a RAM-vector ordinal to an explicitly encoded checked audit locator (journal
identity, physical sequence/batch, payload offset/length/kind/checksum, semantic
ordinal; environment supplied by the eventual bound root). Update fixed-page size
arithmetic and index tests. Make `RetainedState::fact`, `source` and typed `lookup`
fetch/decode the selected authoritative frame. Change pointer-returning lookup to
owning fallible results scoped across transition validation: no permanent historical
cache and no dangling borrowed results when a second lookup occurs. Exact duplicate
checks must still fetch original bytes and retain application cold-read ownership.
Keep the independent eager/full replay path and generated transition expectations.

Then migrate main:199/205/221/247, session:143/169/199/219, context:134/324,
coding:789/825/850/1427/1515/1541, backstop:105 and station:20 to deliberate bounded
history or typed current-state queries. Audit inspection is the one migrated reader.
Native reconcile still enumerates admissions from all facts; migrate it to unresolved
closure (settled terminal summaries stay indexed), and complete bounded overlay
before claiming S1. RetainedEnvironment `load_selected` still opens every predecessor
and reconstructs Snapshot; S3 must replace that production path, not just standalone
RetainedState::open. S2's complete projection and S3/S4 remain unchanged requirements.

No further review or assurance pass was attempted after the failed provider call.
The final added tests are direct fault cases, not a new certification layer. No
unit-complete marker exists. Branch checkpoints are pushed; Root owns integration.
