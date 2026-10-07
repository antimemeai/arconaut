# Startup loading renewed unit: valid request-ready measurements

Candidate branch `candidate/startup-loading-r2-2026-10-07`, preserved base
`ea50c4b` carrying partial `8e35f55`. Renewed deadline1791386846; no reset.
Root owns serial integration; this candidate is not master or the primary runtime.

## Useful implementation

Context reconstruction projects unused root `candidate` fields while validating
EVERY input byte, syntax/Unicode/duplicate keys and original JSON limits. Full
immutable audited packet custody and publication-chain checks remain unchanged.
Unescaped strings use an in-place bounded printable-ASCII scan; escaped strings
retain the prior decoder. No persisted cache, dependency, service, runtime or
inode/mtime trust shortcut. Original lookup/repair remains eager and unchanged.

The old10 error20 runs remain invalid. Error20 is audit_unavailable: recovery
confirmation was insufficient without custody reconciliation. Typed native
validation does not establish the old arbitrary actor/workflow definitions. The
repaired probe uses CodingEngine with a deterministic local provider: actual
workflow/generation/context, serialization, native decision/invocation/admission,
open, captured response and terminal success. It rejects unresolved custody and
checks exact current-attempt linkage/bytes, not any historical admission. No
external provider service/effect or guessed no-effect observation is involved.
`admitted_ms` ends immediately at provider callback entry (before probe validation);
post-callback known-success settlement is necessary for exit0 but not timed as
admission. This measures true LOCAL audited request readiness, not network latency.

## Matched successful releaseOFF measurements

LLVM23.1.2, arm64 macOS SDK, C++20, Release -O3 -DNDEBUG,
BLACKBIRD_DEBUG=OFF / BLACKBIRD_PROFILING=OFF; own checkout builds <=j2.
Baseline overrides only old `json.cpp` and `context.cpp` objects from ea50c4b;
remaining libraries AND repaired CodingEngine probe are shared. It measures the
renewed slice versus preserved partial lazy history, not the old eager-history
slice or the old failed probe. No primary executable was rebuilt/replaced.

Five interleaved warm same-host runs per variant, all exit0, identical private
synthetic initial prefix335,783,054 bytes,1,282 committed facts. One private copy
of the existing synthetic fixture is reused, resetting only successful settled
probe tails between runs. No writable operator session open or repeated copy.
Rejected candidate string contents alone335,544,320 bytes; 640 historical rejected
edits plus one accepted live append. Actual live prompt and request input37 bytes.
The final successful private tail is335,837,968 bytes /1,311 facts. History size
must not be confused with live context or audit space reclamation.

All milliseconds, medians with min–max:

| Boundary/stage | Baseline | Candidate |
|---|---:|---:|
| Native open/replay + confirmation/custody | 153.316 (148.542–158.528) | 160.891 (155.018–180.061) |
| Context restoration (stage only) | 1158.542 (1129.042–1175.435) | 19.705 (19.336–19.931) |
| Usable prompt items (cumulative) | 1310.620 (1284.820–1328.180) | 180.675 (174.672–199.397) |
| Engine initialization + request preparation/admission (stage) | 87.820 (81.120–136.390) | 85.965 (80.040–113.483) |
| Audited local request admission (cumulative) | 1394.550 (1372.640–1464.570) | 265.961 (263.631–294.158) |
| Max RSS bytes (whole settled process) | 356925440 (355794944–356990976) | 354304000 (354222080–355549184) |

Median cumulative speedup5.24x, context-stage58.80x. **UNDER100ms NOT achieved**:
every candidate admission sample exceeds263ms, and full journal replay alone
exceeds155ms. Dominant remaining loading work is authoritative full335MB journal
scan/replay/decoded retained payload ownership (~161ms median), then actual engine
initialization and durable request admission (~86ms). Loading/checksum/copy reduction
or a precisely integrity-bound replay accelerator needs a FUTURE bounded unit;
reservation/admission flush mechanism belongs to the other lane. No promise that a
banner, unvalidated manifest or merely lazy JSON solves that remaining work.

## Deliberate profiling observation (NOT normal steady state)

Separate own optimized DEBUG=ON/PROFILING=ON probe; explicit per-process
BLACKBIRD_LOCAL_TIMING, shutdown flush32 buffered records /0 dropped /write_ok.
Native spans observed replay188.854ms wall /180.102ms thread CPU, restoration
22.405/22.405ms, request-and-SETTLEMENT115.584/3.705ms. That last native span includes
provider validation/capture/settlement and does NOT share admitted_ms's earlier
boundary. ReleaseOFF numbers above are the performance comparison. External
`/usr/bin/time -l` CPU/RSS/page-fault/block-I/O logs and short-process `/usr/bin/sample`
stack files retained. Stack coverage is sampled/incomplete; blocked synchronize
appears in request work. Native observed_bytes is NOT all copied bytes/allocations;
spans do not account for all short children, and have finite8192-record capacity.
Instrumentation does not affect audit and is excluded from releaseOFF runtime.

## Two layers, checks and review

Layer1 direct JSON/context/workflow_repair checks pass, covering projected/full
acceptance and errors, root-only behavior, duplicate escaped keys, malformed UTF8,
controls/escapes/scalars, byte/node/depth limits and all positions0–64 around fixed16
blocks, native restoration, originals/repair, publications and rejected history.
One authenticated Codex/ChatGPT static review <=90s of NEW invariant found three P2
oracle issues: callback timestamp after probe scan, historical admission mistaken
for current, prefix-only history comparison. Fixed all three; layer2 scoped recheck
of JSON/context/workflow_repair passed3/3. Full history pages compared with revision
pinning. No third assurance, new survey, mutant run or unrelated suite. Old review
preserved. Journal CRC/suffix/custody code unchanged, no new cache fallback surface;
this unit did not rerun old journal crash/suffix suites.

## Limits and handoff

Synthetic history-heavy fixture only; LARGE LIVE CONTEXT/original payload workloads
are not established. Eager originals, live context and native journal replay remain.
Escaped discarded strings still allocate; retained strings still materialize.
No external real provider/network readiness measurement, no operator-megasession
claim, no cold-disk comparison or copied-byte/allocation census. Candidate inactive
pending Root integration. Scoped binaries built under build/release and
build/startup-profile; primary harness untouched. Build/review/raw measurement logs
persist at `/Users/patrickbeam/projects/blackbird/context/startup-loading-r2-2026-10-07/evidence`.

Binary/source identities (full per-sample metrics in valid-measurements.json):

- Fixture SHA256 `084f8df8867a96fe322ccc8d0c9652b825710177543b02c544d883c591f53cb2`
- baseline release probe SHA256 `62284c503b2e3aaa877c71dfd3737b6350179b1f8a18b60fe365dcda96f4876e`
- candidate release probe SHA256 `0e480146219b9c84c89eac270f7a389035ddf07a2a6107530b7bdbd1a29bc35b`
- Final probe release binary identical to measured candidate after debug-only instrumentation additions.
