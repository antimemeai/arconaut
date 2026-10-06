# station

Room/action scientific ecosystem: heterogeneous researcher agents exchange capsules/mail, delegate experiments to coder workers, receive task scores and independent report audits.

Role: scientific multi-agent environment. Runtime: Python.

Pinned source: [https://github.com/dualverse-ai/station](https://github.com/dualverse-ai/station); revision/version `782088e561998bf357d95a838a0566a8229425d9`.

Python threaded tick orchestrator, concurrent stateful provider connectors, serial action commits/fast-lane writers, owned CLI coder/auditor sessions and fresh Python experiment subprocesses.

Owns ecosystem scheduling, agent YAML/history, shared research storage/index and coder/evaluation processes. This is a research control plane, not merely a consumer of independently governed shared kernels.

Inspection: provider/staged tick, parser/room dispatch, mail concurrent-state merge, context anchors, persistent index/history scope, coder/process lifecycle, experimental verdict/repair, configuration refresh and selected fault-class tests

Limits of this study: Full domain evaluator correctness, all providers, room/action catalogs, multistart/lineage/controller internals, web authorization and whole-system crash atomicity remain uninspected.

## Actions

### /execute_action{command args} / submit_response

Surface: researcher model textual action protocol.

Input: Native room command/args plus dictionary YAML block; multiple actions capped, navigation and role constraints.

Result: Ordered action result strings, room state/notifications and optional first multistep internal handler.

Lifecycle: Parsed after provider response; ordinary actions committed in turn order; fast-lane experiment submission can occur earlier.

Authority: Active agent identity, room/guest/maturity/cooldown/supervisor constraints; no unrestricted general shell tool.

Evidence: [e1](#evidence-e1), [e2](#evidence-e2), [e3](#evidence-e3), [e4](#evidence-e4), [e7](#evidence-e7).

### Mail create / reply

Surface: researcher model room action.

Input: YAML title/content/recipient list, capsule/message numeric ID for reply.

Result: Persistent mail capsule/message ID plus full-content pending recipient notification.

Lifecycle: Recipients must be mature recursive agents; atomic notification append survives concurrent turn; recipient read IDs marked on notification delivery, not explicit ack.

Authority: Room participant/role checks; next observation incorporates mail, no active provider injection.

Evidence: [e10](#evidence-e10), [e11](#evidence-e11), [e12](#evidence-e12), [e13](#evidence-e13), [e14](#evidence-e14), [e15](#evidence-e15).

### Research Center read_task / submit

Surface: researcher model room action.

Input: Read task; YAML title/tags/abstract/instruction with artifact references.

Result: Queued evaluation ID, status/notification or limit/validation denial.

Lifecycle: Coder delegates experiment execution; fast-lane provisional run/op identity, timeout may still leave queued request proceeding.

Authority: Recursive researcher after task read, holiday/cooldown/author active limit; supervisors forbidden.

Evidence: [e26](#evidence-e26), [e27](#evidence-e27), [e28](#evidence-e28).

### Research Center storage info / list / read

Surface: researcher model room action.

Input: Storage path and page, native read or storage read/list.

Result: Paged path/list/text in next System Messages.

Lifecycle: Persistent shared/lineage storage; read cooldown where configured, write/delete refused here.

Authority: Researcher read-only; separately launched coder has code/data writes.

Evidence: [e29](#evidence-e29).

### CliJobManager launch / poll / stop

Surface: runtime supplied worker API.

Input: Evaluation/job spec, backend command/model/env/workspace/storage/network options, resume token.

Result: Active process session ID/PID/transcript/stderr/report paths and exit/retry state.

Lifecycle: Start new OS session. Abort waits/escalates direct child; stop merely signals then closes/clears, so caller must not assume settled descendants.

Authority: Station governs its coder/auditor workers and storage; no shared-service lease established.

Evidence: [e30](#evidence-e30), [e31](#evidence-e31), [e35](#evidence-e35).

### execute_submission_in_python_sandbox

Surface: owned evaluation runtime primitive.

Input: Task code/result wrapper, tmp storage, CPU/GPU IDs, timeout/memory settings.

Result: Structured success/result/log/stdout/stderr or timeout/memory/error.

Lifecycle: Fresh Python subprocess for each experiment, group kill/child wait on timeout; not a persistent shared kernel.

Authority: Full ordinary Python OS behavior within configured limits; sandbox name does not establish namespace/security isolation.

Evidence: [e32](#evidence-e32), [e33](#evidence-e33), [e34](#evidence-e34).

### submit_audit.sh <evaluation_id> pass|fail / submit_audit

Surface: auditor model CLI artifact API.

Input: Existing nonempty audit report and literal pass/fail.

Result: Verdict artifact/status; manager finalizes report or requests bounded repair.

Lifecycle: Temporary-file replace, later invocation can replace verdict; failing critique resumes previous coder where supported, otherwise fresh worker; exhausted repair partial.

Authority: Separate scientific auditor role instructed not to alter experiment or make new official submission; trust is model judgement plus task oracle.

Evidence: [e35](#evidence-e35), [e36](#evidence-e36), [e37](#evidence-e37).

### maybe_run_context_compaction_after_turn / sync_state

Surface: runtime maintenance/provider API.

Input: Agent token threshold, full model-generated summary, protected context and tick anchor; disk pruning/system role changes.

Result: Persisted pending/anchored summary and reloaded effective provider history/token estimate.

Lifecycle: After-turn maintenance generates summary; next observation includes summary/protected items; originals ordinarily retained until separate recovery/retention mutation.

Authority: Orchestrator-managed context; model supplies summary, no general model-controlled arbitrary turn program.

Evidence: [e16](#evidence-e16), [e17](#evidence-e17), [e18](#evidence-e18), [e19](#evidence-e19), [e20](#evidence-e20).

### _get_current_connector_for_agent

Surface: runtime provider dispatch API.

Input: Agent provider/model/system settings and current runtime API config generation.

Result: Existing current-generation connector or reinitialized factory instance.

Lifecycle: Refresh before new requests; already-running requests are not repointed; failed creation can return older connector. General constants require restart.

Authority: Operator/server configuration; model provider choice independent per external researcher.

Evidence: [e9](#evidence-e9), [e42](#evidence-e42).

### suggest / request_human

Surface: researcher model room action.

Input: Suggestion content, or assistance title/content.

Result: Suggestion YAML append or human request ID with awaiting-intervention flag.

Lifecycle: Suggest does not itself demand repair; request_human pauses participation. Persistence helper may hide failure; no automatic full-state capture/database bead.

Authority: Recursive room participation/administrative request; operator resolves assistance separately.

Evidence: [e39](#evidence-e39), [e40](#evidence-e40), [e23](#evidence-e23).

### request_manual_pause

Surface: operator orchestration API.

Input: No args; current running/paused state.

Result: Pending control event/message.

Lifecycle: Requests boundary pause, not cancellation/settlement of existing requests/jobs.

Authority: Operator control plane.

Evidence: [e41](#evidence-e41).

### load_yaml_lines_tick_window

Surface: dashboard/service program API.

Input: Dialogue YAML log path, earliest/recent and tick limit.

Result: Chronological decoded entry list plus window metadata.

Lifecycle: Queries retained records; malformed/read errors may return partial and original deleted/expired records unavailable.

Authority: Host/dashboard file access, not unrestricted model SQL.

Evidence: [e38](#evidence-e38).

## Capabilities

### filesystem

**L — Files** (source): Researcher native storage actions read/list only; coder/evaluator processes write persistent code/data with separate authority. Room capsules, indices and histories are host-owned files.

Evidence: [e29](#evidence-e29), [e30](#evidence-e30), [e32](#evidence-e32).

### processes

**I — OS programs** (source): Owns coder/auditor CLI OS sessions and fresh Python experiment groups. Timeout/abort can wait/escalate; stop_active_cli_jobs signals and clears without settlement, a material refit distinction.

Evidence: [e30](#evidence-e30), [e31](#evidence-e31), [e33](#evidence-e33), [e34](#evidence-e34).

### code-actions

**I — Code actions** (source): Model-authored instructions delegate actual coding experiments to CLI worker and fresh Python executor, returning evaluation/report IDs. This is not a direct resident Python kernel tool.

Evidence: [e27](#evidence-e27), [e28](#evidence-e28), [e30](#evidence-e30), [e32](#evidence-e32), [e33](#evidence-e33).

### persistent-kernel

**? — Kernel** (source): No shared persistent computation kernel established in inspected worker/evaluation paths; experiments use fresh temporary Python subprocesses and coder provider histories are not kernel state.

Evidence: [e30](#evidence-e30), [e32](#evidence-e32), [e33](#evidence-e33).

### standing-database

**S — Standing DB** (source): Persistent agent/capsule/evaluation state and supplied SQLite capsule metadata index support shared ecosystem data; native researcher reads are domain room/capsule operations, not generic SQL DB access.

Evidence: [e43](#evidence-e43), [e44](#evidence-e44).

### workflow-programming

**S — Workflows** (source): Action chains, navigation, supplied InternalActionHandler init/step, concurrent tick phase and model-programmed experiment instructions compose workflows. Core room/tick programs remain developer-owned.

Evidence: [e1](#evidence-e1), [e3](#evidence-e3), [e4](#evidence-e4), [e5](#evidence-e5), [e27](#evidence-e27).

### multi-model

**I — Models** (source): Independent per-agent provider/model factory, stateful history and concurrent turn requests support heterogeneous researchers; delegated coder/auditor are additional roles.

Evidence: [e5](#evidence-e5), [e6](#evidence-e6), [e9](#evidence-e9), [e35](#evidence-e35).

### live-collaboration

**I — Peer chat** (source): Mature recursive peers exchange mail content/capsule IDs and pending notifications; latest-state merge preserves arrivals during another model generation. It is queued context, not provider-stream interruption.

Evidence: [e10](#evidence-e10), [e11](#evidence-e11), [e12](#evidence-e12), [e13](#evidence-e13).

### concurrent-work

**L — Concurrency** (source): Concurrent provider generation, serial ordinary action commit and single-writer provisional fast lanes; worker timeout does not cancel request, tick rollback can delete uncommitted history and restart tick.

Evidence: [e5](#evidence-e5), [e7](#evidence-e7), [e25](#evidence-e25), [e26](#evidence-e26), [e27](#evidence-e27).

### steering-interrupt

**L — Steer/interrupt** (source): Pending mail/operator pause at agent boundary; request_human flags awaiting intervention. Existing provider calls are not canceled by pause and worker stop need not settle programs.

Evidence: [e10](#evidence-e10), [e31](#evidence-e31), [e40](#evidence-e40), [e41](#evidence-e41).

### turn-redefinition

**S — Turn program** (source): Maintainer extension interface supplies multistep init/step room handlers and provider state rebuild on disk prompt/pruning changes; no model API replacing arbitrary core turn dispatcher during ordinary operation.

Evidence: [e3](#evidence-e3), [e4](#evidence-e4), [e20](#evidence-e20).

### compaction

**I — Compaction** (source): After-turn summary is persisted as pending tick event; next observation injects full summary/protected messages and anchors provider effective history. Unknown token recount flagged stale; compaction call itself recorded.

Evidence: [e16](#evidence-e16), [e17](#evidence-e17), [e18](#evidence-e18), [e19](#evidence-e19), [e20](#evidence-e20).

### context-repair

**S — Repair** (source): Saved unfiltered history, mutable pruning/anchor metadata and sync_state allow rebuilding effective chat; protected notifications survive compaction. Separate recovery intentionally deletes uncommitted originals, limiting reconstruction.

Evidence: [e17](#evidence-e17), [e18](#evidence-e18), [e19](#evidence-e19), [e20](#evidence-e20), [e25](#evidence-e25).

### original-audit

**L — Original audit** (source): Decoded provider text/thinking/history/metadata and tick snapshots retained; raw API sanitizer drops fields/clips, empty turns omitted and append failure swallowed. Recovery deletes uncommitted same-tick history, snapshots expire. Not every original provider byte/effect.

Evidence: [e6](#evidence-e6), [e8](#evidence-e8), [e21](#evidence-e21), [e22](#evidence-e22), [e23](#evidence-e23), [e24](#evidence-e24), [e25](#evidence-e25).

### audit-query

**L — Audit query** (source): Dashboard/service tick-window queries and native domain capsule/evaluation references inspect retained histories; partial parse/read returns possible and deleted/expired originals unrecoverable.

Evidence: [e24](#evidence-e24), [e25](#evidence-e25), [e38](#evidence-e38).

### hot-change

**L — Hot change** (source): New requests refresh runtime provider/API generation; disk prompt/pruning/anchor differences rebuild effective chat. General constant_config requires restart; failed chat reinit logs but may proceed stale. No global turn/workflow change barrier.

Evidence: [e9](#evidence-e9), [e20](#evidence-e20), [e42](#evidence-e42).

### rebuild-continuity

**? — Rebuild continuity** (source): Inspected code reconstructs provider history and guards interrupted ticks/workers, but no compile/refit/outpost handoff or executable harness reinhabitation with all owned activity quiesced is established.

Evidence: [e20](#evidence-e20), [e25](#evidence-e25), [e31](#evidence-e31).

### remote-services

**I — Remote** (source): Provider requests consume remote models through per-agent factory; CLI coders/auditors may receive network access. Shared experiment infrastructure is generally governed by Station, not external fabric consumers.

Evidence: [e9](#evidence-e9), [e30](#evidence-e30), [e35](#evidence-e35).

### self-improvement

**? — Self-improve** (source): Scientific experiment code/report repair and suggestion filing are implemented; model-governed experimental improvement/activation of Station harness itself not established in inspected paths.

Evidence: [e36](#evidence-e36), [e39](#evidence-e39).

### complaints

**L — Complaints** (source): Native suggest logs authored content/agent/tick without obligation; no automatic full agent-state capture/DB bead or follow-up priority. request_human is a different, participation-blocking assistance mechanism.

Evidence: [e39](#evidence-e39), [e40](#evidence-e40), [e23](#evidence-e23).

### authority

**I — Authority** (source): Active identity/action cap/ascension filter, room role/maturity/holiday/task/cooldown limits; researcher storage read-only while code/evaluator workers have broader OS/network authority. Prompted auditor constraint is not OS fencing.

Evidence: [e2](#evidence-e2), [e10](#evidence-e10), [e28](#evidence-e28), [e29](#evidence-e29), [e32](#evidence-e32), [e35](#evidence-e35).

### evaluation

**I — Evaluation** (source): Research delegation supplies task score/result protocol plus separate auditor model checking code/official results/report, pass/fail/repair/partial. Numeric/domain truth depends on concrete task evaluator; critique tests verify state transitions, not scientific correctness.

Evidence: [e32](#evidence-e32), [e35](#evidence-e35), [e36](#evidence-e36), [e37](#evidence-e37), [e45](#evidence-e45).

### time-order

**I — Time/order** (source): Explicit tick/turn order, concurrent requests then serial ordinary commits, provisional op IDs and pending/anchored compaction ticks; physical fast-lane/external effect order differs from display turn order.

Evidence: [e5](#evidence-e5), [e7](#evidence-e7), [e17](#evidence-e17), [e25](#evidence-e25), [e26](#evidence-e26), [e27](#evidence-e27).

## Inspected test oracles

- [quarantine/station/tests/test_cross_agent_notification_atomic.py](../../../quarantine/station/tests/test_cross_agent_notification_atomic.py): Concurrent notification at shared broadcast update. Oracle: Read1–168: test manager injects a background notification at load/update boundary, then asserts both preexisting evaluator message and new public capsule message remain. Deterministic lost-update oracle; no live parallel provider validation. Read, **not executed**.
- [quarantine/station/tests/test_context_compaction.py](../../../quarantine/station/tests/test_context_compaction.py): Persisted maintenance history and concurrent protected/anchor merge. Oracle: Read27–81,200–264: fake connector sees persist=True and reloaded, exact summary saved; fixed lists assert concurrent/turn items and anchor replacement. Good wiring/merge assertions, not actual provider compression or original-byte retention. Read, **not executed**.
- [quarantine/station/tests/test_parallel_research_sync.py](../../../quarantine/station/tests/test_parallel_research_sync.py): Provisional recovery and deliberately removed uncommitted originals. Oracle: Read473–530,595–625: tombstone status/notification suppression, new ID3 and committed record preserved; exact agent histories[6] versus[6,7,7] establish real deletion of uncommitted turns. This recovery oracle explicitly contradicts universal append-original audit. Read, **not executed**.
- [quarantine/station/tests/test_cli_job_manager.py](../../../quarantine/station/tests/test_cli_job_manager.py): Stop versus awaited abort lifecycle. Oracle: Read261–303: mocked abort asserts TERM→KILL plus two waits; stop asserts signal→requeue→close→close and empty map, with no wait. Tests confirm bookkeeping order, not OS descendant settlement. Read, **not executed**.
- [quarantine/station/tests/test_research_audit.py](../../../quarantine/station/tests/test_research_audit.py): Verdict artifact input/state and repair exhaustion. Oracle: Read31–123: missing report fails, existing pass remains on invalid verdict, temp artifact absent after replace; pass completes, fail queues repair then partial and final report. Prompts inspected by substring assertions; no independent scientific truth oracle. Read, **not executed**.
- [quarantine/station/tests/test_lazy_connector_and_mail.py](../../../quarantine/station/tests/test_lazy_connector_and_mail.py): Lazy provider dispatch and role-scoped mail directory. Oracle: Read1–225: constructor raising verifies no eager initialization, first use creates once, stale directory identity raises and guest excluded; fake providers do not test live connection continuity. Read, **not executed**.

## Useful mechanisms

- Concrete per-agent heterogeneous providers plus queued peer collaboration with lost-update merge.
- Compaction events distinguish pending summary from actual anchored effective context and preserve protected task data.
- Researcher/coder/auditor authority and task score versus critique are explicit separate roles.
- Provisional evaluation tombstones retain IDs and suppress stale notifications.
- Core Python with ordinary subprocess/thread semantics; source openly shows its owned control-plane boundaries.

## Material limits

- No resident/shared computation kernel established; Station owns its research process/service ecosystem.
- No complete original audit: API projections, swallowed append failures, deleted uncommitted history and expiring snapshots.
- Fast-lane timeout returns without canceling queued/active side effect; recovery is rollback/retry, not universal effect settlement.
- Stopping coder workers signals/closes/forgets without waiting; abort has a stronger separate path.
- Scientific repair is not model-governed harness autodroit; suggestion lacks automatic state bundle/database.

## Arconaut design questions

- Can context retry/rollback change effective history without ever deleting provider originals from the core audit?
- How should a fast-lane caller report unknown outcome and later resolve the same operation ID rather than exposing timeout as nonexecution?
- What actor owns a shared evaluation/kernel service: Arconaut consumer or independently governed fabric?
- Can model-governed experimental harness changes use task-derived oracles plus independent critique without conflating model agreement with scientific truth?
- How should an always-available nonblocking complaint capture state and remain separate from blocking human intervention?

## Evidence

### Evidence e1

[quarantine/station/station/action_parser.py:262–333](../../../quarantine/station/station/action_parser.py#L262): Parser normalizes action chains, neutralizes YAML action strings/removes thought blocks, extracts native command/args and safe-loaded dictionary YAML or parsing error.

### Evidence e2

[quarantine/station/station/station.py:767–857](../../../quarantine/station/station/station.py#L767): submit_response logs original response then checks active identity; truncates to MAX_ACTIONS_PER_TURN and suppresses non-navigation actions on ascension turns.

### Evidence e3

[quarantine/station/station/station.py:1183–1230](../../../quarantine/station/station/station.py#L1183): Current room dispatch calls native handle_action and collects action strings plus first internal handler; sequential location changes affect subsequent dispatch.

### Evidence e4

[quarantine/station/station/base_room.py:34–81](../../../quarantine/station/station/base_room.py#L34): RoomContext supplies shared managers; InternalActionHandler init/step provides optional multistep model continuation contract.

### Evidence e5

[quarantine/station/station/sync/parallel_runner.py:337–449](../../../quarantine/station/station/sync/parallel_runner.py#L337): Prepares saved observation/deep copy for each active agent; concurrent ThreadPool requests collected with as_completed, errors become explicit initial results.

### Evidence e6

[quarantine/station/station/sync/parallel_runner.py:490–587](../../../quarantine/station/station/sync/parallel_runner.py#L490): Staged send temporarily disables connector disk persistence, logs decoded prompt/thinking/result, captures token/API metadata and restores persist flag; errors pause orchestrator.

### Evidence e7

[quarantine/station/station/sync/parallel_runner.py:177–235](../../../quarantine/station/station/sync/parallel_runner.py#L177): Initial responses commit in prepared turn order after original staged history flush; provider completion order differs from normal action commit order.

### Evidence e8

[quarantine/station/station/sync/parallel_runner.py:840–904](../../../quarantine/station/station/sync/parallel_runner.py#L840): Staged user/model turns persist via provider append with metadata/signature fallbacks; connector may reload afterward. Persistence adapter errors can be hidden below this method.

### Evidence e9

[quarantine/station/station/station_runner.py:701–760](../../../quarantine/station/station/station_runner.py#L701): Provider factory binds per-agent model/system role/settings; runtime API generation lock recreates connector for new requests, preserving disk state rather than replacing in-flight calls.

### Evidence e10

[quarantine/station/station/rooms/mail.py:192–277](../../../quarantine/station/station/rooms/mail.py#L192): Mail notification/update under agent functional lock deduplicates pending/shown content, updates read IDs, validates mature recursive recipients; delivery is state mutation, not current provider interrupt.

### Evidence e11

[quarantine/station/station/rooms/mail.py:315–402](../../../quarantine/station/station/rooms/mail.py#L315): Native create/reply validates permissions/YAML/recipients and stores capsule IDs; sender read state updated.

### Evidence e12

[quarantine/station/station/rooms/mail.py:497–545](../../../quarantine/station/station/rooms/mail.py#L497): New mail notification includes content and recipient read-item IDs; notification delivery automatically marks IDs read, not explicit model acknowledgement.

### Evidence e13

[quarantine/station/station/station.py:1255–1338](../../../quarantine/station/station/station.py#L1255): Final save merges newly arrived pending mail, protected items and compaction events against latest state instead of overwriting turn-start snapshot.

### Evidence e14

[quarantine/station/station/agent.py:432–469](../../../quarantine/station/station/agent.py#L432): Agent functional updates use per-file flock with monotonic timeout; file presence is not lock ownership.

### Evidence e15

[quarantine/station/station/agent.py:587–613](../../../quarantine/station/station/agent.py#L587): Functional update reloads latest active/inactive YAML inside lock, applies mutator and saves; errors print/false rather than a transaction over all station artifacts.

### Evidence e16

[quarantine/station/station/station_runner.py:867–1010](../../../quarantine/station/station/station_runner.py#L867): After-turn compaction generates/persists full model summary, records error/pause, saves pending anchor and reloads session in finally; separate provider call, not unlogged replacement.

### Evidence e17

[quarantine/station/station/agent.py:324–402](../../../quarantine/station/station/agent.py#L324): Compaction events preserve summary/tick with pending versus anchored status and latest-anchor lookup; event is updated keyed by tick.

### Evidence e18

[quarantine/station/station/station.py:1394–1486](../../../quarantine/station/station/station.py#L1394): Same-tick pending observation reused; next observation injects complete summary/protected messages and marks compaction anchor under atomic snapshot update.

### Evidence e19

[quarantine/station/station/llm_connectors/base.py:252–283](../../../quarantine/station/station/llm_connectors/base.py#L252): Effective history filters entries before context start tick while underlying saved history remains separate; malformed/missing-tick entries excluded.

### Evidence e20

[quarantine/station/station/llm_connectors/base.py:832–925](../../../quarantine/station/station/llm_connectors/base.py#L832): Before observation/send, disk pruning/system prompt/context anchor change rebuilds provider chat and attempts token recount; stale budgets flagged when unavailable, reinit exception logged and caller may proceed stale.

### Evidence e21

[quarantine/station/station/llm_connectors/openai.py:373–410](../../../quarantine/station/station/llm_connectors/openai.py#L373): OpenAI history appends decoded text/thinking/token and prepared metadata; empty pair omitted and append exceptions printed rather than propagated.

### Evidence e22

[quarantine/station/station/llm_connectors/base.py:529–585](../../../quarantine/station/station/llm_connectors/base.py#L529): API raw_return sanitizer drops generated text/content/arguments/delta/reasoning and clips long remaining strings; raw provider originals not universal.

### Evidence e23

[quarantine/station/station/file_io_utils.py:521–575](../../../quarantine/station/station/file_io_utils.py#L521): Append YAML flushes/fsync but catches and prints exceptions; callers can proceed as if durable recording succeeded.

### Evidence e24

[quarantine/station/station/sync/parallel_state.py:182–235](../../../quarantine/station/station/sync/parallel_state.py#L182): Completed tick snapshot directories expire by retention window (default10ticks); current recovery marker removed on completion.

### Evidence e25

[quarantine/station/station/sync/parallel_state.py:257–345](../../../quarantine/station/station/sync/parallel_state.py#L257): Incomplete tick recovery is not full replay journal: rollback provisional research/surveys, reset index, rewrite history deleting entire tick for history-flushed/action-uncommitted agents.

### Evidence e26

[quarantine/station/station/sync/fast_lane_service.py:25–103](../../../quarantine/station/station/sync/fast_lane_service.py#L25): Single writer owns queued copied request; caller timeout returns while queued/active request remains and worker eventually sets done. Stop joins bounded and drains queued work.

### Evidence e27

[quarantine/station/station/eval_research/submission_service.py:110–187](../../../quarantine/station/station/eval_research/submission_service.py#L110): Validated experiment instruction creates provisional evaluation keyed run/op ID and per-author active limit, wakes evaluator and returns evaluation ID/notification.

### Evidence e28

[quarantine/station/station/rooms/research_center.py:960–1057](../../../quarantine/station/station/rooms/research_center.py#L960): Research read_task and submit require recursive role, task read, nonholiday/non-supervisor/cooldown limits and title/tags/abstract/instruction; creates evaluation rather than executing researcher shell.

### Evidence e29

[quarantine/station/station/rooms/research_center.py:1175–1247](../../../quarantine/station/station/rooms/research_center.py#L1175): Research storage info/list/read returns paged pending notifications; write/delete explicitly rejected for researcher agents. Coder workers have separate storage authority.

### Evidence e30

[quarantine/station/station/workers/job_manager.py:249–302](../../../quarantine/station/station/workers/job_manager.py#L249): Coder/auditor backend prepares actual command and launches subprocess with transcript/stderr files, OS environment and new session; owned process/handle tracked.

### Evidence e31

[quarantine/station/station/workers/job_manager.py:432–472](../../../quarantine/station/station/workers/job_manager.py#L432): Abort checks direct child and waits/escalates group signals; stop_active_cli_jobs signals, invokes hook, closes handles and clears map without waiting for settlement.

### Evidence e32

[quarantine/station/station/eval_research/executor_sandbox.py:245–305](../../../quarantine/station/station/eval_research/executor_sandbox.py#L245): Experiment sandbox command is plain Python wrapper optionally taskset, fresh tmp storage lifetime; not a resident kernel or namespace security boundary.

### Evidence e33

[quarantine/station/station/eval_research/executor_sandbox.py:533–595](../../../quarantine/station/station/eval_research/executor_sandbox.py#L533): Fresh Python experiment subprocess owns new session and optional RLIMIT_AS/CPU affinity; network/OS containment not inferred from sandbox name.

### Evidence e34

[quarantine/station/station/eval_research/executor_sandbox.py:620–675](../../../quarantine/station/station/eval_research/executor_sandbox.py#L620): Timeout kills process group and waits child, joins output readers with bounds; clipped stderr and execution errors are structured outcomes, not arbitrary effect settlement.

### Evidence e35

[quarantine/station/station/eval_research/coder_manager.py:135–219](../../../quarantine/station/station/eval_research/coder_manager.py#L135): Independent auditor prompt judges instruction/code/official result/report, accepts honestly qualified budget limits and writes pass/fail report; backend CLI launch may have separately configured network access.

### Evidence e36

[quarantine/station/station/eval_research/coder_manager.py:1318–1421](../../../quarantine/station/station/eval_research/coder_manager.py#L1318): Pass finalizes report; nonfinal fail persists round report and resumes coder thread or queues fresh repair with one official attempt minimum; exhaustion partial, infrastructure blocks and pauses.

### Evidence e37

[quarantine/station/station/eval_research/submit_audit_cli.py:11–30](../../../quarantine/station/station/eval_research/submit_audit_cli.py#L11): Verdict helper validates pass/fail and nonempty report, writes temporary verdict then replace; not immutable after later invocation or fsync guarantee.

### Evidence e38

[quarantine/station/station/file_io_utils.py:304–360](../../../quarantine/station/station/file_io_utils.py#L304): Dashboard/program query reads earliest/recent tick window and chronological entries; missing/malformed reads print warnings and may return partial records.

### Evidence e39

[quarantine/station/station/rooms/misc.py:146–183](../../../quarantine/station/station/rooms/misc.py#L146): Model suggest records tick/name/authored suggestion YAML, not DB/full state; helper catches errors so success message can exceed persistence certainty.

### Evidence e40

[quarantine/station/station/rooms/admin.py:268–345](../../../quarantine/station/station/rooms/admin.py#L268): request_human records authored title/content/request ID, model/agent/tick/time and flags awaiting intervention; it is blocking assistance request, not optional rageshake.

### Evidence e41

[quarantine/station/station/station_runner.py:1422–1438](../../../quarantine/station/station/station_runner.py#L1422): Manual pause requests after current agent turn/before next; does not directly cancel already-running provider requests or jobs.

### Evidence e42

[quarantine/station/README.md:195–209](../../../quarantine/station/README.md#L195): General constant_config configuration requires Station restart; runtime provider-generation refresh is a separate narrower mechanism.

### Evidence e43

[quarantine/station/station/capsule_index.py:107–151](../../../quarantine/station/station/capsule_index.py#L107): Parameterized visible/type/tag capsule metadata listing uses persistent SQLite with optional limit and count; service owns index, model sees room contract.

### Evidence e44

[quarantine/station/station/capsule_index.py:235–254](../../../quarantine/station/station/capsule_index.py#L235): SQLite capsule index connection configures busy timeout, foreign keys, WAL/NORMAL when setup; source YAML and index distinct.

### Evidence e45

[quarantine/station/station/eval_research/base_evaluator.py:227–258](../../../quarantine/station/station/eval_research/base_evaluator.py#L227): Task evaluator wrapper calls concrete evaluate_submission and formats returned success/score/details/optional sort key; validity remains task-owned oracle.

