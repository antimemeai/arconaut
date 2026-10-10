# Instrumentation study: product and harness surfaces

2026-10-10. Research and proposed design only. No implementation, metric policy,
evaluator, dependency, or activation is selected by this report. Source inspected
on the `command-jobs` candidate; the previous operational metadata patch is not
the design proposed here. No builds, tests, provider requests, or reference
programs were executed for this study.

The operator's boundary is exact: **instrumentation emits metrics; operations
and evals consume metrics**. A hook exposes a system occurrence or observation
to instrumentation. The hook payload is not automatically a metric, and an
audit query is not an instrumentation surface merely because someone can derive
numbers from its output.

This report owns the product/harness inventory. Low-level allocation, storage,
OS process, transport, and emission machinery need the companion system study.
It identifies overlap explicitly rather than treating a product operation and
its underlying resource work as one measurement.

## Findings that change the design

1. Instrumentation belongs at the actual semantic owner. `CodingEngine::call`
   wraps native tools, but Lua's `manage`, `edit`, `append`, `inspect`, and
   `restore` bridge operations call `ContextStore` directly
   (`src/coding.cpp:440–500`). A hook only in `CodingEngine::operation` would miss
   these paths. A hook only in operator dispatch would miss model/Lua usage.
2. Existing presentation callbacks are not the desired interface. The public
   callbacks in `include/blackbird/coding.hpp:77–94` mostly carry strings or
   untyped native values; `src/main.cpp:444–500` connects them to terminal
   presentation. Task and UI notifications deliberately swallow failures after
   committed work (`src/coding.cpp:1358`, `src/tasks.cpp:592`). They do not define
   complete lifecycle observation, causal context, or metric emission.
3. Request, response receipt, decoding, acceptance, context publication, and UI
   delivery are different occurrences. `CodingEngine::request` streams bytes,
   imports the response, extracts selected usage, and then appends output to the
   context (`src/coding.cpp:1876–1999`). A response can arrive and contain usage
   even when import or publication fails. Instrumentation must preserve the
   distinctions rather than assigning one `success` boolean to everything.
4. Product states require product semantics. A managed context proposal is
   currently staged by calling `reject_managed(..., "staged")`, producing
   `accepted=false`, then adding `staged=true` (`src/context.cpp:937`). Counting
   all `accepted=false` replies as rejected proposals would be wrong. Similarly,
   a completed command does not complete a task, a participant's accepted
   direction is not delivery, and a recovered UI draft is not a submitted prompt.
5. Worker execution must be observed on the worker. Participants accept and
   retain requests on the owner, perform transport on workers, publish guarded
   state, then drain captures on the owner (`src/participants.cpp:104,263,329`).
   Each of those is a separate surface; using owner drain as execution timing
   would measure scheduling/drain behavior rather than request behavior.
6. Complete audit remains required independently of metrics. The existing
   queue capacity and post-effect retention failures are audit deficiencies,
   not a justification for a lossy instrumentation interface. This research
   does not resolve the P0 custody issue or the logical-clock system upgrade.

## Read primary material

The following are read design sources, not a selected SDK or backend.

