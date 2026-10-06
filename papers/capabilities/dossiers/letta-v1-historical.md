# letta-v1-historical

Memory-editing model agent with searchable persisted messages, managed compaction, tool rules, approval handoff and multi-agent service primitives.

Role: retired stateful agent API server. Runtime: Python.

Pinned source: [https://github.com/letta-ai/letta](https://github.com/letta-ai/letta); revision/version `56ba9c25552605eec89de8ed3dc6394b625c1993`.

Async server persists agent/block/message/run/step state; V3 model adapter streams then dispatches typed server or client tools.

Historical server owns persistent agent memory/state and sandbox execution; consumes providers/MCP/E2B; current Letta implementation is a separate source tree.

Inspection: V3 request/dispatch/checkpoint/compaction, tool execution/sandbox, memory/search/messaging and historical direct test assertions

Limits of this study: Retired implementation only; no live APIs/tests. Older agent/group classes coexist and their mechanisms are not automatically the V3 entry path.

## Actions

### V3 step / stream

Surface: service API.

Input: agent/conversation, user/approval messages, max_steps and stream settings

Result: stream messages, run/step/tool IDs, stop reason

Lifecycle: provider then tools/checkpoint; pause on approval/client handoff

Authority: actor and tool rules

Evidence: [e2](#evidence-e2), [e3](#evidence-e3), [e7](#evidence-e7).

### ToolExecutionManager.execute_tool_async

Surface: model dispatch.

Input: function name/args, Tool type/config and step ID

Result: ToolExecutionResult/status/value/stdout/stderr, truncated return if oversized

Lifecycle: await typed executor; cancellation reports error without remote settlement

Authority: actor and exposed tool authority

Evidence: [e4](#evidence-e4), [e5](#evidence-e5), [e11](#evidence-e11), [e12](#evidence-e12).

### memory_replace (and sibling insert/rethink/patch)

Surface: model memory tool.

Input: block label, unique old string/new string

Result: updated model-visible memory or validation error

Lifecycle: next request uses block; separate service persistence

Authority: writable agent block authority

Evidence: [e15](#evidence-e15).

### conversation_search

Surface: model recall tool.

Input: query, roles, limit; signature also date args

Result: formatted matching message text/roles or no results

Lifecycle: service query projection, date args ignored by this native implementation

Authority: same agent actor; omitted record classes explicit

Evidence: [e13](#evidence-e13), [e14](#evidence-e14).

### compact / compact_messages

Surface: service policy/API.

Input: messages, per-call/agent compaction settings, model/tools and trigger/stats

Result: attributed summary and new system/summary/tail context

Lifecycle: new provider request; original objects kept apart from context IDs

Authority: configured summarizer/model policy

Evidence: [e8](#evidence-e8), [e9](#evidence-e9), [e10](#evidence-e10).

### send_message_to_agent_and_wait_for_reply

Surface: model peer tool.

Input: same-org target agent ID/message

Result: formatted target ID and assistant reply

Lifecycle: synchronous API chain; automatically attributed but input may be lowercase duplicated

Authority: sandbox client organization authority

Evidence: [e16](#evidence-e16).

### send_message_to_agent_async

Surface: model peer tool.

Input: same-org target ID/one-way notification

Result: confirmation after synchronous message create

Lifecycle: not a nonblocking pending delivery handle; rejects prod Cloud

Authority: local/self-host environment and actor organization

Evidence: [e17](#evidence-e17).

### _checkpoint_messages

Surface: server persistence primitive.

Input: completed new messages and current context IDs, run/step/conversation IDs

Result: persisted objects and selected context position

Lifecycle: only successful/safe-step checkpoint; split persistence calls after effects

Authority: server actor DB authority

Evidence: [e7](#evidence-e7).

## Capabilities

### filesystem

**I — Files** (source): Typed file/custom tool sandbox execution is wired; direct mode executes generated Python with fresh globals and process-global stdout/env mutation, E2B optional.

Evidence: [e4](#evidence-e4), [e11](#evidence-e11), [e12](#evidence-e12).

### processes

**I — OS programs** (source): Typed file/custom tool sandbox execution is wired; direct mode executes generated Python with fresh globals and process-global stdout/env mutation, E2B optional.

Evidence: [e4](#evidence-e4), [e11](#evidence-e11), [e12](#evidence-e12).

### code-actions

**I — Code actions** (source): Typed file/custom tool sandbox execution is wired; direct mode executes generated Python with fresh globals and process-global stdout/env mutation, E2B optional.

Evidence: [e4](#evidence-e4), [e11](#evidence-e11), [e12](#evidence-e12).

### persistent-kernel

**L — Kernel** (source): Local direct exec has fresh globals; environment/venv/E2B reuse is not a contracted shared standing interpreter heap.

Evidence: [e11](#evidence-e11), [e12](#evidence-e12).

### standing-database

**I — Standing DB** (source): Agent memory blocks, messages and in-context IDs persist through service managers; archival memory is another service primitive, not model-governed infrastructure.

Evidence: [e7](#evidence-e7), [e13](#evidence-e13), [e15](#evidence-e15).

### workflow-programming

**S — Workflows** (source): Tool rules/terminal/approval/client handoff and group orchestration compose server loops; no arbitrary model-authored orchestration program in traced V3 paths.

Evidence: [e3](#evidence-e3), [e4](#evidence-e4), [e6](#evidence-e6), [e18](#evidence-e18).

### multi-model

**I — Models** (source): Agent provider/config and configurable summarizer model, plus distinct participant agents; not solely model selection mistaken for collaboration.

Evidence: [e2](#evidence-e2), [e10](#evidence-e10), [e18](#evidence-e18).

### live-collaboration

**L — Peer chat** (source): Agent-to-agent service API and historical speaker runner exist; one-way async-named tool is sync create and forbidden in production Cloud, no room/event delivery contract.

Evidence: [e16](#evidence-e16), [e17](#evidence-e17), [e18](#evidence-e18).

### concurrent-work

**I — Concurrency** (source): Only tools marked parallel gathered; gathered group executes before serial group, result order restored by index.

Evidence: [e6](#evidence-e6).

### steering-interrupt

**L — Steer/interrupt** (source): Model invocation/client approval handoff and async cancellation exist; CancelledError becomes tool error but arbitrary local/E2B side effects may continue or be partial.

Evidence: [e3](#evidence-e3), [e5](#evidence-e5).

### turn-redefinition

**S — Turn program** (source): Memory/tool/config and compaction policy influence next step; actual model loop/scheduling remains server code, not general hot program turn.

Evidence: [e2](#evidence-e2), [e6](#evidence-e6), [e10](#evidence-e10), [e15](#evidence-e15).

### compaction

**I — Compaction** (source): Configurable summary/sliding/other compaction modes build new context, rebuild memory/system and persist identified summary before overflow retry.

Evidence: [e8](#evidence-e8), [e9](#evidence-e9), [e10](#evidence-e10).

### context-repair

**S — Repair** (source): Searchable original message objects separated from context IDs and memory edits provide repair inputs; search projection omits roles/tool records and date filters are not forwarded by native helper.

Evidence: [e7](#evidence-e7), [e13](#evidence-e13), [e14](#evidence-e14), [e15](#evidence-e15).

### original-audit

**L — Original audit** (source): Messages/run/step attribution retained, but action return trimmed before persistence and direct exec/global capture does not cover every original provider/OS byte.

Evidence: [e5](#evidence-e5), [e7](#evidence-e7), [e12](#evidence-e12).

### audit-query

**L — Audit query** (source): Message ID/query retrieval useful; native conversation_search forwards only query/roles/limit and searchable projection excludes specific tools/nonchat roles.

Evidence: [e13](#evidence-e13), [e14](#evidence-e14).

### hot-change

**S — Hot change** (source): Memory/block and configuration changes refresh next request, compaction repairs persisted system; no wired server executable hot reload traced.

Evidence: [e9](#evidence-e9), [e15](#evidence-e15).

### rebuild-continuity

**L — Rebuild continuity** (source): Persistent message/context/agent state loads future requests; completed-step checkpoint not an atomic external action journal and no active program/provider refit migration.

Evidence: [e5](#evidence-e5), [e7](#evidence-e7), [e9](#evidence-e9).

### remote-services

**I — Remote** (source): Provider model adapters, MCP executor/E2B and agent-message APIs; historical server governance separate from Arconaut consumer.

Evidence: [e3](#evidence-e3), [e4](#evidence-e4), [e11](#evidence-e11), [e16](#evidence-e16).

### self-improvement

**S — Self-improve** (source): Model edits memory/skills/config and separate sleeptime types exist; traced path does not establish experimental evaluated harness promotion.

Evidence: [e4](#evidence-e4), [e10](#evidence-e10), [e15](#evidence-e15).

### complaints

**S — Complaints** (source): Step/tool error/stdout/stderr and telemetry context useful diagnostics; full captured-state complaint tracking not established.

Evidence: [e5](#evidence-e5), [e7](#evidence-e7), [e20](#evidence-e20).

### authority

**I — Authority** (source): Actor organization and tool roster/types/approval rules; client-side actions return approval request boundary rather than executed by server.

Evidence: [e3](#evidence-e3), [e4](#evidence-e4), [e16](#evidence-e16), [e17](#evidence-e17).

### evaluation

**L — Evaluation** (source): Exact threshold test useful for policy calculations; inspected summary attribution test suppresses all exceptions and cannot certify output success/fidelity.

Evidence: [e19](#evidence-e19), [e20](#evidence-e20).

### time-order

**I — Time/order** (source): Context position/run/step IDs and indexed result order; parallel-before-serial scheduling differs from original call-list effect order.

Evidence: [e6](#evidence-e6), [e7](#evidence-e7).

## Inspected test oracles

- [quarantine/letta-v1-historical/tests/test_compaction_thresholds.py](../../../quarantine/letta-v1-historical/tests/test_compaction_thresholds.py): numeric compaction policy Oracle: Exact 90% GPT5/proactive and full nonGPT threshold; not summary correctness. Read, **not executed**.
- [quarantine/letta-v1-historical/tests/test_provider_trace_summarization.py](../../../quarantine/letta-v1-historical/tests/test_provider_trace_summarization.py): summary telemetry attribution Oracle: Mocks assert agent/tags/run/step/call_type, but blanket exception suppression permits failed summary; explicitly limited oracle. Read, **not executed**.

## Useful mechanisms

- Persisted message objects distinct from compacted context IDs.
- Model-native precise memory edits and conversation retrieval support continuity.
- Executor type and client approval boundaries explicit.

## Material limits

- Retired V1 cannot stand in for current Letta Code.
- Return truncation before persistence/search projection limits original audit.
- Direct local exec mutates global capture/env and does not give persistent kernel or reliable quiescence.

## Arconaut design questions

- How can retrieval see every original while serving bounded context?
- Do advertised search inputs actually reach query execution?
- Should parallel mutation ordering be explicit in model-facing program semantics?

## Evidence

### Evidence e1

[quarantine/letta-v1-historical/README.md:7–11](../../../quarantine/letta-v1-historical/README.md#L7): This source identifies itself as legacy V1 API; active development and self-hosting moved to letta-code App Server.

### Evidence e2

[quarantine/letta-v1-historical/letta/agents/letta_agent_v3.py:1090–1109](../../../quarantine/letta-v1-historical/letta/agents/letta_agent_v3.py#L1090): V3 builds provider request from messages/memory/system/config and allowed tools.

### Evidence e3

[quarantine/letta-v1-historical/letta/agents/letta_agent_v3.py:1150–1178](../../../quarantine/letta-v1-historical/letta/agents/letta_agent_v3.py#L1150): Actual model adapter streams invocation with requires-approval/client tool set and actor/step attribution.

### Evidence e4

[quarantine/letta-v1-historical/letta/services/tool_executor/tool_execution_manager.py:32–69](../../../quarantine/letta-v1-historical/letta/services/tool_executor/tool_execution_manager.py#L32): Tool type selects core/memory/files/builtin/MCP/sandbox executor; fallback custom tool executes in sandbox.

### Evidence e5

[quarantine/letta-v1-historical/letta/services/tool_executor/tool_execution_manager.py:95–154](../../../quarantine/letta-v1-historical/letta/services/tool_executor/tool_execution_manager.py#L95): Exec awaits typed executor then truncates return payload to tool char limit; CancelledError converted to error/stderr, not proof remote action stopped.

### Evidence e6

[quarantine/letta-v1-historical/letta/agents/letta_agent_v3.py:1822–1872](../../../quarantine/letta-v1-historical/letta/agents/letta_agent_v3.py#L1822): Eligible parallel tool batch gather runs before sequential tools, results put in original indices; not global call-list side-effect order.

### Evidence e7

[quarantine/letta-v1-historical/letta/agents/letta_agent_v3.py:758–816](../../../quarantine/letta-v1-historical/letta/agents/letta_agent_v3.py#L758): Safe completed-step checkpoint persists message objects separately from in-context message-ID/conversation ordering; external tool effect precedes this point.

### Evidence e8

[quarantine/letta-v1-historical/letta/services/summarizer/compact.py:426–472](../../../quarantine/letta-v1-historical/letta/services/summarizer/compact.py#L426): Compaction constructs attributed summary and final system/summary/tail list; original message DB is not deleted by this helper.

### Evidence e9

[quarantine/letta-v1-historical/letta/agents/letta_agent_v3.py:1239–1276](../../../quarantine/letta-v1-historical/letta/agents/letta_agent_v3.py#L1239): Overflow compaction rebuilds persisted system/memory, refreshes messages and checkpoints summary/context before retry provider.

### Evidence e10

[quarantine/letta-v1-historical/letta/agents/letta_agent_v3.py:2077–2134](../../../quarantine/letta-v1-historical/letta/agents/letta_agent_v3.py#L2077): Compaction configurable per call/agent defaults/model and statistics, passes available tools for self-summary/cache compatibility.

### Evidence e11

[quarantine/letta-v1-historical/letta/services/tool_executor/tool_execution_sandbox.py:94–109](../../../quarantine/letta-v1-historical/letta/services/tool_executor/tool_execution_sandbox.py#L94): Configured E2B vs local sandbox dispatch; environment/runtime branch explicit, not a permanent universal kernel.

### Evidence e12

[quarantine/letta-v1-historical/letta/services/tool_executor/tool_execution_sandbox.py:255–290](../../../quarantine/letta-v1-historical/letta/services/tool_executor/tool_execution_sandbox.py#L255): Direct Python execution creates fresh globals but temporarily redirects process-global stdout/stderr and environment; no retained heap guarantee.

### Evidence e13

[quarantine/letta-v1-historical/letta/functions/function_sets/base.py:137–163](../../../quarantine/letta-v1-historical/letta/functions/function_sets/base.py#L137): conversation_search implementation calls message-manager query_text/roles/limit; documented date args are not forwarded in this path.

### Evidence e14

[quarantine/letta-v1-historical/letta/services/message_manager.py:129–153](../../../quarantine/letta-v1-historical/letta/services/message_manager.py#L129): Search projection only assistant/user/tool roles and omits send_message/conversation_search tool records; not complete audit query coverage.

### Evidence e15

[quarantine/letta-v1-historical/letta/functions/function_sets/base.py:350–390](../../../quarantine/letta-v1-historical/letta/functions/function_sets/base.py#L350): memory_replace requires unique exact text, updates model-visible block and returns value; ambiguity fails.

### Evidence e16

[quarantine/letta-v1-historical/letta/functions/function_sets/multi_agent.py:60–99](../../../quarantine/letta-v1-historical/letta/functions/function_sets/multi_agent.py#L60): Synchronous agent messaging uses service API, automatically adds sender identity but duplicates mixed-case text lowercase before dispatch.

### Evidence e17

[quarantine/letta-v1-historical/letta/functions/function_sets/multi_agent.py:163–191](../../../quarantine/letta-v1-historical/letta/functions/function_sets/multi_agent.py#L163): One-way messaging explicitly disallowed prod Cloud; implementation calls non-stream create synchronously, return confirmation not live-peer delivery handle.

### Evidence e18

[quarantine/letta-v1-historical/letta/groups/round_robin_multi_agent.py:34–66](../../../quarantine/letta-v1-historical/letta/groups/round_robin_multi_agent.py#L34): Separate older group runner loads agents and iterates speakers; not evidence V3 arbitrary live concurrent room.

### Evidence e19

[quarantine/letta-v1-historical/tests/test_compaction_thresholds.py:23–53](../../../quarantine/letta-v1-historical/tests/test_compaction_thresholds.py#L23): Exact numeric family thresholds/force-proactive behavior tested; not semantic compaction correctness.

### Evidence e20

[quarantine/letta-v1-historical/tests/test_provider_trace_summarization.py:99–117](../../../quarantine/letta-v1-historical/tests/test_provider_trace_summarization.py#L99): Summary test catches all exceptions then asserts telemetry context attribution only; cannot prove summarization succeeds.

