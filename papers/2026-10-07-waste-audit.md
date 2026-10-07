# Arconaut waste audit: bytes, copies, work and lifetime

Operator2026-10-07 asks for a John Carmack style waste audit: every byte matters.
This is an engineering audit of our actual source and running sessions, not a claim
of endorsement by Carmack. No Electron/browser runtime is present; native software
can still squander memory, CPU and I/O. Findings below identify how.

Audit allowance45min, separate from the already-running G6 implementation. Direct
measurements/source tracing; remediation bounded25min/two layers within the owning
unit. No allocator framework, mandatory performance certification, new library,
mutants, broad rewrite or deletion of operator history. Startup repair has its own
scope/results in 2026-10-07-startup-replay.md. Raw profiles/local sessions remain
ignored. Published census contains sizes/counts only, no request contents or auth.

## What we measured

Two existing closed audit files streamed one frame at a time using the on-disk
112byte header/32byte frame/schema1 formats read from actual source. This counts
representation; it is not a new checksum or recovery authority. Native reopening
separately exercises validation. Census JSON in waste-audit-2026-10-07/ retains
counts. No histories were truncated, recompressed or overwritten.

Operator session at census:315,711,524bytes,179 admissions. Principal payloads:

| Representation | Bytes | Meaning |
| --- | ---: | --- |
| Context application records |96,072,214|186 full revision packets, including cumulative entries.|
| Original source frames |69,804,292|13,767 captured source frames; necessary originals, representation/cadence separately reviewable.|
| Decision records |46,604,734|179 records; continuation embeds input plus attribution.|
| Invocation records |46,570,112|179 records carrying encoded input.|
| Admission records |46,570,112|179 records carrying encoded input again.|
| Log metadata records |2,533,051|13,767 source-log records.|
| Observation records |4,265,161|179 outcomes.|
| Identity reservations |233,760|14,610 records, plus their commit/frame overhead and synchronization work.|
| Commit payloads |703,008|29,292 complete batch markers.|

All179 invocation/admission input blobs have matching length and hash: exactly
46,559,372bytes of repeated blob payload in that pair alone. All179 decisions embed
the same logical input when canonical JSON is compared. Decision metadata is
legitimate, duplicating the entire input is not its essential purpose. Disk-size
categories include schema/identities; blob duplication count excludes that overhead.
These are actual observations, not inferred compression ratios.

Closed G4 session91,898,491bytes,154 admissions: context25,067,914, original sources
23,895,928, decision12,539,174, invocation12,509,270, admission12,509,270bytes.
7,974 identity reservations and7,285 source/log frames. This is not only an unusually
old operator conversation. Small files or few calls are not the governing invariant.

Source tree observed approximately500KiB src/104KiB include; release directory25MiB
includes targets, archives and tests, not installed distribution size. Native
executables approximately1MiB. Ignored campaign/build/source archives are developer
artifacts. Do not conflate those with per-install runtime requirements or erase
valuable audit history merely to improve du output.

## Findings and course

1. **Critical: full payload prefix copied per replay batch.**
   src/retained_state.cpp replay copied Snapshot at every batch. Existing immutable
   payloads were reallocated repeatedly. Initial stack sample showed vector/fact
   deep copies; direct64-record oracle allocated3,532,536bytes for57,712 journal
   bytes. Replace with append-only prepared replay and batch sizes/counter rollback.
   This keeps valid-prefix semantics, not a second history. Repair delivered as the
   startup unit; correctness/crash/allocation oracles remain direct.

2. **Critical: forward fork deep-copies retained payloads.**
   append_impl's candidate copy copies every event's vectors. Merely issuing an ID
   for startup or stream logging copies the same huge byte history again. Follow-up
   stack sampling caught CodingEngine construction -> reserve_identity -> append
   -> Snapshot/vector payload copies. Make retained payload values read-only and
   share storage. Candidate owns changed/new values and metadata; replacing a value
   leaves prior shares unchanged. No on-disk change. Startup unit repairs sharing;
   metadata container copies below remain an explicit separate issue.

3. **Critical: replay identity/dependency scans are quadratic.**
   existing() scans prior facts for each new reservation/application ID; every log
   dependency scans prior source records. Many small chunks make counts large.
   With payload sharing, measured phased reopen still spent23.4624s in root replay/
   recovery,0.7441s in context reconstruction, about0.011s settings/engine identities
   on that run. Earlier whole reopen53.616s demonstrates environmental variance;
   do not claim a controlled speedup from unlike runs. Startup repair builds a small
   derived key/source index once for replay; index disappears afterward, owns no
   payloads, is not authoritative history, and rolls back entries with rejected batches.

