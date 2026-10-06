# gpt-engineer

A functional code-generation/entrypoint/diff-repair pipeline with injectable stages, clarification, clipboard provider option and bounded generated-application self-heal. Pipeline/model editing must be distinguished from a general coding-agent action loop or governed harness self-improvement.

Role: coding agent. Runtime: Python.

Pinned source: [https://github.com/AntonOsika/gpt-engineer](https://github.com/AntonOsika/gpt-engineer); revision/version `a90fcd543eedcc0ff2c34561bc0785d2ba83c47e`.

Synchronous staged generator/diff-improvement pipeline through LangChain model invoke/stream-to-stdout, plus fresh local Popen processes; no generic model tool-dispatch loop or resident runtime.

Owns project FilesDict/disk stores/logs and subprocess execution; consumes a model provider. File-backed memory is not shared database/compute governance.

Inspection: CLI selection→injected staged agent→concrete provider invoke/normalization→file/diff parse/applicability repair→disk writes/execution/results; self-heal, fresh preprompts, append/archive logs, opted-in human review/telemetry, benchmark assertion path and relevant functional/no-oracle tests.

Limits of this study: Source read only, reference not executed. LangChain provider internals and detailed diff algorithm/version-manager paths are outside trace. No reference tests ran. Source-inferred operational concerns: blocking stdout/stderr reads occur before deadline checks; kill lacks reap, self-heal has unbounded communicate and stops for code 2; benchmark supplies timeout as positional stdin argument.

## Actions

### generated file blocks / chat_to_files_dict

Surface: model response convention.

Input: filename plus fenced source contents

Result: FilesDict of generated files

Lifecycle: host parses one generated response then later writes/executed files; not individual file tool calls

Authority: model chooses textual names/content; operator-selected staged generation

Evidence: [e6](#evidence-e6), [e10](#evidence-e10), [e11](#evidence-e11).

### generated unified diffs / improve_fn

Surface: model response convention.

Input: selected existing files and improvement prompt; response diff blocks

Result: applied/salvaged FilesDict and hunk-applicability errors

Lifecycle: bounded requery for faulty diffs, accumulated changes then operator apply prompt

Authority: model proposes file modifications; host parses/applies; operator CLI final apply confirmation

Evidence: [e2](#evidence-e2), [e9](#evidence-e9).

### generated run.sh / execute_entrypoint

Surface: model response convention plus operator-triggered execution.

Input: entrypoint/install script fenced content and generated code files

Result: combined execution console output; original FilesDict returned

Lifecycle: single blocking run after default consent; script can launch arbitrary programs but no native workflow handles

Authority: model writes script, operator default Y/n execution confirmation; direct host API may bypass

Evidence: [e7](#evidence-e7), [e8](#evidence-e8), [e12](#evidence-e12), [e13](#evidence-e13).

### SimpleAgent.init / improve; CliAgent.init / improve

Surface: programmable API.

Input: Prompt, FilesDict and injected model/memory/env/stage functions

Result: generated/improved files

Lifecycle: staged pipeline with optional clarification/lite/self-heal; no universal next-tool loop

Authority: host programmer/operator, normal model cannot replace stage functions

Evidence: [e3](#evidence-e3), [e4](#evidence-e4), [e5](#evidence-e5).

### self_heal

Surface: programmable configured processing stage.

Input: generated files/entrypoint plus requested specification

Result: FilesDict after repeated run/repair or ten-attempt bound

Lifecycle: automatic application execution→improve for failures; no timeout passed to communicate; exit 2 stops repair

Authority: operator selects --self-heal; model repairs generated application, not harness execution machinery

Evidence: [e1](#evidence-e1), [e21](#evidence-e21).

### clarified_gen / lite_gen

Surface: programmable configured generation stages.

Input: operator specification and clarification replies, or main prompt

Result: clarified/lite generated file response

Lifecycle: clarification synchronously prompts operator until model says nothing unclear, then generates

Authority: operator selects generation variant; provider/model informs questions

Evidence: [e1](#evidence-e1), [e22](#evidence-e22).

### DiskExecutionEnv.upload / download / popen / run

Surface: programmable API.

Input: FilesDict, command string and optional run timeout

Result: downloaded files, raw Popen handle or stdout/stderr/returncode tuple

Lifecycle: fresh OS process; run has blocking-read/timeout and reap limits; caller owns raw popen lifetime

Authority: host API unrestricted local execution, not a persistent shared kernel control plane

Evidence: [e11](#evidence-e11), [e12](#evidence-e12), [e13](#evidence-e13).

### AI.start / next / ClipboardAI.next

Surface: programmable provider API.

Input: system/user or message list, optional prompt and step label

Result: normalized appended model response/messages and usage counters

Lifecycle: synchronous provider invoke/backoff; clipboard waits operator EOF/paste

Authority: host-selected one provider/model; no native live peer roles

Evidence: [e14](#evidence-e14), [e15](#evidence-e15), [e16](#evidence-e16), [e17](#evidence-e17), [e18](#evidence-e18).

### PrepromptsHolder.get_preprompts / custom preprompts

Surface: programmable API and operator configuration.

Input: directory of named prompt files

Result: current prompt texts

Lifecycle: reread when stage invokes getter; improvement loop captures prompts before refinement retries

Authority: operator/host can edit ordinary files; not executable hot reload/refit

Evidence: [e6](#evidence-e6), [e9](#evidence-e9), [e19](#evidence-e19).

### DiskMemory.get/set/log/archive_logs

Surface: programmable file-backed memory API.

Input: string/path key and content or rendered log

Result: persisted files and timestamped appended text

Lifecycle: stores code/debug/diff/session logs; startup moves logs to timestamp archive

Authority: host API; model has no general standing DB/query tool

Evidence: [e20](#evidence-e20).

### opted-in human review / collect_learnings

Surface: operator feedback and supplied telemetry primitive.

Input: ran/quality/usefulness answers and comments plus prompt/model/config/logs

Result: human review/learning payload, bounded optional telemetry send

Lifecycle: post-generation interaction, logs may truncate before send

Authority: operator consent; no model complaint or tracked external issue DB contract

Evidence: [e22](#evidence-e22), [e23](#evidence-e23), [e24](#evidence-e24), [e25](#evidence-e25).

### benchmark.run

Surface: operator/programmatic evaluation API.

Input: tasks, initial code/prompts, commands and assertion callbacks; optional timeout

Result: per-task assertion booleans/success rate and generation duration

Lifecycle: sequential improve→execute→assert; timeout positional argument source concern

Authority: host-supplied oracles; not measured model-governed harness autoresearch

Evidence: [e26](#evidence-e26), [e27](#evidence-e27).

## Capabilities

### filesystem

**I — Files** (source): Model-generated file blocks and unified diffs become FilesDict and filesystem writes; selected files constrain improvement inputs, without native per-file observation/edit actions. Binary pull uses a marker.

Evidence: [e6](#evidence-e6), [e9](#evidence-e9), [e10](#evidence-e10), [e11](#evidence-e11).

### processes

**L — OS programs** (source): Host API runs/popens fresh local processes and generated entrypoints execute after consent. run blocking per-pipe readline precedes timeout and kill lacks wait/reap; raw Popen is not a managed model handle.

Evidence: [e8](#evidence-e8), [e12](#evidence-e12), [e13](#evidence-e13).

### code-actions

**I — Code actions** (source): The model emits complete programs/launch scripts which the staged host parses, writes and executes; self-heal repeatedly runs generated application code. This is staged code execution, not tool-cell orchestration.

Evidence: [e6](#evidence-e6), [e7](#evidence-e7), [e8](#evidence-e8), [e21](#evidence-e21).

### persistent-kernel

**L — Kernel** (source): DiskExecutionEnv starts fresh subprocesses; stored files/memory and raw Popen do not establish a persistent namespace kernel or shared client-service protocol.

Evidence: [e11](#evidence-e11), [e12](#evidence-e12), [e13](#evidence-e13).

### standing-database

**S — Standing DB** (source): DiskMemory is file-backed keyed text/log storage; host can read/write it and reuse project files. No model-accessible arbitrary standing DB or external shared DB contract is established.

Evidence: [e20](#evidence-e20).

### workflow-programming

**S — Workflows** (source): CliAgent accepts injected generation/improvement/processing functions and interchangeable AI/memory/env; clarification and self-heal are alternative stages. Host composition is real, normal model agency over workflow definitions is not exposed.

Evidence: [e1](#evidence-e1), [e4](#evidence-e4), [e5](#evidence-e5), [e21](#evidence-e21), [e22](#evidence-e22).

### multi-model

**? — Models** (source): Inspected AI builds one configured Azure/OpenAI/Anthropic provider; no successive routing, independently active roles or multi-model request program established in staged pipelines.

### live-collaboration

**? — Peer chat** (source): No peer mailbox/channel/shared room or model-addressed colleague action established in staged agent/model/clarification/execution paths.

### concurrent-work

**S — Concurrency** (source): Host popen returns an OS handle and generated Unix script can compose programs, but inspected staged agent/benchmark execution is sequential with no ongoing-work scheduler or ownership registry.

Evidence: [e7](#evidence-e7), [e12](#evidence-e12), [e26](#evidence-e26).

### steering-interrupt

**L — Steer/interrupt** (source): Default execution prints Ctrl+C guidance and kills its direct process when KeyboardInterrupt occurs; it does not wait/reap or preserve model/provider/program state for continuation. Operator clarification/apply prompts are separate boundaries.

Evidence: [e8](#evidence-e8), [e13](#evidence-e13), [e22](#evidence-e22).

### turn-redefinition

**S — Turn program** (source): Programmer injects generation/improvement/process callables and replaces AI/env/memory. That is a staged functional interface, without normal model hot turn completion/admission/compaction redefinition.

Evidence: [e4](#evidence-e4), [e5](#evidence-e5).

### compaction

**L — Compaction** (source): Nonvision AI collapses same-role messages and retains only first text block for list content. This is formatting normalization with potential representation loss, not managed compaction with choices/events/repair.

Evidence: [e14](#evidence-e14), [e15](#evidence-e15).

### context-repair

**S — Repair** (source): Selected source files, rendered append logs and host message objects can be reused/reinjected by programmer/operator. No model-authorized original-context restore/merge operation; message normalization drops metadata.

Evidence: [e9](#evidence-e9), [e14](#evidence-e14), [e20](#evidence-e20).

### original-audit

**L — Original audit** (source): Append logs/archive preserve rendered conversations/debug/diff information, but generation logs after provider response and does not capture original request/stream/program lifecycles. Message collapse rewrites representation; feedback telemetry may truncate logs.

Evidence: [e6](#evidence-e6), [e7](#evidence-e7), [e9](#evidence-e9), [e14](#evidence-e14), [e15](#evidence-e15), [e20](#evidence-e20), [e24](#evidence-e24).

### audit-query

**S — Audit query** (source): File-backed logs/source and API key retrieval permit ordinary analysis. No native audit query/index service with original complete event records established.

Evidence: [e20](#evidence-e20).

### hot-change

**S — Hot change** (source): Preprompt files are reread when each stage calls get_preprompts, while refinement captures messages/prompts for its loop. Function injection remains host mutable Python state, without executable watcher/activation protocol.

Evidence: [e4](#evidence-e4), [e9](#evidence-e9), [e19](#evidence-e19).

### rebuild-continuity

**? — Rebuild continuity** (source): No executable refit, outpost context handoff or restoration of unresolved provider/program handles established in AI/staged agent/memory/execution lifecycle.

### remote-services

**I — Remote** (source): Consumes configured LangChain OpenAI/Azure/Anthropic provider objects with streaming stdout callback; clipboard adapter is operator-mediated provider input/output. Dependency implementations are not inspected.

Evidence: [e16](#evidence-e16), [e17](#evidence-e17), [e18](#evidence-e18).

### self-improvement

**S — Self-improve** (source): --self-heal performs bounded generated-application run/repair. It does not alter/measure/select/reinhabit the harness; unbounded communicate and exit-code-2 stopping rule further delimit success claims.

Evidence: [e1](#evidence-e1), [e21](#evidence-e21).

### complaints

**L — Complaints** (source): Opted-in human review records generated-code quality/comments and sends learning telemetry. It is operator feedback, not model rageshake with captured agent state and external bead/DB tracking.

Evidence: [e23](#evidence-e23), [e24](#evidence-e24), [e25](#evidence-e25).

### authority

**L — Authority** (source): Default entrypoint execution and CLI improvement application ask approval. Operator-selected self-heal directly executes generated application without that default prompt; direct environment API is host authority. No expert change-activation contract is supplied.

Evidence: [e2](#evidence-e2), [e8](#evidence-e8), [e21](#evidence-e21).

### evaluation

**I — Evaluation** (source): Benchmark supplies task assertion callbacks and success rates; functional source tests check exact generated/improved file output. Several complex-diff tests have no assertions and interrupt only checks mock kill; benchmark passes optional timeout in communicate input position.

Evidence: [e26](#evidence-e26), [e27](#evidence-e27), [e28](#evidence-e28), [e29](#evidence-e29), [e30](#evidence-e30), [e31](#evidence-e31).

### time-order

**L — Time/order** (source): Logs/timing use wall time; rate-limit backoff bounded. Execution run timeout cannot bound blocking readline waits and self-heal communicate is unbounded. No causal clock/event chronology foundation supplied.

Evidence: [e13](#evidence-e13), [e16](#evidence-e16), [e20](#evidence-e20), [e21](#evidence-e21), [e26](#evidence-e26).

## Inspected test oracles

- [quarantine/gpt-engineer/tests/core/default/test_simple_agent.py](../../../quarantine/gpt-engineer/tests/core/default/test_simple_agent.py): Generated and changed code must produce independently specified output. Oracle: Mock-model program is run and exact output.txt is asserted Hello World then reversed text; this checks generated program behavior, not provider correctness or execution deadlines. Read, **not executed**.
- [quarantine/gpt-engineer/tests/core/default/test_disk_execution_env.py](../../../quarantine/gpt-engineer/tests/core/default/test_disk_execution_env.py): Interrupt and output preservation. Oracle: Interrupt only checks MagicMock.kill invoked; output test directly checks mocked popen/communicate bytes. Neither checks live exit/reap, skewed pipe deadlock, terminal output drain or timeout. Read, **not executed**.
- [quarantine/gpt-engineer/tests/core/test_salvage_correct_hunks.py](../../../quarantine/gpt-engineer/tests/core/test_salvage_correct_hunks.py): Complex malformed diff correction/application. Oracle: Several cases invoke salvage or print returned files without correctness assertions; cleanup assert True is no oracle. Do not count nominal examples as conformance. Read, **not executed**.

## Useful mechanisms

- Injected generation/improvement/processing callables make the staged host interface simple to vary.
- Fresh preprompt lookup and append/archive rendered logs are useful small primitives with explicit stage-level scope.
- Selected-file diff applicability repair separates model proposals from host application and supplies concrete diagnostics for retry.

## Material limits

- LangChain provider internals and detailed diff algorithm/version-manager paths are outside trace. No reference tests ran. Source-inferred operational concerns: blocking stdout/stderr reads occur before deadline checks; kill lacks reap, self-heal has unbounded communicate and stops for code 2; benchmark supplies timeout as positional stdin argument.

## Arconaut design questions

- How should model-program outputs become tracked actions with durable original audit before execution and operator change activation?
- Can independent asymmetric-pipe/deadline and child-exit oracles replace nominal mock kill checks?
- What measurement decides that application self-heal or harness autoresearch improved behavior rather than only making a process return?
- How can fresh prompt-file lookup preserve model continuity and explicit activation after current turn/workflows?
- Which minimal functional stage interface supports model-owned programs without requiring a full fixed pipeline replacement?

## Evidence

### Evidence e1

[quarantine/gpt-engineer/gpt_engineer/applications/cli/main.py:474–514](../../../quarantine/gpt-engineer/gpt_engineer/applications/cli/main.py#L474): CLI selects standard/lite/clarification generation and self-heal/default execution, then constructs injected CliAgent with disk memory/environment and preprompts.

### Evidence e2

[quarantine/gpt-engineer/gpt_engineer/applications/cli/main.py:530–552](../../../quarantine/gpt-engineer/gpt_engineer/applications/cli/main.py#L530): Improvement changes require operator apply confirmation; standard generation runs initialization, collects opted-in human feedback then stages/writes generated files.

### Evidence e3

[quarantine/gpt-engineer/gpt_engineer/core/default/simple_agent.py:69–92](../../../quarantine/gpt-engineer/gpt_engineer/core/default/simple_agent.py#L69): SimpleAgent initialization generates code then entrypoint; improve applies a generated diff without using its execution_command parameter.

### Evidence e4

[quarantine/gpt-engineer/gpt_engineer/applications/cli/cli_agent.py:88–111](../../../quarantine/gpt-engineer/gpt_engineer/applications/cli/cli_agent.py#L88): CLI agent injects generation/improvement/process functions and owns AI/memory/execution/preprompt objects.

### Evidence e5

[quarantine/gpt-engineer/gpt_engineer/applications/cli/cli_agent.py:166–219](../../../quarantine/gpt-engineer/gpt_engineer/applications/cli/cli_agent.py#L166): CLI init chains generation→entrypoint→selected execution; improve uses injected improvement stage and does not run commented-out entrypoint stage.

### Evidence e6

[quarantine/gpt-engineer/gpt_engineer/core/default/steps.py:143–150](../../../quarantine/gpt-engineer/gpt_engineer/core/default/steps.py#L143): gen_code sends system/user request, logs rendered returned messages, and converts textual file blocks into FilesDict.

### Evidence e7

[quarantine/gpt-engineer/gpt_engineer/core/default/steps.py:178–202](../../../quarantine/gpt-engineer/gpt_engineer/core/default/steps.py#L178): gen_entrypoint requests one Unix launch/install script, extracts fenced blocks as run.sh and logs rendered messages.

### Evidence e8

[quarantine/gpt-engineer/gpt_engineer/core/default/steps.py:230–269](../../../quarantine/gpt-engineer/gpt_engineer/core/default/steps.py#L230): Default execute_entrypoint asks operator consent then uploads/runs bash run.sh and returns original file dict; process output is not fed back in this standard path.

### Evidence e9

[quarantine/gpt-engineer/gpt_engineer/core/default/steps.py:298–360](../../../quarantine/gpt-engineer/gpt_engineer/core/default/steps.py#L298): Improve sends selected files/prompt, salvages valid diffs and re-prompts boundedly for malformed hunks; this validates applicability, not behavioral correctness.

### Evidence e10

[quarantine/gpt-engineer/gpt_engineer/core/chat_to_files.py:38–67](../../../quarantine/gpt-engineer/gpt_engineer/core/chat_to_files.py#L38): Generated textual file blocks are regex-parsed, filenames normalized and inserted as content strings; no ordinary model tool catalog/action handle exists.

### Evidence e11

[quarantine/gpt-engineer/gpt_engineer/core/default/file_store.py:39–62](../../../quarantine/gpt-engineer/gpt_engineer/core/default/file_store.py#L39): FileStore pushes names/content to filesystem and pulls text recursively, replacing undecodable binary content with a marker.

### Evidence e12

[quarantine/gpt-engineer/gpt_engineer/core/default/disk_execution_env.py:54–70](../../../quarantine/gpt-engineer/gpt_engineer/core/default/disk_execution_env.py#L54): Programmatic upload/download and popen supply raw OS subprocess handles; no agent-owned process registry is provided.

### Evidence e13

[quarantine/gpt-engineer/gpt_engineer/core/default/disk_execution_env.py:72–111](../../../quarantine/gpt-engineer/gpt_engineer/core/default/disk_execution_env.py#L72): run polls then synchronously reads stdout.readline and stderr.readline before checking timeout; kill on timeout/KeyboardInterrupt has no wait/reap and terminal output drain is not guaranteed.

### Evidence e14

[quarantine/gpt-engineer/gpt_engineer/core/ai.py:147–205](../../../quarantine/gpt-engineer/gpt_engineer/core/ai.py#L147): Nonvision conversation normalization rebuilds collapsed same-role messages from extracted text, taking only the first text block of list content.

### Evidence e15

[quarantine/gpt-engineer/gpt_engineer/core/ai.py:229–254](../../../quarantine/gpt-engineer/gpt_engineer/core/ai.py#L229): AI.next invokes provider, records token usage and appends a response; normalized returned messages are distinct from original input message metadata.

### Evidence e16

[quarantine/gpt-engineer/gpt_engineer/core/ai.py:256–287](../../../quarantine/gpt-engineer/gpt_engineer/core/ai.py#L256): Model invocation is synchronous through LangChain, with bounded OpenAI rate-limit backoff, not a managed asynchronous request handle.

### Evidence e17

[quarantine/gpt-engineer/gpt_engineer/core/ai.py:345–379](../../../quarantine/gpt-engineer/gpt_engineer/core/ai.py#L345): Configured provider object is Azure/OpenAI/Anthropic with stdout streaming callbacks; selectable providers do not establish multiple active colleagues.

### Evidence e18

[quarantine/gpt-engineer/gpt_engineer/core/ai.py:386–437](../../../quarantine/gpt-engineer/gpt_engineer/core/ai.py#L386): Clipboard adapter serializes messages to clipboard/text file and accepts operator pasted response, appending one AIMessage.

### Evidence e19

[quarantine/gpt-engineer/gpt_engineer/core/preprompts_holder.py:23–28](../../../quarantine/gpt-engineer/gpt_engineer/core/preprompts_holder.py#L23): Preprompt holder rereads all prompt files through DiskMemory for each getter call, allowing stage-boundary prompt updates.

### Evidence e20

[quarantine/gpt-engineer/gpt_engineer/core/default/disk_memory.py:288–326](../../../quarantine/gpt-engineer/gpt_engineer/core/default/disk_memory.py#L288): DiskMemory appends timestamped rendered text logs and archives prior logs at startup; it is files rather than a standing query database.

### Evidence e21

[quarantine/gpt-engineer/gpt_engineer/tools/custom_steps.py:85–122](../../../quarantine/gpt-engineer/gpt_engineer/tools/custom_steps.py#L85): Self-heal loops up to ten generated-application runs with unbounded communicate, asks improve_fn for failures, and treats returncode 2 as a stopping result; timed_out flag never changes here.

### Evidence e22

[quarantine/gpt-engineer/gpt_engineer/tools/custom_steps.py:148–197](../../../quarantine/gpt-engineer/gpt_engineer/tools/custom_steps.py#L148): Clarification is an operator/model question-answer loop, then a generated-code pass; it has no live teammate or programmable scheduler.

### Evidence e23

[quarantine/gpt-engineer/gpt_engineer/applications/cli/learning.py:133–173](../../../quarantine/gpt-engineer/gpt_engineer/applications/cli/learning.py#L133): Opted-in human review asks generated-code ran/quality/usefulness/comments questions; this is operator feedback, not model complaint capture.

### Evidence e24

[quarantine/gpt-engineer/gpt_engineer/applications/cli/collect.py:94–114](../../../quarantine/gpt-engineer/gpt_engineer/applications/cli/collect.py#L94): Telemetry extracts learning/logs and truncates oversized logs before retrying send; optional collection is not comprehensive core audit.

### Evidence e25

[quarantine/gpt-engineer/gpt_engineer/applications/cli/collect.py:170–177](../../../quarantine/gpt-engineer/gpt_engineer/applications/cli/collect.py#L170): Human review collection sends learning only when consented review exists; no normal model-native rageshake operation.

### Evidence e26

[quarantine/gpt-engineer/gpt_engineer/benchmark/run.py:46–92](../../../quarantine/gpt-engineer/gpt_engineer/benchmark/run.py#L46): Benchmark sequentially improves tasks, runs command and applies supplied assertion callbacks to files/process/output. communicate(benchmark.timeout) passes timeout as first positional input rather than timeout keyword.

### Evidence e27

[quarantine/gpt-engineer/gpt_engineer/benchmark/types.py:56–88](../../../quarantine/gpt-engineer/gpt_engineer/benchmark/types.py#L56): Benchmark defines optional timeout and assertion functions; task result success rate counts literal true assertions, with empty assertions yielding zero.

### Evidence e28

[quarantine/gpt-engineer/tests/core/default/test_simple_agent.py:16–76](../../../quarantine/gpt-engineer/tests/core/default/test_simple_agent.py#L16): Two functional tests run mock-model generated/improved code and require exact output-file content for generation and changed behavior.

### Evidence e29

[quarantine/gpt-engineer/tests/core/default/test_disk_execution_env.py:43–77](../../../quarantine/gpt-engineer/tests/core/default/test_disk_execution_env.py#L43): Interrupt test only asserts mock kill invocation; output test targets popen/communicate mocked return values, not run pipe draining or reaping.

### Evidence e30

[quarantine/gpt-engineer/tests/core/test_salvage_correct_hunks.py:46–100](../../../quarantine/gpt-engineer/tests/core/test_salvage_correct_hunks.py#L46): Several complex-diff tests invoke salvage without result assertions; some print outputs, so they offer no independent correctness oracle.

### Evidence e31

[quarantine/gpt-engineer/tests/core/test_salvage_correct_hunks.py:103–105](../../../quarantine/gpt-engineer/tests/core/test_salvage_correct_hunks.py#L103): Cleanup test asserts True, providing no cleanup oracle.

