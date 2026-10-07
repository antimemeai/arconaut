# Workflow ecosystem: semantics Blackbird must be able to express

2026-10-07. Bounded reconnaissance for the workflow campaign. This is discussion
and order-cutting input, not an adopted runtime or an implementation-completeness
claim. No reference implementation, installer, example, test, or provider was
executed. No dependency was adopted.

**Recommendation:** give Lua a small native execution vocabulary, then make DAGs,
statecharts, reactive flows, durable functions, scientific pipelines and agent
teams alternate programs over that vocabulary. Keep connector catalogs and
remote execution services outside the default installation. A callable adapter
to another workflow service is useful interoperability; it does not establish
that Blackbird implements that service's semantics.

“Superset” has three separate meanings worth retaining in every campaign order:

- **Expressibility:** a program can represent the behavior and its policies.
- **Operational semantics:** concurrency, failure, cancellation, ordering and
  recovery actually satisfy that behavior under specified faults.
- **Interoperability:** an adapter speaks a foreign service or file format.

Lua being a general-purpose language supplies the first only in a weak sense.
It cannot turn synchronous effects into durable concurrency by wishing. Nor
does a format converter establish operational compatibility. The intended
superset should be measured by representative behaviors, with unsupported
semantics reported explicitly rather than silently translated away.

## Evidence and inherited corpus

**S** means implementation source read locally, plus earlier pinned dossiers.
**D** means current primary documentation read; implementation/fault behavior
not independently established. **N** means normative standard identified,
without a complete semantics or conformance study. Nothing below means tests
have been run or that Blackbird already offers the capability.

The requested `orchestration-workflows.md` does not exist in the current study
tree. Its relevant existing material is:

- [Programmable workflows reconnaissance](../2026-09-30-programmable-workflows-frumentarii.md).
- [Programmable frameworks](../capabilities/studies/programmable-frameworks.md).
- [Collaboration and orchestration](../capabilities/studies/collaboration-orchestration.md).
- [Systems primitives](../capabilities/studies/systems-primitives.md).
- The [Windmill](../capabilities/dossiers/windmill.md),
  [Restate](../capabilities/dossiers/restate.md),
  [Dagger](../capabilities/dossiers/dagger.md),
  [Hermes](../capabilities/dossiers/hermes-agent.md) and
  [Forge](../capabilities/dossiers/forge-workflow.md) dossiers.

Fresh Hermes is a separate lane. Its older dossier remains a dated finding;
this report does not promote it to a fresh release study. Several inherited
Restate source paths have moved: the actual current quarantined state machine
is under `crates/worker/src/partition/state_machine/`, not the older dossier's
`crates/partition-processor/` path. Follow actual source, not a stale citation.

Direct local rereads used these concrete mechanisms:

