# W1 — callable workflows and POWERWORDS

Operator authorizes this first core slice2026-10-07. Purpose: turn authored Lua
workflows into discoverable actions for humans and models, without building a
durable scheduler before we can use registered workflows. Implementation follows
the fresh Hermes and ecosystem studies, especially shared callable definitions,
stable invocation identity and explicit activation boundaries.

Use one registry for dispatch, model/Lua discovery and TUI command completion.
Each definition identifies its name, description, source and invocation settings.
Retain effective source/configuration with the invocation, using existing audit
and CodingEngine paths. Models may author/edit definitions during normal work.
Registry changes activate after the affected turn/workflow; do not evaluate Lua
or reread files on every composer keystroke.

Slash names are configurable: a registry prefix can produce `/wf-research`, or
no naming prefix can expose `/research` directly. Optional bare invocation is
explicit per definition. Built-in command collisions and ambiguous aliases are
reported, never silently shadowed. Registered commands appear in the slash menu,
arrow navigation, completion and help with their descriptions.

POWERWORDS are configurable exact tokens, such as `ultracode`, that select a
workflow or named workflow behavior. Give them configured distinct TUI coloring
in the composer and submitted user text. Specify matching position/case behavior;
never match a substring such as `notultracode`. Activation comes from submitted
operator text, not assistant/tool output. Preserve the user's prompt and record
the matched trigger and chosen definition. Conflicting matches must have an
explicit deterministic policy. Color is presentation, not the dispatch oracle.

Deliver one working `ultracode` example, useful self-development instructions,
registry discovery/invocation from Lua/model and CLI, and documentation of exact
supported behavior. Reuse existing effects, context and successful-workflow
activation; no new library, connector catalog, external service governance or
claims of durable parallel execution. Efficient cached discovery and incremental
rendering matter; measure real work rather than guessing.

Direct checks: slash alias/prefix dispatch, arguments/prompt preservation, built-in
collision rejection, exact POWERWORD boundaries, matching/ambiguous policy,
shared discovery, reload boundary, and colored rendered cells without unrelated
redraw. Use fake providers for deterministic dispatch tests; no paid inference
needed to prove registry routing. Existing relevant regression checks only.

BB writes its short source-grounded subplan before coding, implements one whole
unit, runs direct checks, obtains one independent code review, fixes findings and
rechecks once. Absolute25-minute allowance; no recursive certification. Commit
and push the candidate even if a remaining gap must be reported. No implicit
master merge. Performance collection starts with the run, raw evidence stays in
ignored context, useful outcomes/measurements belong in the journal/report.
