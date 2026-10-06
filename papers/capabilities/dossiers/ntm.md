# ntm

Heterogeneous tmux agent orchestration with addressed prompt delivery, workflow/pipeline execution, uncertain-send recovery, context rotation and local audit.

Role: terminal coordination control and workflow runtime. Runtime: Go.

Pinned source: [https://github.com/Dicklesworthstone/ntm](https://github.com/Dicklesworthstone/ntm); revision/version `ee589e7e7f95bcb3aec3b5945031e60b1c952097`.

Compiled CLI/TUI plus optional resident/detached workers; tmux/SSH actuates external coding-agent TUIs, and pipeline command steps launch /bin/sh children.

Governs pane/session lifecycle and its workflow workers; provider requests/inner agent programs belong to external harnesses. Consumes Agent Mail and trackers. This is closer to a control plane than an ordinary Arconaut consumer.

Inspection: shared prompt dispatch, tmux submission/interrupt, pipeline command and recovery state, context rotation/native compaction, audit/query, config watcher, mail wrapper and selected real test oracles

Limits of this study: No provider adapters/inner harness tool execution traced; broad ensemble/tracker/REST server/checkpoint import/encryption surfaces remain uninspected. Terminal evidence is bounded and provider-layout dependent.

## Actions

### ntm send / --robot-send; dispatch Prepare/Dispatch

Surface: operator/model CLI or programmatic terminal coordination.

Input: Session, exact pane selectors/type/tag filters, prompt/context file, submit/clear/pacing/redaction options.

Result: Per-pane prepared/delivered/failed/blocked receipts with target/protocol and aggregate counts; no original prompt in neutral receipt.

Lifecycle: Pre-actuation refusal is retry-safe; post-keystroke outcome can be uncertain; terminal submission is separate from model work completion.

Authority: OS/tmux target access, optional configured preflight/redaction/guard; inner agent decides own execution.

Evidence: [e1](#evidence-e1), [e2](#evidence-e2), [e3](#evidence-e3), [e4](#evidence-e4), [e26](#evidence-e26).

### ntm interrupt / SendInterruptForAgent

Surface: operator/model terminal control.

Input: Target agent pane/type.

Result: Terminal-key command success/error.

Lifecycle: Escape for OMP, Ctrl+C otherwise; requests interruption but does not prove provider/work quiescence.

Authority: Control-plane access to target pane.

Evidence: [e5](#evidence-e5).

### ntm pipeline run / resume; Executor.run/ResumeWithOptions

Surface: operator/model CLI and programmable YAML/TOML workflow.

Input: Graph of prompt/command/template/composite steps, variables, dependencies/limits/wait/retries; run ID and continue/restart-failed/force-iter/reset.

Result: Durable run/step states, bounded outputs, per-attempt uncertain/delivered/completed agent record; start acknowledgements for detached worker.

Lifecycle: Saves intent before prompt send; default resume refuses unknown delivery, retains completed rounds/results; deliberate restart may resend. Command effects are not a general transaction.

Authority: Own run/pane locks and child processes; user-defined programs act with OS authority, not inner model tools.

Evidence: [e6](#evidence-e6), [e7](#evidence-e7), [e8](#evidence-e8), [e9](#evidence-e9), [e10](#evidence-e10), [e11](#evidence-e11), [e12](#evidence-e12), [e13](#evidence-e13).

### command step: /bin/sh -c

Surface: workflow supplied primitive.

Input: Substituted command/stdin/env args, cwd, timeout/output limits and wait mode.

Result: Bounded captured/parsed output and native status; tracked child for WaitNone.

Lifecycle: Own process group; Linux leader pinned until descendants settle; TERM then KILL/error if unsettled. Escaped groups not arbitrary OS effect settlement.

Authority: Workflow author grants ordinary OS execution authority; independent daemons remain outside ownership.

Evidence: [e12](#evidence-e12), [e13](#evidence-e13), [e14](#evidence-e14).

### rotate context compact / rotate

Surface: operator/model CLI and control API.

Input: Exact original agent, action and provider supported compaction or generated handoff.

Result: Fresh measured compaction result or replacement pane/rotation outcome.

Lifecycle: Native compaction requires attributed fresh provider accounting and stable ready state. Replacement receives summary before original retirement; failure preserves predecessor.

Authority: Own target agents; no main-harness compile/refit or all-client-work drain implemented.

Evidence: [e15](#evidence-e15), [e16](#evidence-e16), [e28](#evidence-e28).

### sessions restore / RestoreContext

Surface: operator/model CLI and topology API.

Input: Saved pane topology/launch specs/directories, target/force options.

Result: Recreated topology or partial/error outcome.

Lifecycle: Validate before destructive replacement; RestoreContext alone does not launch agents; fresh readiness required for separate context injection.

Authority: Explicit force can kill existing owned session; stops local monitor first.

Evidence: [e28](#evidence-e28), [e29](#evidence-e29).

### ntm audit show / SearchContext

Surface: operator/model CLI and query API.

Input: Session/time/type/target/grep/offset/limit filters and cancellation.

Result: Matching redacted JSONL entries/count/truncated flag.

Lifecycle: Persistent writer-local hashes and buffered/fsync logs; query can skip malformed lines and individual file errors.

Authority: Host file access; privacy can disable records; not all provider traffic available.

Evidence: [e17](#evidence-e17), [e18](#evidence-e18), [e19](#evidence-e19), [e20](#evidence-e20).

### config.Watch

Surface: runtime program API, wired dashboard.

Input: Working directory and merged-config callback.

Result: Reloaded config callback and close function.

Lifecycle: Debounced500ms; invalid reload logs error; dashboard pending update replaces older value, no affected-workflow activation barrier.

Authority: Runtime-owned config reader; project command overrides filtered elsewhere, no blanket model rewrite.

Evidence: [e23](#evidence-e23), [e24](#evidence-e24).

### ntm mail send

Surface: operator/model external service client.

Input: Session agents/addresses, explicit subject/body/file or one-use prepared-redaction handle.

Result: Delegated Agent Mail delivery result.

Lifecycle: Consumes separately running mail service; prepared handle consumption differs from persistent mailbox lifecycle.

Authority: HumanOverseer/client admission is separate from service identity and agent terminal control.

Evidence: [e30](#evidence-e30).

## Capabilities

### filesystem

**I — Files** (source): Workflow commands have ordinary cwd/filesystem authority; pipeline state, audit and history write owned local files. Not an inner-agent filesystem tool suite.

Evidence: [e9](#evidence-e9), [e12](#evidence-e12), [e22](#evidence-e22).

### processes

**I — OS programs** (source): Controls terminal sessions/agent-specific keys and owns isolated command groups; Linux WNOWAIT pins identity through descendant cleanup. External providers/escaped daemon work are separate.

Evidence: [e5](#evidence-e5), [e12](#evidence-e12), [e13](#evidence-e13), [e14](#evidence-e14), [e29](#evidence-e29).

### code-actions

**S — Code actions** (source): User-defined shell workflow and external program orchestration are code-action support; no persistent model code interpreter is supplied.

Evidence: [e12](#evidence-e12).

### persistent-kernel

**— — Kernel** (inspection): Outside this terminal coordination control and workflow runtime role: the reference supplies Heterogeneous tmux agent orchestration with addressed prompt delivery, workflow/pipeline execution, uncertain-send recovery, context rotation and local audit. rather than an agent execution engine.

### standing-database

**— — Standing DB** (inspection): Outside this terminal coordination control and workflow runtime role: the reference supplies Heterogeneous tmux agent orchestration with addressed prompt delivery, workflow/pipeline execution, uncertain-send recovery, context rotation and local audit. rather than an agent execution engine.

### workflow-programming

**I — Workflows** (source): Executable persisted pipeline graphs, variable substitution, composite round progress and resume policies; the agent pane is an endpoint, not a supplied generic LLM turn implementation.

Evidence: [e6](#evidence-e6), [e9](#evidence-e9), [e10](#evidence-e10), [e11](#evidence-e11), [e12](#evidence-e12).

### multi-model

**I — Models** (source): Addresses heterogeneous external agent panes by native type/variant and routes per-agent submit/interrupt choreography; it does not own their provider adapters.

Evidence: [e1](#evidence-e1), [e2](#evidence-e2), [e5](#evidence-e5).

### live-collaboration

**S — Peer chat** (source): Addressed pane prompts and external Agent Mail let operator/model coordinate live agents; receipt shows transport submission, not recipient decision or peer-room original context.

Evidence: [e1](#evidence-e1), [e2](#evidence-e2), [e30](#evidence-e30).

### concurrent-work

**I — Concurrency** (source): Parallel/composite workflow progress and cross-process pane/run ownership support concurrent work; state resumes settled rounds while unknown sends require intervention.

Evidence: [e6](#evidence-e6), [e10](#evidence-e10), [e11](#evidence-e11).

### steering-interrupt

**L — Steer/interrupt** (source): Can clear/stage/submit/interrupt per-agent input. Busy/TUI observations are layout-dependent; unknown/capture failures can proceed best effort, key success does not settle execution.

Evidence: [e2](#evidence-e2), [e3](#evidence-e3), [e5](#evidence-e5).

### turn-redefinition

**— — Turn program** (source): Owns outer coordination/pipeline programs, while the external coding agents own their provider turns. No replacement of their internal turn programs is supplied.

Evidence: [e1](#evidence-e1), [e12](#evidence-e12).

### compaction

**I — Compaction** (source): Native provider compaction has exact target/process checks, server-wide transcript attribution, fresh accounting and stable completion; generated rotation summary is a distinct replacement mode.

Evidence: [e15](#evidence-e15), [e16](#evidence-e16).

### context-repair

**S — Repair** (source): Archived prompt history and predecessor-preserving handoff can supply repair input; saved context is withheld until exact replacement readiness. No reconstruction of every original provider/context byte.

Evidence: [e15](#evidence-e15), [e21](#evidence-e21), [e22](#evidence-e22), [e28](#evidence-e28).

### original-audit

**L — Original audit** (source): Has redacted hash-linked buffered JSONL, prompt history and bounded delivery evidence. Privacy disables records, text is redacted/clipped, call sites may ignore audit errors; not all external model traffic or OS effects.

Evidence: [e1](#evidence-e1), [e7](#evidence-e7), [e17](#evidence-e17), [e18](#evidence-e18), [e21](#evidence-e21), [e22](#evidence-e22), [e29](#evidence-e29).

### audit-query

**L — Audit query** (source): Structured/grep/time/session query over stored audit with bounded defaults and cancellation; malformed lines and per-file errors silently omitted, so successful query is not a completeness verdict.

Evidence: [e19](#evidence-e19), [e20](#evidence-e20).

### hot-change

**I — Hot change** (source): Merged config watcher is actually wired into dashboard; debounced updates may replace pending value. It does not wait current turn/affected workflows or rebuild every client/runtime.

Evidence: [e23](#evidence-e23), [e24](#evidence-e24).

### rebuild-continuity

**L — Rebuild continuity** (source): Actual external-agent replacement launches same identity/type and delivers generated handoff before predecessor retirement; topology restore is separate. This does not compile/reinhabit NTM or preserve unresolved provider/program execution.

Evidence: [e15](#evidence-e15), [e28](#evidence-e28), [e29](#evidence-e29).

### remote-services

**I — Remote** (source): Consumes Agent Mail and can actuate separate tmux endpoint over SSH; local transcript compaction explicitly refuses remote accounting assumptions.

Evidence: [e16](#evidence-e16), [e25](#evidence-e25), [e30](#evidence-e30).

### self-improvement

**? — Self-improve** (source): No model-governed experimental modification/evaluation/activation of the NTM harness established in inspected dispatch/pipeline/rotation/config paths. User-authored workflows alone do not prove it.

### complaints

**? — Complaints** (source): No agent-state complaint/bead action established in inspected dispatch, mail, pipeline, rotation and audit paths; broad support/tracker surfaces were not inspected.

### authority

**L — Authority** (source): OS/tmux control and optional DCG/redaction preflight; positive dead-shell/trust-frame refusals precede actuation. DCG-unavailable skips gate; inner agent permissions are independent.

Evidence: [e2](#evidence-e2), [e26](#evidence-e26), [e29](#evidence-e29).

### evaluation

**L — Evaluation** (source): Native compaction checks measured fresh reduction; workflow declared outputs only stat existence and warn without failing status, not research quality or harness improvement truth.

Evidence: [e16](#evidence-e16), [e27](#evidence-e27).

### time-order

**I — Time/order** (source): Persisted prompt delivery stages/times, completed-round watermarks, wall-clock expiry/poll thresholds and writer-local sequence/hash; no global order across agent effects.

Evidence: [e6](#evidence-e6), [e7](#evidence-e7), [e10](#evidence-e10), [e16](#evidence-e16), [e18](#evidence-e18).

## Inspected test oracles

- [quarantine/ntm/internal/pipeline/submission_verification_test.go](../../../quarantine/ntm/internal/pipeline/submission_verification_test.go): Unknown transport and explicit resend authority. Oracle: Read277–308: failed submission creates sending intent and exactly one attempt; ordinary resume sends nothing; explicit restart-failed authorizes exactly one additional paste. Mock transport oracle attacks duplicate dispatch, not actual provider acceptance. Read, **not executed**.
- [quarantine/ntm/internal/context/rotation_test.go](../../../quarantine/ntm/internal/context/rotation_test.go): Fresh accounting, transcript identity, busy completion and process substitution. Oracle: Read75–160: eight explicit modes, exact command/count and before150000/after30000 oracle; only reduced/busy-then-reduced succeed. Injected observations do not validate every changing provider layout. Read, **not executed**.
- [quarantine/ntm/internal/tmux/opencode_submit_test.go](../../../quarantine/ntm/internal/tmux/opencode_submit_test.go): Composer versus transcript echo and dropped payload. Oracle: Read1–105: recorded scrubbed OpenCode captures assert exact composer strings, unrelated/other-agent/plain-shell negatives; native live-pane test is gated and was not run. Read, **not executed**.
- [quarantine/ntm/internal/pipeline/process_group_lifecycle_linux_test.go](../../../quarantine/ntm/internal/pipeline/process_group_lifecycle_linux_test.go): Descendant effect after cancellation. Oracle: Read61–98: child ignoresTERM and redirects IO, publishes PID, waits on gate; after cleanup returns gate opens and any late-effect file fails. Strong causal OS oracle, Linux-specific, read only. Read, **not executed**.
- [quarantine/ntm/internal/config/config_test.go](../../../quarantine/ntm/internal/config/config_test.go): Actual project reload and command-source precedence. Oracle: Read1116–1183: filesystem edit waits callback and asserts project cc5 and retained global command; real async timeout catches no update. Not an all-workflow activation test. Read, **not executed**.
- [quarantine/ntm/internal/audit/audit_test.go](../../../quarantine/ntm/internal/audit/audit_test.go): Writer-discriminated chain filenames. Oracle: Read435–497: test name/comment says concurrent writers, but only first logger is constructed/logged; verifies one file checksum and prefix/date/discriminator. It does not instantiate second writer or concurrent writes, so name overstates oracle. Read, **not executed**.

## Useful mechanisms

- Durable unknown-send state refuses guessing/replay; explicit restart distinguishes intentional duplicate authority.
- Provider compaction requires attributed fresh accounting rather than cached token estimates.
- Replacement handoff preserves original until context submission succeeds.
- Linux process cleanup pins leader identity and tests post-cleanup causal effect.
- Model-friendly machine outputs separate terminal transport receipt from messages.

## Material limits

- Go/tmux control plane rather than provider-owning agent; consumer versus governance boundary differs from Arconaut.
- Screen parsing/layout/capture errors limit terminal acceptance certainty.
- Audit/log originals redacted/bounded and privacy/ignored failures leave gaps.
- Hash chains are writer-local; inspected concurrency-named test is actually single writer.
- No core executable refit or durable settlement of arbitrary external effects.

## Arconaut design questions

- Which operations need separate offered/submitted/accepted/completed identities, and when does uncertain state require a model decision rather than automatic retry?
- Can a provider-native compaction contract expose attributed before/after originals/accounting directly, avoiding screen heuristics?
- What minimum owned-program settlement boundary should survive compile/refit while shared daemons remain external?
- How can the model write expressive workflows without making its consumer harness the whole fleet governor?

## Evidence

### Evidence e1

[quarantine/ntm/internal/dispatch/dispatch.go:1–197](../../../quarantine/ntm/internal/dispatch/dispatch.go#L1): Human/robot adapter shares neutral addressed Request, target/protocol, per-pane redaction-safe receipts. Result intentionally excludes outbound original text.

### Evidence e2

[quarantine/ntm/internal/dispatch/dispatch.go:373–439](../../../quarantine/ntm/internal/dispatch/dispatch.go#L373): Delivery refuses known dead-agent shells/trust frames before keystrokes, marks guaranteed non-actuation, then composer clear and stage/single/double Enter with verifier. Unknown/capture failures may proceed best effort.

### Evidence e3

[quarantine/ntm/internal/tmux/opencode_submit.go:132–192](../../../quarantine/ntm/internal/tmux/opencode_submit.go#L132): OpenCode pre-Enter refuses positively empty composer after polls but capture/layout failure returns success; post-submit verifies held payload separately.

### Evidence e4

[quarantine/ntm/internal/tmux/session.go:2201–2232](../../../quarantine/ntm/internal/tmux/session.go#L2201): Double-Enter uses cancellable delays, optional OpenCode staged check, then two Enter actuation calls.

### Evidence e5

[quarantine/ntm/internal/tmux/session.go:3254–3296](../../../quarantine/ntm/internal/tmux/session.go#L3254): Interrupt sends agent-specific terminal key: Escape for OMP, otherwise Ctrl+C. It is not direct provider/program settlement.

### Evidence e6

[quarantine/ntm/internal/pipeline/executor.go:1738–1847](../../../quarantine/ntm/internal/pipeline/executor.go#L1738): Saved uncertain-send intent precedes paste; recovery binds step hash/session/tmux endpoint/pane PID. Sending status refuses resume; successful verification stores delivered.

### Evidence e7

[quarantine/ntm/internal/pipeline/executor.go:1845–1916](../../../quarantine/ntm/internal/pipeline/executor.go#L1845): Observed response accumulates bounded scrollback deltas excluding prompt echo; WaitNone is only submit completion, time/idle modes differ, completion saved before pane release.

### Evidence e8

[quarantine/ntm/internal/pipeline/executor.go:987–1007](../../../quarantine/ntm/internal/pipeline/executor.go#L987): Automatic retry stops for sending-state unknown delivery rather than assuming failure means non-delivery.

### Evidence e9

[quarantine/ntm/internal/pipeline/state.go:260–344](../../../quarantine/ntm/internal/pipeline/state.go#L260): Persistent JSON run state validates run ID/version, uses atomic write and preserves schema marker; this is a current recovery projection, not an append-all-original ledger.

### Evidence e10

[quarantine/ntm/internal/pipeline/state_resume.go:13–95](../../../quarantine/ntm/internal/pipeline/state_resume.go#L13): Continue/restart-failed/force-iter and explicit reset define recovery policy; foreach tracks item fingerprint/completed iterations/round watermarks to avoid rerunning settled work.

### Evidence e11

[quarantine/ntm/internal/pipeline/background_resume.go:1–105](../../../quarantine/ntm/internal/pipeline/background_resume.go#L1): Detached same-binary resume acknowledges startup; stable run ID/new immutable request identity, child RunControl ownership precedes load/verification; no parent replacement checkpoint.

### Evidence e12

[quarantine/ntm/internal/pipeline/executor.go:1305–1400](../../../quarantine/ntm/internal/pipeline/executor.go#L1305): Command steps launch /bin/sh -c with own process group/env/cwd, bounded stdout/stderr, timeout and explicit wait mode; fire-and-forget child wait is tracked.

### Evidence e13

[quarantine/ntm/internal/pipeline/process_wait_linux.go:16–128](../../../quarantine/ntm/internal/pipeline/process_wait_linux.go#L16): Linux WNOWAIT pins leader before group signal; waits ordinary group descendants, TERM/grace/KILL/settle joins observer then reaps. Escaped groups and settlement errors remain bounded claims.

### Evidence e14

[quarantine/ntm/internal/pipeline/process_group_unix.go:1–100](../../../quarantine/ntm/internal/pipeline/process_group_unix.go#L1): Owned Setpgid, bounded inherited-output wait and group signals with best-effort procfs descendant fallback; not a shared service governor.

### Evidence e15

[quarantine/ntm/internal/context/rotation.go:1505–1615](../../../quarantine/ntm/internal/context/rotation.go#L1505): Waits source completed handoff, launches replacement and submits context before retiring original; handoff failure preserves original and cleans replacement. Summaries are not exact heap/context recovery.

### Evidence e16

[quarantine/ntm/internal/context/rotation.go:1684–1872](../../../quarantine/ntm/internal/context/rotation.go#L1684): Native compaction checks original process identity, server-wide provider transcript attribution, fresh accounting and completed input state; rejects ambiguity/remote transcript assumptions and requires two stable polls.

### Evidence e17

[quarantine/ntm/internal/audit/logger.go:145–193](../../../quarantine/ntm/internal/audit/logger.go#L145): LogEvent sanitizes/redacts payload/metadata and may skip persistence due to privacy; caller must handle failure separately.

### Evidence e18

[quarantine/ntm/internal/audit/logger.go:513–580](../../../quarantine/ntm/internal/audit/logger.go#L513): Writer-local audit entries gain UTC time, sequence/hash link then buffer; flush performs fsync, but Log can return before buffer flush.

### Evidence e19

[quarantine/ntm/internal/audit/query.go:118–202](../../../quarantine/ntm/internal/audit/query.go#L118): Structured audit search default limit10000 returns partial on cancellation, but individual file errors are skipped rather than surfaced as complete-corpus uncertainty.

### Evidence e20

[quarantine/ntm/internal/audit/query.go:335–383](../../../quarantine/ntm/internal/audit/query.go#L335): Grep and structured filter parse line records; malformed JSON lines are skipped and scanner capped10MB.

### Evidence e21

[quarantine/ntm/internal/session/prompts.go:100–132](../../../quarantine/ntm/internal/session/prompts.go#L100): Prompt history uses privacy guard and cross-process lock; privacy skips success rather than recording original.

### Evidence e22

[quarantine/ntm/internal/session/prompts.go:180–238](../../../quarantine/ntm/internal/session/prompts.go#L180): Prompt history current JSON writes atomically after redaction; warn/block persistence modes coerce to redact, so originals are not universally retained.

### Evidence e23

[quarantine/ntm/internal/config/watch.go:12–78](../../../quarantine/ntm/internal/config/watch.go#L12): Merged global/project configuration watcher debounces500ms and calls callback only after successful reload; errors keep previous config.

### Evidence e24

[quarantine/ntm/internal/tui/dashboard/model.go:694–723](../../../quarantine/ntm/internal/tui/dashboard/model.go#L694): Dashboard actually wires config watcher, replacing pending channel update when full. No global end-of-turn activation is imposed.

### Evidence e25

[quarantine/ntm/internal/tmux/client.go:279–301](../../../quarantine/ntm/internal/tmux/client.go#L279): Tmux commands run locally or through SSH target with separate command construction; cancellation governs transport, not all remote agent effects.

### Evidence e26

[quarantine/ntm/internal/cli/send.go:3588–3626](../../../quarantine/ntm/internal/cli/send.go#L3588): Optional DCG inspects likely commands for non-Claude targets; unavailable adapter skips gate, not a universal tool authorization layer.

### Evidence e27

[quarantine/ntm/internal/pipeline/executor.go:604–657](../../../quarantine/ntm/internal/pipeline/executor.go#L604): Cancel cancels workflow context; declared-output stat checks issue warnings and never flip pipeline status. Existence is not quality or semantic evaluation.

### Evidence e28

[quarantine/ntm/internal/checkpoint/restore_context_ready.go:16–90](../../../quarantine/ntm/internal/checkpoint/restore_context_ready.go#L16): Saved-context delivery waits exact target and fresh stable observations, without clearing draft/answering dialogs/interruption/relaunching merely to make it ready.

### Evidence e29

[quarantine/ntm/internal/session/restore.go:24–139](../../../quarantine/ntm/internal/session/restore.go#L24): Session restore validates topology/spec/directories before optional forced replacement, pins physical panes and stops local resident monitor before kill-session; audit failures ignored at this call site.

### Evidence e30

[quarantine/ntm/internal/cli/mail.go:100–146](../../../quarantine/ntm/internal/cli/mail.go#L100): Mail send accepts explicit raw/prepared-redaction handle/file/body; handle requires explicit subject and consumes once to avoid raw prefix leaking into derived metadata.

