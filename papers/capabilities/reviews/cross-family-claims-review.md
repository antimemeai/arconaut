# Independent review of selected cross-family claims

2026-10-01. Reviewer: primary agent, independently reading selected sources after
the native, interactive, science and framework authors supplied their rows.
This challenges semantic capability claims. Renderer/schema/source-range checks
attack a different fault class. No acquired program or test was executed.
This is bounded review of consequential claims, not a complete audit of 105 engines.

## Corrections

**Letta public index: rebuild continuity and linked feature claims.** The row had
classified rebuild continuity as a documented capability because the landing
README links to memory, identity and conversation availability. The axis concerns
actual executable replacement and re-inhabitation. Those links do not supply that
contract. The author corrected this and other feature axes inferred from landing
links to inapplicable for this index role. Current Letta Code and retired V1 remain
separate evidence objects; neither transfers implementation into the landing row.
[Row](../rows/letta.json), [complete README](../../../quarantine/letta/README.md#L1).

**OpenAI4S query: optional row limit.** The standing-database cell described bounded
read-only SELECT/CTE. `Store.query` defaults `limit=None`, then uses `fetchall()`
when no truthy limit is supplied. Its real SQLite authorizer, scoped views, shared
connection lock and statement timeout support the SQL capability, but not a
mandatory returned-row bound. Requested correction: explicitly distinguish the
optional row limit from statement timeout. The author confirmed and updated the
row to say optional row limit, with default all rows.
[Row](../rows/openai4s.json), [query](../../../quarantine/openai4s/openai4s/store.py#L5826).

## Claims checked against actual mechanisms

**VTCode queued-input ownership.** Independently followed `send_input` →
`restart_child` → `launch_child` → record dequeue/apply. Noninterrupt input while
Running/Queued changes status to Waiting and queues input without restart. Another
input while Waiting enters the restart path. `launch_child` creates a task and
overwrites the stored handle; the inspected path does not reject another live
loop for that record. Each loop can update the same status/summary/context.
The row's source-inferred overlapping-run concern is supported. It remains a
potential schedule-dependent defect, not an executed reproduction or proof that
every send overlaps.
[Input](../../../quarantine/vtcode/crates/codegen/vtcode-core/src/subagents/controller_spawn_run.rs#L194),
[restart](../../../quarantine/vtcode/crates/codegen/vtcode-core/src/subagents/controller_spawn_run.rs#L1058),
[launch](../../../quarantine/vtcode/crates/codegen/vtcode-core/src/subagents/controller_child_loop.rs#L40),
[shared record](../../../quarantine/vtcode/crates/codegen/vtcode-core/src/subagents/types.rs#L421).

**Dagger returned conversation and reseed.** Read the actual adoption branch:
the continuation becomes the base, tool-result/state selectors apply to it, and
cancelled calls still produce paired history on a cancellation-detached context.
`Reseed` rejects running/draining states and records resolution of consumed input
when replacing a suspended conversation. Those are real conversation/continuation
mechanisms, not a prompt-only feature. They do not establish our default whole
turn/workflow activation or settlement of external effects. The reseed rewind
marker is explicitly best-effort. The full-audit limit is directly supported by
the tool-result guard: oversized original return values were not saved elsewhere.
[Continuation](../../../quarantine/dagger/core/llm.go#L2420),
[reseed](../../../quarantine/dagger/core/agent.go#L2233),
[lost return bytes](../../../quarantine/dagger/core/mcp.go#L1800).

**Docker Agent original history versus complete originals.** `AddMessage` calls
`capToolResultContent` before appending its message item. The inspected SQLite
reopen test checks retained item count, summary/first-kept metadata and
provider-scoped native compaction blocks; it does not compare the complete
pre-cap tool payload. Retention through compaction and clipping before capture
are compatible facts. The row's limited original-audit classification is supported.
[Ingestion](../../../quarantine/docker-agent/pkg/session/session.go#L881),
[reopen oracle](../../../quarantine/docker-agent/pkg/runtime/native_compaction_test.go#L439).

**Oh My Pi model complaint.** The grievance schema contains model, version, tool,
report, timestamp and push/error status. The device calls a detached async
consent/insert/flush pipeline, then returns an acknowledgement. This is a real
model-facing complaint mechanism with partial state capture; acknowledgement is
not proof of insertion and cannot stand in for Arconaut's captured-state bead plus
database linkage. The row's limitation is supported.
[Schema](../../../quarantine/oh-my-pi/packages/coding-agent/src/tools/report-tool-issue.ts#L248),
[pipeline and result](../../../quarantine/oh-my-pi/packages/coding-agent/src/tools/report-tool-issue.ts#L591).

**Prime addressed message and busy-target witness.** The Python skill invokes the
host bridge with parent/sibling/child role and optional receiver name, or a
broadcast target, and exposes delivery receipts. The daemon controller defines
family identity and direct/supervisor routing. The busy-worker test calls actual
dispatch and checks `queued` plus a timestamp, not recipient consumption or a
reply. The live collaboration claim is supported within that distinction; the
test cannot demonstrate end-to-end model observation.
[Skill](../../../quarantine/prime-agent/skills/agent-message/src/agent_message/__init__.py#L17),
[controller](../../../quarantine/prime-agent/crates/pa-daemon/src/agent_messaging.rs#L227),
[busy oracle](../../../quarantine/prime-agent/crates/pa-daemon/src/worker/agent_message_tests.rs#L204).

**Claude Science contract scope.** Re-read official core concepts, tools/environments
and artifact descriptions. Documented local remembered facts are distinct from
a generic SQL interface. Kernels have idle, session and environment-restart limits.
Artifact provenance captures messages/code/execution/environment/review while
scratch files expire. Implementation remains unavailable in this acquisition.
The row correctly separates documentation from engine evidence and does not
borrow certainty from another application or unofficial Claude source.
[Memory](../../../quarantine/claude-science/raw/core-concepts.md#L53),
[kernel](../../../quarantine/claude-science/raw/tools-and-environments.md#L9),
[artifact scope](../../../quarantine/claude-science/raw/artifacts.md#L9).

**OpenAI4S recovery refuses partial replay promotion.** Independently read the
candidate replay path. Hash/safety failures are recorded as skipped; failed replay
is recorded; validation issues lead to a partial outcome and candidate shutdown.
That supports conservative logical recovery and its limitations. It cannot certify
all external effects or preserve a live foreground process during a harness refit.
[Recovery](../../../quarantine/openai4s/openai4s/kernel/recovery.py#L1080).

**Bub anchor summary projection.** `TapeContext` defaults to LAST_ANCHOR, the
store begins after the anchor, and default projection selects message entries.
Handoff stores state in an anchor plus event; neither contributes a message through
that default selector. The row's warning about default handoff-summary exclusion
is supported. A custom selector or explicit full-history read is a different
contract and must not be described as universally broken compaction.
[Selector](../../../quarantine/bub/src/bub/tape.py#L148),
[handoff](../../../quarantine/bub/src/bub/tape.py#L291),
[slice](../../../quarantine/bub/src/bub/store.py#L180).

## Remaining validation boundary

Each row enumerates inspection scope and tests read. Schema/range validation can
reject corrupt references and dangling citations; it cannot establish causal
settlement, truthful model evaluation, scientific correctness or faithful repair.
The [synthesis](../SYNTHESIS.md) proposes discriminating witnesses for those claims.
Future executable research must preserve its own subject/evaluator/version identities
and cannot retroactively turn this source study into executed conformance.
