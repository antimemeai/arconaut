# opendev

Rust migrated coding agent with schema deferral, custom executable tools, staged compaction and genuine asynchronous multi-model teammates; custody and history boundaries need scrutiny.

Role: coding agent. Runtime: Rust.

Pinned source: [https://github.com/opendev-to/opendev](https://github.com/opendev-to/opendev); revision/version `d32c660e4eed1a8e988d1fd58da88e41ba641d08`.

Rust multi-crate Tokio/ReAct core; typed tools; CLI/TUI/web surfaces; selected reqwest or curl transport

Owns local shell groups, background children, session/team/mailbox files; consumes model/MCP services. Current pinned core is Rust despite Python-family registry label.

Inspection: Primary CLI entry, loop/provider transport, default and team tool registration, dispatch, shell process/drop paths, subagent creation/resume, mailbox and sender state, compaction/session events/debug, skill/MCP changes and relevant tests.

Limits of this study: Source read only, reference not executed. Optional browser/LSP and other unregistered implementation modules not promoted into exposed capabilities; schedule executor consumer not traced. Source faults are inferred, not executed. TUI/web behavior not exhaustively traced.

## Actions

### Read / Glob / Grep

Surface: model tools.

Input: file_path/offset/limit; file patterns; search patterns

Result: text/list/search hits or error; clipped

Lifecycle: awaited; classified concurrent-safe

Authority: Model through default registry; host filesystem and sanitizer.

Evidence: [e3](#evidence-e3), [e16](#evidence-e16).

### Write / Edit

Surface: model tools.

Input: path/content/create_dirs; path/old_string/new_string/replace_all

Result: write or fuzzy replacement results

Lifecycle: awaited writes block interruption per tool declaration; outer dispatcher still selective

Authority: Model; sensitive paths refused by tools.

Evidence: [e17](#evidence-e17), [e18](#evidence-e18), [e10](#evidence-e10).

### Bash

Surface: model tool.

Input: command, timeout/run_in_background/description/workdir

Result: output/exit data or background ID/PID metadata

Lifecycle: foreground awaited; background process stored; no model-native poll/kill verbs in Bash schema

Authority: Model; approvals may be disabled, dangerous-command patterns still refused.

Evidence: [e13](#evidence-e13), [e11](#evidence-e11), [e12](#evidence-e12), [e42](#evidence-e42).

### NotebookEdit

Surface: model tool.

Input: notebook, cell selection/new source/type/op

Result: updated notebook

Lifecycle: synchronous file edit, no execution session

Authority: Model through default registry.

Evidence: [e3](#evidence-e3), [e39](#evidence-e39).

### memory

Surface: model tool.

Input: read/write/search/list, file/content/query, project/global scope and match mode

Result: memory text/files/search result

Lifecycle: persistent files, per-invocation access

Authority: Model; owned file state, not external general DB.

Evidence: [e37](#evidence-e37), [e3](#evidence-e3).

### CronCreate

Surface: model tool.

Input: create/list/remove/enable/disable; command/delay/interval

Result: saved schedule entries/status

Lifecycle: JSON standing-job declarations; actual executor consumer not inspected

Authority: Model default registered; schedule metadata only is traced.

Evidence: [e38](#evidence-e38), [e3](#evidence-e3).

### Agent

Surface: model tool.

Input: agent type/task/model, task_id, run_in_background

Result: child text/stats and task ID; optional immediate background response

Lifecycle: isolated new child loop; ID reuse labelled resume but no saved history load in this path

Authority: Model; per-agent tool policy; prevent background-in-background spawn.

Evidence: [e19](#evidence-e19), [e20](#evidence-e20), [e21](#evidence-e21).

### TeamCreate / TeamDelete

Surface: model tools.

Input: team_name, named typed tasked members

Result: team metadata/member task IDs

Lifecycle: TeamCreate idle registration; TeamDelete implementation not deeply traced

Authority: Model; shared team metadata.

Evidence: [e4](#evidence-e4), [e22](#evidence-e22).

### SpawnTeammate

Surface: model tool.

Input: team/member/type/task, model, worktree isolation

Result: task ID and asynchronous completion event

Lifecycle: Tokio task; child context/mailbox; worktree failure shared fallback

Authority: Model; shared registry and independently chosen child model.

Evidence: [e24](#evidence-e24), [e20](#evidence-e20).

### SendMessage / CheckMailbox

Surface: model tools.

Input: to/message/team; optional agent_name for CheckMailbox

Result: sent counts/errors; received unread messages

Lifecycle: file-backed send; receive marks read; automatic drain only at startup

Authority: Model; caller-supplied mailbox identity and hardcoded leader sender lack custody-bound identity.

Evidence: [e23](#evidence-e23), [e25](#evidence-e25), [e26](#evidence-e26), [e27](#evidence-e27).

### TeamAddTask / TeamListTasks / TeamClaimTask / TeamCompleteTask

Surface: model tools.

Input: team,title/description/dependencies; status filter; task_id/claimed_by; success/result

Result: shared task states and claim results

Lifecycle: task-list operations, claim checks readiness; no forced execution orchestration implied

Authority: Model; claimed_by supplied or constructor name.

Evidence: [e4](#evidence-e4), [e28](#evidence-e28).

### TodoWrite / TaskUpdate / TaskList / TaskStop / EnterPlanMode

Surface: model tools.

Input: todo/task fields and plan mode requests

Result: task state or stop/control result

Lifecycle: default tools; dispatcher special-cases TaskStop as noninterruptible

Authority: Model; only registration/control exception traced in depth.

Evidence: [e3](#evidence-e3), [e10](#evidence-e10).

### AskUserQuestion / WebFetch / WebSearch

Surface: model tools.

Input: question choices; URL/search query

Result: operator reply or network text

Lifecycle: default registered; individual network/question executors not deeply traced

Authority: Model native catalog; implementation limits disclosed.

Evidence: [e3](#evidence-e3).

### ToolSearch / Skill

Surface: model discovery / prompt tools.

Input: tool lookup; skill_name/arguments or list

Result: activated schemas/loaded prompt and optional model override

Lifecycle: tool deferral per loop; skill cache invalidates next load

Authority: Model; no transactional changed-turn activation.

Evidence: [e4](#evidence-e4), [e5](#evidence-e5), [e6](#evidence-e6), [e40](#evidence-e40).

### custom command manifests (.opendev/tools, .opencode/tool)

Surface: extension API.

Input: manifest name/description/command/schema; JSON stdin

Result: stdout/stderr parsed into tool result

Lifecycle: construction discovery; command timeout, no proved process cleanup on dropped future

Authority: Program/operator supplies executable; model calls registered schemas.

Evidence: [e14](#evidence-e14), [e15](#evidence-e15).

### MCP bridge discovery

Surface: extension API.

Input: server config and discovered schemas/prompts

Result: native registry tools and Skill prompt additions

Lifecycle: asynchronous runtime initialization; live registry mutation

Authority: Operator/program config; consumed remote service.

Evidence: [e41](#evidence-e41).

### query / compact / resume session

Surface: operator/runtime API.

Input: prompt/current session; compact; stored session selection

Result: AgentResult, compacted/saved session

Lifecycle: loop then current-session persistence; compaction/pre_count mismatch; saved data resume, no active-work continuity

Authority: Operator; compiled runtime.

Evidence: [e2](#evidence-e2), [e30](#evidence-e30), [e31](#evidence-e31).

### EventStore / debug logger

Surface: supporting audit API.

Input: normalized session events; optional normalized payload debug calls

Result: sequenced JSONL events or debug lines

Lifecycle: best-effort writes, current session is primary; no complete capture guarantee

Authority: Runtime internal, not native model audit query.

Evidence: [e32](#evidence-e32), [e33](#evidence-e33), [e34](#evidence-e34), [e35](#evidence-e35), [e36](#evidence-e36).

## Capabilities

### filesystem

**I — Files** (source): Default native file/search/edit operations and shell; sensitive-path tool checks remain with approvals off.

Evidence: [e3](#evidence-e3), [e16](#evidence-e16), [e17](#evidence-e17), [e18](#evidence-e18), [e42](#evidence-e42).

### processes

**L — OS programs** (source): Foreground group cleanup exists, but outer cancellation drops future before inner cleanup; background stored buffers are stale clones and no Bash poll/kill verbs. Custom commands lack kill-on-drop.

Evidence: [e10](#evidence-e10), [e11](#evidence-e11), [e12](#evidence-e12), [e13](#evidence-e13), [e14](#evidence-e14).

### code-actions

**I — Code actions** (source): Bash and custom JSON-stdin executable tools; notebook tool edits files only. Read-only heuristic even accepts cargo test/build-script-capable commands.

Evidence: [e13](#evidence-e13), [e14](#evidence-e14), [e39](#evidence-e39), [e48](#evidence-e48).

### persistent-kernel

**? — Kernel** (source): No notebook execution/kernel transport in inspected default tool/runtime paths.

### standing-database

**S — Standing DB** (source): Owned memory files/session events/team state and JSON schedules; general shared DB client/table action not established.

Evidence: [e27](#evidence-e27), [e32](#evidence-e32), [e37](#evidence-e37), [e38](#evidence-e38).

### workflow-programming

**S — Workflows** (source): Agent specifications, custom tools, skills and shared task dependency claims compose workflows; ReAct loop remains compiled fixed program.

Evidence: [e5](#evidence-e5), [e14](#evidence-e14), [e20](#evidence-e20), [e28](#evidence-e28), [e40](#evidence-e40).

### multi-model

**I — Models** (source): Subagent/teammate model override and spec-specific model selection before parent fallback.

Evidence: [e20](#evidence-e20), [e24](#evidence-e24), [e6](#evidence-e6).

### live-collaboration

**L — Peer chat** (source): Real running colleagues share file-backed messages, but sender is always leader and mailbox argument not custody-bound. Automatic drain occurs only before run, not per decision.

Evidence: [e23](#evidence-e23), [e24](#evidence-e24), [e25](#evidence-e25), [e26](#evidence-e26), [e27](#evidence-e27).

### concurrent-work

**I — Concurrency** (source): Early read-only-classified tools overlap provider streaming; background Bash/agents and teammate Tokio tasks; effect-free classification not guaranteed.

Evidence: [e9](#evidence-e9), [e12](#evidence-e12), [e24](#evidence-e24), [e48](#evidence-e48).

### steering-interrupt

**L — Steer/interrupt** (source): Cancellation reaches reqwest and tool context but outer select drops futures; nonstreaming curl and summary omit cancellation, foreground cleanup may be bypassed.

Evidence: [e8](#evidence-e8), [e7](#evidence-e7), [e10](#evidence-e10), [e11](#evidence-e11), [e29](#evidence-e29).

### turn-redefinition

**S — Turn program** (source): Model can invoke skills changing next prompt/model and ToolSearch schema activation; compiled loop not model-defined full turn.

Evidence: [e5](#evidence-e5), [e6](#evidence-e6), [e40](#evidence-e40).

### compaction

**L — Compaction** (source): Automatic staged masking/pruning/summary mutates effective messages; CLI persistence slices with stale pre_count. Manual compaction destructive current-session replacement.

Evidence: [e29](#evidence-e29), [e30](#evidence-e30), [e31](#evidence-e31), [e47](#evidence-e47).

### context-repair

**L — Repair** (source): Primary CLI append events may retain previous normalized messages, but no model original-audit repair action; child advertised resume is fresh messages with reused ID.

Evidence: [e19](#evidence-e19), [e21](#evidence-e21), [e32](#evidence-e32), [e34](#evidence-e34), [e35](#evidence-e35).

### original-audit

**L — Original audit** (source): Primary CLI wires sequenced normalized EventStore, but best-effort, events incomplete and saved after query; optional debug normalized payloads pre-adapter, outputs clipped before middleware.

Evidence: [e2](#evidence-e2), [e32](#evidence-e32), [e33](#evidence-e33), [e34](#evidence-e34), [e35](#evidence-e35), [e36](#evidence-e36), [e7](#evidence-e7), [e43](#evidence-e43), [e44](#evidence-e44).

### audit-query

**S — Audit query** (source): EventStore and saved sessions provide local study primitives; model memory/file tools do not expose complete structured audit query.

Evidence: [e32](#evidence-e32), [e33](#evidence-e33), [e37](#evidence-e37), [e16](#evidence-e16).

### hot-change

**L — Hot change** (source): Skills reload at next access and MCP mutates registry asynchronously; no default whole-turn/affected-workflow activation transaction.

Evidence: [e40](#evidence-e40), [e41](#evidence-e41), [e6](#evidence-e6).

### rebuild-continuity

**L — Rebuild continuity** (source): Primary saved session resume is data restoration; child resume reuses ID without history; no refit with own provider/process quiescence and external outpost.

Evidence: [e2](#evidence-e2), [e19](#evidence-e19), [e21](#evidence-e21), [e10](#evidence-e10), [e11](#evidence-e11).

### remote-services

**I — Remote** (source): Adapted provider clients and MCP bridges consume remote services; own shared-fabric governance not implied.

Evidence: [e7](#evidence-e7), [e8](#evidence-e8), [e41](#evidence-e41).

### self-improvement

**S — Self-improve** (source): File/program/skill tools allow modifications; no governed harness autoresearch and reinhabitation lifecycle traced.

Evidence: [e17](#evidence-e17), [e18](#evidence-e18), [e14](#evidence-e14), [e40](#evidence-e40).

### complaints

**? — Complaints** (source): No state-rich model complaint/bead operation in default native registry/team/loop paths.

### authority

**L — Authority** (source): Approvals can be disabled, tools still refuse patterns/sensitive paths; mailbox sender/name and task claimed_by are labels supplied without actor binding.

Evidence: [e42](#evidence-e42), [e13](#evidence-e13), [e17](#evidence-e17), [e18](#evidence-e18), [e23](#evidence-e23), [e26](#evidence-e26), [e28](#evidence-e28).

### evaluation

**I — Evaluation** (source): Native source tests for filesystem/shell/classification/mailboxes/compaction; key lifecycle and cross-layer compaction assertions absent from inspected oracles.

Evidence: [e45](#evidence-e45), [e46](#evidence-e46), [e47](#evidence-e47), [e48](#evidence-e48).

### time-order

**L — Time/order** (source): Event sequence and task/provider IDs exist; clipped/best-effort capture and detached process/colleague tasks do not establish total causal audit/custody.

Evidence: [e33](#evidence-e33), [e8](#evidence-e8), [e24](#evidence-e24), [e35](#evidence-e35), [e36](#evidence-e36), [e12](#evidence-e12).

## Inspected test oracles

- [quarantine/opendev/crates/opendev-tools-impl/src/bash/mod.rs](../../../quarantine/opendev/crates/opendev-tools-impl/src/bash/mod.rs): Process group custody/read-only classification Oracle: Cleanup test invokes helper without proving descendant death; cargo test classified read-only is asserted but arbitrary test effects not ruled out. Read, **not executed**.
- [quarantine/opendev/crates/opendev-runtime/src/mailbox/tests.rs](../../../quarantine/opendev/crates/opendev-runtime/src/mailbox/tests.rs): Concurrent inbox writes Oracle: Five writer threads and received-count oracle; identity uniqueness, sender binding and in-loop colleague delivery untested. Read, **not executed**.
- [quarantine/opendev/crates/opendev-context/src/compaction/compactor/tests.rs](../../../quarantine/opendev/crates/opendev-context/src/compaction/compactor/tests.rs): Context compaction shape Oracle: Checks shorter vector/system/summary marker, not stale CLI pre_count indexing or original audit repair. Read, **not executed**.

## Useful mechanisms

- Model-selected real asynchronous teammates and inbox/task primitives, distinct from simulated colleague sampling.
- Native tool schema deferral and custom JSON-stdin executable extension.
- Sequenced sidecar event format is a useful partial audit primitive with visible failure semantics.

## Material limits

- Optional browser/LSP and other unregistered implementation modules not promoted into exposed capabilities; schedule executor consumer not traced. Source faults are inferred, not executed. TUI/web behavior not exhaustively traced.

## Arconaut design questions

- Can child cancellation own cleanup rather than race/drop the cleanup future?
- Can live messages bind authenticated actor identity and delivery checkpoints rather than constructor labels?
- Can compaction/persistence use message identities and projection events rather than length offsets?
- Can context repair and executable refit be designed independently of current JSON chat save?

## Evidence

### Evidence e1

[quarantine/opendev/Cargo.toml:1–35](../../../quarantine/opendev/Cargo.toml#L1): Pinned OpenDev is Rust migration, edition 2024/version 0.1.9; group label does not establish Python implementation.

### Evidence e2

[quarantine/opendev/crates/opendev-cli/src/runners.rs:52–75](../../../quarantine/opendev/crates/opendev-cli/src/runners.rs#L52): Primary CLI wires EventStore and saved-session resume.

### Evidence e3

[quarantine/opendev/crates/opendev-cli/src/runtime/tools.rs:15–59](../../../quarantine/opendev/crates/opendev-cli/src/runtime/tools.rs#L15): Default exposed tools; implementation modules alone are not evidence of registration.

### Evidence e4

[quarantine/opendev/crates/opendev-cli/src/runtime/mod.rs:450–535](../../../quarantine/opendev/crates/opendev-cli/src/runtime/mod.rs#L450): Registers agent/team/mailbox/task-list/ToolSearch tools.

### Evidence e5

[quarantine/opendev/crates/opendev-agents/src/react_loop/execution.rs:118–237](../../../quarantine/opendev/crates/opendev-agents/src/react_loop/execution.rs#L118): ReAct iteration, collector/context/schema preparation, model response and dispatch continuation.

### Evidence e6

[quarantine/opendev/crates/opendev-agents/src/react_loop/phases/llm_call.rs:58–165](../../../quarantine/opendev/crates/opendev-agents/src/react_loop/phases/llm_call.rs#L58): Builds normalized payload, applies skill override, conditional debug logging and adapted provider stream/request.

### Evidence e7

[quarantine/opendev/crates/opendev-http/src/adapted_client.rs:109–223](../../../quarantine/opendev/crates/opendev-http/src/adapted_client.rs#L109): Provider request/response adapters and nonstreaming curl path with no cancellation token or kill-on-drop.

### Evidence e8

[quarantine/opendev/crates/opendev-http/src/client.rs:206–254](../../../quarantine/opendev/crates/opendev-http/src/client.rs#L206): reqwest request races cancellation with unique request ID; response body reading is a later phase.

### Evidence e9

[quarantine/opendev/crates/opendev-agents/src/react_loop/streaming_executor.rs:97–158](../../../quarantine/opendev/crates/opendev-agents/src/react_loop/streaming_executor.rs#L97): Early read-only-classified tool tasks spawned during provider stream with child token/semaphore.

### Evidence e10

[quarantine/opendev/crates/opendev-agents/src/react_loop/phases/tool_dispatch.rs:450–565](../../../quarantine/opendev/crates/opendev-agents/src/react_loop/phases/tool_dispatch.rs#L450): Outer cancellation select drops ordinary registry future; TaskStop/task_complete exceptions; result projects output/error.

### Evidence e11

[quarantine/opendev/crates/opendev-tools-impl/src/bash/foreground.rs:43–162](../../../quarantine/opendev/crates/opendev-tools-impl/src/bash/foreground.rs#L43): Shell group, output reader tasks and inner cancellation/timeout group cleanup; spawned child lacks kill_on_drop.

### Evidence e12

[quarantine/opendev/crates/opendev-tools-impl/src/bash/background.rs:146–175](../../../quarantine/opendev/crates/opendev-tools-impl/src/bash/background.rs#L146): Background handle stores clones of early output vectors while detached readers continue into separate Arc buffers.

### Evidence e13

[quarantine/opendev/crates/opendev-tools-impl/src/bash/mod.rs:229–356](../../../quarantine/opendev/crates/opendev-tools-impl/src/bash/mod.rs#L229): Bash schema command/deadlines/background/workdir, unconditional dangerous-command block, auto-background and truncation.

### Evidence e14

[quarantine/opendev/crates/opendev-tools-impl/src/custom_tool.rs:108–150](../../../quarantine/opendev/crates/opendev-tools-impl/src/custom_tool.rs#L108): Custom subprocess receives JSON stdin and wait_with_output under timeout; no group/kill-on-drop path here.

### Evidence e15

[quarantine/opendev/crates/opendev-cli/src/runtime/mod.rs:132–140](../../../quarantine/opendev/crates/opendev-cli/src/runtime/mod.rs#L132): Discovers custom command manifests at runtime construction.

### Evidence e16

[quarantine/opendev/crates/opendev-tools-impl/src/file_read/mod.rs:88–164](../../../quarantine/opendev/crates/opendev-tools-impl/src/file_read/mod.rs#L88): Read schema, concurrency classification and bounded filesystem read dispatch.

### Evidence e17

[quarantine/opendev/crates/opendev-tools-impl/src/file_write.rs:18–101](../../../quarantine/opendev/crates/opendev-tools-impl/src/file_write.rs#L18): Write schema and host path resolution; sensitive-file refusals even under autoapproval.

### Evidence e18

[quarantine/opendev/crates/opendev-tools-impl/src/file_edit.rs:39–114](../../../quarantine/opendev/crates/opendev-tools-impl/src/file_edit.rs#L39): Edit schema, fuzzy replacement and sensitive-file gating.

### Evidence e19

[quarantine/opendev/crates/opendev-tools-impl/src/agents/spawn.rs:264–382](../../../quarantine/opendev/crates/opendev-tools-impl/src/agents/spawn.rs#L264): Agent task_id only reuses saved ID, manager.spawn fresh run; output says resume but never loads prior child messages here.

### Evidence e20

[quarantine/opendev/crates/opendev-agents/src/subagents/manager/spawn.rs:96–124](../../../quarantine/opendev/crates/opendev-agents/src/subagents/manager/spawn.rs#L96): Child model selection explicit override then spec then parent.

### Evidence e21

[quarantine/opendev/crates/opendev-agents/src/subagents/manager/spawn.rs:212–255](../../../quarantine/opendev/crates/opendev-agents/src/subagents/manager/spawn.rs#L212): Child context/tool registry and newly built messages delegated to runner; mailbox and cancellation passed.

### Evidence e22

[quarantine/opendev/crates/opendev-tools-impl/src/agents/team_tools.rs:91–181](../../../quarantine/opendev/crates/opendev-tools-impl/src/agents/team_tools.rs#L91): TeamCreate registers idle metadata/members, does not launch colleagues.

### Evidence e23

[quarantine/opendev/crates/opendev-tools-impl/src/agents/team_tools.rs:250–343](../../../quarantine/opendev/crates/opendev-tools-impl/src/agents/team_tools.rs#L250): SendMessage selects team/target and labels all messages sender leader.

### Evidence e24

[quarantine/opendev/crates/opendev-tools-impl/src/agents/spawn_teammate.rs:310–435](../../../quarantine/opendev/crates/opendev-tools-impl/src/agents/spawn_teammate.rs#L310): Real independently spawned colleague with optional model/worktree and mailbox; failed worktree creation falls back to shared workdir.

### Evidence e25

[quarantine/opendev/crates/opendev-agents/src/subagents/runner/standard.rs:27–59](../../../quarantine/opendev/crates/opendev-agents/src/subagents/runner/standard.rs#L27): Mailbox is automatically drained only before ReAct; continuing colleague must call CheckMailbox.

### Evidence e26

[quarantine/opendev/crates/opendev-tools-impl/src/agents/check_mailbox.rs:29–112](../../../quarantine/opendev/crates/opendev-tools-impl/src/agents/check_mailbox.rs#L29): CheckMailbox optional agent_name selects identity; receives and marks read.

### Evidence e27

[quarantine/opendev/crates/opendev-runtime/src/mailbox/mod.rs:58–176](../../../quarantine/opendev/crates/opendev-runtime/src/mailbox/mod.rs#L58): File-backed inbox, send/receive/peek/poll and corrupt-file preservation/reset.

### Evidence e28

[quarantine/opendev/crates/opendev-tools-impl/src/agents/task_list_tools.rs:300–344](../../../quarantine/opendev/crates/opendev-tools-impl/src/agents/task_list_tools.rs#L300): TeamClaimTask trusts claimed_by argument then claims task with dependency/state checks.

### Evidence e29

[quarantine/opendev/crates/opendev-agents/src/react_loop/compaction.rs:25–158](../../../quarantine/opendev/crates/opendev-agents/src/react_loop/compaction.rs#L25): Threshold-based in-place context masking/pruning/summary; summary call has no cancellation token.

### Evidence e30

[quarantine/opendev/crates/opendev-cli/src/runtime/query.rs:253–335](../../../quarantine/opendev/crates/opendev-cli/src/runtime/query.rs#L253): pre_count before loop is reused to slice compacted result.messages; source permits wrong persistence or out-of-bounds panic.

### Evidence e31

[quarantine/opendev/crates/opendev-cli/src/runtime/query.rs:670–770](../../../quarantine/opendev/crates/opendev-cli/src/runtime/query.rs#L670): Manual compaction replaces session messages then saves.

### Evidence e32

[quarantine/opendev/crates/opendev-history/src/event_store.rs:32–85](../../../quarantine/opendev/crates/opendev-history/src/event_store.rs#L32): Normalized session/message/file events, not exhaustive provider/stream/action event vocabulary.

### Evidence e33

[quarantine/opendev/crates/opendev-history/src/event_store.rs:149–180](../../../quarantine/opendev/crates/opendev-history/src/event_store.rs#L149): JSONL sequence envelope append.

### Evidence e34

[quarantine/opendev/crates/opendev-history/src/session_manager/mod.rs:151–220](../../../quarantine/opendev/crates/opendev-history/src/session_manager/mod.rs#L151): Normalized message event on add and atomic current JSON session save.

### Evidence e35

[quarantine/opendev/crates/opendev-history/src/session_manager/mod.rs:389–398](../../../quarantine/opendev/crates/opendev-history/src/session_manager/mod.rs#L389): Event store append failure warns rather than failing operation; JSON session remains primary.

### Evidence e36

[quarantine/opendev/crates/opendev-runtime/src/debug_logger.rs:85–181](../../../quarantine/opendev/crates/opendev-runtime/src/debug_logger.rs#L85): Optional full normalized payload debug logging; append write errors ignored.

### Evidence e37

[quarantine/opendev/crates/opendev-tools-impl/src/memory.rs:32–75](../../../quarantine/opendev/crates/opendev-tools-impl/src/memory.rs#L32): Native memory read/write/search/list file abstraction and scopes; not SQL.

### Evidence e38

[quarantine/opendev/crates/opendev-tools-impl/src/schedule.rs:38–139](../../../quarantine/opendev/crates/opendev-tools-impl/src/schedule.rs#L38): CronCreate persists JSON schedule entries and CRUD; scheduler consumer not traced.

### Evidence e39

[quarantine/opendev/crates/opendev-tools-impl/src/notebook_edit.rs:23–63](../../../quarantine/opendev/crates/opendev-tools-impl/src/notebook_edit.rs#L23): NotebookEdit manipulates ipynb cells, no kernel execution.

### Evidence e40

[quarantine/opendev/crates/opendev-agents/src/skills/loader.rs:214–236](../../../quarantine/opendev/crates/opendev-agents/src/skills/loader.rs#L214): Skill mtime cache invalidation on next load.

### Evidence e41

[quarantine/opendev/crates/opendev-cli/src/runtime/query.rs:38–79](../../../quarantine/opendev/crates/opendev-cli/src/runtime/query.rs#L38): MCP connection discovery mutates shared registry and Skill prompts asynchronously.

### Evidence e42

[quarantine/opendev/crates/opendev-cli/src/runtime/mod.rs:719–725](../../../quarantine/opendev/crates/opendev-cli/src/runtime/mod.rs#L719): Headless/skip approvals disables approval layer, not native sensitive-path/command checks.

### Evidence e43

[quarantine/opendev/crates/opendev-tools-impl/src/truncation.rs:158–203](../../../quarantine/opendev/crates/opendev-tools-impl/src/truncation.rs#L158): Overflow files themselves clip beyond hard maximum, so not original capture.

### Evidence e44

[quarantine/opendev/crates/opendev-tools-core/src/registry/execution.rs:210–251](../../../quarantine/opendev/crates/opendev-tools-core/src/registry/execution.rs#L210): Sanitizer runs before after middleware/cache and returned result.

### Evidence e45

[quarantine/opendev/crates/opendev-tools-impl/src/bash/mod.rs:629–647](../../../quarantine/opendev/crates/opendev-tools-impl/src/bash/mod.rs#L629): Process-group cleanup test calls helper but has no postkill process-death oracle.

### Evidence e46

[quarantine/opendev/crates/opendev-runtime/src/mailbox/tests.rs:88–114](../../../quarantine/opendev/crates/opendev-runtime/src/mailbox/tests.rs#L88): Five writer threads and count=5 oracle; does not check identities/content uniqueness.

### Evidence e47

[quarantine/opendev/crates/opendev-context/src/compaction/compactor/tests.rs:128–150](../../../quarantine/opendev/crates/opendev-context/src/compaction/compactor/tests.rs#L128): Compaction oracle only shorter messages/first system/summary marker.

### Evidence e48

[quarantine/opendev/crates/opendev-tools-impl/src/bash/mod.rs:783–791](../../../quarantine/opendev/crates/opendev-tools-impl/src/bash/mod.rs#L783): Read-only heuristic accepts cargo test/clippy/check, which may execute build scripts/tests and write outputs.

