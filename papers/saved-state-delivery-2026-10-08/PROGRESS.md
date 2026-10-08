# Native saved-state delivery — partial candidate, 2026-10-08

Owner: slot-0. Branch `candidate/saved-state-delivery-2026-10-08`.
Implementation checkpoints: `bbb6ec1`, `0285d35`, `1e34d31` (all pushed).
**Not complete. Do not integrate as completed saved-state recovery.**
No primary checkout, installed binary, Beads, pool or board was edited. Builds are
private checkout `build/release`, Release / BLACKBIRD_DEBUG=OFF / -j2. No restart.

## Implemented native behavior

- RetainedState owns a versioned contiguous current-state reduction: exact current
  context/order and the corresponding *original* live entries, effective session
  identity/settings/RRC packet, effective workflow/tool/budget packets, issuer
  reservation highwater and unresolved-operation linkage. It does not serialize
  Snapshot::facts or the physical record vector into the saved state.
- Two checked root slots bind header/environment/journal/issuer namespace, exact
  acknowledged commit cursor and commit-frame checksum. New derived catalog is
  written/synced/replaced/directory-synced before the root. Root itself is fully
  written/synced/replaced/directory-synced. Prior slot remains available. Short
  writes and failed publication report errors without replaying uncertain writes
  or undoing an acknowledged audit commit. Lost extent evidenced by a valid root
  is an error; a damaged full-replay prefix stays inspectable and fenced.
- Missing/corrupt/legacy roots retain independent full replay. Saved projection
  makes ContextStore skip prefix context JSON; current programs are reduced from
  effective records, folding omitted unchanged program_config and independent
  budget chronology. Session/Coding constructors use the program projection.
  Staged/failed labels do not become effective. The actual main RetainedState
  path consumes this projection; it does NOT yet skip ledger history.
- Context original/history access is deferred. It currently materializes exact
  cold archive metadata on first explicit historical access. Accepted context
  suffix transitions deliberately fall back to full ContextStore reduction until
  archived-original predicates are paged. This is safe fallback, not bounded
  historical query completion.
- ArchiveCatalog publishes sorted 96-byte typed-key-to-physical-locator entries
  in contiguous batches. Reopen retains just a checked header/file handle;
  binary queries check selected entries and return fallible absence/error.
  No historical RAM ordinals are persisted. RetainedState fact/source queries
  and identity/source-dependency predicates use this catalog. The predicate
  resolver still finds its borrowed fact/evidence in the resident replay oracle.
- FramedJournal can scan only after a supplied checked JournalResume anchor,
  accounting for archived physical records in admission capacity. Tests cover
  append/capacity/anchor/truncation. **RetainedState does not activate this seam**:
  no compact semantic Snapshot restore exists yet. Restage on this lower-level
  seam is fenced unsupported rather than comparing suffix metadata to a full
  prefix. RetainedEnvironment remains full-replay multi-segment fallback.

## Direct checks and the single findings recheck

Direct generated saved-state test: current view/settings/programs/highwater,
archived removed original restoration, exact historical reads, accepted tail,
RRC single injection, unresolved open effect/custody fence, staged/failed labels,
short write, file sync, replacement, directory sync failures, corrupt root fallback.
Archive test: 1,600 exact keys, generation replacement preserving old reader,
identity/bounds, corrupt and short selected reads. Physical resume test: suffix
only records, complete capacity accounting and append, wrong commit CRC and
truncated prefix. Existing context and cold-history exact/duplicate/fault tests.

Scoped recheck ran only saved_state/archive_catalog/journal_resume/cold_history/
context. Four passed; cold_history exposed overly strict rejection of the
already-fenced damaged semantic prefix. Fix distinguishes actual lost extent
from full-replay damage. Only that failed check was rerun and passed. No third
assurance layer or unrelated full-suite run. Final candidate native binary built.
Independent concrete-code review remains Root's responsibility.

## Actual diagnostic measurements (not success)

