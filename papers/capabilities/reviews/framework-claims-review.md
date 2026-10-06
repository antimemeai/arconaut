# Independent framework and science claim review

## Written review plan

Challenge selected positive mechanisms after the interactive-client and collaboration
cohorts: Letta Code live mods versus safe activation, FastAgent original archive
reconstruction versus semantic context repair, Restate durable replay versus external
effect atomicity, and Claude Science public contracts versus unavailable engine
implementation. Read the canonical claim and its actual source/document wiring, inspect
relevant tests when available, and preserve exact paths/line evidence and bounded
coverage. Do not execute acquired code, install dependencies, use credentials/providers,
or mutate canonical rows without coordinating with the owner. Interpret changes against
Arconaut's post-turn/workflow activation, shared-service consumer boundary, retained
original audit and quiescent refit, not as capabilities transferred from a neighboring
mechanism. A subsequent bounded independent check covers Open SWE original audit,
especially the observer-to-transaction boundary; it follows the same read-only rules.

## Letta Code: live mod agency survives the challenge; safe activation does not follow

Canonical claims reviewed: `letta-code.hot_change` (L; e15–e17),
`turn_redefinition` and `self_improvement` (I, bounded mod optimization; e19–e21).
I agree with the row's distinction. Real mods are transpiled/imported with an mtime
cache key, receive a substantial live API and register disposers; this is real
programmability, not just prompt customization.

The precise activation order matters. [mod-engine.ts](../../../quarantine/letta-code/src/mods/mod-engine.ts)
1405–1484 creates an owner AbortController, imports the module, awaits its factory
and records its disposer. Disposal at 1678–1714 sends abort, synchronously invokes
disposers, unregisters global providers/permissions/tools and clears the local
registry. There is no awaited settlement of arbitrary owner-started OS/provider
work in this function. Reload at 1739–1791 disposes the old registry **before**
loading its replacement, publishes an empty registry, then publishes in-progress
registrations and the completed generation. Generation checks prevent stale load
completion from becoming the current registry; they do not make old→new activation
atomic or defer it until affected work ends. A failed replacement cannot rely on
an intact old registry from this path.

The adapter makes the interruption visible to event semantics:
[mod-adapter.ts](../../../quarantine/letta-code/src/mods/mod-adapter.ts) 150–162
drops best-effort events while loading; 182–231 sets loading, publishes it, awaits
the engine reload and clears loading. No busy-turn/workflow drain is introduced
in these inspected adapter functions. This is a useful explicit limitation, not
a claim that every UI caller always permits reload mid-turn.

The source-read test oracle reinforces the narrower interpretation:
[mod-engine.test.ts](../../../quarantine/letta-code/src/mods/mod-engine.test.ts)
1705–1769 deliberately gates async activation and **expects the empty snapshot**
until release. 1772–1852 makes stale and fresh generations register the same tool
and asserts only the fresh tool/command/generation survive. 1635–1658 asserts
old owner signal aborted and stale panel rejected; it never asserts an external
process/model request ceased. These real temporary-module imports and explicit
gates are good registry fault oracles, read only here.

The optimization path is wired but scoped.
[cli/commands/mods.ts](../../../quarantine/letta-code/src/cli/commands/mods.ts)
755–819 invokes `runModLearning` with generation/evaluation model and candidate/spec
controls. [learning-harness.ts](../../../quarantine/letta-code/src/mods/learning-harness.ts)
1120–1172 evaluates handler count, diagnostics and exact tool-argument preservation;
2355–2374 promotes by copying only a selected passing candidate. This demonstrates
operator-invoked mod experimentation and promotion under its declared assertions,
not broad model-governed harness self-research correctness or executable refit.

Arconaut implication: preserve agency over extensions while defining change
versions, affected-work boundaries and owner-work settlement separately. A signal
and a generation number are insufficient evidence for refit quiescence. No canonical
row mutation requested; the existing bounded cells remain appropriate. Coverage is
the loader/adapter and selected registry tests plus optimization dispatch/promotion;
provider internals, all callers and external-effect termination were not audited.

