# forgecode

Tailcall ForgeCode is a native Rust agent with configurable personas, child task delegation, deterministic context compression, filesystem/shell/MCP operations and compiled lifecycle hooks.

Role: coding agent. Runtime: Rust.

Pinned source: [https://github.com/tailcallhq/forgecode](https://github.com/tailcallhq/forgecode); revision/version `571a28902b9c562594c02fd089fc58bf8595108f`.

Tokio application tasks; streamed provider HTTP; sequential native actions and parallel child tasks; one shared async shell mutex.

Owns conversations/config/tool execution and child chats; consumes providers, MCP and workspace search services.

Inspection: Traced native main -> ForgeApp -> Orchestrator -> transformed streamed provider -> action registry -> results/context/save/continuation; inspected child sessions, abort/drop, shell ownership, compactor, SQLite, config cache, hooks and substantive tests.

Limits of this study: Source read only, reference not executed. No reference execution. Other provider formats inspected at router boundary only; no claim of complete external MCP/search implementations. Quiescence/rebuild not established. Abrupt stream abort bypasses final post-run save; latest completed-step saves remain.

## Actions

### read, write, fs_search, remove, patch, multi_patch, undo

Surface: model tools.

Input: file_path/path, line range/pattern, contents or edits; write.overwrite

Result: file/image values, matching lines, mutation result/diff or error; undo latest tracked change

Lifecycle: awaited under native timeout; read-before-edit metrics for edit/overwrite

Authority: agent tool allowlist; optional restricted policy; ordinary host filesystem

Evidence: [e6](#evidence-e6), [e7](#evidence-e7), [e8](#evidence-e8), [e9](#evidence-e9), [e10](#evidence-e10).

### sem_search

Surface: model tool.

Input: queries with query/use_case

Result: deduplicated codebase query result groups

Lifecycle: query work parallel within one awaited tool; indexed-workspace requirement

Authority: agent allowlist and workspace search service

Evidence: [e7](#evidence-e7).

### shell

Surface: model tool.

Input: command,cwd,keep_ansi,env variable names,description

Result: stdout/stderr/exit_code; oversized output prefix/suffix plus retained file path

Lifecycle: one shared executor serializes; streams terminal output; tool timeout/drop kills direct child; no returned ongoing-job handle

Authority: optional restricted native policy, otherwise operator OS authority

Evidence: [e7](#evidence-e7), [e10](#evidence-e10), [e13](#evidence-e13), [e35](#evidence-e35).

### fetch

Surface: model tool.

Input: url,raw

Result: fetched content or error; large result archive path

Lifecycle: awaited native timeout

Authority: allowlist and optional native policy

Evidence: [e7](#evidence-e7), [e9](#evidence-e9), [e10](#evidence-e10), [e35](#evidence-e35).

### followup

Surface: model tool.

Input: question,multiple,option1 through option5

Result: operator answer or absence

Lifecycle: interactive awaited tool; orchestrator yields so next user chat continues

Authority: model invokes; operator selects

Evidence: [e7](#evidence-e7), [e35](#evidence-e35), [e36](#evidence-e36).

### plan, skill, todo_write, todo_read

Surface: model tools.

Input: plan_name/version/content; skill.name; todo items content/status

Result: plan artifact; skill text; updated/current todo state

Lifecycle: awaited; todos owned by conversation metrics and content keys

Authority: agent tools; no governed executable self-change

Evidence: [e7](#evidence-e7), [e27](#evidence-e27), [e35](#evidence-e35).

### task; dynamically named agent tools

Surface: model tools.

Input: agent_id,tasks,optional session_id; custom agent tasks

Result: each child final text with conversation ID or error, some child stream events forwarded

Lifecycle: parallel awaited independent/resumed child chats; no timeout; parent cannot separately address a live child in this interface

Authority: per-agent definitions/model/provider/tools; shared host services

Evidence: [e6](#evidence-e6), [e10](#evidence-e10), [e11](#evidence-e11), [e12](#evidence-e12), [e25](#evidence-e25).

### MCP tools / mcp reload

Surface: model tools / operator command.

Input: server-discovered tool arguments; reload configured servers

Result: server-specific values or error; refreshed catalog

Lifecycle: MCP calls timed; service owns connection lifecycle

Authority: agent tool validation; native restricted policy branch does not wrap MCP dispatch

Evidence: [e10](#evidence-e10), [e33](#evidence-e33).

### ForgeApp.chat; EventHandle / Hook

Surface: programmable API / supplied primitive.

Input: agent ID plus ChatRequest conversation/event; compiled event handlers mutate Conversation

Result: bounded response stream; completion/interrupt; handler-injected context

Lifecycle: owned task aborted on stream drop; normal terminal save; End additions can resume model loop

Authority: host program; compiled hooks selected by app, not model-loaded turn code

Evidence: [e2](#evidence-e2), [e3](#evidence-e3), [e4](#evidence-e4), [e14](#evidence-e14), [e26](#evidence-e26).

### /compact; compact_conversation

Surface: operator command / programmable API.

Input: current conversation and active agent compact policy

Result: before/after messages/tokens plus persisted structured summary

Lifecycle: in-place lossy context replacement, automatic response-hook path too

Authority: operator/API; no traced model restoration operation

Evidence: [e16](#evidence-e16), [e17](#evidence-e17), [e18](#evidence-e18).

### /dump; conversation JSON import

Surface: operator commands.

Input: HTML/JSON dump option; CLI conversation file

Result: current parent/related child records; editable imported conversation

Lifecycle: snapshot export/import across launches

Authority: operator; requires previously retained snapshot for repair

Evidence: [e21](#evidence-e21), [e22](#evidence-e22).

### update_config / reload_agents

Surface: operator / programmable API.

Input: typed ConfigOperation session model/provider or other config

Result: written config; invalidated/reloaded agent cache

Lifecycle: future chats capture new agent; no staged executable refit or continuity transaction

Authority: operator/API config writes; cache reload error ignored

Evidence: [e2](#evidence-e2), [e23](#evidence-e23), [e24](#evidence-e24).

### Ctrl-C

Surface: operator action.

Input: interrupt signal

Result: prompt returns or headless exit

Lifecycle: drops chat consumer; abort task; last completed persisted step remains; final-save block may never run

Authority: operator

Evidence: [e2](#evidence-e2), [e14](#evidence-e14), [e15](#evidence-e15), [e29](#evidence-e29).

## Capabilities

### filesystem

**I — Files** (source): Concrete native read/search/edit/undo with per-agent tools, optional restricted policies and prior-read path gate; freshness is not checked.

Evidence: [e6](#evidence-e6), [e7](#evidence-e7), [e8](#evidence-e8), [e9](#evidence-e9), [e10](#evidence-e10).

### processes

**L — OS programs** (source): Real shell execution and streaming, but global executor mutex serializes commands and no model-addressable ongoing handle; cancellation kills direct child only by drop.

Evidence: [e7](#evidence-e7), [e10](#evidence-e10), [e13](#evidence-e13), [e14](#evidence-e14).

### code-actions

**S — Code actions** (source): Arbitrary ordinary shell programs can compute; native action schema has no persistent general-purpose program kernel or code-action orchestration language.

Evidence: [e6](#evidence-e6), [e7](#evidence-e7), [e13](#evidence-e13).

### persistent-kernel

**? — Kernel** (source): Not established in catalog, orchestration or shell execution; no kernel session traced.

### standing-database

**S — Standing DB** (source): Internal SQLite conversation/todo persistence is host-owned; shell/MCP can compose external database access but no standing DB model contract traced.

Evidence: [e7](#evidence-e7), [e19](#evidence-e19), [e33](#evidence-e33).

### workflow-programming

**L — Workflows** (source): Declarative agent/tool/prompt policies, parallel delegation and compiled mutable lifecycle hooks; no exposed programmable workflow interpreter. CI workflows are not agent workflows.

Evidence: [e10](#evidence-e10), [e11](#evidence-e11), [e25](#evidence-e25), [e26](#evidence-e26).

### multi-model

**I — Models** (source): Per-agent model/provider and concurrent child conversations; adapter normalizes model-specific reasoning; no common live multi-model room.

Evidence: [e3](#evidence-e3), [e10](#evidence-e10), [e12](#evidence-e12), [e25](#evidence-e25), [e34](#evidence-e34).

### live-collaboration

**L — Peer chat** (source): Parent-child task delegation supports continued conversation ID and returned text; no live peer send/receive/observe/identity channel traced.

Evidence: [e6](#evidence-e6), [e10](#evidence-e10), [e12](#evidence-e12).

### concurrent-work

**L — Concurrency** (source): Task calls and semantic queries run concurrently, then pair results in original order; ordinary tools sequential, shared shell serial, no child handle for detached work.

Evidence: [e7](#evidence-e7), [e10](#evidence-e10), [e11](#evidence-e11), [e13](#evidence-e13).

### steering-interrupt

**L — Steer/interrupt** (source): Ctrl-C drop aborts owned chat task; tools expire by timeout. No queued in-flight prompt steering traced; abrupt abort can bypass final save.

Evidence: [e2](#evidence-e2), [e10](#evidence-e10), [e14](#evidence-e14), [e15](#evidence-e15).

### turn-redefinition

**S — Turn program** (source): Compiled lifecycle hooks mutate context; End additions resume loop. Model request/finish/tool phases remain compiled, not hot programmable model/operator turn replacement.

Evidence: [e2](#evidence-e2), [e4](#evidence-e4), [e26](#evidence-e26), [e27](#evidence-e27).

### compaction

**I — Compaction** (source): Deterministic structured transformer compaction, threshold response hook and manual compact; retention/eviction ranges and preserved usage/last reasoning. Originals spliced away.

Evidence: [e16](#evidence-e16), [e17](#evidence-e17), [e18](#evidence-e18), [e30](#evidence-e30).

### context-repair

**S — Repair** (source): Editable conversation JSON dump/import lets operator restore supplied snapshot; no automatic repair of lost originals nor model context retrieval/repair action traced.

Evidence: [e16](#evidence-e16), [e19](#evidence-e19), [e21](#evidence-e21), [e22](#evidence-e22).

### original-audit

**L — Original audit** (source): Stored context overwrites same DB row and compaction destroys originals. Tracing skips request payload and does not capture all success/delta events.

Evidence: [e16](#evidence-e16), [e19](#evidence-e19), [e20](#evidence-e20), [e22](#evidence-e22).

### audit-query

**L — Audit query** (source): Conversation SQLite lookup/list and related-conversation dump support history inspection, not a queryable complete original audit.

Evidence: [e19](#evidence-e19), [e21](#evidence-e21), [e22](#evidence-e22).

### hot-change

**L — Hot change** (source): Typed configuration writes invalidate agent cache; new chat captures agent/settings. MCP reload exists; no settled-turn pending change transaction traced; reload error ignored.

Evidence: [e2](#evidence-e2), [e23](#evidence-e23), [e24](#evidence-e24), [e33](#evidence-e33).

### rebuild-continuity

**? — Rebuild continuity** (source): Conversation import/resume is available, but quiescence/outpost handoff and unresolved-work continuity through executable replacement not established in inspected paths.

### remote-services

**I — Remote** (source): Provider HTTP streams, MCP actions, fetch and workspace query services consumed; host owns its conversation database and shell execution.

Evidence: [e5](#evidence-e5), [e7](#evidence-e7), [e33](#evidence-e33), [e34](#evidence-e34).

### self-improvement

**? — Self-improve** (source): File edits and prompt/agent files enable ordinary development; no governed measured harness-improvement loop traced.

### complaints

**? — Complaints** (source): Tracing/retry/error responses exist; model-authored grievance with state snapshot and external tracking not established.

### authority

**I — Authority** (source): Agent tool validation plus optional restricted-mode native operation policies and read-before-edit; MCP/agent dispatch has distinct authority branch.

Evidence: [e8](#evidence-e8), [e9](#evidence-e9), [e10](#evidence-e10).

### evaluation

**I — Evaluation** (source): Read source tests assert drop cancellation, compaction accounting, config merge, history/reminder ordering; no reference tests executed.

Evidence: [e28](#evidence-e28), [e29](#evidence-e29), [e30](#evidence-e30), [e31](#evidence-e31), [e32](#evidence-e32).

### time-order

**S — Time/order** (source): Request count, ordered pairing, Tokio timeout and wall-clock conversation timestamps; no causal audit sequence or replay clock established.

Evidence: [e4](#evidence-e4), [e10](#evidence-e10), [e11](#evidence-e11), [e19](#evidence-e19).

## Inspected test oracles

- [quarantine/forgecode/crates/forge_stream/src/mpsc_stream.rs](../../../quarantine/forgecode/crates/forge_stream/src/mpsc_stream.rs): Drop cancellation Oracle: Virtual-time completion flag stays false after stream drop; does not assert process descendants or final context durability. Read, **not executed**.
- [quarantine/forgecode/crates/forge_app/src/compact.rs](../../../quarantine/forgecode/crates/forge_app/src/compact.rs): Accounting through lossy compaction Oracle: Explicit summed usage and surviving message equality; safe threshold simulation tests policy estimates, not a live provider window. Read, **not executed**.
- [quarantine/forgecode/crates/forge_app/src/orch_spec/orch_spec.rs](../../../quarantine/forgecode/crates/forge_app/src/orch_spec/orch_spec.rs): Tool-loop reminder ordering / completion Oracle: Mock replies assert reminder after repeated successful tool history; pending-todo test confirms two Stop responses complete with unchanged pending items. Read, **not executed**.
- [quarantine/forgecode/crates/forge_app/src/agent.rs](../../../quarantine/forgecode/crates/forge_app/src/agent.rs): Agent/workflow defaults Oracle: Regression expects default retention_window=0 to win over workflow ten; records current behavior, not independent desired requirement. Read, **not executed**.

## Useful mechanisms

- Native scheduler and explicit stream ownership make cancellation path inspectable.
- Tool-call IDs and original order retained across concurrent child task batch.
- Pure structured compaction avoids extra provider request and transfers accounting.
- End-hook continuation is a concrete host program composition point.

## Material limits

- No reference execution. Other provider formats inspected at router boundary only; no claim of complete external MCP/search implementations. Quiescence/rebuild not established. Abrupt stream abort bypasses final post-run save; latest completed-step saves remain.

## Arconaut design questions

- Should dropping a UI observer cancel work, or detach only its view? How does persistence survive interrupted phases?
- How should per-agent child workflows consume shared process/database services without secretly serializing independent computations?
- Can deterministic compression preserve references to originals and give the model selective repair operations?
- Which configuration defaults mean unset versus deliberate zero, and which activation transaction enforces current-turn/workflow boundaries?

## Evidence

### Evidence e1

[quarantine/forgecode/crates/forge_main/src/main.rs:47–69](../../../quarantine/forgecode/crates/forge_main/src/main.rs#L47): Native Rust Tokio entry into UI/application services.

### Evidence e2

[quarantine/forgecode/crates/forge_app/src/app.rs:60–197](../../../quarantine/forgecode/crates/forge_app/src/app.rs#L60): Per-chat agent/model/tool configuration is resolved; compiled hooks build orchestrator; task saves after normal dispatch.

### Evidence e3

[quarantine/forgecode/crates/forge_app/src/orch.rs:201–244](../../../quarantine/forgecode/crates/forge_app/src/orch.rs#L201): Request context transformers normalize tool calls/images/reasoning and stream model completion through AgentService.

### Evidence e4

[quarantine/forgecode/crates/forge_app/src/orch.rs:246–467](../../../quarantine/forgecode/crates/forge_app/src/orch.rs#L246): Turn loops request, response hooks, paired actions, stored context; yields on followup/stop/error/request limits; End hooks adding messages resume.

### Evidence e5

[quarantine/forgecode/crates/forge_repo/src/provider/openai.rs:214–257](../../../quarantine/forgecode/crates/forge_repo/src/provider/openai.rs#L214): OpenAI-compatible adapter serializes transformed request and consumes HTTP eventsource.

### Evidence e6

[quarantine/forgecode/crates/forge_domain/src/tools/catalog.rs:40–100](../../../quarantine/forgecode/crates/forge_domain/src/tools/catalog.rs#L40): Concrete native catalog and task agent_id/tasks/session_id schema.

### Evidence e7

[quarantine/forgecode/crates/forge_app/src/tool_executor.rs:155–339](../../../quarantine/forgecode/crates/forge_app/src/tool_executor.rs#L155): Native read/write/search/semantic queries/remove/patch/undo/shell/fetch/followup/plan/skill/todo dispatch.

### Evidence e8

[quarantine/forgecode/crates/forge_app/src/tool_executor.rs:347–388](../../../quarantine/forgecode/crates/forge_app/src/tool_executor.rs#L347): Patch/multipatch and overwrite require prior read; operation emits UI output, dumps large data, produces paired result.

### Evidence e9

[quarantine/forgecode/crates/forge_app/src/tool_executor.rs:33–111](../../../quarantine/forgecode/crates/forge_app/src/tool_executor.rs#L33): Prior-read checks accessed-path metrics, not current bytes; full large shell/fetch outputs saved to non-cleaning temporary file.

### Evidence e10

[quarantine/forgecode/crates/forge_app/src/tool_registry.rs:48–219](../../../quarantine/forgecode/crates/forge_app/src/tool_registry.rs#L48): Allowed-tool validation; restricted-only native policy checks; timed native/MCP actions; Task/custom agents parallel untimed child chats.

### Evidence e11

[quarantine/forgecode/crates/forge_app/src/orch.rs:59–166](../../../quarantine/forgecode/crates/forge_app/src/orch.rs#L59): Task tool calls run concurrently, other tools sequentially with UI acknowledgement and hooks; final pairing reconstructs original order.

### Evidence e12

[quarantine/forgecode/crates/forge_app/src/agent_executor.rs:50–144](../../../quarantine/forgecode/crates/forge_app/src/agent_executor.rs#L50): Child new/resumed conversation via ForgeApp; stream-forward selected events and return final task text plus conversation identity.

### Evidence e13

[quarantine/forgecode/crates/forge_infra/src/executor.rs:26–150](../../../quarantine/forgecode/crates/forge_infra/src/executor.rs#L26): Shared async mutex serializes shell commands; inherited stdin, captured concurrent stdout/stderr, kill_on_drop child, awaited exit.

### Evidence e14

[quarantine/forgecode/crates/forge_stream/src/mpsc_stream.rs:8–40](../../../quarantine/forgecode/crates/forge_stream/src/mpsc_stream.rs#L8): Chat stream owns spawned task; Drop closes receiver and aborts JoinHandle.

### Evidence e15

[quarantine/forgecode/crates/forge_main/src/ui.rs:379–425](../../../quarantine/forgecode/crates/forge_main/src/ui.rs#L379): Ctrl-C select drops active UI future and returns to prompt/finishes headless run.

### Evidence e16

[quarantine/forgecode/crates/forge_app/src/compact.rs:39–169](../../../quarantine/forgecode/crates/forge_app/src/compact.rs#L39): Pure structured compaction selects range, summarizes transformations, splices away originals, transfers usage and last reasoning.

### Evidence e17

[quarantine/forgecode/crates/forge_app/src/hooks/compaction.rs:33–51](../../../quarantine/forgecode/crates/forge_app/src/hooks/compaction.rs#L33): Response hook checks thresholds and replaces conversation context in place.

### Evidence e18

[quarantine/forgecode/crates/forge_app/src/app.rs:220–275](../../../quarantine/forgecode/crates/forge_app/src/app.rs#L220): Explicit compact API computes before/after messages/tokens and persists compacted conversation.

### Evidence e19

[quarantine/forgecode/crates/forge_repo/src/conversation/conversation_repo.rs:46–76](../../../quarantine/forgecode/crates/forge_repo/src/conversation/conversation_repo.rs#L46): SQLite upsert replaces same conversation context/metrics/title rather than immutable event journal.

### Evidence e20

[quarantine/forgecode/crates/forge_app/src/hooks/tracing.rs:45–129](../../../quarantine/forgecode/crates/forge_app/src/hooks/tracing.rs#L45): Request hook logs no payload; response logs usage/count; tool failures have output logging. Not complete request/delta/success event audit.

### Evidence e21

[quarantine/forgecode/crates/forge_main/src/ui.rs:3841–3874](../../../quarantine/forgecode/crates/forge_main/src/ui.rs#L3841): Operator conversation-file import accepts editable full ConversationDump or Conversation JSON and upserts context.

### Evidence e22

[quarantine/forgecode/crates/forge_main/src/ui.rs:4104–4127](../../../quarantine/forgecode/crates/forge_main/src/ui.rs#L4104): JSON dump exports current conversation and related child conversations, not original event history.

### Evidence e23

[quarantine/forgecode/crates/forge_api/src/forge_api.rs:243–255](../../../quarantine/forgecode/crates/forge_api/src/forge_api.rs#L243): Session config update invalidates agent cache after write; reload error intentionally ignored.

### Evidence e24

[quarantine/forgecode/crates/forge_services/src/agent_registry.rs:98–120](../../../quarantine/forgecode/crates/forge_services/src/agent_registry.rs#L98): Agents cached in memory; reload clears cache then reloads; no executable replacement.

### Evidence e25

[quarantine/forgecode/crates/forge_repo/src/agent_definition.rs:16–68](../../../quarantine/forgecode/crates/forge_repo/src/agent_definition.rs#L16): Agent files configure model/provider/prompt templates/tools/compact/maxturns; these are declarative policies.

### Evidence e26

[quarantine/forgecode/crates/forge_domain/src/hook.rs:103–165](../../../quarantine/forgecode/crates/forge_domain/src/hook.rs#L103): Compiled lifecycle EventHandle can mutate Conversation and handlers compose in order.

### Evidence e27

[quarantine/forgecode/crates/forge_app/src/hooks/pending_todos.rs:42–132](../../../quarantine/forgecode/crates/forge_app/src/hooks/pending_todos.rs#L42): End hook injects pending-todo reminder once per same set; unchanged pending items need not prevent second Stop completion.

### Evidence e28

[quarantine/forgecode/crates/forge_app/src/orch_spec/orch_spec.rs:605–658](../../../quarantine/forgecode/crates/forge_app/src/orch_spec/orch_spec.rs#L605): Source oracle asserts reminder content; two Stop replies finish even though todos remain pending.

### Evidence e29

[quarantine/forgecode/crates/forge_stream/src/mpsc_stream.rs:64–99](../../../quarantine/forgecode/crates/forge_stream/src/mpsc_stream.rs#L64): Virtual-time Drop test asserts task completed flag remains false, not OS descendant or save durability.

### Evidence e30

[quarantine/forgecode/crates/forge_app/src/compact.rs:574–666](../../../quarantine/forgecode/crates/forge_app/src/compact.rs#L574): Compaction oracle checks usage accumulation across destroyed original messages and surviving messages.

### Evidence e31

[quarantine/forgecode/crates/forge_app/src/agent.rs:296–332](../../../quarantine/forgecode/crates/forge_app/src/agent.rs#L296): Compaction merge regression explicitly expects default agent retention_window zero to override workflow retention ten.

### Evidence e32

[quarantine/forgecode/crates/forge_app/src/orch_spec/orch_spec.rs:408–493](../../../quarantine/forgecode/crates/forge_app/src/orch_spec/orch_spec.rs#L408): Repeated-call test asserts fourth successful action retained and next-request reminder follows triggering history.

### Evidence e33

[quarantine/forgecode/crates/forge_app/src/mcp_executor.rs:16–41](../../../quarantine/forgecode/crates/forge_app/src/mcp_executor.rs#L16): MCP discovery accepts legacy and Claude name styles and routes tool call through service.

### Evidence e34

[quarantine/forgecode/crates/forge_repo/src/provider/chat.rs:65–113](../../../quarantine/forgecode/crates/forge_repo/src/provider/chat.rs#L65): Provider routing and model-cache refresh task retained via abort handle, separate from chat request task.

### Evidence e35

[quarantine/forgecode/crates/forge_domain/src/tools/catalog.rs:592–720](../../../quarantine/forgecode/crates/forge_domain/src/tools/catalog.rs#L592): Concrete shell/fetch/followup/plan/skill/todo arguments.

### Evidence e36

[quarantine/forgecode/crates/forge_domain/src/tools/catalog.rs:903–923](../../../quarantine/forgecode/crates/forge_domain/src/tools/catalog.rs#L903): Tool name lookup normalizes catalog names; followup asks orchestrator to yield.

