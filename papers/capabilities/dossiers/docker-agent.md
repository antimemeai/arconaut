# docker-agent

Programmable hook-driven coding team with steering/follow-up queues, model changes, subagents and managed compaction.

Role: coding-agent/team runtime. Runtime: Go.

Pinned source: [https://github.com/docker/docker-agent](https://github.com/docker/docker-agent); revision/version `83fca5b2577d5548c59bde40d5d0b067d532a71a`.

Go goroutine runtime, provider stream loop, parallel tool batches and native/background OS processes.

Owns sessions, toolset supervision, child jobs and native memory DB adapter; consumes providers, MCP/toolsets and configured evaluator endpoints.

Inspection: provider/dispatch loop, tool/hook contracts, OS job lifetime, delegation, SQLite compaction, memory/evaluator contracts and direct test assertions

Limits of this study: No upstream execution; broad runtime sampled through selected actual surfaces. MCP/provider dependencies and full configuration/CLI paths not exhaustively traced.

## Actions

### read_file / edit_file / write_file / search_files_content

Surface: model filesystem toolset.

Input: paths, text/edit instructions or query

Result: text/file result or handler error

Lifecycle: awaited dispatch; result appended subject to cap

Authority: model within toolset permissions/hooks; native host filesystem

Evidence: [e22](#evidence-e22), [e4](#evidence-e4), [e15](#evidence-e15).

### shell

Surface: model native tool.

Input: command, cwd, timeout

Result: formatted output/exit/timeout result

Lifecycle: await child; cancel group TERM then direct-child KILL

Authority: model approved native host shell, ordinary command authority opaque

Evidence: [e5](#evidence-e5), [e6](#evidence-e6).

### run_background_job / list_background_jobs / view_background_job / wait_background_job / stop_background_job

Surface: model native background toolset.

Input: cmd/cwd/recall then job ID

Result: job_<epoch>_<counter>, bounded output/status/exit; optional recall steering

Lifecycle: host Map tracks request-independent child; not restored heap/daemon service

Authority: model via permissions; completion can steer runtime if recall enabled

Evidence: [e7](#evidence-e7), [e8](#evidence-e8).

### transfer_task / handoff / run_skill

Surface: model/runtime-managed tool.

Input: configured target agent or skill, task/expected output

Result: child events/answer or route change

Lifecycle: fresh child session, attached-file/lineage propagation; forwarding waits drained child

Authority: caller-scoped agent resolver, pin protects concurrent foreground routing

Evidence: [e4](#evidence-e4), [e10](#evidence-e10), [e11](#evidence-e11).

### RunAgent

Surface: programmatic runtime API/background agent.

Input: SubSessionConfig with agent/task/context settings and content callback

Result: RunResult/content/error/usage

Lifecycle: pinned child stream collecting; completion hook on failure/cancel

Authority: runtime-resolved caller; no implied full parent context

Evidence: [e10](#evidence-e10), [e12](#evidence-e12).

### Steer / FollowUp / CancelSteer

Surface: operator/program runtime API.

Input: QueuedMessage/content/message ID

Result: queue accepted or full/not-found error

Lifecycle: urgent after current batch; followups full subsequent turns

Authority: runtime client controls steering; queued message hook may veto

Evidence: [e9](#evidence-e9), [e2](#evidence-e2).

### change_model / revert_model

Surface: model/runtime-managed tool.

Input: model identifier/override

Result: model switch/result events

Lifecycle: subsequent request model selection; per caller

Authority: tool permissions plus caller agent rather than unscoped global

Evidence: [e4](#evidence-e4), [e1](#evidence-e1).

### before_llm_call / before_compaction / tool_input_transform / tool_guard

Surface: programmable hook registry.

Input: event JSON + command/builtin/model configuration, updated messages/input/summary

Result: continue/block/rewritten context or tool input/diagnostics

Lifecycle: timed hooks at explicit loop phases; partial failed output nonauthoritative

Authority: operator authors hooks, registered handler boundary; guard decisions policy-governed

Evidence: [e1](#evidence-e1), [e13](#evidence-e13), [e18](#evidence-e18), [e24](#evidence-e24).

### add_memory / get_memories / search_memories / update_memory / delete_memory

Surface: model native memory tools.

Input: memory/category or ID/query

Result: ID, JSON list or DB error

Lifecycle: await injected DB mutation/query; record timestamp not effect-journal transaction

Authority: model tool approval; memory DB adapter owns storage separately from callable schema

Evidence: [e19](#evidence-e19).

### RestartToolset / Evaluator.Evaluate

Surface: programmatic runtime/support APIs.

Input: toolset display name; separately configured evaluation state

Result: restart error or typed Result probabilities/score/usage

Lifecycle: restart lifecycle serialized; evaluator HTTP timed independently

Authority: runtime/operator configuration and endpoint credentials; assessment policy left caller

Evidence: [e17](#evidence-e17), [e20](#evidence-e20), [e21](#evidence-e21).

## Capabilities

### filesystem

**I — Files** (source): Native filesystem read/edit/write/search/directory toolset, passed through approval/hook dispatcher.

Evidence: [e22](#evidence-e22), [e4](#evidence-e4).

### processes

**I — OS programs** (source): Foreground native shell with group cancellation and background jobs with recall, wait/list/view/stop handles; deliberate request-independent background lifetime.

Evidence: [e5](#evidence-e5), [e6](#evidence-e6), [e7](#evidence-e7), [e8](#evidence-e8).

### code-actions

**S — Code actions** (source): Shell/command-hook programs provide ordinary opaque native computation; no traced structured notebook/code-action state or persistent language kernel.

Evidence: [e5](#evidence-e5), [e18](#evidence-e18).

### persistent-kernel

**L — Kernel** (source): Background shell job exists beyond request but per-host job Map/output buffer; does not serialize interpreter heap or restore owned handle after rebuild.

Evidence: [e7](#evidence-e7), [e8](#evidence-e8).

### standing-database

**I — Standing DB** (source): Injected native memory DB CRUD/search and SQLite session/summary storage; these local harness-owned stores differ from externally governed general computation services.

Evidence: [e14](#evidence-e14), [e19](#evidence-e19).

### workflow-programming

**I — Workflows** (source): Event hook registry/pipeline with command/builtin/model handlers, task/skill/handoff delegation and runtime-managed actions; imperative workflow through tools/hooks rather than durable graph replay.

Evidence: [e4](#evidence-e4), [e10](#evidence-e10), [e11](#evidence-e11), [e18](#evidence-e18).

### multi-model

**I — Models** (source): Provider fallback and model change/revert; pinned child agents run their own configured models without global foreground mutation.

Evidence: [e1](#evidence-e1), [e4](#evidence-e4), [e11](#evidence-e11), [e12](#evidence-e12).

### live-collaboration

**S — Peer chat** (source): Concurrent subagent streams and completion recall steer foreground; selected delegation surfaces are task-oriented, not traced peer room/mail protocol.

Evidence: [e7](#evidence-e7), [e9](#evidence-e9), [e11](#evidence-e11), [e12](#evidence-e12).

### concurrent-work

**I — Concurrency** (source): Parallel tool batches/background jobs and pinned agent sessions; interactive confirmations serialize because decisions unkeyed by call ID.

Evidence: [e3](#evidence-e3), [e7](#evidence-e7), [e11](#evidence-e11), [e12](#evidence-e12).

### steering-interrupt

**L — Steer/interrupt** (source): Urgent steer after current batch and queued followups; batch cancellation settles error responses; foreground group TERM but KILL escalates direct child, backgrounds deliberately remain request-independent.

Evidence: [e3](#evidence-e3), [e5](#evidence-e5), [e6](#evidence-e6), [e7](#evidence-e7), [e9](#evidence-e9).

### turn-redefinition

**I — Turn program** (source): Before-LLM hook can stop/rewrite messages; custom compaction summary/veto, tool transform/guard and command/builtin/model hook registry reshape normal execution.

Evidence: [e1](#evidence-e1), [e4](#evidence-e4), [e13](#evidence-e13), [e18](#evidence-e18), [e24](#evidence-e24).

### compaction

**I — Compaction** (source): Managed applied/skipped/failed outcomes, custom/native/LLM modes; atomic stored metadata+summary and provider-specific continuation payload.

Evidence: [e13](#evidence-e13), [e14](#evidence-e14), [e23](#evidence-e23).

### context-repair

**S — Repair** (source): Retained pre-summary items and readable fallback summary plus configurable precompaction hook support reconstruction; no automatic corruption-repair protocol established.

Evidence: [e13](#evidence-e13), [e14](#evidence-e14), [e23](#evidence-e23).

### original-audit

**L — Original audit** (source): Stored session/history and typed events preserve many originals through summary; configured textual tool cap applies BEFORE append, hook guards may suppress bodies and OS capture is bounded.

Evidence: [e15](#evidence-e15), [e16](#evidence-e16), [e8](#evidence-e8), [e18](#evidence-e18), [e23](#evidence-e23).

### audit-query

**S — Audit query** (source): SQLite session_items/summary structure and structured memory queries available; this storage is not complete original request/effect/queryable audit.

Evidence: [e14](#evidence-e14), [e19](#evidence-e19).

### hot-change

**L — Hot change** (source): Toolset restart lifecycle serialization and post-tool reprobe updates next iteration tool discovery. Selected path lacks full config/code refit with all owned work paused/drained.

Evidence: [e2](#evidence-e2), [e17](#evidence-e17).

### rebuild-continuity

**L — Rebuild continuity** (source): Reopen resumes logical session/native compaction payload; transient child pin/lineage/job maps and active provider/OS handles are not durable continuations.

Evidence: [e8](#evidence-e8), [e10](#evidence-e10), [e11](#evidence-e11), [e23](#evidence-e23).

### remote-services

**I — Remote** (source): Consumes provider fallback, external toolsets and separately configured typed evaluator endpoint; local native jobs are harness-owned rather than fabric-managed.

Evidence: [e1](#evidence-e1), [e17](#evidence-e17), [e20](#evidence-e20).

### self-improvement

**S — Self-improve** (source): Command hooks/evaluators can be composed into experiments; typed assessment primitive leaves policy to caller and selected source does not establish governed self-modifying harness research.

Evidence: [e18](#evidence-e18), [e20](#evidence-e20), [e21](#evidence-e21).

### complaints

**S — Complaints** (source): Error/loop notifications, hook diagnostics and session/tool identities support attributable faults; state-capturing external complaint/bead DB not established.

Evidence: [e2](#evidence-e2), [e7](#evidence-e7), [e18](#evidence-e18).

### authority

**I — Authority** (source): Ordered session/team permissions and mode/guard/hook approval outcomes. Autonomous removes mode prompts but session ask remains authoritative; noninteractive approval has explicit deny path.

Evidence: [e3](#evidence-e3), [e4](#evidence-e4).

### evaluation

**I — Evaluation** (source): Independent typed boolean/choice/score evaluator contracts and test oracles; assessment does not itself select or promote harness changes.

Evidence: [e20](#evidence-e20), [e21](#evidence-e21), [e24](#evidence-e24).

### time-order

**L — Time/order** (source): Mutex-assigned append positions and compaction transaction order; concurrent tools execute independent, and queue messages activate after batches. IDs/time stamps are not full external causal-effect settlement.

Evidence: [e3](#evidence-e3), [e7](#evidence-e7), [e9](#evidence-e9), [e14](#evidence-e14), [e15](#evidence-e15).

## Inspected test oracles

- [quarantine/docker-agent/pkg/runtime/native_compaction_test.go](../../../quarantine/docker-agent/pkg/runtime/native_compaction_test.go): provider-native summary persistence/reopen and cross-provider replay boundary Oracle: Mock compactor plus real temporary SQLite reopen checks four retained items, first-kept index, exact native block, readable fallback and provider-scoped replay. Tests continuation data, not summary faithfulness or live request refit. Read, **not executed**.
- [quarantine/docker-agent/pkg/hooks/tool_phases_test.go](../../../quarantine/docker-agent/pkg/hooks/tool_phases_test.go): hook patch composition, immutability, failure policy and verdict classes Oracle: Exact map/string and Allowed/exit-code assertions prove ordered patch accumulation and original input unchanged; command fixtures only source-read. No cross-process/harness upgrade oracle. Read, **not executed**.
- [quarantine/docker-agent/pkg/evaluator/provider/typesafe_test.go](../../../quarantine/docker-agent/pkg/evaluator/provider/typesafe_test.go): typed evaluator primitive response contract Oracle: HTTP fixture table matches exact parsed boolean/choice/score/usage JSON including zero confidence and ties. Measures protocol conversion, not assessment correctness or optimization benefit. Read, **not executed**.

## Useful mechanisms

- Hooks materially reshape normal turns, tools and compaction; not observation-only.
- Pinned sub-session routing and explicit urgent/follow-up queues expose useful concurrency ownership.
- Atomic summary continuation plus provider-specific opaque block retained alongside earlier items.

## Material limits

- Captured tool text may already be capped before stored append; complete originals need separate retention.
- Request-independent in-memory background jobs are not restartable computation services; daemonized inherited pipes have explicit loss/SIGPIPE boundary.
- Toolset restart/reprobe is narrower than whole-harness refit; evaluation primitive is not autoresearch.

## Arconaut design questions

- Can mutable phase hooks retain exact pre/post representations and versioned authority decisions without imposing routine prompts?
- How should an owned-work inventory distinguish request-independent jobs from shared daemon consumers before refit?
- Can provider-native summary blocks coexist with repairable, queryable originals and cross-model context portability?

## Evidence

### Evidence e1

[quarantine/docker-agent/pkg/runtime/loop.go:839–897](../../../quarantine/docker-agent/pkg/runtime/loop.go#L839): Before-provider hooks may stop/rewrite messages; fallback provider chain executes request with current tools/budget admission.

### Evidence e2

[quarantine/docker-agent/pkg/runtime/loop.go:970–1082](../../../quarantine/docker-agent/pkg/runtime/loop.go#L970): Records assistant message then dispatches tool batch, reprobes toolsets for next iteration, detects repetition and drains urgent steering after tools.

### Evidence e3

[quarantine/docker-agent/pkg/runtime/toolexec/dispatcher.go:239–292](../../../quarantine/docker-agent/pkg/runtime/toolexec/dispatcher.go#L239): Independent model calls run parallel through concurrent MapSlice; cancellation stops siblings, confirmations serialized because resume decisions lack call IDs.

### Evidence e4

[quarantine/docker-agent/pkg/runtime/tool_dispatch.go:36–121](../../../quarantine/docker-agent/pkg/runtime/tool_dispatch.go#L36): Binds change_model/revert_model/transfer_task/handoff/run_skill and ordered permission tiers; session ask beats every safety mode.

### Evidence e5

[quarantine/docker-agent/pkg/tools/builtin/shell/shell.go:147–208](../../../quarantine/docker-agent/pkg/tools/builtin/shell/shell.go#L147): Native shell starts process group, collects output, waits on completion/cancel, SIGTERM then direct-child SIGKILL escalation.

### Evidence e6

[quarantine/docker-agent/pkg/tools/builtin/shell/cmd_unix.go:12–26](../../../quarantine/docker-agent/pkg/tools/builtin/shell/cmd_unix.go#L12): Unix Setpgid and negative-PID SIGTERM group; direct-child escalation in shell handler is not universal group SIGKILL.

### Evidence e7

[quarantine/docker-agent/pkg/tools/builtin/backgroundjobs/backgroundjobs.go:230–338](../../../quarantine/docker-agent/pkg/tools/builtin/backgroundjobs/backgroundjobs.go#L230): Background job ID/job map, bounded writer, child deliberately outlives request context; monitor can enqueue completion recall.

### Evidence e8

[quarantine/docker-agent/pkg/tools/builtin/backgroundjobs/backgroundjobs.go:30–79](../../../quarantine/docker-agent/pkg/tools/builtin/backgroundjobs/backgroundjobs.go#L30): In-memory background handle/output cap 10MB; grandchild pipe-close may truncate output/SIGPIPE, recommends daemon output redirection.

### Evidence e9

[quarantine/docker-agent/pkg/runtime/runtime.go:2003–2026](../../../quarantine/docker-agent/pkg/runtime/runtime.go#L2003): Steer admitted after current tool batch; FollowUp queues full next turns one message at a time, queue-full error and cancel-steer ID.

### Evidence e10

[quarantine/docker-agent/pkg/runtime/agent_delegation.go:218–245](../../../quarantine/docker-agent/pkg/runtime/agent_delegation.go#L218): Fresh child context/task instructions inherits attached files, agent options and delegation lineage rather than full implicit parent history.

### Evidence e11

[quarantine/docker-agent/pkg/runtime/agent_delegation.go:350–405](../../../quarantine/docker-agent/pkg/runtime/agent_delegation.go#L350): Forwarding delegation pins background parent separately from shared foreground agent and drains child events for completion hooks.

### Evidence e12

[quarantine/docker-agent/pkg/runtime/agent_delegation.go:440–475](../../../quarantine/docker-agent/pkg/runtime/agent_delegation.go#L440): Background collecting child resolves caller from pinned session and streams/forwards usage, completion hook even failure/cancel.

### Evidence e13

[quarantine/docker-agent/pkg/runtime/session_compaction.go:71–128](../../../quarantine/docker-agent/pkg/runtime/session_compaction.go#L71): Before-compaction hook veto/custom summary; native or delegated LLM strategy, terminal outcomes distinguish applied/skipped/failed.

### Evidence e14

[quarantine/docker-agent/pkg/session/store.go:1452–1513](../../../quarantine/docker-agent/pkg/session/store.go#L1452): SQLite compaction metadata+summary row one transaction, apply in-memory only after commit; not arbitrary tool-effect transaction.

### Evidence e15

[quarantine/docker-agent/pkg/session/session.go:881–901](../../../quarantine/docker-agent/pkg/session/session.go#L881): Session append middle-out caps textual tool payload at ingestion and clears request-only assembly marks before stored transcript.

### Evidence e16

[quarantine/docker-agent/pkg/session/session.go:346–365](../../../quarantine/docker-agent/pkg/session/session.go#L346): Configured ingestion cap and prompt backstop have bounded/unlimited settings; old-call prompt truncation separate.

### Evidence e17

[quarantine/docker-agent/pkg/runtime/runtime.go:1210–1250](../../../quarantine/docker-agent/pkg/runtime/runtime.go#L1210): RestartToolset uses canonical lifecycle wrapper restart serialization; display-name collisions skip nonrestartable matches. No whole runtime compile/refit barrier here.

### Evidence e18

[quarantine/docker-agent/pkg/hooks/executor.go:321–382](../../../quarantine/docker-agent/pkg/hooks/executor.go#L321): Registered command/builtin/model handlers operate on event JSON with timeout, cancellation normalization and partial output dropped on failure.

### Evidence e19

[quarantine/docker-agent/pkg/tools/builtin/memory/memory.go:160–227](../../../quarantine/docker-agent/pkg/tools/builtin/memory/memory.go#L160): Native add/get/delete/search/update memory dispatch to injected DB with ID/category/created timestamp and JSON results.

### Evidence e20

[quarantine/docker-agent/pkg/evaluator/provider/provider.go:26–105](../../../quarantine/docker-agent/pkg/evaluator/provider/provider.go#L26): Configured typesafe evaluator independent endpoint/env credentials per evaluation and timeout; builds boolean/choice/score questions.

### Evidence e21

[quarantine/docker-agent/pkg/evaluator/evaluator.go:1–29](../../../quarantine/docker-agent/pkg/evaluator/evaluator.go#L1): Evaluator returns typed probabilities/choice/score/usage; caller determines policy, not autonomous harness optimization.

### Evidence e22

[quarantine/docker-agent/pkg/tools/builtin/filesystem/filesystem.go:354–445](../../../quarantine/docker-agent/pkg/tools/builtin/filesystem/filesystem.go#L354): Native read/edit/write/list/search/mkdir/rmdir tools expose filesystem handlers and schema.

### Evidence e23

[quarantine/docker-agent/pkg/runtime/native_compaction_test.go:439–481](../../../quarantine/docker-agent/pkg/runtime/native_compaction_test.go#L439): SQLite reopen oracle preserves original items, summary/first-kept/native provider block; another provider gets readable summary but not block replay.

### Evidence e24

[quarantine/docker-agent/pkg/hooks/tool_phases_test.go:24–83](../../../quarantine/docker-agent/pkg/hooks/tool_phases_test.go#L24): Composition test asserts transform patches retain unrelated fields and caller original not mutated; failure policy retains prior patches.