## FastAgent: strict original reconstruction is real; repair remains a composition task

Canonical claims reviewed: `fast-agent.context_repair` (S; e3/e16/e17/e23),
`original_audit` (L), and the `compact_conversation / reconstruct_history` action
explicitly labeled compaction plus **export primitives**. The positive storage
claim survives independent inspection. It should retain that export/primitive
qualification when entering synthesis.

[history/compaction.py](../../../quarantine/fast-agent/src/fast_agent/history/compaction.py)
490–535 writes an original history snapshot to a temporary file and publishes by
hard link to a UUID-bearing final filename without overwriting another archive;
partial temp files are removed. 568–659 snapshots current history, awaits a tool-free
side-channel summarizer, refuses empty summary or missing archive when persistence
is enabled, and records filename/SHA256, template/tail counts, summarizer prompt
and parsed response before loading the replacement. This preserves reconstructable
model-message originals when configured. It is neither raw provider byte capture
nor an append-only OS/tool effect ledger; no persistence mode intentionally allows
compaction without an archive. Final compacted-session persistence at 689–718 is a
separate best-effort write that catches errors after working history has changed.

[history/atif_reconstruction.py](../../../quarantine/fast-agent/src/fast_agent/history/atif_reconstruction.py)
51–65 validates every archived model message and linked byte digest. 68–160 refuses
multiple checkpoints, non-template prefix, missing archive directory, basename/path
escape, symlinks, count/tail mismatch, ambiguous legacy candidate and cyclic lineage.
161–204 recursively reconstructs original chronology, inserts an explicit context
**replace boundary**, and labels templates/tail as copied context rather than
misrepresenting already-executed tool events as newly executed effects. The archive
check is exact structural evidence, not a semantic guarantee that the summary
retains the right concepts. Fail-closed reconstruction is valuable even when recovery
is unavailable.

The actual exposed wiring matters: [session/trace_exporter.py](../../../quarantine/fast-agent/src/fast_agent/session/trace_exporter.py)
150–182 calls reconstruction for ATIF export. The checked operation produces a
reviewable audit trajectory; it is not itself a model-visible live context-repair
command. [agents/tool_runner.py](../../../quarantine/fast-agent/src/fast_agent/agents/tool_runner.py)
106–133 declares mutation-friendly callbacks and 198–247 awaits the before-model
hook before the actual provider step. These enable a program to compose restoration,
but an automatic semantic repair policy cannot be inferred. Auto-compaction is also
an explicit loop operation: [hooks/compaction.py](../../../quarantine/fast-agent/src/fast_agent/hooks/compaction.py)
76–125 chooses settings/usage and calls compaction with mid-turn/tool-exchange
controls. Managed context changes need their own work-version/serialization rule.
The compactor snapshots, awaits, then replaces without a version check in that
function; this bounded observation is not a claim that every caller races it.

The directly relevant test oracle is stronger than count-only assertions:
[test_compaction.py](../../../quarantine/fast-agent/tests/unit/fast_agent/history/test_compaction.py)
683–689 compares reconstructed non-export messages to exact original history;
693–723 checks recorded summarizer request/response; 825–849 injects archive failure
and proves unchanged working history, and rejects full reconstruction when
persistence was disabled. 852–884 tests unique complete archives, collision bytes
unchanged and cleanup after injected partial serialization. The fake summary model
cannot establish semantic fidelity, and successful local file publication does not
by itself prove power-loss durability. Tests were read, not run.

Arconaut implication: keep immutable original history, model-visible context views
and explicit transform lineage as separate entities. Strict original reconstruction
with replace/copy boundaries is a concrete lead for audit and controlled repair,
while model repair policy, activation ownership and complete IO recording remain
design work. No canonical mutation requested; S context repair and L original audit
are warranted. Coverage excludes all live hot-reload/session/process-supervisor
paths; this review is specifically the compaction→archive→reconstruction chain.

## Restate: journal replay preserves recorded results, not arbitrary external effects

Canonical claims reviewed: `restate.rebuild_continuity` and `original_audit` (L),
`hot_change` (L; e11/e12), and durable execution primitives. Their existing limits
are correct. Restate is independently governed workflow infrastructure rather than
a coding-agent turn engine; Arconaut would consume it as a service.

