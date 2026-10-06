# jido-ai

Composable model request transforms, tool interceptors, steering and signed caller-owned continuation tokens; logical restore explicitly loses active process handles.

Role: AI integration and programmable ReAct runtime. Runtime: Elixir.

Pinned source: [https://github.com/agentjido/jido_ai](https://github.com/agentjido/jido_ai); revision/version `01b7dca0f8897980097260c6cd15515658c56676`.

Task-coordinated ReAct runner or Jido actor strategy; ReqLLM provider dependency, typed action modules, configurable concurrent tools.

Owns request/reasoning state and local retrieval memory, consumes provider/action/service dependencies; independent external kernel/DB lifecycle remains outside.

Inspection: request/stream loop, turn execution/tool batches, checkpoints/tokens, request transform/hooks, steering, retrieval and observable-event sanitization, direct test oracles

Limits of this study: ReqLLM/Jido.Action dependencies not populated here; no live model calls; no general automatic compaction/rebuild protocol established.

## Actions

### Agent.ask / ask_stream / ask_sync / await

Surface: operator/application API.

Input: actor PID, query, per-request options

Result: request ID/handle, event stream or final result/error

Lifecycle: task/actor-backed model-tool loop; asynchronous handle tracked to terminal

Authority: application owns provider credentials and exposed action roster

Evidence: [e1](#evidence-e1), [e2](#evidence-e2), [e7](#evidence-e7).

### Turn.execute / execute_module

Surface: application/model dispatch primitive.

Input: tool name/module, params, context, timeout and action map

Result: canonical result/error/effects

Lifecycle: await action; agent callbacks apply only in managed runtime, standalone bypass explicit

Authority: advertised typed action module and application context

Evidence: [e3](#evidence-e3), [e5](#evidence-e5).

### request_transformer.transform_request

Surface: program hook.

Input: request/state/config/runtime context

Result: validated effective model/messages/tools/options or error

Lifecycle: each model step; transformer participates in token fingerprint

Authority: authored module can redefine request; no implicit model permission needed

Evidence: [e6](#evidence-e6), [e16](#evidence-e16).

### before_tool_call / after_tool_call

Surface: program hooks.

Input: resolved tool identity+arguments/context or final retried result

Result: changed arguments, interrupt/error, changed result/effects

Lifecycle: before validation/retries; after once final result; standalone bypasses callbacks

Authority: application agent module/effect policy; identity immutable

Evidence: [e5](#evidence-e5).

### steer / inject / cancel

Surface: operator/program/inter-agent API.

Input: active actor/request ID, content/source/refs or reason

Result: accepted queue/error; injected event/canceled result

Lifecycle: steering drained at boundary; undrained best-effort input may drop; cancellation cooperative

Authority: expected request ID/source and application actor access

Evidence: [e7](#evidence-e7), [e8](#evidence-e8), [e14](#evidence-e14).

### ReAct.stream_from_state / rt2 Token.issue/decode_state

Surface: application continuation API.

Input: logical state/query/config, signed token

Result: streamed continuation/state or signature/expiry/config error

Lifecycle: after model/tools/terminal checkpoints; does not migrate active tasks

Authority: token secret and matching config fingerprint

Evidence: [e1](#evidence-e1), [e9](#evidence-e9), [e10](#evidence-e10).

### Retrieval.Store.upsert / recall / clear

Surface: application primitive.

Input: namespace/id/text/metadata/query/top_k

Result: stored memory or token-overlap-ranked results/deletion count

Lifecycle: in-process ETS lifetime; no external DB lease implied

Authority: application namespace policy

Evidence: [e12](#evidence-e12).

### AI agent checkpoint/restore sanitizer

Surface: storage integration.

Input: agent state containing requests/strategy

Result: serializable state with interrupted stream error and idle strategy

Lifecycle: drops live handles; new runtime after restore

Authority: application storage/agent callbacks

Evidence: [e11](#evidence-e11), [e15](#evidence-e15).

### Test.expect_react script

Surface: test API.

Input: user prompt, named tool calls and scripted answers

Result: deterministic provider boundary consumed by real runtime

Lifecycle: per test process tree or explicit script; no live provider

Authority: test author chooses expected sequence and must assert real output

Evidence: [e18](#evidence-e18).

## Capabilities

### filesystem

**S — Files** (source): Authored typed action modules can perform arbitrary application I/O/programs; no built-in unrestricted shell/kernel implied by tool execution.

Evidence: [e3](#evidence-e3), [e4](#evidence-e4).

### processes

**S — OS programs** (source): Authored typed action modules can perform arbitrary application I/O/programs; no built-in unrestricted shell/kernel implied by tool execution.

Evidence: [e3](#evidence-e3), [e4](#evidence-e4).

### code-actions

**S — Code actions** (source): Authored typed action modules can perform arbitrary application I/O/programs; no built-in unrestricted shell/kernel implied by tool execution.

Evidence: [e3](#evidence-e3), [e4](#evidence-e4).

### persistent-kernel

**L — Kernel** (source): Long-lived actors and checkpoint state are logical state; active worker/process handles reset explicitly, not retained interpreter heap.

Evidence: [e1](#evidence-e1), [e11](#evidence-e11).

### standing-database

**L — Standing DB** (source): Retrieval memory is namespace ETS with token-overlap recall; no shared durable service standing DB contract from this store.

Evidence: [e12](#evidence-e12).

### workflow-programming

**I — Workflows** (source): Reasoning strategies, per-turn request transforms and bounded concurrent action execution/interceptors are actual programmable orchestration.

Evidence: [e3](#evidence-e3), [e4](#evidence-e4), [e5](#evidence-e5), [e6](#evidence-e6).

### multi-model

**I — Models** (source): Effective model can change in per-turn transformer; separate actor configurations/strategies can compose models through Jido runtime.

Evidence: [e2](#evidence-e2), [e6](#evidence-e6).

### live-collaboration

**S — Peer chat** (source): Inter-agent inject is a wired same-run input primitive; Jido actor messaging supplies peers, but a full live multi-model room/roster is application composition.

Evidence: [e7](#evidence-e7), [e8](#evidence-e8).

### concurrent-work

**I — Concurrency** (source): Monitored runner tasks and bounded concurrent tools with original order results; timeouts/retries bound rounds, not remote transaction settlement.

Evidence: [e1](#evidence-e1), [e4](#evidence-e4).

### steering-interrupt

**L — Steer/interrupt** (source): Request-scoped cancel and steer/inject are real; steering can be dropped before drain and cancellation checks do not undo or universally stop external action effects.

Evidence: [e7](#evidence-e7), [e8](#evidence-e8), [e14](#evidence-e14).

### turn-redefinition

**I — Turn program** (source): Request transforms control model/messages/tools/options; agent before/after tool callbacks control arguments/results/effects. Identity restrictions and standalone bypass are explicit.

Evidence: [e5](#evidence-e5), [e6](#evidence-e6).

### compaction

**S — Compaction** (source): Transformer can replace model messages; inspected core does not supply managed summarization with original archive/semantic fidelity oracle.

Evidence: [e6](#evidence-e6), [e13](#evidence-e13).

### context-repair

**S — Repair** (source): Caller-owned continuation state and request message transform can rebuild context; redacted/truncated telemetry alone cannot restore lost originals.

Evidence: [e6](#evidence-e6), [e9](#evidence-e9), [e13](#evidence-e13).

### original-audit

**L — Original audit** (source): Identified sequence events/checkpoints and context capture are useful, but sanitization/redaction/truncation and configured delta capture prevent all-byte audit claim.

Evidence: [e10](#evidence-e10), [e13](#evidence-e13), [e14](#evidence-e14).

### audit-query

**S — Audit query** (source): Consumer receives identified events/checkpoint tokens; telemetry/result inspection external composition, not queryable all-original core DB.

Evidence: [e9](#evidence-e9), [e10](#evidence-e10), [e13](#evidence-e13).

### hot-change

**L — Hot change** (source): Per-turn transformer redefines effective request; checkpoint token fingerprint rejects changed model/transformer config rather than silently accepting arbitrary hot program replacement.

Evidence: [e6](#evidence-e6), [e9](#evidence-e9), [e16](#evidence-e16).

### rebuild-continuity

**L — Rebuild continuity** (source): Signed tokens support caller-owned completed-boundary continuation; logical actor checkpoint removes live worker/stream handles and interrupted requests fail. No effect-settled refit/outpost.

Evidence: [e9](#evidence-e9), [e10](#evidence-e10), [e11](#evidence-e11), [e15](#evidence-e15).

### remote-services

**I — Remote** (source): ReqLLM provider stream/generate integration and authored action service calls; ownership stays with independently configured service.

Evidence: [e2](#evidence-e2), [e3](#evidence-e3).

### self-improvement

**S — Self-improve** (source): Transformers/actions/test scripts can support controlled experiments; no model-governed hypothesis/promotion loop in inspected runtime.

Evidence: [e5](#evidence-e5), [e6](#evidence-e6), [e18](#evidence-e18).

### complaints

**S — Complaints** (source): Structured interceptor/provider/runtime errors and attributed events useful for complaints, but no full-state externally tracked bead action traced.

Evidence: [e3](#evidence-e3), [e5](#evidence-e5), [e8](#evidence-e8), [e10](#evidence-e10).

### authority

**I — Authority** (source): Named action roster, preflight, effect/interceptor policies and signed/fingerprinted continuation token; not mandatory per-command operator approval.

Evidence: [e3](#evidence-e3), [e4](#evidence-e4), [e5](#evidence-e5), [e9](#evidence-e9).

### evaluation

**I — Evaluation** (source): Scripted provider and direct checkpoint/signature/steering assertions test actual state transitions with mocked model boundary; not semantic model or external side-effect oracle.

Evidence: [e15](#evidence-e15), [e16](#evidence-e16), [e17](#evidence-e17), [e18](#evidence-e18).

### time-order

**I — Time/order** (source): Event IDs/monotonic seq; ordered concurrent tool output and heartbeat sequence adoption; checkpoint emitted before terminal to avoid lost final state.

Evidence: [e4](#evidence-e4), [e8](#evidence-e8), [e10](#evidence-e10).

## Inspected test oracles

- [quarantine/jido-ai/test/jido_ai/checkpoint_test.exs](../../../quarantine/jido-ai/test/jido_ai/checkpoint_test.exs): logical vs process continuity Oracle: Exact stream_interrupted/failed/idle states, no runtime handles and roundtrip serializable payload; old active request does not magically resume. Read, **not executed**.
- [quarantine/jido-ai/test/jido_ai/react/token_test.exs](../../../quarantine/jido-ai/test/jido_ai/react/token_test.exs): continuation authenticity/config drift Oracle: Tampered signature and changed model/transformer produce exact errors; no proof of physical external task settlement. Read, **not executed**.
- [quarantine/jido-ai/test/jido_ai/integration/react_steering_integration_test.exs](../../../quarantine/jido-ai/test/jido_ai/integration/react_steering_integration_test.exs): same-request steering Oracle: Stubbed provider captures exact user/assistant messages on second call; original handle resolves A2 and idle inject rejects. Read, **not executed**.

## Useful mechanisms

- Request/response/tool transform program hooks are substantial agency surfaces.
- Explicitly sanitizes dead process handles instead of fabricating active continuation.
- Signed caller-owned tokens and monotonic boundary events make continuity contract testable.

## Material limits

- Local ETS retrieval is not shared durable computation.
- Redacted bounded telemetry cannot serve complete original audit.
- Config mismatch limits token reuse across program refit; cooperative cancellation and retries do not settle external effects.

## Arconaut design questions

- How can a refit deliberately migrate/fingerprint logical state rather than merely reject changed config?
- How should model-program hooks be authored/reloaded while retaining original inputs/results?
- Should accepted steering return a durable delivery handle to avoid undrained input loss?

## Evidence

### Evidence e1

[quarantine/jido-ai/lib/jido_ai/reasoning/react/runner.ex:45–108](../../../quarantine/jido-ai/lib/jido_ai/reasoning/react/runner.ex#L45): Runner starts monitored task and streamed events; stream_from_state appends new input to checkpoint state.

### Evidence e2

[quarantine/jido-ai/lib/jido_ai/reasoning/react/runner.ex:491–530](../../../quarantine/jido-ai/lib/jido_ai/reasoning/react/runner.ex#L491): Stream/generate provider dispatch via ReqLLM; scripted testing boundary uses same consumption path.

### Evidence e3

[quarantine/jido-ai/lib/jido_ai/turn.ex:235–282](../../../quarantine/jido-ai/lib/jido_ai/turn.ex#L235): Tool name map resolves authored action module, timeout/context/options passed to execution; missing tool produces structured error.

### Evidence e4

[quarantine/jido-ai/lib/jido_ai/reasoning/react/runner.ex:706–787](../../../quarantine/jido-ai/lib/jido_ai/reasoning/react/runner.ex#L706): Tool preflight, active tool map, ordered bounded concurrent Task.async_stream with retries; heartbeat sequence range adopted before completion events.

### Evidence e5

[quarantine/jido-ai/lib/jido_ai/tool_interceptor.ex:1–86](../../../quarantine/jido-ai/lib/jido_ai/tool_interceptor.ex#L1): Before interceptor can change arguments or interrupt, not action identity; after interceptor changes result/effects once after retries; standalone Turn execution does not use agent callbacks.

### Evidence e6

[quarantine/jido-ai/lib/jido_ai/reasoning/react/runner.ex:338–382](../../../quarantine/jido-ai/lib/jido_ai/reasoning/react/runner.ex#L338): Per-turn request transformer overrides messages/tools/model/options and validates effective request; errors/exceptions structured.

### Evidence e7

[quarantine/jido-ai/lib/jido_ai/agent.ex:845–902](../../../quarantine/jido-ai/lib/jido_ai/agent.ex#L845): Public cancel emits request-scoped signal; steer/inject target active run with best-effort queue and explicitly drop if never drained before termination.

### Evidence e8

[quarantine/jido-ai/lib/jido_ai/reasoning/react/runner.ex:1752–1802](../../../quarantine/jido-ai/lib/jido_ai/reasoning/react/runner.ex#L1752): Drain injects attributed input into same state/context and emits input_injected; incomplete blank terminal responses rejected.

### Evidence e9

[quarantine/jido-ai/lib/jido_ai/reasoning/react/token.ex:1–91](../../../quarantine/jido-ai/lib/jido_ai/reasoning/react/token.ex#L1): Signed rt2 token includes run/request IDs, minimal state, config fingerprint, issuance/expiration; validates before continuation and can mark checkpoint canceled.

### Evidence e10

[quarantine/jido-ai/lib/jido_ai/reasoning/react/runner.ex:1245–1275](../../../quarantine/jido-ai/lib/jido_ai/reasoning/react/runner.ex#L1245): Checkpoint issued after model/tools/terminal; terminal token emitted before terminal event and sink closure. Events monotonic seq/run/request/iteration identities.

### Evidence e11

[quarantine/jido-ai/lib/jido_ai/checkpoint.ex:1–84](../../../quarantine/jido-ai/lib/jido_ai/checkpoint.ex#L1): Logical agent checkpoints remove request stream sinks/worker processes and reset interrupted runs to idle; interrupted streamed request recorded failed.

### Evidence e12

[quarantine/jido-ai/lib/jido_ai/retrieval/store.ex:1–89](../../../quarantine/jido-ai/lib/jido_ai/retrieval/store.ex#L1): Local namespace ETS memory upsert/recall/clear; retrieval is token-overlap ranking, not external durable vector/SQL service.

### Evidence e13

[quarantine/jido-ai/lib/jido_ai/observe/sanitize.ex:160–189](../../../quarantine/jido-ai/lib/jido_ai/observe/sanitize.ex#L160): Observation applies sensitive redaction, payload summaries and truncation markers; telemetry projection is not a complete original archive.

### Evidence e14

[quarantine/jido-ai/lib/jido_ai/reasoning/react/runner.ex:1700–1718](../../../quarantine/jido-ai/lib/jido_ai/reasoning/react/runner.ex#L1700): Tool args can be redacted and cancel is cooperative check of request ref; stopping underlying external action effect requires its implementation.

### Evidence e15

[quarantine/jido-ai/test/jido_ai/checkpoint_test.exs:82–111](../../../quarantine/jido-ai/test/jido_ai/checkpoint_test.exs#L82): Direct oracle asserts failed interrupted stream, no process-local handles, term serialization and restored idle state with new task supervisor.

### Evidence e16

[quarantine/jido-ai/test/jido_ai/react/token_test.exs:42–82](../../../quarantine/jido-ai/test/jido_ai/react/token_test.exs#L42): Token tampering/model/request-transformer mismatch rejected; config fingerprint deliberately constrains reuse under changed program.

### Evidence e17

[quarantine/jido-ai/test/jido_ai/integration/react_steering_integration_test.exs:60–95](../../../quarantine/jido-ai/test/jido_ai/integration/react_steering_integration_test.exs#L60): Mock provider observes next request containing Q1/Q2 and prior A1; original request yields A2, idle inject rejected.

### Evidence e18

[quarantine/jido-ai/lib/jido_ai/test.ex:1–73](../../../quarantine/jido-ai/lib/jido_ai/test.ex#L1): Public deterministic model scripts exercise actual ReAct/tools/event projection without claiming live model correctness.

