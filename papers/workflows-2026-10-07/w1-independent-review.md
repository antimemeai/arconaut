# W1 independent adversarial review — 2026-10-07

Reviewed candidate `c8498f01d7c5b4d908d8d385d4f7e0eb4306a2a5` against
`8342bb1`. One substantive source review, bounded to five minutes. The earlier
Claude attempt returned no report and is not counted as a substantive review.
No candidate edits, upstream execution, or test campaign were performed here.
Line references below refer to the candidate revision, not the primary checkout.

Scope: callable-registry validation and activation, Lua invocation and deferred
provider-call ordering, failure propagation, immutable publication, command
catalog consumers, exact-token matching, and terminal presentation. Contracts
read: `docs/CALLABLE_WORKFLOWS.md`, `docs/W1_IMPLEMENTATION.md`; inspected the
changed implementation, focused test source, and existing runtime/operation and
context-output bookkeeping. This is not a certification claim.

## F1 — High: hot registry publication invalidates palette selection bounds

`src/terminal.cpp:72–91` rebuilds each thread's immutable command catalog when
the published registry changes. `Composer` retains `palette_selected_` across
that change. The selection is then used unchecked at lines 827, 870, and 892:
`commands[matches[palette_selected_]]`. Checking only `!matches.empty()` does
not establish that the retained index still exists. `palette_lines()` also
retains that index and can render an empty selection window after shrinkage.

Trigger: while a model turn is running, the operator opens the command menu and
selects a workflow near the bottom. The turn successfully publishes a config
with fewer workflows (including an empty registry, explicitly supported).
Without editing the query or moving selection, Enter or Tab now indexes beyond
the smaller match vector. This is C++ undefined behavior and can crash the TUI
or select corrupt data. The mutex and shared pointers protect catalog lifetime;
they do not protect the logical index into a replacement catalog.

Minimal fix: normalize selection whenever its catalog/query result changes, and
bound the index immediately before every use. Rendering should derive its
window from the same bounded selection. Preserving selection by command name
is optional; resetting/clamping on publication is sufficient. Keep one catalog
snapshot per operation as the implementation already does.

Direct regression oracle: open and move the palette/slash menu to a workflow
index beyond the built-in count, publish an empty or shorter registry, then
Enter/Tab without further movement. Selection must remain a valid command and
rendering must show a valid selection. No second review is needed to recheck it.

## F2 — Medium: workflow invocation-cap failure bypasses the failure latch

`src/coding.cpp:1145–1146` rejects the 65th registered execution **before**
the `try` whose catch assigns `workflow_failure_` (lines 1169–1171).
At successful turn completion, the explicit workflow-failure check is at
lines 1862–1863. The invocation-count guard does not set `capacity_stopped_`
either. Lua can catch the resulting bridge error with `pcall`; a later ordinary
bridge call resets its runtime failure slot. Therefore the outer turn can
succeed and activate a staged configuration after this registered-invocation
failure, unlike failures occurring inside the guarded execution body.

Trigger: an outer Lua program stages a valid config, performs 64 synchronous
invocations of a trivial registered workflow, catches the 65th invocation's
capacity error, and returns normally. Neither `workflow_failure_` nor
`capacity_stopped_` forces failure at the boundary. This contradicts the stated
failure-preserves-effective-configuration contract and the deliberately latched
child-failure behavior already tested in `tests/workflows_test.cpp`.

Minimal fix: put the invocation/depth guard inside the existing failure-latching
`try`, or explicitly latch its error before throwing. Do not rely on an uncaught
Lua error: `pcall` is normal programmable workflow behavior. Preserve the limit
itself and avoid silently resetting the counter.

Direct regression oracle: with fake effects and a trivial registered workflow,
stage config, catch invocation 65 with `pcall`, perform a harmless subsequent
host call, then return. The outer turn must fail and effective registry/config
must remain unchanged. The subsequent host call distinguishes the explicit
engine latch from incidental runtime error-slot persistence.

## Other inspected paths

No additional actionable finding established within this review's bound.
Registry snapshots own their definitions; continuations copy selected source,
arguments and revision, preserving the selection across pending edits.
Unanswered provider outputs defer invocation until protocol validation succeeds;
normal successful boundaries drain the queue. Queues are discarded on exit and
are not represented as a durable scheduler. Publication uses immutable shared
snapshots with a mutex; no new worker concurrency is claimed. Exact ASCII token
matching deliberately treats non-ASCII bytes as word constituents and excludes
assistant/tool dispatch, consistent with the written contract.

The supplied focused tests cover many matching, activation, source-retention and
ordinary child-failure cases. They do not exercise either catalog replacement
with an already selected menu index or caught invocation-count exhaustion.
