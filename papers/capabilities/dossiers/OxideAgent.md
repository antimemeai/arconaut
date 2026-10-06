# OxideAgent

Early Rust multi-agent manager with three native tools, shared operator focus and persisted chat; several claimed control paths are incomplete.

Role: coding agent. Runtime: Rust.

Pinned source: [https://github.com/Juan-LukeKlopper/OxideAgent](https://github.com/Juan-LukeKlopper/OxideAgent); revision/version `4ad71bdb1d8c90934b54df760b0c45834a9b87f9`.

Tokio TUI/orchestrator plus per-agent bounded mpsc task and broadcast output; native file/shell calls use blocking std APIs inside async methods.

Owns local sessions/tool permissions and child MCP processes; consumes Ollama and MCP endpoints.

Inspection: Entry→per-agent mailbox→Ollama streaming→approval/tool dispatch→message state and continuation; session/model switch, event routing, MCP connection registry and tests.

Limits of this study: Source read only, reference not executed. No runtime execution. Model-switch announcement does not update captured agent_model; automatically preapproved tool execution does not schedule its own continuation. Model-to-model messaging/cancellation/refit not established in exposed registry.

## Actions

### write_file

Surface: model tool.

Input: path, content

Result: Success text/errors

Lifecycle: Blocking fs::write, awaited registry dispatch

Authority: Named permission; host filesystem access

Evidence: [e2](#evidence-e2), [e5](#evidence-e5), [e6](#evidence-e6).

### read_file

Surface: model tool.

Input: path

Result: Entire UTF-8 text/errors

Lifecycle: Blocking fs::read_to_string

Authority: Named permission; host filesystem

Evidence: [e2](#evidence-e2), [e5](#evidence-e5), [e6](#evidence-e6).

### run_shell_command

Surface: model tool.

Input: command

Result: Success stdout; failure stdout/stderr message

Lifecycle: Blocking sh -c output; no timeout/PTY/process handle

Authority: Named approval; host process identity

Evidence: [e2](#evidence-e2), [e5](#evidence-e5), [e6](#evidence-e6).

### McpToolAdapter::<discovered name>

Surface: model tool.

Input: Discovered input_schema JSON

Result: Remote/stdio result/error text

Lifecycle: Awaited registry connection call; shared connection ID

Authority: Same named-tool permission; operator configures startup endpoint

Evidence: [e10](#evidence-e10), [e11](#evidence-e11).

### SwitchAgent; SwitchSession; SwitchModel; ListSessions

Surface: operator command.

Input: AppEvent agent/session/model names

Result: UI status/history/models/session listing

Lifecycle: Queued per-agent session operations; model switch announcement incomplete

Authority: Operator TUI event channel

Evidence: [e7](#evidence-e7), [e14](#evidence-e14).

### MultiAgentManager::create_agent; send_event_to_agent

Surface: programmable API.

Input: Name/model/session; AgentId + AppEvent

Result: AgentId/queued send error

Lifecycle: Own task + bounded100 mailbox; busy target processes message after awaited chat/tool

Authority: Harness code API; not exposed to model

Evidence: [e3](#evidence-e3), [e4](#evidence-e4), [e15](#evidence-e15).

## Capabilities

### filesystem

**I — Files** (source): Whole UTF-8 reads and overwrite writes on host; no intrinsic search/patch tool.

Evidence: [e2](#evidence-e2).

### processes

**L — OS programs** (source): sh -c capture waits synchronously; no PTY, background handle or cancellation; success stderr lost.

Evidence: [e2](#evidence-e2).

### code-actions

**? — Code actions** (source-inspection): Not established in the inspected entry, model loop, action dispatch and state paths; no universal absence claim.

### persistent-kernel

**? — Kernel** (source-inspection): Not established in the inspected entry, model loop, action dispatch and state paths; no universal absence claim.

### standing-database

**? — Standing DB** (source-inspection): Not established in the inspected entry, model loop, action dispatch and state paths; no universal absence claim.

### workflow-programming

**? — Workflows** (source-inspection): Not established in the inspected entry, model loop, action dispatch and state paths; no universal absence claim.

### multi-model

**L — Models** (source): Multiple Ollama-model worker tasks; only Ollama provider. SwitchModel path does not change worker captured model.

Evidence: [e3](#evidence-e3), [e7](#evidence-e7), [e12](#evidence-e12).

### live-collaboration

**S — Peer chat** (source): Addressed worker mailbox exists as program API; no exposed model peer-message tool; busy worker consumes after chat completes.

Evidence: [e3](#evidence-e3), [e4](#evidence-e4), [e15](#evidence-e15).

### concurrent-work

**I — Concurrency** (source): Independent per-agent Tokio tasks can be active; each worker serially awaits its chat and tool batch; std shell may block runtime threads.

Evidence: [e2](#evidence-e2), [e3](#evidence-e3), [e4](#evidence-e4).

### steering-interrupt

**L — Steer/interrupt** (source): Operator can queue input/select another agent; active worker serially awaits operations without cancellation branch in inspected mailbox.

Evidence: [e4](#evidence-e4), [e14](#evidence-e14).

### turn-redefinition

**? — Turn program** (source-inspection): Not established in the inspected entry, model loop, action dispatch and state paths; no universal absence claim.

### compaction

**? — Compaction** (source-inspection): Not established in the inspected entry, model loop, action dispatch and state paths; no universal absence claim.

### context-repair

**? — Repair** (source-inspection): Not established in the inspected entry, model loop, action dispatch and state paths; no universal absence claim.

### original-audit

**L — Original audit** (source): Persisted chat/settings and diagnostic log overwritten at startup; tool result rephrased into user message, not original complete IO.

Evidence: [e1](#evidence-e1), [e5](#evidence-e5), [e8](#evidence-e8).

### audit-query

**? — Audit query** (source-inspection): Not established in the inspected entry, model loop, action dispatch and state paths; no universal absence claim.

### hot-change

**L — Hot change** (source): Session and worker selection are live events; model-change UI notification is not active request configuration replacement.

Evidence: [e7](#evidence-e7), [e14](#evidence-e14).

### rebuild-continuity

**? — Rebuild continuity** (source-inspection): Not established in the inspected entry, model loop, action dispatch and state paths; no universal absence claim.

### remote-services

**I — Remote** (source): Ollama HTTP/model discovery and discovered MCP adapters consume services; startup manager launches owned MCP children.

Evidence: [e1](#evidence-e1), [e10](#evidence-e10), [e11](#evidence-e11), [e12](#evidence-e12).

### self-improvement

**? — Self-improve** (source-inspection): Not established in the inspected entry, model loop, action dispatch and state paths; no universal absence claim.

### complaints

**? — Complaints** (source-inspection): Not established in the inspected entry, model loop, action dispatch and state paths; no universal absence claim.

### authority

**I — Authority** (source): Native and discovered tools require name approval unless session/global allowlisted; global allow decisions saved.

Evidence: [e5](#evidence-e5), [e6](#evidence-e6).

### evaluation

**L — Evaluation** (source): Registry/tools mostly mocks; separate-directory permission roundtrip oracle does not prove contested shared writes.

Evidence: [e13](#evidence-e13).

### time-order

**S — Time/order** (source): Event carries SystemTime/source/destination; no durable causal clock or pending-request cancellation deadline.

Evidence: [e9](#evidence-e9).

## Inspected test oracles

- [quarantine/OxideAgent/tests/unit/core/test_tools.rs](../../../quarantine/OxideAgent/tests/unit/core/test_tools.rs): Registry schema/profile and mocked file/shell calls Oracle: Tool names/profiles/results compared to mocks; tests do not establish real process cancellation or filesystem boundaries. Read, **not executed**.
- [quarantine/OxideAgent/tests/test_concurrent_directory_changes.rs](../../../quarantine/OxideAgent/tests/test_concurrent_directory_changes.rs): Permission persistence test isolation Oracle: Save/reload exact allowset under independent TempDirs, not shared concurrent writers. Read, **not executed**.
- [quarantine/OxideAgent/tests/direct_tool_test.rs](../../../quarantine/OxideAgent/tests/direct_tool_test.rs): Ollama tool payload exploratory probe Oracle: Program prints live provider response; no assertion of agent loop correctness and not executed here. Read, **not executed**.

## Useful mechanisms

- Per-worker mailbox and output forwarding make lifecycle ownership visible.
- Session/global standing named authority can eliminate repeated prompts.

## Material limits

- No runtime execution. Model-switch announcement does not update captured agent_model; automatically preapproved tool execution does not schedule its own continuation. Model-to-model messaging/cancellation/refit not established in exposed registry.

## Arconaut design questions

- What oracle catches the preapproved-tool continuation branch diverging from approved-once execution?
- How should addressed model messages and cancellation bypass a worker awaiting its provider request?

## Evidence

### Evidence e1

[quarantine/OxideAgent/src/main.rs:18–52](../../../quarantine/OxideAgent/src/main.rs#L18): Tokio entry uses Ollama and truncates diagnostic log at startup.

### Evidence e2

[quarantine/OxideAgent/src/core/tools.rs:89–228](../../../quarantine/OxideAgent/src/core/tools.rs#L89): write_file/read_file/run_shell_command; shell waits for output without PTY or handle, successful stderr omitted.

### Evidence e3

[quarantine/OxideAgent/src/core/multi_agent_manager.rs:89–173](../../../quarantine/OxideAgent/src/core/multi_agent_manager.rs#L89): Creates per-agent task/mailbox, restores session/model, forwards output separately.

### Evidence e4

[quarantine/OxideAgent/src/core/multi_agent_manager.rs:250–342](../../../quarantine/OxideAgent/src/core/multi_agent_manager.rs#L250): Mailbox serially awaits model chat/approval; continue event invokes another chat.

### Evidence e5

[quarantine/OxideAgent/src/core/multi_agent_manager.rs:404–554](../../../quarantine/OxideAgent/src/core/multi_agent_manager.rs#L404): Preapproved tools execute serially; outputs added as user text; no automatic continuation event on this branch.

### Evidence e6

[quarantine/OxideAgent/src/core/multi_agent_manager.rs:556–695](../../../quarantine/OxideAgent/src/core/multi_agent_manager.rs#L556): Approval can be once/session/global; allowed branch enqueues ContinueConversation; denial writes user text.

### Evidence e7

[quarantine/OxideAgent/src/core/orchestrator.rs:239–250](../../../quarantine/OxideAgent/src/core/orchestrator.rs#L239): SwitchModel changes orchestrator field and announces success but not captured worker agent_model.

### Evidence e8

[quarantine/OxideAgent/src/core/session.rs:145–202](../../../quarantine/OxideAgent/src/core/session.rs#L145): Serializes session state via pid temporary file then rename; this is chat/settings persistence.

### Evidence e9

[quarantine/OxideAgent/src/core/events.rs:53–121](../../../quarantine/OxideAgent/src/core/events.rs#L53): Events carry source/destination/SystemTime and ephemeral bounded broadcast bus.

### Evidence e10

[quarantine/OxideAgent/src/core/mcp_manager.rs:34–121](../../../quarantine/OxideAgent/src/core/mcp_manager.rs#L34): MCP command discovery registers tool adapters using named connection IDs.

### Evidence e11

[quarantine/OxideAgent/src/core/mcp/manager.rs:145–196](../../../quarantine/OxideAgent/src/core/mcp/manager.rs#L145): MCP tool adapter forwards registered names/args to shared connection registry.

### Evidence e12

[quarantine/OxideAgent/src/core/llm/mod.rs:1–15](../../../quarantine/OxideAgent/src/core/llm/mod.rs#L1): Only Ollama factory implemented; future provider comment is not provider support.

### Evidence e13

[quarantine/OxideAgent/tests/test_concurrent_directory_changes.rs:8–60](../../../quarantine/OxideAgent/tests/test_concurrent_directory_changes.rs#L8): Permission save/reload in two separate temporary directories; does not attack concurrent shared-file writes.

### Evidence e14

[quarantine/OxideAgent/src/core/orchestrator.rs:105–193](../../../quarantine/OxideAgent/src/core/orchestrator.rs#L105): Operator input/approval routes to active agent and agent switch selects/creates tasks.

### Evidence e15

[quarantine/OxideAgent/src/core/multi_agent_manager.rs:698–722](../../../quarantine/OxideAgent/src/core/multi_agent_manager.rs#L698): Program API can address a worker ID via send_event_to_agent; not itself a model tool.

