# jido

Pluggable strategies, explicit state and runtime-owned effects, with append-only interaction thread/checkpoint support.

Role: actor/action framework and runtime. Runtime: Elixir.

Pinned source: [https://github.com/agentjido/jido](https://github.com/agentjido/jido); revision/version `90b163eef87ace89a153e94f114c8dc736be2ccd`.

Immutable agent command decisions; OTP GenServer signal routing, supervised directives/actions and storage adapters.

Owns agent runtime/supervisors/ephemeral instance control state; user actions consume external programs/services. AI belongs to companion integration.

Inspection: command normalization/hooks, signal router/call/cast, directive effect execution, runtime store, thread/persistence/file adapter and test assertions

Limits of this study: Jido.Action/Jido.Signal dependency implementations are not in this reference; no arbitrary remote effect settlement or BEAM hot code upgrade traced.

## Actions

### Agent.cmd

Surface: application API.

Input: agent struct, action/instruction list and context/options

Result: new agent plus runtime directive list or validation error

Lifecycle: functional strategy decision with before/after hooks; action execution may be effectful

Authority: application owns strategy/modules and context

Evidence: [e1](#evidence-e1), [e5](#evidence-e5).

### AgentServer.call / cast

Surface: actor API.

Input: PID/logical server ID, typed Signal, call timeout

Result: updated agent/error or immediate enqueue acknowledgment

Lifecycle: serialized actor handling; cast is not completion receipt

Authority: application route/plugin policy

Evidence: [e2](#evidence-e2), [e3](#evidence-e3).

### Directive.Emit / RunInstruction

Surface: runtime extension.

Input: signal/dispatch or instruction/result_action/meta

Result: dispatch task or normalized result/effects re-enter strategy

Lifecycle: supervised external dispatch can be fire-and-forget; RunInstruction feeds command

Authority: authored directive handlers and propagated context

Evidence: [e4](#evidence-e4), [e5](#evidence-e5), [e7](#evidence-e7).

### Directive.Spawn / Schedule

Surface: runtime primitive.

Input: child spec/tag or delay_ms/message

Result: child PID/runtime effect or timer-delivered signal

Lifecycle: supervised child; process-local timer; explicit hard stop can orphan work

Authority: runtime/application supervisor

Evidence: [e6](#evidence-e6), [e7](#evidence-e7).

### Persist.hibernate / thaw

Surface: application storage API.

Input: storage adapter, agent module/key and state

Result: checkpoint or reconstructed agent/error with exact thread revision

Lifecycle: journal flush precedes checkpoint; thaw logical state, no live task migration

Authority: application adapter/service policy

Evidence: [e10](#evidence-e10), [e11](#evidence-e11), [e12](#evidence-e12).

### Thread.append / storage append_thread

Surface: application journal primitive.

Input: thread/entries, optional expected revision

Result: new revision and ordered entries or conflict

Lifecycle: append authored facts; projection separate; storage adapter durability varies

Authority: application decides what original events to append

Evidence: [e9](#evidence-e9), [e14](#evidence-e14).

### RuntimeStore.get/fetch/put/delete

Surface: instance coordination primitive.

Input: instance/hive/key/value

Result: ephemeral value/status

Lifecycle: survives API process but not owning instance restart

Authority: Jido instance control plane, not external service authority

Evidence: [e8](#evidence-e8).

## Capabilities

### filesystem

**S — Files** (source): Owned file adapter stores checkpoints/journals; user-supplied action modules can perform other I/O, not a built-in coding-agent file tool catalog.

Evidence: [e5](#evidence-e5), [e13](#evidence-e13), [e14](#evidence-e14).

### processes

**S — OS programs** (source): BEAM actor/task spawn and delayed signal primitives; OS process runner belongs to authored actions/dependencies, not core catalog.

Evidence: [e4](#evidence-e4), [e6](#evidence-e6).

### code-actions

**S — Code actions** (source): Validated authored action modules execute with state/context and feed result/effects into strategy; core is not a Python-code model action.

Evidence: [e1](#evidence-e1), [e5](#evidence-e5).

### persistent-kernel

**S — Kernel** (source): Long-lived GenServer/state can host application computation; no independent shared interpreter kernel supplied by actor lifetime.

Evidence: [e3](#evidence-e3), [e8](#evidence-e8).

### standing-database

**L — Standing DB** (source): Storage adapters retain checkpoints/threads; RuntimeStore is explicitly ephemeral instance-owned ETS, not a durable shared standing application DB.

Evidence: [e8](#evidence-e8), [e10](#evidence-e10), [e13](#evidence-e13).

### workflow-programming

**I — Workflows** (source): Strategies, normalized instructions, hooks, custom directive protocols, signal routing and supervised effects form actual programmable orchestration.

Evidence: [e1](#evidence-e1), [e2](#evidence-e2), [e5](#evidence-e5), [e6](#evidence-e6), [e7](#evidence-e7).

### multi-model

**— — Models** (source): Core explicitly has no provider integration; Jido.AI companion is separately studied.

Evidence: [e1](#evidence-e1).

### live-collaboration

**S — Peer chat** (source): Signal-addressed actors/parent-child spawn and dispatch compose peers; not an IRC/model collaboration UI without AI integration.

Evidence: [e3](#evidence-e3), [e4](#evidence-e4), [e6](#evidence-e6).

### concurrent-work

**I — Concurrency** (source): Supervised actor/task/dispatch processes can run concurrently; individual GenServer signal processing serial.

Evidence: [e3](#evidence-e3), [e4](#evidence-e4), [e6](#evidence-e6).

### steering-interrupt

**L — Steer/interrupt** (source): Signals enqueue state changes but hard stop can orphan async work/drop directives; no universal effect-settled pause/refit barrier.

Evidence: [e3](#evidence-e3), [e5](#evidence-e5), [e7](#evidence-e7).

### turn-redefinition

**S — Turn program** (source): Strategy.cmd and before/after hooks redefine application decision programs; routes initialized from modules, not automatically model-authored/hot core turns.

Evidence: [e1](#evidence-e1), [e2](#evidence-e2).

### compaction

**S — Compaction** (source): Append-only interaction Thread versus separate context projection is useful basis; summarizer/compaction policy outside core.

Evidence: [e9](#evidence-e9).

### context-repair

**S — Repair** (source): Original thread and explicit restore with rev check supply repair/reprojection inputs; no model-native repair action in core.

Evidence: [e9](#evidence-e9), [e12](#evidence-e12).

### original-audit

**L — Original audit** (source): Appended authored interaction entries preserved independently from context; not all action/provider/OS I/O automatically captured, with queue overflow/hard-stop gaps.

Evidence: [e5](#evidence-e5), [e7](#evidence-e7), [e9](#evidence-e9), [e14](#evidence-e14).

### audit-query

**S — Audit query** (source): Thread entries/checkpoint retrieval and revision pointers are inspectable storage primitives, not whole-machine audit database/query tools.

Evidence: [e9](#evidence-e9), [e12](#evidence-e12), [e14](#evidence-e14).

### hot-change

**? — Hot change** (inspection): No wired code_change/upgrade/refit mechanism established in command/router/persist paths; BEAM capability alone is not evidence.

### rebuild-continuity

**L — Rebuild continuity** (source): Logical checkpoint/thread/scheduler manifests reconstruct state and reject mismatch; process timers/tasks/ETS do not survive as active work. No outpost protocol.

Evidence: [e6](#evidence-e6), [e8](#evidence-e8), [e10](#evidence-e10), [e11](#evidence-e11), [e12](#evidence-e12).

### remote-services

**S — Remote** (source): Custom action and signal-dispatch integrations consume external services; shared service governance is not implied by agent supervision.

Evidence: [e4](#evidence-e4), [e5](#evidence-e5).

### self-improvement

**S — Self-improve** (source): Application-owned strategies/actions/protocols make experiment machinery implementable, but no model-governed automatic improvement loop supplied.

Evidence: [e1](#evidence-e1), [e7](#evidence-e7).

### complaints

**S — Complaints** (source): Error directives/queue-overflow and traced signals support diagnostics; full snapshot complaint tracking would need composition.

Evidence: [e4](#evidence-e4), [e5](#evidence-e5), [e7](#evidence-e7).

### authority

**S — Authority** (source): Runtime invokes authored module/route/plugin and action context; dependency/remote credentials and approval policy supplied by application.

Evidence: [e1](#evidence-e1), [e2](#evidence-e2), [e5](#evidence-e5).

### evaluation

**I — Evaluation** (source): Reusable storage contract asserts exact roundtrip/overwrite/delete/missing results; plugin integration asserts routing effects. No live-provider or crash-durability test execution here.

Evidence: [e13](#evidence-e13), [e15](#evidence-e15).

### time-order

**I — Time/order** (source): Thread revision, expected-rev append and checkpoint pointer check; GenServer signal order distinct from fire-and-forget task/timer completion.

Evidence: [e3](#evidence-e3), [e4](#evidence-e4), [e9](#evidence-e9), [e12](#evidence-e12), [e14](#evidence-e14).

## Inspected test oracles

- [quarantine/jido/test/support/storage_checkpoint_conformance.ex](../../../quarantine/jido/test/support/storage_checkpoint_conformance.ex): checkpoint adapter contract Oracle: Exact missing/roundtrip/overwrite/arbitrary-term/delete assertions shared across adapters; does not establish power-loss/fsync/thread semantics. Read, **not executed**.
- [quarantine/jido/test/jido/agent_server/plugin_signal_hooks_test.exs](../../../quarantine/jido/test/jido/agent_server/plugin_signal_hooks_test.exs): plugin routing authority Oracle: GenServer test asserts overridden action state and zero normal increment; error leaves state unchanged. Read, **not executed**.

## Useful mechanisms

- Separates logical strategy, effectful actions and runtime effect directives without falsely declaring all actions pure.
- Original interaction thread can be projected independently; checkpoint binds its exact revision.
- Direct actor/plugin and storage contract oracles are useful patterns.

## Material limits

- Core is AI-optional and no provider/kernel agent loop.
- Ephemeral ETS/timers and fire-and-forget dispatch are not durable replay/settlement.
- No traced BEAM hot upgrade makes language-level hot code support an Arconaut feature.

## Arconaut design questions

- Which effect directives deserve acknowledgment handles versus deliberate fire-and-forget?
- How should original events be automatically captured beyond application-chosen Thread entries?
- What explicit effect barrier is needed before checkpoint/recompile?

## Evidence

### Evidence e1

[quarantine/jido/lib/jido/agent.ex:904–957](../../../quarantine/jido/lib/jido/agent.ex#L904): Command hooks normalize instructions with current state/signal, delegate strategy.cmd and after-command; commands may perform effectful actions.

### Evidence e2

[quarantine/jido/lib/jido/agent_server/signal_router.ex:1–85](../../../quarantine/jido/lib/jido/agent_server/signal_router.ex#L1): Strategy/agent/plugin routes have explicit priority and target action/module/strategy command; router built from those sources.

### Evidence e3

[quarantine/jido/lib/jido/agent_server.ex:321–361](../../../quarantine/jido/lib/jido/agent_server.ex#L321): call/cast resolve agent server reference; GenServer call waits for signal result while cast enqueues asynchronously.

### Evidence e4

[quarantine/jido/lib/jido/agent_server/directive_executors.ex:10–55](../../../quarantine/jido/lib/jido/agent_server/directive_executors.ex#L10): Emit propagates trace context and dispatches self or supervised external dispatch task; no durable acknowledgment ledger shown.

### Evidence e5

[quarantine/jido/lib/jido/agent_server/directive_executors.ex:74–116](../../../quarantine/jido/lib/jido/agent_server/directive_executors.ex#L74): RunInstruction executes action then feeds normalized result/effects back into command, updating state and enqueuing directives; queue overflow logs and drops.

### Evidence e6

[quarantine/jido/lib/jido/agent_server/directive_executors.ex:151–218](../../../quarantine/jido/lib/jido/agent_server/directive_executors.ex#L151): Spawn starts supervised child; Schedule uses Process.send_after for attributed delayed signal, process-local timer not durable by itself.

### Evidence e7

[quarantine/jido/lib/jido/agent_server/directive_exec.ex:1–65](../../../quarantine/jido/lib/jido/agent_server/directive_exec.ex#L1): Custom directive protocol is extension surface; documented hard stop drops pending effects and can orphan async work; completion should be state, not exit.

### Evidence e8

[quarantine/jido/lib/jido/runtime_store.ex:1–25](../../../quarantine/jido/lib/jido/runtime_store.ex#L1): RuntimeStore ETS is instance-owned ephemeral coordination; survives its API process restart but not instance stop/restart.

### Evidence e9

[quarantine/jido/lib/jido/thread.ex:1–85](../../../quarantine/jido/lib/jido/thread.ex#L1): Canonical Thread is provider-agnostic append-only entries, monotonic rev; model context is projected rather than destructively stored there.

### Evidence e10

[quarantine/jido/lib/jido/persist.ex:165–180](../../../quarantine/jido/lib/jido/persist.ex#L165): Hibernate flushes thread journal then creates/stores checkpoint; separate operations, not atomic external-effect settlement.

### Evidence e11

[quarantine/jido/lib/jido/persist.ex:353–409](../../../quarantine/jido/lib/jido/persist.ex#L353): Checkpoint strips embedded runtime thread and stores id/rev pointer plus state/scheduler manifest.

### Evidence e12

[quarantine/jido/lib/jido/persist.ex:408–480](../../../quarantine/jido/lib/jido/persist.ex#L408): Thaw validates/migrates checkpoint, reconstructs agent/thread/manifest and checks state budget; thread rev mismatch/missing journal rejected.

### Evidence e13

[quarantine/jido/lib/jido/storage/file.ex:73–94](../../../quarantine/jido/lib/jido/storage/file.ex#L73): File checkpoint term serialization writes temp then rename; no fsync or multiprocess checkpoint CAS established here.

### Evidence e14

[quarantine/jido/lib/jido/storage/file.ex:189–215](../../../quarantine/jido/lib/jido/storage/file.ex#L189): Thread append validates expected revision, appends encoded entries then writes metadata; runtime effect occurs outside this storage sequence.

### Evidence e15

[quarantine/jido/test/jido/agent_server/plugin_signal_hooks_test.exs:131–176](../../../quarantine/jido/test/jido/agent_server/plugin_signal_hooks_test.exs#L131): Real GenServer/plugin tests assert override suppresses normal increment and rejection yields error, not merely no crash.

