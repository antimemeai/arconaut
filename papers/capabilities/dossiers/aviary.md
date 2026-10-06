# aviary

Typed action IDs, batch ordering, safe/unsafe concurrency and timing/error observations are useful execution primitives.

Role: agent environment/action/evaluation framework. Runtime: Python.

Pinned source: [https://github.com/Future-House/aviary](https://github.com/Future-House/aviary); revision/version `7167342915e656fba1d93af4a5f0549cb803cd76`.

Async environment steps, generated typed function tools, thread offload for sync actions and optional HTTP environment service.

Environment/tool supplier consumed by agent; supplied environments own task state, external services own remote execution.

Inspection: Read exports, tool schema/serialization, exec dispatch/locks/timeouts/results, HTTP client and concurrency/timeout tests.

Limits of this study: Not every domain environment/tool selector inspected; timeout/cancellation does not validate termination of arbitrary sync thread work.

## Actions

### Tool.from_function

Surface: tool-definition API.

Input: Python function, signature/docstring and concurrency_safe flag

Result: Model-facing typed schema plus native callback

Lifecycle: Callback live in environment; serialization omits implementation.

Authority: Registered environment function authority

Evidence: [tool](#evidence-tool).

### Environment.exec_tool_calls

Surface: action dispatch API.

Input: Ordered ToolRequestMessage, timeout/concurrency/error policy

Result: Call-ID-paired responses, typed/text/JSON content and time info

Lifecycle: Gather safe actions; local batch lock, sync thread offload.

Authority: Environment authority

Evidence: [exec](#evidence-exec), [response](#evidence-response).

### EnvironmentClient.reset / step

Surface: remote API.

Input: Task state/action and HTTP auth/timeout options

Result: Observations/tools or reward/done/truncated response

Lifecycle: Remote task service request; failures may be placeholder observations by opt-in.

Authority: Client/service authority

Evidence: [client](#evidence-client).

## Capabilities

### filesystem

**— — Files** (role): filesystem: this reference supplies agent environment/action/evaluation framework; an agent policy, model interface and context lifecycle are outside its supplied role.

### processes

**— — OS programs** (role): processes: this reference supplies agent environment/action/evaluation framework; an agent policy, model interface and context lifecycle are outside its supplied role.

### code-actions

**S — Code actions** (source): Python function callbacks are typed tool implementations; model emits structured calls, not arbitrary persistent executable programs.

Evidence: [tool](#evidence-tool).

### persistent-kernel

**? — Kernel** (inspection scope): Domain environment state can persist, but a general retained executable language namespace was not established in inspected generic dispatch/client paths.

### standing-database

**— — Standing DB** (role): standing_database: this reference supplies agent environment/action/evaluation framework; an agent policy, model interface and context lifecycle are outside its supplied role.

### workflow-programming

**S — Workflows** (source): Environment reset/step and supplied Python functions compose action domains; client/agent policy supplied separately.

Evidence: [exec](#evidence-exec), [client](#evidence-client).

### multi-model

**— — Models** (role): multi_model: this reference supplies agent environment/action/evaluation framework; an agent policy, model interface and context lifecycle are outside its supplied role.

### live-collaboration

**— — Peer chat** (role): live_collaboration: this reference supplies agent environment/action/evaluation framework; an agent policy, model interface and context lifecycle are outside its supplied role.

### concurrent-work

**S — Concurrency** (source): One exec batch gathers safe actions and serializes unsafe actions via local reader/writer lock; does not prove cross-batch global exclusion.

Evidence: [exec](#evidence-exec), [response](#evidence-response).

### steering-interrupt

**L — Steer/interrupt** (source): Coroutine wait timeout may cancel await, but synchronous function runs through to_thread; no promise the worker or external program is terminated.

Evidence: [exec](#evidence-exec).

### turn-redefinition

**— — Turn program** (role): turn_redefinition: this reference supplies agent environment/action/evaluation framework; an agent policy, model interface and context lifecycle are outside its supplied role.

### compaction

**— — Compaction** (role): compaction: this reference supplies agent environment/action/evaluation framework; an agent policy, model interface and context lifecycle are outside its supplied role.

### context-repair

**— — Repair** (role): context_repair: this reference supplies agent environment/action/evaluation framework; an agent policy, model interface and context lifecycle are outside its supplied role.

### original-audit

**L — Original audit** (source): Tool results/state frames capture structured observations and optional subset state; not all original message/provider/process bytes.

Evidence: [response](#evidence-response), [frame](#evidence-frame).

### audit-query

**— — Audit query** (role): audit_query: this reference supplies agent environment/action/evaluation framework; an agent policy, model interface and context lifecycle are outside its supplied role.

### hot-change

**S — Hot change** (source): Environment can register Python tools/functions, but serialized Tool removes callable; live code activation/client replacement requires application machinery.

Evidence: [tool](#evidence-tool).

### rebuild-continuity

**L — Rebuild continuity** (source): Serializable frames/tool schemas do not preserve callable implementation or active thread work across process replacement.

Evidence: [tool](#evidence-tool), [frame](#evidence-frame).

### remote-services

**S — Remote** (source): HTTP environment client reset/step consumes optional authenticated task service; transport reconnect/work identity is caller/service-specific.

Evidence: [client](#evidence-client).

### self-improvement

**— — Self-improve** (role): self_improvement: this reference supplies agent environment/action/evaluation framework; an agent policy, model interface and context lifecycle are outside its supplied role.

### complaints

**— — Complaints** (role): complaints: this reference supplies agent environment/action/evaluation framework; an agent policy, model interface and context lifecycle are outside its supplied role.

### authority

**S — Authority** (source): Registered Python tools inherit environment authority; safe/unsafe declaration and exception handling are caller policy, no generic model approval gate.

Evidence: [tool](#evidence-tool), [exec](#evidence-exec).

### evaluation

**S — Evaluation** (source): Actual mixed-concurrency counter test attacks overlap/exclusion; timeouts test raising TimeoutError, not termination of sync work.

Evidence: [test](#evidence-test), [exec](#evidence-exec).

### time-order

**S — Time/order** (source): Call response records monotonic start/end and preserves input call order; no persisted restart clock.

Evidence: [response](#evidence-response).

## Inspected test oracles

- [quarantine/aviary/tests/test_tools.py](../../../quarantine/aviary/tests/test_tools.py): Safe/unsafe execution overlap within one batch. Oracle: Unsafe result counter exactly 1 and at least one safe count >1; only one batch and cooperating sleep functions. Read, **not executed**.
- [quarantine/aviary/tests/test_envs.py](../../../quarantine/aviary/tests/test_envs.py): Per-tool await timeout. Oracle: pytest.raises TimeoutError for SlowEnv actions; does not observe whether sync worker keeps changing external state afterward. Read, **not executed**.

## Useful mechanisms

- Action schema, call/result pairing and lifecycle timing are explicit.
- Per-action concurrency safety declarations are stronger than blanket parallel JSON dispatch.

## Material limits

- Unsafe lock scope is one batch; concurrent calls into one environment need an owning policy.
- Serialization excludes executable callable, and timeout is not universal work termination.

## Arconaut design questions

- Which actions are concurrent-safe across whole workflow/turn, not merely one batch?
- Can every timeout/interruption report whether its external effect is still running rather than just returning an error?

## Evidence

### Evidence tool

[quarantine/aviary/src/aviary/tools/base.py:342–389](../../../quarantine/aviary/src/aviary/tools/base.py#L342): Typed tool metadata plus callable; concurrency_safe defaults true, callable excluded from ordinary serialized state.

### Evidence args

[quarantine/aviary/src/aviary/tools/base.py:54–82](../../../quarantine/aviary/src/aviary/tools/base.py#L54): Malformed JSON tool arguments become invalid tool name with empty arguments rather than trusted executable input.

### Evidence exec

[quarantine/aviary/src/aviary/env.py:199–284](../../../quarantine/aviary/src/aviary/env.py#L199): Each batch creates reader-writer lock, invokes async function or sync function in thread, and optionally times out/catches errors.

### Evidence response

[quarantine/aviary/src/aviary/env.py:286–355](../../../quarantine/aviary/src/aviary/env.py#L286): Typed/JSON/text results paired to call ID with monotonic start/end; concurrent gather then input-order responses.

### Evidence client

[quarantine/aviary/src/aviary/env_client.py:42–113](../../../quarantine/aviary/src/aviary/env_client.py#L42): HTTP reset/step client forwards optional API key and timeout; caller can turn HTTP errors into placeholder observations.

### Evidence frame

[quarantine/aviary/src/aviary/env.py:52–78](../../../quarantine/aviary/src/aviary/env.py#L52): Frame contains serializable full/subset state and metadata, optionally deepcopied; not guaranteed original IO corpus.

### Evidence test

[quarantine/aviary/tests/test_tools.py:1109–1157](../../../quarantine/aviary/tests/test_tools.py#L1109): Counter-based batch test asserts unsafe action count 1 and some safe action overlap.

