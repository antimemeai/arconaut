# Startup loading: bounded first slice

Bound: original deadline 1791382317; total 25 minutes, builds -j2. Layer one:
remove eager decoded context-history ownership and repeated history deep copies;
retain validated immutable audited payload handles and load history JSON only for
inspection. Direct checks: context replay/lookup/repair and pagination equivalence,
append-tail replay, malformed context rejection; matched optimized native startup
and local audited-request admission measurement on one private large fixture.
Layer two: one independent Codex review, at most 90 seconds; fix relevant findings
and rerun affected direct checks once. No third layer.

Grounding: papers/2026-10-07-startup-replay.md and waste-audit.md identify full-prefix
copying and unnecessary representation retention. Studied local quarantined
LevelDB db/version_set.cc Recover and table/two_level_iterator.cc: Recover validates
manifest edits and required metadata before publishing; TwoLevelIterator retains
an index and opens only the selected data block, propagating index/data errors.
No reference code executed or adopted.

This first slice keeps the authoritative native journal replay, CRC checks,
complete-batch recovery confirmation and unknown-effect fences unchanged. Context
replay still validates every packet and publication chain before requests. Only
historical decoded JSON is demand-loaded; live entries and originals remain eager.
The history index owns immutable payload shares, not pointers into a reallocating
fact vector. Newly prepared history is allocated before durable publication, and
swapped only afterward. Historical inspection parses/canonicalizes one document
at a time and preserves prior pagination semantics, including rejected revisions.
No persisted accelerator, filesystem trust heuristic, new identity or event codec.
Thus cache corruption/truncation is not a new trust surface: authoritative recovery
continues to reject suffixes. This is not a claim of sub-100ms full startup; journal
payload replay and context validation remain dominant potential work.

## Renewed unit (1791385355–1791386846)
Authorized continuation, 25 minutes total including builds/review. This checkout
is branch candidate/startup-loading-r2-2026-10-07 (slot-1); root is unchanged.
Layer 1: repair failed admission oracle from actual error detail and retained
validator; preserve one private large fixture; build separate release-OFF baseline
and candidate <=j2. Measure successful replay, restoration, live prompt and local
audited admission separately, five warm samples. Optimize dominant loading using
validated projection, not untrusted persisted metadata. Direct checks cover JSON
validation/projection equivalence, original repair/history pagination and publication
chain/suffix checks affected by change. Existing LevelDB grounding above applies.
Layer 2: one authenticated review of new invariant <=90 seconds; repair findings
and one scoped recheck. No third layer or reset. Unknown suffixes remain unknown;
no operator session writes. Finish report/commit/push/marker even if <100ms missed.

### Implemented renewed slice and result
Strict root-field projection validates the complete JSON document with the same
syntax, duplicate-key, Unicode scalar, byte/node/depth rules; it omits only the
unused root `candidate` representation during context reconstruction. Nested
`candidate` application data remains ordinary data. Full immutable history bytes
remain authoritative and are loaded for inspection. Unescaped strings validate
in-place; bounded fixed16 printable-ASCII reductions avoid decoded-byte push loops.
Escaped discarded strings still allocate a temporary decoded string. No persisted
cache/index, filesystem-equivalence heuristic, weakened suffix rule or new trust
surface. Live entries/originals and full journal replay remain eager.

Actual error20 was `audit_unavailable`: confirm_recovery alone leaves the writer
recovered, not live; custody reconciliation was missing. Native dependencies check
source references and invocation planning, not those hardcoded definition IDs.
Replaced fabricated admission with actual CodingEngine request preparation/admission
and a deterministic local provider. Exact new attempt/request bytes, decision /
invocation / definition / context linkage, open and terminal success are checked.
Empty custody rejects unresolved attempts; no old effect is dispatched or declared
successful. Timestamp is first instruction of provider callback, before probe work.

Layer 1 JSON/context/workflow_repair checks passed. One authenticated Codex static
review of NEW invariant completed within the90s allowance: three P2 oracle findings
(timestamp pollution, historical admission alias, prefix-only history check).
All fixed; one scoped layer2 JSON/context/workflow_repair recheck passed. Entire
history pagination now compared, including large rejected candidate. No third
review/certification/full-suite layer. Old lazy-history review was not repeated.

Five interleaved warm releaseOFF baseline/candidate samples all exit0 on the same
private synthetic335,783,054-byte prefix (1,282 facts; live prompt37 bytes; rejected
candidate strings alone335,544,320 bytes). Baseline is preserved ea50c4b json/context
objects, NOT the invalid old10 samples. Remaining objects and repaired probe matched.
Median full audited request admission:1394.550ms ->265.961ms (5.24x); context stage:
1158.542ms ->19.705ms. Full journal replay still160.891ms; subsequent actual engine
preparation/durable admission median85.965ms. UNDER100ms NOT achieved. Large LIVE
context and large originals workloads not measured. See renewed report for ranges,
build/source hashes, exact boundaries, profiling limitations and integration handoff.
