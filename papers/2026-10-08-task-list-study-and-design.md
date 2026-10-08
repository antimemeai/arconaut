# Task lists: presentation, model use, and freshness

2026-10-08. Research and a design candidate for operator discussion. No product
implementation or dependency adoption. Source observations below describe retained
snapshots, not a comparison of the installed versions in the startup benchmark.

## Recommendation

Make the task list durable, shared working state at session/workstream scope,
with optional links to Beads. A bead can contain many granular tasks; a task can
also be useful without a bead. Keep a compact task viewport near the composer,
with an expandable tree for inspection and editing. Models, Lua programs, and
the operator use the same native state and mutation path.

Separate three things: intended work, actual runtime activity, and whether the
viewer has received current state. Their indicators answer different questions.
An active task does not establish that a command is running; a heartbeat does
not establish progress; an unchanged task does not establish a stale connection.

## Reference implementations inspected

Acquisition and restoration are recorded in
[known-agent acquisition](2026-09-30-known-agent-acquisition.json),
[legacy catalog](legacy-reference-catalog.json), and
[Hermes acquisition](workflows-2026-10-07/hermes-acquisition.json).
Paths below are relative to the named directory under ignored `quarantine/`.
The legacy catalog contains earlier revisions; the known-agent acquisition
records identify the refreshed snapshots actually inspected here.

