# U1 Kimi plan rereview

Unchanged assistant reply; read-only review, not execution evidence.

## Re-review: docs/U1_RETAINED_STATE_SUBPLAN.md (post-integration)

**Scope:** reread the revised sub-plan against the original five findings + capacity minor, checking resolutions and introduced interactions against CORE_DESIGN, TESTING_PLAN T1/T3, and foundation.hpp. Read-only; no execution.

**Format arithmetic rechecked:** the new 112-byte header sums correctly — 8+16+16+8+16+8+8+4+4+4+4+12 = 108, +4 CRC = 112, CRC over the first 108 bytes (lines 41–45). Frame header, commit payload, and the new semantic-frame prefix (schema u16 + event kind u16 + dependency count u32 + refs of 16+8 bytes, count bounded by payload length before allocation) are internally consistent.

### Finding resolutions

**F1 (unacknowledged tail disposition) — resolved, and your correction is sound.** Lines 76–87 now state the rule I asked for without inventing an on-disk ack bit: all reopened batches are labelled recovered/acknowledgement-unknown, staged unpublished, made authoritative only by a *new* successful sync ("authoritative now, not proof that the earlier sync/ack occurred"), with admission closed until recovery sync plus explicit owner reconciliation, and open attempts fenced in the reconciliation set with IDs discoverable even before recovery publication. This matches CORE_DESIGN lines 130 and 133–135 exactly. The duplicate-submission red case (lines 203–205) — recovered-pending before recovery sync, recovered/reconciliation-required after, never not-found/dispatch — is the right oracle and closes the double-dispatch hole I raised. I verified consistency with the staging path: line 84–85 makes staged IDs visible to dedup lookup, so the "returns existing state" contract (line 141–142) has a defined recovered-state flavor rather than a gap.

**F2 (continuation ownership) — resolved.** U1 now owns linked continuation with a concrete header lineage (predecessor journal ID, validated commit sequence, validated end offset), a defined sync order (predecessor → new header/batch → parent directory), failure-leaves-admission-closed, retained diagnostic bytes, and blocking on missing/corrupt predecessor headers with exposed loss rather than chain-guessing (lines 104–113). The first-batch records the recovery choice and reconciled attempt dispositions, giving U3 a concrete seam later. The capacity-vs-poison asymmetry minor is also fixed (lines 112–113: capacity rejection has no partial write, does not poison).

**F3 (storage seam) — resolved.** `JournalDirectory` (`create_exclusive`/`open_existing`/`synchronize_directory`) plus `NativeJournalFile : Storage` with `lock_writer` (lines 115–121) covers every operation I listed as missing, with named error mappings (conflict/busy/unsupported/io+errno). Extending the ErrorCode enum in U1 while leaving the U0 data-I/O interface untouched is a reasonable bounded choice; it is a one-line compatible change to foundation.hpp, not a U0 redesign.

**F4 (issuer rebuild) — resolved.** Rebuild consumes the max counter over committed/recovered reservations *and* fully decoded CRC-valid pending reservation frames, burning uncertain ones (lines 158–160), and the namespace rotates on continuation only after checking every namespace in the intact predecessor-header chain (lines 162–166) — this correctly covers the "pending counter bytes damaged" case that pure counter-rebuild could not. Your rejection of my proposed red trigger is correct: a conformant issuer never returns an ID before its reservation sync (line 160–161), so the honest oracle is retained-reservation non-reuse, which lines 207–209 now test ("complete unsynchronized reservations as consumed and the next returned ID as distinct"). Burning CRC-valid pending reservation frames is conservative in the right direction; scan reachability is bounded by the no-scanning-past-damage rule (line 70), so the burn set is well-defined.

**F5 (dependency check point/encoding) — resolved.** Dependency references are now a required decodable frame-level prefix field (journal ID + source sequence), checked during replay against an earlier authoritative source or same-batch source before publication, with cross-predecessor references permitted only within the validated chain (lines 63–70). The check location (replay, pre-publication) and the withholding rule (line 69) are exactly what was missing; the damaged-earlier-source red case now has a defined oracle.

### Introduced-interaction check

I looked specifically for contradictions created by the resolutions:

- **Recovery-publishes-pending vs CORE's "incomplete batches never authoritative":** consistent — only *complete* surviving batches become authoritative via the new sync; incomplete/torn material stays diagnostic (lines 75, 102–103).
- **Recovered batch depending on a lost pending batch:** a recovered batch whose dependency references a source only present in an incomplete/damaged earlier batch fails the replay check and is withheld with semantic-conflict cause — the withholding is permanent-but-visible on that file, and continuation preserves the bytes. Consistent with line 70.
- **Sequence restart across continuation:** new journal sequences restart at 1 while dependency references carry journal ID (lines 47, 64), so cross-chain references are unambiguous; header's predecessor validated sequence/offset fields anchor the chain tip. No collision.
- **Namespace rotation vs ID stability:** old IDs keep old namespaces; rotation only affects newly issued IDs, and the chain-wide namespace check prevents reuse. No conflict with the exact-non-reuse claim, which is explicitly conditioned on intact chain headers (lines 165–166).
- **Continuation before U3 exists:** "reconciled attempt dispositions" in the continuation's first batch are supplied by "operator/model recovery code" (line 24) rather than U3's living-resource evidence; for U1's own tests the injected seam and staged reconciliation set suffice. Acceptable scoping, not a hole.

### Residual minor (non-blocking)

1. **Flags field validation unspecified (line 44–45):** root=1/continuation=2 are defined, but nothing states whether unknown flag bits or both-bits-set reject open. Given red cases already cover "reserved fields," add flags to that oracle (unknown bits and root|continuation together reject). One sentence suffices.
2. **Recovery sync strength:** line 79's "new successful selected sync" should name that it uses the file's declared profile strength (full on Mac profile), so a data-strength sync can't silently certify the recovered tail. Arguably implied by "selected sync" elsewhere; making it explicit costs nothing.

### Verdict

All five findings and the capacity minor are concretely resolved with matching red cases; the F1 correction (stage → resync → publish as recovered, no ack-delivery fiction) is faithful to CORE_DESIGN and T1. The two residual items are one-line specification tightenings, not structural problems. The unit remains bounded — continuation is a single linked-file operation, not a rotation/segment framework, and no second durability system crept in. The sub-plan is ready for red-case authoring once the flags/recovery-strength sentences land.

**Limitations:** document-level review only; no implementation exists to execute. Grounding papers (refit-custody, audit-state studies) were not re-derived; format and recovery rules were checked for internal and cross-document consistency only.
