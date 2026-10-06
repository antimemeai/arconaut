# Historical Arconaut

The historical row is a reference for small interfaces and concrete failure paths,
not a vote to resurrect the implementation. This study re-read the wired CLI/Soul,
provider conversions, tool dispatch, terminal ownership, bus, context, audit and
selected tests. Earlier executed evidence remains in the
[2026-09-29 oracle assessment](../../2026-09-29-oracle-assessment.md); no inherited
code was executed during this capabilities study.

The current executable uses the old sequential Soul; the newer reactor run_turn is
still a stub. Provider adapters lose outbound tool protocol or do not parse tool
calls, and endpoint concatenation removes the needed separator. TUI terminal_send
awaits a reply serviced by the same select loop that is awaiting the whole turn.
Interrupt acknowledgement is deferred status text. Component tests put an independent
receiver task in place and therefore do not exercise that owner cycle.

The turn-wide result cache has no purity/freshness contract: read/write/read, rerun
identical tests after edits and repeated side effects change meaning. This also
corrupts a foundation-relevant primitive: timestamp has no arguments and returns
UTC now, so repeated observations within one turn reuse its earlier cached result.
Time needs its own semantics, not generic tool result reuse.

The unconfigured compactor replaces deleted material with a count, and checkpoints
store lengths/counts rather than originals. The audit hook captures only selected
turn/tool events, reduces errors to brief, misses provider failures and is bypassed
on cached calls. Logger failures print but do not reach callers. A passing build and
local suites do not establish any of the stronger whole-harness claims.

The refreshed baseline should carry forward discriminating experiments and useful
small contract questions, rather than phase labels, incomplete wiring or test
expectations that preserve broken behavior. Implementation disposition remains a
separate reviewed task after the operator discussion and ground-up specification.
