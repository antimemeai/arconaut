# Local candidate and contrast tools

`build/release/arco-candidate POOL COMMAND REQUEST.bbm` uses native BBM2 requests/results; `REQUEST.json` explicitly selects JSON interchange; nonzero
refuses/fails without pretending success. Use through ordinary audited `exec`.
POOL and request/evidence files belong under ignored `context/`, not publication.
Source commits and concise reports are the publication surface. All commands
serialize state under a local file lock; owned runs hold a per-slot lock while
other slots can run. Do not bypass managed runs for builds/programs using a slot.
Detached/escaped clients cannot be inferred stopped from leader exit. Observe
and record that all users stopped before settlement/reuse/retirement. This is
state reconciliation, not command approval. Unrelated primary dirt is untouched.

- `init`: `{"repo":"/absolute/primary","cap":1}`. `cap` is the actual configured
  maximum, never auto-enlarged. Pilot default `limit` is 4; caller may explicitly
  configure another limit, e.g. cap=5, limit=5. Idle slots allocate on demand.
- `enqueue`: name (letters/digits/hyphen/underscore), base commit/ref, hypothesis,
  independently predeclared discriminator, identities (source/Lua/context POLICY/
  model/settings/access/colleague references), baseline, and optional parent
  candidate reference. Exact base commit freezes on enqueue. Branches default to
  `candidate/NAME`. Do not reuse candidate identities; retain/push source branches.
- `dispatch`: `{}`. FIFO queued candidates use an empty slot, creating a linked
  worktree only the first time. Full pool returns queued, never creates more slots.
  Existing slots must be owned linked worktrees, not aliases/foreign directories.
- `run`: slot, seconds 1..3600, argv array. Captures leader output in an immutable
  local record (16MiB bound); timeout/limit/process failure explicitly unknown,
  never replayed. EVEN NORMAL EXIT leaves the slot unknown until `settle` because
  leader exit is not evidence that every descendant/standing client has stopped.
- `settle`: slot, stopped="all checkout programs/builds stopped", nonempty evidence.
  Confirms expected branch, records observations, then allows work. A false caller
  assertion cannot magically prove an escaped process stopped; inspect it first.
- `checkpoint`: slot, stopped/evidence, files array (explicit relative regular
  files, no symlink escape), message. Rejects preexisting staging. Named additions
  commit on the candidate branch; clean source and exact commit are recorded.
  Empty files permits a same-source experiment checkpoint. No blanket primary add.
- `artifact`: slot, stopped/evidence, file (relative regular file). Copies bytes to
  content-addressed storage OUTSIDE execution checkout and records original path.
  Preserve all relevant artifacts before release/retire; don't commit raw output.
- `release` / `retire`: slot, stopped/evidence, artifacts="all relevant artifacts
  retained". Requires clean source and a checkpoint equal to HEAD. Release reuses
  this same slot; retire removes it but retains branches, records, copied artifacts.
- `activation`: slot, stopped/evidence, source_commit equal to checkpoint,
  qualification, effective_program, request, mode. These are ACTUAL observed
  qualified reload/native quiet-RRC references, not a source-commit certification.
  CLI records activation, does not replace a running executable or discard context.
- `abandon`: unknown slot, stopped/evidence. If checkout absent, retains existing
  branch/base and marks transition abandoned, no replay. For a present clean prior
  linked checkout: reconcile="retain clean prior checkout", observed_branch and
  observed_head must match. For a partial directory: reconcile="quarantine partial
  directory without execution" preserves entire directory outside the slot before
  freeing it. No automatic deletion/prune/repair; stale Git registration requires
  separate explicit observed Git repair. A symlink alias is always refused.
- `record`: question, action, outcome, references, plus resource/identity fields.
  Local immutable linked question/action/outcome record, no blocking utility judge.
- `status`: `{}`. Current state/record locator; inspect retained records as needed,
  not the entire trace on every turn. Git intent is NOT successful effect evidence.

## Editable Lua

Load `programs/useful_work.lua` by audited read and `load(content,...,'t')()`.
`page(cursor,count,watermark)` avoids the actual reserved `end` syntax failure;
`original(record,source,offset,limit,watermark)` pages exact immutable originals;
`find(cursor,watermark,pages,predicate)` visits at most 1..16 pages, retains causal
occurrences and returns continuation/incomplete rather than false exhaustiveness.
`number` converts runtime tagged JsonNumber explicitly; missing is not zero.

`feedback()` returns actual serialized input/last-request bytes, last available
provider usage and top3 entry sizes plus actionable options/tradeoffs. It does not
estimate tokens, dictate compaction or claim cheaper native replay. `record` and
`contrast` retain narrow question/action/outcome plus resources through the CLI;
contrast requires hypothesis, predeclared discriminator, baseline/candidate,
measurement, keep/revise/revert/inconclusive and exact references. Model judgment
is a hypothesis, not statistical evidence; actual behavior/outcomes remain separate.
Use a distinct local request file per actual attempt; failed/unknown effects are
returned, never retried automatically.

`programs/useful_turn.lua` reads the helper and `context/useful-work/steer.bbm`
with at least a `direction` string at each affected workflow boundary. Missing or
invalid steer is visible failure, not silent disregard. Operator direction persists
on disk and runtime workflow settings survive RRC. `/workflow ABSOLUTE_FILE`
selects it normally; `/lua`/ordinary model tools can configure source normally.
Explicit interruption remains native cancellation. Boundary feedback is advisory;
no forced read budget, universal utility metric, judge or approval gate. Both
arms of any pilot retain common audit/original/complaint instrumentation. Read
CONTEXT_ECONOMY_EXPERIMENT before declaring a finite matched experiment.

The finite matched pilot is complete: see papers/2026-10-06-useful-work-pilot.md
and docs/USEFUL_WORK_RECOVERY_RECIPES.md. Results are mixed; bounded-first is not
an unconditional default or demonstrated acceleration. Post-pilot `M.lines(path,
first,last)` and `M.bytes(path,first,last)` send exactly one read range mode,
addressing the six actual invalid_range failures. Model chooses ranges versus
small known complete files; checks/constraints/original capture are not weakened.
