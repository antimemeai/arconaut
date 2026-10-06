# mini-swe-agent

A deliberately small replaceable query→bash→observation loop with explicit recovery flow exceptions, interactive operator modes, raw-output/response preservation and benchmark generation. Tiny core makes boundaries legible; it does not furnish the richer Arconaut control/audit/refit semantics.

Role: coding agent. Runtime: Python.

Pinned source: [https://github.com/SWE-agent/mini-swe-agent.git](https://github.com/SWE-agent/mini-swe-agent.git); revision/version `2afd0fb81bacbf0aacfac9ded6f093c5acd0bf7c`.

Synchronous Python run/query/execute loop; blocking LiteLLM provider call and subprocess.run action. Benchmark throughput uses ThreadPoolExecutor across independent agents. Docker owns one container per environment, not a resident computation namespace.

Owns in-memory conversation/config/counters, optional JSON trajectory and environment/container; consumes model provider and OS commands. No common shared compute/DB control plane is supplied.

Inspection: CLI/class selection through complete default and interactive loops, concrete LiteLLM request and tool/text action parse/result pairing, local/Docker lifecycle, successive-model routing, serialization/inspection, batch scheduling and relevant failure-oracle tests.

Limits of this study: Source read only, reference not executed. LiteLLM and SWE-ReX dependency internals, remote backends beyond Docker, process-tree cancellation and full provider-retry persistence are outside inspection. A source-inferred budget concern: malformed paid replies increment global model stats but raise before DefaultAgent adds returned message cost, so its cost budget can undercount FormatError calls.

## Actions

### bash

Surface: model tool.

Input: command string; any shell-supported program/file operation

Result: combined output, returncode, exception info; full raw_output in trajectory alongside rendered content

Lifecycle: sequential, blocking fresh subshell per action; timeout yields error/partial output, no resumable process handle

Authority: model through selected environment; DefaultAgent has no command approval layer, interactive confirm/yolo/whitelist controls operator interaction

Evidence: [e6](#evidence-e6), [e7](#evidence-e7), [e8](#evidence-e8), [e10](#evidence-e10).

### COMPLETE_TASK_AND_SUBMIT_FINAL_OUTPUT

Surface: model shell convention.

Input: successful command stdout whose first nonblank line is this sentinel; following text is final submission

Result: Submitted flow event and submission text

Lifecycle: ends run, or interactive confirmation permits a new task

Authority: model through ordinary bash output; protocol trusts command stdout, not a separate signed control channel

Evidence: [e8](#evidence-e8), [e9](#evidence-e9).

### text action fence / action_regex

Surface: model text surface.

Input: exactly one mswea_bash_command block or configured regex match

Result: one command action or recoverable FormatError

Lifecycle: alternative model adapter reuses same environment and loop

Authority: model; parser enforces exactly one text action, while tool-call adapter can parse several

Evidence: [e12](#evidence-e12), [e13](#evidence-e13).

### /u /c /y /m /h; Ctrl+C steering

Surface: operator commands.

Input: mode switch, multiline comment or interruption/new-task text

Result: mutable mode and user/rejection/interruption message

Lifecycle: prompt-driven; Ctrl+C caught around step and continues, not asynchronous queued submission

Authority: operator; yolo removes confirmation prompts, human sends commands without model query

Evidence: [e9](#evidence-e9), [e10](#evidence-e10), [e11](#evidence-e11).

### DefaultAgent.run / step / query / execute_actions / add_messages

Surface: programmable API.

Input: task, model/environment implementations and mutable message/config state

Result: flow messages, observations, terminal status/submission

Lifecycle: synchronous duck-typed loop, subclass hooks can replace stages; no hot-load contract

Authority: host programmer/operator; model gets bash, not these loop mutation APIs

Evidence: [e2](#evidence-e2), [e3](#evidence-e3), [e4](#evidence-e4), [e15](#evidence-e15), [e16](#evidence-e16).

### serialize / save

Surface: programmable API and CLI output.

Input: optional output path and extra merged dictionaries

Result: JSON trajectory including message/raw-response/raw-output metadata, config and stats

Lifecycle: whole-file replacement after each step; no native reload/reinhabitation operation

Authority: host/operator can examine or rewrite saved data; no model-authorized context repair operation

Evidence: [e4](#evidence-e4), [e5](#evidence-e5), [e7](#evidence-e7).

### RouletteModel / InterleavingModel

Surface: programmable model adapter.

Input: list of model configs, optional interleaving sequence

Result: one selected model response with model_name

Lifecycle: selects one model each successive synchronous query, shared conversation

Authority: operator/program chooses routing policy, no live agents or model-selected peer channels

Evidence: [e14](#evidence-e14).

### mini CLI class/config selection

Surface: operator command.

Input: --agent-class/--model-class/--environment-class, merged config specs, task/yolo/output

Result: instantiated replaceable loop/model/environment and saved trajectory

Lifecycle: startup configuration; interactive mode mutation thereafter

Authority: operator custom class import; no automatic file/code reload

Evidence: [e1](#evidence-e1), [e15](#evidence-e15).

### LocalEnvironment.execute / DockerEnvironment.execute

Surface: programmable environment API.

Input: command action, cwd and optional timeout; Docker image/interpreter config

Result: structured combined stdout/stderr result or submission flow event

Lifecycle: local fresh shell; Docker owns persistent container but exec launches fresh interpreter; provider/kernel resources not shared-agent governance

Authority: host/plugin environment replaceable; command authority is environment/operator choice

Evidence: [e8](#evidence-e8), [e17](#evidence-e17).

### inspector

Surface: operator utility.

Input: saved trajectory paths, navigation or full JSON inspection

Result: step/trajectory pages and JSON view

Lifecycle: read-only analysis of saved snapshots

Authority: operator; not a core audit-query database

Evidence: [e19](#evidence-e19), [e20](#evidence-e20).

### SWE-bench batch runner

Surface: operator evaluation support.

Input: dataset instances, models/environments, worker count/output directory

Result: prediction patches, trajectories, per-instance exit/progress

Lifecycle: independent agents overlap in thread pool; interrupt cancels pending jobs and waits for running ones

Authority: operator benchmark generation, not live collaborating agent scheduling or autonomous harness optimization

Evidence: [e21](#evidence-e21), [e22](#evidence-e22).

## Capabilities

### filesystem

**I — Files** (source): Native bash can read/write/edit files using ordinary OS programs in local/container environment. There are no separate native file-tool semantics in inspected catalog.

Evidence: [e6](#evidence-e6), [e8](#evidence-e8), [e17](#evidence-e17).

### processes

**L — OS programs** (source): Blocking subprocess.run returns output/exit/error; every action starts a fresh shell. There is no agent-level resumable PTY/background handle/owned-process-group contract. Backend timeout and model-call wall limits have separate scope.

Evidence: [e3](#evidence-e3), [e8](#evidence-e8), [e17](#evidence-e17).

### code-actions

**S — Code actions** (source): Model can author/execute scripts through shell and invoke any available interpreter; no dedicated code-cell tool or persistent-program service is supplied.

Evidence: [e6](#evidence-e6), [e8](#evidence-e8).

### persistent-kernel

**L — Kernel** (source): Local/Docker actions use new interpreter invocation each time. Persistent container filesystem is not a persistent Python/JS namespace; external kernel clients could be composed through bash.

Evidence: [e8](#evidence-e8), [e17](#evidence-e17).

### standing-database

**? — Standing DB** (source): No standing DB/query interface is established in the inspected default/interactive agents, bash catalog, local/Docker environments and serialized state; arbitrary shell DB clients would be operator composition.

### workflow-programming

**S — Workflows** (source): Duck-typed agent/model/environment replacement and subclassable query/step provide a small host programmable base; bash scripts compose OS workflows. No in-session model-facing workflow language or managed workflow identity was established.

Evidence: [e3](#evidence-e3), [e15](#evidence-e15), [e16](#evidence-e16).

### multi-model

**I — Models** (source): Roulette/interleaving adapters route successive queries in one history across configured models, recording selected model name. This is more than a static provider selector but does not create colleagues.

Evidence: [e14](#evidence-e14).

### live-collaboration

**? — Peer chat** (source): No messaging/shared-room/child-agent operation was established in synchronous agent protocols, bash catalog or successive-model routing; benchmark threads are independent instances.

### concurrent-work

**S — Concurrency** (source): Benchmark ThreadPoolExecutor runs independent agent/environment instances concurrently; ordinary default/interactive actions within one run execute sequentially and cannot be steered as a live team.

Evidence: [e4](#evidence-e4), [e9](#evidence-e9), [e22](#evidence-e22).

### steering-interrupt

**L — Steer/interrupt** (source): KeyboardInterrupt caught around interactive step prompts a user message and can resume; partial/unexecuted action results are paired. No queued steering or real provider/process cancellation oracle was inspected; batch interruption waits for running jobs.

Evidence: [e9](#evidence-e9), [e22](#evidence-e22), [e24](#evidence-e24).

### turn-redefinition

**S — Turn program** (source): Host subclasses can replace query/step/execute_actions through class imports and protocols. Normal model surface remains bash, with no exposed hot definition of turn admission/completion/compaction.

Evidence: [e3](#evidence-e3), [e15](#evidence-e15), [e16](#evidence-e16).

### compaction

**L — Compaction** (source): LiteLLM ContextWindowExceededError is an abort exception rather than a managed compression path in the inspected loop/model. Long tool output is rendered with elision, a different mechanism from conversation compaction.

Evidence: [e5](#evidence-e5), [e27](#evidence-e27).

### context-repair

**S — Repair** (source): Mutable messages and serialized raw-output/response metadata permit programmer/operator reconstruction and re-injection. No model repair tool or native restore/merge lineage protocol is exposed.

Evidence: [e2](#evidence-e2), [e4](#evidence-e4), [e5](#evidence-e5), [e7](#evidence-e7).

### original-audit

**L — Original audit** (source): Trajectories keep returned response dumps, full raw command output and malformed-format replies, but save overwrites a JSON snapshot after steps, without append/crash-atomic observations or all retry/wire events. A cost-calculation exception can precede response recording.

Evidence: [e3](#evidence-e3), [e4](#evidence-e4), [e5](#evidence-e5), [e7](#evidence-e7), [e18](#evidence-e18).

### audit-query

**S — Audit query** (source): Inspector navigates saved steps/trajectories and opens step/full JSON; model can use shell on files. This is saved-data analysis rather than authoritative original-event query.

Evidence: [e19](#evidence-e19), [e20](#evidence-e20).

### hot-change

**S — Hot change** (source): Operator mode/limits mutate at interactive prompts; host can mutate Python objects or replace subclass stages. Startup class/config selection is not a watcher or executable refit protocol.

Evidence: [e1](#evidence-e1), [e10](#evidence-e10), [e11](#evidence-e11), [e15](#evidence-e15).

### rebuild-continuity

**? — Rebuild continuity** (source): No outpost, executable replacement or serialized continuation restore is established in run/save/CLI/environment lifecycle. Python source editability alone does not preserve live state through replacement.

### remote-services

**I — Remote** (source): Model calls consume LiteLLM provider service synchronously; backend is replaceable and Docker is a separate container boundary. Provider adapter internals and remote deployment behavior are not inspected here.

Evidence: [e5](#evidence-e5), [e16](#evidence-e16), [e17](#evidence-e17).

### self-improvement

**? — Self-improve** (source): Bash can change source and host classes are replaceable, but inspected loops/config/benchmarks do not establish a model-governed experiment/measurement/selection or self-reinhabitation protocol.

### complaints

**? — Complaints** (source): No captured-state model complaint/bead/external-DB operation established in agent loop, tool catalog, interactive commands or trajectory inspector.

### authority

**I — Authority** (source): Default agent invokes environment without approval. Interactive human/confirm/yolo and regex whitelist control execution prompts, with CLI yolo/exit-immediately. No claim of an additional command-policy or sandbox gate in local backend.

Evidence: [e1](#evidence-e1), [e8](#evidence-e8), [e10](#evidence-e10).

### evaluation

**I — Evaluation** (source): Source tests have independent computed partial-output, exact interruption-message and malformed-response serialization oracles. Batch runner saves SWE-bench predictions/trajectories; grading and real in-flight cancellation are not implied, and tests were not run.

Evidence: [e21](#evidence-e21), [e23](#evidence-e23), [e24](#evidence-e24), [e25](#evidence-e25), [e26](#evidence-e26).

### time-order

**L — Time/order** (source): Serial action/message order is explicit; timestamps and budget elapsed time use time.time. Wall limit is checked only before a query, cannot interrupt an outstanding request/action, and start/counters persist if the same agent object is rerun.

Evidence: [e2](#evidence-e2), [e3](#evidence-e3), [e4](#evidence-e4), [e7](#evidence-e7), [e18](#evidence-e18).

## Inspected test oracles

- [quarantine/mini-swe-agent/tests/agents/test_default.py](../../../quarantine/mini-swe-agent/tests/agents/test_default.py): Timeout partial output and budget boundary. Oracle: Computes 111×9 independently, requires that output after timeout and a subsequent final submission; step limit asserts one call. Cost test does not exercise paid malformed replies. Read, **not executed**.
- [quarantine/mini-swe-agent/tests/agents/test_interactive.py](../../../quarantine/mini-swe-agent/tests/agents/test_interactive.py): Operator interruption→continued agent flow. Oracle: Mocks query raising KeyboardInterrupt then checks exactly one interruption message and final submission; not a real in-flight provider/process cancel test. Read, **not executed**.
- [quarantine/mini-swe-agent/tests/models/test_format_error_response_persistence.py](../../../quarantine/mini-swe-agent/tests/models/test_format_error_response_persistence.py): Malformed tool response remains inspectable. Oracle: Forces unknown tool name and checks retained response is nonempty, JSON serializable and dumped in JSON mode; does not prove all provider errors/retries are audited. Read, **not executed**.

## Useful mechanisms

- Very small synchronous stage interfaces make model, environment and loop replacement straightforward for host programs.
- Full raw command output is kept separately from elided model observations, and malformed formatted responses are explicitly preserved.
- Tool-call formatter supplies a result for unexecuted actions, preserving pairing under operator rejection/interruption.

## Material limits

- LiteLLM and SWE-ReX dependency internals, remote backends beyond Docker, process-tree cancellation and full provider-retry persistence are outside inspection. A source-inferred budget concern: malformed paid replies increment global model stats but raise before DefaultAgent adds returned message cost, so its cost budget can undercount FormatError calls.

## Arconaut design questions

- Should authority/mode changes leave identical action-result-continuation behavior while removing routine confirmation friction?
- Can a cost oracle charge a deliberately malformed paid response even when parsing raises before the ordinary success return?
- How should a queued interrupt distinguish cancelling owned shell descendants from preserving externally shared kernel/DB services?
- What minimal event log makes raw provider/program observations recoverable before a step finishes, rather than after snapshot save?
- How can model-accessible turn programs and workflow identities grow from this small stage interface without hiding activation/refit semantics?

## Evidence

### Evidence e1

[quarantine/mini-swe-agent/src/minisweagent/run/mini.py:65–109](../../../quarantine/mini-swe-agent/src/minisweagent/run/mini.py#L65): CLI composes agent/model/environment classes and config, selects interactive local defaults and offers yolo/exit-immediately/output options.

### Evidence e2

[quarantine/mini-swe-agent/src/minisweagent/agents/default.py:38–72](../../../quarantine/mini-swe-agent/src/minisweagent/agents/default.py#L38): Agent owns a mutable message list, model and environment; call/cost counters and wall-clock start are initialized once on construction.

### Evidence e3

[quarantine/mini-swe-agent/src/minisweagent/agents/default.py:86–130](../../../quarantine/mini-swe-agent/src/minisweagent/agents/default.py#L86): run resets messages and loops query→actions, appends flow exceptions, saves each step, and exits on an exit message. Budget checks precede model calls; cost increments only after a returned model message.

### Evidence e4

[quarantine/mini-swe-agent/src/minisweagent/agents/default.py:131–171](../../../quarantine/mini-swe-agent/src/minisweagent/agents/default.py#L131): Actions execute sequentially; serialization includes configuration/stats/messages, and save replaces one JSON file without a replay/append journal.

### Evidence e5

[quarantine/mini-swe-agent/src/minisweagent/models/litellm_model.py:49–114](../../../quarantine/mini-swe-agent/src/minisweagent/models/litellm_model.py#L49): Concrete provider path calls synchronous LiteLLM completion with bash tool; successful and malformed replies retain response dumps, but parse FormatError is raised before returning cost to the agent.

### Evidence e6

[quarantine/mini-swe-agent/src/minisweagent/models/utils/actions_toolcall.py:11–65](../../../quarantine/mini-swe-agent/src/minisweagent/models/utils/actions_toolcall.py#L11): Native exposed function is bash(command); multiple valid bash calls are parsed, unknown/missing/invalid actions become FormatError.

### Evidence e7

[quarantine/mini-swe-agent/src/minisweagent/models/utils/actions_toolcall.py:68–103](../../../quarantine/mini-swe-agent/src/minisweagent/models/utils/actions_toolcall.py#L68): Observation formatting preserves raw output separately from model-visible template; unexecuted calls receive explicit paired placeholders.

### Evidence e8

[quarantine/mini-swe-agent/src/minisweagent/environments/local.py:12–79](../../../quarantine/mini-swe-agent/src/minisweagent/environments/local.py#L12): Each local action invokes fresh shell=True subprocess.run with merged environment/cwd, combined stderr/stdout and timeout; no ongoing execution handle is returned. Successful first-line submit sentinel raises completion.

### Evidence e9

[quarantine/mini-swe-agent/src/minisweagent/agents/interactive.py:101–132](../../../quarantine/mini-swe-agent/src/minisweagent/agents/interactive.py#L101): KeyboardInterrupt prompts operator steering; partial action outputs are formatted in finally, with unexecuted action placeholders supplied by the model formatter.

### Evidence e10

[quarantine/mini-swe-agent/src/minisweagent/agents/interactive.py:160–209](../../../quarantine/mini-swe-agent/src/minisweagent/agents/interactive.py#L160): Human/confirm/yolo modes and regex whitelist control confirmation; slash mode changes mutate config during a prompt, without a watcher or executable reload.

### Evidence e11

[quarantine/mini-swe-agent/src/minisweagent/agents/interactive.py:69–94](../../../quarantine/mini-swe-agent/src/minisweagent/agents/interactive.py#L69): TimeExceeded exits rather than asking for budgets; noninteractive stdin prevents the limits prompt from crashing unattended runs.

### Evidence e12

[quarantine/mini-swe-agent/src/minisweagent/models/litellm_textbased_model.py:7–45](../../../quarantine/mini-swe-agent/src/minisweagent/models/litellm_textbased_model.py#L7): Alternate text model extracts a configured code fence rather than native tool calls and requests exactly one action.

### Evidence e13

[quarantine/mini-swe-agent/src/minisweagent/models/utils/actions_text.py:17–40](../../../quarantine/mini-swe-agent/src/minisweagent/models/utils/actions_text.py#L17): Text parser enforces exactly one regex action and stores malformed model text in FormatError metadata.

### Evidence e14

[quarantine/mini-swe-agent/src/minisweagent/models/extra/roulette.py:13–64](../../../quarantine/mini-swe-agent/src/minisweagent/models/extra/roulette.py#L13): Roulette selects one configured model per query; interleaving uses a model sequence. This is successive routing in one conversation, not concurrent colleague interaction.

### Evidence e15

[quarantine/mini-swe-agent/src/minisweagent/agents/__init__.py:15–28](../../../quarantine/mini-swe-agent/src/minisweagent/agents/__init__.py#L15): Operator/program config can import arbitrary agent classes; subclassing/replacing protocols is actual host programmability.

### Evidence e16

[quarantine/mini-swe-agent/src/minisweagent/__init__.py:43–85](../../../quarantine/mini-swe-agent/src/minisweagent/__init__.py#L43): Model/environment/agent duck-typed protocols supply synchronous query/execute/run/save replacement boundaries.

### Evidence e17

[quarantine/mini-swe-agent/src/minisweagent/environments/docker.py:72–139](../../../quarantine/mini-swe-agent/src/minisweagent/environments/docker.py#L72): Docker backend owns a long-lived container but each action uses a separate docker exec interpreter, with merged bounded output and timeout.

### Evidence e18

[quarantine/mini-swe-agent/src/minisweagent/models/utils/retry.py:9–25](../../../quarantine/mini-swe-agent/src/minisweagent/models/utils/retry.py#L9): Provider retries are bounded exponential waits up to sixty seconds, with selected abort exceptions including KeyboardInterrupt.

### Evidence e19

[quarantine/mini-swe-agent/src/minisweagent/run/utilities/inspector.py:24–43](../../../quarantine/mini-swe-agent/src/minisweagent/run/utilities/inspector.py#L24): Trajectory inspector groups saved assistant/action messages into step pages.

### Evidence e20

[quarantine/mini-swe-agent/src/minisweagent/run/utilities/inspector.py:75–109](../../../quarantine/mini-swe-agent/src/minisweagent/run/utilities/inspector.py#L75): Inspector supplies operator next/previous/first/last step and trajectory navigation plus full/step JSON inspection.

### Evidence e21

[quarantine/mini-swe-agent/src/minisweagent/run/benchmarks/swebench.py:134–176](../../../quarantine/mini-swe-agent/src/minisweagent/run/benchmarks/swebench.py#L134): Batch runner creates independent agent/environment instances, saves trajectories and predictions even after exceptions; this generation path does not itself establish benchmark grading.

### Evidence e22

[quarantine/mini-swe-agent/src/minisweagent/run/benchmarks/swebench.py:245–271](../../../quarantine/mini-swe-agent/src/minisweagent/run/benchmarks/swebench.py#L245): ThreadPoolExecutor overlaps independent benchmark instances; interruption cancels only not-yet-running futures and waits for running work.

### Evidence e23

[quarantine/mini-swe-agent/tests/agents/test_default.py:206–227](../../../quarantine/mini-swe-agent/tests/agents/test_default.py#L206): Timeout partial-output oracle derives 111×9 independently and requires that output in the timeout observation plus successful later submission.

### Evidence e24

[quarantine/mini-swe-agent/tests/agents/test_interactive.py:263–310](../../../quarantine/mini-swe-agent/tests/agents/test_interactive.py#L263): Interruption test mocks query to raise KeyboardInterrupt, then verifies exactly one steering message and final submission; it does not interrupt a real busy provider/process.

### Evidence e25

[quarantine/mini-swe-agent/tests/models/test_format_error_response_persistence.py:77–103](../../../quarantine/mini-swe-agent/tests/models/test_format_error_response_persistence.py#L77): Malformed tool-response test forces unknown tool, then checks retained nonempty JSON-serializable response and JSON-mode serialization.

### Evidence e26

[quarantine/mini-swe-agent/tests/agents/test_default.py:152–183](../../../quarantine/mini-swe-agent/tests/agents/test_default.py#L152): Step-limit test asserts exactly one call and LimitsExceeded; cost test checks an exit status but does not cover paid malformed responses.

### Evidence e27

[quarantine/mini-swe-agent/src/minisweagent/config/default.yaml:123–149](../../../quarantine/mini-swe-agent/src/minisweagent/config/default.yaml#L123): Configured model-visible output keeps head/tail and elides long middle content; raw_output retained by formatter is a distinct representation.

