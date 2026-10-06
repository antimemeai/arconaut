# openhands

Current OpenHands repository is Agent Canvas 1.24.0, a multi-backend coding-agent/automation controller, not the older Python coding engine. Actual browser child-launch mediation exposes admission versus completion and authority boundaries.

Role: Agent Canvas frontend/controller and local service launcher. Runtime: TypeScript, JavaScript.

Pinned source: [https://github.com/All-Hands-AI/OpenHands](https://github.com/All-Hands-AI/OpenHands); revision/version `953c944eee62b8077a65436c2c8fd6b355342562`.

React/browser controller plus Node service launcher; coding/model turns delegated to separately distributed Python agent server and automation services

Consumes local/remote/cloud conversation backends through TypeScript client; launcher provisions its own local stack. Agent SDK/server source is not in this snapshot.

Inspection: Read current README/package/default pins, launcher command construction/process utilities, actual child-launch WebSocket wiring/local/cloud/control/result/dedup paths, conversation file/send/condense/fork/model APIs, automation dispatch/cancel and independent mock/literal source-test oracles.

Limits of this study: No reference execution/install. Unvendored agent SDK/server/automation backend and TypeScript client implementation not audited; kernel/provider/turn/audit guarantees cannot be inferred from frontend requests. Full desktop/extension/event caching pipeline not exhaustively inspected.

## Actions

### launch_child_conversation

Surface: model client-tool bridge.

Input: model ActionEvent task, local/cloud target, workspace/isolation/title/repository

Result: backend conversation/start-task ID, URL, isolation/parent-link caveats or error guidance

Lifecycle: server acknowledgement first; browser async launch then usually posts result; active goal can suppress; dedup is browser-local

Authority: model client tool handled by operator browser; selected service credentials

Evidence: [wiring](#evidence-wiring), [dedup](#evidence-dedup), [local-child](#evidence-local-child), [cloud-child](#evidence-cloud-child), [result](#evidence-result).

### sendMessage / createConversation / switchProfile / switchAcpModel

Surface: controller API.

Input: conversation and message or profile/model plus selected backend/runtime credentials

Result: sent event or backend start task; model-switch response

Lifecycle: calls external clients; sent message != consumed input or completed turn

Authority: operator/controller API; model launch uses configured service

Evidence: [send](#evidence-send), [models](#evidence-models), [cloud-child](#evidence-cloud-child).

### readConversationFile / downloadConversation / forkConversation

Surface: controller API.

Input: conversation ID and scoped file/event cursor

Result: file/trajectory bytes or local fork ID

Lifecycle: backend request; cloud fork rejected; older server fork contract differs

Authority: controller API under selected backend credentials

Evidence: [files](#evidence-files), [fork](#evidence-fork).

### condenseConversation

Surface: controller API.

Input: conversation ID/runtime URL/session key

Result: external compaction completion/error

Lifecycle: runtime endpoint call; no local implementation of transformation custody

Authority: operator/controller API

Evidence: [condense](#evidence-condense), [condense-test](#evidence-condense-test).

### create/update/dispatch/cancel automation

Surface: controller API.

Input: prompt, repository, event/cron/timezone specification or run ID

Result: external automation record/run status

Lifecycle: scheduler and run lifecycle external; cancel response not observed process settlement

Authority: operator/controller against configured automation service

Evidence: [automation](#evidence-automation), [create](#evidence-create), [automation-control](#evidence-automation-control).

### agent-canvas / buildAgentServerCommand / signalProcessTree

Surface: operator command / launcher primitive.

Input: startup mode, local path/Git ref/version and stack process handle

Result: server argv/process group or signal status

Lifecycle: separate Python distributions launched via uvx; source rebuilt next invocation; no active compiled refit handoff

Authority: operator launcher, not ordinary model tool

Evidence: [launcher](#evidence-launcher), [server-command](#evidence-server-command), [process](#evidence-process), [signal](#evidence-signal).

## Capabilities

### filesystem

**S — Files** (source): Controller supplies scoped remote file/trajectory downloads and asks backend to create shared/worktree workspace. Actual model edits and tool engine are unvendored.

Evidence: [files](#evidence-files), [local-child](#evidence-local-child).

### processes

**S — OS programs** (source): Launcher builds coherent server install/run argv and process-group options/signals. These are stack lifecycle primitives, not model shell execution or final death proof.

Evidence: [server-command](#evidence-server-command), [process](#evidence-process), [signal](#evidence-signal).

### code-actions

**? — Code actions** (inspection scope): code_actions: not established beyond the inspected browser/controller/launcher/client operations; server implementation is separate and unavailable in this snapshot.

### persistent-kernel

**? — Kernel** (inspection scope): persistent_kernel: not established beyond the inspected browser/controller/launcher/client operations; server implementation is separate and unavailable in this snapshot.

### standing-database

**? — Standing DB** (inspection scope): standing_database: not established beyond the inspected browser/controller/launcher/client operations; server implementation is separate and unavailable in this snapshot.

### workflow-programming

**S — Workflows** (source): Automation client can import prompt plus cron/event spec, create/update/dispatch/cancel externally owned runs; scheduler and executable workflow engine are separate distribution.

Evidence: [automation](#evidence-automation), [create](#evidence-create), [automation-control](#evidence-automation-control).

### multi-model

**S — Models** (source): Actual profile/ACP model-switch requests and local/cloud model configuration surfaces exist. Provider execution/switch timing remains server contract.

Evidence: [models](#evidence-models), [cloud-child](#evidence-cloud-child).

### live-collaboration

**L — Peer chat** (source): Model client tool can launch a child and normally receive a new-message result; sends address remote conversations. Active goal suppresses result, cloud child lacks parent link; no general peer-room transport or observation guarantee established.

Evidence: [wiring](#evidence-wiring), [local-child](#evidence-local-child), [cloud-child](#evidence-cloud-child), [result](#evidence-result), [send](#evidence-send).

### concurrent-work

**S — Concurrency** (source): Controller supports independently running backend conversations/automation and child launches. It consumes external concurrency, and launch acknowledgement precedes actual browser/server completion.

Evidence: [wiring](#evidence-wiring), [local-child](#evidence-local-child), [cloud-child](#evidence-cloud-child), [automation-control](#evidence-automation-control).

### steering-interrupt

**S — Steer/interrupt** (source): sendEvent(run=True) and automation cancel requests are concrete controller APIs; actual mid-turn consumption, worker cancellation and provider/effect settlement are in unvendored servers.

Evidence: [send](#evidence-send), [automation-control](#evidence-automation-control).

### turn-redefinition

**? — Turn program** (inspection scope): turn_redefinition: not established beyond the inspected browser/controller/launcher/client operations; server implementation is separate and unavailable in this snapshot.

### compaction

**S — Compaction** (source): Calls conversation condense endpoint with exact runtime routing; summary strategy and retained original semantics cannot be inferred from client call.

Evidence: [condense](#evidence-condense), [condense-test](#evidence-condense-test).

### context-repair

**S — Repair** (source): Local event-cursor fork and downloaded trajectory supply repair/branching inputs; cloud fork rejected and backend-version behavior differs. No model original reconstruction implementation in this tree.

Evidence: [fork](#evidence-fork), [files](#evidence-files).

### original-audit

**L — Original audit** (source): Frontend imports/downloads backend event/trajectory views; child launch ledger only remembers handled IDs and can fail open. No complete original provider/program/transform event capture established by client controller.

Evidence: [files](#evidence-files), [dedup](#evidence-dedup), [wiring](#evidence-wiring), [result](#evidence-result).

### audit-query

**S — Audit query** (source): Controller can download trajectory and branch at selected event ID; authoritative retention/search belongs to the selected backend, not local browser claims.

Evidence: [files](#evidence-files), [fork](#evidence-fork).

### hot-change

**S — Hot change** (source): Actual runtime profile and ACP model switch requests; editable SDK launch command picks source on next invocation. Server activation and old-work handling are unavailable, not turn/workflow-conclusion guarantee.

Evidence: [models](#evidence-models), [server-command](#evidence-server-command).

### rebuild-continuity

**L — Rebuild continuity** (source): Local editable packages rebuilt on server invocation and controller can reconnect to independently hosted conversations. No independently powered compile outpost/main settlement/paused work/reinhabitation protocol traced.

Evidence: [server-command](#evidence-server-command), [role](#evidence-role), [send](#evidence-send).

### remote-services

**I — Remote** (source): Routes local and cloud conversation requests, resolves per-conversation runtime URL/session key, launches cloud child and consumes automation service. Boundaries and credentials are explicitly selected; underlying provider execution not audited.

Evidence: [send](#evidence-send), [cloud-child](#evidence-cloud-child), [create](#evidence-create), [automation-control](#evidence-automation-control).

### self-improvement

**? — Self-improve** (inspection scope): self_improvement: not established beyond the inspected browser/controller/launcher/client operations; server implementation is separate and unavailable in this snapshot.

### complaints

**? — Complaints** (inspection scope): complaints: not established beyond the inspected browser/controller/launcher/client operations; server implementation is separate and unavailable in this snapshot.

### authority

**S — Authority** (source): Browser controller invokes model-requested child with operator-selected backend/settings and workspace; launcher describes full-access local server. This does not establish model shell approval policy in separate engine.

Evidence: [role](#evidence-role), [wiring](#evidence-wiring), [local-child](#evidence-local-child), [cloud-child](#evidence-cloud-child).

### evaluation

**S — Evaluation** (source): Literal routing/request/result witnesses plus real argv passthrough test; child/worktree/condense endpoints mocked, so no engine correctness, isolation, compaction or process-tree settlement conformance.

Evidence: [child-test](#evidence-child-test), [fallback-test](#evidence-fallback-test), [replay-test](#evidence-replay-test), [condense-test](#evidence-condense-test), [process-test](#evidence-process-test).

### time-order

**L — Time/order** (source): Cloud poll uses Date.now deadline; browser call-ID dedup precedes work and proceeds if storage fails; separate event-cursor fork contract. No durable cross-client causal settlement identity.

Evidence: [cloud-wait](#evidence-cloud-wait), [dedup](#evidence-dedup), [fork](#evidence-fork).

## Inspected test oracles

- [quarantine/openhands/__tests__/services/child-conversation-launch.test.ts](../../../quarantine/openhands/__tests__/services/child-conversation-launch.test.ts): Real browser bridge wiring with mocked backend operations Oracle: Literal child request/results, worktree failure shared fallback disclosure, repeated call ID one launch, active goal no result message. Does not witness actual worktree isolation, failed localStorage or cross-tab duplicate admission. Read, **not executed**.
- [quarantine/openhands/__tests__/api/agent-server-conversation-service-condense.test.ts](../../../quarantine/openhands/__tests__/api/agent-server-conversation-service-condense.test.ts): Exact per-conversation runtime routing Oracle: Mocked ConversationClient called with runtime URL/key; missing URL fails without request. No compaction outcome or original reconstruction oracle. Read, **not executed**.
- [quarantine/openhands/__tests__/scripts/dev-process-utils.test.ts](../../../quarantine/openhands/__tests__/scripts/dev-process-utils.test.ts): Signal receipt versus child exit and shell-free argument transport Oracle: Literal unexited process remains running even killed=true; actual Node child emits exact argument containing < and exits zero. No descendants/reaping/service effect witness. Read, **not executed**.

## Useful mechanisms

- Explicit separation between controller and independently hosted agent backends.
- Actual model/browser child launch includes isolation downgrade and parent-link caveats.
- Process utility/test distinguishes successfully signalling from actual process exit.

## Material limits

- Current snapshot omits coding SDK/server implementation; cannot audit native provider/tools/kernel here.
- Browser tool acknowledgement occurs before action; goal-mode result suppression and storage fail-open affect delivery/replay.
- Remote start polling can return pending state; no complete original audit/refit promise.

## Arconaut design questions

- An outpost/colleague controller must distinguish admitted launch, running identity, result and settled work.
- Browser focus/reload/goal policy must not silently revoke model delivery custody.
- Keep consumed shared service/governance separate; study the actual software-agent SDK separately before assigning engine capabilities.

## Evidence

### Evidence role

[quarantine/openhands/README.md:1–88](../../../quarantine/openhands/README.md#L1): Current role is Agent Canvas across local/remote/cloud backends; local agent-server separately installed, filesystem authority described in setup.

### Evidence package

[quarantine/openhands/package.json:1–32](../../../quarantine/openhands/package.json#L1): Agent Canvas version/core JS module, external TypeScript client dependency; not embedded coding engine.

### Evidence pins

[quarantine/openhands/config/defaults.json:1–33](../../../quarantine/openhands/config/defaults.json#L1): Separate agent-server/automation pins, version compatibility and state-path settings.

### Evidence launcher

[quarantine/openhands/bin/agent-canvas.mjs:134–177](../../../quarantine/openhands/bin/agent-canvas.mjs#L134): Imports production static stack launcher after mode/build checks; runtime server loaded separately.

### Evidence server-command

[quarantine/openhands/scripts/dev-safe.mjs:411–518](../../../quarantine/openhands/scripts/dev-safe.mjs#L411): uvx command selects local editable SDK workspace, Git ref, explicit or default server version; coherent SDK/tools/workspace versions; local source rebuilt on invocation.

### Evidence process

[quarantine/openhands/scripts/dev-process-utils.mjs:1–37](../../../quarantine/openhands/scripts/dev-process-utils.mjs#L1): Explicit running status distinguishes sent signal from actual child exit; service spawn bypasses shell and POSIX new process group.

### Evidence signal

[quarantine/openhands/scripts/dev-process-utils.mjs:90–110](../../../quarantine/openhands/scripts/dev-process-utils.mjs#L90): Group signal/direct fallback returns signalling status; this helper does not wait for effect settlement.

### Evidence wiring

[quarantine/openhands/src/contexts/conversation-websocket-context.tsx:838–864](../../../quarantine/openhands/src/contexts/conversation-websocket-context.tsx#L838): Model client-tool ActionEvent actually dispatches browser child-launch handler; server has already acknowledged before browser work.

### Evidence dedup

[quarantine/openhands/src/services/child-conversation-launch.ts:208–231](../../../quarantine/openhands/src/services/child-conversation-launch.ts#L208): Browser localStorage claims tool ID before work; corrupt/full storage proceeds with replay risk rather than blocking launch.

### Evidence local-child

[quarantine/openhands/src/services/child-conversation-launch.ts:273–350](../../../quarantine/openhands/src/services/child-conversation-launch.ts#L273): Requests child in parent workspace; worktree failure retries shared workspace; reports isolation downgrade and best-effort title/parent compatibility.

### Evidence cloud-wait

[quarantine/openhands/src/services/child-conversation-launch.ts:366–385](../../../quarantine/openhands/src/services/child-conversation-launch.ts#L366): Cloud start polling uses wall-clock deadline and returns latest state even without final conversation ID.

### Evidence cloud-child

[quarantine/openhands/src/services/child-conversation-launch.ts:387–475](../../../quarantine/openhands/src/services/child-conversation-launch.ts#L387): Cloud child request inherits repository/settings but deliberately no server parent link; launched result can contain pending start_task_id/null conversation URL.

### Evidence result

[quarantine/openhands/src/services/child-conversation-launch.ts:482–559](../../../quarantine/openhands/src/services/child-conversation-launch.ts#L482): Actual result normally posted as new user message; active goal suppresses it to avoid ending loop; errors are caught because server already acknowledged success.

### Evidence send

[quarantine/openhands/src/api/conversation-service/agent-server-conversation-service.api.ts:420–456](../../../quarantine/openhands/src/api/conversation-service/agent-server-conversation-service.api.ts#L420): Resolves cloud runtime URL/session key then sends event with run=True via external client; returns sent message, not model completion.

### Evidence files

[quarantine/openhands/src/api/conversation-service/agent-server-conversation-service.api.ts:828–862](../../../quarantine/openhands/src/api/conversation-service/agent-server-conversation-service.api.ts#L828): Client reads confined conversation file path/downloads trajectory from selected backend; server effect/file implementation external.

### Evidence condense

[quarantine/openhands/src/api/conversation-service/agent-server-conversation-service.api.ts:903–925](../../../quarantine/openhands/src/api/conversation-service/agent-server-conversation-service.api.ts#L903): Calls selected runtime condense endpoint; rejects missing cloud runtime URL; no local summary/original-retention engine.

### Evidence fork

[quarantine/openhands/src/api/conversation-service/agent-server-conversation-service.api.ts:1021–1057](../../../quarantine/openhands/src/api/conversation-service/agent-server-conversation-service.api.ts#L1021): Requests local event-cursor fork and copies client metadata; cloud rejected, older server contract copies full history.

### Evidence models

[quarantine/openhands/src/api/conversation-service/agent-server-conversation-service.api.ts:1089–1175](../../../quarantine/openhands/src/api/conversation-service/agent-server-conversation-service.api.ts#L1089): Profile/model switch requests sent to server; local new usage identity avoids old registry entry, ACP model switching delegated to server.

### Evidence automation

[quarantine/openhands/src/api/automation-service/automation-service.api.ts:130–176](../../../quarantine/openhands/src/api/automation-service/automation-service.api.ts#L130): Maps event/cron trigger and imported prompt into automation request; timezone/default timestamp identities.

### Evidence create

[quarantine/openhands/src/api/automation-service/automation-service.api.ts:321–344](../../../quarantine/openhands/src/api/automation-service/automation-service.api.ts#L321): Automation creation routed through pinned cloud/local backend client; external service owns execution.

### Evidence automation-control

[quarantine/openhands/src/api/automation-service/automation-service.api.ts:429–466](../../../quarantine/openhands/src/api/automation-service/automation-service.api.ts#L429): Dispatch/cancel requests return external service run representation, not direct OS lifecycle proof.

### Evidence child-test

[quarantine/openhands/__tests__/services/child-conversation-launch.test.ts:214–254](../../../quarantine/openhands/__tests__/services/child-conversation-launch.test.ts#L214): Mock API oracle checks requested parent worktree and exact returned child ID/status/workspace result.

### Evidence fallback-test

[quarantine/openhands/__tests__/services/child-conversation-launch.test.ts:343–378](../../../quarantine/openhands/__tests__/services/child-conversation-launch.test.ts#L343): Mock worktree failure requires shared retry and explicit isolation downgrade in model-visible result.

### Evidence replay-test

[quarantine/openhands/__tests__/services/child-conversation-launch.test.ts:559–610](../../../quarantine/openhands/__tests__/services/child-conversation-launch.test.ts#L559): Repeated tool ID does not launch twice; active goal starts child but no result posted. Storage failure and cross-tab races not covered.

### Evidence condense-test

[quarantine/openhands/__tests__/api/agent-server-conversation-service-condense.test.ts:64–121](../../../quarantine/openhands/__tests__/api/agent-server-conversation-service-condense.test.ts#L64): Mock client oracle checks direct cloud runtime URL/session key and missing-URL failure; no summary fidelity/original retention oracle.

### Evidence process-test

[quarantine/openhands/__tests__/scripts/dev-process-utils.test.ts:12–57](../../../quarantine/openhands/__tests__/scripts/dev-process-utils.test.ts#L12): Sent signal with unexited child remains running; actual spawned Node returns literal metacharacter argument; no descendant shutdown test here.

