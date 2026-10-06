# fast-agent

Mutation-friendly turn hooks, typed workflow agents, live card/server reconfiguration and explicit archived compaction reconstruction.

Role: programmable Python MCP/coding-agent and workflow framework. Runtime: Python.

Pinned source: [https://github.com/evalstate/fast-agent](https://github.com/evalstate/fast-agent); revision/version `5e3ad9a78d9ebb4d2208939051805a515a26b7c4`.

Asyncio provider/tool loop, MCP aggregation, local shell runtimes and separate detached POSIX durable process supervisors.

Owns agent/session/history, AgentCard instances and managed child processes; consumes model/MCP/external runtime services. Detached supervisors remain local owned computation, not general shared-fabric governance.

Inspection: ToolRunner/provider delegation/local-MCP dispatch, shell profiles/durable lifetime, session/trajectory/archive reconstruction, reload path, evaluator workflow and direct test assertions

Limits of this study: No upstream code/tests run. Individual provider adapters/MCP servers/ACP internals not exhaustively inspected; claimed persistence scoped to actual records, not all transport or arbitrary external effects.

## Actions

### list_tools / call_tool

Surface: model/programmatic discovery and dispatch.

Input: name, arguments, tool_use_id/request params

Result: CallToolResult text/media/error, progress callback

Lifecycle: local then aggregator routing, awaited native callable or remote transport

Authority: native host/external service authority and explicit web-search permission boundary

Evidence: [e4](#evidence-e4), [e5](#evidence-e5), [e6](#evidence-e6).

### execute / bash / shell / exec; poll_process / process / terminate_process

Surface: model shell profiles.

Input: command/cwd/timeout/background/lifecycle then process ID/poll request

Result: text output/status, managed or durable process handle

Lifecycle: foreground yield or background; persistent selects detached supervisor, bounded capture

Authority: configured local shell authority; aliases/profile shapes differ

Evidence: [e7](#evidence-e7), [e8](#evidence-e8), [e9](#evidence-e9), [e15](#evidence-e15).

### DurableProcessStore.create / launch / request_stop / link_session

Surface: supplied process lifecycle API.

Input: command/shell/cwd/origin/retention limits; process ID/environment

Result: snapshot with spec/status/PIDs/capture counts and links

Lifecycle: create distinct launch, exclusive launch claim; durable stop/control record, detached supervision

Authority: private directories/capacity locking; environment inherited but not persisted

Evidence: [e9](#evidence-e9), [e10](#evidence-e10), [e11](#evidence-e11).

### resume_durable_processes

Surface: session restoration primitive.

Input: agent runtime map/session ID/fallback agent

Result: attached/unavailable/unattached snapshots

Lifecycle: reattaches existing linked process; never assumes command replay settles effects

Authority: runtime consumer with matching origin/fallback shell

Evidence: [e14](#evidence-e14), [e24](#evidence-e24).

### read_text_file / write_text_file / attach_media / read_skill

Surface: model local tool routing.

Input: path/text or media/skill source

Result: contents/write result or staged model-compatible media/skill text

Lifecycle: await local handler; media staging influences next call

Authority: enabled filesystem/skill runtime, model media limits separate

Evidence: [e6](#evidence-e6).

### ToolRunnerHooks before_llm_call / after_llm_call / before_tool_call / after_tool_call / after_turn_complete

Surface: programmable normal-turn hooks.

Input: runner/history/message/request params

Result: mutated history/params/injected messages

Lifecycle: await phase hooks on current iteration/once terminal

Authority: operator/program authored callbacks; low-level host access

Evidence: [e1](#evidence-e1), [e2](#evidence-e2), [e3](#evidence-e3).

### ParallelAgent.generate / EvaluatorOptimizerAgent.generate

Surface: programmable workflow agent.

Input: messages, request params, fanout/fanin or generator/evaluator and threshold/refinement cap

Result: aggregated or best-rated response; refinement history

Lifecycle: parallel gather then fanin; bounded actor-critic response loop

Authority: configured agents/models, no harness promotion implied

Evidence: [e21](#evidence-e21), [e22](#evidence-e22).

### compact_conversation / reconstruct_history

Surface: programmatic/operator compaction + export primitives.

Input: settings/instructions/keep-turns; context history/session dir

Result: summary/estimates/digest-linked archive then exact reconstructed messages/context boundary or error

Lifecycle: archive originals before replace when enabled; no tools summary side channel

Authority: explicitly configured persistence, strict path/digest/schema rejection

Evidence: [e16](#evidence-e16), [e17](#evidence-e17), [e23](#evidence-e23).

### reload_agents / _refresh_shared_instance

Surface: operator/program AgentCard refresh.

Input: registry changes/active managed run state

Result: AgentRefreshResult/new instance map/restored history

Lifecycle: rebuild impacted, shutdown old under instance lock; no refit barrier in traced body

Authority: operator/program card definitions and runtime-owned instances

Evidence: [e20](#evidence-e20).

### save_trajectory_record / session.save_history

Surface: supplied audit/checkpoint primitive.

Input: TrajectoryRecord or agent history/identity/checkpoint

Result: JSON invocation path or rotating history filename/metadata

Lifecycle: atomic file publication vs current/previous snapshot rotation

Authority: session directory owner; no universal raw transport archive

Evidence: [e18](#evidence-e18), [e19](#evidence-e19).

## Capabilities

### filesystem

**I — Files** (source): Model-facing read_text_file/write_text_file/attach_media local filesystem routing plus arbitrary registered MCP capabilities.

Evidence: [e6](#evidence-e6).

### processes

**I — OS programs** (source): Profile-specific execute/bash/exec + poll/process/terminate; persistent background launches supervisor and returns durable handle; ownership/lifecycle explicit.

Evidence: [e7](#evidence-e7), [e8](#evidence-e8), [e9](#evidence-e9), [e10](#evidence-e10), [e15](#evidence-e15).

### code-actions

**S — Code actions** (source): Native shell/local FunctionTools and external runtime permit ordinary computation; selected core no structured persistent notebook executor. Arbitrary local callables are host code.

Evidence: [e5](#evidence-e5), [e6](#evidence-e6), [e8](#evidence-e8).

### persistent-kernel

**S — Kernel** (source): Persistent child shell can host computation outside main harness and be reattached, but this is surviving OS process not serialized interpreter/kernel state or multi-consumer service contract.

Evidence: [e8](#evidence-e8), [e10](#evidence-e10), [e14](#evidence-e14), [e15](#evidence-e15).

### standing-database

**S — Standing DB** (source): Session/trajectory/durable-record stores are standing structured files; MCP services could supply databases. No general queryable shared standing DB core inferred.

Evidence: [e9](#evidence-e9), [e18](#evidence-e18), [e19](#evidence-e19).

### workflow-programming

**I — Workflows** (source): Composable parallel fanout/fanin and actor-critic refinement plus local callable tools/phase hooks. Workflow responses retained; no durable graph replay inferred.

Evidence: [e3](#evidence-e3), [e5](#evidence-e5), [e21](#evidence-e21), [e22](#evidence-e22).

### multi-model

**I — Models** (source): Fanout/fanin/evaluator/generator are independent agent objects with provider steps; model-specific shell/tool profile selection.

Evidence: [e1](#evidence-e1), [e7](#evidence-e7), [e21](#evidence-e21), [e22](#evidence-e22).

### live-collaboration

**S — Peer chat** (source): Programmed subagent fanout/fanin and parent-child trajectory attribution; selected workflows lack peer mailbox/room protocol, so concurrent models alone do not establish live conversation.

Evidence: [e18](#evidence-e18), [e21](#evidence-e21).

### concurrent-work

**I — Concurrency** (source): Planned parallel call batches, fanout gather and separate persistent supervisor processes.

Evidence: [e4](#evidence-e4), [e10](#evidence-e10), [e21](#evidence-e21).

### steering-interrupt

**L — Steer/interrupt** (source): Cancellation reconciles/resumes history and persistent process stop uses durable request; deliberate durable jobs survive runtime close. No claim external effects undone by history rollback.

Evidence: [e2](#evidence-e2), [e11](#evidence-e11), [e13](#evidence-e13), [e15](#evidence-e15).

### turn-redefinition

**I — Turn program** (source): Low-level mutation-friendly before/after provider/tool/terminal hooks change history, request params and injected messages; actual two-step hook test checks injection and order.

Evidence: [e1](#evidence-e1), [e2](#evidence-e2), [e3](#evidence-e3).

### compaction

**I — Compaction** (source): Summary side channel with no tools, retained complete tail/templates and explicit archives/digest; archive failure prevents replacement if persistence enabled.

Evidence: [e16](#evidence-e16), [e23](#evidence-e23).

### context-repair

**S — Repair** (source): Strict original reconstruction checks archive hash/path/schema and context replace boundary; hooks can reload context. This gives controlled primitives, not automatic semantic repair policy.

Evidence: [e3](#evidence-e3), [e16](#evidence-e16), [e17](#evidence-e17), [e23](#evidence-e23).

### original-audit

**L — Original audit** (source): Compaction retains original history and summarizer request/response; stateless trajectories retain original/effective tool args. Output retention caps, snapshots and optional no-history mode prevent universal everything-audit claim.

Evidence: [e9](#evidence-e9), [e16](#evidence-e16), [e18](#evidence-e18), [e19](#evidence-e19), [e23](#evidence-e23).

### audit-query

**S — Audit query** (source): Strict archive reconstruction/structured trajectory schema and process output records enable downstream study; complete database query facility over all raw events not traced.

Evidence: [e9](#evidence-e9), [e17](#evidence-e17), [e18](#evidence-e18).

### hot-change

**L — Hot change** (source): Registry-version AgentCard instance rebuild/restore and dynamic local/MCP tool routing; refresh instance lock/shutdown is not explicit all-owned-work quiescence/default workflow-completion barrier.

Evidence: [e6](#evidence-e6), [e20](#evidence-e20).

### rebuild-continuity

**L — Rebuild continuity** (source): Session restore plus durable supervisor reattachment preserves logical history and independently running local processes; active provider/task heaps not serialized; persistent work remains running rather than paused.

Evidence: [e14](#evidence-e14), [e15](#evidence-e15), [e19](#evidence-e19), [e20](#evidence-e20).

### remote-services

**I — Remote** (source): MCP aggregation and external runtime dispatcher consume services with request handlers; service governance not generally adopted by agent.

Evidence: [e6](#evidence-e6).

### self-improvement

**S — Self-improve** (source): Evaluator-optimizer refines generated answers until threshold/max, picks best response; hooks/tools could compose experiments but this is not harness self-research promotion.

Evidence: [e3](#evidence-e3), [e22](#evidence-e22).

### complaints

**S — Complaints** (source): Trajectory IDs/parent call/effective args and cancellation/checkpoint records can underpin complaints; no traced external bead-table/state snapshot filing contract.

Evidence: [e2](#evidence-e2), [e18](#evidence-e18).

### authority

**L — Authority** (source): Local callables run host code; MCP/external/filesystem/shell route boundary and search permission distinct. Individual external service/ACP authorities not inferred from local tool dispatch.

Evidence: [e5](#evidence-e5), [e6](#evidence-e6), [e9](#evidence-e9).

### evaluation

**I — Evaluation** (source): Structured evaluator ratings/feedback drive bounded response refinement; archive/resume/hook tests have concrete structural lifecycle oracles, not semantics-quality proof.

Evidence: [e22](#evidence-e22), [e23](#evidence-e23), [e24](#evidence-e24).

### time-order

**L — Time/order** (source): Exclusive record launch/stop/fsync, checkpoint locking, parent trajectory IDs and keyed tool results give local order; concurrent external calls no complete side-effect settlement journal.

Evidence: [e4](#evidence-e4), [e10](#evidence-e10), [e11](#evidence-e11), [e18](#evidence-e18), [e19](#evidence-e19).

## Inspected test oracles

- [quarantine/fast-agent/tests/unit/fast_agent/history/test_compaction.py](../../../quarantine/fast-agent/tests/unit/fast_agent/history/test_compaction.py): original archive preservation, reconstruction and summarizer accounting Oracle: Fake model summary tests reconstruct exact original messages; compare summary_request and parsed response; injected archive/link/partial-write failures assert unchanged history/bytes/unique outputs. Does not test semantic summary fidelity. Read, **not executed**.
- [quarantine/fast-agent/tests/unit/fast_agent/session/test_durable_process_resume.py](../../../quarantine/fast-agent/tests/unit/fast_agent/session/test_durable_process_resume.py): linked process restoration/ownership/unavailable classification Oracle: Source fixtures create POSIX sleep process supervisors and stale records, assert original worker attachment and unavailable/unattached sets with close cleanup. Read only, not executed; it checks lifecycle classification, not arbitrary daemon effect settlement. Read, **not executed**.
- [quarantine/fast-agent/tests/unit/fast_agent/agents/test_tool_runner_hooks.py](../../../quarantine/fast-agent/tests/unit/fast_agent/agents/test_tool_runner_hooks.py): two-step hook mutation/order Oracle: Fake LLM captures request messages; exact hook sequence and injected text assertion prove actual turn agency. No provider-live or refit guarantee. Read, **not executed**.

## Useful mechanisms

- Mutable phase hooks and rich local/MCP routing expose real programmable turns.
- Compaction archives preserve originals with digest-linked strict reconstruction and full summarizer request/response accounting.
- Detached durable supervisor records and explicit session-vs-persistent lifetime are concrete restartable process-consumer mechanisms.

## Material limits

- Persistent supervisor keeps owned computation running at main shutdown; Arconaut refit policy requires pause/drain or true separate daemon ownership.
- Trajectory files apply especially to stateless invocations; current/previous session snapshots and bounded process output are not universal original audit.
- Response evaluator-optimizer and AgentCard refresh do not establish harness optimization or executable refit.

## Arconaut design questions

- Can compaction archive/error discipline become normal audit/context repair with full raw originals beyond selected structured messages?
- What external lifecycle contract distinguishes paused owned processes from independent shared daemon consumers at refit?
- Which exact workflow/hook changes should activate after affected runs conclude, and what counts as safe forced interruption?

## Evidence

### Evidence e1

[quarantine/fast-agent/src/fast_agent/agents/tool_runner.py:190–251](../../../quarantine/fast-agent/src/fast_agent/agents/tool_runner.py#L190): Iterator folds selected polling history, auto-compacts followup, discovers tools, before-LLM hook, provider step and after-LLM hook.

### Evidence e2

[quarantine/fast-agent/src/fast_agent/agents/tool_runner.py:252–348](../../../quarantine/fast-agent/src/fast_agent/agents/tool_runner.py#L252): Tool-use/continue iteration checkpoints, canceled-turn rollback/persistence and terminal after-turn hook; cancellation does not transactionally undo external effects.

### Evidence e3

[quarantine/fast-agent/src/fast_agent/agents/tool_runner.py:106–133](../../../quarantine/fast-agent/src/fast_agent/agents/tool_runner.py#L106): Low-level hooks explicitly permit history/load/request-parameter mutations and injected messages at loop boundaries.

### Evidence e4

[quarantine/fast-agent/src/fast_agent/agents/tool_agent.py:966–1042](../../../quarantine/fast-agent/src/fast_agent/agents/tool_agent.py#L966): Tool discovery/planning matches actual names, schedules parallel or sequential calls and keyed results/timings.

### Evidence e5

[quarantine/fast-agent/src/fast_agent/agents/tool_agent.py:1083–1153](../../../quarantine/fast-agent/src/fast_agent/agents/tool_agent.py#L1083): Local function executes FastMCP tool with invocation/progress context, converts native result and synthesizes error CallToolResult.

### Evidence e6

[quarantine/fast-agent/src/fast_agent/agents/mcp_agent.py:1628–1729](../../../quarantine/fast-agent/src/fast_agent/agents/mcp_agent.py#L1628): Routes external/filesystem/skill/shell/local handlers first then MCP aggregator; web search permission boundary explicit and human-input opt-in builtin.

### Evidence e7

[quarantine/fast-agent/src/fast_agent/tools/shell_runtime.py:398–490](../../../quarantine/fast-agent/src/fast_agent/tools/shell_runtime.py#L398): Model/tool profiles expose execute/poll/terminate or bash/process or shell/exec aliases, separate capability settings.

### Evidence e8

[quarantine/fast-agent/src/fast_agent/tools/shell_runtime.py:2970–3025](../../../quarantine/fast-agent/src/fast_agent/tools/shell_runtime.py#L2970): Native persistent background shell path invokes durable launch and returns tracked status/handle, not an unused helper.

### Evidence e9

[quarantine/fast-agent/src/fast_agent/tools/durable_processes.py:180–249](../../../quarantine/fast-agent/src/fast_agent/tools/durable_processes.py#L180): Private durable spec/record/capacity lock, command/cwd/origin session/agent and output retention; creation distinct from launch.

### Evidence e10

[quarantine/fast-agent/src/fast_agent/tools/durable_processes.py:286–330](../../../quarantine/fast-agent/src/fast_agent/tools/durable_processes.py#L286): Launch claims process exactly once and starts detached supervisor inheriting environment without persisting it, separate session/fds.

### Evidence e11

[quarantine/fast-agent/src/fast_agent/tools/durable_processes.py:1112–1158](../../../quarantine/fast-agent/src/fast_agent/tools/durable_processes.py#L1112): Stop request published via exclusive hardlink, flush/fsync and directory fsync; status/capture records temp+replace+fsync.

### Evidence e12

[quarantine/fast-agent/src/fast_agent/tools/durable_process_supervisor.py:141–195](../../../quarantine/fast-agent/src/fast_agent/tools/durable_process_supervisor.py#L141): Supervisor opens append capture streams and launches shell in its own process session with DEVNULL stdin, drains output through threads and retention limits.

### Evidence e13

[quarantine/fast-agent/src/fast_agent/tools/durable_process_supervisor.py:345–383](../../../quarantine/fast-agent/src/fast_agent/tools/durable_process_supervisor.py#L345): Stop/cleanup sends group TERM then group KILL, including remaining descendants after direct child ends.

### Evidence e14

[quarantine/fast-agent/src/fast_agent/session/durable_processes.py:44–108](../../../quarantine/fast-agent/src/fast_agent/session/durable_processes.py#L44): Resumed session discovers/deduplicates matching linked active records; attaches original-agent/fallback shell; reports unavailable/unattached rather than recreating commands.

### Evidence e15

[quarantine/fast-agent/src/fast_agent/tools/shell_runtime.py:2818–2875](../../../quarantine/fast-agent/src/fast_agent/tools/shell_runtime.py#L2818): Close terminates session processes but detaches durable ones, explicitly reports remaining handles; not refit pause/quiescence.

### Evidence e16

[quarantine/fast-agent/src/fast_agent/history/compaction.py:568–682](../../../quarantine/fast-agent/src/fast_agent/history/compaction.py#L568): No-tool side-channel summary, failure leaves history; archive must succeed when persistence enabled, digest-linked metadata retains summary request/response, then installs templates+summary+tail.

### Evidence e17

[quarantine/fast-agent/src/fast_agent/history/atif_reconstruction.py:51–116](../../../quarantine/fast-agent/src/fast_agent/history/atif_reconstruction.py#L51): Strict archive validator checks every message, digest/path/symlink and unique checkpoint; reconstruction inserts model-context boundary instead of replaying retained tail as new effects.

### Evidence e18

[quarantine/fast-agent/src/fast_agent/session/trajectory.py:1–126](../../../quarantine/fast-agent/src/fast_agent/session/trajectory.py#L1): Stateless invocation records original/effective args, rendered child input, messages/model/usage and parent-call IDs, atomic temp rename; source says future append-only stream, not current universal journal.

### Evidence e19

[quarantine/fast-agent/src/fast_agent/session/session_manager.py:365–443](../../../quarantine/fast-agent/src/fast_agent/session/session_manager.py#L365): Exclusive checkpoint lock, history current/previous rotation and session metadata update; conversational snapshots rather than complete append-only core audit.

### Evidence e20

[quarantine/fast-agent/src/fast_agent/core/managed_runtime.py:463–544](../../../quarantine/fast-agent/src/fast_agent/core/managed_runtime.py#L463): Registry-version refresh rebuilds impacted instances under instance lock, shuts old agents, finalizes new and restores session; no all-work drain shown in this refresh body.

### Evidence e21

[quarantine/fast-agent/src/fast_agent/agents/workflow/parallel_agent.py:191–209](../../../quarantine/fast-agent/src/fast_agent/agents/workflow/parallel_agent.py#L191): Fanout child generation via asyncio.gather, telemetry; subsequent fanin separate agent/model.

### Evidence e22

[quarantine/fast-agent/src/fast_agent/agents/workflow/evaluator_optimizer.py:190–260](../../../quarantine/fast-agent/src/fast_agent/agents/workflow/evaluator_optimizer.py#L190): Evaluator response rating/needs-improvement determines refinement/threshold/max; returns best response, not modified harness or deployment.

### Evidence e23

[quarantine/fast-agent/tests/unit/fast_agent/history/test_compaction.py:825–883](../../../quarantine/fast-agent/tests/unit/fast_agent/history/test_compaction.py#L825): Exact fail-archive unchanged history, persistence-disabled reconstruction rejection, unique archive/no-overwrite collision and partial-write cleanup oracle.

### Evidence e24

[quarantine/fast-agent/tests/unit/fast_agent/session/test_durable_process_resume.py:70–92](../../../quarantine/fast-agent/tests/unit/fast_agent/session/test_durable_process_resume.py#L70): Resume assertion routes active linked process to original worker, classifies stale/unavailable and leaves main unattached.

