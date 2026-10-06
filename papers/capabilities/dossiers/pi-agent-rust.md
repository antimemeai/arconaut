# pi-agent-rust

Extensive systems-oriented Pi reconstruction with effect-aware concurrent dispatch, model code cells, managed memory/skills and child-process hub collaboration.

Role: coding agent. Runtime: Rust, Python, JavaScript.

Pinned source: [https://github.com/Dicklesworthstone/pi_agent_rust](https://github.com/Dicklesworthstone/pi_agent_rust); revision/version `38d4f8835a069480f54b92ec72423e746a41c22d`.

Rust/asupersync core; OS child agents, jobs and Python kernel; QuickJS dedicated kernel thread and isolated extension actors.

Owns session kernels, services, child processes and project memory bank; consumes provider APIs and tool/service interfaces.

Inspection: Entry and approval posture; provider/turn/dispatch segments; actual eval kernels/bridge; hub and peer dispatcher; jobs ownership; memory transactions; extension rewriting/replacement, compaction, persistence and selected fault tests. Very large modules inspected in named segments, not line-for-line in entirety.

Limits of this study: Source read only, reference not executed. No executable rebuild/outpost continuity traced. Individual media/browser/GitHub/debug/LSP implementations, all extension hostcalls, full swarm/evaluation corpus and every storage backend not exhaustively inspected. JS/Python module header comments are stale relative to actual implemented JS path; dispatch and tests determine claims.

## Actions

### read / write / edit / hashline_edit / ast_edit / grep / ast_grep / find / ls

Surface: model tools.

Input: Path/text/range or structured edit/search parameters

Result: Content, matches, changes or errors

Lifecycle: Awaited; mutation recorder and effect classification participate in dispatch

Authority: Enabled selection; plan-state and approval/extension policy

Evidence: [e2](#evidence-e2), [e5](#evidence-e5), [e6](#evidence-e6).

### bash / jobs

Surface: model tools.

Input: command, optional background; jobs list/wait/cancel and jobId

Result: Output plus session-owned job handle, rolling artifact and completion notice

Lifecycle: Foreground awaited or monitored background; timeout/tree teardown; foreign session id rejected

Authority: Model under plan/approval; ownership resolves at call time

Evidence: [e2](#evidence-e2), [e19](#evidence-e19).

### eval

Surface: model tool.

Input: code, kernel=python|js, timeout_secs

Result: stdout/stderr/value/error and cell/restarted metadata

Lifecycle: Persistent session Python process or JS realm; Python timeout destroys state, JS interruption can preserve it

Authority: Model approval; bridge read/grep/find/ls only, code itself has host language authority

Evidence: [e7](#evidence-e7), [e8](#evidence-e8), [e9](#evidence-e9), [e10](#evidence-e10), [e11](#evidence-e11).

### hub start / ps / logs / send / stop / restart / describe

Surface: model tool.

Input: Launch spec/readiness gates; name; cursor/tail/grep/wait; PTY input/key/signal

Result: Service descriptor, bounded incremental log, readiness error

Lifecycle: Session service process or explicit detached process; TERM/KILL lifecycle

Authority: Model tool gate; owned services rather than shared-fabric consumer abstraction

Evidence: [e17](#evidence-e17), [e18](#evidence-e18).

### subagent

Surface: model tool.

Input: single task / tasks + concurrency / chain; named definition, output schema and isolation

Result: Child final text or validated data, worktree path/diff/patch

Lifecycle: Bounded child-process execution; chain substitution; request-wide deadline

Authority: Named child configurations may pin other models; depth/host limits

Evidence: [e14](#evidence-e14).

### hub agent roster / transcript / steer / send / inbox / kill / revive

Surface: model tool.

Input: op=agent, action, child name, text, optional from label

Result: Roster/redacted transcript or accepted bus sequence; lifecycle descriptor

Lifecycle: Cross-process disk steering delivered between turns; recipient must remain live; revive is new child

Authority: Parent model controls registry; sender label supplied, not authenticated colleague identity

Evidence: [e15](#evidence-e15), [e16](#evidence-e16), [e18](#evidence-e18).

### retain / recall / reflect / memory_edit / learn / manage_skill

Surface: model tools.

Input: Fact/lesson/query/session key; memory id mutation; managed skill CRUD

Result: Durable fact/search/citation values or managed skill result

Lifecycle: SQLite transaction for bank; managed skill files separate; promotion can fail without losing lesson

Authority: Bank configuration and live session scope; managed marker protects user-authored skills

Evidence: [e12](#evidence-e12), [e13](#evidence-e13), [e27](#evidence-e27), [e28](#evidence-e28).

### current_time / browser / computer / debug / github / lsp / web_search / read_media / inspect_image / generate_image / tts / security_scan / ask / todo / submit_plan / xdev

Surface: model tool catalog.

Input: Individual JSON schemas; xdev tier discovers dispatchable operations

Result: Tool-specific content and result details

Lifecycle: Catalog registration traced; individual internals not all inspected

Authority: Selectable or host-coupled according to registry; extension/policy gates

Evidence: [e2](#evidence-e2), [e6](#evidence-e6).

### compact_now / shake_now / execute_extension_command

Surface: programmable API; operator /compact.

Input: Compaction notes/mode or registered command args

Result: Compaction event/result or command value

Lifecycle: Before-compaction interception; summary apply admission; background result later turn; shake drops old outputs without model call

Authority: Operator/program; extension override constrained by contract

Evidence: [e20](#evidence-e20), [e21](#evidence-e21), [e22](#evidence-e22).

### context / before_provider_request / tool-call / tool-result / turn lifecycle extension hooks

Surface: extension program API.

Input: Messages/request/tool events and responses

Result: Rewritten model input/results, block/cancel or lifecycle events

Lifecycle: Per-request/per-tool hooks with failure policies; actual core remains host loop

Authority: Authorized extension programs; not unrestricted model-defined scheduler replacement

Evidence: [e4](#evidence-e4), [e6](#evidence-e6), [e23](#evidence-e23), [e26](#evidence-e26).

### /reload / /omfg

Surface: operator commands.

Input: Idle reload; grievance text

Result: Reloaded resources diagnostics or grievance/rule id

Lifecycle: Reload refused while processing; /omfg activates stream abort/retry rule

Authority: Operator surface; complaint record does not capture full agent state

Evidence: [e25](#evidence-e25), [e29](#evidence-e29), [e30](#evidence-e30).

### SessionControlHandle.transfer_pending_to

Surface: programmable API.

Input: Finished source control handle and accepting destination

Result: New input receipts preserving lane and attachments

Lifecycle: All-or-nothing mailbox move, no replay of already delivered work

Authority: Embedding host controls turns; provider or executable continuity not acknowledged

Evidence: [e34](#evidence-e34).

## Capabilities

### filesystem

**I — Files** (source): Native selected read/search/write/edit tools with effect classification and optional undo recorder.

Evidence: [e2](#evidence-e2), [e5](#evidence-e5), [e6](#evidence-e6).

### processes

**I — OS programs** (source): Background bash jobs and PTY hub services expose identities, readiness/log/input/cancel lifecycle; session-owned by default with detached option.

Evidence: [e17](#evidence-e17), [e18](#evidence-e18), [e19](#evidence-e19).

### code-actions

**I — Code actions** (source): eval runs Python or JS code with captured cell outcomes; bridge is deliberately limited to four reading tools.

Evidence: [e8](#evidence-e8), [e9](#evidence-e9), [e10](#evidence-e10).

### persistent-kernel

**I — Kernel** (source): Session-owned Python process namespace and dedicated persistent QuickJS thread; different timeout state-loss semantics, neither arbitrary recompile continuity.

Evidence: [e7](#evidence-e7), [e9](#evidence-e9), [e11](#evidence-e11), [e31](#evidence-e31).

### standing-database

**L — Standing DB** (source): Project SQLite fact/lesson bank and session-shared exact values are real standing data, with atomic mutation/index/audit; this is not arbitrary external operator database access.

Evidence: [e12](#evidence-e12), [e13](#evidence-e13).

### workflow-programming

**L — Workflows** (source): Native subagent chain/parallel/schema composition plus code cells and extension hooks; no fully model-defined turn scheduler traced.

Evidence: [e8](#evidence-e8), [e14](#evidence-e14), [e23](#evidence-e23).

### multi-model

**I — Models** (source): Named child definitions/role model selection permit different worker models; provider and extension request interfaces separate.

Evidence: [e14](#evidence-e14).

### live-collaboration

**L — Peer chat** (source): Model can inspect/steer registered child processes through hub disk mailboxes with acceptance sequences; delivered between turns, bounded inbox and redacted transcript; not an unrestricted shared IRC room.

Evidence: [e15](#evidence-e15), [e16](#evidence-e16).

### concurrent-work

**I — Concurrency** (source): Compatible tool effect batches run concurrently then record in call order; background jobs/services and bounded child parallel tasks provide additional overlap.

Evidence: [e5](#evidence-e5), [e14](#evidence-e14), [e19](#evidence-e19).

### steering-interrupt

**I — Steer/interrupt** (source): Abort races provider stream and tool batches; steering generates explicit skipped results and lossless turn handoff. Individual effect/lifecycle boundaries still matter.

Evidence: [e3](#evidence-e3), [e4](#evidence-e4), [e5](#evidence-e5), [e34](#evidence-e34).

### turn-redefinition

**L — Turn program** (source): Extensions rewrite provider context and tool outcomes, intercept compaction and lifecycle; controlled host orchestration is not wholesale hot replacement of turn definition.

Evidence: [e4](#evidence-e4), [e6](#evidence-e6), [e21](#evidence-e21), [e23](#evidence-e23).

### compaction

**I — Compaction** (source): Model summary or non-provider shake, cancellable/replacement extension hook and quota-bounded background preparation applied on later turns.

Evidence: [e20](#evidence-e20), [e21](#evidence-e21), [e22](#evidence-e22).

### context-repair

**S — Repair** (source): Persisted branch graph and extension replacement of outbound context support repair composition; no traced universal model-operated restoration protocol for all lost state.

Evidence: [e23](#evidence-e23), [e24](#evidence-e24), [e34](#evidence-e34).

### original-audit

**L — Original audit** (source): Session graph retains messages and explicit model/thinking/compaction/custom changes, but provider input is rewritten/redacted and persisted entry types do not establish every original stream/program event.

Evidence: [e4](#evidence-e4), [e24](#evidence-e24).

### audit-query

**S — Audit query** (source): Model hub transcript reads and SQLite memory recall/audit provide bounded query primitives; complete original-audit analytical surface not traced.

Evidence: [e12](#evidence-e12), [e16](#evidence-e16), [e24](#evidence-e24).

### hot-change

**L — Hot change** (source): Idle-only resource reload and transactional cold extension shard swap are real; candidate replaces realm after stream cleanup so old realm state does not persist by default.

Evidence: [e25](#evidence-e25), [e26](#evidence-e26), [e33](#evidence-e33).

### rebuild-continuity

**? — Rebuild continuity** (source-inspection): Not established in the inspected entry, model loop, action dispatch and state paths; no universal absence claim.

### remote-services

**S — Remote** (source): Provider and bridge/extension interfaces consume outside services; hub principally owns local PTY services rather than external shared computation fabric.

Evidence: [e4](#evidence-e4), [e17](#evidence-e17), [e18](#evidence-e18).

### self-improvement

**L — Self-improve** (source): Model can retain lessons and create/update/delete linted managed skills, with user/project precedence; no governed measured executable autoresearch/refit loop traced.

Evidence: [e27](#evidence-e27), [e28](#evidence-e28).

### complaints

**L — Complaints** (source): Operator /omfg stores grievance text and creates active stream rule; no model-everywhere complaint command plus captured agent state/database bead contract.

Evidence: [e29](#evidence-e29), [e30](#evidence-e30).

### authority

**I — Authority** (source): Always-ask default and explicit yolo mode; plan mutation barrier then approval then tool hooks; secret restoration occurs before approval.

Evidence: [e1](#evidence-e1), [e6](#evidence-e6).

### evaluation

**S — Evaluation** (source): Concrete kernel, torn-mailbox and resource-trust source test oracles support experiments; they were read, not run, and do not establish full harness quality.

Evidence: [e31](#evidence-e31), [e32](#evidence-e32), [e33](#evidence-e33).

### time-order

**I — Time/order** (source): Accepted bus sequence advances only after disk append, turn elapsed Instant budgets and timestamped parent-linked session entries; no global distributed clock guarantee.

Evidence: [e3](#evidence-e3), [e15](#evidence-e15), [e24](#evidence-e24).

## Inspected test oracles

- [quarantine/pi-agent-rust/src/eval.rs](../../../quarantine/pi-agent-rust/src/eval.rs): Kernel persistence, bridge authority and interrupted cell state Oracle: Seeds globals; subsequent cell exact numeric result; bridge bash denied; infinite-loop JS cell errors and subsequent keep=5 survives. Real embedded runtime oracle in source, not executed here. Read, **not executed**.
- [quarantine/pi-agent-rust/src/agent_hub.rs](../../../quarantine/pi-agent-rust/src/agent_hub.rs): Torn mailbox rejection and retry atomicity Oracle: Deliberately removes frame newline; append fails without advancing seq/inbox or modifying bytes; repair then retry gets seq=2 and exactly ordered messages. Read, **not executed**.
- [quarantine/pi-agent-rust/src/interactive/commands.rs](../../../quarantine/pi-agent-rust/src/interactive/commands.rs): Hot resource reload trust boundary Oracle: Explicit CLI skill remains under untrusted reload, project skill absent; trusted manager then includes both. Resource loading oracle, not compiled executable replacement. Read, **not executed**.

## Useful mechanisms

- Separate persistent code domains behind Rust scheduler.
- Concrete effect-batch concurrency with paired cancellation results.
- Readiness and incremental cursor/input service interface.
- Disk-first accepted steering sequence and strong torn-frame oracle.
- Model-authored skill and lesson updates are explicitly distinct from user resources.

## Material limits

- No executable rebuild/outpost continuity traced. Individual media/browser/GitHub/debug/LSP implementations, all extension hostcalls, full swarm/evaluation corpus and every storage backend not exhaustively inspected. JS/Python module header comments are stale relative to actual implemented JS path; dispatch and tests determine claims.

## Arconaut design questions

- Should kernel and hub service ownership instead sit outside Arconaut behind shared customer interfaces?
- Can one uniform lifecycle keep accepted steering receipts, process handles and provider requests coherent during pause/refit?
- Which extension rewrites need immutable before/after original audit lineage?
- Should complaint capture be decoupled from immediate rule activation and support model state attachment?
- What executable refit oracle would distinguish preserved context/mailboxes from lost Python/JS identities?

## Evidence

### Evidence e1

[quarantine/pi-agent-rust/src/main.rs:24–128](../../../quarantine/pi-agent-rust/src/main.rs#L24): Rust entry imports asupersync and defines cross-surface approval posture with explicit yolo override.

### Evidence e2

[quarantine/pi-agent-rust/src/tools.rs:5440–5609](../../../quarantine/pi-agent-rust/src/tools.rs#L5440): Selectable and separately registered tool catalogs and concrete registry construction.

### Evidence e3

[quarantine/pi-agent-rust/src/agent.rs:3180–3315](../../../quarantine/pi-agent-rust/src/agent.rs#L3180): Owning input surface validated before consuming follow-ups; turn and elapsed-budget handling.

### Evidence e4

[quarantine/pi-agent-rust/src/agent.rs:4062–4135](../../../quarantine/pi-agent-rust/src/agent.rs#L4062): Provider context is extension-rewritten, secrets-filtered and abort-raced before stream admission.

### Evidence e5

[quarantine/pi-agent-rust/src/agent.rs:4910–5167](../../../quarantine/pi-agent-rust/src/agent.rs#L4910): Compatible effect batches run concurrently; deterministic original ordering and explicit skipped/aborted tool result pairing.

### Evidence e6

[quarantine/pi-agent-rust/src/agent.rs:5169–5259](../../../quarantine/pi-agent-rust/src/agent.rs#L5169): Plan-state gate and approval run before tool hooks and dispatch; results may be rewritten then secrets-masked.

### Evidence e7

[quarantine/pi-agent-rust/src/eval.rs:32–175](../../../quarantine/pi-agent-rust/src/eval.rs#L32): Session-owned Python kernel subprocess, IO reader, process-group teardown and state slots.

### Evidence e8

[quarantine/pi-agent-rust/src/eval.rs:178–321](../../../quarantine/pi-agent-rust/src/eval.rs#L178): Persistent JS dispatch and bridge whitelist read/grep/find/ls.

### Evidence e9

[quarantine/pi-agent-rust/src/eval.rs:323–443](../../../quarantine/pi-agent-rust/src/eval.rs#L323): Python cell deadline kills kernel with explicit state-loss error; completed kernel returned to slot.

### Evidence e10

[quarantine/pi-agent-rust/src/eval.rs:530–586](../../../quarantine/pi-agent-rust/src/eval.rs#L530): eval source/kernel/timeout schema and actual Python/JS dispatch.

### Evidence e11

[quarantine/pi-agent-rust/src/eval/js_kernel.rs:1–82](../../../quarantine/pi-agent-rust/src/eval/js_kernel.rs#L1): Dedicated QuickJS thread with persistent globals, promise pump and interrupt deadline; separate from extensions.

### Evidence e12

[quarantine/pi-agent-rust/src/memory.rs:1–25](../../../quarantine/pi-agent-rust/src/memory.rs#L1): Project-scoped memory bank and exact session-shared values; SQLite row/index/audit storage boundary.

### Evidence e13

[quarantine/pi-agent-rust/src/memory/transactions.rs:30–81](../../../quarantine/pi-agent-rust/src/memory/transactions.rs#L30): BEGIN IMMEDIATE memory action; guarded rollback and bounded fresh-snapshot commit retries.

### Evidence e14

[quarantine/pi-agent-rust/src/subagents.rs:334–392](../../../quarantine/pi-agent-rust/src/subagents.rs#L334): subagent single/parallel/chain schemas, model definitions, output schemas and worktree isolation contract.

### Evidence e15

[quarantine/pi-agent-rust/src/agent_hub.rs:303–353](../../../quarantine/pi-agent-rust/src/agent_hub.rs#L303): Disk steering append precedes sequenced acceptance receipt; bounded parent inbox and cross-process steering.

### Evidence e16

[quarantine/pi-agent-rust/src/tools.rs:9054–9136](../../../quarantine/pi-agent-rust/src/tools.rs#L9054): Actual model hub agent roster/transcript/steer/send dispatch and redacted transcript result.

### Evidence e17

[quarantine/pi-agent-rust/src/hub.rs:1–79](../../../quarantine/pi-agent-rust/src/hub.rs#L1): PTY service launch specs, readiness gates, bounded log ring and detached lifecycle.

### Evidence e18

[quarantine/pi-agent-rust/src/tools.rs:8697–8798](../../../quarantine/pi-agent-rust/src/tools.rs#L8697): hub start/ps/logs/send/stop/restart/describe/jobs/agent native schema and dispatch entry.

### Evidence e19

[quarantine/pi-agent-rust/src/jobs.rs:1–95](../../../quarantine/pi-agent-rust/src/jobs.rs#L1): Background job handles and dynamically resolved owning session identity; process shutdown boundary.

### Evidence e20

[quarantine/pi-agent-rust/src/agent.rs:14110–14127](../../../quarantine/pi-agent-rust/src/agent.rs#L14110): Program API compact_now and shake_now; shake is non-provider output dropping, not a complaint.

### Evidence e21

[quarantine/pi-agent-rust/src/agent.rs:14755–14870](../../../quarantine/pi-agent-rust/src/agent.rs#L14755): Before-compaction extension can cancel or supply replacement summary; serialized application admission.

### Evidence e22

[quarantine/pi-agent-rust/src/compaction_worker.rs:1–41](../../../quarantine/pi-agent-rust/src/compaction_worker.rs#L1): Background compaction on asupersync, subsequent-turn application and explicit quotas.

### Evidence e23

[quarantine/pi-agent-rust/src/agent.rs:3905–3945](../../../quarantine/pi-agent-rust/src/agent.rs#L3905): Context extension can replace outbound message list; invalid payload ignored.

### Evidence e24

[quarantine/pi-agent-rust/src/session.rs:6523–6598](../../../quarantine/pi-agent-rust/src/session.rs#L6523): Persisted session message/change/compaction/branch/custom graph with IDs, parents and timestamps, not original provider-wire inventory.

### Evidence e25

[quarantine/pi-agent-rust/src/interactive/commands.rs:3920–4008](../../../quarantine/pi-agent-rust/src/interactive/commands.rs#L3920): Operator reload is idle-only and asynchronously reloads resources with model registry diagnostics.

### Evidence e26

[quarantine/pi-agent-rust/src/extensions.rs:11790–11880](../../../quarantine/pi-agent-rust/src/extensions.rs#L11790): Transactional candidate JS shard construction then active-provider cleanup and cold replacement, no arbitrary realm-state continuity.

### Evidence e27

[quarantine/pi-agent-rust/src/tools.rs:8373–8459](../../../quarantine/pi-agent-rust/src/tools.rs#L8373): learn persists lesson and optionally promotes to managed skill, with warning on failed promotion.

### Evidence e28

[quarantine/pi-agent-rust/src/tools.rs:8487–8570](../../../quarantine/pi-agent-rust/src/tools.rs#L8487): manage_skill model CRUD; agent-authored marker and user/project precedence boundary.

### Evidence e29

[quarantine/pi-agent-rust/src/interactive/workspace_reports.rs:89–126](../../../quarantine/pi-agent-rust/src/interactive/workspace_reports.rs#L89): Operator /omfg complaint logs text then generates/activates stream rule.

### Evidence e30

[quarantine/pi-agent-rust/src/stream_rules.rs:615–682](../../../quarantine/pi-agent-rust/src/stream_rules.rs#L615): Grievance record contains id/time/complaint/suggested rule/resolved, no agent-state capture.

### Evidence e31

[quarantine/pi-agent-rust/src/eval.rs:820–914](../../../quarantine/pi-agent-rust/src/eval.rs#L820): Read test oracles cover cross-cell state, promises, whitelist denial and interrupt state preservation.

### Evidence e32

[quarantine/pi-agent-rust/src/agent_hub.rs:1108–1142](../../../quarantine/pi-agent-rust/src/agent_hub.rs#L1108): Torn disk frame test checks rejection, sequence/inbox unchanged and successful repair/retry.

### Evidence e33

[quarantine/pi-agent-rust/src/interactive/commands.rs:4770–4855](../../../quarantine/pi-agent-rust/src/interactive/commands.rs#L4770): Reload trust test checks explicit skill survives while untrusted project resources stay excluded.

### Evidence e34

[quarantine/pi-agent-rust/src/session_control/recovery.rs:1–80](../../../quarantine/pi-agent-rust/src/session_control/recovery.rs#L1): All-or-nothing finished-turn mailbox transfer preserves lane/attachments with new identities; not executable refit.