The actual replay boundary is concrete.
[service_protocol_runner_v4.rs](../../../quarantine/restate/crates/invoker-impl/src/invocation_task/service_protocol_runner_v4.rs)
307–375 sends start metadata/preloaded state and replays stored journal entries from
a storage transaction; 1108–1112 accepts a Run command. 998–1037 decodes a client
Run-completion value/failure into a notification proposal and routes it to the
invoker. [entries/mod.rs](../../../quarantine/restate/crates/worker/src/partition/state_machine/entries/mod.rs)
355–414 applies notification, writes raw journal at the predicted index with
record-created timestamp and updates invocation status. The server stores the
completion protocol payload; it does not execute or atomically commit arbitrary
SDK-closure IO to a third-party system.

The protocol contract makes the split explicit.
[protocol.proto](../../../quarantine/restate/service-protocol/dev/restate/service/protocol.proto)
221–227 carries completion bytes/failure; 235–236 identifies side effects currently
executed by the SDK, and 241–263 states that the reply acknowledges the proposal
being stored/replicated and specifies replay notification ordering. 638–660 separates
Run command from completion. The server sends proposal acknowledgment at
[service_protocol_runner_v4.rs](../../../quarantine/restate/crates/invoker-impl/src/invocation_task/service_protocol_runner_v4.rs)
648–654. An external effect that succeeds before its completion is recorded lies
outside the inspected durable-result boundary. Therefore a crash in that interval
may leave an unknown outcome and require idempotency/reconciliation in that external
system; this is an inference from the separated SDK effect/proposal/commit boundary,
not an executed failure experiment or a claim about every SDK retry policy.

Pause is also narrower than physical cessation:
[manual_pause.rs](../../../quarantine/restate/crates/worker/src/partition/state_machine/lifecycle/manual_pause.rs)
50–72 records Paused, asks the invoker to abort and accepts; comments explicitly
distinguish straggler-effect write fencing. The 130–158 test asserts paused state
and an **abort action**, not that a remote model/provider or OS process stopped.
At resume, [manual_resume.rs](../../../quarantine/restate/crates/worker/src/partition/state_machine/lifecycle/manual_resume.rs)
68–101 resolves deployment and accepts protocol-version compatibility; 187–229
patches pinned deployment at the command's log position and resumes. Compatibility
of a protocol is not equivalence of changed handler control flow or effect order.
The source-read deployment tests at 273–345 exercise mock metadata/pinned versions;
they cannot establish semantic replay safety of an arbitrary new executable.

Likewise [tests/idempotency.rs](../../../quarantine/restate/crates/worker/src/partition/state_machine/tests/idempotency.rs)
108–166 preloads a Completed invocation with result bytes 123, repeats its invocation
key, and asserts the stored output returned. This directly tests Restate's keyed
result behavior, not an external API being executed once.
[journal/schema.rs](../../../quarantine/restate/crates/storage-query-datafusion/src/journal/schema.rs)
19–72 exposes raw entry bytes/index/target/timestamps, useful for protocol study but
not closure-internal provider envelopes/OS IO. All tests were read only.

Arconaut implication: journal managed operations and retained results can restore
logical workflows across a rebuilt process. Refit still needs consumer-side
provider/program quiescence and explicit treatment of uncertain external outcomes.
Never translate durable result replay into an exactly-once guarantee for arbitrary
commands/model requests or a global original audit. No canonical mutation requested.
Coverage is server v4 result/replay, entry application, selected pause/resume and
idempotency tests; SDK closure execution and third-party services remain outside it.

## Claude Science: useful documented contracts, opaque implementation

Canonical claims reviewed: `claude-science.original_audit`, `context_repair`,
`hot_change` and `rebuild_continuity` (L, documentation basis), plus the persistent
kernel/database distinction. The row correctly preserves a documentation basis and
does not turn its public distribution into available engine source. These captured
docs accompany the 0. 1. 55 reference artifacts; the documentation need not describe
every behavior of that exact binary. No binary, installer, example or provider was
executed in this review, and no application source/test oracle is available here.

