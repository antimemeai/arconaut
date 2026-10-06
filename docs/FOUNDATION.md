# Foundation reconstruction

2026-09-30. Status: operator intent and current research direction; no product
specification or implementation plan has yet been adopted.

## Operator intent

The operator described the goal on 2026-09-30:

> a coding agent that is designed from the beginning for advanced, technically
> sophisticated operator who will want to mix and match paradigms, do unusual or
> novel things (e.g. multi-model IRC-style live collaboration), integrate more
> completely with standard computing tools, programs, etc. and has almost zero
> interest in approving commands or fretting over tool calls

This is current design input. The product's purpose is to support expert work
and experimentation with models inside the operator's computing environment.
The foundation must be judged by how well it enables that purpose.

Further operator requirements supplied in the same discussion:

- Powerful, expressive workflows and orchestration available to both operator
  and model. Standing databases and persistent Python kernels are examples to
  investigate, along with new approaches we have not yet considered.
- Model ergonomics as a design concern in its own right.
- Hot configurable redefinition of the turn.
- Compaction as a managed event with different options.
- A core audit log capturing everything for study and improvement; the operator
  explicitly emphasized **audit**.
- Hot reload everywhere it is physically possible.
- **Autodroit mode:** operator and model collaborate on autoresearch-style
  improvements in situ. Arconaut's self-improvement is a core principle.
- A strong systems foundation whose behavior is not governed by a JS event loop
  or chosen for OpenTUI convenience. Rust is optional. JS/TS and Go face strong
  operator distrust; JVM approaches are generally forbidden. The operator excludes
  Python from production (2026-10-01); the proposed Rust/Python stack is rejected.
- Compiled languages are attractive, provided a model can preserve continuity,
  rebuild the harness, and inhabit the rebuilt version. Until that is supported,
  minimizing the compiled component is the operator's provisional preference.

The operator selected **C++ with Lua scripting** on 2026-10-01. Native C++ should
support live recompilation through RCC++, Godot-related machinery or a similar
mechanism; the particular mechanism remains open. Lua makes ephemeral tool
definitions, workflows and experimental interfaces convenient to create and revise.
This selects the languages, not a reload framework, runtime version or library.
The [starting design](STARTING_DESIGN.md) sketches the first self-development slice;
the [C++ rigor seed](../papers/2026-10-01-cpp-rigor-seed.md) adapts Rhizome research,
and the [Lua study](../papers/2026-10-01-lua-rigor-stack.md) supplies its counterpart.

These are current design inputs.
The research must investigate how the requirements work together, rather than
recasting self-improvement as only prompt editing or audit as debug logging.

### Discussion decisions: agency, autodroit, and the first milestone

The operator agreed with the programmable-working-environment premise and clarified
these requirements during the brief discussion:

- **First milestone: build Arconaut in Arconaut as soon as possible.** Get to a
  usable environment in which its own development can continue. The operator
  agreed to the completion scenario in BEHAVIOR on 2026-09-30: real repository work;
  changes to governing programs with deferred/interrupt activation; outpost refit
  with continuity and failed-build recovery; retained originals, model-usable
  context repair, and rageshake with captured state and tracked follow-up.
  Detailed interfaces and implementation order remain for the specification.
- **Programmability is available at all times.** The model has substantial agency
  to configure and modify its working programs during normal operation; merely
  exposing them for inspection is insufficient.
- **Autodroit is tunable.** It spans a couple of manual interventions through
  full autoresearch governed by the model, with steering from the operator.
  It adds an experimental working style, not the exclusive permission to change
  the harness. There is no assumed operator promotion approval for every change.

These settle product intent and priority. The subsequent C++/Lua decision above
selects languages; it does not prescribe an optimizer, adopt a resource-owner
architecture, or begin implementation ahead of the agreed brief discussion and
specification sequence.

### Discussion decision: CLM belongs in the first core

On 2026-10-01 the operator explicitly includes Context Language Models and its
evolution in the first-pass core. Model-editable live context is a first-milestone
requirement. The model can write its own transformations using ordinary tools and
Lua, inspect/revise the result, recover originals, and evolve its context-management
programs. A fixed menu of compaction tools is insufficient.

