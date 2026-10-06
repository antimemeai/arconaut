# agent-orchestrator-rust

A substantial Rust workflow daemon manages native agent phases, parallel items, fenced terminal takeover and logical recovery. Its implemented self-build/exec mechanism is useful evidence, but drain and semantic-completion limits prevent a seamless-refit claim.

Role: Rust external-agent workflow control plane. Runtime: Rust, TypeScript (desktop UI outside execution trace).

Pinned source: [https://github.com/c9r-io/orchestrator](https://github.com/c9r-io/orchestrator); revision/version `c4098a7d14d58bfead858fd2c08feec43db10646`.

Tokio daemon/gRPC/UDS workers, SQLite records and managed external process-group drivers

owns workflow/scheduler/security/session/audit fabric; external CLI agents own providers and inner tool turns

Inspection: Selected source excerpts trace exposed entry/loop/request/action/result/state boundaries and relevant capability mechanisms; listed tests are read as source only.

Limits of this study: No acquired code executed, dependencies installed or live providers invoked. Claims concern this pinned snapshot; untraced catalog tools and dependency internals are not inferred.

## Actions

### orchestrator apply / Workflow resource

Surface: operator / agent CLI.

Input: YAML Agent/Workflow/StepTemplate/ExecutionProfile and related manifests; dry run/project

Result: validation errors/warnings, resource config revision

Lifecycle: validated config merge; live activation semantics vary by consumer

Authority: authorized daemon client; models may invoke CLI as external agents

Evidence: [workflow](#evidence-workflow), [apply](#evidence-apply).

### task create / start / pause / recover / retry / get

Surface: operator / agent CLI / gRPC.

Input: goal, workflow, task/item ID, retry state

Result: task/item/run IDs and persisted summaries

Lifecycle: scheduler owns queued/running status; bounded parallel item segments; pause kills process

Authority: daemon authorizes client; external agent may invoke orchestrator CLI

Evidence: [phase](#evidence-phase), [parallel](#evidence-parallel), [pause](#evidence-pause).

### shell/cli / claude/cli / codex/cli start

Surface: supplied driver API.

Input: prompt, model, session ref, timeout/profile, environment

Result: DriverSession event stream, PID and opaque session ref

Lifecycle: external child provider/tools; stream single consumer; semantic terminal can precede exit

Authority: configured driver/native tool policy and runner profile

Evidence: [drivers](#evidence-drivers), [args](#evidence-args), [spawn](#evidence-spawn), [session](#evidence-session), [result](#evidence-result).

### DriverSession.send / cancel

Surface: programmable API.

Input: user message/tool result/permission response or cancel

Result: stdin-write completion or cancellation signal

Lifecycle: Claude live input only; cancel returns before child reaping

Authority: driver host; native model processing receipt not exposed

Evidence: [input](#evidence-input), [wait](#evidence-wait).

### AgentSessionAttach / SendInput / read / detach / close

Surface: operator gRPC.

Input: session/client ID, lease/fencing token, 1–4096 input bytes and idempotency key

Result: accepted bytes, stream/session state

Lifecycle: writer lease and process identity; accepted means FIFO write, not model read

Authority: authorized writer/controller; supports terminal takeover

Evidence: [takeover](#evidence-takeover), [accept](#evidence-accept).

### run_tests

Surface: model MCP tool.

Input: target workspace|core|runner|scheduler

Result: exit code and stdout/stderr evidence paths

Lifecycle: awaited allowlisted test child with deadline/capture

Authority: authenticated run token and allowed tool set; governed runner profile

Evidence: [coord](#evidence-coord), [testtool](#evidence-testtool), [schemas](#evidence-schemas).

### mark_item / mark_done / generate_items / record_metric

Surface: model MCP tools.

Input: status+summary; bounded item IDs/vars/replace; bounded numeric name/value

Result: updated item status, generated count, metric record

Lifecycle: daemon-owned task effects; structured errors/results

Authority: allowed current task/item run context

Evidence: [schemas](#evidence-schemas), [ticket](#evidence-ticket), [coord](#evidence-coord).

### create_ticket / scan_tickets

Surface: model MCP tools.

Input: current item; preceding failing run_tests

Result: deduplicated ticket path or active ticket list

Lifecycle: requires failing evidence; tracks QA issue outside immediate repair

Authority: authenticated allowed run; narrower than free-form rageshake

Evidence: [ticket](#evidence-ticket), [schemas](#evidence-schemas).

### handoff.generate / resume.plan / resume.execute

Surface: operator gRPC.

Input: task/boundary/mode, actor, state version and approval context

Result: immutable snapshot, expiring consequence plan, correlated logical resume

Lifecycle: retry/restart/native-session continuation; no filesystem rollback

Authority: authorized controller with elevated confirmation for replay-risk class

Evidence: [handoff](#evidence-handoff).

### self_test / self_restart builtin

Surface: workflow step.

Input: self workspace and current task/item

Result: build/verification result, hashes/stable snapshot, restart_pending

Lifecycle: build then deferred task drain and same-PID daemon exec; drain failures do not all fence exec

Authority: workflow author selects builtin; mechanism supports modifying own harness

Evidence: [build](#evidence-build), [ready](#evidence-ready), [defer](#evidence-defer), [drain](#evidence-drain), [exec](#evidence-exec).

### task timeline / trace / logs

Surface: operator / agent query API.

Input: task ID, cursor/query/verbose filters

Result: sanitized persisted event/run projections and output

Lifecycle: cursor-based projection; retention may prune terminal events

Authority: authorized read client; native provider internals not fully captured

Evidence: [query](#evidence-query), [trace](#evidence-trace), [capture](#evidence-capture), [prune](#evidence-prune).

## Capabilities

### filesystem

**I — Files** (source): Runner external shell/native-agent processes act on configured workspace; compiler/test/file output paths exposed. Inner native editor/tool implementation belongs to separate agents.

Evidence: [spawn](#evidence-spawn), [testtool](#evidence-testtool), [build](#evidence-build).

### processes

**I — OS programs** (source): Rust daemon spawns managed process groups with profiles/environment policy and sanitized streams; cancellation owns group kill, but API result/cancel is not necessarily reaped completion.

Evidence: [spawn](#evidence-spawn), [group](#evidence-group), [session](#evidence-session), [wait](#evidence-wait).

### code-actions

**S — Code actions** (source): Workflow shell commands, CRD command hooks and external model agent tools can execute authored programs; orchestrator itself supplies named coordination tools rather than code kernel.

Evidence: [apply](#evidence-apply), [drivers](#evidence-drivers), [coord](#evidence-coord).

### persistent-kernel

**— — Kernel** (inspection-limit): Control plane launches/manages external agent/command processes; no native computation notebook/kernel role in inspected driver/workflow path.

### standing-database

**S — Standing DB** (source): Embedded SQLite stores task/event/config/session state and queries; model coordination tools expose narrow task data, not arbitrary standing database computation.

Evidence: [apply](#evidence-apply), [takeover](#evidence-takeover), [query](#evidence-query), [trace](#evidence-trace).

### workflow-programming

**I — Workflows** (source): YAML step/loop/capability resources, prompt templates and bounded parallel item segments run external agents; model tools generate task items and metrics. Separate DAG struct is not assumed to be live scheduler.

Evidence: [workflow](#evidence-workflow), [phase](#evidence-phase), [parallel](#evidence-parallel), [schemas](#evidence-schemas).

### multi-model

**I — Models** (source): Different native/shell drivers and per-driver model arguments allow heterogeneous workflow phases/parallel items.

Evidence: [drivers](#evidence-drivers), [args](#evidence-args), [parallel](#evidence-parallel).

### live-collaboration

**S — Peer chat** (source): Run-scoped tools manipulate shared task/item evidence and delegated workflow work; interactive FIFO steering exists. No peer room/mail/chat transport established in these traced tools/drivers.

Evidence: [coord](#evidence-coord), [schemas](#evidence-schemas), [parallel](#evidence-parallel), [takeover](#evidence-takeover).

### concurrent-work

**I — Concurrency** (source): Daemon workers and semaphore-bounded JoinSet execute task/items; collect-all segment errors and per-item state.

Evidence: [parallel](#evidence-parallel), [defer](#evidence-defer).

### steering-interrupt

**L — Steer/interrupt** (source): Task pause kills active children; terminal takeover uses fenced input lease. Driver cancel only signals background task and scheduler returns without reaped/captured end.

Evidence: [pause](#evidence-pause), [takeover](#evidence-takeover), [accept](#evidence-accept), [input](#evidence-input), [wait](#evidence-wait).

### turn-redefinition

**S — Turn program** (source): Model provider turn lives in external CLI; manifests choose workflow phases/templates and model coordination tools, not inner per-request turn program.

Evidence: [phase](#evidence-phase), [args](#evidence-args), [coord](#evidence-coord).

### compaction

**? — Compaction** (inspection-limit): No managed native context-compaction policy established in selected control-plane/driver/session/handoff trace; provider owns inner transcript.

### context-repair

**S — Repair** (source): Logical handoff/resume plans project persisted task state with explicit replay consequence and optional provider-session reuse; not editable authoritative provider context repair.

Evidence: [handoff](#evidence-handoff), [phaseend](#evidence-phaseend).

### original-audit

**L — Original audit** (source): Task/control/session records and sanitized stdout exist, but coordination logging can fail silently and omits arguments/results; streams redact originals and retention prunes events.

Evidence: [coord](#evidence-coord), [terminal](#evidence-terminal), [capture](#evidence-capture), [prune](#evidence-prune).

### audit-query

**I — Audit query** (source): Task timeline cursor and trace join persisted command runs/events with redaction/anomaly projections; coverage remains recorded control-plane scope.

Evidence: [query](#evidence-query), [trace](#evidence-trace).

### hot-change

**S — Hot change** (source): Manifest config apply changes persisted resources; current phase/parallel segment retain captured request/task context. Uniform affected-turn/workflow activation policy not established across resource consumers.

Evidence: [apply](#evidence-apply), [phase](#evidence-phase), [parallel](#evidence-parallel).

### rebuild-continuity

**L — Rebuild continuity** (source): Real self-build/verification/hash/pending-state→deferred other-task drain→same-PID exec is implemented. Native terminal events may precede OS exit, drain timeout does not block exec, and no outpost/session transport transfer.

Evidence: [build](#evidence-build), [ready](#evidence-ready), [defer](#evidence-defer), [exec](#evidence-exec), [result](#evidence-result), [wait](#evidence-wait).

### remote-services

**I — Remote** (source): gRPC/UDS client consumes daemon-owned scheduler/SQLite/security fabric; model providers are external native CLIs. Suitable as external-fabric reference, not Arconaut consumer core.

Evidence: [role](#evidence-role), [shim](#evidence-shim), [takeover](#evidence-takeover).

### self-improvement

**L — Self-improve** (source): Workflow can run own implement/test/rebuild cycles, model tools record metrics/generate work and create QA tickets. Build --help/snapshot do not establish model-governed experiment validity or safe executable refit.

Evidence: [schemas](#evidence-schemas), [build](#evidence-build), [ready](#evidence-ready), [exec](#evidence-exec).

### complaints

**S — Complaints** (source): create_ticket records deduplicated QA failure linked to stdout/stderr after run_tests; no unrestricted frustration/state snapshot complaint action in traced catalog.

Evidence: [ticket](#evidence-ticket), [schemas](#evidence-schemas).

### authority

**I — Authority** (source): Runner profiles/environment allowlist and self-PID guard; native permission mapping and callback token/run allowlist; terminal writer lease/fencing/idempotency. Configurable governed control-plane orientation differs from unrestricted operator intent.

Evidence: [spawn](#evidence-spawn), [args](#evidence-args), [coord](#evidence-coord), [takeover](#evidence-takeover).

### evaluation

**L — Evaluation** (source): Read real-child driver signal/exit tests distinguish OS kill from ordinary failure; restart readiness test uses fake cargo/fake help binary and asserts SQLite/stable file, not actual exec/session/drain continuity.

Evidence: [result](#evidence-result), [exec](#evidence-exec), [build](#evidence-build).

### time-order

**L — Time/order** (source): Persisted task/event cursors and fenced idempotent FIFO reservation improve control ordering; provider semantic Finished before capture/exit and write-before-audit external effect prevent completion/quiescence equivalence.

Evidence: [query](#evidence-query), [takeover](#evidence-takeover), [accept](#evidence-accept), [terminal](#evidence-terminal), [wait](#evidence-wait).

## Inspected test oracles

- [quarantine/agent-orchestrator-rust/crates/orchestrator-runner/src/driver/providers.rs](../../../quarantine/agent-orchestrator-rust/crates/orchestrator-runner/src/driver/providers.rs): Real child stdin and OS signal versus ordinary exit propagation Oracle: cat child exact output and kill-XCPU/exit-3 assertions challenge process→DriverEvent boundary; unexecuted source, not live providers. Read, **not executed**.
- [quarantine/agent-orchestrator-rust/crates/orchestrator-scheduler/src/scheduler/safety/tests.rs](../../../quarantine/agent-orchestrator-rust/crates/orchestrator-scheduler/src/scheduler/safety/tests.rs): Self-restart readiness and persisted pending state Oracle: Fake cargo exits zero and fake new binary serves help; asserts RestartReady, SQLite restart_pending and stable file. Does not perform real compiler/exec or prove concurrent worker quiescence. Read, **not executed**.

## Useful mechanisms

- Typed driver events and real OS signal boundary tests.
- Logical resume consequences explicitly distinguish replay from workspace rollback.
- Actual self-build/hash/pending-state/deferred exec path.

## Material limits

- Native protocol Finished can precede output persistence and process reaping; scheduler exits that boundary early.
- Cancel signals without await, and daemon supervisor drain timeout logs then exec continues.
- Sanitized control-plane logs are selective/retained, coordination audit failures ignored.
- Shared daemon owns orchestration fabric; peer live room and inner model loop belong elsewhere.

## Arconaut design questions

- What exact barrier makes no active provider/processes a precondition for binary switch?
- Can rebuild supervision transfer to an independent outpost while this daemon remains consumer-only?
- Should semantic answer, process exit, output persisted and effects committed be separate lifecycle events?

## Evidence

### Evidence role

[quarantine/agent-orchestrator-rust/README.md:9–21](../../../quarantine/agent-orchestrator-rust/README.md#L9): Control plane wraps external shell/native coding agents through gRPC/UDS daemon, workers and embedded SQLite; not an inner model tool loop.

### Evidence workflow

[quarantine/agent-orchestrator-rust/core/src/resource/workflow.rs:31–77](../../../quarantine/agent-orchestrator-rust/core/src/resource/workflow.rs#L31): YAML workflow resource validates steps/loop constraints and applies a resource representation; custom nonempty step IDs resolve as agent capabilities.

### Evidence apply

[quarantine/agent-orchestrator-rust/core/src/service/resource/mod.rs:41–124](../../../quarantine/agent-orchestrator-rust/core/src/service/resource/mod.rs#L41): Manifest apply parses/validates and merges current SQLite-backed config; invalid documents accumulate errors while valid documents are processed. Exact activation for every runtime policy is outside this trace.

### Evidence phase

[quarantine/agent-orchestrator-rust/crates/orchestrator-scheduler/src/scheduler/phase_runner/mod.rs:108–158](../../../quarantine/agent-orchestrator-rust/crates/orchestrator-scheduler/src/scheduler/phase_runner/mod.rs#L108): Phase run carries task/item/step/provider/session/execution-profile context and enters setup, then spawn/wait/validation/record stages.

### Evidence phaseend

[quarantine/agent-orchestrator-rust/crates/orchestrator-scheduler/src/scheduler/phase_runner/mod.rs:236–297](../../../quarantine/agent-orchestrator-rust/crates/orchestrator-scheduler/src/scheduler/phase_runner/mod.rs#L236): TTY phase may return early; ordinary phase waits driver result, drops coordination tool host, validates selected driver/output events and remembers native session reference.

### Evidence parallel

[quarantine/agent-orchestrator-rust/crates/orchestrator-scheduler/src/scheduler/loop_engine/segment.rs:334–438](../../../quarantine/agent-orchestrator-rust/crates/orchestrator-scheduler/src/scheduler/loop_engine/segment.rs#L334): Item segments spawn bounded JoinSet with semaphore, optional stagger and per-item finalization; collects all results, errors and completeness count.

### Evidence drivers

[quarantine/agent-orchestrator-rust/crates/orchestrator-runner/src/driver/providers.rs:17–108](../../../quarantine/agent-orchestrator-rust/crates/orchestrator-runner/src/driver/providers.rs#L17): Shell, Claude and Codex drivers launch external commands and ProcessSession; Claude takes structured input, shell closes initial stdin, Codex command owns prompt.

### Evidence args

[quarantine/agent-orchestrator-rust/crates/orchestrator-runner/src/driver/providers.rs:160–253](../../../quarantine/agent-orchestrator-rust/crates/orchestrator-runner/src/driver/providers.rs#L160): Native driver maps model/budget/turn/permission options and opaque resume references; Claude allows run-scoped MCP callback tools, raw args pass through.

### Evidence spawn

[quarantine/agent-orchestrator-rust/crates/orchestrator-runner/src/runner/spawn.rs:37–98](../../../quarantine/agent-orchestrator-rust/crates/orchestrator-runner/src/runner/spawn.rs#L37): Runner enforces profile/policy, creates process group, kill-on-drop, environment allowlist and self-daemon-kill guard before child spawn.

### Evidence group

[quarantine/agent-orchestrator-rust/crates/orchestrator-runner/src/runner/spawn.rs:181–204](../../../quarantine/agent-orchestrator-rust/crates/orchestrator-runner/src/runner/spawn.rs#L181): Explicit Unix cancellation SIGKILLs child process group; non-Unix falls back to direct child kill.

### Evidence session

[quarantine/agent-orchestrator-rust/crates/orchestrator-runner/src/driver/process.rs:28–158](../../../quarantine/agent-orchestrator-rust/crates/orchestrator-runner/src/driver/process.rs#L28): ProcessSession owns separate stdout/stderr capture and background wait task; cancellation triggers group kill and reaping; events are unbounded, native terminal event suppresses later process-status terminal.

### Evidence input

[quarantine/agent-orchestrator-rust/crates/orchestrator-runner/src/driver/process.rs:163–219](../../../quarantine/agent-orchestrator-rust/crates/orchestrator-runner/src/driver/process.rs#L163): Driver events consumed once; send writes/flushed stdin and only Claude accepts live input encoding; cancel merely signals background oneshot and returns.

### Evidence terminal

[quarantine/agent-orchestrator-rust/crates/orchestrator-runner/src/driver/process.rs:256–305](../../../quarantine/agent-orchestrator-rust/crates/orchestrator-runner/src/driver/process.rs#L256): Parsed provider events are emitted before sanitized line write; terminal flag can be set before process exit/output capture completes. Session identifiers are redacted in saved output.

### Evidence result

[quarantine/agent-orchestrator-rust/crates/orchestrator-runner/src/driver/process.rs:390–413](../../../quarantine/agent-orchestrator-rust/crates/orchestrator-runner/src/driver/process.rs#L390): Claude protocol result becomes Finished with semantic success/error and no OS signal, rather than a reaped-process observation.

### Evidence wait

[quarantine/agent-orchestrator-rust/crates/orchestrator-scheduler/src/scheduler/phase_runner/wait.rs:237–371](../../../quarantine/agent-orchestrator-rust/crates/orchestrator-scheduler/src/scheduler/phase_runner/wait.rs#L237): Scheduler treats Finished as end of phase; timeout/stall/pause calls cancel then returns without waiting for OS exit/capture reaping. Heartbeat uses output byte changes.

### Evidence pause

[quarantine/agent-orchestrator-rust/crates/orchestrator-scheduler/src/scheduler/runtime.rs:121–146](../../../quarantine/agent-orchestrator-rust/crates/orchestrator-scheduler/src/scheduler/runtime.rs#L121): Task pause sets stop flag, kills current/DB child then updates task state and inserts task_control; not cooperative standing-service suspension.

### Evidence coord

[quarantine/agent-orchestrator-rust/crates/orchestrator-scheduler/src/scheduler/coordination_tools.rs:174–280](../../../quarantine/agent-orchestrator-rust/crates/orchestrator-scheduler/src/scheduler/coordination_tools.rs#L174): Authenticated loopback run tools enforce per-run allowed set, execute effects and return structured result; started/completed event insertion errors are ignored, payloads omit tool arguments/results.

### Evidence testtool

[quarantine/agent-orchestrator-rust/crates/orchestrator-scheduler/src/scheduler/coordination_tools.rs:284–320](../../../quarantine/agent-orchestrator-rust/crates/orchestrator-scheduler/src/scheduler/coordination_tools.rs#L284): run_tests maps allowlisted target to command, persists sanitized output, applies timeout and waits capture on normal exit.

### Evidence ticket

[quarantine/agent-orchestrator-rust/crates/orchestrator-scheduler/src/scheduler/coordination_tools.rs:401–502](../../../quarantine/agent-orchestrator-rust/crates/orchestrator-scheduler/src/scheduler/coordination_tools.rs#L401): create_ticket requires last failing run_tests evidence; scan_tickets queries current item; generate_items validates bounded IDs then creates dynamic items and records event.

### Evidence schemas

[quarantine/agent-orchestrator-rust/crates/orchestrator-scheduler/src/scheduler/coordination_tools.rs:585–629](../../../quarantine/agent-orchestrator-rust/crates/orchestrator-scheduler/src/scheduler/coordination_tools.rs#L585): Exact model catalog is run_tests, mark_item, mark_done, create_ticket, scan_tickets, generate_items and record_metric, filtered to run allowlist.

### Evidence shim

[quarantine/agent-orchestrator-rust/crates/orchestrator-runner/src/bin/orch_mcp_tools.rs:1–71](../../../quarantine/agent-orchestrator-rust/crates/orchestrator-runner/src/bin/orch_mcp_tools.rs#L1): stdio MCP shim forwards JSON-RPC to bearer-authenticated daemon callback; effects remain in daemon, shim owns no collaboration state.

### Evidence takeover

[quarantine/agent-orchestrator-rust/crates/daemon/src/server/session.rs:474–606](../../../quarantine/agent-orchestrator-rust/crates/daemon/src/server/session.rs#L474): Interactive input requires byte limit, idempotency key, process identity and valid writer fencing token; audit reservation distinguishes accepted/in-progress/hash conflict.

### Evidence accept

[quarantine/agent-orchestrator-rust/crates/daemon/src/server/session.rs:645–678](../../../quarantine/agent-orchestrator-rust/crates/daemon/src/server/session.rs#L645): Accepted input means atomic FIFO write followed by database outcome, not model consumption; crash between external FIFO and outcome remains distinct boundary.

### Evidence handoff

[quarantine/agent-orchestrator-rust/core/src/handoff.rs:300–367](../../../quarantine/agent-orchestrator-rust/core/src/handoff.rs#L300): Resume plan persists expiring logical boundary with state fingerprint, effect class and consequences; explicitly no workspace rollback and provider resume fallback.

### Evidence build

[quarantine/agent-orchestrator-rust/crates/orchestrator-scheduler/src/scheduler/safety/restart.rs:43–151](../../../quarantine/agent-orchestrator-rust/crates/orchestrator-scheduler/src/scheduler/safety/restart.rs#L43): Self-restart performs actual cargo build then new binary --help with timeout, rejects failed build/verification.

### Evidence ready

[quarantine/agent-orchestrator-rust/crates/orchestrator-scheduler/src/scheduler/safety/restart.rs:155–230](../../../quarantine/agent-orchestrator-rust/crates/orchestrator-scheduler/src/scheduler/safety/restart.rs#L155): Build hashes old/new binaries, snapshots new stable file, sets restart_pending and persists self_restart_ready event before signaling exec readiness.

### Evidence defer

[quarantine/agent-orchestrator-rust/crates/daemon/src/main.rs:1431–1499](../../../quarantine/agent-orchestrator-rust/crates/daemon/src/main.rs#L1431): Self-restart defers while other registered tasks remain; completion unregisters and can trigger shutdown when running registry reaches zero.

### Evidence drain

[quarantine/agent-orchestrator-rust/crates/daemon/src/main.rs:1189–1247](../../../quarantine/agent-orchestrator-rust/crates/daemon/src/main.rs#L1189): Daemon shutdown signals workers, gives five seconds then forces task kill; interactive session drain is separate best-effort reclaim.

### Evidence exec

[quarantine/agent-orchestrator-rust/crates/daemon/src/main.rs:1269–1315](../../../quarantine/agent-orchestrator-rust/crates/daemon/src/main.rs#L1269): Supervisor drain timeout/panic and pending-reset error log but do not prevent exec of new daemon with same PID/args. No outpost chat transfer.

### Evidence capture

[quarantine/agent-orchestrator-rust/crates/orchestrator-runner/src/output_capture.rs:81–116](../../../quarantine/agent-orchestrator-rust/crates/orchestrator-runner/src/output_capture.rs#L81): Capture redacts stream before file write and flushes; no raw retention/fsync proof.

### Evidence query

[quarantine/agent-orchestrator-rust/crates/orchestrator-scheduler/src/service/task.rs:154–173](../../../quarantine/agent-orchestrator-rust/crates/orchestrator-scheduler/src/service/task.rs#L154): Timeline queries persisted source with cursor watermark and redaction patterns.

### Evidence trace

[quarantine/agent-orchestrator-rust/crates/orchestrator-scheduler/src/service/task.rs:408–438](../../../quarantine/agent-orchestrator-rust/crates/orchestrator-scheduler/src/service/task.rs#L408): Trace joins recorded events and command runs for timeline/anomaly projection.

### Evidence prune

[quarantine/agent-orchestrator-rust/crates/orchestrator-persistence/src/event_retention.rs:64–87](../../../quarantine/agent-orchestrator-rust/crates/orchestrator-persistence/src/event_retention.rs#L64): Retention deletes old terminal-task events in bounded batches; event database is not indefinite complete audit.

