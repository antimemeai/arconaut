# plandex

Remote staged-plan coding agent with model role pipeline, context maps/loading, cumulative file changes, operator-selectable automation and failure-driven local execution/retry.

Role: coding agent with remote plan service. Runtime: Go, Python (owned LiteLLM proxy service).

Pinned source: [https://github.com/plandex-ai/plandex](https://github.com/plandex-ai/plandex); revision/version `e2d772072efadbe41d2946d97d79be55532dbab5`.

Go CLI/TUI consumes Go HTTP plan server; Go goroutines stream planner/summary/per-file build requests; client applies staged changes and launches local shell process groups. Server starts LiteLLM proxy.

Client owns local file application/command process; server owns plan jobs, Git plan repositories/active stream state and its proxy, consumes PostgreSQL/model endpoints. It is a plan service consumer/owner split, not arbitrary shared kernel fabric.

Inspection: Selected source excerpts trace exposed entry/loop/request/action/result/state boundaries and relevant capability mechanisms; listed tests are read as source only.

Limits of this study: No acquired code executed, dependencies installed or live providers invoked. Claims concern this pinned snapshot; untraced catalog tools and dependency internals are not inferred.

## Actions

### tell / POST /plans/{planId}/{branch}/tell

Surface: operator command / authenticated API.

Input: Prompt or piped/file text; branch, context/build/auto-continue/exec flags, session ID

Result: Streaming reply/build/load events, model-stream ID, staged plan state or errors

Lifecycle: Launches server job; --bg returns while server continues; same plan/branch active stream rejected

Authority: Operator/program auth and plan authorization; model participates through response protocol

Evidence: [e2](#evidence-e2), [e3](#evidence-e3), [e4](#evidence-e4), [e5](#evidence-e5), [e6](#evidence-e6).

### PlandexBlock / ### Move Files / ### Remove Files / ### Reset Changes

Surface: model response protocol.

Input: File path/lang/content; source/destination or paths; end tags

Result: Operation records and per-path pending builds/staged file results

Lifecycle: Parsed from streamed text; queues build work while planning may continue

Authority: Model emits operations; server dispatch enabled outside chat-only and according to build mode; local apply separate

Evidence: [e11](#evidence-e11), [e12](#evidence-e12), [e13](#evidence-e13), [e14](#evidence-e14), [e16](#evidence-e16), [e17](#evidence-e17).

### ### Files → AutoLoadContextFiles / POST .../auto_load_context

Surface: model response / client service API.

Input: Backtick file paths restricted to submitted project map; client uploads selected file bodies

Result: Context records, skip/size messages and acknowledgement for server wait

Lifecycle: UI performs local reads; server waits 30 seconds then continues/errors

Authority: Model selects within available paths; authenticated client has filesystem access and bounds

Evidence: [e20](#evidence-e20), [e22](#evidence-e22), [e23](#evidence-e23), [e24](#evidence-e24), [e25](#evidence-e25), [e55](#evidence-e55).

### build / PATCH /plans/{planId}/{branch}/build

Surface: operator API / planner dispatch.

Input: Pending proposed operations grouped by path

Result: PlanFileResult staged content/removal and build events/IDs

Lifecycle: Go workers concurrent across paths; per-path queue and flags

Authority: Server model roles generate/apply structured proposal; no workspace execution implied by build

Evidence: [e15](#evidence-e15), [e16](#evidence-e16), [e17](#evidence-e17), [e18](#evidence-e18), [e54](#evidence-e54).

### apply / PATCH /plans/{planId}/{branch}/apply

Surface: operator command / local client + server API.

Input: Current staged changes, confirmation/commit/exec/debug options

Result: Local file changes, optional project commit, server applied state, rollback record

Lifecycle: Tentatively writes files, then command execution; success commits server applied state

Authority: Operator manual or selected full-auto flags; proposed model content interpreted by client

Evidence: [e26](#evidence-e26), [e27](#evidence-e27), [e59](#evidence-e59), [e54](#evidence-e54).

### _apply.sh / execApplyScript

Surface: model-produced script / client execution.

Input: Shell script or explicit command; inherited cwd/env/stdin

Result: Live merged output, exit code, success/failure; no durable process handle

Lifecycle: Local process group; forwarded signal then 2-second kill escalation; temp script removed

Authority: AutoExec or operator confirmation; ordinary OS authority; generated set/trap/shebang lines rewritten

Evidence: [e27](#evidence-e27), [e28](#evidence-e28), [e29](#evidence-e29), [e30](#evidence-e30).

### GetOnApplyExecFail / debug retry

Surface: operator workflow / client program.

Input: Exit status/output, rollback plan, attempt bound

Result: New failure prompt, planner changes and next application attempt

Lifecycle: Rollback tracked file changes then TellPlan/apply; configured attempt limit may prompt user for extension

Authority: Operator chooses autonomy; model fixes project task through ordinary plan loop, not harness refit protocol

Evidence: [e31](#evidence-e31), [e50](#evidence-e50).

### continue

Surface: operator command.

Input: Current plan/branch and refreshed config-derived flags

Result: New streamed planner iteration on persisted plan state

Lifecycle: New request with IsUserContinue; active stream exclusion applies

Authority: Authenticated operator; no serialized suspended provider request

Evidence: [e53](#evidence-e53), [e6](#evidence-e6).

### connect / PATCH .../connect / ps

Surface: operator command / authenticated API.

Input: Plan/branch or active stream selection

Result: Existing server stream rendered in new client

Lifecycle: Attach to still-running server; not a new provider peer

Authority: Authorized operator; server work outlives detached client

Evidence: [e52](#evidence-e52), [e48](#evidence-e48), [e55](#evidence-e55).

### stop / DELETE .../stop

Surface: operator command / authenticated API.

Input: Active plan/branch

Result: Abort event, stopped partial message when persistence succeeds

Lifecycle: Routes to owning host; cancellation deferred even if partial-store operation fails

Authority: Authenticated plan authority; stop cancels plan and summary, local command needs separate OS signal

Evidence: [e39](#evidence-e39), [e40](#evidence-e40), [e29](#evidence-e29).

### convo / log / GET .../convo / GET .../logs

Surface: operator query / authenticated API.

Input: Plan/branch; client conversation or Git history selection

Result: Original stored conversation JSON or Git log/SHA list

Lifecycle: Read repository operation; corrupt message read errors; log retention/history scope differs

Authority: Authenticated operator; no full-event model query language traced

Evidence: [e35](#evidence-e35), [e36](#evidence-e36), [e54](#evidence-e54).

### rewind / PATCH .../rewind

Surface: operator command / authenticated API.

Input: Target SHA; optional local project reversion choice

Result: Reset plan repository and token metadata, optional local restored files

Lifecycle: Git hard-reset state rollback; further local restoration can fail independently

Authority: Authenticated operator; not model projection repair preserving immutable event history

Evidence: [e36](#evidence-e36), [e37](#evidence-e37), [e38](#evidence-e38).

### set-config / set-auto / PUT /plans/{planId}/config

Surface: operator command / authenticated API.

Input: PlanConfig fields or autonomy preset

Result: Stored config; command cache updated by client paths

Lifecycle: Config write immediate; CLI captures flags at command boundary; server reloads role settings each iteration

Authority: Operator/config API; no model editable workflow language or deferred affected-work fence exposed

Evidence: [e42](#evidence-e42), [e43](#evidence-e43), [e50](#evidence-e50), [e60](#evidence-e60), [e41](#evidence-e41).

### RegisterHook / ExecHook

Surface: server embedding API.

Input: Hook name and Go function; structured plan/auth/request/build payloads

Result: HookResult or ApiError

Lifecycle: Synchronous registration/execution; some dispatchers call asynchronously

Authority: Server program/deployment code; not normal model-discovered user script

Evidence: [e44](#evidence-e44), [e45](#evidence-e45).

### RegisterNotifyErrFn / NotifyErr

Surface: server embedding primitive.

Input: Severity and arbitrary monitoring data

Result: Optional callback side effect

Lifecycle: No registered callback means no-op; callback panics caught

Authority: Host/server integration; no state-rich model bead contract

Evidence: [e51](#evidence-e51).

## Capabilities

### filesystem

**I — Files** (source): Model text proposes file/move/remove/reset operations; server stages results, client loads files and later applies local writes with rollback metadata. Application is not a transactional filesystem sandbox.

Evidence: [e12](#evidence-e12), [e13](#evidence-e13), [e16](#evidence-e16), [e23](#evidence-e23), [e26](#evidence-e26), [e59](#evidence-e59).

### processes

**I — OS programs** (source): Client runs generated _apply.sh with inherited OS authority/process group, merged output and signal escalation. Server also owns a LiteLLM proxy. No persistent shell job handle traced.

Evidence: [e1](#evidence-e1), [e28](#evidence-e28), [e29](#evidence-e29), [e30](#evidence-e30).

### code-actions

**S — Code actions** (source): Model-produced _apply.sh composes OS commands and is executed by local client; streamed file operations/builds produce code edits rather than model-authored programs composing harness tools. No general tool-code interpreter traced.

Evidence: [e27](#evidence-e27), [e28](#evidence-e28), [e31](#evidence-e31).

### persistent-kernel

**? — Kernel** (inspection-limit): Inspected planner/build loop and local _apply.sh execution establish fresh command processes; persistent evaluable kernel/session variables are not established.

### standing-database

**S — Standing DB** (source): Server consumes PostgreSQL for plan/config/summary metadata with timeouts; no model-accessible standing SQL/data-workspace action was traced.

Evidence: [e49](#evidence-e49), [e34](#evidence-e34).

### workflow-programming

**L — Workflows** (source): Fixed context→planning→implementation/build/apply/debug workflow with model-produced task/operation text and tunable automation. Server Go hooks permit embedding; no ordinary model-editable orchestration program/discovery was traced.

Evidence: [e7](#evidence-e7), [e14](#evidence-e14), [e21](#evidence-e21), [e31](#evidence-e31), [e44](#evidence-e44), [e50](#evidence-e50).

### multi-model

**I — Models** (source): Architect/planner/coder/builder/summary roles resolve actual model requests and role packs; per-file builders and background summary can overlap. This is pipeline role division, not peer room.

Evidence: [e7](#evidence-e7), [e8](#evidence-e8), [e18](#evidence-e18), [e19](#evidence-e19), [e61](#evidence-e61).

### live-collaboration

**L — Peer chat** (source): Remote plan streams permit operator reconnection and multiple model roles. Same plan/branch rejects concurrent active streams; inspected paths expose no live model peer mailbox/room.

Evidence: [e6](#evidence-e6), [e52](#evidence-e52), [e61](#evidence-e61).

### concurrent-work

**I — Concurrency** (source): Server Go planner jobs, background summaries, per-file build queues and independent plans/branches overlap; process-local active registry plus DB host metadata coordinate streams. Ordering/race invariants not runtime-validated.

Evidence: [e5](#evidence-e5), [e6](#evidence-e6), [e15](#evidence-e15), [e19](#evidence-e19).

### steering-interrupt

**I — Steer/interrupt** (source): Stop delivers abort then partial-save attempt and cancellation; CLI commands can restart/continue against saved state. Local shell signal handling is separate from remote plan stop. No busy-turn prompt injection traced.

Evidence: [e39](#evidence-e39), [e40](#evidence-e40), [e53](#evidence-e53), [e29](#evidence-e29).

### turn-redefinition

**L — Turn program** (source): Model creates/removes task/operation text and configured AutoContinue governs a compiled stage machine. Go hooks can gate requests; no general model-authorized redefinition of turn program exposed.

Evidence: [e7](#evidence-e7), [e21](#evidence-e21), [e44](#evidence-e44), [e50](#evidence-e50).

### compaction

**I — Compaction** (source): Summary rows separate from original per-message JSON; budget selects fitting summary prefix then later original messages. Timestamp-based projection and ID fallback are actual mechanisms, not loss validation.

Evidence: [e32](#evidence-e32), [e33](#evidence-e33), [e34](#evidence-e34), [e35](#evidence-e35).

### context-repair

**L — Repair** (source): Original conversations and Git plan rewind/reload support operator reconstruction; model can request additional mapped file context. No model-issued summary repair/restoration with original-event attribution protocol traced; rewind resets plan history head.

Evidence: [e23](#evidence-e23), [e33](#evidence-e33), [e35](#evidence-e35), [e36](#evidence-e36), [e37](#evidence-e37).

### original-audit

**L — Original audit** (source): Original conversation/state files and Git commits are retained separately from summary projection, but debug logs rotate, request dump is commented out, optional request-completion hooks may be absent/fail, shell captures scanner text only. Not comprehensive immutable audit.

Evidence: [e35](#evidence-e35), [e8](#evidence-e8), [e44](#evidence-e44), [e45](#evidence-e45), [e46](#evidence-e46), [e29](#evidence-e29), [e37](#evidence-e37).

### audit-query

**L — Audit query** (source): Authenticated conversation and Git history/SHA APIs permit useful plan study; internal SQL summary query exists. No unified query over provider wire/process/UI/config/original event stream traced.

Evidence: [e34](#evidence-e34), [e35](#evidence-e35), [e36](#evidence-e36), [e54](#evidence-e54).

### hot-change

**L — Hot change** (source): Operator config updates persist immediately; CLI resolves automation flags at command start while server role/org settings reload each planner iteration. No coherent fence after all affected workflows or apply-now operation established.

Evidence: [e41](#evidence-e41), [e42](#evidence-e42), [e43](#evidence-e43).

### rebuild-continuity

**L — Rebuild continuity** (source): Detached client can reconnect to a living server; new continue request reconstructs plan state. Server shutdown waits bounded time before HTTP shutdown/proxy cleanup. No executable-context outpost handoff or enforced request/program quiescence before refit.

Evidence: [e47](#evidence-e47), [e52](#evidence-e52), [e53](#evidence-e53), [e1](#evidence-e1).

### remote-services

**I — Remote** (source): CLI consumes authenticated cloud/self-host Go plan service and SSE streams; server consumes model HTTP services/DB and starts its own LiteLLM proxy. This server governs plan jobs rather than treating all computation as external shared fabric.

Evidence: [e1](#evidence-e1), [e4](#evidence-e4), [e9](#evidence-e9), [e48](#evidence-e48), [e49](#evidence-e49).

### self-improvement

**L — Self-improve** (source): Bounded automated failure→rollback→model fix→apply cycle can improve arbitrary project work; no model-governed harness autoresearch criterion/experiment selection/recompile continuity protocol traced.

Evidence: [e31](#evidence-e31), [e50](#evidence-e50), [e47](#evidence-e47).

### complaints

**S — Complaints** (source): RegisterNotifyErrFn provides host-injected error monitoring; absent callback no-ops. No model-invocable grievance filing that captures agent state/database bead semantics traced.

Evidence: [e51](#evidence-e51).

### authority

**I — Authority** (source): Server APIs authenticate and authorize plans; autonomy presets can bypass routine apply/command confirmation. Local command inherits environment/stdin and OS authority; model is constrained by compiled response protocol and operator-supplied execution flags.

Evidence: [e4](#evidence-e4), [e27](#evidence-e27), [e28](#evidence-e28), [e39](#evidence-e39), [e50](#evidence-e50).

### evaluation

**L — Evaluation** (source): Read fixture oracles for exact operation rendering/state, task extraction and unique replacement. Stream processor test omits shouldStop assertion; none of these establishes provider/process cancellation, plan concurrency, summary fidelity or refit conformance. Not run.

Evidence: [e56](#evidence-e56), [e57](#evidence-e57), [e58](#evidence-e58).

### time-order

**I — Time/order** (source): Creation timestamps order conversations/summaries; stream/build/plan IDs identify work. Actual first-token/inactivity, file-load and command signal/shutdown deadlines differ. No unified causal event order/audit time semantics established.

Evidence: [e6](#evidence-e6), [e10](#evidence-e10), [e20](#evidence-e20), [e29](#evidence-e29), [e34](#evidence-e34), [e35](#evidence-e35), [e47](#evidence-e47).

## Inspected test oracles

- [quarantine/plandex/app/server/model/plan/tell_stream_processor_test.go](../../../quarantine/plandex/app/server/model/plan/tell_stream_processor_test.go): Partial tag/stop-prefix rendering and parser state transitions Oracle: Table expected stream/content/buffer/state comparisons; shouldStop is present in fixtures but not compared. Pure helper tests, not a provider/command lifecycle oracle. Read, **not executed**.
- [quarantine/plandex/app/server/syntax/unique_replacement_test.go](../../../quarantine/plandex/app/server/syntax/unique_replacement_test.go): Unique and ambiguous approximate replacement Oracle: Exact expected matched string or empty rejection for fixed fixtures, no generative/file lifecycle evidence. Read, **not executed**.
- [quarantine/plandex/app/server/model/parse/subtasks_test.go](../../../quarantine/plandex/app/server/model/parse/subtasks_test.go): Model task-text parsing Oracle: Exact fixture count/title/description/file-list assertions; no planner semantic correctness. Read, **not executed**.

## Useful mechanisms

- Separates proposed edits from local application and traces failure-driven rollback/retry with explicit autonomy levels.
- Persisted conversation originals plus separate summary projections and plan Git versions allow useful context/history study.
- Go service/client separation exposes durable plan/branch/stream identity, client reconnect and concurrent per-file construction.

## Material limits

- Fixed response-text protocol/stage machine is much narrower than general model-editable workflow/turn programs.
- Stop preserves partial reply best effort and cancels; local command cancellation separate; no outpost/refit quiescence protocol.
- Conversation/state/version retention and optional request hook are not a full immutable core audit.
- Implementation continuation has 200 cap but inspected planning branch does not apply it.
- Read parser tests omit shouldStop assertion; lifecycle/concurrency/summary loss remain unvalidated.
- Go/native service architecture is study material and does not override operator language distrust or imply adoption.

## Arconaut design questions

- Can staged-plan changes be a programmable workflow option while preserving ordinary direct-tool modes and model control over programs?
- Which work identity belongs to Arconaut client versus separately governed fabric, and what continues across client refit without pausing shared service?
- How should original conversation/event storage support model repair of a selected summary projection without a destructive whole-plan rewind?
- How should configuration versions bind to complete affected workflows when role settings refresh every iteration but command flags were captured earlier?
- What direct cancellation/quiescence oracle detects remaining provider receivers, local process groups and background summaries before arcorefit?
- Should failure diagnosis retain raw command bytes and uncertain effects rather than reconstruct only scanner text and rollback known file edits?

## Evidence

### Evidence e1

[quarantine/plandex/app/server/main.go:14–36](../../../quarantine/plandex/app/server/main.go#L14): Go HTTP server starts/owns a LiteLLM proxy, registers proxy shutdown, initializes DB and serves routes.

### Evidence e2

[quarantine/plandex/app/cli/cmd/tell.go:41–99](../../../quarantine/plandex/app/cli/cmd/tell.go#L41): CLI resolves project/auth and config-derived flags, calls TellPlan, optionally applies local changes after it returns.

### Evidence e3

[quarantine/plandex/app/cli/plan_exec/tell.go:127–151](../../../quarantine/plandex/app/cli/plan_exec/tell.go#L127): CLI submits prompt, project path set, execution/context/build/continuation flags and session ID to remote TellPlan.

### Evidence e4

[quarantine/plandex/app/server/handlers/plans_exec.go:23–118](../../../quarantine/plandex/app/server/handlers/plans_exec.go#L23): Authenticated authorized TellPlan handler decodes request, initializes provider clients, launches modelPlan.Tell and optionally connects response stream.

### Evidence e5

[quarantine/plandex/app/server/model/plan/tell_exec.go:32–70](../../../quarantine/plandex/app/server/model/plan/tell_exec.go#L32): Tell activates plan and launches an asynchronous Go planner iteration; return means launched, not completed.

### Evidence e6

[quarantine/plandex/app/server/model/plan/activate.go:27–79](../../../quarantine/plandex/app/server/model/plan/activate.go#L27): Activation rejects existing in-memory or DB model streams for plan/branch and records host and model-stream identity.

### Evidence e7

[quarantine/plandex/app/server/model/plan/tell_exec.go:177–203](../../../quarantine/plandex/app/server/model/plan/tell_exec.go#L177): Context/planning/implementation stages select architect/planner/coder role configurations.

### Evidence e8

[quarantine/plandex/app/server/model/plan/tell_exec.go:494–581](../../../quarantine/plandex/app/server/model/plan/tell_exec.go#L494): Request selects fallback, builds streamed messages/config/stop token, retains original request in RAM, starts provider call and listener; request-file dump is commented out.

### Evidence e9

[quarantine/plandex/app/server/model/client.go:323–391](../../../quarantine/plandex/app/server/model/client.go#L323): Provider request JSON posts to configured chat-completions URL with context, auth and SSE stream reader; actual HTTP request dispatch is wired.

### Evidence e10

[quarantine/plandex/app/server/model/plan/tell_stream_main.go:59–150](../../../quarantine/plandex/app/server/model/plan/tell_stream_main.go#L59): Separate goroutine receives blocking stream chunks while main listener handles context cancellation and first-token/inactivity timeout; retry allowed before any content.

### Evidence e11

[quarantine/plandex/app/server/model/plan/tell_stream_processor.go:58–155](../../../quarantine/plandex/app/server/model/plan/tell_stream_processor.go#L58): Chunks feed ReplyParser, optionally use visible reasoning, guard an existing-but-unloaded file, append rendered reply, stream it and dispatch new operations.

### Evidence e12

[quarantine/plandex/app/server/types/reply.go:193–270](../../../quarantine/plandex/app/server/types/reply.go#L193): Parser recognizes file-labelled/XML PlandexBlock and move/remove/reset section headers; completed code block becomes file operation.

### Evidence e13

[quarantine/plandex/app/server/types/reply.go:295–330](../../../quarantine/plandex/app/server/types/reply.go#L295): Remove/reset paths become deduplicated pending operations; parser exposes file/operation state and tokens.

### Evidence e14

[quarantine/plandex/app/server/model/plan/tell_stream_processor.go:540–590](../../../quarantine/plandex/app/server/model/plan/tell_stream_processor.go#L540): New operations queue build work in auto build mode with reply/path/content/move/remove/reset fields and append active operation list.

### Evidence e15

[quarantine/plandex/app/server/model/plan/build_exec.go:86–138](../../../quarantine/plandex/app/server/model/plan/build_exec.go#L86): Go build queues are grouped by path with IsBuildingByPath flags; different paths launch asynchronous builds while same-path work is queued.

### Evidence e16

[quarantine/plandex/app/server/model/plan/build_exec.go:247–310](../../../quarantine/plandex/app/server/model/plan/build_exec.go#L247): Move queues separate remove/create builds; removal returns a staged PlanFileResult rather than mutating operator workspace.

### Evidence e17

[quarantine/plandex/app/server/model/plan/build_exec.go:359–397](../../../quarantine/plandex/app/server/model/plan/build_exec.go#L359): New files yield staged content result; existing files dispatch structured edit strategy.

### Evidence e18

[quarantine/plandex/app/server/model/plan/build_whole_file.go:25–110](../../../quarantine/plandex/app/server/model/plan/build_whole_file.go#L25): Whole-file fallback prepares source/proposal prompt, selects builder role and invokes model.ModelRequest with plan/build/message identities.

### Evidence e19

[quarantine/plandex/app/server/model/plan/tell_stream_finish.go:98–147](../../../quarantine/plandex/app/server/model/plan/tell_stream_finish.go#L98): Finished reply is stored before launching background summary model call using separately cancellable SummaryCtx.

### Evidence e20

[quarantine/plandex/app/server/model/plan/tell_stream_finish.go:160–223](../../../quarantine/plandex/app/server/model/plan/tell_stream_finish.go#L160): File load requests stream to client, wait up to 30 seconds for client acknowledgement, then continuation launches a new planner iteration.

### Evidence e21

[quarantine/plandex/app/server/model/plan/tell_stream_status.go:150–224](../../../quarantine/plandex/app/server/model/plan/tell_stream_status.go#L150): Planning stage follows context/task flags and AutoContinue; implementation checks task completion and 200-iteration cap. That cap is not applied to planning branch.

### Evidence e22

[quarantine/plandex/app/server/model/plan/tell_context.go:383–441](../../../quarantine/plandex/app/server/model/plan/tell_context.go#L383): Model selects backtick-enclosed file paths from declared ProjectPaths during planning; ### Files counts explicit selection.

### Evidence e23

[quarantine/plandex/app/cli/lib/context_auto_load.go:17–151](../../../quarantine/plandex/app/cli/lib/context_auto_load.go#L17): Client loads selected files with file/count/total-size limits, encodes images/normalizes text, and calls AutoLoadContext even for empty selection to release waiting server.

### Evidence e24

[quarantine/plandex/app/cli/stream_tui/update.go:499–533](../../../quarantine/plandex/app/cli/stream_tui/update.go#L499): TUI response dispatcher starts loadContextCmd for server load request and quits on error/finish/abort.

### Evidence e25

[quarantine/plandex/app/cli/stream_tui/update.go:743–763](../../../quarantine/plandex/app/cli/stream_tui/update.go#L743): loadContextCmd calls real AutoLoadContextFiles with its own background-derived cancellation context; completion is UI message.

### Evidence e26

[quarantine/plandex/app/cli/lib/apply.go:193–250](../../../quarantine/plandex/app/cli/lib/apply.go#L193): Local tentative ApplyFiles precedes execution; success then marks server plan applied and optionally commits operator project.

### Evidence e27

[quarantine/plandex/app/cli/lib/apply.go:254–298](../../../quarantine/plandex/app/cli/lib/apply.go#L254): _apply.sh or explicit command content is shown and executed only after user confirmation or AutoExec.

### Evidence e28

[quarantine/plandex/app/cli/lib/apply.go:352–425](../../../quarantine/plandex/app/cli/lib/apply.go#L352): Generated script gets fixed shell error header, removes user shebang/set/trap lines, writes local executable script and starts shell with inherited environment/stdin and merged stdout/stderr.

### Evidence e29

[quarantine/plandex/app/cli/lib/apply.go:438–568](../../../quarantine/plandex/app/cli/lib/apply.go#L438): Signal handler forwards signals to group then SIGKILL after 2 seconds; waits command, collects scanner text, asks success after interrupt and feeds exit/output into failure continuation.

### Evidence e30

[quarantine/plandex/app/cli/lib/apply_proc.go:8–15](../../../quarantine/plandex/app/cli/lib/apply_proc.go#L8): Local command uses process group and negative PID signal targeting.

### Evidence e31

[quarantine/plandex/app/cli/plan_exec/apply_exec.go:26–164](../../../quarantine/plandex/app/cli/plan_exec/apply_exec.go#L26): AutoDebug compares attempt bound, otherwise offers operator choices; retry rolls back tracked file changes, sends failure status/output to TellPlan and reapplies recursively.

### Evidence e32

[quarantine/plandex/app/server/model/plan/tell_summary.go:60–138](../../../quarantine/plandex/app/server/model/plan/tell_summary.go#L60): Conversation budget selects stored summary that fits limits; timestamp lookup has message-ID fallback, and missing usable summary can error.

### Evidence e33

[quarantine/plandex/app/server/model/plan/tell_summary.go:144–230](../../../quarantine/plandex/app/server/model/plan/tell_summary.go#L144): Request projection retains stored conversation separately: summary replaces prefix for provider request, later messages selected by timestamp; no summary deletion of originals here.

### Evidence e34

[quarantine/plandex/app/server/db/summary_helpers.go:10–43](../../../quarantine/plandex/app/server/db/summary_helpers.go#L10): Summaries are separate PostgreSQL rows filtered by latest conversation message ID and ordered by created_at.

### Evidence e35

[quarantine/plandex/app/server/db/convo_helpers.go:22–131](../../../quarantine/plandex/app/server/db/convo_helpers.go#L22): Original conversation messages are per-ID JSON files; retrieval fails on unreadable/corrupt files, sorts by creation time; store writes file and DB token metadata.

### Evidence e36

[quarantine/plandex/app/server/handlers/plans_versions.go:16–183](../../../quarantine/plandex/app/server/handlers/plans_versions.go#L16): Authenticated APIs expose Git history/SHA list and plan rewind; rewind resets repository and synchronizes token metadata.

### Evidence e37

[quarantine/plandex/app/server/db/git.go:460–466](../../../quarantine/plandex/app/server/db/git.go#L460): Plan rewind calls git reset --hard to requested SHA; it is ordinary Git state rollback, not immutable original event retention.

### Evidence e38

[quarantine/plandex/app/cli/cmd/rewind.go:393–418](../../../quarantine/plandex/app/cli/cmd/rewind.go#L393): After server rewind, client can optionally apply analyzed project changes to local workspace, separately confirmed.

### Evidence e39

[quarantine/plandex/app/server/handlers/plans_exec.go:258–331](../../../quarantine/plandex/app/server/handlers/plans_exec.go#L258): Stop routes to active host, authenticates/authorizes, sends aborted message, attempts partial-reply persistence and defers cancel even if DB operation fails.

### Evidence e40

[quarantine/plandex/app/server/model/plan/stop.go:10–49](../../../quarantine/plandex/app/server/model/plan/stop.go#L10): Stop cancels summary and active plan; stopped partial assistant message is saved through StoreConvoMessage.

### Evidence e41

[quarantine/plandex/app/server/model/plan/tell_load.go:20–92](../../../quarantine/plandex/app/server/model/plan/tell_load.go#L20): Every planner iteration reloads settings and org-user configuration under repository operation; request flags remain supplied request state.

### Evidence e42

[quarantine/plandex/app/cli/cmd/plan_exec_helpers.go:158–207](../../../quarantine/plandex/app/cli/cmd/plan_exec_helpers.go#L158): Operator execution flags derive from plan config at CLI command start unless explicitly supplied.

### Evidence e43

[quarantine/plandex/app/server/handlers/plan_config.go:56–89](../../../quarantine/plandex/app/server/handlers/plan_config.go#L56): Authorized config update writes config immediately; handler does not enqueue activation fence or cancel an active plan.

### Evidence e44

[quarantine/plandex/app/server/hooks/hooks.go:170–183](../../../quarantine/plandex/app/server/hooks/hooks.go#L170): Server program may register in-process hook function; no-hook execution succeeds with empty result. This is compiled embedding API, not discovered model script interface.

### Evidence e45

[quarantine/plandex/app/server/model/plan/tell_stream_usage.go:32–73](../../../quarantine/plandex/app/server/model/plan/tell_stream_usage.go#L32): Optional completion hook receives request/result and model/plan/message IDs asynchronously; hook error only logged.

### Evidence e46

[quarantine/plandex/app/cli/main.go:47–58](../../../quarantine/plandex/app/cli/main.go#L47): Client debug logger rotates at 10 MB with 3 backups and 28-day age; debug log is not unlimited audit retention.

### Evidence e47

[quarantine/plandex/app/server/setup/setup.go:119–184](../../../quarantine/plandex/app/server/setup/setup.go#L119): Shutdown waits up to 60 seconds for active plans before HTTP shutdown and registered hooks; request admission is not closed before wait in this path.

### Evidence e48

[quarantine/plandex/app/cli/api/clients.go:23–54](../../../quarantine/plandex/app/cli/api/clients.go#L23): Client consumes cloud/default or authenticated self-host server with auth/version headers.

### Evidence e49

[quarantine/plandex/app/server/db/db.go:24–63](../../../quarantine/plandex/app/server/db/db.go#L24): Server consumes configured PostgreSQL DB with timeout settings and connection pool; it is internal plan storage rather than model standing SQL interface.

### Evidence e50

[quarantine/plandex/app/shared/plan_config.go:128–145](../../../quarantine/plandex/app/shared/plan_config.go#L128): Full-auto preset enables continuation/build/context/apply/commit/exec/debug and five auto debug attempts.

### Evidence e51

[quarantine/plandex/app/server/notify/errors.go:18–33](../../../quarantine/plandex/app/server/notify/errors.go#L18): Server embedding may register monitoring callback; unregistered NotifyErr is no-op. No exposed model complaint action/state bead traced.

### Evidence e52

[quarantine/plandex/app/cli/cmd/connect.go:30–55](../../../quarantine/plandex/app/cli/cmd/connect.go#L30): CLI reconnects chosen plan/branch to existing server stream and starts terminal UI.

### Evidence e53

[quarantine/plandex/app/cli/cmd/continue.go:36–63](../../../quarantine/plandex/app/cli/cmd/continue.go#L36): Continue launches a new TellPlan request with IsUserContinue rather than resuming a serialized in-flight stack.

### Evidence e54

[quarantine/plandex/app/server/routes/routes.go:131–163](../../../quarantine/plandex/app/server/routes/routes.go#L131): Native APIs expose current state, apply, rejection, diffs, context CRUD/body, conversation, rewind, logs, branches, settings, status and tell/build.

### Evidence e55

[quarantine/plandex/app/server/routes/routes.go:173–202](../../../quarantine/plandex/app/server/routes/routes.go#L173): Native API routing exposes model packs/config/active-stream connection/stop/missing-file/auto-context acknowledgement.

### Evidence e56

[quarantine/plandex/app/server/model/plan/tell_stream_processor_test.go:453–511](../../../quarantine/plandex/app/server/model/plan/tell_stream_processor_test.go#L453): Table tests compare buffer/render/parser-state fields but do not assert got.shouldStop despite stop cases; isolated parser tests not provider/process lifecycle.

### Evidence e57

[quarantine/plandex/app/server/syntax/unique_replacement_test.go:7–84](../../../quarantine/plandex/app/server/syntax/unique_replacement_test.go#L7): Replacement fixtures check exact unique/no/ambiguous-match return strings.

### Evidence e58

[quarantine/plandex/app/server/model/parse/subtasks_test.go:106–125](../../../quarantine/plandex/app/server/model/parse/subtasks_test.go#L106): Task parser asserts length and exact title/description/uses-files values against fixtures.

### Evidence e59

[quarantine/plandex/app/cli/lib/apply.go:622–690](../../../quarantine/plandex/app/cli/lib/apply.go#L622): ApplyFiles gathers rollback content/mode before local writes, excludes _apply.sh, writes multiple files via Go goroutines.

### Evidence e60

[quarantine/plandex/app/cli/cmd/set_config.go:19–95](../../../quarantine/plandex/app/cli/cmd/set_config.go#L19): Operator set-config/set-auto commands retrieve, mutate and submit plan configuration.

### Evidence e61

[quarantine/plandex/app/shared/ai_models_packs.go:133–151](../../../quarantine/plandex/app/shared/ai_models_packs.go#L133): Default role pack assigns architect/planner/coder, summary, builder, namer/commit/status roles across model configurations.

### Evidence e62

[quarantine/plandex/app/server/model/litellm.go:106–135](../../../quarantine/plandex/app/server/model/litellm.go#L106): Server starts Python uvicorn litellm_proxy subprocess with stdout/stderr forwarding; confirms supporting runtime role and ownership.

