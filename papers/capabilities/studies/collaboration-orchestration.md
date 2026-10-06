# Collaboration and orchestration: action and capability study

## Written study plan

Study each pinned reference sequentially and save its completed evidence row before
starting the next: Gas City, Gas Town, Claudex Pattern, Chat System, OpenAgents SDK,
MetaGPT, Agent Orchestrator Rust, AgentChat, AgentPool, Forge Workflow, MCP Agent
Mail, NTM and Station. Coordination-services ownership was subsequently delegated
to the science lane; see [coordination-services.md](coordination-services.md) for
MCP Agent Mail, NTM and Station. This study owns the first ten. Establish whether the reference supplies a model loop,
shared room, mailbox, process supervisor, workflow runtime or infrastructure service.
Trace actual dispatch, identities, results, continuation, interruption and state
ownership; follow representative workflow, context, original audit, reload/refit,
complaint and test mechanisms. A provider selector, observed terminal text, saved
message, process restart or generated prompt is not automatically peer collaboration,
comprehensive audit, repaired context or executable reinhabitation.

Rows follow the common 23-axis contract and name native operations with argument,
result, work-handle, lifecycle and authority evidence. Source paths enumerate actual
read excerpts; inspected tests are described by their real assertions and mocking
boundaries, never executed. Acquired sources stay unchanged. No dependency install,
reference execution, provider credentials, paid requests or adoption occurs. Arconaut
remains the consumer of future shared fabric; work fencing, original context repair,
model agency and quiescent refit are comparison requirements, not inherited claims.

## Progress

The interactive-client cohort is complete in its separate study. Collaboration and
orchestration study has begun; rows are saved incrementally below.

## Gas City

Gas City owns a city control plane: external runtime sessions, durable work and graph stores, threaded mail, nudges and reconciliation. The traced main path is CLI → CityRuntime.run → configured runtime Start; actual model requests and coding tools live in the external agent. Sling returns durable workflow identity after promoting and linking the graph root, before work completes. Formula mutation and routing are real programming surfaces, but do not redefine a child harness model turn.

Message persistence, optional notification, queue receipt, terminal submission and model observation have different outcomes. Provider nudge waits only best-effort then submits on timeout; managed wait-idle can queue. C-c is terminal input, not a quiescence acknowledgment. Mail survives recipient downtime and archive retains body. These distinctions are useful for a live collaboration fabric.

Reload acceptance is independent of expensive reconciliation; application occurs on a city tick, not all affected child turn/workflow completion. Live storage swaps are refused. Soft reload accepts desired fingerprints without rebuilding running processes. Handoff packages context mail and can kill a restartable process; drain acknowledgment stamps worker incarnation and releases held claims. None establishes a compiled harness refit with provider and active programs proven absent.

Sequenced JSONL events and raw provider transcript reads are valuable evidence surfaces but not complete audit. Record drops can warn only, acknowledged writes are not fsynced, retention can prune and readers skip malformed lines. The graph projector avoids permanently suppressing a definition after an unacknowledged append by tolerating repeat emission. Read-only source tests use fake lifecycle environments and a real temporary filesystem recorder; they were not run and do not prove live terminal delivery or crash persistence.

## Gas Town

Gas Town is a role/workspace orchestrator around external harnesses. Start resolves agent presets, settings, command and environment before tmux creation. It owns rigs/worktrees, work/mail state and role daemons; it is closer to external fabric than an Arconaut consumer. The built-in Claude and Gemini presets explicitly bypass routine approvals. Provider requests and actual coding tools remain in those harnesses.

Formula sling and step readiness are wired, but handleParallelSteps is a revealing counterexample: it marks steps in progress, launches goroutines that only print readiness, and continues the first step. Other tasks require other agents/manual action. Marketing or introductory comments must not turn this into executed parallel work. Mail does persist before asynchronous notification. Delivery labels stamp recipient/time before the final ack, while escalation sets RuntimeNotified=true after Router.Send even though the asynchronous notification error is ignored.

