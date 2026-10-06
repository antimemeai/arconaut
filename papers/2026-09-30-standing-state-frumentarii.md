# Standing state, queryable execution, and the workbench

2026-09-30. Intermediate frumentarii report for Arconaut. This is source-led
reconnaissance, not a storage selection or an implementation plan.

## Scope and evidence

Read the active [doctrine](../BLACKBIRD.md), [agent instructions](../AGENTS.md),
[foundation brief](../docs/FOUNDATION.md), README, and journal. The operator's
subsequent clarifications received during this investigation also govern its
questions: model ergonomics; live collaboration; almost no routine approval
friction; hot redefinition of the turn; managed compaction; comprehensive audit;
hot reload where physically possible; and human/model improvement of the harness
in situ, including continuity into a rebuilt harness. Rust is optional, JS/TS and
Go are strongly distrusted, and JVM approaches generally excluded. A small stable
substrate with mutable control is one research alternative, not an adopted design.

This report distinguishes **observed source**, **documented vendor behavior**, and
**inference/design questions**. Public documentation was checked on September 30;
source revisions are recorded below. No reference implementation was installed,
executed, or tested. Source inspection is deliberately narrow. It cannot establish
end-to-end reliability, performance, audit completeness, or advertised continuity.
Historical repository instructions were read only as reference material.

The six mechanisms answer different questions. An evidence database holds
research claims; an execution journal supports resumed computation; a reactive
database coordinates current state; a versioned database supports alternatives;
memory files and compaction govern model context; a semantic query engine helps
study the resulting corpus. Calling all six “memory” hides their failure modes.

## 1. Claude Science: evidence database, app memory, and artifact provenance

