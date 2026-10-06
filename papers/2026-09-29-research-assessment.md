# Research and design reassessment — 2026-09-29

The inherited research contains useful hypotheses and substantial reference
material. It does not establish the June architecture as the right restart.
The most consequential weakness is the conversion of limited evidence into
implementation commitments: a result about structured editing becomes mandatory
Neovim; a trajectory-compression result becomes an inference-stack roadmap;
observed behavior becomes an uncalibrated intervention rule. Preserve the ideas,
reopen the choices, and make the next research answer specific design questions.

This is an independent assessment of the documents and selected reference
sources, not a new literature survey or an implementation audit. No product code,
operator credentials, paid inference, reference checkout, Git state, or issue
database was changed by this assessment. Current instructions came from
`AGENTS.md`, Blackbird, README, and the journal. Inherited instructions were read
only as historical evidence.

Source location updated 2026-09-30: the inherited implementation and its June
documents now live in `quarantine/arconaut/`. Links below point into that intact
reference. Current reconstruction begins with time and foundation design.

## Intent and authority

The direct current operator evidence is the request to restart a personal coding
harness, critically assess the inherited base, revisit research/design/planning,
and decide what remains active. Blackbird independently supplies the working
method: read the sources, design before implementation, adversarially review
design and plan, and choose direct oracles for real claims.

The June documents contain a coherent possible product direction: an agent with
good tools and recovery, long-running work, persistent interactive processes,
remote execution, accessible research, and operator steering. These are valuable
interview material. They are not newly ratified requirements. In particular:

| Evidence | What it establishes | What it does not establish |
| --- | --- | --- |
| [Architecture](../quarantine/arconaut/docs/architecture.md), §§1–2 | A recorded vision of a personal, Rust, Ghostty-oriented harness with model ergonomics prioritized | That Rust, Ghostty, eight crates, IRC, or the stated permission policy remain current choices |
| [P5.7 notes](../quarantine/arconaut/docs/P5.7-notes.md), §§1, 3–4 | Recorded emphatic choices for Brush, Neovim-only editing, and an agent-controlled terminal | Who approved each choice, the alternatives evaluated, or whether its underlying problem still exists |
| [Master pre-design](../quarantine/arconaut/docs/P5.7-master-pre-design.md), §§5–7 | A synthesis and a proposed sequence, with several questions marked closed | A demonstrated compression stack, calibrated heuristics, or a validated editing/runtime boundary |
| [Phase A](../quarantine/arconaut/docs/P5.7-phase-a-spec.md) and [Phase B](../quarantine/arconaut/docs/P5.7-phase-b-spec.md) | Concrete interfaces, examples, and conformance intentions | Independent evidence that the proposed interfaces satisfy the operator's workflows; the documents combine specification, conformance, and implementation plan |
| [Lemonade notes](../quarantine/arconaut/lemonnotes.md) | An otherwise untracked storage proposal preserved from the source checkout | An approved storage design, tested integration, or authenticated operator rationale |

No conversation transcript or attributed approval record was found in these
documents for the individual commitments. That is a limit on attribution, not
evidence that the operator never approved them. Equally, emphatic prose such as
“Decision,” “CRITICAL,” and “Closed” cannot supply missing evidence. A Git author
would establish who committed text, not who originated or accepted a requirement.

## Corpus and provenance

The read-only legacy reference root is
`/Users/patrickbeam/projects_old/miscellaneous/arconaut/quarantine/`.
It contains **53 immediate directories**, of which **52 retain root `.git`
metadata**, and a root `shell_execution_survey.md`. No root restoration manifest
was present. Existing metadata is useful for recovering revisions before clean
ingestion; do not strip or modify the preserved original.

The directory inventory is:

```text
Figment OxideAgent PolarisDB SWE-bench arbitrary bashkit beads brush-shell
cargo-fuzz cargo-mutants chat-system claude-code-unofficial claudex-pattern
cline codex continue criterion.rs devika flamegraph gascity gastown gemini-cli
goose gpt-engineer halloy hyperfine kimi-cli markdown2pdf metagpt mini-swe-agent
oh-my-pi opencode opendev openhands pgvector pi pi-agent-rust plandex printpdf
proptest qdrant quickcheck qwen-code ratatui roo-code rust-bash swe-agent sweep
tabby tarpc tonic trae-agent usearch
```

The inherited [agent survey](../quarantine/arconaut/docs/quarantine-survey.md) says it surveyed 24
repositories; that is a survey scope, not a complete inventory. Its paths are
useful reading leads, but performance numbers and judgments still need their
original benchmark conditions. The [testing survey](../quarantine/arconaut/docs/quarantine-testing-survey-report.md)
is another historical lead, not certification of those repositories or Arconaut.

