# What the acquired corpus contributes to Arconaut

2026-10-01. Discussion input from the acquired 105-reference study: 104 external
references and historical Arconaut. The [matrix](matrix.md), [operation inventory](actions.md)
and [study index](README.md) lead to individual dossiers with evidence and limits. This is a
source study, not executed product conformance or an adopted specification.

The useful result is a map of mechanisms and their boundaries. The references
include coding agents, scientific applications, coordination services, SDKs,
runtimes, experiment libraries, templates and opaque distributions. Counting
positive cells would confuse those roles and reward broad claims. Use the native
operations and specific source/test witnesses to compare the behavior we need.

## Most useful comparators by question

| Arconaut question | References to study together | Actual contribution and boundary |
| --- | --- | --- |
| Can a model use executable programs as its ordinary action language? | [Prime Agent](dossiers/prime-agent.md), [GPTMe](dossiers/gptme.md), [Codex](dossiers/codex.md), [RLM](dossiers/rlm.md), [Smolagents](dossiers/smolagents.md) | Real Python or code-action bridges, helpers and child calls. Continuing interpreter state, fresh cells with explicit JSON state and nested model requests have different lifetimes. Several own kernels rather than consume shared services. |
| Can ordinary model work change the harness's behavior? | [Letta Code](dossiers/letta-code.md), [Agent Zero](dossiers/agent-zero.md), [GPTMe](dossiers/gptme.md), [AgentPool](dossiers/agentpool.md), [FastAgent](dossiers/fast-agent.md), [Dagger](dossiers/dagger.md) | Actual mod/extension/hooks, live-object introspection, programmable iteration or returned conversation continuations. Hooks, prompts, tool lists and the turn scheduler are separate degrees of agency. Registration alone does not settle old work or define our activation boundary. |
| How can independently working models communicate? | [Prime Agent](dossiers/prime-agent.md), [Oh My Pi](dossiers/oh-my-pi.md), [OpenClaw](dossiers/openclaw.md), [Dagger](dossiers/dagger.md), [Qwen Code](dossiers/qwen-code.md), [Adulari](dossiers/forge-adulari.md) | Addressed messages, busy-target queues/steering and correlated replies. Admission, durable commit, consumption, reply and effect completion are different observations. Parent fanout or selecting providers alone is insufficient. |
| What should a scientific standing-state interface expose? | [Claude Science](dossiers/claude-science.md), [OpenAI4S](dossiers/openai4s.md), [K-Dense BYOK](dossiers/k-dense-byok.md), [NullClaw](dossiers/nullclaw.md), [SpacetimeDB](dossiers/spacetimedb.md), [Squirreling](dossiers/squirreling.md) | Local memory facts, provenance notebooks, actual read-only SQL, service transactions and lazy query-mediated effects. Internal storage is not automatically a model-facing database; a digest can identify missing data without recovering it. |
| Can context be changed without destroying the source? | [GPTMe](dossiers/gptme.md), [FastAgent](dossiers/fast-agent.md), [OpenAI4S](dossiers/openai4s.md), [Hermes](dossiers/hermes-agent.md), [Letta Code](dossiers/letta-code.md), [Dagger](dossiers/dagger.md) | Separate views/range summaries, archives, digest/parent links, context replacement and logical restoration. Repair depends on what was captured before clipping, filtering, summarizing or deletion. |
| How should long-running work remain owned? | [OpenClaw](dossiers/openclaw.md), [FastAgent](dossiers/fast-agent.md), [GPTMe](dossiers/gptme.md), [Prime Agent](dossiers/prime-agent.md), [s6](dossiers/s6.md) | Continuing cell leases, durable process supervisors, actual transfer of shell custody, released background handles and daemon supervision. Detaching, pausing, terminating and proving settlement require separate contracts. |
| What can be learned about durable workflows? | [Restate](dossiers/restate.md) and [Python](dossiers/restate-sdk-python.md)/[Rust](dossiers/restate-sdk-rust.md) SDKs, [Windmill](dossiers/windmill.md), [Dagger](dossiers/dagger.md), [Jido](dossiers/jido.md)/[Jido.AI](dossiers/jido-ai.md) | Journaled invocations/results, scheduling, actor strategies, checkpoints and continuations. Recorded closure replay avoids some repeated calls; an effect before result settlement still needs reconciliation/idempotency. |
| Can the model file a useful complaint while continuing work? | [Oh My Pi](dossiers/oh-my-pi.md), [Claude public plugins](dossiers/claude-code-official-public.md), operator feedback paths in the dossiers | Oh My Pi has a real model issue device and grievance database. Its schema is a partial state capture and acknowledgement can precede insertion. Operator-only feedback and disabled issue stubs do not satisfy universal model rageshake. |
| What makes autodroit a measured experiment? | [AutoHarness](dossiers/auto-harness.md), [SelfHarness](dossiers/self-harness.md), [autoresearch](dossiers/autoresearch.md), [GEPA](dossiers/gepa.md), [LDP](dossiers/ldp.md), Letta Code's mods-learning path | Candidate generation, editable policies, trial runners, scores and promotion have different scopes. Model completion promises, prompt compilation and application self-repair are weaker than independent harness improvement evidence. |
| What can support compiled rebuild continuity? | [Rust Agent Orchestrator](dossiers/agent-orchestrator-rust.md), [SBCL](dossiers/sbcl.md), [SLY](dossiers/sly.md), [OTP](dossiers/erlang-otp.md), [s6](dossiers/s6.md), selected continuation/archive paths above | Actual self-build/exec, image saving, runtime loading, code generations, debugger/RPC, supervision and logical session restoration supply parts. No inspected implementation establishes our full quiescent refit plus independently powered outpost and re-inhabitation contract. |

