# Arconaut restart baseline

Assessed 2026-09-29; reconstruction disposition updated 2026-09-30.
Status: source assessment and research leads. Current reconstruction starts with
[operator intent and the foundation inquiry](FOUNDATION.md).
This is not an approved product specification or permission to resume June phases.

Arconaut is worth recovering as accumulated knowledge and selective implementation.
It is not a demonstrated working harness that merely needs its remaining backlog
finished. The main damage is the distance between names, documents, tests, and
actual end-to-end behavior. We found concrete defects and unsupported claims; this
assessment does not establish malicious tampering or exhaustively rule it out.

## Foundation and source

Blackbird is the current doctrine. Research informs design; adversarial review
challenges design and then the implementation plan. Direct oracles establish
bounded behavioral claims. Test counts, phase labels, and formal-looking documents
do not substitute for that chain of reasoning.

The public master was `76ddf00`. The live older-workspace checkout has 15 later
commits ending at `3bf2056`. This assessment starts from the later source on
`assessment/2026-09-29`, preserving the earlier history. The original large ZIP
records the same master but was not exhaustively compared. The untracked `lemonnotes.md`
has also been preserved. [Sources and restoration](../QUARANTINE.md) describe
the intact bundle, archive, original checkout, and clean reference corpus.

The operator has directed the entire inherited implementation into ignored
`quarantine/arconaut/`. Its code, Cargo/build/lint configuration, June documents,
historical instructions, old issue records, and CI are preserved in their original
source layout. The active root holds doctrine, current research and reconstruction
documents, utilities, journal, and issue tracking. No legacy module has been
selected for the new implementation. No product code was patched.

## What the evidence establishes

The independent [architecture report](../papers/2026-09-29-architecture-assessment.md)
traces actual CLI wiring and gives file/line references for each mechanism. The
[oracle report](../papers/2026-09-29-oracle-assessment.md) records execution and its
limits. The [research report](../papers/2026-09-29-research-assessment.md) revisits
the sources behind the design claims.

| Claim or surface | Assessed reality | Consequence for the restart |
| --- | --- | --- |
| Multi-provider coding loop | Anthropic and compatible URL construction omits a slash. Outbound conversions lose tool-call/result identity; compatible responses do not parse tool calls. Gemini chat is a stub. | Provider count is not an established capability. Define and exercise a complete conversation round trip. |
| Agent terminal and interruption | The task awaiting the turn must also service the terminal tool's reply. Interrupt is handled only after the awaited turn and sends a status string. | Model work, tool work, UI input, and cancellation need an explicit ownership design. |
| Deduplication | The old loop caches all tools by name/arguments within a turn, without resource-version invalidation. | Read → write → same read and test → edit → same test can return stale results. Repetition detection and effect reuse must be separate decisions. |
| Context management | Tool-result text is not counted. Clear followed by revert can restore a token count with no messages. The optional compactor replaces old content with a count-only label and is not wired into the CLI. | Context size, continuity, and recoverability need real contracts; enabling the compactor would not solve them. |
| Sessions and modes | Session is constructed but unused; both modes of CLI entry ignore agent mode while registering write/edit/bash. | Names do not establish persistence or tool authority. Specify meanings before retaining these options. |
| Successful headless work | Provider-construction failure can select a mock returning `done`; turn errors are printed and then return success. | Completion, configuration failure, provider failure, and demonstration mode need distinct observable outcomes. |
| Pilot/reactor replacement | `run_turn` and `continue_turn` are unimplemented; optional demonstration wiring discards the error. Runtime prototypes exist independently. | The new architecture is a proposal with useful experiments, not a completed migration. |
| Research-backed closed choices | Several real studies have been extrapolated beyond their evaluated tasks; hosted prompt caching was dismissed incorrectly; some proposed controls require inference-server access. | Reopen choices around the actual deployment and operator workflow. |

For the inherited source, on the installed Rust 1.89.0 toolchain,
the locked whole-workspace check including
all targets/features passed. Seventy selected core, audit, and provider tests
passed, yet direct
probes of the unmodified code found the context counterexamples above. This is
evidence that the code compiles and some local properties hold, not that the
harness works. See the oracle report for exact commands and additional checks.
No live provider, operator credentials, or remote fleet was used; no mutants ran.

## Disposition now

The table identifies research leads from the assessment. Every implementation
listed is now quarantined. Reuse requires the reviewed new foundation and its
implementation plan; none has current implementation standing.

