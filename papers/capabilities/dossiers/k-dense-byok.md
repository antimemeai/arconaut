# k-dense-byok

A scientific Pi consumer adds specialists, managed remote jobs, authored notebooks, bounded provenance and scientific compaction.

Role: scientific coding-agent workbench. Runtime: TypeScript, JavaScript, Python.

Pinned source: [https://github.com/K-Dense-AI/k-dense-byok](https://github.com/K-Dense-AI/k-dense-byok); revision/version `ef60be84a27bd2096851ab307514ac3de734214f`.

Fastify host embeds Pi sessions; browser SSE observes detached runs; remote Modal workers have durable job records.

Local tools run as the backend OS user. Modal and model/MCP providers are consumed services; ordinary chat runs depend on the host remaining alive.

Inspection: Kady session creation, run lifecycle, steering/compaction, notebook recall, child controls, remote tool dispatch and selected test assertions

Limits of this study: Pi and pi-subagents dependency internals are not vendored here; their inner execution loops are not independently proven by Kady wrappers. No acquired code or tests executed.

## Actions

### POST /sessions/:id/run

Surface: operator/API.

Input: Session/project, prompt/images, model, thinking level and optional Fusion config.

Result: SSE frames, runId, usage/cost/error/done; reconnects observe retained broker buffer.

Lifecycle: Detached host task owns Pi prompt and finalizers; browser lifetime is independent, backend restart ends ordinary turn.

Authority: Host session/model credential and spend gates; Fusion temporarily clears executable tool registry.

Evidence: [e2](#evidence-e2), [e3](#evidence-e3).

### POST /sessions/:id/steer; POST /sessions/:id/abort

Surface: operator/API.

Input: Live session plus steering message; or session abort.

Result: Pending messages or 409 with restored texts; abort returns cleared queued texts.

Lifecycle: Steering lands after current tool calls via Pi; abort is recorded before awaiting provider teardown.

Authority: Host budget rechecked for steering; abort cannot silently authorize queued future work.

Evidence: [e4](#evidence-e4).

### session_before_compact / POST /sessions/:id/compact

Surface: lifecycle extension / operator API.

Input: Pi cut preparation, prior summary, custom instructions, abort signal and scientific stores.

Result: Compaction summary with firstKeptEntryId, usage and science-state metadata.

Lifecycle: Custom hook runs on compaction; manual route refuses a streaming turn; errors delegate to Pi fallback.

Authority: Credentials of selected compaction model; pending handles retained literally.

Evidence: [e5](#evidence-e5), [e6](#evidence-e6).

### notebook

Surface: model tool.

Input: Typed hypothesis/method/observation/decision/note, evidence/artifact/result links, corrections and optional plans.

Result: Stamped entry ID or soft save error; snapshots/bindings stored alongside narrative.

Lifecycle: Append to session notebook; supersedes links preserve prior authored records.

Authority: Model writes narrative fields, host stamps attribution/run/time and snapshot facts.

Evidence: [e8](#evidence-e8).

### notebook source-linked search/read

Surface: programmable API / recall tool support.

Input: Bounded query or explicit source identity plus optional expectedDigest.

Result: Ranked source-linked records, corrections/coverage/currentness qualifiers and bounded text.

Lifecycle: Read-time artifact rechecks; omitted records remain explicit and are not rewritten into facts.

Authority: Project/session namespace and source/digest validation; recalled text is historical data.

Evidence: [e9](#evidence-e9).

### modal_run; modal_submit; modal_submit_batch

Surface: model tools.

Input: Command, instance/GPU count, image/packages/environment/cache, files_in/out, timeout, optional group.

Result: Job/group IDs and public state; modal_run also retained stdout/stderr.

Lifecycle: Submit persists before remote scheduling; async survives chat abort; blocking run abort cancels.

Authority: Provider credentials, project budget reservations and transfer validation; local sandbox remains canonical.

Evidence: [e11](#evidence-e11), [e12](#evidence-e12).

### modal_status; modal_wait; modal_cancel; modal_results

Surface: model tools.

Input: job_id and optional wait timeout.

Result: State/accounting/output metadata; retained logs; cancellation acknowledgement.

Lifecycle: Wait timeout does not imply remote failure; cancel terminates remote sandbox; durable identity belongs to job store.

Authority: Project-scoped manager resolves job ownership; bounded log/retention limits.

Evidence: [e11](#evidence-e11).

### subagents:rpc:v1:request / Host.rpc / preflight

Surface: host programmable control.

Input: Method/params or child agent/task/model/context plus parent session and tool ceiling.

Result: Correlated success/data or error; 20-second RPC timeout.

Lifecycle: Session-scoped host registration disposes pending calls on closure.

Authority: Inherited/intersected capability ceiling; distinct child models and scoped parent context.

Evidence: [e14](#evidence-e14), [e13](#evidence-e13).

## Capabilities

### filesystem

**L — Files** (source): Pi filesystem tools are wired and their events recorded; actual builtin byte read/write implementation belongs to Pi. Opaque shell and simultaneous file edits escape complete attribution.

Evidence: [e1](#evidence-e1), [e10](#evidence-e10), [e16](#evidence-e16).

### processes

**L — OS programs** (source): Local shell executes with backend OS-user authority through Pi; Modal command execution has explicit submit/wait/cancel/result lifecycle. Local isolation and complete shell effects are not provided.

Evidence: [e1](#evidence-e1), [e11](#evidence-e11), [e16](#evidence-e16).

### code-actions

**S — Code actions** (source): Pi edit/bash plus MCP codemode/search are composed into the session. Kady supplies no independently traced compiler/LSP action engine here.

Evidence: [e1](#evidence-e1).

### persistent-kernel

**? — Kernel** (inspection): Not established in the explicitly inspected Kady session creation, run lifecycle, steering/compaction, notebook recall, child controls, remote tool dispatch and selected test assertions; no universal absence claim.

### standing-database

**S — Standing DB** (source): Append-oriented notebook/provenance/job stores provide durable project records and source-linked recall; these are application records, not a generic standing SQL service or kernel variable database.

Evidence: [e8](#evidence-e8), [e9](#evidence-e9), [e12](#evidence-e12).

### workflow-programming

**L — Workflows** (source): Specialist workflow scripts and schedules enter pi-subagents via host RPC/preflight. The 326 UI workflows are prompt templates; timers require Kady and overlap is skipped.

Evidence: [e14](#evidence-e14), [e15](#evidence-e15), [e18](#evidence-e18).

### multi-model

**I — Models** (source): Run model selection, independently configured specialist models and optional reviewer/Fusion paths are explicit. This is not an IRC peer room.

Evidence: [e2](#evidence-e2), [e13](#evidence-e13), [e14](#evidence-e14).

### live-collaboration

**L — Peer chat** (source): Supervisor questions and scoped child control RPC support lead/child conversation and operator transcript/steer/stop/resume. No general peer-room protocol is established.

Evidence: [e13](#evidence-e13), [e14](#evidence-e14).

### concurrent-work

**I — Concurrency** (source): Detached run ownership, background specialists and durable Modal batches have distinct handles; shared project files can conflict and ordinary host turns are not restart durable.

Evidence: [e2](#evidence-e2), [e11](#evidence-e11), [e12](#evidence-e12), [e14](#evidence-e14), [e16](#evidence-e16).

### steering-interrupt

**I — Steer/interrupt** (source): Steer queues a message into a streaming run with raced-message restoration; abort marks broker state before awaited model abort and clears queued prompts. Blocking Modal run cancellation differs from submitted durable jobs.

Evidence: [e4](#evidence-e4), [e11](#evidence-e11).

### turn-redefinition

**S — Turn program** (source): Session construction installs executable lifecycle hooks (before-agent, tool-call, before-compaction, provider rewrite) and active-tool changes. No operator/model live arbitrary turn-program replacement contract is established.

Evidence: [e1](#evidence-e1), [e2](#evidence-e2), [e6](#evidence-e6), [e14](#evidence-e14).

### compaction

**I — Compaction** (source): Bounded scientific preamble is combined with model narrative, cut-entry/usage metadata and split-turn handling. Literal job/control handles and uncertainty are retained; any hook failure uses Pi default.

Evidence: [e5](#evidence-e5), [e6](#evidence-e6).

### context-repair

**S — Repair** (source): Source-linked notebook search/read reports supersession, changed artifacts, incomplete coverage and digest identity. These supply recall for repair; no full original-context reconstruct/replace action is traced.

Evidence: [e9](#evidence-e9), [e5](#evidence-e5).

### original-audit

**L — Original audit** (source): Session/tool-derived JSONL provenance and durable job stores record useful lineage, not all original IO. Opaque commands infer bounded scans; nested children, overwritten bytes and flush errors are gaps.

Evidence: [e3](#evidence-e3), [e10](#evidence-e10), [e16](#evidence-e16).

### audit-query

**L — Audit query** (source): Artifact provenance exposes bounded version-aware upstream lineage; research recall returns source URI/digest and qualifiers. Completeness/currentness are explicitly withheld when coverage or identity is missing.

Evidence: [e9](#evidence-e9), [e10](#evidence-e10).

### hot-change

**L — Hot change** (source): A run can change selected model/thinking and Fusion active tools; remote credentials can enable already-present tools. Skills/MCP/specialist/watchdog definitions generally apply to new chats, not universal hot code reload.

Evidence: [e1](#evidence-e1), [e2](#evidence-e2), [e13](#evidence-e13), [e16](#evidence-e16).

### rebuild-continuity

**L — Rebuild continuity** (source): Browser reconnect observes a detached live backend run; ordinary turns end on backend restart. Modal jobs use separate durable recovery. No harness-refit/context-outpost handoff is established.

Evidence: [e2](#evidence-e2), [e11](#evidence-e11), [e16](#evidence-e16).

### remote-services

**I — Remote** (source): Named MCP tools are wired through Pi pipeline and Modal submit/status/wait/results consume external compute. Modal job lifetime differs from chat/host lifetime; no governance of a shared scientific fabric is shown.

Evidence: [e1](#evidence-e1), [e11](#evidence-e11), [e12](#evidence-e12).

### self-improvement

**? — Self-improve** (inspection): Not established in the explicitly inspected Kady session creation, run lifecycle, steering/compaction, notebook recall, child controls, remote tool dispatch and selected test assertions; no universal absence claim.

### complaints

**S — Complaints** (documentation): Watchdog warnings have evidence/proposed action and can steer continuation, but are bounded authored review findings, not a universal model complaint capturing full state into an external issue DB.

Evidence: [e17](#evidence-e17).

### authority

**L — Authority** (source): Host OS account is the local execution authority; child tool ceilings intersect host policy, external CLI specialists bypass accounting and are disabled by default. Pattern guards and budget gates have admitted-call/opaque-script limits.

Evidence: [e1](#evidence-e1), [e14](#evidence-e14), [e16](#evidence-e16).

### evaluation

**L — Evaluation** (documentation): Optional watchdog reviews bounded diffs/transcripts and can request repair; it neither reruns analyses nor proves scientific validity. Static tests below test known boundary faults, not model scientific competence.

Evidence: [e17](#evidence-e17), [e16](#evidence-e16).

### time-order

**L — Time/order** (source): Run claims serialize message activation through finalization; event-bus RPC uses unique request IDs and timeout. Durable job sequence/cursors and wall-clock timestamps are useful local orders, not a global causal clock.

Evidence: [e7](#evidence-e7), [e14](#evidence-e14), [e12](#evidence-e12).

## Inspected test oracles

- [quarantine/k-dense-byok/server/test/compaction-bridge.test.ts](../../../quarantine/k-dense-byok/server/test/compaction-bridge.test.ts): Lost child/remote handles and false certainty after compaction Oracle: Seeded real stores assert literal pending job/control IDs, foreign/terminal exclusions, corrected/uncertain records; fake generator tests fallback and usage. It does not prove narrative fidelity. Read, **not executed**.
- [quarantine/k-dense-byok/server/test/session-message-gate.test.ts](../../../quarantine/k-dense-byok/server/test/session-message-gate.test.ts): Reentrant follow-up overtaking provenance/accounting Oracle: Real Pi sessions with a fake provider and gated provenance flush assert zero dispatch while claimed, FIFO message boundaries, two separately ledgered runs and claim release. No live provider cancellation oracle. Read, **not executed**.
- [quarantine/k-dense-byok/server/test/modal-durable.test.ts](../../../quarantine/k-dense-byok/server/test/modal-durable.test.ts): Crash recovery accounting and bounded durable remote results Oracle: Fake Modal factory with real temporary stores asserts orphan reservation release, terminal job persistence, cursor rollover and independently known installed result bytes. It does not exercise real cloud sandbox recovery. Read, **not executed**.
- [quarantine/k-dense-byok/server/test/provenance-harvest.test.ts](../../../quarantine/k-dense-byok/server/test/provenance-harvest.test.ts): False write-time certainty from retrospective child hashes Oracle: Synthetic child transcripts and known local file bytes assert harvest identity, no scan baseline for opaque shell, stale mismatch and unknown on matching retrospective hash. This deliberately rejects a misleading currentness claim. Read, **not executed**.

## Useful mechanisms

- Literal handles and explicit uncertainty survive scientific compaction.
- Run claims include finalization and queued system-message activation.
- Remote job lifecycle and citation/provenance identity limits are unusually explicit.

## Material limits

- TypeScript/Pi dependent host is architecturally outside the preferred Arconaut core languages.
- Application JSONL records and authored notebook are not a complete original audit or shared standing database.
- Configuration applies mainly to new chats; refit continuity and autonomous harness improvement are not established.

## Arconaut design questions

- Should turn completion include audit persistence as a required commit rather than Kady's warn-and-continue finalizer?
- Which literal service/child handles and authority facts must survive each compaction option?
- How should source-linked correction recall preserve uncertainty without converting authored notebook entries into truth?

## Evidence

### Evidence e1

[quarantine/k-dense-byok/server/src/agent/session-registry.ts:385–446](../../../quarantine/k-dense-byok/server/src/agent/session-registry.ts#L385): Pi session construction wires custom scientific tools, data guards and MCP/codemode/search extensions; initial resource reload occurs before session creation.

### Evidence e2

[quarantine/k-dense-byok/server/src/api/sessions.ts:810–889](../../../quarantine/k-dense-byok/server/src/api/sessions.ts#L810): Per-run model/thinking selection and Fusion tool suppression precede detached executeRun(session.prompt); observer HTTP closure does not own finalization.

### Evidence e3

[quarantine/k-dense-byok/server/src/agent/run-pipeline.ts:303–440](../../../quarantine/k-dense-byok/server/src/agent/run-pipeline.ts#L303): Events feed provenance and usage; flush/ledger occur before completion, but their errors warn and execution still completes.

### Evidence e4

[quarantine/k-dense-byok/server/src/api/sessions.ts:463–547](../../../quarantine/k-dense-byok/server/src/api/sessions.ts#L463): Abort marks handle before awaiting session abort and clears queues; live steering rechecks spend and returns raced-undelivered messages.

### Evidence e5

[quarantine/k-dense-byok/server/src/agent/compaction-bridge.ts:38–220](../../../quarantine/k-dense-byok/server/src/agent/compaction-bridge.ts#L38): Bounded deterministic scientific preamble retains authored uncertainty/corrections, result/environment IDs and literal pending Modal/child control handles.

### Evidence e6

[quarantine/k-dense-byok/server/src/agent/compaction-bridge.ts:233–325](../../../quarantine/k-dense-byok/server/src/agent/compaction-bridge.ts#L233): Compaction calls the configured summary generator with custom instructions; returns summary/cut-entry/usage/details or falls back to Pi default on errors.

### Evidence e7

[quarantine/k-dense-byok/server/src/agent/session-message-gate.ts:1–44](../../../quarantine/k-dense-byok/server/src/agent/session-message-gate.ts#L1): Deferred session messages wait for run release before and after delivery; a failed submission does not discard later queued notices.

### Evidence e8

[quarantine/k-dense-byok/server/src/agent/notebook.ts:105–195](../../../quarantine/k-dense-byok/server/src/agent/notebook.ts#L105): Notebook validates typed narratives, snapshots artifact/result links and stamps host role/run/time, allowing only selected model-authored fields; save failure is a soft error.

### Evidence e9

[quarantine/k-dense-byok/server/src/agent/notebook-memory.ts:61–147](../../../quarantine/k-dense-byok/server/src/agent/notebook-memory.ts#L61): Recall reports correction/history/coverage qualifiers, source identities and read-time artifact health, with bounded response size; it is not a permanent extracted-fact writer.

### Evidence e10

[quarantine/k-dense-byok/docs/provenance.md:9–91](../../../quarantine/k-dense-byok/docs/provenance.md#L9): Provenance differentiates observed, inferred and declared edges, version lineage and bounded environment snapshots; opaque scans/children and retention have explicit gaps.

### Evidence e11

[quarantine/k-dense-byok/server/src/agent/modal-tool.ts:270–437](../../../quarantine/k-dense-byok/server/src/agent/modal-tool.ts#L270): Native Modal tool family submits durable jobs and returns identifiers/state/logs; blocking run abort cancels while asynchronous submit survives chat abort.

### Evidence e12

[quarantine/k-dense-byok/server/src/modal/manager.ts:360–419](../../../quarantine/k-dense-byok/server/src/modal/manager.ts#L360): Job record is persisted before asynchronous scheduling; batch failure cancels already-submitted children and returns one group handle.

### Evidence e13

[quarantine/k-dense-byok/docs/sub-agents.md:3–74](../../../quarantine/k-dense-byok/docs/sub-agents.md#L3): Specialists share files but have distinct models/context/tool ceilings; external CLIs are disabled by default, direct-child harvest only and configuration generally affects new chats.

### Evidence e14

[quarantine/k-dense-byok/server/src/agent/subagent-control.ts:16–100](../../../quarantine/k-dense-byok/server/src/agent/subagent-control.ts#L16): Scoped event-bus RPC has UUID reply correlation and 20-second timeout; preflight uses parent model/session/tool ceiling and pending Modal work enters background-work tracking.

### Evidence e15

[quarantine/k-dense-byok/docs/automation.md:6–50](../../../quarantine/k-dense-byok/docs/automation.md#L6): Persisted schedules depend on a resident backend, fresh contexts, catch-up policy and no-overlap; missions expose mission.show.

### Evidence e16

[quarantine/k-dense-byok/docs/limitations.md:19–69](../../../quarantine/k-dense-byok/docs/limitations.md#L19): Host sandbox is a workspace rather than OS isolation; ordinary runs end on backend restart, records are not complete execution IO and hashes cannot recover overwritten versions.

### Evidence e17

[quarantine/k-dense-byok/docs/watchdog.md:3–38](../../../quarantine/k-dense-byok/docs/watchdog.md#L3): Optional reviewer gets bounded transcript/diffs, can trigger continuations/stalemate and may fail silently; it does not execute or independently verify science.

### Evidence e18

[quarantine/k-dense-byok/docs/contributing-workflows.md:3–26](../../../quarantine/k-dense-byok/docs/contributing-workflows.md#L3): Workflow catalogue consists of JSON prompt templates plus UI-selected input references, not an executable orchestration graph.

