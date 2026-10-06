# open-swe

Current software-factory application wires configurable model/subagent tools, busy-thread steering, persistent sandbox reconnection, background command handles, skills/settings/automation controls and transactional transcript projections. Observer capture is deliberately best-effort.

Role: hosted/local engineering workflow application on Deep Agents. Runtime: Python, TypeScript, JavaScript.

Pinned source: [https://github.com/langchain-ai/open-swe](https://github.com/langchain-ai/open-swe); revision/version `9613f663bbff2b0b158dd9238b915ae3874aa67b`.

Per-run async graph factory delegates decision loop to Deep Agents/LangGraph; Python server/middleware/transcript service; remote or local sandbox plus background supervisor scripts

Application manages per-thread sandbox binding, credentials, scheduling and records; providers and graph runtime are dependencies. This includes fabric/control-plane functions beyond Arconaut consumer role.

Inspection: Read graph entry/factory and actual middleware wiring, queued-message consume/steer/cancel and model-facing thread operations, sandbox reconnect/publish/local backend, detached command supervisor/start/poll/stop, compaction wrapper, transcript append/projection/blob/read APIs, model-authorized skills/settings/schedules, incident report scope and direct source-test oracles.

Limits of this study: No source executed or dependencies installed. Deep Agents/LangGraph runtime and provider transports are delegated; all integrations/desktop/CLI/reviewer/scheduler paths not audited. Read graph-construction/wrappers, not an imaginary owned core loop.

## Actions

### Deep Agents filesystem / execute / task and curated integration tools

Surface: model tool surface via delegated SDK.

Input: file/edit/command/task plus selected tools/model/backend/context

Result: results or child graph output through framework protocol

Lifecycle: actual graph wires backend/subagent/middleware; engine/transport lifecycle delegated

Authority: model within configured sandbox/tool/access policies

Evidence: [factory](#evidence-factory), [local](#evidence-local).

### background_execute / background_task(status/list/stop)

Surface: model tool.

Input: command+deadline or stable task ID/action

Result: running/status/exit/bounded-output record; launch may fail observation after start

Lifecycle: detached sandbox supervisor owns group; monitor/cron separate; stop request then status observation

Authority: model command task; workspace refresh control admin-only

Evidence: [background](#evidence-background), [background-start](#evidence-background-start), [background-control](#evidence-background-control).

### get_thread / manage_thread(send_message/cancel/resolve) / start thread

Surface: model coordination tool.

Input: authorized thread locator, input, model/effort or operation

Result: selected state/queue/run identity, queued or started response

Lifecycle: busy next-model input or new run; cancellation state not foreign effect settlement

Authority: model under triggering verified actor/participant/admin restrictions

Evidence: [get-thread](#evidence-get-thread), [thread-tools](#evidence-thread-tools), [send-tool](#evidence-send-tool), [live-steer](#evidence-live-steer), [cancel](#evidence-cancel).

### save_user_settings / save_user_skill / delete_user_skill

Surface: model tool.

Input: ordinary settings or named instructions/description

Result: updated durable user settings/skill or policy error

Lifecycle: store mutation for later construction/use; no candidate-performance promotion

Authority: model triggering private owner; concierge setting operator-only

Evidence: [settings](#evidence-settings), [skills](#evidence-skills).

### create/update/list automation

Surface: model orchestration tool.

Input: prompt/workspace/repository, trigger/schedule, model/effort and notifications

Result: stored schedule/automation identity

Lifecycle: recurring/event work delegated to schedule/graph services

Authority: model in trusted admin thread; schedule machine identity separated

Evidence: [schedules](#evidence-schedules).

### manual/automatic conversation offload

Surface: context middleware.

Input: current messages, trigger/budget or manual factory flag

Result: start/fail/complete event with cutoff/file reference; manual path ends graph

Lifecycle: delegated summarization wrapper; hidden auxiliary model streaming

Authority: operator/configured graph strategy

Evidence: [compact](#evidence-compact), [factory](#evidence-factory).

### ensure_sandbox_for_thread / reconnect/recreate

Surface: sandbox primitive.

Input: thread binding and selected provider/workspace/replacement policy

Result: same live sandbox adapter or typed unreachable/new binding

Lifecycle: init and persisted binding precede cache publication; no silent swap of uncommitted workspace

Authority: host lifecycle/model explicit recreate surface; governs its own per-thread resources

Evidence: [sandbox-contract](#evidence-sandbox-contract), [sandbox-publish](#evidence-sandbox-publish), [init-test](#evidence-init-test).

### transcript append / snapshot/output/event read

Surface: persistence/API primitive.

Input: thread command ID, typed event, attachments/output or cursor/call ID

Result: sequenced receipt/event/projected state and capped sidecars

Lifecycle: lock/transaction commit storage; observation queue separately best-effort, failed batches not retried here

Authority: host observer/API under thread read auth; not generic model SQL

Evidence: [engine](#evidence-engine), [engine-write](#evidence-engine-write), [writer](#evidence-writer), [read-output](#evidence-read-output), [output](#evidence-output).

### record_incident_report / incident evidence search

Surface: domain model tool.

Input: business incident draft/evidence and authorized incident session

Result: stored report/document/digest and optional responder message

Lifecycle: document update precedes report write; real business incident flow, not agent-state grievance

Authority: model incident-session tools under workspace policy

Evidence: [incident](#evidence-incident).

## Capabilities

### filesystem

**I — Files** (source): Graph exposes Deep Agents filesystem/shell backend plus sandbox tools and skill stores. Local adapter uses actual selected host directory; core tool implementation delegated, all path policies not audited.

Evidence: [factory](#evidence-factory), [local](#evidence-local), [skills](#evidence-skills).

### processes

**I — OS programs** (source): Actual background supervisor script launches command group, stores status/output, uses monotonic deadline/escalation/root wait and model status/list/stop provider. Starts may outlive failed monitor response.

Evidence: [background](#evidence-background), [background-start](#evidence-background-start), [background-control](#evidence-background-control).

### code-actions

**S — Code actions** (source): Model command can execute authored programs in sandbox/host and configure stored instructions/skills. No exposed persistent interpreter or generalized executable tool-composition kernel traced.

Evidence: [background](#evidence-background), [local](#evidence-local), [skills](#evidence-skills).

### persistent-kernel

**? — Kernel** (inspection scope): persistent_kernel: not established beyond inspected graph/middleware/sandbox/transcript/thread/tool paths; framework internals and other integrations are outside this trace.

### standing-database

**S — Standing DB** (source): Native get/manage-thread, schedules, user skills and incident records expose standing structured application data. PostgreSQL transcript is service storage, not generic model SQL/database access.

Evidence: [get-thread](#evidence-get-thread), [schedules](#evidence-schedules), [skills](#evidence-skills), [incident](#evidence-incident), [engine](#evidence-engine).

### workflow-programming

**S — Workflows** (source): Model-authorized automation creates recurring/event-triggered graph work; factory composes middleware/subagent programs. These are configured host workflows, not full model replacement of own turn scheduler.

Evidence: [factory](#evidence-factory), [schedules](#evidence-schedules), [settings](#evidence-settings).

### multi-model

**I — Models** (source): Factory separately configures main/subagent models; thread send can choose model+effort for started/follow-up work and schedules persist model choices.

Evidence: [factory](#evidence-factory), [send-tool](#evidence-send-tool), [schedules](#evidence-schedules).

### live-collaboration

**I — Peer chat** (source): Model can address permitted active/idle threads via manage_thread: queue input before next model step or start new run, observe selected state. Actor identity remains triggering owner; no universal independent peer-room semantics inferred.

Evidence: [thread-tools](#evidence-thread-tools), [send-tool](#evidence-send-tool), [live-steer](#evidence-live-steer), [queue](#evidence-queue), [get-thread](#evidence-get-thread).

### concurrent-work

**I — Concurrency** (source): Independent per-thread sandboxes and subagents/background command handles; application schedules work and requests independent state reads. Graph task lifetime delegated to runtime, not settled merely by tool result.

Evidence: [factory](#evidence-factory), [sandbox-publish](#evidence-sandbox-publish), [background-start](#evidence-background-start), [background-control](#evidence-background-control), [get-thread](#evidence-get-thread).

### steering-interrupt

**L — Steer/interrupt** (source): Busy-target steering queues input and guards live-run exit race; interruption records state and may continue followups. Model timeout/cancel and stopped runs do not establish external-provider/command settlement.

Evidence: [queue](#evidence-queue), [live-steer](#evidence-live-steer), [cancel](#evidence-cancel), [timeout](#evidence-timeout).

### turn-redefinition

**S — Turn program** (source): Actual factory middleware can alter model input, tools, queue pickup and manual offload termination. Native model settings/skills modify ordinary future construction; no full scheduler replacement or our delayed-activation transaction established.

Evidence: [factory](#evidence-factory), [compact](#evidence-compact), [settings](#evidence-settings), [skills](#evidence-skills).

### compaction

**S — Compaction** (source): Wired summarization wrapper publishes trigger/cutoff/file/event and manual/automatic outcome; underlying transformation/original storage is delegated Deep Agents, not independently established here.

Evidence: [factory](#evidence-factory), [compact](#evidence-compact).

### context-repair

**S — Repair** (source): Persisted transcript events/output sidecars, bounded thread inspection and offload file reference give recall inputs. Output clipping/failed capture and no traced full restore/merge prevent original-complete repair claim.

Evidence: [engine](#evidence-engine), [output](#evidence-output), [get-thread](#evidence-get-thread), [compact](#evidence-compact), [writer](#evidence-writer).

### original-audit

**L — Original audit** (source): Transactional sequenced transcript and same-transaction sidecars are strong captured-state machinery. Capture is normalized/capped, hidden calls excluded, failed append swallowed/marked done, drain bounded then writer canceled; not complete provider/program/transformation IO.

Evidence: [engine](#evidence-engine), [engine-write](#evidence-engine-write), [capture](#evidence-capture), [capture-tool](#evidence-capture-tool), [writer](#evidence-writer), [output](#evidence-output).

### audit-query

**S — Audit query** (source): Authenticated transcript API returns retained output; model get_thread exposes selected state/run/plan/queue information. These are scoped projected records, not every-original query corpus.

Evidence: [read-output](#evidence-read-output), [get-thread](#evidence-get-thread), [output](#evidence-output).

### hot-change

**S — Hot change** (source): Model owner can patch settings and saved skill definitions; next graph construction wires selected programs/config. Exact active-work generation handling and whole-turn/workflow default activation not traced.

Evidence: [settings](#evidence-settings), [skills](#evidence-skills), [factory](#evidence-factory).

### rebuild-continuity

**L — Rebuild continuity** (source): Thread-bound sandbox reconnect preserves resource identity and refuses unreachable replacement except explicit rederivable/gone cases. This is sandbox continuity, not main executable compile/refit/outpost/reinhabitation protocol.

Evidence: [sandbox-contract](#evidence-sandbox-contract), [sandbox-publish](#evidence-sandbox-publish).

### remote-services

**I — Remote** (source): Actual sandbox/backend proxies reconnect to thread service and model-control tools call graph/API/schedules; consumes remote computation while also governing workspace refresh and scheduling outside Arconaut scope.

Evidence: [sandbox-publish](#evidence-sandbox-publish), [background-control](#evidence-background-control), [send-tool](#evidence-send-tool), [schedules](#evidence-schedules).

### self-improvement

**S — Self-improve** (source): Model can modify user skills/settings and automation prompts. No measured harness-candidate evaluation/promote path established; engineering review/self-repair workflow scope not harness autoresearch.

Evidence: [settings](#evidence-settings), [skills](#evidence-skills), [schedules](#evidence-schedules).

### complaints

**? — Complaints** (inspection scope): complaints: not established beyond inspected graph/middleware/sandbox/transcript/thread/tool paths; framework internals and other integrations are outside this trace.

### authority

**L — Authority** (source): Native model settings/skills/thread/admin automation operations enforce triggering-owner/private/admin policies. Local backend direct host authority and workflow-specific approval scope are explicit; not operator-minimal unrestricted own-program governance.

Evidence: [settings](#evidence-settings), [skills](#evidence-skills), [thread-tools](#evidence-thread-tools), [schedules](#evidence-schedules), [local](#evidence-local), [role](#evidence-role).

### evaluation

**S — Evaluation** (source): Selected test oracles independently read DB receipt version/count/capped output and inject setup/queue/cancel failures; several providers/stores are mocked. No overall engineering/scientific quality or autoresearch criterion proven.

Evidence: [receipt-test](#evidence-receipt-test), [cap-test](#evidence-cap-test), [init-test](#evidence-init-test), [queue-test](#evidence-queue-test), [cancel-test](#evidence-cancel-test).

### time-order

**L — Time/order** (source): Captured transcript has transactionally gapless per-thread versions/receipts; command supervisor monotonic budget vs wall timestamps. Store queue consume is equality/reread with separate write, not complete causal admission/consumption order.

Evidence: [engine](#evidence-engine), [receipt-test](#evidence-receipt-test), [background](#evidence-background), [queue-consume](#evidence-queue-consume), [writer](#evidence-writer).

## Inspected test oracles

- [quarantine/open-swe/tests/transcript/test_engine.py](../../../quarantine/open-swe/tests/transcript/test_engine.py): Stored command replay and original-output cap Oracle: Configured DB exact version2 reused, no second event/count2; over-cap output explicitly truncated and retained length fixed. No original payload above cap or external effect idempotency oracle. Read, **not executed**.
- [quarantine/open-swe/tests/middleware/test_check_message_queue.py](../../../quarantine/open-swe/tests/middleware/test_check_message_queue.py): Followup queued while first message builds Oracle: Fake store appends second during image build; first injected and second stays. Does not establish arbitrary concurrent read/write/DB schedules or crash between consume and graph commit. Read, **not executed**.
- [quarantine/open-swe/tests/sandbox/test_sandbox_publish_ordering.py](../../../quarantine/open-swe/tests/sandbox/test_sandbox_publish_ordering.py): Initialization failure must not publish usable backend Oracle: Mock create/binding failure raises and proxy cache remains unready. No actual remote sandbox/credential/connection behavior tested. Read, **not executed**.
- [quarantine/open-swe/tests/middleware/test_transcript.py](../../../quarantine/open-swe/tests/middleware/test_transcript.py): Child cancellation attribution Oracle: Mock nested cancelled tool leaves parent running and completes parent turn; no process/provider settlement oracle. Read, **not executed**.

## Useful mechanisms

- Captured transcript versions/receipts/projection/output transaction is explicit and directly exercised by source test oracle.
- Existing unreachable workspace is preserved rather than silently replaced.
- Model can address other authorized thread work and edit owner skills/settings/admin automations.

## Material limits

- Core loop/compaction and provider execution delegated to framework dependencies, not audited as owned Open SWE machinery.
- Observer failures deliberately fail open; capped originals and bounded drain can lose capture.
- Application includes resource/control-plane governance beyond Arconaut consumer boundary; ordinary refit protocol absent.

## Arconaut design questions

- Keep a transactional captured-original ledger distinct from queued best-effort observation and projected UI state.
- When a command starts but monitor initialization fails, preserve handle/custody in error result and reconnect.
- Separate model coordination identity from inherited owner identity; retain queue admission/consumption boundaries across failed context commits.

## Evidence

### Evidence role

[quarantine/open-swe/README.md:20–83](../../../quarantine/open-swe/README.md#L20): Software factory on Deep Agents/LangGraph, per-thread persistent sandboxes, local direct execution, configured triggers/providers and workflow-specific approval scope.

### Evidence deps

[quarantine/open-swe/pyproject.toml:1–42](../../../quarantine/open-swe/pyproject.toml#L1): Python package delegates graph/core agent/provider execution to declared dependencies; TS interfaces in same application.

### Evidence factory

[quarantine/open-swe/agent/server.py:1870–1975](../../../quarantine/open-swe/agent/server.py#L1870): Actual Deep Agents factory wires model/tools/backend/subagent, offloading/preparation/transcript/policy/pairing/queue middleware. Child excludes root queue/routing hooks.

### Evidence queue

[quarantine/open-swe/agent/middleware/check_message_queue.py:207–282](../../../quarantine/open-swe/agent/middleware/check_message_queue.py#L207): Before-model middleware reads pending snapshot and builds human input; missing store/read fails open.

### Evidence queue-publish

[quarantine/open-swe/agent/middleware/check_message_queue.py:324–362](../../../quarantine/open-swe/agent/middleware/check_message_queue.py#L324): Only after constructing input removes consumed snapshot and returns message update; failure is logged and returns None.

### Evidence queue-consume

[quarantine/open-swe/agent/middleware/check_message_queue.py:159–178](../../../quarantine/open-swe/agent/middleware/check_message_queue.py#L159): Rereads pending item and keeps messages not equal to consumed snapshot; nontransactional read/write, no claim all concurrent enqueue schedules safe.

### Evidence thread-tools

[quarantine/open-swe/agent/tools/threads.py:958–1016](../../../quarantine/open-swe/agent/tools/threads.py#L958): Model manage_thread verifies actor/authorized target then sends input, cancels/resolves thread and routes private/admin access.

### Evidence send-tool

[quarantine/open-swe/agent/tools/threads.py:841–904](../../../quarantine/open-swe/agent/tools/threads.py#L841): Actual queued send, fallback run.start via proxy on conflict, selected model/effort and admitted run ID/status; model can address another permitted thread.

### Evidence get-thread

[quarantine/open-swe/agent/tools/threads.py:722–769](../../../quarantine/open-swe/agent/tools/threads.py#L722): Model get_thread loads selected thread/state/recent/pending runs/plan/comments/approvals and queue count concurrently; not SQL/general original audit query.

### Evidence live-steer

[quarantine/open-swe/agent/threads/runs.py:1095–1166](../../../quarantine/open-swe/agent/threads/runs.py#L1095): Records human identity, queues input with queue_id, fails if store delivery fails and reprobes ended live run to start reject-strategy follow-up.

### Evidence cancel

[quarantine/open-swe/agent/threads/handlers.py:342–389](../../../quarantine/open-swe/agent/threads/handlers.py#L342): Requests active run interruption by owner, records transcript interrupted and may dispatch queued follow-up; does not prove remote sandbox/OS effects settled.

### Evidence sandbox-contract

[quarantine/open-swe/agent/sandboxes/lifecycle.py:382–414](../../../quarantine/open-swe/agent/sandboxes/lifecycle.py#L382): Existing unreachable sandbox must not be silently replaced; gone/reviewer-rederivable cases allow replacement; binds same thread resource.

### Evidence sandbox-publish

[quarantine/open-swe/agent/sandboxes/lifecycle.py:444–516](../../../quarantine/open-swe/agent/sandboxes/lifecycle.py#L444): Connect/create selected sandbox, persist binding after init, publish backend only after tool provisioning; failed setup is not silently accepted.

### Evidence local

[quarantine/open-swe/agent/sandboxes/providers/local.py:1–66](../../../quarantine/open-swe/agent/sandboxes/providers/local.py#L1): Constructs actual LocalShellBackend with selected root/env and scoped Git identity file; direct host execution not OS isolation.

### Evidence background

[quarantine/open-swe/agent/tools/background_execute.py:28–151](../../../quarantine/open-swe/agent/tools/background_execute.py#L28): Authored supervisor starts new command process group, captures bounded head/tail output, uses monotonic deadline, TERM/KILL and final root wait/status file.

### Evidence background-start

[quarantine/open-swe/agent/tools/background_execute.py:289–339](../../../quarantine/open-swe/agent/tools/background_execute.py#L289): Starts command with task ID, tracks running and completion monitor; monitoring failure can return success=False after command already started.

### Evidence background-control

[quarantine/open-swe/agent/tools/background_task.py:37–98](../../../quarantine/open-swe/agent/tools/background_task.py#L37): Model common status/list/stop API routes command versus workspace-refresh task provider; workspace-wide stop/trace admin-gated.

### Evidence compact

[quarantine/open-swe/agent/middleware/conversation_offloading.py:1–130](../../../quarantine/open-swe/agent/middleware/conversation_offloading.py#L1): Subclasses delegated summarization; hides summary model streaming, emits start/failure/complete+cutoff/file events, supports manual finish path. Original archive semantics belong to dependency.

### Evidence timeout

[quarantine/open-swe/agent/middleware/model_call_timeout.py:1–61](../../../quarantine/open-swe/agent/middleware/model_call_timeout.py#L1): wait_for bounds async model handler and turns timeout into typed failure; no external-provider effect settlement proof.

### Evidence engine

[quarantine/open-swe/agent/transcript/engine.py:75–149](../../../quarantine/open-swe/agent/transcript/engine.py#L75): Per-thread advisory lock/transaction assigns gapless versions, idempotency receipts, writes events/projection/head/notify and publishes only after transaction context returns.

### Evidence engine-write

[quarantine/open-swe/agent/transcript/engine.py:208–271](../../../quarantine/open-swe/agent/transcript/engine.py#L208): Actual event insert, attachment/output storage and projection application in same connection/transaction; normalization occurs before log.

### Evidence capture

[quarantine/open-swe/agent/middleware/transcript.py:1–80](../../../quarantine/open-swe/agent/middleware/transcript.py#L1): Transcript observation failures swallowed; one background writer per run; transformed text/delta selection and explicit output/error caps/hidden-tag exclusions.

### Evidence capture-tool

[quarantine/open-swe/agent/middleware/transcript.py:393–468](../../../quarantine/open-swe/agent/middleware/transcript.py#L393): Converts output to text then UTF8 byte cap; normalized delta callback attributes one generated message ID per model call.

### Evidence writer

[quarantine/open-swe/agent/middleware/transcript.py:541–587](../../../quarantine/open-swe/agent/middleware/transcript.py#L541): Failed append batch warning then task_done; finish joins queue bounded30s then cancels writer, so queue completion is not guaranteed durable capture.

### Evidence output

[quarantine/open-swe/agent/transcript/tool_output.py:1–79](../../../quarantine/open-swe/agent/transcript/tool_output.py#L1): Output peer storage caps at256K chars, overwrites same thread/call key, returns only retained text; explicitly not recoverable original beyond cap.

### Evidence read-output

[quarantine/open-swe/agent/transcript/routes.py:162–173](../../../quarantine/open-swe/agent/transcript/routes.py#L162): Authorized API loads stored output by thread/tool call or reports404.

### Evidence settings

[quarantine/open-swe/agent/tools/save_user_settings.py:1–23](../../../quarantine/open-swe/agent/tools/save_user_settings.py#L1): Actual model settings patch under trusted-private/owner/direct policy; concierge_mode must change in dashboard.

### Evidence skills

[quarantine/open-swe/agent/tools/user_skills.py:1–53](../../../quarantine/open-swe/agent/tools/user_skills.py#L1): Owner policy model can create/update/delete user skills in shared store; editable instructions, no quality/promotion evaluator here.

### Evidence schedules

[quarantine/open-swe/agent/tools/automations.py:14–97](../../../quarantine/open-swe/agent/tools/automations.py#L14): Read/admin write policies and native create_automation invoke schedule store with prompt/workspace/model/effort/trigger/notification fields.

### Evidence incident

[quarantine/open-swe/agent/incidents/runtime.py:115–155](../../../quarantine/open-swe/agent/incidents/runtime.py#L115): Business incident report finalizes evidence, updates document before report row and optional responder delivery; not universal harness complaint/snapshot.

### Evidence queue-test

[quarantine/open-swe/tests/middleware/test_check_message_queue.py:85–118](../../../quarantine/open-swe/tests/middleware/test_check_message_queue.py#L85): Fake store injects second message during first image build; exact old-message output and remaining queue witness, not arbitrary DB concurrent append proof.

### Evidence init-test

[quarantine/open-swe/tests/sandbox/test_sandbox_publish_ordering.py:1–54](../../../quarantine/open-swe/tests/sandbox/test_sandbox_publish_ordering.py#L1): Mock setup/thread-bind failure must leave proxy/backend unpublished; actual failure boundary, no live sandbox tested.

### Evidence receipt-test

[quarantine/open-swe/tests/transcript/test_engine.py:99–130](../../../quarantine/open-swe/tests/transcript/test_engine.py#L99): Real configured DB append replay asserts same literal version, no new events and count2; no external effect idempotency claim.

### Evidence cap-test

[quarantine/open-swe/tests/transcript/test_engine.py:276–310](../../../quarantine/open-swe/tests/transcript/test_engine.py#L276): Oversized output test checks truncated flag and exact stored cap; supports bounded custody, not full source bytes.

### Evidence cancel-test

[quarantine/open-swe/tests/middleware/test_transcript.py:311–335](../../../quarantine/open-swe/tests/middleware/test_transcript.py#L311): Mock cancelled child tool must not mark parent turn interrupted; ownership-attribution oracle, no foreign work death.