These are comparators, not dependency recommendations. Their implementation
languages do not transfer with the ideas. The operator's distrust of JS/TS and Go,
general JVM prohibition and open Rust/C++/other decision remain current.

Catalog families are navigation, not language determinations. The acquired OpenDev
is now Rust; Mistral Vibe contains both the default Python loop and an experimental
Rust core with Python host adapters. The acquired OpenHands is a TypeScript
control center around separately distributed engines. Current source identity and
actual runtime boundaries take precedence over a remembered product architecture.

## The common execution model is a starting point

Most inspected coding agents construct provider context, ask the model for an
answer or actions, parse/admit those actions, execute, record results and choose
whether to continue. The important variation is who owns each transition.
Programs may be synchronous, cancellable futures, threads, subprocesses, background
supervisors or remote invocations. Results may be protocol objects, structured
values, clipped text or prose injected as a user message. Different choices can
look identical in a chat window.

An operation inventory is therefore more useful than a list of tool names.
For each grouped surface the study records input, result/handle, lifetime,
authority and evidence. A file tool can directly write bytes or merely propose an
editor change. A shell action can block until exit, return a resident handle or
transfer a still-running process to a supervisor. A code tool can create a fresh
namespace or share one with other agents. A tool may return a new conversation,
so its effect includes replacing the future execution environment.

Arconaut's editable decision/workflow programs need a precise relationship to
the part retaining operation identity, effects and originals. Broad model agency
is compatible with a small host that makes these transitions observable. The
matrix does not establish how much of that host should be compiled. That should
follow from the refit and failure contracts, rather than copying the size or
language of a reference.

## Model agency and activation are separate questions

Prime's model-requested compact/refine changes wait for a settled turn. Some
agents capture configuration for a request, then accept new settings at the next
model step. GPTMe takes a callback snapshot on invocation. Letta Code can dispose
and replace a mod generation; Dagger can adopt a conversation returned by a tool.
These all implement real change, with different timing and old-work consequences.

AgentPool's model tool executes Python with live context, agent and pool objects,
and optional state saving. That is unusually direct ordinary agency over the
running arrangement. It also makes shared object ownership and the persistence of
changes consequential. Kimi CLI supplies branching FlowRunner prompt graphs and
explicit background task operations; its reload preserves background work rather
than settling it. Both are useful programmability comparisons without satisfying
our chosen change/refit boundary automatically.

Our chosen default remains activation after the current turn and affected
workflows finish, with an explicit interrupt-and-apply-now operation. This timing
does not introduce a permission gate. The model should ordinarily be able to
author and install its own programs. The definition identity used by an existing
turn/workflow must remain recoverable while a candidate and activation request
are recorded separately. Failure during loading, interrupted activation and
overlapping workflow versions are questions the spec must answer.

## Shared computation requires explicit client custody

References frequently make a kernel or database an agent-owned resource. That
can simplify a prototype while making kernel destruction look like cancellation
or compaction. It is not Arconaut's boundary: computation may be shared by many
Arconauts, and the eventual fabric governs it. Arconaut consumes a service even
when the service happens to be on this machine.

