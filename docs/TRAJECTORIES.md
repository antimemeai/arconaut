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

## Timed observations implementation unit

Operator sent the corrected direction on2026-10-09. Allowance90min total,
including at most25min two-layer hardening. Implement clock-stamped operation
metadata and generic retained point observations; observe effective instruction
bytes by reference without a doctrine authoring/activation API. Read-only explicit
Git HEAD/history observations keep metadata times separate from observation time.
Provide bounded variable queries and local operator point plots over the same
trajectory page. No clock service control, automatic polling or universal variables
inventory. Cross-host synchronization is unverified and shown unknown.

Direct oracles: injected wall-clock adjustments with monotonic progression;
clock domains/precision retained; third variable through the generic native API;
invalid sample rejected before publication; prefix/page bounds and reopen; stable
content observation for unchanged doctrine and source links; actual operator/Lua
query and temporary-repo HEAD/history/author/committer values without Git writes;
point plots never manufacture active intervals. Existing affected coding/trajectory/
audit/terminal cases answer integration faults. Reuse native bounded Child and
retained metadata; no dependency adoption. Git formatting grounded in
[upstream documentation](https://git-scm.com/docs/pretty-formats); trace time
representation informed by [OpenTelemetry time](https://opentelemetry.io/docs/specs/otel/trace/api/#time).

## Timed observation usage and limits

The `trajectories` candidate retains operation-start UTC/monotonic pairs and
result observation time/duration in the existing audit. Each observer has a clock
identity with host/process provenance. `sampling_span_ns` is the acquisition span
of the local clock pair; it is not a measured UTC accuracy or cross-host offset.
Synchronization remains unknown. Old, absent or malformed time metadata stays
undated. Wall-clock adjustments are retained; monotonic durations remain local.

```text
/git-observe {"path":"/absolute/repository","history":8}
/variables {"variable":"git.head","cursor":0,"count":64,"scan":256}
/correlate {"cursor":0,"count":64,"scan":256}
/trace {"attempt":"32-lowercase-hex-characters"}
```

The plot uses one UTC axis and separate named lanes. `o` marks a sampled point;
`+` means several points occupy one column at the current width. Details give
record locators, values and clock IDs. It draws no effective intervals. Multiple
clock domains remain explicitly unaligned. The page reports `next` and `end`;
continue with those as `cursor` and `end` to read the same finite prefix. A page
can be empty while `next` advances. At most256 facts are examined and64 rows
returned per call; a long history needs explicit paging.

Models use `variables_read({query=...})`, `trajectory_read({query=...})` and
`git_observe({path=...,history=...})`; Lua calls the same tools through
`blackbird.call`. The owned native `Observations::sample(name,value,source,status)`
accepts other named variables without choosing a universal inventory. Values and
source objects are limited to4096 serialized bytes each; invalid samples are
rejected before publication. Existing bounded journal capacity governs retention.
There is no polling loop or separate authoritative timeline.

`doctrine.effective` observes the final native request `instructions` field,
including any composed guidance. Its source names the invocation, attempt and
field, so existing `audit_inspect` can retrieve the original request input.
Consecutive identical instruction bytes reuse a content-observation identity;
a changed representation gets a new one. Restarting the observer also starts a
new identity scope, which alone establishes no doctrine change. This is an
**effective request representation**, not an established central-doctrine registry.
It adds no editor, activation mechanism or routine revision cadence.

Git observations explicitly invoke bounded read-only native Git commands.
`git.head` records the HEAD found at acquisition time (or unavailable).
`git.commit` preserves author/committer Unix seconds and parents from history
rooted at that observed commit. Their sample UTC time records acquisition;
it does not replace the original commit metadata times. Repository lookup and
HEAD/history reads are separate calls, not an atomic snapshot. No transition time,
worktree byte identity or loaded-executable identity is inferred. Git environment
overrides are removed so they cannot silently redirect the requested repository.
The first plot shows acquisition points; plotting commit metadata times as a
separate historical axis and measuring cross-host alignment remain subsequent
work in the campaign.

## Timed unit verification

Seven scoped cases passed on Mac debug/release (Clang23.1.2, Lua5.4.8)
and Linux debug (Clang18.1.3, libstdc++13, Lua5.4.8): observations,
observations_cli, trajectory, trajectory_cli, audit, terminal and coding.
Linux capture: context/linux/run-s674dyxm. Local release build:
context/observations-release-build.log. Private native fixtures exercise retained
samples/reopen, clock reversal/domains, malformed clocks, unchanged instruction
bytes and source links. Real CLI/Git fixtures compare known HEAD/history and
separate authored/committed dates, verify Git file bytes remain unchanged,
reject oversized history, and expose unavailable repositories. No provider call.

Remediation scoped both the retry fixture and production process-output reader
to their relevant audit record kinds before requiring request/process metadata.
This preserves output retrieval with clock/sample records present. Generic sample
selection ignores non-sample packets; malformed time metadata remains undated.
The bare `/git-observe` usage path was checked locally after its small follow-up.
No startup benchmark, certification or further assurance layer.
