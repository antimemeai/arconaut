# Autodev resource collector: one adversarial review

2026-10-07. Scope: `scripts/profile-autodev` and
`docs/AUTODEV_PROFILING.md`; the existing `scripts/profile` wrapper was read only
to understand stack-sample lifecycle. No production changes, new test framework,
provider requests or additional assurance round. This is the requested single
review; implementation owner owns remediation and one direct recheck.

## Findings

### F1 — Reject root/child identity changes at the actual traversal observation

**Medium; observation correctness.** At review-time lines 276–298, the collector
checks the manager birth time. It then calls `api.observe(pid)` again at line 307
and labels that item manager based only on PID. If the manager exits and its PID
is reused between those two reads, `observe()` can legitimately return the new
process: its internal before/after identity matches. That unrelated process and
its children can enter one capture tick before the next outer root check catches
reuse. This contradicts the documented prevention of following a reused manager.

Children have the analogous enumeration-to-observation gap. Traversal stores
only integer PIDs, so an enumerated child PID reused for a process with another
parent is accepted and its descendants are scanned. Retaining the item's birth
time labels the wrong observation honestly; it does not establish tree membership.

**Small correction:** carry expected root identity into traversal and compare
the returned root observation before appending/expanding it. Carry the expected
parent PID with discovered child entries, reject observations with another
parent, and fence expansion against parent birth changing while children are
enumerated. Record a sampling error/terminal reuse status as appropriate. These
checks reduce attribution races; do not claim an atomic whole-tree snapshot.

### F2 — Terminal status can be skipped by capture initialization/final metrics

**Low; retained status accuracy.** `capture.json` becomes `recording` before
`resources.jsonl` is opened outside the guarded `try` at line 265. An open failure
there exits through top-level stderr handling without changing metadata to
`failed`. In `finally`, stream flush/fsync and `api.observe(os.getpid())` likewise
precede terminal metadata publication. If either raises, readable metadata can
remain `recording` despite collector exit.

**Small correction:** put post-metadata initialization within the lifecycle
guard; attempt terminal status publication even if an optional final self
observation fails, retaining its error. Persisting failure status is necessarily
best effort when the underlying output storage cannot accept writes. State that
limitation explicitly; no secondary watchdog/certification mechanism is needed.

## Reviewed strengths and scope bounds

- ABI declarations match the documented installed-SDK layouts; the author
  supplied actual compiled offsets/sizes and independent `getrusage` CPU-unit
  evidence. CPU conversion uses integer Mach tick × numerator / denominator,
  with no floating precision loss. This review did not duplicate those checks.
- Child counters remain separate from individual process counters. Docs
  explicitly prohibit double counting, distinguish resident/footprint gauges,
  distinguish disk bytes from network bytes and label descriptor table capacity
  correctly. Overlapping session/audit storage totals are explicitly identified.
- Retained birth identities, new-binary references and sampled disappearance
  events are appropriate. Fingerprints are identified as current path bytes,
  not falsely asserted as exact loaded images. Short-lived and detached work
  are expressly outside complete lifetime accounting.
- Default capture does not launch a continuous heavyweight profiler. Observation
  cadence, bounded directory scans, per-tick cost and cumulative collector
  resources make its own overhead inspectable. A first read/hash of a large
  executable remains real measured collector work, not free overhead.
- Optional stack windows have explicit child handles and a separate process
  group; collector interruption targets that group, not the observed harness.
  The author's live interruption check supplies direct evidence for that path.
- Output directories refuse overwrite and are private. JSONL is flushed per
  tick and fsynced periodically; final metadata uses a temporary file plus
  atomic replacement. This review does not infer power-failure guarantees or
  complete lifetime accounting from those mechanisms.

No broader native instrumentation, Linux adapter, campaign policy or speculative
launch wrapper was reviewed. Findings were reported promptly to the parent for
bounded correction; no review of the review is requested.
