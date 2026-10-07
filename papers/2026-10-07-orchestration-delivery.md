# G6 scoped Lua orchestration

Unit arconaut-7iy.6 starts 2026-10-07T06:16:52.385066Z, deadline
07:46:52.385066Z. Root supplied a fresh working context; no predecessor seed or
attempt authority inherited. Existing startup-replay changes in retained native
source, launcher and tests belong to another owner and are excluded.

## Small source-grounded subplan

Build a first synchronous orchestration module using ordinary Lua/native calls:
explicit owner and participant IDs, sequential calls, boolean branch, selected join,
finite participant/attempt limits, persistent local pause, conservative exceptions,
and explicit retry only for refused/not_dispatched. No asynchronous futures,
detached participants, crash persistence, remote cancellation or replay claims.
Native tool calls own actual process lifetimes; a Lua callback is not containment.
Callback outcomes are explicit caller assertions, not native quiescence evidence.

Read source: programs/maintenance.lua, useful_work.lua; current named-module
interface; docs/PARTICIPANTS_AND_MODES.md; candidate/giga-colleagues docs/COLLEAGUE.md.
The existing workflow literature (2026-09-30-programmable-workflows-frumentarii,
Restate sections and OTP supervision implications) distinguishes orchestration
from external effects and lifetime supervision from persistent state. Concrete
consequences: no replay, unknown exceptions cannot authorize retry, no resurrected
admissions, immutable returned snapshots, and a running participant cannot join as
complete. Reference sources remain quarantined/study-only; no dependency adoption.

G5 is blocked on actual heterogeneous access: Claude expired OAuth401, Kimi403,
MiMo403/expired key401. Its independent candidate binary is callable via native
exec, not integrated/activated runtime. G6 may use that existing source-qualified
context-only OpenAI slice for useful work without asserting heterogeneous success
or retrying failed external accounts. Full parallel/multi-provider behavior stays
unavailable rather than becoming a hidden acceptance claim.

Direct red oracle: tests/orchestration_test.lua initially fails because module is
absent. Count actual callback admissions: duplicate identity, unknown exception,
malformed result and pause must cause zero additional dispatch; only explicit
refused/not_dispatched permits retry; attempt/participant bounds refuse; selected
branch alone runs; join preserves unresolved and living outcomes; snapshot mutation
cannot grant authority. Useful work: compose bounded source reads and a context-only
colleague diagnosis, then write an operating source map with actual observations.

Implementation/use allowance within original90min. Hardening max25min inside it:
layer1 inspect/fix concrete identity, result-alias, retry, active-call, pause and
bound defects with direct Lua cases; layer2 one recheck/fix of those changes, no
third. Declare exact start/deadline before that work. Checks are targeted Lua5.4.8
Mac and Linux execution, syntax/formatting, actual retained-module activation/use;
no native source changes therefore no replacement executable/RRC needed. At bound,
archive candidate and exact blocker; do not activate unsafe source. Next action is
implement and execute failing dispatch oracles, then useful native-call composition.

## Initial implementation observations

06:19:44UTC direct Mac Lua5.4.8 cases passed, six counted callback dispatches.
The initial missing-module failure is retained in current tool audit. No provider
call or native process lifetime assertion made by these deterministic cases.

## Bounded hardening and actual delivery

Hardening start06:20:42UTC, maximum deadline06:45:42UTC; completed06:21:52UTC.
Layer1 fixed sparse join selection and contradictory unknown/not_dispatched
classification, expanded direct alias/bound cases. Mac and Linux Lua5.4.8 passed.
Linux temporary source at /tmp/arco-g6-ZtL7EmiZ compiled only the existing qualified
Lua archive with Clang18; captured context/g6/linux-layer1.log. Layer2 one Mac/Linux
recheck at06:21:46 passed dispatch oracles (six calls; refusal loop bounded at two).
Formatting discrepancy fixed at06:21:52; no third recheck or full-profile runs.
Source SHA256 b39677380201658d05272bd9882bbd3086146cc417cebbb4225968feb5b57434;
test SHA256 e935b21c43083fd722b3115531aedec2a312bfbf044ef02f31e1192c8da66ae9.

Actual ordinary Lua/native composition performed selected-source reads, selected
branch, addressed G5 candidate OpenAI call, and join: completed, one request, no
retry. Request g6-ownership-c7d34276-1, actual gpt-6.1-sol, provider reported4319
input/766 output/5085 total tokens; account cost unavailable. Colleague wrote the
useful operating map, published through a second orchestrated native write at
docs/G6_ORCHESTRATION_MAP.md. This was documentation work, not colleague safety
review or a third assurance layer. Captures context/g6/ownership-call-c7d34276;
outer state context/g6/actual-orchestration.json. Raw/context files remain ignored.

Staged named orchestration module in current native session at revision
c7d34276b2ed4987ad0b000000000000. Fresh root-authored session had no effective
modules; no predecessor maintenance module inheritance. Normal successful boundary
activation and actual retained import remain next before scoped acceptance.
No native change/build/RRC necessary for this Lua-only unit. Parallel scheduling,
first-class native participant cancellation/messaging and durable admissions remain
unavailable; bounded synchronous tool/colleague composition is the delivered slice.
