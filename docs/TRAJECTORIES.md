# Instrumentation, evaluations and trajectories

Operator authorized this campaign after provider-auth integration on2026-10-09.
Own the implementation. No telemetry SDK, evaluation framework or Entire runtime
is adopted. Start from the causal audit we already retain.

## One connected working surface

A trajectory is a bounded view of actual work: decisions, invocations, admitted
attempts, dispatch, original captures, observations, context/program changes,
participants and later evaluations. Journal/sequence and existing causal IDs link
these facts. The view never infers success from admission or rewrites an unknown
outcome because a later request succeeded. Inspect original bytes on demand.

Use the same native query for operator, model and Lua. The first `/trace` surface
is a readable timeline; later an inspect pane can select a turn, expand its
requests/tools/participants and show timing, usage, context changes and evaluations.
The task pane stays below the sprite. A trace's pinned end and next cursor make
freshness explicit; current and historical observations must remain distinguishable.

Operational timing belongs in ordinary retained metadata: monotonic durations,
process incarnation, observed wall time, requested/actual model, reported usage,
retry grouping, original/result references and missing fields explicitly absent.
Keep CPU/allocation profiling behind the existing developer switch. Do not infer
cost from tokens without provider/price/version information; no estimated billing
default. Do not infer total order or causation from timestamps.

An evaluation names a specific candidate, cases, evaluator definition and version,
input references, execution attempts and raw results. Preserve per-case results,
scores/labels, errors, unknowns and evaluator explanation separately from the
selection decision. Models may write evaluators and annotations; deterministic
checks and model judgments retain their different meanings. Re-evaluation is a
new run. Comparisons show denominators, unrun/error cases and actual environment.
The first useful interface should evaluate selected retained trajectories with an
explicit bounded Lua evaluator, then compare two named runs. No automatic scoring,
promotion, self-replay or model-judge ceremony.

## Trajectories correlated with central variables

Operator correction supersedes the prior Entire-derived abstraction. The product
is trajectories plotted against time-indexed central variables. There is no
capture/restore object organizing this campaign. Start with commit history and
active central doctrine; keep the variable interface extensible without defining
all variables up front.

A trajectory describes observed work and its causal links. A variable supplies
observations or changes with source identity, value/version, time information and
availability. Join these for aligned plots, comparisons and bounded queries.
Variables may be categorical (commit, doctrine identity) or numeric (duration,
reported usage, evaluation result). Preserve source locators so a plotted value
can be inspected. Do not duplicate original payloads into chart data.

Use a shared time axis with explicit clock domains. Pair local monotonic readings
with UTC wall-time observations and process/host identity; retain alignment
information, measured offsets/uncertainty when available, and clock adjustments.
Monotonic clocks order/duration work within their domain; they are not directly
comparable across processes/hosts. Show uncertainty or missing alignment rather
than drawing falsely precise overlaps. Use existing causal links where available;
clock proximity supplies correlation, not causation. Clock synchronization and
its observability are an owning implementation question, not assumed established.

Commit history is one variable series: retain original author/committer times,
repository/worktree identity, and separately the observed time a commit became
HEAD in the working environment. Commit timestamps alone do not say when that
code governed a run. Loaded executable/program identity is another possible
series, distinct from checkout HEAD. Uncommitted changes and external writers
remain explicit limits on any code attribution.

**Central doctrine** replaces "system prompt" as the product concept. Observe
which doctrine content/version was effective for work and link that observation
to the retained request representation. An observation of effective doctrine
introduces no authoring tool, routine revision cadence or turn-boundary activation
rule. Doctrine can remain unchanged over many trajectories. Scope, composition,
sharing and change authority remain open design questions.

The variable interface is about observing and aligning values, not owning their
lifecycles. For state-valued series, record effective intervals only when their
boundaries are observed; point samples leave uncertainty between samples. Do not
silently fill gaps or infer a change happened at the time it was discovered.

The integrated view should let the operator align trajectory events with these
variable lanes, select an interval or value, compare associated runs and outcomes,
and drill into source material. This gives correlative muscle: discover patterns,
form hypotheses and choose explicit evaluations. An association does not establish
that changing the variable caused an improvement. File restoration, Git commits,
doctrine editing and publication are independent operations, outside this surface.

## Grounding and design review