Preserve original observations, editable context revisions, and the exact provider
requests produced from them as distinct records. Editing presentation cannot erase
past effects or replace the original audit. Context repair may increase size.
Ordinary context data edits may affect the next request within a running turn;
replacement of its governing programs obeys the chosen deferred/interrupt rule.
The [CLM source study](../papers/2026-10-01-context-language-models-study.md)
grounds this capability and identifies released-reference limits. Incorporate the
pattern into owned C++/Lua machinery; this does not adopt its Python implementation
or make serving-side cache research a bootstrap prerequisite.

The operator also explicitly reaffirms that reaching self-development quickly
does not permit haphazard work. Literature and source sufficiency must be assessed
against the actual design claims; acquired file counts and a working demo are not
substitutes for grounded contracts, reviewed design and direct oracles.

### Discussion decision: Arconaut consumes the computational fabric

The operator corrected the lifetime/ownership proposal: kernels, databases, and
similar computation are services potentially shared by an army of Arconauts.
Arconaut is their **consumer/customer, not their governor**. A future meta-project
fabric supplies control-plane, scaling, and shared-service governance. A service
can be local or remote; physical location does not give the harness ownership.

Arconaut configures its own programs, submits work, uses service APIs, and retains
the references and observations needed to continue. Service allocation, global
lifetimes, resident heaps, and scaling belong to the provider/fabric. The model's
substantial agency over its own work is preserved; consuming an API does not make
the harness responsible for governing the service behind it.

This supersedes the earlier proposal that Arconaut owns resident kernels/databases
or a resource governor shared by all participants. Research implications to carry
into the ground-up specification: client/workload identities and reconnect semantics;
observed versus service-authoritative state; attribution of shared mutations; and
auditing Arconaut's actual exchanges without inventing service-internal capture.
Exact API, transport, runtime, and fabric architecture remain open.

Refit quiesces Arconaut's own execution and client requests. Its own standing
programs, if present, obey the pause/preservation rule. Shared services and their
independently governed computation are outside that pause scope; refitting one
consumer must not pause or terminate them. Reconnecting is not a promise that an
external service's state never changed while the consumer was absent.

The early self-development milestone can consume available services. Designing
the future fabric control plane or scaling is not a prerequisite for building
Arconaut inside Arconaut.

### Discussion decision: defer changes until current work concludes

The operator set the default activation rule: changes take effect after conclusion
of the current turn or affected workflows underway. This supersedes the proposed
default of switching policy at the next intermediate decision. The running unit
continues under its existing definitions; the change is pending for subsequent
work. What counts as conclusion is part of the current turn/workflow program,
not merely the end of one provider response or tool operation.

Provide an explicit combined **interrupt and apply now** action, analogous to
submitting queued prompts immediately with a gesture such as Control+Enter.
The gesture is illustrative; no exact shortcut or UI is selected. The action
interrupts the affected Arconaut work and brings the pending change into effect
without waiting for its natural completion. Make pending, interrupting, and applied
states legible. Interruption/transition mechanics and overlapping-workflow scope
need the ground-up specification; a request to interrupt is not itself proof that
the change is active.

This is an activation choice available to operator and model, not an approval
gate. Previously dispatched requests retain their actual inputs/bindings and
observed effects remain real. The action concerns the chosen consumer's turn/workflow;
it does not pause shared fabric services. Native executable refit still obeys
the established quiescence rules.

### Discussion decision: audit enables context repair and longitudinal study

The operator emphasized two further purposes of the core audit: the model can
repair overzealously compacted or otherwise corrupted working context, and the
accumulated record becomes a valuable dataset over time. Original observations
and transformation inputs/outputs remain accessible independently of the active
context that needs repair.

Proposed implications for specification: model-usable search/query/retrieval of
original material; locating a decision or constraint omitted/misstated in a summary;
inspecting context/compaction lineage; and rebuilding a useful working context from
selected originals and current observations. The repair is another recorded
transformation with its sources and actual result. Earlier defective context remains
available for studying the failure; repairing it does not rewrite history.

