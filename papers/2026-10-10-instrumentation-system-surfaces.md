# Instrumentation: system surfaces, source evidence, and emission proposals

Research lane, 2026-10-10. Source inspected at `e85a845340d600b40b07b255040bf57d1b4e51d1` on `command-jobs`. This document proposes instrumentation; it selects no implementation, dependency, exporter, budget, or evaluator. Operator discussion and approval precede implementation under BLACKBIRD.md. No tests, provider calls, or source changes were performed in this lane.

The operator's boundary is exact: **instrumentation emits system/product metrics; operations and evals consume them**. A hook is a system affordance for instruments to observe a defined point, interval, transition, quantity, or transfer. An instrument turns those observations into named, typed measurements and emits metrics. Export, retention, and consumption are subsequent responsibilities. Calling everything a lifecycle event hides important differences, as does adding convenience fields to an audit query.

Complete audit and system ordering through a logical clock remain their own work (`arconaut-nls.10`, `arconaut-nls.9`). Instrumentation cannot authorize omission of audited work, repair missing evidence with a loss counter, or substitute physical timestamps for causality. The previously rejected metadata overlay is not a foundation for this proposal. `papers/2026-10-10-instrumentation-initial-sketch.md` preserves the retired, unapproved early sketch; its synchronous callbacks and fixed schema/CLI bounds are not adopted here.

## Sources actually read

Current implementation:

- `include/blackbird/local_timing.hpp`, `src/local_timing.cpp`, and every current `LocalSpan` call site in main, coding, retained state, and terminal.
- Native value/COW ownership (`value.hpp`, `value.cpp`), persistent radix sequence (`shared_sequence.hpp`), owned packet codec (`packet.cpp`), native error/result/handle/clock/storage abstractions (`foundation.hpp/.cpp`).
- Journal storage, framed append and scan, checkpoint publication; retained append, replay/reconciliation, settlement capacity, source/history lookup, saved-state publication; recovery index and archive catalog.
- Command jobs' collector thread, pipe/PTY bootstrap, stdin deliveries, signals/resizes, wait epochs, mailbox, pending and tail buffers, owner drain and terminal settlement; `native_process.hpp`, child custody accounting, job runner, and terminal custody/resize paths.
- Participants' worker request loop, capture and direction queues, owner drain/save, cancellation, archive/reopen state; existing test oracles named below.
- Local file tools' reads, edits, temporary file writes, sync/rename, original-capture callbacks and subprocess wrapper.

Quarantined reference implementations:

- TigerBeetle `quarantine/storage-tigerbeetle-c95d7a53a3d0/src/trace.zig`, `src/trace/event.zig`, `src/trace/statsd.zig`.
- SQLite `quarantine/storage-sqlite-5af1b822f5da/src/status.c` and allocator status update call sites in `src/malloc.c`.
- Newly acquired LTTng-UST, Perfetto and Tracy snapshots under `quarantine/instrumentation-2026-10-10/`: LTTng's declared tracepoint/probe invocation and release; Perfetto's slice/instant/counter/state/flow call surfaces; Tracy's scoped zone, producer queue, plot/allocation and lock instrumentation. Root owns acquisition, intact archives and restoration manifest.
- Existing primary platform document corpus under `quarantine/command-jobs-platform-documents-2026-10-09/` (POSIX write/wait/terminal/group semantics and Linux/Darwin PTY documentation). These remain platform references, not executable dependencies.

Primary documentation read online:

