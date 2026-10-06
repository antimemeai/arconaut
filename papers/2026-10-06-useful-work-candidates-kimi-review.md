# Unit2 Consequential Review — candidate manager + useful-work Lua

Scope inspected: `src/candidate.cpp`, `tests/candidate_test.cpp`, `programs/useful_work.lua`, `programs/useful_turn.lua`, `tests/useful_work_test.lua`, `docs/USEFUL_WORK_CANDIDATE_SUBPLAN.md`. Read-only; no execution.

## Fault 1 (consequential): crashed/failed dispatch transition is an unrecoverable dead-end — slot permanently locked out of the pool

**Location:** `src/candidate.cpp` lines 209–215 (dispatch), 221–232 (abandon), 233–238 (settle), 239/160 (leased gate).

**Mechanism:** `dispatch` mutates state to `slot=unknown`, `candidate=transition` and durably saves `dispatch.intent` (lines 209–210) *before* the git effects (`worktree add` / `checkout -b`, lines 211–212). If `git` throws, the process is killed, or power is lost anywhere between the intent save and `dispatch.observed`, the slot/candidate stay `unknown`/`transition`. Now enumerate the recovery commands:

- `settle` (line 235): requires `git checkout(i) branch --show-current` == candidate branch. If `worktree add` never completed, `checkout(i)` itself refuses ("slot is not an owned linked worktree", line 128) or git errors; if `checkout -b` failed, branch mismatch refuses. Dead.
- `abandon` (line 224): refuses whenever the checkout directory exists ("inspect/record/retire it, do not abandon"). But there is **no inspect/record/retire command reachable** — `checkpoint`/`release`/`retire` are all gated behind `leased(i)` (line 239), which refuses `unknown`. Dead.
- Re-`dispatch`: slot is not `empty` (line 201) and candidate is not `queued` (line 204), so the candidate is also permanently undeployable; with cap=1 (the default serial slot), the entire pool is bricked.

**Trigger:** any interruption of `worktree add`/`checkout -b` mid-dispatch (disk full, signal, git failure leaving a partial directory at `pool/slot-i`, or a leftover directory from a prior crash).

**Consequence:** permanent loss of the only serial slot and of the candidate, with no evidence-linked reconciliation path — exactly the "interrupted transition requires explicit evidence-linked reconciliation" case the subplan (docs line 20) demands but the CLI cannot express.

**Fix:** add a `reconcile` (or widen `abandon`) command that, given explicit stopped + inspected evidence, accepts a present-but-unowned/partial checkout: records the observed leftover state, optionally `git worktree remove --force` / directory removal as an *explicit, evidence-bound* caller choice, marks candidate `abandoned transition; retained source/records`, and frees the slot. Also make `settle` tolerate `checkout(i)` refusal by surfacing the raw observation instead of throwing before evidence can be attached.

**Missing direct oracle:** `tests/candidate_test.cpp` exercises neither `abandon` nor any dispatch-failure recovery. Add a test that kills/fails dispatch mid-transition (e.g., pre-create a plain non-worktree directory at `slot-0`, or a request whose `base` checkout fails after intent) and asserts a defined evidence-linked recovery path exists and does not replay the effect.

## Fault 2 (moderate): crash inside `save("run.intent")` leaves slot reading `leased` with a durable run-intent record and no unknown marking

**Location:** lines 245 (`set(s,"status","running"); auto id=save(...)`), 142–156 (`save` writes record file first, then `state.json`).

**Mechanism:** `save` is not atomic across its two writes: the immutable record (containing the *new* state with `running`) is durable before `state.json` is renamed. A crash between them leaves `state.json` showing `leased` while `records/event-*` proves a run intent existed. On next invocation the manager reads only `state.json`; `leased(i)` passes, so `checkpoint`/`release`/`retire`/another `run` are all accepted even though a run was intended and its checkout effects were never observed stopped. That violates the stated invariant "all runs remain unknown until caller explicitly observes all checkout programs/builds stopped."

**Trigger:** kill/power loss in the narrow window between the record rename and the `state.json` rename inside `save("run.intent", r)`.

**Consequence:** checkout-preservation and unknown-replay guarantees silently downgrade: a slot whose program may have run (or partially run) is treated as settled; a subsequent `run` on it is a replay of an unobserved-effect slot.

**Fix:** on startup (Manager ctor) or before any `leased`-gated command, reconcile: if the newest record's embedded state differs from `state.json`, or a `*.intent` record lacks its matching `*.observed`/`*.observation` record, force the affected slot to `unknown` and require `settle`. Alternatively write `state.json` first and treat orphan intents as unknown on load.

**Missing oracle:** no test asserts that an intent-without-observation blocks `leased` commands. Add one that fabricates `run.intent` + stale `state.json` and asserts `checkpoint`/`release` refuse until `settle` with stopped evidence.

## Fault 3 (minor): pool nested inside the primary repo is not rejected

**Location:** lines 167–169. Only `fs::canonical(root)==primary` equality is refused. `init` with `pool = repo/sub/pool` passes; `slot-0` then lives inside the primary checkout. Depending on git version, `worktree add` inside another worktree is either refused late (after intent is saved → feeds Fault 1) or accepted, putting candidate checkouts inside the primary tree where the "primary dirt never touched" invariant is harder to reason about and `inside()`/`clean()` path checks still pass.

**Fix:** refuse init when `root` is lexically inside `primary` (and vice versa) using `lexically_relative`, same style as `inside()`. Missing oracle: no test for nested pool.

## Fault 4 (minor): candidate status not set `unknown` alongside slot after `run`

**Location:** lines 259–262. After a run, `slot(i).status = unknown` but `candidate.status` remains `leased` (only `settle`/`abandon` reconcile it). `status` output therefore shows a candidate as `leased` while its slot is unsettled; `abandon` (which reads candidate, line 225) and any future consumer of candidate status see a misleading settled state. Consequence is limited because all effectful commands gate on slot status, but it weakens the record as an oracle. Fix: set `candidate(n).status = "unknown"` in the run-observation save, mirroring slot.

## Lua files

`programs/useful_work.lua`, `programs/useful_turn.lua`, `tests/useful_work_test.lua`: no consequential faults found in scope. Checked specifically: `M.record`/`M.contrast` never retry on `timed_out/unknown` (test line 35 asserts single exec/write); `M.find` honors the 1..16 page budget and returns `complete=false` with limitation rather than claiming exhaustiveness; watermark passes through the reserved `end` key losslessly (test line 22); `M.number` errors on missing instead of zero (test line 20); `contrast` refuses retrofitted discriminators (test line 31). Minor observations (not faults): `useful_turn.lua` line 3 hardcodes a cwd-relative path (documented "configure in source"), and line 18 `return`s silently at `calls==0`, so a turn ending without calls leaves no record — acceptable as documented budget behavior.

## What I did not verify

No execution (read-only allowance); green-test claims taken as stated. `native_process.hpp`/`json.hpp` semantics (e.g., `Child` destructor actually killing escaped descendants, `collect` timeout behavior) were assumed per comments. Fault 2's window is narrow and timing-dependent; its existence is structural (two-write `save`), not timing-independent.
