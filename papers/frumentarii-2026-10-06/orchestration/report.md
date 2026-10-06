# Frumentarii: orchestration and observability of harness evolution

2026-10-06. Research for discussion, not an adopted design or dependency. The most
useful next step is an audit explorer and evidence-linked complaint surface,
followed by failure-triggered colleague consultation measured against an ordinary
single-agent workflow. Arco already has useful persistence, RRC, managed context
and recoverable tool deadlines; it need not wait for an elaborate optimizer.

This lane read workspace/repository AGENTS, Blackbird, README, recent journal,
previous autodroit reconnaissance and the giga campaign proposal; acquired five
pinned source archives and three PDFs; read actual implementation and test source.
**No acquired code, reference tests, model calls, benchmarks or account-auth flows
were executed.** Source observation establishes the existence and shape of a
mechanism. It does not establish that a release works, that a test passes, or that
Arco will improve. `manifest.json` retains pins, hashes, archives and restoration
instructions. Archives remain intact; extracted trees have no nested Git metadata
or filesystem detritus. Existing sources were reused rather than re-ingested.

## 1. Failure-triggered diagnosis, distinct from mandatory self-certification

[MAST v2](https://arxiv.org/abs/2503.13657v2) gives a useful vocabulary for
specification/design, coordination and verification failures. Read the intervention
and limitations discussion: high-level objective verification can catch defects
that code-level checks miss, but a failure label does not identify its cause.
The study deliberately excludes efficiency from its taxonomy. Consequently it
cannot supply our acceleration score. This is a source of diagnostic hypotheses,
not a universal taxonomy or an oracle for successful software.

The newer [AdaMAST source](https://github.com/multi-agent-systems-failure-taxonomy/AdaMAST/tree/3c83d81c85937f5e8911fd96daa2ca38c1ad2585)
implements a more interesting selective intervention. Inspected:

- `adamast/hosts/claude_code/runtime.py::post_tool`: explicit failure or matching
  output produces an **advisory** nudge; repeated-output digest, call-distance and
  elapsed-time throttles suppress repetition. It retains checkpoint identity and
  transcript offset, then harvests later reflections.
- `adamast/core/evidence.py::record_reflection`: records task/session/conversation,
  agent/turn/episode, taxonomy identity, observations, proposed causes and decisions;
  clean checkpoints are recorded too. Labels remain model assertions.
- `adamast/protocol/gate.py::evaluate_pre_submission`, `pin_gate_decision`: the
  final gate parses verdict text, uses runtime-owned retry counters, and pins a
  recovered verdict across a formatting re-prompt. A format retry must not silently
  become a new substantive assessment. Exhausted repair can produce an honest
  unresolved report.
- `tests/test_custom_hooks.py::test_valid_reflection_unblocks_and_records`,
  `test_retry_guard_releases_after_max_failures`, `test_distinct_tool_use_ids_make_distinct_checkpoints`;
  `tests/test_learning_cost_control.py` tests completion-triggered learning and a
  cap on automatic learning cycles. These are inspected tests, not run results.

**Borrow:** a failure can initiate one focused, optional diagnostic program with
links to the actual operation, context revision, request and retained output.
A model rageshake can enqueue the same evidence even if it continues ordinary work.
Report observation separately from suspected cause and proposed repair. The
consumer DB can track disposition; Arco does not govern that service.

**Do not copy the default blocking semantics.** A parser accepting a READY/pass
statement has checked response shape, not correctness. Four agreeing model
annotators are not four independent executable oracles. The local evidence file
is rewritten as accumulated JSON; this is not Arco's append-only core audit.
Never let reporting become an obligation to fix the reported defect immediately.

**Direct experiment:** inject a tool deadline, stale-context proposal and repeated
identical failure into a real coding task. Compare ordinary continuation with an
advisory diagnosis plus optional Kimi/MiMo/Claude/Grok/ChatGPT consultation. Check
completion against an independently stated behavioral outcome; count repeated
commands, unknown effects, recovery time, useful findings and reporting overhead.
A clean task must not accumulate invented complaints.

## 2. Durable peer mail, and the boundary between delivery and influence

Reused the Python mail source at `3fad5ec67286…` and acquired the current
[Rust mail implementation](https://github.com/Dicklesworthstone/mcp_agent_mail_rust/tree/840b72ff1366b2746a9958c43c0ba00d9311d47c).
Inspected Rust `crates/mcp-agent-mail-tools/src/messaging.rs::send_message`, its
idempotent message-creation branch and `try_dispatch_archive_write`:

- Message and recipients are created in one database transaction. An optional
  client key and fingerprint return the original result for the same request;
  changed payload under that key is a typed conflict. Replay skips one-time
  notification/archive dispatch.
- Database state is authoritative. Archive writes are deferred. If queueing fails,
  a background backlog is attempted; its ephemeral/dropped dispositions are
  explicitly distinguished. A quick acknowledgement does not mean archival
  materialization is durable.
- `tests/idempotency_tool_acceptance.rs::send_message_replay_is_exactly_once_and_archive_at_most_once`
  invokes the actual tool, asserts the original message ID and replay marker,
  counts canonical archive files, and rejects changed payload. The test flushes
  background writes; it is not itself a crash-at-every-boundary proof.
- Reused Python `app.py::acknowledge_message` and
  `tests/test_message_delivery_regression.py::test_read_and_ack_are_idempotent`:
  read/ack timestamps are separate and idempotent. Their acknowledgement does
  **not** establish the model saw, understood or acted on the message.

**Arco consequence:** retain accepted message, selected-for-request, actual request
inclusion and response/action correlation separately. Busy-target mail waits for
an ordinary boundary unless interrupt-and-apply-now is explicitly selected. An
idle target can be triggered. Cross-provider identities must not be a fixed pair
matrix. Acknowledgement and summary should not overwrite the original mail.

**Direct oracle:** deliver a unique constraint to a busy colleague; interrupt/RRC
before and after durable acceptance; inject duplicate transport delivery; assert
one accepted identity and inclusion in the eventual actual provider request.
Have the resulting code obey the constraint. Delivery-only assertions are weaker
than the product claim. Timeout retry is safe only for interfaces with a real
idempotency contract; arbitrary shell commands still have unknown effects.

## 3. Parallelize the uncertainty, not merely the headcount

[Anthropic's compiler-team account](https://www.anthropic.com/engineering/building-c-compiler)
describes separate checkouts, task files and repeated sessions. Its useful lesson
is that the Linux build became a serial collision point; mixing a known-good
compiler with the candidate converted diagnosis into independently attributable
subsets. Pair interactions still needed reduction. This is a documented experience,
not a controlled comparison proving a speedup. The account also distinguishes the
compiler from assembler/linker support and unsuccessful goals.

Acquired [the pinned compiler artifact](https://github.com/anthropics/claudes-c-compiler/tree/6f1b99acb2f4ec2414592136c2009fe7713deec3).
Inspected `current_tasks/fix_x86_standalone_kernel_link_errors.txt`, concrete
unresolved expression/label failures, and `src/backend/x86/asm_stub.sh`, which
exits with an explicit unsupported error. The public archive inspected does not
contain the complete team driver, evaluator or experiment logs. The article's
full workflow cannot be independently reproduced from this archive alone.

**Arco option:** allow separate workspaces for competing hypotheses or independent
conceptual units, then compare a patch against a known behavioral implementation
or explicit model oracle. Carry starting source identity and integration owner.
A task-file lease coordinates intention; it does not prevent conflicting writes.
No Git reset/push behavior from a reference authorizes those actions here.

**Experiment:** two independent repairs to distinct injected fault families plus
an integration case where both changes interact. Compare sequential baseline with
two isolated workers, holding total resource limits explicit. Measure accepted
work, merge/reconciliation effort and regressions rather than commits per hour.
Use differential decomposition only where a valid known-good boundary exists.

## 4. Collaboration topology is a tunable treatment

[Towards a Science of Scaling Agent Systems v1](https://arxiv.org/abs/2512.08296v1)
compares single, independent, central, decentralized and hybrid teams under
controlled budgets. Read setup, metric definitions and limitations: task structure
changes the result, and communication consumes the same budget as solving. The
study's heterogeneous teams vary capabilities within a family; it does not test
our requested arbitrary cross-provider mixtures. I found no linked runnable study
implementation in the inspected HTML. Treat numerical thresholds and explanatory
claims as reported results, not an Arco rule or a verified architecture law.
Some text moves from observed correlations to claims about hidden model internals;
that inference is not needed for our design.

**Arco option:** expose participant profile, source/context subset, tools, model,
communication and budget independently. Let workflows choose a reviewer, pair,
room or isolated proposer. Retain a strong serial baseline and an equal-total-budget
comparison separately from an equal-wall-time comparison. Cross-provider consultation
may buy complementarity, but a different vendor is not proof of independence.

**Experiment:** one implementer alone; same implementer plus same-model critic;
plus independently contextualized different-provider critic. Use matched fresh
fault families and inspect findings before showing implementer's conclusions.
Count actual unique valid defects, false findings, integrated fixes, task quality,
latency and all participant usage. Do not score prose disagreement as independence.

## 5. Exact audit history can become cheaper without becoming a summary

Acquired [Inspect AI](https://github.com/UKGovernmentBEIS/inspect_ai/tree/38779d1e38fe210124c20680d57e0fde20d86590).
Its [log guide](https://inspect.aisi.org.uk/eval-logs.html) distinguishes model API
logging from normalized transcripts, and warns that raw-call capture defaults are
selective. Arco's capture-all promise therefore cannot be inferred from a similar
log viewer or adopted default.

Actual inspected machinery:

- `src/inspect_ai/event/_pool.py::_strict_eq`, `_strict_eq_prefix_len`,
  `materialize_pooled_events`: pooled messages and wire-call values can be
  reconstructed; equality deliberately distinguishes JSON-distinct values such
  as integer, float and Boolean values.
- `src/inspect_ai/log/_condense.py::WalkContext`,
  `attachment_refs_from_object`: cache scope matters when messages mutate, and
  retained identity alone cannot justify reusing old content. Object-walk caching
  avoids re-materializing full histories for each attachment scan.
- `tests/log/test_condense_linear.py::test_buffer_condense_is_linear`,
  `test_transcript_store_reopen_reuses_pool_rows`,
  `test_batch_call_prefix_breaks_on_json_distinct_values`,
  `test_condense_model_call_interleaved_streams`,
  `test_condense_model_call_forked_streams_keep_separate_lineages`:
  these assert specific operation-count bounds and reconstruction distinctions,
  not merely elapsed time or absence of crashes. The notification-shape and
  slot-cap cases test realistic cache pressure.
- `tests/util/test_span_rotation_scope.py::test_rotation_visible_to_previously_spawned_task`
  tests the attribution pitfall where already spawned work sees an obsolete span.
  Arco's C++ implementation need not have Python's ContextVar failure, but it still
  needs explicit generation/attempt identity across concurrent work and RRC.

**Arco consequence:** the current scale evidence makes exact audit storage and
replay a productive optimization target. Keep immutable original payloads and exact
final requests. A pooled/delta storage treatment must independently reconstruct
those bytes/values, handle edits and divergent request lineages, and retain ordering.
Context compaction is a separate treatment. Deduplication must not become semantic
summarization or collapse distinct occurrences into one causal event.

**Direct oracle:** a known request/output sequence with branch divergence, changed
message content, reopen and truncation/failure boundaries; recover exact final
request and original chunks. A separate counting oracle checks work proportional
to new content over growing histories. Record serialization, publication, replay,
request assembly, network and inference times separately. No new storage dependency
is proposed by studying this code.

## 6. Station operation needs explicit pending and committed work

Reused [Station source](https://github.com/dualverse-ai/station/tree/782088e561998bf357d95a838a0566a8229425d9),
especially `station/station_runner.py::_trigger_pause_due_to_llm_error`,
`_check_automatic_wait_conditions`, `_check_wait_conditions_resolved` and the
manual-pause distinction. It persists the relevant agent index; waits can resolve
automatically while operator-required pauses stay distinct. This is a reference
research orchestrator, **not evidence of our proposed feed-listening station mode**.

Read `tests/test_parallel_research_sync.py`:
`test_parallel_recovery_reserves_rolled_back_fast_lane_eval_ids` asserts provisional
work becomes a tombstone, notification is suppressed, a committed neighbor survives
and replacement gets a new identity. `test_parallel_recovery_removes_uncommitted_flushed_history`
distinguishes flushed prompt/response from committed action. The status test keeps
running internal actions visible after apparent agent completion. These are useful
recovery distinctions; deleting uncommitted history is unsuitable for our original
capture audit. Retain it with an explicit disposition instead.

**Arco option:** station and campaign share engine/context/audit, differing in
admission and attachment. Feed item identity, durable acceptance, running attempt,
result and source cursor remain distinct. A trigger must not automatically replay
an unknown effect after RRC. Busy queues/coalescing should be Lua policy and visible
to model/operator, not a universal hardcoded scheduling doctrine. External kernels
and DBs remain independent services.

**Direct oracle:** duplicates, out-of-order triggers, busy arrival and restart at
acceptance/dispatch/completion/cursor-update boundaries. Assert no silent loss,
no unintended duplicate effect and inspectable unknown outcomes. A feed item can
be admitted once while legitimately producing multiple distinct attempts.

## 7. Acceleration is accepted work and operator capacity, not one stopwatch

[METR's February 2026 update](https://metr.org/blog/2026-02-24-uplift-update/)
explains why its later productivity estimate is unreliable: developers/task choices
selected out of no-AI conditions and concurrent agents complicated task-time reports.
Task choice and quality also changed. Do not extrapolate its earlier slowdown or
later raw speedup to Arco. For this operator, gaining capacity for work previously
not attempted is a legitimate outcome, different from doing the same work faster.

Inspected its [pinned data/analysis](https://github.com/METR/Measuring-Late-2025-AI-on-OSS-Devs/tree/55f007216a26493f0a9b7650ab3191cad7fb9e7f):
`regression.py::zero_fill_post_review_time`, `drop_ditched_issues`,
`filter_to_complete_issues`, `run_regression`. The executable analysis sums initial
and post-review time, fills missing review time with zero, excludes ditched work,
and reports a predicted-time-adjusted log-time regression with HC3 errors.
Those are analytical choices, not measurement truths. For Arco, retain unfinished,
abandoned and reverted work in the outcome inventory and make missing time explicit.
Do not transplant the regression without a suitable experimental unit.

Reused GPTMe `gptme/util/cost_tracker.py` and `gptme/eval/cost.py::token_fields_from_cost`:
per-request model and cache fields are useful, but a missing summary maps token
counts to zero while dollar cost is absent. Arco should distinguish unavailable
usage from measured zero; subscription access also makes marginal dollar cost
unavailable/different from resource consumption.

**Small measurement set:** behavioral correctness; accepted tasks per observation
window; elapsed assignment-to-usable-result time; operator active/review/repair
time; all participant provider requests and available usage; build/test resources;
failed/reverted work; steering latency; and audit/replay cost. These attack different
claims. Do not invent a composite productivity score or measure the measurements.
Use matched fresh task variants and reserve unseen final cases. Preserve experiment
assignment and hardware/provider settings. Change one interpretable policy at a time
when attribution matters; productive feature work and harness treatment are separate.

## 8. Resource configuration and adaptive feedback are experimental treatments

[Anthropic's infrastructure-noise account](https://www.anthropic.com/engineering/infrastructure-noise)
reports that hard resource ceilings versus transient headroom changed coding-eval
outcomes. It distinguishes guarantees, limits and CPU/RAM effects. This is a vendor
experiment report, not inspected infrastructure code or a portable prediction.

**Arco consequence:** shorter completion after moving to a larger machine is not
isolated evidence for a better context/review policy. Record executable/compiler,
CPU/RAM limits, host contention, provider identity and tool deadlines with the trial.
Classify infrastructure failure separately, but do not erase its operational cost.
A deadline increase is a real workflow intervention, not simply removal of a bad
score. RRC repair and human rescue are outcomes to retain.

The model should be able to govern trial selection and course correction. Cadence
and budgets are configuration, not per-command approvals. Diagnosis may suggest a
new test; adding that test changes the evaluator identity and requires a subsequent
comparison, rather than retroactively relabeling the earlier trial. A complaint
frequency drop is ambiguous if the reporting policy also changed.

## Suggested immediate order

1. Build the already proposed audit explorer and local rageshake while doing useful
   Arco work. Provide exact originals/request/context/attempt references; keep the
   complaint delivery sink replaceable.
2. Make bounded colleague invocation an ordinary Lua operation with independent
   context and truthful running/completed/failed states. Add durable mail before
   live-room ambitions require subtle steering delivery semantics.
3. Run a small matched trial of ordinary continuation versus failure-triggered
   diagnosis/colleague help on fresh real coding variants, plus deterministic
   timeout/context/message fault cases. Report outcome and costs separately.
4. Address the measured audit/replay bottleneck using exact reconstruction oracles.
   Then add station admission and parallel proposal experiments where independent
   conceptual units actually exist. No optimizer platform is prerequisite.

The doctrine should be recognizable in executed behavior: reading the relevant
source, selecting a direct oracle, finishing useful software, preserving uncertain
outcomes, changing strategy after failure and integrating valid criticism. A
self-reflection checklist alone would reproduce the ceremony Blackbird rejects.

## Acquisition gaps and limits

No runnable released implementation for the scaling study or complete compiler-team
experiment was found in this pass. The compiler artifact is valuable but insufficient
for reproducing its orchestration claims. AdaMAST's inspected source/tests show
mechanisms, not independently measured uplift; independent semantic correctness
and fault-sensitive product evaluation remain ours to establish. Cross-provider
complementarity, station-trigger semantics and model-governed improvement of compiled
Arco under RRC still require our own direct experiments.
