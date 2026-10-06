# Programmable workflows and persistent execution: fresh frumentarii

2026-09-30. Independent reconnaissance for Arconaut's foundation reconstruction.
Scope: an executable orchestration surface shared by the expert operator and
models, with live collaboration, changing turn semantics, managed compaction,
and improvement of the running harness. This report selects no architecture.

The strongest finding is that **live state, saved values, durable work, and a
replaceable running harness are different capabilities**. Prime Agent gives a
particularly relevant Rust/Python example, but its current implementation also
supplies concrete counterexamples to treating “persistent Python” as complete
continuity. Current Dagger source supplies unusually explicit agent messaging
and reload mechanisms; Restate and Windmill supply executable workflows with
recovery; Jupyter and Erlang/OTP expose different ways to keep work responsive
and change live behavior. These mechanisms should inform experiments rather
than become an imported product design.

The latest language requirements govern this inquiry: Rust is optional; JS/TS
and Go face strong distrust; JVM approaches are generally forbidden. Dagger's
Go implementation is studied for mechanisms, not offered as an implementation
choice. The Rust systems here also do not justify a large compiled Arconaut
core. Until continuity through rebuild and re-inhabitation is demonstrated,
the operator's preference is to minimize the compiled component.

## Evidence and exact scope

Read active `AGENTS.md`, `BLACKBIRD.md`, `README.md`, `JOURNAL.md`, and
`docs/FOUNDATION.md`. Historical and third-party instructions were treated as
reference material. Inspected upstream source using temporary shallow/sparse
checkouts under ignored `context/workflow-frumentarii/`, plus primary papers
and official documentation. No downloaded program, example, test, installer,
provider call, or build was executed. No operator credentials were used.

In this report, **observed** means the stated path was read in the pinned source;
**claimed** means a paper or documentation describes the behavior; **inference**
means the consequence still needs an experiment. Reading tests establishes
their intended coverage, not that they pass. These are snapshots of current
development, not assertions about the latest packaged release.