- [Linux kernel tracepoints](https://docs.kernel.org/trace/tracepoints.html): declared, typed hook/probe interface, caller execution context, and safe removal synchronization. Its kernel implementation and performance claims do not establish Blackbird's costs.
- [LTTng tracepoint API](https://lttng.org/man/3/tracepoint/v2.9/): enabled check plus preparation must stay paired; rechecking separately can race configuration. The general v2.13 documentation endpoint was crawler-blocked; do not claim it was read through that endpoint.
- [Perfetto track events](https://perfetto.dev/docs/instrumentation/track-events): distinct slices, counter snapshots, and flows. Thread-stack scope is inappropriate for unrelated asynchronous intervals; argument callbacks are synchronous and can execute for multiple sessions. Static versus dynamic string lifetime is an explicit concern.
- [SQLite runtime status](https://www.sqlite.org/c3ref/status.html), [connection status](https://www.sqlite.org/c3ref/db_status.html), and [allocator statistics](https://www.sqlite.org/c3ref/memory_highwater.html): independently queryable current/high-water values, explicit reset semantics, and allocation scope. Shared cache attribution is separately described in [connection status options](https://sqlite.org/c3ref/c_dbstatus_options.html).
- [Lua 5.4 reference manual](https://www.lua.org/manual/5.4/manual.html#lua_Alloc): per-state allocator interface and allocation/free/reallocation behavior. The source uses `luaL_newstate`, not an owned accounting allocator today.

## What the reference mechanisms teach

TigerBeetle is particularly useful because it distinguishes instrumentation by purpose instead of forcing all observations through one generic trace. `trace/event.zig:124` explains that trace state needs room for concurrent instances, whereas metric aggregation needs room for the distinct dimensions being aggregated. A concurrent operation index belongs to trace identity but often disappears from the aggregate timing key (`Event` versus `EventTiming`, lines 138 and 219). `trace.zig:241`, `250`, and `430` implement last-set gauges, cumulative counts, and min/max/sum/count timing respectively. Fixed slots avoid arbitrary dynamic labels on critical paths. Its aggregate cannot reconstruct quantiles or individual request histories from min/max/sum/count alone.

TigerBeetle's StatsD emitter (`trace/statsd.zig:98`, `205`) builds bounded packets and sends separately from measurement updates; it returns Busy when the previous packet set is still in flight. It does not make metric transport failure kill application work. `trace.zig:413` says reset-after-emit is appropriate to StatsD and would need removal for Prometheus. This is evidence that temporality belongs to the instrument/export contract, not a casually chosen flush implementation. Neither UDP, that reset policy, its trace assertions, nor its JSON trace format is selected for Blackbird.

SQLite's `status.c:89` and `100` update current/high-water resource state under the already responsible allocation/cache mutex. `status.c:134` snapshots both under the matching mutex. A live-resource measure is produced by the resource owner, not guessed by traversing every reference or exported as a function duration. The allocator call sites (`malloc.c:259`, `286`, `397`, `528`, `548`) separate largest request, current allocation bytes, allocation count, and resize deltas. Documentation makes the accounting scope explicit: these are SQLite allocator amounts, not all process RSS. Blackbird needs the equivalent distinction for shared immutable values, owned buffers, Lua state, OS descriptors, and process memory.

Linux tracepoints teach a stable, typed semantic surface distinct from an attached probe and a safe detach rule. Perfetto teaches that thread intervals, independent asynchronous activities, samples, and cross-thread flow identities are different affordances. LTTng teaches that enablement and payload preparation must be one operation with safe configuration lifetime. These are useful owned design patterns. No external SDK is necessary to supply them.

The newly acquired native source makes those lessons concrete:

- **LTTng-UST** `include/lttng/tracepoint.h:60–70` has an inexpensive shared state check followed by the typed callback. Its generated callback takes an RCU read lock, dereferences the installed probe array and invokes probes (`:191–214`). `src/lib/lttng-ust-tracepoint/tracepoint.c:159` releases replaced probe storage only after an RCU grace period; queued removal similarly synchronizes before freeing (`:782–799`). For Blackbird, the lesson is the need to state **when a detached instrument's storage can be freed**. RCU itself is not selected; immutable process-owned configurations with boundary activation might be sufficient for our owner lifecycle.
- **Perfetto** `include/perfetto/tracing/track_event.h:373–447` exposes distinct begin/end, scoped, instant, state and numeric-counter entry points. A counter includes declared unit and optional multiplier. `track_event_args.h:29–103` gives flow identity its own API, including process-scoped/global/terminating flow distinctions. Source comments in track_event.h still say to add flow events; the actual flow arguments exist and must be read instead of treating that comment as the implementation inventory. The lesson is semantic affordance separation, not copying its transport/Protobuf or pointer-derived ID choices.
- **Tracy** `public/client/TracyScoped.hpp:20–86` forbids both copying and moving thread-scoped zones, checks activation at construction, and emits paired begin/end queue items. On-demand sessions save connection identity and omit an end for a changed connection. That is a diagnostic session policy, not a complete audit rule we can adopt. `public/client/TracyProfiler.hpp:177–195` prepares a producer queue item then release-publishes it; plots emit typed integer/float/double samples (`:443–476`). Named/static source locations avoid transient-name work; dynamic zone text copies into owned storage (`TracyScoped.hpp:89–101`). Its allocation/free observations use a **serial lock and serial queue** (`TracyProfiler.hpp:582–605`, `1125–1157`), rather than pretending all event types are interchangeable lock-free records. Allocation identity and ordering affect the mechanism.
- **Tracy lock surfaces** `public/client/TracyLock.hpp:54–104` emit wait/obtain/release independently. The wrapper calls before-lock, performs the actual mutex acquisition, then after-lock (`:193–197`); release observation occurs before unlock (`:200–203`). It distinguishes try-lock success (`:206–210`). This grounds our proposed contention/held-time hooks and highlights their real cost and reentrancy risks. We do not copy its wrapper or claim metrics have no effect on scheduler timings.

These references also resist a universal `emit(name, arbitrary payload)` hot-path API: activation semantics, pairing, ownership, queue publication, allocation ordering and lock safety differ by surface. A shared emitter can exist behind typed instruments without erasing those distinctions.

## Orthogonal classification: hook types, not subsystem names

The following types occur across many subsystems. A journal is a domain; a durable publication transition, sync interval, transferred-byte quantity, and capacity snapshot are hook types within it. A provider/request taxonomy alone would miss much of the system.

| Hook type | What the surface exposes | Suitable instruments and emission | Important boundary |
| --- | --- | --- | --- |
| Occurrence/outcome | One actual occurrence with a typed outcome and source identity | Count, selected-detail observation, failure count by stable failure class | Requested action, accepted action, attempted effect, and observed result are different occurrences |
| Synchronous interval | Start/end of one scoped computation or syscall phase | Duration distribution, local thread CPU where appropriate, bytes/work done during that interval | RAII can finish on C++ exceptions; Lua longjmp/protected-call behavior requires explicit boundary handling |
| Asynchronous interval | Independent activity identity and explicit start/end/abandonment | Queue wait, run duration, settlement delay, cancellation response duration | A stack span moved across threads cannot use start-thread CPU as if measured on the end thread |
| State transition | Old/new typed state at the mutation authority | Transition count, current state populations, residence-time instrument | Do not infer state changes from periodic snapshots; request to stop is not observed stopped |
| Quantity/delta | Exact work quantity at the operation that performs it | Monotonic total, histogram of batch/transfer sizes, positive/negative resource balance | Bytes requested/read/accepted/retained/rendered measure different work |
| Ownership/resource change | Acquire/release/resize and resource kind at its owner | Current live quantities and high-water marks, allocation churn, COW detach work | Reference counts and logical byte size are not physical unique bytes or OS RSS |
| Data-flow transfer | Item/chunk/control identity, stage, queue/endpoint, and quantity | Enqueue-to-dequeue distribution, transfer rates, stage occupancy, backpressure intervals | Flow relationships are explicit identities; timestamp proximity is not linkage |
| Sampling/snapshot | A coherent current state plus freshness/domain/status | Gauge/sample emission, periodic resource observations | Snapshot measurement is produced by instrumentation; an operations/evals consumer reading it does not become its producer |
| Programmable/domain-defined measurement | Registered instrument semantics and a native/Lua measurement call | Custom counters, distributions, samples associated with program/product concepts | Definition must specify unit, aggregation and scope; scripts cannot mutate hook arguments or gain effect control through measurement |
| Instrumentation status | Instrument/configuration/transport boundary outcome | Activation/collection status, receiver failure, buffer capacity, invalid definition or clock status | This is direct interface status, not a second-order certification system; required audit failures remain audit failures |

Occurrence and delta may be represented by the same call-site descriptor but retain different semantics. Sampling a current quantity periodically and emitting every resource change are options, not interchangeable claims of completeness. Full audit is independent of any optional selection of metric detail or profiling samples.

Measurement shape is another axis, distinct from the hook's mechanical class:

| Measurement shape | Meaning | Example instrument fed by hooks |
| --- | --- | --- |
| Nonnegative delta | Work newly performed at this boundary | Bytes returned by one read, one successful launch |
| Signed delta | Change in an owned current quantity | Queue enqueue/dequeue balance, allocation resize/free |
| Absolute cumulative value | Total within a stated owner/incarnation/epoch | Bytes received by this process so far |
| Absolute current value/gauge | Quantity true at the sample point | Pending queue bytes, direct owned children |
| Individual distribution observation | One measured instance, before temporal aggregation | One actual sync duration, one batch size |
| Distribution aggregate | Stated summary over observations and interval | Count/sum/buckets with defined boundaries; min/max/sum/count alone gives no quantiles |
| State/category observation | Defined categorical fact/transition | Ready, stopped, draining, publication rejected |

A static hook/probe, scoped interval hook, explicitly matched async hook, owner snapshot callback, or queue transfer point can supply more than one shape. A signed delta is not itself a hook registration mechanism. The instrumentation producer owns measurement and emission semantics; operations/evals consume the emitted metrics, whether delivered as a stream or collected snapshot.

## Existing instrumentation affordances and their limits

`LocalSpan` is real instrumentation, not just audit metadata. `local_timing.hpp:17` describes fixed records holding action/source/outcome, elapsed wall/thread CPU, bytes/history/sequence, and revision/attempt linkage. The debug sink has 8,192 records and a mutex (`:26`). Push does no file I/O but takes that mutex; at capacity it increments a dropped count (`local_timing.cpp:26`). Timing is opt-in through a process environment variable, uses CLOCK_MONOTONIC and CLOCK_THREAD_CPUTIME_ID (`:16`), and persists at shutdown (`:34`, `:167`) in native BBMS/BBM packets with durability explicitly false. Release builds erase clocks, TLS, sink, and output (`local_timing.hpp:79`). Existing getters of size/record entries are not documented concurrent snapshots; do not repurpose them as a live multi-thread consumer API without a new synchronization contract.

Current hook sites: startup initialize (`main.cpp:66`), restoration (`:242`), first terminal frame (`terminal.cpp:1283`), worker dispatch/perform (`:1452`, `:1467`), render (`:1706`), input-save (`:2020`), coding admission (`coding.cpp:1329`), provider prepare/transport (`:1697`, `:1890`), and retained append (`retained_state.cpp:1169`). The terminal explicitly carries the sink into its worker TLS (`terminal.cpp:1450`) and uses CPU-disabled dispatch timing for the cross-thread handoff. Participant and command collector workers do not inherit this mechanism. The terminal `command.perform` finishes with outcome `returned` even after its catch paths (`:1468–1478`); consumers must not interpret that label as successful work.

`Observations::sample` (`observations.cpp:82`) is a durable program/operator-defined sample: validates name/status/source, retains `variable.sample`, value and provenance, supplies clock fields and returns audit locators. This is a useful domain sampling affordance but presently allocates/formats native values and performs audit work. It is not suitable for allocator, signal-handler, codec-node or mutex-held hot paths. It is not a generic definition/aggregation registry. `Observations::time` publishes a UTC/steady-clock bracket; `foundation.hpp:255` also has typed clock domains and elapsed/deadline checks. These multiple mechanisms need one approved role division rather than another ad hoc clock schema.

Presentation callbacks (`Participants::Publish`, coding/UI hooks), raw original capture callbacks (`ColleagueCapture`, `LocalTools::observer_`, provider chunks), and journal `Storage` methods are existing interfaces at useful semantic locations. They are not interchangeable metric receivers: audit capture can fail work, presentation intentionally cannot, and storage determines durable state.

## Domain-by-domain system surface inventory and proposal

### A. Native values, numeric representations, codec and language bridge

Sites: `value.hpp:44–115` creates `shared_ptr<Storage>`, shares unexposed copies, copies exposed storage, and detaches on mutable borrow; `shared_sequence.hpp:23–27`, `70–83` creates/copies radix nodes and leaves. `foundation.cpp:82` copies bounded byte buffers. `packet.cpp:140` increments node/depth accounting; `:371`, `384` delimit encode/decode/projection and distinguish capacity/corrupt/unsupported/allocation errors. Decode projection suppresses retained subtrees but still validates input (`:347–354`). `encode_packet_string` additionally copies an encoded byte vector into a string (`:409`). The Lua bridge recursively converts native values (`coding.cpp:123` and its corresponding native-to-Lua helpers).

Proposed affordances:

- Typed encode/decode/format/bridge begin/end hooks expose direction, encoding, result/error, input/output bytes, visited nodes, maximum depth and actually materialized nodes. Counters/distributions distinguish total parsing work from retained projection size. Get these values from work already performed; no second tree traversal to compute them merely for metrics.
- A coarse occurrence/delta surface at COW detach and radix-node/leaf creation exposes representation kind, copied element/byte count available at that point, and resource owner. For hot inline constructors use an out-of-line small wrapper or explicit instrument pointer that does not increase every `Value`'s footprint without measurement. No arbitrary callback or Value construction inside allocation/failure observation.
- Allocation-request and allocation-success are separate hooks. Rejected capacity versus failed allocation are distinct outcomes. Lack of a successful allocation observation must not be counted as zero requested bytes. Physical allocation bytes require an actual owned allocator/accounting mechanism; until available label owned payload/element quantities precisely.
- Do not count every pinned `Value` or root as a physical copy. Unique-storage accounting updates when storage is created/destroyed; sharing/pinning is separate activity. Counting unique resident bytes requires lifetime tracking, not merely adding `sizeof(Value)` times logical nodes. The cost/benefit of adding that tracking is an explicit design option.

Emitter options: aggregate per-owner counters at coarse boundaries for ordinary metrics; separately selectable detailed creation/detach observations for diagnosis. Systemwide global `operator new` interception is not proposed as the first implementation: it risks recursion, misses attribution, changes unrelated allocation behavior, and needs independent source study. Typed accounting at owned allocations is narrower and truthful.

Direct future oracles: exact codec fixtures with independently known packet bytes/depth/nodes; failed capacity/unsupported/corrupt paths each produce their proper measurement; COW copied prefix and newly allocated radix-path counts match a simpler ownership model; repeated immutable copies cause no invented payload-allocation deltas. `native_value_test.cpp`, `packet_test.cpp` and retained-state sharing tests already exercise the underlying semantics; add metric expectations to actual work, not tests that merely invoke the emitter.

### B. Journal physical I/O, framing and durability

Sites: `NativeJournalFile::read_at/write_at/synchronize/lock_writer` (`journal_storage.cpp:83`, `95`, `116`, `119`); native sync differentiates F_FULLFSYNC/fsync (`:31`). `FramedJournal::read_exact/write_exact` (`journal_writer.cpp:263`, `285`) handles short/zero/interrupted I/O. `append` (`:303`) validates capacity/state, stages allocations, checks extent, writes and syncs, then publishes cursor/index (`:367–397`). Failed write or sync poisons state; conflicting extent blocks it. These phases are materially different from the existing aggregate retained.append duration.

Proposed affordances:

- At the storage boundary emit read/write/sync/lock intervals and exact requested/actual byte deltas, syscall retry/short-I/O counts and native error classes. Preserve per-file owner kind (journal/index/catalog/checkpoint/head/scratch), not raw paths as unconstrained metric dimensions.
- At framed append emit logical batch accepted/refused, source/semantic/frame counts and payload/framing/commit bytes; then write attempt, sync attempt and durable acknowledgement. Total append duration plus phase distributions answer different questions; neither double-counts a request counter if each definition names its boundary.
- State transition hook at live/blocked/poisoned/recovery-pending/recovered changes. Metric counts do not change the failure fence. A sync result cannot be fabricated from later in-memory publication, or from rename success alone.
- Low-level storage hook must avoid reading journal data, allocating audit values or synchronously exporting through the same journal. Capture fixed primitive observations outside allocation-sensitive phases and emit through an owned safe path; no recursive append from an append hook.

Direct future oracles: existing fault-injected `journal_batch_test.cpp`, `journal_native_test.cpp`, `journal_crash_test.cpp` and `journal_resume_test.cpp` independently expose partial writes and crashes. Assert measured committed count only after actual sync success and cursor publication; partial syscall work remains measured even when append fails. A simulated short write must increment actual bytes by each returned amount, not full requested batch length on every retry. Storage failure does not dispatch effects or turn audit completeness defects into metric transport defects.

### C. Retained semantic state, indexes, checkpoints, archives and recovery

Sites: `RetainedState::replay` (`retained_state.cpp:937`) stages a valid prefix and rolls back rejected batches; append (`:1165`) protects settlement and capacity, stages indexed candidate state, encodes facts, and commits. `protect_settlement/refresh_settlement` (`:1115`, `1139`) reserve known credit. Automatic checkpoint admission thresholds (`:1186`) and bounded suffix capacity (`:1245`) expose pressure and maintenance results. Current saved state publishes catalog then saved-state root, trims resident suffix only after success (`:2038–2175`). `ContextStore::checkpoint` (`context.cpp:363`) publishes and retires non-live resident original metadata after successful archive publication. `RecoveryIndex::read/write/find/insert/range/synchronize` (`recovery_index.cpp:56`, `115`, `157`, `177`, `292`, `308`), `ArchiveCatalog::publish_merge/open/entry/find` (`archive_catalog.cpp:151`, `194`, `231`, `259`) are distinct disk/query surfaces. Head selector and root-file publication own rename/directory-sync boundaries (`environment_head.cpp:160`, `saved_state.cpp:191`, `journal_checkpoint.cpp:164`).

Proposed affordances:

- Snapshot hook from the sole state owner: resident facts/sources, archived counts, live suffix bytes, remaining journal capacity, unresolved operations, outstanding settlement credit, maintenance state. Obtain existing counts, not full history scans.
- Explicit maintenance/restore phase hooks: checkpoint requested/started/skipped/failed/published; catalog/index/root publication phases; compact restore chosen/refused; scan bytes/frames; semantic replay batches/facts/bytes accepted/rejected; reconciliation outcome and unresolved counts. Differentiate scratch cleanup, archive availability, resident retirement and physical original deletion.
- Query/path hooks expose index hit/miss, index pages read/written, archive versus resident lookup, projected payload bytes, full replay fallback selected/refused and work performed. Latency distributions can then explain expensive opens without inventing a single generic restoration measure.
- Separate per-live-session gauge ownership from process-cumulative work counters. Checkpointing does not reset process work totals; process incarnation/reset metadata must make reopened counter scope unambiguous. Do not recalculate cumulative counts by replaying old semantic facts as if they happened again now.
- Preserve distinct actual source/semantic rejection outcomes and failed publication phases even if the prior checkpoint remains selected. A high-water resource sample is not evidence that required originals may be deleted. The current expiring diagnostic storage includes provider originals; that is the audit-completeness deficiency already filed, not a metric retention policy we approve.

Direct future oracles: `journal_checkpoint_test.cpp`, `saved_state_test.cpp`, `archive_catalog_test.cpp`, `recovery_index_test.cpp`, `retained_state_test.cpp` and `session_recovery_test.cpp` exercise real durable source. Use independently known counts and disk-operation fakes for expected phase work. Reopening unchanged state should add restore/scan work but no newly executed tool/provider counts; failed checkpoint must not emit publication or resident-retirement success.

### D. Ordinary file tools and native external-process adapter

Sites: `tools.cpp:8` reads with exact chunk sizes; `:24` builds a private temp file, writes short chunks, syncs, closes and renames; `LocalTools::run` (`:179`) captures before/proposed/read originals and distinguishes edit conflict. `detail::Child` (`native_process.hpp:59`) owns spawn, stdout/stderr observers, send/read/poll/collect and group cleanup/reap. Its capture callback is invoked before buffer capacity checks (`:307–335`). Process custody counters are shared in `process_lifetime.hpp:9`; this is cooperative direct-child custody, not containment of arbitrary descendants. This adapter is used by providers/auth/Beads and legacy tools, not just user exec.

Proposed affordances:

- File acquisition/read/presentation/write-temp/sync/rename phase hooks expose counts, exact transfer bytes, proposed bytes, retained capture bytes and outcome. Presented slice bytes differ from bytes actually read; preserve both definitions. Edit-match conflict happens before write and is its own outcome, not an I/O failure.
- Native child hooks at spawn requested/success/failure, bytes sent, stdout/stderr bytes read, parsed-message boundary, output EOF, observed leader exit, signal attempt/result, cleanup and authoritative reap. Category/source identifies provider adapter versus tool/editor/auth process without recording command text as a metric label.
- Poll wait duration, I/O progress and timeout/cancellation are distinct. Successful read syscall means bytes were received; capture acceptance and durable retention are further phases. Emit measurements before a subsequent local failure erases convenient aggregate result fields.
- Child CPU/RSS/resource usage is not currently available at these points. A proposed platform adapter could collect exit usage using an appropriate native wait/resource API, but exact single-child attribution and process-group descendant coverage require further qualification. Do not turn process-global RUSAGE_CHILDREN deltas into exact per-job usage amid concurrent children.

Direct future oracles: real file fixtures independently inspect write/edit results and original bytes; fake syscall counts check partial writes; existing isolated child tests independently count launches/output/exit. A missing executable must count one failed spawn attempt and zero observed running children. Byte totals must match child-side fixture bytes, including invalid UTF-8/NULs and separate stderr.

### E. Command jobs: lifecycle and collector state

Sites: `command_jobs.cpp:653` constructs/admits a Job and launches one collector; `:311` handles pipe/PTY bootstrap, launch and ready state. Job state contains starting/running/stopped/draining/settling/completed components (`:222–237`, `:263`) rather than one boolean. PTY runner establishes private session/terminal then signals bootstrap readiness (`command_job_runner.cpp`). Worker sees leader exit through WNOWAIT (`command_jobs.cpp:577`), independently observes stopped/continued (`:594`), drains descendant-held output and finally reaps (`:619`). Owner drains chunks/terminal (`:932`), acknowledges retention (`:972`) and terminal settlement (`:986`).

Proposed affordances:

- State transition hook per actual mutation; stable enum components allow instruments to count active/running/stopped/draining/settlement populations without treating observational projection as authority. Launch success/ready/leader-exit/output-close/reap/durable-terminal are separate occurrence surfaces.
- Explicit async intervals: start-to-spawn, spawn-to-ready/bootstrap, execution lifetime, leader-exit-to-output-EOF, finished-to-terminal-retention and cleanup/reap. At each point give job and execution context identities as association fields; aggregate series can use bounded source/PTY/outcome classes rather than a series per PID/job.
- Deadline expiration, cooperative stop request, TERM attempt, KILL escalation and forced output closure are separate hooks (`:422–451`, `:604`). Their metrics expose actual behavior even where effect outcome remains unknown. Hooks do not relax containment or recovery fences.
- Off-thread collector observations must not access ContextStore/AuditLog or inherit a borrowed main-thread sink pointer without a guaranteed lifetime. Give collector a stable emitter/context handle scoped to CommandJobs' shutdown/join protocol; publish through a non-reentrant owned channel or per-owner instrument storage.

Direct future oracles: `command_jobs_test.cpp`'s independently observed child fixture checks group/PTY identity, EOF-before-exit, descendant tails, actual SIGSTOP/SIGCONT, exec failure, timeouts and exact reap. Extend those real observations with expected hook states/counts. One launch can create many wait intervals but must remain one launch; stop/continue stays the same PID and is not a new execution. No terminal measurement before output-before-terminal ordering and actual owner settlement.

### F. Command data flow, stdin/resize/signal deliveries, foreground/background

Sites: collector output read (`command_jobs.cpp:542`), pending queue/tail accounting (`:549–563`), deferred reads at pending capacity (`:615`), drain (`:932`), and retained offset acknowledgement (`:972`). Delivery queue stores requested bytes, written offset, done/failed (`:211`); partial stdin writes (`:511`), resize ioctls (`:529`), signal request dequeue (`:497`). Control validates and deduplicates request identity (`:759–874`); UI mailbox admission/drain (`:876`, `903`) and foreground wait epoch (`:685`, `:794`, `:1002`) are different transfers.

Proposed affordances:

- Byte-flow deltas at received, queued, dequeued, durable-retained and presented stages. Per-stage sizes and backpressure intervals are available without copying payloads. Detail observations may reference chunk offset/length; aggregate instruments count bytes exactly once at the named stage. Full original bytes remain audit's obligation.
- Input request accepted/rejected/duplicate; queue admission/dequeue; actual kernel write bytes; delivery complete/partial/unknown. Duplicate command controls must emit duplicate-observation metrics without recounting bytes as newly delivered. Resize requested versus ioctl accepted and signal requested versus OS signal attempt/result versus observed stop/continue/exit must remain separate.
- Foreground acquisition/release hooks expose the actual wait epoch and release reason (terminal, yield deadline, background request, cancellation). An app foreground wait is **not** POSIX terminal foreground process-group reassignment. Background releases the wait; it does not send SIGSTOP or relaunch the child. Metrics should preserve this distinction rather than grouping all of it into a vague foreground/background state.
- Record matched handoffs through queue item/request identities; do not assume a main-thread sequence is the worker execution order. Hook captures state under the existing mutex; prepare a fixed observation and publish after unlock where possible. Arbitrary receivers/formatters while holding `Impl::mutex` could block stdin, drains, stop and readiness; no synchronous generic user callback there.
- Current `signal_owned` discards actual kill errors (`:325–329`), so successful request dequeue alone does not support a signal-delivered claim. An implementation wishing to emit OS acceptance must expose the syscall outcome without changing cleanup policy and must distinguish group fallback. This is an identified observability limitation, not a declaration that the current control API is fixed.

Direct future oracles: actual fixture stdin receipts, 65,536-byte blocked input, dedup, PTY resize, stale foreground epochs, full 1-MiB pending queue and TERM/KILL behavior (`command_jobs_test.cpp:286–604`). Queue saturation should create a backpressure measurement, not imaginary received bytes. Independent child-side data validates bytes actually delivered; receipt acceptance alone does not.

### G. Participants and cross-thread queues/concurrency/cancellation

Sites: `Participants::Impl::work` (`participants.cpp:104`) dispatches actual requests and consumes directions at request boundaries; capture callback queues received raw data (`:114`) with current 8-MiB/1,024-event limits. Owner drain retains before pop (`:263`) and saves state independently (`:294`). Worker state updates/waits/direction dequeue (`:140–199`); publish deliberately catches presentation failures (`:92`). Start/send/cancel/await/join/archive/shutdown (`:329`, `425`, `490`, `508`, `536`, `576`, `595`) and reopen conversion to unknown (`:239`) are separate points.

Proposed affordances:

- Worker request begin/end emits one actual-request outcome and worker-execution interval; row lifetime/request lifetime/direction wait remain separate. Metrics derive reported usage only from response observations at the provider boundary, not from requested models or queue state.
- Direction accepted/dedup/rejected, queued/dequeued/abandoned; capture received/queue-admitted/retained; owner-save attempted/acknowledged/failed. Queue item context supplies transfer links and enqueue/dequeue measurements. Capture/direction resource samples read counts/bytes under the responsible mutex.
- Run state transitions and concurrency-admission failures; separate cancellation requested, worker observed cancellation, transport interruption disposition, request-boundary stop, join completed/timed out. Await timeout does not mean request timeout or worker cancellation. A reopened row unknown does not mean a remote provider request occurred during reopen.
- `Retain` is an audit boundary and `Publish` is presentation; add instrumentation directly at semantic points, not by treating one callback as both. Metric receiver failure cannot suppress mandatory retain/save. Existing raw-capture queue refusal is a real audit deficiency: count/observe it to diagnose, but do not claim the loss counter makes audit complete.
- Publish fixed measurements safely outside lock and avoid generic callbacks from the transport capture closure under the participant mutex. Immutable emitter lifetime follows worker shutdown/join, not the arbitrary caller's stack. Collection snapshots may lag worker mutations only if instrument contract states snapshot time; a delayed owner drain cannot overwrite execution timestamp.

Direct future oracles: `participants_test.cpp`'s counted transport and capture-failure paths, `participant_recovery_test.cpp` reopen semantics. Exact actual request counts with queued directions/cancel-before-dispatch; request status is independently supplied by transport fixture. Retention retry must count repeated retention attempts while keeping actual provider dispatch count one. Save failure and presentation receiver failure are separate injected outcomes.

### H. Terminal custody, UI worker and native synchronization

Sites: outer TerminalMode acquires/restores termios and screen modes (`terminal.cpp:307`), editor suspension (`:330`), native editor process (`:355`), SIGWINCH self-pipe (`:34`), turn worker handoff (`:1447`), foreground lease projection (`:1643`), child resize request (`:1687`) and terminal writes (`:294`). Child's private PTY/session/foreground belongs to command runner, while outer terminal editing/paste/composer custody belongs to UI. Native spawn serialization uses `process_spawn_mutex`; command/participant queues and UI have their own mutexes/condition variables.

Proposed affordances:

- Custody transition hooks: outer raw-mode acquired/restored, editor handoff start/end/failure, child-input route acquired/released, terminal-control flush completed/timed out. Presented/rendered frame and terminal accepted bytes are not interchangeable; actual output writing can have its own interval/delta surface.
- Worker queue/handoff begin/dequeue/finish and user-input-to-owner-request transfer; existing cross-thread worker-dispatch span is an example but lacks general explicit flow semantics. Actual terminal resize handled and request accepted are distinct observations.
- No allocating/locking/clock-reader generic instrumentation inside `resize_signal`. Instrument the owner draining the wakeup pipe and handling the resize; if raw signal occurrences are wanted, design a qualified signal-safe counter or fixed record mechanism. Self-pipe byte coalescing means drain count does not automatically equal OS signal count.
- Lock contention/wait instrumentation is a separate, selectively enabled diagnostic surface, not achieved by timing an entire operation. Pair before acquire/after acquire/before release at owned critical locks; count acquisitions plus wait/held intervals. Avoid recursive sink-lock observation and emitting under the observed lock. Taking every mutex through a new wrapper has broad implementation/overhead costs and requires approval.
- Process resource snapshots (own CPU, RSS, descriptors, threads) need dedicated platform owner/provider and status/freshness fields. Process CPU samples cover all threads; thread CPU interval measurements cover one thread. Do not compare the two as identical quantities.

Direct future oracles: existing PTY terminal/UI tests plus explicit blocked-output fixtures validate custody restoration and background release. Inject fixed clocks for handoff duration; verify no clock/thread CPU request when a disabled or CPU-inapplicable surface executes. Separate scheduling cost from held-lock duration. No signalhandler behavior change justified by a metrics test.

### I. Clocks, instrumentation control and measurement delivery

Sites: typed domains (`foundation.hpp:255`), wrong-domain/regressed checks (`foundation.cpp:95`, `128`), native clock sample (`:162`), local debug clock pair (`local_timing.cpp:16`), Observations' bracketed sample/domain publication (`observations.cpp:44`, `57`). Debug sink capacity/shutdown/status are current delivery behavior, not an adopted production policy.

Proposed affordances:

- A system hook context carries the approved logical ordering identity when available, with explicit physical clock domain/sample for duration measurement. This integrates with nls.9 without quietly selecting its algorithm. Logical order and elapsed nanoseconds are distinct quantities; logical clocks do not replace a physical duration source.
- Instrument definition/enable/disable/reset/retire and collection boundaries are explicit surfaces. Definitions specify quantity unit, scope, monotonic versus bidirectional update, distribution/sample semantics, temporal aggregation and measurement validity. Runtime enablement must not leave a dangling receiver or permit arguments prepared under one configuration to be emitted under another.
- Two delivery classes are design options: cheap always-available aggregate owner state, and detailed selected observations through producer buffers. A single arbitrary synchronous receiver bus is inadequate for syscall/allocator/lock/capture paths. A buffer implies an explicit capacity, lifecycle, overflow and flush contract; naming dropped counts does not satisfy audit completeness. Required measurement retention semantics must be discussed instead of assumed lossy.
- Direct interface status (emission refused, definition invalid, receiver failure, buffer fill, clock unavailable/regressed, consumer lag) must remain inspectable even when that metric transport is itself unavailable, for example through stable owner status. No recursive metric emitter for its own failed emission. This is a direct operational interface outcome, not a metric whose only job is to certify another metric.
- Collection supplies coherent snapshots/epochs or clearly states approximate concurrency semantics. Consumers do not mutate producer state by default; resetting high-water values/counters is an explicit instrumentation control action. A scrape should not silently reset a process counter, and two consumers should not steal each other's interval data.

Direct future oracles: fixed clock domain/regression fixtures, two collectors receiving identical cumulative state, detach while in-flight producer is blocked, buffer/transport failure without product outcome changes, exact enabled/disabled hook-work counts and reentrancy prevention. `local_timing_test.cpp` provides existing injected clock and native packet persistence oracles, not production metrics qualification.

## Cross-cutting proposed contracts

1. **Semantics at owner:** hook points occur where their fact becomes true. Record requested/accepted/dispatched/observed/retained/published separately. Report failed and interrupted paths; a normal return label is not successful work.
2. **Typed small observation:** fixed descriptors and primitive fields for low-level hooks; native values only where the domain already has them safely. Never JSON round-trips internally. Detail association identity and aggregate metric dimensions are separate concerns.
3. **No duplicate work:** derive byte/node/frame counts from actual loops or owner-maintained state. An instrument does not recursively traverse original payloads or reread history just to compute counts that can be exposed once at the source.
4. **No hidden control:** emission does not change dispatch/retry/state policy; operations/evals receivers consume. Instrumentation activation/configuration is explicit and separately authorized, not an evaluator asking a synchronous product callback to perform effects.
5. **Bounded critical-path work by design:** enabled check before optional formatting, clock sampling and payload inspection; safe configuration snapshot for paired preparation/emission. Owned hot-path updates can be primitive increments; formatting/export happens outside owner locks. Actual costs must be measured after approval, not promised from a reference's benchmark.
6. **Lifetime and concurrency:** emitter/context ownership survives worker use; detached instruments wait for in-flight calls or use a safe immutable lifetime mechanism. Borrowed byte views cannot enter an asynchronous queue without an owned copy or pinned existing payload. Avoid broad producer serialization on one global sink mutex.
7. **Truthful resources:** logical payload bytes, allocated storage bytes, physical disk bytes, queued bytes and RSS are different units/scopes. Shared immutable payloads are not counted once per handle as physical memory. Reopen/replay counts reconstruction work rather than reexecution.
8. **Retention decision still open:** audit everything is already a requirement. Optional profiling selection and aggregate metrics may have different collection needs, but no lossy metric policy is selected here. State delivery guarantees and failure behavior concretely before implementing them.

## Decisions the integrated design must present for discussion

- Which hooks are permanently available in release and which diagnostic detail categories are selectable? Debug-only instrumentation cannot serve release operations.
- Whether owner-maintained aggregate instruments plus selectable detail channels are sufficient, and what metric streams/snapshots consumers need without defining an evaluator.
- Exact durability/retention requirements of emitted measurements, including rejected/failed work, before choosing producer buffers, synchronous delivery or external export.
- Instrument definition/registration, dynamic Lua/program measures, semantic versioning and lifecycle across source generation/hot reload; metric definitions do not silently change under a continuing name.
- How logical-clock context is supplied to hooks/worker transfers; final algorithm and audit/system propagation belong to nls.9.
- Per-domain work/space costs and receiver failure behavior; neither a guessed 4-KiB packet nor a generic callback timeout is a measured budget.
- Whether owned allocation accounting, OS resource sampling and selective lock tracing should ship together or as separately approved capabilities. Each is a concrete surface class; none should be silently omitted by calling the design comprehensive.

The immediate implementation plan must follow approval. It should add direct expectations to real underlying operations and qualify failure paths, concurrency/lifetimes, native format and meaningful disabled/enabled cost. Full prescribed C++/Lua gates and selected sanitizer/platform checks remain required. No evaluator selection/execution is implied by testing instrumentation itself.

## Source coverage boundary for integration

The system lane accounts for these units at their relevant ownership/operation boundaries, not every function as a code-coverage claim:

| Covered group | Source/header units | Coverage in this paper |
| --- | --- | --- |
| Native foundation/resources | foundation.hpp/.cpp, value.hpp/.cpp, shared_sequence.hpp, packet.hpp/.cpp | Error/resource/clock/storage/handle contracts; representation sharing and codec work |
| Physical durable storage | journal.hpp/.cpp, journal_storage.hpp/.cpp, journal_writer.hpp/.cpp, journal_checkpoint.cpp | Frame/write/sync/scan/checkpoint and state mutation phases |
| Retained/archive/recovery storage | retained_state.hpp/.cpp, archive_catalog.hpp/.cpp, recovery_index.hpp/.cpp, saved_state.hpp/.cpp, environment_head.hpp/.cpp; context.cpp checkpoint/original lookup | Append/credit/index/replay/reconcile/saved-root/archive/resident retirement boundaries |
| OS work/processes | tools.hpp/.cpp file/process paths, native_process.hpp, process_lifetime.hpp, command_jobs.hpp/.cpp, command_job_runner.cpp | I/O quantities, launch/custody/PTY, command states/data flow/control/wait/delivery |
| Concurrency | participants.hpp/.cpp; terminal.cpp worker/resize/terminal custody; shared process spawn lock | Owner/worker transfer, queue/save/retention/cancellation, custody and selective lock timing |
| Existing measurements | local_timing.hpp/.cpp, observations.hpp/.cpp; main/coding/terminal/retained timing call sites | Actual current hooks and validity/lifetime/disabled-build behavior |
| Diagnostics | diagnostics.cpp, stream_capture.hpp, retained-state diagnostic creation/expiry sites | Current expiry/capture limitations, metric versus audit failure distinction |

Other lanes must integrate provider/network/auth/CLI stream semantics, full Lua/workflow/program/generation lifecycle, context revision/compaction beyond its storage boundary, tasks/candidates/station/backstop/session/restart controls, Beads/integration adapters, UI input/render/layout beyond custody, product-domain/custom instruments, and optional/deferred multiplayer/services. This lane inspected shared boundary portions of coding/context/session/main/workflows but does not claim a complete domain inventory for them.

Still unqualified system capabilities are exact physical heap/accounting per owner (the code currently uses ordinary shared_ptr/vector/string allocations), per-state Lua allocator accounting, platform process/RSS/descriptor/resource sampling, single-child/descendant resource attribution, instrument activation/detach lifetime, transport/durable metric retention and the logical-clock propagation algorithm. They are explicit proposed surfaces/design choices, not silently missing implementations excused by the existing audit or debug spans.
