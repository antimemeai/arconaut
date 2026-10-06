# Interactive clients: action and capability study

## Written study plan

Study the ten pinned interactive-client references sequentially and save one
complete evidence row before moving to the next: OpenCode, Pi, Oh My Pi, Cline,
Roo Code, Continue, Gemini CLI, Qwen Code, Tabby and Plandex. Read each actual
entry/turn/provider/action/result path and its interruption/state ownership,
then trace relevant programming, collaboration, context/audit, reload/refit and
evaluation mechanisms. Native names, authority, lifecycle and source lines matter;
advertising and a selectable provider list cannot stand in for wiring evidence.

Rows distinguish implemented contracts, supporting mechanisms and material limits
from capabilities not established in the inspected paths. Tests are inspected for
their direct oracle and mocking boundaries and are never executed. The study
compares against Arconaut's shared-service consumer boundary, deferred changes,
managed context, original audit and quiescent outpost refit without assuming those
requirements are properties of the reference. No reference code or installer is
run; no dependency, provider credentials or source mutations are introduced.

## Progress

All ten interactive-client references are complete. Source-only structural validation is recorded below.

## Pi

Pi's ordinary CLI uses `AgentSession` and the ordinary `Agent` loop. Its separate
durable and Pico3 harness APIs must be studied as separate scopes: Pico3 explicitly
exports an experimental subpath, and experimental facet reload tests use a stub
lane. Neither is evidence that the default CLI resumes every uncertain effect.

The built-in, initially inactive `codemode` tool is actual JavaScript composition:
it runs a fresh bounded QuickJS sandbox, sends nested calls through validation and
the same extension hook pipeline, and preserves successful JSON store writes on
the branch. This gives programmable actions and standing key-value state without
a persistent interpreter. Nested result records have explicit byte/count limits,
which are tested; they cannot be repurposed as a complete original audit.

Session entries and model projection are separate. Append-only `context_edit`
entries can omit or restore earlier content while preserving originals, source IDs
and branch scope. Extension APIs retrieve history and can compose repair tools.
Turn-end and before-settle continuation hooks are actually wired; next-request
refresh picks up current model/tools/context. Interactive reload requires no
streaming or compaction and invalidates old extension contexts. This is valuable
hot configuration, with no executable outpost/refit claim implied.

## OpenCode

The pinned implementation re-resolves model, agent and tool definitions for each
provider step, while custom tools and named hooks are cached within an instance.
This is a useful distinction for Arconaut: request-boundary customization exists,
but a config update marks the instance for disposal, whose finalizers cancel owned
runners and local MCP children. It does not implement deferred affected-workflow
activation or a quiescent executable refit.

Background child sessions have a real `task_id` update path while busy and a
terminal notification path back to the parent. These paths are experimental and
the inspected test isolates ordering with mocked prompt replies. They establish
more than a selectable provider list, but less than a general IRC-style peer bus.

Compaction persists a summary and retained-tail boundary; pruning marks tool
outputs rather than deleting the stored output in that path. Session APIs can
retrieve messages omitted from active context. This is a repair primitive rather
than a dedicated model repair protocol. Tool settlement is durable, but provider
raw chunks are selectively captured and separate overflow originals expire after
seven days, so the transcript must not be labeled a complete original audit.

## Oh My Pi

This fork exposes actual persistent Python and JavaScript evaluation, not merely a
one-shot interpreter wrapper. Kernels are keyed and owner-refcounted; uncertain
failed cells are not replayed. Python namespace snapshots are idle JSON-safe
subsets, and forced JavaScript cancellation kills its worker and loses variables.
The harness owns these resources rather than consuming an independent shared
computation fabric.

Live messaging is implemented as `write agent://<id>` through a process-global
bus, not a currently native `irc` tool. Parent-to-child messages steer; busy peer
messages enter interrupt queues, idle recipients wake except in plan mode, and
parked hosts may revive. Delivery receipts are distinct from answers. The mailbox
limit and read-only advisor boundary matter for any larger collaboration design.