The corpus can support longitudinal research on workflows, model/configuration
differences, compaction, failure/repair, and autodroit interventions. Derived indexes,
annotations, and datasets retain their relationship to original evidence. Exact
retrieval and analysis mechanisms remain open for the ground-up specification.

### Discussion decision: model rageshake with captured state and follow-up

The operator wants model **rageshake** available throughout Arconaut: when the
model encounters frustration, a bug, or another problem, it can file a complaint
with details. The illustrative invocation/name was `LLMmmshake`; naming remains
open. The clarified requirement is a **bead capturing agent state in various ways**,
also tracked in a **separate database table elsewhere**. The reporter has no
obligation to fix the issue in that moment. The complaint should be followed up
for repair at the first suitable opportunity; reporting is not an abandonment path.

Proposed specification implications: one complaint identity connecting the bead,
tracking row, and original evidence; automatic capture of available context and
recent request representations, effective/pending definitions and configuration,
workflow/task state, operation handles/outcomes, runtime diagnostics where available,
and relevant audit material. Retain actual captured state or references to retained
originals, rather than requiring the model to manually reconstruct a report.
Service state is observed through the consumer's interfaces; reporting does not
give Arconaut custody of a shared kernel/database or require pausing it.

Keep a short complaint operation usable across working modes, including when a
normal operation or context has failed. The bead carries the actionable complaint
and captured-state material; the separate table supports querying/tracking complaints
and follow-up across runs. These serve different purposes from the original audit.
No obligation to investigate immediately is added to the reporting turn. Exact
capture, ticket/table synchronization, and triage/repair scheduling belong to the
ground-up specification. No actual bug complaint or implementation was created by
this design discussion.

Implications to develop and challenge in research and design:

- **Composition.** Models, context construction, coordination, and execution
  should be independently changeable enough to support different ways of
  working. Study what can be changed during ongoing work and what needs a new
  session. A fixed serial turn loop or parent/child delegation tree cannot be
  assumed to cover every intended paradigm.
- **Live collaboration.** Treat the IRC-style example as a concrete scenario:
  human and model participants exchange messages and contribute while other
  work remains in flight. Study participant identity, addressed conversations,
  individual model context, shared results, concurrent edits, and joining or
  leaving ongoing work. The wire protocol and coordination topology remain open.
- **Computing integration.** Study interaction with existing processes, files,
  pipes, streams, terminals, editors, version control, and remote execution.
  Examine use from programs and scripts as well as interactive use. Preserve
  meaningful behavior such as input, output, exit status, signals, and process
  lifetime when these tools become part of model-driven work.
- **Autonomous execution.** Design for standing operator authority and routine
  execution without command-by-command approval. Any access configuration must
  respect that preference. Useful visibility, intervention, and failure reporting
  should support directing the work with little ceremony. Their exact interfaces
  require design; a stream of approval requests is not the intended interaction.

These implications are proposed interpretations of the stated goal. They select
no plugin system, event store, capability scheme, model vendor, UI, or transport.
The operator's preference describes Arconaut's product behavior; it does not
itself request an external publication or other unrelated action in this session.

## Research consequences

Model ergonomics includes whether the model can discover, compose, inspect, and
revise useful operations with understandable results, failures, and ongoing-work
handles. Human and model access to orchestration must both be studied. A workflow
may contain programs, model calls, messages, waits, branching, iteration, and live
operator input; research must not silently reduce workflow to a static DAG.

Study a turn as configurable execution semantics: how work begins, which model
and context participate, how ongoing work is observed, and what ends or suspends
it. Compaction needs explicit initiation, strategy choice, before/after context,
and consequences for continued work. Define when a changed configuration or
workflow takes effect on work already in flight.

For audit, determine what the core observes and records: model requests and
responses, participant communication, program input/output and outcomes, context
construction and compaction, state/configuration/code changes, and experimental
results. Study correlation, ordering, completeness, durable recording, and useful
querying. Identify unobservable information and recording gaps honestly. A saved
conversation or reconstructed reproduction script cannot alone establish that
the actual execution was captured.