The kernel contract is specific. [tools-and-environments.md](../../../quarantine/claude-science/raw/tools-and-environments.md)
9–36 documents persistent Python/R variables, kernel termination after idle time,
package operations, session end or restart, and named package environments shared
across projects on the machine. Inline package installation lasts only until kernel
restart; packages installed into a named environment survive sessions. These are
distinct lifetimes. [core-concepts.md](../../../quarantine/claude-science/raw/core-concepts.md)
11–17 gives each session its own workspace and potentially multiple running kernels;
54–58 describes locally stored, editable memory facts. Shared package environments
are not evidence of a shared live computation service, and a local memory database
is not an exposed general-purpose standing SQL database. The public contract leaves
kernel supervision inside the application rather than proving Arconaut's consumer
boundary to independently governed computation.

Artifact provenance is a useful documented study surface with clear retention
limits. [artifacts.md](../../../quarantine/claude-science/raw/artifacts.md) 9–17 keeps
saved artifacts until deletion but clears other written files a few hours after
session completion; 17–27 exposes versions and specific-version links. 31–43 describes
messages, generated code, command execution log, environment versions and review
tabs, calls the execution log authoritative when it differs from the code tab, and
keeps artifact provenance after a session is deleted. This is stronger than an
unstructured chat transcript. It does not expose an engine schema, raw provider
envelopes, original context-transform lineage, every process IO byte or capture and
crash guarantees. The word authoritative concerns the documented artifact command
record; it cannot establish a global complete audit. Past-session/artifact references
at [core-concepts.md](../../../quarantine/claude-science/raw/core-concepts.md) 62–64
support retrieval, not a traced inverse of compaction or model-visible original
history repair operation.

One documented hot-change rule is unusually concrete:
[the-reviewer.md](../../../quarantine/claude-science/raw/the-reviewer.md) 9–24 describes
independent review of recent responses, approved plans, saved artifacts and execution
records; the reviewer does not rerun analyses or establish that a methodology fits
the scientific question. Line 32 says turning automatic review off prevents new reviews
while started or waiting reviews finish, and re-enabling covers work done while off.
That is a useful pending-work activation contract for one subsystem. It does not
generalize to arbitrary program replacement. Conversely,
[configuration-file-reference.md](../../../quarantine/claude-science/raw/configuration-file-reference.md)
9–15 explicitly requires restart for file changes, and 26 makes connector certificates
take effect at connector relaunch. Hot configuration must name its actual subsystem
and activation moment rather than borrow the review toggle's semantics.

Likewise [command-line-settings.md](../../../quarantine/claude-science/raw/command-line-settings.md)
10–25 documents `serve`, `status`, `logs`, `stop` and `update`; 21–42 describes clean
stop, verified atomic vendor binary replacement, explicit build selection and data
layout compatibility checks. Line 37 requires restart after an update. A vendor binary
update is not model-authored recompilation, state handoff or outpost continuation;
the adjective clean does not expose a provider/process-settlement protocol. The
documented Stop action at [core-concepts.md](../../../quarantine/claude-science/raw/core-concepts.md)
50 stops the whole session rather than an individual delegated track. The actual
termination mechanism remains opaque. Operator convenience also has named limits:
[command-line-settings.md](../../../quarantine/claude-science/raw/command-line-settings.md)
80–86 documents sandbox/approval settings, exceptions to approval skipping and
organization policy that can pause new work until restart. This is not evidence of
unrestricted model authority over the harness.

Arconaut implication: study the separate package/kernel/session lifetimes, rich
artifact provenance and reviewer change timing, while requiring owned source and
testable contracts for core audit, managed context repair and refit. No canonical
mutation requested; documentation-based limited cells are appropriate. Coverage is
the six named captured pages and their lifecycle/retention/change claims, not an
unavailable execution engine, proprietary provider runtime or scientific validity.

## Open SWE: strong transactional storage behind a deliberately best-effort observer

