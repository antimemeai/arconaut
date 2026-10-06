# U1 physical replay: resolution and qualification

2026-10-02. Scoped physical component accepted; selected-chain environment ownership
and whole U1 remain unimplemented/unaccepted. Maintenance codecs and complete RAM
proposal ownership were independently resolved earlier. No new dependency or wire
format/checksum was introduced.

## Resolved implementation/oracle findings

- O1: actual multi-source batch prefix rejects inferred source1 boundary {2,203};
  acknowledged source1 metadata/CRC can survive a valid combined-batch rewrite,
  yet its old commit is gone. The restage next-record batch guard refuses it.
- O2: deselected second committed source3 precedes uncommitted complete source5 in
  pending inspection, asserted exactly [3,5] before/after confirmation. Selection
  is exercised with allocation disabled, not merely assumed nonallocating.
- D1: failed payload reads retain an unknown-write Poisoned state for both native
  I/O and CRC failures. Direct red failed before the fix.
- D2: confirmation clears old staging metadata after its nonallocating swap.
  Direct red failed before the fix; pending inspection remains separate.
- Reentrant payload validation is a state-mutating read on failure and returns
  Busy during restage. Its injected-storage callback was red before the guard.
- Historical no-resume oracle omits reselection after restage, exposing any flag
  clearing that a repeated selection would conceal.

Kimi final rereview explicitly withdrew its mistaken residual claim that the
restage batch guard was redundant. actual uses staged_[selected-1], not the last
record in a newly combined batch; both inferred endpoints remain203. Only the
actual scanned batch-finality guard rejects it. No mutation campaign was run.
The capacity review's minor claim that old_limits equality was newly invented
is also incorrect: the existing wire/replay section already required equality
with the predecessor header. These reviewer errors did not trigger code changes.
Original reports are retained unchanged beside the corrective final rereview.

## Direct evidence and limits

The suite checks every partial second-append tail and every shortened acknowledged
first batch; exact empty/first/later commit selection vs middle/mismatched offsets;
CRC-valid same-geometry source rewrite (fresh scan succeeds, acknowledged owner
refuses); complete unknown-sync batch reconstruction; restage after semantic veto;
wrong/malformed headers, bounded extent/read interruptions reset by progress, concrete
I/O and impossible/zero counts, OOM, reentrant mutations, no selection allocation,
source checksum publication after live append, and native flock retention through
restage/selection/confirmation until destruction. No original bytes are truncated.

Raw local logs are in ignored context: u1-physical-replay-review-green.txt (final
debug changed journal_batch/native/retained_state), u1-physical-replay-reviewed-
release.txt, -asan.txt, -tidy.txt. Latest formatting checks pass. Full debug12/rigor
passed before the review's small read/state refinements (final-rigor log); subsequent
checks directly cover those changed fault classes. Do not rename the earlier
whole-suite run as a later run. Final Neuroses ordinary debug/release/ASan+UBSan
checks cover journal_batch/native/crash/retained_state/environment_head under
context/linux/run-zhqbapeo; initial run-pkwt31hd is retained separately.

CRC32C collisions/adversarial cryptographic rewriting are outside the existing
checksum fault domain. Fault injection and process SIGKILL do not qualify physical
power loss. No real U3 worker custody or whole selected-chain replay is claimed.
Kimi is a static independent reviewer; executable test evidence is our direct runs.

## Next unit refinement

The reviewed capacity section now separates explicit root/linked old-new policy
from independently configured framing/file/index/segment/history bounds. Root-only
policy has no persisted capacity anchor before the first continuation; supplied
configuration is trusted and checked, not reconstructed from absent bytes. Accepted
headers remain unchanged. Exact environment API, snapshot-entry accounting and
semantic staging/source routing must be pinned before their red cases and code.
