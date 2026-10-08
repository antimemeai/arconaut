# Task list implementation

Operator2026-10-08 authorizes building the researched task-list candidate, with
the pane at far right below the sprite and exactly one level of subtasks.
Source/literature basis: papers/2026-10-08-task-list-study-and-design.md.

First delivery: one session-scoped list, native retained delta edits with stable
IDs and version guards, bounded read/model context, Lua/model tools, operator
commands, persistent current-state reduction, and independently navigable pane.
Parents close explicitly; the summary counts parents and exposes child rollups.
No new library. Peers using the same owner receive the same edit semantics;
project-wide synchronization and arbitrary cross-session sharing are later scope.

Implement state and direct state/reopen checks, then coding integration and actual
provider-request checks, then terminal integration and PTY/render checks. Explicit
runtime task bindings must describe observed activity without completing tasks.
Use current retained publication and output backpressure paths. Bound batches,
read pages and pending display state; avoid full-list replacement per task edit.

Direct fault cases: third-level nesting, stale same-item edits, disjoint edits,
duplicate adds on retry, failed retained publication, parent/child counts, empty
state, compact reopen, context projection after compaction, narrow/wide layout,
input focus, task scrolling and update visibility. Check affected existing coding,
saved-state and terminal tests. Two layers only: implementation/direct checks,
then one focused recheck/fixes. Initial work allowance: roughly one implementation
session plus its focused checks; no unrelated performance/assurance campaign.
Record actual delivery and any remaining scope here and in the journal.

## Delivered 2026-10-08

Implemented native `TaskState`/`TaskStore`, retained task deltas reduced into the
existing saved program projection, model `tasks_read`/`tasks_edit`, Lua
`blackbird.tasks.read/edit`, and `/tasks` operator editing/navigation. Right pane
sits below the existing12x6sprite; composer uses the left region. Narrow terminals
have a summary and expandable task view. Parent counts/child rollups, stable IDs,
version guards, bounded retries and explicit exec task bindings are operational.
Usage and precise limits are in [TASKS](TASKS.md).

Regular row edits stage only changed rows and affected rollups, without copying
the task map or ordered vector. Structural batches copy/reorder the bounded
current ID vector. UI holds one current derived state under its existing mutex;
there is no task-publication queue. Cached pane rows change on state/viewport/width
updates rather than on every animation tick. Task-history payloads remain in the
existing journal, and compact reopen restores the reduced current projection.
These describe source behavior, not measured speed claims.

Direct state/provider/render cases and a real PTY check cover the selected fault
cases, a512item list, bounded one-row results, retry-window expiry, context editing,
recovery admission and compact reopening. PTY checks observe actual right-pane
cells, folding, blockers, bound running/failed activity, a60edit burst, input cursor
and narrow expansion. Existing affected coding/Lua/saved-state/terminal oracles
also run. MacOS debug/release/ASan+UBSan and Neuroses Linux debug/release/ASan+UBSan
checks cover this unit; the final small fixes receive the same affected direct
checks. Changed-source clang-tidy passes; its initial whole-unit invocation also
reported existing findings outside the changed lines, which are not reopened here.

Focused recheck fixes: explicit optional-order access, bounded conflict responses,
runtime metadata shown before long owner names, a persistent pane navigation hint,
and avoiding anchor scans on nonstructural updates. Test fixtures now use the real
recovery admission sequence and the existing exact-number Lua representation.
The macOS debug cache initially selected an old SDK; configure with the current
Xcode SDK rather than changing native process code to accommodate it. The Linux
driver now bundles the pane oracle, enables instrumentation's declared debug flag,
and supports scoped target/profile selection with accurate output.

Scope kept explicit: one list per session and one native sequential owner; busy
operator mutations follow the existing command queue. Cross-session/remote peer
transport, dependency scheduling, task-specific historical browser and branch/fork
task policies remain separate future work. Existing audit inspection can read
retained edits and bound operation results. Runtime badges are live observations,
not restored activity promises. No new production dependency or assurance loop.

Built the optimized executable and atomically published it as the interactive
launcher's `build/release/blackbird-ui` generation. The real launcher passes the
same task-pane PTY oracle after fixing its shutdown wait to continue draining
the launcher's output. Existing running instances load it through their normal
restart path; their processes were not interrupted by this work.
