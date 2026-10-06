# Sustained history .14.3: measured design and first implementation slice

Retry .14.2 is closed after actual fresh-session quiet replacement and ordinary
activation; no live outage claim. Old exhausted audit is still untouched/unsettled.

## Observation before capacity choice

Read journal_writer/main/retained_state/context/session. Fixed512MiB/200000records;
32MiB maximum payload,96MiB batch. Physical frame/semantic replay scans all history.
Every batch copies its staging Snapshot; ContextStore retains full entries per
revision plus originals/history, copying maps/history when appending. Native
compaction shrinks presentation but not audit. Operation request bytes occur in
Decision/Invocation/Admission as well as provider original; this is not just SSE
chunk verbosity.

Bounded read-only census (154175 old frame headers;128byte semantic prefixes,
observed ends bounded by initial stat; no CRC/settlement claim): old536870704 bytes,
208bytes below configured cap. Context packets161407687bytes; operation decision/
invocation/admission233992512bytes; source frames121321669bytes; log envelopes
6167818bytes; commit frames3516800bytes. Thus simply batching SSE chunks does not
solve dominant byte growth. Current fresh journal observation130666506bytes:
context38349403, triple request operation56302831, source30775872; nonstationary
because managed compaction has changed presentations. No rate extrapolation claim.

Native release diagnostic replay of copied prefixes ending at observed commit
boundaries:8139310bytes/1291facts replay0.203175s, context reconstruction0.0174953s,
peak RSS20496384bytes;33356846bytes/5992facts replay1.99251s, context0.040115s,
RSS187301888bytes. Single measurements, no significance/asymptotic proof. Diagnostic
read-only fd denies writes/create; sync only copied files and containing diagnostic
directory for publishing recovered state, no custody, reconciliation or effects.
Initial probe default sysroot failed compile; explicit installed Xcode SDK fixed.
Private-permission issue corrected. Initial pre-confirm facts0 and rejected sync
probe are NOT context replay results. Captures context/resilience/frame-growth.jsonl,
replay-costs-published-copies.txt and diagnostic source. Full old replay not run;
no claim about full512MiB latency/peak RSS.

Acquired Codex rollout recorder.rs328-342/898-935 separates logical fork ordinal
from physical history base and writes explicit source/fork IDs for new rollout.
Transfer explicit successor lineage, not its database/dependencies. Our root factory
rejects JournalHeader predecessor: linked root-chain factory is not implemented.
Blindly wiring that header would pretend supported historical authority and still
replay all predecessors. Do not change cap or auto-truncate.

## First native slice: honest headroom visibility

Expose physical extent observation, committed end, configured byte/record limits,
indexed data-record counts, saturating remaining headroom, writer state and explicit
read error/null when extent cannot be observed. This is observation, not admission
permission, not a reservation, nor proof that the next turn will fit. Commit frames
occupy bytes but do not consume max_records. Include in Lua/context_stats and show
pre-provider warning at <=25% remaining in either dimension. Keep warnings out of
new audit-growth loops (ordinary display/status only; stats already audited by its
caller). No false promise of safe settlement under arbitrary tool/stream sizes.
Direct tests: live exact accounting, record-vs-byte threshold, extent read failure,
nonlive/damaged tail observation, unchanged native cap, status threshold and model
stats exposure. This slice remains part of active .14.3, not a capacity fix closure.

## Follow-on seam (before implementation)

Prefer explicit successor *session* with fresh identities over linked authoritative
root pretending inherited admissions. Preserve old path/journal ID, selected prefix
sequence/end, context revision, reason, unknown/unsettled status and original-entry
locators in new-session lineage record. Old originals remain inspectable without
copying all old history into every successor. Context transfer must be model-selected
and protocol-valid; do not automatically replay any old request/tools. Settlement
requires reserved byte/record allowance before next workflow admission, with bounded
provider/tool output handling and explicit failure if resource cannot be retained.
A warning alone is inadequate. Decide clean-handoff reservation and cross-session
inspection API after this measured checkpoint, before coding rollover machinery.

## First-slice result

Implemented JournalUsage/RetainedState observation plus CodingEngine stats.audit
and pre-provider visible warning. `prefix_end` names acknowledged prefix while
live, staged prefix during recovery; writer_state disambiguates. Restage retains
overlapping acknowledged/staged indexes, so count max of acknowledged vs staged+
pending, not their sum. Unindexed damaged tails consume observed bytes but are not
claimed indexed records. Read failure gives null byte headroom plus typed error,
never0 or permission. Existing offline `--audit-last` shows headroom even without
a model/tool invocation (still performs existing full replay and requires lease).
CLI smoke used copied8MiB prefix only, not old live/exhausted original. First
incorrect `--inspect` flag rejected without effects; actual flag `--audit-last`.

Direct memory/native coding cases pass. Small256KiB fixture/200KiB retained filler
causes one visible warning, provider runs once, stats reports approaching. Initial
coding assertion used wrong fixture cap and corrected to its actual64MiB/10000;
mechanical prefix rename caught by compiler and corrected. Mac release/debug/ASan
and Linux debug/release/ASan affected4/4 then final changed coding/batch2/2 each.
Private captures context/resilience/capacity-*; no new full-suite qualification.
Native release/arco built; checkpoint72d7116 pushed and actual quiet RRC
continued. context_stats.audit observed237635772extent/299235140byte headroom
in fresh journal7d62dd99e41e86aafbafb29cb3a3014b, proving gauge activation. .14.3 remains
active: this makes approach visible, DOES NOT solve arbitrary-turn exhaustion or
provide clean-handoff reservation/cross-session transfer. No capacity increase.

## Next dependency: atomic declared successor seed

Before admission reservation, implement a usable *explicit* destination, not an
automatic physical chain. Add --seed-session JSON for a genuinely empty new audit,
validated before journal creation. Bounded1MiB input/4096 selected entries. Manifest
version1 contains source session absolute path, environment/journal IDs, observed
prefix sequence/end, context revision, status (settled/unsettled/unknown), reason,
and selected {id,item} entries. This is model/operator DECLARED lineage, not source
verification, custody or proof of settlement. No old file is opened or mutated.
Validate identity/locator shapes, object entries and complete ordered call/output
linkage; reject duplicate calls/locators, incomplete groups and invalid roles.
Protected instruction selection is the author’s responsibility; native seeding
must not invent omitted source material or secretly read/replay predecessors.

Publish new originals/entries AND declared lineage+old-entry-to-new-entry order in
ONE context packet/batch. Failure cannot publish context without its lineage.
New originals get fresh IDs; old locators remain recorded, not overwritten.
Reject a nonempty destination at CLI and API boundaries; do not seed into recovery
or inherit old admissions. No provider/tool executes during seeding. Ordinary
interactive startup may subsequently run new chosen work; not automatic continuation.
History inspection exposes lineage with bounded current-store pages. Stable old
entry resolution without full parent replay, managed live handoff, admission reserve
and output bounds remain follow-on scope. Test invalid seeds before mutation,
exact selected context/mapping, replay reconstruction, destination refusal and
unsettled declaration, plus native CLI no-provider/nonempty refusal. Build/activate
native changes only after affected checks. No cap/configuration changes.
