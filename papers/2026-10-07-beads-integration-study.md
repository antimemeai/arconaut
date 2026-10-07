# Beads integration: source-grounded first boundary

2026-10-07. Six-minute research allowance; read-only inspection of installed
`bd 0.58.0 (Homebrew)` and matching upstream source. No live issue writes,
upstream execution, installers, tests, service reconfiguration or dependency
adoption. The current upstream repository redirects from `steveyegge/beads` to
[gastownhall/beads](https://github.com/gastownhall/beads).

## Recommendation

Build a native C++ **typed adapter over the installed `bd --json` CLI** first.
This is construction scaffolding around an existing operator tool, not importing
Go into the harness. Native model tools, slash commands and the task view share
that adapter and its cached typed results. Preserve Beads as the task authority;
Blackbird supplies intent, effect audit, presentation and task selection.

Start with bounded `ready`, `list`, `show`, `create`, single-ID `update`, `claim`,
`close`, and dependency add/remove. Discovery includes explicit backend identity,
installed CLI version and capability support. Use argv spawning, never shell
interpolation; retain independent stdout, stderr, exit status, truncation and
timeout observations. Missing CLI/database is an ordinary unavailable result.

Do not run CLI probes, launch Dolt or fetch task lists on every keystroke or
startup critical path. Bind integration lazily; render cached task data, label
its freshness, refresh on demand and after observed mutations. Failed refresh
must not erase the last useful snapshot. A Beads task list is not a provider
context obligation: inject selected task summaries explicitly.

## What the actual boundary returns

Installed read-only observations: `bd --readonly --json where` returns an object
with `path`, `prefix`, `database_path`; `ready --limit 2`, `list` and `show ID`
return arrays. A missing `show` printed a human diagnostic to stderr and a JSON
error object to stdout. Parse commands separately rather than treating every
successful response as the same shape or stderr as JSON.

Matching source: `cmd/bd/create.go:766` emits one issue object;
`cmd/bd/update.go:427–455` emits an array of successfully updated issues. Multi-ID
update continues past failures and exits nonzero only when no issue was updated:
exit zero is not an all-targets-success guarantee. First adapter mutation calls
should therefore address one explicit ID. Never use the implicit last-touched
issue behavior documented by `bd update --help`.

`bd ready` uses blocker-aware `GetReadyWork`; installed help explicitly says
`bd list --ready` merely filters open status and is not equivalent. In-progress,
blocked, deferred and hooked items are excluded by normal ready selection.
Dependencies and parent-child structure are separate semantics. Dependency
direction is `dep add TASK PREREQUISITE`: TASK is blocked by PREREQUISITE, not
the reverse. Preserve upstream ready semantics instead of approximating them
from a locally sorted list.

## Parallel agents and claims

`bd update ID --claim --actor UNIQUE_AGENT` is the upstream arbitration primitive.
`internal/storage/dolt/issues.go:583–656` starts a SQL transaction, performs a
conditional update only where assignee is empty/null, records the claim event,
and commits. Competing selection from `ready` is not a claim; do not start work
before observing claim success. Give each Blackbird instance a distinct actor.

The predicate checks assignee, **not expected status or blocker state**. A ready
observation can become stale before claiming; after claim, inspect current task
and dependencies before admitting the unit. This does not create a generic
lease: no expiry, release policy, ownership heartbeat or reaping guarantee was
established. Do not describe assignment as an OS worktree/process lease.

Claim plus other update fields are separate operations in `cmd/bd/update.go`;
claim can succeed before a later field update fails. Send claim alone. Ordinary
updates have no general expected-revision/CAS flag in installed help. A prior
`show` plus later `update` is not an atomic conditional update. Ownership reduces
contention; surface conflicts/observed changes rather than inventing protection.

## Unknown mutation outcomes and retry

Nonzero exit, broken stdout, timeout and cancellation do not prove no write.
In matching `internal/storage/dolt/transaction.go:41–91`, SQL changes commit
**before** a separate `DOLT_COMMIT` on a pool connection. The subsequent version
commit can fail after changes became visible. The older design note's atomic
SQL-plus-version-history description is therefore not a universal guarantee of
the actual implementation. `ClaimIssue` uses a different inside-transaction
version-commit path. Dependencies also SQL-commit before their version commit.

`cmd/bd/create.go:577` creates the issue before adding parent/dependencies/labels;
a single create invocation can leave a partially completed semantic operation.
Treat issue creation and relationship setup as independently observable steps.

Audit mutation intent/target/actor and the raw outcome through Blackbird's
existing effect boundary. After uncertain mutation, reread the concrete target
and dependency facts. Report desired state observed versus operation identity
proven; they are different claims. Do not blindly retry append-notes/comments,
fresh-ID create, claim, or composite operations.

The schema has an index, not a uniqueness constraint, on `external_ref`.
Metadata/external references can correlate a retained Blackbird operation ID,
but are not an exactly-once mechanism. Explicit issue IDs likewise are **not**
automatically safe idempotency keys: `insertIssue` at
`internal/storage/dolt/issues.go:1144–1190` uses `ON DUPLICATE KEY UPDATE`,
replacing title, status, assignee and other content. Reissuing create with an
existing ID can overwrite intervening changes. Reconcile before retrying.

## Project and worktree binding

This project's installed identity resolves to
`/Users/patrickbeam/projects/blackbird/.beads`, prefix `arconaut`, Dolt server
mode, database `arconaut`, configured port **13308**. The legacy prefix/database
name is valid; renaming the harness does not authorize database migration.

`internal/beads/beads.go:345+` discovers worktree redirect, worktree-local
database, then main-repository fallback. `BEADS_DIR` takes precedence;
`BEADS_DB` is deprecated. Discovery stops at repository boundaries and supports
one redirect level. Existing candidate worktrees can contain tracked metadata
without the primary live database, so implicit working-directory discovery is
inappropriate for this armada. Configure the adapter with the canonical primary
project and explicit `BEADS_DIR`; verify `where` identity once on activation.
Allow explicit separate-project binding later. Never initialize a missing
database automatically, copy `.beads`, use the workspace parent as a repository,
or assume default Dolt port 3307.

Exact child binding: `internal/config/config.go:43–49` also gives `BEADS_DIR`
priority for **config.yaml selection**, while `internal/beads/beads.go:274–286`
uses it for database discovery. `cmd/bd/main.go:479–506` derives `beadsDir` from
that database path, then loads metadata/configfile database name and resolves
server port via `doltserver.DefaultConfig(beadsDir)`. Read-only `where` from the
workspace parent, with `BEADS_DIR` set to the absolute primary `.beads`, returned
the canonical primary identity. Use a child-only environment overlay and child
working directory; never process-global `chdir`/`setenv`. An absolute
`--db .../.beads/dolt` supplies a storage path, but by itself does not ensure
early YAML config discovery uses that project's config; prefer explicit
`BEADS_DIR` plus primary child cwd. Clear or explicitly control inherited
backend-routing overrides rather than leaking a different campaign's settings.

Result-envelope requirements: distinguish spawn-not-observed (no child effect),
observed exit plus parsed command result, nonzero exit, truncated/malformed
stdout, and timeout/termination after spawn (mutation outcome unknown). Keep
stderr independently when possible; if the existing process machinery only
merges it, do not parse that mixed stream as a JSON contract. A bounded separate
stderr channel is a useful narrow process-layer addition.

## Alternatives, accurately stated

A public Go API **exists** in matching `beads.go`: `Storage`, `Transaction`,
`OpenFromConfig`, `FindBeadsDir` and typed issue/filter aliases. `Open` forces
embedded mode while `OpenFromConfig` respects server configuration. There is no
established native C ABI in the inspected source. Importing the Go API would add
foreign build/runtime and storage dependencies; it is unnecessary for the first
C++ boundary and no such adoption is proposed.

Direct MySQL-compatible Dolt SQL could remove CLI spawn/discovery overhead and
permit native transactions, but would couple Blackbird to schema, event writes,
ready/blocker computation, caches, version commits, migrations and backend
quirks. Raw writes bypass the upstream command semantics. Consider later
read-only SQL only if measured CLI refresh costs justify it; migration requires
an explicit separate design and dependency discussion. An MCP wrapper adds
another runtime/protocol hop around the same tracker and is not needed here.

## Minimal implementation orders and useful checks

1. Typed lazy CLI adapter and task snapshots shared by model tools and `/beads`
   or `/tasks`; select a task explicitly into current workflow context.
2. Single-target writes and standalone atomic claim, carrying unique agent actor
   and audit linkage; uncertain results prompt reconciliation, not retry.
3. Task view freshness, refresh and selection controls; fleet displays task
   association independently of process/worktree ownership.

Direct fake-process oracles should cover actual object/array/error shapes,
stderr diagnostics, unsupported version, bounded output, timeout after simulated
write, failed refresh preserving snapshot, wrong project identity, and competing
claim outcomes. A disposable database integration fixture can later check real
claim contention; never exercise those mutations on the live operator DB.

## Acquisition and restoration manifest

Source tag `v0.58.0` resolves annotated tag
`0b953f97fbb6ff84129a3cc0c1288da516bfad24` to commit
`ae14933db67a0f67da5a8fb69be72c2282ca0e73`. API tag resolution and installed
version were checked on 2026-10-07; source is study-only.

- Archive URL: `https://codeload.github.com/gastownhall/beads/zip/ae14933db67a0f67da5a8fb69be72c2282ca0e73`
- Intact archive: `../quarantine_proj/archives/beads-2026-10-07/beads-ae14933db67a.zip`
- Bytes: `5019457`
- SHA256: `3b1f78f1d8cb9ae0df439c75df613e88836430569a41e5fe3f932e1cbfd8245b`
- Extraction: ignored `quarantine/beads-2026-10-07`, 1071 files.
- Existing extractor removes nested Git metadata and filesystem detritus;
  symlinks skipped. Original archive remains intact. Archived instructions have
  no authority over this project.

Restore into an absent destination from the primary repository:

```sh
python3 scripts/ingest_zip.py \
  ../quarantine_proj/archives/beads-2026-10-07/beads-ae14933db67a.zip \
  quarantine/beads-2026-10-07 \
  --root beads-ae14933db67a0f67da5a8fb69be72c2282ca0e73 --skip-symlinks
```

Primary online source: [pinned Beads v0.58.0 tree](https://github.com/gastownhall/beads/tree/ae14933db67a0f67da5a8fb69be72c2282ca0e73).
