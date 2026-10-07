# B1 native Beads implementation

Purpose: one lazy C++ semantic adapter over externally installed bd 0.58.0,
shared by model/Lua and basic slash controls. No embedded Go, SQL, new dependency,
startup probe, initializer, migration command, service, or paint-time polling.

Mechanism: explicitly configure canonical project, executable and exact actor;
child-only cwd/BEADS_DIR with routing overrides removed. Version JSON precedes
readonly where identity verification. Owned Child machinery retains group custody,
cancellation and stdout/stderr separately. Typed argv supports ready/list/show and
standalone create/update/close/claim. A spawned write with any uncertain outcome
settles UNKNOWN in retained AttemptObservationEvent as well as its envelope; no
retry. Claim is assignment only; reread status/dependencies before working.
Explicit refresh updates cache only on success; explicit selection appends data.
Readonly bd may perform upstream maintenance; this is not strict DB isolation.

Direct oracles (fixed deadline 1791383181; build <= -j2): deterministic fake CLI
records exact argv/cwd/env; unsupported version stops before where; wrong project
unavailable; command-specific object/array/error shapes; dash/metacharacter data;
claim alone; no child before use; timeout/nonzero/malformed/overflow writes UNKNOWN
in retained facts; refresh failure preserves cache. Native fixture measures binding,
queries and fake writes (elapsed/bytes), not provider waits. One authenticated CLI
adversarial review <=90s, fixes and affected direct recheck only. No upstream tests
or operator-task mutations. Unsafe/unreviewed candidate remains inactive.

## Use and boundaries

Binding is process-local and deliberately explicit/lazy; it is not restored across
restarts. Model/Lua use `beads` with `op="configure"`, absolute `project` and
`executable`, and exact nonempty `actor`. The project must have canonical `.beads`.
Configure starts no subprocess. First query probes version and readonly `where`;
subsequent explicit queries reuse verified binding. Changing configuration resets
binding/cache. No initialization, migration or service command is issued by BB.

Slash: `/beads configure {"project":"/absolute/project","executable":"/absolute/bd","actor":"operator"}`;
then `/beads ready`, `/beads list`, `/beads show ID`, `/beads cached`, or
`/beads select ID`. Only successful explicit select appends external task data to
context. Slash and Lua/model all call the same audited typed operation.

Operations: `ready`/`list` accept `limit` 1..100 (default 20), list also `status`;
show/update/close/claim require exactly one complete `id`. Create requires `title`;
create/update support `body`, `type`, string `priority`; update also `status`;
close accepts `reason`. Standalone `claim` accepts no fields besides op/id and uses
configured actor. No explicit-ID create, external-ref idempotency, dependency
editing, labels, bulk operations, or automatic replay. Literal body `"-"` is
rejected before spawning because pinned bd treats it as stdin, not literal data.
Option values use `--flag=value`, IDs follow `--`, and no shell is involved.

Exact-ID readonly preflight precedes existing-task writes, not a transactional
status/dependency CAS. Claim checks returned assignee/status but remains atomic
assignment only; reread dependencies/status before work. BEADS_DIR disables pinned
prefix routing (`cmd/bd/routed.go`). Where verifies selected `.beads`; BB makes no
claim of isolation from concurrent upstream changes or readonly maintenance.

Independent stdout/stderr are retained as audit sources under `audit_ref`;
stdout JSON is limited to 64KiB, stderr accumulation likewise, diagnostic preview
2KiB, child deadline 5 seconds. Output shape failure after a write is UNKNOWN,
including zero-exit error objects, wrong target, NUL/oversized IDs. Nonzero can
follow an applied write. A conservative mutation-admission marker preserves UNKNOWN
for exceptions including allocation failures. No automatic retry is performed.
Cache updates only for validated successful reads; a failed refresh preserves it.