Retain the complete captured original material independently of active context,
compaction, and derived query views, including failed or abandoned work and actual
transformation inputs/outputs. References must locate retained bytes. A summary,
successful continuation, or deletion marker cannot replace that material for study.
Any recording-loss or retention policy requires explicit treatment against the
operator's comprehensive audit requirement. Use known emitted input/output as a
direct completeness oracle; internal consistency of surviving records is insufficient.

For hot reload and autodroit, distinguish live definition replacement, migration
of existing state, and executable replacement. Study how model context, ongoing
programs, outstanding effects, operator connections, and the audit record survive
each. A small compiled supervisor with programmable control, a live dynamic
systems environment, and a rebuildable native harness are competing approaches
to examine. Their mutable/stable boundaries remain design questions.

Autodroit's proposed interpretation is joint improvement of Arconaut during real
work: inspect the audit, formulate an experiment, change the relevant harness
behavior, assess the result, and keep or reverse the change. Research must establish
how meaningful comparisons coexist with live changes to the system doing the
measurement. Configuration, workflows, context policies, and harness code all
belong in the inquiry; the exact experimental procedure is not yet specified.

## Outposts: working environments and collaborating peers

The operator sees uses beyond refit. An outpost may receive the full context or
a selected subset, run on a remote server, let the model work directly against
that server's environment, and communicate with a colleague in the main chat.
This is current design intent, with mechanism and interface still open.

The operator also proposed a local outpost with a Gemini key as a way to expose
Google search and other provider-specific capabilities, and noted that some
outposts may be useful without an active model. This is a capability example,
not a verified provider integration. Outposts need not all be chatting agents.
Record that breadth and continue the integrated design; the operator explicitly
asked us not to get lost in outposts.

An agent outpost needs its own useful execution environment, provider access,
local computing tools, working context, and a way to communicate with other
participants. A model-free outpost may expose useful operations without an
agent loop. Remote work occurs in the remote environment; its results and progress
can be shared with the main colleague. The design must account for the actual
host, workspace, active definitions, and model/provider configuration.

Distinguish two proposed uses:

- **Conversation handoff.** During arcorefit, the outpost takes over the working
  conversation and operator focus while the main harness is quiescent. Subsequent
  outpost conversation is part of the data returned to the refitted harness.
- **Collaborating peer.** A remote outpost starts from selected context and works
  alongside an active main colleague. It has a distinct working context and can
  exchange messages, observations, questions, and results. Communication need not
  follow a fixed parent/child tree or wait for a final task report.

Research/design questions include what the selected data contains, how sources
and workspace identity remain legible, and how later context updates are shared.
Selection, summarization, and ongoing synchronization are different operations;
the complete captured audit remains separate from each participant's active
context. A remote path or observation must retain its environment identity when
discussed in the main chat.

Study message acceptance, delivery, and consumption while peers are working,
network disconnection/reconnection, independent outpost lifetime, and which
participant owns each action. Both operator and model should have expressive
access to the same outpost operations. These questions select no transport,
remote deployment system, synchronization policy, or approval ceremony.

## Refit: operational rules and the outpost proposal

The operator supplied this concrete autodroit example and subsequent rules on
2026-09-30. Context is local working data. A refit can hand that data and the
operator conversation to another process while rebuilding the main harness.
The example command name **arcorefit** is provisional.

Current operational rules, explicitly supplied by the operator:

- The main harness has no active programs or provider requests at refit time.
- Arconaut's own harness-managed standing work is preserved and paused during
  refit; it is not terminated as the refit strategy. This does not include consumed
  shared kernels/databases/services, whose governance belongs to their provider/fabric.
- Work that must run uninterrupted belongs in independently managed OS daemons.
  The operator arranges that deployment; it is outside the harness pause scope.