A continuing namespace is only one dimension. Record the service/session
identity, which state is shared, who can mutate it, where a request is running,
what can be paused and how reconnection identifies a still-live request. A chat
snapshot cannot preserve a process. A Python pickle can skip unpicklable values.
A conservative replay may refuse promotion because steps would repeat effects.
Remote task IDs support observation but a local timeout may leave the remote
task running. These differences belong in the service interface and original
record, rather than being concealed by the word persistence.

Claude Science is explicitly included. Its available official contracts describe
Python/R variables across session steps, environment reuse, local remembered
facts, remote computation and artifact-version provenance. The kernel has idle,
session-end and environment-restart limits. The local memory database is not a
documented generic model SQL interface. The acquired distribution is opaque;
these are documented contracts, not a source audit of its engine. OpenAI4S and
NullClaw provide contrasting inspectable SQL interfaces, while K-Dense exposes
notebook/job/provenance records and their recall boundaries.

## Original audit and mutable context must remain distinct

Retaining messages through compaction is a valuable mechanism. GPTMe's on-disk
view/main and reload tests demonstrate it directly. FastAgent blocks replacement
when enabled original archival fails, then validates reconstruction against a
digest and rejects ambiguous or escaped material. Hermes and Letta Code preserve
range/parent relations rather than silently overwriting the selected context.

None of those facts establishes the whole original audit requested here.
Several implementations clip tool results before appending, omit unsuccessful
attempts or provider deltas, filter progress/review events, overwrite snapshots
or prune events at checkpoints. Dagger explicitly says bounded tool return values
were not persisted elsewhere. Docker Agent caps textual tool content at ingestion
even though its compaction metadata survives a SQLite reopen. GPTMe's CLI can
lose a buffered step during tool execution. Provenance records may distinguish
observed, inferred and declared edges while still missing overwritten artifacts.

The specification needs capture boundaries for requests, responses, programs,
outputs, errors, interrupted/abandoned work and context transformations. A reduced
model view should identify its originals and transformation history. Model repair
must record what it read, what it changed and why, retaining the damaged view and
failed repair attempts for study. A summary or hash cannot replace lost source.
Separate state checkpoints from the audit that produced those states.

## Live colleagues need more than asynchronous calls

Inspect the receiver while it is busy. Does a message become immediate steering,
queued next-turn input, a notification, a terminal write or a durable mailbox
record? Can an idle colleague continue from it? Does sending success mean admitted,
committed or consumed? Can the recipient finish just after admission? Do sender
completion, window loss or focus change revoke delivery custody?

Prime, Oh My Pi, OpenClaw and Dagger expose useful addressed operations. Coordination
fabrics and mail services expose additional surfaces, but they can govern external
harnesses rather than own model turns. An IRC-shaped client/server also needs real
model integration and effect ownership; sockets and rooms alone do not create
colleague agency.

Source defects make useful probes. The unofficial Claude artifact can swallow a
mailbox write failure while reporting success. VTCode's traced first queued input
makes a running child Waiting; another input in that state can launch a second
loop and overwrite its stored handle. **This is source-inferred, not reproduced.**
GasTown's inspected parallel-step helper marks work in progress while its goroutines
only report readiness. Those boundaries prevent a concurrency label from becoming
an unsupported coordination guarantee.

[NTM](dossiers/ntm.md) supplies a particularly useful delivery comparison: it
records intent before terminal paste, binds the target pane/process identity and
treats an ambiguous send as unknown rather than automatically retrying. Its
deliberate resend is a separate action. [MCP Agent Mail](dossiers/mcp-agent-mail.md)
instead commits database records before a best-effort Git/filesystem bundle;
reservations are advisory, and notification/read state is not model consumption.
[Open SWE](dossiers/open-swe.md) supplies a real busy-thread injection queue,
whose store consumption and graph publication have separate failure boundaries.
These are different useful contracts, rather than one generic reliable-send bit.

## Refit needs a stronger barrier than cancelled

A future can be dropped while its worker runs. A child can be marked cancelled
before reaching a checkpoint. A timed-out model subcall can finish during a later
cell. A killed root can leave descendants or remote effects. A background process
can intentionally survive harness closure. Several source paths explicitly have
these semantics, so finished chat work is an insufficient refit witness.

OpenClaw's code-mode owner is a useful narrower example: it revokes dispatch but
retains a runtime-refresh lease until late executions and continuation cleanup
settle; failed cleanup remains owned for retry. That controlled-promise oracle
checks custody ordering. Its channel reload can nevertheless proceed with active
work on timeout/lease demand, so it does not satisfy our strict refit rule.

