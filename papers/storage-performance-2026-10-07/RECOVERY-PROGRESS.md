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
