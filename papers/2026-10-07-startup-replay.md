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

## Operator-directed continuation: remaining startup latency

Explicit operator scope:12s remains unacceptable; keep digging. Actual same-session
reopen12.1236s, three-second macOS sample2041/2127 samples in bit-serial CRC32C.
Study local LevelDB util/crc32c.cc: chunked table processing and incremental checksum
semantics; no dependency/code adoption. Implement compile-time generated slicing-by-8
Castagnoli tables (8KiB read-only, no allocation), preserving complemented prior/final
CRC and all checks. Independent bit-serial specification over varied lengths, offsets,
seeds and incremental splits plus existing RFC3720 vectors/corruption oracles.
FramedJournal payload handles currently scan every record. Its three record vectors
are maintained in sequence order through scan/append/suffix rejection/restage/swap;
binary-search sequence then compare the entire physical handle, preserving forged/
stale rejection without a second index. Existing journal batch/recovery/crash tests.
Bound25minutes remediation/recheck, no third assurance; isolated accepted UI archive
build again, no campaign executable collision. Measure real reopen after changes;
if another dominant cost remains, state it rather than claim the whole audit solved.

Continuation result: portable slicing-by-8 + binary physical-handle lookup reduced
same-session startup to4.4377s. A subsequent sample still shows checksum work dominant
in scan/replay. Local qualified Clang target defines __ARM_FEATURE_CRC32; compiler's
arm_acle.h supplies __crc32cd/__crc32cb Castagnoli instructions. Use those only for
compiler-guaranteed CRC targets with little-endian loads via memcpy (unaligned safe).
Other targets keep portable slicing-by-8. Hardware build omits unused8KiB tables.
Actual same315MB session reopen1.3661s to prompt/EOF exit0. Timings are ordinary local
reopens with cache/load uncontrolled; no cold-start or universal timing guarantee.
No cache sidecar, weakened checksum, truncated history or restored/copied audit.

Direct RFC/bit-serial/split/unusual seed/unaligned length oracles caught an initial
constexpr table initialization ordering defect before activation: later slices must
be built only after the entire first table exists. Corrected; fixed-frame golden bytes
and corruption checks pass. Forged same-sequence offset and absent commit-sequence
handles remain stale. Final affected release/ASan+UBSan journal_codec, journal_batch,
journal_crash, retained_state, retained_state_crash pass. Additionally compiled the
final codec with __ARM_FEATURE_CRC32 undefined: portable fallback passes exact same
specification oracle. No Linux qualification claim. Both production paths checked,
no third assurance layer. Interactive arco-ui atomically published; campaign release
unchanged. Remaining work is ordinary journal I/O/decode/context reconstruction and
forward metadata copies; no second authority or architectural bypass introduced.
