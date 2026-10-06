# forge-workflow

Rust local control plane with genuine persistent native chats, scoped actions, workflow task governance and source-preserving chat context graph; internal engine, task-context and audit/refit boundaries remain distinct.

Role: Rust agent-host and coding-task control plane. Runtime: Rust, TypeScript (web UI).

Pinned source: [https://github.com/ForgeAILab/forge](https://github.com/ForgeAILab/forge); revision/version `cf652b1aa1c50cf0e2fe6d69704323d1b821ed1c`.

Tokio native host consumes separate pinned agent-runtime engine; owns SQLite task/chat workers, external CLI executors/daemon paths, scoped OS processes and web/MCP/REST surfaces.

Forge is governor of local tasks/worktrees/leases/domain state and runtime sessions; consumes providers/external CLIs. Shared independently governed computation is not the core ownership model.

Inspection: Selected source excerpts trace exposed entry/loop/request/action/result/state boundaries and relevant capability mechanisms; listed tests are read as source only.

Limits of this study: No acquired code executed, dependencies installed or live providers invoked. Claims concern this pinned snapshot; untraced catalog tools and dependency internals are not inferred.

## Actions

### AgentSessionBackend.run_turn

Surface: native runtime API.

Input: host session/scope/workspace, provider profile, input/history/system prompt, cancellation

Result: AgentTurnOutput text/usage/context manifest/pending interaction or typed error

Lifecycle: awaits runtime event completion and persistence; actual model/tool engine delegated to agent-runtime

Authority: persisted host-issued scope/identity/workspace, protected credential source

Evidence: [scope](#evidence-scope), [runtime](#evidence-runtime), [drive](#evidence-drive), [output](#evidence-output).

### forge_task_read / forge_task_list / forge_task_write

Surface: native model tools.

Input: scoped path/glob or path/content

Result: bounded contents/list/truncation or written:true

Lifecycle: synchronous filesystem effects inside awaited tool invocation

Authority: task-role scoped FsRead/FsWrite; task planner read only

Evidence: [tools](#evidence-tools), [read](#evidence-read), [write](#evidence-write).

### forge_task_command / forge_task_validate

Surface: native model process tools.

Input: allowlisted program,args; validation role-selected tool

Result: exit status, capped stdout/stderr, optional observation ID

Lifecycle: owned ephemeral process group/timeout; descendants killed on return/drop, no long-running service handle

Authority: worker/reviewer/verification scope; cleared env except PATH

Evidence: [tools](#evidence-tools), [command](#evidence-command), [bounded](#evidence-bounded), [killgroup](#evidence-killgroup).

### forge_scope_read / forge_scope_propose / forge_main_orchestration_read|propose / forge_project_orchestration_read|propose

Surface: native model domain tools.

Input: operation/arguments or payload,dedupe_key,correlation_id

Result: structured code/safe message/retry/permitted recovery, mutations or approval-required envelope

Lifecycle: awaited host command; task.adaptive yields persistent task identities

Authority: captured host identity/scope; allowed operation catalog/policy/version guards

Evidence: [propose](#evidence-propose), [nativeadapt](#evidence-nativeadapt), [structured](#evidence-structured).

### task.adaptive

Surface: operation through native scoped proposal.

Input: split/sequence/replace, source_task_id, expected_task_version/board_revision,rationale

Result: updated/created task IDs and structured outcome

Lifecycle: durable policy/version checked TaskService command

Authority: model has bounded planning agency for bound project, no actor/scope overrides

Evidence: [nativeadapt](#evidence-nativeadapt).

### forge_create_task / forge_create_sub_tasks / forge_add_task_dependency / forge_reorder_sub_tasks / forge_transition_task / forge_cancel_task

Surface: MCP tools.

Input: project/title/description/parent/dependency IDs; task/status/version

Result: task/result JSON or service error

Lifecycle: persisted task lifecycle, ready work picked up by dispatcher; root isolated vs serial children

Authority: MCP admitted context; transition actor recorded system:Mcp, not actual model identity

Evidence: [mcp](#evidence-mcp), [create](#evidence-create), [mcpactor](#evidence-mcpactor), [scan](#evidence-scan).

### WorkflowEngine.transition / move_task / workflow hook registry

Surface: programmable control-plane API.

Input: task/version/target/state graph/hooks/config/actor/authority

Result: transition/guard/cascade results and task mutation

Lifecycle: sequential hooks; dispatch gates and current workflow/version conflict detection

Authority: host workflow definitions and actor permissions; closed compiled action registry

Evidence: [workflow](#evidence-workflow), [guards](#evidence-guards), [hooks](#evidence-hooks), [registry](#evidence-registry).

### forge_send_agent_chat_message / cancel_turn / retry_turn

Surface: MCP/user/service APIs.

Input: chat/content/dedupe; job/version/idempotency

Result: durable message/turn job/status with correlation/source

Lifecycle: queued leased job, bounded retry/cooldown, up to32 active; logical cancellation precedes backend termination

Authority: authorized chat account, frozen admission profile for automatic retries; explicit retry re-admits current authority

Evidence: [mcp](#evidence-mcp), [send](#evidence-send), [canceljob](#evidence-canceljob), [retry](#evidence-retry), [workers](#evidence-workers), [frozen](#evidence-frozen).

### forge_create_agent_handoff

Surface: MCP/user/service API.

Input: Main source chat,target project,bounded content/source revisions/dedupe

Result: provenance-linked handoff and target chat turn identity

Lifecycle: durable target admission; designated Main→Project path

Authority: current owner/Main binding and target project scope

Evidence: [mcp](#evidence-mcp), [handoff](#evidence-handoff).

### cancel / steer

Surface: active native session API.

Input: runtime_session_id; steering text

Result: interrupt/steer request result or missing session

Lifecycle: queues runtime interrupt/input; public call itself does not await full cessation

Authority: configured active backend/session; CLI chat reports steer false

Evidence: [control](#evidence-control), [drive](#evidence-drive), [drop](#evidence-drop), [cli](#evidence-cli).

### SqliteLcmStore.append / commit_leaf / commit_condensation / expand / truncate_from

Surface: supplied context store API.

Input: authorized view,operation ID/fingerprint/revision,range/node/cursor/limit

Result: revision/summary graph or original entries/child nodes; bounded errors

Lifecycle: SQLite immediate/CAS commits retain summary sources; explicit unprotected-tail truncation can delete entries

Authority: issued timeline view/secret classification; engine-facing store, no proven native model repair tool from this wrapper

Evidence: [lcmappend](#evidence-lcmappend), [leaf](#evidence-leaf), [condense](#evidence-condense), [expand](#evidence-expand), [truncate](#evidence-truncate).

### forge_memory_search / forge_memory_get

Surface: MCP retrieval tools.

Input: project/query/layer/token_budget/limit/cursor or item ID

Result: retrieved_context plus pagination

Lifecycle: SQL-backed layered context query through service

Authority: admitted project MCP scope; retrieved content is contextual data

Evidence: [mcp](#evidence-mcp), [memory](#evidence-memory).

### LogReader.read / GET execution logs

Surface: query API.

Input: execution log path/from_sequence/limit

Result: selected JSONL entries/has_more/next_sequence

Lifecycle: per-log pagination, malformed entries skipped; cap limits retained evidence

Authority: control-plane/UI client; no complete original audit claim

Evidence: [query](#evidence-query), [logwriter](#evidence-logwriter), [logsink](#evidence-logsink).

### GracefulShutdown.shutdown

Surface: operator/system lifecycle API.

Input: shutdown signal/service instance

Result: admission stopped, task recovery events/auto-resume records

Lifecycle: executor cancellation then recovery; error logging/default suppression permits incomplete cessation

Authority: Forge control plane owns workers/worktrees/leases; not consumer-only shared service governance

Evidence: [shutdown](#evidence-shutdown), [shellcancel](#evidence-shellcancel), [workers](#evidence-workers).

## Capabilities

### filesystem

**I — Files** (source): Native model scoped read/list/write and Task workspaces are wired; paths/caps/roles constrain access. Inner external agents differ.

Evidence: [tools](#evidence-tools), [read](#evidence-read), [write](#evidence-write).

### processes

**L — OS programs** (source): Native commands are bounded ephemeral process groups with15-minute deadline and cleared env; background descendants deliberately killed. Shell adapter has execution identities/cancel but return is not full reaping barrier.

Evidence: [command](#evidence-command), [bounded](#evidence-bounded), [killgroup](#evidence-killgroup), [shellcancel](#evidence-shellcancel).

### code-actions

**S — Code actions** (source): Model can write source and call allowlisted program with args, enabling scripts/programs; no dedicated unrestricted executable composition/kernel action traced.

Evidence: [write](#evidence-write), [command](#evidence-command).

### persistent-kernel

**? — Kernel** (inspection-limit): No persistent shared computational kernel established in native tool composition, task executors, LCM or chat workers.

### standing-database

**S — Standing DB** (source): Forge owns SQLite domain/context/memory state and exposes layered search; this is application storage, not general independently governed standing DB access.

Evidence: [leaf](#evidence-leaf), [memory](#evidence-memory).

### workflow-programming

**I — Workflows** (source): State graph JSON, role config, sequential guards/hooks and cascades control task execution; actions resolved from closed compiled host registry. Model task.adaptive changes plan topology under policy.

Evidence: [workflow](#evidence-workflow), [hooks](#evidence-hooks), [registry](#evidence-registry), [nativeadapt](#evidence-nativeadapt).

### multi-model

**I — Models** (source): Separate native/provider/CLI profiles and role/chat/execution instances actually run independently selected models, not just one provider selector.

Evidence: [provider](#evidence-provider), [chatnative](#evidence-chatnative), [tasknative](#evidence-tasknative), [cli](#evidence-cli), [workers](#evidence-workers).

### live-collaboration

**L — Peer chat** (source): Persistent Main/Project chats and explicit provenance-linked Main→Project handoffs plus task outcomes supply cooperation; not general IRC peer room. CLI chat does not expose steer/persistent native checkpoint/LCM.

Evidence: [send](#evidence-send), [handoff](#evidence-handoff), [workers](#evidence-workers), [cli](#evidence-cli).

### concurrent-work

**I — Concurrency** (source): Up to32 leased chat jobs in JoinSet, role capacity checks and separate task execution identities; active native generation prohibits simultaneous turns in one runtime session.

Evidence: [workers](#evidence-workers), [dispatch](#evidence-dispatch), [drive](#evidence-drive).

### steering-interrupt

**L — Steer/interrupt** (source): Native active-session steering/interrupt is real. Backend run cancellation can await shutdown, but public cancel/job cancellation and shell cancel do not return proof of all provider/process cessation.

Evidence: [control](#evidence-control), [drive](#evidence-drive), [canceljob](#evidence-canceljob), [shellcancel](#evidence-shellcancel), [drop](#evidence-drop).

### turn-redefinition

**S — Turn program** (source): Host RuntimeBuilder composes per-turn tool catalog/system prompt/model/compaction/security/broker; model domain operations alter task plans, not inner compiled model-loop program.

Evidence: [runtime](#evidence-runtime), [scope](#evidence-scope), [nativeadapt](#evidence-nativeadapt).

### compaction

**L — Compaction** (source): Chat LCM is wired with SQLite original/summary graph and deterministic excerpts; Task uses structural compaction without LCM and reviewers/inquiries are self-contained. Dependency owns coordinator algorithm.

Evidence: [runtime](#evidence-runtime), [contextmode](#evidence-contextmode), [leaf](#evidence-leaf), [excerpt](#evidence-excerpt).

### context-repair

**S — Repair** (source): Authorized paginated expansion returns retained original leaf entries/child nodes with source cursor validation. Native model-facing rehydration/repair commands are in engine dependency/untraced; task structural history differs.

Evidence: [expand](#evidence-expand), [leaf](#evidence-leaf), [contextmode](#evidence-contextmode).

### original-audit

**L — Original audit** (source): LCM leaves retain source entries but explicit tail truncation can delete unprotected history. Visible logs omit/redact raw tool data, silently ignore callback write errors and stop after10MiB; not core everything audit.

Evidence: [leaf](#evidence-leaf), [truncate](#evidence-truncate), [logsink](#evidence-logsink), [logwriter](#evidence-logwriter).

### audit-query

**L — Audit query** (source): Per-execution sequence pagination, domain/chat IDs and memory/context queries exist; malformed lines skipped and capped semantic logs are not unified immutable original audit.

Evidence: [query](#evidence-query), [send](#evidence-send), [memory](#evidence-memory), [logwriter](#evidence-logwriter).

### hot-change

**L — Hot change** (source): Frozen chat admission protects automatic retry config; explicit retry re-admits new profile. Dispatcher loads current project workflow and transitions can reject stale authority. No general defer-until-affected-work-completes hot activation.

Evidence: [frozen](#evidence-frozen), [retry](#evidence-retry), [scan](#evidence-scan), [guards](#evidence-guards).

### rebuild-continuity

**? — Rebuild continuity** (inspection-limit): No executable rebuild/outpost/reinhabitation protocol established in inspected runtime, persistent sessions, graceful shutdown and recovery; persistent task/session retry alone is not refit.

### remote-services

**S — Remote** (source): External CLI/daemon executors and provider HTTP are consumed, but Forge owns local control plane, worktrees, leases, task governance and context DB; external fabric consumer boundary differs from Arconaut.

Evidence: [role](#evidence-role), [cli](#evidence-cli), [tasknative](#evidence-tasknative), [shutdown](#evidence-shutdown).

### self-improvement

**S — Self-improve** (source): Model adaptive split/sequence/replace and file/command work support development under scoped policy. No model-governed experiment/activation/rollback/autodroit runtime traced.

Evidence: [nativeadapt](#evidence-nativeadapt), [write](#evidence-write), [command](#evidence-command).

### complaints

**? — Complaints** (inspection-limit): No everywhere model complaint capturing complete agent state into external issue table established in inspected task worklog/domain/action/sink paths; task errors/outcomes are narrower.

### authority

**L — Authority** (source): Host binds identity/scope/workspace and role permissions, allowlists commands, controls structured mutations/approval and rejects model authority overrides. MCP transition attributes system:Mcp due to absent agent execution identity.

Evidence: [scope](#evidence-scope), [tools](#evidence-tools), [command](#evidence-command), [propose](#evidence-propose), [structured](#evidence-structured), [mcpactor](#evidence-mcpactor).

### evaluation

**S — Evaluation** (source): Source-read tests attack real SQLite source expansion/CAS and real Unix process-group liveness, plus deterministic retry-input and stale workflow mutation invariants; not executed here.

Evidence: [leaf](#evidence-leaf), [expand](#evidence-expand), [bounded](#evidence-bounded), [guards](#evidence-guards).

### time-order

**L — Time/order** (source): DB chat sequence/job leases/version/source/causation and LCM revisions provide ordering; JSONL per-writer sequence/UTC time capped/skippable and not universal durable causal audit.

Evidence: [send](#evidence-send), [workers](#evidence-workers), [lease](#evidence-lease), [lcmappend](#evidence-lcmappend), [logwriter](#evidence-logwriter), [query](#evidence-query).

## Inspected test oracles

- [quarantine/forge-workflow/crates/agent-host/tests/lcm_store.rs](../../../quarantine/forge-workflow/crates/agent-host/tests/lcm_store.rs): LCM authorization/idempotency/source expansion/reopen adapter Oracle: In-memory SQLite migration+real store append/leaf assert revision/replay/content/expansion count and foreign-view rejection. Reopened adapter shares same in-memory DB, not OS crash/disk durability; stale runtime revision string hardcoded. Read, **not executed**.
- [quarantine/forge-workflow/crates/services/src/workflow/engine/tests.rs](../../../quarantine/forge-workflow/crates/services/src/workflow/engine/tests.rs): Stale project workflow authority at transition boundary Oracle: Real SQLite fixture changes project workflow before transition; expects VersionConflict and unchanged task/status/version, no transition log or running execution. Does not test all changed-running-work activation semantics. Read, **not executed**.
- [quarantine/forge-workflow/crates/agent-host/src/typed_tools.rs](../../../quarantine/forge-workflow/crates/agent-host/src/typed_tools.rs): Bounded command descendant termination on normal exit/timeout/dropped future Oracle: Unix shell spawns background child and oracle probes PID existence with kill(signal None), asserts capped command exit/error and eventual child gone; real OS process invariant for that group, not provider/network quiescence. Read, **not executed**.
- [quarantine/forge-workflow/crates/agent-host/src/native.rs](../../../quarantine/forge-workflow/crates/agent-host/src/native.rs): Retry after unanswered model tool round and answered duplicate input Oracle: Artificial Message arrays assert continuation substituted only for unanswered matching user input; no external-effect replay guarantee. Read, **not executed**.

## Useful mechanisms

- Host-issued scope/tool composition and version checks separate model proposal input from identity authority.
- Chat LCM preserves original entries behind summary nodes and validates paged expansion sources.
- Real process-group liveness and stale SQLite workflow tests attack direct fault classes.

## Material limits

- Native model request encoding/inner tool iteration/LCM coordinator live in separate git dependency, not independently traced here.
- Task structural context compaction is distinct from chat LCM; CLI chat capabilities narrower.
- Turn callback log failures ignored; previews/redaction/caps and explicit LCM truncation preclude full original audit.
- MCP mutation attribution can be system identity rather than invoking model; cancellation request and cessation differ.

## Arconaut design questions

- Can original-source expansion become a first-class model repair operation across all task/chat modes?
- Which immutable original ledger should survive capped/redacted displays and tail truncation?
- How can a consumer harness use this orchestration fabric without inheriting its worktree/service governance?
- What version/defer boundary preserves model agency while preventing changed programs from activating mid-work?

## Evidence

### Evidence role

[quarantine/forge-workflow/README.md:16–46](../../../quarantine/forge-workflow/README.md#L16): Forge describes a local control plane for persistent assistants, isolated coding tasks, serial shared-worktree children and CI/review delivery gates.

### Evidence transitionstatus

[quarantine/forge-workflow/README.md:131–140](../../../quarantine/forge-workflow/README.md#L131): README explicitly marks Main/Project Agent Chat replacement model as being implemented; broad feature claims require wiring checks.

### Evidence dependency

[quarantine/forge-workflow/Cargo.toml:1–35](../../../quarantine/forge-workflow/Cargo.toml#L1): Rust workspace includes agent-host, executors/services; native engine is separate pinned git agent-runtime dependency ca6c17e, not inline source.

### Evidence provider

[quarantine/forge-workflow/crates/agent-host/src/native.rs:152–217](../../../quarantine/forge-workflow/crates/agent-host/src/native.rs#L152): Native provider construction resolves protected credential source and selects concrete runtime provider implementation; dependency owns request encoding/inner loop.

### Evidence scope

[quarantine/forge-workflow/crates/agent-host/src/native.rs:420–501](../../../quarantine/forge-workflow/crates/agent-host/src/native.rs#L420): run_turn verifies persisted scope/workspace binding and composes scoped tools; result observer retains per-call structured summaries.

### Evidence runtime

[quarantine/forge-workflow/crates/agent-host/src/native.rs:503–596](../../../quarantine/forge-workflow/crates/agent-host/src/native.rs#L503): RuntimeBuilder installs model limits/provider/workspace/broker, persistent stores and scope-specific LCM/structural compaction, system prompt and reasoning, then starts session.

### Evidence drive

[quarantine/forge-workflow/crates/agent-host/src/native.rs:601–706](../../../quarantine/forge-workflow/crates/agent-host/src/native.rs#L601): Active generation prevents concurrent turn per runtime session; send starts turn, events forward selected deltas/tools, cancellation branch awaits shutdown, completion/persistence precedes registry finish.

### Evidence output

[quarantine/forge-workflow/crates/agent-host/src/native.rs:715–831](../../../quarantine/forge-workflow/crates/agent-host/src/native.rs#L715): Return reconstructs latest assistant text, context-manifest link and per-attempt usage; failures/cancel retain usage reports but do not return successful output.

### Evidence control

[quarantine/forge-workflow/crates/agent-host/src/native.rs:835–860](../../../quarantine/forge-workflow/crates/agent-host/src/native.rs#L835): cancel merely requests interrupt_current_turn and steer calls steer_current_turn on active session; methods do not await caller-independent quiescence.

### Evidence drop

[quarantine/forge-workflow/crates/agent-host/src/native.rs:63–111](../../../quarantine/forge-workflow/crates/agent-host/src/native.rs#L63): Dropping active backend future requests session cancel and spawns shutdown/registry cleanup; cleanup is asynchronous, not drop-time quiescence.

### Evidence contextmode

[quarantine/forge-workflow/crates/agent-host/src/native.rs:296–387](../../../quarantine/forge-workflow/crates/agent-host/src/native.rs#L296): Chat uses LCM/persistent session; Task worker/planner use persistent structurally compacted history without LCM; reviewer/inquiry limits differ. Retry-aware input avoids duplicate unanswered input.

### Evidence tools

[quarantine/forge-workflow/crates/agent-host/src/typed_tools.rs:439–484](../../../quarantine/forge-workflow/crates/agent-host/src/typed_tools.rs#L439): Task role and allowed permissions determine actual read/list/write/command/validate/web tools; planner lacks writes/processes.

### Evidence read

[quarantine/forge-workflow/crates/agent-host/src/typed_tools.rs:1952–2030](../../../quarantine/forge-workflow/crates/agent-host/src/typed_tools.rs#L1952): forge_task_read prepare/invoke revalidate scoped path and return at most128KiB lossy UTF8 with truncation flag.

### Evidence write

[quarantine/forge-workflow/crates/agent-host/src/typed_tools.rs:2120–2180](../../../quarantine/forge-workflow/crates/agent-host/src/typed_tools.rs#L2120): forge_task_write scopes path, caps content1MiB and writes after second path check.

### Evidence command

[quarantine/forge-workflow/crates/agent-host/src/typed_tools.rs:2220–2321](../../../quarantine/forge-workflow/crates/agent-host/src/typed_tools.rs#L2220): forge_task_command accepts allowlisted program/args; prepare requests ProcessSpawn, invocation returns exit/output and optional persisted command-observation ID.

### Evidence bounded

[quarantine/forge-workflow/crates/agent-host/src/typed_tools.rs:2400–2512](../../../quarantine/forge-workflow/crates/agent-host/src/typed_tools.rs#L2400): Command runs with cleared env except PATH,15-minute timeout, capped drained stdout/stderr and own process group killed on completion/drop; child wait/status drives completion.

### Evidence killgroup

[quarantine/forge-workflow/crates/agent-host/src/typed_tools.rs:2534–2567](../../../quarantine/forge-workflow/crates/agent-host/src/typed_tools.rs#L2534): Unix ProcessGroupGuard drop sends SIGKILL ignoring error; mechanism kills group but does not synchronously reap every descendant.

### Evidence propose

[quarantine/forge-workflow/crates/agent-host/src/typed_tools.rs:1870–1944](../../../quarantine/forge-workflow/crates/agent-host/src/typed_tools.rs#L1870): Scoped proposal checks allowed operation, dedupe/correlation and rejects model authority overrides; invoke passes captured identity/scope/session to host provider.

### Evidence nativeadapt

[quarantine/forge-workflow/crates/services/src/native_tools.rs:3019–3089](../../../quarantine/forge-workflow/crates/services/src/native_tools.rs#L3019): task.adaptive is wired to policy-checked TaskService split/sequence/replace with expected task/board versions and rationale, server-derived actor and idempotency.

### Evidence structured

[quarantine/forge-workflow/crates/services/src/native_tools.rs:4109–4203](../../../quarantine/forge-workflow/crates/services/src/native_tools.rs#L4109): Forge provider returns structured success/failure/approval requirements; payload validation gives correct-input retry instructions.

### Evidence send

[quarantine/forge-workflow/crates/services/src/agent_chat_service.rs:429–527](../../../quarantine/forge-workflow/crates/services/src/agent_chat_service.rs#L429): Chat send guards content and admits durable message+turn job with source/correlation; server resolves responder/profile/policy and allocates message sequence.

### Evidence canceljob

[quarantine/forge-workflow/crates/services/src/agent_chat_service.rs:530–580](../../../quarantine/forge-workflow/crates/services/src/agent_chat_service.rs#L530): Chat cancellation uses expected job version and idempotency in DB transaction; not direct provider cancellation in this method.

### Evidence retry

[quarantine/forge-workflow/crates/services/src/agent_chat_service.rs:583–594](../../../quarantine/forge-workflow/crates/services/src/agent_chat_service.rs#L583): Explicit terminal retry keeps trigger message but re-resolves new authority; differs from automatic retries preserving frozen admission.

### Evidence handoff

[quarantine/forge-workflow/crates/services/src/agent_chat_service.rs:973–1052](../../../quarantine/forge-workflow/crates/services/src/agent_chat_service.rs#L973): Main→Project handoff validates current bindings/target, guards bounded content and captures source revisions/causation for admitted target turn.

### Evidence workers

[quarantine/forge-workflow/crates/services/src/agent_chat_turn_worker.rs:3530–3605](../../../quarantine/forge-workflow/crates/services/src/agent_chat_turn_worker.rs#L3530): Chat worker claims DB jobs into JoinSet up to32 active, cancellation on shutdown joins all tasks; claim uses immediate transaction and provider cooldown gating.

### Evidence frozen

[quarantine/forge-workflow/crates/services/src/agent_chat_turn_worker.rs:3926–3976](../../../quarantine/forge-workflow/crates/services/src/agent_chat_turn_worker.rs#L3926): Automatic retry uses same frozen responder/profile/policy and validates authority/provider availability before durable usage admission.

### Evidence lease

[quarantine/forge-workflow/crates/services/src/agent_chat_turn_worker.rs:4488–4517](../../../quarantine/forge-workflow/crates/services/src/agent_chat_turn_worker.rs#L4488): Owner heartbeat renews leased job/version; zero-row/error cancels turn.

### Evidence chatnative

[quarantine/forge-workflow/crates/services/src/agent_chat_turn_worker.rs:2815–2892](../../../quarantine/forge-workflow/crates/services/src/agent_chat_turn_worker.rs#L2815): Main and Project chat native turns resolve separate profile/model/session/credential and scratch/verification workspace, command allowlist then backend.run_turn.

### Evidence cli

[quarantine/forge-workflow/crates/services/src/agent_chat_turn_worker.rs:602–694](../../../quarantine/forge-workflow/crates/services/src/agent_chat_turn_worker.rs#L602): CLI chat backend reports no native persistent checkpoints/LCM/steer; creates per-job sandbox, runs adapter and invokes cancel on token cancellation.

### Evidence tasknative

[quarantine/forge-workflow/crates/services/src/embedded_task_executor.rs:391–463](../../../quarantine/forge-workflow/crates/services/src/embedded_task_executor.rs#L391): Task execution validates provider-call admission, applies project command allowlist and issues host-scoped native turn using frozen execution provider/model config.

### Evidence scan

[quarantine/forge-workflow/crates/services/src/task_dispatcher.rs:101–131](../../../quarantine/forge-workflow/crates/services/src/task_dispatcher.rs#L101): Dispatcher skips paused projects, resolves current workflow and recovers in-flight work before starting ready tasks.

### Evidence workflow

[quarantine/forge-workflow/crates/services/src/workflow/engine/mod.rs:992–1034](../../../quarantine/forge-workflow/crates/services/src/workflow/engine/mod.rs#L992): Workflow definitions deserialize JSON with default fallback; root and child/actor distinctions resolve different workflow programs.

### Evidence guards

[quarantine/forge-workflow/crates/services/src/workflow/engine/mod.rs:1098–1212](../../../quarantine/forge-workflow/crates/services/src/workflow/engine/mod.rs#L1098): Transitions enforce declared edges/system actors, user override rules and optional expected project workflow/version, then load merged state config.

### Evidence hooks

[quarantine/forge-workflow/crates/services/src/workflow/engine/mod.rs:1337–1407](../../../quarantine/forge-workflow/crates/services/src/workflow/engine/mod.rs#L1337): Before-exit hook actions actually execute sequentially, refresh task state and block transition on configured guard failure.

### Evidence registry

[quarantine/forge-workflow/crates/services/src/workflow/registry.rs:18–96](../../../quarantine/forge-workflow/crates/services/src/workflow/registry.rs#L18): Workflow actions are closed host registry including run_ci_steps/run_merge/dispatch_role_agent/dependency_gate/retry/cleanup cascades; unknown action errors.

### Evidence dispatch

[quarantine/forge-workflow/crates/services/src/workflow/actions/dispatch.rs:305–355](../../../quarantine/forge-workflow/crates/services/src/workflow/actions/dispatch.rs#L305): Role dispatch checks agent pause/capacity and current reviewer/workflow authority before admission; inner launch continuation beyond excerpt not independently audited.

### Evidence lcmappend

[quarantine/forge-workflow/crates/agent-host/src/lcm.rs:813–903](../../../quarantine/forge-workflow/crates/agent-host/src/lcm.rs#L813): LCM append authorizes view, validates timeline/fingerprint/idempotency and current revision/tail sequence before DB commit.

### Evidence leaf

[quarantine/forge-workflow/crates/db/src/sqlite/lcm.rs:395–491](../../../quarantine/forge-workflow/crates/db/src/sqlite/lcm.rs#L395): LCM leaf commit immediate transaction checks CAS, exact original entry sequence and overlap, inserts summary node/operation without deleting original entries.

### Evidence expand

[quarantine/forge-workflow/crates/agent-host/src/lcm.rs:742–807](../../../quarantine/forge-workflow/crates/agent-host/src/lcm.rs#L742): LCM expansion authorizes view, bounds page size and verifies node/source fingerprint cursor, returning original leaf entries or child nodes.

### Evidence condense

[quarantine/forge-workflow/crates/agent-host/src/lcm.rs:1053–1121](../../../quarantine/forge-workflow/crates/agent-host/src/lcm.rs#L1053): Condensation validates active child IDs/order/source fingerprint/classification, refuses secret sources and checks revision metadata.

### Evidence truncate

[quarantine/forge-workflow/crates/db/src/sqlite/lcm.rs:348–393](../../../quarantine/forge-workflow/crates/db/src/sqlite/lcm.rs#L348): LCM truncation refuses ranges intersecting any summary node but deletes unprotected tail entries; store is not universally immutable originals.

### Evidence excerpt

[quarantine/forge-workflow/crates/agent-host/src/lcm.rs:280–310](../../../quarantine/forge-workflow/crates/agent-host/src/lcm.rs#L280): Wired deterministic summary model reduces text to bounded per-message excerpts; not semantic model summarization or repair of lost task structural history.

### Evidence logsink

[quarantine/forge-workflow/crates/services/src/turn_log_sink.rs:123–175](../../../quarantine/forge-workflow/crates/services/src/turn_log_sink.rs#L123): Turn sink records selected readable deltas and credential-masked tool previews/summaries; write errors ignored at event callbacks.

### Evidence logwriter

[quarantine/forge-workflow/crates/executors/src/log_writer.rs:51–118](../../../quarantine/forge-workflow/crates/executors/src/log_writer.rs#L51): JSONL per-writer sequence, flush but no fsync, output cap then truncation/no further entries; existing-file sequence resumes but byte count resets at construction.

### Evidence query

[quarantine/forge-workflow/crates/executors/src/log_reader.rs:13–57](../../../quarantine/forge-workflow/crates/executors/src/log_reader.rs#L13): Log reader paginates from sequence, skips malformed lines and advances cursor from last retained record.

### Evidence mcp

[quarantine/forge-workflow/crates/mcp-server/src/tools/mod.rs:13–90](../../../quarantine/forge-workflow/crates/mcp-server/src/tools/mod.rs#L13): Exposed MCP dispatch includes tasks/dependencies/memory/agents/projects/chat/handoff APIs with representative handlers traced.

### Evidence create

[quarantine/forge-workflow/crates/mcp-server/src/tools/handlers.rs:56–137](../../../quarantine/forge-workflow/crates/mcp-server/src/tools/handlers.rs#L56): forge_create_task validates project/title/parent and calls create_task_with_dependencies, returning task API data after service mutation.

### Evidence memory

[quarantine/forge-workflow/crates/mcp-server/src/tools/handlers.rs:447–512](../../../quarantine/forge-workflow/crates/mcp-server/src/tools/handlers.rs#L447): forge_memory_search enforces admitted project context and returns layered retrieved context/cursor, not general DB kernel.

### Evidence mcpactor

[quarantine/forge-workflow/crates/mcp-server/src/tools/handlers.rs:583–668](../../../quarantine/forge-workflow/crates/mcp-server/src/tools/handlers.rs#L583): MCP cancellation/queries/updates/transitions call service/DB; transition explicitly attributes caller as system:Mcp because agent execution identity is unavailable.

### Evidence shutdown

[quarantine/forge-workflow/crates/services/src/shutdown.rs:45–113](../../../quarantine/forge-workflow/crates/services/src/shutdown.rs#L45): Graceful shutdown stops admission, cancels active executors then records recovery/auto-resume; cancellation errors warn and recovery write failure defaults away.

### Evidence shellcancel

[quarantine/forge-workflow/crates/executors/src/shell.rs:155–204](../../../quarantine/forge-workflow/crates/executors/src/shell.rs#L155): Shell adapter sends TERM/grace and KILL group, starts direct child kill if needed then returns without final await/reaping barrier.

### Evidence expired

[quarantine/forge-workflow/crates/services/src/agent_chat_turn_worker.rs:3680–3719](../../../quarantine/forge-workflow/crates/services/src/agent_chat_turn_worker.rs#L3680): Lease expiry records prior started/pending provider invocation as unreplayable coverage gap; not replay of the prior provider result.

