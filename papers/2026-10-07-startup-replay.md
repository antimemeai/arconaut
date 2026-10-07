# Silent operator startup: audit replay copying

Purpose: reopen the operator's existing conversation promptly, preserving every
original and recovery fence; keep the development campaign running separately.
Observed2026-10-07: G4 supervisor62238/native62248 active, caller startup on
context/my-arco silently computes. Audit315711420bytes. Sampling native67752 shows
RetainedState::replay repeatedly constructing the full prior RetainedFact vector.
Actual source copies Snapshot at every committed batch, including growing payloads.
This is quadratic retained-payload copying. Killed only Root's diagnostic startup
reader before it reached writable confirmation; operator originals unchanged.

Grounding: CORE_DESIGN complete-batch publication/rejected-suffix semantics;
RetainedState::apply/source replay and existing invalid-batch/crash/allocation
oracles. Replay is prepared, not visible. apply only appends facts/sources and
advances issuer counter in this path; identity/dependency checks inspect existing
facts but never mutate them. Replace full prefix copies with a batch-local rollback
checkpoint of sizes/counter. On invalid batch or early return restore that exact
prefix; successfully validated batch keeps appended entries. Guard unwinds on
allocation failure. No changes to ordinary append publication, on-disk encoding,
recovery confirmation, original retention or unknown-effect handling.

Direct red oracle: reopen many distinct payload-bearing batches while counting
allocated bytes, assert a generous bound proportional to journal bytes rather than
all prior prefixes, and assert exact facts/payloads. Existing rejected-batch/source/
counter and crash tests attack atomicity. Mac release and ASan affected checks;
actual315MB operator audit reopen timed to UI/EOF. No timing threshold used as the
correctness oracle. Build qualified source in isolated archive/build, excluding
concurrent G4. Never replace operator executable with unaccepted G4 by accident.
Total45min allowance/hardening25min two layers, no third. Launcher reports opening
and interactive pin choice may separate UI from mutable campaign build path; actual
selected executable and remaining limits recorded at delivery.

## Delivery observations

Follow-up actual reopen still timed out at30s; its wrapper child outlived the initial
subprocess timeout and then exited. No escaped diagnostic reader remains. Subsequent
probes use their own process group and bounded group shutdown. Sampling identified
forward identity reservation copying prior payloads. Fixture initially used private/
nonexistent methods; corrected to public reconcile/issue before treating it as an
oracle. Corrected old representation failed the payload-alias oracle. Replace byte
vectors in retained event payloads with read-only shared values (std::shared_ptr
const vector), preserving wire format and exact equality; changed-value tests now
construct replacement bytes rather than mutate a retained share. No new library.
Forward total fixture allocations include metadata and in-memory filesystem writes;
a guessed total-byte bound was discarded, not relaxed into an artificial pass.
The direct invariant is that existing byte storage is shared and exact content stays.

A phased actual run spent23.4624s in root replay/recovery,0.7441s reconstructing
context, about0.011s saving settings/issuing engine IDs. Add temporary replay-only
fact/source lookups, built once, owning no payloads, rolled back with bad batches,
destroyed after replay. Ordinary append still copies metadata containers; that is a
reported waste-audit finding, not silently claimed fixed. Invalid/duplicate/source/
allocation/crash oracles pass. Final scoped release and ASan+UBSan retained_events,
retained_state, retained_environment and retained_state_crash checks all passed.
No third assurance pass. Earlier tests rerun only for actual representation/index
changes and corrected fixtures, not confidence/consensus.

Built isolated70575f2 accepted UI source with current retained changes; archive
compiled workflow points to primary programs/turn.lua. It excludes concurrent
unaccepted G4/G6 implementation. Profiling main was temporary archive-only and was
removed before final build. Atomic published build/release/arco-ui selected for
interactive launch; --once/--resume-once campaign keeps mutable build/release/arco,
explicit ARCO_EXECUTABLE wins. Startup prints opening session on terminal before
replay; no blank mysterious wait. This split was prompted by documented shared-build
collisions and does not change the model's own session/audit.

Actual final existing315MB context/my-arco reopened to prompt/EOF with exit0 in
12.0578s. Earlier probes24.7022s/53.6161s were not controlled experiments; no universal
speedup/latency guarantee. Remaining startup work documented in waste audit. Source
format/originals unchanged; normal identity/session events can append during probes.
No Linux check within this unit's bound; portable C++ stdlib behavior checked on
Mac release/ASan, affected Linux qualification remains explicit follow-up.

For interactive self-development, publish a checked executable to arco-ui atomically
before /restart, or select ARCO_EXECUTABLE explicitly for the generation being built.
A restart continues the selected executable; it does not magically select a file
compiled under a different name. Development campaign's --once launcher still uses
release/arco. Old installed UI pin preserved by build archive/report; source commit
and selected binary generation are separate recorded facts.
