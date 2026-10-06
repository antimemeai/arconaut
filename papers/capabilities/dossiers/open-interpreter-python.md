# open-interpreter-python

The original Python lineage exposes executable code and computer APIs with real cross-action Python/shell state, configurable continuation and auto-run. Selective transcript output, weak stop semantics and shared-state helper concurrency constrain the claims.

Role: programmable local computer/code agent. Runtime: Python.

Pinned source: [https://github.com/endolith/open-interpreter](https://github.com/endolith/open-interpreter); revision/version `e77c93612380ccd8a4954de1bd295a4be0ebb875`.

Synchronous streamed provider→code loop; cached Jupyter and subprocess language runtimes; async server wraps response threads

Owns local language/kernel processes and computer automation; consumes model API via LiteLLM and optional server clients

Inspection: Read loop/request/code admission and streaming store, terminal language caching, Jupyter/subprocess/shell run-stop-terminate, async input/settings, files/skills APIs, request trimming and representative literal tests.

Limits of this study: No imported/executed source. Full computer-use alternative host, all language/browser/desktop backends, profiles and provider dependency internals not exhaustively traced; no feature transferred from current Rust Open Interpreter.

## Actions

### language code action

Surface: model code action.

Input: Python/shell/other supported language and code

Result: streamed console/image/code output and active-line indicators

Lifecycle: cached runtime across actions; confirmation before run; Python interrupt requests preserve kernel

Authority: model under confirmation or standing auto_run

Evidence: [execute](#evidence-execute), [terminal](#evidence-terminal), [kernel](#evidence-kernel), [store](#evidence-store).

### computer.files.edit/search

Surface: code API.

Input: path and original/replacement text or search arguments

Result: actual edit or dependency search results/errors

Lifecycle: synchronous file replacement; no atomic multi-file reservation

Authority: admitted Python program with computer API

Evidence: [files](#evidence-files), [execute](#evidence-execute).

### computer.stop/terminate; interpreter.reset

Surface: runtime API.

Input: active language collection/reset request

Result: flag/signal/channel stop, cleared language/message state

Lifecycle: stop differs per language and shell inherits no-op; terminate no subprocess wait; reset combines context and runtime destruction

Authority: host/operator/program access to objects

Evidence: [terminal](#evidence-terminal), [kernel-stop](#evidence-kernel-stop), [base-stop](#evidence-base-stop), [subprocess](#evidence-subprocess), [reset](#evidence-reset).

### async input go/stop/new-message

Surface: server API.

Input: chunked message or matching approval digest/command

Result: streamed reply, error or stopped response

Lifecycle: go resumes existing approval thread; new message/stop joins current thread synchronously

Authority: server client under configured authentication and exact approval match

Evidence: [async](#evidence-async).

### settings/model/custom instructions

Surface: operator/server/runtime config.

Input: attribute/model configuration

Result: new per-use settings or partial-mutation error

Lifecycle: model setter invalidates config; settings no atomic candidate/deferred workflow fence

Authority: operator/server permitted fields; model may manipulate its admitted code environment

Evidence: [settings](#evidence-settings), [models](#evidence-models), [loop](#evidence-loop).

### skills import/create/save

Surface: code API.

Input: opt-in Python skill files or teaching steps

Result: reusable function source and persisted file

Lifecycle: imports may repeat partial effects on fallback; save existence success has no experimental promotion oracle

Authority: admitted computer API, opt-in skills

Evidence: [skills](#evidence-skills), [skill-save](#evidence-skill-save).

### computer.ai parallel map/reduce helper

Surface: code API.

Input: text chunks/query and one llm object

Result: ordered list/reduced prose

Lifecycle: thread pool temporarily mutates/restores shared interpreter context per request, isolation not guaranteed

Authority: code API invoking same model object

Evidence: [ai-concurrency](#evidence-ai-concurrency).

## Capabilities

### filesystem

**I — Files** (source): Arbitrary admitted kernel/shell programs access files; actual computer.files edit reads/replaces/writes, search delegates external aifs.

Evidence: [execute](#evidence-execute), [files](#evidence-files).

### processes

**L — OS programs** (source): Persistent shell subprocess with streaming two readers and code-completion marker; no model live PID/job result or confirmed process-tree cleanup. Shell stop inherits no-op; terminate has no wait.

Evidence: [subprocess](#evidence-subprocess), [shell](#evidence-shell), [base-stop](#evidence-base-stop).

### code-actions

**I — Code actions** (source): Model language/code blocks execute directly in cached language runtime; computer API enables further file/automation/program composition.

Evidence: [execute](#evidence-execute), [terminal](#evidence-terminal), [files](#evidence-files).

### persistent-kernel

**I — Kernel** (source): Actual owned Jupyter Python kernel/client and cached language objects persist across actions; reset terminates them. Namespace/image serialization or shared service ownership not established.

Evidence: [kernel](#evidence-kernel), [terminal](#evidence-terminal), [reset](#evidence-reset).

### standing-database

**? — Standing DB** (inspection scope): standing_database: not established in the inspected Python core, code/language, settings and computer helper paths; not a universal absence claim.

### workflow-programming

**S — Workflows** (source): Executable programs and opt-in reusable skill definitions compose computer actions; configured continuation text loop is an actual policy but not durable arbitrary workflow scheduling.

Evidence: [execute](#evidence-execute), [continue](#evidence-continue), [skills](#evidence-skills), [skill-save](#evidence-skill-save).

### multi-model

**I — Models** (source): Current model/API configuration is captured into request; model setter invalidates loaded configuration. No independently modeled concurrent colleague roles established.

Evidence: [models](#evidence-models).

### live-collaboration

**L — Peer chat** (source): Parallel computer.ai map/reduce shares and temporarily replaces one interpreter conversation rather than isolated addressable colleagues; source-inferred state interference risk.

Evidence: [ai-concurrency](#evidence-ai-concurrency).

### concurrent-work

**L — Concurrency** (source): Listener/reader threads and parallel AI chunk helpers overlap, but shared interpreter mutation and lack of ongoing model-facing owner handles limit meaningful independent work.

Evidence: [kernel-run](#evidence-kernel-run), [subprocess](#evidence-subprocess), [ai-concurrency](#evidence-ai-concurrency).

### steering-interrupt

**L — Steer/interrupt** (source): Async new input stops/joins current response, not queued noninterrupt steering. Kernel interrupt is requested via flags; blocking stdin helper can delay it. Shell stop inherited no-op and terminate unjoined.

Evidence: [async](#evidence-async), [kernel-run](#evidence-kernel-run), [kernel-stop](#evidence-kernel-stop), [base-stop](#evidence-base-stop), [subprocess](#evidence-subprocess).

### turn-redefinition

**S — Turn program** (source): Current custom/system instructions re-render per iteration and configured loop breakers govern continuation; Python host remains fixed respond function, no general hot turn program established.

Evidence: [loop](#evidence-loop), [continue](#evidence-continue).

### compaction

**L — Compaction** (source): Provider request token trimming is real with fallback/fail-open, not a managed model-visible summary event. Stored console truncation destroys originals independently.

Evidence: [trim](#evidence-trim), [store](#evidence-store).

### context-repair

**L — Repair** (source): Current conversation data is available and saved, but clipped console/review events unavailable; reset also terminates computation, with no transformation lineage repair operation established.

Evidence: [store](#evidence-store), [save](#evidence-save), [reset](#evidence-reset).

### original-audit

**L — Original audit** (source): Current JSON transcript overwrites after response, clips program output and excludes reviews/active-line events; request conversion/trimming and stdin-helper calls not comprehensively captured as original attempts.

Evidence: [store](#evidence-store), [save](#evidence-save), [trim](#evidence-trim), [kernel-run](#evidence-kernel-run).

### audit-query

**S — Audit query** (source): Saved JSON/current message list can be read through ordinary code; no original audit query beyond selective transcript.

Evidence: [save](#evidence-save), [execute](#evidence-execute), [store](#evidence-store).

### hot-change

**L — Hot change** (source): Prompt re-read/model setter and settings mutate subsequent use; settings fields apply incrementally with possible partial failure, not default affected-turn/workflow deferred atomic activation.

Evidence: [loop](#evidence-loop), [models](#evidence-models), [settings](#evidence-settings).

### rebuild-continuity

**L — Rebuild continuity** (source): Saved clipped conversation and persistent runtime objects support ordinary operation; reset terminates them and no compile/outpost/re-inhabitation protocol or VM snapshot traced.

Evidence: [save](#evidence-save), [reset](#evidence-reset), [kernel](#evidence-kernel).

### remote-services

**S — Remote** (source): Async settings/input/output server exposes local host to clients; configured API endpoint consumes provider services. Remote compute identity/reconnect/cancellation not traced.

Evidence: [async](#evidence-async), [settings](#evidence-settings), [models](#evidence-models).

### self-improvement

**S — Self-improve** (source): Opt-in generated skill write/import defines reusable functions but success checks existence; no independent quality/performance oracle or harness refit.

Evidence: [skill-save](#evidence-skill-save), [skills](#evidence-skills).

### complaints

**? — Complaints** (inspection scope): complaints: not established in the inspected Python core, code/language, settings and computer helper paths; not a universal absence claim.

### authority

**I — Authority** (source): Default path yields execution confirmation, auto_run suppresses it; admitted ordinary code operates through language runtime/computer APIs. Server settings disallow selected sensitive fields separately.

Evidence: [execute](#evidence-execute), [store](#evidence-store), [settings](#evidence-settings).

### evaluation

**S — Evaluation** (source): Read tests assert exact smoke markers and deliberate stream-recording loss/confirmation behavior; not a cross-cell persistence, shared-helper isolation or process-death oracle.

Evidence: [runtime-test](#evidence-runtime-test), [core-test](#evidence-core-test).

### time-order

**L — Time/order** (source): Jupyter silence handling uses wall time; code completion uses IOPUB idle/output marker rather than recorded causal request ID in traced listener. Server stop synchronously joins without a bound.

Evidence: [kernel-run](#evidence-kernel-run), [shell](#evidence-shell), [async](#evidence-async).

## Inspected test oracles

- [quarantine/open-interpreter-python/tests/core/test_core_extended.py](../../../quarantine/open-interpreter-python/tests/core/test_core_extended.py): confirmation suppression and deliberate transcript loss Oracle: Mock respond stream asserts zero yielded confirmations for auto_run, truncated stored content and ephemeral exclusions; not execution authority/cancellation integration. Read, **not executed**.
- [quarantine/open-interpreter-python/tests/test_language_subprocess.py](../../../quarantine/open-interpreter-python/tests/test_language_subprocess.py): basic language execution stdout Oracle: Actual kernel/shell marker tests with bounded pytest timeout and optional runtime skips; no namespace/cancel/foreign effect lifetime oracle. Read, **not executed**.

## Useful mechanisms

- Actual cross-action Python/shell state and streamed language-program interface.
- Programmatic computer operations and opt-in skill functions are compact model ergonomics.
- Matching approval resumes existing thread instead of spawning a duplicate.

## Material limits

- Owned computational runtimes rather than shared fabric consumer; reset destroys them with context.
- Shell stop no-op and async join/provider helper are unbounded; refit settlement unestablished.
- Source-inferred shared interpreter races in AI helper and normal-input finish/interrupt conflict; no execution.
- Transcript clips outputs and omits reviews; no complete original audit or governed skill experiment.

## Arconaut design questions

- Expose persistent service clients with identity and outcomes independently from selected context/reset.
- Correlate kernel output/status to exact action request, with accepted stop separate from observed settlement.
- Give parallel model helper distinct context/model state and record request/result attempts.

## Evidence

### Evidence loop

[quarantine/open-interpreter-python/interpreter/core/respond.py:14–88](../../../quarantine/open-interpreter-python/interpreter/core/respond.py#L14): Each iteration re-renders current system/custom/language/computer instructions and calls selected model on copied messages.

### Evidence execute

[quarantine/open-interpreter-python/interpreter/core/respond.py:257–362](../../../quarantine/open-interpreter-python/interpreter/core/respond.py#L257): Validates language/code, yields confirmation, re-reads edited code, syncs computer config and streams computer.run results.

### Evidence continue

[quarantine/open-interpreter-python/interpreter/core/respond.py:423–459](../../../quarantine/open-interpreter-python/interpreter/core/respond.py#L423): Configured loop continues until model text contains a loop breaker, removes prior loop messages and coalesces assistant messages.

### Evidence store

[quarantine/open-interpreter-python/interpreter/core/core.py:310–435](../../../quarantine/open-interpreter-python/interpreter/core/core.py#L310): Stream recorder drops review/active-line events, auto_run suppresses confirmation and truncates stored console content after emitting output.

### Evidence save

[quarantine/open-interpreter-python/interpreter/core/core.py:270–302](../../../quarantine/open-interpreter-python/interpreter/core/core.py#L270): After successful full response, overwrites one current-conversation JSON file; not an append-only original event journal.

### Evidence reset

[quarantine/open-interpreter-python/interpreter/core/core.py:443–447](../../../quarantine/open-interpreter-python/interpreter/core/core.py#L443): Reset terminates all owned languages, clears messages and computer import flag; context reset also kills computation.

### Evidence terminal

[quarantine/open-interpreter-python/interpreter/core/computer/terminal/terminal.py:156–207](../../../quarantine/open-interpreter-python/interpreter/core/computer/terminal/terminal.py#L156): Caches language objects across actions, closes generator via stop, terminates/removes all runtime objects on reset.

### Evidence kernel

[quarantine/open-interpreter-python/interpreter/core/computer/terminal/languages/jupyter_language.py:41–87](../../../quarantine/open-interpreter-python/interpreter/core/computer/terminal/languages/jupyter_language.py#L41): Owns one started python3 Jupyter kernel/client, reused until channel stop/kernel shutdown.

### Evidence kernel-run

[quarantine/open-interpreter-python/interpreter/core/computer/terminal/languages/jupyter_language.py:133–223](../../../quarantine/open-interpreter-python/interpreter/core/computer/terminal/languages/jupyter_language.py#L133): IOPUB listener interrupts on finish/stop flag, may perform blocking model stdin-helper request after output silence, sets finish even before non-Ctrl-C stdin input; idle status ends run without parent-request correlation in this path.

### Evidence kernel-stop

[quarantine/open-interpreter-python/interpreter/core/computer/terminal/languages/jupyter_language.py:295–354](../../../quarantine/open-interpreter-python/interpreter/core/computer/terminal/languages/jupyter_language.py#L295): Exec sends code to cached client; output polls flags, stop sets finish_flag without joining listener or observing actual effect settlement.

### Evidence subprocess

[quarantine/open-interpreter-python/interpreter/core/computer/terminal/languages/subprocess_language.py:38–143](../../../quarantine/open-interpreter-python/interpreter/core/computer/terminal/languages/subprocess_language.py#L38): Keeps subprocess with separate stdout/stderr reader threads; terminate requests immediate process stop/closes streams without wait; write failure restarts/retries potentially uncertain code.

### Evidence base-stop

[quarantine/open-interpreter-python/interpreter/core/computer/terminal/base_language.py:12–36](../../../quarantine/open-interpreter-python/interpreter/core/computer/terminal/base_language.py#L12): Default stop and terminate are no-ops despite halt/state contract comment; shell inherits stop.

### Evidence shell

[quarantine/open-interpreter-python/interpreter/core/computer/terminal/languages/shell.py:8–61](../../../quarantine/open-interpreter-python/interpreter/core/computer/terminal/languages/shell.py#L8): Shell subclasses persistent SubprocessLanguage, injects marker to recognize completion; no wall timeout and no stop override in class.

### Evidence async

[quarantine/open-interpreter-python/interpreter/core/async_core.py:136–210](../../../quarantine/open-interpreter-python/interpreter/core/async_core.py#L136): New input or stop sets event, cancels pending approval then synchronously joins response thread in async handler; accepted go resumes existing approved thread rather than spawning duplicate.

### Evidence settings

[quarantine/open-interpreter-python/interpreter/core/async_core.py:805–840](../../../quarantine/open-interpreter-python/interpreter/core/async_core.py#L805): Settings API validates/mutates sequential attributes; can return error after earlier fields changed, no candidate atomicity or turn/workflow activation fence.

### Evidence trim

[quarantine/open-interpreter-python/interpreter/core/llm/llm.py:205–287](../../../quarantine/open-interpreter-python/interpreter/core/llm/llm.py#L205): Converts request messages and token-trims; unknown context fallback 8000 and trimming exceptions fall open. Does not retain transformation originals/lineage.

### Evidence models

[quarantine/open-interpreter-python/interpreter/core/llm/llm.py:283–337](../../../quarantine/open-interpreter-python/interpreter/core/llm/llm.py#L283): Provider request captures model/messages/current credentials/config; assigning model invalidates loaded model config for subsequent load.

### Evidence files

[quarantine/open-interpreter-python/interpreter/core/computer/files/files.py:8–36](../../../quarantine/open-interpreter-python/interpreter/core/computer/files/files.py#L8): Computer files helper forwards search to dependency and implements actual read/replace/write.

### Evidence skills

[quarantine/open-interpreter-python/interpreter/core/computer/skills/skills.py:94–148](../../../quarantine/open-interpreter-python/interpreter/core/computer/skills/skills.py#L94): Opt-in imports skill Python files into kernel; combined import failure retries individual files, without effect rollback.

### Evidence skill-save

[quarantine/open-interpreter-python/interpreter/core/computer/skills/skills.py:241–259](../../../quarantine/open-interpreter-python/interpreter/core/computer/skills/skills.py#L241): Writes generated reusable skill and execs definition; file existence is success proxy, not behavior improvement experiment.

### Evidence ai-concurrency

[quarantine/open-interpreter-python/interpreter/core/computer/ai/ai.py:80–110](../../../quarantine/open-interpreter-python/interpreter/core/computer/ai/ai.py#L80): Parallel map/reduce calls fast_llm on same llm, each mutating/restoring shared interpreter messages/system message; no per-participant isolation traced.

### Evidence core-test

[quarantine/open-interpreter-python/tests/core/test_core_extended.py:168–216](../../../quarantine/open-interpreter-python/tests/core/test_core_extended.py#L168): Mock stream asserts confirmation suppression, actual stored truncation and exclusion of ephemeral events; deliberate loss oracle.

### Evidence runtime-test

[quarantine/open-interpreter-python/tests/test_language_subprocess.py:58–98](../../../quarantine/open-interpreter-python/tests/test_language_subprocess.py#L58): Jupyter/Python and shell smoke tests check literal output markers, not cross-cell isolation/cancellation/settlement.