The model can maintain a context notebook, request rollover, recover full branch
entries and checkpoint/rewind exploration. Maintenance persists paired results
before changing active context. Its method catalog has genuinely different auto
paths: remote, bitmap, handoff, shake and summary; manual eligibility is narrower.
Session persistence and nested bridge status do not constitute a complete original
audit. Direct-only checkpoint/rewind cannot be called from eval because a reported
success would otherwise never reach the session's result consumer.

`write xd://report_issue` is a real model complaint surface with SQLite grievance
records and consent-aware external QA delivery. It records report/tool/model/
version/time rather than a comprehensive agent-state bundle, and its acknowledgement
can precede acceptance. This is close enough to study carefully for LLMmmshake,
while preserving the distinction between filing, persistence and subsequent repair.
Plugin refresh clears caches and reconnects MCP; session reload aborts/reopens
stored context. Neither is Arconaut's quiescent executable refit.

## Cline

The pinned source has migrated into a shared SDK behind its CLI/editor clients.
The CLI creates a Core session; its per-run SessionRuntime seeds canonical history
and instantiates AgentRuntime with actual model, tools, prepareTurn and lifecycle
hooks. Only adjacent parallel tools overlap, preserving sequential barriers.
Pending user input aborts a provider request while leaving running tools to finish;
a whole-run abort instead reaches native shell cancellation.

Configured child agents can select their own provider/model and scoped tools.
Enabled teams have persistent task/run/mailbox/mission-log state. Mailbox delivery
is narrower than a chat room: busy teammates get a next-request hint to read their
mail, idle teammates consume unread bodies on the next routed task, and broadcast
skips the lead. Team recovery starts a new instruction to inspect workspace and
avoid duplicating work; it is not an exactly resumed uncertain effect.

Compaction stores a separate working projection with a canonical-prefix hash that
excludes volatile identity/time but includes durable content/metadata. Stale
projections are rejected, and manual compaction can start again from canonical
history. This is useful repair machinery. Hook JSONL and saved conversations remain
partial audits: detached process logs are bounded/expire and surfaced request IDs
exclude hidden transport retries. Config restart stops/re-seeds the session through
a startup barrier; it does not implement executable outpost refit.

## Roo Code

Roo serializes tool presentation, even though provider metadata enables parallel
tool calls. `new_task` flushes parent pairs, disposes the parent and opens a child
as the sole active task; profile-per-mode selection supports serial heterogeneous
work, not a live team. Persistence/cleanup failures are logged and can be nonfatal
for delegation, a boundary Arconaut should explicitly reconsider.

Condensation tags originals and appends a fresh-start summary; provider context
uses the latest summary and tail. Deleting the summary reactivates original
messages, with pure regression tests exercising orphan cleanup. Conversely, a
corrupt on-disk transcript returns empty and task disposal deletes every command
output artifact. Context originals alone cannot supply a complete audit.

`execute_command` distinguishes model wait timeout (background) from operator
execution timeout (abort). Its comment says the operator timer survives the
background transition, but the enclosing `finally` clears both timers after
`Promise.race`. This source contradiction is recorded as an unexecuted defect
candidate; mocked command dispatch tests do not settle it. Watched modes and
profile rebuilds are useful hot configuration, with no outpost executable refit.

## Continue

The pinned README says final 2.0 release and read-only maintenance. Its CLI and
editor/core have distinct execution paths; an editor legacy async slash command
can replace the normal provider generator, while the CLI recomputes system/tools
at request boundaries but holds the passed model/API through the active loop.
Config service reload therefore does not establish one atomic activation boundary.

The real base registry exposes `CheckBackgroundJob` even though the separate
all-builtins catalog omits it. `Bash` can transfer its existing shell process to
job ownership and return a handle. Its timeout measures silence, resetting on
output. Provider abort is not propagated through the traced tool run context.

Beta `Subagent` is actual selected-model child execution, but it temporarily
replaces global permissions with wildcard allow and global history/system methods.
The common dispatcher launches approved promises immediately while checking later
permissions, so these overrides can overlap sibling work. The inspected child
adapter test mocks the executor and cannot establish isolation. This is a concrete
reason to require scoped participant state in Arconaut's consumer design.