| Disposition | Material | Basis |
| --- | --- | --- |
| Study as possible primitives | Message/content/tool identities, Tool and ChatProvider seams, ordered registry, scoped variable precedence, selected file/skill mechanisms, ordinary JSONL logging | Small, legible pieces with useful boundaries. Any selected implementation must satisfy the new contract, including error behavior. |
| Carry forward as intent to discuss | Agent-facing environment, less mechanical work for the model, observable execution, operator steering, continuity across long work | Strong themes in the notes, but the original operator conversation is unavailable. Confirm priorities without treating model-written “decided” labels as authorization. |
| Preserve as experiments | Neovim RPC/editing, brush shell, SSH tools, TUI widgets, pilot/reactor boundary | Real work whose fitness requires comparison with alternatives on the intended tasks. No mandatory editor backend or UI is selected. |
| Redesign before reuse | Provider codecs/auth resolution, turn and cancellation ownership, context accounting/reduction, effects and caching, persistence and recovery | Defects cross component boundaries. Patching them one by one would silently accept an unreviewed architecture. |
| Historical evidence only | June phase plan/completion claims, old issue backlog/instructions/CI, count-only compaction semantics, unsupported compression/prefetch benefits, unconnected corpus/eval/multi-agent promises | Preserve for learning and provenance. They do not define the restart roadmap. |

Rust itself is an available implementation base, not evidence that the present
ten-crate partition is right. Likewise, losing a particular prototype is not
losing the research question that motivated it.

## Operator intent and design problem

On 2026-09-30 the operator specified a coding agent designed from the beginning
for advanced technical use: mixing paradigms, unusual experiments including
multi-model IRC-style live collaboration, deeper integration with ordinary
computing tools and programs, and almost no interest in routine command approvals.
[FOUNDATION.md](FOUNDATION.md) preserves the statement and proposed implications.
This replaces the earlier generic workflow hypothesis as current design input.
The same discussion added expressive workflows for human and model, model
ergonomics, hot turn redefinition, managed compaction, comprehensive core audit,
hot reload, and autodroit self-improvement. Language and rebuild/continuity
constraints are recorded in FOUNDATION; the inherited Rust code selects no language.

Develop representative scenarios for ordinary coding and live collaboration,
including changing coordination styles and driving existing programs. Make the
integrated design explain them, including concurrent activity, failures, steering,
and recovery. A single serial workflow cannot stand in for this ambition. Model
access, particular interfaces, deployment, and coordination mechanisms remain
design questions.

Proposed invariants to pressure-test in that design:

1. Requested, observed completed, failed, and outcome-unknown effects remain
   distinguishable and correlated with their requests. A crash after an external
   effect but before recording its result can leave the outcome unknown. Retry
   and reconciliation policy must handle that uncertainty explicitly; a model
   retry must not silently duplicate or erase an effect.
2. Repeated observation can see a changed workspace. Cached answers are reused
   only under a declared validity rule.
3. Operator steering and cancellation have specified behavior while provider,
   shell, and tool work are in flight; the UI does not announce unperformed actions.
4. Context construction accounts for the complete request and preserves required
   instructions and call/result relationships across reduction. Durable records
   are distinguishable from the model's current context.
5. Resume and completion have explicit meanings. Process exit, interrupted side
   effects, model errors, and missing configuration cannot masquerade as success.
6. Standing operator authority permits routine execution without repeated approval
   requests. Any configured execution boundary has specified, truthful behavior;
   visibility and steering remain useful while autonomous work proceeds.
7. Concurrent participants, messages, observations, and running work retain their
   identities. Shared workspace changes and independent model context do not
   silently become one another; switching coordination style has defined effects
   on work already in progress.

These are questions for a behavioral specification, not claims satisfied today.
Recovery semantics, cancellation boundaries, and conversation/effect transitions
are candidates for small formal models when those models expose useful faults.
Formalizing labels or duplicating executable assertions adds no value.

## Research and design sequence

The stated operator intent informs the foundation and harness studies together.
Recovering historical time work is a provenance inquiry, not a prerequisite for
investigating live collaboration or computing integration. The studies below are
research directions, not an implementation plan.
Fresh frumentarii are investigating standing agent state, programmable execution
and workflows, systems/runtime continuity, and evidence-driven self-improvement.
References written in distrusted languages may teach mechanisms without becoming
candidate implementation substrates. Audit, hot redefinition, and model ergonomics
are questions across these studies, not optional embellishments added after a loop.
Beads owns task status and dependencies; the sequence below explains the reasoning.