| Reference / source revision | Source paths | Mechanism and consequence |
| --- | --- | --- |
| [Codex](https://github.com/openai/codex/tree/08e2b58b07b8a423d6577b66fb7756e980b53dbf) / `08e2b58b07b8` | `codex-rs/core/src/tools/handlers/plan.rs`; `codex-rs/protocol/src/plan_tool.rs` | `update_plan` accepts a complete list of text/status steps, emits a typed plan event, and normally returns a short acknowledgment. Cheap feedback, but no stable step identity in this schema and no current-list readback in this handler. This checklist tool is distinct from Plan mode. |
| [OpenCode](https://github.com/sst/opencode/tree/2006259a02a87edf9e37f253cbddf3188309026b) / `2006259a02a87` | `packages/opencode/src/tool/todo.ts`; `src/session/todo.ts`; `packages/tui/src/routes/session/index.tsx` | Writes the entire content/status/priority list; database transaction deletes and reinserts session rows in order, then publishes `todo.updated`. Good commit-before-notification ordering. Whole-list replacement and no stable task IDs are poor fits for independent concurrent edits. Tool rendering distinguishes updating from returned todos. |
| [Gemini CLI](https://github.com/google-gemini/gemini-cli/tree/c6bccb7ecbf6d8368d995455dd725ed34466faad) / `c6bccb7ecbf6` | `packages/core/src/tools/write-todos.ts` | Complete replacement, readable result and separate typed display result, including blocked/cancelled states. Validates at most one in-progress item. Separate display data is useful; single-active behavior does not fit Blackbird's concurrent programs. |
| [Pi](https://github.com/earendil-works/pi/tree/8ce69e9d2b171d173fe4b6b2b6256f1f4411e69d) / `8ce69e9d2b17` | `packages/coding-agent/examples/extensions/todo.ts` | Example extension, not a baseline built-in task tool. Integer IDs, add/list/toggle/clear, branch-aware reconstruction from tool-result snapshots, cached panel rendering. Stable IDs and branch semantics are useful. Toggle is unsafe to blindly repeat; scanning history and echoing snapshots are not our preferred hot path. |
| [Oh My Pi](https://github.com/can1357/oh-my-pi/tree/8b25ad4a05625dde65df41d057756b4815f4837c) / `8b25ad4a0562` | coding-agent `src/tools/todo.ts`, `src/session/todo-tracker.ts`, `src/prompts/tools/todo.md`, `docs/tools/todo.md`; `packages/tui/src/tools/todo.ts` | Phases, incremental operations, blocked reasons, bounded sticky preview, recent-completion context, editor/copy commands and restored state. Tasks are addressed by verbatim content; prompts explicitly warn against guessing it. Starting work enforces one active task and normalization can start pending work. Runtime descriptions can fuzzily match task wording. Keep the bounded viewport and useful editing; replace text identity and fuzzy activity attribution with IDs. Do not auto-start tasks or import completion nag loops. |
| [Kimi CLI](https://github.com/MoonshotAI/kimi-cli/tree/9ab1286b8fe4e6bcd116949a27ce5e0ac3389c82) / `9ab1286b8fe4` | `src/kimi_cli/tools/todo/__init__.py` | Historical Python implementation: omit todos to read, supply them to replace; typed display blocks and session persistence. Root reads reload disk state. Useful explicit readback and freshness awareness, but no IDs or demonstrated concurrent-writer protection here. This is not the installed Kimi Code 2.1.1 implementation. |
| [Hermes](https://github.com/NousResearch/hermes-agent/tree/a3ed4a173070e981332e4d879ff6cc8b9efd57ab) / `a3ed4a173070` | `tools/todo_tool.py`; `apps/desktop/src/store/todos.ts` | Stable IDs, optional parents, merge operations, monotonic revision, rollback on validation failure, bounded data, and active tasks/ancestors restored into compressed context. Desktop rejects older revisions, but also applies unversioned speculative tool-start updates; its turn-end handling can drop an unfinished visible list. Borrow IDs, revision fencing, and compact context restoration. Keep accepted state separate from pending edits and preserve unfinished work across turns. The revision watermark is not itself writer conflict detection. |
| [Qwen Code](https://github.com/QwenLM/qwen-code/tree/310f4ba3ab954eb858b500c6aee3551cc564ee0a) / `310f4ba3ab95` | `packages/cli/src/ui/opentui/sticky-todos.ts` | Latest history snapshot drives sticky todos; an explicit empty list stops lookup rather than resurrecting an earlier list. New user messages can hide the panel as stale; completed lists also hide. Preserve empty-versus-absent semantics, but steering should not erase or silently hide unfinished Blackbird tasks. |

The public [Claude Code agent-team documentation](https://code.claude.com/docs/en/agent-teams)
also describes shared tasks, dependency-aware claiming, file-locking for claims,
and local task persistence. This is documentation evidence, not inspection of a
proprietary implementation. Availability varies; no assumption that every model
has the same task tools. It reinforces the need for deliberate concurrent-edit
semantics rather than a cosmetic checkbox list.

## Literature and what it changes

These sources motivate choices; they do not establish that our candidate will
work well before trying it. Shneiderman and Harrison PDFs are retained locally
under `papers/task-list-2026-10-08/` and ignored by Git. The Microsoft PDFs were
read through the browsing tool, but direct archival downloads returned HTTP 403.
URLs, acquired identities, and those acquisition failures are in
[sources.json](task-list-2026-10-08/sources.json).

| Source | Relevant finding or guidance | Design consequence |
| --- | --- | --- |
| Shneiderman, 1996, [The Eyes Have It](https://www.cs.umd.edu/users/ben/papers/Shneiderman1996eyes.pdf) | Overview, filtering, details on demand; relations and history are useful operations. | Small actionable overview, expandable hierarchy, filters and task detail/history. Do not permanently display every granular step. |
| Amershi et al., CHI 2019, [Guidelines for Human-AI Interaction](https://www.microsoft.com/en-us/research/wp-content/uploads/2019/01/Guidelines-for-Human-AI-Interaction-camera-ready.pdf) | Relevant information, easy correction/dismissal, recent-interaction memory, cautious updates and immediate consequences. | Direct operator editing, stable focus while updates arrive, visible accepted changes, and preserved unfinished work. |
| Horvitz, CHI 1999, [Principles of Mixed-Initiative User Interfaces](https://www.microsoft.com/en-us/research/wp-content/uploads/2016/11/chi99horvitz.pdf) | Consider attention, uncertain goals and interruption costs; make invocation, termination and refinement easy. | Passive progress by default; surface actual blockers without modal interruptions. Neither infer operator intent from a spinner nor force checklist completion before conversation can continue. |
| Nielsen, 1993, [Response Times](https://www.nngroup.com/articles/response-times-3-important-limits/) | Roughly 100 ms feels immediate; feedback and display stability matter during waiting. | A prospective local accepted-edit-to-output target around 100 ms under an unblocked terminal. Measure it if implemented; do not equate this with model response time or promise it under blocked output. |
| Harrison et al., UIST 2007, [Rethinking the Progress Bar](https://www.chrisharrison.net/index.php/Research/ProgressBars) | The shape of progress feedback can affect perceived duration. | Motion communicates observed activity. Counts communicate task state. A changing task count is not an effort percentage or a reliable ETA. Known-denominator operation progress can be shown separately. |
| Anthropic, 2025, [Writing effective tools for agents](https://www.anthropic.com/engineering/writing-tools-for-agents) | Few purposeful tools, meaningful identifiers, relevant bounded results, actionable errors and explicit schemas. | Compact stable IDs accompanied by human-readable titles; small batched edits, useful readback, paging, and conflict responses that contain the current affected rows. |
| Anthropic, 2025, [Effective context engineering](https://www.anthropic.com/engineering/effective-context-engineering-for-ai-agents) | Structured notes outside the context window can preserve ongoing work through compaction. | Reintroduce the current task projection at continuation/compaction boundaries; do not rely on an old checklist buried in transcript text. |
| Kleppmann et al., 2019, [Local-first software](https://www.inkandswitch.com/essay/local-first/) | Local reads and writes support immediate interaction; synchronization introduces separate concerns. | A local current projection and one native owner. This does not call for a CRDT, a distributed task service, or governance of independently owned kernels. |
| W3C, [Understanding Status Messages](https://www.w3.org/WAI/WCAG22/Understanding/status-messages.html) | Status changes need understandable context without stealing focus; excessive announcements are distracting. | Glyph/text alternatives to color, motion-off support, meaningful plain-text changes, and restrained announcements. This is guidance applied to a terminal, not a WCAG compliance claim. |

## Candidate data and model interface

Each work list has an ID, title, revision, optional bead reference, and ordered
groups/tasks. Groups organize work and have derived rollups; actionable tasks
have explicit states: queued, active, blocked, done, dropped. Count actionable
tasks once, excluding organizational groups. A task retains a compact stable ID
such as `t17` through rename, reordering and reopening. IDs are qualified by list
when crossing scopes. Multiple active tasks are ordinary.

A task can carry an owner, short note, blocker reason, optional dependencies,
last semantic edit, and explicit links to native run/operation IDs. Dependencies
provide readiness information; they need not impose a universal scheduler on
model-authored workflows. A task marked done is an actor's recorded decision.
Successful command exit does not silently complete it. A failed attempt appears
as an attempt result, distinct from the task's state. Reopening remains possible.

Candidate APIs, not existing Blackbird tool names:

- `tasks.read`: compact actionable projection plus ancestor groups by default;
  read particular IDs, filtered pages, details, or changes since a revision.
- `tasks.edit`: atomic batch of add, edit, move, state assignment, and archive
  operations. Available as model tools and Lua through one native implementation.

An ordinary batch can finish `t17`, activate `t20`, and add two newly discovered
steps. Return the accepted list revision, changed rows and their item revisions,
new-ID mappings, totals and a bounded next-work summary. Do not echo an entire
large list for every checkbox. Explicit state assignment is preferable to toggle.
Retryable batches need a stable operation identity so repeated adds do not create
duplicates. Duplicate outcomes must be recoverable from retained operations,
without an ever-growing second resident deduplication store.

Guard edits with the versions of items they change; guard structural list changes
with the list revision. Serialize acceptance at the native owner. Different
workers can update different tasks; a stale edit of the same task returns its
current row and a specific conflict. Never silently overwrite an operator edit.
For rewind/fork, distinguish a branch-local list from an explicitly shared list:
restore branch-local state at the branch point; do not rewind shared peer state
merely because one model rewinds its conversation. Record the chosen scope.
This costs more machinery than a whole-list setter, but serves actual parallel
use. Avoid full replacement as the normal model interface.

Reads and pages identify their revision. A page cursor cannot silently mix
incompatible list revisions. Bounded change buffers can return `resync_required`
with a fresh bounded projection when older deltas are no longer available.
Distinguish an intentional empty list from no task-list state.

At turn start, continuation, and compaction, provide a small current projection
with IDs, versions, active/blocked work and relevant ancestors. Further work is
read on demand. A provider request already in flight remains an immutable request;
task updates do not pretend to change what it has seen. The next decision boundary
gets current state or a delta, and guarded writes detect intervening changes.
Link this projection to the recorded request/context revision so model readback
is explainable using the existing audit, rather than maintaining a separate log.

Encourage updates at actual work transitions, not after every keystroke or token.
Runtime starts/finishes appear independently and promptly. Fine granularity must
remain useful to the model, not become compulsory status-reporting busywork.

## Candidate operator view

Default: a bounded dock above the composer, expandable to a task tree. On a
short terminal it collapses to a summary line; width determines title wrapping
and optional detail columns. Existing chat remains readable and input stays put.
Example, with ten actionable tasks total:

```text
TASKS  Task-list feature     3 done · 2 active · 1 blocked · 4 queued
 ✓ t12  Read reference tools
 ▶ t17  Design update protocol       main · model responding
 ▶ t18  Probe narrow terminal        peer · command running 00:12
 ! t19  Choose default placement     waiting for Patrick
 ○ t20  Exercise resume path
   +5 more                         /tasks expand
```

Use semantic colors plus glyphs and words. Animate only indicators bound to
observed live activity. An active task with no attached live run has a static
marker. A recently completed row briefly stays visible so the transition can
be understood; closed work remains available in history. Completion can collapse
to a summary, but unfinished lists persist across turns and steering messages.

Expansion provides groups, search/filter, owner and blocker views, task detail,
recent changes, run results, bead links and direct editing. Selection anchors to
task identity; incoming updates do not reorder the row under the operator's cursor.
Offscreen activity gets a count/badge. Operator edits use the same version-aware
mutation path; rejected/saving states are explicit. Editing a task changes working
state immediately, while any change to the model's current turn instructions
follows Blackbird's separately agreed activation timing.

Do not put revision numbers everywhere. Show them in inspection/conflict details.
Likewise distinguish a task note such as “unchanged for 12m” from a disconnected
feed warning. A local viewer receiving no changes while nothing changes is current.
Remote viewers can show last received revision and reconnection state. An elapsed
runtime clock means elapsed time, not proof that useful work is advancing.

## Freshness, crisp rendering and bounded growth

Proposed flow:

```text
task edit → native acceptance + retained publication → current projection
         → typed UI notification + existing wake pipe → changed grid rows
```

Current Blackbird already has the relevant renderer foundation:
`include/blackbird/terminal.hpp`, `src/terminal.cpp` use queued typed messages,
a wake pipe and output-aware polling; `include/blackbird/chat_view.hpp` and
`src/chat_view.cpp` implement semantic grids, diff painting, cached transcript
rows and synchronized frames. The wake pipe can interrupt idle polling. The
busy animation timer is not a required delay for new task state.

There is no task-list message/view or task projection in current
`include/blackbird/saved_state.hpp`. `AuditLog`/retained state and current-state
reduction provide integration points, not a ready-made task-store API. Add a
typed task projection and saved-current-state support rather than a second
authoritative todo file. Accepted edits use existing retained publication;
reopening restores current state without replaying all archived task history
for every edit. Preserve immutable retained-prefix sharing.

Publish accepted task state, not speculative model tool arguments. A pending
operator edit can show “saving” independently. Preserve committed state on
failure. Ignore older UI revisions and recover gaps from the current projection.
Keep one latest pending view per list rather than an unbounded publication queue;
coalescing display notifications must not discard retained mutations.

A partially emitted terminal frame must finish before replacing it with a newer
frame. Prepare the next frame from the latest revision after draining output;
commit the painter only when its output packet is fully written. This uses the
existing backpressure discipline and avoids mixing revisions on screen.

Maintain lookup/order indexes and counts. Aim for work proportional to changed
items, affected groups and visible rows; expensive reorder/page operations have
their own explicit costs. Do not scan every task on an 80 ms animation tick or
send the entire task tree in each model result. Bound notes, mutation batches,
response pages, delta buffers and resident views. Reject oversized edits clearly
rather than silently truncating tasks. Exact bounds await real-use sizing.

Archive finished lists explicitly; keep archived payloads out of the resident
working view. Unfinished work is not silently expired. The agreed diagnostics
TTL remains 30 days and is not a task-state retention policy. Reuse existing
retained-history lifecycle decisions instead of adding a new task-history authority.

Read-only Rhizome watch: its relocated, inactive research shelf under
`projects_old/inactive-2026-10-07/rhizome/papers/` has no newer C++ findings than
the existing October 1 material. Relevant lessons remain explicit ownership,
mutable-to-published transitions, fallible publication and state/history oracles.
No new tool or library follows from this watch.

## Questions to settle in discussion

1. Default placement: compact dock above input, or a persistent side pane on
   wide terminals? Recommend the dock initially, with expansion on demand.
2. Granularity: nested organizational groups with actionable steps, or tasks
   that can themselves contain actionable subtasks? Recommend groups plus steps
   initially to avoid ambiguous completion and double-counting; revisit if real
   workflows need mixed nodes.
3. Sharing: session/workstream lists with explicit peer access, or project-wide
   live lists by default? Recommend scoped lists and optional bead links; do not
   turn every exploratory step into project-wide state by default.

After choosing the interaction shape, direct implementation checks should cover
rename identity, retry without duplicate adds, conflicting edits, failed
publication, all-blocked lists, continuation/reopen after compaction, bounded
readback on a large list, and PTY update bursts under narrow width and output
backpressure. Measure edit-to-output delay and typing responsiveness on the actual
panel. These are prospective checks, not a new assurance campaign or results.