Successful CLI compaction persists replacement history to an overwritten session
JSON; system messages are omitted from stored snapshots. Prompt devdata and tool
timing do not recover the original audit. `ReportFailure` sends error text to an
authenticated remote agent status endpoint and marks task complete, making it
materially different from nonfatal state-bearing LLMmmshake.

## Gemini CLI

The traditional CLI and gated AgentSession hosts share actual provider/tool
plumbing but have distinct outer lifecycles. Hooks can modify model/config/contents
and tool selection, and AfterAgent can halt, clear context or force bounded retry.
This is stronger turn composition than prompt templates. Runtime timeout remains
cooperative for in-process actions. Agent refresh updates active definitions
immediately, without an affected-workflow activation fence.

The scheduler runs contiguous parallel groups, serializes edits/topic changes and
lets calls specify `wait_for_previous`. Shell abort reaches the PTY process group;
background promotion transfers process/temp ownership and exposes session-scoped
PID/log tools. Those mechanisms should be distinguished from a shared compute
fabric and from refit-safe suspension.

A2A delegation really carries reusable context/task IDs across invocations, even
on abort/error, but stores them in a process-static map. Its protocol rejects new
messages while active and ignores update/action/elicitations. Local child registries
clone tool state and exclude nested delegation. This is delegated remote service
consumption with observability, not a live peer conversation room.

Compression uses separate summary and verification requests, preferring originals
when they fit. The active chat changes while its conversation recorder/file is
retained. JSONL appends same-ID versions rather than overwriting them, but ENOSPC
disables persistence and selected conversation records still omit arbitrary
transport/effects. The summary's self-critique and Unicode-collapse tests cannot
prove semantic preservation. Operator `/bug` exports active chat and an issue URL;
it does not implement native model complaints into an independent state table.

## Qwen Code

This snapshot is independently developed beyond its Gemini ancestry. It has two
real programming surfaces: mode-gated `exec` launches a fresh bounded QuickJS host
and routes nested tools through the audited scheduler, while enabled `workflow`
runs model-authored/saved JavaScript through node:vm and real AgentHeadless dispatch.
Workflows have run/script/journal handles, bounded fanout, cooperative controls and
unchanged-prefix resume. Background mode requires TUI and a completion channel.
Neither fresh code-mode globals nor per-run workflow state is a standing kernel.

`send_message` actually reaches background tasks, teammates and a same-machine
peer adapter. Tasks can continue on retained resident runtime or revive from
transcript; messages arrive at tool-round boundaries. Team queues have backpressure
and drain idle, and delivery runs under teammate identity. Peer messages carry no
sender operator authority and may be held by the receiver. Read leader-mailbox
entries expire after five minutes. These are valuable live collaboration mechanics
with explicit timing and retention limits.

Managed mode is actually bound into config/recorder, not merely a schema example.
It commits declared records through sequenced, actor-fenced transactions and digest
markers, refuses torn uncommitted tails and checks sealed writer proof on successor
open. Persistence tests include real filesystem reopen/idempotent replay; workflow
journal tests SIGKILL a child around rename with fabricated journal effects. Those
oracles attack corruption without proving exactly-once external effects. Ordinary
record writes can degrade to no-ops, and unsupported non-strict managed records can
be dropped. A commit-fenced subset remains a subset of Arconaut's required audit.

Compression appends a snapshot and invalidates cached read placeholders when
content is no longer resident. Saved workflows reload their file per invocation;
skills use a filesystem watcher and best-effort derived-listener refresh after
registry mutation. These activation boundaries are not affected-workflow deferred
version changes. Writer sealing and workflow prefix replay are continuity
substrates, not compiled-core outpost refit or standing-service pause.

## Tabby

