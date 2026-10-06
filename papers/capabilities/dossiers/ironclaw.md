# ironclaw

Strong typed lifecycle and projection boundaries, bounded native/WASM hooks and atomic swaps of individual provider wrappers; current composition disables model subagent spawning despite implementation/tests.

Role: native durable-process Rust agent. Runtime: Rust, TypeScript (web UI).

Pinned source: [https://github.com/nearai/ironclaw](https://github.com/nearai/ironclaw); revision/version `b0b999d96781516ee05e6ba961d6f3ead900da96`.

Current Reborn workspace: canonical sealed loop families/stages, durable turn/process coordination, native/WASM hook middleware and per-user Docker command substrate.

Owns turn/process journals and sandbox lifecycle; host ports consume model/filesystem/DB services. Per-user sandbox is governed by harness, unlike Arconaut consumer-only shared compute.

Inspection: Traced current workspace rather than removed src monolith: planner/stages, prompt middleware production wiring, process cancellation/supervisor, shell/sandbox, compaction artifacts/effectiveness, provider reload and subagent deny filter.

Limits of this study: No acquired code run. Every domain/extension, full original transcript storage, hook install entry point and crash recovery not exhaustively audited. Self-authored hook types not credited as exposed model operation; production caller not found in scope.

## Actions

### builtin.shell / scoped output

Surface: model capability.

Input: command/workdir/time/environment

Result: exit/status/rendered output and readable saved-output path

Lifecycle: new shell action in persistent workspace; hard120s; async owned sandbox task

Authority: Ask manifest plus runtime policy/scoped mounts/credential binding

Evidence: [shell](#evidence-shell), [output](#evidence-output), [sandbox](#evidence-sandbox), [detached](#evidence-detached).

### Hook before_prompt / before_capability observers

Surface: host extension API.

Input: installed native/WASM hook, context

Result: additive prompt patches, gate decision and event metadata

Lifecycle: bounded extension calls around compiled loop; not whole-turn replace

Authority: trust-tier/host-granted hook sink, model self-authorship caller not found

Evidence: [mutator](#evidence-mutator), [prompt](#evidence-prompt), [wiring](#evidence-wiring), [wasm](#evidence-wasm), [self-hook](#evidence-self-hook).

### Compaction task / projection summary

Surface: host lifecycle.

Input: safe completed message range and trigger

Result: summary artifact identity, ratio/redaction metrics

Lifecycle: separate range artifact selected into projection, bounded ineffective automatic attempts

Authority: host service scoped transcript access

Evidence: [compact](#evidence-compact), [summary](#evidence-summary), [effectiveness](#evidence-effectiveness).

### Provider reload

Surface: operator API.

Input: operator config/provider/key selection

Result: candidate chain swap or typed error

Lifecycle: per-call provider snapshot; newly-added cheap slot restart-required

Authority: operator config/key-store service

Evidence: [adapter](#evidence-adapter), [reload](#evidence-reload), [per-call](#evidence-per-call).

### Process cancel / supervisor shutdown

Surface: host control API.

Input: scoped process identity or shutdown

Result: cancel wake or awaited supervisor closure

Lifecycle: cancel request cooperative; Drop request differs from awaited shutdown; external effects separate

Authority: host scoped process registry/journal

Evidence: [cancel](#evidence-cancel), [supervisor](#evidence-supervisor), [shutdown](#evidence-shutdown).

### spawn_subagent (disabled by default)

Surface: currently disabled model capability.

Input: task/flavor/background mode

Result: child run/thread and terminal/result metadata

Lifecycle: implemented dependent-run machinery; default policy removes model surface

Authority: explicit host configuration override needed

Evidence: [deny](#evidence-deny), [default-deny](#evidence-default-deny), [child-status](#evidence-child-status), [child-test](#evidence-child-test).

## Capabilities

### filesystem

**S — Files** (source): Compiled coding manifests and scoped saved-output publication; individual file operations not exhaustively traced.

Evidence: [registration](#evidence-registration), [output](#evidence-output).

### processes

**I — OS programs** (source): Native shell with per-user sandbox task/container ownership; normal output publish and resource ceiling.

Evidence: [shell](#evidence-shell), [sandbox](#evidence-sandbox), [output](#evidence-output).

### code-actions

**S — Code actions** (source): Shell executes ordinary programs and WASM extension hook bodies; no model persistent interpreter kernel established.

Evidence: [shell](#evidence-shell), [wasm](#evidence-wasm).

### persistent-kernel

**L — Kernel** (source): Persistent filesystem workspace, but shell process state explicitly does not persist; no shared Python namespace path established.

Evidence: [workspace-state](#evidence-workspace-state), [shell](#evidence-shell).

### standing-database

**? — Standing DB** (inspection scope): Not established beyond inspected current Reborn loop, process, hook, shell, compaction and model reload paths.

### workflow-programming

**L — Workflows** (source): Native sealed stage/family composition plus bounded additive/gate hooks; arbitrary model/operator turn program replacement unavailable in inspected API.

Evidence: [planner](#evidence-planner), [pipeline](#evidence-pipeline), [mutator](#evidence-mutator), [wiring](#evidence-wiring).

### multi-model

**I — Models** (source): Primary/cheap provider chains configurable and hot swapped; simultaneous child override policy not established.

Evidence: [reload](#evidence-reload), [adapter](#evidence-adapter).

### live-collaboration

**L — Peer chat** (source): Child lifecycle/result vocabulary exists, model spawning default-denied; no unrestricted live peer room proved.

Evidence: [deny](#evidence-deny), [default-deny](#evidence-default-deny), [child-status](#evidence-child-status).

### concurrent-work

**I — Concurrency** (source): Leased supervisor task pool and independently owned sandbox execution tasks; model children default disabled.

Evidence: [supervisor](#evidence-supervisor), [detached](#evidence-detached), [deny](#evidence-deny).

### steering-interrupt

**L — Steer/interrupt** (source): Cooperative token + wake and leased execution fencing; outer command await can detach owned execution, not quiescence.

Evidence: [cancel](#evidence-cancel), [supervisor](#evidence-supervisor), [detached](#evidence-detached).

### turn-redefinition

**L — Turn program** (source): Default planner slots sealed, hooks additive/policy/observer; full executable turn cannot be hot replaced through traced surface.

Evidence: [planner](#evidence-planner), [mutator](#evidence-mutator), [pipeline](#evidence-pipeline).

### compaction

**I — Compaction** (source): Validated range summary artifacts and rebuilt-prompt effectiveness circuit; retain projection separation rather than mutate core stage.

Evidence: [compact](#evidence-compact), [summary](#evidence-summary), [effectiveness](#evidence-effectiveness), [compact-test](#evidence-compact-test).

### context-repair

**S — Repair** (source): Range-addressed separate summary artifacts support replacing selection while retaining reference to source span; arbitrary model repair operation/source retention storage not fully traced.

Evidence: [summary](#evidence-summary).

### original-audit

**L — Original audit** (source): Typed sanitized runtime events and saved command outputs are useful, but event schema deliberately excludes raw provider/program payload.

Evidence: [event](#evidence-event), [output](#evidence-output).

### audit-query

**S — Audit query** (source): Durable scoped event/result references are query primitives; model full-original audit query API not established.

Evidence: [event](#evidence-event), [child-status](#evidence-child-status), [output](#evidence-output).

### hot-change

**I — Hot change** (source): Actual candidate provider-chain replacement; each in-flight request retains its wrapper snapshot, but primary and cheap wrappers swap separately rather than as one atomic generation. An initially absent cheap wrapper cannot newly activate without restart.

Evidence: [reload](#evidence-reload), [per-call](#evidence-per-call), [adapter](#evidence-adapter).

### rebuild-continuity

**L — Rebuild continuity** (source): Durable process/turn vocabulary supports recovery, but no compiled refit/outpost transfer and external-effect settlement barrier established.

Evidence: [supervisor](#evidence-supervisor), [detached](#evidence-detached), [child-status](#evidence-child-status).

### remote-services

**S — Remote** (source): Provider/host-scoped filesystem/process ports and credentialed sandbox consumers; harness still owns per-user container lifecycle.

Evidence: [shell](#evidence-shell), [sandbox](#evidence-sandbox), [adapter](#evidence-adapter).

### self-improvement

**L — Self-improve** (source): Self-authored hook contract is restrictive/run-scoped and source caller only found in tests during inspection; cannot credit as model-governed autoresearch.

Evidence: [self-hook](#evidence-self-hook), [planner](#evidence-planner).

### complaints

**? — Complaints** (inspection scope): Not established beyond inspected current Reborn loop, process, hook, shell, compaction and model reload paths.

### authority

**I — Authority** (source): Shell Ask default, hard120s/resource bounds, scoped credentials and disabled spawn; authority vocabulary a substantial mismatch with unrestricted operator brief.

Evidence: [shell](#evidence-shell), [sandbox](#evidence-sandbox), [deny](#evidence-deny).

### evaluation

**S — Evaluation** (source): Direct rebuilt-prompt compaction oracle; child scripted e2e is ignored by default and must be explicitly enabled, not normal capability evidence.

Evidence: [compact-test](#evidence-compact-test), [child-test](#evidence-child-test).

### time-order

**S — Time/order** (source): Host runtime stamps UTC loop start, Instant-backed latency and task heartbeat/deadline vocabulary; not logical paused-time/refit contract.

Evidence: [wiring](#evidence-wiring), [supervisor](#evidence-supervisor), [shell](#evidence-shell).

## Inspected test oracles

- [quarantine/ironclaw/crates/loop/ironclaw_agent_loop/src/executor/tests/compaction.rs](../../../quarantine/ironclaw/crates/loop/ironclaw_agent_loop/src/executor/tests/compaction.rs): rebuilt summary effectiveness Oracle: Exactly third ineffective compact opens circuit and fourth automatic attempt suppressed; mocked prompts, not summary fidelity; not run. Read, **not executed**.
- [quarantine/ironclaw/tests/reborn_subagent_spawn_e2e.rs](../../../quarantine/ironclaw/tests/reborn_subagent_spawn_e2e.rs): dependent parent/child lifecycle Oracle: Scripted provider park/resume with child result; explicitly ignored while spawn default disabled; not run. Read, **not executed**.

## Useful mechanisms

- Typed durable identity/status and honest request-vs-terminal distinctions.
- Separate summary artifacts and actual rebuilt-prompt compaction usefulness test.
- Production hook materialization and bundle-authority reissue wired rather than just interface declarations.

## Material limits

- Model subagent capability denied by current default composition; README/test presence cannot imply availability.
- Loop strategy slots sealed and hook mutations additive.
- Sandbox owned task survives cancellation of outer await; complete settlement needs additional state.
- Runtime events sanitized metadata, not raw original audit.
- Provider snapshots are atomic per wrapper; primary/cheap replacements are separate, with no shared-generation activation barrier.

## Arconaut design questions

- Adopt lifecycle distinctions conceptually while allowing model ordinary program changes.
- Test hot activation across whole turn/workflow boundary, not only provider object swap.
- Separate durable internal fencing from confirmed foreign program/provider settlement.

## Evidence

### Evidence workspace

[quarantine/ironclaw/Cargo.toml:62–76](../../../quarantine/ironclaw/Cargo.toml#L62): Legacy monolith deleted; current root package only integration tests.

### Evidence planner

[quarantine/ironclaw/crates/loop/ironclaw_agent_loop/src/default_planner.rs:1–69](../../../quarantine/ironclaw/crates/loop/ironclaw_agent_loop/src/default_planner.rs#L1): Default strategy composition sealed/crate-private, not operator/model mutable slots.

### Evidence pipeline

[quarantine/ironclaw/crates/loop/ironclaw_agent_loop/src/executor/pipeline.rs:11–55](../../../quarantine/ironclaw/crates/loop/ironclaw_agent_loop/src/executor/pipeline.rs#L11): Actual compiled stage objects and order vocabulary.

### Evidence mutator

[quarantine/ironclaw/crates/loop/ironclaw_hooks/src/kinds/mutator.rs:1–55](../../../quarantine/ironclaw/crates/loop/ironclaw_hooks/src/kinds/mutator.rs#L1): Prompt patches additive snippets/metadata only; cannot replace messages or identity.

### Evidence prompt

[quarantine/ironclaw/crates/loop/ironclaw_hooks/src/middleware/prompt_port.rs:143–155](../../../quarantine/ironclaw/crates/loop/ironclaw_hooks/src/middleware/prompt_port.rs#L143): Actual before-prompt dispatch called by middleware.

### Evidence wiring

[quarantine/ironclaw/crates/loop/ironclaw_turn_runner/src/loop_driver_host.rs:1886–1922](../../../quarantine/ironclaw/crates/loop/ironclaw_turn_runner/src/loop_driver_host.rs#L1886): Production wraps prompt port with instruction materialization and refreshed bundle authority.

### Evidence self-hook

[quarantine/ironclaw/crates/loop/ironclaw_hooks/src/self_authored.rs:1–26](../../../quarantine/ironclaw/crates/loop/ironclaw_hooks/src/self_authored.rs#L1): Self-authorship contract only restriction and current run; durable surface future, no exposed model caller established.

### Evidence wasm

[quarantine/ironclaw/crates/loop/ironclaw_hooks/src/wasm/runtime.rs:28–70](../../../quarantine/ironclaw/crates/loop/ironclaw_hooks/src/wasm/runtime.rs#L28): Installed WASM hook resolution and bounded sink vocabulary, not arbitrary hot core replacement.

### Evidence cancel

[quarantine/ironclaw/crates/kernel/ironclaw_processes/src/cancellation.rs:37–78](../../../quarantine/ironclaw/crates/kernel/ironclaw_processes/src/cancellation.rs#L37): Cooperative cancellation atomic flag and wake token, not resource termination.

### Evidence supervisor

[quarantine/ironclaw/crates/kernel/ironclaw_processes/src/supervisor.rs:587–647](../../../quarantine/ironclaw/crates/kernel/ironclaw_processes/src/supervisor.rs#L587): Leased task heartbeat fencing stops local execution on lease loss; does not confirm every external effect settled.

### Evidence shutdown

[quarantine/ironclaw/crates/kernel/ironclaw_processes/src/supervisor.rs:343–365](../../../quarantine/ironclaw/crates/kernel/ironclaw_processes/src/supervisor.rs#L343): Explicit shutdown awaits supervisor; Drop only requests it.

### Evidence shell

[quarantine/ironclaw/crates/kernel/ironclaw_host_runtime/src/first_party_tools/shell.rs:27–100](../../../quarantine/ironclaw/crates/kernel/ironclaw_host_runtime/src/first_party_tools/shell.rs#L27): Shell actual native dispatch, Ask default and hard120s maximum.

### Evidence output

[quarantine/ironclaw/crates/kernel/ironclaw_host_runtime/src/first_party_tools/shell.rs:128–184](../../../quarantine/ironclaw/crates/kernel/ironclaw_host_runtime/src/first_party_tools/shell.rs#L128): Saved output published through scoped filesystem before rendered JSON result.

### Evidence registration

[quarantine/ironclaw/crates/kernel/ironclaw_host_runtime/src/first_party_tools/mod.rs:242–263](../../../quarantine/ironclaw/crates/kernel/ironclaw_host_runtime/src/first_party_tools/mod.rs#L242): Actual first-party shell/file/skill/trigger manifests registered before policy filters.

### Evidence workspace-state

[quarantine/ironclaw/crates/kernel/ironclaw_host_runtime/src/first_party_tools/mod.rs:306–321](../../../quarantine/ironclaw/crates/kernel/ironclaw_host_runtime/src/first_party_tools/mod.rs#L306): Persistent per-user workspace guidance explicitly says shell process state does not persist between calls.

### Evidence sandbox

[quarantine/ironclaw/crates/lanes/ironclaw_sandbox/src/sandbox_process.rs:767–825](../../../quarantine/ironclaw/crates/lanes/ironclaw_sandbox/src/sandbox_process.rs#L767): Per-user lifecycle gate, credential guard and container ensure performed by harness.

### Evidence detached

[quarantine/ironclaw/crates/lanes/ironclaw_sandbox/src/sandbox_process.rs:888–919](../../../quarantine/ironclaw/crates/lanes/ironclaw_sandbox/src/sandbox_process.rs#L888): Command wrapped in spawned owned task; dropping outer await does not itself abort task.

### Evidence compact

[quarantine/ironclaw/crates/loop/ironclaw_loop_host/src/compaction_task.rs:266–290](../../../quarantine/ironclaw/crates/loop/ironclaw_loop_host/src/compaction_task.rs#L266): Validate range, build input, model summary, sanitize and persist artifact.

### Evidence summary

[quarantine/ironclaw/crates/loop/ironclaw_loop_host/src/compaction_task.rs:578–613](../../../quarantine/ironclaw/crates/loop/ironclaw_loop_host/src/compaction_task.rs#L578): Separate range-addressed summary artifact with replace-when-selected cumulative-barrier policy.

### Evidence effectiveness

[quarantine/ironclaw/crates/loop/ironclaw_agent_loop/src/state/compaction.rs:24–79](../../../quarantine/ironclaw/crates/loop/ironclaw_agent_loop/src/state/compaction.rs#L24): Effectiveness judged after rebuilt prompt including summary; third ineffective automatic compact trips run circuit.

### Evidence event

[quarantine/ironclaw/crates/events/ironclaw_event_log/src/runtime_event.rs:72–135](../../../quarantine/ironclaw/crates/events/ironclaw_event_log/src/runtime_event.rs#L72): Runtime event schema carries redacted safe metadata/byte counts and bounded closed labels, not original IO.

### Evidence reload

[quarantine/ironclaw/crates/domains/ironclaw_llm/src/runtime.rs:315–346](../../../quarantine/ironclaw/crates/domains/ironclaw_llm/src/runtime.rs#L315): Reload builds candidate then swaps primary/cheap wrappers; adding absent cheap wrapper needs restart.

### Evidence per-call

[quarantine/ironclaw/crates/domains/ironclaw_llm/src/runtime.rs:196–222](../../../quarantine/ironclaw/crates/domains/ironclaw_llm/src/runtime.rs#L196): Each model call snapshots current provider wrapper; in-flight call retains prior object.

### Evidence adapter

[quarantine/ironclaw/crates/product/ironclaw_operator/src/llm_admin/llm_reload.rs:86–122](../../../quarantine/ironclaw/crates/product/ironclaw_operator/src/llm_admin/llm_reload.rs#L86): Operator live reload resolves file/provider stored key then invokes actual handle.

### Evidence deny

[quarantine/ironclaw/crates/loop/ironclaw_turn_runner/src/runtime.rs:850–863](../../../quarantine/ironclaw/crates/loop/ironclaw_turn_runner/src/runtime.rs#L850): Composition global deny removes spawn_subagent model surface; explicit config override can re-enable.

### Evidence default-deny

[quarantine/ironclaw/crates/loop/ironclaw_turn_runner/src/runtime.rs:294–299](../../../quarantine/ironclaw/crates/loop/ironclaw_turn_runner/src/runtime.rs#L294): Default disabled IDs includes spawn_subagent.

### Evidence child-status

[quarantine/ironclaw/crates/loop/ironclaw_turn_runner/src/subagent/spawn_result.rs:8–57](../../../quarantine/ironclaw/crates/loop/ironclaw_turn_runner/src/subagent/spawn_result.rs#L8): Child run/thread identity and CancelRequested distinct from Cancelled in event vocabulary.

### Evidence child-test

[quarantine/ironclaw/tests/reborn_subagent_spawn_e2e.rs:23–85](../../../quarantine/ironclaw/tests/reborn_subagent_spawn_e2e.rs#L23): Ignored test proves parked parent/child result in scripted model when explicitly enabled, not default availability.

### Evidence compact-test

[quarantine/ironclaw/crates/loop/ironclaw_agent_loop/src/executor/tests/compaction.rs:77–124](../../../quarantine/ironclaw/crates/loop/ironclaw_agent_loop/src/executor/tests/compaction.rs#L77): Direct prompt-stage oracle counts ineffective rebuilt prompts and fourth automatic compact suppression.

