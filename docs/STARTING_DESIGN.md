# Arconaut starting design

2026-10-01. Discussion sketch for getting Arconaut developed inside Arconaut.
The operator has selected **C++ with Lua scripting**. The architecture below is
proposed; the native reload implementation, runtime versions, dependencies, and
detailed specification remain open. Research and review precede product coding.
The operator includes CLM in the first-pass core: model-authored live context
editing, repair and evolvable context-management programs are in the initial slice.
The [research readiness assessment](../papers/2026-10-01-design-readiness.md)
connects first-core claims to literature and acquired mechanisms. Enter specification
with those source limits; the sketch is not an operationally qualified stack.

## The division of work

C++ supplies execution, resource ownership, audit capture, provider transport,
and continuity through replacement. Hot-recompiled C++ components make substantive
native behavior editable without rebuilding the entire running environment. Lua
supplies convenient, ephemeral programming: tool definitions, workflows, turn and
context policies, service adapters, and experimental interfaces. Either language
can carry substantial behavior; this is a useful default division, not a restriction
on what the model may change.

Human, model, Lua programs, and interface clients use the same operation vocabulary.
The model can inspect and modify its governing source during ordinary work.
Autodroit adds experiment orchestration and comparison; it does not grant otherwise
unavailable editing privileges. Routine execution follows standing operator authority.

| Part | Initial responsibility | Evolution |
| --- | --- | --- |
| C++ foundation | Operation identities and lifecycle, process/stream ownership, clocks, audit originals, generation references, session continuity | Rebuild and outpost refit |
| Replaceable C++ components | Provider-specific behavior, native tools, richer interfaces and other substantial extensions | Compile candidate, check it, activate at a defined boundary |
| Lua programs | Tools and schemas, workflow composition, default coding turn, context/compaction strategies, quick interface experiments | Load candidate definitions; ongoing work keeps its generation |
| Operator interfaces | Conversation, output, pending changes, interruption, reload/refit status | Clients of the operation interface; presentation does not govern execution |
| Outpost | Independent conversation and tools while the main runtime is quiescent or unavailable | Retained runnable artifact and provider access independent of the candidate build |

Kernels, databases, and remote computation remain consumed services. Arconaut owns
its client operations and observations; it does not acquire their control plane.

## Operations and programmable turns

Start with a small useful vocabulary: inspect/edit files; launch and interact with
processes; request a model response; send participant messages; construct/query
context; inspect retained audit; stage definitions; activate or interrupt; refit;
and report a captured-state complaint. Tool definitions describe discovery and
model-facing inputs/results, then invoke these operations or compose programs.
They can be added and changed without recompiling the foundation.

Each effectful attempt receives an identity and records its actual inputs and
governing generation before dispatch. An ongoing operation exposes observations,
input where supported, completion, and cancellation status. Cancellation requested,
process exit, stream EOF, provider termination, and effect settlement are distinct.
An unknown external outcome remains unknown and is not automatically retried.

The default coding turn is an ordinary Lua program: build context, call a model,
dispatch requested tools, deliver observations, respond to steering, and decide
when the turn concludes. The foundation supplies scheduling and operation state;
it does not bake in a universal alternating model/tool loop. Later programs can
coordinate multiple model participants and live rooms using those same primitives.

Lua coroutines can make workflows convenient, but their stacks are not the durable
operation record. Each Lua VM has a defined execution owner. Awaiting an operation
yields through a specified binding; blocking OS work runs outside that VM. Exact
thread/process scheduling is a specification question, not a reason to adopt an
unexamined framework event loop.

The Lua rigor proposal favors ordinary annotated Lua and a narrow owned binding.
Qualify a C++-built Lua runtime so its errors unwind C++ owners correctly; native
exception conversion must not catch Lua's internal control-transfer exception.
Version/tool compatibility and continuation semantics are real qualification work,
not properties supplied by annotations. See the
[Lua rigor study](../papers/2026-10-01-lua-rigor-stack.md).

## Replacement and state

Editing, compilation/loading, checking, and activation are separate steps. Changes
are pending until the affected current turn/workflows conclude by default. An
explicit interrupt-and-apply-now stops admission under the old definitions and
settles the affected work before activation. If settlement fails, the change stays
pending and the reason remains visible. Unrelated work need not be stopped.