- [Windmill flow variants](../../quarantine/windmill/backend/windmill-types/src/flows.rs#L1003):
  scripts/subflows, finite and while loops, branch-one/branch-all, bounded
  loop parallelism and embedded AI-agent configurations.
- [Windmill WAC checkpoint](../../quarantine/windmill/backend/windmill-common/src/wac.rs#L23):
  source hash, completed and pending steps, retained child job IDs and consumed
  resume-row identities. Its golden stable resume-ID test and normalized task
  failure tests are direct contract examples; read, not executed.
- [Restate journal append](../../quarantine/restate/crates/worker/src/partition/state_machine/entries/mod.rs#L365)
  stores indexed entries after state-machine processing; append time derives
  from the replicated record, rather than the applying machine's local clock.
- [Restate call/outbox](../../quarantine/restate/crates/worker/src/partition/state_machine/entries/call_commands.rs#L96)
  records invocation identity, correlation and idempotency information before
  publishing an internal call. This is a platform boundary, not an atomic
  transaction with an arbitrary external shell/API effect.

## Capability matrix

Each row names the distinctive semantic lesson and the integration boundary.
The list covers major families, not every product or all enterprise editions.

| Family/reference | Capability to study | Blackbird implication | Evidence and limit |
| --- | --- | --- | --- |
| Temporal | Replay of deterministic orchestration against retained activity/timer results; asynchronous signals, read-only queries, synchronous tracked updates | Distinguish enqueue acceptance, observation and completed interaction. Provider calls belong on the effect side of replay. | **D:** [tasks/replay](https://docs.temporal.io/tasks), [message passing](https://docs.temporal.io/encyclopedia/workflow-message-passing), [determinism](https://docs.temporal.io/workflow-definition). Reading these does not establish arbitrary activity exactly-once effects. |
| Restate | Journaled execution; keyed single-writer objects; invocation identity; delayed sends and durable RPC | A useful actor/durable-function paradigm and optional separately governed execution service. Keep its handle alongside the local audit. | **S+D:** local paths above; [concepts](https://docs.restate.dev/foundations/key-concepts). Current docs distinguish available concurrency caps from planned rate limits/priorities. |
| DBOS | Workflow/step checkpoints in a database, deterministic replay and explicitly idempotent steps; transactions have a different guarantee | Separate checkpoint completion from effect completion. Atomic database work and external API work need different retry contracts. | **D:** [architecture](https://docs.dbos.dev/architecture), [workflow guarantees](https://docs.dbos.dev/python/tutorials/workflow-tutorial). Steps are at least once until checkpointed; uncaught workflow exceptions terminate rather than magically recover. |
| Airflow | Dependency scheduling, conditional trigger rules, logical data intervals and historical backfill | Represent logical workload time separately from actual run time. “Run missed periods” is a policy, not an accidental cron flood. | **D:** [Dag model](https://airflow.apache.org/docs/apache-airflow/stable/core-concepts/dags.html). No scheduler source audit or provider integration exercised. |
| Dagster | Named materialized assets with upstream dependencies and code versions; multi-asset and graph-backed assets | A data-product view alongside a task view. Run success, output validity and artifact freshness are distinct. | **D:** [asset definitions](https://dagster.io/docs/guides/build/assets/defining-assets). Partition/backfill mechanics remain a targeted follow-up, not source-verified here. |
| Prefect | Ordinary authored flow/task programs; task states, retry/cache policies and future-returning submission | Dynamic program composition should remain first class alongside declarative DAGs. A reusable cached result is not the same thing as an active future. | **D:** [flows](https://docs.prefect.io/v3/concepts/flows), [tasks](https://docs.prefect.io/v3/concepts/tasks). Task “transactional” terminology is not evidence that arbitrary external effects are rolled back. |
| Windmill | Code and OpenFlow representations; branch/loop/subflow structure; queue job handles, checkpoint/resume and embedded agents | Shared native and model-facing callables, useful stable job identities, explicit failed-step data. Consume remote service instead of governing its fleet. | **S+D:** local paths above; [flow editor and capabilities](https://www.windmill.dev/docs/flows/flow_editor). OSS/enterprise bounds in the old dossier still require care; UI catalog does not prove public implementation. |
| n8n | Item-oriented conditional routing, merging, loops, waits, sub-workflows and error workflows | Typed item identity/lineage and join behavior deserve explicit semantics; a generic list is insufficient for correlated data flows. | **D:** [flow logic](https://docs.n8n.io/build/flow-logic.md). The wait overview was readable; detailed offload/resume implementation was not inspected. |
| Activepieces | A trigger plus executable actions, with schedules, webhooks and service event triggers | Connector packages should advertise trigger/action schemas and cursor behavior. Catalog size belongs to plugins, not core bloat. | **D:** [building flows](https://www.activepieces.com/docs/flows/building-flows). This page does not establish replay, exactly-once delivery or connector coverage. |
| Argo Workflows | Kubernetes tasks composed as DAGs or nested steps; configurable fail-fast scheduling | DAG failure policy must say whether to stop admission, cancel siblings or drain them. Remote placement is separate from dependency evaluation. | **D:** [DAG semantics](https://argo-workflows.readthedocs.io/en/latest/walk-through/dag/). Kubernetes service remains independently governed; no cluster/controller testing here. |
| GitHub Actions | Event-driven jobs, `needs`, expressions, matrix expansion, reusable workflows and concurrency settings | Build/CI profiles need matrix generation and runner/environment identity. A dispatched run should be addressable and inspectable like any other remote task. | **D:** [workflow syntax](https://docs.github.com/en/actions/reference/workflows-and-actions/workflow-syntax). No claim of full Actions compatibility or runner implementation. |
| GNU Make | Target/prerequisite rules and recipes; goal-directed work selection | Support demand-driven execution and work avoidance, not just start-at-root DAG traversal. File freshness is one possible cache policy. | **D:** [rule introduction](https://www.gnu.org/software/make/manual/html_node/Rule-Introduction.html). Timestamp rules are not content-addressed reproducibility. |
| Nix | Derivation of outputs from a builder, inputs, system and environment specification | Capture environment identity when claiming reusable results. A pure recipe and a general effectful workflow need different cache rules. | **D:** [derivations](https://nix.dev/manual/nix/2.32/language/derivations). No dependency adoption, sandbox evaluation or reproducibility qualification. |
| Dagger | Typed container/build objects combined with actual agent handles, mailboxes and continuation transformations | Agent steps should compose with ordinary tools and artifact handles; mailbox delivery and response deserve separate identities. | **S:** [pinned implementation study](../capabilities/dossiers/dagger.md). Source/API is experimental; cache and archive are not arbitrary effect settlement. |
| LangGraph | Graph and functional workflow APIs; routing, parallelization, dynamically assigned work, evaluator/optimizer patterns; checkpointed thread state | Graphs and direct Lua programs should use the same task/state machinery. Stateful branching should expose ancestry and retained originals. | **D:** [patterns](https://docs.langchain.com/oss/python/langgraph/workflows-agents), [persistence](https://docs.langchain.com/oss/python/langgraph/persistence), [time travel](https://docs.langchain.com/oss/python/langgraph/use-time-travel). In-memory checkpointers do not survive restarts; forking state is not undoing external actions. |
| CrewAI | Event-driven starts/listeners/routers; typed shared state; nested crews; optional persisted state and distinct resume/fork hydration | Team topology is a user program; model/provider choice is a task property. Persisting selected state is weaker than journaled effect recovery. | **D:** [Flows](https://docs.crewai.com/en/concepts/flows). Current docs' fork/resume and missing-state behaviors merit direct tests before copying their ergonomics. |
| Hermes/Prime-style skills | Executable reusable skills plus model-authored curation; child runs with inspection/steering; standing execution handles | Skills should be actual programs where appropriate, shared between humans and models. “Delegate,” “wait” and “stop” need honest live-state results. | **S, dated:** [Hermes dossier](../capabilities/dossiers/hermes-agent.md), [Prime study](../2026-09-30-programmable-workflows-frumentarii.md). Fresh Hermes is a separate report; Python kernels are reference mechanisms, not production language choices. |
| Node-RED | Message objects routed through a live graph; node/flow/global context scopes, configurable persistence | Reactive flows differ from finite DAG runs. Message identity, bounded buffers, backpressure and explicit context scope matter. | **D:** [messages](https://nodered.org/docs/user-guide/messages), [context](https://nodered.org/docs/user-guide/context). Context defaults to memory; persisted context alone is not a durable message-processing journal. |
| XState/statecharts | States/transitions, nested and parallel configurations; actor persistence/restoration | A statechart frontend should retain its active configuration and queued events, while making invoked-effect restart policy explicit. | **D:** [statecharts](https://stately.ai/docs/state-machines-and-statecharts), [persistence](https://stately.ai/docs/persistence). Restored machine actions are not re-executed, but invocations restart; this is not resurrection of an arbitrary live process heap. |
| BPMN/Camunda | Token-routing gateways, scoped compensation handlers, interrupting/noninterrupting event relationships | Workflow failure can require a new compensating action rather than history deletion. Scope and multi-instance completion change compensation behavior. | **N+D:** [OMG standard](https://www.omg.org/spec/BPMN/2.0.2/About-BPMN/), [gateways](https://docs.camunda.io/docs/components/modeler/bpmn/gateways/), [compensation](https://docs.camunda.io/docs/components/modeler/bpmn/compensation-events/). No full BPMN conformance or model import claim. |
| Nextflow | Scientific dataflow/task execution; resume through task metadata plus retained work outputs; environment/input-sensitive keys | A cache must check both recorded success and usable output artifacts. Concurrent arrival order must not silently alter joins or cache keys. | **D:** [cache/resume](https://www.nextflow.io/docs/latest/cache-and-resume.html). Metadata and work directories are both required; default file hashing is not universal content hashing. JVM implementation remains reference only. |
| Snakemake | Rules connecting named input/output files, wildcard instantiation, resources and data-dependent checkpoints | Demand-driven output planning and dynamically discovered dependencies belong in the vocabulary; scientific execution needs explicit artifact/resource contracts. | **D:** [rules](https://snakemake.readthedocs.io/en/stable/snakefiles/rules.html). Scheduling/executor behavior not source-audited; Python engine is not selected for Blackbird. |

## Twelve primitives that cannot be replaced by a bag of recipes

This is a design inference from the map, not a claim that the projects share one
formal model. These are semantic obligations; some can be implemented as Lua
modules over native operations rather than twelve new native subsystems.

| Primitive | Minimum contract | Distinct failure it addresses |
| --- | --- | --- |
| P1. Definition and identity | Stable run/task/attempt IDs; retained effective program and schema revision; inputs/results refer to their sources | An edited program or reused name accidentally masquerades as the original execution |
| P2. Effect boundary | Recorded admission, dispatch, completion and unknown outcome; per-effect retry/idempotency/reconciliation declaration | Replay repeats a side effect that succeeded before its result was recorded |
| P3. Supervised tasks | Spawn/observe/await; parent/child scope; cancellation request distinct from settlement; confirmed local process/provider quiescence | Timeout abandons a live child and refit begins while it still acts |
| P4. Durable state and continuation | Explicit serializable state/checkpoint boundaries; version-compatible recovery; recorded nondeterminism; separate replay and fork | Restored state is paired with different code, or a fork pretends external effects were undone |
| P5. Composition and dependency | Sequence, branch, loop, dynamic fan-out, join/reduce, subflow, explicit failure propagation | Join races, hidden task failures, or “fail fast” admits unintended work |
| P6. Messages and subscriptions | Addressed event identity/correlation; cursor/ack semantics; bounded backlog; documented ordering, duplicates and backpressure | Station work skips events, processes them twice, or consumes unbounded memory |
| P7. Time and activation | Deadline versus delay; durable timer identity; schedules/logical intervals; explicit missed-run/backfill policy | Wall-clock changes or downtime produce premature expiry or a catch-up storm |
| P8. Capacity and placement | Shared permit/resource names; bounded parallelism; provider budgets; task priority/fairness; remote executor handles | Nested fan-out multiplies cost, oversubscribes the laptop or seizes governance of shared services |
| P9. Artifacts and reuse | Typed output handles and lineage; existence/validity checks; explicit cache key and invalidation policy | A recorded success reuses deleted, stale or environmentally incompatible output |
| P10. Intervention and change | Model/operator steer, pause/resume and staged revisions; default current-turn/affected-workflow boundary; forced interrupt/apply after settlement | Hot edits mutate half a workflow, or a paused task is mistaken for a stopped service |
| P11. Extensible callable surface | Shared model/operator/program discovery and schemas; versioned modules/skills/connectors; explicit credentials/capabilities | Recipes or integrations only work from one interface, or secrets leak into reusable definitions |
| P12. Observation and experimentation | Queryable causal run/task/effect history; original/context distinction; costs, latency, outcomes; fake clocks/effects for direct tests | “It ran” is mistaken for correctness or measured improvement |

Task schemas, immutable source identities and references should be shared rather
than replicated into every representation. Do not clone transcripts into every
DAG node, checkpoint and agent. Retain originals once, publish bounded projections,
and measure byte traffic and scheduler work on actual workloads.

## Feasible order dependencies

These are proposed orders to discuss, not an implementation authorization beyond
the parent's campaign order. Each accepts one whole behavior with a direct oracle.
The initial slice should advance current self-development, not build a distributed
workflow platform before using it.

1. **Executable definitions and inspectable runs:** P1/P11 plus existing audit
   machinery. First useful recipe: literature/reference acquisition, bounded
   design task and explicit handoff result. Same callable from Lua, model and CLI.
   Direct check: effective source and output identities survive reload/reopen.
2. **Structured concurrent work:** P2/P3/P5/P8. Run independent build/reference/
   reviewer tasks, await settled results and aggregate in deterministic input
   order. Start with existing execution/provider capability, not every adapter.
   Direct check: constrained fan-out, failing sibling policy, cancellation and
   actual process death. A task graph display can follow these authoritative
   states rather than become a second control plane.
3. **Restartable workflow segments:** P4 atop stable identity/effect contracts.
   Pin definition and modules per run; choose explicit checkpoints or restricted
   replay instead of promising to serialize arbitrary Lua closures/stacks.
   Direct crash check: completed effects are not redispatched; ambiguous effects
   enter reconciliation; changed code is rejected or explicitly migrated.
4. **Station and reactive programs:** P6/P7 using task/state substrate. Start
   with local timer/file/event input and one plugin trigger. Specify consumed
   cursor and overload behavior before exposing endless feeds. Check restart
   between ingest, task admission and acknowledgment.
5. **Artifact/data/build workflows:** P9 over definitions/tasks. Build a genuine
   work-avoidance example and a dynamic dataset fan-out. Cache only effects whose
   policies permit reuse. Validate missing outputs, changed inputs and execution
   environment identity; preserve capability to force actual execution.
6. **Alternative frontends and network executors:** statechart/DAG/flow recipes,
   optional service connectors and remote placement over P1–P11. Add formats
   only with explicitly supported semantics. A Temporal/Argo client adapter is
   an integration order, not a local scheduler completion badge.

P10 and P12 are cross-cutting requirements in every slice. Existing program
activation, audit and context controls are leverage, but not a reason to skip
workflow-specific version and settlement tests. Shared kernels/DBs/executors stay
independently governed. Local refit pauses this Blackbird's clients and own work;
it does not commandeer those services.

## Concrete superset demonstrations

Use behaviors to cut orders, not a race to accumulate decorator names:

| Demonstration | Families it exercises | Real observable result |
| --- | --- | --- |
| Research → parallel reference inspection → design → one adversarial review → bounded fix → build | Agent teams, code flows, DAGs | Useful source-grounded design and working change; task outcomes and costs inspectable |
| Interrupt/rebuild/resume an experiment between an external effect and result recording | Durable execution, hot self-development | No blind replay of ambiguous action; explicit reconciliation and continued progress |
| Station consumes feed, batches correlated records, enriches with two models, publishes one artifact | Reactive automation, dataflows, heterogeneous agents | Event cursor, bounded backlog, provenance and selected failure policy survive restart |
| Matrix of build profiles generates a report and reuses only valid artifacts | CI, scientific workflow, demand-driven builds | Changed code/environment invalidates correct entries; unrelated valid outputs avoid work |
| Operator or model steers a waiting workflow, changes a module and applies it at the agreed boundary | Signals/statecharts, human/model collaboration | Recorded delivery and activation boundary; in-flight run does not silently switch definitions |

“Possibly more” can mean model-authored topology and context programs, measured
policy evolution, and live operator/model intervention across all these paradigms.
It need not mean pretending a code cell, workflow service, agent conversation and
OS process are interchangeable continuity units.

## Critical missing references and bounded follow-up

Acquire **targeted** implementations before coding their semantics. Existing
Windmill/Restate/Dagger are already enough for the first comparative design;
collecting every product is not a blocker on a useful first order.

| Before implementing | Primary reference to acquire/read | Exact question |
| --- | --- | --- |
| Lua task scheduling and replay | [Temporal SDK Core](https://github.com/temporalio/sdk-core), [LangGraph](https://github.com/langchain-ai/langgraph) | Activation history, task completion ordering, replay mismatch and checkpoint faults |
| Database-backed checkpoint recovery | [DBOS Python](https://github.com/dbos-inc/dbos-transact-py) | Commit/result ordering and recovery of an incomplete step; study, do not adopt Python |
| Demand-driven assets/dynamic mapping | [Dagster](https://github.com/dagster-io/dagster), [Prefect](https://github.com/PrefectHQ/prefect), [Airflow](https://github.com/apache/airflow) | Snapshot identity, expansion keys, omitted/missing dependencies and backfill policies |
| Reactive messages and statecharts | [Node-RED](https://github.com/node-red/node-red), [XState](https://github.com/statelyai/xstate) | Message cloning, buffering, event transition order and restored invoked-effect behavior |
| Trigger/plugin contracts | [n8n](https://github.com/n8n-io/n8n), [Activepieces](https://github.com/activepieces/activepieces) | Cursor versus acknowledgment, resume-token scope, and connector lifecycle/version policy |
| Workflow compensation/import | [OMG BPMN 2.0.2](https://www.omg.org/spec/BPMN/2.0.2/About-BPMN/), [Camunda](https://github.com/camunda/camunda) | Supported token/compensation scope; explicit unsupported import cases |
| Scientific executor/cache | [Nextflow](https://github.com/nextflow-io/nextflow), [Snakemake](https://github.com/snakemake/snakemake), [Nix](https://github.com/NixOS/nix) | Valid output reuse, dynamic dependency discovery and resource/environment identity |
| Kubernetes execution adapter | [Argo](https://github.com/argoproj/argo-workflows) | Submit/reconnect/cancel/settlement and external job identity; defer governance |

No manual-acquisition blocker arose. Detailed current n8n wait and Activepieces
piece-trigger pages were inaccessible through this browsing tool; treat their
specific lifecycle/recovery semantics as missing until source is inspected.
General pages were accessible and cited above. Documentation is a date-stamped
capability lead; fault guarantees remain implementation/design/test questions.

One testing lesson from the current LangGraph pattern guide: its introductory
joke gate accepts output containing `?` or `!`. This illustrates graph routing;
it does not assess joke quality. Demonstration predicates are not campaign
success oracles. Use source/behavior contracts and actual useful outcomes when
evaluating our workflows, with bounded one-round review rather than recursive
certification of the measurement machinery.