The operator's reference is real. Anthropic's June 30 announcement describes
Jérôme Lecoq's custom computational-review workflow: roughly twenty skills,
agents extracting claims and quantitative findings into an evidence database,
other agents building figures from it, and separate critics checking the work.
This is a concrete example of structured intermediate research shared among
agents. The announcement does **not** publish that database's schema, engine,
query/write protocol, transaction boundaries, or recovery behavior. It is not
evidence that every Science session exposes a general standing-state database.
[Announcement](https://www.anthropic.com/news/claude-science-ai-workbench).

Official app documentation describes a separate mechanism: short remembered
facts live in a local app database. Humans can list, add, edit, or delete them;
recalled facts enter the session's model input. Projects contain sessions and
artifacts, each session has its own workspace and kernels, and history does not
automatically follow an account to another computer. Standing access grants
exist, but plans and delegation retain product-specific controls, including
whole-session rather than individual-track stopping. These are documented
affordances, not an observed public SQL/API interface to the internal database.
[Core concepts](https://claude.com/docs/claude-science/core-concepts).

Artifact provenance is more substantial than remembered facts. A version has
messages, a reproducibility script, execution log, environment, and review tabs.
The documentation explicitly makes the execution log authoritative when the
script disagrees. Human text edits create versions, and conversation links retain
the referenced version. However, unsaved scratch files are removed a few hours
after a session ends. Saved artifacts persist until deletion; deleting a session
preserves their provenance, while deleting a project removes it. A useful artifact
history therefore does not imply comprehensive retention of the work that led
there. [Artifacts](https://claude.com/docs/claude-science/artifacts).

The reviewer compares claims to the recorded work. It does not rerun analyses or
decide whether the chosen method was appropriate for the research question.
That bounded responsibility is valuable: an observer can challenge what happened
without being misrepresented as an oracle for scientific validity.
[Reviewer](https://claude.com/docs/claude-science/the-reviewer).

**Inference for Arconaut.** Study shared structured findings and inspectable
artifact versions, while keeping authoritative execution evidence separate from
generated explanations. The reviewed pages do not establish raw provider request
and streaming-response retention, complete process byte streams, configuration
history, compaction lineage, or programmatic parity with every human control.
Persistent kernels also do not establish recoverability of kernel memory after
a crash. These are unknowns, not demonstrated absences in proprietary code.

## 2. Restate: query the execution itself, then intervene with a version

Restate exposes SQL tables for invocations, inboxes, journals, deployments,
idempotency, and application state. The UI, CLI, and HTTP query endpoint offer
different entrances to the same inspectable system. Application state can be
joined to running work; it is not necessary to infer “what is blocked?” from
chat prose. Retention matters: invocation records are not a perpetual audit;
completed workflow/idempotent invocation records have configured retention.
The documentation also describes rejecting a state edit if intervening work
changed the state, unless the caller forces it.
[Introspection](https://docs.restate.dev/services/introspection).

**Observed source.** The Rust CLI's state editor and JSON patch command compute a
version from the read state and submit it with the proposed mutation. Force omits
that version. The patch command explicitly distinguishes successful submission
from later application; mutations can queue behind an active invocation. JSON
output and direct patch input make the same operation usable by a model or script.
This is a useful concrete alternative to both unguarded shared-variable writes
and mandatory human approval of each edit. I inspected the client construction,
not a full proof of the server's concurrency path.
[Patch](https://github.com/restatedev/restate/blob/2180b55410f6512f8534012167aa536116d75c8a/cli/src/commands/state/patch.rs),
[edit](https://github.com/restatedev/restate/blob/2180b55410f6512f8534012167aa536116d75c8a/cli/src/commands/state/edit.rs),
[mutation utility](https://github.com/restatedev/restate/blob/2180b55410f6512f8534012167aa536116d75c8a/cli/src/commands/state/util.rs).

Service semantics are explicit: ordinary services allow concurrency; virtual
objects serialize writes per key while allowing shared reads; workflows have a
single main execution per identifier and supporting handlers. Deployments are
immutable, with in-flight invocations pinned to their deployment. This supplies
an intelligible version boundary, but it does not demonstrate hot replacement of
an arbitrary ongoing computation.
[Services](https://docs.restate.dev/foundations/services).

Durable steps wrap nondeterministic work and journal its result for recovery;
failing steps can be retried. The documented unit is the step, not every byte
exchanged with a provider or subprocess.
[Durable steps](https://docs.restate.dev/develop/ts/durable-steps).
**Inference:** successful result journaling can prevent repeated completed work,
yet an external effect followed by a lost acknowledgement still needs a defined
resolution. The database's knowledge of the effect and the external world's
state are different facts. Capturing only the returned string would miss partial
streams, attempts, and intermediate process interactions needed by Arconaut's
audit. Queryable recovery state is promising; its completeness must not be
inflated into comprehensive audit. The inspected Restate code carries a Business
Source License header: this is a source-available reference, not an unqualified
open-source recommendation.

## 3. SpacetimeDB: transactional shared state with live subscribed views

SpacetimeDB makes application changes through reducers that execute as atomic
database transactions. Clients subscribe to queries and maintain local views
with change callbacks; a Rust client path is available. This supports the
interesting workbench case where a model, terminal client, and visual interface
observe the same evolving structured state without polling rendered chat.
[Reducers](https://spacetimedb.com/docs/functions/reducers/),
[Rust clients](https://spacetimedb.com/docs/clients/rust/).

A consequential current distinction appears in the v2 migration documentation:
confirmed reads wait for durable transactions before exposing results, whereas
the older behavior could expose changes earlier. Clients can opt out. “Another
participant saw it” and “it survives failure” are therefore deliberately separate
contracts, not interchangeable definitions of commit.
[Migration documentation](https://spacetimedb.com/docs/upgrade/?client-language=rust&server-language=rust).

**Observed source.** The durability layer separates enqueueing a transaction from
the advancing durable offset; consumers can wait for an offset. Subscription
messages carry transaction-offset channels for visibility control, and the Rust
SDK exposes a confirmed-read option. This gives concrete paths to inspect when
studying visibility and recovery together. It is not a failure-injection result.
[Durability](https://github.com/clockworklabs/SpacetimeDB/blob/629e8c1e809a3d60bba7c863243a64d1e0dbb7d9/crates/durability/src/lib.rs),
[subscription manager](https://github.com/clockworklabs/SpacetimeDB/blob/629e8c1e809a3d60bba7c863243a64d1e0dbb7d9/crates/core/src/subscription/module_subscription_manager.rs),
[client option](https://github.com/clockworklabs/SpacetimeDB/blob/629e8c1e809a3d60bba7c863243a64d1e0dbb7d9/sdks/rust/src/db_connection.rs).

The documented commit log supports reconstruction of committed database state.
It records transaction mutations, with intermediate in-transaction states absent
from the final mutation set. The documentation describes prefix replay and a
log that is not compacted. These are useful historical-state properties, but a
log of committed database effects does not automatically contain rejected
requests, tool streams, external processes, or what a model actually received.
[Commit log](https://spacetimedb.com/docs/reference/internals/commitlog/).

**Inference for Arconaut.** Reactive current-state views and comprehensive audit
can coexist, but the former do not supply the latter by accident. If a model
subscribes to a queue while its context is being compacted, what identifies the
exact visible state and pending notifications? If configuration is a table row,
which operation adopted which version? Atomic reducers constrain database
mutations; they do not resolve an external process's lifetime or make every
application invariant true. The relevant research is about those boundaries,
not selecting a reducer framework in advance.

## 4. Dolt: branching structured alternatives for in situ experiments

Dolt exposes SQL state together with database branches and commits. Its current
transaction page states Read Committed and distinguishes SQL transaction commits
from Dolt history commits; an option can connect the two. However, the February
2026 concurrency article below calls the isolation level repeatable read. This
is a conflict between primary sources, unresolved in this lane; a design must
check the target version's implementation and behavior before relying on either
label. Merely using SQL `COMMIT` does not justify claiming every intermediate
state has become a named historical revision.
[Transactions](https://www.dolthub.com/docs/concepts/dolt/sql/transaction/).

The vendor's February 2026 concurrency account describes three-way merging of
concurrent changes: disjoint cells can merge, while different values written to
the same cell conflict. This was read, not independently exercised. Its
workspaces provide special temporary refs with query history and cumulative diffs,
which can be committed or discarded. These are useful precedents for letting
humans and models explore structured alternatives through ordinary queries.
[Concurrency](https://www.dolthub.com/blog/2026-02-17-dolt-concurrency/),
[workspaces](https://www.dolthub.com/docs/products/dolthub/workspaces/).

**Inference for Arconaut.** Branches could represent alternative context policies,
research hypotheses, or experiment definitions. They should not suggest that
the external world branches with the database: a launched process, sent message,
or provider charge is not undone by reverting rows. Nor does a clean cell-level
merge establish a coherent combined research conclusion. Attempts, discarded
hypotheses, and unsuccessful executions need to remain queryable as events if
the audit is to support studying improvement rather than only inspecting the
winning state. Model annotations must remain distinguishable from observations.

Dolt is a mechanism reference here, especially given the operator's Go concerns;
it is not an implied core dependency. No Dolt implementation code was inspected
in this lane. The existing use of Dolt by beads selects nothing for Arconaut's
product runtime. Hot branch switching is also not evidence of safe hot code or
schema replacement while tools are active.

## 5. Current Letta: versioned memory and explicit compaction events

A freshness trap matters here. At the inspected revision, `letta-ai/letta` points
to `letta-code` as current source and identifies the old server as archived.
An assessment based only on older memory-block APIs would describe a different
generation of the system.
[Repository README](https://github.com/letta-ai/letta/blob/5bcdd177d70fa2b31a754cfcd801e77b2e1ab16a/README.md).

Current SDK material presents memory as a Git repository: `system/` files enter
the prompt each turn; other files are read on demand. Background consolidation
can be triggered by step counts or compaction, with reminder or automatic launch
behavior. The SDK offers local, remote, and hosted backends. These are vendor
documented interfaces; the local runtime is Node/TypeScript, making it a mechanism
reference rather than a fit for the requested core.
[Agent SDK](https://www.letta.com/agent-sdk/).

**Observed source, shared writes.** The attached-repository sync path serializes
work by mount path within the process, skips dirty/conflicted trees, and pushes
clean pending commits. A non-fast-forward push leads to rebase and another push;
conflicts and failures remain explicit outcomes. The in-process queue is not a
global lock across independently running agents. Git supplies a history and
conflict language, but textual conflict resolution does not establish semantic
consistency of shared memory.
[Repository sync](https://github.com/letta-ai/letta-code/blob/96eb977b9477045423005a3630e5f9293140249a/src/agent/attached-repository-git-sync.ts).

**Observed source, compaction.** Local transcripts use versioned JSONL entries
with identifiers, parent links, and timestamps. Compaction has its own entry:
summary, first retained entry, token count, replacement message, and optional
statistics. The normal compaction path changes the active message IDs and appends
that event instead of replacing all transcript history. Separate rewrite and
migration/repair paths exist, so this is not evidence of an immutable audit.
The inspected append call does not by itself establish crash durability. Crucially,
persisted history and the model's active context are explicitly different objects.
[Transcript types](https://github.com/letta-ai/letta-code/blob/96eb977b9477045423005a3630e5f9293140249a/src/backend/local/local-transcript.ts),
[store and compaction persistence](https://github.com/letta-ai/letta-code/blob/96eb977b9477045423005a3630e5f9293140249a/src/backend/local/local-store.ts).

Compaction source offers all-history and sliding-window modes with configurable
instructions and tool-return clipping. This is a useful policy surface to study.
It also shows why retained original input is necessary: a compacted message is
an interpreted replacement, and clipped summarizer input may already differ
from the full interaction. The inspected transcript types do not establish
retention of raw provider exchanges, every runtime configuration transition,
or process I/O below tool-message representation.
[Compaction](https://github.com/letta-ai/letta-code/blob/96eb977b9477045423005a3630e5f9293140249a/src/backend/local/compaction.ts).

**Inference for Arconaut.** A saved conversation and memory repository offer
material for resumption, not restored model cognition or restored OS resources.
A new model request reconstructs input from records; it cannot recover provider
internal state that was never exposed. Compaction should be a queryable,
replaceable operation over retained material, with its chosen policy and input
visible to both human and model. Background consolidation is itself work whose
inputs, changes, conflicts, and consequences need attribution in the audit.

## 6. Squirreling: query-time interpretation of the audit corpus

The May 2026 paper *A Query Engine for the Agents* proposes an asynchronous query
engine where expensive cells can be evaluated lazily, including remote model
calls. The interesting idea for this task is not a benchmark number. It is that
studying agent traces may require combining ordinary predicates with expensive
interpretation, while avoiding a model call for every stored row.
[Paper](https://arxiv.org/abs/2605.27785).

**Observed source.** Squirreling's expression evaluator obtains cell values by
calling their deferred functions and short-circuits eligible predicates. Its
expensive-cell tests count evaluations: selecting only cheap columns, applying
cheap filters, and limiting results should avoid unnecessary expensive reads.
These tests were inspected, not run. The evaluator also explicitly yields to
the JavaScript event loop so cancellation can fire.
[Evaluator](https://github.com/hyparam/squirreling/blob/249f5bb27ed4d685e13bd23bf67bfc8053117eab/src/expression/evaluate.js),
[direct counting tests](https://github.com/hyparam/squirreling/blob/249f5bb27ed4d685e13bd23bf67bfc8053117eab/test/execute/expensive.test.js).

**Inference for Arconaut.** This is an analysis mechanism, not a durable state
system or core-runtime proposal. It could inform questions such as “among runs
that lost an outstanding obligation after compaction, which summaries omitted
the relevant source?” Structural filtering can narrow cases before semantic
interpretation. The interpretation must become an attributed, revisable derived
result, with its model, instructions, inputs, and actual output retained. It
cannot overwrite the evidence it judges. A repeated query using a nondeterministic
model is a new experiment unless it explicitly uses a retained result. The JS
implementation conflicts with the requested core direction; its useful lazy
evaluation pattern can be studied independently of that implementation choice.

## What each mechanism actually preserves

This table condenses the evidence above, not additional vendor guarantees.

| Mechanism | Strongest demonstrated/documented surface | Concurrent change model | What it does not establish for the audit |
| --- | --- | --- | --- |
| Science workbench | Structured research outputs, editable remembered facts, versioned artifact provenance | Internal database behavior unpublished in reviewed material | Raw provider/process capture, retained scratch work, compaction lineage |
| Restate | SQL over execution journal and application state | Keyed serialization and version-checked intervention | Permanent history or all bytes inside a durable step |
| SpacetimeDB | Transactional tables and subscribed query results | Reducer transactions; explicit durable visibility | Attempts/intermediate states outside committed mutations |
| Dolt | Queryable branched relational state | Transactions plus merge/conflict behavior | External-world rollback or an exhaustive event chronology |
| Letta Code | Versioned memory, active-context projection, explicit compaction entry | Git sync/conflicts; local sequencing in inspected path | Full transport audit or recovery of arbitrary live resources |
| Squirreling | Lazy structured/semantic analysis | Async query evaluation, not a shared-state protocol | Durable records, reproducible model inference, authoritative findings |

A durable database can faithfully preserve false, stale, or incompletely
attributed facts. A retrieval system can locate the right old text without
knowing whether another participant superseded it. A transcript can preserve
every displayed message while omitting the actual provider input. Continuity
requires enough retained evidence and explicit current-state meaning to resolve
these differences; storage persistence alone does not supply it.

## Comprehensive audit as the primary research surface

The operator asked for a core audit capturing everything for study and
improvement. None of the inspected mechanisms justifies weakening this into a
selection of milestones, debug messages, spans, or tool-result summaries. The
following are questions for the design, derived from that requirement and the
boundaries exposed above. They do not prescribe a database or event schema.

| Surface to account for | Evidence needed to study actual behavior |
| --- | --- |
| Provider interaction | Actual outgoing payload after context construction, resolved provider/model options, response chunks in observed order, available usage/error metadata, retries, cancellations, and final interpretation |
| Tool and process interaction | Actual arguments, stdin and control input, stdout/stderr bytes including partial output, launch context, lifecycle, signals, exit/result, truncation applied to model-visible output, and remote/local attribution |
| Context and compaction | Full retained pre-compaction material; selected summarizer input; summarizer exchange; policy/options; resulting summary; retained/omitted spans; exact next model input; pending messages and work at transition |
| Configuration and code | Requested change, resolved before/after values, author/initiator, source revision and actual running build identity, activation boundary, and which in-flight operations kept an older version |
| Shared work and files | Participant identity, what was read and from which version, concurrent writes and conflict outcomes, produced artifacts and actual bytes, and observations of changes outside the harness |
| Experiments and improvement | Parent run, hypothesis, candidate harness/config/policy, changed inputs, execution lineage, observed outcomes, interpretive judgments, and promotion/reversion actions |

Query indexes and references should locate retained original material, not replace
it with hashes or receipts. Derived projections can make queries cheap and model
inputs manageable without turning those projections into the only surviving
history. The key ergonomic question is whether both operator and model can ask
“what exactly did this participant see and cause under that version?” and reach
the underlying material without a special debugging workflow.

“Everything” still requires a stated observation boundary. Provider-internal
computation, unobserved external file writes, and physical effects outside a
tool's instrumentation cannot be conjured into a log. Buffer loss, disconnected
streams, truncated capture, and uncertain remote outcomes must remain visible
as coverage gaps; absence cannot silently become evidence that nothing occurred.
Ordering also needs honesty: per-stream order, causal order, local elapsed time,
and wall-clock labels answer different questions across concurrent participants.
The time/foundation lane should own that ordering design rather than letting a
database timestamp select it accidentally.

This is an audit for study, not a claim that replaying a log recreates the whole
world. Reconstructing current tables, reconstructing the next model request,
replaying a captured response, and reissuing a real external action have different
semantics. Analyses should name which one they performed.

## Rebuilding and inhabiting the changed harness

The new harness may resume an agent's identity, retained conversation, chosen
memory/context, known obligations, and experiment lineage. A persisted process
identifier does not restore the process; a stream identifier does not restore
its connection; a saved kernel description does not recreate its heap. The
continuity question is therefore what can be reattached, what can be reconstructed,
what must be restarted, and what must remain explicitly uncertain.

For in situ self-improvement, ask where ownership passes between old and new
harness versions while tools and peer models continue working. Which version
receives an arriving tool result? Can both mistake themselves for the owner?
Can the new version interpret old records and unresolved actions? What happens
if the candidate starts but cannot reconstruct the chosen context? What exact
source, build output, dependencies, and effective settings produced the behavior
being evaluated? A Git commit alone misses a dirty tree, a different executable,
or configuration changed after startup.

Restate's deployment pinning supplies one explicit answer for ongoing work;
Letta's persisted context supplies a different portion of continuity. Neither
inspected path demonstrates a model recompiling its host and seamlessly taking
over all live resources. That remains an Arconaut research problem. Keeping a
small substrate stable while replacing programmable control, restarting a whole
harness with resource reattachment, and migrating between running versions are
alternatives to compare against representative scenarios. No language choice or
hot-reload mechanism is selected here.

## Focused next investigations and direct oracles

These are candidate experiments for the research design, not authorization to
start product coding or a new task backlog.

1. **Two models and an operator revise one shared conclusion.** Interleave reads,
   writes, a contradictory observation, and a participant reconnect. Compare
   version rejection, transactional updates, and branch/merge approaches using
   the same intended invariant. The oracle is whether the final conclusion
   identifies the evidence and supersession actually available, with every
   conflicting attempt recoverable from the audit. A successful SQL commit is
   insufficient.
2. **Compaction with work still in flight.** Retain one input history and compare
   explicit summary policies while a tool result and peer message arrive. Ask a
   successor model to enumerate unresolved obligations and their sources. The
   direct oracle is the known set of obligations in retained evidence, with
   exactly which input each successor received available for inspection. Compare
   summary, retained-window, and on-demand access without changing the originals.
3. **Failure after an external effect but before recorded completion.** Use a
   controlled local effect with observable count and injectable failure points.
   Determine whether recovery duplicates it, loses it, or correctly reports an
   unresolved outcome. Independently compare captured byte streams to known
   emitted input/output to expose audit gaps. Avoid substituting crash-free
   execution for either oracle.
4. **Change turn semantics and then rebuild during collaboration.** Keep two
   participants and a long-lived process active while changing scheduling/context
   rules, then transition to a rebuilt harness. The oracle is attributable
   ownership and exact rule version for each accepted input/result, plus explicit
   disposition of every live resource and outstanding obligation. This directly
   compares the alternatives above without assuming universal hot reload.
5. **Autodroit analysis of an actual failure.** Start with one retained failure,
   let human and model query it, formulate a candidate change, and run a controlled
   comparison. Require the finding to lead back to original evidence and the
   candidate's actual executed code/config. The oracle concerns the fault being
   improved, not whether an evaluator emitted a favorable score. Preserve failed
   candidates as study material. Model-generated diagnoses remain hypotheses.

## Source inventory and acquisition suggestions

Temporary read-only research checkouts are under
`context/standing-state-frumentarii/`. Sparse checkouts contain only inspected
areas and Git metadata for this reconnaissance; they are **not** ingested
quarantine references. The parent owns clean acquisition and the manifest.

| Reference | Exact inspected revision | Acquisition value and limit |
| --- | --- | --- |
| [restatedev/restate](https://github.com/restatedev/restate) | `2180b55410f6512f8534012167aa536116d75c8a` (2026-09-29) | State CLI, SQL state schema, durability/version boundaries; source-available license must remain accurately described |
| [clockworklabs/SpacetimeDB](https://github.com/clockworklabs/SpacetimeDB) | `629e8c1e809a3d60bba7c863243a64d1e0dbb7d9` (2026-09-30) | Durability offsets, subscription visibility, reducer/state history; not a completed core audit |
| [letta-ai/letta-code](https://github.com/letta-ai/letta-code) | `96eb977b9477045423005a3630e5f9293140249a` (2026-09-29 local commit date; v0.34.0 bump) | Local transcript/compaction and shared-memory synchronization; TS runtime is a reference limitation |
| [letta-ai/letta](https://github.com/letta-ai/letta) | `5bcdd177d70fa2b31a754cfcd801e77b2e1ab16a` (2026-09-10) | README inspected to establish migration; no need to acquire an archived server merely because old surveys cite it |
| [hyparam/squirreling](https://github.com/hyparam/squirreling) | `249f5bb27ed4d685e13bd23bf67bfc8053117eab` (2026-09-28; v0.16.8) | Small query-planning and direct-counting-test reference; analysis algorithm only, not core candidate |

The first three active implementations offer the highest immediate source value;
Squirreling is a small, distinct optional analysis reference. Dolt and Claude
Science were examined through the cited current official documentation, not
implementation clones. The query-engine paper is a focused literature candidate;
its performance claims are not reproduced here and have not been independently
verified. No proprietary source or authenticated product was available in this
lane. A later inquiry into Science's internal database, raw audit exports, or
crash recovery should seek an actual interface specification or an observed
product trace, rather than extrapolating from its announcement.

No implementation, quarantine manifest, beads database, current design document,
or Git history was changed by this research lane. This report and temporary
research material are its outputs.
