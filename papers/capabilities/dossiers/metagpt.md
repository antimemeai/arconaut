# metagpt

MetaGPT combines role SOP/react/planner workflows and model-controlled delegation with a separate Python notebook code-generation path. Its local team model is real collaboration but recovery snapshots do not preserve unresolved computation/effects.

Role: resident multi-role agent/workflow framework with notebook interpreter. Runtime: Python.

Pinned source: [https://github.com/geekan/MetaGPT](https://github.com/geekan/MetaGPT); revision/version `11cdf466d042aece04fc6cfd13b28e1a70341b1f`.

asyncio concurrent role rounds; model command maps; owned notebook kernels and persistent shells

owns resident team routing, memory, planning and computation; consumes model/browser/RAG services

Inspection: Selected source excerpts trace exposed entry/loop/request/action/result/state boundaries and relevant capability mechanisms; listed tests are read as source only.

Limits of this study: No acquired code executed, dependencies installed or live providers invoked. Claims concern this pinned snapshot; untraced catalog tools and dependency internals are not inferred.

## Actions

### Team.hire / run_project / run / serialize / deserialize

Surface: operator / Python API.

Input: roles, idea, round limit/budget, snapshot path

Result: environment history and restored Team

Lifecycle: concurrent role rounds; exceptions snapshot selected state

Authority: Python author controls role implementations, configuration and budget

Evidence: [team](#evidence-team), [env](#evidence-env), [recover](#evidence-recover).

### Role.run / set_actions / react modes

Surface: programmable API.

Input: message, action classes, ReAct/order/plan configuration

Result: Messages/TaskResult and peer publications

Lifecycle: observe→think→act; base publishes at run end

Authority: role author chooses methods/actions, model can select states

Evidence: [observe](#evidence-observe), [react](#evidence-react).

### TeamLeader.publish_team_message

Surface: model command.

Input: content and named send_to

Result: peer inbox message; leader state reset

Lifecycle: resident message now, peer responds on scheduling round

Authority: leader model controls delegation/routing

Evidence: [leader](#evidence-leader), [env](#evidence-env), [mgx](#evidence-mgx).

### Plan.append_task / reset_task / replace_task / finish_current_task / end

Surface: model commands.

Input: task fields/IDs and optional completion

Result: mutable plan/status and termination

Lifecycle: serial commands; planner review may clear working memory

Authority: RoleZero model configured with plan command map

Evidence: [map](#evidence-map), [commands](#evidence-commands), [review](#evidence-review).

### Editor.create_file / write / read / edit_file_by_replace / search_dir / similarity_search

Surface: model commands.

Input: paths, file content/replacements, search query

Result: editor values/results or errors

Lifecycle: serial command map; catalog includes more editor navigation operations

Authority: configured RoleZero owns local editor capability

Evidence: [map](#evidence-map), [commands](#evidence-commands).

### Browser.goto / click / type / scroll / tab_focus / close_tab

Surface: model commands.

Input: URL, locator/input/tab arguments

Result: browser state/results

Lifecycle: awaited mapped operations

Authority: configured RoleZero model

Evidence: [map](#evidence-map), [commands](#evidence-commands).

### Terminal.run_command / get_stdout_output / close

Surface: model command / Python API.

Input: command string, daemon flag

Result: text output; background queue; no stable job receipt

Lifecycle: persistent shell sentinel stream; daemon reader bug and EOF loop limits

Authority: Engineer2 map exposes terminal; substring exclusion not approval system

Evidence: [engineer](#evidence-engineer), [terminal](#evidence-terminal), [terminalread](#evidence-terminalread), [commands](#evidence-commands).

### DataInterpreter / ExecuteNbCode.run / terminate / reset

Surface: model-generated code / Python API.

Input: Python code or markdown and timeout

Result: output string/success, notebook cells/file

Lifecycle: persistent kernel between cells; timeout interrupt; plan end terminates

Authority: model code can import/execute in owned local kernel

Evidence: [di](#evidence-di), [kernel](#evidence-kernel), [cell](#evidence-cell).

### ask_human / reply_to_human

Surface: model command.

Input: question or response content

Result: input text or claimed-success string

Lifecycle: MGX local human input; no remote delivery proof in default reply

Authority: optional model human request; autonomous planner auto-run bypasses review

Evidence: [mgx](#evidence-mgx), [commands](#evidence-commands), [review](#evidence-review).

### Memory.get_by_content / try_remember / RoleZeroLongTermMemory.get

Surface: Python API.

Input: keyword/recent-count/new requirement

Result: matching resident or retrieved old messages

Lifecycle: mutable memory and optional retrieval dependency

Authority: role author; no dedicated model repair operation

Evidence: [memory](#evidence-memory), [rag](#evidence-rag).

## Capabilities

### filesystem

**I — Files** (source): RoleZero editor and Engineer2 terminal/code actions expose filesystem operations; notebook writes code.ipynb after cells.

Evidence: [map](#evidence-map), [engineer](#evidence-engineer), [cell](#evidence-cell).

### processes

**L — OS programs** (source): Persistent terminal and notebook kernel are owned resident processes; terminal daemon has no handle and reader does not enable background queue, timeout or EOF termination.

Evidence: [terminal](#evidence-terminal), [terminalread](#evidence-terminalread), [kernel](#evidence-kernel).

### code-actions

**I — Code actions** (source): DataInterpreter model writes executable Python cells with recommended tool information and retries against actual results.

Evidence: [di](#evidence-di), [cell](#evidence-cell).

### persistent-kernel

**L — Kernel** (source): Notebook kernel reuses state across cells; timeout interrupts and dead-kernel reset loses live values; plan-and-act termination closes it at run end. Harness owns kernel rather than consuming independent shared service.

Evidence: [kernel](#evidence-kernel), [cell](#evidence-cell), [di](#evidence-di).

### standing-database

**S — Standing DB** (source): Optional Chroma/RAG memory supplies persistent retrieval of selected older messages; no general standing SQL/query interface traced in role/tool/kernel paths.

Evidence: [rag](#evidence-rag).

### workflow-programming

**I — Workflows** (source): Python roles/actions, watch/route rules, reaction strategies and model-editable plans express workflows; role command maps are subclass-extensible.

Evidence: [react](#evidence-react), [map](#evidence-map), [leader](#evidence-leader).

### multi-model

**I — Models** (source): Role/action private model/config can override shared context, permitting mixed-model peers.

Evidence: [private](#evidence-private), [env](#evidence-env).

### live-collaboration

**I — Peer chat** (source): Resident role inboxes, routed messages and TeamLeader model dispatch coordinate peers; RoleZero observes between actions. Standard environment is in-process, round-based, with optimistic route success.

Evidence: [env](#evidence-env), [leader](#evidence-leader), [zero](#evidence-zero), [mgx](#evidence-mgx).

### concurrent-work

**I — Concurrency** (source): Non-idle roles run concurrently via gather per round; model commands within each role are serial, and DataInterpreter cells share one executor.

Evidence: [env](#evidence-env), [commands](#evidence-commands), [di](#evidence-di).

### steering-interrupt

**L — Steer/interrupt** (source): RoleZero picks up inbox news between actions and model ask_human stop ends role. Notebook timeout interrupts; exception snapshot is not global cancellation/drain of other role effects.

Evidence: [zero](#evidence-zero), [commands](#evidence-commands), [cell](#evidence-cell), [recover](#evidence-recover).

### turn-redefinition

**S — Turn program** (source): Overridable observe/think/act/react, selectable strategies and mutable command map provide Python turn design; not a uniform model-exposed hot turn-program API.

Evidence: [react](#evidence-react), [map](#evidence-map), [zero](#evidence-zero).

### compaction

**L — Compaction** (source): Provider offers configurable pre/post message/token truncation; RoleZero uses bounded recent memory and optional older retrieval. No managed compaction event, alternatives or revisioned projections.

Evidence: [compress](#evidence-compress), [think](#evidence-think), [rag](#evidence-rag).

### context-repair

**S — Repair** (source): Mutable memory queries, optional RAG and Team snapshot restore can recover selected state. Exception recovery deletes latest observed item to force reobservation, risking effect replay; no authoritative original/context revision repair.

Evidence: [memory](#evidence-memory), [rag](#evidence-rag), [team](#evidence-team), [recover](#evidence-recover).

### original-audit

**L — Original audit** (source): Debug environment memory, masked provider logging, overwritten notebook and Team snapshots record selected state; memories delete/clear and recovery changes them. No full raw request/tool/process audit traced.

Evidence: [env](#evidence-env), [provider](#evidence-provider), [cell](#evidence-cell), [memory](#evidence-memory), [recover](#evidence-recover).

### audit-query

**S — Audit query** (source): Role memory searches by content/cause/role and RAG retrieve selected messages; no durable full-event audit query.

Evidence: [memory](#evidence-memory), [rag](#evidence-rag).

### hot-change

**S — Hot change** (source): Python role/tool/config setters support reconfiguration, but cached private model and mutable map have no affected-turn/workflow activation fence or source reload contract.

Evidence: [private](#evidence-private), [map](#evidence-map), [react](#evidence-react).

### rebuild-continuity

**L — Rebuild continuity** (source): Selected Team/role state serializes after ordinary exceptions/KeyboardInterrupt, and observed message is made retryable; live kernel/terminal handles are excluded and unresolved external effects lack dedup receipts.

Evidence: [team](#evidence-team), [recover](#evidence-recover), [di](#evidence-di), [terminal](#evidence-terminal).

### remote-services

**S — Remote** (source): Provider services and optional browser/search/RAG extensions are consumed; peer environment, shell and notebook are locally owned. Remote MGX human methods explicitly require override.

Evidence: [private](#evidence-private), [map](#evidence-map), [mgx](#evidence-mgx), [rag](#evidence-rag).

### self-improvement

**S — Self-improve** (source): Experience cache and code error-repair loops plus editor/terminal allow programming improvements; no governed harness refit/research/adoption loop or compiled continuity traced.

Evidence: [think](#evidence-think), [di](#evidence-di), [engineer](#evidence-engineer).

### complaints

**? — Complaints** (inspection-limit): No dedicated model frustration report with captured agent state and separate tracked repair queue established in role, team, memory and tool paths inspected.

### authority

**L — Authority** (source): Mapped model commands execute directly; autonomous planner accepts successful code while manual reviewer may accept failures. Terminal substring exclusions and post-execution pip failure label are limited policy mechanisms.

Evidence: [commands](#evidence-commands), [review](#evidence-review), [terminal](#evidence-terminal), [cell](#evidence-cell).

### evaluation

**L — Evaluation** (source): Read notebook tests exercise real cross-cell variables, exception and timeout behavior; team serialization tests patch writer and assert reconstructed role types/count, not process/request/effect continuity.

Evidence: [kernel](#evidence-kernel), [cell](#evidence-cell), [recover](#evidence-recover).

### time-order

**L — Time/order** (source): Round gather defines coarse barriers and role inbox/memory order; fixed shell sentinel and optimistic no-recipient publish cannot establish durable command/delivery causal receipts.

Evidence: [env](#evidence-env), [observe](#evidence-observe), [terminalread](#evidence-terminalread).

## Inspected test oracles

- [quarantine/metagpt/tests/metagpt/actions/di/test_execute_nb_code.py](../../../quarantine/metagpt/tests/metagpt/actions/di/test_execute_nb_code.py): Cross-cell state, arithmetic exception, timeout and termination Oracle: Source invokes actual notebook kernel: later cell asserts z==3, division fails, timeout prefix and cleared kernel handle asserted; tests unexecuted here, do not prove shared service/refit. Read, **not executed**.
- [quarantine/metagpt/tests/metagpt/serialize_deserialize/test_team.py](../../../quarantine/metagpt/tests/metagpt/serialize_deserialize/test_team.py): Role type and count reconstruction Oracle: Pydantic dump/validate checks role types; save/recover tests patch Team.serialize to different mock writer. No live process, tool-effect or provider-request recovery oracle. Read, **not executed**.

## Useful mechanisms

- Model-editable plans and per-role provider overrides support heterogeneous workflow teams.
- Notebook executes model-authored cells with shared variable state and result-driven repair.
- RoleZero re-observes peer/operator messages between actions.

## Material limits

- Local round-based routing reports success even with no recipient; default human reply returns success without transport.
- Terminal background reader omits daemon flag, so advertised queue stays unpopulated on that path; EOF can loop indefinitely.
- Exception recovery reobserves message without external-effect idempotency; live kernel/shell excluded from snapshots.
- Compaction is token/message cutting; memory/snapshot/notebook are mutable selected-state records.

## Arconaut design questions

- Should collaboration scheduling use independent participants rather than gather barriers?
- How should computation ownership move to shared independent kernels without losing code/result ergonomics?
- What durable effect receipts make role recovery safe when an action partly completed?

## Evidence

### Evidence team

[quarantine/metagpt/metagpt/team.py:59–138](../../../quarantine/metagpt/metagpt/team.py#L59): Team hire/publish/run bounds rounds and budget; snapshot serialization records model data/context; normal run archives project Git, exception decorator handles snapshots.

### Evidence env

[quarantine/metagpt/metagpt/environment/base_env.py:175–247](../../../quarantine/metagpt/metagpt/environment/base_env.py#L175): Environment routes to resident role inboxes, appends debug memory and returns True even without recipients; non-idle role.run coroutines gather concurrently per round.

### Evidence observe

[quarantine/metagpt/metagpt/roles/role.py:340–427](../../../quarantine/metagpt/metagpt/roles/role.py#L340): Role selects fixed/order/model actions and observes watched/direct messages after inbox pop; history is role memory, with latest observed item retained for recovery.

### Evidence react

[quarantine/metagpt/metagpt/roles/role.py:454–554](../../../quarantine/metagpt/metagpt/roles/role.py#L454): Base role chooses ReAct/order/plan-and-act; planner loops tasks, role memory is updated and final response is published to peers.

### Evidence mgx

[quarantine/metagpt/metagpt/environment/mgx/mgx_env.py:18–88](../../../quarantine/metagpt/metagpt/environment/mgx/mgx_env.py#L18): MGX leader/direct/public routing rewrites message content; ask_human reads local input; reply_to_human returns claimed success string without transport delivery implementation.

### Evidence leader

[quarantine/metagpt/metagpt/roles/di/team_leader.py:36–85](../../../quarantine/metagpt/metagpt/roles/di/team_leader.py#L36): Model-visible publish_team_message targets named peer and resets leader state to wait; live room is resident environment, not remote IRC transport.

### Evidence map

[quarantine/metagpt/metagpt/roles/di/role_zero.py:103–171](../../../quarantine/metagpt/metagpt/roles/di/role_zero.py#L103): RoleZero initializes autonomous planner, exposes plan/human/editor/browser command map and subclass extensions.

### Evidence think

[quarantine/metagpt/metagpt/roles/di/role_zero.py:198–274](../../../quarantine/metagpt/metagpt/roles/di/role_zero.py#L198): RoleZero builds plan/tool/context request, processes observations and calls configured LLM through optional experience cache; memory subset and output normalization affect prompt.

### Evidence zero

[quarantine/metagpt/metagpt/roles/di/role_zero.py:280–335](../../../quarantine/metagpt/metagpt/roles/di/role_zero.py#L280): RoleZero parses model command list, stores response/results, observes new inbox items between actions and can ask human to extend large action bound.

### Evidence commands

[quarantine/metagpt/metagpt/roles/di/role_zero.py:385–447](../../../quarantine/metagpt/metagpt/roles/di/role_zero.py#L385): Command map invokes coroutine/sync functions serially; first unknown/error breaks batch; end/finish/human/terminal handled specially, human stop invokes end.

### Evidence engineer

[quarantine/metagpt/metagpt/roles/di/engineer2.py:95–106](../../../quarantine/metagpt/metagpt/roles/di/engineer2.py#L95): Normal Engineer2 exposes native terminal plus code write/review/fix and pull/deploy functions, extending RoleZero map.

### Evidence di

[quarantine/metagpt/metagpt/roles/di/data_interpreter.py:42–148](../../../quarantine/metagpt/metagpt/roles/di/data_interpreter.py#L42): DataInterpreter produces and executes notebook code with three-attempt repair, stores code/results in working memory; plan completion/error terminates kernel.

### Evidence kernel

[quarantine/metagpt/metagpt/actions/di/execute_nb_code.py:89–139](../../../quarantine/metagpt/metagpt/actions/di/execute_nb_code.py#L89): Executor starts a notebook kernel and reuses it; terminate kills it and clears handles; reset terminates/builds new kernel rather than replaying old values.

### Evidence cell

[quarantine/metagpt/metagpt/actions/di/execute_nb_code.py:225–282](../../../quarantine/metagpt/metagpt/actions/di/execute_nb_code.py#L225): Cell timeout interrupts and reports failure; dead kernel resets; Python/markdown cells persist notebook to fixed code.ipynb. Executed pip is marked failure after execution, not blocked before.

### Evidence terminal

[quarantine/metagpt/metagpt/tools/libs/terminal.py:47–107](../../../quarantine/metagpt/metagpt/tools/libs/terminal.py#L47): Terminal owns persistent shell, substring-filters forbidden commands and appends sentinel. Daemon launches reader without passing daemon=True, so expected queue path is not enabled.

### Evidence terminalread

[quarantine/metagpt/metagpt/tools/libs/terminal.py:134–181](../../../quarantine/metagpt/metagpt/tools/libs/terminal.py#L134): Reader consumes stdout until fixed sentinel; EOF loops instead of breaking; no timeout/exit-code result. Background queue is populated only when reader daemon argument True; close waits without escalation.

### Evidence provider

[quarantine/metagpt/metagpt/provider/base_llm.py:179–209](../../../quarantine/metagpt/metagpt/provider/base_llm.py#L179): Provider request formatting logs masked debug prompt, applies configured truncation and awaits completion; debug logging is not raw immutable request audit.

### Evidence compress

[quarantine/metagpt/metagpt/provider/base_llm.py:340–412](../../../quarantine/metagpt/metagpt/provider/base_llm.py#L340): Configurable pre/post token/message cuts retain system prefix and earliest/latest content; token allowance becomes character slicing at cutoff, no compaction event/repair revision.

### Evidence private

[quarantine/metagpt/metagpt/context_mixin.py:27–96](../../../quarantine/metagpt/metagpt/context_mixin.py#L27): Role/action private configuration and provider override shared context; lazy private model permits different peer models but configuration changes do not automatically replace cached provider.

### Evidence recover

[quarantine/metagpt/metagpt/utils/common.py:675–714](../../../quarantine/metagpt/metagpt/utils/common.py#L675): Team exception wrapper logs then serializes; role exceptions delete latest observed message so it can be reobserved. No external-effect receipt or idempotency prevents repeated action effects.

### Evidence memory

[quarantine/metagpt/metagpt/memory/memory.py:27–91](../../../quarantine/metagpt/metagpt/memory/memory.py#L27): Memory supports mutable add/delete/clear and content/role/cause lookup; transcript is in-memory mutable state, not complete audit.

### Evidence rag

[quarantine/metagpt/metagpt/memory/role_zero_memory.py:31–129](../../../quarantine/metagpt/metagpt/memory/role_zero_memory.py#L31): Optional RoleZero Chroma memory stores selected older messages and retrieves related messages for a new requirement under conditions; not authoritative transcript repair.

### Evidence review

[quarantine/metagpt/metagpt/strategy/planner.py:119–153](../../../quarantine/metagpt/metagpt/strategy/planner.py#L119): Auto review accepts successful TaskResult; manual confirmation can accept failed code and successful task clears working memory.

