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
