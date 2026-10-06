# Two observed-state recovery recipes

Derived from the finite pilot's accepted source-grounded deliverables; no command
below is automatic replay or proof of process cessation. These are CLI recipes,
not permission gates. Raw retained output/audit stays local.

## Managed run leader exited0; checkpoint/reuse pending

Both candidate and slot remain **unknown**, even on0 (`src/candidate.cpp:384-428`).
Inspect the actual run/output and pool/slot/expected branch. Establish that ALL
checkout programs/builds, escaped descendants and standing checkout users stopped.
Leader exit and an available slot lock do not establish that. If unresolved, retain
unknown and continue independent work; do not rerun or switch the checkout.

Then submit a distinct local request file with actual observations, not placeholders:

```json
{"slot":0,"stopped":"all checkout programs/builds stopped","evidence":"ACTUAL observed cessation evidence and local references"}
```

`build/release/arco-candidate POOL settle REQUEST.json` checks the expected branch
and records the observed transition back to leased. It validates assertion shape,
not the truth of escaped-process cessation. Subsequent checkpoint needs explicit
regular relative files, clean source and no preexisting staging; empty files records
same source. Preserve relevant artifacts before release/retire, requiring checkpoint
at HEAD and `artifacts="all relevant artifacts retained"`. Source checkpoint does
not establish executable qualification/activation (`:430-501`).

## Unknown failed dispatch; existing partial real directory

Do not execute, delete/reset, retry dispatch, or automatically prune Git registration.
Observe all users stopped; confirm a real partial directory rather than a symlink
or clean linked checkout. Submit a distinct observed reconciliation request:

```json
{"slot":0,"stopped":"all checkout programs/builds stopped","evidence":"ACTUAL observations and local references","reconcile":"quarantine partial directory without execution"}
```

`build/release/arco-candidate POOL abandon REQUEST.json` preserves the WHOLE
directory outside execution before freeing the slot; source branches/base and
records survive (`src/candidate.cpp:323-369`). Observe the result and retained
quarantine path. Intent is not completed effect; unknown recovery must not be
replayed automatically. Symlink aliases are refused. A clean prior linked checkout
uses the separate exact observed-branch/head reconciliation, not this recipe.
Stale registration can require separately observed explicit Git repair; abandonment
never implies successful dispatch, a runnable checkout or qualification.

## Avoid the actual mixed-range inspection failures

Ordinary model/Lua interaction may load `programs/useful_work.lua` and call
`M.lines(path, first, last)` (inclusive,1-based) or
`M.bytes(path, first, last)` (half-open,0-based). Each sends exactly one range mode,
not byte AND line fields. Six pilot read calls mixed modes and were rejected;
these typed helpers were added afterward, independently checked and actually loaded.
This is a useful fault correction, not a pilot treatment or certified speedup.
