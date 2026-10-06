# Managed compaction B1: implementation and serious campaign

2026-10-06. **Scoped B1 implementation and campaign complete.** This report closes
arconaut-uaa.9 only; broader milestone/acceptance and B2 automatic policy remain open.
Latest rebuilt release/arco was actually inhabited and verified after quiet native
RRC, not merely built. Product source remained unchanged throughout final Mac/Linux
qualification. Historical context/autodev/session was not modified.

## Features and use

Owned C++ ContextStore supports explicit **summarize, select, archive and restore**.
Every proposal retains reason, source, selected identities, input revision, candidate,
outcome and resulting revision in the native audit. Originals are immutable;
synthetic summaries get new identities and ancestry. Rejected proposals preserve the
presentation, but remain recoverable in history. Supplied stale bases are rejected,
not silently rebased. The model may explicitly omit base to bind its invocation
snapshot; supplied bases are always strict.

Managed workflow changes stage until successful **end-of-workflow** settlement.
Current open tool interval and appended suffix survive; publication never strands
its own tool result. Failure/interruption cancels pending work, and reopen does not
activate orphaned proposals. One pending operation is allowed. `staged:true` with
`accepted:false` means pending, not permanent rejection. Idle operator /compact
commands publish at the completed command boundary. Restore grows presentation and
repairs whole groups in original capture order without re-executing tools/effects.

All live user/system/developer messages are mandatory. Summaries normally use
assistant role; an explicitly chosen developer summary becomes a pinned instruction.
Generic model CLM/Lua context editing remains available, not replaced by this menu.
Structural validity does **not** establish semantic summary accuracy.

Surfaces:
- Model: `context_manage({proposal:...})`, `context_inspect({query:...})`.
- Lua: `arco.manage(proposal)`, `arco.inspect(query)`; generic context/edit APIs remain.
- Operator: `/context`, `/compact JSON`, `/inspect JSON`, `/restore ENTRY`.
  For protocol repair prefer a managed batch restore rather than separate single IDs.

Example idle operator archive, using actual `/context` base and complete group IDs:

```text
/compact {"base":"CURRENT_BASE","mode":"archive","ids":["CALL_ID","RESULT_ID"],"reason":"offload completed experiment","source":"explicit selected complete tool group"}
/inspect {"kind":"originals","entry":"CALL_ID","offset":0,"limit":4096}
/compact {"base":"NEW_CURRENT_BASE","mode":"restore","ids":["CALL_ID","RESULT_ID"],"reason":"exact repair","source":"immutable originals inspected by ID"}
```

`select` names the retained entries; `archive` names removed entries; `summarize`
names replaced entries and supplies `summary:{role:"assistant",content:"..."}`.
All mandatory entries and protocol groups must remain valid.

Inspection kinds `originals`, `history`, `index` return bounded **hex-json-utf8**
bytes, total_bytes, clamped offset, next, revision and context_revision. Default4096,
limit1..65536; arbitrary byte splits including UTF8 are recoverable by concatenation.
Use `entry`, not `id`, for a targeted original. Bind later pages to the **returned
revision**; stale revisions yield conflict and require restarting pagination.
History uses latest history packet revision, including rejected proposals; originals
and index conservatively use presentation head. context_revision reports view head.
Omitting revision is an explicitly unguarded read, not a historical snapshot.

## Source grounding that changed implementation and oracles

Original CLM paper2609.37725v1 §§3/4.1/4.2 and original harness/edit/environment/
evolution paths were read firsthand. Needle/offload tasks motivated independent
predeclared semantic facts and exact recovery rather than shrink-only self-rating.
Its arbitrary transition agency motivated preserving generic CLM, while its
shrink/fit gate was rejected for exact repair that must grow. Its pre-tool-output
synchronization challenged our opposite ordering and led to open-interval and
actual request-boundary oracles.

Original gptme engine/resume and its actual concurrent-unlock stale-result test
challenged stale publication and master-log recovery; letta's actual transcript
parity test provided a contrasting lossy-formatting oracle, not a template.
gptme snapshot checks motivated the productive inspection revision task after live
compaction. Review exposed the mistaken adaptation of view head to history; we
reproduced and fixed it using payload-specific snapshots. Its master-log byte-range
mechanism also informed the fix for a real live aggregate-history ceiling failure.
Detailed paths, rejected alternatives and consequences are in
[the source-consequences paper](2026-10-06-compaction-source-consequences.md).
References were not executed/adopted as dependencies or edited.