Existing `src/audit.cpp` provides stable fact indices, pinned prefixes and bounded
original-byte inspection. `CodingEngine::operation` admits before dispatch and
retains results plus terminal dispositions. Provider stream diagnostic files expire
after30days. `LocalTimingSink` is developer-only. These are useful starting points;
they do not yet supply a complete trajectory UI or integrated evaluator.

Entire CLI atb4d2443bf18e links sessions/checkpoints to commits, preserves separate
ephemeral and persistent checkpoints, and now supports independently addressable
checkpoint refs. Its transcript sanitization/redaction/export machinery shows why
our private audit cannot simply become a pushed Git transcript. Inspected
`docs/architecture/sessions-and-checkpoints.md`, `ref-checkpoint-backend.md`,
`cmd/entire/cli/checkpoint/checkpoint.go` and `persistent.go`. Pinned archive
and restoration are in QUARANTINE. We should improve integration through exact
native causal references and reusable local evaluations, rather than duplicate
our audit into a second authoritative transcript.

GEPA at3f160c295000, `src/gepa/core/adapter.py`, separates outputs, optional
trajectories, scores and objective scores. Its one-result-per-case relationship
is useful; evaluator scores still depend on evaluator quality. Existing study:
[evaluation mechanisms](../papers/capabilities/studies/experiment-evaluation.md).

OpenTelemetry's current GenAI conventions separate model, agent/workflow and tool
spans and evaluation events; fields distinguish requested model from observed
response identity. They remain in development. Study mapping/export as an adapter,
without making evolving external names our retained schema. Primary sources:
[agent spans](https://github.com/open-telemetry/semantic-conventions-genai/blob/main/docs/gen-ai/gen-ai-agent-spans.md),
[evaluation events](https://github.com/open-telemetry/semantic-conventions-genai/blob/main/docs/gen-ai/gen-ai-events.md).

Source-driven challenges before implementation: a filtered page must bound examined
facts, not only returned matches; a pinned prefix must not borrow later outcomes;
metadata views must not read enormous originals; query operations must not create
an automatic self-observation loop; expired diagnostics must remain visibly
unavailable; a Git attribution heuristic must not masquerade as observed causation.
First slice resolves the first four through a stateless paged metadata reader and
explicit source drill-down. Later units own diagnostic/export and time-aligned variable semantics.

## Implementation sequence

1. Read-only trajectory timeline over existing committed audit; `/trace`, model
   `trajectory_read` and Lua `blackbird.call` share it. No new capture format.
2. Review existing capture/disposition paths and fill concrete operational metadata
   gaps: timing, usage, model identity, parent work/participant links and dropped
   observations. Define a finite capture matrix before changing persistence.
3. Retained evaluation definitions/runs, case results, comparison and annotation
   interfaces over explicit selected inputs. Exercise with useful real failures.
4. Time-indexed variable interface and aligned trajectory plots, starting with
   commit history and observed effective doctrine. Preserve clock alignment,
   uncertainty, provenance and the distinction between association and causation.
5. Integrated inspect/evaluate UI and optional interoperable trace export.

First slice allowance45min including at most15min hardening/two layers. Direct
oracles: exact event order/links, admission versus terminal unknown, filtered
scan/page bounds, frozen-prefix stability after append/reopen, no oversized cold
payload reads, rejected malformed selectors, shared Lua/operator/model dispatch,
and real CLI output without provider traffic. No old startup benchmark or live
provider probe. Broader campaign is sequenced work, not a promise to finish all
units inside this first allowance.

## First slice delivery

Implemented on candidate branch `trajectories`. Direct checks passed on Mac debug
and release, plus Linux debug (context/linux/run-mtrl3_4p). The native fixture
checks exact causal filtering including sibling attempts, admission versus later
unknown settlement, prefix stability across append and reopen, scan limits when
no rows match, page cursor correctness, invalid queries, and no oversized cold
metadata payload read. It exercises real operator/Lua/model tool dispatch against
a fake provider. The actual CLI case reads a synthetic file, verifies readable
output without copying the original, and repeats a pinned timeline after new
inspection operations. No provider traffic, benchmark or new capture format.

Remediation corrected the fixture's illegal competing attempt admission (give
the sibling its own invocation), and compared JSON objects without relying on
Lua/native object member order. A source review kept attempt filtering from
including sibling attempts merely because they share a decision. Scoped audit
and terminal checks remain green. No additional assurance layer.
