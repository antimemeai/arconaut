# Operational capture matrix and implementation — 2026-10-09

This is nls.2's finite AFK unit. Evaluation semantics and execution wait for the
operator. No SDK or second audit is adopted.

## Grounding and consequences

Current native source is authoritative: CodingEngine::operation admits before
dispatch and retains one result original plus a terminal locator. request groups
retries but its usage summary is process-local and metadata pages omit usage/model.
Nested Lua operations currently have depth but no parent attempt. Participants
queue original captures with run/request identity; owner drain time is not worker
execution time. Command collection exposes received/retained byte counts and
incomplete capture; result timing must distinguish an initial wait from final exit.

OpenTelemetry primary conventions distinguish requested and response model and
provider-reported token counters. Current conventions have moved to the dedicated
[GenAI repository](https://github.com/open-telemetry/semantic-conventions-genai/blob/main/docs/gen-ai/client-inference.md).
Use the distinction, not an evolving SDK/schema or derived billing. Quarantined
OpenClaw src/sessions/session-lifecycle-diagnostics.ts and
src/infra/diagnostic-trace-context.ts retain parent-span identity explicitly;
Letta's src/telemetry/channel.ts separates display labels from authority and
src/telemetry/index.ts exposes individual usage counters. Study-only; no source
or dependency imported.

| Path | Existing evidence | Gap this unit owns |
| --- | --- | --- |
| Main provider admission/retry | Exact input, attempt, group/ordinal, local clock pair | Concise requested model/provider; explicit decoded-result model/usage availability |
| Main provider decoded response | Exact retained result; successful usage UI | Bounded reported metadata even if later response import fails; absence never zero |
| Provider transport failure | Exact received diagnostic blocks, UNKNOWN terminal | Mark decoded response unavailable; retain per-attempt status, no billed-cost inference |
| Native/CLI colleague | Exact prepared/upstream/decoded/result bytes | Error/refusal metadata preserved, missing/ambiguous model visible; c54 classification |
| Nested native/Lua/workflow call | Actor/workflow/generation and own attempt | Actual active parent attempt, not guessed from temporal proximity |
| Program activation | Generation and exact program source | Local start clock attached to existing capture; operation generation links remain |
| Participant request/direction | Run/request identity, owner-only queued capture | Worker execution duration and actual observation time, bounded result metadata |
| Participant capture refusal | Bounded queue throws; run becomes unknown | Rejected bytes/events visible in persisted row, no uncertain provider replay |
| Ordinary tool result/capture | Admission/result/source links | Explicit bounded task/job/run/request links and parent identity |
| Command output/terminal | Exact offset bytes and final capture counters | Expose final received/retained/loss counters without duplicating output; original wait duration distinct from final deadline |
| Query/reopen | Pinned metadata paging, no cold large payload reads | New fields visible in the same query without reading provider result originals |

## Plan and direct oracles

Bound model strings to256 bytes and reported model names to16 entries. Usage is a
reported bounded numeric object, not an estimate: at most2048 packet bytes,
128 nodes and four levels. Missing values are null/unavailable; malformed or
over-limit observations stay explicit and exact original locators remain usable.
Use process clock provenance already owned by Observations.

Add real-engine oracles before product changes: exact requested versus returned
model and usage, absent/malformed values, same retry group with separate attempts,
active nested parent, participant delivery identity and queue loss, pinned/reopen
metadata. Add post-dispatch allocation UNKNOWN oracle to the existing colleague
engine case. Use actual committed facts, effects and source references.

Unit start22:31:23UTC, ceiling00:01:23UTC including max25min two-layer hardening.
Root owns source integration and six-worker builds. Existing settled checks are
not reopened beyond relevant shared-boundary regressions and required commit hook.

Integrated direct debug cases are green: session_recovery3.57s,
operational_metadata0.77s, colleague_engine0.90s, participants0.29s and
command_jobs_engine3.11s (8.65s total). Separate colleague/provider_colleague
red/green evidence is retained. Nested actual parent, bounded metadata, reported
usage, retry identities, participant loss and saved/pinned reopen now have direct
oracles. One instrumentation review found valid long participant fields could
make metadata capture alter a completed outcome; bounded null/status projection
and a counted successful9000-byte request fix it. Encoding refusal enters loss
accounting. No new dependency, provider probe or evaluation.

The hardening allowance includes the review/remediation already begun at
22:46:20UTC: at most25minutes through23:11:20UTC, within each original90minute
unit. One affected profile recheck/fix plus mandatory clean-staged-tree full debug
and owned C++/Lua lint completes integration. No new assurance campaign.

The final bounded-field projection fix also preserves null `request_id` and
`requested_model` plus their `*_status` in metadata pages. A real engine participant
with9000-byte fields dispatches exactly once, completes, reports exact usage and
process clock, appears in a run-filtered page, and survives compact pinned reopen.
Its extended direct debug oracle passes1.15s. Full request originals remain exact.
The earlier literal whitelist edit had not applied; concrete source inspection
caught that omission before the final profile snapshot. Historical logs remain
historical. No invented red-test claim is made for this review finding.

Bounds: projected reported_models additionally has a4096-byte packet ceiling;
queue metadata contributes to the8MiB cap. Rejected callback bytes/events are
accumulated per participant row; operation unretained_bytes is cumulative for the
current turn. Model/usage fields discarded from a projection are availability
`bounded`, with exact originals retained separately. Local operation duration
includes admission/dispatch/result handling; participant duration includes local
colleague preparation/capture. Neither duration is provider compute time or billed
latency. Program-source time marks local activation observation, not whole-turn
execution time. Clock synchronization remains unknown.

Mac affected nine-case release/ASAN passes11.16s/16.51s. The final bounded-field
projection rechecks pass3/3 release4.47s and ASAN6.95s; final TSAN9/9 passes23.32s.
Neuroses Linux first snapshot and final projection recheck continue; captures are
under context/linux and are historical until their commands complete. Full debug
and complete owned C++/Lua analysis are enforced by the ordinary checkpoint hook;
this note does not claim that pending gate passed. Closing nls.2/c54 records the
implemented direct-oracle result; a failed gate still prevents publication.
05n stays open for unsupported historical payloads and the unidentified original
session/error. Beads supported backup records these dispositions. Evaluations wait.

Final stable Linux snapshot passes3/3 debug2.25s, release1.93s, ASAN3.31s,
including actual long participant metadata, pinned compact reopen and command
capture projection (context/linux/run-t614eecp/checks.log). Earlier nine-case
Linux debug/release/ASAN passes3.74s/3.19s/5.82s. Remote workspaces cleaned.

First mandatory gate passes all85 debug tests in74.49s, then clang-tidy finds
two unchecked optional accesses in new recovery fixtures: a custom require does
not establish analyzer-visible control flow for the later dereference. Stop that
failed gate, add explicit presence guards without weakening outcome/linkage
assertions or suppressing diagnostics, and rerun the same mandatory hook. Product
source stays unchanged; no repeat host campaign. The same23:11:20UTC hardening
ceiling applies. Final gate results are established by the guarded source commit,
not by this earlier pending note. No hook bypass or new suppression.

## Final disposition at the hardening bound

The25minute source hardening window ends23:11:20UTC. No further instrumentation
repair is made after that bound. The final mandatory gate completes C++ analysis
and rejects one remaining easily-swappable-parameters diagnostic in the added
participants_test callback (label/raw). Full debug85/85 passes78.83s; all planned
Mac/Linux dynamic cases pass. Complete C++/Lua gate has NOT passed for this draft;
no source activation or qualified instrumentation delivery is claimed.

Preserve the full integrated state in Git stash "AFK instrumentation draft at
declared hardening bound" and context/nls-afk/integrated-before-split. The reviewable
nonexecuting [instrumentation overlay](2026-10-09-operational-instrumentation-draft.patch)
is retained in papers and is not applied to this checkpoint. nls.2 remains open.
Applying/repairing that held candidate needs explicit resumed operator scope; the
old allowance is not reset. Evaluation selection/execution remains deferred.

As independent useful delivery, package only the already source-checked colleague
and native recovery fixes. Their code is unchanged from the passing affected
checks; remove the held instrumentation overlay from the active source tree.
The ordinary every-commit full debug/C++/Lua gate qualifies that coherent bug-only
checkpoint. No new review, host/profile campaign, dependency or hardening repair.
05n retains its historical payload/original-session residual; no main activation.