Handoff --cycle refuses respawn after context-mail persistence failure, then starts native --continue and clears scrollback. Auto handoff only warns on mail failure and can still leave a continuation marker. Collected state is bounded and omits failed subprocess views. Seance launches a fork/resume of a predecessor as a new external model process; that is an interesting information-recovery operation, not direct repair of the present context. No active-program/provider emptiness gate or compiled executable replacement was traced.

Operational config is reread at daemon use sites and on heartbeat reset, without affected-child-turn fencing. Timestamped operational JSONL and opt-in Claude file watchers supply selective evidence; no complete original audit is implied. The inspected tests assert command strings, label state and formula readiness, and were not run.

## Claudex Pattern

This is an unversioned local historical Python runner, not a canonical upstream engine. The complete script and example brief/config files were read. A commander owns editable roster, briefs, shared doctrine, workdirs, model/effort and permission flags. Headless dispatch is a blocking list comprehension over external Claude invocations, so all workers run sequentially. Independent windowed tmux launches can coexist, but each launch kills a prior same-named session and records only launcher status.

The channel is capture-pane and direct literal send-keys plus Enter. It has no queue/idle check or consumption acknowledgment, and ignores the text-send error while returning Enter status. Captured prompt/stdout/stderr/meta are useful small artifacts; windowed output is not comprehensively captured, run identifiers have second precision and reused directories overwrite files. Config rereads affect new invocations, not current child turns. No native compaction/refit/test suite was present. This offers a useful minimal commander interface to improve, not evidence of a complete live collaboration runtime.

## Chat System

Rust chat transports are supporting infrastructure, not model agents. Tagged configuration builds an adapter; generic calls forward to it. The manager awaits backends serially and logs/swallow receive errors. Optional search/edit/reaction/presence/membership methods default to empty or successful no-op, so a unified method surface does not establish uniform capability. Group activation/isolation keys and streaming flush plans are exported helpers; callers must enforce policy, store context and perform sends.

IRC illustrates why implementation tracing matters: the client TLS flag still creates a Plain TCP connection, explicitly described as intent. Registration timeout/EOF can still set connected=true; send_raw can return success without a connection. The built-in server calls the handler and replies on that client connection, without a shared room router. Listener shutdown stops accept, while child connection tasks neither clone the alive sender nor receive shutdown, so server completion does not establish callback quiescence.

The normalized messages and synthetic millisecond IDs do not provide original audit or durable causal order. Real-localhost source tests assert callback text/both ports, but use sleeps and abort teardown; manager tests use Console queues and check counts/results. No tests were executed, and none of the read assertions challenges TLS encryption or active-child shutdown. These are design prompts for an owned transport fabric, not library adoption.

## OpenAgents SDK

The pinned SDK offers real networked agents and channel/direct conversations, plus a separate workspace-adapter path that launches native coding CLIs. Worker custom event handlers precede configured model reactions; the SDK model loop renders context templates, performs bounded provider iterations and awaits named tools serially. Project templates, state and remote caches provide useful orchestration/data primitives, but belong to an independently governed network fabric.

Two implementation distinctions are decisive. The advertised ONM staged pipeline is a separate future event envelope, while the running SDK uses first-intercept mods. More seriously, the logger wired into orchestration constructs an LLM entry and returns an ID without writing or sending it. File logger tests exercise a different class. Message archives themselves batch, overwrite and expire rather than preserving authoritative originals.

Workspace adapters serialize work per channel and run different channels concurrently. Their raw cursor advances before dispatch. Claude native sessions have per-channel resume IDs and process-group stop; stop drops queues. The direct-LLM adapter instead shares one history list across all channels and sends no tool definitions, despite tool-oriented prompt material. Mod load/unload mutates the network immediately and network restart governs shared services. Neither gives Arconaut's deferred change boundary or outpost refit.

## MetaGPT

MetaGPT exposes both workflow-oriented teams and executable notebook agents. Team rounds gather active roles concurrently; messages route to role inboxes, and RoleZero re-observes between command batches. Private role/action providers permit heterogeneous models. A team leader can issue named peer work and wait, while model commands edit plans, files, browser state and terminal work. This is real resident collaboration with round scheduling; routing still returns success when no recipient matches.

