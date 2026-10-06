# vtcode

A broad Rust agent with planning, tracked execution, parent/child control, dynamic skill/catalog changes, multi-strategy compaction and canonical retained event stores. Strong persistence/error oracles coexist with limited audit retention and a source-inferred repeated-child-follow-up race.

Role: coding agent. Runtime: Rust.

Pinned source: [https://github.com/vinhnx/VTCode](https://github.com/vinhnx/VTCode); revision/version `fc68c9e1f454a2ffc7da794decee27c8ebb92771`.

Rust main drives a multithreaded Tokio runtime; native unified provider/action loop and owned blocking/event-drain work. A configured codex provider selects a distinct Codex App Server adapter. Python/JavaScript snippets are fresh external interpreter processes, not the core.

Owns context, session stores, execution/PTY handles, child tasks and managed background subprocess desired state; consumes model providers and MCP/web services. These owned processes are not a shared fabric governor.

Inspection: Native entry/runtime selection through request snapshots/concrete OpenAI HTTP, tool admission/dispatch/results/continuation; public versus removed tools, execution sessions, memory/cron, children/background restoration, compaction/artifacts, canonical persistence/retention/query, skills/config activation, update restart, verifier API and relevant tests.

Limits of this study: Source read only, reference not executed. Optional Codex App Server internals, provider variants, full web implementations, OS sandbox/process-tree semantics and complete checkpoint navigation are outside this trace. No paid/network/refit/concurrent-child experiments were run. Canonical events are retained rather than unlimited originals, and the inferred Waiting/restart overlap requires an independent controlled oracle.

## Actions

### exec_command

Surface: model tool.

Input: cmd; optional workdir, tty, background, yielding/output limits and sandbox request

Result: output, exit code or reusable session_id; spool metadata when output is bounded

Lifecycle: owned pipe/PTY process; explicit waits can return in-progress; background sessions retained separately

Authority: model admitted by command, sandbox and session policy; confirmation skipping is separate

Evidence: [e14](#evidence-e14), [e15](#evidence-e15), [e16](#evidence-e16), [e18](#evidence-e18).

### write_stdin

Surface: model tool.

Input: session_id; chars or action=write/poll/wait; optional yield/wait deadline

Result: new bounded output and ongoing/completed session state

Lifecycle: wait deadline preserves the handle; actual dispatcher has no inspect/terminate/close actions

Authority: model continuation of an admitted owned session

Evidence: [e17](#evidence-e17), [e18](#evidence-e18).

### code_search

Surface: model tool.

Input: literal query or alternatives, path/type/result filters and result limit

Result: workspace definition/usage/text/path matches

Lifecycle: awaited native search; eligible readonly group overlap

Authority: model; native Allow default plus public admission

Evidence: [e13](#evidence-e13), [e54](#evidence-e54).

### apply_patch

Surface: model tool.

Input: patch input, optionally encoded input normalized by handler

Result: structured patch result/errors and affected paths

Lifecycle: awaited mutation, hooks/policy validation and code-change tracking

Authority: model; default Prompt, active policy can permit

Evidence: [e64](#evidence-e64), [e8](#evidence-e8), [e65](#evidence-e65).

### memory

Surface: model tool.

Input: command=view/create/str_replace/insert/delete/rename with virtual /memories paths and text

Result: note listing/content or mutation acknowledgement/error

Lifecycle: persistent files survive sessions; generated/other paths restricted

Authority: model; feature/config dependent, selected writable note locations

Evidence: [e21](#evidence-e21).

### cron

Surface: model tool.

Input: action=create/list/delete; prompt and exactly one cron/delay_minutes/run_at; delete ID

Result: scheduled task summaries/IDs, list or deletion record

Lifecycle: prompts scheduled inside current runtime; end at process exit

Authority: model; session tool budget and schedule validation

Evidence: [e22](#evidence-e22), [e23](#evidence-e23).

### agent: spawn / spawn_subprocess

Surface: model tool.

Input: agent type, message/items, optional model/effort, fork_context/background/max_turns

Result: child ID/status or managed subprocess record and transcript/exec identities

Lifecycle: owned child task or separate VTCode process; background desired records can respawn

Authority: model with controller, depth/concurrency/config and operator delegation signals

Evidence: [e24](#evidence-e24), [e26](#evidence-e26), [e27](#evidence-e27), [e30](#evidence-e30), [e62](#evidence-e62).

### agent: send_input / resume / wait / close

Surface: model tool.

Input: child ID(s), follow-up message/items, interrupt flag, wait timeout

Result: child status/summary/error; wait returns terminal target or timeout

Lifecycle: default follow-up queued; interrupt aborts current task and replaces queued prompts; close cascades; source-inferred repeated-follow-up overlap concern

Authority: model through active parent controller; not a peer broadcast room

Evidence: [e24](#evidence-e24), [e25](#evidence-e25), [e27](#evidence-e27), [e28](#evidence-e28), [e29](#evidence-e29), [e61](#evidence-e61).

### mcp

Surface: model tool.

Input: action=search_tools/get_tool_details/list_servers/connect/disconnect; query/name

Result: discovery results, schemas/server state or lifecycle errors

Lifecycle: external server discovery and connection management; invocation tools registered separately

Authority: model; Allow base metadata, connection action/config authority is separate

Evidence: [e14](#evidence-e14), [e69](#evidence-e69).

### search_tools

Surface: model tool.

Input: natural-language capability query and optional limit

Result: ranked session tools and expanded_for_next_segment

Lifecycle: deferred tools become available on the next request segment

Authority: model catalog discovery

Evidence: [e14](#evidence-e14), [e63](#evidence-e63).

### web_fetch / web_search

Surface: model tool catalog.

Input: URL or query and registered tool-specific parameters

Result: configured web content or search result payloads

Lifecycle: awaited external network operations; registered handlers, detailed network implementations outside this trace

Authority: model when catalog/config/provider exposure permits; fetch is prompted by default

Evidence: [e13](#evidence-e13), [e68](#evidence-e68).

### start_planning / task_tracker

Surface: model tool catalog.

Input: planning prompt/plan parameters; tracker create/update/list/add task metadata

Result: plan and persisted hierarchical task/checklist state

Lifecycle: compiled planning workflow and optional build-agent handoff; not an arbitrary workflow language

Authority: model, planning mode/action masks and policy

Evidence: [e66](#evidence-e66), [e67](#evidence-e67), [e11](#evidence-e11).

### list_skills / load_skill / load_skill_resource / activated skill names

Surface: model tool and extension catalog.

Input: skill discovery query/type, selected name or active resource path; activated tool schema

Result: skill metadata, instructions/resource content and refreshed tools

Lifecycle: activation rereads an inactive skill then refreshes tools; an already-active skill returns cached content; fork skills can delegate

Authority: model; traditional activation policy, native/plugin/system utilities have distinct contracts

Evidence: [e46](#evidence-e46), [e47](#evidence-e47).

### ToolRegistry::execute_tool / execute_public_tool_ref

Surface: programmable API.

Input: tool name and JSON args, or prepared admission record

Result: JSON value or structured error

Lifecycle: awaited registry dispatch; internal and public assemblies have different accessible names

Authority: host programmer; removed model names remain internal compatibility machinery

Evidence: [e12](#evidence-e12), [e54](#evidence-e54).

### internal command_session code / CodeExecutor::execute

Surface: supplied primitive, model-hidden.

Input: Python/JavaScript program with generated MCP/builtin SDK

Result: stdout/stderr, exit code and parsed JSON result

Lifecycle: fresh interpreter/process per snippet, temporary files and shared workspace IPC; no resident globals

Authority: host/internal code action, subject to interpreter policy; not current public model action

Evidence: [e12](#evidence-e12), [e54](#evidence-e54), [e55](#evidence-e55), [e56](#evidence-e56).

### SubagentController::verify_proposed_change

Surface: programmable API.

Input: diff description and affected file paths

Result: approved boolean, issues and reviewer reasoning

Lifecycle: isolated bounded child review, sixty-second wait; no verifier rejects; keyword fallback if decision absent

Authority: host caller; not a registered agent action and not default reconciliation path

Evidence: [e31](#evidence-e31), [e32](#evidence-e32).

### session-store list / inspect / facts / pack / migrate / gc

Surface: operator command.

Input: session/limits, audit pack output or verification path, retention/migration options

Result: retained store metadata/facts, hash comparison and reported missing/modified/new files

Lifecycle: inspect retained local material; GC/migrate mutate operator-owned stores

Authority: operator command; packing verifies current contents, not completeness of observations

Evidence: [e45](#evidence-e45).

### /feedback

Surface: operator command.

Input: no arguments

Result: opens GitHub issue chooser URL or prints opening error/URL

Lifecycle: browser action, no tracked complaint or captured run state

Authority: operator via external URL approval/guard

Evidence: [e53](#evidence-e53).

### queued update relaunch

Surface: operator/update API.

Input: relaunch preference captured after update

Result: replacement executable or manual restart diagnostic

Lifecycle: post-dispatch exec/spawn retains argv/cwd, without outpost/provider/program state transfer

Authority: operator update path

Evidence: [e50](#evidence-e50), [e51](#evidence-e51), [e52](#evidence-e52).

### defuddle_fetch

Surface: supplied primitive, model-hidden.

Input: remote http(s) URL and optional markdown byte limit

Result: bounded extracted markdown or error

Lifecycle: registered native network operation; hidden from ordinary model catalog

Authority: host/internal tool invocation rather than advertised model tool

Evidence: [e68](#evidence-e68).

## Capabilities

### filesystem

**I — Files** (source): Public model paths are native code_search/apply_patch and shell filesystem operations; many older read_file/list_files/edit_file registrations are expressly removed from public routes. Memory notes add a separate bounded file surface.

Evidence: [e12](#evidence-e12), [e13](#evidence-e13), [e15](#evidence-e15), [e21](#evidence-e21), [e64](#evidence-e64).

### processes

**I — OS programs** (source): Owned pipe/PTY execution sessions have output spooling, yielding and background retention; wait/poll/input continue handles. Full close and foreground-only cancellation differ; strong test observes actual reaped PTY exit.

Evidence: [e16](#evidence-e16), [e17](#evidence-e17), [e18](#evidence-e18), [e19](#evidence-e19), [e20](#evidence-e20).

### code-actions

**S — Code actions** (source): Internal code action builds Python/JavaScript SDK programs that can call MCP and curated builtins through IPC, but that legacy public name is removed. Model can still invoke external interpreters through exec_command; no claim of current dedicated code tool.

Evidence: [e12](#evidence-e12), [e54](#evidence-e54), [e55](#evidence-e55), [e56](#evidence-e56).

### persistent-kernel

**L — Kernel** (source): Harness kernel is compiled catalog/mode filtering. Inspected code executor creates and deletes a fresh program/process, so neither supplies a resident namespace; externally hosted kernels could be composed through tools.

Evidence: [e54](#evidence-e54), [e55](#evidence-e55), [e56](#evidence-e56), [e57](#evidence-e57).

### standing-database

**S — Standing DB** (source): Persistent native memory files and session-store query_facts provide standing local notes/facts. Neither inspected model surface is a general shared standing database with arbitrary queries; external DB use remains composition.

Evidence: [e21](#evidence-e21), [e45](#evidence-e45).

### workflow-programming

**S — Workflows** (source): Planning/task tools, session cron, executable lifecycle hooks, dynamic traditional skill tools and child fork execution are composable workflow pieces. The compiled loop/catalog filter is not an operator/model-defined turn/workflow program.

Evidence: [e8](#evidence-e8), [e22](#evidence-e22), [e23](#evidence-e23), [e46](#evidence-e46), [e47](#evidence-e47), [e57](#evidence-e57), [e66](#evidence-e66), [e67](#evidence-e67).

### multi-model

**I — Models** (source): Child spawn accepts model/effort overrides and tests observe an effective override; parent settings can switch the next request. Model resolution supports configured provider/model boundaries, not unrestricted peer role creation.

Evidence: [e4](#evidence-e4), [e26](#evidence-e26), [e62](#evidence-e62).

### live-collaboration

**L — Peer chat** (source): Parent can queue, interrupt/restart, resume/wait/close live children, including managed subprocesses, but there is no traced peer room. First queued follow-up changes Running to Waiting; another while Waiting calls unguarded restart/launch and overwrites its handle, source-inferred overlapping runs for one child record.

Evidence: [e24](#evidence-e24), [e25](#evidence-e25), [e27](#evidence-e27), [e28](#evidence-e28), [e29](#evidence-e29).

### concurrent-work

**I — Concurrency** (source): Native readonly tool groups overlap and independent children/retained subprocesses can run concurrently; mutations remain governed by admission/group ordering. Repeated queued input has a separate per-child ownership concern.

Evidence: [e10](#evidence-e10), [e16](#evidence-e16), [e24](#evidence-e24), [e25](#evidence-e25), [e27](#evidence-e27), [e28](#evidence-e28), [e29](#evidence-e29).

### steering-interrupt

**I — Steer/interrupt** (source): Operator steering/settings are consumed at request boundaries; Ctrl+C races a nonstreaming request. Child interrupt aborts current task and clears queued prompts, ordinary follow-ups should continue afterward; source concern in Waiting transition is retained.

Evidence: [e4](#evidence-e4), [e6](#evidence-e6), [e25](#evidence-e25).

### turn-redefinition

**L — Turn program** (source): Compiled TurnHandlerOutcome and harness mode filters govern stop/continue/primary-agent handoff. Hooks can rewrite arguments and alter context and skills can change tools, but no hot programmable replacement of the full turn protocol was established.

Evidence: [e8](#evidence-e8), [e11](#evidence-e11), [e46](#evidence-e46), [e57](#evidence-e57).

### compaction

**I — Compaction** (source): Manual/auto/recovery paths support provider-native or local summaries, reject noop/growing results, preserve opaque replay output, clear response chains and emit boundaries. Tests protect exact current-turn suffix and failure atomicity.

Evidence: [e33](#evidence-e33), [e34](#evidence-e34), [e58](#evidence-e58), [e59](#evidence-e59).

### context-repair

**S — Repair** (source): When dynamic persistence is enabled, compaction writes original rendered context plus a linked memory envelope for shell retrieval. It supports model recovery of details but lacks a traced native model-authorized restore/merge operation; history rendering and ten-file retention have limits.

Evidence: [e35](#evidence-e35), [e36](#evidence-e36), [e37](#evidence-e37).

### original-audit

**L — Original audit** (source): Production canonical events have ordered/backpressured persistence and drain error reporting, more than optional trajectory logging. Default 10,000-event retention rewrites original completed-turn bytes into counts/kinds/selected facts; rendered compaction history also changes representation and timestamps. No original-everything guarantee.

Evidence: [e37](#evidence-e37), [e38](#evidence-e38), [e39](#evidence-e39), [e40](#evidence-e40), [e41](#evidence-e41), [e42](#evidence-e42), [e43](#evidence-e43), [e44](#evidence-e44).

### audit-query

**I — Audit query** (source): Operator session-store exposes list/inspect/facts and audit pack creation/verification, with retained-turn reconstruction in the store. Verification proves current retained files match a pack, not complete original capture; model shell can inspect artifacts.

Evidence: [e43](#evidence-e43), [e45](#evidence-e45).

### hot-change

**I — Hot change** (source): Model/effort applies after an in-flight request at the next request boundary, possibly before the current turn ends; accepted idle config reload preserves old state on parse errors. Model skill activation refreshes catalog but active skill content is cached. These differ from after-whole-turn/workflow activation.

Evidence: [e4](#evidence-e4), [e47](#evidence-e47), [e48](#evidence-e48), [e49](#evidence-e49), [e63](#evidence-e63).

### rebuild-continuity

**L — Rebuild continuity** (source): Updater relaunch retains argv/cwd and replaces the process after CLI dispatch; current session is not injected unless original argv already supplies it. Desired background subprocess state can respawn but not retain process identity. Finalization can detach provider-backed memory work, so required refit quiescence/handoff is not established.

Evidence: [e30](#evidence-e30), [e50](#evidence-e50), [e51](#evidence-e51), [e52](#evidence-e52).

### remote-services

**I — Remote** (source): Consumes provider HTTP/websocket/compatibility requests and discovered MCP/web tooling; native codex-provider path delegates to a separate App Server runtime. This does not govern a shared compute/service fabric.

Evidence: [e3](#evidence-e3), [e7](#evidence-e7), [e13](#evidence-e13), [e14](#evidence-e14), [e68](#evidence-e68), [e69](#evidence-e69).

### self-improvement

**S — Self-improve** (source): Dynamic skills and a bounded verifier API support changing programs/instructions and reviewing changes. Default worktree reconciliation uses a heuristic verifier, not that API. No governed measured harness autoresearch or executable refit protocol was established.

Evidence: [e31](#evidence-e31), [e32](#evidence-e32), [e46](#evidence-e46), [e47](#evidence-e47), [e50](#evidence-e50).

### complaints

**L — Complaints** (source): Operator /feedback opens a GitHub issue chooser. It neither exposes model rageshake nor captures context/agent state into a tracked external complaint DB or bead.

Evidence: [e53](#evidence-e53).

### authority

**I — Authority** (source): Admission revalidates hook-rewritten args, then checks concrete command/sandbox and configured safety/permission policies, agent mode and delegation hints. skip_confirmations changes interaction rather than all execution constraints.

Evidence: [e8](#evidence-e8), [e15](#evidence-e15), [e26](#evidence-e26), [e57](#evidence-e57), [e65](#evidence-e65).

### evaluation

**I — Evaluation** (source): Inspected source tests check failed-compaction history identity, exact suffix/boundary count, 3,253 canonical events/unique IDs, real PTY reaping and layered reload rollback. Synthetic tree tests do not prove live message isolation; no tests were executed.

Evidence: [e20](#evidence-e20), [e48](#evidence-e48), [e58](#evidence-e58), [e59](#evidence-e59), [e60](#evidence-e60), [e61](#evidence-e61).

### time-order

**I — Time/order** (source): Serialized canonical event admission preserves caller order; prompt schedules distinguish cron, intervals and local one-shot times; request/wait cancellation uses Tokio deadlines. This is local ordering/deadlines, not a cross-service total causal clock.

Evidence: [e6](#evidence-e6), [e18](#evidence-e18), [e22](#evidence-e22), [e39](#evidence-e39).

## Inspected test oracles

- [quarantine/vtcode/crates/codegen/vtcode-core/src/tools/exec_session.rs](../../../quarantine/vtcode/crates/codegen/vtcode-core/src/tools/exec_session.rs): Force termination must reap a real PTY child, not merely return success. Oracle: Creates /bin/sh sleep child, enforces 8-second kill budget, then observes an exit status within two seconds and bounded close; read only, not executed. Read, **not executed**.
- [quarantine/vtcode/src/agent/runloop/unified/turn/compaction/tests.rs](../../../quarantine/vtcode/src/agent/runloop/unified/turn/compaction/tests.rs): Provider compaction failure and recovery prefix/suffix boundaries. Oracle: Mock failure preserves exact original vector; successful recovery preserves exact current-turn suffix, shrinks, clears response ID and emits one correct boundary. Mocks do not establish provider opaque replay conformance. Read, **not executed**.
- [quarantine/vtcode/src/agent/runloop/unified/inline_events/harness.rs](../../../quarantine/vtcode/src/agent/runloop/unified/inline_events/harness.rs): Canonical bounded-handoff drain and event identity. Oracle: After 3,253 accepted events, close must yield exactly 3,253 lines with distinct IDs; checks accepted-event persistence, not raw-provider capture or retention completeness. Read, **not executed**.
- [quarantine/vtcode/src/agent/runloop/unified/settings_interactive/mod.rs](../../../quarantine/vtcode/src/agent/runloop/unified/settings_interactive/mod.rs): Reload malformed or newly created/deleted config layer. Oracle: Observe provider changes for valid layer, exact prior provider after malformed layer, defaults after deletion. This is config activation, not executable refit. Read, **not executed**.
- [quarantine/vtcode/crates/codegen/vtcode-core/src/subagents/tests.rs](../../../quarantine/vtcode/crates/codegen/vtcode-core/src/subagents/tests.rs): Close/resume descendant cascade and model override. Oracle: Synthetic parent/child/grandchild records become closed then queued; separate override test observes effective model. Neither drives two noninterrupt messages to an actually busy child, the discriminating race oracle still missing. Read, **not executed**.

## Useful mechanisms

- Production canonical sink is opened before session, isolates optional exporters, backpressures and reports drain errors.
- Compaction failure atomicity, current-turn suffix preservation and conditional history artifacts explicitly address recoverability.
- Execution handle lifecycle has a strong bounded PTY reap assertion; readonly group parallelism and child restoration are concrete mechanisms.
- Dynamic traditional skills refresh session tools and deferred tools activate at a named request segment.

## Material limits

- Optional Codex App Server internals, provider variants, full web implementations, OS sandbox/process-tree semantics and complete checkpoint navigation are outside this trace. No paid/network/refit/concurrent-child experiments were run. Canonical events are retained rather than unlimited originals, and the inferred Waiting/restart overlap requires an independent controlled oracle.

## Arconaut design questions

- Hold one child provider response, send two noninterrupt follow-ups, and assert one active runner and one owned JoinHandle for its ID; can Waiting ever trigger a second live run?
- What records must be retained before canonical event caps or compaction to permit original-byte replay, causal study and model repair?
- Should a next-request model/catalog activation wait for the entire current turn or affected workflow by default, with explicit interruption for immediate activation?
- Before update/refit, can an oracle prove zero owned live commands and provider requests, including detached persistent-memory finalization, and preserve context through an outpost?
- How can externally shared kernels/DBs be consumed through these action handles without moving their lifecycle governance into the harness?

## Evidence

### Evidence e1

[quarantine/vtcode/src/main.rs:83–117](../../../quarantine/vtcode/src/main.rs#L83): A bootstrap thread drives the Rust entrypoint; the runtime is not inferred from the npm distribution.

### Evidence e2

[quarantine/vtcode/src/main.rs:278–294](../../../quarantine/vtcode/src/main.rs#L278): Main builds a multithreaded Tokio runtime with configured workers.

### Evidence e3

[quarantine/vtcode/src/agent/agents.rs:125–183](../../../quarantine/vtcode/src/agent/agents.rs#L125): Provider codex selects a separate CodexSessionRuntime; other sessions select the native unified runtime.

### Evidence e4

[quarantine/vtcode/src/agent/runloop/unified/turn/turn_loop.rs:913–940](../../../quarantine/vtcode/src/agent/runloop/unified/turn/turn_loop.rs#L913): Steering and pending model/effort settings are applied before the next request, after the previous in-flight request finished, potentially within the same turn.

### Evidence e5

[quarantine/vtcode/src/agent/runloop/unified/turn/turn_processing/llm_request/mod.rs:162–230](../../../quarantine/vtcode/src/agent/runloop/unified/turn/turn_processing/llm_request/mod.rs#L162): Each request captures current model/capability/tool state and constructs and validates the provider request.

### Evidence e6

[quarantine/vtcode/src/agent/runloop/unified/turn/turn_processing/llm_request/mod.rs:433–475](../../../quarantine/vtcode/src/agent/runloop/unified/turn/turn_processing/llm_request/mod.rs#L433): Nonstreaming generation races a Ctrl+C notification and keepalive timer, so cancellation can drop a waiting provider future.

### Evidence e7

[quarantine/vtcode/crates/codegen/vtcode-llm/src/providers/openai/provider/generation.rs:286–349](../../../quarantine/vtcode/crates/codegen/vtcode-llm/src/providers/openai/provider/generation.rs#L286): Concrete OpenAI generation builds a Responses payload and submits authorized HTTP, with distinct streaming/websocket and compatibility paths.

### Evidence e8

[quarantine/vtcode/src/agent/runloop/unified/tool_pipeline/execution_run.rs:125–256](../../../quarantine/vtcode/src/agent/runloop/unified/tool_pipeline/execution_run.rs#L125): Admission and pre-tool hooks precede safety/permission checks; rewritten arguments are revalidated rather than inheriting old validation.

### Evidence e9

[quarantine/vtcode/src/agent/runloop/unified/turn/tool_outcomes/dispatch.rs:8–115](../../../quarantine/vtcode/src/agent/runloop/unified/turn/tool_outcomes/dispatch.rs#L8): Prepared calls dispatch to batch or individual execution and recover blocked/preflight responses on exits.

### Evidence e10

[quarantine/vtcode/src/agent/runloop/unified/turn/tool_outcomes/handlers_batch.rs:159–321](../../../quarantine/vtcode/src/agent/runloop/unified/turn/tool_outcomes/handlers_batch.rs#L159): Admitted read-only parallel groups use FuturesUnordered; cancel notifiers race execution and results are recorded as each completes.

### Evidence e11

[quarantine/vtcode/src/agent/runloop/unified/turn/tool_outcomes/execution_result.rs:150–241](../../../quarantine/vtcode/src/agent/runloop/unified/turn/tool_outcomes/execution_result.rs#L150): Success/failure/timeout/cancellation is paired with the call; model history is updated before UI rendering, then continuation or compiled primary-agent handoff is selected.

### Evidence e12

[quarantine/vtcode/crates/codegen/vtcode-core/src/tools/registry/assembly.rs:96–139](../../../quarantine/vtcode/crates/codegen/vtcode-core/src/tools/registry/assembly.rs#L96): Public routing expressly excludes legacy unified action and read_file/list_files/write_file/edit_file names; internal registration does not establish model exposure.

### Evidence e13

[quarantine/vtcode/crates/codegen/vtcode-core/src/tools/registry/builtins.rs:168–234](../../../quarantine/vtcode/crates/codegen/vtcode-core/src/tools/registry/builtins.rs#L168): Agent and code_search are exposed native operations; agent aliases are registered, and web_fetch defaults to prompted permission.

### Evidence e14

[quarantine/vtcode/crates/codegen/vtcode-core/src/tools/registry/builtins.rs:296–357](../../../quarantine/vtcode/crates/codegen/vtcode-core/src/tools/registry/builtins.rs#L296): mcp and search_tools are exposed discovery/management operations; exec_command/write_stdin are model operations, while exec_pty_cmd is hidden.

### Evidence e15

[quarantine/vtcode/crates/codegen/vtcode-core/src/tools/registry/executors.rs:279–345](../../../quarantine/vtcode/crates/codegen/vtcode-core/src/tools/registry/executors.rs#L279): Shell preparation validates concrete command policy and sandbox scope before execution; skipping confirmation does not remove these checks.

### Evidence e16

[quarantine/vtcode/crates/codegen/vtcode-core/src/tools/registry/executors/exec_sessions.rs:32–150](../../../quarantine/vtcode/crates/codegen/vtcode-core/src/tools/registry/executors/exec_sessions.rs#L32): Command execution creates an owned pipe or PTY session, captures output, and returns stable session metadata with bounded yielding/background retention.

### Evidence e17

[quarantine/vtcode/crates/codegen/vtcode-core/src/tools/registry/executors.rs:647–670](../../../quarantine/vtcode/crates/codegen/vtcode-core/src/tools/registry/executors.rs#L647): write_stdin actually dispatches only write, poll or wait; broader terminate/inspect/close language elsewhere does not expand this dispatcher.

### Evidence e18

[quarantine/vtcode/crates/common/vtcode-utility-tool-specs/src/lib.rs:218–269](../../../quarantine/vtcode/crates/common/vtcode-utility-tool-specs/src/lib.rs#L218): Public execution schemas expose cmd/background/tty/workdir/output limits and session wait deadlines; a wait timeout returns an ongoing handle instead of killing it.

### Evidence e19

[quarantine/vtcode/crates/codegen/vtcode-core/src/tools/exec_session.rs:1500–1559](../../../quarantine/vtcode/crates/codegen/vtcode-core/src/tools/exec_session.rs#L1500): Execution manager closes all owned sessions for full termination, while active-session cancellation excludes retained background sessions.

### Evidence e20

[quarantine/vtcode/crates/codegen/vtcode-core/src/tools/exec_session.rs:2770–2807](../../../quarantine/vtcode/crates/codegen/vtcode-core/src/tools/exec_session.rs#L2770): The force-termination test checks a bounded return and a real observed exit status, then verifies close completes.

### Evidence e21

[quarantine/vtcode/crates/codegen/vtcode-core/src/tools/native_memory.rs:15–107](../../../quarantine/vtcode/crates/codegen/vtcode-core/src/tools/native_memory.rs#L15): Native memory offers view/create/str_replace/insert/delete/rename under /memories; writable preferences/repository facts/notes are persistent files, not a general DB.

### Evidence e22

[quarantine/vtcode/crates/codegen/vtcode-core/src/tools/registry/executors.rs:155–238](../../../quarantine/vtcode/crates/codegen/vtcode-core/src/tools/registry/executors.rs#L155): cron creates, lists and deletes session prompt tasks from exactly one cron/fixed-interval/local-time schedule.

### Evidence e23

[quarantine/vtcode/crates/codegen/vtcode-core/src/tools/registry/builtins.rs:99–126](../../../quarantine/vtcode/crates/codegen/vtcode-core/src/tools/registry/builtins.rs#L99): The cron model description explicitly scopes scheduled prompts to the current VTCode process lifetime.

### Evidence e24

[quarantine/vtcode/crates/codegen/vtcode-core/src/tools/registry/executors/subagents.rs:12–112](../../../quarantine/vtcode/crates/codegen/vtcode-core/src/tools/registry/executors/subagents.rs#L12): The exposed agent dispatcher requires an active controller and calls spawn, managed subprocess, send_input, resume, wait and close.

### Evidence e25

[quarantine/vtcode/crates/codegen/vtcode-core/src/subagents/controller_spawn_run.rs:194–243](../../../quarantine/vtcode/crates/codegen/vtcode-core/src/subagents/controller_spawn_run.rs#L194): First noninterrupt input marks Running/Queued as Waiting and queues work; interrupt aborts the handle and replaces queued prompts, while other states launch a restart.

### Evidence e26

[quarantine/vtcode/crates/codegen/vtcode-core/src/subagents/controller_spawn_run.rs:821–883](../../../quarantine/vtcode/crates/codegen/vtcode-core/src/subagents/controller_spawn_run.rs#L821): Delegation checks operator turn hints/selected agent; write-capable automatic delegation is restricted, with explicit agent choice providing another admission path.

### Evidence e27

[quarantine/vtcode/crates/codegen/vtcode-core/src/subagents/controller_child_loop.rs:40–112](../../../quarantine/vtcode/crates/codegen/vtcode-core/src/subagents/controller_child_loop.rs#L40): launch_child spawns a fresh task and overwrites the stored handle; child_loop dequeues work by child ID. It does not guard against another live task for that ID.

### Evidence e28

[quarantine/vtcode/crates/codegen/vtcode-core/src/subagents/controller_spawn_run.rs:1058–1077](../../../quarantine/vtcode/crates/codegen/vtcode-core/src/subagents/controller_spawn_run.rs#L1058): restart_child fills an empty queue from last prompt and unconditionally calls launch_child, without cancelling or checking an existing handle.

### Evidence e29

[quarantine/vtcode/crates/codegen/vtcode-core/src/subagents/types.rs:421–470](../../../quarantine/vtcode/crates/codegen/vtcode-core/src/subagents/types.rs#L421): Each child run dequeues a prompt and later mutates shared status, summary and stored context; two concurrent loops for one record can race those updates.

### Evidence e30

[quarantine/vtcode/crates/codegen/vtcode-core/src/subagents/controller_background_ops.rs:298–339](../../../quarantine/vtcode/crates/codegen/vtcode-core/src/subagents/controller_background_ops.rs#L298): Background restoration reads desired-enabled records and respawns helpers whose exec-session identity is no longer live; this is restoration of intent, not old OS process identity.

### Evidence e31

[quarantine/vtcode/crates/codegen/vtcode-core/src/subagents/controller_verify.rs:49–135](../../../quarantine/vtcode/crates/codegen/vtcode-core/src/subagents/controller_verify.rs#L49): A programmable verifier API spawns a bounded read-only review child and parses explicit decisions/issue lines with a keyword fallback; no verifier rejects.

### Evidence e32

[quarantine/vtcode/crates/codegen/vtcode-core/src/subagents/controller_verify.rs:153–200](../../../quarantine/vtcode/crates/codegen/vtcode-core/src/subagents/controller_verify.rs#L153): Automatic worktree reconciliation instead uses HeuristicDiffVerifier inside spawn_blocking, not the asynchronous LLM-verifier API.

### Evidence e33

[quarantine/vtcode/src/agent/runloop/unified/turn/compaction/mod.rs:308–411](../../../quarantine/vtcode/src/agent/runloop/unified/turn/compaction/mod.rs#L308): Manual compaction supports provider standalone/native-inline/local strategies, operates on cloned input and rejects unchanged/nonshrinking results.

### Evidence e34

[quarantine/vtcode/src/agent/runloop/unified/turn/compaction/mod.rs:552–649](../../../quarantine/vtcode/src/agent/runloop/unified/turn/compaction/mod.rs#L552): Compaction runs a precompact hook, preserves opaque provider replay output, writes a memory/history envelope first, replaces history and resets response-chain/token state.

### Evidence e35

[quarantine/vtcode/crates/codegen/vtcode-core/src/compaction/memory_envelope.rs:528–587](../../../quarantine/vtcode/crates/codegen/vtcode-core/src/compaction/memory_envelope.rs#L528): Original-context text artifacts are written only when dynamic context and persist_history are enabled; the memory envelope links the artifact for later retrieval.

### Evidence e36

[quarantine/vtcode/crates/codegen/vtcode-core/src/context/history_files.rs:29–51](../../../quarantine/vtcode/crates/codegen/vtcode-core/src/context/history_files.rs#L29): History files default to a ten-file retention cap; they are configurable persisted context, not an unlimited archive.

### Evidence e37

[quarantine/vtcode/crates/codegen/vtcode-core/src/context/history_files.rs:422–487](../../../quarantine/vtcode/crates/codegen/vtcode-core/src/context/history_files.rs#L422): History conversion renders text, reasoning and tool calls into strings and gives messages a shared conversion timestamp; it is not original typed/wire payload preservation.

### Evidence e38

[quarantine/vtcode/src/agent/runloop/unified/inline_events/harness.rs:83–108](../../../quarantine/vtcode/src/agent/runloop/unified/inline_events/harness.rs#L83): Production constructs an authoritative canonical session sink before the session, independently of optional legacy exports.

### Evidence e39

[quarantine/vtcode/src/agent/runloop/unified/inline_events/harness.rs:184–216](../../../quarantine/vtcode/src/agent/runloop/unified/inline_events/harness.rs#L184): Canonical event handoff serializes caller order and can backpressure; optional WebMCP consumer failures do not disable canonical persistence.

### Evidence e40

[quarantine/vtcode/src/agent/runloop/unified/inline_events/harness.rs:325–379](../../../quarantine/vtcode/src/agent/runloop/unified/inline_events/harness.rs#L325): Finalization drains canonical persistence and reports failures, while legacy export losses are only diagnostics.

### Evidence e41

[quarantine/vtcode/crates/codegen/vtcode-core/src/core/agent/events/mod.rs:210–239](../../../quarantine/vtcode/crates/codegen/vtcode-core/src/core/agent/events/mod.rs#L210): The canonical sink opens the event store with DEFAULT_MAX_EVENTS and drains accepted records on a blocking worker through a bounded queue.

### Evidence e42

[quarantine/vtcode/crates/codegen/vtcode-memory/src/event_log.rs:21–36](../../../quarantine/vtcode/crates/codegen/vtcode-memory/src/event_log.rs#L21): Canonical event retention defaults to 10,000 events, retaining a summary before oldest completed turns are evicted.

### Evidence e43

[quarantine/vtcode/crates/codegen/vtcode-memory/src/event_log.rs:580–605](../../../quarantine/vtcode/crates/codegen/vtcode-memory/src/event_log.rs#L580): Retention replaces the event file with remaining bytes after writing a summary; original evicted event bytes are removed.

### Evidence e44

[quarantine/vtcode/crates/codegen/vtcode-memory/src/event_log.rs:1054–1076](../../../quarantine/vtcode/crates/codegen/vtcode-memory/src/event_log.rs#L1054): The default eviction artifact holds counts, event kinds and selected grounded facts, rather than complete evicted events.

### Evidence e45

[quarantine/vtcode/src/cli/session_store.rs:16–103](../../../quarantine/vtcode/src/cli/session_store.rs#L16): Operator session-store can migrate/GC/list/inspect/query facts and create or verify an audit pack against retained files.

### Evidence e46

[quarantine/vtcode/src/agent/runloop/unified/session_setup/skill_setup.rs:39–89](../../../quarantine/vtcode/src/agent/runloop/unified/session_setup/skill_setup.rs#L39): Native list_skills/load_skill_resource/load_skill tools are wired to the session registry, snapshot notification and optional child-agent fork executor.

### Evidence e47

[quarantine/vtcode/crates/codegen/vtcode-core/src/tools/skills/mod.rs:104–151](../../../quarantine/vtcode/crates/codegen/vtcode-core/src/tools/skills/mod.rs#L104): Traditional skill activation registers a new tool and refreshes current catalog; active skills return AlreadyActive and do not reread their content.

### Evidence e48

[quarantine/vtcode/src/agent/runloop/unified/settings_interactive/mod.rs:1681–1713](../../../quarantine/vtcode/src/agent/runloop/unified/settings_interactive/mod.rs#L1681): Config reload test observes valid creation/deletion and preserves the last valid draft after malformed input.

### Evidence e49

[quarantine/vtcode/src/agent/runloop/unified/turn/session_loop_runner/orchestration.rs:2703–2721](../../../quarantine/vtcode/src/agent/runloop/unified/turn/session_loop_runner/orchestration.rs#L2703): Idle session reload publishes accepted config and retains old configuration on rejected reloads.

### Evidence e50

[quarantine/vtcode/src/main_helpers/relaunch.rs:48–111](../../../quarantine/vtcode/src/main_helpers/relaunch.rs#L48): Update relaunch preserves original argv/cwd and flushes output, then Unix exec replaces the process; it does not create an outpost or inject current session state.

### Evidence e51

[quarantine/vtcode/src/agent/runloop/unified/turn/session_loop_runner/orchestration.rs:2630–2675](../../../quarantine/vtcode/src/agent/runloop/unified/turn/session_loop_runner/orchestration.rs#L2630): Normal finalization may detach a provider-backed persistent-memory task after a five-second kickoff wait; this does not establish no-provider-request quiescence.

### Evidence e52

[quarantine/vtcode/src/agent/runloop/unified/turn/session_loop_runner/orchestration.rs:2687–2700](../../../quarantine/vtcode/src/agent/runloop/unified/turn/session_loop_runner/orchestration.rs#L2687): Exit/cancel/error performs best-effort owned-exec cleanup, while other transition reasons do not establish global quiescence.

### Evidence e53

[quarantine/vtcode/src/agent/runloop/unified/turn/session/slash_commands/apps.rs:83–103](../../../quarantine/vtcode/src/agent/runloop/unified/turn/session/slash_commands/apps.rs#L83): Operator feedback opens an ordinary GitHub new-issue URL through a guarded browser action, without agent-state complaint capture.

### Evidence e54

[quarantine/vtcode/crates/codegen/vtcode-core/src/tools/registry/executors.rs:453–514](../../../quarantine/vtcode/crates/codegen/vtcode-core/src/tools/registry/executors.rs#L453): Code-search dispatch is native. Hidden code execution spawns a configured external interpreter and supplies an MCP/builtin SDK, rather than a resident kernel.

### Evidence e55

[quarantine/vtcode/crates/codegen/vtcode-core/src/exec/code_executor.rs:204–238](../../../quarantine/vtcode/crates/codegen/vtcode-core/src/exec/code_executor.rs#L204): Code execution generates SDK plus a unique temporary program; the IPC directory is workspace-wide.

### Evidence e56

[quarantine/vtcode/crates/codegen/vtcode-core/src/exec/code_executor.rs:406–435](../../../quarantine/vtcode/crates/codegen/vtcode-core/src/exec/code_executor.rs#L406): Code execution runs a fresh process, parses stdout JSON and removes program/IPC files, with a bounded wait for the IPC handler.

### Evidence e57

[quarantine/vtcode/crates/codegen/vtcode-core/src/core/agent/harness_kernel.rs:11–118](../../../quarantine/vtcode/crates/codegen/vtcode-core/src/core/agent/harness_kernel.rs#L11): Harness kernel functions filter compiled tool catalogs/mode/action exposure; they are not a programmable computation kernel or a replaceable turn program.

### Evidence e58

[quarantine/vtcode/src/agent/runloop/unified/turn/compaction/tests.rs:1502–1529](../../../quarantine/vtcode/src/agent/runloop/unified/turn/compaction/tests.rs#L1502): A failing mock compactor must preserve exact preexisting history.

### Evidence e59

[quarantine/vtcode/src/agent/runloop/unified/turn/compaction/tests.rs:1732–1788](../../../quarantine/vtcode/src/agent/runloop/unified/turn/compaction/tests.rs#L1732): Recovery-compaction test checks an identical current-turn suffix, shrinking prefix, cleared response ID and exactly one boundary event.

### Evidence e60

[quarantine/vtcode/src/agent/runloop/unified/inline_events/harness.rs:762–798](../../../quarantine/vtcode/src/agent/runloop/unified/inline_events/harness.rs#L762): Canonical drain test writes 3,253 events then checks exact line count and unique event IDs.

### Evidence e61

[quarantine/vtcode/crates/codegen/vtcode-core/src/subagents/tests.rs:2374–2421](../../../quarantine/vtcode/crates/codegen/vtcode-core/src/subagents/tests.rs#L2374): Synthetic child-tree records verify close cascades and reopen queues descendants; this does not prove live concurrent execution isolation.

### Evidence e62

[quarantine/vtcode/crates/codegen/vtcode-core/src/subagents/tests.rs:1470–1496](../../../quarantine/vtcode/crates/codegen/vtcode-core/src/subagents/tests.rs#L1470): Model-override test observes effective child model selection; model choice is distinct from a shared colleague room.

### Evidence e63

[quarantine/vtcode/crates/common/vtcode-utility-tool-specs/src/lib.rs:270–293](../../../quarantine/vtcode/crates/common/vtcode-utility-tool-specs/src/lib.rs#L270): search_tools explicitly expands deferred tools for the next request segment rather than instantly mutating an in-flight request.

### Evidence e64

[quarantine/vtcode/crates/codegen/vtcode-core/src/tools/registry/builtins.rs:470–490](../../../quarantine/vtcode/crates/codegen/vtcode-core/src/tools/registry/builtins.rs#L470): apply_patch is a prompted native operation and dynamic skill catalog names are explicitly identified.

### Evidence e65

[quarantine/vtcode/src/agent/runloop/unified/tool_pipeline/execution_run.rs:638–710](../../../quarantine/vtcode/src/agent/runloop/unified/tool_pipeline/execution_run.rs#L638): Permission flow applies approvals/denials/interrupts with runtime agent permissions, policy and skip_confirmations; hook phases are not rerun.

### Evidence e66

[quarantine/vtcode/crates/codegen/vtcode-core/src/tools/registry/builtins.rs:126–164](../../../quarantine/vtcode/crates/codegen/vtcode-core/src/tools/registry/builtins.rs#L126): start_planning and task_tracker are registered native tools with workflow-dependent schemas and checklist aliases.

### Evidence e67

[quarantine/vtcode/crates/codegen/vtcode-core/src/tools/handlers/planning_task_tracker.rs:44–92](../../../quarantine/vtcode/crates/codegen/vtcode-core/src/tools/handlers/planning_task_tracker.rs#L44): Planning tracker actions create/update/list/add carry hierarchical indexes, file/outcome and verification-command metadata.

### Evidence e68

[quarantine/vtcode/crates/codegen/vtcode-core/src/tools/registry/builtins.rs:234–294](../../../quarantine/vtcode/crates/codegen/vtcode-core/src/tools/registry/builtins.rs#L234): web_search is a prompted public query tool; defuddle_fetch is registered but explicitly model-hidden.

### Evidence e69

[quarantine/vtcode/crates/codegen/vtcode-core/src/tools/registry/executors.rs:607–635](../../../quarantine/vtcode/crates/codegen/vtcode-core/src/tools/registry/executors.rs#L607): Native MCP action dispatch actually invokes search/details/list/connect/disconnect handlers.

