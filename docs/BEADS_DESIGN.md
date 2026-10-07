# Native Beads integration

Operator2026-10-07 requests sketch, design, then Blackbird autodev. First backend
is installed external bd0.58.0. Native means C++ semantic tools, Lua and terminal
controls share an implementation; this pass still requires bd. No Go code or
database dependency enters the harness. Direct Dolt is a separate measured design.

Grounding: [study](../papers/2026-10-07-beads-integration-study.md), pinned upstream
ae14933, CodingEngine::operation, complaint delivery, native child lifetime and
slash dispatch. Actual ready/claim/version-commit semantics guide the interface.

## Boundary and ownership

Beads owns issues, relationships and ready/claim arbitration. Blackbird consumes
it; no Dolt governance, automatic DB initialization/migration/copy or new control
plane. Task status is distinct from campaign/process/worktree ownership. A turn
finishing does not close a task automatically.

Shared typed service: validate fields, build argv, parse command-specific JSON,
return a result envelope. Model/Lua tool `beads`; `/beads` uses the same path and
appears in slash discovery/help. B1 does not require a full task browser. Selected
task content enters context explicitly as data, not an instruction replacement or
automatic backlog dump. Normal model programmability remains via existing exec.

Lazy project binding: canonical project/.beads, executable, bounded deadline and
result size, actor. Explicit configuration overrides first-use repo discovery;
candidates can bind primary. Verify readonly JSON `where` once per binding.
Mismatch is unavailable, not permission to use another DB. Child-only BEADS_DIR
binds database AND config.yaml; control inherited routing overrides. No global
chdir/setenv. No bd probes, task loading or DB I/O during startup or keystrokes.

## Surface

| Operation | Semantics |
| --- | --- |
| ready | Actual blocker-aware bd ready; bounded count |
| list | Explicit stored-status/filter query, not equivalent to ready |
| show | One explicit issue ID; response array |
| create | Title/body/type/priority; standalone creation |
| update | One explicit ID and fields, never implicit last-touched issue |
| close | One explicit ID/reason; only observed success marks closed |
| claim | One explicit ID, unique actor, claim alone; inspect task/dependencies afterward |

Dependency edits, comments and richer browsing follow independently. Do not
combine claim with fields or create with relationship setup: composite commands
can partially succeed. Ordinary update has no general CAS; assignment is not an
expiring lease. External refs/explicit IDs are not exactly-once create keys;
recreating an explicit ID can overwrite another issue. Do not invent pagination
unsupported by this bd version. Exact validation/argv generation belongs in code.

## Effects and presentation

Use retained effect admission before spawn, linking intent/project/actor/target
and source/revision/attempt to actual response. External task ID does not replace
Blackbird operation identity. Envelope distinguishes unavailable/invalid before
spawn, successful exit with command-shaped JSON, rejected query and unknown write.

Nonzero exit, timeout, cancellation, malformed/truncated stdout or lost settlement
after spawn can leave a mutation applied. Preserve uncertainty in BOTH result and
retained attempt disposition. No blind mutation retry. Explicit reread establishes
desired state observed, not exactly-once operation identity. Read retries are
deliberate/bounded. Raw stdout and stderr are distinct; never parse merged stderr
as JSON. Use separate bounded channels or honestly record a diagnostic gap.

Beads calls participate in cancellation/refit quiescence. Missing CLI/DB is an
ordinary unavailable integration. Cached snapshots label freshness; failed refresh
preserves prior data and exposes error. No polling per paint, background service
or all-task context. Measure binding/query/mutation latency and bytes apart from
provider wait. Keep audit originals while bounding model-visible results.

## B1 implementation orders

One whole native semantic boundary, lazy project binding, model/Lua dispatch and
basic slash operations. Prefer seven operations above; if25minutes forces smaller
delivery, retain a complete read path and leave writes explicitly unadvertised.
No unsafe mutating placeholder, full browser, scheduler, SQL or complaint rewrite.

Plan before code; use actual pinned reference sources. Direct fake-process oracles:
argv/environment/project isolation; object/array/error shapes; standalone claim;
zero startup subprocesses; shell metacharacters remain data; timeout after simulated
write; unknown settlement; failed refresh. Installed CLI comparison is READ-ONLY.
Mutation checks use fake backends or disposable isolated DB, never operator tasks.
Only affected checks, one substantive code review/fixes and direct recheck within
fixed25minutes. No third layer, mutants or allowance reset. Commit/push candidate
and report measurements/gaps. Root integrates serially; primary UI/build stays live.
