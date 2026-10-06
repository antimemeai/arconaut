# prime-agent

Code-first coding agent: the shipped model tool is ipython; Python composes host skills, shell handles, RLM child sessions and mutable harness state.

Role: coding agent. Runtime: Rust, Python.

Pinned source: [https://github.com/PrimeIntellect-ai/prime-agent](https://github.com/PrimeIntellect-ai/prime-agent); revision/version `e75f59efc6f74fcb23e45048f0f23b34490571d0`.

Rust provider/turn core and daemon supervisor/workers; per-session owned CPython stdio kernel on one asyncio loop; bash background process groups, RLM child sessions and independent provider requests.

Owns session workers, Python namespaces, child/batch process containment and JSON harness stores; consumes providers/MCP; kernels are session-owned rather than shared fabric consumers.

Inspection: Shipped tool selection/loop→kernel provision/execute/result→host bridge→family routing, compaction/refine settlement, snapshot limits/cleanup; scripted continuation and busy delivery test assertions.

Limits of this study: Source read only, reference not executed. No reference execution. Rust port retains misleading TypeScript comments in Python skills. Snapshot persistence omits unsafe/unpicklable/oversize namespace objects and does not preserve live process/socket identity. Refinement governs prompt/memory/skill/subagent entries, not executable rebuild and validated autoresearch.

## Actions

### ipython

Surface: model tool.

Input: Python code string

Result: stdout/stderr/repr/error/attachments/duration; host receipts in details

Lifecycle: Sequential cell in owned persistent Python namespace; abort can leave busy kernel needing wait/kill

Authority: Model code has ordinary filesystem/import/host authority, no tool approval gate traced here

Evidence: [e1](#evidence-e1), [e2](#evidence-e2), [e4](#evidence-e4).

### bash; BashHandle.pid/running/output/tail/poll/kill; await handle

Surface: programmable API.

Input: Shell string; signal/grace; tail lines

Result: Persistent process handle and BashResult

Lifecycle: Starts immediately; foreground cancellation kills group; released background work survives cell abort but not kernel teardown

Authority: Model via Python; owned host process group

Evidence: [e5](#evidence-e5), [e6](#evidence-e6), [e10](#evidence-e10).

### rlm.spawn; rlm.create_session; rlm.find_models; rlm.list_subagents; rlm.collect; rlm.delete_subagent

Surface: programmable API.

Input: Prompt/name/model/thinking/cwd; child selectors/timeout

Result: Child/session typed IDs; roster/results

Lifecycle: Independent child sessions retained until deletion; collect nonblocking or bounded await

Authority: Model Python host bridge; direct-family lifecycle; create_session daemon root only

Evidence: [e14](#evidence-e14), [e15](#evidence-e15).

### agent_message.send

Surface: programmable API.

Input: message, receiver_role parent/sibling/child and receiver_name; broadcast all

Result: Identity/delivery receipts

Lifecycle: Busy target queued with queuedAt; role/family route; peer then supervisor fallback

Authority: Model skill inside kernel; paused/capacity gates

Evidence: [e16](#evidence-e16), [e17](#evidence-e17), [e18](#evidence-e18).

### agent_observe.list_agents/get_agent/recent_messages

Surface: programmable API.

Input: Target; count/max_chars

Result: Family/session state and bounded message previews

Lifecycle: Read-only host requests

Authority: Model skill; bounded transcript scope

Evidence: [e25](#evidence-e25).

### HarnessState.get/list/search/create/update/delete; create_memory/create_prompt_note/create_skill/create_subagent; record_refinement

Surface: programmable API.

Input: Typed entries/id/content/scope; query; evidence/outcome

Result: Structured entries and durable JSON harness state

Lifecycle: mtime refresh then save; local/global definitions affect future prompt/resources

Authority: Model Python mutations; explicit global scope

Evidence: [e11](#evidence-e11), [e12](#evidence-e12), [e13](#evidence-e13).

### compact.status/run; refine.status/run; /compact; /refine

Surface: programmable API.

Input: Optional instructions; global refine flag

Result: Scheduled/reason/status and durable summary/refinement outcomes

Lifecycle: Queued until settled turn; compaction then refinement; requested compaction stops run and queued continuation resumes

Authority: Model skill and operator command; no mid-cell transform

Evidence: [e19](#evidence-e19), [e20](#evidence-e20), [e21](#evidence-e21), [e22](#evidence-e22), [e23](#evidence-e23).

### /reload

Surface: operator command.

Input: None

Result: Success; fresh per-use config

Lifecycle: Settings/auth/MCP next-use refresh; no executable recompile

Authority: Operator

Evidence: [e24](#evidence-e24).

## Capabilities

### filesystem

**I — Files** (source): Python ordinary file APIs and skill imports compose edits/search; not dedicated JSON tools in shipped surface.

Evidence: [e1](#evidence-e1), [e2](#evidence-e2), [e4](#evidence-e4).

### processes

**I — OS programs** (source): Immediate shell handles, bounded tail/poll/group kill, foreground versus released-background cancellation semantics.

Evidence: [e5](#evidence-e5), [e6](#evidence-e6), [e10](#evidence-e10).

### code-actions

**I — Code actions** (source): Python code cells compose bash/host skills/child calls and normal programs.

Evidence: [e2](#evidence-e2), [e4](#evidence-e4), [e14](#evidence-e14).

### persistent-kernel

**L — Kernel** (source): Session-owned CPython namespace persists; snapshots selectively dill variables with16MiB/256MiB limits; handles/threads/sockets cannot be assumed restored.

Evidence: [e4](#evidence-e4), [e7](#evidence-e7), [e8](#evidence-e8), [e9](#evidence-e9), [e10](#evidence-e10).

### standing-database

**I — Standing DB** (source): Queryable mutable local/global structured HarnessState JSON entries and refinements; not a general shared SQL service.

Evidence: [e11](#evidence-e11), [e12](#evidence-e12), [e13](#evidence-e13).

### workflow-programming

**S — Workflows** (source): Python code, named skills/subagent definitions and child APIs supply programmable composition; full turn scheduler remains Rust hooks.

Evidence: [e3](#evidence-e3), [e4](#evidence-e4), [e12](#evidence-e12), [e14](#evidence-e14).

### multi-model

**I — Models** (source): Model lookup and per-child model/thinking selection; simultaneous independent child sessions rather than only UI switch.

Evidence: [e14](#evidence-e14), [e16](#evidence-e16).

### live-collaboration

**I — Peer chat** (source): Model role-addressed/broadcast messages with receipts and busy-target queue; bounded ingestion and pause gate, no blind retry.

Evidence: [e16](#evidence-e16), [e17](#evidence-e17), [e18](#evidence-e18).

### concurrent-work

**I — Concurrency** (source): RLM child sessions and released background bash handles can outlive one cell; Python execution cells remain sequential.

Evidence: [e2](#evidence-e2), [e5](#evidence-e5), [e6](#evidence-e6), [e14](#evidence-e14), [e15](#evidence-e15).

### steering-interrupt

**I — Steer/interrupt** (source): Loop steering/follow-up polling and abort; Python interrupt recovery distinguishes busy previous cell; shell foreground versus background semantics.

Evidence: [e3](#evidence-e3), [e5](#evidence-e5), [e6](#evidence-e6).

### turn-redefinition

**S — Turn program** (source): Rust API offers steering/continuation/stop hooks; model changes skills/subagent/prompt entries, not arbitrary Rust scheduling body in flight.

Evidence: [e3](#evidence-e3), [e12](#evidence-e12), [e19](#evidence-e19).

### compaction

**I — Compaction** (source): Automatic/manual/model-scheduled summaries retain tail anchor/previous summary; durable notice explicitly prunes oversized kernel names.

Evidence: [e19](#evidence-e19), [e20](#evidence-e20), [e21](#evidence-e21).

### context-repair

**S — Repair** (source): Retained session entries and bounded family previews plus Python file access can compose retrieval; no exposed original-complete repair action traced.

Evidence: [e11](#evidence-e11), [e25](#evidence-e25), [e26](#evidence-e26).

### original-audit

**L — Original audit** (source): Session/refinement/compaction durable records preserve important transitions, but diagnostic rotation/output bounds and namespace loss preclude complete original IO claim.

Evidence: [e7](#evidence-e7), [e8](#evidence-e8), [e20](#evidence-e20), [e22](#evidence-e22), [e28](#evidence-e28).

### audit-query

**S — Audit query** (source): Python file reading and bounded family recent-message APIs support inspection; structured harness search queries memories, not complete original transport audit.

Evidence: [e2](#evidence-e2), [e11](#evidence-e11), [e25](#evidence-e25).

### hot-change

**I — Hot change** (source): Model mutates harness entries; host refit rebuilds system prompt after settled turn; settings/auth/MCP resolved per use, executable unchanged.

Evidence: [e11](#evidence-e11), [e19](#evidence-e19), [e22](#evidence-e22), [e23](#evidence-e23), [e24](#evidence-e24).

### rebuild-continuity

**L — Rebuild continuity** (source): Kernel/session snapshot restoration and daemon lifecycle are partial continuity primitives; no actual executable self-compile/quiescent outpost handoff traced.

Evidence: [e7](#evidence-e7), [e8](#evidence-e8), [e9](#evidence-e9), [e10](#evidence-e10), [e24](#evidence-e24).

### remote-services

**I — Remote** (source): Consumes providers and MCP through host/Python bridge; owns per-session kernels and child processes rather than shared kernel service consumer boundary.

Evidence: [e2](#evidence-e2), [e14](#evidence-e14), [e16](#evidence-e16), [e24](#evidence-e24).

### self-improvement

**L — Self-improve** (source): Dedicated plan/apply/rollback of harness entry definitions and automatic review gates exist; expected outcomes/history are not discriminating experiment/evaluation governance or executable rebuild.

Evidence: [e13](#evidence-e13), [e19](#evidence-e19), [e22](#evidence-e22), [e23](#evidence-e23).

### complaints

**? — Complaints** (source-inspection): Not established in the inspected entry, model loop, action dispatch and state paths; no universal absence claim.

### authority

**I — Authority** (source): Shipped ipython allows model authored ordinary Python and host skill/harness mutations; scope/relationship constraints around global stores and family delivery.

Evidence: [e2](#evidence-e2), [e11](#evidence-e11), [e17](#evidence-e17), [e23](#evidence-e23).

### evaluation

**I — Evaluation** (source): Scripted model continuation oracle checks actual second-request ID/result ordering; busy message oracle checks queued receipt. Mocks do not validate live provider/kernel/platform behavior.

Evidence: [e18](#evidence-e18), [e26](#evidence-e26).

### time-order

**I — Time/order** (source): Explicit scheduled heartbeat run times/state and awaited-handle lifecycle; monotonic timeout/cancellation boundary supports local ordering, not cross-system original causal ledger.

Evidence: [e5](#evidence-e5), [e15](#evidence-e15), [e19](#evidence-e19), [e27](#evidence-e27).

## Inspected test oracles

- [quarantine/prime-agent/crates/pa-agent/tests/agent_loop_scripted.rs](../../../quarantine/prime-agent/crates/pa-agent/tests/agent_loop_scripted.rs): Tool continuation and event order Oracle: Second scripted provider call must include matching tool-call ID/result and correct role order; scripted transport only. Read, **not executed**.
- [quarantine/prime-agent/crates/pa-daemon/src/worker/agent_message_tests.rs](../../../quarantine/prime-agent/crates/pa-daemon/src/worker/agent_message_tests.rs): Busy-target message retention Oracle: Worker busy flag forces queued receipt/queuedAt and queue content assertions; no live-model delivery exercised. Read, **not executed**.
- [quarantine/prime-agent/crates/pa-core/src/session_engine/ipython_state.rs](../../../quarantine/prime-agent/crates/pa-core/src/session_engine/ipython_state.rs): Post-compaction kernel-loss disclosure Oracle: Probe fixtures require prune notice/name list; a mocked namespace probe does not establish dill or OS serialization safety. Read, **not executed**.

## Useful mechanisms

- Code-first single tool gives model a compact compositional surface.
- Harness entry changes and compact/refine are scheduled at a meaningful settled boundary.
- Busy-target receipts and explicit foreground/background process ownership are unusually precise.

## Material limits

- No reference execution. Rust port retains misleading TypeScript comments in Python skills. Snapshot persistence omits unsafe/unpicklable/oversize namespace objects and does not preserve live process/socket identity. Refinement governs prompt/memory/skill/subagent entries, not executable rebuild and validated autoresearch.

## Arconaut design questions

- Which standing services should become external consumers instead of owned per-session kernels?
- Can refinement expected outcomes be replaced with explicit direct-property oracles and experiment lineage?
- Which transcript/kernel snapshot omissions must trigger model-readable continuity warnings and repair paths?

## Evidence

### Evidence e1

[quarantine/prime-agent/crates/pa-cli/src/prompt_command.rs:113–124](../../../quarantine/prime-agent/crates/pa-cli/src/prompt_command.rs#L113): Effective shipped tool selection is ipython, not independent shell/edit JSON tools.

### Evidence e2

[quarantine/prime-agent/crates/pa-core/src/tools/ipython.rs:489–520](../../../quarantine/prime-agent/crates/pa-core/src/tools/ipython.rs#L489): Single sequential code-string tool forwards to persistent kernel with structured outputs/attachments/errors.

### Evidence e3

[quarantine/prime-agent/crates/pa-agent/src/agent_loop/run.rs:30–263](../../../quarantine/prime-agent/crates/pa-agent/src/agent_loop/run.rs#L30): Continuation pairs tool results; steering/follow-up/continuation/stop hooks and abort checks govern boundaries.

### Evidence e4

[quarantine/prime-agent/prime-agent-runtime/src/rlm/repl.py:636–680](../../../quarantine/prime-agent/prime-agent-runtime/src/rlm/repl.py#L636): Persistent namespace compiles/executes Python cell with async support and error/interrupt events.

### Evidence e5

[quarantine/prime-agent/prime-agent-runtime/src/rlm/bash.py:376–429](../../../quarantine/prime-agent/prime-agent-runtime/src/rlm/bash.py#L376): BashHandle exposes pid/running/output/tail/poll/kill with process group escalation.

### Evidence e6

[quarantine/prime-agent/prime-agent-runtime/src/rlm/bash.py:954–973](../../../quarantine/prime-agent/prime-agent-runtime/src/rlm/bash.py#L954): Foreground await cancellation kills command group; released background handle survives cell cancellation, owned until kernel teardown.

### Evidence e7

[quarantine/prime-agent/crates/pa-core/src/kernel/state_snapshot.rs:14–59](../../../quarantine/prime-agent/crates/pa-core/src/kernel/state_snapshot.rs#L14): Namespace snapshot caps are 16MiB per variable and 256MiB aggregate with saved/skipped/pruned manifest.

### Evidence e8

[quarantine/prime-agent/prime-agent-runtime/src/rlm/repl.py:770–894](../../../quarantine/prime-agent/prime-agent-runtime/src/rlm/repl.py#L770): Dill per-name snapshot skips failures/size excess, stages payload/manifest and may prune oversize live names.

### Evidence e9

[quarantine/prime-agent/crates/pa-core/src/kernel/manager/mod.rs:428–435](../../../quarantine/prime-agent/crates/pa-core/src/kernel/manager/mod.rs#L428): Last shared inner owner drop performs best-effort hard cleanup.

### Evidence e10

[quarantine/prime-agent/crates/pa-core/src/kernel/manager/teardown.rs:47–188](../../../quarantine/prime-agent/crates/pa-core/src/kernel/manager/teardown.rs#L47): Graceful snapshot/host drain/protocol shutdown followed by deadline cleanup and orphan-group reaping.

### Evidence e11

[quarantine/prime-agent/prime-agent-runtime/src/rlm/harness.py:322–395](../../../quarantine/prime-agent/prime-agent-runtime/src/rlm/harness.py#L322): Model-accessible HarnessState uses structured local/global JSON with mtime reload.

### Evidence e12

[quarantine/prime-agent/prime-agent-runtime/src/rlm/harness.py:615–704](../../../quarantine/prime-agent/prime-agent-runtime/src/rlm/harness.py#L615): Harness entries can be fetched/deleted/listed/created, persisted with explicit scope.

### Evidence e13

[quarantine/prime-agent/prime-agent-runtime/src/rlm/harness.py:895–945](../../../quarantine/prime-agent/prime-agent-runtime/src/rlm/harness.py#L895): Refinement records evidence/outcome and small-edit planning, not an experiment oracle.

### Evidence e14

[quarantine/prime-agent/prime-agent-runtime/src/rlm/__init__.py:161–234](../../../quarantine/prime-agent/prime-agent-runtime/src/rlm/__init__.py#L161): RLM spawn/create_session/model lookup uses host bridge and returns stable child/session handles.

### Evidence e15

[quarantine/prime-agent/prime-agent-runtime/src/rlm/__init__.py:390–428](../../../quarantine/prime-agent/prime-agent-runtime/src/rlm/__init__.py#L390): RLM collect polls or bounded waits for retained direct-child results, no parent steering.

### Evidence e16

[quarantine/prime-agent/crates/pa-daemon/src/agent_messaging.rs:227–303](../../../quarantine/prime-agent/crates/pa-daemon/src/agent_messaging.rs#L227): Family controller uses durable identity, peer transport then supervisor fallback; no blind retry of non-idempotent sends.

### Evidence e17

[quarantine/prime-agent/skills/agent-message/src/agent_message/__init__.py:17–62](../../../quarantine/prime-agent/skills/agent-message/src/agent_message/__init__.py#L17): Role-addressed/broadcast send calls host bridge and returns delivery receipts.

### Evidence e18

[quarantine/prime-agent/crates/pa-daemon/src/worker/agent_message_tests.rs:204–222](../../../quarantine/prime-agent/crates/pa-daemon/src/worker/agent_message_tests.rs#L204): Busy-worker test asserts queued status/time rather than silently dropping or denying delivery.

### Evidence e19

[quarantine/prime-agent/crates/pa-daemon/src/agent_engine/turn/boundary.rs:33–128](../../../quarantine/prime-agent/crates/pa-daemon/src/agent_engine/turn/boundary.rs#L33): Pending compact/refine consumed at settled boundary, compact then refine; summarizer gets its own cancellation slot.

### Evidence e20

[quarantine/prime-agent/crates/pa-core/src/session_engine/ipython_state.rs:180–227](../../../quarantine/prime-agent/crates/pa-core/src/session_engine/ipython_state.rs#L180): Compaction prunes oversized variables and appends namespace/persistence notice.

### Evidence e21

[quarantine/prime-agent/crates/pa-core/src/session_engine/compaction_exec.rs:42–93](../../../quarantine/prime-agent/crates/pa-core/src/session_engine/compaction_exec.rs#L42): Summary request includes previous summary and authoritative retained-tail anchor.

### Evidence e22

[quarantine/prime-agent/crates/pa-core/src/session_engine/refine.rs:23–112](../../../quarantine/prime-agent/crates/pa-core/src/session_engine/refine.rs#L23): Refinement durable audit type, auto review defaults/gates, local scope guard and model-facing outcome/notice.

### Evidence e23

[quarantine/prime-agent/crates/pa-core/src/refinement/executor.rs:202–313](../../../quarantine/prime-agent/crates/pa-core/src/refinement/executor.rs#L202): Plan requests scoped JSON entry edits; apply handles rollback/result, not harness executable compilation.

### Evidence e24

[quarantine/prime-agent/crates/pa-daemon/src/session_custom.rs:335–347](../../../quarantine/prime-agent/crates/pa-daemon/src/session_custom.rs#L335): Reload resolves settings/auth/MCP per use; command succeeds without executable reload.

### Evidence e25

[quarantine/prime-agent/skills/agent-observe/src/agent_observe/__init__.py:15–52](../../../quarantine/prime-agent/skills/agent-observe/src/agent_observe/__init__.py#L15): Read-only family listing/live summaries/bounded recent message previews.

### Evidence e26

[quarantine/prime-agent/crates/pa-agent/tests/agent_loop_scripted.rs:265–300](../../../quarantine/prime-agent/crates/pa-agent/tests/agent_loop_scripted.rs#L265): Tool continuation oracle inspects second request exact user/assistant/toolResult pairing and ID.

### Evidence e27

[quarantine/prime-agent/crates/pa-core/src/session_engine/host_requests.rs:21–68](../../../quarantine/prime-agent/crates/pa-core/src/session_engine/host_requests.rs#L21): Standing heartbeat state includes schedule/next/last run/status/count and steer/follow-up delivery.

### Evidence e28

[quarantine/prime-agent/crates/pa-core/src/kernel/manager/startup.rs:23–109](../../../quarantine/prime-agent/crates/pa-core/src/kernel/manager/startup.rs#L23): Kernel stderr tails/log budgets/rotation bound retained diagnostic bytes.

