# nullclaw

Actual model SQL query surface, named multi-provider child agents and explicit subset config/skill reload; compaction and durable session history have different content scopes.

Role: native persistent Zig agent. Runtime: Zig, C (SQLite binding).

Pinned source: [https://github.com/nullclaw/nullclaw](https://github.com/nullclaw/nullclaw); revision/version `55907af88e51ff37cc114dcb6eb3ca438e5b0c2b`.

Blocking provider/tool loop, thread-backed subagents, native vtable components and channel/session bus.

Owns sessions, child threads, command processes and configured memory clients; read-only SQLite query consumes workspace DB files.

Inspection: Read turn/tool dispatch, in-turn replay cache, compaction, persistence, process watcher/reap, child spawning/model routing, SQL tool wiring, config/skills reload and audit.

Limits of this study: Source only, no acquired execution. Full channel claim/peer routing, every memory backend and provider request cancellation not exhaustively traced.

## Actions

### shell

Surface: model tool.

Input: command/cwd

Result: stdout success or selected error

Lifecycle: new noninteractive shell per action; watcher signals then root wait; finite capture

Authority: configured policy/sandbox and path scope

Evidence: [shell](#evidence-shell), [run](#evidence-run), [watch](#evidence-watch), [reap](#evidence-reap).

### sqlite_query

Surface: model tool.

Input: workspace DB path, SELECT/WITH/table_info, max rows

Result: bounded redacted rows

Lifecycle: opens existing DB read-only; not create/update or namespace service

Authority: resolved-path guard, classifier and SQLite readonly backstop

Evidence: [sql](#evidence-sql), [tools](#evidence-tools).

### spawn / named delegate

Surface: model tools.

Input: task/agent/context/origin

Result: thread task ID and followup, or one provider completion

Lifecycle: spawn async cap/threads vs awaited delegate completion; teardown joins threads

Authority: named provider/credential/profile and inherited execution scope

Evidence: [children](#evidence-children), [child-model](#evidence-child-model), [delegate](#evidence-delegate), [shutdown](#evidence-shutdown).

### message

Surface: model tool.

Input: channel/chat/content

Result: bus publication acknowledgement

Lifecycle: queued outbound transport; external delivery not confirmed at return

Authority: configured bus/default destination

Evidence: [message](#evidence-message).

### config mutation / reload / reloadSkillsAll

Surface: operator commands and host API.

Input: allowed config path/value or skill changes

Result: validated candidate and apply/skip/failure summary

Lifecycle: explicit supported hot subset; skill invalidation under session locks; some paths restart

Authority: operator/slash/host mutation program

Evidence: [config](#evidence-config), [hot](#evidence-hot), [apply](#evidence-apply), [skills](#evidence-skills), [restart](#evidence-restart).

### auto/force compaction

Surface: host lifecycle.

Input: history/keep/source/summary caps

Result: replacement summary+tail or kept tail

Lifecycle: frees prior projection; force can drop without summary

Authority: compiled host context policy

Evidence: [compact](#evidence-compact), [force](#evidence-force).

## Capabilities

### filesystem

**S — Files** (source): Native all-tools set includes workspace operations; code/SQL paths enforce scope. Dedicated file implementation not all studied.

Evidence: [tools](#evidence-tools), [sql](#evidence-sql), [run](#evidence-run).

### processes

**I — OS programs** (source): Shell process group and cancellation watcher/root wait; sequential pipe drains and swallowed watcher creation can weaken progress.

Evidence: [run](#evidence-run), [watch](#evidence-watch), [reap](#evidence-reap).

### code-actions

**S — Code actions** (source): Shell can execute programs, but no interpreter-based tool orchestration namespace established.

Evidence: [run](#evidence-run).

### persistent-kernel

**? — Kernel** (inspection scope): Not established beyond inspected turn, process, SQL, subagent, persistence and reload paths.

### standing-database

**I — Standing DB** (source): Actual default sqlite_query tool reads standing workspace DBs, plus configured memory tool registration; SQL writes disallowed.

Evidence: [tools](#evidence-tools), [sql](#evidence-sql).

### workflow-programming

**L — Workflows** (source): Native vtables/compiled loop, shell and scheduling primitives; no hot executable whole-turn DSL in inspected paths.

Evidence: [dispatch](#evidence-dispatch), [run](#evidence-run), [tools](#evidence-tools).

### multi-model

**I — Models** (source): Named child agent/provider/model/credentials and explicit runtime model/provider hot change.

Evidence: [child-model](#evidence-child-model), [delegate](#evidence-delegate), [hot](#evidence-hot).

### live-collaboration

**L — Peer chat** (source): Asynchronous child completion and outbound channel messages; outbound send alone does not establish peer mailbox/busy model delivery.

Evidence: [children](#evidence-children), [message](#evidence-message).

### concurrent-work

**I — Concurrency** (source): Thread-backed subagents capped/reserved under mutex; primary tool calls sequential.

Evidence: [children](#evidence-children), [dispatch](#evidence-dispatch).

### steering-interrupt

**L — Steer/interrupt** (source): Interrupt checked at tool boundaries, shell watcher signals/reaps root; child shutdown joins without active cancellation; provider cancellation not established.

Evidence: [dispatch](#evidence-dispatch), [watch](#evidence-watch), [reap](#evidence-reap), [shutdown](#evidence-shutdown).

### turn-redefinition

**L — Turn program** (source): Fixed compiled loop and hot config subset, not replacing turn program at a settled workflow boundary.

Evidence: [dispatch](#evidence-dispatch), [hot](#evidence-hot), [apply](#evidence-apply).

### compaction

**I — Compaction** (source): Summary/capped input and tail pairing, destructive in-memory replacement; exhaustion force discards middle.

Evidence: [compact](#evidence-compact), [force](#evidence-force).

### context-repair

**L — Repair** (source): Persisted user/final assistant can recover conversation text; intermediate tools/redacted provider data not original repair corpus.

Evidence: [persist](#evidence-persist), [results](#evidence-results), [compact](#evidence-compact).

### original-audit

**L — Original audit** (source): Synced rotated security events and optional bounded/redacted diagnostic IO; final user/assistant storage drops intermediate original activity and ignores storage failures.

Evidence: [audit](#evidence-audit), [audit-write](#evidence-audit-write), [results](#evidence-results), [persist](#evidence-persist).

### audit-query

**S — Audit query** (source): SQL query could inspect local stored tables but full original IO is absent; memory/session scope differs.

Evidence: [sql](#evidence-sql), [persist](#evidence-persist), [audit](#evidence-audit).

### hot-change

**I — Hot change** (source): Explicit model/provider/config subset and lock-protected skills prompt invalidation; restart-required classification.

Evidence: [hot](#evidence-hot), [apply](#evidence-apply), [skills](#evidence-skills), [restart](#evidence-restart).

### rebuild-continuity

**L — Rebuild continuity** (source): Persisted final messages/config reload do not serialize child threads or settle provider/program work with outpost handoff.

Evidence: [persist](#evidence-persist), [shutdown](#evidence-shutdown), [restart](#evidence-restart).

### remote-services

**S — Remote** (source): Configured model providers and outbound channel bus consume remote services; external shared kernels not traced.

Evidence: [child-model](#evidence-child-model), [message](#evidence-message).

### self-improvement

**? — Self-improve** (inspection scope): Not established beyond inspected turn, process, SQL, subagent, persistence and reload paths.

### complaints

**? — Complaints** (inspection scope): Not established beyond inspected turn, process, SQL, subagent, persistence and reload paths.

### authority

**I — Authority** (source): Configured shell validation yields risk block/approval modes and sandbox wrapper, DB strictly read-only; policy breadth not exhaustively traced.

Evidence: [shell](#evidence-shell), [run](#evidence-run), [sql](#evidence-sql).

### evaluation

**S — Evaluation** (source): Read literal SQL content/readonly and persisted roles/usage, actual command cancellation flag oracle; no descendant-liveness oracle in selected cancel test.

Evidence: [sql-test](#evidence-sql-test), [persist-test](#evidence-persist-test), [cancel-test](#evidence-cancel-test).

### time-order

**L — Time/order** (source): Command watchdog sums nominal sleep quanta rather than measuring elapsed monotonic time; diagnostics use wall milliseconds.

Evidence: [watch](#evidence-watch), [results](#evidence-results).

## Inspected test oracles

- [quarantine/nullclaw/src/tools/sqlite_query.zig](../../../quarantine/nullclaw/src/tools/sqlite_query.zig): SQL content and readonly authority Oracle: Real temporary DB literal count and schema columns, write/PRAGMA restrictions and caps; not run. Read, **not executed**.
- [quarantine/nullclaw/src/agent/turn_persistence.zig](../../../quarantine/nullclaw/src/agent/turn_persistence.zig): saved conversation scope Oracle: Exact two roles/content and usage exposes final-message-only storage; not run. Read, **not executed**.
- [quarantine/nullclaw/src/tools/process_util.zig](../../../quarantine/nullclaw/src/tools/process_util.zig): cancel flag handling Oracle: Real runner returns failure/interrupted after joined command runner; does not inspect all descendants; not run. Read, **not executed**.

## Useful mechanisms

- Native Zig runtime with real SQL tool and profile provider/model routing.
- Explicitly reports unsupported hot settings and distinguishes restart-required paths.
- Session prompt reload locks around active turn state.

## Material limits

- Successful IDless tool calls cached by signature without purity; source-inferred stale reads or repeated-side-effect suppression possible.
- Normal process path drains stdout/stderr sequentially, watcher spawn failure ignored; progress should be checked directly before reuse.
- Final message persistence and security audit omit full original activity.

## Arconaut design questions

- Use exact call identity for replay; do not memoize effects by equal arguments.
- Keep SQL query scope visible, including read-only versus writable standing DB.
- Make reload scope and confirmed process settlement explicit.

## Evidence

### Evidence turn

[quarantine/nullclaw/src/agent/root.zig:1943–1965](../../../quarantine/nullclaw/src/agent/root.zig#L1943): Turn refreshes child tool context, local slash handling and optional redaction.

### Evidence dispatch

[quarantine/nullclaw/src/agent/root.zig:2704–2757](../../../quarantine/nullclaw/src/agent/root.zig#L2704): Compiled sequential tool dispatch checks cross-thread interrupt before calls and consults replay cache.

### Evidence results

[quarantine/nullclaw/src/agent/root.zig:2769–2807](../../../quarantine/nullclaw/src/agent/root.zig#L2769): Optional diagnostic args/results bounded1024; tool output scrubbed/redacted and inserted as user reflection text.

### Evidence dedup

[quarantine/nullclaw/src/agent/root.zig:2981–3048](../../../quarantine/nullclaw/src/agent/root.zig#L2981): Native calls dedup by ID; IDless successful calls by name/raw argument signature without purity or external-state invalidation.

### Evidence compact

[quarantine/nullclaw/src/agent/compaction.zig:104–172](../../../quarantine/nullclaw/src/agent/compaction.zig#L104): Summary may cover capped source and truncate merged summary; frees prior in-memory messages.

### Evidence force

[quarantine/nullclaw/src/agent/compaction.zig:187–208](../../../quarantine/nullclaw/src/agent/compaction.zig#L187): Force context recovery explicitly drops middle without summary, extends keep boundary for tool pairing.

### Evidence persist

[quarantine/nullclaw/src/agent/turn_persistence.zig:18–52](../../../quarantine/nullclaw/src/agent/turn_persistence.zig#L18): Persist only user/final assistant/runtime commands/usage; storage errors swallowed, reset clears history.

### Evidence persist-test

[quarantine/nullclaw/src/agent/turn_persistence.zig:55–87](../../../quarantine/nullclaw/src/agent/turn_persistence.zig#L55): Exact two-message roles/content and usage oracle.

### Evidence tools

[quarantine/nullclaw/src/tools/root.zig:495–540](../../../quarantine/nullclaw/src/tools/root.zig#L495): Actual SQLite query/memory/delegate/spawn registrations in all-tools construction.

### Evidence sql

[quarantine/nullclaw/src/tools/sqlite_query.zig:55–142](../../../quarantine/nullclaw/src/tools/sqlite_query.zig#L55): Workspace scoped read-only SQLite open, statement classifier and engine readonly check; row/byte limits.

### Evidence sql-test

[quarantine/nullclaw/src/tools/sqlite_query.zig:732–778](../../../quarantine/nullclaw/src/tools/sqlite_query.zig#L732): Literal count result/table columns and refusal of state-changing PRAGMA.

### Evidence shell

[quarantine/nullclaw/src/tools/shell.zig:178–205](../../../quarantine/nullclaw/src/tools/shell.zig#L178): Actual command authority policy errors include blocked risk and approval required.

### Evidence run

[quarantine/nullclaw/src/tools/shell.zig:301–335](../../../quarantine/nullclaw/src/tools/shell.zig#L301): Per-action shell process, bounded captures; success discards stderr, failures/timeouts return selected error.

### Evidence watch

[quarantine/nullclaw/src/tools/process_util.zig:160–206](../../../quarantine/nullclaw/src/tools/process_util.zig#L160): Cancel watcher signals group and Linux descendants, TERM then KILL; timeout counted nominal20ms sleeps.

### Evidence reap

[quarantine/nullclaw/src/tools/process_util.zig:375–438](../../../quarantine/nullclaw/src/tools/process_util.zig#L375): Separate process group, watcher spawn failure swallowed; sequential stdout then stderr drains and child.wait on normal path.

### Evidence cancel-test

[quarantine/nullclaw/src/tools/process_util.zig:496–534](../../../quarantine/nullclaw/src/tools/process_util.zig#L496): Real command cancellation thread joined; asserts interrupted/failure flags, not every descendant death.

### Evidence children

[quarantine/nullclaw/src/subagent.zig:206–277](../../../quarantine/nullclaw/src/subagent.zig#L206): Mutex/cap reservation then thread spawn with origin/session association and rollback.

### Evidence child-model

[quarantine/nullclaw/src/subagent.zig:587–635](../../../quarantine/nullclaw/src/subagent.zig#L587): Named child provider/model/credential/workspace and inherited authority passed to task runner.

### Evidence shutdown

[quarantine/nullclaw/src/subagent.zig:171–188](../../../quarantine/nullclaw/src/subagent.zig#L171): Manager deinit joins child threads; no cancellation branch in this teardown.

### Evidence delegate

[quarantine/nullclaw/src/tools/delegate.zig:108–143](../../../quarantine/nullclaw/src/tools/delegate.zig#L108): Delegate is named provider completion with context, not automatically full tool child loop.

### Evidence message

[quarantine/nullclaw/src/tools/message.zig:52–86](../../../quarantine/nullclaw/src/tools/message.zig#L52): Message tool publishes outbound channel/chat content to bus; sent acknowledgement means queued bus item.

### Evidence hot

[quarantine/nullclaw/src/agent/commands.zig:4706–4775](../../../quarantine/nullclaw/src/agent/commands.zig#L4706): Fixed hot paths mutate model/provider, temperature and iteration/history/time settings.

### Evidence apply

[quarantine/nullclaw/src/agent/commands.zig:4883–4913](../../../quarantine/nullclaw/src/agent/commands.zig#L4883): Actual explicit reload applies supported non-null paths and reports skipped/failed.

### Evidence skills

[quarantine/nullclaw/src/session.zig:2221–2239](../../../quarantine/nullclaw/src/session.zig#L2221): Skill prompt reload locks every session before invalidating system prompt.

### Evidence config

[quarantine/nullclaw/src/config_mutator.zig:410–475](../../../quarantine/nullclaw/src/config_mutator.zig#L410): Allowed config mutation builds and validates candidate before disk apply; restart classification distinct.

### Evidence restart

[quarantine/nullclaw/src/config_mutator.zig:122–127](../../../quarantine/nullclaw/src/config_mutator.zig#L122): Channels/runtime/memory backend/profile/default queue mode classified restart-required.

### Evidence audit

[quarantine/nullclaw/src/security/audit.zig:40–76](../../../quarantine/nullclaw/src/security/audit.zig#L40): Security audit schema holds action/outcome metadata, not provider/program original payload.

### Evidence audit-write

[quarantine/nullclaw/src/security/audit.zig:218–249](../../../quarantine/nullclaw/src/security/audit.zig#L218): Optional rotated JSON security events with file.sync, command result selected metadata only.

