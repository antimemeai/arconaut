# W1 implementation — 2026-10-07

Bound: absolute deadline 1791378646; implementation and focused checks, one
independent review (90 seconds maximum), then one findings recheck. No scheduler,
new dependencies, upstream execution, or full-suite assurance cascade.

Source choices: Hermes `skills_tool.py:187` resolves a single cached catalog for
callers, returning copies to avoid cache poisoning. Its filesystem/TTL scan is
unnecessary here: use retained program_config plus successful-boundary activation.
Community `wfcommon.py:36,49` distinguishes definition from execution identity;
retain full selected source/config revision in existing audit, not a mutable file
name masquerading as identity. Ecosystem P1/P10/P11 guide identity, boundary and
shared discovery; P3/P4 are explicitly outside W1.

Implement a bounded owned registry value under program_config (optional workflows
and prefix, backward compatible with modules/model/effort). Validate aliases,
bare names and built-in collisions together. Shared lookup drives operator routing,
model/Lua discovery/invocation, and cached terminal commands. Invoke through existing
audited effects and Lua runtime, with bounded nesting; no parallel/durable claims.
Powerwords: case-sensitive ASCII identifier tokens anywhere in submitted operator
text, including punctuation boundaries; non-ASCII bytes count as word constituents.
Preserve prompt; distinct definitions matching together fail, repeated/same-target
matches select once. Explicit slash/bare selection wins over powerwords. Assistant
and tool text never select workflows. Configured colors are existing named inks.

Direct oracles: alias/prefix and exact argument suffix; collisions reject; substring,
case and UTF-8 boundaries; conflict rejection; discovery identical to routing;
staged old/new values and failed-turn preservation using fake effects/provider;
completion/help and actual colored grid cells. Build only this checkout release.

## Delivered checkpoint

Implemented registry, retained configuration, model/Lua and operator invocation,
shared cached terminal discovery, exact-token selection and wrapped coloring.
Provider-selected invocation is sequential after tool outputs in the same turn;
see CALLABLE_WORKFLOWS.md for ordering, limits, and no durability/parallel claims.
Focused direct checks and affected regressions passed; one bounded findings
recheck completed (fixture delimiter correction verified by only its direct test).
At the original checkpoint, the independent Claude attempt timed out at90s with
no output and the candidate stayed inactive. Subsequent review fixes and integration
are recorded below.

## Integrated completion — 2026-10-07

Operator's wrap-up direction reopens only the two established findings from
[papers/workflows-2026-10-07/w1-independent-review.md](../papers/workflows-2026-10-07/w1-independent-review.md),
inside the same finite integration pass. Palette rendering and selection clamp
against the current catalog, including Enter/Tab and Up after publication shrinks
it. Invocation/depth guards now run inside the existing failure-latching try;
catching exhaustion and making another host call cannot publish staged config.
Direct regressions cover all these paths. No second source review.

Merged with native Jev, Beads, durable issuer ranges and validated context-history
projection. Existing DEBUGOFF profiling guards preserved. Release build <=j2;
all16 affected checks passed in32.90s, including workflows5.78s, native decision
models, retained state/crash/environment, context/JSON, coding/session and terminal
PTY/RRC. Historical review/timeouts above describe the original candidate period;
W1 is now accepted on master. It still does not provide a parallel/durable scheduler.