The [TUI literature catalog](../quarantine/arconaut/docs/tui-research/literature-catalog.md) and
[TUI quarantine index](../quarantine/arconaut/docs/tui-research/quarantine-index.md) explicitly target
`rocket_surgeon`, a neural-network debugger. Several listed reference trees are
absent from this Arconaut quarantine. This is identifiable cross-project
provenance, not inherently bad material; debugger/tensor visualization findings
must not silently become requirements for a coding harness.

The three reports named by the master pre-design were absent from Arconaut's
inherited non-quarantine Markdown, but were recovered during this assessment
from `projects_old/neurotic_library/outposts` and preserved unchanged:

- [Token-efficiency synthesis](history/llm_token_efficiency_comprehensive_synthesis.md):
  §7.3 explicitly identifies hosted Anthropic/OpenAI prompt caching, and §9 leaves
  the combined optimization stack open. Its appendix points to a 55-paper intake
  manifest and master bibliography; those underlying acquisition artifacts were
  not inspected in this bounded assessment.
- [BufferManager bibliography](history/arconaut_p5_7_buffer_manager_bibliography.md):
  identifies seven papers and acquisition sites. It distinguishes file-level
  prediction from hardware-cache prediction, but also drifts into database
  buffers and per-transaction working sets. Its 94% sentence is now traceable to
  an intermediate synthesis, not yet to the original experiment.
- [Memory landscape](history/llm_memory_systems_landscape_report.md): recommends
  starting with files and introducing retrieval when needed; distinguishes
  existing deterministic project state from episodic discoveries and decisions.
  This is a simpler historical alternative to the later storage proposal, not
  independent validation of its own benchmark table or product recommendations.

The recovered passages make some design drift directly inspectable: the
pre-design contradicts or closes questions its own sources kept open. The
[librarian letter](../quarantine/arconaut/LETTER_FROM_LIBRARIAN_TO_ARCONAUT.md) remains
useful provenance, but its “shipped” status does not establish today's library
availability. Recovery of a synthesis does not verify all its citations.

The shared wiki has an archive identity and restoration instructions in
[`quarantine_proj/MANIFEST.md`](../../quarantine_proj/MANIFEST.md). Its
[verification essay](../../quarantine_proj/xx_wiki/src/content/docs/ai-software-dev/verification.md)
is useful context for oracle strength, independence, and construct validity.
It is historical material. Where its treatment of second-order metrics differs
from today's Blackbird, Blackbird controls.

Selected reference snapshots actually inspected:

| Reference and recorded HEAD | Reading performed | Useful lesson |
| --- | --- | --- |
| OpenCode `2006259a02a87edf9e37f253cbddf3188309026b` | `packages/opencode/src/session/processor.ts`, especially 525–550 | Repeated tool-name/argument detection exists in an open implementation; understand its trigger and permission response rather than calling the entire area novel |
| mini-swe-agent `2afd0fb81bacbf0aacfac9ded6f093c5acd0bf7c` | `src/minisweagent/agents/default.py`, especially 87–124 | A small query/execute loop, step persistence, and explicit limits provide a useful complexity baseline; this is not an endorsement of its sufficiency for interactive work |
| Brush `f84ae8a74c43cad85824477d67ab3da5de972558` | README capability and embedding description | Embedding `brush_core::Shell` is a real option; shell compatibility and operator workflow still require a comparison |
| USearch `9fd6b0115dcdd0016e037fb5335f36824dd02090` | README serialization, disk views, and filtering sections | A persisted/mapped approximate-neighbor index is real; integration and recovery claims must be tested separately |

These are snapshot findings, not statements about each project's latest HEAD.
OpenCode, mini-swe-agent, and Brush now have clean active copies under
`quarantine/`; [QUARANTINE.md](../QUARANTINE.md) records their restoration.
USearch was inspected in the untouched legacy checkout. No wholesale corpus
ingestion is needed for these findings.

## Consequential claims checked against primary sources

Web sources below were read on 2026-09-29. Numeric claims are bounded by the cited
experiment, not projected onto Arconaut.

### Structured tools are promising; mandatory Neovim is unproven

