# Fresh frumentarii: a programmable, self-improving coding environment

2026-09-30. Comparative reconnaissance after the operator clarified Arconaut's
purpose. This report informs research design; it is not a language selection,
product specification, approved implementation plan, or acceptance of legacy code.

## Current purpose

Arconaut serves an advanced operator who composes paradigms, models, workflows,
and ordinary computing tools, including live IRC-style multi-model collaboration.
Routine command approval should contribute almost no friction. Both operator
and model need expressive orchestration. Model ergonomics, hot turn redefinition,
managed compaction, comprehensive core audit, hot reload, and autodroit improvement
in situ belong to the foundation inquiry.

Rust is optional; JS/TS and Go face strong distrust; JVM approaches are generally
forbidden. Compiled languages remain attractive if the model can preserve
continuity, rebuild its harness, and inhabit the result. Until that is supported,
minimizing the compiled component is the operator's provisional preference.
[The current brief](../docs/FOUNDATION.md) preserves the explicit requirements and
separates them from proposed interpretations. The old source stays quarantined.

Subsequent brief discussion clarified service ownership: kernels/databases/computation
may be shared by an army of Arconauts. Arconaut is their consumer/customer; the later
meta-project fabric governs their lifetimes and supplies control-plane/scaling.
The source comparisons below remain research evidence. They do not assign custody
of shared computational services to Arconaut or put them in its refit pause scope.

## Evidence and acquired corpus

Three independent conceptual reconnaissance units and a parent study produced:

- [Standing state, execution queries, and the workbench](2026-09-30-standing-state-frumentarii.md).
- [Programmable workflows and persistent execution](2026-09-30-programmable-workflows-frumentarii.md).
- [Systems runtime and continuity](2026-09-30-systems-runtime-frumentarii.md).
- [Autodroit and experiment mechanisms](2026-09-30-autodroit-frumentarii.md).

They use primary documentation, original papers, and exact source revisions.
Observed code, documented claims, and inference are distinguished. None executed
a reference product, installed its dependencies, called a live provider, or ran
a performance/recovery experiment. Current upstream source can differ from the
installed release. A source inspection establishes a mechanism or a visible limit,
not comprehensive behavioral correctness.

Acquired sixteen source snapshots: Prime Agent, Jido, SBCL, SLY/Slynk, Guile Fibers,
s6, Restate, Dagger, ipykernel, RLM, current Letta Code, SpacetimeDB, Squirreling,
GEPA, autoresearch, and Self-Harness. All 33,036 retained regular files and their
executable bits match the preserved ZIPs directly. Exact revisions, archive
hashes, omitted entries, and restoration inputs are in
[the source catalog](2026-09-30-reference-acquisition.json) and
[QUARANTINE](../QUARANTINE.md). Source archives remain intact. Distrusted-language
implementations are mechanism references, not assumed implementation substrates.

Seven PDFs were acquired: CodeAct, RLM, Prime Agent, Continual Harness, GEPA,
Self-Harness, and A Query Engine for the Agents. Versions/hashes/local ignored
paths are in [the literature catalog](2026-09-30-literature-acquisition.json).
The reports state what was read and what each study establishes. CodeAct's
download is v4; the workflow colleague's cited text inspection is v3, with a parent
v4 scan of executable-action/error-feedback material. No unperformed replication
or exhaustive version comparison is implied.

## Findings that change the inquiry