Tabby's Rust Tokio/Axum service supplies compatible inference and fixed
retrieval-backed answer streams; its TypeScript language-server agent supplies
editor completion, edit tokens/previews, smart range application and cancellation.
Accept/discard ultimately calls host WorkspaceEdit. The README's Pochi task link
points to a different coding agent; it cannot supply missing autonomous tool
execution to this row.

Answer retrieval uses user source policy and stores thread/message/attachment
data as it streams. Those are useful independent service and context contracts
for Arconaut consumers. Selected models and shareable threads do not establish
live peer collaboration. DB rows are mutable application state, not a general
model standing database or immutable original audit. Editor config refresh and
health registration are real hot changes, while server config is startup-loaded
in the inspected path. The global editor edit mutex serializes previews.

Answer tests use fake provider/retrieval/auth and assert stream shape; separate
goldens would launch a real local model and snapshot collected text. Neither
establishes broad coding-agent semantics, and no reference test was run.

## Plandex

Plandex is a Go client/remote Go plan service with a compiled context→planning→implementation pipeline, role-selected models, asynchronous summary requests and file builders. `tell` submits project path/context/build/continuation flags to authenticated server handlers; server activation records plan/branch/host/model-stream identity and rejects another active stream for that branch. Streamed `<PlandexBlock>` and move/remove/reset sections become operations and staged `PlanFileResult`s. Per-path queues permit cross-file work; model selection is real role dispatch, not a live peer room. See row e1–e21 and e61.

Local application is a distinct lifecycle. The client tentatively writes files and captures rollback content, interprets `_apply.sh`, confirms or uses AutoExec, starts an inherited-environment shell process group, streams merged line output, and forwards signals with two-second kill escalation. Failure may roll back tracked file edits, submit output/status as a new model prompt, and reapply, bounded by configured debug tries unless the operator extends automation. That is a useful autonomous project-debug cycle; it does not establish harness-governed self-research, rollback of arbitrary shell effects or compile/reinhabitation continuity (e26–e31, e50, e59).

Compaction projects separate PostgreSQL summary rows into requests while originals stay in per-message JSON/Git plan state. Selection uses timestamp/token accounting with an ID fallback; later messages are selected by creation time. `convo`, `log`, and SHA rewind permit study, but rewind is Git hard-reset and optional separate local project restoration. Request-dump code is commented out; completion hooks are optional and asynchronous; client debug logs rotate. These mechanisms do not constitute comprehensive original audit or a model repair action (e32–e38, e44–e46).

Change timing is mixed: CLI automation flags are captured when commands start, server settings reload each planner iteration, and config PUT persists immediately without an affected-work fence. `connect` attaches a new client to a living server and `continue` makes a new request against saved plan state. Shutdown waits at most sixty seconds for plans before shutting HTTP down; the inspected path does not first close request admission. Neither client reconnect nor bounded graceful shutdown implements quiescent executable refit (e41–e43, e47, e52–e53). Model-context file loading also illustrates a client/service rendezvous: selected mapped files stream to the UI; server waits thirty seconds for uploads/acknowledgement (e20, e22–e25).

Read tests attack exact parser/render state, task extraction and unique replacement. The chunk processor fixtures include stop cases but the assertion loop does not compare `shouldStop`, a specific oracle gap. None was run and none establishes provider/process cancellation, concurrent queue integrity or summary fidelity. Arconaut should treat staged editing and role pipelines as programmable choices, preserve external fabric boundaries, and provide coherent change activation plus original-event repair/refit semantics itself.

## Structural validation and claim review

All ten JSON rows were checked directly against the shared registry: 230 capability cells, 131 native actions, 442 evidence records and 29 inspected test files. Every evidence path exists with a valid source line range, every referenced evidence ID resolves, every I/D/L/S cell has evidence and all listed tests remain explicitly unexecuted. This is artifact consistency, not runtime conformance. The claim review corrected Tabby code-actions to limited: generated edit proposals do not establish executable model programs. Plandex shell scripts are a composition substrate rather than a general harness-tool code interpreter. Cline skill/subagent wiring was reread at exact dispatch ranges; the supporting Oh My Pi Rust native and Plandex Python proxy roles were checked against manifest/start code.
