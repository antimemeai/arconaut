# Numeric and emission design review

Research-only review, 2026-10-10. Reviewed the current
[instrumentation proposal](../docs/INSTRUMENTATION.md), the
[numeric literature study](2026-10-10-instrumentation-literature.md), and the
native number and reported-usage paths cited below. This is one design review,
not an implementation, qualification campaign or review of a review. No code,
dependencies, provider requests, evaluations or dynamic tests were introduced.

The proposal correctly separates source observations, emitted measurements,
aggregates and consumers; preserves independent readers; distinguishes physical
duration from logical order; and does not claim exact quantiles from buckets or
sketches. Its revised retention rule is unconditional for acquired evidence and
failed acquisition outcomes. Optional activation does not authorize discarding
acquired observations. The earlier literature report's conditional discussion
of historical reproduction must not be read as an exception to this rule.

I found interpretation gaps rather than a contradictory numeric architecture.
The following choices need explicit contracts before the corresponding
implementation can claim exactness. Suggested prose is discussion input; it
does not select a new record schema, identifier scheme or dependency.

## 1. A versioned mapping does not by itself establish reconstruction

**High consequence if exact historical acquisition or rebuilding is promised.**
The proposal allows programmable native/Lua projections and says retained facts
plus a versioned mapping can supply measurements. That is valid only when the
mapping and all its inputs are sufficient. Preventing tool/provider effects
does not make a projection deterministic: it can still read mutable local
state, use an acquired clock/resource value, or depend on its program's previous
invocations. A semantic version identifies an interpretation; it is not the
missing input or emitted value.

The proposal already qualifies rebuilding with “where source facts suffice.”
Define that condition for programmable projections rather than allowing each
implementation to infer it differently. This is an engineering inference from
the proposed programmable capability and complete-custody requirement, not a
claim that OTel supplies a replay contract.

Suggested prose:

> A measurement is reconstructible only when its mapping is deterministic and
> its complete inputs, applicable definition/activation state and numeric
> semantics are retained. Other programmable measurements retain their emitted
> values and required context in authoritative custody. A semantic version alone
> is not reconstruction evidence.

This can reuse the existing audit; it does not require a second metrics journal.
Choose how an approved projection declares/checks its reconstruction contract.

## 2. Position and emission identity need an explicit population contract

**High consequence for exact readers, retries and replay.** The collection
section promises retained observation-derived measurements “by position” and
retry identity, but a source fact can produce multiple quantities, multiple
measurements of one quantity, or outputs under different definition generations.
Conversely, multiple retained facts can support one measurement. A source-fact
position alone does not establish which emitted population a reader has
consumed. Independent reader state prevents interference; it does not settle
that expansion or deduplication rule.

Also distinguish reacquiring one retained emission from requesting a fresh
physical sample. Two readers consuming the same retained resource sample must
not manufacture two source observations. Two separate physical acquisitions
are separate evidence even if their numerical values happen to match.