DataInterpreter writes Python cells, observes outputs and retries failures. The notebook keeps variables between cells, interrupts on timeout and resets on dead kernel; normal plan completion terminates it. Computation is owned locally, unlike Arconaut's shared-service consumer requirement. Terminal retains shell state but its daemon path starts a reader without the daemon flag needed to fill the output queue; it also loops at EOF and provides no exit-code/job receipt.

Team exception recovery serializes selected state and removes the latest observed message to make it re-observable. It does not preserve live kernel/shell handles or deduplicate partially completed effects. Provider compression cuts earliest/latest messages or characters; debug memory, masked logs and overwritten notebooks are not an original audit. Read kernel tests use actual cross-cell assertions; serialization tests patch the writer and cannot prove executable continuity.

## Agent Orchestrator (Rust)

This is an external-agent control plane, not a Rust implementation of every model turn. YAML workflows select native/shell drivers; item segments really execute bounded parallel work. A small authenticated MCP catalog lets models run allowlisted tests, mark/generate items, record metrics and create tickets from failing evidence. Terminal takeover has writer fencing, process identity and idempotency keys, while acceptance means FIFO write rather than model consumption. No peer room is established by these task-coordination tools.

Its actual self-restart path builds the daemon, tests --help, snapshots/hashes the binary, persists restart_pending and waits for other registered tasks before same-PID exec. This deserves study for Arconaut, with a specific warning: Claude result is converted to Finished before output persistence and OS reaping; scheduler treats it as phase end. Timeout/pause signal cancellation then return. Daemon supervisor drain timeout is logged but does not prevent exec. A native answer, requested cancellation and proven process quiescence are distinct boundaries.

Handoff plans explicitly state logical replay, effect class, state version and no workspace rollback. Audit query is substantial at the control-plane level, but sanitized streams, ignored coordination-event failures and retention prevent full-original claims. Read driver tests use real OS child status; restart tests use fake compiler/help programs and stop at readiness, not full exec continuity.

## Agentchat

Agentchat is an actual WebSocket agent room with model MCP send/listen, server-owned channel membership/replay and optional persistent connection daemon. It runs no model provider; independent participants can use different harnesses. The pinned server explicitly rejects DMs and file transfers, despite client tools and README advertising them. Work proposals are signed marketplace state, not automatic execution of the proposed task.

Protocol ergonomics are weakened at the MCP boundary. The direct handler and listen shape omit message/thread IDs, yet floor claim requires msg_id. Claim reports successful socket send rather than server grant; YIELD is advisory. Listen keeps newest capped messages, deduplicates by timestamp/sender/content prefix and advances one shared timestamp cursor, so older, same-timestamp or other-channel data can be skipped. The daemon clears whole outbox after logging individual failures.

JSONL audit captures selected event metadata through fire-and-forget writes, and channel content is deliberately absent. Inbox retention is a ring buffer, with different writers using different locking/replace rules. These records cannot satisfy original model audit or context repair. Localhost tests assert exact channel content; floor tests and receipt tests validate narrower primitives, not provider continuity or lossless delivery.

## AgentPool

AgentPool is a Python resident programmable message graph. The native branch creates a fresh PydanticAI Agent for each run, registers current tools and iterates model/tool nodes; ACP instead owns a child stdio client and delegates the inner turn. Independently configured agents connect through run/context/forward edges, parallel teams, queued sequential chains and model-visible task/worker tools. Flow events occur before target work, so event delivery is not completed execution. Team cancellation cancels outstanding tasks without awaiting them, and adapter interrupts do not establish provider/process quiescence.

Its especially useful model-agency lead is execute_introspection: model Python sees ctx, run_ctx, me and the pool, and save/state retains arbitrary Python objects between calls. This permits substantial live reconfiguration. Ordinary execute_tool instead gets a fresh namespace of current original tool callables, awaits async main and saves script/metadata to an internal memory filesystem. Inner calls are outside native per-tool confirmation/hook wrappers; pre-hooks can also replace inputs after the outer confirmation. This is expressive composition, with authority and activation boundaries that must be explicit. Native model assignment is immediate; each new agentlet captures current configuration. Config PATCH only mutates state.config. Neither supplies Arconaut's affected-turn/workflow deferred activation contract.

