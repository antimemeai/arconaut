# windmill

Programmable job/flow service; embedded model tools are ordinary permissioned jobs, with separate execution history and compactable context.

Role: workflow service with embedded AI-agent steps. Runtime: Rust, Python, TypeScript.

Pinned source: [https://github.com/windmill-labs/windmill](https://github.com/windmill-labs/windmill); revision/version `8953ca670aac63feab1a04369df50a669c1f40bc`.

PostgreSQL queue and Rust workers launch language child processes; AI steps await provider/MCP requests and queued tool jobs.

Service owns worker scheduling, queue, job state, memory and administration; Arconaut would consume it rather than acquire its governance.

Inspection: queue completion/cancellation/deploy restart, child lifecycle, Python wrapper, flow advancement, AI dispatch/memory/compaction, public API and direct test assertions

Limits of this study: Private enterprise implementations are absent; no service/process execution and no exhaustive language executor or UI review.

## Actions

### POST /jobs/run/preview, /run/preview_flow, /run/agent/{path}

Surface: operator/program HTTP.

Input: script/flow/agent path or preview program plus args/workspace

Result: job UUID; separately query/list results and progress

Lifecycle: queued worker execution with timeout/cancel

Authority: caller workspace identity and effective job authority

Evidence: [e19](#evidence-e19), [e8](#evidence-e8).

### execute_tool_calls -> enqueue_windmill_tool

Surface: model tool dispatch.

Input: advertised function name and JSON arguments; authored flow module

Result: child job UUID, ordered tool result, AgentAction identity

Lifecycle: batch concurrent jobs; poll completed table and optionally reserve one local worker

Authority: job or configured on-behalf-of identity with tag checks

Evidence: [e6](#evidence-e6), [e7](#evidence-e7), [e8](#evidence-e8).

### MCP tool dispatch

Surface: model extension.

Input: registered MCP function and JSON arguments

Result: tool content/action and stream events

Lifecycle: await remote MCP client; cleanup/abort after parent cancellation

Authority: configured external credentials and advertised tool roster

Evidence: [e6](#evidence-e6), [e16](#evidence-e16).

### WAC dispatch/suspend/resume

Surface: program primitive.

Input: named steps, checkpoints, pending sleep/approval and resume rows

Result: serialized completed_steps/pending steps/job IDs

Lifecycle: fresh process replays program against stored checkpoint; excludes already-consumed resume rows

Authority: authored workflow and authorized resume actor

Evidence: [e1](#evidence-e1), [e18](#evidence-e18).

### cancel job / timeout child

Surface: operator/service primitive.

Input: job UUID and cancellation author/reason or time limit

Result: canceled result/reason and child exit; errors on termination

Lifecycle: poll DB; grace, direct-child signal escalation and reap; does not reverse remote effects

Authority: caller/job service policy

Evidence: [e3](#evidence-e3), [e15](#evidence-e15), [e16](#evidence-e16).

### compact_if_needed / Compactor.compact

Surface: AI-worker policy.

Input: context history, provider, budget and trigger

Result: shorter model context or unchanged failure; original result unchanged

Lifecycle: additional provider request; storage truncation remains distinct

Authority: configured step policy; not arbitrary model powerword

Evidence: [e9](#evidence-e9), [e10](#evidence-e10), [e17](#evidence-e17).

### read_from_memory / write_to_memory

Surface: service primitive.

Input: workspace/conversation/step, messages, allow_truncation

Result: stored messages or count dropped/error

Lifecycle: DB UPSERT after run; public backend capped 100KB

Authority: worker DB authority scoped by key

Evidence: [e11](#evidence-e11), [e12](#evidence-e12).

### restart_perpetual_runs_on_new_version

Surface: deploy service.

Input: workspace/script path/new deployed version

Result: replacement perpetual queued runs

Lifecycle: cancel and two-pass race recovery, not live execution continuation

Authority: deployer/service administrator

Evidence: [e14](#evidence-e14).

## Capabilities

### filesystem

**I — Files** (source): Jobs run arbitrary authored language programs with per-job filesystem/result and monitored child lifecycle; not a shared standing kernel.

Evidence: [e1](#evidence-e1), [e2](#evidence-e2), [e3](#evidence-e3).

### processes

**I — OS programs** (source): Jobs run arbitrary authored language programs with per-job filesystem/result and monitored child lifecycle; not a shared standing kernel.

Evidence: [e1](#evidence-e1), [e2](#evidence-e2), [e3](#evidence-e3).

### code-actions

**I — Code actions** (source): Jobs run arbitrary authored language programs with per-job filesystem/result and monitored child lifecycle; not a shared standing kernel.

Evidence: [e1](#evidence-e1), [e2](#evidence-e2), [e3](#evidence-e3).

### persistent-kernel

**L — Kernel** (source): Fresh wrapper/process plus WAC serialized checkpoints, not retained Python heap; public dedicated-loop worker support is not established.

Evidence: [e1](#evidence-e1), [e2](#evidence-e2).

### standing-database

**I — Standing DB** (source): Service owns PostgreSQL queue, completed results, flow status and scoped replaceable AI memory. A consuming harness does not govern the database.

Evidence: [e4](#evidence-e4), [e5](#evidence-e5), [e11](#evidence-e11), [e12](#evidence-e12).

### workflow-programming

**I — Workflows** (source): Authored flow modules and WAC checkpoint/resume are actual programs; embedded AI tools dispatch nested jobs. Flow retry is not an external-effect transaction.

Evidence: [e5](#evidence-e5), [e6](#evidence-e6), [e18](#evidence-e18).

### multi-model

**S — Models** (source): AI provider settings and nested AI-agent tool modules permit composed model roles; the inspected batching is jobs, not a live multi-model peer room.

Evidence: [e6](#evidence-e6), [e7](#evidence-e7), [e10](#evidence-e10).

### live-collaboration

**L — Peer chat** (source): Nested AI-agent jobs produce returned results; traced queue/agent paths do not establish addressable simultaneous chat peers.

Evidence: [e6](#evidence-e6), [e7](#evidence-e7).

### concurrent-work

**I — Concurrency** (source): Tool job batches execute independently, poll by UUID and publish ordered results; service has multiple workers.

Evidence: [e7](#evidence-e7), [e8](#evidence-e8).

### steering-interrupt

**I — Steer/interrupt** (source): Cancel/timeouts propagate through DB polling/graceful agent cancellation and child signals; force abort does not undo external effects.

Evidence: [e3](#evidence-e3), [e16](#evidence-e16).

### turn-redefinition

**S — Turn program** (source): User-authored flows/tools/WAC program control steps and outputs; AI step still uses a fixed worker loop rather than model-redefinable core turns.

Evidence: [e5](#evidence-e5), [e6](#evidence-e6), [e18](#evidence-e18).

### compaction

**I — Compaction** (source): Threshold/context rejection/storage policies summarize selected prefix without changing run result; failed/non-shrinking summaries preserve context.

Evidence: [e9](#evidence-e9), [e10](#evidence-e10), [e17](#evidence-e17).

### context-repair

**S — Repair** (source): Original per-run result retained separately from context enables reconstruction by a consumer; no model-native context-repair operation traced.

Evidence: [e9](#evidence-e9), [e17](#evidence-e17).

### original-audit

**L — Original audit** (source): Uncompacted run messages and job results exist, but saved memory can drop history, child logs have bounds, and community audit API is explicitly no-op/private.

Evidence: [e3](#evidence-e3), [e9](#evidence-e9), [e11](#evidence-e11), [e13](#evidence-e13), [e17](#evidence-e17).

### audit-query

**L — Audit query** (source): Job/list/queue data and stored transcripts are queryable surfaces; enterprise audit query has no public implementation and returns empty/errors in OSS.

Evidence: [e13](#evidence-e13), [e19](#evidence-e19).

### hot-change

**L — Hot change** (source): Deploy triggers perpetual-run cancellation/replacement, including race repair; not default turn-conclusion activation or in-process hot code swap.

Evidence: [e14](#evidence-e14).

### rebuild-continuity

**L — Rebuild continuity** (source): Durable job/flow/WAC data and lost-cancel repair support service restart; no harness outpost/refit protocol and no preservation of arbitrary process heap.

Evidence: [e1](#evidence-e1), [e4](#evidence-e4), [e15](#evidence-e15), [e18](#evidence-e18).

### remote-services

**I — Remote** (source): Provider/MCP clients and HTTP job APIs are actual external boundaries. Shared Windmill service remains separately governed.

Evidence: [e6](#evidence-e6), [e16](#evidence-e16), [e19](#evidence-e19).

### self-improvement

**S — Self-improve** (source): Authored program deployment and preview execution supply change/experiment primitives, not a model-governed autoresearch evaluator.

Evidence: [e14](#evidence-e14), [e19](#evidence-e19).

### complaints

**S — Complaints** (source): Stranded-job alerts/cancel reasons/logs capture service faults, not full agent-state complaint beads.

Evidence: [e3](#evidence-e3), [e15](#evidence-e15).

### authority

**I — Authority** (source): Effective identity/workspace/tag restrictions bound queued actions; service policies can be separate from a consuming operator-centric harness.

Evidence: [e8](#evidence-e8).

### evaluation

**I — Evaluation** (source): Inspected SQL tests specify eligible stranded jobs and race exclusion; compaction asserts original serialized results and tool exchange integrity, without judging semantic summary fidelity.

Evidence: [e15](#evidence-e15), [e20](#evidence-e20).

### time-order

**I — Time/order** (source): Call-order transcript publication is separate from completion order; SQL cancellation and flow/checkpoint identity avoid stale/racing updates.

Evidence: [e7](#evidence-e7), [e15](#evidence-e15), [e18](#evidence-e18).

## Inspected test oracles

- [quarantine/windmill/backend/src/stranded_jobs.rs](../../../quarantine/windmill/backend/src/stranded_jobs.rs): unserved-root selection and pull/cancel race Oracle: Exact sorted eligible tag/count set; running job cancellation remains None; DB fixtures exercise real SQL state but not external process stop. Read, **not executed**.
- [quarantine/windmill/backend/windmill-worker/src/ai/compaction.rs](../../../quarantine/windmill/backend/windmill-worker/src/ai/compaction.rs): original retention and exchange boundaries Oracle: Exact serialized before/after result equality, distinct action IDs and exchange starts; synthetic summaries do not assess model fidelity. Read, **not executed**.

## Useful mechanisms

- Tool execution composes ordinary durable workflow jobs with useful UUIDs and ordered results.
- Separate result/context arrays directly preserve in-run action history through compaction.
- Public source makes community/enterprise audit and memory boundaries visible.

## Material limits

- Job DB completion is not atomic with arbitrary side effects.
- 100KB public memory and optional truncation require a separate original archive.
- Unix direct-child signals alone do not settle arbitrary descendants or remote work.

## Arconaut design questions

- Which job/result handles should Arconaut retain while Windmill remains an independent service?
- Can compaction retain source originals and expose model repair without copying enterprise governance?
- What observable acknowledgment settles each child/remote effect before refit?

## Evidence

### Evidence e1

[quarantine/windmill/backend/windmill-worker/src/python_executor.rs:1000–1025](../../../quarantine/windmill/backend/windmill-worker/src/python_executor.rs#L1000): Python wrappers persist result/errors; WAC checkpoint is loaded and prepared before a fresh Python process runs.

### Evidence e2

[quarantine/windmill/backend/windmill-worker/src/python_executor.rs:1192–1238](../../../quarantine/windmill/backend/windmill-worker/src/python_executor.rs#L1192): Child is monitored with timeout/cancellation, then structured result read; WAC dispatch is interpreted afterward, not resident Python variables.

### Evidence e3

[quarantine/windmill/backend/windmill-worker/src/handle_child.rs:215–322](../../../quarantine/windmill/backend/windmill-worker/src/handle_child.rs#L215): Child wait competes with logs/timeout/cancel; Unix escalation signals direct child PID, then kills and reaps it. This alone is not an arbitrary descendant/service quiescence guarantee.

### Evidence e4

[quarantine/windmill/backend/windmill-queue/src/jobs.rs:1770–1828](../../../quarantine/windmill/backend/windmill-queue/src/jobs.rs#L1770): Queue row deletion and completed-result insertion are one SQL statement; external script effects precede database completion.

### Evidence e5

[quarantine/windmill/backend/windmill-worker/src/worker_flow.rs:415–495](../../../quarantine/windmill/backend/windmill-worker/src/worker_flow.rs#L415): Flow advancement reloads durable module status and pinned runnable/preview flow after child completion.

### Evidence e6

[quarantine/windmill/backend/windmill-worker/src/ai/tools.rs:91–192](../../../quarantine/windmill/backend/windmill-worker/src/ai/tools.rs#L91): AI tools are discovered by exposed name, MCP calls dispatched, adjacent ordinary modules batched, structured-output pseudo-tool terminates with arguments.

### Evidence e7

[quarantine/windmill/backend/windmill-worker/src/ai/tools.rs:756–886](../../../quarantine/windmill/backend/windmill-worker/src/ai/tools.rs#L756): Queued tool UUIDs are polled from completed jobs; concurrent results become transcript/tool messages in original call order.

### Evidence e8

[quarantine/windmill/backend/windmill-worker/src/ai/tools.rs:503–590](../../../quarantine/windmill/backend/windmill-worker/src/ai/tools.rs#L503): Tool job inherits effective job/on-behalf-of authority, workspace/tag restrictions, parent/root identities and priority.

### Evidence e9

[quarantine/windmill/backend/windmill-worker/src/ai/compaction.rs:39–90](../../../quarantine/windmill/backend/windmill-worker/src/ai/compaction.rs#L39): AgentHistory maintains separate result and context; prefix replacement modifies only model context.

### Evidence e10

[quarantine/windmill/backend/windmill-worker/src/ai/compaction.rs:259–317](../../../quarantine/windmill/backend/windmill-worker/src/ai/compaction.rs#L259): Compaction installs only a smaller usable summary, retains original history on failure/unhelpful output and bounds consecutive failures.

### Evidence e11

[quarantine/windmill/backend/windmill-worker/src/memory_common.rs:5–95](../../../quarantine/windmill/backend/windmill-worker/src/memory_common.rs#L5): Standing memory key workspace/conversation/step; 100KB DB entry, optional oldest-message truncation, UPSERT replaces saved view.

### Evidence e12

[quarantine/windmill/backend/windmill-worker/src/memory_oss.rs:1–62](../../../quarantine/windmill/backend/windmill-worker/src/memory_oss.rs#L1): Public OSS memory reads/writes PostgreSQL; enterprise object-storage implementation is separate/unavailable.

### Evidence e13

[quarantine/windmill/backend/windmill-audit/src/audit_oss.rs:72–109](../../../quarantine/windmill/backend/windmill-audit/src/audit_oss.rs#L72): Community audit_log is a no-op, list_audit empty, get_audit rejects. Job transcripts are not this enterprise audit feature.

### Evidence e14

[quarantine/windmill/backend/windmill-queue/src/jobs.rs:649–686](../../../quarantine/windmill/backend/windmill-queue/src/jobs.rs#L649): Deploy can restart perpetual scripts; second pass catches completion/restart race. This is cancellation/replacement rather than live kernel/code migration.

### Evidence e15

[quarantine/windmill/backend/src/stranded_jobs.rs:315–406](../../../quarantine/windmill/backend/src/stranded_jobs.rs#L315): Stranded cancellation marks under SQL lock and repairs asynchronous completion lost on server stop; it excludes running jobs.

### Evidence e16

[quarantine/windmill/backend/windmill-worker/src/ai_executor.rs:1120–1187](../../../quarantine/windmill/backend/windmill-worker/src/ai_executor.rs#L1120): Agent cancellation gets grace period, then task abort handles and orphaned-job cleanup; MCP clients cleaned after outcome.

### Evidence e17

[quarantine/windmill/backend/windmill-worker/src/ai_executor.rs:2250–2268](../../../quarantine/windmill/backend/windmill-worker/src/ai_executor.rs#L2250): Returned run result uses unrewritten execution messages; saved memory may retain only newest complete exchanges or none.

### Evidence e18

[quarantine/windmill/backend/windmill-worker/src/wac_executor.rs:255–337](../../../quarantine/windmill/backend/windmill-worker/src/wac_executor.rs#L255): WAC resume uses durable consumed resume-row IDs, inserts approval answer into completed_steps and saves checkpoint; identity separates approval steps.

### Evidence e19

[quarantine/windmill/backend/windmill-api/src/jobs.rs:271–310](../../../quarantine/windmill/backend/windmill-api/src/jobs.rs#L271): Public HTTP run preview/flow/agent and queue/list APIs expose program invocation and job identities.

### Evidence e20

[quarantine/windmill/backend/windmill-worker/src/ai/compaction.rs:880–943](../../../quarantine/windmill/backend/windmill-worker/src/ai/compaction.rs#L880): Direct tests compare complete serialized result before/after prefix change and assert parallel tool exchanges stay intact.

