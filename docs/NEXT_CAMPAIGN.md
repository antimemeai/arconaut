# Proposed next Blackbird campaign

Status: operator authorized implementation on 2026-10-08 ("let’s ride").
Tracked as `arconaut-3sm`, delivered serially on master. The previous Giga remains
paused. Each unit has 90 minutes including at most 25 minutes hardening, two layers.

Make local collaboration an ordinary Blackbird working mode: the operator and
model choose a colleague, give it selected context and a task, watch what is
actually happening, exchange direction, and use the result without losing the
main conversation. Build on the retained core, task list and callable workflows.
The [assessment](../papers/2026-10-08-consolidation-and-field-assessment.md) explains
why this has more immediate merit than another startup campaign.

## One useful campaign outcome

From one terminal, a main model and a configured colleague complete a real
Blackbird change. Their assigned work and reported/observed outcomes are visible.
The operator can inspect and redirect a participant while independent work
continues. A context compaction and a quiescent native restart preserve the working
plan and completed results; ambiguous external effects remain explicitly unknown.
The change lands with its relevant checks. We record actual usefulness and operator
interventions, without claiming a general coding uplift from one demonstration.

## Orders and delivery

| Order | Deliverable | Direct outcome and limits |
| --- | --- | --- |
| 1. Everyday continuity | Session name/picker/resume over current discovery, with task/draft state coherent on return. Keep the known physical tiny-window bug as a separately bounded repair. | Switch between two saved sessions from the UI, preserve the old draft/context/tasks, restore the actual selected session, and show the selected task state. Keep tasks/subtasks at two levels. The tiny-window repair uses actual dimensions and its own direct case; it does not gate the session picker. |
| 2. Shared colleague invocation | Integrate the existing context-only colleague contract with the coding engine's retained call boundary. Model tool, Lua call and operator command use one schema/result. Show configured versus actually available provider/model and concise failure reasons. | A selected-context request through each interface reaches the same audited operation/result. Produce one useful live non-OpenAI answer with a working configured account; old access refusals are historical, not permission to repeat failed probes. Tool-using remote agents remain separate scope. |
| 3. Bounded local participants | Native run handles, configured concurrency, observe/await/join, addressed steering and explicit cancel requests; operator run view and task bindings. | Two independent participants work concurrently; inspect one, send direction, handle failure and cancellation, then join in declared order. Show requested cancellation separately from observed settlement and unknown remote effects. First scope is process lifetime, not arbitrary worker resurrection. |
| 4. Sustained Blackbird work | Use the delivered loop to fix a real repository issue, compact/repair relevant context, build, quiesce, restart and continue. Triage the complaint path encountered during work. | Useful committed change, restored working plan/results, no silent retry of unknown effects. Record elapsed time, available usage, operator interventions and actual obstacles. Compare a small matched solo case only if it answers whether collaboration helped. |

Order 2's interface work can proceed with deterministic transports even if a live
account is unavailable. Account failure blocks the live heterogeneous outcome,
not unrelated product progress. Native observation must not convert an API/CLI
exit into proof that every remote side effect stopped. Do not classify an
unavailable price or failed-attempt usage as zero.

Session interaction comes first because `/new` alone creates a destination but
leaves returning to it awkward. Order 3 comes after a common callable surface:
adding threads around several unrelated CLIs would otherwise cement inconsistent
context, result and lifetime rules. Order 4 uses the machinery and exposes friction
before investing in a broader platform.

## Machinery and model ergonomics

Own the required C++/Lua machinery. No framework, provider SDK, MCP dependency or
new runtime is selected here. Study the targeted reference paths before implementing
their semantics. Reuse the current owner/journal/wake/render mechanisms where
appropriate; do not clone the complete transcript into each participant or task.

A run identity, its task binding and selected context should be addressable across
model, Lua and UI. Keep current reads concise: state, owner, source generation,
provider/model, last result or blocker, and the next legal control. Read originals
and detailed output on demand. A task records the plan; a run records execution.
A worker returning successfully does not automatically complete the task.

Messages need sender/recipient and delivery identity, ordering and bounded pending
storage. Steering affects an explicitly defined boundary. The default governing-
program change remains after the current affected work concludes; interrupt/apply
requires observed local settlement and a legible activation result. Arbitrary
closure/heap serialization and crash-surviving subprocess custody are later work.

The demonstration uses two participants. Production concurrency should be an
explicit operating/resource choice, not an architectural restriction on provider
pairs. Provider-specific capability and authentication differences stay visible.

## Campaign discipline

Proposed allowance: four implementation units, each initially one focused working
session, with a concrete scope and resource allowance declared before coding.
Use the existing Giga default of at most 90 minutes per whole unit unless the
operator chooses otherwise; at most 25 minutes of that is hardening, in the existing
two layers. An unfinished safe partial is recorded and checkpointed; an unsafe
candidate remains inactive. Do not reset its allowance by renaming it.

Integrate conceptual deliveries serially onto the default main line. Parallel
implementation is optional only when explicitly requested and ownership separates
cleanly. Reuse bounded candidate checkout leases; retire branch heads after
integration rather than accumulating a second invisible backlog. Source archives
and experiment results remain available independently of checkout lifetime.

Checks attack changed behavior directly: selection/context/result linkage,
concurrency and deterministic join, stale task updates, delivery/backpressure,
cancel-versus-settlement, active input focus and quiescent restart. Run applicable
Mac/Linux profiles, fix the one recheck's findings, and use the live outcome once.
No review of reviews, blanket certification or fresh 300-start benchmark for each
unit. Re-measure presentation only when a concrete regression warrants it.

## After this campaign

Advance the two-independent-peer P2P/E2EE design using the existing networking
research and now-concrete participant/message contracts. Add station attachment
and useful optional connectors over the same run/control surface. Treat standalone
login/distribution, linked-history continuation and live worker refit as explicit
owning units. The further stream-capture measurement excluded by the operator
stays excluded. Local diagnostics keep their 30-day TTL.

## Order 1 implementation note

Use retained session settings for an optional 128-byte name, backward-compatible
with existing snapshots. Reuse the command palette for named session selection;
Enter loads a resume command while saving the previous draft, then explicit Enter
switches. /resume DIRECTORY works in plain mode too. Switching checkpoints the
old context, tears down its UI/worker/journal, and opens the destination without
sending a provider request. Destination settings, task state and UI drafts belong
to that destination. Failed destination opening returns to the old saved session
once, preserving the error and avoiding a retry loop. Discovery reads metadata
only, with audit still authoritative. No new library.

Direct checks: retained name round trip/invalid input, palette search/arrows, two
saved sessions with independent identities/context/tasks/drafts, busy queue
boundary, failed/busy destination, and restart routing after selection. Existing
new-session PTY is extended rather than adding a second receipt mechanism.