## Direct fault campaign

- **160 varied deterministic transitions against an independent simple model**, with
  exact view, capture-order identities, original values and ancestry after each step;
  **16 physical native reopens**. Append/select/archive/summarize/sparse restore,
  stale proposals, stage/cancel and persistent generic reorder are covered.
- This randomized model uses role/reasoning items, **not randomly generated tool
  protocols**. Separate targeted cases attack parallel call/result groups, partial
  group rejection, duplicate calls, malformed order and exact repair after reopen.
- Actual CodingEngine assembled provider-request input checked before and after
  end settlement; multiple post-stage tool exchanges preserve their full suffix.
  Malformed suffix rejects without changing appended view. Malformed request data
  is rejected before provider dispatch, then repaired through actual Lua-only
  archive; no effect replay or permanent-brick assumption.
- Malformed/stale proposals, overlap, generic-edit settlement conflict, cancellation,
  orphan stages, developer pinning, immutable summary ancestry and rejected-candidate
  recovery are directly exercised. Paged expected bytes come from the independent
  model/source, not from recombining inspection output as its own expectation.
- A large-history oracle independently concatenates native original context packet
  payloads. History exceeds16MiB; exact seven-byte pages at start, midpoint,
 16MiB-3, tail and beyondEOF match expected bytes/total/next. Small-model tests cover
  UTF8 splits, bounds and index metadata.

## Actual live semantic continuity, productive code and repair

This live model authored a managed summary of **182 closed nonmandatory entries**,
not a scripted fixture pretending to be model work. Facts/mission constraints were
predeclared in live-oracle.json before summarization. After activation plus quiet RRC,
all six retained literals were present; the deliberately omitted
`recovery-code=AMBER-913` was absent. Bounded inspection recovered it from the exact
567-byte immutable source, independently matched against predeclaration.

The summary retained the next task: inspection snapshot guards. We continued that
actual C++ task after compaction/RRC through written subplan, direct red,
source-grounded implementation, green, independent concrete review, consequential
red/fix/follow-up, rebuilt release and further native RRC. Thus continuity includes
productive implementation, not just repeating facts from a summary.

A **distinct live archive** removed a completed failed read_file call/result pair.
Absence was verified; both bounded originals independently matched prearchive
expected entries. Managed batch restore then activated at successful workflow end.
After the latest RRC both exact entries returned adjacently at positions14/15;
all **seven pinned user/instruction originals** matched immutable captures exactly.
No tool was re-executed as repair.

Actual replacement history inspection now returns seven bytes from
**104245469 serialized history bytes**. A fresh guarded page equals the unguarded
page; deliberately stale-base manage rejection changes history but leaves the view
exactly unchanged; the old history revision yields `conflict`. Raw live-replacement-
guard.json and live-restore-verification.json record these observations.

Eight actual plain operator processes separately exercised /lua,/compact,/inspect:
predeclared UTF8 source/mission, summary, malformed/stale rejection, rejection-only
history conflict, bounded exact recovery/restore chronology, distinct selection,
archive and reopen repair. **Zero provider requests** in this operator fixture;
that is separate from the actual live model campaign above.

## Measurements

Latest qualified release run, final-scale-raw.log, isolated64 completed tool turns
with8192-byte results:

| Measurement | Result |
|---|---:|
| Serialized JSON input, before → after | 549166 → 225 bytes |
| Retained native audit, before → after | 18868418 → 18898258 bytes |
| Audit growth for this publication | 29840 bytes |
| Publication wall latency | 13.2805 ms |
| Physical native reopen/replay | 293.827 ms |
| Request assembly + audit + scripted mock | 111.762 ms |
| Complete assembled request incl tool schema | 6778 bytes |

Earlier measured replay573.338ms illustrates run variability; these are observations,
not latency guarantees. Request schema grew during implementation. No network/
inference latency, provider-cost, tokenizer or cache-savings claim follows.

Live captured serialized input684527bytes at fixture plant →61448bytes in captured
18-entry postactivation snapshot. View JSON69167bytes includes identity/ancestry and
is a different measurement. Observation points contain intervening activity, not a
controlled latency benchmark. Pre-publication observed native session files192473791
bytes already included stage/subsequent logs; not clean summary-only audit growth.

