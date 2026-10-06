# sweep

Issue/comment-to-plan-to-edit-to-commit/PR automation with focused XML edit tools, baseline validation and seeded review voting.

Role: coding agent. Runtime: Python, TypeScript documentation/client material.

Pinned source: [https://github.com/sweepai/sweep](https://github.com/sweepai/sweep); revision/version `a8b8b67bda4f89faac9314d34e7c7d5a64f76046`.

Historical GitHub webhook/CLI Python threaded pipeline; synchronous model SDK streaming; independent process pool reviews

Owns clone/staged file state and worker threads; consumes GitHub/provider/Mongo services. Current JetBrains product pointed to by README is not audited by this historical code.

Inspection: CLI/API event worker scheduling, planning-to-edit-to-PR pipeline, edit parser/dispatch/results/continuation, provider calls, context reset, Mongo logging and validation/matching oracle.

Limits of this study: Source read only, reference not executed. Current JetBrains product implementation unavailable here; full retrieval/index pipeline, all review/rerank executors and extension/IDE frontend not exhaustively studied. No runtime tests executed.

## Actions

### make_change

Surface: model XML action.

Input: file_name/original_code/new_code/replace_all

Result: updated staged file contents/diff, warning or retry

Lifecycle: serial edit, parse/lint versus baseline then stage

Authority: Model edits planned files through host parser.

Evidence: [e12](#evidence-e12), [e13](#evidence-e13), [e16](#evidence-e16), [e17](#evidence-e17), [e18](#evidence-e18).

### create_file

Surface: model XML action.

Input: file_name/new_code

Result: staged new file/error

Lifecycle: serial; existing directory/existence checks

Authority: Model in cloned repository file staging.

Evidence: [e15](#evidence-e15), [e18](#evidence-e18).

### submit_task / submit_result

Surface: model XML action.

Input: submission justification text

Result: next task SUCCESS or DONE

Lifecycle: marks pending task complete even when initial no-change check failed

Authority: Model; host also synthesizes submission.

Evidence: [e14](#evidence-e14), [e11](#evidence-e11).

### no_tool_call

Surface: host recovery action.

Input: unparsed/no tool output

Result: format instructions

Lifecycle: continues bounded edit loop

Authority: Host recovery; no external work.

Evidence: [e18](#evidence-e18), [e12](#evidence-e12).

### modify(...) / modify.stream(...)

Surface: programmable API.

Input: FileChangeRequests, clone, context and prior staged files

Result: final file map or intermediate code suggestion stream

Lifecycle: bounded generator; final return consumed by StreamableFunction

Authority: Program authors; loop/prompts construction-time.

Evidence: [e6](#evidence-e6), [e7](#evidence-e7), [e8](#evidence-e8), [e9](#evidence-e9).

### sweep run / sweep watch / GitHub issue webhook

Surface: operator/service commands.

Input: issue URL/repo events and GitHub credentials

Result: tracking ID, progress comments, branch/commit/PR

Lifecycle: thread per issue; replacement injects SystemExit without join/quiescence; watch untraced beyond command existence

Authority: Operator configured installation delegates repo writes.

Evidence: [e2](#evidence-e2), [e3](#evidence-e3), [e4](#evidence-e4), [e5](#evidence-e5).

### PR comments / review webhook

Surface: operator/service actions.

Input: comment or changed PR

Result: queued edit follow-up/review comments

Lifecycle: per-PR comments queued; review replaces prior worker; five seeded review processes

Authority: Authorized GitHub event; no agent-to-agent messaging.

Evidence: [e2](#evidence-e2), [e24](#evidence-e24), [e25](#evidence-e25).

### check_syntax / get_check_results

Surface: supporting validation APIs.

Input: path and candidate code

Result: parse/linter result relative to baseline

Lifecycle: host parser and npx subprocess deadlines

Authority: Host code; not exposed general process/kernel tool.

Evidence: [e17](#evidence-e17), [e22](#evidence-e22).

### ChatLogger.add_chat

Surface: supporting persistence API.

Input: normalized messages/model/output/meta

Result: optional Mongo documents with index/expiration

Lifecycle: background writer thread per call; no ack/fail transaction, TTL expiry

Authority: Host logger consumes external DB, not model table query.

Evidence: [e23](#evidence-e23), [e10](#evidence-e10).

## Capabilities

### filesystem

**I — Files** (source): Model make_change/create_file stage code; host clone/rename/commit/PR writes.

Evidence: [e7](#evidence-e7), [e15](#evidence-e15), [e16](#evidence-e16), [e4](#evidence-e4), [e5](#evidence-e5).

### processes

**S — OS programs** (source): Host linter subprocesses, API worker threads, review multiprocessing pool; no general model Bash action in traced edit set.

Evidence: [e22](#evidence-e22), [e2](#evidence-e2), [e24](#evidence-e24), [e18](#evidence-e18).

### code-actions

**L — Code actions** (source): Model text edits with parse/lint feedback; no generic model code execution in traced dispatch.

Evidence: [e17](#evidence-e17), [e18](#evidence-e18), [e22](#evidence-e22).

### persistent-kernel

**? — Kernel** (source): No persistent code kernel/native process namespace in API/edit/provider/linter paths.

### standing-database

**S — Standing DB** (source): ChatLogger consumes Mongo for optional normalized chats/tickets, but model has no table/database actions.

Evidence: [e23](#evidence-e23).

### workflow-programming

**S — Workflows** (source): Python callable streamable plan/edit/validate/publish stages; fixed host pipeline, not model-programmable general orchestration.

Evidence: [e4](#evidence-e4), [e6](#evidence-e6), [e7](#evidence-e7), [e8](#evidence-e8).

### multi-model

**I — Models** (source): Edit retry escalation uses configured faster/slower model; no simultaneous peer roles demonstrated by provider selection.

Evidence: [e11](#evidence-e11), [e19](#evidence-e19), [e20](#evidence-e20).

### live-collaboration

**L — Peer chat** (source): Seeded independent review samples voted after join and GitHub human comments; no running model peer bus.

Evidence: [e24](#evidence-e24), [e2](#evidence-e2).

### concurrent-work

**I — Concurrency** (source): Independent issue/review threads, logging threads and five review processes; comment sequencing per PR queue.

Evidence: [e2](#evidence-e2), [e23](#evidence-e23), [e24](#evidence-e24), [e25](#evidence-e25).

### steering-interrupt

**L — Steer/interrupt** (source): New issue/review injects SystemExit to old Python thread; no join/provider/process-quiescence handshake. Comments queued.

Evidence: [e2](#evidence-e2), [e20](#evidence-e20), [e24](#evidence-e24).

### turn-redefinition

**S — Turn program** (source): Replaceable Python generator stages/prompts can be programmed by host; model native edit tools do not install a new live turn.

Evidence: [e7](#evidence-e7), [e8](#evidence-e8), [e9](#evidence-e9).

### compaction

**L — Compaction** (source): After changed completed task both model and detailed logger histories replaced with system/current code state; no managed choice/retention transaction.

Evidence: [e10](#evidence-e10).

### context-repair

**L — Repair** (source): Current files/plan re-rendered, prior detail lost from both histories and optional expiring logs; no model original-message repair API traced.

Evidence: [e10](#evidence-e10), [e23](#evidence-e23).

### original-audit

**L — Original audit** (source): Optional Mongo normalized chat logging is detached, expiring and may omit earlier detail after reset; synthetic assistant calls and stop truncation mean not original provider audit.

Evidence: [e9](#evidence-e9), [e10](#evidence-e10), [e19](#evidence-e19), [e20](#evidence-e20), [e23](#evidence-e23).

### audit-query

**S — Audit query** (source): Mongo records and external progress IDs aid operator study; no native model audit query.

Evidence: [e23](#evidence-e23), [e2](#evidence-e2).

### hot-change

**? — Hot change** (source): No after-turn/workflow activation or hot prompt/program reload transaction in inspected captured pipeline.

### rebuild-continuity

**? — Rebuild continuity** (source): Webhook/CLI reruns new pipeline; no paused owned work/outpost/recompile/reinhabitation in inspected paths.

### remote-services

**I — Remote** (source): Consumes GitHub repository APIs, OpenAI/Anthropic/Bedrock providers and optional Mongo.

Evidence: [e3](#evidence-e3), [e5](#evidence-e5), [e19](#evidence-e19), [e20](#evidence-e20), [e23](#evidence-e23).

### self-improvement

**S — Self-improve** (source): Automatic code-edit/lint repair and multi-sample review are search primitives for target repo, not governed harness self-improvement.

Evidence: [e17](#evidence-e17), [e11](#evidence-e11), [e24](#evidence-e24).

### complaints

**L — Complaints** (source): Tracking IDs/errors and PostHog host events exist; no model complaint with captured agent state/bead custody.

Evidence: [e2](#evidence-e2), [e6](#evidence-e6).

### authority

**I — Authority** (source): Configured GitHub delegation automatically commits/creates regular PR; edit tool directory/existence checks, no individual command gate in this path.

Evidence: [e3](#evidence-e3), [e4](#evidence-e4), [e5](#evidence-e5), [e15](#evidence-e15).

### evaluation

**L — Evaluation** (source): Inspected matching test has concrete deterministic oracles; linter syntax checks are not behavioral correctness and skipped tasks still complete.

Evidence: [e26](#evidence-e26), [e17](#evidence-e17), [e18](#evidence-e18), [e14](#evidence-e14).

### time-order

**L — Time/order** (source): Tracking IDs/queue sequence and Mongo index structure events; detached shared-index logging and destructive context prevent complete causal ordering.

Evidence: [e2](#evidence-e2), [e23](#evidence-e23), [e25](#evidence-e25), [e10](#evidence-e10).

## Inspected test oracles

- [quarantine/sweep/sweepai/utils/search_and_replace_test.py](../../../quarantine/sweep/sweepai/utils/search_and_replace_test.py): Matching placement and whitespace Oracle: Exact expected scores/first-occurrence/no-match and whitespace positives/negative; primitive only, not edit completion/process quiescence. Read, **not executed**.

## Useful mechanisms

- Same streamable generator supports final return and incremental program consumption.
- Baseline-relative parser/linter feedback and concrete staged diffs.
- Public repository writes expressed as complete issue-to-PR workflow.

## Material limits

- Current JetBrains product implementation unavailable here; full retrieval/index pipeline, all review/rerank executors and extension/IDE frontend not exhaustively studied. No runtime tests executed.

## Arconaut design questions

- Can task completion distinguish skipped, validated and behaviorally successful work?
- Can human steering replace async thread-exception injection with owned resource cancellation and observations?
- Can actual model responses be captured before synthetic assistant calls and code-state compaction?

## Evidence

### Evidence e1

[quarantine/sweep/README.md:1–5](../../../quarantine/sweep/README.md#L1): Pinned repo directs current product to JetBrains; inspectable source is earlier GitHub automation, not proof of new product mechanics.

### Evidence e2

[quarantine/sweep/sweepai/api.py:106–229](../../../quarantine/sweep/sweepai/api.py#L106): Ticket/review workers per repo key and Python async SystemExit injection; comments queued per PR without restart.

### Evidence e3

[quarantine/sweep/sweepai/cli.py:325–357](../../../quarantine/sweep/sweepai/cli.py#L325): CLI run(issue_url) loads configured GitHub request and invokes on_ticket.

### Evidence e4

[quarantine/sweep/sweepai/handlers/on_ticket.py:519–579](../../../quarantine/sweep/sweepai/handlers/on_ticket.py#L519): Planner streams FileChangeRequests; branch creation, modify, summary and commit pipeline.

### Evidence e5

[quarantine/sweep/sweepai/handlers/on_ticket.py:620–668](../../../quarantine/sweep/sweepai/handlers/on_ticket.py#L620): Changes summarized and normal draft=False GitHub PR created.

### Evidence e6

[quarantine/sweep/sweepai/handlers/create_pr.py:39–127](../../../quarantine/sweep/sweepai/handlers/create_pr.py#L39): Modify pipeline consumes streamable return; updates in-memory file statuses and analytics.

### Evidence e7

[quarantine/sweep/sweepai/agents/modify.py:63–82](../../../quarantine/sweep/sweepai/agents/modify.py#L63): Modify is streamable generator, captures configured prompt and renames local cloned files.

### Evidence e8

[quarantine/sweep/sweepai/utils/streamable_functions.py:7–30](../../../quarantine/sweep/sweepai/utils/streamable_functions.py#L7): Same generator callable exposes intermediate stream or consumes final return.

### Evidence e9

[quarantine/sweep/sweepai/agents/modify.py:94–163](../../../quarantine/sweep/sweepai/agents/modify.py#L94): Edit loop initial plan state, synthetic precompiled assistant calls, bounded 15*task iteration.

### Evidence e10

[quarantine/sweep/sweepai/agents/modify.py:173–215](../../../quarantine/sweep/sweepai/agents/modify.py#L173): Task progress replaces both model and detailed logger history with system/current state; DONE logs diffs.

### Evidence e11

[quarantine/sweep/sweepai/agents/modify.py:236–286](../../../quarantine/sweep/sweepai/agents/modify.py#L236): Auto submit compiled tasks, MODEL→SLOW_MODEL escalation and repeated-output skip.

### Evidence e12

[quarantine/sweep/sweepai/agents/modify_utils.py:625–640](../../../quarantine/sweep/sweepai/agents/modify_utils.py#L625): Parses XML calls, picks first recognized tool; parser later enumerates by tool order rather than occurrence.

### Evidence e13

[quarantine/sweep/sweepai/core/chat.py:178–194](../../../quarantine/sweep/sweepai/core/chat.py#L178): XML parser groups matches per known tool name; no native provider tool schema in edit path.

### Evidence e14

[quarantine/sweep/sweepai/agents/modify_utils.py:897–920](../../../quarantine/sweep/sweepai/agents/modify_utils.py#L897): submit_task marks next pending task completed; no-change failure calculation overwritten by SUCCESS except all-done sentinel.

### Evidence e15

[quarantine/sweep/sweepai/agents/modify_utils.py:922–959](../../../quarantine/sweep/sweepai/agents/modify_utils.py#L922): create_file records text into file dict after directory/existence checks.

### Evidence e16

[quarantine/sweep/sweepai/agents/modify_utils.py:980–1025](../../../quarantine/sweep/sweepai/agents/modify_utils.py#L980): make_change operates latest code from clone/file dict; original/new code and replace_all.

### Evidence e17

[quarantine/sweep/sweepai/agents/modify_utils.py:1124–1163](../../../quarantine/sweep/sweepai/agents/modify_utils.py#L1124): Replacement and parser/linter comparison, baseline-relative failing-parse detection.

### Evidence e18

[quarantine/sweep/sweepai/agents/modify_utils.py:1182–1229](../../../quarantine/sweep/sweepai/agents/modify_utils.py#L1182): Repeated error marks task complete/SKIPPED and may return DONE; only make_change/create_file/submit/no-tool dispatch.

### Evidence e19

[quarantine/sweep/sweepai/core/chat.py:397–436](../../../quarantine/sweep/sweepai/core/chat.py#L397): Synchronous OpenAI streaming SDK call with textual chunks and stop sequence truncation.

### Evidence e20

[quarantine/sweep/sweepai/core/chat.py:433–474](../../../quarantine/sweep/sweepai/core/chat.py#L433): Anthropic/Bedrock stream normalizes messages and accumulates text; no request cancellation contract here.

### Evidence e21

[quarantine/sweep/sweepai/core/chat.py:663–711](../../../quarantine/sweep/sweepai/core/chat.py#L663): Continuation extends long textual replies until stop sequence/maxcalls; edits previous assistant content.

### Evidence e22

[quarantine/sweep/sweepai/utils/code_validators.py:499–547](../../../quarantine/sweep/sweepai/utils/code_validators.py#L499): Syntax/linter validation; host npx subprocess shell with 5/30s timeout, no native model shell.

### Evidence e23

[quarantine/sweep/sweepai/utils/chat_logger.py:40–82](../../../quarantine/sweep/sweepai/utils/chat_logger.py#L40): Optional consumed Mongo chat store, TTL, detached logging threads and shared index increment.

### Evidence e24

[quarantine/sweep/sweepai/core/review_utils.py:1016–1090](../../../quarantine/sweep/sweepai/core/review_utils.py#L1016): Five seeded reviewer processes or sequential samples; joins process pool before collecting/voting; not live peer bus.

### Evidence e25

[quarantine/sweep/sweepai/utils/safe_pqueue.py:5–32](../../../quarantine/sweep/sweepai/utils/safe_pqueue.py#L5): Per-PR queue priority invalidation, lock, silent put exceptions.

### Evidence e26

[quarantine/sweep/sweepai/utils/search_and_replace_test.py:15–61](../../../quarantine/sweep/sweepai/utils/search_and_replace_test.py#L15): Whitespace, exact, ambiguous first-match and no-match assertions for matching primitive.

