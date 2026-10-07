# Custom Lua failure repair: operating checklist

Use this checklist when a custom workflow fails or leaves unanswered tool calls. Repairing the model-facing presentation is not proof that a process was contained or that its effects succeeded.

## 1. Stop and inspect existing evidence

- Do not replay the failed workflow, a timed-out exec, or earlier calls merely to obtain a cleaner result.
- Inspect existing tool results and retained process output first. A timeout with `effect_outcome=unknown` remains unknown; a written marker proves only that the marker was written, not that the command finished or was contained.
- Preserve original entry IDs, call IDs, output references, program locators, and audit/source references. Keep originals available; do not overwrite history to make the failure look successful.
- Record separately what is observed, what remains unknown, and what evidence supports each conclusion. A `workflow_result_missing` placeholder is not an execution result.

## 2. Bind repair to the current context

- Inspect the current editable context and obtain its base revision.
- Use that exact base for context edits or explicit protocol repair. A stale-base conflict means inspect again and reconsider the proposal, not force an overwrite.
- Keep live user/system/developer instructions and valid tool call/result linkage. Do not remove an active call while retaining its result, or fabricate a successful result.

## 3. Run an explicit recovery workflow

- Select a known-working recovery Lua workflow that does not dispatch the failed effects.
- Before the next provider request, invoke `context_repair` with the current base for calls already unanswered when the recovery workflow began.
- Repair appends unknown-result placeholders while preserving originals and existing outputs. It does not replay tools, settle effects, or prove containment.
- Respect refusal conditions: active-workflow calls, malformed linkage, and live owned children are not repaired by inventing results. Investigate the blocking condition separately; do not treat refusal as permission to retry.

## 4. Select a working workflow before continuing

- Select a known-working normal workflow before making a new useful request. Governing workflow file changes activate on the next turn, not midway through the current workflow.
- Choose independent work that does not depend on the uncertain effects. If dependent work is necessary, first obtain concrete evidence or an explicit decision about how to handle the uncertainty.
- State the repaired presentation status separately from process containment and effect status. Keep unknown effects unknown until supported by evidence.

## Done criteria

- Existing results were inspected without replay.
- Original locators remain available and linked to the uncertainty record.
- Explicit repair used a current strict base and preserved protocol linkage.
- A working workflow is selected for the next useful request.
- No claim of successful execution or containment rests solely on presentation repair.

Documentation note: the requested read of `docs/USING_ARCO.md` lines 200–249 returned `invalid_range`; those lines were not retrieved. This checklist is based on the available native tool contracts, not a verified reading of that section.