The [OTel data model](https://github.com/open-telemetry/opentelemetry-specification/blob/ef25ddb7d2ab36a194412c1c3bf71ae5900afdec/specification/metrics/data-model.md)
establishes origin, stream identity and window semantics; the
[SDK](https://github.com/open-telemetry/opentelemetry-specification/blob/ef25ddb7d2ab36a194412c1c3bf71ae5900afdec/specification/metrics/sdk.md)
separates reader state. Neither determines Blackbird's retained source expansion.
Use the owned identity/logical-clock design in `arconaut-nls.9` rather than
inventing a competing order here.

Suggested prose:

> Cursor coverage names emitted occurrences or a deterministic expansion of
> retained source facts, including multiple outputs and definition generations.
> Reacquiring an existing emission is a read; a new physical acquisition is a
> distinct observation. A collection states its actual source coverage/cut, and
> delivery retries preserve the emission identity required by that contract.

This need not imply a globally simultaneous snapshot or a total physical-time
order across unrelated owners.

## 3. Numerical domains need admission and representation rules

**Medium consequence; the failure paragraph handles part of this already.**
The definition includes a numeric domain, and failed mappings/clock reads and
overflow now have explicit unavailable/integrity outcomes. The concrete domain
contract still needs the accepted range, sign, unit-conversion behavior and
representation-failure behavior. Those are different from an aggregate's
overflow. A provider can report an invalid value before an aggregate exists.

The native [double constructor](../src/value.cpp) rejects nonfinite values
(`Number::Number(double)`, lines 39–42). NaN or infinity therefore cannot serve
as a normal native metric value or missing-value sentinel. Preserve malformed
external evidence under authoritative custody and report its typed failure;
do not force it through `Number` or substitute zero. This follows the current
native representation, not another ecosystem's NaN conventions.

Suggested prose:

> Definitions specify admissible inputs, unit conversion and numerical range.
> Invalid, unrepresentable and out-of-range observations have explicit outcomes
> distinct from absence and aggregate overflow. Their evidence is retained;
> nonfinite doubles are not normal native values or missing-value sentinels.

The existing rule that instrument failures do not gain product-state control
should apply to these paths as well. Exact counts can remain exact natively
while an external adapter explicitly reports a precision limit.

## 4. Histogram compatibility is more specific than aggregation algebra

**Medium consequence for multi-owner merging and representation changes.**
The optional fixed-bucket baseline correctly promises exact membership counts
and estimated within-bucket quantiles. To merge it, consumers also need bucket
boundaries and inclusivity, value units, overflow/underflow handling and the
representation version. Two exact bucket vectors with different boundaries
cannot be added position by position. Rebinning is exact only where compatible
boundaries permit it; otherwise it introduces an additional approximation.

This follows the explicit-boundary and representation-compatibility treatment
in the [OTel data model](https://github.com/open-telemetry/opentelemetry-specification/blob/ef25ddb7d2ab36a194412c1c3bf71ae5900afdec/specification/metrics/data-model.md).
The proposal already requires coherent bundles; preserve that requirement for
the histogram's count, sum and bucket population rather than independently
publishing fields from different cuts.

Suggested prose:

> Distribution representations identify their boundaries/resolution, inclusivity,
> range handling and compatibility version. Merge requires compatible
> representations and an identified nonduplicated population. Rebinning states
> whether it is exact or approximate. Representation changes remain interpretable
> across definition/source generations.

No global layout or particular sketch is needed to state this contract.

## 5. Reported usage needs update and overlap semantics

**Medium consequence for plausible but incorrect totals.** Naming provider/tool
usage separately from local measurements is necessary but does not say whether
each report is a delta, absolute total, replacement or terminal statement.
Repeated copies of one report are not new work. Nested reported fields may
overlap totals and must not be added indiscriminately. Retries require actual
attempt/report scope rather than attribution to a final result alone.

The current [provider receipt path](../src/coding.cpp) stores selected usage,
invokes `observed_usage` and renders a diagnostic (lines 1964–1995); its status
view later exposes the same `usage_` (line 2434). These are different surfaces
through which one report travels, not evidence of multiple usage quantities.
The proposal's rejection of duplicate measurement paths is correct; make its
adapter-level rule explicit. No provider-side protocol assumption is needed.

Suggested prose:

> Each external quantity identifies its report scope and whether updates are
> deltas, absolute observations, revisions or final statements. Normalization,
> status reads and presentation do not re-record the same report as new usage.
> All received reports remain evidence; a canonical usage projection declares
> how revisions and overlapping fields are interpreted.

## Result

The revised custody rule resolves the earlier retention ambiguity. The proposed
failure outcomes and owner-consistent snapshots also address the principal
reference-implementation traps. The five gaps above should become explicit
discussion choices or contract prose, followed by implementation-specific tests
after approval. They do not justify running evaluations now or silently adopting
an exporter, telemetry SDK or sketch library.
