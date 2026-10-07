# Cold historical payloads: implementation direction

Operator explicitly resumes the redesign after the physical-hint partial. This
unit changes payload ownership and the consumers that defeat demand loading.
It is a foundation for compact current-state recovery, not a new hint optimization.
One native BB owner handles storage references, loading, publication and consumers
within this same invariant. No new dependency, survey or worktree proliferation.

## Required useful behavior

On the existing history-heavy, tiny-live-context workload, historical application
payloads must no longer remain resident merely because replay encountered them.
Cold records carry exact disk locations/identities and checked sizes; explicit
reads supply bytes when inspecting history or restoring originals. Useful active
state/caches may consume substantial memory, with purpose/lifetime/benefit stated.
No arbitrary memory ceiling. Working-state checkpoints must reference cold history,
not re-embed the whole archive.

Main's native session/context/coding path must use this representation. A standalone
index/writer/helper without integrated consumers is not this unit's delivery.
Archive content and source/effect linkage stay exact. An explicitly eager legacy
or forensic path may remain, but its memory cannot be the ordinary path by accident.
Do not conceal loading behind .data() and permanently pin every visited record.

## Source mechanisms to use

- LMDB700e10f91a65, `libraries/liblmdb/intro.doc` and `mdb.c`: immutable borrowed
  value lifetime; select a durable root rather than allocate all value contents.
- SQLite5af1b822f5da `src/wal.c:3740`: selected frame reads by checked offsets.
  Buffer size follows selected work, not total historical values.
- Bitcaskd84c8d913713 `c_src/bitcask_nifs.c:140`: location metadata separate from
  values; do not import the all-history resident keydir growth.
- Aeronad4baf8dcd5a `RecordingLog.java:1694`: snapshot restored at a definite
  replay boundary. Current context must eventually start at such a boundary;
  discarded candidates must not be read solely to reconstruct an already saved view.
- TigerBeetlec95d7a53a3d0 `src/vsr/grid.zig`: explicitly bounded cache ownership;
  immutable references bind address and contents. No distributed repair machinery.

Pinned extraction paths and restoration in references.json; read actual relevant
code. The general STUDY.md and r1 report establish the earlier decisions and limits.

## Integration points established by Root's source inspection

`RetainedState::replay` decodes events and owns them through Snapshot::facts.
`Decoder::blob` copies bytes; `ApplicationRecordEvent::payload` currently requires
resident ImmutableBytes. `committed_facts()` exposes decoded history to ContextStore,
AuditLog, SessionStore, CodingEngine, main inspection, station and backstop.
ContextStore also owns history_, captured_, originals_ and current entries_.
These consumers need deliberate current-state/history access, not an implicit
whole-history materialization behind an apparently cheap span accessor.

Choose a coherent implementation shape in a short concrete design note before
editing. Fallible archival reads should return errors; define ownership across
refit/close/recovery, malformed offsets and missing files. Existing logical event
equality/duplicate submission must remain EXACT when one side is disk-backed;
a checksum or empty placeholder is not byte equality. Preserve recovered-pending,
uncertain suffixes, settlement credits and custody fences. No new effect permission
comes from a cached state root. Historical state/repair stays readable.

Full current-state checkpoint integration remains the architecture's destination.
If it cannot fit the original allowance, a delivered cold-payload representation
and integrated native consumers are a useful complete slice; a new physical hint
or uninhabited prototype is not. State which event kinds/consumers remain eager.

## Direct checks and experiment

1. Cold versus eager/full replay: exact current context, originals/history pages,
   reservation highwater, duplicates and pending effects. Use a generated history
   containing accepted, rejected and no-op context edits.
2. Historical read errors: wrong identity, truncation, corrupt data/offsets, failed
   reads, owner lifetime; no partial publication or blind uncertain-effect retry.
3. Native ReleaseOFF prompt AND actual CodingEngine audited request readiness on
   fixed live state/tail while archived payload size varies. Record actual read
   bytes, retained payload bytes, allocations/copies where measured, CPU/wall and
   memory categories. Test varying record counts separately. Warm/cold separate.
4. Native application target must build before final handoff. Reserve build time;
   begin affected builds early rather than leave main compilation to the deadline.

Normal DEBUGOFF remains unchanged; only deliberate development observations enable
instrumentation. No imported code execution or mutations. One review of NEW source,
findings fixes and affected recheck only. No re-review of unchanged r1 source or
additional certification. New explicit operator scope permits this fresh bounded
unit; deadline set at launch,25minutes total including model/build/review waits.
