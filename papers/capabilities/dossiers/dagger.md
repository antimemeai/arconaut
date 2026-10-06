# dagger

Typed immutable conversations with actual continuation tools and addressable live agents; execution cache/archives are separate from durable external-effect settlement.

Role: container workflow engine and programmable agent runtime. Runtime: Go.

Pinned source: [https://github.com/dagger/dagger](https://github.com/dagger/dagger); revision/version `4129d95c0c43198d11e1b42791d3adbbdb505799`.

Engine evaluates typed content-addressed DAG objects; session agent registry runs detached Go loops and container/module/MCP actions.

Owns session agent/control registry, build execution, services and telemetry; consumes model providers and host/remote resources. Its service management must not become Arconaut governance of shared computation.

Inspection: LLM step/provider/tool schedule, agent mailbox/lifecycle/reseed, typed container/service actions, recording comparison, trace inspection, archive restore and direct tests

Limits of this study: Source-only; no execution. No exhaustive cache/BuildKit conformance or cloud archive review; API is explicitly experimental.

## Actions

### LLM.step / loop

Surface: operator/program API.

Input: immutable conversation, maxTokens/maxSteps and bound module/MCP tools

Result: next LLM object ID, protocol results/errors, usage/trace

Lifecycle: step provider+tools; loop until no pending input; completed prefix returned on cancel

Authority: caller capabilities and allowed module/provider policy

Evidence: [e2](#evidence-e2), [e3](#evidence-e3), [e20](#evidence-e20), [e26](#evidence-e26).

### module tool returning LLM (continuation)

Surface: model/program tool.

Input: injected current conversation plus authored transform program

Result: new model/tools/workspace/history continued by same agent

Lifecycle: runs after ordinary mutations, one continuation per turn; validates tools and missing state

Authority: bound callable module has substantial control of next conversation

Evidence: [e3](#evidence-e3), [e4](#evidence-e4), [e5](#evidence-e5).

### LLM.spawn / agent(handle)

Surface: program API.

Input: conversation seed/name; optional restoration handle/lifecycle/parent

Result: unique agent runtime handle and immutable object recipe

Lifecycle: inert until send/resume; live entry per session; restoration preserves identity

Authority: handle capability; restoration refuses existing collision and live-state fiction

Evidence: [e1](#evidence-e1), [e21](#evidence-e21), [e26](#evidence-e26).

### Agent.send / AgentMessage.ref,delivery,response

Surface: model/operator/program API.

Input: target handle, text/media, optional replyTo mailbox ref

Result: permanent #N message record; actual delivery evidence and correlated reply

Lifecycle: nonblocking enqueue; response waits; failed pending records can retry after resume

Authority: sender derived from caller context; wait graph refuses self/cycles

Evidence: [e6](#evidence-e6), [e7](#evidence-e7), [e10](#evidence-e10), [e11](#evidence-e11), [e12](#evidence-e12).

### Agent.notify / wait

Surface: program/model supervisor.

Input: target/subscriber handle and state list or desired/settled state

Result: attributed lifecycle messages or state result

Lifecycle: subscription active across transitions; waits cancelable, settled includes failure/stop

Authority: known agent handles and cycle guard

Evidence: [e10](#evidence-e10), [e12](#evidence-e12).

### Agent.pause(interrupt) / resume / stop

Surface: operator/program/model lifecycle.

Input: handle and cooperative/preemptive/kill option

Result: paused/stopped snapshot or relaunched same entry

Lifecycle: interrupt cancels current step; consumed input remains pending, queued mail discarded; stop ends loop

Authority: capability-bound handle access; does not undo external effects

Evidence: [e7](#evidence-e7), [e8](#evidence-e8), [e12](#evidence-e12).

### Agent.reseed

Surface: operator/program API.

Input: same runtime handle and replacement LLM conversation

Result: same instance with changed committed snapshot; old consumed prompt receives rewind error

Lifecycle: refuses active step/drain/stopped instance; next queued turn uses replacement

Authority: caller with handle and new LLM can replace history/model/tools

Evidence: [e9](#evidence-e9), [e12](#evidence-e12).

### Container.withExec / stdout / directory

Surface: program/model bound object tools.

Input: container object, argv/stdin/options/mounts

Result: new lazy container, joint filesystem/exec metadata, stdout

Lifecycle: evaluation realizes/caches outputs; not durable external transaction

Authority: container/module exposure and explicit capabilities

Evidence: [e13](#evidence-e13), [e14](#evidence-e14).

### Service.start / up / stop

Surface: program/model bound object tools.

Input: service recipe, port mappings/kill flag

Result: health-checked runtime/tunnel endpoint or exit

Lifecycle: uncached imperative service state; owned session lifetime

Authority: service handle/runtime authority; keep shared fabric consumption distinct

Evidence: [e15](#evidence-e15), [e16](#evidence-e16).

### LoadTrace / ReadLogs / ReadTrace / FindSpans / InspectCall

Surface: model native introspection.

Input: trace ID/span/query/digest/view/limits

Result: imported root IDs, bounded logs/reports/timings/call recipe

Lifecycle: LoadTrace inspects only; paging/filtering needed for omitted data

Authority: session trace access; cloud import uses connecting client auth

Evidence: [e18](#evidence-e18), [e19](#evidence-e19).

### archive restore graph

Surface: operator CLI implementation.

Input: sealed trace/control roster, recipe anchors, optional focus

Result: inert original-handle agent graph, subscriptions and selected focus

Lifecycle: verify/import then rehydrate; skip named broken agents; no old active process preservation

Authority: operator and archived capability/recipe data

Evidence: [e21](#evidence-e21), [e22](#evidence-e22), [e23](#evidence-e23).

### recording/ model SendQuery

Surface: test/program provider.

Input: exported messages and incoming rendered history

Result: next recorded assistant response or divergence error

Lifecycle: replay drives real loop/tool schedule; no live model invoked

Authority: test-authored recording

Evidence: [e24](#evidence-e24).

## Capabilities

### filesystem

**I — Files** (source): Typed directory/container/module tools compose actual code execution and joint cached outputs; arbitrary external side effects are outside DAG value identity.

Evidence: [e13](#evidence-e13), [e14](#evidence-e14).

### processes

**I — OS programs** (source): Typed directory/container/module tools compose actual code execution and joint cached outputs; arbitrary external side effects are outside DAG value identity.

Evidence: [e13](#evidence-e13), [e14](#evidence-e14).

### code-actions

**I — Code actions** (source): Typed directory/container/module tools compose actual code execution and joint cached outputs; arbitrary external side effects are outside DAG value identity.

Evidence: [e13](#evidence-e13), [e14](#evidence-e14).

### persistent-kernel

**S — Kernel** (source): Long-lived services/container state can host a kernel; no standing shared interpreter heap is supplied by immutable LLM or lazy exec itself.

Evidence: [e13](#evidence-e13), [e15](#evidence-e15), [e16](#evidence-e16).

### standing-database

**S — Standing DB** (source): Trace/control/archive state is queryable infrastructure; standing application DB would be an independently consumed service, not the LLM conversation object.

Evidence: [e15](#evidence-e15), [e19](#evidence-e19), [e22](#evidence-e22).

### workflow-programming

**I — Workflows** (source): Typed DAG module programs, explicit step/loop and ordered continuation tools compose execution; cache replay does not atomically settle remote side effects.

Evidence: [e2](#evidence-e2), [e3](#evidence-e3), [e4](#evidence-e4), [e13](#evidence-e13), [e26](#evidence-e26).

### multi-model

**I — Models** (source): Each immutable LLM composition carries provider/model and can be swapped by returned-LLM continuation or same-instance reseed; different seeded agents can run concurrently.

Evidence: [e1](#evidence-e1), [e3](#evidence-e3), [e5](#evidence-e5), [e9](#evidence-e9).

### live-collaboration

**I — Peer chat** (source): Addressable agent handles, attributed FIFO mailbox/reply refs, lifecycle subscriptions and caller-injected Agent primitives support concurrent peer/parent collaboration.

Evidence: [e6](#evidence-e6), [e7](#evidence-e7), [e11](#evidence-e11), [e12](#evidence-e12).

### concurrent-work

**I — Concurrency** (source): Detached agents and concurrent readonly batches; writes in a batch sequential. Shared-state mutation order is deliberate.

Evidence: [e1](#evidence-e1), [e4](#evidence-e4), [e7](#evidence-e7).

### steering-interrupt

**I — Steer/interrupt** (source): Mail arriving during open turn joins at step boundary. Pause is cooperative; interrupt cancels a step and parks with committed history, discarding unconsumed queue.

Evidence: [e6](#evidence-e6), [e7](#evidence-e7), [e8](#evidence-e8).

### turn-redefinition

**I — Turn program** (source): A tool can return an entirely new LLM with tools/model/workspace/history; next step adopts it, validates new tools and records transform, with one continuation per turn.

Evidence: [e3](#evidence-e3), [e5](#evidence-e5).

### compaction

**S — Compaction** (source): Self-compaction can return a replacement LLM; engine supports conversation transformation but inspected paths do not establish automatic faithful summarization policy.

Evidence: [e3](#evidence-e3), [e5](#evidence-e5), [e9](#evidence-e9).

### context-repair

**I — Repair** (source): Same-instance reseed/continuation and original trace recipes support explicit replacement/rewind; no guarantee arbitrary missing external content can be reconstructed.

Evidence: [e9](#evidence-e9), [e21](#evidence-e21), [e25](#evidence-e25).

### original-audit

**L — Original audit** (source): Recipe/telemetry/archive retain many original events and rewind markers, but raw oversized tool return values explicitly have no retained original escape hatch; not every OS/provider byte.

Evidence: [e17](#evidence-e17), [e22](#evidence-e22), [e25](#evidence-e25).

### audit-query

**I — Audit query** (source): Model-native LoadTrace/ReadLogs/ReadTrace/FindSpans/InspectCall query session telemetry and call recipes with bounded projections and external auth for cloud imports.

Evidence: [e18](#evidence-e18), [e19](#evidence-e19).

### hot-change

**I — Hot change** (source): Continuation tools can carry edited/rebound workspace/tool env into same loop; reseed refuses active execution/drain. This differs from default whole-turn activation.

Evidence: [e2](#evidence-e2), [e3](#evidence-e3), [e5](#evidence-e5), [e9](#evidence-e9).

### rebuild-continuity

**L — Rebuild continuity** (source): Archive restore rehydrates inert agents under original handles and dependency closures; old RUNNING state cannot survive. No compiled harness refit with full effect-quiescence established.

Evidence: [e21](#evidence-e21), [e22](#evidence-e22), [e23](#evidence-e23), [e26](#evidence-e26).

### remote-services

**I — Remote** (source): Model provider requests, container/MCP services and host tunnels are wired; service runtime owns managed containers, consuming Arconaut must respect external fabric boundary.

Evidence: [e2](#evidence-e2), [e15](#evidence-e15), [e16](#evidence-e16), [e26](#evidence-e26).

### self-improvement

**S — Self-improve** (source): Module execution/edit→continuation/reload supports in-situ program change; no inspected autonomous hypothesis/evaluator/promotion protocol.

Evidence: [e3](#evidence-e3), [e5](#evidence-e5), [e13](#evidence-e13).

### complaints

**S — Complaints** (source): Agent failure/rewind telemetry and trace query expose diagnostics; no full-state complaint/bead workflow traced.

Evidence: [e19](#evidence-e19), [e25](#evidence-e25).

### authority

**I — Authority** (source): Module/tool capability binding, caller provenance and allowed-model modules; new unknown Git module model use can prompt. Handle possession addresses agent.

Evidence: [e11](#evidence-e11), [e12](#evidence-e12), [e20](#evidence-e20).

### evaluation

**I — Evaluation** (source): Inspected direct tests assert reseed race rejection, exported rewind identity, recipe closure failure and restore ordering; recording provider detects rendered-history divergence, not model reasoning quality.

Evidence: [e9](#evidence-e9), [e21](#evidence-e21), [e23](#evidence-e23), [e24](#evidence-e24), [e25](#evidence-e25).

### time-order

**I — Time/order** (source): Mailbox refs/delivery commit, turn-resolution lock, ordered writes/parallel reads and sealed archive high-water cut distinguish causal/physical order.

Evidence: [e4](#evidence-e4), [e6](#evidence-e6), [e7](#evidence-e7), [e22](#evidence-e22).

## Inspected test oracles

- [quarantine/dagger/core/agent_reseed_test.go](../../../quarantine/dagger/core/agent_reseed_test.go): paused-drain race and suspended-turn abandonment Oracle: Exact old snapshot and unresolved message preserved on reject; after drain ends replacement adopted and consumed message gets rewind error. Read, **not executed**.
- [quarantine/dagger/core/agent_rewind_ctx_test.go](../../../quarantine/dagger/core/agent_rewind_ctx_test.go): rewind telemetry context Oracle: SpanRecorder asserts one exported rewind span and exact agent ID; proves event export under mock tracer, not archive retention. Read, **not executed**.
- [quarantine/dagger/engine/archive/verify_test.go](../../../quarantine/dagger/engine/archive/verify_test.go): missing/cyclic recipes and truncated bootstrap Oracle: Exact closure length and errors; truncated stream invokes zero partial callbacks. No recipes run in the oracle. Read, **not executed**.
- [quarantine/dagger/internal/cmd/dagger/restore_test.go](../../../quarantine/dagger/internal/cmd/dagger/restore_test.go): restore ordering and partial failures Oracle: Fake destination exact call sequence requires all rehydration before address/adopt; named unrestorable entries skipped. Does not prove runtime external effects survive. Read, **not executed**.
- [quarantine/dagger/core/llm_recording_test.go](../../../quarantine/dagger/core/llm_recording_test.go): tool-result telemetry Oracle: Exact authoritative patch payload/content type in log records; recorded-provider display tests preserve production nesting, not raw unbounded external return archive. Read, **not executed**.

## Useful mechanisms

- Actual tools can redefine the next conversation/program; identity-preserving reseed has race checks.
- Addressable peer agents with correlated replies and wait-cycle refusal closely match sophisticated collaboration needs.
- Trace-native query and inert archive restore make continuity inspectable.

## Material limits

- Go engine is a studied reference, not an architecture choice.
- Raw oversized tool result originals are not retained; cache/recipe closure does not settle external effects.
- Experimental session-agent restore is not compiled refit or ongoing provider/process migration.

## Arconaut design questions

- Can continuation-as-return-value make arbitrary turn programs model-ergonomic without monolithic policy?
- What changes from step-boundary Dagger activation to the operator-selected default turn/workflow conclusion?
- How should independent shared services be addressed without importing engine service governance?
- Can every tool/provider original survive separately from bounded context and rendered telemetry?

## Evidence

### Evidence e1

[quarantine/dagger/core/agent.go:27–64](../../../quarantine/dagger/core/agent.go#L27): Spawn identity is a fresh runtime handle; immutable seed/object is separate from mutable session registry.

### Evidence e2

[quarantine/dagger/core/llm.go:2311–2408](../../../quarantine/dagger/core/llm.go#L2311): Step derives exposed tools, renders history, sends provider request, materializes assistant response before calling tools and captures entering bound state.

### Evidence e3

[quarantine/dagger/core/llm.go:2420–2490](../../../quarantine/dagger/core/llm.go#L2420): Continuation adopts returned LLM; workspace/tool changes recorded as selectors. Cancellation still records pure result history detached from cancellation before returning cause.

### Evidence e4

[quarantine/dagger/core/mcp.go:1879–1909](../../../quarantine/dagger/core/mcp.go#L1879): Readonly tool runs can execute concurrently; mutations sequential in list order; continuation tools scheduled last after earlier effects.

### Evidence e5

[quarantine/dagger/core/mcp.go:965–1005](../../../quarantine/dagger/core/mcp.go#L965): Only one returned-LLM continuation per turn; rejects missing/null env and unreflected state changes. New model/history/workspace/tool state is actual continuation, not merely a prompt.

### Evidence e6

[quarantine/dagger/core/agent.go:1418–1488](../../../quarantine/dagger/core/agent.go#L1418): Mailbox assigns monotonic per-agent refs and provenance; stopped/failed/paused/running delivery states differ; steering is only finalized on actual drain.

### Evidence e7

[quarantine/dagger/core/agent.go:1950–2038](../../../quarantine/dagger/core/agent.go#L1950): Loop commits each successfully recorded prefix even on cancellation; late steering serialized with turn resolution, consumed messages correlate to final reply.

### Evidence e8

[quarantine/dagger/core/agent.go:2063–2123](../../../quarantine/dagger/core/agent.go#L2063): Pause retains suspended turn; interrupt cancels active step, keeps consumed input pending and discards queued unconsumed mail.

### Evidence e9

[quarantine/dagger/core/agent.go:2233–2276](../../../quarantine/dagger/core/agent.go#L2233): Reseed replaces same agent conversation only without active step/drain, settles abandoned suspended-turn messages with rewind error and retains lifecycle identity.

### Evidence e10

[quarantine/dagger/core/agent.go:372–423](../../../quarantine/dagger/core/agent.go#L372): Waits-for graph rejects self waits and cycles with actionable errors; non-agent callers are outside graph guard.

### Evidence e11

[quarantine/dagger/core/agent_context.go:10–56](../../../quarantine/dagger/core/agent_context.go#L10): Tool LLM/Agent arguments resolve caller context/function-call provenance, enabling child-parent messaging without forgeable sender argument.

### Evidence e12

[quarantine/dagger/core/schema/agent.go:62–144](../../../quarantine/dagger/core/schema/agent.go#L62): Public send/message/pause/resume/wait/notify/stop/reseed methods wire actual lifecycle and explicit reply refs.

### Evidence e13

[quarantine/dagger/core/container_exec.go:1283–1338](../../../quarantine/dagger/core/container_exec.go#L1283): withExec creates lazy joint FS/exec metadata/mount outputs, evaluated through engine cache; code action is container execution not permanent interpreter heap.

### Evidence e14

[quarantine/dagger/core/schema/container.go:1680–1743](../../../quarantine/dagger/core/schema/container.go#L1680): Public exec inputs check stdin conflict, clone container, dispatch owned WithExec; stdout evaluation materializes execution metadata.

### Evidence e15

[quarantine/dagger/core/service.go:510–576](../../../quarantine/dagger/core/service.go#L510): Service dispatch owns container/tunnel/reverse-tunnel startup and attaches cached container under client session; separate from external resource ownership.

### Evidence e16

[quarantine/dagger/core/schema/service.go:126–145](../../../quarantine/dagger/core/schema/service.go#L126): Public imperative start/up/stop are uncached; start health check, host tunnel and kill-vs-grace semantics are distinct.

### Evidence e17

[quarantine/dagger/core/mcp.go:1800–1845](../../../quarantine/dagger/core/mcp.go#L1800): Tool result safety cap drops middle/long lines; source explicitly says raw return value was never persisted elsewhere, unlike captured logs.

### Evidence e18

[quarantine/dagger/core/mcp.go:2729–2753](../../../quarantine/dagger/core/mcp.go#L2729): LoadTrace imports authenticated historical trace for inspection, without executing recipes or restoring agents; read-log IDs returned.

### Evidence e19

[quarantine/dagger/core/mcp.go:3217–3294](../../../quarantine/dagger/core/mcp.go#L3217): ReadTrace inspect/timings/report and FindSpans query session trace; tool reports are filtered/bounded projections.

### Evidence e20

[quarantine/dagger/core/llm.go:2968–3006](../../../quarantine/dagger/core/llm.go#L2968): Git modules need allowed-model capability or operator prompt; local/nonmodule requests allowed; this is configurable authority rather than command approval on every action.

### Evidence e21

[quarantine/dagger/internal/cmd/dagger/restore.go:83–170](../../../quarantine/dagger/internal/cmd/dagger/restore.go#L83): Restore orders parents first, resolves anchors and subscriptions, skips unrestorable entries explicitly, creates inert runtimes before adopting/prompting.

### Evidence e22

[quarantine/dagger/internal/cmd/dagger/restore_archive.go:21–89](../../../quarantine/dagger/internal/cmd/dagger/restore_archive.go#L21): Archive roster selected from sealed witnessed revisions and fixed timestamp/high-water cut; missing implementation evidence is not reconstructed by guesses.

### Evidence e23

[quarantine/dagger/engine/archive/verify.go:35–75](../../../quarantine/dagger/engine/archive/verify.go#L35): Recipe dependency closure checked without evaluating calls; missing/cyclic dependencies fail. Closure validation is not external-effect settlement.

### Evidence e24

[quarantine/dagger/core/llm_recording.go:149–180](../../../quarantine/dagger/core/llm_recording.go#L149): Recorded provider compares model-rendered history text/roles/media identity and reports divergence; it supplies next response rather than live-provider correctness oracle.

### Evidence e25

[quarantine/dagger/core/agent_telemetry.go:103–138](../../../quarantine/dagger/core/agent_telemetry.go#L103): Rewind marker preserves from/to recipe digests and original trace visibility; best-effort event, not removal of old telemetry.

### Evidence e26

[quarantine/dagger/core/schema/llm.go:181–235](../../../quarantine/dagger/core/schema/llm.go#L181): Public skills/MCP/step/loop/spawn surfaces; restoring explicit handles refuses RUNNING/WAITING_INPUT because old process died.

