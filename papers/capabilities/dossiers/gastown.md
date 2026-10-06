# gastown

Gas Town coordinates persistent work and ephemeral external sessions through role-specific automation, formulas, hooks, mail, nudges, handoff and escalation.

Role: external-agent workspace, role and workflow orchestrator. Runtime: Go.

Pinned source: [https://github.com/gastownhall/gastown](https://github.com/gastownhall/gastown); revision/version `649b832b7672bc7a2dbef26f5983aba6198b819b`.

CLI and daemon supervise role-specific external harness processes in tmux/worktrees; hooks, detached pollers, async notifications.

Owns town/rigs/worktree/Beads and role infrastructure; external harness owns provider requests, coding tools and compaction.

Inspection: Selected source excerpts trace exposed entry/loop/request/action/result/state boundaries and relevant capability mechanisms; listed tests are read as source only.

Limits of this study: No acquired code executed, dependencies installed or live providers invoked. Claims concern this pinned snapshot; untraced catalog tools and dependency internals are not inferred.

## Actions

### StartSession / StopSession

Surface: Go lifecycle API.

Input: role, session/workdir/agent override/command/env; graceful bool

Result: runtime start result or error

Lifecycle: tmux external process; C-c wait then kill

Authority: configured command/env; runtime-specific approval bypass presets

Evidence: [e2](#evidence-e2), [e3](#evidence-e3), [e4](#evidence-e4), [e5](#evidence-e5).

### gt sling <bead-or-formula> [target]

Surface: operator/agent command.

Input: target, vars, formula, force/raw/dry-run flags

Result: hooked base bead and attached workflow wisp

Lifecycle: durable assignment before external work completion; rollback on partial spawn failures

Authority: operator or model shell; force can discard old molecules

Evidence: [e12](#evidence-e12).

### gt mol step done <step-id>

Surface: operator/agent command.

Input: step ID, dry-run/JSON

Result: closed step, next/parallel/complete/blocked result

Lifecycle: reads durable readiness; continuation restarts agent; fan-out notices need manual/other-agent work

Authority: agent declares completion; not independently verified task correctness

Evidence: [e14](#evidence-e14), [e15](#evidence-e15).

### gt mail send / Router.Send

Surface: operator/agent CLI and Go API.

Input: recipient/message body/priority/thread; list/queue/announce/channel/group

Result: persisted message or validation/write error

Lifecycle: notification async best-effort; WaitPendingNotifications fences goroutine lifetime only

Authority: configured address routing/backend; no proof of model reception

Evidence: [e6](#evidence-e6), [e7](#evidence-e7), [e8](#evidence-e8).

### AcknowledgeDeliveryBead

Surface: Go mail API.

Input: bead ID, recipient identity, routes

Result: acked labels or error

Lifecycle: multiwrite ack-last ordering and retry timestamp reuse

Authority: recipient/caller claims receipt; not semantic completion

Evidence: [e9](#evidence-e9).

### gt nudge <target> --mode wait-idle|queue|immediate

Surface: operator/agent command.

Input: target/message/priority/DND force

Result: terminal submission or queue/error

Lifecycle: prompt detection, cooperative hook/poller; timeout watchers; immediate fallback if queue fails

Authority: caller force overrides DND; receipt is not model result

Evidence: [e10](#evidence-e10), [e11](#evidence-e11).

### gt handoff --auto / --cycle

Surface: operator/agent CLI and compaction hook.

Input: subject/message/stdin/reason

Result: context mail ID and marker/restart

Lifecycle: auto can continue after mail failure; cycle requires persistence then native continue/respawn

Authority: child supplies context/compaction; restart can kill active work

Evidence: [e16](#evidence-e16), [e17](#evidence-e17), [e18](#evidence-e18), [e19](#evidence-e19), [e20](#evidence-e20).

### gt seance [--talk ID] [-p prompt]

Surface: operator/agent command.

Input: previous native session ID/prefix, optional question

Result: one-shot answer stdio or interactive child chat

Lifecycle: external --fork-session --resume; temporary transcript links cleaned

Authority: new external model request authority; no context repair in current session implied

Evidence: [e21](#evidence-e21).

### gt escalate / ack / close / stale

Surface: operator/agent command.

Input: description/severity/reason/source/related bead/fingerprint

Result: tracked escalation bead, per-route statuses

Lifecycle: dedup+severity mail; ack/close and stale handling; notification status can overclaim

Authority: model/operator may report blocker; no full state snapshot captured

Evidence: [e22](#evidence-e22), [e23](#evidence-e23).

### gt done

Surface: agent command.

Input: completed|deferred|escalated, branch/issue/verify options

Result: MR/work exit outcome

Lifecycle: retirement only when durable handoff succeeds; source helper guards push/MR/local strategy

Authority: worker and configured merge workflow; not refit

Evidence: [e33](#evidence-e33), [e34](#evidence-e34).

### gt feed

Surface: operator command.

Input: plain/problems/window/time/rig

Result: curated event/Beads/convoy views

Lifecycle: live activity monitor, polling and dashboard actions

Authority: documented exposed monitor; full rendering branch not traced

Evidence: [e29](#evidence-e29).

### Daemon.Run

Surface: service API.

Input: town config/context

Result: role/process lifecycle and maintenance

Lifecycle: signal/timer loops; operational accessor rereads disk

Authority: owns town infrastructure and lifecycle, not consumer-only fabric

Evidence: [e24](#evidence-e24), [e25](#evidence-e25), [e26](#evidence-e26).

## Capabilities

### filesystem

**S — Files** (source): Town rigs/worktree/hooks and per-agent workdir/settings; actual coding tools delegated external harness.

Evidence: [e1](#evidence-e1), [e2](#evidence-e2).

### processes

**I — OS programs** (source): tmux startup/health/kill, detached nudge poller and predecessor subprocess.

Evidence: [e2](#evidence-e2), [e3](#evidence-e3), [e4](#evidence-e4), [e11](#evidence-e11), [e21](#evidence-e21).

### code-actions

**S — Code actions** (source): Formula TOML and launch-command composition orchestrate external work; no traced model program VM for native tool composition.

Evidence: [e2](#evidence-e2), [e12](#evidence-e12), [e13](#evidence-e13).

### persistent-kernel

**— — Kernel** (inspection-limit): Inspected role controller supplies external agent processes rather than persistent language interpreter.

### standing-database

**S — Standing DB** (source): Beads/Dolt work/mail/identity ledger accessible through CLI; town owns/routs infrastructure, not general shared computation consumer.

Evidence: [e7](#evidence-e7), [e9](#evidence-e9), [e12](#evidence-e12).

### workflow-programming

**L — Workflows** (source): Real formula attachment and readiness/step completion; fan-out handler marks ready work and prints goroutine notices, then restarts first step. Other steps need separate agents/manual execution.

Evidence: [e12](#evidence-e12), [e13](#evidence-e13), [e14](#evidence-e14), [e15](#evidence-e15).

### multi-model

**S — Models** (source): Runtime presets and per-role/override commands can launch distinct agent/model harnesses; no own provider sampling or model switching protocol inferred.

Evidence: [e2](#evidence-e2), [e5](#evidence-e5).

### live-collaboration

**I — Peer chat** (source): Concurrent external roles communicate with persistent mail, channels/groups and nudges; asynchronous delivery acknowledgment distinct from answered conversation.

Evidence: [e6](#evidence-e6), [e7](#evidence-e7), [e8](#evidence-e8), [e9](#evidence-e9), [e10](#evidence-e10).

### concurrent-work

**L — Concurrency** (source): Many external roles/pollers can coexist; specific molecule parallel path does not perform parallel tasks despite comments.

Evidence: [e1](#evidence-e1), [e11](#evidence-e11), [e15](#evidence-e15).

### steering-interrupt

**L — Steer/interrupt** (source): DND-aware nudge and queued hooks/poller; wait-idle can fall back to immediate input when queue fails. C-c followed by kill is process stop, not provider quiescence acknowledgment.

Evidence: [e4](#evidence-e4), [e10](#evidence-e10).

### turn-redefinition

**S — Turn program** (source): Config/presets/hooks/continuation prompts/formula work structure external harness turns; no inspected programmable inner model step.

Evidence: [e2](#evidence-e2), [e5](#evidence-e5), [e12](#evidence-e12), [e16](#evidence-e16).

### compaction

**L — Compaction** (source): PreCompact auto/cycle packages state and native --continue succession; underlying agent does actual compaction. Auto mode can write marker despite failed mail.

Evidence: [e16](#evidence-e16), [e17](#evidence-e17), [e18](#evidence-e18).

### context-repair

**S — Repair** (source): Bounded handoff state and predecessor fork/resume query can recover information; no traced original-to-current context restoration API.

Evidence: [e20](#evidence-e20), [e21](#evidence-e21).

### original-audit

**L — Original audit** (source): Operational timestamp JSONL plus optional Claude native transcript watcher. Missing workspace drops events; callers ignore errors; no fsync or universal original model/tool I/O capture. Handoff clears terminal scrollback.

Evidence: [e18](#evidence-e18), [e27](#evidence-e27), [e28](#evidence-e28).

### audit-query

**D — Audit query** (source): Feed time/rig/visibility and Beads/convoy monitor plus predecessor discovery/query; inspected entry contract not full query/renderer implementation.

Evidence: [e21](#evidence-e21), [e29](#evidence-e29).

### hot-change

**L — Hot change** (source): Operational accessors reread config, heartbeat reset adopts next interval; launch presets applied when starting/restarting. No default affected-turn/workflow activation fence traced.

Evidence: [e2](#evidence-e2), [e24](#evidence-e24), [e25](#evidence-e25), [e26](#evidence-e26).

### rebuild-continuity

**L — Rebuild continuity** (source): Persist-before-cycle context mail and external --continue respawn, with kill semantics; no compilation/outpost transfer or verified active program/provider request emptiness.

Evidence: [e18](#evidence-e18), [e19](#evidence-e19), [e21](#evidence-e21).

### remote-services

**S — Remote** (source): Town can stream optional conversation logs to external endpoint; owns town database/lifecycle rather than merely consumes externally governed fabric. Federation documented but internals not traced.

Evidence: [e1](#evidence-e1), [e3](#evidence-e3), [e28](#evidence-e28).

### self-improvement

**? — Self-improve** (inspection-limit): Inspected formula/config/role mechanisms permit composition; no governed model autoresearch/executable refit loop established.

### complaints

**L — Complaints** (source): Native severity escalation captures reason/source/related bead/fingerprint and tracks ack/close; does not capture full agent state into external complaint DB. RuntimeNotified can mean async attempted, not observed.

Evidence: [e22](#evidence-e22), [e23](#evidence-e23).

### authority

**I — Authority** (source): Model/operator CLI access and powerful runtime defaults; built-in Claude/Gemini bypass routine approvals. Force nudge and molecule replacement remain caller powers.

Evidence: [e5](#evidence-e5), [e10](#evidence-e10), [e12](#evidence-e12).

### evaluation

**L — Evaluation** (source): Read tests check restart-command strings, label partial-write semantics and formula readiness. No executed model, tmux successor or actual parallel task oracle.

Evidence: [e30](#evidence-e30), [e31](#evidence-e31), [e32](#evidence-e32).

### time-order

**L — Time/order** (source): Ack-last metadata ordering/retry identity, timer heartbeat and separate async notification; event RFC3339 timestamp not global monotonic sequence or causality.

Evidence: [e7](#evidence-e7), [e9](#evidence-e9), [e26](#evidence-e26), [e27](#evidence-e27).

## Inspected test oracles

- [quarantine/gastown/internal/cmd/handoff_test.go](../../../quarantine/gastown/internal/cmd/handoff_test.go): Restart continuity command construction Oracle: Temp filesystem/settings/env assert --continue and custom prompt in string; does not observe successor context. Read, **not executed**.
- [quarantine/gastown/internal/mail/delivery_test.go](../../../quarantine/gastown/internal/mail/delivery_test.go): Partial acknowledgment and retry labels Oracle: Pure label parser keeps partial ack pending and exposes recipient/time after final ack; no real crash during bd writes. Read, **not executed**.
- [quarantine/gastown/internal/formula/parallel_test.go](../../../quarantine/gastown/internal/formula/parallel_test.go): Formula graph readiness Oracle: Reads template, asserts dependency and selected next sequential step; not real parallel execution. Read, **not executed**.

## Useful mechanisms

- Persist-before-cycle refusal preserves continuity source when mail fails.
- Acknowledgment labels separate partial state from terminal receipt.
- Native predecessor fork/resume offers a concrete context recovery experiment.

## Material limits

- Molecule fan-out handler supplies readiness and notices, not execution of parallel work.
- Escalation RuntimeNotified status can overclaim asynchronous best-effort mail notification.
- Town succession kills/respawns external harness rather than performing compile refit; audit is selective and runtime-dependent.

## Arconaut design questions

- How can Arconaut package predecessor access as inspectable context repair without opaque fork side effects?
- Which mailbox/model-consumption statuses should be distinct and causally linked?
- Can executable continuity be proved independently of terminal readiness and workflow status labels?

## Evidence

### Evidence e1

[quarantine/gastown/README.md:1–119](../../../quarantine/gastown/README.md#L1): Role-based town/worktree orchestration around external coding harnesses; advertised workflows, monitoring and federation.

### Evidence e2

[quarantine/gastown/internal/session/lifecycle.go:144–228](../../../quarantine/gastown/internal/session/lifecycle.go#L144): Start resolves agent config, provisions settings and constructs command/env before tmux startup.

### Evidence e3

[quarantine/gastown/internal/session/lifecycle.go:267–315](../../../quarantine/gastown/internal/session/lifecycle.go#L267): Startup verifies runtime health and optionally tails Claude conversation to remote logging; external provider loop remains separate.

### Evidence e4

[quarantine/gastown/internal/session/lifecycle.go:355–378](../../../quarantine/gastown/internal/session/lifecycle.go#L355): Graceful stop sends C-c then ultimately kills session process tree.

### Evidence e5

[quarantine/gastown/internal/config/agents.go:228–265](../../../quarantine/gastown/internal/config/agents.go#L228): Built-in Claude/Gemini launch flags bypass routine permissions; resume/hooks/fork capabilities are runtime-specific.

### Evidence e6

[quarantine/gastown/internal/mail/router.go:863–890](../../../quarantine/gastown/internal/mail/router.go#L863): Mail routing selects list/queue/announce/channel/group or single recipient.

### Evidence e7

[quarantine/gastown/internal/mail/router.go:1112–1208](../../../quarantine/gastown/internal/mail/router.go#L1112): Single message validates and persists via bd; notification scheduled asynchronously and error ignored; ephemeral messages may not sync Git.

### Evidence e8

[quarantine/gastown/internal/mail/router.go:1655–1698](../../../quarantine/gastown/internal/mail/router.go#L1655): Idle notification submits terminal nudge or queues cooperative boundary delivery; notified count includes queue receipt.

### Evidence e9

[quarantine/gastown/internal/mail/delivery.go:31–115](../../../quarantine/gastown/internal/mail/delivery.go#L31): Delivery acknowledgment writes recipient/time before final ack label and removes pending after ack; retries reuse sole matching timestamp.

### Evidence e10

[quarantine/gastown/internal/cmd/nudge.go:190–304](../../../quarantine/gastown/internal/cmd/nudge.go#L190): Default wait-idle checks prompt support; queues and starts non-Claude poller; queue failure can degrade to immediate terminal send; lost session returns error.

### Evidence e11

[quarantine/gastown/internal/nudge/poller.go:31–98](../../../quarantine/gastown/internal/nudge/poller.go#L31): Detached nudge-poller has configured poll/idle intervals, PID tracking and process Release; PID write failure leaves untracked process.

### Evidence e12

[quarantine/gastown/internal/cmd/sling.go:878–991](../../../quarantine/gastown/internal/cmd/sling.go#L878): Sling defaults bare polecat work to formula, guards existing attachment, allows force burning, instantiates then stores base-bead/molecule relationship.

### Evidence e13

[quarantine/gastown/internal/formula/parser.go:13–115](../../../quarantine/gastown/internal/formula/parser.go#L13): TOML parsing validates workflow/convoy/expansion/aspect types and references; declarative program rather than model-turn interpreter.

### Evidence e14

[quarantine/gastown/internal/cmd/molecule_step.go:95–177](../../../quarantine/gastown/internal/cmd/molecule_step.go#L95): Step done closes durable step then finds readiness and returns/dispatches continue/parallel/done/blocked.

### Evidence e15

[quarantine/gastown/internal/cmd/molecule_step.go:298–422](../../../quarantine/gastown/internal/cmd/molecule_step.go#L298): Continuation respawns pane; fan-out only marks steps in progress, prints notices in goroutines and continues first; no actual parallel work execution.

### Evidence e16

[quarantine/gastown/internal/cmd/handoff.go:98–128](../../../quarantine/gastown/internal/cmd/handoff.go#L98): Handoff --auto saves state, --cycle saves and replaces session; hook wiring delegates underlying model compaction.

### Evidence e17

[quarantine/gastown/internal/cmd/handoff.go:378–432](../../../quarantine/gastown/internal/cmd/handoff.go#L378): Auto mode collects bounded state, mail failure only warns and marker is written anyway.

### Evidence e18

[quarantine/gastown/internal/cmd/handoff.go:515–591](../../../quarantine/gastown/internal/cmd/handoff.go#L515): Cycle requires mail persistence before event/restart, constructs --continue prompt, clears scrollback and respawns pane; no executable rebuild fence.

### Evidence e19

[quarantine/gastown/internal/cmd/handoff.go:342–372](../../../quarantine/gastown/internal/cmd/handoff.go#L342): Self succession relies on respawn-pane kill semantics so handoff command survives to issue restart; no active provider/tool emptiness test.

### Evidence e20

[quarantine/gastown/internal/cmd/handoff.go:1467–1529](../../../quarantine/gastown/internal/cmd/handoff.go#L1467): Collected continuity data combines Git, hook, bounded inbox and work views with failed subprocesses omitted.

### Evidence e21

[quarantine/gastown/internal/cmd/seance.go:208–277](../../../quarantine/gastown/internal/cmd/seance.go#L208): Seance resolves predecessor, temporarily links native session, launches external --fork-session --resume with one-shot or interactive stdio; clears nested-session guard.

### Evidence e22

[quarantine/gastown/internal/cmd/escalate.go:23–61](../../../quarantine/gastown/internal/cmd/escalate.go#L23): Exposed severity-routed escalation command with ack/close/stale and fingerprint.

### Evidence e23

[quarantine/gastown/internal/cmd/escalate_impl.go:90–188](../../../quarantine/gastown/internal/cmd/escalate_impl.go#L90): Creates deduplicated tracked escalation fields and routed mail; RuntimeNotified=true is set after asynchronous Router.Send and overstates observed notification.

### Evidence e24

[quarantine/gastown/internal/daemon/handler.go:389–398](../../../quarantine/gastown/internal/daemon/handler.go#L389): Rigs and operational config reread from disk by accessor.

### Evidence e25

[quarantine/gastown/internal/daemon/daemon.go:729–749](../../../quarantine/gastown/internal/daemon/daemon.go#L729): Daemon signal dispatch processes lifecycle or tracker reload; other signal shuts down.

### Evidence e26

[quarantine/gastown/internal/daemon/daemon.go:829–843](../../../quarantine/gastown/internal/daemon/daemon.go#L829): Heartbeat interval is reread operational config after tick; no affected-agent turn fence.

### Evidence e27

[quarantine/gastown/internal/events/events.go:28–148](../../../quarantine/gastown/internal/events/events.go#L28): Timestamped orchestration JSONL has audit/feed visibility and cross-process flock; absent workspace silently drops; writer returns errors without fsync; callers often ignore.

### Evidence e28

[quarantine/gastown/internal/agentlog/claudecode.go:15–90](../../../quarantine/gastown/internal/agentlog/claudecode.go#L15): Opt-in Claude transcript watcher chooses newest eligible native file by time and tails across session replacement; not universal provider audit.

### Evidence e29

[quarantine/gastown/internal/cmd/feed.go:46–108](../../../quarantine/gastown/internal/cmd/feed.go#L46): Exposed feed combines event file, Beads and convoy views; time/rig filters, problems/attach/nudge/handoff documented contract.

### Evidence e30

[quarantine/gastown/internal/cmd/handoff_test.go:349–405](../../../quarantine/gastown/internal/cmd/handoff_test.go#L349): Temporary settings/env test asserts continue flag/custom prompt in constructed restart command; does not spawn real successor.

### Evidence e31

[quarantine/gastown/internal/mail/delivery_test.go:12–54](../../../quarantine/gastown/internal/mail/delivery_test.go#L12): Pure delivery-label test keeps partial acknowledgments pending until terminal ack label.

### Evidence e32

[quarantine/gastown/internal/formula/parallel_test.go:7–55](../../../quarantine/gastown/internal/formula/parallel_test.go#L7): Parser/readiness test verifies expected formula dependency and sequential choice; does not execute parallel work.

### Evidence e33

[quarantine/gastown/internal/cmd/done.go:30–59](../../../quarantine/gastown/internal/cmd/done.go#L30): Done completion/defer/escalate command submits branch to merge queue and retires after durable handoff; underlying coding work external.

### Evidence e34

[quarantine/gastown/internal/cmd/done.go:92–104](../../../quarantine/gastown/internal/cmd/done.go#L92): Retirement withheld on push/MR failure or local merge strategy.

