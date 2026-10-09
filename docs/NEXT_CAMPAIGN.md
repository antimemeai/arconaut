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

## Order 2 implementation note

Route one explicit selected-context colleague request through CodingEngine's
existing admitted operation, originals and settlement. Tool `colleague`, Lua
`blackbird.colleague(request)` and `/colleague JSON` share the exact contract.
Catalog discovery describes installed transports and untested authentication;
only an observed call establishes availability for that provider/model. Keep
requested and actual models separate. Extend the existing native transport's
cooperative cancellation input, preserving unknown remote disposition on local
interruption. No implicit transcript export, continuation or retries.

Allowance 90 minutes including at most 25 minutes hardening, two layers. Direct
engine tests cover interface equivalence, original/attempt linkage, malformed
admission, remote-unknown settlement and one dispatch. Existing colleague tests
cover provider decoding. One useful selected-source Claude call supplies the live
outcome; a failed account stops that probe without repeated login attempts.

## Order 3 implementation note

A native participant owns a bounded selected request and a worker, never the main
engine or a Lua heap. Configure concurrent active participants before work; keep
at most 32 resident runs. Start admits a durable pending record before dispatch.
Workers capture into a bounded queue drained only by the owning engine, so they
never write the main journal concurrently. The first request returns into a visible
waiting-for-direction state. Addressed sends have caller message IDs, sender,
recipient, bounded text/inbox and delivery deduplication. They run at the next
request boundary; they cannot rewrite an in-flight provider request. Join seals
admission, drains already accepted directions and returns results in requested
order. Each direction is an explicitly accepted new one-request colleague call,
with the prior answer selected explicitly and bounded, not a hidden transcript.

Cancellation is a request flag propagated to native transport. Only worker return
establishes local settlement; its remote disposition remains observed/unknown.
Restart/session-switch refuses live participants; join/cancel and settle first.
On process reopen unfinished runs become unknown, never restarted. Completed
results remain readable. Archive settled runs to free resident slots; originals
stay in retained history. Task badges observe run state, never complete the plan.
No external service control, framework or arbitrary closure serialization.

90 minutes including at most 25 minutes hardening, two layers. Direct deterministic
barriers cover real overlap, configured cap, direction boundary/order/dedup,
backpressure, cancellation-versus-settlement, declared join order and crash-reopen
unknowns. Exercise model/Lua/operator on the same callable surface and a real UI
case for reading/cancelling one run while another is active. No new startup trial.

## Order 4 implementation note

Use the new participant surface for a selected-source review of the actual tiny
viewport rendering path, then the main Blackbird model for a concrete repair if
source/direct physical-terminal checks expose one. The old clamp bug is already
removed; do not manufacture that defect again. Scope implementation to minimal
viewport clearing/rendering and its direct PTY case. Record actual model calls,
source/result linkage, task state, context compaction/repair, build and quiescent
restart/continuation. Keep the operator's live process untouched. No general uplift
claim or matched timing experiment unless a useful comparison is available.

90 minutes including at most25 minutes two-layer hardening. Claude's single source
review timed out with unknown outcome; do not repeat it. One configured OpenAI
participant call may establish the usable local loop without pretending this is
heterogeneous collaboration. If that account also fails, retain the attempt and
specific outcome and leave the live demonstration outstanding; finish independent
product deliveries rather than repeating account probes.

## Delivered state

Orders1/2/3 delivered. The initial Claude call timed out unknown; the explicitly
authorized360-second attempt completed in337.331s. Its findings await source
triage. Order4
used a live OpenAI participant and main model to repair tiny viewport rendering;
physical checks, managed compaction/original repair and actual release restart
preserved working state. The operator's process was not interrupted.

Read [campaign results](../papers/2026-10-08-collaboration-campaign.md) for actual
checks, calls, interventions and limits. No further provider probe or broader
comparison is implied. The historical Giga remains paused.

## Operator follow-through: owned provider authentication

Operator prioritizes graduating from other installed harnesses' auth lifecycle.
Current OpenAI code reads Codex auth.json and asks Codex app-server for refresh;
Claude uses the installed CLI and its own authentication. Own provider account
selection, secret persistence, expiry/refresh and explicit login/logout/status;
separate local credential state, actual provider rejection and transport timeout.
Do not silently switch subscription traffic to separately billed API access.