The operator's proposed sequence is an independently running **outpost** with
provider access inherited or obtained through authentication. At a quiescent
boundary, arcorefit redirects the working context and operator focus to its chat,
tells the model that refit is underway, and lets the outpost run and observe the
compilation, including stdout/stderr. A separate chat window is an interface
possibility, not a selected requirement. Outpost chat and build activity are
separate from the main harness's quiescence rule.

Research/design consequences of this proposal:

1. Stop admitting new main-harness work, settle current commands/provider requests,
   and pause standing work before entering refit. Merely sending a pause/cancel
   request does not establish the required state. If quiescence cannot be reached,
   the main harness has not entered refit. Drain/cancel policy remains to be designed.
2. Hand over the conversation's working data and relevant obligations, with clear
   ownership of subsequent messages/actions. The outpost must be able to continue
   useful discussion and compilation without dependence on the harness being rebuilt.
3. Keep any harness-owned standing programs alive and paused through executable replacement. Their
   lifetime, reconnection, and eventual resumption need a mechanism; recreating a
   program from its remembered name alone does not establish this preservation.
   Reconnect to consumed services under their own contracts without pausing,
   relaunching, or claiming custody of their underlying computation.
4. Preserve a continuous audit of quiescence, pause, context/focus handoff, build
   inputs/output, running version, outpost activity, return, and resume. Original
   captured material remains available independently of the model's active context.
5. Design return of the accumulated outpost conversation to the rebuilt harness,
   and reconnection/resumption of standing work. A build or startup failure leaves
   the outpost available for diagnosis and repair; recovery/rollback policy remains
   an explicit design question. Give the handed-off conversation one active
   execution owner at a time; separately identified collaborating peers can work
   concurrently under the outpost model above.

These consequences develop the operator's example; they do not select IPC, state
storage, suspension primitives, process supervision, authentication, a UI, or a
language. The refit scenario now tests **quiesce → outpost → rebuild → return →
resume**. Active-work hot redefinition remains a separate research question.

## Recover the intended foundation

The operator remembers work on time and foundation pieces and was surprised by
the recovered provider and tool-execution code. They directed us to place the
whole inherited implementation in `quarantine/arconaut/` and reconstruct the
project following the fresh Rhizome pattern. That move is the working baseline.
Remembering the earlier stopping point helps establish provenance; it does not
limit the new product to time primitives. Current intent governs the new design.

Begin by recovering what the remembered time/foundation work was meant to do.
Use the preserved source, design notes, and operator recollection as evidence.
Git records when files appeared; configured author identities and phase labels
do not establish who directed or approved the work. Keep unresolved attribution
explicit. A timestamp field alone does not constitute a time design.

Questions to develop into a coherent foundation design include:

- How are people, models, conversations, and running programs identified? Which
  events and lifecycle states matter, and what ordering can concurrent work
  actually establish?
- What does time mean for elapsed work, deadlines, interruption, paused sessions,
  and resumption? Which questions require wall time or a monotonic clock?
- What must remain known after restart, and when must an external effect's
  outcome remain unknown?
- How do shared observations, participant messages, and private model context
  relate? How do we distinguish an observation from a still-current fact when
  another participant changes the workspace?
- What does steering mean while several participants and programs are active?
  What should interruption affect, and what continues independently?
- What continuity is possible through reload, recompilation, and executable
  replacement? Which runtime resources must outlive the component being changed?
- How can audit completeness and useful experiment comparisons survive changes
  to turn semantics, compaction, and the harness itself?
- Which responsibilities belong to the foundation, and which require a later
  provider, tool, interface, memory, or coordination design?

These are proposed research questions. They do not select clock types, a storage
engine, a crate partition, or a particular execution architecture. Live
collaboration and autonomous execution inform these questions from the beginning.

## Reconstruction sequence

Develop representative scenarios covering both ordinary repository work and the
operator's unusual collaboration/composition ambitions. Use them to challenge
the foundation together; one convenient serial scenario cannot define its whole
scope. Historical time work can inform this inquiry without blocking research
on the new requirements.

The [fresh frumentarii synthesis](../papers/2026-09-30-frumentarii-synthesis.md)
and intermediate reports supply current mechanisms, source limits, and proposed
discriminating scenarios. They select no architecture.