Running units retain the complete definition generation they need, including Lua
closures and native callbacks. Replacing a table or function name does not replace
an active stack. Before retiring a native generation, account for every executing
call, callback, object, thread, and Lua reference into it. Keeping an older generation
loaded temporarily can be correct; silently accumulating generations forever is not
an adequate retirement design.

Cross-generation state uses explicit schemas and migration, or stable foundation
handles whose validity is checked. Native interface/layout changes require refit
unless an explicitly designed compatibility boundary supports them. Migration
failure leaves the old generation usable where the failure contract permits; no
claim of rolling back arbitrary native faults or external effects is made.

RCC++ is the leading standalone mechanism to study: it compiles modules and swaps
runtime objects using explicit serialization. Standard Godot GDExtension reload is
documented as editor-only; JENOVA documents runtime C++ script reload but requires
explicit cross-reload storage and currently documents Windows/Linux support.
These are research leads, not selected dependencies. The qualification subject is
our generation/state/activation contract on macOS arm64, followed by fleet targets.
See the [C++ rigor seed](../papers/2026-10-01-cpp-rigor-seed.md).
The subsequent [native source study](../papers/2026-10-01-native-reload-grounding.md)
finds that RCC++ retains modules but destroys replaced objects; this does not supply
our old-work preservation contract. Neither RCC++ nor cr fault protection establishes
safe C++/Lua unwinding or recovery from arbitrary native corruption. Specify the
owned lifetime/failure boundary before choosing a mechanism.

## Audit and context

The foundation captures original model exchanges, participant messages, process
input/output, operation transitions, effective/pending source definitions, context
transformation inputs/outputs, and refit/experiment activity. Records connect
participant, environment, attempt, context, and generation identities. Retain bytes;
summaries, indexes, and active context are rebuildable derived data.

Proposed minimum storage is an owned append-oriented record stream plus retained
original blobs, with a replaceable query/index layer. Specify write failures,
short writes, stream backpressure, recovery, and the durability point before coding.
If recording fails, stop admitting new effects and expose the failure. Already
running producers require an explicit buffering/spooling/pause/failure disposition;
there is no promise of unlimited RAM or lossless capture after disk exhaustion.
The storage-engine choice remains open; future Rhizome integration is not a bootstrap
prerequisite.

