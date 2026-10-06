# letta-code

Operator/model-configurable harness with default unrestricted mode, executable mods, background workflow programs, peer messaging, managed context and mod-learning candidates.

Role: stateful coding harness/App Server/channel runtime. Runtime: TypeScript, JavaScript.

Pinned source: [https://github.com/letta-ai/letta-code](https://github.com/letta-ai/letta-code); revision/version `96eb977b9477045423005a3630e5f9293140249a`.

Node/Bun event-loop host with local or remote backend, pi provider adapters, client tools/background processes/workflows and scoped mods.

Owns harness conversation/agent runtime, local transcript/memory files and managed children; consumes optional Cloud/providers/SDK/remote computers. Local and Cloud stores are different boundaries.

Inspection: local provider path/headless approval-return loop, tool manager/shell/workflow, local store compaction, mod import/reload/hooks/learning, peer sends and direct source test assertions

Limits of this study: Large TS harness; inspected concrete mechanisms only, not complete App Server/channel/Cloud engine. SDK/pi dependency internals and Cloud behavior are not inferred.

## Actions

### headless/interactive send -> ProviderTurnExecutor

Surface: operator/backend API.

Input: agent/conversation input and tool/provider settings

Result: stream chunks/model call IDs plus pending client tool calls

Lifecycle: client executes results and continues unless interrupted

Authority: backend/provider credentials and scoped tool authority

Evidence: [e1](#evidence-e1), [e2](#evidence-e2), [e3](#evidence-e3).

### executeTool / PreToolUse / mod tool_start,tool_end

Surface: model/program dispatch.

Input: native/mod tool name,args, call/context ID and parent scope

Result: tool result/error or rewritten/blocked input/result

Lifecycle: hooks and mod checks around execution; foreground AbortSignal

Authority: unrestricted default or configured policy; scoped context

Evidence: [e3](#evidence-e3), [e4](#evidence-e4), [e18](#evidence-e18), [e24](#evidence-e24).

### Bash / background task tools

Surface: model tools.

Input: command/cwd/env/timeout/run_in_background

Result: stdout/stderr or bash/task ID and output file

Lifecycle: background intentionally outlives turn abort; explicit later control needed

Authority: shell sandbox/permission mode; invocation secrets scrubbed

Evidence: [e5](#evidence-e5), [e6](#evidence-e6).

### Workflow

Surface: model tool.

Input: JS script/scriptPath/args/model/maxConcurrent/allowedTools

Result: background workflow ID/progress/result notification and journal directory

Lifecycle: VM program; every launched subagent drained even unawaited or abort

Authority: exposed tool policy plus SDK subagent allowed roster

Evidence: [e7](#evidence-e7), [e8](#evidence-e8), [e9](#evidence-e9), [e10](#evidence-e10).

### workflow agent / decide / parallel / pipeline

Surface: workflow program API.

Input: prompt/model/tool/options or state/decision schema; optional conversation continuation ID

Result: raw worker result/outcome/token accounting or structured decision

Lifecycle: bounded concurrency/counts; original script/outcome persisted, no replay from journal

Authority: authored workflow plus spawner service authority

Evidence: [e7](#evidence-e7), [e8](#evidence-e8), [e9](#evidence-e9), [e10](#evidence-e10).

### SendAgentMessage

Surface: model peer tool.

Input: target agent/conversation/computer and message

Result: accepted steer turn receipt or tracked followup completion/task

Lifecycle: Letta enqueue or external Claude/Codex existing session; same computer

Authority: sender from runtime context, not arbitrary forged sender

Evidence: [e22](#evidence-e22), [e23](#evidence-e23).

### mod activate API + reload

Surface: operator/program extension.

Input: scoped TS/JS factory registering tools/providers/commands/hooks/permissions

Result: owner/generation registry snapshot and diagnostic/disposer

Lifecycle: mtime import cache; dispose/abort old generation then publish new during load

Authority: mod API capability flags; executable code trusted within selected scope

Evidence: [e14](#evidence-e14), [e15](#evidence-e15), [e16](#evidence-e16), [e17](#evidence-e17), [e18](#evidence-e18).

### compactConversationAll / compaction event

Surface: operator/backend policy.

Input: conversation/agent, summary, kept tail and stats

Result: new context IDs plus append-only parent-linked compaction record

Lifecycle: model summary request separate; prior local transcript retained

Authority: configured compaction settings/backend

Evidence: [e11](#evidence-e11), [e12](#evidence-e12), [e13](#evidence-e13).

### mods learn / runModLearning

Surface: operator experiment command.

Input: objective/spec/evaluation scenarios/candidate diversity/max candidates/promote path

Result: per-candidate reports/history/artifacts, score and optional promoted mod

Lifecycle: generation→behavior checks/markers→best passed optional copy; not deployment barrier

Authority: operator-specified run/promote scope; model candidate code may be evaluated in subprocess

Evidence: [e19](#evidence-e19), [e20](#evidence-e20), [e21](#evidence-e21).

## Capabilities

### filesystem

**I — Files** (source): Coding tools/shell run locally with scoped context, background IDs/output files and explicit permission/hook dispatch; shared remote services stay independently governed.

Evidence: [e3](#evidence-e3), [e4](#evidence-e4), [e5](#evidence-e5).

### processes

**I — OS programs** (source): Coding tools/shell run locally with scoped context, background IDs/output files and explicit permission/hook dispatch; shared remote services stay independently governed.

Evidence: [e3](#evidence-e3), [e4](#evidence-e4), [e5](#evidence-e5).

### code-actions

**I — Code actions** (source): Model Workflow executes plain JavaScript orchestration with agent/decision/parallel primitives in VM; shell executes ordinary programs. JS host remains event-loop based.

Evidence: [e7](#evidence-e7), [e8](#evidence-e8), [e9](#evidence-e9).

### persistent-kernel

**L — Kernel** (source): Background process Maps/VM script are per host; persistent output/transcript does not retain computation heap. Shared kernel would be separately consumed.

Evidence: [e5](#evidence-e5), [e6](#evidence-e6), [e9](#evidence-e9).

### standing-database

**I — Standing DB** (source): Local agent/conversation JSONL and MemFS state plus selectable remote backend; append-compaction retains originals while context IDs change. Not one universal Cloud/local DB.

Evidence: [e11](#evidence-e11), [e12](#evidence-e12), [e13](#evidence-e13).

### workflow-programming

**I — Workflows** (source): Native background Workflow JS programs spawn/resume subagents, decide, parallel/pipeline with limits, cancellation and drain settlement; journal is explicitly not replay engine.

Evidence: [e7](#evidence-e7), [e8](#evidence-e8), [e9](#evidence-e9), [e10](#evidence-e10).

### multi-model

**I — Models** (source): Workflow model/decision settings, provider-registering mods, and external Claude/Codex sessions compose multiple models, not only picker.

Evidence: [e7](#evidence-e7), [e15](#evidence-e15), [e22](#evidence-e22), [e23](#evidence-e23).

### live-collaboration

**I — Peer chat** (source): SendAgentMessage uses target agent/conversation and external session steering with completion/interrupt handles; scope/source governs sender and original computer.

Evidence: [e22](#evidence-e22), [e23](#evidence-e23).

### concurrent-work

**I — Concurrency** (source): Background shells/subagents/workflows and semaphore-bounded concurrent calls; drains launched workers even if not awaited by script.

Evidence: [e5](#evidence-e5), [e7](#evidence-e7), [e8](#evidence-e8), [e9](#evidence-e9).

### steering-interrupt

**L — Steer/interrupt** (source): Abort propagates foreground tools/workflow and peer active steering has receipt; deliberate background shell outlives turn AbortSignal, mods disposal is not a whole-work barrier.

Evidence: [e2](#evidence-e2), [e3](#evidence-e3), [e5](#evidence-e5), [e9](#evidence-e9), [e16](#evidence-e16), [e23](#evidence-e23).

### turn-redefinition

**I — Turn program** (source): Executable mods register commands/tools/providers/permissions and transform turn-start/tool-start/end; hooks can block/rewrite tool inputs. Certain LLM/compaction events are observation only.

Evidence: [e4](#evidence-e4), [e14](#evidence-e14), [e15](#evidence-e15), [e18](#evidence-e18).

### compaction

**I — Compaction** (source): Local all/sliding compaction and configurable prompt/model separate transcript append from context IDs; source tests preserve prior originals and rebuild system context.

Evidence: [e11](#evidence-e11), [e12](#evidence-e12), [e13](#evidence-e13), [e26](#evidence-e26).

### context-repair

**S — Repair** (source): Append-only prior transcript/first-kept linkage and separate projected context permit recall/reconstruction; full automatic model-governed corrupted-context repair not inferred from storage.

Evidence: [e11](#evidence-e11), [e12](#evidence-e12), [e13](#evidence-e13).

### original-audit

**L — Original audit** (source): Original transcript and workflow script/args/outcomes retained; secrets scrubbed, observation scopes bounded, Maps/remote dependencies and raw transport/OS byte archive not universal.

Evidence: [e5](#evidence-e5), [e6](#evidence-e6), [e10](#evidence-e10), [e13](#evidence-e13), [e18](#evidence-e18).

### audit-query

**S — Audit query** (source): Transcript/parent-linked entries and workflow journals queryable by consumers; not complete original core audit DB merely because local history search exists.

Evidence: [e10](#evidence-e10), [e13](#evidence-e13).

### hot-change

**L — Hot change** (source): Mods transpile/import/register and reload generations; disposal/new registry published before load finishes, no active-work draining and no default turn/workflow-conclusion gate in this loader.

Evidence: [e15](#evidence-e15), [e16](#evidence-e16), [e17](#evidence-e17).

### rebuild-continuity

**L — Rebuild continuity** (source): Persistent logical transcript/memory and peer continuation IDs survive future sessions; live background process handles are in-memory and mod reload not compiled full-harness outpost refit.

Evidence: [e6](#evidence-e6), [e10](#evidence-e10), [e13](#evidence-e13), [e16](#evidence-e16), [e23](#evidence-e23).

### remote-services

**I — Remote** (source): Provider stream/local/remote backend, SDK workflow agents and remote/external session messages actual consuming boundaries; Cloud engine not available here.

Evidence: [e1](#evidence-e1), [e7](#evidence-e7), [e22](#evidence-e22).

### self-improvement

**I — Self-improve** (source): Operator mods learn runs candidate generation/evaluation with behavioral assertions/markers and optional passed-selected promotion. This bounded mod optimization is not general self-improvement correctness.

Evidence: [e19](#evidence-e19), [e20](#evidence-e20), [e21](#evidence-e21).

### complaints

**S — Complaints** (source): Mod diagnostics, workflow outcomes and task IDs offer attributable fault records; full snapshot+external bead-table rageshake not established.

Evidence: [e10](#evidence-e10), [e14](#evidence-e14), [e17](#evidence-e17).

### authority

**I — Authority** (source): Default unrestricted; scoped modes and mod permission/PreToolUse policies allow sophisticated configuration. External sender scope and computer constraints are explicit.

Evidence: [e4](#evidence-e4), [e22](#evidence-e22), [e24](#evidence-e24).

### evaluation

**I — Evaluation** (source): Workflow drain oracle gates slow worker; transcript preservation exact IDs; mod-learning checks declared args/markers. No executed conformance or semantic model evaluation claimed.

Evidence: [e19](#evidence-e19), [e25](#evidence-e25), [e26](#evidence-e26).

### time-order

**I — Time/order** (source): Transcript entry IDs/parents differ from message IDs, workflow task completion drains workers, mods generation protects stale load, peer turn/completion handles separate states.

Evidence: [e9](#evidence-e9), [e13](#evidence-e13), [e17](#evidence-e17), [e23](#evidence-e23).

## Inspected test oracles

- [quarantine/letta-code/src/tools/workflow/workflow-engine.test.ts](../../../quarantine/letta-code/src/tools/workflow/workflow-engine.test.ts): unawaited work settlement and cancellation Oracle: Gated spawner exact unsettled-before-release, final tokens/results; abort tests inspect outcome/journal. Fake spawner contract not arbitrary remote effect stop. Read, **not executed**.
- [quarantine/letta-code/src/mods/mod-engine.test.ts](../../../quarantine/letta-code/src/mods/mod-engine.test.ts): mod registry owner/generation and shadowing Oracle: Actual temp module import registration compared exact loaded paths/owner/generation; tests are not run here and do not ensure active programs quiesce on reload. Read, **not executed**.
- [quarantine/letta-code/src/backend/local-backend.test.ts](../../../quarantine/letta-code/src/backend/local-backend.test.ts): compaction originals and reconstruction Oracle: Exact original entry IDs retained, new parent-linked compaction append and backend reload; mocked summary/provider cannot judge semantic fidelity. Read, **not executed**.

## Useful mechanisms

- Model-ergonomic executable mods and Workflow supply substantial programming surfaces.
- Default unrestricted policy and inter-agent/external-session receipts directly fit expert-operator aims.
- Separate original transcript/compaction and explicit drain of launched workflow workers are useful patterns.
- Mod-learning evaluates declared behavior before optional selected promotion.

## Material limits

- Node/Bun TS event-loop architecture conflicts with operator preference; study does not select it.
- Mod reload clears old registry before new activation and does not wait all external effects.
- Workflow journal is debug record, not durable replay; process handles in-memory; Cloud boundaries separate.

## Arconaut design questions

- Can owned systems-runtime continuation retain this mod/workflow ergonomics without event-loop dependence?
- How do model changes stage until turn/affected workflows finish while interrupt/apply-now settles every own action?
- Which mod-learning oracles are independent of generated candidate and attack real task quality?
- Can all original actions/provider requests be retained before redaction/truncation while operator-visible secrets stay protected?

## Evidence

### Evidence e1

[quarantine/letta-code/src/backend/dev/provider-turn-executor.ts:552–565](../../../quarantine/letta-code/src/backend/dev/provider-turn-executor.ts#L552): Provider executor builds turn input, adapter streams and projects provider context usage into Letta chunks.

### Evidence e2

[quarantine/letta-code/src/headless.ts:2635–2673](../../../quarantine/letta-code/src/headless.ts#L2635): Headless executes client approval batch, exposes paired tool events, checks interrupt before sending results back and continues backend round.

### Evidence e3

[quarantine/letta-code/src/agent/approval-execution.ts:207–230](../../../quarantine/letta-code/src/agent/approval-execution.ts#L207): Approved tool passes AbortSignal, exact call/context IDs and parent scope to shared executeTool.

### Evidence e4

[quarantine/letta-code/src/tools/manager.ts:2220–2280](../../../quarantine/letta-code/src/tools/manager.ts#L2220): Mod permission check precedes PreToolUse hook; hooks can block/rewrite input, cancellation signal injected only internally.

### Evidence e5

[quarantine/letta-code/src/tools/impl/bash.ts:396–438](../../../quarantine/letta-code/src/tools/impl/bash.ts#L396): Background shell has ID/output file, launcher/sandbox/env and timeout; background mode deliberately does not inherit caller abort signal; output streamed/scrubbed.

### Evidence e6

[quarantine/letta-code/src/tools/impl/process_manager.ts:15–79](../../../quarantine/letta-code/src/tools/impl/process_manager.ts#L15): Background process/task identity/handles are in-memory Maps; outputFile optional persisted data does not preserve live handle across restart.

### Evidence e7

[quarantine/letta-code/src/tools/impl/workflow.ts:1–90](../../../quarantine/letta-code/src/tools/impl/workflow.ts#L1): Model Workflow validates/starts JS program as background task, progress/completion bridge; SDK agents consume model/tool settings and return task ID.

### Evidence e8

[quarantine/letta-code/src/tools/workflow/workflow-engine.ts:63–105](../../../quarantine/letta-code/src/tools/workflow/workflow-engine.ts#L63): Every agent() promise tracked even if script forgets await; bounded semaphore and total agent/decision caps, separate decision primitive.

### Evidence e9

[quarantine/letta-code/src/tools/workflow/workflow-engine.ts:330–384](../../../quarantine/letta-code/src/tools/workflow/workflow-engine.ts#L330): Workflow VM executes plain JS; script result/error/abort drains every launched subagent before settlement. Signal honoring is a spawner contract.

### Evidence e10

[quarantine/letta-code/src/tools/workflow/journal.ts:1–76](../../../quarantine/letta-code/src/tools/workflow/journal.ts#L1): Persists original script/args and completed subagent outcomes in owner-only run directory; source explicitly says nothing replays from journal.

### Evidence e11

[quarantine/letta-code/src/backend/local/local-store.ts:1157–1222](../../../quarantine/letta-code/src/backend/local/local-store.ts#L1157): Compaction updates resident/context IDs but appends transcript compaction event with previous message linkage, not rewrite from bounded resident tail.

### Evidence e12

[quarantine/letta-code/src/backend/local/local-store.ts:2935–2974](../../../quarantine/letta-code/src/backend/local/local-store.ts#L2935): Persistence distinguishes append-compaction from explicit rewrite; guard prevents bounded resident tail from truncating original transcript.

### Evidence e13

[quarantine/letta-code/src/backend/local/local-store.ts:3006–3045](../../../quarantine/letta-code/src/backend/local/local-store.ts#L3006): Compaction append retains parent and first-kept IDs, timestamps/stats; JSONL append preserves earlier entries and rebuild mapping.

### Evidence e14

[quarantine/letta-code/src/mods/mod-engine.ts:1374–1417](../../../quarantine/letta-code/src/mods/mod-engine.ts#L1374): Scoped mod discovery creates owner/generation/capabilities and abort-controller registry; extension active per source.

### Evidence e15

[quarantine/letta-code/src/mods/mod-engine.ts:1440–1482](../../../quarantine/letta-code/src/mods/mod-engine.ts#L1440): TS mod transpiled/imported with mtime cache key; activates API and registers disposer.

### Evidence e16

[quarantine/letta-code/src/mods/mod-engine.ts:1678–1709](../../../quarantine/letta-code/src/mods/mod-engine.ts#L1678): Disposal aborts owners, invokes synchronous disposers, unregisters tools/providers/permissions and clears cache; no await of all owned external work.

### Evidence e17

[quarantine/letta-code/src/mods/mod-engine.ts:1739–1788](../../../quarantine/letta-code/src/mods/mod-engine.ts#L1739): Reload disposes active registry and publishes empty/new generations during load; stale generation cannot overwrite current registry. Not transactional old-to-new with unchanged availability.

### Evidence e18

[quarantine/letta-code/src/mods/types.ts:316–345](../../../quarantine/letta-code/src/mods/types.ts#L316): Mod turn/tool results can alter input/output; LLM/compaction boundary events are observation callbacks, not raw provider transport control.

### Evidence e19

[quarantine/letta-code/src/mods/learning-harness.ts:1120–1172](../../../quarantine/letta-code/src/mods/learning-harness.ts#L1120): Mod-learning behavioral assertions check handler count, diagnostics and exact argument preservation; pass determined by all checks.

### Evidence e20

[quarantine/letta-code/src/mods/learning-harness.ts:2350–2378](../../../quarantine/letta-code/src/mods/learning-harness.ts#L2350): Selects best candidate report; optional promotion copies only if selected report passed. Evaluation score is declared marker/assertion scope, not general coding quality.

### Evidence e21

[quarantine/letta-code/src/cli/commands/mods.ts:755–819](../../../quarantine/letta-code/src/cli/commands/mods.ts#L755): Operator mods learn command actually invokes candidate harness with configured spec/candidate/promote options; not an unwired helper.

### Evidence e22

[quarantine/letta-code/src/tools/impl/send-agent-message.ts:52–105](../../../quarantine/letta-code/src/tools/impl/send-agent-message.ts#L52): Peer message derives authenticated runtime sender/conversation; target may be Letta or external Claude/Codex session, original-computer constraint explicit.

### Evidence e23

[quarantine/letta-code/src/tools/impl/send-agent-message.ts:101–132](../../../quarantine/letta-code/src/tools/impl/send-agent-message.ts#L101): Codex in-flight steering returns accepted turn handle; new followup requires completion and interrupt handles before background tracking.

### Evidence e24

[quarantine/letta-code/src/permissions/mode.ts:1–18](../../../quarantine/letta-code/src/permissions/mode.ts#L1): Current default permission mode is unrestricted; other standard/acceptEdits/strict modes configurable.

### Evidence e25

[quarantine/letta-code/src/tools/workflow/workflow-engine.test.ts:373–399](../../../quarantine/letta-code/src/tools/workflow/workflow-engine.test.ts#L373): Direct test gates un-awaited slow worker and asserts workflow unsettled until worker completes and token accounting included.

### Evidence e26

[quarantine/letta-code/src/backend/local-backend.test.ts:799–824](../../../quarantine/letta-code/src/backend/local-backend.test.ts#L799): Local compaction test asserts original message entries remain with distinct entry IDs, appended parent-linked summary, then reloads backend.

