# Steering: present state and pre-design discussion

Research on trajectories at f109fc1, 2026-10-09. Operator asks for a deep look
before design. These are findings and recommendations, not an adopted design.
No runtime behavior changed or provider account exercised.

## Present state

Busy terminal input is a follow-up queue for the next entire Lua workflow.
`src/terminal.cpp` owns the queued strings; its worker calls `perform(prompt)`.
Only after worker completion does the UI dequeue another prompt and start a new
worker. The footer says “Enter queue”. The wakeup pipe already makes output
notifications responsive; the missing part is an active-work input path.

The queue is bounded and persisted with composer state in ui-state.bbm. Reopened
queued messages become recovered drafts for explicit submission. That is useful
draft recovery, but it does not record delivery into an active model request.
Ctrl-C and /cancel clear the queue and cancel the workflow.

`CodingEngine::turn` in src/coding.cpp creates a new Lua runtime, appends the initial
operator message and executes the workflow synchronously. Cancellation preserves
retained context/effect evidence but unwinds Lua and discards its continuation and
pending configuration. Cancel followed by a new turn therefore loses execution
continuity even when conversation context survives.

`CodingProvider::respond` is blocking. It has a cancellation callback but no active
response control channel. src/openai.cpp and the owned auth request path send HTTP
through curl; streaming observes incoming bytes but cannot send another message
into that response. `CodingEngine::request` builds a fixed input snapshot, and its
transport retries reuse that request. Preview text is display, not new context.

programs/turn.lua requests a response, executes its function calls, and repeats
until no calls remain or 64 steps are reached. It does not check operator input
before a request, before a tool, or before returning. A correction received during
inference cannot gate subsequent tools selected by the old response. The request
disables parallel tool calls, but dispatch still lacks a steering boundary.

Participant directions have admission retention, duplicate-message detection,
bounds and a condition-variable wakeup. src/participants.cpp explicitly promises
“after current request”. Each direction prepares another context-only request;
directions prepared while a request runs cannot include its not-yet-published
answer. This is not steering of an in-flight colleague response.

Station “steer” replaces direction text for later event prompts. The synchronous
main station loop samples controls between events. Plain input similarly blocks
on perform and cannot read new input during work.

## What the references establish

[Codex App Server](https://learn.chatgpt.com/docs/app-server) distinguishes active
turn steering from interruption. turn/steer requires the expected active turn ID,
appends input to that turn and emits no new turn-start notification.

[OpenAI mid-turn steering](https://developers.openai.com/api/docs/guides/steering)
documents GPT-6 Responses WebSocket steering. Acknowledgement means queued input;
continuation follows a safe output boundary. Started tools are not canceled.
Pending client tool outputs must be supplied without repeating accepted input.
Connection loss requires reconciliation before replay.

The [event reference](https://developers.openai.com/api/reference/cli/resources/beta/subresources/responses)
identifies successor response creation as the point where queued input enters the
continuation. It excludes conversation-bound and automatic-compaction requests.
Missing acknowledgement means unknown delivery, not rejection. Public API support
does not establish capability on our subscription endpoint; that remains a direct
transport experiment. Other provider capabilities have not been established here.

Owned source study used these retained, historical snapshots:

* openai/codex 08e2b58b07b8a423d6577b66fb7756e980b53dbf:
  core/src/session/{turn_input,input_queue,turn}.rs under quarantine/codex/codex-rs.
  Admission atomically checks active turn identity, reserves input order, appends
  turn-local input and signals activity. The loop drains pending input into history
  and pending input can require continuation even when the model would finish.
* can1357/oh-my-pi 8b25ad4a05625dde65df41d057756b4815f4837c:
  packages/ai/src/providers/openai-codex/live-steering.ts and its test file.
  A pump claims pending input during streaming; continuation planning removes
  already accepted input from subsequent requests. Tests cover automatic
  continuation, required tool output, rejection, and mismatched continuation.
* The Codex-derived openinterpreter snapshot
  9acdb707400234bebe31e672b808235495916b95 supplied corroborating source only.

Exact archives/restoration are recorded in the existing 2026-09-30 known-agent
and discovered-agent acquisition catalogs. No quarantined code changed or ran,
and no dependency was selected.

[Vakulenko, Kanoulas and de Rijke (2021)](https://arxiv.org/html/2104.07096v2)
analyze initiative through dialogue roles, topic introduction and follow-up.
This is conversational-search evidence, not coding-agent validation. Its useful
implication here: delivery speed and maintaining the task are separate evaluation
questions. Word overlap or a verbal acknowledgement cannot establish successful
course correction.

Read-only Rhizome watch: cpp-types-and-refactoring.md in the relocated
projects_old/inactive-2026-10-07/rhizome/papers. Transferable point: represent
different outcomes and identities explicitly; a tag alone does not establish
valid ownership or transitions. No Rhizome decision adopted.

## Direction to discuss

One bounded native inbox should target active work independently of the Lua
worker. Keep IDs, submission order, origin and delivery state; retain payloads
once in the existing binary machinery. Let terminal, station and addressed
messages route through a common mechanism without mutating Lua/context from
the UI thread. Atomically resolve admission versus work completion.

Preserve the original objective, progress, current constraints, tool outcomes and
logical work identity. A status question should get an answer and continuation;
a correction changes the approach; explicit cancellation or replacement changes
the objective. Central doctrine remains separately versioned governing context:
an ordinary steering message should not silently rewrite it. Compaction must
preserve active directions and their source linkage.

Expose steering to authored workflows, but enforce admission/dispatch/completion
coordination in native machinery so a custom Lua loop cannot accidentally bypass
it. Before dispatching further tools from an outdated response, incorporate the
new direction and let the model reconsider. Preserve running tool results and
settle unstarted calls truthfully when provider protocol requires outputs; do not
reuse “turn_interrupted” for work that was never started or repeat uncertain effects.

Provider-native live steering needs a bidirectional transport. For providers
without it, next-model-request delivery alone can leave a long stale inference
running. An inference-only cancel-and-resample fallback is worth investigating,
with the workflow and original objective kept alive. It is a distinct operation
from canceling the workflow. Capability and delivery limits must be visible.

Tentative interaction: Enter while busy steers; an explicit alternate action
queues later work; stop remains separate. Show concise delivery indicators near
the conversation/task pane: pending, sent, included in continuation, failed or
unknown. Do not label a server acknowledgement “applied”. Exact bindings and
presentation remain open.

Trajectories should link input admission, target work, provider response and first
continuation inclusion to existing clock domains and instruction/context versions.
Measure these intervals separately from observable behavioral response.

Direct cases for the eventual implementation: correction during inference;
correction between tools; direction while a tool runs; status question followed
by original task completion; explicit replacement; multiple ordered inputs;
arrival at final completion; capacity refusal; compaction; disconnect before/after
acknowledgement; pending tool output without duplicate steering/effects; restart
with uncertain delivery. Native simulation should establish dispatch/order/race
behavior; live task trials should evaluate course correction and task continuity.
No latency or parity claim has been measured in this research.

Discussion choices: ordinary busy input defaults to steering; running tools finish
unless explicitly stopped; unsupported live transports use inference-only restart
rather than wait for a long response. These are recommendations for operator
discussion, not implementation authorization.