Rust Agent Orchestrator goes further than a hypothetical rebuild command. It
builds the executable, probes its help entry, hashes it, records restart-pending
state in SQLite, defers other tasks and finally execs the daemon. Its driver can
report semantic completion before process exit, cancellation does not always wait
for reaping, and a bounded drain can expire without fencing exec. This is a real
compiled self-rebuild lead with material settlement limits, not evidence that
compiled self-development is impossible or that our complete refit already exists.

Our operation requires no active main-harness programs or provider requests,
preserved paused harness-owned standing work, and an independently powered outpost
that keeps model/operator continuity during compilation and activation. Consumed
shared services remain outside the pause scope. Work that must remain uninterrupted
belongs in independent OS daemons. Compilation failure, new executable failure,
outpost loss and unsuccessful re-inhabitation each need an observable recovery
path. This conversation handoff remains distinct from a concurrently working
remote peer with selected context. The corpus supplies parts, not proof of the
whole operation.

## Autodroit needs declared experimental authority and evidence

There are real candidate/evaluation/promotion mechanisms to study. Their metrics
can still be narrower than their names. A model saying done is not an external
quality witness. Repeating a benchmark twice and taking a mean does not resolve
task leakage, unstable denominators, selection effects or candidate changes to the
evaluator. A score selector can bypass a default acceptance threshold. A task
timeout need not terminate its execution.

Our range from a few manual changes to model-governed autoresearch should make
the experiment program editable and record who may change the subject, evaluator,
budget and promotion rule. Separate application repair from harness improvement.
Retain candidate definitions, unsuccessful trials, evaluation inputs/outputs,
decisions and recovery. Model governance with operator steering should not be
silently replaced by fixed host heuristics. The historical Arconaut implementation
is evidence for failure cases; no old module is selected for reuse.

## Discriminating experiments before specifying the guarantees

These are research proposals, not authorization to execute acquired programs or
a product implementation plan.

| Claim to distinguish | Independent witness or injected failure |
| --- | --- |
| Standing authority preserves ordinary continuation | Run the same action/continuation with interactive and standing authority; compare the next actual provider request and call/result identity. |
| Model changes become active at the intended boundary | Keep a turn and two workflow generations underway; publish a new definition; observe identities in each subsequent request. Interrupt-and-apply-now must record what was settled, paused or abandoned. |
| Cancellation is sufficient for refit | Use a child, descendant and remote request that leave externally observable continuing-effect markers; verify the refit barrier separately from cancellation acknowledgement. |
| Concurrent computation is isolated as declared | Two clients use distinct variable/output markers and overlapping cells; observe namespace, request attribution and stream ownership from outside the clients. |
| Busy-peer delivery is reliable | Send across active/idle/closing transitions, inject mailbox write failure and lose sender focus; distinguish admission, durable commit, consumption and reply. |
| Compaction retains repairable originals | Insert an unmistakable omitted fact, transform context, reopen from disk, retrieve the exact original and repair the active view; retain bad view and repair provenance. |
| Audit precedes apparent success | Fail capture/storage before an effect result or context replacement becomes visible; verify which originals and attempts remain and how the failure is represented. |
| Rebuild preserves the conversation | Compile while an independently authenticated outpost is live; fail the build and activation separately; verify identity/context lineage and that main-owned activity was settled/paused. |
| Autodroit improvement is measured | Keep evaluator material independent, inject a candidate that cheats or degrades another task, and preserve all rejected/failed trial records as well as the selected result. |
| Rageshake survives a broken tool/session | File from a model during an error or context problem; verify bead/database linkage and captured definition, context, work and error state after reopening. |

The first milestone remains building Arconaut in Arconaut as soon as possible.
Discussion should now decide the smallest ordinary editing/checking loop that
already exposes real program agency, original capture and a path to refit. Service
consumption, change timing and recoverable context are foundation boundaries;
elaborate deployment/control-plane work belongs to the separate fabric project.

## Rhizome research watch

The current [C++ rigor proposal](../../../rhizome/papers/cpp-rigor-stack.md) and
[type/refactoring study](../../../rhizome/papers/cpp-types-and-refactoring.md) remain
proposals, with language/dependency choices open. The transferable lesson is to
declare ownership, publication, invalidation and failure contracts before choosing
analysis tools; a large compiler flag list or sanitizer pass does not prove them.
Runtime continuation/request identities could benefit from distinct domain types,
while active/paused/settled/refitting states need representation and transition
contracts. Type/refactoring findings locate candidate patterns; their semantic
meaning and rewrite remain owned design work. This watch was read-only and does
not import Rhizome's scope or adopt its tooling for Arconaut.
