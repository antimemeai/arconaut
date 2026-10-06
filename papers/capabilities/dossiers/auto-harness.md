# auto-harness

Governance-first tool middleware with constitution/prompt compiler and a main loop whose actual path differs materially from full advertised pipeline.

Role: governance middleware plus partial agent-loop framework. Runtime: Python.

Pinned source: [https://github.com/aiming-lab/AutoHarness](https://github.com/aiming-lab/AutoHarness); revision/version `3561e468f9ca9f9bf282512e695bd32e4e90fef4`.

Sync AgentLoop/callback provider, separate synchronous/async governance pipelines and supporting task/concurrency utilities.

Owns permission/risk/hook/audit summaries and optional transcript/session briefing files; tool effects supplied by caller callbacks. Supporting fork/background/orchestrator objects are not assumed wired into AgentLoop.

Inspection: AgentLoop/provider/tool callback path, evaluate vs process/async pipeline, audit originals/query limits, compaction/transcript/resume, supporting concurrency/fork/background and direct source assertions

Limits of this study: No upstream execution. Main-loop integration bounded to traced AgentLoop; wrappers/CLI/integrations and every verification rule not exhaustively traced. Advertisement tests/counts not accepted as conformance.

## Actions

### AgentLoop.run / step

Surface: operator/program agent API.

Input: task or current messages + registered tools/provider callback

Result: final text or response/tool results

Lifecycle: run fresh local context, capped iterations; serial callbacks via precheck

Authority: constitution precheck; ask noninteractive denied; original args executed despite replacement precheck

Evidence: [e1](#evidence-e1), [e2](#evidence-e2), [e3](#evidence-e3), [e5](#evidence-e5), [e6](#evidence-e6).

### ToolRegistry.register / unregister / get / to_api_schema

Surface: support/model discovery API.

Input: ToolDefinition name/schema/flags/aliases/execute callback

Result: tool schemas/registered definition or lookup miss

Lifecycle: mutable registry; run captures schemas once

Authority: caller implements tool behavior; names/flags are not actual shell/filesystem primitives

Evidence: [e1](#evidence-e1), [e19](#evidence-e19).

### ToolGovernancePipeline.evaluate / process / aprocess

Surface: governance middleware program API.

Input: ToolCall and config/hooks/executor

Result: PermissionDecision only or ToolResult/audit when full process used

Lifecycle: evaluate no execute/posthooks; async full path separate; no generic external-effect replay

Authority: operator constitution and hooks; callers must propagate transformed input correctly

Evidence: [e5](#evidence-e5), [e6](#evidence-e6), [e7](#evidence-e7).

### ToolOrchestrator.execute_batch

Surface: supplied concurrency primitive.

Input: calls and async executor, registry safety flags/concurrency cap

Result: results submission order/errors

Lifecycle: safe subset concurrently FIRST then unsafe serial; not main-loop wired

Authority: caller declares concurrency safety; external side effects opaque

Evidence: [e13](#evidence-e13), [e2](#evidence-e2).

### AutoCompactor.compact / microcompact

Surface: wired context-management primitive.

Input: messages/summarizer/token budget or keep_recent

Result: summary+new projected messages/token estimates

Lifecycle: side summary callback, copied placeholder old results, no durable originals archive guarantee

Authority: operator mode/threshold, summary quality delegated

Evidence: [e1](#evidence-e1), [e11](#evidence-e11), [e12](#evidence-e12).

### AuditEngine.log / get_records / stream_records

Surface: governance audit/support query API.

Input: call/risk/hooks/permission/result; session/type/limit

Result: hashed-input execution summary JSONL/parsed records

Lifecycle: append flush, malformed rows skipped; retention/rotation separately available

Authority: explicitly enabled audit, not main evaluate path nor full original custody

Evidence: [e8](#evidence-e8), [e9](#evidence-e9).

### TranscriptWriter.append / resume_session / PromptCompiler.compile

Surface: conversation/briefing/prompt support APIs.

Input: normalized message or saved session/constitution

Result: JSONL selected content, briefing text or governance prompt addendum

Lifecycle: file append vs task-list resume vs text compilation; no heap/request continuation

Authority: operator configuration/session folder; compiler is prompt formatting

Evidence: [e10](#evidence-e10), [e16](#evidence-e16), [e17](#evidence-e17).

### build_forked_messages / BackgroundAgentManager.register,complete,fail,drain_notifications

Surface: delegation/task SUPPORT primitives.

Input: parent messages/directive or description/agent ID/output/error

Result: cloned context/placeholder tool pairs or task/notification/output file

Lifecycle: constructs/tracks records but no actual launch in class; resident map

Authority: caller must own actual child launch/settlement; not main-loop multiagent claim

Evidence: [e14](#evidence-e14), [e15](#evidence-e15).

### verify_session / VerificationEngine.verify

Surface: programmatic evaluation primitive.

Input: claimed result and reconstructed audit or supplied original ToolCall/ToolResult arrays

Result: aggregate rule verdict/evidence/issues

Lifecycle: checks supplied evidence; audit-based reconstruction strips original input and output

Authority: caller rule/context authority; cannot infer execution success from sparse audit

Evidence: [e18](#evidence-e18).

## Capabilities

### filesystem

**S — Files** (source): Caller can register filesystem callbacks; registry names/governance path checks do not implement native filesystem operations automatically.

Evidence: [e3](#evidence-e3), [e19](#evidence-e19).

### processes

**S — OS programs** (source): Caller-supplied execute callbacks may run shell/processes; main source no owned process handles or default launcher supplied.

Evidence: [e3](#evidence-e3), [e19](#evidence-e19).

### code-actions

**S — Code actions** (source): Arbitrary host callback code and middleware execution APIs; selected agent outputs JSON tools, not persistent model-authored code kernel.

Evidence: [e2](#evidence-e2), [e3](#evidence-e3), [e7](#evidence-e7).

### persistent-kernel

**L — Kernel** (source): Main run starts fresh local conversation; support background objects track status but no launch/heap persistence routine, no persistent computation protocol.

Evidence: [e1](#evidence-e1), [e14](#evidence-e14), [e16](#evidence-e16).

### standing-database

**S — Standing DB** (source): Governance JSONL/session/transcript files retain records; no general shared transactional database and summaries omit original bodies.

Evidence: [e8](#evidence-e8), [e9](#evidence-e9), [e10](#evidence-e10), [e16](#evidence-e16).

### workflow-programming

**L — Workflows** (source): Registered callbacks/provider replacement and separately exposed pipelines/orchestrator/fork constructors; main run sequential evaluate+execute bypasses full process/hook/sanitize/audit path.

Evidence: [e1](#evidence-e1), [e2](#evidence-e2), [e3](#evidence-e3), [e5](#evidence-e5), [e7](#evidence-e7), [e13](#evidence-e13), [e15](#evidence-e15).

### multi-model

**S — Models** (source): Replaceable LLM callback can route models; selected native AgentLoop one model, fork constructor no child launch. No inferred live multi-model engine from support utilities.

Evidence: [e4](#evidence-e4), [e15](#evidence-e15).

### live-collaboration

**L — Peer chat** (source): Fork prompt construction/background completion notifications exist as supports; no main-loop launch/mail/room wiring traced.

Evidence: [e2](#evidence-e2), [e14](#evidence-e14), [e15](#evidence-e15).

### concurrent-work

**L — Concurrency** (source): Exposed async orchestrator concurrent safe group then unsafe serial and original return ordering; AgentLoop constructs but does not use it in traced run, executes callbacks serially.

Evidence: [e2](#evidence-e2), [e13](#evidence-e13).

### steering-interrupt

**L — Steer/interrupt** (source): Separate pipeline turn/async governor supports policy boundaries, but selected synchronous run has no queue/provider interrupt/drain contract and background manager no process cancellation.

Evidence: [e1](#evidence-e1), [e2](#evidence-e2), [e7](#evidence-e7), [e14](#evidence-e14).

### turn-redefinition

**L — Turn program** (source): Provider callback/prompt/registry and pre/post hooks configurable; evaluate may decide on modified replacement while AgentLoop executes original input and skips post-hook/process chain.

Evidence: [e1](#evidence-e1), [e2](#evidence-e2), [e3](#evidence-e3), [e5](#evidence-e5), [e6](#evidence-e6), [e7](#evidence-e7), [e19](#evidence-e19).

### compaction

**I — Compaction** (source): Main wired auto-compaction and microcompact old-tool clearing; summary+canned acknowledgment/current user without archived lineage, pair constraints depend helper/mode.

Evidence: [e1](#evidence-e1), [e11](#evidence-e11), [e12](#evidence-e12).

### context-repair

**S — Repair** (source): Optional transcript can reconstruct previous conversation; reactive/output recovery support names not automatic semantic repair. Reader skips malformed rows and compaction lacks strict integrity lineage.

Evidence: [e10](#evidence-e10), [e11](#evidence-e11), [e12](#evidence-e12), [e16](#evidence-e16).

### original-audit

**L — Original audit** (source): Governance audit deliberately hashes input and stores size/status not bodies; AgentLoop precheck path skips full audit. Optional transcript preserves selected normalized content but provider conversion omits other raw blocks.

Evidence: [e2](#evidence-e2), [e4](#evidence-e4), [e5](#evidence-e5), [e8](#evidence-e8), [e10](#evidence-e10).

### audit-query

**I — Audit query** (source): get_records/stream/summary filters governance record session/type; skips malformed lines and cannot recover raw originals from hashes.

Evidence: [e8](#evidence-e8), [e9](#evidence-e9).

### hot-change

**S — Hot change** (source): Registry register/unregister mutable supplied APIs; system prompt/tools built once per run and no executable hotreload/activation barrier traced.

Evidence: [e1](#evidence-e1), [e19](#evidence-e19).

### rebuild-continuity

**L — Rebuild continuity** (source): Session resume gives task briefing; background task Map resident; no live provider/callback heap/outpost restore or shared kernel reattach.

Evidence: [e14](#evidence-e14), [e16](#evidence-e16).

### remote-services

**S — Remote** (source): Replaceable provider callback/direct Anthropic SDK and arbitrary callback tools can consume external services; service runtime governance not supplied by this wrapper.

Evidence: [e3](#evidence-e3), [e4](#evidence-e4).

### self-improvement

**L — Self-improve** (source): Harness engineering name refers governance construction; PromptCompiler emits text and verification scores rule evidence, not model-governed candidate harness experimentation/promotion.

Evidence: [e17](#evidence-e17), [e18](#evidence-e18).

### complaints

**S — Complaints** (source): Trace/audit-error/session failed lists support structured diagnostics; selected path no model-triggered external complaint snapshot/table contract.

Evidence: [e8](#evidence-e8), [e10](#evidence-e10), [e16](#evidence-e16).

### authority

**L — Authority** (source): Constitution risk/permission/prehook decisions; main noninteractive ask becomes error, transformed input may be checked but original executed, so normal main path requires critical integration repair before reuse.

Evidence: [e2](#evidence-e2), [e3](#evidence-e3), [e5](#evidence-e5), [e6](#evidence-e6).

### evaluation

**L — Evaluation** (source): verify_session sees empty tool input/body-less output from audit; tests assert audit fields and loop routing, denial oracle admits every str result and fails to prove blocking.

Evidence: [e8](#evidence-e8), [e18](#evidence-e18), [e20](#evidence-e20).

### time-order

**L — Time/order** (source): Flush/lock JSONL timestamps and ordered batch results; safe-first orchestrator reorders effect execution vs submissions, no side-effect intent/settlement atomicity.

Evidence: [e8](#evidence-e8), [e10](#evidence-e10), [e13](#evidence-e13).

## Inspected test oracles

- [quarantine/auto-harness/tests/test_agent_loop.py](../../../quarantine/auto-harness/tests/test_agent_loop.py): loop/tool routing and advertised governance integration Oracle: Fake callback and SafeTool prove result reaches subsequent provider request. Denial test assertion includes isinstance(result,str), so any normal string output passes; does not assert denied callback not invoked, transformed-input execution or posthook/audit coverage. Read, **not executed**.
- [quarantine/auto-harness/tests/test_audit.py](../../../quarantine/auto-harness/tests/test_audit.py): structured governance field serialization Oracle: Manual AuditEngine.log fixture asserts event/tool/hook/sanitized summaries and counts; tests isolated engine, not that AgentLoop logs actual executions or preserves raw bodies. Read, **not executed**.

## Useful mechanisms

- Supplied middleware APIs and callback registry are explicit and easily inspectable.
- Governance query/audit and transcript/briefing roles are separate in source.
- Context projection works on copies rather than pretending all memory is magic.

## Material limits

- Main AgentLoop only evaluate-prechecks then executes original callback args; replacement args returned nowhere and posthooks/full audit not wired.
- Orchestrator/fork/background support does not prove actual concurrent agent launch; background manager is tracking, not executor.
- Governance audit summaries/hash-only inputs cannot support core original study; trivial denial test weakens advertised coverage.

## Arconaut design questions

- How do action transformation APIs return exact effective arguments and retain both representations so the executor cannot silently use pre-transform data?
- Can main integration tests prove intended paths run and prohibited/failed paths do not, instead of checking any string output?
- Which support modules are actually reachable normal-operation capabilities, and what minimal wiring belongs in baseline?

## Evidence

### Evidence e1

[quarantine/auto-harness/autoharness/agent_loop.py:251–309](../../../quarantine/auto-harness/autoharness/agent_loop.py#L251): Each run builds system prompt/tool schemas once and fresh local messages; auto/micro compaction then provider, optional transcript task logging.

### Evidence e2

[quarantine/auto-harness/autoharness/agent_loop.py:328–435](../../../quarantine/auto-harness/autoharness/agent_loop.py#L328): Main loop records assistant/tool transcript, calls pipeline.evaluate then direct _execute_tool serially, denies ask noninteractive; no process pipeline/orchestrator call here.

### Evidence e3

[quarantine/auto-harness/autoharness/agent_loop.py:703–738](../../../quarantine/auto-harness/autoharness/agent_loop.py#L703): Direct registered callback executes original tool input; absent execute callback returns explanatory text; exceptions model-visible error.

### Evidence e4

[quarantine/auto-harness/autoharness/agent_loop.py:632–701](../../../quarantine/auto-harness/autoharness/agent_loop.py#L632): Provider callback replaceable, otherwise Anthropic SDK call creates client; converts only text/tool_use/selected usage, other raw blocks omitted.

### Evidence e5

[quarantine/auto-harness/autoharness/core/pipeline.py:466–510](../../../quarantine/auto-harness/autoharness/core/pipeline.py#L466): evaluate is precheck only/no execution/no post-hooks; replacement frozen ToolCall local, returns only PermissionDecision rather than transformed input.

### Evidence e6

[quarantine/auto-harness/autoharness/core/pipeline.py:1066–1080](../../../quarantine/auto-harness/autoharness/core/pipeline.py#L1066): Hook modify returns new frozen call with original metadata; main AgentLoop still passes original input to execution.

### Evidence e7

[quarantine/auto-harness/autoharness/core/pipeline.py:560–601](../../../quarantine/auto-harness/autoharness/core/pipeline.py#L560): Separate async process full executor/posthook/audit route, independent from AgentLoop evaluate path.

### Evidence e8

[quarantine/auto-harness/autoharness/core/audit.py:200–284](../../../quarantine/auto-harness/autoharness/core/audit.py#L200): Audit input hash only and output status/size/duration/error summary; no raw input/output body; lock flush not complete effect journal.

### Evidence e9

[quarantine/auto-harness/autoharness/core/audit.py:292–340](../../../quarantine/auto-harness/autoharness/core/audit.py#L292): Governance query filters session/event and reverse-limit; skips malformed records, not lossless integrity enforcement.

### Evidence e10

[quarantine/auto-harness/autoharness/session/transcript.py:18–99](../../../quarantine/auto-harness/autoharness/session/transcript.py#L18): Optional append/flush JSONL conversation transcript separate from governance audit; reader skips malformed JSON lines.

### Evidence e11

[quarantine/auto-harness/autoharness/context/autocompact.py:202–305](../../../quarantine/auto-harness/autoharness/context/autocompact.py#L202): Compactor caller summary/circuit breaker, returns summary+synthetic acknowledgment+last-user deep copy; no archive/digest/empty-summary guard in body.

### Evidence e12

[quarantine/auto-harness/autoharness/context/microcompact.py:54–111](../../../quarantine/auto-harness/autoharness/context/microcompact.py#L54): Old tool result view replaced by placeholder, copied input and token estimates; preservation keyed selected names.

### Evidence e13

[quarantine/auto-harness/autoharness/tools/orchestrator.py:31–108](../../../quarantine/auto-harness/autoharness/tools/orchestrator.py#L31): Supporting execute_batch partitions concurrency-safe then unsafe; safe group first even if submitted later; ordered return not original effect order.

### Evidence e14

[quarantine/auto-harness/autoharness/agents/background.py:29–106](../../../quarantine/auto-harness/autoharness/agents/background.py#L29): Background manager registers tasks/completes/fails and notifications/output file; no actual launch routine in inspected full class.

### Evidence e15

[quarantine/auto-harness/autoharness/agents/fork.py:35–97](../../../quarantine/auto-harness/autoharness/agents/fork.py#L35): build_forked_messages clones context and placeholder results/directive; supplied prompt constructor does not start child/provider requests.

### Evidence e16

[quarantine/auto-harness/autoharness/session/resume.py:9–67](../../../quarantine/auto-harness/autoharness/session/resume.py#L9): Resume generates structured briefing from saved task lists/status; not serialized provider/tool continuation.

### Evidence e17

[quarantine/auto-harness/autoharness/compiler/prompt.py:23–60](../../../quarantine/auto-harness/autoharness/compiler/prompt.py#L23): PromptCompiler.compile builds governance text, not binary compiler or harness generation/rebuild.

### Evidence e18

[quarantine/auto-harness/autoharness/core/pipeline.py:762–803](../../../quarantine/auto-harness/autoharness/core/pipeline.py#L762): verify_session reconstructs ToolCalls with empty input because hash only, ToolResults without raw output, from audit summary; limits evidence for rule verdict.

### Evidence e19

[quarantine/auto-harness/autoharness/tools/registry.py:16–80](../../../quarantine/auto-harness/autoharness/tools/registry.py#L16): Callback/schema/flags/aliases/deferred metadata registry supports register/unregister/lookup; core tools not automatically implemented by names.

### Evidence e20

[quarantine/auto-harness/tests/test_agent_loop.py:295–357](../../../quarantine/auto-harness/tests/test_agent_loop.py#L295): Loop integration oracle checks result routing; denial oracle OR isinstance(result,str) is trivially weak because run returns str and does not prove callback was blocked.

