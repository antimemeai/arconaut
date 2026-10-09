# Interrupted native session recovery — 2026-10-09

Unit starts 22:31:23 UTC, ends no later than 00:01:23 UTC; at most 25 minutes
of hardening, initial repair and one affected recheck/fixes. Root owns builds,
declarations/launcher/docs and checkpoint. This unit owns recovery implementation
and its existing direct test. No dependencies, provider calls, replay or migration.

## Evidence and actual defect

`src/coding.cpp:1126-1214` admits only linked provider/participant operations on
reopen. Native `exec`, `process`, `read_file`, `write_file`, and `edit_file` attempts
therefore block reopening even when the operator explicitly wants to acknowledge
their unknown outcome and continue inspecting/working. Neither an adapter receipt
nor a disappeared process supplies a terminal effect outcome. The current
`tests/session_recovery_test.cpp` covers this refusal; its mixed fixture includes
a JSON exec continuation and is intentionally invalid, rather than a valid native
mixed interrupted session.

`RetainedState::dispatch` refuses recovered or already opened attempts
(`src/retained_state.cpp:1772-1821`); terminal UNKNOWN preserves that refusal.
`reconcile` validates the entire unresolved set before making the physical writer
live (`:1889-1910`). Existing recovery appends all observations in one batch and
blocks admission on failure. Preserve both mechanisms. Tighten linkage so a
decision must plan exactly the invocation referenced by its admission in either
supported native input-binding representation; operation names alone establish
neither effect completion nor process custody.

Historical `JOURNAL.md:1784-1818` records a provider-only interrupted bootstrap
case and expressly does not establish that it was the reported operator session.
`context/live-bootstrap/audit` remains present, 7,305,266 bytes, SHA256
`92c4de77119d2c4ccde06aa7136f2d6aef2c81a6e898e59604d5bc8177c78e70`.
A read-only structural scan consumed 4,225 frames / 1,772 schema-1 retained events;
all 31 decision continuations start with JSON objects, none with BBM2. ARCOJ001
is also today's container signature and by itself does not establish compatibility.
Current `read_packet` rejects the historical payload encoding. No command is
replayed and no operator audit is opened for writing or converted. The exact
original operator session/error remains unidentified.

## Source grounding

`quarantine/hermes-agent-2026-10-07/tools/process_registry.py:1002-1070` distinguishes
PID gone/reused from collected exit, expressly avoiding a completion when there
is no waitable handle. Its bare-liveness fallback/recovered PID adoption are not
appropriate here. Recovery must not adopt or signal an old PID, invent exit or
claim containment.

`quarantine/auto-harness/autoharness/session/resume.py:24-63` distinguishes
in-progress work and work not to retry in a retained briefing. Transfer the explicit
unknown/no-replay explanation, not its unverified status labels.
`context/standing-state-frumentarii/letta-code/src/agent/turn-recovery-policy.ts:464-475`
closes stale pending calls with a specific explanation but suggests reissuing them;
that suggestion is rejected for unknown arbitrary effects.