Private APFS clones only; immutable seed never opened writable. Heavy original
and conversion clone SHA-256 both:
`c13736475ca4fc027926a2a388348378a7158c46e0688e65789e1d48b133e331`.

Three intermediate probes: prompt 610.177 / 509.104 / 921.121 ms;
admitted 704.285 / 583.398 / 983.390 ms. All exact input 302,241 bytes and known
success settlement. This is not a matched statistical performance claim.
Final code probe: prompt **339.268 ms**, admitted **397.796 ms**,
reads through admission **673,841,790 bytes**. The physical and semantic archive
passes remain; archived rejected bodies are still read twice. Goal ZERO is not
met, nor is <100 ms. Full-replay conversion is separate from routine reads.
Final conversion published 734,870 derived bytes across catalog/root and used
6 sync calls (stdout retained in tool audit). Roots about597 KiB useful state;
ArchiveCatalog 1,600-key file exactly153,760 bytes. No claim about cold caches.
Raw diagnostic outputs are adjacent to this file.

## Limits requiring continuation

Publication trigger is opportunistic on ContextStore construction/mutations:
64 physical sequences or4 MiB since saved boundary. It is NOT yet hard tail
backpressure, and program-only/ledger-only traffic is not bounded by it. There
is no settlement-capacity/stalled-checkpoint maintenance policy. Archive catalog
is a full metadata bulk rewrite on checkpoint, not a dirty-page incremental tree.
Two replaced generations bound successful file growth, but failed temporary files
are not reclaimed. Existing directory seam has no unlink; add bounded cleanup.
Unresolved closure currently includes old nonterminal observations for each still
unresolved attempt, not just the latest transition: reduce this before enabling
compact semantic restore. StationStore/RRC origin lookup and historical consumers
still have full walks. These are incomplete criteria, not certification caveats.

## Authorized continuation, deadline 1791431497.25 — still NOT complete

Pushed code checkpoints `944f5fc`, `8d377da`. Catalog ordinal relocation finding
is fixed in format v2 with bounded CRC-covered uint32 ordinals and direct intact
entry relocation coverage. Root's context tail/forward-reference and resident
copy findings are fixed. Typed semantic/source readers own checked selected bytes;
context original/history/index explicit queries use fallible bounded pages and
owned generation readers. Latest-only unresolved observations and original retry
invocation decision closure are retained. Reconcile, restart prerequisites,
provider recovery linkage and complaint active sets now use closure/exact queries.

Root/catalog selection is under the actual writer lease before physical scan.
Compact Snapshot counts/highwater and selected JournalResume suffix restoration
are coded, but **no generated root emits its coverage marker**. Therefore the
unsafe/incomplete candidate remains inactive. `committed_facts` is explicitly
unsupported on compact state, not silently truncated; ordinary reducers have an
explicit `tail_facts` API. Further intent migration and compact maintenance remain.
Root correctly clarified that a new semantic-envelope subsystem is NOT required:
exact typed queries use selected checked frames, explicit forensic traversal is
allowed to read old bodies on demand. This correction is the continuation design.

Early native target and final native/dev probe built Release DEBUGOFF -j2.
One scoped recheck: archive_catalog, journal_resume, saved_state, context,
cold_history, session_store, session_recovery all passed. Direct new lease-selector
and latest-unresolved-observation checks passed. No unrelated full-suite or third
assurance layer. No restart, installed binary, primary source, board or Beads edits.

Actual private APFS clone `/tmp/astra-continuation-1791431378`: migration published
1,469,740 bytes, read 1,087,247,736 bytes, 10 syncs. Routine **open-forbid FAILED**,
exit1, `audit_unavailable`, **forbidden_attempts=1**; no admitted request or known
success settlement. This is not the required zero-read delivery. Raw outputs are
`continuation-migration.txt` and `continuation-forbidden.txt`. Seed was never opened
writable. No unit-complete marker. `continuation.json` records exact remaining
activation/maintenance/consumer work; do not integrate this as completed Astra.
