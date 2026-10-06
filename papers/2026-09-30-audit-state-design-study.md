# Core audit and standing state: proposed contracts

**Later operator clarification supersedes the computational ownership premise.**
Kernels/databases/computation are potentially shared fabric services; Arconaut is
their consumer/customer. The resource-custody recommendation below predates this
correction and applies only to any actual harness-owned work. Audit of Arconaut's
own observed exchanges remains relevant; its ledger is not authoritative for
global service state, and refit does not pause shared services. See the
[current scope decision](../docs/FOUNDATION.md#discussion-decision-arconaut-consumes-the-computational-fabric).

2026-09-30. Design study for the integrated foundation. These are recommendations
for review, not an adopted implementation or a language decision. Read current
AGENTS, FOUNDATION, the updated frumentarii synthesis, and the standing-state
study. This study supersedes the earlier study's active-work rebuild experiment
for **arcorefit**: main-harness programs/provider requests settle first, managed
standing programs remain alive and paused, and independent OS daemons are outside
that pause scope. Hot policy changes during ordinary active work remain separate.

## Recommendation

Make durable audit admission part of ordinary execution, rather than an observer
that may fall behind and discard data. Recommend a small local service that owns
the authoritative append history, effect admission, and managed resource handles
across replacement of the main controller. Study SQLite as its first storage
candidate: original byte chunks and the corresponding event metadata can share
one transaction. Keep rebuildable analytical views outside the write-critical
path. Preserve originals independently of context selection, summaries, indexes,
and experiment outcomes.

This is a proposed process/storage boundary, not a proposal to place all workflow
or model policy in a compiled service. Runtime and language remain undecided.
The reason for a persistent owner is concrete: closing the controller must not
close the paused standing programs' descriptors or the recording path. The owner
must itself be replaceable under a separately demonstrated procedure; “small”
does not mean permanently exempt from autodroit.

The normal contract should be **record before dispatch; record before publication**.
It can guarantee that an admitted action and a published observation have
retained original material. It cannot guarantee that every external byte produced
before a power failure reached durable media, or that an external action and a
local database commit are atomic. Those are explicit fault boundaries, with
unknown outcomes retained as unknown.

## 1. What the core observes

The observation boundary must be an implemented interface, not the phrase
“everything in the session.” Proposed first boundary:

| Surface | Capture required at the boundary | Limit that must remain visible |
| --- | --- | --- |
| Participant ingress and egress | Exact message/input representation, sender and intended recipients, admission, delivery, consumption, and transformations | A participant receiving data does not mean its next model request contains it |
| Provider transport | Final outgoing request representation and actual body bytes after context construction; received bytes before decoding; exposed transport errors, stream boundaries, cancellation, usage, and parsed interpretation | TLS/network packet capture and provider-internal computation are separate scopes; opaque transport transformations cannot be labeled observed |
| Programs | Launch arguments/environment/cwd as used, stdin bytes, output bytes, terminal/control input, observed lifecycle/signals/results, handle identity and stream offsets | A subprocess's internal heap, arbitrary file reads, direct network calls, and uninstrumented descendants are not exposed merely because the harness launched it |
| Files and artifacts through core operations | Actual read/write bytes, path plus environment identity, operation outcome, and observed version/change | External writers and reads performed invisibly inside arbitrary programs are not a complete filesystem history |
| Definitions, configuration, context | Submitted bytes/values, resolved inputs, validation/load results, actual activated version, context selections and transformations | A source checkout is not the effective loaded runtime; unobserved dynamic loads need an explicit coverage limit |
| Experiment work | Candidate, intervention, execution, evaluator inputs/outputs, failed/abandoned variants, and resulting decision | A score or accepted edit does not establish causal benefit or generalization |

The provider adapter must own or expose the final serialization boundary. Capturing
an earlier SDK argument object is insufficient if later code changes headers,
body, compression, or framing invisibly. Record the representation precisely:
for example, original response body bytes plus observed header fields is not
the same claim as exact serialized HTTP frames. If a transport exposes final
plaintext protocol bytes, capture those too and retain the connection/stream
context needed to interpret them. A provider integration whose boundary cannot
be observed cannot claim complete raw-exchange coverage. Selecting the actual
HTTP/stream transport belongs to the provider design.

For program output, raw means bytes, including invalid UTF-8, carriage returns,
partial lines, and interleaved streams. A decoded display, merged terminal view,
line-oriented tool result, truncation, or model summary is a derived representation.
Capture the raw input to that representation and its policy. Preserve separate
stream order and explicitly observed multiplexing order; do not infer a global
execution order from timestamps or from whichever pipe was read first.

Ordinary computing remains expressive. A shell can run arbitrary software, but
its mediated stdin/stdout/lifecycle coverage must not silently become a claim
to have observed every side effect of that software. The audit records the code
and interface observations available at its boundary. Broader process tracing
would be a different feature with its own cost and platform contract.

## 2. Original history, operational state, and model context

Recommend three distinct meanings with explicit relationships:

1. **Original captured history.** Append events and original byte payloads.
   Corrections, supersessions, failed attempts, cancellations, rejected edits,
   and abandoned experiments append new facts. They do not rewrite old facts.
   Raw payload references resolve to retained bytes, not merely checksums.
2. **Authoritative operational state.** The current effect states, owner epochs,
   resource identities, policy versions, and admission status needed to execute
   safely and coherently. Change them in the same transaction as the audit event
   that explains the change. They are current state, not an alternate history.
3. **Derived views and context.** Search indexes, research annotations, summaries,
   rendered conversation, and each model's chosen input. Each carries its source
   cursor/versions and derivation identity. These may be rebuilt or superseded
   without removing the original inputs.

Research claims remain claims, with their author and supporting observations.
Changing a model's conclusion does not change what an earlier tool returned.
Database retention also does not make a retained assertion true or current.

An event needs enough identity to relate cause and use: journal identity,
durable sequence, actor/session, effect or resource identity, parent causes,
relevant definition/config/context versions, and payload references. A stream
has its own identity, byte offsets, and end/error state. Local durable sequence
orders commits to this store; it does not assert real-time order across external
hosts. Wall time and local monotonic observations remain separate. Exact types
and clock semantics should come from the integrated time design.

The same programmatic operations should serve human tools and model workflows:
query current state at a stated cursor; inspect an effect and its original
exchange; read a byte range; follow events after a cursor; propose a state
transaction against an expected version; and inspect the resulting conflict or
new version. A human CLI is another client of those operations. This introduces
no command-by-command approval step.

Operational mutations go through the versioned mutation surface, including
expressive multi-field transactions; read/query surfaces can remain general.
Unrestricted SQL updates to historical rows would violate the append contract.
Read-only SQL over original records and projections is useful. Analytical views
must expose their applied cursor and definition version; a client requiring a
newer cursor waits or gets an explicit not-yet-materialized result. Never return
a stale projection as authoritative current execution state.

Capture operator/model query requests, errors, and the actual result representation
emitted at the core interface, with a finite source cursor. A SQL string plus a
current view name is insufficient when view code or data later changes. Existing
immutable byte chunks may be referenced without copying them. Keep the recorder's
internal persistence steps distinct from participant actions: appending an event
must not recursively create another event about that append. Live audit watches
also need an explicit rule excluding their own delivery bookkeeping from their
automatic feed; that bookkeeping remains available in finite historical queries.
Otherwise studying the audit can itself create an unbounded observation loop.

Restate's inspected state mutation request includes an expected version; it is
useful prior art for this optimistic edit contract, not evidence that our proposed
storage already implements it.
[Pinned mutation utility](https://github.com/restatedev/restate/blob/2180b55410f6512f8534012167aa536116d75c8a/cli/src/commands/state/util.rs).

## 3. Admission, dispatch, and uncertainty

Use one operation identity across client retries, and a distinct attempt identity
for each actual dispatch. An idempotent local submission is not a promise of an
idempotent external effect.

Proposed normal path:

1. Receive a proposed action with its causal/context references, intended
   configuration version, and client operation identity. Validate against the
   current operational state. Append rejected proposals as well as accepted ones
   whenever recording is healthy.
2. Atomically retain the proposal and its required original inputs, advance
   authoritative state, and acknowledge **admitted** only after durable commit.
   Repeating the same operation identity returns the existing result; reusing it
   for different content is a recorded conflict, not a replacement.
3. Before an external attempt, retain its final resolved request, including
   credential-bearing representation under the protected-record policy below,
   and an **attempt-open** transition. Dispatch only after that commit succeeds.
   Provider retries, redirects that cause another request, stdin writes, signals,
   and process launches each need the applicable attempt record.
4. Capture returned bytes/outcomes and publish them to ordinary consumers only
   after durable capture. A write return or process status is an observation;
   it does not imply the remote application completed the intended action.
5. Preserve partial results and failure. Final completion records distinguish
   observed success, observed failure, cancellation, and unresolved outcome.

If the controller disappears after admission but before **attempt-open**, the
service can still know no compliant dispatcher started that attempt. Once
**attempt-open** is durable, a crash before the outcome leaves uncertainty: the
effect may not have started, may have partly happened, or may have completed.
Do not infer failure from missing completion and automatically replay an arbitrary
effect. Reconciliation or a permitted retry is a new recorded decision. The
execution design may offer operation-specific retry policies, with the provider
or program's actual guarantees stated.

Durable publication can be batched by bytes/time without losing original chunk
identity. A batch becomes visible after its commit, not after entering an in-memory
queue. Exact batch limits and latency are feasibility variables, not invented
performance guarantees. Status may distinguish received/queued from committed,
but ordinary model context must not depend on uncommitted observations. A storage
failure status itself is necessarily an exceptional, potentially unrecorded
control observation; it must be labeled as such.

There is an irreducible gap between an OS/network read and its durable write.
A host failure in that interval can lose bytes already emitted by the source.
Without a cooperating replayable source, the receiver cannot reconstruct them.
The guarantee is therefore complete retained material for acknowledged capture,
and truthful coverage status for interrupted capture. A crash with an open stream
makes the unconfirmed tail uncertain even if the database is internally sound.
This does not meet a claim of zero-loss all-world capture, and must never be
advertised as doing so.

## 4. Capacity and recording failure are execution states

Recommend bounded memory queues and configured storage thresholds. Queue limits
are backpressure limits, not sampling budgets. The system must not silently
drop old raw chunks, shorten retained tool output, or discard failed trials to
stay responsive. Model-facing truncation is permitted only as a derived view.

| Condition | Required ordinary behavior | What remains uncertain or exceptional |
| --- | --- | --- |
| Writer temporarily slower than producers | Stop admitting additional work at a high-water mark; apply bounded backpressure; keep already captured bytes queued until committed | Kernel/network buffers have finite capacity; upstream may block, time out, or disconnect |
| Low free space or excessive WAL growth | Stop new effect admission before exhaustion, settle current work where possible, and pause managed standing producers; report the condition | Thresholds buy time, not a guarantee against another process filling the disk |
| Commit fails, including full disk/I/O error | Acknowledge no success; expose no affected observation as durable; enter audit-unavailable state; resolve database transaction outcome before retry | Do not assume a failed statement means the entire transaction vanished |
| Writer stalls/disconnects while resource owner remains alive | Controllers stop new normal dispatch; the live owner stops reading beyond bounded capacity and pauses producers where possible | An open provider stream may finish remotely or disconnect; unseen bytes/outcomes stay unknown |
| Combined audit/resource-owner process crashes | Surviving controllers stop new normal work and use only available emergency controls; recovery reconciles actual resources and open attempts | A dead owner cannot pause children or retain descriptors; resource survival and unrecorded output depend on the actual process arrangement, and must not be promised |
| Projection/query engine fails or lags | Keep original recording running; report the view unavailable/behind; rebuild from originals | Execution must consult authoritative state, not the failed projection |
| Decryption key unavailable | Permit suitable metadata inspection; block new protected capture and corresponding normal work | Existing ciphertext still exists but original plaintext is unavailable until the key returns |
| Recovery after unclean stop | Recover storage, identify open streams/attempts, append the known gap/recovery facts, and reconcile before resuming affected work | Missing end markers identify uncertainty; they cannot invent missing bytes or exact gap length |

Emergency pause, cancel, or disconnect must remain possible when the audit cannot
write. Otherwise recording failure could trap a verbose or destructive program
in continued execution. Attempt to record the intervention; if that fails, show
an explicit unaudited emergency action and preserve available details for later
recovery. The later record must state that it is retrospective. This is a narrow
degraded control path, not permission to continue ordinary unaudited work.

For resident standing programs, the default fault response preserves them and
attempts confirmed pause. It does not terminate them as a recording or refit
strategy. If the relevant process cannot actually be paused, mark that condition;
the system cannot claim quiescence. An operator-directed termination remains a
distinct action. OS daemons that the operator manages independently are not
silently pulled into this lifecycle.

No extra emergency log or chain of receipts is proposed to certify the main log.
A reserve of disk capacity can improve the chance of orderly stopping, but no
finite reserve cures failed media. Honest failure states and direct completeness
tests are the relevant mechanisms. Automatic deletion of original history is
not an acceptable capacity strategy under the present requirement. Retention
changes need an explicit design/operator decision; a deletion marker cannot
substitute for the deleted study material.

## 5. Plausible local storage and process arrangement

Recommend studying one persistent local owner with a single authoritative writer
and a narrow framed IPC interface. It holds managed process/terminal handles and
accepts records from the controller and provider adapters. It can delegate slow
analysis to another process. Its role is capture, lifecycle/admission enforcement,
and state ownership; turn policy, planning, context strategy, and experiment
selection remain replaceable control behavior.

For a first local storage design, propose:

- An **audit database** containing append-only event rows and chunked original
  payloads, together with the small mutable operational ledger. Commit each
  semantic transition and its payload references atomically. A large stream
  grows through committed chunks; a later close record identifies its observed
  end. Partial streams remain useful original evidence.
- A **derived state database** for convenient SQL views, search, and annotations.
  Apply committed source events in order and store the applied cursor in the
  same transaction as the derived change. Rebuild it from originals when its
  projection code changes. Its successful update is not a condition for capture.
- Protected raw payloads encrypted before entering SQLite, so temporary database
  pages and its WAL do not receive their plaintext. Plain metadata indexes should
  be deliberately bounded in meaning; sanitized display text belongs to derived
  views. A value accidentally copied into metadata would fall outside that
  protection and is an implementation fault to test.

SQLite is a candidate because a single transaction can bind the event, complete
payload chunks, and operational state. This avoids an initial two-store protocol
where an append log commits but an external blob is missing, or the reverse.
This is a design judgment, not a performance result. Very large-volume capture
may eventually justify segment files, but that would require an explicit joint
publication/recovery protocol rather than assuming file existence means commit.

The official WAL documentation supports concurrent readers and one writer on
the same host, warns against network-filesystem WAL use, and explains how long
read transactions can obstruct checkpoints. Therefore put prolonged analytical
queries against the derived store or a bounded snapshot, and keep raw-store
inspection transactions short. The two databases need no cross-file atomic
transaction because the derived one is rebuildable.
[SQLite WAL](https://www.sqlite.org/wal.html).

Propose `WAL` with `synchronous=FULL` for durable admission, with the actual VFS,
filesystem, and platform flush behavior checked on supported hosts. `NORMAL`
has a weaker power-loss durability contract. On macOS the `fullfsync` option is
distinct from the synchronous setting and needs deliberate evaluation. Verify
the effective settings rather than assume requested pragmas took effect.
[Synchronization pragmas](https://www.sqlite.org/pragma.html#pragma_synchronous),
[sync flags](https://www.sqlite.org/c3ref/c_sync_dataonly.html).

SQLite reports that disk-full and I/O errors can roll back a statement or the
entire transaction. The writer must check the resulting transaction state and
establish a known boundary before retrying; blindly retrying the last statement
can produce the wrong operational history.
[Transaction errors](https://www.sqlite.org/lang_transaction.html),
[autocommit state](https://www.sqlite.org/c3ref/get_autocommit.html).

Pin and inspect a current fixed SQLite build rather than trusting an unspecified
system library. Official release notes document a WAL-reset corruption fix in
2026; the examined 3.53.3 release includes it. This study did not inspect or run
that SQLite implementation and does not claim that release has no remaining
faults. Back up through a database-aware consistent snapshot, not by copying a
live main file while ignoring its WAL. Backup is recovery material, not another
receipt layer.
[Release 3.53.3](https://www.sqlite.org/releaselog/3_53_3.html),
[backup API](https://www.sqlite.org/backup.html).

## 6. Credentials and original bytes

Provider authentication creates a real tension: exact outgoing requests can
contain live bearer tokens or other credentials. Routine redaction before
capture would destroy the original and silently weaken the audit requirement.
Conversely, dumping raw credentials into every model query/display is unnecessary
for studying most behavior. Resolve this through retained originals and explicit
views, not by calling a redacted record raw.

Recommend reversible, authenticated encryption for **all raw payload records**,
with key material protected separately under the operator's local authority.
This avoids relying on a perfect secret detector to choose which bytes deserve
protection. Bind the envelope to its journal/record/stream identity as associated
data; use a reviewed library construction with its nonce/key rules, not an
invented cipher or log-signature scheme. Standard AEAD provides the relevant
confidentiality and integrity primitive; it does not itself establish capture
completeness or key recoverability.
[Libsodium AEAD documentation](https://doc.libsodium.org/secret-key_cryptography/aead).

The exact plaintext request, including credentials, then remains recoverable
as original material. Ordinary query/display projections can suppress credential
values while retaining the raw record reference and the transformation's identity.
Raw inspection should be an explicit operation under standing authority available
through the same human/model interface, not a new approval prompt for each action.
Its access and any export become audit events where recording is healthy.

Credential rotation does not require destroying old raw audit records. Audit-key
rotation must preserve the ability to decrypt earlier records unless destruction
is deliberately chosen. Losing or deleting a key is loss of original accessibility,
not successful retention merely because ciphertext remains. If the operator
chooses credential omission instead, record the policy/version and precise scope
of omission, label those exchanges incomplete at that boundary, and do not claim
complete originals. No such omission is selected by this study.

Key management, raw inspection defaults, and export behavior need focused review.
No credentials were read or used in this study. Encrypting original payloads does
not solve endpoint compromise or an authorized process observing its own request;
those are different threat boundaries from accidental disclosure through audit
queries and stored files.

## 7. Compaction, loaded definitions, and experiments

Compaction is an operation with a retained input selection, strategy/version,
parameters, actual summarizer exchange where applicable, output, and activation
boundary. Preserve the entire previously captured context history, including
material excluded from the summarizer input. Record which messages/byte ranges
were selected, truncated, externalized, or omitted from the next model request.
The next request must refer to the actual adopted context version. A background
memory consolidation is another attributed operation, not an invisible edit.

Likewise, a definition change records source bytes, resolved configuration,
compile/load input and output, success/failure, and its point of effect. A failed
candidate remains in history. A live replacement may leave old frames or captured
definitions active; an operation must identify the governing version rather
than simply inherit the latest source revision label. Preserve actual owned
executable artifacts and build inputs required by the adopted build contract.
Do not claim to have captured every system dependency read invisibly by a build
tool without the corresponding observation mechanism.

Letta's inspected compaction path demonstrates separating active context IDs from
an appended compaction event, but its transcript alone does not establish all
the original input/transport retention required here.
[Pinned compaction persistence](https://github.com/letta-ai/letta-code/blob/96eb977b9477045423005a3630e5f9293140249a/src/backend/local/local-store.ts).

For autodroit, link each intervention to its parent run, changed definitions,
effective loaded artifact/configuration, actual task/evaluator input, observations,
and decision. Keep counterproductive and abandoned trials. Changing the recorder
or evaluator is itself an intervention and may change comparability. Annotations
can be wrong and revised; the underlying observed material must remain available
to question the annotation. No automatic keep/revert criterion is selected here.

## 8. Refit and replacement continuity

Ordinary arcorefit should leave the local audit/resource owner alive. Proposed
audit-visible boundary sequence:

1. Commit closure of main-harness admission at a named epoch. Finish or settle
   existing main commands/provider requests according to the integrated policy.
   Confirm standing programs paused through their actual owner, with their
   live identities/descriptors retained. A requested pause is not a completed
   pause. Drain and capture already available output without pretending that
   an open stream has reached its final end.
2. Commit the quiescent main state, outstanding known/unknown obligations, context
   version, paused resource inventory, and conversation ownership transition.
   The independent refit work supplies its conversation/build records under its
   own identified producer. Its interface and remote topology are outside this
   study; they must not depend on a dead main controller for recording.
3. Rebuild/restart the main controller. The successor presents its actual running
   artifact/protocol version, reads authoritative state, and reattaches existing
   resource handles by identity and ownership epoch. It does not recreate a
   standing program merely because its old name appears in a database.
4. Commit acceptance of the successor and the returned context/conversation
   ownership. Resume preserved standing programs only after that transition
   is durable and the successor can receive their recorded observations. Reopen
   normal main admission at the intended boundary.

The owner rejects commands from an old controller epoch. A failed build or failed
successor startup keeps the main paused and leaves the independent refit work
available for diagnosis. Audit data remains queryable. Refit is not an implicit
retry of unresolved external effects and not a reason to terminate standing work.
Existing externally managed OS daemons continue outside the pause inventory.

The persistent owner is a temporary continuity anchor, not an immortal component.
An owner upgrade needs a separate protocol because it holds the database and OS
handles. One plausible local Unix mechanism to investigate is same-process image
replacement at a quiescent boundary: deliberately preserved descriptors can
survive `exec`, while application heap and runtime objects do not. Reopen SQLite
normally; do not treat an inherited database connection object as usable state.
Pass a versioned descriptor/state inventory, restore signal/runtime behavior,
and verify the successor recognizes the still-paused resources before admitting
work. POSIX specifies descriptor survival except close-on-exec descriptors, but
that narrow rule does not establish an entire correct handoff.
[POSIX.1-2024 exec specification](https://pubs.opengroup.org/onlinepubs/9799919799/functions/exec.html).

Whether this works on the intended macOS/Linux process arrangements remains an
unrun feasibility question. Until demonstrated, owner upgrade must report that
boundary unsupported; it must not fake continuity by relaunching resident programs.
Power loss or loss of the actual resource-owning process is a different failure
class from replacing a controller while the owner stays alive.

## 9. Direct oracles and remaining decisions

No experiment, test, provider request, installation, or mutation was run. These
are the direct checks the integrated plan should use, after design review:

| Contract | Direct oracle |
| --- | --- |
| Raw completeness | Controlled emitters produce known byte sequences across stdout/stderr, stdin, provider fragments, invalid text, and partial failures. Compare all retained original bytes and stream identities to what the emitter actually sent; do not derive the expected result from the recorder |
| Durable admission | Inject failure before/after each actual commit and dispatch boundary. Independently count external effects; verify no normal dispatch precedes its durable attempt input and unresolved cases remain unresolved |
| Backpressure/failure | Deliberately stall/fill/fail the recording backend. Observe bounded memory, stopped admissions, actual producer pause/backpressure, explicit emergency behavior, and retained originals after recovery |
| Query meaning | Use a known concurrent state-change history; require current reads/expected-version writes and projection cursors to match their stated boundaries. Rebuild views and compare their intended semantics |
| Context/definition lineage | Give a later action a context/code version differing from the latest source. Recover exactly which retained inputs and effective definitions governed it, including unsuccessful transformations |
| Credential handling | Use synthetic credentials; decrypt retained raw exchange to the exact emitted input while ordinary projections apply their stated transformation. Exercise missing/rotated keys without replacing loss with a completeness claim |
| Refit continuity | With main requests settled and standing programs confirmed paused, replace the controller and reattach the same live resources. Compare identities, output offsets, context ownership, audit continuation, and actual running artifact |

Remaining decisions are concrete: supported capture transports; local filesystem
and durability fault domain; batching/backpressure limits; how managed programs
are actually paused; credential key lifecycle; initial query/schema/versioning
surface; and whether a tested owner replacement mechanism is available. These
must be resolved without shrinking audit to application debug logs or insisting
that ordinary work wait for user approvals.

Read Rhizome's [C++ rigor proposal](../../rhizome/papers/cpp-rigor-stack.md) at this
checkpoint. Transferable evidence is its explicit ownership/publication contracts
and direct persistence-seam fault oracles. Its C++ preference, component scope,
and library-adoption rules are not imported into Arconaut. No language choice
follows from this reference.

Official SQLite, cryptographic-library, and POSIX material was checked narrowly
to resolve storage and replacement contracts. POSIX search exposed the current
descriptor rule; a direct page fetch returned HTTP 403, so no stronger claim is
made from an uninspected full platform implementation. Pinned Restate and Letta
paths are the previously inspected mechanism references, not adopted dependencies.
Only this study file was written by this lane.
