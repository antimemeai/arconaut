# swe-agent

Configurable coding agent with one tool action per reply, uploaded command bundles, continuing shell state, history projections, bounded requery/autosubmit and sequential model-reviewed attempt selection. Replay means fresh action execution, and synthetic colleague sampling is not live peer collaboration.

Role: coding agent. Runtime: Python, shell command bundles.

Pinned source: [https://github.com/princeton-nlp/SWE-agent](https://github.com/princeton-nlp/SWE-agent); revision/version `3ea751c087f32b16e039a2233dd6eefecef325d5`.

Synchronous Python agent loop and LiteLLM requests; asyncio.run adapts SWE-ReX external deployment/runtime calls with one persistent bash session. Benchmarks overlap independent instances through ThreadPoolExecutor.

Owns agent history/trajectory/review strategy and deployment client lifecycle; consumes SWE-ReX session/process implementation and model provider. Tool/state JSON is per-environment, not a shared compute/DB governor.

Inspection: CLI→environment setup→model/action-sampler→parser/filter→SWE-ReX command/interrupt→state/history/trajectory→continuation; default editor/submit executors, optional source bundle catalogs, retry/reviewer strategies, history projection, replay, benchmark/evaluation hooks and fault-oracle tests.

Limits of this study: Source read only, reference not executed. SWE-ReX/LiteLLM/sb-cli dependency internals, full optional browser/windowed executors, OS process-tree semantics and all alternative reviewer implementations are outside trace. Source-inferred editor defect: fresh JSON getter mutations are not written back, so advertised undo history is not established. Single-run exception cleanup lacks a finally stop call.

## Actions

### bash

Surface: model tool.

Input: command string translated by function-calling parser

Result: continuing-shell output and subsequent environment state; timeout/retry/final submission flow

Lifecycle: one model action at a time; persistent SWE-ReX bash session rather than mini fresh subshell; no exposed resumable process handle

Authority: model without routine approval prompt; configurable prefix/exact command filter and deployment constraints

Evidence: [e8](#evidence-e8), [e9](#evidence-e9), [e10](#evidence-e10), [e15](#evidence-e15), [e16](#evidence-e16).

### str_replace_editor

Surface: model tool, default configured bundle.

Input: command=view/create/str_replace/insert/undo_edit; absolute path and operation-specific text/range

Result: bounded file/directory view or edit result/error; replace rejects ambiguous old text

Lifecycle: fresh command program stores intended history in registry JSON; source-inferred failure to write it back undermines advertised undo

Authority: model through same shell dispatch; file path existence/absolute-path and action constraints

Evidence: [e21](#evidence-e21), [e22](#evidence-e22), [e23](#evidence-e23), [e24](#evidence-e24), [e25](#evidence-e25), [e26](#evidence-e26), [e27](#evidence-e27).

### submit / submit -f; exit

Surface: model tool or shell control.

Input: no args, force used by harness autosubmission; literal exit action

Result: staged Git patch wrapped in submission token, review-stage prompts or explicit exit

Lifecycle: configured submit stages return prompts before final submission; agent recognizes sentinel; force bypasses review

Authority: model default submit; host force on errors; prompts are not an independent correctness gate

Evidence: [e14](#evidence-e14), [e16](#evidence-e16), [e28](#evidence-e28).

### optional find_file / search_dir / search_file

Surface: config-defined bundle catalog.

Input: name/pattern or search term and optional directory/file

Result: configured file-search command output

Lifecycle: available only when bundle selected/installed; command executors not independently traced here

Authority: model through generated schema and shell filter; metadata contract source only

Evidence: [e10](#evidence-e10), [e11](#evidence-e11), [e42](#evidence-e42).

### optional open / create / goto / scroll_up / scroll_down

Surface: config-defined bundle catalog.

Input: file/line selection or no-argument window movement

Result: windowed file content and state metadata

Lifecycle: configured state-command/file window; alternate edit/rewrite/replace bundles can add edit/insert

Authority: model with selected bundle; catalog semantics, not a full executor trace

Evidence: [e11](#evidence-e11), [e43](#evidence-e43).

### optional open_site / close_site / screenshot_site / click_mouse / double_click_mouse / move_mouse / drag_mouse / type_text / scroll_on_page / execute_script_on_page / navigate_back / navigate_forward / reload_page / wait_time / press_keys_on_page / set_browser_window_size / get_console_output

Surface: config-defined browser bundle catalog.

Input: URL; mouse coordinates/path/button; text; scroll deltas; JS script; milliseconds; keys; window size; no-arg history/screenshot/console

Result: declared browser content/screenshot/action/console responses

Lifecycle: selected browser bundle operates outside default action surface; browser lifecycle implementation beyond this trace

Authority: model if bundle installed, source catalog contract only

Evidence: [e11](#evidence-e11), [e44](#evidence-e44).

### optional filemap / view_image

Surface: config-defined bundle catalog.

Input: Python-file path or image-file path

Result: declared shortened file map or image view

Lifecycle: selected tool command programs, not default configured tools

Authority: model through generated bundle surface; detailed execution outside this trace

Evidence: [e11](#evidence-e11), [e45](#evidence-e45), [e46](#evidence-e46).

### DefaultAgent.setup / step / run; ToolHandler.install/reset; add_hook

Surface: programmable API.

Input: model/tools/templates/processors, SWEEnv, problem statement and host stage hooks

Result: StepOutput/trajectory/info and mutation/observation hooks

Lifecycle: synchronous stages, configuration deep-copied and catalogs cached; programmer can replace/mutate objects

Authority: host API, not normal model action to redefine the turn

Evidence: [e2](#evidence-e2), [e4](#evidence-e4), [e5](#evidence-e5), [e10](#evidence-e10), [e13](#evidence-e13), [e34](#evidence-e34).

### RetryAgent / ScoreRetryLoop / ChooserRetryLoop

Surface: programmable configured workflow.

Input: attempt agent configs, cost/attempt/accept limits and reviewer/chooser model config

Result: review scores/selection, preserved per-attempt trajectories and final best solution

Lifecycle: sequential task attempts with hard environment reset and reviewer cost accounting

Authority: operator/program config governs strategy; no model-driven harness experiment/rebuild protocol

Evidence: [e29](#evidence-e29), [e30](#evidence-e30), [e31](#evidence-e31).

### AskColleagues action sampler

Surface: programmable configured strategy.

Input: sample count and one configured model plus current trajectory/history

Result: combined proposed ideas and one final action selection

Lifecycle: multiple synchronous completions then comparison; one shared model, no messages to durable peers

Authority: operator/program selects sampler; model receives synthesized colleague prompt

Evidence: [e32](#evidence-e32), [e33](#evidence-e33).

### save_trajectory / get_trajectory_data

Surface: programmable API.

Input: current history/steps/info/replay config and trajectory path

Result: JSON snapshot including successful and failed-step projections

Lifecycle: whole-file replacement after steps; raw wire/stream and complete process events not captured

Authority: host/operator; saved data can be inspected or supplied as demos

Evidence: [e18](#evidence-e18), [e19](#evidence-e19).

### sweagent run replay / traj-to-demo / inspector

Surface: operator utilities.

Input: saved trajectory and optional deployment/config override, demo conversion or trajectory browse

Result: reexecuted actions/new observations, demo or saved-trajectory view

Lifecycle: replay creates new agent/environment and ReplayModel; not resume of unresolved work

Authority: operator, re-execution is an action; only source inspected here

Evidence: [e35](#evidence-e35), [e36](#evidence-e36).

### SWEEnv.start / communicate / interrupt_session / read_file / close

Surface: programmable environment API.

Input: deployment config; continuing-shell command/timeout/check; filesystem path; interrupt or stop

Result: shell/file observations or backend errors

Lifecycle: start creates SWE-ReX session; interrupt is explicit action; close stops owned deployment; runtime external

Authority: host consumes deployment/runtime API; implementation of session/process isolation is outside source boundary

Evidence: [e15](#evidence-e15).

### batch benchmark / SweBenchEvaluate hook

Surface: operator evaluation support.

Input: instance list/worker count, predictions and supported benchmark subset

Result: per-instance attempts/patches, external evaluation call and report

Lifecycle: threaded independent runs; interruption waits for active tasks; evaluation invokes separate sb-cli

Authority: operator, not live agent-team or self-improvement control plane

Evidence: [e37](#evidence-e37), [e38](#evidence-e38), [e39](#evidence-e39).

## Capabilities

### filesystem

**I — Files** (source): Default editor view/create/replace/insert plus bash files are concrete. Editor outputs clip before harness capture; advertised undo has source-inferred history write-back failure, preserved explicitly.

Evidence: [e21](#evidence-e21), [e22](#evidence-e22), [e23](#evidence-e23), [e24](#evidence-e24), [e25](#evidence-e25), [e26](#evidence-e26), [e27](#evidence-e27).

### processes

**L — OS programs** (source): Consumes a continuing SWE-ReX bash session with command timeout/interrupt, but no normal model ongoing-work handle or source proof of descendant reaping. RunSingle closes deployment only after normal completion, without finally.

Evidence: [e1](#evidence-e1), [e15](#evidence-e15), [e16](#evidence-e16).

### code-actions

**S — Code actions** (source): Bash executes scripts/interpreters and optional browser catalog exposes execute_script_on_page. No separate persistent code-cell execution surface traced; browser executor remains outside this study.

Evidence: [e8](#evidence-e8), [e15](#evidence-e15), [e44](#evidence-e44).

### persistent-kernel

**S — Kernel** (source): Persistent bash shell retains cwd/env/state across actions via SWE-ReX create_session/run_in_session. This supplies shell continuity, not a general Python/JS kernel or shared compute service.

Evidence: [e13](#evidence-e13), [e15](#evidence-e15).

### standing-database

**S — Standing DB** (source): Tool registry/state JSON retain command data within environment; editor intends file-history persistence. No queryable shared standing DB is exposed, and editor getter mutation is not written back in inspected paths.

Evidence: [e12](#evidence-e12), [e13](#evidence-e13), [e24](#evidence-e24), [e26](#evidence-e26), [e27](#evidence-e27).

### workflow-programming

**S — Workflows** (source): Configured tool bundles, history processors, stage hooks, action sampling and sequential reviewer/retry loops are actual host strategy composition. Model gets installed commands, not arbitrary new turn/workflow definitions.

Evidence: [e10](#evidence-e10), [e11](#evidence-e11), [e29](#evidence-e29), [e30](#evidence-e30), [e31](#evidence-e31), [e32](#evidence-e32), [e34](#evidence-e34).

### multi-model

**I — Models** (source): Retry attempts use distinct copied model configs, plus configured reviewer/chooser models. This is sequential attempt/review routing; AskColleagues itself samples the same configured model.

Evidence: [e29](#evidence-e29), [e30](#evidence-e30), [e31](#evidence-e31), [e32](#evidence-e32), [e33](#evidence-e33).

### live-collaboration

**L — Peer chat** (source): AskColleagues is synthetic multiple-completion discussion and retry agents run sequentially. No durable child/peer mailbox or busy-colleague steering established in inspected action sampler/agent/environment interfaces.

Evidence: [e29](#evidence-e29), [e30](#evidence-e30), [e32](#evidence-e32), [e33](#evidence-e33).

### concurrent-work

**S — Concurrency** (source): Benchmark thread pool overlaps independent problem/environment instances. Normal agent has exactly one tool action per model reply; no general ongoing workflow/task registry.

Evidence: [e8](#evidence-e8), [e37](#evidence-e37).

### steering-interrupt

**L — Steer/interrupt** (source): CommandTimeoutError triggers an explicit session interrupt when below consecutive-timeout cap. KeyboardInterrupt is reraised, and batch cancel preserves running instances; no interactive live steering queue or no-active-request guarantee.

Evidence: [e16](#evidence-e16), [e19](#evidence-e19), [e37](#evidence-e37).

### turn-redefinition

**S — Turn program** (source): Programmer hooks and copied configurable model/tools/processors/action-sampling/retry stages permit host variation. Completed loop/flow semantics remain fixed Python methods; no model-facing hot turn replacement.

Evidence: [e2](#evidence-e2), [e4](#evidence-e4), [e5](#evidence-e5), [e29](#evidence-e29), [e34](#evidence-e34).

### compaction

**S — Compaction** (source): LastNObservations elides selected old observations in a provider-facing projection, with keep/remove tags and cache processors. Original stored history is separate; this is deterministic projection rather than model-managed summary/repair event.

Evidence: [e3](#evidence-e3), [e20](#evidence-e20), [e41](#evidence-e41).

### context-repair

**S — Repair** (source): Unprojected history and full trajectory observations/requeries survive for operator/program inspection or demos/replay. No model-authorized repair/restore/merge operation was traced; bundle-side clipping has already lost data.

Evidence: [e3](#evidence-e3), [e17](#evidence-e17), [e18](#evidence-e18), [e20](#evidence-e20), [e23](#evidence-e23), [e35](#evidence-e35), [e36](#evidence-e36).

### original-audit

**L — Original audit** (source): Trajectory records request history, thought/action/output/observation/state and failed requery steps, with debug provider logs. It replaces JSON after steps; normalized provider fields lose raw response metadata, and bundle clipping precedes capture. This is not original-everything event audit.

Evidence: [e4](#evidence-e4), [e6](#evidence-e6), [e17](#evidence-e17), [e18](#evidence-e18), [e19](#evidence-e19), [e23](#evidence-e23).

### audit-query

**S — Audit query** (source): Saved trajectories/history, inspector and replay/demo utilities support manual/program analysis. No model/native event-query service with complete originals/lineage established.

Evidence: [e18](#evidence-e18), [e35](#evidence-e35), [e36](#evidence-e36).

### hot-change

**L — Hot change** (source): Agent/tool configs are deep-copied, tool schemas/docs cached and bundles installed during setup. Host hooks can mutate stages but no file watcher or explicit after-turn activation/refit event is established.

Evidence: [e2](#evidence-e2), [e10](#evidence-e10), [e13](#evidence-e13), [e34](#evidence-e34).

### rebuild-continuity

**L — Rebuild continuity** (source): Replay constructs new environment/config/ReplayModel and reexecutes saved actions. Retry hard reset reconstructs attempts. Neither transfers live provider/session/program state or hands context to an outpost during executable replacement.

Evidence: [e29](#evidence-e29), [e35](#evidence-e35), [e36](#evidence-e36).

### remote-services

**I — Remote** (source): Consumes configured LiteLLM provider/API endpoints/fallbacks and SWE-ReX deployment/runtime; source exposes client lifecycle boundaries rather than adopting provider/deployment internals.

Evidence: [e6](#evidence-e6), [e15](#evidence-e15).

### self-improvement

**S — Self-improve** (source): Retry/reviewer/action comparison search alternative task solutions and choose scores under budgets. This can inform autodroit design, but no measured harness change/selection/refit/reinhabitation protocol is traced.

Evidence: [e29](#evidence-e29), [e30](#evidence-e30), [e31](#evidence-e31), [e32](#evidence-e32).

### complaints

**? — Complaints** (source): No model complaint with captured agent state and external DB/bead tracking established in inspected loops/tool catalog/hooks/retry/evaluation/replay.

### authority

**I — Authority** (source): Model commands normally execute without confirmation, constrained by configured prefix/exact/regex filters and environment/tool validation. These coarse filters are not a general sandbox or shell-language authorization proof.

Evidence: [e9](#evidence-e9), [e14](#evidence-e14), [e16](#evidence-e16).

### evaluation

**I — Evaluation** (source): Source scripted-model tests distinguish trajectory of failed requery from successful provider history. Configurable review scores guide attempt choice; external sb-cli performs benchmark grading when enabled. History-count tests do not prove repair or invariance, and none ran here.

Evidence: [e31](#evidence-e31), [e38](#evidence-e38), [e39](#evidence-e39), [e40](#evidence-e40), [e41](#evidence-e41).

### time-order

**L — Time/order** (source): Per-action perf_counter durations and cumulative execution budget are explicit; total budget is checked before the next step and does not interrupt a current command/provider. Synchronous order and backend deadlines are not a global causal clock.

Evidence: [e4](#evidence-e4), [e16](#evidence-e16).

## Inspected test oracles

- [quarantine/swe-agent/tests/test_agent.py](../../../quarantine/swe-agent/tests/test_agent.py): Failed-format retry must remain study data while provider history proceeds correctly. Oracle: Scripted model/dummy runtime requires trajectory lengths 2/3/4 for requery and later calls, successful action/observation history lengths 5/7, and budget exit; controlled state oracle, not a real provider/session interrupt. Read, **not executed**.
- [quarantine/swe-agent/tests/test_history_processors.py](../../../quarantine/swe-agent/tests/test_history_processors.py): Last-N omission and tool-call tag selection. Oracle: Expected omitted count is derived from total observations minus retained count; edits receive tag. It does not check exact original bytes retained, tagged removal interactions or model-driven repair. Read, **not executed**.

## Useful mechanisms

- Saved query/trajectory and retained original history are distinct from projected provider context, making recoverability scope explicit.
- Configurable command bundles have duplicate-name detection, hidden-tool validation and generated schemas; client/session boundary is straightforward.
- Retry strategy keeps attempts and reviewer model stats, separating task search from the ordinary action loop.

## Material limits

- SWE-ReX/LiteLLM/sb-cli dependency internals, full optional browser/windowed executors, OS process-tree semantics and all alternative reviewer implementations are outside trace. Source-inferred editor defect: fresh JSON getter mutations are not written back, so advertised undo history is not established. Single-run exception cleanup lacks a finally stop call.

## Arconaut design questions

- Can command timeout/cancel and KeyboardInterrupt prove owned deployment/session/process cleanup independently of ordinary success paths?
- Should editor undo persist a complete before-image with stable string keys, and can a direct cross-process replace→undo oracle challenge the current temporary getter mutation?
- How can model-controlled context projection/repair preserve original audit while dynamically changing what the next request sees?
- Which parts of budgeted reviewer/attempt search transfer to model-governed harness improvement, and what independent measurement decides improvement?
- What explicit activation operation replaces cached tool catalogs and stage programs after the current turn/affected workflows?

## Evidence

### Evidence e1

[quarantine/swe-agent/sweagent/run/run_single.py:170–206](../../../quarantine/swe-agent/sweagent/run/run_single.py#L170): RunSingle creates model/agent/SWEEnv from config, starts environment, runs agent and closes only after ordinary completion; no finally cleanup wraps the run.

### Evidence e2

[quarantine/swe-agent/sweagent/agent/agents.py:443–518](../../../quarantine/swe-agent/sweagent/agent/agents.py#L443): Default agent owns model, tool handler, processors, history/trajectory and counters; from_config deep-copies configuration to isolate instances.

### Evidence e3

[quarantine/swe-agent/sweagent/agent/agents.py:537–550](../../../quarantine/swe-agent/sweagent/agent/agents.py#L537): Provider-facing messages are a filtered history projection chained through processors, preserving a separate original history object.

### Evidence e4

[quarantine/swe-agent/sweagent/agent/agents.py:1006–1061](../../../quarantine/swe-agent/sweagent/agent/agents.py#L1006): Forward deep-copies the query, invokes configured model/action sampler, parses thought/action/tool IDs, then dispatches action; failures retain the partial StepOutput.

### Evidence e5

[quarantine/swe-agent/sweagent/agent/agents.py:1235–1294](../../../quarantine/swe-agent/sweagent/agent/agents.py#L1235): Each step appends model/output/state history and trajectory, updates model stats and hooks; synchronous run saves after steps until done.

### Evidence e6

[quarantine/swe-agent/sweagent/agent/models.py:691–782](../../../quarantine/swe-agent/sweagent/agent/models.py#L691): Concrete LiteLLM path checks context size, submits configured tools/providers/fallbacks, extracts normalized text/tool/thinking fields and accounts response cost before returning.

### Evidence e7

[quarantine/swe-agent/sweagent/agent/models.py:843–864](../../../quarantine/swe-agent/sweagent/agent/models.py#L843): History conversion copies data into role/content/tool-call messages; one tool-call identity is associated with each observation.

### Evidence e8

[quarantine/swe-agent/sweagent/tools/parsing.py:438–459](../../../quarantine/swe-agent/sweagent/tools/parsing.py#L438): Default function-calling parser requires exactly one tool call and maps its args to one command action.

### Evidence e9

[quarantine/swe-agent/sweagent/tools/tools.py:32–65](../../../quarantine/swe-agent/sweagent/tools/tools.py#L32): Default action filter blocks selected prefixes, bare interactive interpreters and regex-constrained commands; this is configurable filtering, not confirmation.

### Evidence e10

[quarantine/swe-agent/sweagent/tools/tools.py:170–213](../../../quarantine/swe-agent/sweagent/tools/tools.py#L170): Tool catalog combines optional bash and selected bundles, rejects duplicate command names and caches generated schemas/docs.

### Evidence e11

[quarantine/swe-agent/sweagent/tools/bundle.py:18–57](../../../quarantine/swe-agent/sweagent/tools/bundle.py#L18): Bundle loads config.yaml metadata, validates hidden names and exposes all configured nonhidden Command objects.

### Evidence e12

[quarantine/swe-agent/sweagent/tools/tools.py:254–267](../../../quarantine/swe-agent/sweagent/tools/tools.py#L254): Reset publishes tool environment variables, registry JSON and state JSON, then executes reset commands in continuing shell state.

### Evidence e13

[quarantine/swe-agent/sweagent/tools/tools.py:292–335](../../../quarantine/swe-agent/sweagent/tools/tools.py#L292): Installation uploads selected bundles, adds bin directories to PATH, sources install.sh and checks availability; state reads JSON from environment.

### Evidence e14

[quarantine/swe-agent/sweagent/tools/tools.py:337–378](../../../quarantine/swe-agent/sweagent/tools/tools.py#L337): State commands update environment state; should_block_action checks configured prefix/exact/regex filters and submission recognizes stdout sentinel.

### Evidence e15

[quarantine/swe-agent/sweagent/environment/swe_env.py:168–232](../../../quarantine/swe-agent/sweagent/environment/swe_env.py#L168): SWEEnv owns deployment lifecycle and creates a persistent SWE-ReX bash session; commands and explicit BashInterruptAction go through that runtime, whose implementation is external.

### Evidence e16

[quarantine/swe-agent/sweagent/agent/agents.py:936–1004](../../../quarantine/swe-agent/sweagent/agent/agents.py#L936): Native action dispatch applies filters, executes guarded command, handles timeout with session interrupt, tracks execution duration/state and interprets retry/forfeit/submission tokens.

### Evidence e17

[quarantine/swe-agent/sweagent/agent/agents.py:714–755](../../../quarantine/swe-agent/sweagent/agent/agents.py#L714): Model history receives rendered/truncated observation; tool identities are preserved and original full observation is a separate trajectory value.

### Evidence e18

[quarantine/swe-agent/sweagent/agent/agents.py:759–811](../../../quarantine/swe-agent/sweagent/agent/agents.py#L759): Trajectory saves history/steps/info/replay configuration as a replaced JSON file; requery mistakes are kept as trajectory entries while omitted from successful model history.

### Evidence e19

[quarantine/swe-agent/sweagent/agent/agents.py:1062–1137](../../../quarantine/swe-agent/sweagent/agent/agents.py#L1062): Format/blocklist/syntax errors requery boundedly; partial failed steps are saved in trajectory; KeyboardInterrupt is reraised rather than becoming live steering.

### Evidence e20

[quarantine/swe-agent/sweagent/agent/history_processors.py:150–178](../../../quarantine/swe-agent/sweagent/agent/history_processors.py#L150): LastNObservations replaces old observation content in copied entries with line/image counts; kept entries and original stored history remain distinct.

### Evidence e21

[quarantine/swe-agent/config/default.yaml:32–69](../../../quarantine/swe-agent/config/default.yaml#L32): Default selects registry, Anthropic editor and multi-stage submit-review bundles with bash and function-calling; cache-control is a projection processor.

### Evidence e22

[quarantine/swe-agent/tools/edit_anthropic/config.yaml:1–56](../../../quarantine/swe-agent/tools/edit_anthropic/config.yaml#L1): Editor tool schema exposes view/create/str_replace/insert/undo_edit with paths, text and ranges; advertised undo requires implementation verification.

### Evidence e23

[quarantine/swe-agent/tools/edit_anthropic/bin/str_replace_editor:28–67](../../../quarantine/swe-agent/tools/edit_anthropic/bin/str_replace_editor#L28): Editor clips large output before it reaches the harness, so full trajectory observation is not an original file/content audit.

### Evidence e24

[quarantine/swe-agent/tools/edit_anthropic/bin/str_replace_editor:350–401](../../../quarantine/swe-agent/tools/edit_anthropic/bin/str_replace_editor#L350): Edit dispatcher branches by operation; file_history getter parses a fresh registry JSON dict on every access, with a separate setter.

### Evidence e25

[quarantine/swe-agent/tools/edit_anthropic/bin/str_replace_editor:516–550](../../../quarantine/swe-agent/tools/edit_anthropic/bin/str_replace_editor#L516): Replace rejects absent, ambiguous or identical old text before writing the replacement.

### Evidence e26

[quarantine/swe-agent/tools/edit_anthropic/bin/str_replace_editor:576–590](../../../quarantine/swe-agent/tools/edit_anthropic/bin/str_replace_editor#L576): Replace appends prior content to the temporary history getter result, with no write-back setter invocation in this path.

### Evidence e27

[quarantine/swe-agent/tools/edit_anthropic/bin/str_replace_editor:634–645](../../../quarantine/swe-agent/tools/edit_anthropic/bin/str_replace_editor#L634): Undo reads/pops another temporary history result and writes it if found; no inspected create/replace/insert caller assigns the history property back to registry.

### Evidence e28

[quarantine/swe-agent/tools/review_on_submit_m/bin/submit:12–54](../../../quarantine/swe-agent/tools/review_on_submit_m/bin/submit#L12): Submit stages a Git patch and returns successive configured review prompts, then emits a submission sentinel; force bypasses stages. Review prompts do not validate changes themselves.

### Evidence e29

[quarantine/swe-agent/sweagent/agent/agents.py:294–326](../../../quarantine/swe-agent/sweagent/agent/agents.py#L294): Retry agent constructs distinct model/tool configs for attempts and hard-resets deployment/environment before next attempt.

### Evidence e30

[quarantine/swe-agent/sweagent/agent/agents.py:409–442](../../../quarantine/swe-agent/sweagent/agent/agents.py#L409): Retry runs sequential attempts, sends submissions to score/chooser reviewers, records reviewer stats, and finally selects an attempt.

### Evidence e31

[quarantine/swe-agent/sweagent/agent/reviewer.py:617–658](../../../quarantine/swe-agent/sweagent/agent/reviewer.py#L617): Score retry bounds cost/attempts/accepts/remaining budget and chooses max reviewer score with shortest-call tie-break; this is task-solution search.

### Evidence e32

[quarantine/swe-agent/sweagent/agent/action_sampler.py:40–94](../../../quarantine/swe-agent/sweagent/agent/action_sampler.py#L40): AskColleagues samples several replies using the same model then asks it to compare ideas; names do not establish durable live peers.

### Evidence e33

[quarantine/swe-agent/sweagent/agent/models.py:783–798](../../../quarantine/swe-agent/sweagent/agent/models.py#L783): Model multi-sample query invokes the same model repeatedly in a loop rather than exposing independent ongoing colleagues.

### Evidence e34

[quarantine/swe-agent/sweagent/agent/hooks/abstract.py:10–54](../../../quarantine/swe-agent/sweagent/agent/hooks/abstract.py#L10): Host hooks observe/mutate agent stages and model-query/message boundaries, supplying programmable composition rather than a model-facing turn language.

### Evidence e35

[quarantine/swe-agent/sweagent/run/run_replay.py:94–127](../../../quarantine/swe-agent/sweagent/run/run_replay.py#L94): Replay reloads saved configuration, applies operator config updates and replaces provider with ReplayModel; no live model/process state is transferred.

### Evidence e36

[quarantine/swe-agent/sweagent/run/run_replay.py:139–171](../../../quarantine/swe-agent/sweagent/run/run_replay.py#L139): Replay extracts saved assistant/tool actions and validates parser compatibility for re-execution.

### Evidence e37

[quarantine/swe-agent/sweagent/run/run_batch.py:276–288](../../../quarantine/swe-agent/sweagent/run/run_batch.py#L276): Benchmark thread pool overlaps independent instances; KeyboardInterrupt cancels pending futures while running instances finish.

### Evidence e38

[quarantine/swe-agent/sweagent/run/hooks/swe_bench_evaluate.py:39–65](../../../quarantine/swe-agent/sweagent/run/hooks/swe_bench_evaluate.py#L39): Evaluation hook constructs external sb-cli commands for supported subsets; execution/grading implementation remains outside snapshot.

### Evidence e39

[quarantine/swe-agent/sweagent/run/hooks/swe_bench_evaluate.py:105–120](../../../quarantine/swe-agent/sweagent/run/hooks/swe_bench_evaluate.py#L105): On completion evaluation calls sb-cli with predictions and records/relocates resulting reports.

### Evidence e40

[quarantine/swe-agent/tests/test_agent.py:149–189](../../../quarantine/swe-agent/tests/test_agent.py#L149): Scripted model/dummy runtime test verifies failed requery remains in trajectory but successful history only has action/observation pairs and later budget exit.

### Evidence e41

[quarantine/swe-agent/tests/test_history_processors.py:26–40](../../../quarantine/swe-agent/tests/test_history_processors.py#L26): History tests check number of elided observations and tags on edit calls, but not exact original-history immutability or model-directed restoration.

### Evidence e42

[quarantine/swe-agent/tools/search/config.yaml:1–37](../../../quarantine/swe-agent/tools/search/config.yaml#L1): Optional search bundle declares find_file/search_dir/search_file arguments; not selected by default config.

### Evidence e43

[quarantine/swe-agent/tools/windowed/config.yaml:1–38](../../../quarantine/swe-agent/tools/windowed/config.yaml#L1): Optional windowed bundle declares open/create/goto/scroll_up/scroll_down and persistent state command.

### Evidence e44

[quarantine/swe-agent/tools/web_browser/config.yaml:1–155](../../../quarantine/swe-agent/tools/web_browser/config.yaml#L1): Optional browser bundle declares URL/screenshot/mouse/text/scroll/script/history/wait/key/window/console operations; browser implementation not deeply traced.

### Evidence e45

[quarantine/swe-agent/tools/filemap/config.yaml:1–9](../../../quarantine/swe-agent/tools/filemap/config.yaml#L1): Optional filemap command declares Python-file map output with lengthy definitions omitted.

### Evidence e46

[quarantine/swe-agent/tools/image_tools/config.yaml:1–9](../../../quarantine/swe-agent/tools/image_tools/config.yaml#L1): Optional view_image command declares file-image view; it is configuration-dependent, not default surface.

