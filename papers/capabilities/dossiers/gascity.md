# gascity

Gas City provides programmable durable workflow and process coordination around heterogeneous agents, with mail, nudges, session history adapters and reconciliation. Its reload/handoff controls are not Arconaut executable refit.

Role: external-agent process and workflow orchestration service. Runtime: Go.

Pinned source: [https://github.com/gastownhall/gascity](https://github.com/gastownhall/gascity); revision/version `0addde8a1e17a7b4c51c11b563c817a527dc4b3b`.

Go city controller/tick and separate accept/dispatch goroutines; runtime transports launch external agent processes (tmux traced).

Owns city configuration, session/work/graph/mail stores, runtime lifecycle and orchestration event stream; external agent harness owns provider requests and native coding tools.

Inspection: Selected source excerpts trace exposed entry/loop/request/action/result/state boundaries and relevant capability mechanisms; listed tests are read as source only.

Limits of this study: No acquired code executed, dependencies installed or live providers invoked. Claims concern this pinned snapshot; untraced catalog tools and dependency internals are not inferred.

## Actions

### gc start / CityRuntime.run

Surface: operator command / controller.

Input: city.toml, runtime provider, Beads stores

Result: managed named sessions and controller event loop

Lifecycle: persistent reconciler; cancel/shutdown terminates loop

Authority: operator config owns runtime commands; child agent owns actual model/tool loop

Evidence: [e3](#evidence-e3), [e4](#evidence-e4), [e38](#evidence-e38).

### SessionHandle.Start / StartResolved / Attach / Create

Surface: Go worker API.

Input: session identity, command/runtime hints, deferred|started

Result: session record or terminal attachment/error

Lifecycle: create/start/resume; blocking attach

Authority: caller selects external agent runtime

Evidence: [e39](#evidence-e39).

### Provider.Stop / Interrupt

Surface: runtime API.

Input: session name

Result: error or best-effort terminal signal

Lifecycle: Stop process-tree destruction; Interrupt terminal C-c

Authority: controller/operator access; C-c is not proven model quiescence

Evidence: [e6](#evidence-e6).

### gc nudge / RuntimeHandle.Nudge

Surface: operator/agent CLI and worker API.

Input: target, text/file content, queue|wait-idle|now

Result: submitted/delivered-unobserved/queued plus reason

Lifecycle: bounded queue claims/retries and target incarnation; delivery receipt not response

Authority: caller via CLI; external process receives terminal/ACP input

Evidence: [e7](#evidence-e7), [e8](#evidence-e8), [e9](#evidence-e9), [e10](#evidence-e10).

### gc mail send

Surface: operator/agent CLI.

Input: recipient, subject/body, optional dedup/notify

Result: message ID, duplicate/notify state

Lifecycle: persist before notify; notification failure leaves mail

Authority: agent shell/operator access; routing resolves actual session

Evidence: [e14](#evidence-e14).

### gc mail inbox / read / peek / archive / reply

Surface: operator/agent CLI and mail API.

Input: recipient or message ID, reply sender/body

Result: messages/read metadata/archive state/new threaded message

Lifecycle: mail persists independently of live recipient; archive retains body

Authority: configured mail provider/store; distinct routable session identity

Evidence: [e11](#evidence-e11), [e12](#evidence-e12), [e13](#evidence-e13).

### gc mail check --inject

Surface: agent hook command.

Input: agent address; hook/injection flags

Result: bounded reminder preview and full message IDs

Lifecycle: next hook boundary; some handoffs archived on injection

Authority: external agent hook integration; hook failure does not fail user prompt

Evidence: [e15](#evidence-e15).

### gc sling

Surface: operator/agent CLI.

Input: target and bead/formula/text, variables/force/nudge/dry-run/scope

Result: BeadID/WorkflowID and assignment/routing result

Lifecycle: durable launch returns before workflow completion; controller poked

Authority: local configured store or authenticated remote city write request

Evidence: [e16](#evidence-e16), [e19](#evidence-e19).

### instantiateViaGraphApply

Surface: workflow primitive.

Input: compiled recipe with vars/deps/routing/idempotency

Result: root ID, mapping, created count

Lifecycle: atomicity delegated graph-store plan contract; validates result

Authority: configured graph backend; not arbitrary LLM code execution

Evidence: [e17](#evidence-e17), [e18](#evidence-e18).

### PUT/DELETE /v0/city/{cityName}/formulas/{name}

Surface: HTTP API.

Input: validated formula draft/name

Result: mutator result/error

Lifecycle: configuration mutation affects future resolution; exact authorization middleware not traced

Authority: configured FormulaMutator; do not infer public unauthenticated writes

Evidence: [e20](#evidence-e20).

### gc reload [--async] [--soft]

Surface: operator/agent CLI.

Input: city/path, wait duration, mode

Result: accepted/applied/no-change/failed/busy/timeout and revision

Lifecycle: accepted separately; applies on controller tick, not agent turn boundary

Authority: controller runtime config; storage changes require restart

Evidence: [e21](#evidence-e21), [e22](#evidence-e22), [e23](#evidence-e23), [e24](#evidence-e24).

### gc runtime drain-ack [name]

Surface: operator/agent CLI.

Input: current/explicit session

Result: acknowledged status

Lifecycle: releases claims, stamps incarnation metadata, controller poke; cooperative worker completion

Authority: worker/operator promises work finished; no verified provider-request fence

Evidence: [e25](#evidence-e25), [e26](#evidence-e26), [e27](#evidence-e27).

### gc handoff [--auto] [--force]

Surface: operator/agent CLI / PreCompact hook.

Input: context body and restartable target

Result: context mail ID and restart requested/killed/mail-only result

Lifecycle: self handoff can defer restart; remote kills then reconciler restart; auto merely packages context

Authority: force bypasses live-subagent guard; underlying agent supplies context/compaction

Evidence: [e28](#evidence-e28), [e29](#evidence-e29).

### SessionHandle.Transcript / TranscriptRecords / History / AgentTranscript

Surface: Go worker API.

Input: session ID, raw/normalized request, cursor/tail compactions/subagent ID

Result: provider-native records or normalized snapshot/ErrHistoryUnavailable

Lifecycle: read-only source file projection; depends on provider transcript discovery

Authority: history adapter and retained external provider files

Evidence: [e30](#evidence-e30).

### gc events [--watch|--follow|--seq] / events rotate

Surface: operator/agent CLI.

Input: event filters, duration, timeout, sequence/city cursor

Result: JSONL event DTOs / SSE envelopes

Lifecycle: replay then watch/follow; city-scoped seq, archive rotation

Authority: configured API/local fallback; recorded orchestration subset

Evidence: [e31](#evidence-e31), [e32](#evidence-e32), [e33](#evidence-e33), [e34](#evidence-e34), [e35](#evidence-e35), [e36](#evidence-e36).

## Capabilities

### filesystem

**S — Files** (source): Stages overlays and files into runtime work directories; coding-agent file actions belong to spawned harness.

Evidence: [e5](#evidence-e5), [e7](#evidence-e7).

### processes

**I — OS programs** (source): Starts external sessions, stops process trees and exposes terminal attach/C-c; runtime availability is separate from model work completion.

Evidence: [e5](#evidence-e5), [e6](#evidence-e6), [e38](#evidence-e38), [e39](#evidence-e39).

### code-actions

**S — Code actions** (source): Configured runtime commands and declarative formula graphs compose work; inspected path does not expose a model-authored program VM composing native tools.

Evidence: [e16](#evidence-e16), [e17](#evidence-e17), [e18](#evidence-e18).

### persistent-kernel

**— — Kernel** (inspection-limit): City controls external agent sessions, not an interactive language kernel; inspected worker surface delegates child computation.

### standing-database

**S — Standing DB** (source): Durable work/graph/message/session Beads stores are orchestration state accessible via operations; not a general shared model-query DB kernel.

Evidence: [e11](#evidence-e11), [e13](#evidence-e13), [e16](#evidence-e16), [e19](#evidence-e19).

### workflow-programming

**I — Workflows** (source): Compiled formula graphs preserve dependencies, variables, routing and root IDs; sling persists links/wakes dispatch; mutator API edits formula catalog.

Evidence: [e16](#evidence-e16), [e17](#evidence-e17), [e18](#evidence-e18), [e19](#evidence-e19), [e20](#evidence-e20).

### multi-model

**S — Models** (source): Provider-neutral external runtime interface permits heterogeneous agent processes; actual LLM selection/inference owned by external harness.

Evidence: [e1](#evidence-e1), [e38](#evidence-e38), [e39](#evidence-e39).

### live-collaboration

**I — Peer chat** (source): Concurrent agents exchange threaded durable mail plus optional nudge/hook injection; submission/queue/wake receipts distinguishable, not synchronous shared inference.

Evidence: [e8](#evidence-e8), [e11](#evidence-e11), [e13](#evidence-e13), [e14](#evidence-e14), [e15](#evidence-e15).

### concurrent-work

**I — Concurrency** (source): City manages many external sessions and durable graph routes; separate reload acceptance and dispatch lanes coexist with tick loop.

Evidence: [e3](#evidence-e3), [e4](#evidence-e4), [e16](#evidence-e16), [e19](#evidence-e19).

### steering-interrupt

**L — Steer/interrupt** (source): Terminal C-c and nudge are best-effort. Default provider nudge falls through idle timeout to immediate send; managed wait-idle can queue and busy delivery acknowledgment need not prove model observation.

Evidence: [e6](#evidence-e6), [e7](#evidence-e7), [e8](#evidence-e8), [e9](#evidence-e9).

### turn-redefinition

**S — Turn program** (source): Agent runtime commands, overlays, hooks and workflow formulas are configurable; model-provider turn logic remains in child harness, not programmable CityRuntime turns.

Evidence: [e5](#evidence-e5), [e15](#evidence-e15), [e18](#evidence-e18), [e38](#evidence-e38).

### compaction

**L — Compaction** (source): Auto handoff packages mail at external PreCompact hook and history can select provider compaction tails; no inspected own context compactor or selectable summarization engine.

Evidence: [e28](#evidence-e28), [e30](#evidence-e30).

### context-repair

**S — Repair** (source): Raw transcript records include entries hidden by active-branch projection; handoff mail supplies continuity material. No traced model-governed restoration of a damaged child context.

Evidence: [e28](#evidence-e28), [e30](#evidence-e30).

### original-audit

**L — Original audit** (source): Sequenced orchestration JSONL plus external provider-native transcripts are useful but not all I/O capture: best-effort Record can drop, ack is not fsynced, rotation/retention and malformed-line skipping matter.

Evidence: [e30](#evidence-e30), [e31](#evidence-e31), [e32](#evidence-e32), [e34](#evidence-e34), [e37](#evidence-e37).

### audit-query

**I — Audit query** (source): Event API/CLI list/watch/follow filter by type/time/payload with per-city seq cursors; transcript normalized/raw/cursor views separate.

Evidence: [e30](#evidence-e30), [e33](#evidence-e33), [e35](#evidence-e35), [e36](#evidence-e36).

### hot-change

**L — Hot change** (source): Reload applies on city reconciliation boundary; child work can continue. Storage mutation refused until restart; soft reload accepts hashes without changing running executable/config.

Evidence: [e4](#evidence-e4), [e21](#evidence-e21), [e22](#evidence-e22), [e23](#evidence-e23), [e24](#evidence-e24).

### rebuild-continuity

**L — Rebuild continuity** (source): Handoff mails context and restarts external process, with optional subagent guard; drain-ack is cooperative metadata/claim release, not compiled harness swap with verified no active tools/provider calls.

Evidence: [e25](#evidence-e25), [e26](#evidence-e26), [e27](#evidence-e27), [e28](#evidence-e28), [e29](#evidence-e29).

### remote-services

**I — Remote** (source): Runtime-neutral sessions plus authenticated remote city sling/event consumers; City owns orchestration stores/reconciliation rather than being solely external-fabric consumer.

Evidence: [e16](#evidence-e16), [e36](#evidence-e36), [e38](#evidence-e38).

### self-improvement

**? — Self-improve** (inspection-limit): Formula/runtime/config mutation is established; inspected paths do not establish governed model-controlled autoresearch or executable refit.

### complaints

**? — Complaints** (inspection-limit): Mail and work beads can carry bug reports, but inspected paths do not establish model rageshake capturing state and tracking external complaint DB.

### authority

**L — Authority** (source): Agents may invoke CLI via underlying shell authority. Remote sling uses request-bound write grant; local runtime commands and force handoff are powerful; no inherited child approval semantics inferred.

Evidence: [e16](#evidence-e16), [e29](#evidence-e29), [e38](#evidence-e38).

### evaluation

**I — Evaluation** (source): Inspected fake lifecycle uncertainty and honest-queue output assertions; real temporary-file recorder factory and roundtrip/sequence/watch contracts. These are unexecuted source tests, not model or live terminal validation.

Evidence: [e40](#evidence-e40), [e41](#evidence-e41), [e42](#evidence-e42), [e43](#evidence-e43).

### time-order

**I — Time/order** (source): Durable per-log sequence and stable cursor filters, bounded queue TTL/claims/retries, separate acceptance/application and graph projection reconciliation. Cross-lane convergence order explicitly nondeterministic.

Evidence: [e4](#evidence-e4), [e9](#evidence-e9), [e31](#evidence-e31), [e33](#evidence-e33), [e37](#evidence-e37).

## Inspected test oracles

- [quarantine/gascity/cmd/gc/session_observation_uncertainty_test.go](../../../quarantine/gascity/cmd/gc/session_observation_uncertainty_test.go): Runtime unavailable vs absent lifecycle destructive-action guard Oracle: Fake runtime/clock/store: unchanged status+metadata, no drain/wake across unavailable observations; not real provider probe. Read, **not executed**.
- [quarantine/gascity/cmd/gc/nudge_honesty_test.go](../../../quarantine/gascity/cmd/gc/nudge_honesty_test.go): Queue receipt honesty Oracle: Pure formatting distinguishes unsupported vs no-idle; missing session store produces wake warning while enqueue succeeds; does not assert eventual model observation. Read, **not executed**.
- [quarantine/gascity/internal/events/conformance_test.go](../../../quarantine/gascity/internal/events/conformance_test.go): Event implementation factory Oracle: Real temp-file recorder and separate fake wired to suite; inspected source only. Read, **not executed**.
- [quarantine/gascity/internal/events/eventstest/conformance.go](../../../quarantine/gascity/internal/events/eventstest/conformance.go): Event contract Oracle: Roundtrip fields, seq monotonicity/LatestSeq and watch existing/new data; no executed crash durability proof, no full provider-wire coverage. Read, **not executed**.

## Useful mechanisms

- Durable graph and source work identities separate launch receipt from completion.
- Provider uncertainty is explicitly distinct from absence; tests challenge destructive lifecycle behavior.
- Mail persistence, notification and delivery observation are separate outcomes.

## Material limits

- Model request/native tool semantics remain external; provider adapters must not borrow guarantees from harnesses.
- Best-effort event recording and non-fsynced acknowledgment do not establish complete original audit.
- Hot config and cooperative drain are not universal affected-workflow barriers; soft reload blesses drift.

## Arconaut design questions

- Which orchestration primitives belong in external fabric rather than Arconaut consumer?
- How should Arconaut prove quiescence instead of trusting drain-ack/C-c?
- Should durable mailbox receipt, runtime submission and model consumption have separate audit events?

## Evidence

### Evidence e1

[quarantine/gascity/README.md:1–195](../../../quarantine/gascity/README.md#L1): Go city orchestration around external agent runtimes, Beads, formulas and configurable providers.

### Evidence e2

[quarantine/gascity/cmd/gc/main.go:32–50](../../../quarantine/gascity/cmd/gc/main.go#L32): CLI process entry funnels dispatch through run.

### Evidence e3

[quarantine/gascity/cmd/gc/controller.go:1170–1228](../../../quarantine/gascity/cmd/gc/controller.go#L1170): Compatibility controller creates CityRuntime and invokes its run loop.

### Evidence e4

[quarantine/gascity/cmd/gc/city_runtime.go:895–1010](../../../quarantine/gascity/cmd/gc/city_runtime.go#L895): Reload acceptance is separate; actual work and event-triggered reconciliation run on city loop. Panic recovery logs and continues.

### Evidence e5

[quarantine/gascity/internal/runtime/tmux/adapter.go:76–140](../../../quarantine/gascity/internal/runtime/tmux/adapter.go#L76): Start stages configured overlays/files and starts a named external runtime session.

### Evidence e6

[quarantine/gascity/internal/runtime/tmux/adapter.go:257–298](../../../quarantine/gascity/internal/runtime/tmux/adapter.go#L257): Stop destroys process tree excluding the calling process; Interrupt submits terminal Ctrl-C, not provider cancellation acknowledgment.

### Evidence e7

[quarantine/gascity/internal/runtime/tmux/adapter.go:548–614](../../../quarantine/gascity/internal/runtime/tmux/adapter.go#L548): Default nudge waits for idle best-effort then sends immediately even after timeout; files are staged; missing sessions can return nil.

### Evidence e8

[quarantine/gascity/cmd/gc/cmd_nudge.go:1153–1232](../../../quarantine/gascity/cmd/gc/cmd_nudge.go#L1153): Managed delivery distinguishes durable queued wake, busy Claude wait-idle downgrade, ACP connection ownership, and submission delivered-unobserved.

### Evidence e9

[quarantine/gascity/cmd/gc/cmd_nudge.go:42–100](../../../quarantine/gascity/cmd/gc/cmd_nudge.go#L42): Queue delivery has TTL, claim/retry/attempt bounds and concrete session/continuation identity.

### Evidence e10

[quarantine/gascity/internal/worker/runtime_handle.go:220–330](../../../quarantine/gascity/internal/worker/runtime_handle.go#L220): Worker message/interrupt/nudge operations use runtime provider; runtime-only transcript/history unavailable.

### Evidence e11

[quarantine/gascity/internal/mail/beadmail/beadmail.go:321–367](../../../quarantine/gascity/internal/mail/beadmail/beadmail.go#L321): Inbox/read operate message beads; reading mutates read metadata while retaining body.

### Evidence e12

[quarantine/gascity/internal/mail/beadmail/beadmail.go:412–439](../../../quarantine/gascity/internal/mail/beadmail/beadmail.go#L412): Archive closes rather than deletes message; live backing store read avoids stale cached tombstone.

### Evidence e13

[quarantine/gascity/internal/mail/beadmail/beadmail.go:668–722](../../../quarantine/gascity/internal/mail/beadmail/beadmail.go#L668): Reply returns to original session route and creates ephemeral message bead with thread/reply identifiers.

### Evidence e14

[quarantine/gascity/cmd/gc/cmd_mail.go:1882–1958](../../../quarantine/gascity/cmd/gc/cmd_mail.go#L1882): Mail send stores first, records event, optionally nudges; duplicate suppresses notify and nudge failure does not undo successful send.

### Evidence e15

[quarantine/gascity/cmd/gc/cmd_mail.go:734–770](../../../quarantine/gascity/cmd/gc/cmd_mail.go#L734): Hook mail check injects bounded priority-sorted reminders; failures still return zero; auto-handoff mail archived on injection.

### Evidence e16

[quarantine/gascity/cmd/gc/cmd_sling.go:375–544](../../../quarantine/gascity/cmd/gc/cmd_sling.go#L375): Sling resolves authenticated remote write target or local config/store/agent, installs runner and graph/work routing dependencies, dispatches batch.

### Evidence e17

[quarantine/gascity/internal/molecule/graph_apply.go:53–80](../../../quarantine/gascity/internal/molecule/graph_apply.go#L53): Instantiation applies compiled graph plan, validates IDs and returns root plus node mapping/count.

### Evidence e18

[quarantine/gascity/internal/molecule/graph_apply.go:152–212](../../../quarantine/gascity/internal/molecule/graph_apply.go#L152): Plan preserves native dependency topology and executable root types; applies vars, routing, root-only and idempotency metadata.

### Evidence e19

[quarantine/gascity/internal/sling/sling_core.go:934–972](../../../quarantine/gascity/internal/sling/sling_core.go#L934): Graph launch promotes workflow root, persists source/store links and wakes controller/control dispatch; returns WorkflowID before execution completes.

### Evidence e20

[quarantine/gascity/internal/api/huma_handlers_formula_write.go:85–145](../../../quarantine/gascity/internal/api/huma_handlers_formula_write.go#L85): HTTP formula upsert/delete validate and invoke configured mutator; no per-turn programming inferred.

### Evidence e21

[quarantine/gascity/cmd/gc/cmd_reload.go:20–143](../../../quarantine/gascity/cmd/gc/cmd_reload.go#L20): Reload exposes accepted/busy/failure/timeout and async/soft options; wait bounds are CLI/controller boundaries.

### Evidence e22

[quarantine/gascity/cmd/gc/city_runtime.go:2081–2137](../../../quarantine/gascity/cmd/gc/city_runtime.go#L2081): Reload failure retains old config; transactional candidate and superseded revision retry checks.

### Evidence e23

[quarantine/gascity/cmd/gc/city_runtime.go:2211–2235](../../../quarantine/gascity/cmd/gc/city_runtime.go#L2211): Storage handles are process-lifetime and changes refuse live reload, requiring city restart.

### Evidence e24

[quarantine/gascity/cmd/gc/soft_reload.go:120–174](../../../quarantine/gascity/cmd/gc/soft_reload.go#L120): Soft reload stamps desired config/provision/launch hashes and cancels matched drains without rebuilding running process.

### Evidence e25

[quarantine/gascity/cmd/gc/cmd_runtime_drain.go:105–142](../../../quarantine/gascity/cmd/gc/cmd_runtime_drain.go#L105): Drain acknowledgment metadata binds acknowledging pane incarnation; cross-session ack has weaker binding.

### Evidence e26

[quarantine/gascity/cmd/gc/cmd_runtime_drain.go:517–537](../../../quarantine/gascity/cmd/gc/cmd_runtime_drain.go#L517): Exposed drain-ack asks worker to finish current work then permit controller stop.

### Evidence e27

[quarantine/gascity/cmd/gc/cmd_runtime_drain.go:887–912](../../../quarantine/gascity/cmd/gc/cmd_runtime_drain.go#L887): Drain ack releases held claims first, stamps acknowledgment and pokes controller; poke failure only warns.

### Evidence e28

[quarantine/gascity/cmd/gc/cmd_handoff.go:267–356](../../../quarantine/gascity/cmd/gc/cmd_handoff.go#L267): Handoff creates self-addressed context mail and restart request only for restartable sessions; auto PreCompact handoff mail does not restart.

### Evidence e29

[quarantine/gascity/cmd/gc/cmd_handoff.go:433–519](../../../quarantine/gascity/cmd/gc/cmd_handoff.go#L433): Remote handoff guards live subagents unless forced, then persists mail and kills running restartable session for reconciliation restart.

### Evidence e30

[quarantine/gascity/internal/worker/handle_history.go:11–131](../../../quarantine/gascity/internal/worker/handle_history.go#L11): Provider-native transcript discovery/read/raw records/subagent mapping and normalized cursor/tail compaction history; no invented canonical full audit.

### Evidence e31

[quarantine/gascity/internal/events/recorder.go:249–355](../../../quarantine/gascity/internal/events/recorder.go#L249): Record swallows acknowledged-append errors to stderr. RecordAck explicitly not fsynced; AppendBatch reports failure and allocates contiguous sequences under locks.

### Evidence e32

[quarantine/gascity/internal/events/recorder.go:22–143](../../../quarantine/gascity/internal/events/recorder.go#L22): JSONL log rotation and optional archive-age pruning bound retained event history.

### Evidence e33

[quarantine/gascity/internal/events/reader.go:21–71](../../../quarantine/gascity/internal/events/reader.go#L21): Event filters expose type/actor/subject/time and stable seq cursors.

### Evidence e34

[quarantine/gascity/internal/events/reader.go:200–291](../../../quarantine/gascity/internal/events/reader.go#L200): Reads archived and active events, skips malformed JSON lines; scanner failure returns partial data plus error.

### Evidence e35

[quarantine/gascity/cmd/gc/cmd_events.go:150–225](../../../quarantine/gascity/cmd/gc/cmd_events.go#L150): Native events command exposes JSONL list/watch/follow, type/time/payload filters and city cursor resume with incompatible flags rejected.

### Evidence e36

[quarantine/gascity/cmd/gc/cmd_events.go:550–580](../../../quarantine/gascity/cmd/gc/cmd_events.go#L550): Exposed event list uses API or stopped-city local fallback; remote and supervisor consumers supported.

### Evidence e37

[quarantine/gascity/internal/executionevent/projector.go:74–120](../../../quarantine/gascity/internal/executionevent/projector.go#L74): Level-triggered graph facts are restated after failure; step emission marker only committed after acknowledgment, tolerating duplicates rather than permanent miss.

### Evidence e38

[quarantine/gascity/internal/runtime/runtime.go:200–260](../../../quarantine/gascity/internal/runtime/runtime.go#L200): Runtime interface separates Start/Stop/Interrupt/ProcessAlive/Nudge/metadata/peek/list and external runtime ownership.

### Evidence e39

[quarantine/gascity/internal/worker/handle_lifecycle.go:13–98](../../../quarantine/gascity/internal/worker/handle_lifecycle.go#L13): Worker handles start/resume configured external command, attach terminal and create deferred/started session.

### Evidence e40

[quarantine/gascity/cmd/gc/session_observation_uncertainty_test.go:80–170](../../../quarantine/gascity/cmd/gc/session_observation_uncertainty_test.go#L80): Fake runtime and clock tests defer lifecycle and preserve metadata across unavailable observations/nonconsecutive absence.

### Evidence e41

[quarantine/gascity/cmd/gc/nudge_honesty_test.go:76–122](../../../quarantine/gascity/cmd/gc/nudge_honesty_test.go#L76): Tests assert queue downgrade cause and explicit skipped-wake warning, not eventual model reception.

### Evidence e42

[quarantine/gascity/internal/events/conformance_test.go:12–36](../../../quarantine/gascity/internal/events/conformance_test.go#L12): File recorder factory uses real temporary filesystem; fake provider separately receives conformance tests.

### Evidence e43

[quarantine/gascity/internal/events/eventstest/conformance.go:57–112](../../../quarantine/gascity/internal/events/eventstest/conformance.go#L57): Conformance checks event roundtrip fields and nonzero increasing seq; inspected LatestSeq and watch assertions are source only.

