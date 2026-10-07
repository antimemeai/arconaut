# G6 orchestration operating map

## Scope and ownership

`programs/orchestration.lua` is a **current-workflow, synchronous bookkeeping module**, not a worker scheduler. Create a scope with `new({ owner = ..., max_participants = ..., max_attempts = ... })`. Owner and participant identities are nonempty strings of at most 256 bytes; both limits must be finite positive integers.

- `run(id, action)` admits a unique participant and invokes its callback inline, unless paused or out of attempt budget.
- Only one callback can be active in a scope. Nested `run`/`retry` dispatch while it is active is rejected.
- The callback receives `{ owner, id, attempt }`. The owner is explicit attribution, not an authorization or native lifecycle mechanism.
- Participant and attempt budgets are scope-wide. A paused or budget-limited `run` still consumes a participant slot, but not an attempt.
- `get` and `join` return copied records; they do not expose mutable internal rows.

## Cancellation and pause

`pause(reason)` records a reason and prevents subsequent callback invocation. Blocked rows become `paused`. It **does not interrupt an active call, cancel remote work, or establish that remote effects stopped**.

There is no resume or cancellation method. Native call lifetime and cancellation remain with the owning runtime/caller; this module adds no cancellation observation.

## Outcomes: assertions versus observations

Callbacks report a status (`completed`, `refused`, `failed`, or `unknown`) and remote disposition (`completed`, `not_dispatched`, or `unknown`). A `completed` status requires a `completed` disposition; `unknown` requires `unknown`. Outcomes must also pass the module’s copying constraints.

These are **callback assertions**, not independent native observations. The module observes that it invoked a callback, whether it returned or raised, and whether its returned value passed validation. It does not independently observe provider dispatch, completion, or cancellation.

Before invoking a callback, remote disposition becomes `unknown`. Exceptions and malformed outcomes produce an `unknown` row; catching an exception never proves nonexecution. Invoked attempts are recorded in history; pause/budget blocks are not.

## Retry

`retry(id, action)` is allowed only for an existing row currently marked **`refused` / `not_dispatched`**. It consumes the shared attempt budget and increments that participant’s attempt number.

There is no automatic retry. Unknown outcomes require separate reconciliation, but this module supplies no reconciliation API. Retry eligibility relies on the callback’s explicit non-dispatch assertion, not native verification.

## Branching and joining

`branch` requires an explicit boolean and runs only the selected branch.

`join(ids)` reads a nonempty, dense, duplicate-free selection of known participant IDs, preserving selection order. It returns `completed` only when every selected row has status `completed`; otherwise it returns `unsettled`. It does not wait, dispatch, cancel, or reconcile anything.

## Unavailable here

There are no hidden workers, parallel execution, queues, restart/replay, inherited admission, detachment, feed triggers, or durable recovery. The broader participant and station/campaign document describes requirements and proposals—not capabilities implemented by this module.

## Useful next application

Use it for a **bounded, sequential review workflow**: run two narrowly scoped colleague callbacks through the owning runtime’s existing call path, retain their reported outcomes, and join those identities before synthesis. If either outcome is uncertain, expose the join as unsettled rather than automatically repeating the call.

## Actual G6 use

This map was produced by an actual addressed OpenAI context-only colleague call,
`g6-ownership-c7d34276-1`, through the G5 independent candidate executable and
ordinary retained native exec. Selected context was the current orchestration Lua
source and bounded participants brief, not inherited conversation. Actual model
`gpt-6.1-sol`; usage4319 input/766 output/5085 total tokens as reported by provider;
account cost unavailable. Source read -> selected branch -> colleague -> join
completed. One request, no retry. Captures remain at
`context/g6/ownership-call-c7d34276`; outer workflow observation at
`context/g6/actual-orchestration.json`. Completed answer is not correctness proof.
G5 heterogeneous access remains blocked; no other provider was tried by G6.

## Module use

Publish programs/orchestration.lua as a retained named module with program_config;
load on a later successful boundary using `arco.module("orchestration")`.
The proposal is a complete module snapshot: preserve other desired modules.
A callback uses ordinary arco.call and translates actual returned observations
into the documented status/disposition contract; it must not declare a timed-out
effect not_dispatched. Limits bound callback attempts, not native tool calls inside
one callback. Bound tool counts/timeouts separately in the callback/profile.

```lua
local O = arco.module("orchestration")
local scope = O.new{owner="my-work", max_participants=3, max_attempts=3}
local a = scope:run("read", function(identity)
  local read = arco.call("read_file", {path="README.md",line_start=1,line_end=30})
  assert(read.content, "source unavailable")
  return {status="completed",remote_disposition="completed",content=read.content}
end)
local selected = scope:branch(a.status=="completed", "analyze", function()
  -- Perform a configured colleague/tool call here, retain its actual outcome.
  return {status="refused",remote_disposition="not_dispatched",reason="no adapter chosen"}
end, "defer", function()
  return {status="refused",remote_disposition="not_dispatched",reason="source unavailable"}
end)
local joined = scope:join{"read", selected.id}
-- joined remains unsettled here; selecting a branch is not completed work.
```