Compaction is an ordered operator/program-selected pipeline of filters, truncation and model summarization. It rebuilds working ChatMessages with replacement IDs. The OpenCode-compatible summarize route actually replaces durable conversation messages by deleting and reinserting, ignores compaction errors and reduces its local message list to the summary. Final successful SQL messages contain serialized model parts, but incomplete native runs and nonchat events are not comprehensive originals. Durable context repair cannot depend on that destructive store.

Pool managers and debug object state are framework-owned; execution environments/MCP/ACP are possible external boundaries, not a mandate for independent shared-service governance. Source-read tests use TestModel/function callbacks for graph events and background cancellation, artificial model parts for transforms, and schema assertions for remote code-mode tools. They were not run and do not cover external provider cessation, per-tool authority inside composition, archive repair or durable replacement failures.

## Forge Workflow

Forge is a Rust/Tokio local control plane and native agent host, with TypeScript web UI. The inner model/provider/tool engine is a separate pinned agent-runtime git dependency. The inspected host validates saved scope/workspace binding, composes exact model tools, builds a configured runtime, sends input, forwards selected events and awaits completion/persistence. Separate identities/profiles power native chats, task roles and CLI adapters. Active native generation prevents simultaneous turns in one runtime session. The DB chat worker runs up to32 claimed jobs concurrently, freezes admission profile/policy across automatic retry and cancels/joins jobs on shutdown. Explicit retry instead admits current authority against the original trigger. These are current traced paths despite the README's transition warning.

Workflow JSON describes states/roles/hooks; transition guards check declared edges, actors and optional project workflow/version. Sequential hook actions call a closed compiled registry of CI, merge, role dispatch, dependencies, retries and cleanup. Dispatcher resolves current workflows and recovers active tasks first. Model task.adaptive actually calls policy/version checked split, sequence or replace commands. MCP offers broader task/project/chat operations, but transition attribution is explicitly system:Mcp because invoking agent execution identity is absent. A generic mutable workflow graph does not establish hot replacement of the model turn, or defer all changes to the end of affected work.

The useful context mechanism is chat LCM: original SQLite entry rows remain after summary leaf/node commits, paged expansion checks authorized view and source cursor, and summaries have source/revision metadata. The wired summarizer is deterministic text excerpting. Task worker/planner history uses structural compaction without LCM, reviewer/inquiry sessions differ, and explicit LCM tail truncation deletes entries outside protected summary ranges. Original-preserving chat context is therefore a bounded mechanism, not a universal immutable audit or demonstrated model repair UX.

Native file tools restrict admitted roots; command tools take allowlisted program/args, clear environment except PATH and run a bounded ephemeral process group. Normal completion, timeout or future drop kills its group, deliberately excluding standing services. Public native cancel asks for interrupt; the executing backend cancellation branch can await runtime shutdown. Dropped backend futures spawn asynchronous cleanup, shell cancellation returns after signal/grace/KILL without final reaping, and global graceful shutdown warns through cancellation errors. None is complete outpost/refit continuity.

Visible logs carry selected deltas, masked argument previews and bounded result summaries. Callback errors are ignored; output caps stop further entries; flush does not establish fsync and readers skip malformed lines. This is task/chat observability, not Arconaut's full core audit. Source-read tests use real in-memory SQLite CAS/source storage and actual Unix background-process PID liveness; reopen tests reuse the same in-memory DB, and retry-input tests are artificial histories. No tests or acquired programs were run.

## Cohort completion and coordination-services handoff

This study and ten rows cover Gas City, Gas Town, Claudex Pattern, Chat System, OpenAgents SDK, MetaGPT, Agent Orchestrator Rust, AgentChat, AgentPool and Forge Workflow. MCP Agent Mail, NTM and Station are owned by the science lane and covered in [coordination-services.md](coordination-services.md). All conclusions concern pinned snapshots; evidence is source reading, not execution or current-upstream validation. Arconaut can borrow message, context and lifecycle questions without adopting the infrastructure governance or approval style of these references.