**Operator contract.** Develop concrete examples from the stated intent: ordinary
repository work, autonomous program execution, and live collaboration with several
models and the operator. Include changing coordination styles during work. Identify
failures worth preventing, needed interfaces, and what surviving interruption means.
Preserve uncertainty where unanswered; do not ask the operator to reapprove the
goal already supplied.

**Independent conceptual studies.** They can proceed in parallel once their
shared operator assumptions are explicit:

| Study | Compare and read | Direct experiment / deciding evidence |
| --- | --- | --- |
| Live collaboration and composition | Primary material on concurrent conversations, messaging, model coordination, and programmable agent environments; exact implementations with inspectable behavior | Several participants with individual contexts exchange messages and run work concurrently. Examine late arrivals, workspace changes, selective interruption, and a change in coordination style. Check attribution, visibility, and defined handling of in-flight work. |
| Conversation, effects, and recovery | Actual provider protocols; the small mini-swe-agent loop; event/session designs that preserve identities | A scripted provider and temporary repository drive read → edit → test → interruption → resume. Assert resulting files, request/result identity, failure status, and which effects occurred. |
| Execution and steering | Existing terminal wait cycle; brush embedding; OS shell/PTY behavior; process and task ownership | Long output, delayed completion, stdin, nonzero exit, cancellation, and child cleanup under a bounded local process. Check command attribution and responsiveness. Use a PTY only for workflows that require terminal behavior. |
| Editing | CodeStruct and plain editing baselines; Neovim parser/extmark limits | Paired representative edits, including syntax errors, concurrent changes, unsupported language constructs, and stale references. Check intended edit target, accepted result, and failure recovery, with cost and latency. |
| Context reduction | AgentDiet, hosted caching constraints, and deterministic selection baselines | Put required constraints and tool-call/result pairs around a reduction boundary. Check their specified preservation/recoverability, request budget, task outcome, total cost, and cache reuse. |
| Behavioral assistance, if selected | OpenCode's concrete repeated-tool handling and existing intervention logic | Labeled trace replay evaluates detection and false warnings. Any claim that acting on an intervention helps requires comparative live task runs, because intervention changes subsequent actions. |
| Retrieval memory, if selected | Ordinary session records and lexical retrieval before embedding/index choices | Retrieve independently selected answers with the correct source/revision; surface stale conflicts. Compare utility and rebuild burden before selecting multiple storage engines. Durable recovery semantics belong to the conversation/effect study. |

For experiments, state the competing hypotheses and what result would change the
decision before writing machinery. Hold repository/task/model configuration
comparable; repeated stochastic runs are warranted only where they change the
inference. Cost includes auxiliary calls and lost cache reuse. Wall time includes
waiting and retries. A token reduction alone is not improved task performance.
Use primary literature and exact reference revisions; preserve intermediate
reports in `papers/` and acquire missing selected sources into the shopping list.

**Integrated design and adversarial review.** Write the complete behavior and
ownership model with explicit alternatives rejected for reasons. Review against
normal use, provider/tool failure, operator interruption, context reduction,
restart, conflicting edits, and an environment different from the author's.
Resolve findings in the design, then map individual legacy components to it.

**Implementation plan and its review.** Only after that design is coherent,
write dependency-ordered conceptual units and their direct oracles. The plan must
say which legacy pieces are adapted, replaced, or physically archived, and how
the supported modes compose into usable execution with explicit ownership.
Challenge the plan independently before product coding.

**Implementation.** Follow Blackbird's written sub-plan, red oracle, implementation,
green oracle, adversarial code review, and finding-resolution cycle. Use fault
classes to select testing forms; mutations belong on the fleet. Do not resurrect
the June roadmap merely because a file contains an unfinished TODO.

## Limits and remaining decisions

This pass is a source and bounded-execution assessment, not a complete security
audit, current compatibility matrix, fleet evaluation, or finished research
program. It establishes enough to reject blind continuation while identifying
valuable material. The expert-operator purpose, paradigm composition, live
multi-model ambition, computing integration, and low approval friction are now
stated. Concrete collaboration examples and foundation semantics remain to be
developed; no provider, Neovim mandate, storage engine, multi-agent topology, or
final UI has been selected. The clarified purpose changes the research emphasis
without invalidating the source findings or requiring this assessment to be rerun.

Operator recollection on 2026-09-30: the last remembered work was time and
foundation pieces; tool execution and provider implementations were surprising.
Their presence in the June Git history is established, while their relationship
to the intended stopping point and any approval remains unresolved. This makes
the foundation boundary an explicit provenance/design question. Candidate reuse
above does not confer approval on those higher-layer implementations.