Lua chooses how to assemble and compact working context. The model can retrieve
originals and publish a repaired context with lineage to its sources, retaining the
earlier flawed context. Following the operator's
[Context Language Models lead](https://github.com/facebookresearch/context-language-models),
make participant context directly model-editable as local data, potentially ordinary
files, with Lua programs supplying different editing/assembly policies. Bind each
provider request to the exact context revision and assembled input it actually used.
Later edits affect later requests; they cannot rewrite the input already sent.
Concurrent writers need explicit revision/conflict behavior, not silent last-writer
loss. File identity alone does not make several agents share one working context.
Proposed distinction for discussion: editing working context is an ordinary data
operation allowed within a turn under its current program. Replacing the program
that governs context editing/assembly uses the agreed deferred/interrupt activation
rule. This permits repair before the next model request without silently replacing
the running turn's policy. The final specification must make that distinction clear.
Preserve structured roles, tool linkage, attachments and attribution; an editable
text view must not silently become a lossy canonical representation. See the
[CLM source study](../papers/2026-10-01-context-language-models-study.md).

Rageshake captures available state and audit references,
opens a bead, and records the complaint in a separately governed database table.
If an external sink is unavailable, a retained pending delivery remains visible;
claiming the complaint is fully delivered requires both actual sink outcomes.

## Refit and preserved standing work

Refit stops new main-runtime work, settles active programs/provider requests, and
pauses its own standing work. Shared services continue independently. The outpost
takes exclusive ownership of the handed-off conversation, observes compilation,
and remains usable through failed builds or candidate startup. Return includes
the accumulated conversation and audit, then standing work resumes.

Preserving a live process or Lua coroutine across executable replacement requires
something to outlive that executable. The proposed approach is a small independent
C++ continuity custodian holding the operation/connection identities and preserved
worker processes. Standing Lua execution lives in a preservable worker when its
live stack must survive; ordinary completed turns do not require that machinery.
Pausing is an acknowledged workflow checkpoint or a qualified process suspension
mechanism, with a defined process-tree scope. A pause request alone is insufficient.
An unpausable program blocks refit; uninterrupted work belongs in an OS daemon.

Preserved workers retain their old code generation until completion or explicit
migration. Each worker owns the native bindings/code and objects its live Lua state
requires, or reaches a surviving owner through an explicit protocol. Preserving a
VM with pointers into the replaced main runtime does not preserve executable state.
Startup of a new main runtime must not duplicate their effects. The
custodian itself is editable C++; replacing it needs an explicit custody-transfer
protocol with the outpost, not an assumption that it is immortal or immutable.
Custody transfer, crash recovery, and the actual pause mechanism are specification
obligations. This split is proposed because it addresses live-state preservation;
it is not yet an adopted process architecture.

Audit capture also has a surviving owner through handoff and main-runtime downtime.
The custodian or outpost retains the writer and observes build output; each active
runtime records its actual provider/service exchanges and final dispatched request
after adapter transformations. Transfer the observation/writer obligations explicitly
when replacing their owner. An editable-context snapshot alone is not wire-exact
capture, and uninterrupted shared-service internals remain outside our boundary.

## Course to self development

Specify and review one usable slice: one operator conversation, one provider,
ordinary filesystem/process tools, the Lua coding workflow, comprehensive capture,
CLM live-context editing/repair and evolution of its governing programs, and
definition activation. Choose the initial provider from existing operator access
without a credentials hunt or paid experiment during design. A simple terminal
client is sufficient; richer interfaces can follow through the same operations.

Then specify the reload and refit paths against the same real slice. The direct
acceptance exercise is an actual Arconaut source change made inside Arconaut:
edit/check; replace a Lua tool/workflow while current work retains its definitions;
compile/activate a native component; refit a foundation change through the outpost;
recover from a deliberately failed build/start; preserve and resume standing state;
repair a deliberately damaged context; and file a captured-state complaint.
Within that exercise, the model writes and uses its own context transformation,
inspects the resulting revision and final request, and changes the transformation
program under the ordinary generation-activation rules. Recovery from a lossy edit
must use retained originals, not an unexamined summary of the same damaged input.

The course is to settle these contracts, adversarially review the design, derive
and review a dependency-ordered implementation plan, then implement complete units
with their direct red/green oracles. This sketch is not that plan. Broad provider
coverage, a polished GUI, distributed fabric governance, and a general autoresearch
framework are follow-on work once the self-development loop is usable.

## Decisions needed for the specification

The remaining consequential choices are the smallest foundation/component ABI;
Lua runtime and error/yield boundary; native reload mechanism and retirement;
the preservation/custodian process arrangement; audit durability and overload
behavior; first provider/transport; and initial terminal interaction. The
[working brief](BEHAVIOR.md) remains the behavioral authority alongside operator
decisions in [FOUNDATION](FOUNDATION.md).

## Baby TUI proposal

At the operator's request, the initial interface proposal is a small C++ terminal
client with Lua-configurable commands and layout. It displays conversation, a
multiline composer, process/output detail on demand, and compact status for model,
context revision, active work, and pending definitions. Queued prompts, explicit
interruption/application and refit/outpost attachment must be available from this
client. Key bindings remain configurable rather than assuming Control+Enter is
distinguishable in every terminal.

CLM is visible from the beginning: inspect the current structured context, its
revision/history and actual dispatched provider input, and edit through an ordinary
editor or model-authored transformation. The baby UI need not contain a second
full editor. The evolving context and tool/workflow operations remain usable through
programmatic clients as well as its controls.

The leading rendering proposal is a narrow owned terminal layer. Define the actual
supported terminal profile, UTF-8 editing/display widths, bracketed paste, resize,
fragmented input and terminal restoration; do not call these solved by emitting
a few escape sequences. Rendered clipping/wrapping is derived presentation, never
loss of original audit bytes. FTXUI is a C++ candidate to evaluate if owning these
mechanisms becomes disproportionate; it is not adopted. Compare concrete benefit,
community, retained-state/lifecycle fit and maintenance before any adoption.

The client owns its presentation/input loop, while the harness owns execution.
Client exit or rebuild is not cancellation of model/workflow/process operations.
The same client protocol attaches to the outpost during refit; draft/input and
attachment continuity need explicit behavior. This is an interface proposal for
specification, not a selected framework or implemented TUI.