At useful research/design checkpoints, consult Rhizome's
[C++ rigor proposal](../../rhizome/papers/cpp-rigor-stack.md) for applicable findings.
Its C++ choice remains undecided. Toolchain feasibility, explicit ownership and
representation contracts, fault-directed checking, and direct semantic/recovery
oracles may inform a compiled Arconaut component if selected. This is a read-only
cross-project research lead, not a language choice or a recurring automation.

Read the actual material needed for each foundation question, acquire relevant
primary literature and clean reference implementations, and save the intermediate
reasoning in `papers/`. The [assessment](RESTART.md) supplies concrete failure
cases for challenging a design; its possible reuse candidates remain quarantined.

Write the foundation's purpose, behavior, state transitions, time semantics, and
failure boundaries as one coherent design. Choose useful models and direct
oracles by the fault classes they can expose. Adversarial review must challenge
that design before an implementation plan is written.

Then write and adversarially review the implementation plan. Only that plan can
select a legacy mechanism for adaptation. Product coding follows Blackbird's
written sub-plan, red oracle, implementation, green oracle, code review, and
finding-resolution cycle. This fresh root currently has no Cargo workspace.

Beads tracks the work and dependencies. The journal records actions and decisions;
the historical backlog is retained solely as evidence in quarantine.

## Operator decisions on colleagues and operating modes

2026-10-06. Colleagues should include Kimi, MiMo, another ChatGPT session, Claude, or
Grok. The operator requires configurable subagent limitations but fundamentally
wants the available model/inference-provider capability to define the envelope,
rather than arbitrary harness restrictions. Any configured model/provider should
be able to request colleagues on any other configured model/provider, including
same-provider combinations. The named roster is an initial preference, not a
closed enum. Account/provider capabilities and actual callable adapters must be
verified; no invented credentials or model names.

Spawning is an Arconaut operation: a model asks the harness to create another
participant and the harness invokes that participant's chosen provider/adapter.
It need not depend on one vendor's proprietary subagent feature. Configuration
may bound concurrency, nesting, requests/usage, elapsed time, tools/workspace,
context sharing and continuation policy. These are explicit configurable operator
choices, not fixed model pairings or per-call approval gates. Communication need
not follow the spawning tree. Peer identity, independent context, audit, actual
capabilities and completion/cancellation meanings remain required.

The operator proposes station mode as background Arconaut listening to a feed or
trigger, and campaign mode as traditional operator-in-seat work. Both use the
same participant/tool/workflow/context/audit engine. The detailed trigger,
attachment and mode-transition design is discussed in
[participants and operating modes](PARTICIPANTS_AND_MODES.md); these are not
implemented or qualified capabilities of the current bootstrap.

## Operator requirements for optional integrations, HUD and multiplayer

2026-10-06. Keep each Arco installation lean: common integrations such as Linear,
Slack, Discord, GitHub and Hugging Face should be optional packages/plugins or an
equivalent mechanism, not all bundled with their runtime/dependencies. C++/Lua and
library-adoption requirements remain; no particular extension transport is selected.

HUD is a thematic current-events feed relevant to a selected project set: CVEs,
arXiv topics, financial feeds and other configured sources. Harness runtime status
is a separate UI concern. Study Claude Code's new teams and mods features using
current primary docs and actual public source/tests.

Multiplayer means n Arconauts sharing development/build over a network, with
agent-agent, human-human and human-agent communication. n=2 is the initial concrete
thought experiment, not a hardcoded limit. Independent Arco runtimes/contexts and
shared development facts must be distinguished; do not substitute a single lead's
subagents or several attachments to one process. Multiplayer is P2P and end-to-end encrypted, from IRC-style integrated chat
to optional deeper problem/file/build sharing and model mail between live sessions.
No shared-filesystem, rendezvous/relay or cryptographic implementation selected yet. Integrations/HUD/multiplayer discussion follows settling
the useful-work evolutionpilot scheduled after managed compaction and before giga.