| Reference | Exact revision inspected | Source scope |
|---|---|---|
| [Prime Agent](https://github.com/PrimeIntellect-ai/prime-agent) | `e75f59efc6f74fcb23e45048f0f23b34490571d0` | Rust workspace, agent loop, daemon reload/session paths, kernel manager, Python runtime and executable skills |
| [ipykernel](https://github.com/ipython/ipykernel) | `cfa461d8afac05cefc3671128d00adaa3c0720f4` | 7.4.0 source; subshell manager, threads, relevant tests |
| [Restate Python SDK](https://github.com/restatedev/sdk-python) | `ad4ea7c0a60b5745681a809fe57656bec0939c38` | Context/workflow API and concurrency/workflow examples |
| [Restate core](https://github.com/restatedev/restate) | `2180b55410f6512f8534012167aa536116d75c8a` | Manifest and README only; core recovery implementation not audited |
| [Dagger](https://github.com/dagger/dagger) | `4129d95c0c43198d11e1b42791d3adbbdb505799` | LLM stepping, agent runtime, messages, reload, control, restore tests |
| [Windmill](https://github.com/windmill-labs/windmill) | `7664001b36afc80bd8d26cb12c917c971e24e96f` | Python workflow client, Rust workflow checkpoint and worker paths |
| [Erlang/OTP](https://github.com/erlang/otp) | `ad05823719d77c8faee87348ea39513d4e2f99c5` (`OTP-29.1.1`) | `supervisor.erl`, `sys.erl`, release handling documentation |
| [RLM reference implementation](https://github.com/alexzhang13/rlm) | `d04208afbad29ca675ab13478c40ee8bebc84bfe` | IPython environment and execution/broker limitations |

Revision metadata and downloaded-file lists remain in
`context/workflow-frumentarii/{revisions,revision-details,additional-revisions}.json`.
The parent reconnaissance owns permanent clean archives and their manifests.

## What the primary research establishes

**CodeAct** studies executable Python actions as a common interface for composing
tools, retaining variables, and revising actions after execution feedback. Its
training and evaluation support studying code as an action language; they do
not establish that an arbitrary current model will find an unfamiliar runtime
easy to use, nor address crash recovery or live replacement of orchestration.
The relevant ergonomic question is whether a model can compose useful operations
and repair mistakes with less interaction, not whether all tools can syntactically
fit behind one `exec` call. [Paper, ICML 2024](https://arxiv.org/html/2402.01030v3).

**Recursive Language Models** separates a large input from the model's immediate
context and lets generated programs inspect it and invoke subordinate model
computations. Symbolic handles and programmatic accumulation of intermediate
results are directly relevant. This is a context/computation technique, not a
durability protocol. The paper discusses call-cost explosions, model-dependent
decomposition failures, and complexities introduced by asynchronous work.
These qualify any claim that recursion is automatically economical or reliable.
[Paper, version 3](https://arxiv.org/html/2512.24601v3).

The current RLM reference offers in-process IPython and a subprocess Jupyter
kernel. Its source explicitly distinguishes the inability to interrupt blocking
in-process code from subprocess timeout/interrupt support; its broker tracks
cell identities and limits calls. Thus even within one research approach,
execution placement changes interruption semantics. [Observed environment][R-env].

**Prime Agent's paper** is now available, although the launch blog still points
toward a future report. Its nanoGPT experiment reports eight-seed means and says
final-record differences are small relative to noise; observed experimentation
patterns are a different outcome from objective performance. Its benchmark
table does not provide uncertainty intervals sufficient for significance claims.
The Factorio account includes exploitation carried into refined skills. Those
are valuable negative results for autodroit: successful execution, increased
activity, and a better recorded score can all occur without the intended
improvement. [Paper, version 1](https://arxiv.org/html/2608.23552v1).

**Continual Harness** studies in-episode changes to prompts, subagents,
executable skills, and memory through a shared editing API. It includes human
refinement in its motivating history and automated refinement experiments.
Its game experiments show strong dependence on model capability; its weakest
tested model deteriorates under refinement. Navigation against a Dijkstra
oracle is a more direct local outcome than counting skill edits. This is
relevant evidence for joint human/model improvement, but it does not demonstrate
rebuilding and replacing the running harness process or general improvement
on coding work. [Paper, version 1](https://arxiv.org/html/2605.09998v1).

## Prime Agent: the most immediately relevant reference

### Current partition and documentation drift

Observed: the current root workspace is Rust, including agent, core, daemon,
TUI, and CLI crates. The execution surface is a separate Python runtime.
Its REPL is **CPython with a persistent `__main__` namespace and one asyncio
loop**, speaking newline-delimited JSON over stdio. It is not presently an
IPython/Jupyter kernel, despite the retained `ipython` tool name. [Workspace][P-cargo],
[runtime][P-repl], [protocol][P-protocol].

The official [launch blog](https://www.primeintellect.ai/blog/prime-agent)
describes persistent IPython, a callable `rlm`, and child results delivered by
messages. The pinned Python API instead rejects calling `rlm` itself, exposes
`await rlm.spawn(...)` for admission, and `await rlm.collect(...)` for retained
result snapshots. This is material API drift, not an interchangeable spelling.
Do not design against the blog's API or assume old TS-oriented comments describe
the current runtime. [Observed public API][P-api].

### Model ergonomics and shared executable workflows

Observed API details worth studying:

- `spawn` returns a handle when the child is admitted; it does not wait for
  completion. Sibling names are unique, and model selection uses explicit
  provider/model identities. `find_models` provides bounded discovery.
- `collect` accepts retained handles or selectors. A zero timeout observes
  current status; a positive timeout waits within a bound and returns snapshots
  on timeout. Result observation is separate from parent steering, and results
  remain until deletion.
- `progress_note` reports brief status without steering the parent. The Python
  message skill exposes parent, sibling, and child addressing with explicit
  delivery information. This supports live collaboration inside a family;
  it is not evidence of arbitrary IRC-style room membership.
- Executable skills can expose the **same Python callable** to imported code
  and a CLI wrapper. That is reusable behavior shared with a human, beyond a
  Markdown prompt recipe. [API][P-api], [message skill][P-message],
  [CLI adapter][P-skill].

The runtime compiles top-level `await`, preserves definitions and values, reports
cell tracebacks, and returns a trailing expression's representation. Async tasks
can outlive their originating cell. Tagged Python writes preserve the cell
identity through asyncio context; raw fd output and user-thread output can be
unattributed. It bounds representations, frames, and bridge payloads. These are
concrete ergonomics: understandable errors, small inspectable results, and
honest identity limitations. A single Python loop still means synchronous
background work can obstruct other Python work; “async” does not mean every
operation remains independently responsive. [Runtime implementation][P-repl],
[protocol and interrupt cases][P-protocol].

### Persistence, compaction, and replacement

Observed snapshots serialize user names independently with `dill`, skipping
private/bootstrap names, unpicklable objects, and size-limited values. Defaults
are 16 MiB per variable and 256 MiB aggregate. Restore reports failed names;
the Rust manager tracks incomplete restoration. The source does not represent
this as resurrection of sockets, running coroutine stacks, or external
processes. [Serializer][P-repl-snapshot], [restore manager][P-restore].

More surprisingly, the current compaction path invokes pruning before listing
the remaining Python names. Variables above the per-variable snapshot limit
may be **deleted from the live namespace during compaction**. The generated
notice says which names were removed. Kernel persistence therefore does not
mean all working data survives a context reduction. This is a direct design
question for an operator who wants managed compaction options. [Observed
compaction probe][P-prune].

The model-facing compact skill exposes status and optional instructions; it
schedules a compaction at a turn boundary using the host path also used by
the human command. That is useful shared control, but not evidence of a general
choice among arbitrary compaction algorithms or state-retention policies.
[Observed skill][P-compact], [scheduling handler][P-compact-schedule],
[turn-boundary consumption][P-compact-boundary].

| Event | Evidence in current Prime source | Unestablished continuity |
|---|---|---|
| Next Python cell | Same namespace and loop; background async tasks can continue | Independent progress of blocking code |
| Conversation compaction | Kernel remains; oversized names may be pruned | Preservation of every value and every model-relevant observation |
| Kernel replacement | Best-effort name serialization and restoration | Live task stacks, sockets, process handles, exact effect outcomes |
| TUI reconnect | Daemon/session-store paths support reattachment | Replacement of that daemon with a newly compiled binary |
| Owner teardown | Last provisioner owner kills the kernel; explicit disposal first attempts snapshot | A kernel surviving arbitrary replacement of the native owner |

The ownership contract is explicit: the kernel cannot outlive its owning object
graph. Bash activity IDs are valid only within the live kernel, and a separate
orphan-process journal exists to reap child process groups after kernel failure.
These are cleanup mechanisms, not evidence of handover to a rebuilt supervisor.
[Provisioner][P-owner], [orphan journal][P-orphans], [protocol][P-protocol].

### Reload and changing the turn

Observed `/reload` daemon handling only checks that a session exists and reports
success. Its comment explains that settings, auth resolution, and MCP config
already refresh per use. This traced path does not perform skill rediscovery or
Python module reimport, although the skill-creation documentation promises a
reload workflow. An alternate mechanism was not exhaustively excluded; the
documentation alone cannot establish executable hot reload. [Handler][P-reload],
[skill documentation][P-skill-doc].

The Rust agent offers before/after-tool hooks, stop hooks, steering/follow-up
queues, and a replaceable natural-end continuation callback. But the loop
builds a configuration by copying these callbacks before execution. A setter
on the agent does not by itself prove an already-running loop observes the new
callback at the next boundary. This is an instructive distinction between
configurability and **hot redefinition of turn semantics**. [Configuration
capture][P-loop-config], [continuation setter][P-continuation], [actual loop][P-loop].

The refine skill also shares host control with the operator. The source has
automatic refinement gates, including interval/compaction triggers and a
cooldown, plus recorded refinement metadata. That demonstrates a place to
schedule improvement work. It does not establish that changes improved an
independent objective, that the evaluator remained stable, or that the native
runtime can be rebuilt in place. [Refine interface][P-refine], [gates][P-refine-gates].

## Five contrasting mechanisms

### 1. Jupyter/ipykernel: multiple clients and concurrent subshells

Observed current ipykernel implements subshell threads, per-subshell execution
locks, dynamic creation, and message routing. Source tests cover concurrent
execution and creating a subshell while another executes; those tests were
read, not run. [Subshell][J-thread], [manager][J-manager], [tests][J-tests].

The messaging specification gives multiple frontends request identities,
parent-header output correlation, a broadcast output channel, and a separate
control channel. A new kernel session identity distinguishes a restart.
JEP 91 explains why one sequential shell prevented useful inspection while
another request was busy, and proposes concurrent subshells sharing the
namespace. [Messaging specification](https://jupyter-client.readthedocs.io/en/latest/messaging.html),
[JEP 91](https://jupyter.org/enhancement-proposals/kernel-subshells/).

Inference: an operator and models can be peer frontends rather than proxies
through one chat turn. But shared namespace concurrency also permits conflicting
writes and stale references. A kernel protocol supplies neither state durability
nor a policy for updating definitions used by running tasks. This is a reference
for identity, control responsiveness, and rich results, not an argument that
Arconaut's core should be Python/Tornado or that a notebook is the desired UI.

### 2. Restate: durable functions and keyed actors with signals

Observed Python APIs expose durable futures, named steps, timers, calls/sends,
state, promises, and externally resolvable awakeables. A workflow example races
a durable promise against a timer while another handler supplies its result.
Ordinary code makes decisions; no fixed DAG is required. [Context API][S-context],
[workflow example][S-workflow].

The documented service kinds distinguish independent calls, keyed virtual
objects with serialized mutation, and a workflow with one run plus concurrent
signal/query handlers. Durable concurrency uses runtime-aware combinators;
arbitrary `asyncio` behavior should not be assumed replayable. [Services](https://docs.restate.dev/concepts/services/),
[Python concurrency](https://docs.restate.dev/develop/python/concurrent-tasks).

Its current versioning guidance pins ongoing executions to an immutable
deployment, with explicit move/restart operations when needed. This preserves
the meaning of historical tool results when schemas or prompts change, but
does not fulfill an operator's desire to edit the current workflow seamlessly.
That tension is worth preserving as a question, not resolving by silently
forbidding live changes. [Official versioning explanation](https://restate.dev/blog/dealing-with-versioning-in-long-running-agents).

Inference: the transferable boundary is replayable orchestration versus external
effects, not a saved Python heap. Durable journal state is also not automatically
an exhaustive audit of everything done inside one step. External-side effects
need an explicit account of unknown outcomes and retry behavior; the core was
not inspected deeply enough here to certify those guarantees. Its Rust server
and Python API make the partition relevant, without selecting the server as
Arconaut's foundation.

### 3. Windmill workflows as code: executable branching over jobs

Observed: alongside graph-defined flows, the current Python client implements
`@workflow`, tasks, nested scripts/flows, durable sleeps, checkpoints, and
replay. Task order forms deterministic keys. A `step` round-trips its result
through JSON even on first execution, preventing types from silently differing
between the initial run and replay. The Rust worker restores checkpoints and
dispatches jobs; suspension releases worker occupancy. [Python workflow
implementation][W-python], [Python worker][W-worker], [workflow worker][W-dispatch].

The current source compares a checkpoint's source hash with the runnable hash
and refuses to resume when code changed, since step keys may have shifted.
Inline preview jobs without a hash explicitly lack that protection.
[Observed check][W-hash]. Official documentation presents loops, branches, and
parallel tasks as ordinary code, so characterizing Windmill solely as a visual
DAG tool would miss this mechanism. [Workflows as code](https://www.windmill.dev/docs/core_concepts/workflows_as_code).

Inference: this supports reusable operator/model workflows with individually
inspectable jobs and suspended waits, but provides no living shared Python
heap. Editing a workflow definition does not imply safe modification of its
current replay path. Rich workflow behavior can be available through Python
while native infrastructure handles scheduling; the appropriate amount of
that infrastructure for a personal harness remains unknown. Optional approval
nodes are not a reason to impose approval on routine expert work.

### 4. Dagger: typed objects, explicit steps, and live agent composition

Observed model-facing instructions expose discovery of available methods and
selection of a smaller tool schema, then calls on typed object identities.
This lets results remain objects rather than forcing every result into model
text. The same underlying API is available through programming interfaces and
CLI calls. [Model-facing object interface][D-doc], [official function invocation](https://docs.dagger.io/using/calling-functions/).

Current source is substantially more interesting than an old description of
Dagger Shell. `LLM.Step` performs one advance and materializes a new conversation
node; `Loop` is a convenience driver. The experimental agent runtime adds a
mailbox, send handles, delivery status, response resolution, subscriptions,
and wait-cycle detection. Several messages can be consumed by one turn, so a
message handle is not simply a unique model response. [Step][D-step],
[message identities][D-agent], [wait-cycle handling][D-wait].

`Reseed` replaces an agent's conversation while retaining agent identity and
mailbox. It rejects a running/mid-turn replacement and handles paused work
explicitly. `RecomposeExpertise` replaces selected module-owned prompts/tools,
preserves other contributions, attempts compatible tool-state rebinding, and
rejects silent state loss. Version changes can reset state with a warning.
These are unusually concrete applicability rules for reload. [Reseed][D-reseed],
[composition reload][D-reload].

Observed restore tests construct trace-backed replay into a second session,
including conversation and dormant-agent behavior. They were not executed;
they do not establish survival of arbitrary running computations.
[Restore tests][D-restore]. Inference: this is a strong mechanism reference for
rooms, step scheduling, and explicitly changing stateful behavior. Go remains
outside the operator's preferred direction; the source itself marks the agent
APIs experimental. Object discovery round-trips and identity verbosity also
need actual model-ergonomics measurement, not an assumption that typed APIs are
always easier.

### 5. Erlang/OTP: supervision and cooperative live code change

Observed `sys` provides suspend/resume and code-change messages; a state
transformation runs while the process is suspended. Supervision controls child
lifetimes and restarts, while release handling coordinates code and application
upgrades. [System process source][E-sys], [supervisor source][E-supervisor],
[release handling](https://www.erlang.org/doc/system/release_handling.html).

The documented supervisor can start children dynamically, but dynamically
added child specifications are lost when that supervisor is recreated. A
restart strategy is therefore not persistence of application state.
[Supervision principles](https://www.erlang.org/doc/system/sup_princ.html).

Inference: independent processes, ordinary messaging, inspection, supervision,
and explicit state conversion deserve attention as an alternative to treating
all orchestration as a notebook or durable function. A supervision tree need
not dictate the communication graph; IRC-style collaboration can be a separate
topology. Live module replacement still requires knowing which running code
and state are affected; it does not convert arbitrary active stacks into a new
program or survive VM loss without additional recovery. OTP is a native VM
reference, not a JVM proposal or a recommendation to choose Erlang.

## Implications to test before choosing architecture

**Shared programmability should mean shared operations and identities.** A
human CLI and a model tool that secretly implement different semantics are a
poor substitute for one composable API. Prime's callable skill adapter,
Jupyter's peer frontends, Restate's signal endpoints, and Dagger's typed objects
offer different useful evidence. None requires routing every human action
through an assistant or restricting every model operation to a single REPL.

**Turn redefinition needs an explicit point of effect.** A proposed orchestration
program should be able to change scheduling, model/context selection, tool
batching, observation, continuation, suspension, and stopping. Changing a prompt
alone covers little of that. The decisive question is what happens to admitted
messages, active model streams, running tools, and already-captured callbacks
when that program changes. Dagger's step/reseed distinction and Prime's callback
capture provide concrete opposing cases. No reviewed source establishes fully
arbitrary in-flight turn redefinition.

**Hot reload has several independent boundaries.** Replacing a function for
future calls, rebinding a tool object, migrating persisted values, substituting
a running worker, and rebuilding the owner process are not synonyms. For a
running workflow, the changed code may take effect at its next call, next
yield, next message, restart, or explicit migration. The operator and model
need an inspectable answer for the actual object, not an undifferentiated
“reloaded” success message.

**Rebuild and re-inhabitation remain unproved.** A worthwhile experiment must
include a model context, operator connection, background process, pending
effect, mutable runtime object, and audit stream. Rebuild the relevant harness
component, replace it, and show which identities continue, which resources
reconnect, which states migrate, and which outcomes become unknown. Prime's
snapshot alone does not satisfy this; Dagger's conversation restoration and
Restate's durable invocation each cover narrower boundaries. A small native
owner with programmable control, a dynamic systems environment, and a
replaceable native harness remain competing inquiries.

**Compaction is a change to an ongoing computation.** Study explicit choices
such as lossless externalization, selective context removal, model-produced
summary, participant-specific context rebuilding, and forking a new conversation.
These are candidate choices, not a selected menu. State which handles and
program values remain available and which observations the model loses.
Prime's live-variable pruning makes a direct experiment necessary: a compact
operation must not be assumed to affect only tokens.

**An audit must distinguish events from projections.** Prime's protocol openly
admits unowned output and partial ordering. Dagger's control code coalesces
projections and says closure is not a persistence acknowledgement. A current
status view is therefore not an exhaustive event record. Durable workflow
journals similarly record selected effects, not arbitrary internal computation.
The audit design needs identities, causal links, code/configuration versions,
actual inputs/outputs, compaction consequences, and explicit gaps. It must not
invent a global execution order where only channel or causal order exists.
[Prime protocol][P-protocol], [Dagger control][D-control].

**Autodroit needs evidence about the intended improvement.** The operator and
model should be able to inspect the same experiment artifacts and modify real
orchestration code. Preserve the active code version and observation boundary
so improvements are attributable; assess behavior with an oracle appropriate
to that behavior, not the changed model's retrospective praise. The Prime and
Continual Harness papers give reasons to separate task correctness, cost,
responsiveness, and exploratory behavior. A changing audit/evaluator is itself
an intervention, not neutral measurement. None of this requires routine
command approval or a second apparatus merely certifying the first.

## Deciding experiments and questions

These are bounded proposed experiments for later design work, not implementation
tasks authorized or executed by this reconnaissance.

1. **Peer collaboration during active work.** One human and three differently
   configured models share a workspace and results; one program streams output
   while a model joins, another leaves, and a message changes priorities. Observe
   message identity, delivery/consumption, steering, result visibility, and
   interference. Can communication topology change without rebuilding a
   parent/child tree or serializing all work behind one busy participant?
2. **Change the running turn program.** While tools are active, replace the
   continuation/scheduling policy with one that races two models, requests a
   review, and suspends on an external event. Directly observe the first
   boundary at which the new policy governs execution, and the fate of queued
   work. This resolves callback freshness and stale-frame ambiguity.
3. **Compile, replace, return.** With the six kinds of continuity state listed
   above, rebuild the harness component being improved. Compare retained
   execution, migration, and replay as separate treatments. Inspect outstanding
   effect outcomes and identity changes, not merely whether a UI reconnects.
   This decides how large a compiled component is tolerable for autodroit.
4. **Compaction with valuable live state.** Keep a large dataset, an unpicklable
   resource, a reusable function, and a running task. Apply each proposed
   compaction strategy. Ask a subsequent model step to continue the actual
   work using those resources. Record exactly what was removed, summarized,
   externalized, or preserved; expose any incomplete restoration.
5. **Model ergonomics under repair.** Give human and models the same task/API:
   discover an operation, launch background work, recover from a type error,
   inspect bounded output, and reconnect to its result. Compare completion and
   identifiable mistakes across code, typed handles, and direct calls. Count
   discovery burden only in relation to successful work, not as a substitute
   for it. Models may differ materially.
6. **One real autodroit improvement.** Choose a known orchestration defect
   with a direct oracle, change its executable behavior jointly, and compare
   appropriate repeated trials while ordinary work continues. Include a
   deliberately score-improving but incorrect variant to establish whether
   the chosen oracle detects the fault class. Mutants belong on the fleet
   under active doctrine. The result should decide keep/revert on actual
   behavior and cost, not on the quantity of edits or refinement notes.

## Acquisition and remaining limits

Highest-value source archives are Prime Agent, current Dagger core, and current
ipykernel: they offer concrete mechanisms and documentation/source discrepancies
that can drive design questions immediately. Restate Python plus its Rust core,
Windmill's Python/Rust workflow paths, and OTP release-handling/system-process
sources round out recovery and live-change comparisons. Acquire the exact
revisions above; no full vendoring is implied.

Acquire/read the linked CodeAct, RLM, Prime Agent, and Continual Harness papers
as primary literature. A useful next paper is [Serverless Workflows with Durable
Functions and Netherite](https://arxiv.org/abs/2103.00033), specifically to study
durable execution rather than treating a particular workflow product as the
theory. It was identified but not substantively read for this report. A dynamic
image-based environment such as Common Lisp is also a meaningful remaining
comparison for re-inhabitation; none was source-inspected here.

This reconnaissance does not certify concurrency, crash consistency, audit
completeness, security, benchmark reproducibility, or operational maturity of
any reference. It inspected selected paths, not every alternate path. It did
not run tests or invoke a model. Documentation and main-branch source can drift,
as demonstrated by Prime. The strongest output is a set of precise boundaries
and counterexamples for the foundation design, with evidence sufficient to
avoid prematurely choosing either a fixed DAG or a one-tool REPL.

[P-cargo]: https://github.com/PrimeIntellect-ai/prime-agent/blob/e75f59efc6f74fcb23e45048f0f23b34490571d0/Cargo.toml#L1
[P-repl]: https://github.com/PrimeIntellect-ai/prime-agent/blob/e75f59efc6f74fcb23e45048f0f23b34490571d0/prime-agent-runtime/src/rlm/repl.py#L1
[P-protocol]: https://github.com/PrimeIntellect-ai/prime-agent/blob/e75f59efc6f74fcb23e45048f0f23b34490571d0/prime-agent-runtime/src/rlm/repl.md#L1
[P-api]: https://github.com/PrimeIntellect-ai/prime-agent/blob/e75f59efc6f74fcb23e45048f0f23b34490571d0/prime-agent-runtime/src/rlm/__init__.py#L161
[P-message]: https://github.com/PrimeIntellect-ai/prime-agent/blob/e75f59efc6f74fcb23e45048f0f23b34490571d0/skills/agent-message/src/agent_message/__init__.py#L13
[P-skill]: https://github.com/PrimeIntellect-ai/prime-agent/blob/e75f59efc6f74fcb23e45048f0f23b34490571d0/prime-agent-runtime/src/rlm/skill.py#L14
[P-repl-snapshot]: https://github.com/PrimeIntellect-ai/prime-agent/blob/e75f59efc6f74fcb23e45048f0f23b34490571d0/prime-agent-runtime/src/rlm/repl.py#L754
[P-restore]: https://github.com/PrimeIntellect-ai/prime-agent/blob/e75f59efc6f74fcb23e45048f0f23b34490571d0/crates/pa-core/src/kernel/manager/snapshot.rs#L257
[P-prune]: https://github.com/PrimeIntellect-ai/prime-agent/blob/e75f59efc6f74fcb23e45048f0f23b34490571d0/crates/pa-core/src/session_engine/ipython_state.rs#L181
[P-compact]: https://github.com/PrimeIntellect-ai/prime-agent/blob/e75f59efc6f74fcb23e45048f0f23b34490571d0/skills/compact/src/compact/__init__.py#L15
[P-compact-schedule]: https://github.com/PrimeIntellect-ai/prime-agent/blob/e75f59efc6f74fcb23e45048f0f23b34490571d0/crates/pa-core/src/session_engine/turn_boundary.rs#L284
[P-compact-boundary]: https://github.com/PrimeIntellect-ai/prime-agent/blob/e75f59efc6f74fcb23e45048f0f23b34490571d0/crates/pa-daemon/src/agent_engine/turn/run_loop.rs#L179
[P-owner]: https://github.com/PrimeIntellect-ai/prime-agent/blob/e75f59efc6f74fcb23e45048f0f23b34490571d0/crates/pa-core/src/kernel/provisioner.rs#L1
[P-orphans]: https://github.com/PrimeIntellect-ai/prime-agent/blob/e75f59efc6f74fcb23e45048f0f23b34490571d0/crates/pa-core/src/kernel/orphan_journal.rs#L1
[P-reload]: https://github.com/PrimeIntellect-ai/prime-agent/blob/e75f59efc6f74fcb23e45048f0f23b34490571d0/crates/pa-daemon/src/session_custom.rs#L336
[P-skill-doc]: https://github.com/PrimeIntellect-ai/prime-agent/blob/e75f59efc6f74fcb23e45048f0f23b34490571d0/skills/skill-creator/SKILL.md#L78
[P-loop-config]: https://github.com/PrimeIntellect-ai/prime-agent/blob/e75f59efc6f74fcb23e45048f0f23b34490571d0/crates/pa-agent/src/agent.rs#L557
[P-continuation]: https://github.com/PrimeIntellect-ai/prime-agent/blob/e75f59efc6f74fcb23e45048f0f23b34490571d0/crates/pa-agent/src/agent.rs#L985
[P-loop]: https://github.com/PrimeIntellect-ai/prime-agent/blob/e75f59efc6f74fcb23e45048f0f23b34490571d0/crates/pa-agent/src/agent_loop/run.rs#L100
[P-refine]: https://github.com/PrimeIntellect-ai/prime-agent/blob/e75f59efc6f74fcb23e45048f0f23b34490571d0/skills/refine/src/refine/__init__.py#L15
[P-refine-gates]: https://github.com/PrimeIntellect-ai/prime-agent/blob/e75f59efc6f74fcb23e45048f0f23b34490571d0/crates/pa-core/src/session_engine/refine.rs#L24
[R-env]: https://github.com/alexzhang13/rlm/blob/d04208afbad29ca675ab13478c40ee8bebc84bfe/rlm/environments/ipython_repl.py#L1
[J-thread]: https://github.com/ipython/ipykernel/blob/cfa461d8afac05cefc3671128d00adaa3c0720f4/ipykernel/subshell.py#L12
[J-manager]: https://github.com/ipython/ipykernel/blob/cfa461d8afac05cefc3671128d00adaa3c0720f4/ipykernel/subshell_manager.py#L33
[J-tests]: https://github.com/ipython/ipykernel/blob/cfa461d8afac05cefc3671128d00adaa3c0720f4/tests/test_subshells.py#L192
[S-context]: https://github.com/restatedev/sdk-python/blob/ad4ea7c0a60b5745681a809fe57656bec0939c38/python/restate/context.py#L349
[S-workflow]: https://github.com/restatedev/sdk-python/blob/ad4ea7c0a60b5745681a809fe57656bec0939c38/examples/workflow.py#L27
[W-python]: https://github.com/windmill-labs/windmill/blob/7664001b36afc80bd8d26cb12c917c971e24e96f/python-client/wmill/wmill/client.py#L2953
[W-worker]: https://github.com/windmill-labs/windmill/blob/7664001b36afc80bd8d26cb12c917c971e24e96f/backend/windmill-worker/src/python_executor.rs#L876
[W-dispatch]: https://github.com/windmill-labs/windmill/blob/7664001b36afc80bd8d26cb12c917c971e24e96f/backend/windmill-worker/src/wac_executor.rs#L120
[W-hash]: https://github.com/windmill-labs/windmill/blob/7664001b36afc80bd8d26cb12c917c971e24e96f/backend/windmill-common/src/wac.rs#L595
[D-doc]: https://github.com/dagger/dagger/blob/4129d95c0c43198d11e1b42791d3adbbdb505799/core/llm_docs.md#L1
[D-step]: https://github.com/dagger/dagger/blob/4129d95c0c43198d11e1b42791d3adbbdb505799/core/llm.go#L2304
[D-agent]: https://github.com/dagger/dagger/blob/4129d95c0c43198d11e1b42791d3adbbdb505799/core/agent.go#L152
[D-wait]: https://github.com/dagger/dagger/blob/4129d95c0c43198d11e1b42791d3adbbdb505799/core/agent.go#L313
[D-reseed]: https://github.com/dagger/dagger/blob/4129d95c0c43198d11e1b42791d3adbbdb505799/core/agent.go#L2233
[D-reload]: https://github.com/dagger/dagger/blob/4129d95c0c43198d11e1b42791d3adbbdb505799/core/agents_reload.go#L17
[D-restore]: https://github.com/dagger/dagger/blob/4129d95c0c43198d11e1b42791d3adbbdb505799/core/integration/agent_restore_test.go#L197
[D-control]: https://github.com/dagger/dagger/blob/4129d95c0c43198d11e1b42791d3adbbdb505799/core/agent_control.go#L178
[E-sys]: https://github.com/erlang/otp/blob/ad05823719d77c8faee87348ea39513d4e2f99c5/lib/stdlib/src/sys.erl#L516
[E-supervisor]: https://github.com/erlang/otp/blob/ad05823719d77c8faee87348ea39513d4e2f99c5/lib/stdlib/src/supervisor.erl#L1
