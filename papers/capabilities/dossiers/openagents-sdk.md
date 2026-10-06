# openagents-sdk

Networked Python workers communicate through direct/channel events and configured tools; workspace adapters separately launch external coding CLIs. Several prototype/logging contracts are materially weaker than their names suggest.

Role: agent network/framework plus native-CLI workspace adapters. Runtime: Python, TypeScript (workspace UI, outside traced execution).

Pinned source: [https://github.com/openagents-org/openagents-sdk](https://github.com/openagents-org/openagents-sdk); revision/version `faf416fca40b1cf73f585636478587967bc9f6f5`.

asyncio SDK event network and bounded model/tool loop; separate per-channel native CLI subprocess adapters

SDK owns network/mod/membership/project/cache fabric; individual workers/adapters consume it and native/provider services

Inspection: Selected source excerpts trace exposed entry/loop/request/action/result/state boundaries and relevant capability mechanisms; listed tests are read as source only.

Limits of this study: No acquired code executed, dependencies installed or live providers invoked. Claims concern this pinned snapshot; untraced catalog tools and dependency internals are not inferred.

## Actions

### WorkerAgent.react / @on

Surface: programmable API.

Input: EventContext and event-pattern handler

Result: custom handler effects or AgentTrajectory

Lifecycle: matched handlers awaited before configured model trigger

Authority: Python author config; exposed tools determine model authority

Evidence: [worker](#evidence-worker), [trigger](#evidence-trigger), [run](#evidence-run).

### AgentRunner.run_agent / run_llm / finish

Surface: programmable API / model tool.

Input: context, instruction, iteration bound, selected tools; finish reason

Result: action trajectory, tool values/error strings, completion

Lifecycle: serial awaited tools and model calls until finish/direct answer/bound

Authority: configured model invokes installed tools without a per-call approval gate in this loop

Evidence: [loop](#evidence-loop), [dispatch](#evidence-dispatch).

### update_tools / configured Python tools / MCP

Surface: programmable API.

Input: mod adapter tools, MCP config, module/function tool config

Result: current tool list; import failures logged/omitted

Lifecycle: snapshot refreshed explicitly; ongoing orchestration retains selected list

Authority: operator/Python author controls configured implementations

Evidence: [tools](#evidence-tools), [run](#evidence-run).

### send_direct_message / send_channel_message / reply_channel_message

Surface: model tool / programmable API.

Input: target_agent_id|channel, text, optional quote/mentioned_agent_id/reply_to_id

Result: EventResponse and message IDs; notifications with original ID

Lifecycle: network routing then in-memory recipient queue; not model-read receipt

Authority: connected agent; workspace/channel membership and secret boundary

Evidence: [send](#evidence-send), [msg-tools](#evidence-msg-tools), [room](#evidence-room), [auth](#evidence-auth).

### retrieve_channel_messages / retrieve_direct_messages / list_channels

Surface: model tool / programmable API.

Input: channel|peer, limit, offset, include_threads

Result: message/history dictionaries or empty timeout fallback

Lifecycle: awaited response; Worker history requests keyed by channel/peer

Authority: connected configured agent

Evidence: [history](#evidence-history), [msg-tools](#evidence-msg-tools).

### react_to_message / upload_file / get_agent_list

Surface: programmable API.

Input: channel/message/reaction or local file path/filename

Result: EventResponse, file UUID or agent IDs

Lifecycle: awaited workspace calls; local/native file access depends on adapter

Authority: configured worker; not universal tool exposure

Evidence: [send](#evidence-send), [history](#evidence-history).

### start_project / stop_project / complete_project / get_project / send_project_message

Surface: model tool.

Input: template ID, goal, collaborators; project ID/status/content

Result: project IDs/status/messages; template tools optionally final results

Lifecycle: creation response; optional polling until terminal status/time limit

Authority: model configured with project adapter; project permission checked on state operations

Evidence: [project](#evidence-project), [projectwait](#evidence-projectwait), [projectstate](#evidence-projectstate).

### set_project_global_state / get_project_global_state / create_cache

Surface: model tool / programmable API.

Input: project/key/value or value/mime/allowed_agent_groups

Result: state values or cache ID/None

Lifecycle: remote mod state; cache correlated response wait ten seconds

Authority: authorized project member or configured shared-cache consumer

Evidence: [projectstate](#evidence-projectstate), [cache](#evidence-cache).

### schedule_task

Surface: programmable API.

Input: delay seconds and zero-arg coroutine

Result: asyncio.Task

Lifecycle: resident delayed task; cancelled on teardown without await

Authority: Python worker author

Evidence: [schedule](#evidence-schedule), [worker](#evidence-worker).

### AgentNetwork.process_event / subscribe / poll_events

Surface: network API.

Input: Event source/destination/name/payload; subscriptions

Result: queued events and optimistic EventResponse

Lifecycle: in-memory fanout/dedup; poll drains before receiver processes

Authority: fabric owns transport/membership/secret verification; internal mods trusted

Evidence: [gateway](#evidence-gateway), [delivery](#evidence-delivery), [auth](#evidence-auth).

### load_mod / unload_mod / restart

Surface: fabric API / system event.

Input: Python mod path/config or new NetworkConfig

Result: success/error EventResponse or restart Boolean

Lifecycle: immediate shared mutation; restart shuts whole network; ID-only unload mismatch

Authority: network administrator/system-event surface; no turn-bound activation fence

Evidence: [load](#evidence-load), [unload](#evidence-unload), [restart](#evidence-restart), [auth](#evidence-auth).

### Claude workspace adapter set_mode / stop

Surface: operator control.

Input: plan|execute mode; stop action

Result: status; native CLI sessions/results

Lifecycle: mode changes next CLI command; stop terminates active groups and drops queues

Authority: workspace controller; execute passes native permission bypass

Evidence: [control](#evidence-control), [claudeauth](#evidence-claudeauth), [stop](#evidence-stop), [claudeturn](#evidence-claudeturn), [clauderesume](#evidence-clauderesume).

### LLMCallLogger.log_call / LLMLogReader.get_logs

Surface: supporting API.

Input: model/provider/messages/tools/response/latency; agent filters

Result: JSONL IDs and filtered records

Lifecycle: standalone append/rotation; main EventBased logger does not call it

Authority: Python consumer must wire it; logging failure does not prevent work

Evidence: [filewrite](#evidence-filewrite), [filelog](#evidence-filelog), [logread](#evidence-logread), [log-noop](#evidence-log-noop).

## Capabilities

### filesystem

**L — Files** (source): External Claude adapter exposes local Read/Write/Edit/Bash and uploads; SDK core instead supplies workspace/cache file APIs and configured custom tools. Direct text adapter has no tool loop.

Evidence: [claudeauth](#evidence-claudeauth), [history](#evidence-history), [direct](#evidence-direct).

### processes

**I — OS programs** (source): Workspace Claude adapter owns external CLI process groups, streams output and has escalating stop; SDK model executor only awaits supplied tool implementations.

Evidence: [claudeturn](#evidence-claudeturn), [stop](#evidence-stop), [dispatch](#evidence-dispatch).

### code-actions

**S — Code actions** (source): Python event handlers and configured function tools can compose programs; traced model turn dispatches named JSON tools rather than model-authored general code cells.

Evidence: [worker](#evidence-worker), [tools](#evidence-tools), [dispatch](#evidence-dispatch).

### persistent-kernel

**? — Kernel** (inspection-limit): No model-addressable persistent Python execution kernel established in SDK orchestrator, messaging mods or workspace adapters inspected.

### standing-database

**S — Standing DB** (source): Remote project/global state and shared cache are supporting shared data services; no arbitrary standing SQL table/query API established in traced adapters.

Evidence: [projectstate](#evidence-projectstate), [cache](#evidence-cache).

### workflow-programming

**I — Workflows** (source): Worker event triggers/custom handlers, Python coroutine scheduling, mod/MCP tools and generated project-template tools form exposed orchestration surfaces.

Evidence: [worker](#evidence-worker), [trigger](#evidence-trigger), [schedule](#evidence-schedule), [projectwait](#evidence-projectwait), [tools](#evidence-tools).

### multi-model

**I — Models** (source): Each configured AgentRunner resolves its own model/provider before orchestration; network peers may differ. Native workspace adapters separately use external CLIs.

Evidence: [loop](#evidence-loop), [run](#evidence-run), [claudeauth](#evidence-claudeauth), [providers](#evidence-providers).

### live-collaboration

**I — Peer chat** (source): SDK agent direct/channel/group/broadcast messages and threaded replies carry shared-room notifications into event-triggered model reactions. Delivery receipts only prove routing/queue activity, sometimes even less.

Evidence: [delivery](#evidence-delivery), [room](#evidence-room), [trigger](#evidence-trigger), [msg-tools](#evidence-msg-tools).

### concurrent-work

**I — Concurrency** (source): Workspace adapter serializes each channel and runs channels concurrently; SDK schedules resident tasks and multiple network participants. SDK tools within one turn are serial.

Evidence: [adapterqueues](#evidence-adapterqueues), [schedule](#evidence-schedule), [dispatch](#evidence-dispatch).

### steering-interrupt

**L — Steer/interrupt** (source): Workspace control can change mode and stop all current channel processes, losing queued messages; base shutdown gathers workers instead of cancelling them. No universal model/tool quiescence guarantee.

Evidence: [control](#evidence-control), [stop](#evidence-stop), [adapterqueues](#evidence-adapterqueues), [worker](#evidence-worker).

### turn-redefinition

**S — Turn program** (source): Python react override, templates/tool snapshots and mod pipeline are programmable building blocks. ONM staged guard/transform/observe pipeline is separate prototype, not this SDK loop.

Evidence: [worker](#evidence-worker), [loop](#evidence-loop), [tools](#evidence-tools), [pipeline](#evidence-pipeline), [onm-separate](#evidence-onm-separate), [onm](#evidence-onm).

### compaction

**L — Compaction** (source): Claude adapter relays native compaction notices and infers compaction from quiet output; direct adapter simply drops oldest shared history. No managed cross-path compaction choices.

Evidence: [claudeturn](#evidence-claudeturn), [clauderesume](#evidence-clauderesume), [direct](#evidence-direct).

### context-repair

**S — Repair** (source): History retrieval, project state and native per-channel resume IDs can retrieve context; no repair/versioned projection operation preserving authoritative originals traced.

Evidence: [history](#evidence-history), [projectstate](#evidence-projectstate), [clauderesume](#evidence-clauderesume).

### original-audit

**L — Original audit** (source): Main SDK model logger is effectively no-op; message snapshots/archives overwrite, batch and expire. Separate JSONL logger can persist selected calls but neither guarantees complete original I/O retention.

Evidence: [log-noop](#evidence-log-noop), [retain](#evidence-retain), [save](#evidence-save), [archive](#evidence-archive), [filewrite](#evidence-filewrite), [filelog](#evidence-filelog).

### audit-query

**S — Audit query** (source): Standalone LLMLogReader filters file-backed selected calls, and messaging history is queryable. They cannot query unrecorded main-loop calls or prove complete causal history.

Evidence: [logread](#evidence-logread), [history](#evidence-history), [log-noop](#evidence-log-noop).

### hot-change

**L — Hot change** (source): Tools refresh explicitly; live network mod load/unload and immediate adapter mode changes exist. No affected-turn/workflow activation fence; ID-only mod unload can leave instance reachable.

Evidence: [tools](#evidence-tools), [load](#evidence-load), [unload](#evidence-unload), [control](#evidence-control).

### rebuild-continuity

**L — Rebuild continuity** (source): External CLI session IDs resume channels; network restart shuts fabric and may continue after mod-load errors. No outpost transfer, request drain, reversible compile/refit or guaranteed child quiescence.

Evidence: [clauderesume](#evidence-clauderesume), [restart](#evidence-restart), [adapterqueues](#evidence-adapterqueues), [worker](#evidence-worker).

### remote-services

**I — Remote** (source): Agents consume authenticated event network and remote workspace/project/cache; SDK AgentNetwork is itself the fabric governor, distinct from Arconaut consumer role.

Evidence: [auth](#evidence-auth), [delivery](#evidence-delivery), [projectstate](#evidence-projectstate), [cache](#evidence-cache).

### self-improvement

**S — Self-improve** (source): Custom Python tools, module loading and external Bash/write permit editing/extensions; no model-governed experiment/recovery/adoption policy or executable continuity loop traced.

Evidence: [tools](#evidence-tools), [load](#evidence-load), [claudeauth](#evidence-claudeauth).

### complaints

**? — Complaints** (inspection-limit): No dedicated state-capturing model frustration/complaint record with separately tracked repair obligation established in studied SDK/adapters/project/messaging paths.

### authority

**L — Authority** (source): SDK configured tools execute directly. External Claude execute mode bypasses approvals; plan restricts native/MCP set. Normal transport uses agent secrets but broad no-secret system-event exemption requires separate authorization review.

Evidence: [dispatch](#evidence-dispatch), [claudeauth](#evidence-claudeauth), [auth](#evidence-auth).

### evaluation

**L — Evaluation** (source): Read tests cover mock worker message forwarding, actual temporary file logger/storage and separate ONM ordering. They do not validate durable logging of wired loop, live CLI termination or cross-channel direct-history isolation.

Evidence: [log-noop](#evidence-log-noop), [onm-separate](#evidence-onm-separate), [adapterqueues](#evidence-adapterqueues), [direct](#evidence-direct).

### time-order

**L — Time/order** (source): Gateway overwrites timestamps with current seconds, dedup marks before processing and polling drains queues. Message IDs/future keys are not durable receiver acknowledgments or crash-safe causal ordering.

Evidence: [gateway](#evidence-gateway), [delivery](#evidence-delivery), [history](#evidence-history), [adapterqueues](#evidence-adapterqueues).

## Inspected test oracles

- [quarantine/openagents-sdk/tests/workspace/test_worker_agent.py](../../../quarantine/openagents-sdk/tests/workspace/test_worker_agent.py): Worker direct send and EventResponse forwarding Oracle: MockWorkerAgent skips parent initialization and uses AsyncMock workspace send; asserts exact arguments/response fields, not real network delivery. Read, **not executed**.
- [quarantine/openagents-sdk/tests/mods/test_messaging_memory_management.py](../../../quarantine/openagents-sdk/tests/mods/test_messaging_memory_management.py): Bounded in-memory messages and dump retention Oracle: Temporary storage helper receives synthetic timestamps; count limit asserted; archive assertion conditional on directory existence, so not a fail-closed archival oracle. Read, **not executed**.
- [quarantine/openagents-sdk/tests/test_onm_pipeline.py](../../../quarantine/openagents-sdk/tests/test_onm_pipeline.py): Separate prototype rejection/transformation/stage order Oracle: Synthetic guard/transform/observer implementations assert stage order and rejection; not the SDK gateway processor or network integration. Read, **not executed**.
- [quarantine/openagents-sdk/tests/lms/test_llm_logger.py](../../../quarantine/openagents-sdk/tests/lms/test_llm_logger.py): Standalone LLMCallLogger serialization Oracle: Real temporary JSONL file existence and values are asserted; tested class differs from wired EventBasedLLMLogger, leaving no-op integration uncovered. Read, **not executed**.

## Useful mechanisms

- Actual event-triggered peer rooms with tool-backed messaging and project-state APIs.
- Python handlers/mods and externally launched agents supply distinct extension levels.
- Explicit plan/execute native tool set and process-group stop surface.

## Material limits

- Wired model logger does not persist/send; standalone logger tests do not cover it.
- ONM staged pipeline is future-envelope prototype; live SDK first-intercept mod processor has different semantics.
- Adapter cursor advances before dispatch and direct adapter shares history across concurrent channels.
- Network mutation/restart governs shared fabric and lacks turn fence/refit guarantees.

## Arconaut design questions

- What receipt distinguishes routed, queued, started, model-consumed and persisted messages?
- How should shared provider-history isolation and audit integration be tested across concurrent channels?
- Can model-configurable extension activation be fenced at affected turn/workflow boundaries while fabric remains independently managed?

## Evidence

### Evidence trigger

[quarantine/openagents-sdk/src/openagents/agents/collaborator_agent.py:11–42](../../../quarantine/openagents-sdk/src/openagents/agents/collaborator_agent.py#L11): Configured event triggers or react-to-all invoke AgentRunner.run_agent; not every incoming event starts a model turn.

### Evidence worker

[quarantine/openagents-sdk/src/openagents/agents/worker_agent.py:241–338](../../../quarantine/openagents-sdk/src/openagents/agents/worker_agent.py#L241): Custom matched handlers run before collaborator reaction; handler exceptions log; teardown cancels scheduled tasks without awaiting their termination.

### Evidence tools

[quarantine/openagents-sdk/src/openagents/agents/runner.py:113–176](../../../quarantine/openagents-sdk/src/openagents/agents/runner.py#L113): update_tools snapshots mod/MCP/configured Python tools; custom import failures degrade to an empty set and restore sys.path.

### Evidence run

[quarantine/openagents-sdk/src/openagents/agents/runner.py:258–293](../../../quarantine/openagents-sdk/src/openagents/agents/runner.py#L258): run_agent passes selected mod/MCP/custom tools and current configuration/context into orchestrate_agent.

### Evidence loop

[quarantine/openagents-sdk/src/openagents/agents/orchestrator.py:180–256](../../../quarantine/openagents-sdk/src/openagents/agents/orchestrator.py#L180): Each orchestration builds templated system/user context; default maximum is ten iterations; awaits provider completion and attempted event-based logging.

### Evidence dispatch

[quarantine/openagents-sdk/src/openagents/agents/orchestrator.py:322–450](../../../quarantine/openagents-sdk/src/openagents/agents/orchestrator.py#L322): Model tools execute serially by name and parsed JSON arguments; finish ends chain, results/errors append to conversation, and model interaction failure ends trajectory.

### Evidence log-noop

[quarantine/openagents-sdk/src/openagents/lms/llm_logger.py:325–391](../../../quarantine/openagents-sdk/src/openagents/lms/llm_logger.py#L325): Wired EventBasedLLMLogger constructs an entry then debug-logs and returns UUID only: no file write or network event occurs.

### Evidence filelog

[quarantine/openagents-sdk/src/openagents/lms/llm_logger.py:61–89](../../../quarantine/openagents-sdk/src/openagents/lms/llm_logger.py#L61): Separate LLMCallLogger rotates at size threshold and deletes old files by configured retention; it is not the logger used in orchestrate_agent.

### Evidence filewrite

[quarantine/openagents-sdk/src/openagents/lms/llm_logger.py:159–181](../../../quarantine/openagents-sdk/src/openagents/lms/llm_logger.py#L159): Standalone logger appends JSONL; errors are logged rather than propagated, with no fsync contract.

### Evidence logread

[quarantine/openagents-sdk/src/openagents/lms/llm_log_reader.py:51–120](../../../quarantine/openagents-sdk/src/openagents/lms/llm_log_reader.py#L51): Standalone file reader supports model/since/error/search and pagination, skips malformed records; it cannot recover records never written by the main loop.

### Evidence gateway

[quarantine/openagents-sdk/src/openagents/sdk/event_gateway.py:117–202](../../../quarantine/openagents-sdk/src/openagents/sdk/event_gateway.py#L117): SDK timestamps are overwritten with current seconds; IDs enter dedup set before mod processing; first mod response intercepts, otherwise delivery reports success.

### Evidence delivery

[quarantine/openagents-sdk/src/openagents/sdk/event_gateway.py:203–315](../../../quarantine/openagents-sdk/src/openagents/sdk/event_gateway.py#L203): Channel/group/broadcast and direct routes enqueue subscribed recipients; missing recipient queue skips; polling drains in-memory queues without processing acknowledgments.

### Evidence pipeline

[quarantine/openagents-sdk/src/openagents/sdk/event_processor.py:49–120](../../../quarantine/openagents-sdk/src/openagents/sdk/event_processor.py#L49): Live SDK uses ordered first-intercept mods, target-mod routing and shallow copied event; exceptions in ordinary pipeline log and continue.

### Evidence onm-separate

[quarantine/openagents-sdk/src/openagents/core/onm_events.py:1–10](../../../quarantine/openagents-sdk/src/openagents/core/onm_events.py#L1): ONM envelope is explicitly separate from existing SDK Event; guard/transform/observer prototype is not the traced network path.

### Evidence onm

[quarantine/openagents-sdk/src/openagents/core/onm_pipeline.py:32–103](../../../quarantine/openagents-sdk/src/openagents/core/onm_pipeline.py#L32): Separate prototype sorts guard/transform/observer stages and exposes mutable mod registration; unit support does not establish agent availability.

### Evidence load

[quarantine/openagents-sdk/src/openagents/sdk/network.py:989–1068](../../../quarantine/openagents-sdk/src/openagents/sdk/network.py#L989): Runtime load_mod imports Python, binds/initializes then mutates mod registry and live processor dictionary; no turn/workflow fence or fresh-code reload.

### Evidence unload

[quarantine/openagents-sdk/src/openagents/sdk/network.py:1077–1124](../../../quarantine/openagents-sdk/src/openagents/sdk/network.py#L1077): unload checks dynamic ID but retrieves instance by supplied path; ID-only call can unregister without finding/removing path-keyed running mod.

### Evidence restart

[quarantine/openagents-sdk/src/openagents/sdk/network.py:1200–1256](../../../quarantine/openagents-sdk/src/openagents/sdk/network.py#L1200): Network restart owns network shutdown/config/module reset and continues even if module loading fails; not consumer-only executable refit.

### Evidence auth

[quarantine/openagents-sdk/src/openagents/sdk/network.py:867–924](../../../quarantine/openagents-sdk/src/openagents/sdk/network.py#L867): Transport authenticates normal events by agent secret, but no-secret system events except polling/unregister bypass this gate; internal mods bypass authentication.

### Evidence send

[quarantine/openagents-sdk/src/openagents/agents/worker_agent.py:525–620](../../../quarantine/openagents-sdk/src/openagents/agents/worker_agent.py#L525): Worker API sends direct/channel messages and replies/reactions via workspace connection, returning event responses.

### Evidence history

[quarantine/openagents-sdk/src/openagents/agents/worker_agent.py:622–732](../../../quarantine/openagents-sdk/src/openagents/agents/worker_agent.py#L622): History futures keyed only by channel/peer wait ten seconds; timeout/errors return empty-history shape, and concurrent same-channel requests can overwrite future.

### Evidence schedule

[quarantine/openagents-sdk/src/openagents/agents/worker_agent.py:744–758](../../../quarantine/openagents-sdk/src/openagents/agents/worker_agent.py#L744): schedule_task creates tracked asyncio sleep-then-coro task and returns Task handle, not persistent external scheduling.

### Evidence msg-tools

[quarantine/openagents-sdk/src/openagents/mods/workspace/messaging/adapter.py:1268–1429](../../../quarantine/openagents-sdk/src/openagents/mods/workspace/messaging/adapter.py#L1268): Native tool schemas bind send_direct_message, send_channel_message, reply_channel_message, list_channels and retrieve history to adapter methods; threading limit and pagination are explicit.

### Evidence room

[quarantine/openagents-sdk/src/openagents/mods/workspace/messaging/mod.py:1220–1308](../../../quarantine/openagents-sdk/src/openagents/mods/workspace/messaging/mod.py#L1220): Unknown channel is created with all active agents; channel notifications carry original_event_id but per-recipient route errors are logged, not propagated as failed delivery.

### Evidence retain

[quarantine/openagents-sdk/src/openagents/mods/workspace/messaging/mod.py:2176–2215](../../../quarantine/openagents-sdk/src/openagents/mods/workspace/messaging/mod.py#L2176): Message history is mutable ID-keyed dictionary; cleanup/dumping run on insert and main snapshot saves every configured batch, independent of route success.

### Evidence save

[quarantine/openagents-sdk/src/openagents/mods/workspace/messaging/message_storage_helper.py:111–164](../../../quarantine/openagents-sdk/src/openagents/mods/workspace/messaging/message_storage_helper.py#L111): Main message_history.json overwrites nonatomically; serialization/load/write errors log and may yield partial or empty history.

### Evidence archive

[quarantine/openagents-sdk/src/openagents/mods/workspace/messaging/message_storage_helper.py:278–357](../../../quarantine/openagents-sdk/src/openagents/mods/workspace/messaging/message_storage_helper.py#L278): Daily gzip archives merge IDs and overwrite; corrupt existing archive may be replaced after logged parse failure; archive errors log and retention deletes old data.

### Evidence project

[quarantine/openagents-sdk/src/openagents/mods/workspace/project/adapter.py:43–163](../../../quarantine/openagents-sdk/src/openagents/mods/workspace/project/adapter.py#L43): Tools expose list/start/stop/complete/get_project and project messaging with goal/template/collaborator IDs.

### Evidence projectwait

[quarantine/openagents-sdk/src/openagents/mods/workspace/project/adapter.py:480–561](../../../quarantine/openagents-sdk/src/openagents/mods/workspace/project/adapter.py#L480): Generic start returns creation response; generated template tools can wait by two-second polling for terminal project status with 300-second default limit.

### Evidence projectstate

[quarantine/openagents-sdk/src/openagents/mods/workspace/project/mod.py:520–564](../../../quarantine/openagents-sdk/src/openagents/mods/workspace/project/mod.py#L520): Project state get/set checks project permission, then mutates state and returns prior/current values; not an exposed SQL database or persistent computation kernel.

### Evidence cache

[quarantine/openagents-sdk/src/openagents/mods/core/shared_cache/adapter.py:93–160](../../../quarantine/openagents-sdk/src/openagents/mods/core/shared_cache/adapter.py#L93): Shared cache value/mime/group API sends event, waits up to ten seconds for correlated completion and returns cache ID or None.

### Evidence adapterqueues

[quarantine/openagents-sdk/src/openagents/adapters/base.py:178–280](../../../quarantine/openagents-sdk/src/openagents/adapters/base.py#L178): Adapter marks cursor/processed IDs before dispatch, serializes per-channel work in memory and runs separate channels concurrently; finally gathers active tasks rather than cancelling them.

### Evidence control

[quarantine/openagents-sdk/src/openagents/adapters/base.py:130–154](../../../quarantine/openagents-sdk/src/openagents/adapters/base.py#L130): Control set_mode immediately changes plan/execute; other actions delegate; cursor advances before action and exceptions are swallowed.

### Evidence claudeauth

[quarantine/openagents-sdk/src/openagents/adapters/claude.py:290–389](../../../quarantine/openagents-sdk/src/openagents/adapters/claude.py#L290): Plan permits read-oriented tools with Claude plan mode; execute requests permission bypass plus Bash/write and workspace MCP; configuration is fixed for each launched CLI process.

### Evidence stop

[quarantine/openagents-sdk/src/openagents/adapters/claude.py:78–162](../../../quarantine/openagents-sdk/src/openagents/adapters/claude.py#L78): Stop affects all running channel processes, escalates group termination, discards pending channel messages and posts status; it does not pause shared services.

### Evidence claudeturn

[quarantine/openagents-sdk/src/openagents/adapters/claude.py:474–514](../../../quarantine/openagents-sdk/src/openagents/adapters/claude.py#L474): External Claude CLI launches per channel with stdout/stderr pipes and POSIX process group; fifteen-second quiet intervals are labeled compaction without proving actual compaction.

### Evidence clauderesume

[quarantine/openagents-sdk/src/openagents/adapters/claude.py:594–675](../../../quarantine/openagents-sdk/src/openagents/adapters/claude.py#L594): Result session IDs persist channel resume map; system compaction notices surface; process wait followed by final text and temporary MCP cleanup, while most native outputs remain external CLI-owned.

### Evidence direct

[quarantine/openagents-sdk/src/openagents/adapters/llm_direct.py:117–197](../../../quarantine/openagents-sdk/src/openagents/adapters/llm_direct.py#L117): Direct adapter truncates shared conversation list and sends streaming chat-completion payload without tool schemas; same history is shared by concurrent channels.

### Evidence providers

[quarantine/openagents-sdk/src/openagents/agents/orchestrator.py:140–177](../../../quarantine/openagents-sdk/src/openagents/agents/orchestrator.py#L140): Each orchestration resolves auto/environment or configured provider/model credentials and constructs its model provider.

