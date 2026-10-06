# ldp

Explicit state transitions and trainable operation programs support agent-policy experiments; graph/state capture is narrower than complete original audit.

Role: trainable agent computation-graph and rollout framework. Runtime: Python.

Pinned source: [https://github.com/Future-House/ldp](https://github.com/Future-House/ldp); revision/version `7220ca1e06292b856b1a3f1fe2c94170bc933055`.

Async get_asv agent/environment loop, operation graph with call IDs, concurrent rollouts and prompt/memory optimizers.

Consumes Aviary action environments and supplied model chain; owns research rollout/optimizer state, not independent OS service governance.

Inspection: Read Simple/ReAct state and get_asv, rollout step/callback/error/timing, OpResult/OpCtx, LLM call capture, APE prompt update, HTTP agent client and window/optimizer tests.

Limits of this study: Not full memory/tree/local-weights agent variants; alleged DB rehydration comments are not verified implemented persistence; no providers or evaluation executed.

## Actions

### Agent.init_state / get_asv

Surface: agent API.

Input: Tools, prior typed state and observations

Result: OpResult action, next state and value

Lifecycle: One programmable decision step; caller drives environment continuation.

Authority: Agent program under supplied environment authority

Evidence: [step](#evidence-step), [rollout](#evidence-rollout).

### RolloutManager.sample_trajectories

Surface: rollout API.

Input: Agent, environments, callbacks and step/concurrency limits

Result: Transitions with action/reward/state/timing and failures

Lifecycle: Parallel research rollouts; no operator chat implied.

Authority: Caller experiment program

Evidence: [timer](#evidence-timer), [rollout](#evidence-rollout).

### APEOpt.aggregate_trajectory / update

Surface: learning API.

Input: Reward/gradient trajectories and prompt Op

Result: Mutated prompt and optimization trace

Lifecycle: Serialized optimizer update; template variables preserved or proposal discarded.

Authority: Optimizer/model policy program

Evidence: [opt](#evidence-opt).

### HTTPAgentClient.get_asv

Surface: remote API.

Input: Serialized state, observations/training mode

Result: Remote OpResult and next state/value

Lifecycle: Request/reply; call/runtime trace state not automatically migrated.

Authority: Remote agent client/server

Evidence: [remote](#evidence-remote).

## Capabilities

### filesystem

**— — Files** (role): filesystem: this reference supplies trainable agent computation-graph and rollout framework; an agent policy, model interface and context lifecycle are outside its supplied role.

### processes

**— — OS programs** (role): processes: this reference supplies trainable agent computation-graph and rollout framework; an agent policy, model interface and context lifecycle are outside its supplied role.

### code-actions

**— — Code actions** (role): code_actions: this reference supplies trainable agent computation-graph and rollout framework; an agent policy, model interface and context lifecycle are outside its supplied role.

### persistent-kernel

**— — Kernel** (role): persistent_kernel: this reference supplies trainable agent computation-graph and rollout framework; an agent policy, model interface and context lifecycle are outside its supplied role.

### standing-database

**? — Standing DB** (inspection scope): OpCtx mentions a DB backend in comments, but inspected class supplies only in-process dict/get/update/clear; no standing DB implementation established.

### workflow-programming

**S — Workflows** (source): Agent.get_asv, explicit next state, operation graphs, callbacks and optimizer programs are programmable Python interfaces.

Evidence: [step](#evidence-step), [rollout](#evidence-rollout).

### multi-model

**S — Models** (source): LLM op consumes configured model chain and instantiated model abstraction; no independent peer chat semantics.

Evidence: [model](#evidence-model).

### live-collaboration

**— — Peer chat** (role): live_collaboration: this reference supplies trainable agent computation-graph and rollout framework; an agent policy, model interface and context lifecycle are outside its supplied role.

### concurrent-work

**S — Concurrency** (source): Rollout manager uses optional semaphore and async callbacks; independent trajectories can proceed with explicit identities.

Evidence: [timer](#evidence-timer), [rollout](#evidence-rollout).

### steering-interrupt

**? — Steer/interrupt** (inspection scope): No live operator busy-message/cancellation semantics established in inspected get_asv/rollout/HTTP paths; external coroutine cancellation is not a full turn contract.

### turn-redefinition

**— — Turn program** (role): turn_redefinition: this reference supplies trainable agent computation-graph and rollout framework; an agent policy, model interface and context lifecycle are outside its supplied role.

### compaction

**S — Compaction** (source): Sliding-window transition selection and optional hiding preserve pairing shape; managed summary/repair policy not supplied.

Evidence: [state](#evidence-state).

### context-repair

**S — Repair** (source): Nonmutating prior states/transition records can supply omitted material if retained by caller; model-facing original retrieval/lineage repair operation not traced.

Evidence: [state](#evidence-state), [rollout](#evidence-rollout).

### original-audit

**L — Original audit** (source): Optional hidden/windowed active state, first-message LLM return and clearable in-process OpCtx are research observations, not complete retained originals.

Evidence: [state](#evidence-state), [model](#evidence-model), [context](#evidence-context).

### audit-query

**S — Audit query** (source): Run/call keyed OpCtx and Transition records are program-readable; clearable in-process research trace.

Evidence: [context](#evidence-context), [rollout](#evidence-rollout).

### hot-change

**S — Hot change** (source): Config/prompt Op values can change future get_asv calls, and optimizer sets prompt after template check; no settled-turn activation coordinator.

Evidence: [step](#evidence-step), [opt](#evidence-opt).

### rebuild-continuity

**L — Rebuild continuity** (source): HTTP serialized agent state supports remote state handoff, but OpCtx live data is excluded from model_dump and active calls/native process continuity are separate.

Evidence: [remote](#evidence-remote), [context](#evidence-context).

### remote-services

**S — Remote** (source): HTTP agent interface consumes remote decision service with serialized next-state handoff; no native executable outpost refit.

Evidence: [remote](#evidence-remote).

### self-improvement

**S — Self-improve** (source): APE/memory optimizer programs learn policy prompts from trajectories; traced prompt gate checks template compatibility rather than independent performance improvement.

Evidence: [opt](#evidence-opt).

### complaints

**— — Complaints** (role): complaints: this reference supplies trainable agent computation-graph and rollout framework; an agent policy, model interface and context lifecycle are outside its supplied role.

### authority

**S — Authority** (source): Registered environment owns tools; caller programs/optimizer can mutate prompts; generic operator approval/agency posture is application work.

Evidence: [step](#evidence-step), [opt](#evidence-opt).

### evaluation

**S — Evaluation** (source): Trajectory reward/gradient supplies feedback; window test checks protocol pairing, optimizer tests observe prompt trace/reset and task loss.

Evidence: [test](#evidence-test), [opt](#evidence-opt).

### time-order

**S — Time/order** (source): Monotonic per-operation timings plus trajectory timestep and call IDs; no restart/paused-time policy.

Evidence: [timer](#evidence-timer), [rollout](#evidence-rollout).

## Inspected test oracles

- [quarantine/ldp/tests/test_agents.py](../../../quarantine/ldp/tests/test_agents.py): Window transformation retains valid tool request/response transition. Oracle: Exactly four expected messages with user/hidden/request/response types; provider calls use test fixtures/cassettes, no raw original-audit assertion. Read, **not executed**.
- [quarantine/ldp/tests/test_optimizer.py](../../../quarantine/ldp/tests/test_optimizer.py): APE example aggregation, update reset and learned task loss. Oracle: Known good examples, empty consumed example set and growing prompt trace; task-specific loss improvement with retries, not universal harness invariants. Read, **not executed**.

## Useful mechanisms

- Ordinary explicit agent/environment states make research and alternative policy paradigms composable.
- Operations carry call identities and trainable parameters; state/trace are separable from model messages.

## Material limits

- A useful graph trace is not an immutable original audit; comments about DB persistence require separate implementation evidence.
- Template-compatible prompt updates are not independent improvement evidence or safe native core refit.

## Arconaut design questions

- How should active context, retained original state and learned policy parameters have separate identities/lifetimes?
- Can the model experiment with decision programs while independent correctness oracles remain outside editable policy?

## Evidence

### Evidence state

[quarantine/ldp/src/ldp/agent/simple_agent.py:26–108](../../../quarantine/ldp/src/ldp/agent/simple_agent.py#L26): Next-state creation can window transitions, hide prior environment state and hide action content without mutating prior state.

### Evidence step

[quarantine/ldp/src/ldp/agent/simple_agent.py:161–184](../../../quarantine/ldp/src/ldp/agent/simple_agent.py#L161): get_asv gets next state, resolves config/model op and appends chosen action.

### Evidence rollout

[quarantine/ldp/src/ldp/alg/rollout.py:411–472](../../../quarantine/ldp/src/ldp/alg/rollout.py#L411): Rollout explicitly calls agent then environment, stores before/after state/action/reward/done/truncated and timer metadata.

### Evidence context

[quarantine/ldp/src/ldp/graph/ops.py:339–397](../../../quarantine/ldp/src/ldp/graph/ops.py#L339): Op context stores run/call keyed data globally, excluded from model_dump and clearable; DB rehydration discussed but not implemented by these methods.

### Evidence model

[quarantine/ldp/src/ldp/graph/common_ops.py:263–328](../../../quarantine/ldp/src/ldp/graph/common_ops.py#L263): LLM call uses supplied model-chain configuration; result/logprob saved in Op context, returns first message.

### Evidence opt

[quarantine/ldp/src/ldp/alg/optimizer/ape.py:172–241](../../../quarantine/ldp/src/ldp/alg/optimizer/ape.py#L172): Optimizer selects trajectory examples and updates prompt under lock; template-variable check is gate, not held-out improvement test.

### Evidence remote

[quarantine/ldp/src/ldp/agent/agent_client.py:20–76](../../../quarantine/ldp/src/ldp/agent/agent_client.py#L20): HTTP client passes serialized state/observations/training mode and receives action/next state/value.

### Evidence timer

[quarantine/ldp/src/ldp/alg/rollout.py:70–111](../../../quarantine/ldp/src/ldp/alg/rollout.py#L70): Monotonic operation duration and semaphore limit bound rollout concurrency.

### Evidence test

[quarantine/ldp/tests/test_agents.py:836–864](../../../quarantine/ldp/tests/test_agents.py#L836): Sliding window test expects exact user/hidden/tool-request/tool-response sequence after five transitions.