This bounded independent review checks the root-authored `open-swe.original_audit`
cell (L; `engine`, `engine-write`, `capture`, `capture-tool`, `writer`, `output`). The
limitation is supported by source. Transactional consistency of accepted records
must remain distinct from completeness of observed activity.

[transcript/engine.py](../../../quarantine/open-swe/agent/transcript/engine.py) 75–149
serializes each thread's append using a transaction-scoped advisory lock, deduplicates
command receipts, assigns contiguous versions, advances the head and notifies. The
in-process publication happens after leaving the transaction context. 208–271 writes
the event, attachment/output sidecars, projection and receipt through the same
connection and transaction. These are concrete positive mechanisms: accepted
commands and their retained sidecars agree, and replaying an accepted command need
not append a duplicate event. They do not assert that every real command/model
interaction reached this append function.

The actual capture layer expressly chooses the opposite failure policy from a
mandatory audit. [middleware/transcript.py](../../../quarantine/open-swe/agent/middleware/transcript.py)
1–12 treats writes as observability, swallows failures and sends events through one
background writer per run so the model stream does not wait for storage. 74–80 bounds
tool/error text and names hidden model-call tags; 472–478 filters those tags from
stream capture and accepts only the first selected streaming protocol. 393–409
converts tool content to text and truncates it to 256 KiB of UTF-8 bytes. The later
[transcript/tool_output.py](../../../quarantine/open-swe/agent/transcript/tool_output.py)
15–59 normalizes truncation flags and stores at most 256 Ki characters in an output
sidecar keyed by thread/tool-call, updating on conflict. Neither path retains the
original structured content or discarded suffix elsewhere in the inspected chain.
These records are useful normalized representations rather than full original IO.

The drain boundary confirms that storage success is not a prerequisite for workflow
completion. [middleware/transcript.py](../../../quarantine/open-swe/agent/middleware/transcript.py)
540–566 catches a failed append, logs it, then calls `queue.task_done()` for every
command in that failed batch. There is no retry or durable failed-batch spool in this
writer. 569–587 waits at most 30 seconds for the queue's completion counter, then
cancels the writer and removes the run registry entry. The queue can therefore drain
after failed persistence, or terminate with pending writes after timeout. This is a
source-grounded capture gap, not a simulated database failure. A successful later
SQL transaction cannot recover events that this observer dropped or filtered.

The source-read test oracle is meaningful and bounded.
[test_engine.py](../../../quarantine/open-swe/tests/transcript/test_engine.py) 1–5 states
that its oracle uses a real migrated schema. 99–128 checks that replay returns version
2 without a second event; 131–150 checks duplicate commands within a batch append
once; 276–310 asserts the truncation flag and exact capped retained length. It tests
the engine's declared retained-state behavior, not full original output retention.
[test_transcript.py](../../../quarantine/open-swe/tests/middleware/test_transcript.py)
32–65 replaces append with an in-memory collecting fake; 253–310 checks one generated
message identity across chunks and canonical final text. 313–335 checks that a
cancelled nested tool does not label its parent interrupted. These do not exercise
durable observer failure, unknown physical IO settlement or complete raw provider
capture. Tests were read only; no database or acquired test program was executed.

Arconaut implication: reuse the idea of sequenced transactional events plus sidecars,
but place original capture, failure retention and capture acknowledgment in the core
contract. A best-effort observer behind the turn engine cannot alone meet the
operator's audit requirement. No canonical mutation requested: the root's L cell
already states this boundary accurately. Coverage is the append/storage/capture
chain and selected tests, not the hosted agent's delegated engine, sandbox lifecycle
or all transcript consumers.

## Review disposition and coverage

All five selected claims retain their bounded positive mechanisms after independent
inspection. No canonical row changes are requested. Synthesis must retain the
distinctions between live registry loading and safe activation; original export
reconstruction and active semantic repair; durable result replay and external effect
settlement; documented artifact provenance and inspected full audit; transactional
retained-state consistency and complete original observation. These are deliberate
source/document reviews, not runtime validation or blanket endorsements of the five
systems. All directly read files and ranges are linked in the corresponding section;
the original cohort studies remain separate from this limited cross-family challenge.