* [OpenTelemetry Metrics API](https://opentelemetry.io/docs/specs/otel/metrics/api/)
  distinguishes instruments invoked at processing sites from observation
  callbacks invoked at collection. Callbacks have registration lifetimes and
  should be reentrant, bounded in execution, and free of duplicate observations.
  This is useful for separating changes from sampled current state. Its SDK
  object hierarchy, names, policies, and default error handling are not adopted.
* [Prometheus instrumentation practices](https://prometheus.io/docs/practices/instrumentation/)
  distinguishes interactive service, processing pipeline, and terminating batch
  behavior, and discusses queues, libraries, caches, failures, and label costs.
  Blackbird contains all three service shapes at once. The transferable point
  is to instrument actual boundaries and distinguish queue residence, active
  execution, and completed work. Suggested backend and label policies remain
  consumer/export design, not an excuse to discard source observations.
* [W3C User Timing](https://www.w3.org/TR/user-timing/)
  distinguishes named marks from measurements between explicit endpoints, with
  attached detail. That supports programmer-defined workflow phases where the
  native harness cannot know a product milestone. Blackbird should use opaque
  mark/region handles rather than pairing by the latest matching string name,
  and preserve the local clock domain and code generation. This is a semantic
  analogy, not browser API adoption or a logical-clock solution.
* [Google SRE, Monitoring Distributed Systems](https://sre.google/sre-book/monitoring-distributed-systems/)
  distinguishes internal observations from externally visible behavior, and
  symptoms from causes. Applied here: successful provider receipt, successful
  context publication, and successful terminal write are distinct observable
  facts. None establishes user comprehension or task correctness. Those later
  judgments belong to consumers with an explicitly chosen oracle.

## Reference implementations and their tests

Existing quarantined sources remain unchanged. Revisions and restoration are
recorded in `papers/2026-09-30-known-agent-acquisition.json` and
`papers/2026-09-30-discovered-agent-acquisition.json`.

### NullClaw: small typed seam, mistaken metric export semantics

Snapshot `55907af88e51ff37cc114dcb6eb3ca438e5b0c2b`,
[upstream](https://github.com/nullclaw/nullclaw/tree/55907af88e51ff37cc114dcb6eb3ca438e5b0c2b).

`quarantine/nullclaw/src/observability.zig:9` defines a tagged event union with
distinct request/response, tool start/result, agent start/end, subagent,
iteration exhaustion, channel message, and skill-load cases. A separate metric
union appears at line43; a vtable, null implementation, and fan-out appear at
lines51,170,400. This is a useful owned native interface pattern: callers supply
typed facts; different instrumentation handlers can attach without entangling
the core with an exporter.

Actual request and failure call sites are in
`quarantine/nullclaw/src/agent/root.zig:2213–2426`; helper definitions are at
3392–3432. Requests during summarization are observed separately at2851–2900.
That last case matters: internal model work is still model work, not a special
consumer path invisible to instrumentation.

Do not copy two concrete mistakes. At `agent/root.zig:2460–2482` omitted usage
can become text-token estimates before the reported usage event, conflating
reported and inferred quantities. At `observability.zig:2424`, the test
explicitly expects numeric metrics to become spans. Blackbird should retain
measurement kind and provenance instead of changing representation to whatever
the exporter happens to support. NullClaw's thread isolation test at2002 checks
different trace identities, but does not establish causal ordering across
workers. Its test named "interface dispatches correctly" at1869 checks names,
not actual event delivery. The useful direct oracles are typed field assertions,
fan-out delivery, and lifetime/concurrency cases, not those names or span counts.

### Kimi: lifecycle breadth, observational versus behavioral hooks

Snapshot `9ab1286b8fe4e6bcd116949a27ce5e0ac3389c82`,
[upstream](https://github.com/MoonshotAI/kimi-cli/tree/9ab1286b8fe4e6bcd116949a27ce5e0ac3389c82).

`quarantine/kimi-cli/src/kimi_cli/hooks/events.py` exposes session start/end,
prompt submit, pre-tool, post-tool, tool failure, stop/failure, subagent start/end,
pre/post-compaction, and notification payloads. `hooks/engine.py:63` indexes
subscriptions by event and retains background hook tasks until completion.
`soul/kimisoul.py:695,745,1062,1530,1632` contains real prompt, stop, failure, and
compaction hook sites. This inventory exposes important product boundaries that
the rejected Blackbird draft did not enumerate.

Kimi's hooks can block or alter behavior; Blackbird should not silently give an
instrumentation hook that control authority. Study the lifecycle locations and
registration/lifetime mechanism separately from those intervention semantics.
`tests/hooks/test_engine.py:86` directly injects telemetry failure and checks the
behavioral block result is preserved, demonstrating why the two responsibilities
must remain distinct.

The independent telemetry sink in `telemetry/sink.py:19` copies caller events,
adds runtime context, batches under a mutex, flushes periodically, and tries
disk fallback at exit. `tests/telemetry/test_telemetry.py:176–234` checks caller
nonmutation and disk fallback. This is an instructive ownership boundary, but
its clear-buffer-before-send and caught exceptions are not Blackbird's complete
audit or lossless local emission contract.

`tests/hooks/test_integration.py:139–171,272–317` exercises actual hook commands,
matching, compaction lifecycle hooks, and callback resolution.
`tests/telemetry/test_instrumentation.py` is unusually explicit that its tests
call `track()` themselves and do not verify production call sites. Many cases
there simulate a single call and then assert a single event. We must use a real
engine route to qualify a Blackbird hook; manually emitting a metric only tests
the emission API.

### Gemini CLI: broad vocabulary and central descriptors, duplication to avoid

Snapshot `c6bccb7ecbf6d8368d995455dd725ed34466faad`,
[upstream](https://github.com/google-gemini/gemini-cli/tree/c6bccb7ecbf6d8368d995455dd725ed34466faad).

`quarantine/gemini-cli/packages/core/src/telemetry/metrics.ts:34–129` enumerates
tool, API, file, token, compression, retry, routing, authentication-storage,
startup, memory, queue, UI render, and agent recovery surfaces. Typed descriptor
definitions follow. `loggers.ts:235,274,307,458,508,708,722` supplies separate
request, error, response, compaction, retry, agent end, and recovery sites.
This demonstrates the breadth of required inventory and a central place for
metric definitions with names, units, and dimensions.

Avoid copying its exported semantic shape mechanically. `metrics.ts:1511–1576`
emits custom and convention versions of the same underlying token/duration
measurements. Blackbird should emit one native measurement and let presentation
or external adapters translate it; BLACKBIRD.md rejects duplicate measurement
machinery of the same kind. At819 a current memory reading is a Histogram
"until ObservableGauge is available"; the convenient instrument is not a reason
to change the meaning of a sample. Fields such as performance score, baseline
comparison, and regression detection are not selected product facts merely
because this reference emits them.

`metrics.test.ts:786–975` asserts exact token attributes and milliseconds-to-
seconds conversion; later tests cover memory and queue instrumentation.
`memory-monitor.test.ts:153–190` specifically avoids retaining one session's
configuration in a process-global monitor. This illustrates the process-versus-
session attribution hazard. These mocked tests can qualify primitive mapping;
real Blackbird engine/worker/store tests must additionally qualify hookup.

### Docker Agent: injectable instrumentation and actual lifetime tests

Snapshot `83fca5b2577d5548c59bde40d5d0b067d532a71a`,
[upstream](https://github.com/docker/docker-agent/tree/83fca5b2577d5548c59bde40d5d0b067d532a71a).

`quarantine/docker-agent/pkg/runtime/telemetry.go:15` defines an injectable
interface for session start/end, error, every tool result, and per-call token
usage. `telemetry_test.go:139,213` runs the actual runtime with a recording
implementation and examines per-call and session observations. Learn this direct
oracle: instrumentation tests do not need a hosted collector, and they should
exercise the production boundary that is supposed to emit.

`pkg/runtime/event_sink.go:15` separates the producer interface from a channel.
The file has blocking, nonblocking, and bounded-blocking implementations with
different drop semantics. These do not constitute an acceptable Blackbird audit
contract. They do reveal why bus ownership and teardown are design questions,
not exporter details. `pkg/app/event_lifecycle_test.go:16,59,92,154,177` exercises
owner cancellation, subscriber cancellation, producers blocked on retired buses,
replacement startup tails, and cleanup drain. A disappearing UI subscriber must
not own the lifetime of the instrumentation system or cancel other consumers.

## Surface classes and proposed hook forms

Classification is by observation semantics, not by dashboard category. A
subsystem can expose more than one class.

| Class | Meaning | Proposed hook form | Instrumentation emission choices |
| --- | --- | --- | --- |
| Lifecycle / interval | An admitted operation, request, workflow, session attachment, or region begins and later ends or is recovered unresolved. | Typed begin/end with an opaque interval handle; phase and terminal knowledge are separate enums. End contains explicit start reference, measured endpoints, and outcome stage. | Count observations at a defined phase; duration/distribution samples from explicit endpoints; active-work changes. Never synthesize a successful end on exception or recovery. |
| State transition / publication | The owning component accepted, committed, activated, cancelled, or rejected a transition. | Typed before/after transition, revision and operation identity; distinguish proposed, staged, committed, activated, duplicate, and refused. Fire a committed transition only after authority is published. | Transition increments, changed-item counts, size differences, state residence samples when endpoints exist. A user-marked task state is not a correctness judgment. |
| Data flow / boundary | Data was prepared, sent locally, received, decoded, published, or presented at a specified layer. | Typed item/block receipt and boundary outcome with byte/item counts, sequence, encoding, origin, and audit source references. Preserve distinct request and response events. | Byte/item increments, block-size observations, first-byte/first-decoded-content marks, boundary duration samples. Transport chunks are not tokens or semantic messages. |
| Queue / handoff / rendezvous | Work was accepted into a queue, dequeued, delivered, withdrawn, rejected, awaited, joined, or handed off. | Typed transfer identity; sender and receiver occurrences; queue instance and owning epoch. Record every relevant branch, including duplicate and cancellation. | Queue depth snapshots, accepted/dequeued increments, residence/wait samples. Submission is not delivery; cancellation request is not cancellation acknowledgment. |
| Passive state observation | The owning component supplies a coherent current snapshot without doing work. | Registration-scoped observation callback over stable snapshot/atomic reads; collect one generation-consistent result. Include process/session/worker scope and sample provenance. | Gauges and cumulative readings with declared epochs. Collection requests cause instrumentation to produce observations; ops/evals do not emit the metric. |
| Programmable domain surface | A workflow or extension knows a phase/value that native code cannot infer. | Registered native/Lua instruments and explicit mark/region handles; retain descriptor/source generation. Optional subscriptions consume typed hook facts and produce declared measurements. | Application-authored counts, values, durations and distributions, with exact units and source semantics. No evaluator or score is inferred by this facility. |

Metric kinds need their own meaning: nonnegative increments, signed changes,
current-value samples, cumulative readings with reset epochs, and distributions
of individual observations. A boolean terminal enum remains event/outcome
context rather than an ambiguous number. Missing observations remain missing;
provider-reported and application-estimated quantities use different descriptors.
Selection of concrete names, dimensions, aggregation, cadence and histograms
belongs in the reviewed design, not in this inventory.

## Product/harness census

References are current owned source sites. Proposed occurrences are not a claim
that those hooks already exist. `L` means lifecycle, `S` state/publication,
`D` data flow, `Q` queue/handoff, `P` passive observation, `C` custom programmable.
Each row specifies its semantic owner and proposed measurement surface.

### Operator ingress, controls, and presentation

| Surface and actual site | Class / owner | Proposed occurrence and emission semantics |
| --- | --- | --- |
| Native launch and session open: `src/main.cpp:60,211–363` | L/S; session opener | Open requested; native directory acquired; full/compact recovery selected; live session ready; open failed by stage. Session history identity and process attachment identity are distinct. Duration does not include previous process lifetime. |
| CLI/plain/station/TUI source selection: `src/main.cpp:173–209,903–1005` | S/D; ingress adapter | Interface selected, input accepted/refused, complete prompt handed to dispatcher. A prompt occurrence carries source kind and audit reference, not prompt text as a metric label. |
| Composer submit/cancel/quit: `src/terminal.cpp:1012,2110–2123` | D/S; UI owner | Submitted prompt distinct from edited draft; cancellation/quit request distinct from engine stop observation. UI draft recovery does not count as submission. Keyboard activity is not inferred operator attention. |
| Pending prompt queue: `src/terminal.cpp:1662–1679,2240–2258` | Q/P; UI queue | Queue accept, queue refusal retaining draft, dequeue, cancellation withdrawal, recovered draft load. Queue residence ends at actual dispatch; replayed drafts remain drafts until explicit submit. |
| UI state persistence: `src/terminal.cpp:1298–1324` | L/S; terminal state writer | Persistence requested/succeeded/failed, dispatch delayed by failed save. Separate presentation-state durability from audit/session durability. |
| Operator command classification and dispatch: `src/main.cpp:507–818`; `src/coding.cpp:2487,2505` | D/L; dispatcher / engine | Built-in, registered alias/bare/powerword, ordinary prompt, or unmatched command; selected workflow identity and invocation origin. The command occurrence is counted once at ingress; inner native calls remain separate operations. |
| Model/effort/workflow/session settings: `src/main.cpp:673–816`; `src/session.cpp:194` | S; settings owner | Change requested/refused/persisted/effective. Distinguish launch override, durable session default, governing program override and request-local override. |
| UI message post/drain: `src/terminal.cpp:1140,1552` | Q/D; UI message queue | Presentation message enqueued and actually drained, with type and size. This is not provider execution or durable audit capture. |
| Layout, paint and terminal transport: `src/chat_view.cpp:383,489,510,516`; `src/terminal.cpp:1695` | L/D/P; renderer / output transport | Frame construction, packet queued, bytes written, EAGAIN, terminal resize, frame transport completion. "Written to terminal" is the observation; do not call it "seen by operator". |
| PTY keyboard routing: `src/terminal.cpp:1351–1408,1630–1654` | D/Q; command/UI handoff | Input assigned to the actual foreground job/epoch, refused excess, flush accepted/failed, foreground routing changed. Child input and composer input are distinct sources. Command owner qualifies write/signaling semantics. |
| Presentation history trimming: `src/chat_view.cpp:207`; `src/terminal.cpp:609` | S/P; presentation store | Presented/resident rows or bytes, trim transitions and work. UI trimming is not audit deletion or context compaction. |

### Sessions, turns, definitions and execution

| Surface and actual site | Class / owner | Proposed occurrence and emission semantics |
| --- | --- | --- |
| Session identity/settings/reopen: `src/session.cpp:182–205`; `src/main.cpp:218–363` | L/S/P; session owner | Session attachment and detach; settings revision; recovery mode/result; readiness and unresolved state. Never issue new "operations completed" measurements while reconstructing prior history. |
| Turn lifecycle: `src/coding.cpp:2525–2688` | L/S; CodingEngine | Turn attempted, refused if busy/backstop claimed, source pinned, execution began, successful settlement, interrupted/error settlement. Result stage and pending activation disposition remain explicit. |
| Effective program/source generation: `src/coding.cpp:2584–2587` | S/D; program custodian | Source retained/pinned for this turn; bytes and generation; read/retain errors. Source retention is not execution duration. |
| Lua VM creation/loading/evaluation/teardown: `src/coding.cpp:289,547–603` | L/D/P; Runtime | VM created or allocation failed; source validated versus executed; compile versus call failure; bridge invocation category; teardown. Disable costly instruction-level probes independently of product lifecycle surfaces. |
| Lua-to-native bridge: `src/coding.cpp:421–542` | D/L; bridge and target semantic owner | Argument decoding/validation, host call transfer, result encoding, cancellation/error propagation. Store semantic events come from stores, avoiding missed direct APIs and duplicate native-call counts. |
| Native operation admission/dispatch/outcome: `src/coding.cpp:1289–1638` | L/S; native effect boundary | Requested, preadmission refusal, retained admission, dispatch opened, outcome observed, result retained, terminal publication. Treat dispatch and audit retention failures independently; do not only hook the successful return. |
| Workflow selection and deferral: `src/coding.cpp:2083–2113`; `src/workflows.cpp:124` | S/Q; registry / continuation queue | Selection pinned to definition revision; immediate or deferred execution; deferred queue accept/refusal, dequeue or cancellation. Selection completion does not mean workflow execution completed. |
| Callable workflow execution: `src/coding.cpp:1640–1694` | L; workflow executor | Actual invocation and generation, recursion/invocation refusal, child runtime begin/end, continuation result publication. Explicit selection and execution identities link without timestamp guessing. |
| Program/module registry updates: `src/coding.cpp:728–816` | S/D; configuration owner / module loader | Candidate validated, refused, staged, activated or cancelled; module source selected/retained, imported or cached. Module cache hit is not a fresh read/execution. |
| Lua tool definitions/invocation: `src/coding.cpp:834–879,2117–2123` | S/L; tool registry / runtime | Definition schema validation, conflict, staging, activation, source pinning, invocation, completion/failure. Definition name/revision belongs to event identity; external metric dimensions are an explicit choice. |
| Default loop semantic phases: `programs/turn.lua:2–24` | C; program author | Optional iteration, tool-output append, message-presented and loop-budget marks. The native harness cannot assume every turn program uses this loop or a 64-step budget. Custom definitions must state their meaning. |
| Restart scheduling and handoff: `src/coding.cpp:2125`; `src/session.cpp:206–249`; `src/main.cpp:827,1148` | S/Q/L; engine / session / launcher | Requested, custody validation refused, scheduled, durable pending token, process handoff attempted, reopen token consumed, continuation admitted. Scheduling is not restart success; token injection is not resumed useful work. |

### Context, managed transformations and tasks

| Surface and actual site | Class / owner | Proposed occurrence and emission semantics |
| --- | --- | --- |
| Context append/edit/restore: `src/context.cpp:578,628,670` | S/D; ContextStore | Proposed basis and actual observed revision; retained candidate/outcome; exact commit and resulting revision; rejected stale/invalid candidate. Measure changes from actual published presentation, not proposed entries. |
| Managed select/archive/summarize/restore: `src/context.cpp:881,970` | S/L; managed context owner | Requested mode and selected source identities; validation result; staged proposal; settlement publication or cancellation/conflict. "Staged" must have its own outcome despite today's `accepted=false` representation. |
| Working presentation/original/history inspection: `src/context.cpp:408,427,689,1095` | L/D/P; context read owner | Read requested/completed/failed by query mode, rows and bytes actually returned, source ranges; cache/cold retrieval is a lower-level measurement. Inspection itself does not prove repair or comprehension. |
| Context budget policy and advisory trigger: `src/coding.cpp:881–935,1811–1839` | S/P; policy owner / request assembler | Policy proposed/staged/activated/cancelled; request basis and byte observation; advisory threshold crossed/cleared. Native packet bytes are not token counts, provider limits, or physical audit bytes reclaimed. |
| Protocol validation and stop placeholders: `src/coding.cpp:243,2167–2202,2669`; `src/context.cpp:786` | S/D; protocol owner | Valid/refused item linkage; repair request validation and placeholder publication; number and source identities of repaired open call slots. Placeholder reports unknown effect and no replay, not successful tool execution. |
| Context checkpoint and compact reopen: `src/context.cpp:363,400`; `src/main.cpp:235–260` | L/S/D; context/recovery owner | Checkpoint initiated/published/refused; snapshot size and captured cursor; fast restore/full replay selection and actual recovery work. Logical context compaction and physical checkpointing need separate definitions. |
| Task edit batch: `src/tasks.cpp:582–620`; `TaskState::prepare`, `src/tasks.cpp:112` | S; TaskStore | Request parsed/validated, identical retry, conflict, or retained commit; exact changed/removed IDs and before/after state, batch size and revisions. Repeated receipt is not a second task transition. |
| Task state reads/projection: `src/tasks.cpp:504`; `include/blackbird/tasks.hpp:70` | P/D; TaskState | Current explicit queued/active/blocked/done/dropped counts, read result sizes, version conflicts. A task's `done` state is operator/model authored, not an evaluator-certified success. |
| Task activity binding: `src/coding.cpp:1292,1358–1382,2026`; `src/terminal.cpp:1203` | L/S; execution owner / task association | Actual attempt bound to task and observed activity updates. Execution, badge delivery and task state are distinct; no command exit automatically completes a task. |

### Provider, colleague, participant, and auth boundaries

| Surface and actual site | Class / owner | Proposed occurrence and emission semantics |
| --- | --- | --- |
| Main request assembly: `src/coding.cpp:1696–1854` | L/D; request assembler | Context/tool/config revisions selected; options/retry policy validated; native request prepared, byte/item sizes; refusal before request dispatch. Requested model/effort are request facts. |
| Request group / retry policy: `src/coding.cpp:1855–1970` | S/Q/L; requester | Group created, ordinal attempt selected; observed failure; retry selected/refused/exhausted; wait began/ended/interrupted; next request admitted. Requested delay differs from measured wait. |
| Provider authorization/binding/refresh: `src/openai.cpp:123`; `src/provider_auth.cpp:476,721–814` | L/S; auth owner | Binding selected/changed/conflicted; credential resolution begun/failed; refresh attempted/succeeded/refused/unknown; transport admission. Emit provider/binding revision and status, never credential values or headers. |
| Provider outgoing request and transport: `src/openai.cpp:182–253`; `src/coding.cpp:1876` | D/L; adapter / transport | Logical request prepared, locally sent, transport progress/failure, HTTP outcome. Local send success does not establish remote execution. Adapter stages and main operation lifecycle are different intervals. |
| Provider raw stream / decode / preview: `src/coding.cpp:1894–1930`; `src/openai.cpp:255,297`; `src/stream_capture.hpp` | D/L; transport / parser / presentation | Raw byte blocks received; stream first byte/end; parse event/decoded text/tool item; preview posted. First raw byte, first model content and first terminal write are distinct marks, and buffering boundaries must not change totals. |
| Provider response receipt and acceptance: `src/coding.cpp:1931,1971–1999` | D/S; importer / context owner | Decoded response observed incl provider-reported model/usage; import accepted/refused; usage validity/provenance; context publish accepted/conflicted. Usage must remain instrumentable before later processing can throw. |
| Direct colleague preparation/admission: `src/colleague.cpp:148,267` | L/D; colleague adapter | Selected context/profile validated; request prepared/refused; exact request admission retained; transport dispatched. Context-only restrictions and one request are current implementation, not all future colleague semantics. |
| Direct colleague upstream response and wrapper result: `src/colleague.cpp:290–360,364` | D/S; adapter | Raw/decoded upstream response received; reported model/usage observed; refusal/reported failure/CLI exit/corrupt parse/unknown disposition; wrapper result published. Upstream response and normalized reply are different boundaries. |
| Participant start/configure/launch: `src/participants.cpp:316,329–390` | L/S/Q; owner / worker launcher | Configuration change; start validated, initial request retained, run registered, thread launched/failed, first request dispatched. Run identity, per-request identity and owner operation identity remain separate. |
| Participant request execution and worker state: `src/participants.cpp:104–219` | L/S/D; worker | Actual request begin/response/end; running→waiting/failed/cancelled/unknown; publication created; worker finished. Worker observation time is not owner drain time. |
| Participant capture and owner drain: `src/participants.cpp:114–125,263–314` | Q/D; capture custodian / owner | Original capture receipt, durable acceptance, queue transfer, retention success/failure and blocked progress. Original bytes require complete custody; a queue overflow counter would not repair an omitted original. |
| Direction submit/deduplicate/queue/deliver: `src/participants.cpp:425–488,169–197` | Q/S; owner / worker inbox | Message accepted, identical duplicate, conflict/busy/capacity refusal; request prepared/admitted; enqueue; dequeue at request boundary; actual dispatch; cancellation withdrawal. Accepted is not delivered and receipt deduplication is not two submissions. |
| Participant await/join/cancel/archive/shutdown: `src/participants.cpp:490–619` | L/Q/S/P; owner and worker | Cancellation requested/observed; wait/join begin/end/interrupted; seal request and settled state; archive eligibility/result; shutdown requests, worker completion, owner capture drain. Lifetime belongs to run/system, not UI subscription. |
| Auth account management / sign-in: `src/provider_auth.cpp:494,504,515,552,581,610,655`; `src/provider_oauth.cpp` | S/L; auth store / login flow | Account/key change acknowledged, active selection, logout local/revocation stage, provider descriptor change, device begin/poll, pending/slowdown/expiry/refusal/success. No secrets, authorization codes, or identifying account names as default metric labels. |

### Tool adapters, evolution, station, and recovery

| Surface and actual site | Class / owner | Proposed occurrence and emission semantics |
| --- | --- | --- |
| File read/range/presentation: `src/tools.cpp:179–188` | L/D; LocalTools / file adapter | Read requested, native bytes observed, source retained, range presented, failure by stage. Returned-range bytes and full-source bytes are different quantities. |
| File write/edit: `src/tools.cpp:189–217` | L/S/D; file effect adapter | Before observed, edit match validated/refused, proposed bytes retained, write attempted, local publication observed, final outcome. An edit conflict is not an attempted write; write syscall/rename success differs from useful product result. |
| Command start/control/output/final disposition: `src/coding.cpp:2005–2052,1006–1127`; `src/command_jobs.cpp` | L/Q/D/S; command-job owner | Start admitted/launched; foreground wait acquired/released; control/input/resize/signal/stop requested and actual outcome; raw output received/retained; process terminal. Foreground wait end does not imply process exit. Detailed OS/process census belongs in system companion. |
| Retained command output retrieval: `src/coding.cpp:2129–2166` | L/D; output reader | Retrieval requested/invalid/completed; bytes scanned/read/returned, source identity and range. Retrieval must not become a fresh command execution count. |
| Beads read/write adapter: `src/beads.cpp:35,91`; `src/coding.cpp:2064` | L/D/S; adapter / child transport | Request validation, process spawn, stdout/stderr receipt, parsed reply, mutation-possible marker, known/refused/unknown outcome. Read unavailable and potentially dispatched write unknown are distinct. |
| Decision-model request/response: `src/decision_models.cpp:171,237,305`; `src/coding.cpp:2054` | L/D; adapter | Typed application decision requested/validated, request prepared, dispatch/send, response receipt/validation, reported usage, completed/refused/unknown. Current observer records request after `child.send` (line333): a proper lifecycle surface must expose preparation and send separately. This product call is not an eval consuming metrics. |
| Audit/context/trajectory/variable reads: `src/coding.cpp:2279–2294`; `src/audit.cpp` | L/D; read implementation | Query shape/mode, page/cursor/pin, rows/bytes actual return and errors. Read-side observation must not recursively instrument its own instrumentation reads or treat result counts as work success. |
| Read-only Git observation: `src/coding.cpp:2297`; `src/observations.cpp` | L/D; observation adapter | Observation requested, subprocess/read behavior, native result emitted and original locators. Observing a commit does not establish that Blackbird caused it or qualified it. |
| Complaint local record and advisory delivery: `src/coding.cpp:2213–2278` | S/Q/D; complaint / delivery owner | Complaint retained locally, delivery attempted and wrapper outcome, bead creation observed. Delivery success/failure is independent of complaint existence; counting complaints is not assigning quality. |
| Source candidate/checkpoint/run/activation: `src/candidate.cpp:198,255–268,372–432,479` | S/L/Q; candidate owner | Source archived, lease assigned/queued, exact checkpoint pinned, run admitted/observed, source mismatch or qualification/activation transition. Experiment execution exposes ordinary instrumentation; consumers decide its significance. |
| Station control and event intake: `src/station.cpp:65,83,91,105`; `src/main.cpp:907–973` | S/Q/L; StationStore / input adapter | Event read/validated, duplicate or paused refusal, admitted, workflow dispatched, observation and pause; pause/resume/stop/steer controls accepted/duplicate/effective. Feed receipt is distinct from context inclusion and action. |
| Explicit session recovery: `src/coding.cpp:1130–1266`; `src/main.cpp:260` | S/L; recovery owner | Recovery selected, unresolved identities validated, supported/unsupported/refused, terminal unknown acknowledgment committed, uncontained command fence retained. No new dispatch or completion measurement for an old unresolved effect. |
| Backstop lineage/assessment/pivot/action: `src/backstop.cpp:57–288` | L/S/Q; backstop owner and ordinary child engine | Claim attempted/refused, successor intent, selected source lineage, assessment request, pivot selected, action execution, artifact observation, paused/no-progress/deadline/bound. Preserve underlying observations separately; "useful-work-observed" is this implementation's explicit artifact predicate, not a universal evaluation score. |
| Successor seeding / session switch: `src/context.cpp:452,558`; `src/main.cpp:698–728` | S/Q; lineage and launch owners | Seed validation/publication/refusal; custody validation; session destination chosen, launch handoff attempted/failed. A new session is not proof the old one's effects settled. |

### Prospective surfaces with no implemented owner yet

Multiplayer, remote tool-using outposts, general feed connectors, live station UI
attachment, full interrupt-and-apply-now hot activation, arbitrary concurrent
main turn scheduling, external complaint sinks, and native hot plugin reload are
not silently claimed by the current inventory. Each future owner should expose
the same appropriate classes: ingress and delivery, owner lifecycle, state
publication, queue handoff, request/response, passive current state and custom
domain phases. For distributed delivery, hook events must carry the system's
reviewed logical-clock/causal identities; a thread-local parent stack alone is
not a distributed data model. Shared services remain owned by their own control
planes: Blackbird instruments its client work and any reported service facts,
not invented internal server state.

## Proposed attachment and emission design

The hook mechanism should consist of owned typed probes and registration scopes.
A conceptual flow is:

```
real semantic owner
  -> typed occurrence / coherent observed state
  -> registered instrumentation producer
  -> declared measurement with source identity and unit
  -> durable native emission stream and optional external adapters
  -> operations and evals consume
```

Audit is authoritative custody of what actually occurs, including measurement
emission and instrumentation failures. It does not become a filtered metric
store. Nor should adding instrumentation copy every large original into another
payload. The measurement can reference retained originals and source event
identity; raw evidence remains in its authoritative custody. Complete custody,
logical clock propagation and source event typing need the system-wide design,
not ad hoc `parent_attempt` and string metadata.

Proposed native hook payloads have concrete per-class types. For example,
`RequestPrepared` carries request identity, configuration/context revisions and
byte/item counts; `ResponseReceived` carries response identity, request link,
transport/adapter provenance and reported fields; `ContextPublished` carries
old/new revision and actual before/after presentation sizes. They share a
provenance header whose exact causal/logical clock model is supplied by the
system design. They do not all become `Value{ {"kind", ...} }` dictionaries.
Native `Value` remains suitable for explicitly extensible/custom details, with
a declared schema and native packet encoding, never JSON internal IPC.

Instrumentation producers register the hook class/version they understand and
declare instruments once: semantic name, unit, measurement kind, scalar type,
dimension schema, origin/provenance and code generation. Core production sites
pass data already known by their owner; expensive materialization happens only
for enabled producers, without disabling required audit. One native emission
serves different consumers. Translation to an external format is an adapter,
not a second independently counted metric.

Registration must have clear ownership. Process instrumentation outlives a UI
subscriber; a session producer does not leak into a different session; a worker
producer finishes/drains with its run. Worker-facing payloads are immutable and
owned long enough for use, not borrowed `Value&` or stack pointers crossing an
asynchronous handoff. Subscription removal and generation changes cannot free
state still used by another thread. No arbitrary callback should run under a
component's mutation mutex or execute nested effects implicitly.

The core hook observer is observational. Behavioral interception, such as
effect policy or a workflow choosing a different tool, remains an explicit
product control mechanism. It must not gain or lose authority because a metric
handler succeeds, fails, or is absent. A programmer-defined instrumentation
producer may be configurable Lua, but its lifecycle and capabilities must be
designed explicitly rather than copying Kimi's allow/block hooks.

Passive collection reads a component-owned coherent snapshot. It must not
dispatch provider requests, run work, mutate task state, scan the entire audit,
or attribute a process-global gauge to the last session that happened to call
it. A collection requester can ask for observations; the instrumentation
callback remains the emitter. Snapshot schema and reset/epoch semantics make
clear whether a value is instantaneous, lifetime cumulative, or session-local.

Programmable regions use opaque handles with source generation and context.
The end occurrence identifies its actual begin handle. Recursion, repeated
names, nested native calls, exceptions, and cancellation do not accidentally
pair with "the most recent mark named X". A programmer can explicitly emit a
value or define a phase; the facility does not infer useful work, task quality,
success criteria, or an evaluator from that value. Configuration adoption and
activation should follow Blackbird's existing retained, successful-boundary
model, subject to discussion rather than imposed here.

### Failure, delivery, and cardinality questions

The design must not inherit best-effort callback swallowing as its emission
contract. Proposed enabled metric observations are durably accepted locally
before external delivery; a slow or disconnected consumer does not erase them.
If capacity prevents acceptance, behavior must be explicit at the emitting
boundary and preserve required audit—not quietly drop and advertise a loss
counter. Exact backpressure/reserve and continuation rules require discussion
with the system capture design. This report does not claim that arbitrary
amounts of received data can fit a fixed journal or that an error record can
repair missing evidence.

Disabled instrumentation has a recorded configuration; it does not emit fake
zeroes. Producer errors and malformed custom values have explicit dispositions.
No emission retry may redispatch the original provider/tool effect. Export
retries use measurement identities and sink acknowledgment positions to avoid
counting the same source measurement twice.

Keep entity identifiers and arbitrary text out of unconstrained metric label
sets. Task/run/request/session/source identities can remain native provenance
for per-entity consumption without forcing every aggregate to have those
dimensions. The consumer/export view may deliberately group or project
dimensions; that choice must not remove complete audit originals or silently
change an individual measurement's meaning. The old patch's string and packet
limits are not adopted as the metric model.

## Direct validation design after approval

These are proposed direct oracles, not executed tests and not a request to start
implementation. Each exercises a real production route and inspects the emitted
measurement stream with a recording adapter; it does not test a manually issued
`record()` call and pretend the engine is instrumented.

1. **Owner placement and route equivalence:** drive context edit/manage/restore
   through direct Lua bridge, model native call and operator command. Compare
   exactly one semantic store observation per actual transition, plus distinct
   ingress/bridge observations. Existing context, Lua and workflow fixtures are
   suitable engines, but the new oracle must inspect actual emissions.
2. **Phase separation:** return a provider response with reported model/usage,
   then force import or context revision conflict. Require response receipt and
   its measurement; reject fabricated accepted/published occurrences. Also
   exercise predispatch refusal, partial stream, HTTP failure, retry exhaustion
   and interrupted retry wait.
3. **Buffering metamorphism:** deliver identical raw provider/command bytes in
   different chunk partitions. Byte totals and semantic response observations
   must agree; block counts may differ by definition. Preview timing marks
   identify the layer actually observed, not a synthetic final duration.
4. **Staging/commit semantics:** a managed proposal staged during a workflow is
   not rejection or activation. Complete, interrupt and fail that workflow and
   assert exact publication/cancellation observations and state. Existing
   `tests/workflows_test.cpp:226–371` and context fixtures supply real paths.
5. **Deduplication versus transitions:** `tests/tasks_test.cpp:62–118` exercises
   actual edits/retries/conflicts; require attempted edit observations for each
   input and only one committed transition for the identical retry. Different
   source phases may have different count semantics, all explicit.
6. **Worker and inbox concurrency:** use counted fake transports and explicit
   barriers to order direction acceptance, dequeue, dispatch, cancellation and
   join. Assert exact request/response counts and source identities, not sleeps
   or final-state-only checks. `tests/participants_test.cpp:103–148` already
   supplies meaningful dedup, cancellation and exact transport-count oracles.
7. **Registration/lifetime:** remove a UI consumer while a worker emits; switch
   session and generation; retire producer registrations during pending work;
   close and reopen the local emission stream. No freed state, cross-session
   attribution, lost enabled observation or redispatch. Docker Agent's real
   lifecycle tests identify the cancellation fault classes to attack.
8. **Custom region semantics:** nested same-name regions, recursive invocation,
   missing explicit end, exception, cancellation, generation update and decoded
   invalid custom measurement. Use actual Lua host APIs and exact handle/link
   assertions; no inferred completed duration for an unresolved region.
9. **UI truthfulness:** submit while busy, overflow queue, cancel queued prompts,
   recover drafts, fail state persistence, force output EAGAIN and later flush.
   Assert attempted/queued/dispatched/drained/written occurrences at their own
   boundaries. Composer-only tests do not qualify full worker/output hookup.
10. **Recovery truthfulness:** crash/open unresolved work, acknowledge unknowns,
    consume restart token, run a backstop pivot. Observe recovery decisions and
    new work without fresh success/count emissions for replayed old effects.
    Existing session/backstop crash fixtures provide actual native state.
11. **Measurement integrity:** exact numeric representation and units; missing
    versus explicit zero; reported versus estimated provenance; reset epochs;
    immutable payload ownership; selected dimensions; adapter unit conversion.
12. **Custody under emission pressure:** inject refusal and uncertain persistence
    at actual boundaries, exhaust configured emission headroom, disconnect a
    consumer, and restart. The oracle is exact durable measurement/source data
    and unchanged effect dispatch count. A "dropped=1" assertion is inadequate.

The full implementation plan must state its actual fault classes and bounded
hardening allowance under BLACKBIRD.md, then use relevant static/dynamic checks.
Metric API microtests can qualify primitive kind/schema behavior, but they cannot
replace these call-site and lifecycle oracles. A sanitizer finds memory faults;
an exact source-event/count oracle finds missing or duplicated instrumentation.

## Decisions for discussion

The immediate proposal is the six surface classes, semantic-owner placement,
typed observational hooks, generation-scoped programmable producers, coherent
passive collection, and one owned native measurement emission interface.

Still to settle with the operator: exact common event/measurement model and
logical-clock integration; registry/schema evolution; custom Lua producer
capabilities and activation; durable emission acceptance/backpressure contract;
scope of raw per-occurrence retention versus derived aggregates; export/read
interfaces; enabled defaults and sampling policy; which concrete metrics are
worth emitting. An ops dashboard, evaluator, product score, alert policy,
comparison campaign and trajectory UI remain consumers or later choices. This
study selects none of them.