4. **High: duplicated invocation input on disk.**
   CodingEngine::invoke serializes input into decision continuation, invocation and
   admission. Census confirms46.56MB exact redundant bytes in one pair. New source/
   typed immutable blob reference can retain one original input and bind all three
   records to it. Keep attribution/admission independent. Requires explicit versioned
   codec/recovery/migration design and exact equality/identity tests; historical logs
   remain readable and unchanged. Do not silently remove required semantic linkage.

5. **High: stream capture becomes identity/sync/mutation work per chunk.**
   response_observer -> AuditLog::original -> issue -> source+metadata append. Each
   chunk can cause counter reservation and another batch. 13,767 captures and14,610
   reservations for179 admissions. This is a major replay-count/I/O amplifier, not
   large binary bloat. Investigate bounded capture blocks and durable issuer range
   reservation. Every observed byte must remain retained, ordered and recoverable;
   crash/partial transport/pause boundaries cannot hide buffered data or replay effects.
   Measure fsync count/latency and bytes per flush before choosing policy. No loss of
   comprehensive audit. A tiny flush buffer is a mechanism, not a magic speed claim.

6. **High: full context snapshots and duplicate parsed representations.**
   ContextStore append copies entries_, originals_, history_, captured_; packet
   stores cumulative entries plus new originals. Constructor parses every context
   packet and retains history_, captured_, originals_ and effective entries. view()
   and items() build fresh JSON copies; stats() serializes them again. Use immutable
   originals by identity and revision operations/selected views; index historical
   packets instead of retaining full parsed trees merely for inspection. Managed
   compaction changes working context; it does not shrink original audit. Any redesign
   must preserve arbitrary model repair, exact request inclusion, revisions and CAS.
   Measure heap amplification on the actual session;96MB serialized context is not a
   measured96MB heap. No RSS precision invented from file size.

7. **High: forward metadata still copied, even after shared payload repair.**
   Snapshot candidate copies fact/source containers. The new byte-sharing primitive
   removes payload duplication, not all O(history) work or refcount operations. A
   prepared suffix over an immutable prefix, or contiguous append-only store with
   committed/prepared frontiers, is the intended fork shape. Keep fence, allocation,
   no-allocation-after-ack and uncertain-publication semantics. Do not substitute
   nested pointer containers blindly: locality and bookkeeping have costs too.

8. **Medium: serialization and response duplication at provider boundary.**
   Coding request is represented as JSON, audited serialized source, encoded native
   input and curl config's escaped data-binary string. openai.cpp completed_response
   parses SSE item records and response.completed, copying items and full response.
   Raw stream retention is useful; multiple mutable copies/encodings are candidates
   for borrowed views/shared immutable packet storage and one prepared body. Measure
   peak RSS/allocation and maximum partial response. Avoid mmap assumptions while a
   writer appends, dangling string_view, or removing originals to save memory.

9. **Medium: transcript repaint and idle polling.**
   terminal.cpp caps transcript at1MiB, but terminal_lines wraps the whole retained
   transcript for each redraw; busy stream and animation may repaint the complete
   viewport. Composer state writes and JSON/hex encoding add copies. Idle poll50ms
   means up to20 wakeups/s, not proof of high CPU. Cache line indexing by appended
   suffix/width and redraw affected regions; measure keystroke/stream latency, bytes
   written and idle CPU. No frame budget claims without those observations. Keep
   scroll/resize/editor/draft retention oracles. Fleet visual polish is not permission
   for per-install browser machinery.

10. **Low, concrete: avoidable isolated copies.**
    AuditLog::original copies capture.bytes to raw; packet histories are copied then
    appended. Reserve stable output capacities where source gives a real upper bound;
    use move/const references when lifetime permits. These are secondary to quadratic
    behavior and duplicate representations, not a hunt for every temporary at the
    expense of correct ownership. Borrowing needs a documented lifetime.

## Immediate sequence

Finish and publish scoped startup repair; keep operator/UI generation separate from
mutable development builds. Next whole unit: source-addressed single-copy input and
stream capture/issuer cadence, choosing a split if durability invariants differ.
Then context originals/revisions and forward prepared suffix storage, driven by heap/
allocation profiles. TUI/provider fine work follows measured costs. Each unit gets
one source-grounded design, useful direct oracle and bounded implementation/recheck.
Never turn this audit into a permanent certification gate for unrelated work.

Production rule: each long-lived byte representation has an owner, lifetime and
reason. Immutable payloads share a prefix; new context/action state forks selected
new values. Work should scale with bytes newly admitted or explicitly inspected.
Storage must retain required facts, not repeat the same large blob because it is
convenient to serialize another struct. Resource discipline preserves semantics and
model capability; it does not mandate shorter useful context or delete evidence.
