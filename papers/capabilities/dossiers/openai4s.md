# openai4s

An independent hybrid native-tool/code-cell agent with ledger reconstruction, compaction archives, generation-bound execution and conservative recovery.

Role: scientific coding agent and owned execution runtime. Runtime: Python, R, TypeScript.

Pinned source: [https://github.com/PKU-YuanGroup/OpenAI4S](https://github.com/PKU-YuanGroup/OpenAI4S); revision/version `9f20ef8d89c195487e41d39942a146159f190c0c`.

Provider-neutral synchronous Python engine, threaded provider/kernel control, JSON-lines persistent workers, SQLite host; TypeScript frontend.

Owns its per-session kernels and database; consumes model APIs, MCP and BYOC/SSH scientific compute. It is not evidence of Anthropic internals.

Inspection: outer engine, native dispatch, kernel RPC/namespace, ledger/compaction/query, child controls, recovery and selected offline test assertions

Limits of this study: Source was inspected but never executed. Flagged auto-review/remote features are documented separately from traced default paths; update apply modules are absent from this pinned tree.

## Actions

### AgentEngine.run

Surface: programmable API.

Input: RunState/messages, max_turns and composed model/context/executor/interceptor/events/cancel/completion ports.

Result: EngineResult with messages, completion, stop_reason, turns and last reply.

Lifecycle: Synchronous state machine; cancelled/max_turns/no-progress are not completion.

Authority: Composition owner chooses ports; policy stays in adapters/dispatcher.

Evidence: [e1](#evidence-e1).

### native tool batch / execute_tool_call

Surface: model tool dispatcher.

Input: Provider-normalized IDs/raw+parsed arguments and session catalog.

Result: One paired observation/error per declaration, success indicator and durable action identity.

Lifecycle: Read-only leading calls may parallelize; mutation barrier retains provider order.

Authority: Schema/precheck then common host permission/egress/audit envelope.

Evidence: [e2](#evidence-e2), [e3](#evidence-e3), [e4](#evidence-e4), [e8](#evidence-e8).

### Python/R cell; host RPC

Surface: model code action.

Input: First complete language cell; host.method arguments mid-Python-cell.

Result: Captured output/error/interrupt plus matching host_response data/error.

Lifecycle: Lazy persistent worker; manager sole read path; same-call correlation and generation bindings.

Authority: Worker execution and host capabilities are separate authorities; lone soft-error object raises.

Evidence: [e6](#evidence-e6), [e7](#evidence-e7).

### host.exec_background / exec_peek / exec_interrupt

Surface: model host capability.

Input: Independent code or exec_id.

Result: Immediate exec_id/status; accumulated bounded stdout; cancellation delivery verdict.

Lifecycle: Independent worker, manager-memory lifetime, not foreground namespace or durable daemon restart continuation.

Authority: Exact kernel object gets idempotent interrupt; no job from guessed display ID.

Evidence: [e9](#evidence-e9), [e10](#evidence-e10).

### send_message / stop_child / continue_child / materialize_child

Surface: delegation API.

Input: child_id, steering message, optional wait or artifact path selections.

Result: Queued message_id/delivery state; stopped/continued child result; materialized version refs.

Lifecycle: Subtree cancellation precedes signal; restart never automatically continues child provider work.

Authority: Parent owns direct child; publication uses recorded artifact identities.

Evidence: [e11](#evidence-e11).

### host.query

Surface: model host capability.

Input: Read-only SQL SELECT/CTE, parameters, optional row limit (default None), statement timeout (default five seconds) and host-derived scope.

Result: Rows or authorizer/timeout errors; omitted/falsy limit fetches all rows rather than imposing an output-row bound.

Lifecycle: Serialized statement on standing SQLite connection; authorizer bracket removed afterward.

Authority: Agent forbidden tables/CTE shadowing/writes refused; scoped views supplied by trusted host.

Evidence: [e15](#evidence-e15).

### create_checkpoint / fork_session / session_status

Surface: model native tools.

Input: Optional expected head; exactly one checkpoint/cell/message source and branch name.

Result: Immutable checkpoint/branch/recovery/pending-approval metadata.

Lifecycle: Append-only checkpoint; view-only branch; activation/replay remains distinct.

Authority: Current root session only; gateway supplies filesystem-aware domain.

Evidence: [e18](#evidence-e18), [e19](#evidence-e19).

### define_dynamic_tool / list_dynamic_tools / promote_dynamic_tool

Surface: model native extension tools.

Input: Name, schemas, Python execute(args), smoke_args and ttl_s; promotion scope.

Result: Scoped version/manifest catalogue or test/approval result (definition service not fully traced).

Lifecycle: One-shot sandbox execution declared; definition TTL and human promotion are explicit contract.

Authority: High-risk schema; promotion explicitly requires human approval; not engine refit.

Evidence: [e21](#evidence-e21).

## Capabilities

### filesystem

**I — Files** (source): Native write_file uses descriptor-anchored staging/publication; kernels and tools access their scoped workspace and versioned artifacts. This does not audit all arbitrary native extension IO.

Evidence: [e20](#evidence-e20), [e4](#evidence-e4).

### processes

**I — OS programs** (source): Persistent kernel execution and separately identified background processes expose interruption; host RPC shares permission/generation context. Arbitrary shell process-tree proof is outside inspected lines.

Evidence: [e6](#evidence-e6), [e7](#evidence-e7), [e9](#evidence-e9), [e10](#evidence-e10).

### code-actions

**I — Code actions** (source): One Python/R code cell is an engine action; native batches have priority and finalize is distinct. Python worker actually executes in persistent namespace; no LSP semantic engine is implied.

Evidence: [e1](#evidence-e1), [e2](#evidence-e2), [e7](#evidence-e7).

### persistent-kernel

**L — Kernel** (source): Foreground worker namespace persists across cells; background execs have separate namespaces. Restart creates a candidate generation and only safe, verified replay can restore selected state; this is owned session computation, not an external shared kernel fabric.

Evidence: [e6](#evidence-e6), [e7](#evidence-e7), [e9](#evidence-e9), [e16](#evidence-e16), [e17](#evidence-e17).

### standing-database

**I — Standing DB** (source): SQLite host persists agent/control/scientific records and exposes read-only SELECT/CTE through host.query with scoped views and a real authorizer. Statement timeout defaults to five seconds; row limit is optional (None or another falsy limit fetches all rows). Generic mutating SQL and cross-user credential reads are not granted.

Evidence: [e15](#evidence-e15).

### workflow-programming

**S — Workflows** (source): Engine ports, code-as-action and native tool metadata compose orchestration. The model can define sandboxed execute(args) tools, but promotion is human-governed and no general live workflow DSL is established here.

Evidence: [e1](#evidence-e1), [e21](#evidence-e21).

### multi-model

**L — Models** (source): Provider selection and child overrides are part of composition; inspected child continuation/control handles are real. Full cross-provider peer collaboration is not established; model adapter can leave cancelled network work detached.

Evidence: [e5](#evidence-e5), [e11](#evidence-e11).

### live-collaboration

**L — Peer chat** (source): Parent can message child via queued message_id, stop subtree, explicitly continue or materialize child artifacts. This is a delegation tree, not a general peer room.

Evidence: [e11](#evidence-e11).

### concurrent-work

**L — Concurrency** (source): Read-only native waves and separate background workers/child futures coexist, with mutations ordered. Background records are in memory; trusted artifact capture can impose serial delegation (documented engine package contract).

Evidence: [e8](#evidence-e8), [e9](#evidence-e9), [e10](#evidence-e10), [e11](#evidence-e11).

### steering-interrupt

**I — Steer/interrupt** (source): Engine checks cancellation before/after execution, provider cancellation stays monotonic, child stop publishes tree cancellation before signals, and background interrupt reports undelivered verdicts.

Evidence: [e1](#evidence-e1), [e5](#evidence-e5), [e10](#evidence-e10), [e11](#evidence-e11).

### turn-redefinition

**S — Turn program** (source): Model/context/executor/interceptor/event/cancellation/completion ports define executable turn composition. This is an owned API seam; no live arbitrary replacement of an underway turn program was established.

Evidence: [e1](#evidence-e1).

### compaction

**I — Compaction** (source): Budgeted head/handoff/tail transformation archives raw summarized slice, metadata and digest; ledger records handoff coverage transactionally. Preserving raw context enables later recovery without pretending the summary is original.

Evidence: [e13](#evidence-e13), [e14](#evidence-e14).

### context-repair

**S — Repair** (source): Raw compaction archive, canonical ledger history and exact session checkpoint/fork cursors provide repair inputs. Kernel recovery is separate conservative replay and may be partial; no automatic full conversation correction oracle is shown.

Evidence: [e13](#evidence-e13), [e14](#evidence-e14), [e16](#evidence-e16), [e17](#evidence-e17), [e18](#evidence-e18).

### original-audit

**L — Original audit** (source): Append-oriented routed action/outcome/terminal records, raw native metadata and compaction archives preserve reconstructable histories. Redaction and bounded output/capture mean these are not every original provider byte/OS effect; mutable projections are separate.

Evidence: [e2](#evidence-e2), [e4](#evidence-e4), [e12](#evidence-e12), [e13](#evidence-e13), [e14](#evidence-e14).

### audit-query

**I — Audit query** (source): Read-only SQL and current-session status/checkpoint/branch projection query durable records; team/scoped authorizer protects forbidden data. No global complete physical-execution view is implied.

Evidence: [e15](#evidence-e15), [e18](#evidence-e18), [e19](#evidence-e19).

### hot-change

**S — Hot change** (source): Per-call schema source and current-session dynamic tool definition/promotion provide callable mutation points with explicit schemas and TTL. Dynamic execute(args) does not replace the running engine.

Evidence: [e5](#evidence-e5), [e21](#evidence-e21).

### rebuild-continuity

**L — Rebuild continuity** (source): Durable history/checkpoint and candidate kernel recovery preserve selected local state; child restore requires explicit continuation. Pinned updater apply/relaunch transaction is absent, so discovery/verification must not be sold as working harness refit.

Evidence: [e11](#evidence-e11), [e16](#evidence-e16), [e17](#evidence-e17), [e22](#evidence-e22).

### remote-services

**L — Remote** (documentation): Official pinned docs describe consumed SSH/BYOC jobs and poll-driven verified harvest; relevant adapter implementation was not fully traced, and future provider list is explicitly excluded.

Evidence: [e24](#evidence-e24).

### self-improvement

**S — Self-improve** (source): Versioned dynamic tool definition is a model-facing extension primitive, not autonomous evaluation/improvement of the harness. Update machinery is deliberately unreachable from a turn.

Evidence: [e21](#evidence-e21), [e22](#evidence-e22).

### complaints

**S — Complaints** (source): Diagnostics support bundles capture redacted operational state/logs for operator issues. They exclude research DB and do not automatically open a model complaint bead/external tracking row.

Evidence: [e25](#evidence-e25).

### authority

**L — Authority** (source): Native and in-kernel calls share permission/action/generation envelope, scoped SQL and current-root orchestration. Human-governed dynamic promotion and hard safety gates are stronger than the desired low-approval Arconaut posture.

Evidence: [e4](#evidence-e4), [e6](#evidence-e6), [e15](#evidence-e15), [e21](#evidence-e21), [e19](#evidence-e19).

### evaluation

**L — Evaluation** (documentation): Documented opt-in reviewer stages freeze evidence/identity and distinguish shadow from gated promotion; storage-only state must not count as executed review. Inspected offline tests exercise concrete routing/redaction/recovery/identity faults, not scientific general validity.

Evidence: [e23](#evidence-e23).

### time-order

**I — Time/order** (source): Ordered native call ordinals/results, transactional compaction coverage and source/candidate generation identities provide explicit local causality. Correlated child messages and exact-owner interruption avoid recycled-event confusion; not a distributed universal clock.

Evidence: [e2](#evidence-e2), [e5](#evidence-e5), [e8](#evidence-e8), [e11](#evidence-e11), [e13](#evidence-e13), [e16](#evidence-e16).

## Inspected test oracles

- [quarantine/openai4s/tests/test_agent_engine.py](../../../quarantine/openai4s/tests/test_agent_engine.py): Tool/code priority and false terminal success Oracle: Scripted model/executor asserts native batch wins over a Python assertion fence, grouped tool call results retained, and plain prose ends max_turns with no completion. Adapter mocks do not prove actual worker execution. Read, **not executed**.
- [quarantine/openai4s/tests/test_action_ledger_runtime.py](../../../quarantine/openai4s/tests/test_action_ledger_runtime.py): Lost raw metadata or secrets in reconstructed history Oracle: Real temporary SQLite store plus synthetic engine events asserts user/native/terminal groups, call-ID pairing and literal secret absence/redacted markers. This is deliberate transformed history, not original-byte audit. Read, **not executed**.
- [quarantine/openai4s/tests/test_compaction_ledger.py](../../../quarantine/openai4s/tests/test_compaction_ledger.py): Durable handoff lost after process-local Store closes Oracle: Real SQLite close/reopen asserts reconstructed compacted history equality. It proves storage/reducer consistency, not that a model summary faithfully retained every fact. Read, **not executed**.
- [quarantine/openai4s/tests/test_execution_coordinator.py](../../../quarantine/openai4s/tests/test_execution_coordinator.py): Wrong owner/session/ticket cancellation and queue corruption Oracle: Exact wrong-session, queued-ticket and wrong-owner requests must not set events; correct request cancels only active ticket and next FIFO ticket is admitted. This does not deliver OS signals. Read, **not executed**.
- [quarantine/openai4s/tests/test_kernel_recovery.py](../../../quarantine/openai4s/tests/test_kernel_recovery.py): Mutable sidecar replacing frozen recovery code Oracle: Known old/new source bytes, manifest and generated bootstrap assert old module value/hash/origin despite changed disk. This independently selected literal oracle tests recovery provenance; not all scientific kernel state. Read, **not executed**.

## Useful mechanisms

- Small provider-neutral Python engine with explicit ports and deterministic action routing.
- Canonical action ledger and raw compaction archive distinguish projection from history.
- Candidate generation recovery rejects false successful restoration and explicit child continuation prevents replaying remote effects.

## Material limits

- Owned per-session kernels/database differ from Arconaut consuming shared computation.
- Redaction, bounded channels and capture coverage still prevent universal original audit.
- Opt-in staged auto-mode contracts and absent updater apply paths must not be equated with default autonomous repair/refit.

## Arconaut design questions

- Can Arconaut make context transformation a recorded event referencing immutable originals while allowing model repair?
- How will provider cancellation distinguish detached but still-billable requests from quiescent refit readiness?
- Which service handles must preserve request identity separately from attempts across explicit continuation?

## Evidence

### Evidence e1

[quarantine/openai4s/openai4s/agent/engine.py:42–145](../../../quarantine/openai4s/openai4s/agent/engine.py#L42): Engine composes model/context/executor/events/cancellation/completion ports; each turn prepares messages, completes provider, routes, executes, appends result and checks terminal state.

### Evidence e2

[quarantine/openai4s/openai4s/agent/actions.py:47–94](../../../quarantine/openai4s/openai4s/agent/actions.py#L47): Code/native/finalize action types retain native wire ID, ordinal, raw arguments, parse errors and provider metadata.

### Evidence e3

[quarantine/openai4s/openai4s/tools/registry.py:657–711](../../../quarantine/openai4s/openai4s/tools/registry.py#L657): Native invocation resolves catalog, validates/prechecks, dispatches and returns bounded observation plus success indicator; errors are observations.

### Evidence e4

[quarantine/openai4s/openai4s/host_dispatch.py:2070–2129](../../../quarantine/openai4s/openai4s/host_dispatch.py#L2070): Permission denial is soft failure; actual handler results are screened and host calls finally log action/permission/resource identities.

### Evidence e5

[quarantine/openai4s/openai4s/agent/runtime.py:462–505](../../../quarantine/openai4s/openai4s/agent/runtime.py#L462): Provider calls select current tool schemas; detached cancellation is monotonic so a recycled session cancel event cannot revive old responses.

### Evidence e6

[quarantine/openai4s/openai4s/kernel/manager.py:928–988](../../../quarantine/openai4s/openai4s/kernel/manager.py#L928): Mid-cell host RPC binds generation/action/sandbox context and returns same-call-ID data/error; a lone error object becomes worker error.

### Evidence e7

[quarantine/openai4s/openai4s/kernel/worker.py:1095–1140](../../../quarantine/openai4s/openai4s/kernel/worker.py#L1095): Cells compile/evaluate/execute in persistent _NS, latch signals before user statements and distinguish delivered interrupt from user-raised KeyboardInterrupt.

### Evidence e8

[quarantine/openai4s/openai4s/agent/control.py:62–145](../../../quarantine/openai4s/openai4s/agent/control.py#L62): Native batches close each declaration and admit only leading positively read-only/non-conflicting calls to parallel waves; mutations are barriers.

### Evidence e9

[quarantine/openai4s/openai4s/kernel/background.py:1–16](../../../quarantine/openai4s/openai4s/kernel/background.py#L1): Background code gets independent subprocess and exec_id, not the foreground namespace.

### Evidence e10

[quarantine/openai4s/openai4s/kernel/background.py:275–320](../../../quarantine/openai4s/openai4s/kernel/background.py#L275): Background jobs live in manager memory and expose peek/idempotent interrupt with actual undelivered-signal diagnosis.

### Evidence e11

[quarantine/openai4s/openai4s/agent/delegation.py:2069–2196](../../../quarantine/openai4s/openai4s/agent/delegation.py#L2069): Child continuation is explicit after restart, publication materialization copies version refs, subtree cancellation publishes before signals, messages are queued with distinct IDs.

### Evidence e12

[quarantine/openai4s/openai4s/agent/ledger.py:338–371](../../../quarantine/openai4s/openai4s/agent/ledger.py#L338): Ledger records user, routed action/outcome and terminal groups from engine events; stored user messages are redacted.

### Evidence e13

[quarantine/openai4s/openai4s/agent/ledger.py:693–729](../../../quarantine/openai4s/openai4s/agent/ledger.py#L693): Compaction handoff plus covered-through-group/archive reference commit as one group/event transaction, leaving prior ledger history retained.

### Evidence e14

[quarantine/openai4s/openai4s/agent/compaction.py:1474–1549](../../../quarantine/openai4s/openai4s/agent/compaction.py#L1474): Context uses head/handoff/tail; raw summarized middle and summary are saved in digest-addressed JSON with branch/recovery metadata and before/after budgets.

### Evidence e15

[quarantine/openai4s/openai4s/store.py:5826–5898](../../../quarantine/openai4s/openai4s/store.py#L5826): host.query exposes read-only SELECT/CTE with real SQLite authorizer, scoped views, default five-second statement timeout and serialized shared connection; output-row limit is optional and omitted/falsy limit fetches all rows.

### Evidence e16

[quarantine/openai4s/openai4s/kernel/recovery.py:996–1040](../../../quarantine/openai4s/openai4s/kernel/recovery.py#L996): Recovery constructs distinct candidate generation and journals each phase with source/candidate/branch identity.

### Evidence e17

[quarantine/openai4s/openai4s/kernel/recovery.py:1080–1141](../../../quarantine/openai4s/openai4s/kernel/recovery.py#L1080): Replay verifies source hashes and safety; skipped/nonreplayable/failed steps yield partial and candidate is shut down rather than promoted.

### Evidence e18

[quarantine/openai4s/openai4s/tools/session.py:12–120](../../../quarantine/openai4s/openai4s/tools/session.py#L12): Native session status/checkpoint/fork schemas expose exact source cursor and compare-and-swap head; model cannot choose arbitrary host root through these tools.

### Evidence e19

[quarantine/openai4s/openai4s/host/session.py:1–7](../../../quarantine/openai4s/openai4s/host/session.py#L1): Model orchestration is bound to current root; filesystem-aware mutations need gateway domain service.

### Evidence e20

[quarantine/openai4s/openai4s/tools/write_file.py:12–77](../../../quarantine/openai4s/openai4s/tools/write_file.py#L12): Native write_file stages and publishes through verified parent descriptor, preserving permissions and refusing partial-overwrite exposure.

### Evidence e21

[quarantine/openai4s/openai4s/tools/dynamic_control.py:25–106](../../../quarantine/openai4s/openai4s/tools/dynamic_control.py#L25): Dynamic tool definition accepts Python execute(args), schemas/smoke arguments/TTL; promotion is explicitly human governed and declared high risk.

### Evidence e22

[quarantine/openai4s/openai4s/update/README.md:5–16](../../../quarantine/openai4s/openai4s/update/README.md#L5): Pinned updater ships discovery/verification only; apply transaction and routes are planned, and model-turn imports are forbidden.

### Evidence e23

[quarantine/openai4s/docs/architecture.md:79–140](../../../quarantine/openai4s/docs/architecture.md#L79): Flagged auto-mode stages distinguish storage-only/shadow review from gated promotion and frozen independent review snapshots; documentation is not default execution proof.

### Evidence e24

[quarantine/openai4s/docs/compute.md:5–80](../../../quarantine/openai4s/docs/compute.md#L5): Documented BYOC/SSH stage/run/harvest consumer has explicit poll-driven harvest and remote timeout/group-cancellation; unsupported provider families remain future.

### Evidence e25

[quarantine/openai4s/openai4s/diagnostics.py:1–20](../../../quarantine/openai4s/openai4s/diagnostics.py#L1): Diagnostics collects redacted bounded operational metadata and logs, deliberately excluding research database; this is a support bundle rather than automatic complaint submission.

