# Fresh Hermes: workflow mechanisms worth learning from

2026-10-07. Source-only study. Exact snapshots and restoration are in
[hermes-acquisition.json](hermes-acquisition.json). Hermes core is at
[a3ed4a173070](https://github.com/NousResearch/hermes-agent/tree/a3ed4a173070e981332e4d879ff6cc8b9efd57ab);
the independently maintained community plugin is at
[9fc82fe73555](https://github.com/jacobhausler/hermes-workflows/tree/9fc82fe73555145675b7947461af004980364ed2).
Neither upstream code nor tests were executed. Older references stay intact.

Hermes is several interacting workflow surfaces, not one scheduler. Preserve
that distinction when forming a Blackbird superset: reusable procedures, live
child execution, background task boards, timed/event automation, and a separate
community graph runner have different identities and recovery boundaries.

| Surface | Source actually read | Concrete lesson for Blackbird |
| --- | --- | --- |
| Skills | tools/skills_tool.py: skills_list, _skill_catalog, _skill_search_dirs, cache invalidation; developer creating-skills docs | Discoverable procedures with explicit resources/version/location; model-authored procedures should be normal editable programs, not only prompt text. |
| Delegation | tools/delegate_tool_dispatch.py: _Batch, _run_children_parallel, _units_of, _dispatch_background | Independent tasks and grouped joins, bounded child concurrency, parent/origin identity captured before child creation, progress/steer/cancel routing. A completed group need not wait for unrelated siblings. |
| Background delegation | same file: _resolve_async_wake_sid, _resolve_async_session_key, _dispatch_unit | A detached task needs an addressable owner/result route, not merely a background thread. Async admission and parent wake capability are explicit. |
| Cron | cron/jobs.py store/locking; cron/scheduler_tick.py: _tick_admitted; cron/executions.py transaction/owner tracking | Schedule policy, durable occurrence identity, tick ownership, next-run advancement and execution claims. Clock/DST/misfire policy belongs in explicit semantics. |
| Recovery | tests/cron/test_dead_owner_claim_reclaim.py; cron/executions.py | Dead worker recovery records unknown outcome. Process disappearance alone must not authorize duplicate effects. Test the dead-but-unreaped and live-but-slow cases separately. |
| Task graph/board | hermes_cli/kanban_db_graph.py: _validate_children_graph, decompose_triage_task; kanban_db_dispatch.py dispatcher/failure accounting; kanban_workflow.py | Dependencies, review/work lanes, task identity and execution lifecycle must compose. UI columns/manual moves do not themselves define the scheduler's kernel semantics. |
| Automation blueprints | website/docs/guides/automation-blueprints.md; plugin-catalog entries | Parameterized schedule/webhook/skill procedures and named callable workflows. Delivery destinations and trigger integrations belong in opt-in packages. |
| Community graph runner | hermes-workflows wf.py, wfcommon.py: efp, def_hash, save_node; README grammar | Discoverable named graphs, agent/gate/echo nodes, fan-outs/quorum, amend/resume/steer and compact status. Result invalidation propagates through ancestor definitions. |

## Particular seams we should use

Hermes delegates use completion-aware futures and a bounded executor, not a
fake join over callbacks already run inline. `_run_children_parallel` polls
FIRST_COMPLETED with a bounded wait and interrupt handling. Its documented
interrupted/abandoned behavior is a caution: returning control must not claim
that every descendant or remote provider stopped. The inspected interrupt/join
test is a useful fault scenario; it is source evidence, not a passed test here.

Cron has an execution ledger distinct from schedule configuration. The dead-owner
regression expects orphaned claimed/running executions to become unknown and
remain inspectable. Blackbird already has original identities/audit and station
admission; extend those rather than inventing another effect history.

Kanban's workflow module explicitly says it defines manual moves and columns,
not kernel-driven transitions. Avoid mistaking a configurable task board for a
fully programmable execution engine. Our board is an external view; task/run
state belongs to the execution mechanism.

The community workflow plugin is not bundled Hermes core. Its catalog pins an
older revision; we additionally fetched current main for study. It has its own
runner/process lifecycle, graph/event/result files, steering and gate state.
`wfcommon.efp` includes a node definition and its ancestor fingerprints;
`def_hash` intentionally excludes budgets/concurrency and selected policy
annotations. This is a useful way to let resource-budget changes preserve work
while semantic changes invalidate downstream results. It is not sufficient by
itself to prove a result reusable across changed inputs, environments or effects.

`save_node` writes a temporary result then replaces its name. Atomic visibility
is useful, but that operation alone does not establish power-loss durability or
an atomic transaction with a child program's external effects. Model output
validation, graph cache validity, execution outcome and publication authority
must be separately represented. Gates are optional workflow constructs, not a
mandatory tool-approval pageant.

## Proposed demonstrations

1. A model authors and starts a named review graph: heterogeneous colleagues run
   independently; a quorum join advances to synthesis; late steer reaches the
   addressed task; one stalled reviewer does not strand the rest.
2. An operator changes a completed graph's resource allowance without invalidating
   prior results; a semantic upstream edit creates an explicit revision and only
   invalidates affected descendants whose cache/effect contract permits it.
3. A timed/feed-triggered named workflow consumes a retained event exactly once at
   admission, exposes failures/unknowns, and resumes local orchestration without
   automatically repeating an uncertain publication.

These are acceptance workloads for future orders. They do not claim Blackbird
currently implements them. Read the wider [ecosystem matrix](ecosystem.md) before
choosing primitives: durable functions, scientific assets and reactive streams
have additional requirements Hermes's agent graph surface does not cover.
