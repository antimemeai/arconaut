# forge-adulari

Systems Rust agent with unusually direct model-authored multi-agent workflow scripts, durable fleet peer messages, context uncompact/rewind, editable supplemental harness and isolated model comparisons.

Role: coding agent. Runtime: Rust, JavaScript.

Pinned source: [https://github.com/Adulari/forge](https://github.com/Adulari/forge); revision/version `263b45d605486b303e4044eb1580c82a887d81b6`.

Rust/Tokio provider/core/store; embedded fresh QuickJS workflow programs; OS shell jobs/CLI bridge providers and optional daemon/mobile surfaces.

Owns session SQLite, scoped memory/harness entries, child tasks and background jobs; consumes model/provider/MCP interfaces; fleet messaging requires daemon owner.

Inspection: Entry, turn admission/request/step dispatch, concrete registry/file/notebook/process contracts, actual workflow engine/host concurrency, persistent child/fleet dispatch, memory, compaction/rewind/replay/audit caps, refinement/rollback, duel and selected source tests. Large modules read in focused coherent segments.

Limits of this study: Source read only, reference not executed. Full browser/device/proxy internals, all provider bridges, assay/evaluation and OS process isolation not exhaustive. Reliability slogan is bounded by specific heuristics/oracles; no complete original audit or executable refit continuity. Configured program changes do not replace compiled outer loop.

## Actions

### read_file / write_file / append_file / edit_file / multi_edit / apply_patch / delete_file / notebook_edit / list_dir / search / glob

Surface: model native tools.

Input: Workspace path/range/content/patch/cell index or search

Result: Content, diffs/errors; notebook edits clear stale outputs

Lifecycle: Awaited; read-only batches may run concurrently, mutations under preview/snapshot/policy

Authority: Workspace/extra roots confinement and effect/rules; notebook editing does not execute cells

Evidence: [e5](#evidence-e5), [e6](#evidence-e6), [e7](#evidence-e7), [e8](#evidence-e8), [e9](#evidence-e9).

### shell / shell_job

Surface: model tools.

Input: command/cwd/timeout/pty/background/poll; job action/id/lines

Result: Exit/output or durable workspace job PID/log/status

Lifecycle: Awaited bounded command; long job descriptor survives restart; poll call yields for repeated wait; PTY stdin closed

Authority: Shell policy/rules, job workspace resolution; no interactive live-input handle in traced contract

Evidence: [e10](#evidence-e10), [e11](#evidence-e11), [e36](#evidence-e36).

### web_fetch / web_search / browser / browser_network / proxy / proxy_network / device / device_logs / lattice

Surface: model tool catalog.

Input: URL/query/browser/network/device/symbol schemas

Result: Content/search/network/device/code-structure results

Lifecycle: Registered tools; particular browser/proxy/device lifecycle not fully inspected

Authority: Effect-based policy and enabled services; catalog scope explicit

Evidence: [e7](#evidence-e7).

### run_workflow

Surface: model tool.

Input: JS script string; named agent options and host compositions

Result: JSON result, child progress and phase/log events, workflow outcome

Lifecycle: Fresh sandbox QuickJS; async host child calls share total/per-provider budget; no persistent variable realm

Authority: Model authors control flow; no ambient filesystem/network, registered host primitives only

Evidence: [e12](#evidence-e12), [e13](#evidence-e13), [e14](#evidence-e14), [e15](#evidence-e15).

### spawn_agents / send_to_agent / list_subagents / cancel_subagent

Surface: model virtual tools.

Input: Named tasks/concurrency/detached; child name/id prefix, follow-up message

Result: Independent persisted child outputs, live state or lifecycle error

Lifecycle: Tokio children; bounded depth/model pin inheritance; send reuses saved child context; detached results next parent boundary

Authority: Model delegation according to host agent definition and authority, not arbitrary external shared fabric

Evidence: [e16](#evidence-e16), [e17](#evidence-e17), [e18](#evidence-e18), [e19](#evidence-e19).

### message_session

Surface: model virtual tool in daemon.

Input: target unique name/id prefix, message, mode=follow_up|steer

Result: Persisted acceptance or resolution error

Lifecycle: Durable queue before attempted delivery; steer outranks backlog at next turn boundary, never midstream

Authority: Daemon-provided live peer roster, unavailable bare CLI; recorded allowed without fresh tool permission

Evidence: [e20](#evidence-e20), [e21](#evidence-e21).

### remember / ask_user / update_tasks / present_plan / use_skill / manage_heartbeats

Surface: model virtual tool catalog.

Input: Typed fact, question/tasks/plan/named skill; heartbeat label/interval/prompt

Result: Persistent memory/tasks/plan/user result or heartbeat records

Lifecycle: Stateful core boundary; pending plan awaits operator activation; model heartbeat slots separate from user

Authority: Model scoped owner; heartbeat cap/min interval; direct virtual dispatch may bypass general policy

Evidence: [e23](#evidence-e23), [e24](#evidence-e24), [e41](#evidence-e41).

### Session.compact / uncompact / rewind_to / checkpoint; replay_items_full

Surface: operator/program APIs.

Input: Transform mode, stable DB seq/checkpoint or history request

Result: Summary/recovered history/file restore result/replay items

Lifecycle: Compaction preserves originals soft-deleted; uncompact reload; rewind rolls files/context to boundary

Authority: Operator/program paths; precise model context repair API not traced

Evidence: [e25](#evidence-e25), [e26](#evidence-e26), [e27](#evidence-e27), [e28](#evidence-e28).

### /refine / Session.refine / refine_rollback; model pin/effort/mode controls

Surface: operator/automatic program surfaces.

Input: Refinement instructions/scope; saved batch rollback; model/effort settings

Result: Applied/rejected scoped prompt/skill/subagent edits plus journal before/after rationale/expected outcome

Lifecycle: Automatic or manual refinement uses bounded trajectory; subsequent context uses entries; rollback recorded

Authority: Immutable base prompt; model-proposed DB edits, not executable replacement

Evidence: [e32](#evidence-e32), [e33](#evidence-e33), [e34](#evidence-e34).

### /duel / run_duel; model-authored saved .forge/workflows scripts

Surface: operator/program experiment surfaces.

Input: Task and candidate routes; saved script

Result: Isolated worktree candidates/diff/test/time/cost, operator-selected winner

Lifecycle: Concurrent maximum three candidates; tests own worktree; outcome affects future routing

Authority: Operator merge/winner choice; no automatic claim of rigorous generalizable performance

Evidence: [e37](#evidence-e37).

### scripts/rebuild.sh

Surface: developer script.

Input: Cargo build args

Result: Compiled binary and killed MCP wrappers ready to respawn

Lifecycle: Stops wrappers before/after compile; restart on next use

Authority: Developer shell, no quiescence verification/context outpost/kernel identity transfer

Evidence: [e35](#evidence-e35).

## Capabilities

### filesystem

**I — Files** (source): Structured workspace tools and edits; notebook cell editing distinct from persistent kernel execution.

Evidence: [e5](#evidence-e5), [e6](#evidence-e6), [e7](#evidence-e7), [e8](#evidence-e8), [e9](#evidence-e9).

### processes

**I — OS programs** (source): Bounded shell/PTY or durable background jobs with workspace PID/log/status/stop; PTY input closed.

Evidence: [e10](#evidence-e10), [e11](#evidence-e11).

### code-actions

**I — Code actions** (source): Model authors fresh JS workflow control flow over registered async host functions.

Evidence: [e12](#evidence-e12), [e14](#evidence-e14), [e15](#evidence-e15).

### persistent-kernel

**L — Kernel** (source): Workflow creates fresh QuickJS AsyncRuntime each run; notebook action edits JSON only; no session-persistent code execution namespace traced.

Evidence: [e9](#evidence-e9), [e15](#evidence-e15).

### standing-database

**L — Standing DB** (source): Real internal scoped SQLite facts/harness/session state; memory capped/deduplicated, no arbitrary external shared research database action.

Evidence: [e23](#evidence-e23), [e24](#evidence-e24), [e29](#evidence-e29), [e33](#evidence-e33).

### workflow-programming

**I — Workflows** (source): Native model script agent/parallel/pipeline/phase/log/workflow with shared semaphores and host/runtime limits; saved workflow composition.

Evidence: [e12](#evidence-e12), [e13](#evidence-e13), [e14](#evidence-e14), [e15](#evidence-e15).

### multi-model

**I — Models** (source): Mesh child routes or explicit model pins, failover and same-task isolated candidate comparisons.

Evidence: [e4](#evidence-e4), [e16](#evidence-e16), [e37](#evidence-e37).

### live-collaboration

**I — Peer chat** (source): Daemon fleet message_session durably queues peer text with steer/followup modes, plus persistent child follow-up. Scope is live daemon fleet; bare CLI lacks this host.

Evidence: [e17](#evidence-e17), [e18](#evidence-e18), [e20](#evidence-e20), [e21](#evidence-e21).

### concurrent-work

**I — Concurrency** (source): Read-only batch join with original-order pairing; child tasks/workflow concurrency and duel worktrees; serial hooks/mutations constrain overlaps.

Evidence: [e5](#evidence-e5), [e13](#evidence-e13), [e37](#evidence-e37), [e38](#evidence-e38).

### steering-interrupt

**L — Steer/interrupt** (source): Legal-boundary inbox/peer priority and detached cancellation; deadline adds reconciliation model turn rather than immediately quiescing all owned computation.

Evidence: [e3](#evidence-e3), [e19](#evidence-e19), [e20](#evidence-e20), [e22](#evidence-e22).

### turn-redefinition

**L — Turn program** (source): Model-defined workflow programs and editable supplemental harness entries are powerful but compiled outer request/verification/refinement loop remains fixed.

Evidence: [e2](#evidence-e2), [e3](#evidence-e3), [e14](#evidence-e14), [e32](#evidence-e32), [e33](#evidence-e33).

### compaction

**I — Compaction** (source): Round-aligned summaries/pre-post lifecycle, output age pruning; failed summarizer preserves state and operator uncompact restores retained original history.

Evidence: [e3](#evidence-e3), [e25](#evidence-e25), [e26](#evidence-e26).

### context-repair

**L — Repair** (source): Explicit uncompact/rewind restore stored history and snapshots across stable DB seq; exposed operator/program controls, not ubiquitous model-directed arbitrary lineage repair.

Evidence: [e26](#evidence-e26), [e27](#evidence-e27).

### original-audit

**L — Original audit** (source): Tool audit args/results capped 64KiB; live ring 2000 and ~90-day pruning; CLI bridge provides summaries without raw output. Rich provenance not complete original everything.

Evidence: [e29](#evidence-e29), [e30](#evidence-e30), [e31](#evidence-e31).

### audit-query

**S — Audit query** (source): Store/read full replay and tool audit provide query primitives; replay filters visibility and reconstructs success, not original event analytical view.

Evidence: [e27](#evidence-e27), [e28](#evidence-e28), [e30](#evidence-e30).

### hot-change

**L — Hot change** (source): Named definitions loaded for workflow/followup, supplemental entries and model/effort changes; current core stays compiled and kernel fresh per workflow.

Evidence: [e14](#evidence-e14), [e18](#evidence-e18), [e32](#evidence-e32), [e33](#evidence-e33), [e34](#evidence-e34).

### rebuild-continuity

**L — Rebuild continuity** (source): Concrete rebuild script kills MCP wrappers then compiles and kills remnants, expects respawn; no quiescent refit/context-outpost continuity.

Evidence: [e35](#evidence-e35).

### remote-services

**S — Remote** (source): Provider/MCP/tool traits and daemon HTTP peers support consumer composition; actual fleet host owns local sessions rather than external meta-project fabric.

Evidence: [e4](#evidence-e4), [e20](#evidence-e20), [e21](#evidence-e21).

### self-improvement

**L — Self-improve** (source): Continual Harness validates/applies/journals/rolls back model-proposed scoped prompt/skill/subagent entries; base prompt immutable. Duel measures task candidates/routing, not executable self-refit experiments.

Evidence: [e32](#evidence-e32), [e33](#evidence-e33), [e37](#evidence-e37).

### complaints

**? — Complaints** (source-inspection): Not established in the inspected entry, model loop, action dispatch and state paths; no universal absence claim.

### authority

**L — Authority** (source): Bypass/AcceptEdits reduce ceremony, but built-in deny floor always overrides and Plan is hard read-only; virtual messages treated already allowed.

Evidence: [e20](#evidence-e20), [e36](#evidence-e36).

### evaluation

**I — Evaluation** (source): Duel isolated same-task comparisons record actual test status/diff/time/cost and operator choice; source workflow/phantom-claim tests have bounded oracles.

Evidence: [e37](#evidence-e37), [e38](#evidence-e38), [e39](#evidence-e39), [e40](#evidence-e40).

### time-order

**I — Time/order** (source): Durable DB seq/peer queue before delivery, shared task permits and monotonic synchronous-slice budgets; no complete distributed global event audit clock.

Evidence: [e13](#evidence-e13), [e15](#evidence-e15), [e21](#evidence-e21), [e27](#evidence-e27), [e39](#evidence-e39).

## Inspected test oracles

- [quarantine/forge-adulari/crates/forge-workflow/src/lib.rs](../../../quarantine/forge-adulari/crates/forge-workflow/src/lib.rs): Real async host overlap and synchronous compute bound Oracle: Promise.all two 50ms mocked host calls expected result plus elapsed <90ms distinguishes serial 100ms; susceptible timing-load false failures. Infinite loop gets interrupt within 10s; slow 500ms await does not trip 200ms pure-compute limit. Read, **not executed**.
- [quarantine/forge-adulari/crates/forge-core/src/tests/phantom_edit.rs](../../../quarantine/forge-adulari/crates/forge-core/src/tests/phantom_edit.rs): Unsupported prose completion claim Oracle: Provider repeatedly claims edits with no tool calls; asserts more than one request and explicit warning no mutation/not applied. This proves bounded guard behavior, not arbitrary true success inference. Read, **not executed**.

## Useful mechanisms

- Model-authored control flow with explicit host domain and shared concurrency budgets.
- Durable daemon colleague message admission before delivery.
- Uncompact and stable DB-sequence rewind survive model-visible summary transformation.
- Refinement distinguishes scoped supplemental harness edits from immutable base/core.
- Isolated same-task worktree comparison and explicit operator selection.

## Material limits

- Full browser/device/proxy internals, all provider bridges, assay/evaluation and OS process isolation not exhaustive. Reliability slogan is bounded by specific heuristics/oracles; no complete original audit or executable refit continuity. Configured program changes do not replace compiled outer loop.

## Arconaut design questions

- Which script host primitives should instead target externally owned shared kernels/process services?
- Can editable turn definitions live outside immutable outer verification/runtime while remaining recoverable?
- How should an unpruned original event log preserve capped/rewritten provider and program data?
- Would repair actions be model-native and lineage-preserving rather than operator full uncompact?
- What refit outpost/quiescence protocol replaces kill-and-respawn rebuild?
- How should candidate experiments avoid benchmark-specific routing overfit and record independent oracles?

## Evidence

### Evidence e1

[quarantine/forge-adulari/crates/forge-cli/src/main.rs:152–179](../../../quarantine/forge-adulari/crates/forge-cli/src/main.rs#L152): Rust Tokio CLI dispatch entry.

### Evidence e2

[quarantine/forge-adulari/crates/forge-core/src/lib.rs:2613–2659](../../../quarantine/forge-adulari/crates/forge-core/src/lib.rs#L2613): Turn admits workspace, drains finished detached children, prunes older output and builds route context.

### Evidence e3

[quarantine/forge-adulari/crates/forge-core/src/model_loop.rs:224–311](../../../quarantine/forge-adulari/crates/forge-core/src/model_loop.rs#L224): Step-wise age/prune/compact then provider request and response recording; deadline adds one reconciliation model turn.

### Evidence e4

[quarantine/forge-adulari/crates/forge-core/src/model_request.rs:194–269](../../../quarantine/forge-adulari/crates/forge-core/src/model_request.rs#L194): Actual complete_with request includes advertised specs/options/checkpoint; stream idle timeout and failed-attempt mesh outcome.

### Evidence e5

[quarantine/forge-adulari/crates/forge-core/src/tool_dispatch.rs:37–149](../../../quarantine/forge-adulari/crates/forge-core/src/tool_dispatch.rs#L37): Read-only batch concurrent join then paired original-order persistence; hooks require serial path.

### Evidence e6

[quarantine/forge-adulari/crates/forge-tools/src/lib.rs:69–102](../../../quarantine/forge-adulari/crates/forge-tools/src/lib.rs#L69): Tool API exposes effect/schema/full result and side-effect-free diff preview.

### Evidence e7

[quarantine/forge-adulari/crates/forge-tools/src/lib.rs:168–199](../../../quarantine/forge-adulari/crates/forge-tools/src/lib.rs#L168): Exact native tool registration.

### Evidence e8

[quarantine/forge-adulari/crates/forge-tools/src/core_tools.rs:188–260](../../../quarantine/forge-adulari/crates/forge-tools/src/core_tools.rs#L188): Native read_file schema and actual path/range reading entry.

### Evidence e9

[quarantine/forge-adulari/crates/forge-tools/src/core_tools/notebook.rs:1–89](../../../quarantine/forge-adulari/crates/forge-tools/src/core_tools/notebook.rs#L1): notebook_edit manipulates cells and clears stale execution outputs; not notebook execution.

### Evidence e10

[quarantine/forge-adulari/crates/forge-tools/src/shell.rs:118–177](../../../quarantine/forge-adulari/crates/forge-tools/src/shell.rs#L118): shell command/PTY/background/poll tool contract; PTY stdin closed.

### Evidence e11

[quarantine/forge-adulari/crates/forge-tools/src/shell/background.rs:505–581](../../../quarantine/forge-adulari/crates/forge-tools/src/shell/background.rs#L505): shell_job actual list/status/log/stop dispatcher with workspace-scoped stored job descriptor.

### Evidence e12

[quarantine/forge-adulari/crates/forge-core/src/workflow.rs:1–84](../../../quarantine/forge-adulari/crates/forge-core/src/workflow.rs#L1): run_workflow native JS agent/parallel/pipeline/phase/log/workflow host interface.

### Evidence e13

[quarantine/forge-adulari/crates/forge-core/src/workflow.rs:263–377](../../../quarantine/forge-adulari/crates/forge-core/src/workflow.rs#L263): Agent host function routes child and shares global/per-provider permits, emits lifecycle/progress and final outcome.

### Evidence e14

[quarantine/forge-adulari/crates/forge-core/src/orchestration.rs:321–408](../../../quarantine/forge-adulari/crates/forge-core/src/orchestration.rs#L321): Model run_workflow dispatch builds session context and invokes workflow runtime with configured quotas.

### Evidence e15

[quarantine/forge-adulari/crates/forge-workflow/src/lib.rs:1–123](../../../quarantine/forge-adulari/crates/forge-workflow/src/lib.rs#L1): Fresh QuickJS AsyncRuntime per script, explicit host capabilities, heap and continuous synchronous compute limits.

### Evidence e16

[quarantine/forge-adulari/crates/forge-core/src/subagent.rs:102–174](../../../quarantine/forge-adulari/crates/forge-core/src/subagent.rs#L102): Child routing pin inheritance and explicit override; independent model route.

### Evidence e17

[quarantine/forge-adulari/crates/forge-core/src/subagent.rs:209–242](../../../quarantine/forge-adulari/crates/forge-core/src/subagent.rs#L209): Persistent child follow-up reloads stored history and appends new user message.

### Evidence e18

[quarantine/forge-adulari/crates/forge-core/src/orchestration.rs:180–244](../../../quarantine/forge-adulari/crates/forge-core/src/orchestration.rs#L180): send_to_agent resolves previously spawned named child and re-resolves persona definition.

### Evidence e19

[quarantine/forge-adulari/crates/forge-core/src/detached_subagents.rs:14–80](../../../quarantine/forge-adulari/crates/forge-core/src/detached_subagents.rs#L14): list/cancel detached child actual stateful dispatcher.

### Evidence e20

[quarantine/forge-adulari/crates/forge-core/src/fleet.rs:1–96](../../../quarantine/forge-adulari/crates/forge-core/src/fleet.rs#L1): Daemon-only peer capability and follow_up versus steer queue priority; ordinary CLI lacks native fleet host.

### Evidence e21

[quarantine/forge-adulari/crates/forge-cli/src/cli/commands/run/driver/daemon_fleet.rs:16–68](../../../quarantine/forge-adulari/crates/forge-cli/src/cli/commands/run/driver/daemon_fleet.rs#L16): Actual fleet send persists source/recipient/mode before host delivery attempt.

### Evidence e22

[quarantine/forge-adulari/crates/forge-core/src/steer.rs:1–54](../../../quarantine/forge-adulari/crates/forge-core/src/steer.rs#L1): Mid-turn inbox drained only at legal boundaries; surface retains unconsumed prompts.

### Evidence e23

[quarantine/forge-adulari/crates/forge-store/src/memory.rs:1–68](../../../quarantine/forge-adulari/crates/forge-store/src/memory.rs#L1): SQLite durable facts scoped project/global; duplicate salience and bounded scope count.

### Evidence e24

[quarantine/forge-adulari/crates/forge-core/src/session_virtual_tools.rs:234–279](../../../quarantine/forge-adulari/crates/forge-core/src/session_virtual_tools.rs#L234): Model remember actually parses kind/text and stores scoped memory, optional embedding.

### Evidence e25

[quarantine/forge-adulari/crates/forge-core/src/compaction_policy.rs:465–522](../../../quarantine/forge-adulari/crates/forge-core/src/compaction_policy.rs#L465): Compaction hooks, tool-round-aligned preserved tail and routed summarization input.

### Evidence e26

[quarantine/forge-adulari/crates/forge-core/src/compaction_policy.rs:701–734](../../../quarantine/forge-adulari/crates/forge-core/src/compaction_policy.rs#L701): Failed compaction leaves transcript; uncompact reactivates stored originals/reloads full context.

### Evidence e27

[quarantine/forge-adulari/crates/forge-core/src/session_history.rs:7–69](../../../quarantine/forge-adulari/crates/forge-core/src/session_history.rs#L7): Rewind DB sequence repairs compacted mapping then restores snapshots and soft-deletes subsequent messages.

### Evidence e28

[quarantine/forge-adulari/crates/forge-core/src/replay.rs:1–61](../../../quarantine/forge-adulari/crates/forge-core/src/replay.rs#L1): Replay is visibility-filtered saved transcript; tool success reconstructed from error-prefix convention.

### Evidence e29

[quarantine/forge-adulari/crates/forge-store/src/lib.rs:90–143](../../../quarantine/forge-adulari/crates/forge-store/src/lib.rs#L90): 64KiB tool args/result cap, 90-day session pruning and 2000-event live ring.

### Evidence e30

[quarantine/forge-adulari/crates/forge-store/src/session_usage_store.rs:523–562](../../../quarantine/forge-adulari/crates/forge-store/src/session_usage_store.rs#L523): Actual audit insertion caps args/results before transaction and sync revision.

### Evidence e31

[quarantine/forge-adulari/crates/forge-core/src/model_stream.rs:31–85](../../../quarantine/forge-adulari/crates/forge-core/src/model_stream.rs#L31): CLI bridge tool completion has summary only, no raw output to retain/surface.

### Evidence e32

[quarantine/forge-adulari/crates/forge-core/src/refinement.rs:31–80](../../../quarantine/forge-adulari/crates/forge-core/src/refinement.rs#L31): Continual Harness can add prompt/skill/subagent entries; immutable base prompt and separate memory ownership.

### Evidence e33

[quarantine/forge-adulari/crates/forge-core/src/refinement.rs:194–249](../../../quarantine/forge-adulari/crates/forge-core/src/refinement.rs#L194): Refinement validates edits then scoped DB apply journals expected outcome and supports rollback.

### Evidence e34

[quarantine/forge-adulari/crates/forge-core/src/session_controls.rs:169–214](../../../quarantine/forge-adulari/crates/forge-core/src/session_controls.rs#L169): Program API model pin/effort/deadline and related state mutations.

### Evidence e35

[quarantine/forge-adulari/scripts/rebuild.sh:1–23](../../../quarantine/forge-adulari/scripts/rebuild.sh#L1): Developer rebuild kills MCP processes before/after compile, relies on fresh respawn; no context handoff/outpost.

### Evidence e36

[quarantine/forge-adulari/crates/forge-core/src/permission.rs:1–85](../../../quarantine/forge-adulari/crates/forge-core/src/permission.rs#L1): Mode and rule precedence includes built-in deny floor even in Bypass; Plan read-only.

### Evidence e37

[quarantine/forge-adulari/crates/forge-core/src/duel.rs:1–104](../../../quarantine/forge-adulari/crates/forge-core/src/duel.rs#L1): Duel native experiment runs same task in isolated worktrees, records test/diff/time/cost and operator winner; model routing feedback.

### Evidence e38

[quarantine/forge-adulari/crates/forge-workflow/src/lib.rs:384–411](../../../quarantine/forge-adulari/crates/forge-workflow/src/lib.rs#L384): Parallel host-call test measures elapsed to distinguish concurrent versus serialized calls.

### Evidence e39

[quarantine/forge-adulari/crates/forge-workflow/src/lib.rs:487–531](../../../quarantine/forge-adulari/crates/forge-workflow/src/lib.rs#L487): Infinite-loop interrupt and slow-host-await non-interruption source oracles.

### Evidence e40

[quarantine/forge-adulari/crates/forge-core/src/tests/phantom_edit.rs:39–101](../../../quarantine/forge-adulari/crates/forge-core/src/tests/phantom_edit.rs#L39): Scripted prose-only mutation claim triggers repeated request and warning, not objective arbitrary completion proof.

### Evidence e41

[quarantine/forge-adulari/crates/forge-core/src/heartbeat.rs:1–80](../../../quarantine/forge-adulari/crates/forge-core/src/heartbeat.rs#L1): Recurring same-session queued prompts separate model-owned and operator-owned heartbeat slots.