The real CodeStruct paper reports 76–88% fewer **tool-level edit errors** for
three capable models, not a general reduction in agent failure. Its token savings
are model-dependent; GPT-5-nano's tool errors and costs increase while task success
improves. SWE-bench evaluation is Python, and the limitations identify initially
invalid syntax and extension to complex statically typed languages as gaps.
The paper supplies structured read/edit primitives, not a Neovim requirement.
The inherited universal “50% failure” description loses the denominator and model
conditions. The research supports a Rust-relevant comparison of interfaces; it
does not justify deleting every alternative editing path.
[CodeStruct, results and limitations](https://arxiv.org/html/2604.05407v1).

The implementation substrate also has narrower guarantees than the notes imply.
Neovim extmarks track buffer positions with configurable gravity; ranges can
become invalid on deletion. They do not establish permanent semantic identity
across arbitrary rewrites. Language parsers must be available and loadable;
bundled tree-sitter support is not automatic parser coverage for every project.
[Neovim extmark API](https://neovim.io/doc/user/api/#nvim_buf_set_extmark()),
[Neovim parser loading](https://neovim.io/doc/user/treesitter/#treesitter-parsers).

Consequently, contextual marker persistence is a plausible design hypothesis.
The proposed ±10-line search, five-second debounce, and rename recovery are
unvalidated policy choices. UUID uniqueness would not prevent an anchor from
reattaching to the wrong duplicate function. Success must mean the intended
target or an explicit unresolved result, not merely a recreated mark.

### Compression has real evidence, but the integration claims are overstated

AgentDiet reports 39.9–59.7% lower accumulated input tokens in its two-model,
two-benchmark evaluation. Net cost reduction is 21.1–35.9% after reflection
overhead, with measured task success differences of −1 to +2 percentage points.
The paper explicitly accounts for invalidated KV caches. These are credible
results for a candidate technique, not proof that the same proportion of any
Arconaut history is disposable or that fewer prompt tokens always save money.
[AgentDiet, §5 and Table 4](https://arxiv.org/html/2509.23586v1).

The master pre-design's claim that prefix caching applies only to self-hosted
inference is false. It also contradicts §7.3 of its own recovered
[token-efficiency source](history/llm_token_efficiency_comprehensive_synthesis.md),
which names hosted prompt caching. Anthropic documents hosted API prompt caching, reusable
prefixes, cache invalidation, and separate usage accounting. It therefore belongs
in the design comparison for a hosted harness. Rewriting old history can disrupt
prefix reuse; measure cache reads/writes, total spend, and task outcome together.
[Anthropic prompt caching](https://platform.claude.com/docs/en/build-with-claude/prompt-caching).

StreamingLLM's long-stream result concerns inference with a limited attention
window. Its authors explicitly explain that it does not expand the accessible
context window or retain all past tokens. It is not a substitute for remembering
a million-token coding session. Implementing it requires control of inference,
not just rearranging a hosted API request.
[StreamingLLM author implementation and FAQ](https://github.com/mit-han-lab/streaming-llm).

The inherited compression report calls the full three-layer combination unproven;
the recovered source synthesis also lists optimal composition as an open question.
The master pre-design nevertheless marks the stack question closed and plans
toggles before validation. LLMLingua, LongCodeZip, SWE-Pruner, EAGLE, and the
claimed combined gains were **not** independently revalidated in this bounded
assessment. Their presence in a menu or plan would establish no benefit.

### Behavioral observations do not calibrate intervention rules

The cited Mehtiyev–Assunção study is real. It analyzes 9,374 trajectories and finds
that the apparent relationship between length and failure reverses after
controlling for difficulty. Context gathering and validation correlate with
success; model capability also explains much of the variation. This does not
establish that requiring two reads before an edit or interrupting on a fourth
identical call improves outcomes.
[Beyond Resolution Rates](https://arxiv.org/abs/2604.02547).

The inherited claim that no open framework has real-time behavioral detection
is contradicted by the captured OpenCode source: it compares recent tool calls
and requests `doom_loop` permission on repetition. This demonstrates prior art,
not a validated false-positive rate. Polling, retries after transient errors,
re-reading changed files, and deliberately repeated tests are counterexamples to
treating repeated arguments as sufficient evidence of being stuck.
[Inspected OpenCode revision](https://github.com/anomalyco/opencode/blob/2006259a02a87edf9e37f253cbddf3188309026b/packages/opencode/src/session/processor.ts#L525).

### The storage idea is plausible; the proposed composition is not established

`lemonnotes.md` usefully separates durable events, context snapshots, and derived
search state. Its statement that LanceDB sits on `active.hnsw` from USearch is
unsupported. LanceDB's documented interface creates indexes on table columns
using its supported index types; the inspected API does not document mounting an
arbitrary USearch file. This is an unverified integration assumption, not a proof
that no custom bridge could be built.
[LanceDB index API](https://docs.lancedb.com/api-reference/index/create-index).

A single `apply_op` entry point is helpful discipline, but does not by itself make
recovery correct. The note does not establish an atomic boundary among event
append, context snapshot, external tool effect, and vector update. Nor does it
define reproducibility under changed embeddings/index parameters. The inherited
“clear choice” judgment conflates an ANN proximity graph with semantic graph
requirements. A user biography is also insufficient reason to adopt CloudTrail's
event schema. Decide what must be recoverable and queryable before selecting
three storage mechanisms.

### Other quantitative claims remain hypotheses

The claim that a first-order Markov model captures 94% of predictable file access
is now traced to §2 of the recovered
[BufferManager synthesis](history/arconaut_p5_7_buffer_manager_bibliography.md),
which instead attributes 94% of file accesses following the previous access to
Griffioen–Appleton. The original workload, denominator, and experiment remain
unchecked; the pre-design additionally folds this together with a separate
hardware Markov-predictor result. Filesystem/hardware prefetch results cannot
establish a win for model-directed repository reading. The 200K context target,
tempo token-cost table, two-week RPC
rewrite commitment, and proactive-tool thresholds similarly lack local Arconaut
measurements in the inspected reports. They may be useful starting parameters;
they are not research conclusions.

## What is worth carrying forward

Carry forward problems and mechanisms whose purpose remains legible:

- Structured access to code, precise edit failure feedback, and explicit stale
  target handling can reduce mechanical work for the model.
- A visible, persistent process session can support tasks that a one-shot shell
  cannot; its value depends on the chosen workflows.
- Full observations preserved separately from bounded model context support
  inspection, selective rereading, and recovery after compression.
- Provider-neutral lifecycle events and cancellation can give UI, audit, and
  operator steering a common account of what happened.
- Literature retrieval and session memory are plausible tools when their results
  identify original sources and distinguish stale material from current intent.

These are candidate design properties, not a mandate for Neovim, Brush, HNSW,
IRC, an autonomous heuristic engine, or a particular crate structure. Retire the
June phase sequence and unqualified quantitative projections as authority; retain
them as historical proposals with sources and limitations attached.

## Experiments that can actually choose a design

The next unit of research should resolve a disputed choice using the smallest
experiment with a direct oracle. This section is a research agenda, not a second
task tracker or an authorization to start implementation. Select the first
experiments after the operator's concrete workflows are captured.

| Design question | Comparison or fault experiment | Direct oracle and decision value |
| --- | --- | --- |
| What should the first useful harness do? | Describe several actual sessions: repo exploration, planned implementation, interruption/resume, remote interactive work; identify what failed in the previous harness | Operator-defined completion and unacceptable failure examples establish the construct being measured; benchmark pass rate alone cannot choose the product |
| Does structural editing help the intended model and Rust work? | Same tasks/model/budget with ordinary bounded reads/patches, structured reads alone, then structured edits; separately exercise malformed files, macros, duplicate names, and external edits | Independently specified resulting source behavior plus edit-target correctness; count wrong edits, recovery attempts, cost, and latency. This separates retrieval benefit from editor-process benefit |
| Is a persistent embedded shell worth its state? | Compare a small persistent PTY session and embedded Brush on required workflows: cwd/env persistence, foreground/background jobs, interactive input, interrupt, reconnect | Expected process state, output boundaries, exit status, and absence of unintended commands after cancellation. Include output containing terminal-control bytes. Compatibility, ownership, and interruption decide the boundary |
| Which context policy helps hosted work? | Stable uncompressed prefix baseline versus deterministic observation selection versus one AgentDiet-style policy; reuse tasks and fixed tool semantics | Task outcomes and recovery of earlier constraints, together with cache reads/writes, total billed cost, latency, and peak context. Count reflection costs. Avoid multiplying separate papers' savings |
| What should survive a crash or resumed task? | Define one accepted recovery story, then interrupt a disposable session at append/write/tool-result/snapshot boundaries; edit a file externally before resume | Recovered state matches the defined acknowledged prefix; unknown external effects remain unknown and are not replayed blindly. Only then choose log/snapshot/index implementation |
| Is semantic memory useful at this corpus size? | Compare exact identifier/lexical retrieval and simple stored summaries against one embedding candidate using independently selected answerable questions | Correct relevant source and revision retrieved, stale/conflicting material exposed, retrieval latency and rebuild time measured. ANN construction is justified only if a simpler retrieval baseline loses materially |
| Can interventions help without interrupting good work? | Label representative successful and failed traces; run rules in observation-only mode, including polling/retry counterexamples; test interventions only after discrimination is understood | False interruption rate, missed failures, useful warning lead time, and eventually task completion. Repetition detection alone is not the outcome |
| Does prefetch address an actual bottleneck? | First measure cold/warm file and parse latency on the selected workflow, then replay file-access traces against on-demand versus one simple predictor | End-to-end waiting time saved, extra I/O, memory use, and stale-buffer incidents. No predictor is warranted if the delay is immaterial beside inference |

Each eventual experiment needs its own declared fault class and outcome oracle.
Use offline fixtures and disposable workspaces for deterministic mechanisms;
model comparisons require an explicit model/workload budget. A changed prompt
changes future actions, so replaying an old trajectory alone cannot establish
end-to-end task success. Small paired studies can reject bad ideas quickly, but
need repeated runs before claiming stable marginal gains.

After these results and operator choices establish a design, adversarial review
should challenge its untested assumptions and failure boundaries. Then construct
and review the implementation plan. The inherited implementation can supply
components to that design; it should not determine the design by inertia.