**Compiled does not imply a frozen environment.** SBCL/SLY compiles and loads
definitions into a running Lisp image; OTP explicitly controls module versions
and state conversion. The systems report traces their boundaries, including
captured definitions, active frames, thread limits, and external resource loss.
This warrants serious study of live compiled systems alongside a thin native
owner with programmable control. It does not yet establish full rebuild continuity.
[Compile/load source](https://github.com/joaotavora/sly/blob/3ffa216d0818972f7a7fea38a566a6b570349f3b/slynk/backend/sbcl.lisp),
[OTP code loading](https://www.erlang.org/doc/system/code_loading.html).

**Prime is more relevant, and more bounded, than its launch description.** The
pinned core is Rust around a separate CPython REPL. Name serialization, compaction
pruning, owner teardown, and the traced reload handler limit what “persistent”
and “reload” establish. Its admitted-child handles, result collection, and shared
callable/CLI skills offer useful ergonomics. The workflow report gives precise
paths for each finding. Designing against the older IPython/API narrative would
miss actual behavior.
[Current workspace](https://github.com/PrimeIntellect-ai/prime-agent/blob/e75f59efc6f74fcb23e45048f0f23b34490571d0/Cargo.toml),
[current REPL](https://github.com/PrimeIntellect-ai/prime-agent/blob/e75f59efc6f74fcb23e45048f0f23b34490571d0/prime-agent-runtime/src/rlm/repl.py).

**Live collaboration requires more than a message tool.** Goose rejects messages
to busy targets through its inspected orchestrator path; OpenAgents buffers work
on a busy channel. Dagger provides explicit message identities and recomposition
rules, while its experimental status and Go substrate constrain its role here.
Its reseed rejects mid-turn replacement. Each is useful evidence about scheduling
and applicability, not a ready-made realization of arbitrary live rooms.
[Goose orchestrator](https://github.com/aaif-goose/goose/blob/ac15f938151bb8c0efd93ed9c2c1cf61298934bf/crates/goose/src/agents/platform_extensions/orchestrator.rs),
[Dagger agent](https://github.com/dagger/dagger/blob/4129d95c0c43198d11e1b42791d3adbbdb505799/core/agent.go).

**Queryable state, audit, and model context answer different questions.** Science's
editable memory, custom evidence-database example, and artifact execution log
have distinct roles. Restate supplies queryable running work and deployment-pinned
recovery; SpacetimeDB separates visibility from durability. Letta records compaction
events while changing active context. These provide useful ingredients but none
inspected establishes everything Arconaut's core must capture. The standing-state
report also identifies a conflict in current Dolt isolation documentation; no
database choice should rely on an unexamined assumption.
[Science artifacts](https://claude.com/docs/claude-science/artifacts),
[Restate introspection](https://docs.restate.dev/services/introspection),
[Letta transcript](https://github.com/letta-ai/letta-code/blob/96eb977b9477045423005a3630e5f9293140249a/src/backend/local/local-transcript.ts).

**Self-improvement needs interpretable experimental evidence.** GEPA's shared
evaluation surface and candidate/engine contracts, autoresearch's legible loop,
and Self-Harness's failure-grounded changes supply different mechanisms. Prime
and Continual Harness research also expose metric exploitation, noise, and
model-dependent deterioration. They support investigating autodroit as real
executable changes with measured outcomes; they do not prove that any refinement
improves live coding work or preserves a rebuilt runtime. Detailed evidence and
limitations are in the workflow and autodroit reports.

## Competing foundation approaches worth examining

This table groups research alternatives by what owns state and what can change.
It does not select an implementation or promise that the listed runtimes supply
an entire coding harness.

| Approach | Mechanisms to examine | What must be established for Arconaut |
| --- | --- | --- |
| Live compiled Lisp environment | SBCL definitions/classes, Slynk evaluation/compile/load, OS process primitives | Model ergonomics; existing-frame/closure behavior; supervision; audit of effective loaded code; recovery of work outside the image |
| Supervised actor environment | BEAM/OTP messages, independent processes, release handling; Jido strategies/directives | State conversion; mailbox pressure; provider responsiveness; external process/kernel ownership; continuity through VM replacement |
| Programmable controller over a small native resource owner | Python or Scheme control; s6/process ownership; explicit IPC and reattachment | Which resources survive controller replacement; comparable human/model operations; executable lineage; replacement of the owner itself |
| Rebuildable native harness with explicit handover | Prime/Goose as source comparisons; durable execution and deployment boundaries | Demonstrated handover of identity, context, active processes, unresolved effects, and audit into the rebuilt executable |

The third approach need not make its native owner permanently unchangeable.
The fourth remains conditional on actual continuity evidence. Dynamic control
also has scheduling, migration, and failure limits. The compiled/dynamic boundary
should follow resource ownership and change semantics, rather than a language label.

## Scenarios to shape the research design

These are proposed discriminating scenarios; no experiment has been run and no
architecture is approved by listing them. They should be developed together into
a coherent design problem, with direct observations chosen for each fault class.

1. **A live room.** Operator plus several differently configured models share
   work; programs stream output while participants communicate, arrive, leave,
   and change priorities. Determine when a message becomes visible and actionable,
   whose context contains it, and how conflicting workspace changes are handled.
   Include the operator's later
   [remote outpost example](../docs/FOUNDATION.md#outposts-working-environments-and-collaborating-peers):
   a participant receives selected context, works directly on a remote server,
   and chats with the main colleague while both remain active. Preserve environment
   attribution and distinguish this peer use from arcorefit's conversation handoff.
2. **A turn changed during work.** Redefine scheduling/continuation/context policy
   while model requests and programs are active. Determine the exact activation
   boundary and disposition of old frames, queued work, and captured definitions.
   Both operator and model should be able to perform and inspect the change.
3. **Managed compaction.** Compare candidate strategies while retaining useful
   large data, executable definitions, and ongoing work. Record the actual input,
   selected policy, output context, retained sources, and consequences. Attempt
   subsequent work that needs the material, rather than checking token count alone.
   Preserve the complete captured pre-transformation evidence and transformation
   inputs/outputs separately from active context. Any chosen context strategy must
   leave those originals available for study, including failed or abandoned work.
4. **Rebuild and inhabit again.** Include an operator connection, model context,
   current work, standing programs, mutable runtime state, and an audit stream.
   Apply the operator's subsequently supplied
   [arcorefit/outpost rules](../docs/FOUNDATION.md#refit-operational-rules-and-the-outpost-proposal):
   settle main-harness commands/provider requests and pause standing work before
   refit. Transfer the working conversation to an independent outpost, compile
   and observe there, then return to the rebuilt harness and resume preserved
   standing work. Independently managed OS daemons continue outside that pause
   scope. Observe actual quiescence, correct handoff and return, paused-program
   preservation, running version, failed-build recovery, and complete audit.
5. **Audit a messy run.** Combine streamed provider/program output, failures,
   interruption, compaction, reload, and a partial recording failure. Reconstruct
   the recorded work, governing code/configuration versions, and explicit gaps.
   Compare known emitted provider/program inputs and outputs with retained original
   bytes. Self-consistency of surviving records is not a completeness oracle;
   a gap marker does not make discarded captured material available again.
   Define the core's observation boundary so complete audit does not mean invented
   hidden provider state or unobserved external-world facts.
6. **One autodroit improvement.** Human and model jointly inspect a real failure,
   revise the responsible executable behavior, and evaluate task outcome and
   relevant costs under comparable conditions while work continues. Record the
   intervention and its point of effect. Treat changing the evaluator or audit
   as an experimental intervention too; it cannot silently redefine “better.”

Model ergonomics runs through all six: discovery, understandable errors, useful
result/ongoing-work identities, inspectable state, and expressive composition.
Counting calls or tokens alone cannot establish whether the model can work well.
The audit supplies study material; it does not independently prove causal benefit
from a change. Comparative experiments need direct outcomes appropriate to the
claim. Mutations remain fleet-only under Blackbird.

## Next work

The operator asked us to keep an occasional eye on Rhizome's possible C++ direction.
Read its [C++ rigor feasibility proposal](../../rhizome/papers/cpp-rigor-stack.md)
on 2026-09-30. It explicitly leaves language selection and stack feasibility open.
Its contract-first ownership/representation work, fault-specific checking, actual
toolchain trials, and direct semantic/persistence oracles are useful cross-project
leads for any compiled substrate we may select. Check substantive new findings at
useful design checkpoints. Rhizome's scope, performance objectives, and publishing
instructions do not become Arconaut requirements through this read-only reference.

Reconcile concrete operator examples with the foundation questions, then conduct
focused deep research and experiments that can distinguish these alternatives.
Write the integrated design and submit it to adversarial review. Only afterward
write and review the implementation plan and decide any adaptation from quarantine.
There is no reason to resume June phases or treat the old Rust partition as a
default. Beads tracks the brief, design/review, and planning; this reconnaissance
is its own bounded acquisition/research task.

Independent [synthesis review](2026-09-30-frumentarii-review.md) found one valid
audit-retention handoff omission. Integrated its correction into FOUNDATION and
the compaction/audit scenarios: preserve complete captured originals independently
of active context and use known emitted data as the completeness oracle. No other
substantive finding was raised within that review's stated scope.
