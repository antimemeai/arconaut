# hermes-agent

Strong non-destructive transcript compaction, programmable context-engine selection, live child steering and real persistent Python; abandoning tool threads is not resource settlement.

Role: persistent extensible Python agent. Runtime: Python.

Pinned source: [https://github.com/NousResearch/hermes-agent](https://github.com/NousResearch/hermes-agent); revision/version `cfdcea4f2226932adde690f6e53e0c8d259147ec`.

Synchronous conversation loop, daemon tool/child pools, per-owner Python session kernels and gateway background routing.

Consumes model/SSH/container/MCP services; owns local kernels, process registry, SQLite transcript and plugin tasks. This local ownership differs from Arconaut shared-service consumption.

Inspection: Traced actual turn facade/loop context hooks, commit-before-project tools, kernel namespace/destruction, remote fallback, delegation credentials/control, background process routing, SQLite compaction and curator fork.

Limits of this study: Source study only; no acquired code executed. Not every gateway, provider, approval rule or community plugin traced; no universal absence claims.

## Actions

### execute_code / reset

Surface: model tool.

Input: Python code, enabled-tools scope, reset

Result: output/status/kernel execution count and tool results

Lifecycle: persistent owner namespace; timeout/interrupt destroys it; remote spawn failure changes to per-call mode

Authority: model execution guard and configured project/strict/backend scope

Evidence: [namespace](#evidence-namespace), [kernel](#evidence-kernel), [remote](#evidence-remote), [entry](#evidence-entry).

### terminal background / process routing

Surface: model tools.

Input: command/cwd/PTY, notification/watch flags

Result: tracked session handle, output and completion routing

Lifecycle: background survives turn when permitted; finite hosts require polling

Authority: owner/task registry; local or remote environment

Evidence: [background](#evidence-background).

### delegate_task spawn/list/steer/stop

Surface: model tool and operator overlay.

Input: tasks, model/provider override, subagent ID/message

Result: child handles/results, queued steering or interrupt_requested

Lifecycle: sync/parallel/background paths; child timeout can leave worker closing later

Authority: only own spawn tree; bound session/transport checks

Evidence: [credentials](#evidence-credentials), [child](#evidence-child), [child-timeout](#evidence-child-timeout), [steer](#evidence-steer), [control](#evidence-control).

### ContextEngine.select_context / on_turn_complete

Surface: programmable API.

Input: request/history clones, token budget, turn metadata

Result: replacement request projection, completion callback

Lifecycle: per turn, invalid/exception selection keeps original request

Authority: native extension engine access; persisted history clone prevents write-through

Evidence: [context](#evidence-context).

### session_search discover/read/scroll/browse

Surface: model tool.

Input: query/session/anchor/window/profile/role

Result: actual retained messages and links

Lifecycle: standing SQLite history reads; scope differs from raw provider audit

Authority: model registered tool using registry-acquired DB

Evidence: [search](#evidence-search), [search-schema](#evidence-search-schema), [archive](#evidence-archive).

### archive_and_compact

Surface: host API.

Input: summary, covered IDs/watermark, lease holder

Result: new active projection and retained old rows

Lifecycle: atomic publication preserves arrivals, rejects stale lease

Authority: host compaction service and DB API

Evidence: [archive](#evidence-archive), [archive-commit](#evidence-archive-commit).

### Curator review fork

Surface: operator/background program.

Input: configured skills candidates and review prompt

Result: skill mutations, report and scheduler metadata

Lifecycle: background model fork; ledgered skill surface only

Authority: curator excludes terminal; no independent benefit evaluation proved

Evidence: [curator](#evidence-curator).

## Capabilities

### filesystem

**S — Files** (source): Native code and terminal access plus spill files; dedicated file surfaces not exhaustively traced.

Evidence: [entry](#evidence-entry), [background](#evidence-background), [spill](#evidence-spill).

### processes

**I — OS programs** (source): Owner-tracked local/remote process spawn and completion delivery; exact full process-tool control dispatch not studied.

Evidence: [background](#evidence-background).

### code-actions

**I — Code actions** (source): Persistent Python code calls ordinary registered tools through RPC; native context-engine programs.

Evidence: [entry](#evidence-entry), [namespace](#evidence-namespace), [context](#evidence-context).

### persistent-kernel

**I — Kernel** (source): Actual GLOBALS persist across cells, exception retains namespace; timeout/interrupt explicitly destroys state. Remote failures can degrade to per-call execution.

Evidence: [namespace](#evidence-namespace), [kernel](#evidence-kernel), [remote](#evidence-remote), [kernel-test](#evidence-kernel-test).

### standing-database

**I — Standing DB** (source): SQLite persisted/searchable transcript with shared handle borrowing, not general model SQL research DB.

Evidence: [archive](#evidence-archive), [search](#evidence-search), [search-schema](#evidence-search-schema).

### workflow-programming

**S — Workflows** (source): Context engine controls request selection, plugin message injection and child/program orchestration; arbitrary whole-turn replacement not established.

Evidence: [context](#evidence-context), [plugin](#evidence-plugin), [control](#evidence-control).

### multi-model

**I — Models** (source): Per-child model/provider/endpoint credentials resolve distinct from parent.

Evidence: [credentials](#evidence-credentials).

### live-collaboration

**L — Peer chat** (source): Model can steer live owned children at iteration boundary, but no unrestricted peer-room path established in this scope.

Evidence: [steer](#evidence-steer), [control](#evidence-control).

### concurrent-work

**I — Concurrency** (source): Concurrent tool pools, child futures and background process routing.

Evidence: [parallel](#evidence-parallel), [child](#evidence-child), [background](#evidence-background).

### steering-interrupt

**L — Steer/interrupt** (source): Steering queues with missed-delivery accounting; cancellation may abandon still-active tool worker or defer child close. Kernel timeout kills namespace.

Evidence: [steer](#evidence-steer), [abandon](#evidence-abandon), [parallel](#evidence-parallel), [child-timeout](#evidence-child-timeout), [kernel](#evidence-kernel).

### turn-redefinition

**S — Turn program** (source): Custom context engine changes per-request context and completion program; broad native plugins, not proven hot replacement of complete turn semantics.

Evidence: [context](#evidence-context), [plugin](#evidence-plugin).

### compaction

**I — Compaction** (source): Atomic non-destructive projection with coverage/watermark, lease fencing and concurrent-tail cloning.

Evidence: [archive](#evidence-archive), [archive-commit](#evidence-archive-commit), [archive-test](#evidence-archive-test).

### context-repair

**S — Repair** (source): Retained inactive/compacted originals and model search/read support repair; automatic arbitrary corrupted-context repair operation not established.

Evidence: [archive](#evidence-archive), [archive-test](#evidence-archive-test), [search-schema](#evidence-search-schema).

### original-audit

**L — Original audit** (source): Commit-before-project helps transcript durability; transformed/redacted/bounded outputs, capped best-effort spill and truncated curator arguments are not all original IO.

Evidence: [commit](#evidence-commit), [kernel](#evidence-kernel), [spill](#evidence-spill), [curator](#evidence-curator).

### audit-query

**L — Audit query** (source): Actual model message search/read/scroll; raw provider requests/transport/effect streams not established as captured/queryable.

Evidence: [search](#evidence-search), [search-schema](#evidence-search-schema), [commit](#evidence-commit).

### hot-change

**S — Hot change** (source): Plugins own registration cleanup and live injection, code config read per execution; precise all reload settling/staging paths not fully traced.

Evidence: [plugin](#evidence-plugin), [entry](#evidence-entry).

### rebuild-continuity

**L — Rebuild continuity** (source): DB transcript/leases support reattach and component changes, but abandoned tool workers/local-owned kernels do not establish quiescent executable refit/outpost handoff.

Evidence: [turn](#evidence-turn), [parallel](#evidence-parallel), [kernel](#evidence-kernel), [archive](#evidence-archive).

### remote-services

**I — Remote** (source): Configured remote execution consumer with persistent remote kernel/fallback and independent child model endpoint.

Evidence: [remote](#evidence-remote), [credentials](#evidence-credentials), [background](#evidence-background).

### self-improvement

**S — Self-improve** (source): Model curator can consolidate/modify skills through constrained ledgered tools; independent experiment/heldout performance promotion not established.

Evidence: [curator](#evidence-curator).

### complaints

**? — Complaints** (inspection scope): Not established beyond inspected turn, kernel, transcript, plugin and delegation paths.

### authority

**I — Authority** (source): Code execution approved/blocked at guard, project/strict backends; model child control scoped to own tree. Complete bypass/default approval policy outside inspection.

Evidence: [entry](#evidence-entry), [control](#evidence-control).

### evaluation

**S — Evaluation** (source): Read namespace literal tests, real host-death subprocess oracle and exact compaction concurrent-tail/archive oracles; not executed here.

Evidence: [kernel-test](#evidence-kernel-test), [death-test](#evidence-death-test), [archive-test](#evidence-archive-test).

### time-order

**S — Time/order** (source): Monotonic execution deadline/duration distinct from wall-clock DB lease expiry; ordered durable arrivals and explicit inactivity child waits.

Evidence: [parallel](#evidence-parallel), [kernel](#evidence-kernel), [archive](#evidence-archive), [child](#evidence-child).

## Inspected test oracles

- [quarantine/hermes-agent/tests/tools/test_code_kernel.py](../../../quarantine/hermes-agent/tests/tools/test_code_kernel.py): namespace/lifecycle Oracle: Literal retained values and reset/exception behavior; separate real process wait after host death; not run. Read, **not executed**.
- [quarantine/hermes-agent/tests/hermes_state/test_compression_watermark_commit.py](../../../quarantine/hermes-agent/tests/hermes_state/test_compression_watermark_commit.py): compaction concurrent arrivals and originals Oracle: Exact ordered contents/sidecars and inactive original rows, stale lease refusal; not run. Read, **not executed**.

## Useful mechanisms

- Non-destructive compaction preserves original rows and concurrent arrivals.
- Persistent Python kernel is actual cross-cell namespace, with explicit destructive timeout.
- Child stop response explicitly distinguishes request from completion.

## Material limits

- Daemon worker abandonment lets a turn complete with unconfirmed effects.
- Host-owned kernels/processes differ from shared standing-service consumption.
- Selective transcript/result persistence is not original IO audit.

## Arconaut design questions

- Use compaction coverage/watermark and retained originals to model repair datasets.
- Do not infer quiescence from completed turn or cancelled future.
- Make persistent-to-per-call execution degradation explicit to model/operator.

## Evidence

### Evidence turn

[quarantine/hermes-agent/agent/turn_facade.py:141–201](../../../quarantine/hermes-agent/agent/turn_facade.py#L141): Scoped run, finalization and lease refresh thread joins; turn completion does not join all abandoned tool workers.

### Evidence abandon

[quarantine/hermes-agent/agent/tool_executor.py:942–982](../../../quarantine/hermes-agent/agent/tool_executor.py#L942): Sequential interrupt gives3s grace then cancelled result, cancels future and shuts executor without waiting on abandoned worker.

### Evidence parallel

[quarantine/hermes-agent/agent/tool_executor.py:1411–1488](../../../quarantine/hermes-agent/agent/tool_executor.py#L1411): Concurrent tools deadline/interrupt abandon daemon worker pool rather than await all active effects.

### Evidence commit

[quarantine/hermes-agent/agent/tool_executor.py:1110–1153](../../../quarantine/hermes-agent/agent/tool_executor.py#L1110): Tool-result spill/transformation append and DB flush precede completed projection.

### Evidence context

[quarantine/hermes-agent/agent/conversation_loop.py:1280–1346](../../../quarantine/hermes-agent/agent/conversation_loop.py#L1280): Actual optional context-engine selection and completion hooks with copied history and fail-open fallback.

### Evidence namespace

[quarantine/hermes-agent/tools/code_kernel.py:55–73](../../../quarantine/hermes-agent/tools/code_kernel.py#L55): Cell exec uses GLOBALS, capturing output/errors; variables persist across requests.

### Evidence kernel

[quarantine/hermes-agent/tools/code_kernel.py:781–839](../../../quarantine/hermes-agent/tools/code_kernel.py#L781): Timeout/interrupt discard kernel/state, redacted bounded results and spill references; sys.exit explicitly ends kernel.

### Evidence teardown

[quarantine/hermes-agent/tools/code_kernel.py:339–360](../../../quarantine/hermes-agent/tools/code_kernel.py#L339): Kernel teardown signals process tree and removes resources.

### Evidence kill

[quarantine/hermes-agent/tools/code_execution_tool.py:801–821](../../../quarantine/hermes-agent/tools/code_execution_tool.py#L801): TERM tree wait5s then KILL, without second confirmed-death wait in this helper.

### Evidence remote

[quarantine/hermes-agent/tools/code_execution_tool.py:659–697](../../../quarantine/hermes-agent/tools/code_execution_tool.py#L659): Remote persistent kernel first, failures fall open to per-call script execution.

### Evidence entry

[quarantine/hermes-agent/tools/code_execution_tool.py:765–798](../../../quarantine/hermes-agent/tools/code_execution_tool.py#L765): Code guard then local persistent session kernel or remote path; max call/time limits.

### Evidence spill

[quarantine/hermes-agent/tools/code_execution_tool.py:46–101](../../../quarantine/hermes-agent/tools/code_execution_tool.py#L46): Inline byte truncation and best-effort digest spill capped5million characters; not complete original-output audit.

### Evidence background

[quarantine/hermes-agent/tools/terminal_tool_background.py:89–139](../../../quarantine/hermes-agent/tools/terminal_tool_background.py#L89): Tracked local/env process spawn with owner and gateway completion routing; finite hosts report lack of async delivery.

### Evidence credentials

[quarantine/hermes-agent/tools/delegate_tool_config.py:483–504](../../../quarantine/hermes-agent/tools/delegate_tool_config.py#L483): Child model/provider/endpoint override actually resolved, not simply inherited adapter.

### Evidence child

[quarantine/hermes-agent/tools/delegate_tool_child_run.py:850–875](../../../quarantine/hermes-agent/tools/delegate_tool_child_run.py#L850): Thread-backed child run with liveness-aware wait.

### Evidence child-timeout

[quarantine/hermes-agent/tools/delegate_tool_child_run.py:924–941](../../../quarantine/hermes-agent/tools/delegate_tool_child_run.py#L924): Timeout result includes phase/API count/last-event age; still-running worker closes later via callback.

### Evidence steer

[quarantine/hermes-agent/tools/delegate_tool_registry.py:124–157](../../../quarantine/hermes-agent/tools/delegate_tool_registry.py#L124): Live child steering queues without cutting tool; missed boundary explicitly reported, ownership checked.

### Evidence control

[quarantine/hermes-agent/tools/delegate_tool_registry.py:264–301](../../../quarantine/hermes-agent/tools/delegate_tool_registry.py#L264): Model list/steer/stop own spawn tree; stop response says interrupt_requested, not terminated.

### Evidence search

[quarantine/hermes-agent/tools/session_search_tool.py:619–639](../../../quarantine/hermes-agent/tools/session_search_tool.py#L619): Model session query dispatch borrows and releases shared SessionDB handles.

### Evidence search-schema

[quarantine/hermes-agent/tools/session_search_tool.py:652–666](../../../quarantine/hermes-agent/tools/session_search_tool.py#L652): Search/read/scroll/browse actual DB messages, not generated summary; schema contract.

### Evidence archive

[quarantine/hermes-agent/hermes_state_messages.py:1074–1119](../../../quarantine/hermes-agent/hermes_state_messages.py#L1074): Compaction retains soft-archived originals, lease-fenced publish and concurrent arrival watermark.

### Evidence archive-commit

[quarantine/hermes-agent/hermes_state_messages.py:1130–1158](../../../quarantine/hermes-agent/hermes_state_messages.py#L1130): Actual transaction archives old rows, inserts summary and clones concurrent tail; no deletion.

### Evidence plugin

[quarantine/hermes-agent/hermes_cli/plugins.py:1290–1329](../../../quarantine/hermes-agent/hermes_cli/plugins.py#L1290): Owned registration cleanup and plugin-triggered live gateway message injector; remaining global slot profile keying TODO.

### Evidence curator

[quarantine/hermes-agent/agent/curator.py:1085–1150](../../../quarantine/hermes-agent/agent/curator.py#L1085): Actual model fork modifies skills through ledgered surface; metadata tool args truncated, no independent usefulness gate.

### Evidence kernel-test

[quarantine/hermes-agent/tests/tools/test_code_kernel.py:68–97](../../../quarantine/hermes-agent/tests/tools/test_code_kernel.py#L68): Literal cross-cell value/reset/exception namespace tests.

### Evidence death-test

[quarantine/hermes-agent/tests/tools/test_code_kernel.py:106–147](../../../quarantine/hermes-agent/tests/tools/test_code_kernel.py#L106): Real subprocess host kill then kernel.wait oracle, not just registry absence.

### Evidence archive-test

[quarantine/hermes-agent/tests/hermes_state/test_compression_watermark_commit.py:43–137](../../../quarantine/hermes-agent/tests/hermes_state/test_compression_watermark_commit.py#L43): Exact summary+arrival order and sidecars; originals retained inactive.