The retained audit grows despite shrinking presentation. Historical full-view
packets and resident history copies can grow quadratically. Inspection now retains
only a bounded page plus one serialized record (up to ordinary16MiB document limit),
but costs **O(history bytes) per page**; exhaustive tiny-page scans have expensive
quadratic aggregate work. No indexing/cache optimization is claimed.

## Independent reviews and failures resolved

Completed wrapper/child0 reports are preserved unchanged in papers: core tkhnw446,
oracle0yv51w07, inspection yarf2yrz, consequential followup ipu2afde and window
qd_apwqg. Findings were independently investigated and integrated. Interrupted
oracle mz4dltlh has no result.json and remains explicitly incomplete, not counted
as completed review. Its substantive leads were tested through a later completed
review and direct implementation/oracles.

Consequential discovered faults:
1. Core ordering/base-gap, pending cleanup and batch chronology issues fixed after
   direct cases/concrete core review. Append-only workflow continuation intentionally
   remains supported; blanket generic-append rejection would break normal Lua turns.
2. Initial inspection guard incorrectly equated history and view revision. Rejected
   operations grow history without advancing view head. Direct rejection-only red,
   per-kind cursor fix, completed independent followup confirmed resolution.
3. Actual live seven-byte history query returned capacity because the entire history
   was first serialized under16MiB JSON ceiling. Direct scale red reproduced; virtual
   per-record serialization fixed it without increasing global JSON safety limits.
   Independent review found no correctness fault, and replacement now reads104MiB.

Other campaign failures/ergonomics were kept honest: prior compound default120s
build/ASan deadline left an incomplete run, not a test failure; individually bounded
checks completed. Timeout recovery was separately implemented by operator/Codex:
retained partial output/unknown effect plus a distinct next command, no automatic
replay; explicit cancellation still stops. One targeted original query mistyped id
instead of entry and returned bounded all-originals data before correction.
One test compile typo was corrected before genuine runtime red. Ruby2.6 filter_map
failed after operator source planting; saved output was inspected and the harness
resumed without planting twice. Initial latest live restore probe compared distinct
lossless Lua JSON-number tables by identity, raised external_unknown and saved no
result; read-only diagnostic removed that assertion and independently verified
exact restoration/pinning. Lossless numeric tables require encoded/value comparison.

## Final qualification and limits

On unchanged product source snapshot (SHA256 before/after equal):
- Mac qualified LLVM23 profile: **debug11/11, release11/11, ASan+UBSan11/11**.
- Neuroses Clang18.1.3/libstdc++13: **debug11/11, release11/11, ASan+UBSan11/11**.
- Seven relevant TUs clang-tidy clean: context/coding/tools/main and
  context/coding/compaction_campaign tests.
- Selected cases: context,coding,tools,compaction_campaign,openai,json,terminal,
  terminal_pty,rrc,session_store,session_recovery. Timeout/cancellation and native
  replacement behavior are included. This is relevant scoped qualification, not
  a claim that every repository acceptance test was run.

Commands: cmake --build build/{debug,release,asan}; ctest --test-dir build/PROFILE
-R '^(context|coding|tools|compaction_campaign|openai|json|terminal|terminal_pty|rrc|session_store|session_recovery)$'
--output-on-failure; matching scripts/check-linux --test-regex selection;
clang-tidy -p build/debug on the seven TUs. Raw captures context/compaction-campaign;
Linux full checks context/linux/run-qlw6n_a_/checks.log. Native RRC verification was
performed after build/tests and actually used the replacement machinery.

Conservative all-user pinning limits reduction. Opaque/signed provider-prefix
compatibility is not generally certified by structural group checks. A single live
semantic experiment is not a broad summary-fidelity benchmark. Semantic losses
outside predeclared facts may exist; recoverable originals are necessary but do not
automatically surface omissions. B2 automatic thresholds/guessed tokens are excluded.
No production Python/new dependency/service/mutant/Git publication/delegated coding,
neighbor/quarantine/historical session edits. A1–A6 behavior and credentials preserved.

Recommended next action: use these explicit surfaces for sustained work and begin
the operator-sequenced, source-grounded useful-work evolution pilot. Keep automatic
policy (B2), scalable history indexing and broader acceptance as distinct work;
do not interpret this bead's completion as closing that broader milestone.