Before implementing, establish each provider's supported application login and
inference path. OpenAI documents app-owned ChatGPT-plan OAuth tokens and renewal
[here](https://developers.openai.com/siwc/token-sharing-open-source/codex-app-server).
Claude's [authentication documentation](https://code.claude.com/docs/en/authentication)
is the starting point for its own supported options; a working CLI credential
is not evidence of a third-party app login entitlement. Existing arconaut-uaa.5
is the owning design task. No credential files were printed or changed.

OpenCode source follow-through: the formerly bundled
`opencode-anthropic-auth@0.0.13` performed browser PKCE authorization, code exchange,
local token storage and direct refresh with the Claude OAuth client ID; inference
used bearer tokens plus Claude CLI identification headers. Inspected the published
NPM package in memory, without executing or installing it. OpenCode
[removed that builtin on2026-03-19](https://github.com/anomalyco/opencode/pull/18186)
per Anthropic legal requests. This establishes historical mechanics, not present
first-party support. The current community
[opencode-claude-auth source](https://github.com/griffinmartin/opencode-claude-auth)
reads Claude Code Keychain/credential files, refreshes directly with CLI fallback,
and sends direct API requests with Claude Code identity headers/prompt transforms.
That implementation still starts from another harness's credentials. Study these
mechanisms without conflating technically independent OAuth with provider-approved
application access, or borrowed credential refresh with independent registration.

## 2026-10-08 authorized provider-auth implementation

Build owned C++ registry, private atomic0600 credential records, explicit active
account, safe metadata discovery and standalone auth CLI. Models get credential
status, never token entry/retrieval. API keys enter outside chat/audit. Environment
keys are available only when no explicit owned selection exists; a failed owned
subscription never falls through to API billing. Add Kimi/Grok device grants and
OpenAI/Anthropic PKCE login with cross-process serialized refresh. Login commits
only a complete validated response; concurrent logout/reselection cannot be
undone by a late refresh. HTTPS origins are pinned per adapter; no redirects.
Direct colleagues use owned credentials for all priority providers, preserving
selected-context admissions and response originals. Current Codex/Claude bridges
remain explicit compatibility paths until an owned account is selected.

Direct checks: permission/symlink and atomic replacement, malformed/missing token
responses, competing refresh and logout, RFC8628 pending/slow_down/expiry/denial,
PKCE state and callback rejection, secret exclusion from discovery/argv/audit,
HTTP status distinction and protocol routing. Each conceptual unit90min including
at most25min hardening/two layers. No new linked library or SDK; reuse existing
curl process machinery. Crypto mechanisms will be owned or delegated to installed
OS tooling without embedding a third-party auth framework. Browser consent needs
the operator; deterministic flow tests do not imply live account validation.

After auth: instrumentation and integrated evaluation interfaces; audit logging
review/upgrades; tracing and trajectories, including an owned reimplementation of
Entire-like functionality with better integration. Preserve originals and causal
request/effect linkage; do not start that second campaign during auth delivery.

Provider-auth candidate implementation and usage are now in USING_BLACKBIRD.
Shared credentials, priority-provider login adapters and direct peer transports
are implemented; account consent/live subscription checks and remote revocation
remain explicit follow-through. Operator authorized integration on2026-10-09: provider-auth is merged and
pushed on master at59539d3. Existing running processes keep their loaded binary. Next effort is instrumentation, integrated evaluation, audit review
and upgrades, tracing/trajectories and owned Entire-like machinery.

## Active trajectory campaign

Auth integration is complete; live sign-in and remote revocation stay separate
follow-through. [Trajectory design](TRAJECTORIES.md) sequences the next major
effort. First candidate slice implements shared bounded metadata reading and
`/trace`; following units add operational capture metadata, evaluations, a general
checkpoint/explanation interface and integrated UI/export. Git is one checkpoint
target, not the interface's organizing model. Central doctrine replaces "system
prompt" as the product concept and supplies another checkpoint design case.
Target capabilities, storage and restoration remain open. No new dependency selected.