Read the primary [Crash-Only Software](https://www.usenix.org/legacy/events/hotos03/tech/full_papers/candea/candea_html/)
section 3 and [Microreboot](https://www.usenix.org/legacy/events/osdi04/tech/full_papers/candea/candea_html/index.html)
sections 2/8 alongside the prior `papers/2026-10-07-backstop-recovery-study.md`.
The papers separate persistent data from component recovery and require known
idempotence or compensating operations for transparent retries. Microreboot's
external-resource discussion shows why process recovery alone can leave resources
alive. Our consequence is explicit UNKNOWN without retry, independent shared
services and a durable custody warning. It is not advanced resource custody/refit.

## Plan agreed with root

Default `RecoveryMode::provider_only` keeps ordinary startup fences. Explicit
`RecoveryMode::acknowledge_local_unknowns`, exposed by root's
`--recover-unknown-effects`, additionally accepts the five named native local
operations and `lua`, `workflow_execute`, `workflow_invoke` wrappers only after
complete decision/invocation/input/generation/context
validation. Invalid, unrecognized, unsupported or partially linked mixed sets
remain recovered and append no settlements.

These three wrappers are required by actual nested coding dispatch, not effect
success evidence. `CodingEngine::execute_workflow` admits its own attempt around
nested calls; `operator_turn` adds `workflow_invoke`; `call("lua")` similarly admits
an outer attempt. Runtime libraries omit OS/io effects (`src/coding.cpp:331-351`);
host effects use separate native admission. Named custom tools remain unsupported
by this small recovery scope because their old source binding needs separate work.

Retain terminal UNKNOWN for every validated abandoned attempt, with original
attempt identity and operation. No prior effect dispatch is invoked. Local
observations use a small BBM2 packet identifying recovery, uncertainty and no replay.
The same atomic append includes a native program `session.recovery` marker for opened
exec/process attempts: metadata mode `acknowledge-local-unknowns`,
`uncontained_exec:true`, and original attempt IDs. Root exposes it in recovery
notice/stats and blocks restart for this session even after rereopen; an independent
session switch remains permitted. No marker is needed for unopened command
admissions, which never crossed their retained dispatch boundary. File-only recovery
may restart after ordinary protocol repair. Original streams/context remain intact.

## Direct oracles

- Default command/file/native mixed reopen still refuses; explicit valid opened
  and unopened local attempts settle exactly once as UNKNOWN. A second ordinary
  reopen adds no settlement and keeps the command custody marker.
- Perform actual file write/edit and command marker effects through retained
  dispatch in a child, then `_exit` before terminal observation. Parent reopens
  the real audit; effect bytes/count remain exact, old dispatch invokes zero
  effects, context/input/decision linkage remain identical.
- Valid native mixed provider/file/command set settles atomically. Add malformed
  input, invocation/plan/generation/context mismatch or unknown operation to the
  set: whole set remains fenced with unchanged fact count and no observations.
- Exact physical capacity refusal blocks admission, leaves zero terminal
  settlements and does not permit old dispatch. A later reopen with capacity
  records the same original identities once.
- Durable command marker names only crossed exec/process admissions. File-only
  and unopened command recovery do not claim uncontained process activity;
  opened command recovery explicitly does not claim process death/exit/containment.
- Actual engine workflow → Lua → background exec `_exit` retains exactly four
  unresolved attempts (`workflow_invoke`, `workflow_execute`, `lua`, `exec`).
  Recovery settles their original identities UNKNOWN while the old command
  remains independently alive until a fixture release file, with no PID adoption
  or signals. Later file-only recovery cannot clear its custody marker.
- Engine stats/restart/switch inspect the durable marker after full and compact
  saved-state reopen. Restart is refused for opened command recovery; file-only
  and unopened command recovery permit ordinary protocol-valid restart.

## Results

First build compiled product changes and rejected three fixture errors (mutating
const `Value::find`, narrowed int initializer). Corrected fixture members/types.
First runtime build succeeded; recovery test failed the independent engine stats
oracle in 1.42s. Exact product cause: `RetainedState::current_programs` whitelisted
known labels but dropped `session.recovery`, so raw audit held the warning while
engine projection/saved state lost it. Root added the label to that projection;
compact saved-state reopen is part of the same direct oracle. The next run caught
my fixture's use of `ContextStore::view()` as a saved snapshot, which lacks the
required original locators. Replaced that fixture call with production
`ContextStore::checkpoint()` and preserved the compact-path assertion.

Final direct debug `session_recovery` passed in 3.93s, recorded in
`context/nls-afk-recovery-green.log`. This includes the previous 14 recovery
fixtures, 19 explicit native scenarios, the actual retained file/edit/command
effect crash, actual engine nested workflow/Lua/background-command crash, capacity
refusal, exact UNKNOWN linkage, no replay, idempotent ordinary reopen, sticky
custody warning after later file-only recovery, and engine stats/restart/switch
after full and compact saved-state reopen. Root also added the launcher notice on
ordinary later reopen, rather than requiring another explicit acknowledgement.
No standalone test run/build was launched by this lane; root owned compilation
and the direct execution. Formatting and `git diff --check` pass for owned edits.

Source state is frozen for shared integration checks; root records profile/full
hook results in the shared note. These direct results do not claim broader
resource custody or recovery compatibility. Historical format incompatibility,
unidentified original session, custom named Lua tool recovery and pending
provider-protocol repair remain explicit scope limits. No operator journal changed.
