# rlm

Treats input context/history as REPL variables; model Python decomposes inputs, invokes plain or recursive models and declares answer readiness.

Role: recursive code-action inference engine. Runtime: Python, TypeScript (visualizer).

Pinned source: [https://github.com/alexzhang13/rlm](https://github.com/alexzhang13/rlm); revision/version `d04208afbad29ca675ab13478c40ee8bebc84bfe`.

Synchronous completion loop with host LM socket handler, retained local/IPython/Docker REPL and bounded threaded recursive subcalls.

Owns completion environment/handler/kernel/container and temporary workspace; consumes model providers and optional remote sandbox services. persistent=True reuses an owned environment object, not shared-fabric governance.

Inspection: completion/provider/environment lifecycle, local code/subcall/state/compaction, IPython broker/timeout/cleanup, optional logger and persistence/subcall race tests

Limits of this study: No code/tests/models/sandboxes launched; training implementation not deeply studied. Defaults/local, subprocess IPython and Docker lifecycle traced, other remote environment adapters only registry-bound.

## Actions

### RLM.completion / close

Surface: program/operator inference API.

Input: context string/object/root prompt with model/depth/budget/environment config

Result: RLMChatCompletion response/usage/time/optional trajectory or limit/cancel error

Lifecycle: completion owns handler; persistent env retained across calls until close

Authority: operator-configured provider/environment/custom tools, own environment lifecycle

Evidence: [e1](#evidence-e1), [e2](#evidence-e2), [e9](#evidence-e9).

### execute_code / answer[content,ready]

Surface: native model Python action.

Input: Python blocks, answer dictionary readiness/content

Result: stdout/stderr/locals/recursive call metadata/final answer

Lifecycle: sequential blocks in retained namespace; mutation not external effect transaction

Authority: local host exec or selected environment isolation; no routine approval shown

Evidence: [e3](#evidence-e3), [e7](#evidence-e7), [e11](#evidence-e11).

### llm_query / llm_query_batched

Surface: model REPL helper.

Input: prompt(s), optional registered model name

Result: plain response(s)/error strings and call metadata

Lifecycle: await direct model calls, batch ordered; no child REPL

Authority: model has provider-request agency via host handler

Evidence: [e1](#evidence-e1), [e5](#evidence-e5).

### rlm_query / rlm_query_batched

Surface: model recursive REPL helper.

Input: prompt(s), optional model override

Result: child RLM response(s)/errors/trajectory metadata

Lifecycle: separate child REPL/depth budget, bounded threads wait all then ordered metadata; fallback plain call

Authority: custom_sub_tools propagated as child tools; no peer chat room implied

Evidence: [e4](#evidence-e4), [e5](#evidence-e5).

### add_context / add_history / SHOW_VARS

Surface: environment/model state primitives.

Input: new input/context index; messages/history index; inspection

Result: context_N/history_N, first-item aliases or variable description

Lifecycle: retained env lifetime; history deep-copy, cleanup deletes

Authority: model reads code-visible data; memory copies not immutable audit custody

Evidence: [e3](#evidence-e3), [e6](#evidence-e6), [e7](#evidence-e7).

### _compact_history / append_compaction_entry

Surface: managed inference/environment primitive.

Input: running trajectory, threshold/options

Result: summary and replaced provider-history view, prior segments in REPL history

Lifecycle: side model call then new prompt; no archived originals integrity/semantic guard

Authority: configured compaction and environment protocol; custom system policy

Evidence: [e2](#evidence-e2), [e3](#evidence-e3), [e6](#evidence-e6), [e16](#evidence-e16).

### IPythonREPL.execute_code / broker.stop / cleanup

Surface: supplied cell/provider bridge lifecycle.

Input: cell code/timeout, broker stop or cleanup

Result: cell result/TimeoutError text and own-cell calls; channels/kernel shutdown

Lifecycle: kernel interrupt on cell timeout; existing host callbacks continue; drain discards stale cells

Authority: operator-owned kernel/broker with bounded subcall admission; no full work-quiescence guarantee

Evidence: [e10](#evidence-e10), [e11](#evidence-e11), [e12](#evidence-e12), [e13](#evidence-e13), [e18](#evidence-e18).

### RLMLogger.log / get_trajectory

Surface: optional audit/inspection API.

Input: RLMIteration with code/results/prompt/response

Result: memory metadata/current iterations and optional appended JSONL

Lifecycle: per completion current view resets; JSONL persists if configured

Authority: caller explicitly supplies logger; projected values not heap checkpoint

Evidence: [e14](#evidence-e14), [e15](#evidence-e15).

## Capabilities

### filesystem

**I — Files** (source): Local context loading uses files and ordinary Python exec context; Docker own workspace/container. Custom tools can supply further filesystem APIs.

Evidence: [e6](#evidence-e6), [e7](#evidence-e7), [e17](#evidence-e17).

### processes

**S — OS programs** (source): Owned IPython subprocess/Docker environment lifecycle and Python/custom tool computation; no native durable command/process registry traced.

Evidence: [e7](#evidence-e7), [e13](#evidence-e13), [e17](#evidence-e17).

### code-actions

**I — Code actions** (source): Model Python code executes per response block, with context variables, custom tools, llm_query/rlm_query and answer-ready scaffold.

Evidence: [e3](#evidence-e3), [e5](#evidence-e5), [e7](#evidence-e7).

### persistent-kernel

**L — Kernel** (source): persistent=True retains owned local/IPython/Docker environment across completion calls and versioned inputs/history; close destroys it; no harness-rebuild heap reattach/shared multi-consumer lease.

Evidence: [e1](#evidence-e1), [e6](#evidence-e6), [e9](#evidence-e9), [e13](#evidence-e13), [e17](#evidence-e17).

### standing-database

**S — Standing DB** (source): Context_N/history_N and optional trajectory JSONL hold standing in-memory/file data; no general transactional standing database supplied.

Evidence: [e6](#evidence-e6), [e14](#evidence-e14).

### workflow-programming

**I — Workflows** (source): Model Python orchestrates sequential/batched plain or recursive submodels under depth/concurrency caps; custom tool/sub-tool propagation.

Evidence: [e3](#evidence-e3), [e4](#evidence-e4), [e5](#evidence-e5).

### multi-model

**I — Models** (source): Named/depth-routed handler clients and model overrides for code subcalls; recursive child configuration propagation, distinct model roles.

Evidence: [e1](#evidence-e1), [e4](#evidence-e4), [e5](#evidence-e5).

### live-collaboration

**L — Peer chat** (source): Recursive prompt/result subcalls plus callbacks/metadata, not interactive peer room/mail ownership or shared mutable chat.

Evidence: [e4](#evidence-e4), [e5](#evidence-e5), [e10](#evidence-e10).

### concurrent-work

**I — Concurrency** (source): Batched recursive thread pool with ordered results and IPython broker global semaphore over requests, plus provider handler threads.

Evidence: [e5](#evidence-e5), [e10](#evidence-e10).

### steering-interrupt

**L — Steer/interrupt** (source): Root timeout checks between iterations; subprocess cell interrupt does not cancel ongoing host subcall. Shutdown rejects new callbacks but running ones finish, stragglers discarded from future-cell view.

Evidence: [e2](#evidence-e2), [e11](#evidence-e11), [e12](#evidence-e12), [e18](#evidence-e18).

### turn-redefinition

**S — Turn program** (source): Model chooses code-based orchestration/recursive completion and custom tools/system policy; no normal-run hot replacement of outer turn algorithm established.

Evidence: [e3](#evidence-e3), [e4](#evidence-e4), [e5](#evidence-e5), [e7](#evidence-e7).

### compaction

**I — Compaction** (source): Threshold summary replaces provider history and retains prior formatted segments in REPL history; supports local/Docker when append method present, no summary fidelity oracle.

Evidence: [e1](#evidence-e1), [e2](#evidence-e2), [e3](#evidence-e3), [e6](#evidence-e6), [e16](#evidence-e16).

### context-repair

**S — Repair** (source): Model can inspect versioned original inputs/history/SHOW_VARS after compaction; scaffold restored to stop overwritten context/helper aliases surviving cells. No semantic repair/audit-tamper policy.

Evidence: [e3](#evidence-e3), [e6](#evidence-e6), [e7](#evidence-e7).

### original-audit

**L — Original audit** (source): Optional logger retains iteration prompt/response/code/results/subcalls; histories deep copied, but locals projected/shallow copies and late-cell completions discarded, optional disk/no raw external effect completeness.

Evidence: [e6](#evidence-e6), [e7](#evidence-e7), [e12](#evidence-e12), [e14](#evidence-e14), [e15](#evidence-e15), [e18](#evidence-e18).

### audit-query

**S — Audit query** (source): Structured JSONL/get_trajectory and REPL-accessible versioned histories enable study; no complete original query DB or immutable custody.

Evidence: [e6](#evidence-e6), [e14](#evidence-e14), [e15](#evidence-e15).

### hot-change

**S — Hot change** (source): Persistent env handler address repointed each completion and custom code/tools mutable inside REPL; not source/module hot reload or versioned activation barrier.

Evidence: [e1](#evidence-e1), [e7](#evidence-e7).

### rebuild-continuity

**L — Rebuild continuity** (source): Persistent means same owned object reused; close destroys state/container/kernel, logger serializes projections not restorable arbitrary heap or unresolved callbacks.

Evidence: [e9](#evidence-e9), [e12](#evidence-e12), [e13](#evidence-e13), [e14](#evidence-e14), [e15](#evidence-e15), [e17](#evidence-e17).

### remote-services

**I — Remote** (source): Provider handler clients and remote environment registry/custom tool boundaries consume external services; owning created environment differs from separate shared-service consumer.

Evidence: [e1](#evidence-e1), [e4](#evidence-e4), [e9](#evidence-e9), [e17](#evidence-e17).

### self-improvement

**S — Self-improve** (source): Model can code workflows and recursive calls; training advertised separately, selected inference path no self-harness candidate evaluation/promotion loop.

Evidence: [e3](#evidence-e3), [e4](#evidence-e4), [e5](#evidence-e5).

### complaints

**S — Complaints** (source): Code stderr/iteration and recursive call metadata plus cell-ID attribution are diagnostic ingredients; no external complaint table/state bead contract.

Evidence: [e7](#evidence-e7), [e10](#evidence-e10), [e14](#evidence-e14).

### authority

**L — Authority** (source): Local Python exec shares host dependencies and process-global capture/cwd; restricted builtin scaffold is not isolation proof. IPython subprocess/container boundary owns cleanup, callbacks execute parent code.

Evidence: [e7](#evidence-e7), [e8](#evidence-e8), [e10](#evidence-e10), [e11](#evidence-e11), [e13](#evidence-e13), [e17](#evidence-e17).

### evaluation

**S — Evaluation** (source): Budget/error/depth/time limits and direct mock/persistence/race assertions; quality benchmark/training truth not established by these runtime tests.

Evidence: [e2](#evidence-e2), [e4](#evidence-e4), [e18](#evidence-e18).

### time-order

**L — Time/order** (source): Versioned contexts/history, ordered batch results, cell IDs and timestamped iterations support causal attribution; running stale callbacks can outlive cells and discarded metadata prevents complete settlement audit.

Evidence: [e5](#evidence-e5), [e6](#evidence-e6), [e10](#evidence-e10), [e12](#evidence-e12), [e14](#evidence-e14), [e18](#evidence-e18).

## Inspected test oracles

- [quarantine/rlm/tests/test_local_repl_persistent.py](../../../quarantine/rlm/tests/test_local_repl_persistent.py): context/history versioning, copy semantics and code readability Oracle: Exact context aliases/counts and history deep-copy mutation check; code reads history_0/1/2 and compares list. Tests object lifetime multi-turn data, not restart serialization/shared clients. Read, **not executed**.
- [quarantine/rlm/tests/test_rlm_query.py](../../../quarantine/rlm/tests/test_rlm_query.py): bounded concurrent child calls and deterministic result/error metadata Oracle: Mock/tracked subcalls assert output order/error strings, peak concurrency ceiling and repaired overwritten helpers. Ceiling <=3 does not prove full parallelism despite test description. Read, **not executed**.
- [quarantine/rlm/tests/test_ipython_repl.py](../../../quarantine/rlm/tests/test_ipython_repl.py): late provider callback cell attribution and global semaphore/reentrancy Oracle: Read actual timeout race: cell A times out before callback complete, B waits while A finishes, then B rlm_calls empty. Proves isolation of bookkeeping by cell and explicitly continuing provider callback, not quiescence or preserved all-original audit. Read, **not executed**.

## Useful mechanisms

- Context as ordinary data and code-chosen recursive model calls give direct model orchestration agency.
- Persistent versioned inputs/history and compaction work across retained REPL object, with child metadata.
- Cell attribution/semaphore/race assertions name concrete lifetime hazards rather than hiding them.

## Material limits

- persistent=True is object/environment retention, not rebuild heap restoration/shared compute lease.
- Kernel interrupt can leave parent provider calls active; broker shutdown does not settle them and stale bookkeeping may be discarded.
- Local global stdout/cwd changes and structured logger projections are bounded isolation/audit scopes.

## Arconaut design questions

- Can all late callbacks retain original causal/effect records even when model-context projection suppresses them?
- How should shared context objects remain operator/model queryable without giving consumer authority over kernel lifetime?
- What strict no-active-provider-work barrier precedes arcorefit while independently governed services keep running?

## Evidence

### Evidence e1

[quarantine/rlm/rlm/core/rlm.py:225–298](../../../quarantine/rlm/rlm/core/rlm.py#L225): Each completion starts LM handler; persistent environment updates handler/adds context, otherwise builds environment; finally stops handler/cleans nonpersistent env.

### Evidence e2

[quarantine/rlm/rlm/core/rlm.py:350–432](../../../quarantine/rlm/rlm/core/rlm.py#L350): Completion per-iteration timeout/budget, optional compaction, model prompt then code execution, logs iteration/final answer.

### Evidence e3

[quarantine/rlm/rlm/core/rlm.py:602–672](../../../quarantine/rlm/rlm/core/rlm.py#L602): Managed compaction summarizes trajectory to REPL history then replaces prompt; model response code blocks executed sequentially and returned as iteration.

### Evidence e4

[quarantine/rlm/rlm/core/rlm.py:806–836](../../../quarantine/rlm/rlm/core/rlm.py#L806): Recursive child inherits tool/model/environment/depth/budget/time/concurrency settings, separate child logger captures trajectory; not peer room.

### Evidence e5

[quarantine/rlm/rlm/environments/local_repl.py:258–397](../../../quarantine/rlm/rlm/environments/local_repl.py#L258): llm_query/batched direct calls; rlm_query/batched children, bounded thread pool waits all and records completions original prompt order.

### Evidence e6

[quarantine/rlm/rlm/environments/local_repl.py:399–490](../../../quarantine/rlm/rlm/environments/local_repl.py#L399): Versioned context_N/history_N, first-context aliases, deep-copy history and compaction history segments; temporary context file loading.

### Evidence e7

[quarantine/rlm/rlm/environments/local_repl.py:547–602](../../../quarantine/rlm/rlm/environments/local_repl.py#L547): Python exec retained namespace; restores scaffold only on success, exceptions return stderr, shallow locals/result copy; cleanup clears state/temp directory.

### Evidence e8

[quarantine/rlm/rlm/environments/local_repl.py:491–512](../../../quarantine/rlm/rlm/environments/local_repl.py#L491): Per-instance output lock swaps process-global sys.stdout/stderr and cwd during local cell execution; not universal inter-instance isolation.

### Evidence e9

[quarantine/rlm/rlm/core/rlm.py:872–914](../../../quarantine/rlm/rlm/core/rlm.py#L872): Persistence supports only local/ipython/docker protocol; close cleans owned persistent environment, no serialize/reattach heap checkpoint.

### Evidence e10

[quarantine/rlm/rlm/environments/ipython_repl.py:125–205](../../../quarantine/rlm/rlm/environments/ipython_repl.py#L125): Broker bounds ALL in-flight subcalls with semaphore, per-cell metadata attribution and shutdown admission check.

### Evidence e11

[quarantine/rlm/rlm/environments/ipython_repl.py:1294–1358](../../../quarantine/rlm/rlm/environments/ipython_repl.py#L1294): Subprocess cell executes with timeout, on TimeoutError interrupts kernel and drains per-cell metadata; cannot itself cancel host provider subcall.

### Evidence e12

[quarantine/rlm/rlm/environments/ipython_repl.py:257–284](../../../quarantine/rlm/rlm/environments/ipython_repl.py#L257): Stop rejects NEW subcall work; source says already running callbacks continue; drain discards other-cell stragglers to avoid attribution bleed.

### Evidence e13

[quarantine/rlm/rlm/environments/ipython_repl.py:1451–1486](../../../quarantine/rlm/rlm/environments/ipython_repl.py#L1451): Cleanup stops channels, shuts kernel now and broker; in-process shell reset. No preservation of active callbacks across refit.

### Evidence e14

[quarantine/rlm/rlm/logger/rlm_logger.py:18–89](../../../quarantine/rlm/rlm/logger/rlm_logger.py#L18): Optional per-completion memory/JSONL iteration logger, clears current iteration view for next completion; not crash-recovery/core-everything audit contract.

### Evidence e15

[quarantine/rlm/rlm/core/types.py:188–229](../../../quarantine/rlm/rlm/core/types.py#L188): Serialized iteration retains prompt/raw textual response/code/results/recursive calls with locals projected through serializer; not full Python heap image.

### Evidence e16

[quarantine/rlm/rlm/core/rlm.py:438–480](../../../quarantine/rlm/rlm/core/rlm.py#L438): Persistent completion adds message history after success/default final; compaction appends new trajectory segments after continuing iterations.

### Evidence e17

[quarantine/rlm/rlm/environments/docker_repl.py:730–761](../../../quarantine/rlm/rlm/environments/docker_repl.py#L730): Docker cleanup removes owned container/proxy, best-effort workspace cleanup; separate container does not imply standing service lease.

### Evidence e18

[quarantine/rlm/tests/test_ipython_repl.py:633–662](../../../quarantine/rlm/tests/test_ipython_repl.py#L633): Race oracle proves timed-out originating cell subcall still runs while next cell executes, and late completion is not attributed to next cell.

