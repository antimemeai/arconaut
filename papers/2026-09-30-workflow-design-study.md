# Programmable turns, workflows, and model ergonomics

**Later activation decision supersedes the proposed next-decision default.**
Program/configuration changes normally take effect after the current turn or affected
workflows conclude. An explicit combined interrupt-and-apply-now action brings them
forward. See the [current intent](../docs/FOUNDATION.md#discussion-decision-defer-changes-until-current-work-concludes).
Source mechanisms and their limits below remain evidence, not adoption of the older
activation proposal.

**Later operator clarification narrows the ownership assumptions below.** Arconaut
consumes kernels/databases/computation potentially shared by many Arconauts; future
meta-project fabric governs them. Standing workflow/kernel state in a consumed
service follows that service's lifetime, not Arconaut refit pause/custody. Shared
operations and context/definition semantics remain useful research proposals;
service-client/workload identity and reconnection need specification. See the
[current scope decision](../docs/FOUNDATION.md#discussion-decision-arconaut-consumes-the-computational-fabric).

2026-09-30. Focused design study; proposed contracts for review, not an approved
implementation plan. This develops the [current foundation](../docs/FOUNDATION.md),
[synthesis](2026-09-30-frumentarii-synthesis.md), and
[workflow reconnaissance](2026-09-30-programmable-workflows-frumentarii.md).
Read the active instructions, doctrine, README, journal, systems study, and
Rhizome's current C++ rigor proposal; reconciled the emerging
[behavioral brief](../docs/BEHAVIOR.md). Only this new paper is changed.

## Recommendation and its limits

Use **one shared operation model**, accessible through programs, CLI commands,
interactive views, and model tools. Recommend ordinary Python as the first
model-facing workflow binding and an optional independent execution host.
That does not select Python as the main policy/controller language. Keep
resource ownership, admission, pause/reattachment, and audit capture outside
arbitrary user workflow code. A blocking cell must not block those controls.

The evidence supports Python as an economical first surface to examine:
CodeAct and RLM investigate executable composition, Prime exposes usable Python
operations, and Python directly serves the operator's scientific/programming
work. There is **no comparative model-ergonomics evidence here establishing that
Python is superior to Lisp, Scheme, or a particular typed tool interface**.
The systems study makes live compiled SBCL a strong main-controller candidate:
it can compile and load definitions without rebuilding the entire process.
A Lisp controller with a thin native resource/audit owner and independent
Python workflow/kernel hosts is consistent with this paper's contracts.
The model and operator must both be able to inspect, compile/load, and activate
the **real controller policy**, not merely compose canned Python wrappers.
Preserve the shared operation contract rather than choose the whole runtime
from language familiarity. The first direct ergonomics trial should include
editing that controller as well as invoking its Python binding.

Recommend that a turn be an executable control program with named activation
boundaries. It can branch, loop, invoke different models, observe programs,
receive messages, race results, suspend, and return. A reusable workflow is
actual code performing those operations. Neither a static DAG nor one mandatory
REPL call is the product model. Long-lived state and services are available to
these programs without being owned by a particular model response.

The important constraint is honest applicability: changing a definition affects
future dispatch through that definition; changing a captured request or a
suspended stack requires a different, explicit operation. Routine changes need
no approval ceremony. They do need observable effects.

## Source evidence that constrains the proposal

The earlier report supplies full source pins and inspection detail. This study
additionally read or re-read message/wait APIs, workflow combinators, compilation
code, and primary language documentation. No reference code, tests, provider
requests, or experiments ran.

| Evidence | Consequence for proposed semantics |
|---|---|
| Prime `pa-agent/src/agent.rs:557` copies continuation/model configuration into a run; its continuation setter is separate. [Source][prime-config] | A mutable setting is not evidence that the running controller sees a change. Record the activation decision. |
| Prime REPL protocol permits background output after a cell and admits unattributed raw fd output. [Protocol][prime-protocol] | Operation completion and lifetime of spawned work/streams must be distinguishable. |
| Prime's compaction probe calls `prune_oversized_variables`. [Source][prime-prune] | Context reduction must have an explicit independence contract from standing program state. |
| Dagger identifies mailbox messages independently, allows step-boundary steering, and rejects known cyclic agent waits. [Messages][dagger-messages], [waits][dagger-waits] | Sending need not block the sender or wait for a target's final response. Delivery, context inclusion, and reply correlation need distinct meanings. |
| Dagger reseed requires a parked loop; composition reload checks identity and preserves or explicitly resets tool state. [Reseed][dagger-reseed], [reload][dagger-reload] | Replacing conversation, implementation, and mutable state are distinct operations. |
| Restate's `wait_completed` returns completed then uncompleted futures, while its concurrent-greeter example assigns them in reverse. [Implementation][restate-wait], [example][restate-example] | Even a small conventional API can invite a consequential interpretation error. Prefer named result fields and test comprehension with real tasks. This is a source disagreement, not a locally reproduced failure. |
| Slynk's SBCL backend compiles submitted source and loads a returned output file into the live runtime. [Source][sly-compile] | Compiled live change is concrete. The path does not establish transactional load, atomic activation of several definitions, or rollback of load-time effects. |

SBCL's primary implementation notes describe runtime class, generic-function,
and method redefinition. This strengthens the case for studying a live compiled
controller while retaining explicit old-frame and state-migration boundaries.
[SBCL CLOS implementation notes](https://www.sbcl.org/sbcl-internals/Discriminating-Functions.html).

Python's documented `reload` recompiles/reexecutes a module but retains its
namespace; old names may remain, references imported elsewhere are not rebound,
and existing class instances keep the old class. Reload is not thread-safe.
Thus indiscriminate module reload can produce a mixture of old objects and new
globals, rather than a clean version transition.
[Python 3.14.7 importlib documentation](https://docs.python.org/3/library/importlib.html#importlib.reload).

Python tasks run cooperatively. `Task.cancel()` requests an exception and does
not guarantee termination; a coroutine may suppress it. `asyncio.wait()` returns
pending work on timeout and does not cancel that work. These distinctions support
separating observation, cancellation request, and confirmed completion.
[Python task documentation](https://docs.python.org/3/library/asyncio-task.html).
Jupyter also documents output arriving after a request's idle event, so an
execution-complete indication cannot certify that no related output remains.
[Messaging specification](https://jupyter-client.readthedocs.io/en/latest/messaging.html#kernel-status).

Rhizome's [C++ rigor study](../../rhizome/papers/cpp-rigor-stack.md) reinforces an
applicable method: define ownership, publication, failure, and representation
contracts before selecting checks. It does not select Arconaut's language.
Here, the analogous fault classes are stale definition dispatch, lost messages,
misattributed results, premature pause acknowledgement, and erased captured
material. An operation-count metric would not directly detect those faults.

## Shared human/model operations

The following names illustrate semantics, not a final SDK spelling. The same
submitted operation has the same identity, arguments, version, environment,
observable result, and failure whether issued by a model, human CLI, or script.
Authority comes from the operator's standing configuration, not from repeatedly
asking who clicked the button.

| Operation family | Proposed contract |
|---|---|
| Discover/describe | Find available operations by purpose and inspect arguments, return type, effect/lifetime, environment, and current definition. Supply concise examples and discoverable detailed documentation. |
| Start/invoke | Admit a concrete operation, return its handle, and identify the definition and inputs actually admitted. Short calls may offer a wait convenience over the same operation. |
| Inspect/observe/read | Inspect current status or read retained results/output by cursor or range. A preview is explicitly bounded and points to the full captured material. Observation creates no duplicate execution. |
| Wait/select/race | Wait for specified conditions on existing handles. Return named completed/pending members and the actual condition that resolved the wait. A timeout is an observation outcome. |
| Send/subscribe | Send to a participant or room and observe future messages/events. Sending returns after acceptance, independently of target idleness or response. |
| Interrupt/cancel/pause/resume | Request the named lifecycle change and separately observe whether it happened. Scope is explicit: operation, workflow, participant, or standing resource. |
| Define/activate | Load executable definitions and activate a version at a declared boundary. Return pending/active/rejected with affected targets and actual activation point. |
| Context transform/activate | Produce and activate a new context version from a recorded source, with a strategy and complete captured inputs/outputs. It does not delete standing state. |

An operation descriptor should contain enough information to make useful choices;
it should not force every model call through a schema-discovery handshake.
Dagger's mandatory method selection is useful opposing evidence, not a rule to
copy. Keep common actions readily available, with deeper discovery as needed.
The actual tradeoff between initial context size and discovery burden needs
measurement on the target models. [Dagger model interface][dagger-tools].

Use handles for ongoing or large objects, with a short readable label alongside
the stable identity. A handle identifies an **instance**, not a promise that its
current status or content is unchanged. It names its owning environment and
resource generation; an unavailable or stale handle must never silently resolve
to a new object with the same label. Reattachment recovers access to that instance.
Recreation creates a successor identity and states the relationship.

An operation record distinguishes admission, execution status, result, and
knowledge about external effects. Failure can include partial output and an
unknown external outcome. Avoid a single boolean that makes “transport broke”
look like “nothing happened.” Errors identify the failing stage, relevant
argument or object, retained diagnostic/output material, and what can be done
next. Python exceptions, CLI exit status plus structured output, and model tool
errors can present this same information differently.

Retrying an uncertain submission should support lookup by the original request
identity before creating another operation. Reusing an identity concerns that
submission only; it is not result caching by argument equality. A deliberate
second invocation of the same command receives a new identity. Neither contract
promises exactly-once behavior of an arbitrary external effect.

Waiting is observation by default. Cancelling a waiting client does not cancel
the observed job. A race returns the winner/condition and named pending members;
its default leaves the pending work alive. A workflow may explicitly request
cancellation of losers or use a reusable helper with that policy. Finishing a
turn does not implicitly terminate standing work. Ownership and cancellation
groups are selectable without becoming a command-by-command approval flow.

Outposts fit as environments advertising capabilities; some may run peers, and
some may offer a capability without an active model. A search provider is simply
a discoverable operation with its actual execution environment and result
attribution. No separate outpost architecture is needed to settle this contract.

## Live peer messages while other work continues

Participants have independent contexts and control programs. A room supplies
addressing and shared message history; it does not imply one shared model
context, a parent/child execution tree, or synchronized filesystem mutation.
An operator can speak while models and programs are busy.

Proposed message contract:

1. **Accepted** means the receiving service has taken responsibility for the
   message under the audit/storage contract. Return a message handle immediately;
   a busy target is not a rejection condition. Backpressure or disconnection is
   reported explicitly rather than described as delivery.
2. **Available** means the addressed participant's controller can observe it.
   A UI may already display it. This does not mean an active provider request
   has seen it.
3. **Included** identifies the particular context version/provider request or
   controller decision that incorporated the message. Avoid claiming that a
   model understood or acted on a message merely because it was included.
4. Replies are messages with explicit `reply_to` links. A participant may issue
   several replies, combine answers, or never reply. The final output of a
   turn is not implicitly the unique answer to every message it consumed.

Each recipient has a local admission order sufficient to identify a mailbox
selection. A controller selects explicit message identities from that mailbox;
arrivals after its selection remain pending. Do not clear an entire queue after
preparing a context, because that loses concurrent arrivals. Multi-recipient
delivery retains per-recipient facts; it need not pretend simultaneous arrival
or a universal execution order.

At a scheduling boundary, the program may append available messages to the
next request, change its next action, launch another participant, or explicitly
interrupt an active request. A message cannot retroactively change the bytes
already sent to a provider. If a provider has no mid-request steering operation,
immediate steering means cancellation plus a new request, with both attempts
retained. Default acceptance should not automatically incur that interruption.

Blocking on a peer response must not disable mailbox/control processing.
Reject known self-waits and cyclic waits over harness-managed participants, as
the inspected Dagger path does; return the actual dependency path. Arbitrary
external I/O can introduce dependencies the harness cannot know. That limitation
does not justify claiming universal deadlock detection.

Workspace changes remain attributed to the acting participant and environment.
A message reporting a file's value is an observation at a particular point;
another participant may already have changed it. Shared chat does not make
edits serializable. The storage/workspace study owns the eventual conflict
mechanism; this interface must retain enough identity to express it.

## Executable turn and workflow changes

A model request is one operation. A **turn** is a programmable work episode,
identified for observation and operator intervention, whose program chooses
model calls, context, tools, waits, delegation/peers, compaction, and termination.
One turn can contain several concurrent model requests; a participant may also
have standing workflows independent of a conversational turn.

Propose two complementary forms over the same operations:

- Ordinary executable workflows can freely branch, iterate, await inputs, and
  race work. Their existing frames retain their code unless explicitly migrated.
- A hot-redefinable turn controller exposes a named decision entry point and
  explicit state. After a decision yields, the next dispatch resolves the
  currently active definition. It can itself compose ordinary workflow functions.

This is not a requirement to express every workflow as a graph or a one-step
DSL. The controller boundary exists to make live applicability truthful.
Implementations can supply comfortable helpers without hiding its behavior.

**Decision boundary** means the old policy invocation has yielded/returned,
its effects have been admitted or rejected, and the next invocation has not
begun. Already-running external operations may continue across this boundary.
If a program is stuck in synchronous code, the requested activation remains
pending; the independent owner can still inspect, interrupt, or replace that
controller. A timer reporting “reloaded” cannot substitute for reaching the
boundary.

| Change | Proposed first point of effect | Existing work |
|---|---|---|
| Turn scheduling/continuation program | Next declared decision dispatch, after successful load and compatible state handoff | Existing requests keep their captured definitions/context; new policy receives their identified outcomes |
| Model selection or request construction | Next request admission | A sent request keeps its model, options, input bytes, and protocol bindings |
| Tool implementation/schema | Next binding selection for a new request or explicitly versioned direct invocation | A tool call produced from an earlier request retains its original binding identity; do not reinterpret its name under a new schema |
| Reusable function definition | Next lookup through its named versioned entry point | Existing frames/captured function objects remain old unless explicitly migrated |
| Mutable tool/controller state schema | Declared migration boundary, after conversion succeeds | Failed conversion leaves the old instance usable; no silent default-state reset |
| Context selection/compaction | Context activation before a subsequent request is admitted | Existing provider request retains its original context; queued messages remain pending |
| Native executable replacement | Refit after main-harness quiescence and confirmed pause of standing work | No active main command/provider request is carried through refit |

Tool-name binding deserves particular care. Suppose request R saw `score()`
returning a value from 0 to 10, but the tool is changed to 0 to 100 before R
returns a call. The result cannot silently acquire the new interpretation.
Retain R's binding, or reject that call explicitly if the old definition is no
longer executable; the current turn policy may choose to discard/replan it.
Fresh policy does not authorize historical reinterpretation.

Load definitions as new versions before publication. Prefer a fresh definition
namespace with explicit dependencies and separately owned state to an unrestricted
`importlib.reload` of the live module. Retain the loaded source, dependencies,
and effective configuration required to interpret the execution. Publication
changes a target's current version; it does not erase the previous version or
overwrite a captured reference. Competing human/model edits should name the
base version they intend to replace so a later edit does not silently overwrite
an unseen change.

This is also a contract to implement for Lisp; ordinary `LOAD` and live CLOS
redefinition do not automatically provide atomic candidate publication. A state
conversion must construct replacement state without mutating the committed
instance before success. External resources remain separately owned handles.
Arbitrary load-time or migration side effects cannot be undone merely by
restoring the old entry-point pointer, and must not be described as rolled back.

For a suspended workflow that needs different control flow now, support a
declared migration point carrying explicit workflow state and outstanding
operation handles. The new continuation takes over those handles and that
state; it does not replay completed effects to reconstruct a convenient stack.
An arbitrary Python `await` is not automatically such a point. If a program has
no supported migration point, say that the edit applies to future invocation;
the operator can arrange a yield, finish it, or explicitly restart work.

## Compaction creates a context version

Separate four things: captured original material, a participant's active context,
standing computational state, and query/display projections. A context version
is a particular ordered selection/transformation of retained material for one
participant. A provider request captures the actual serialized input as well as
that version's identity. Kernel variables, program instances, database state,
and artifact handles live independently of the context selection.

Proposed operation: `transform_context(base, strategy, options)` produces a
candidate context version and a report of retained, omitted, summarized, or
externalized material. Strategies can include selective context reduction,
model-written summary, rebuilding from chosen evidence, or a new context branch.
They share a contract, not necessarily an algorithm. A summary operation that
calls a model is itself ordinary recorded work.

Activation occurs before a subsequent request, against the expected base
context. If the participant's context changed while the transform ran, do not
overwrite it with the stale candidate. Either retain the candidate as a branch,
retry on the new base, or apply an explicit transform preserving the intervening
material. Mailbox messages accepted meanwhile remain available; neither context
activation nor compaction implicitly marks them included.

Both operator and model can inspect strategy, source context, actual transform
inputs and output, and activation status. They can activate a chosen candidate
or return to an earlier context for subsequent requests. Returning does not
undo tool effects or erase the intervening execution. A required pending
tool-call/result pairing must remain interpretable under the provider protocol;
do not compact a conversation into structurally invalid messages.

**Compaction never implicitly deletes or mutates persistent kernel or standing
state.** A memory cleanup/reset is a separately named operation with its own
effects. Large values may disappear from the active model input while their
handles remain discoverable and the values remain alive. Restoring a context
that names an unavailable resource must surface that fact rather than fabricate
the resource. Snapshot size limits are not context-retention policy.

All complete captured originals remain independently available, including
abandoned candidates, failed summaries, request attempts, and the exact
transformation inputs/outputs. Truncated model previews point to retained bytes.
This contract covers material actually observed by the harness; it does not
pretend to record hidden provider internals or every byte of an uninspected
Python heap. The audit design must state any recording failure honestly without
treating a gap marker as a replacement for lost captured material.

## Refit is a separate quiescent operation

The operator's clarified rule supersedes the earlier reconnaissance experiment
about dragging active commands through replacement. Before refit, main-harness
admission closes, commands/provider requests settle, and managed standing work
is confirmed paused while remaining alive. A cancelled waiter, sent pause signal,
or idle-looking UI does not establish that state. If it cannot be reached,
refit has not begun.

The independent outpost owns the handed-off conversation during compilation;
its build and chat operations belong to its environment. A failed build leaves
that owner available. On return, the rebuilt harness adopts the accumulated
conversation and reconnects to the same paused standing resource instances.
Resume follows actual reconnection. Uninterrupted OS daemons are outside the
pause scope by operator choice. Process suspension, owner placement, and
handoff transport belong to the runtime study; this paper does not prescribe
signals or a supervisor implementation.

In the integrated Lisp/native-owner candidate, long-lived arbitrary Python
workflow stacks stay in their separate, paused hosts through controller
replacement. Reconnecting to those hosts preserves the old frames. Loading new
Python definitions for later calls remains distinct from migrating those frames.

There is one active execution owner for the handed-off conversation. Concurrent
peers are separately identified conversations, not competing owners of that
handoff. Definition/context versions and message admissions during handoff must
be legible on return. This is continuity through a quiet replacement, while hot
turn changes above address a different case with ordinary work still active.

## Direct scenarios and model-ergonomics evidence

These proposed oracles attack concrete faults. They are not tests executed in
this study, and they do not depend on an audit record certifying itself.

| Scenario | Direct outcome to observe |
|---|---|
| Busy room | With a provider stream and program running, send known distinct messages from several peers. Every accepted message is retrievable; each inclusion identifies the actual subsequent request/decision; concurrent arrivals are neither erased nor duplicated. The receiver can respond before unrelated work ends. |
| Policy change across a response | Issue request R under policy/tool binding v1, activate v2 before its response, then cause another decision. The new decision runs v2; R's input and returned tool call still name v1 bindings. A renamed/schema-changed operation is never silently substituted. |
| Waiting, racing, and failure | Race a failing operation and a later successful one with an explicit first-success condition. Return the successful handle plus correct pending/completed members. Cancelling the observer leaves retained jobs unchanged; explicitly cancelling a job produces a separately observable request and outcome. |
| Compaction with valuable state | Keep a large value, an unpicklable resource, a function, and a standing program. Compact while a peer message arrives. Subsequent work can still use the same resource instances; the peer message remains available; all emitted original input/output is recoverable independently of the new context. |
| Conflicting live edits | Two editors activate changes against the same base. One transition applies; the other receives the current version/conflict rather than overwriting it unseen. A failed state conversion leaves the previous instance usable. |
| Refit and return | Verify zero active main commands/provider requests and actual paused, live standing instances before handoff. After rebuild/reattachment, the same instances resume with their prior state; build/chat outputs and return context are retained. A failed build keeps the outpost usable. |
| Stuck control program | Make the programmable controller block. Independent inspection/message acceptance and lifecycle control remain usable. A pending definition change is not falsely reported active. Use a bounded controlled fault, not an unbounded laptop experiment. |

Model ergonomics needs direct task evidence. Compare target models performing
the same useful work through Python composition, ordinary tools, and a typed
handle surface, then include a real Lisp controller-policy edit in the compiled
candidate: discover an operation, launch background work, diagnose a wrong
argument, inspect large output, handle a failed branch, and continue after a
context change. Check actual outputs and resource lifetimes. Record specific
mistakes, recovery, elapsed work, and model usage as explanatory observations.
Call/token count alone is not success, and a model's explanation of its own
ease of use is not the primary oracle.

The working hypotheses are that short stable handles, named result fields,
small useful defaults, exact errors, and readily available examples reduce
mistakes; and that Python improves unusual composition without making common
actions verbose. CodeAct/RLM justify the experiment, not these particular
interface choices. Include more than one relevant model and equivalent tasks;
do not turn this into a broad language tournament. Human use should accomplish
the same operations without impersonating a model or learning a second set of
lifecycle rules.

## Decisions still requiring review

1. Whether the live compiled Lisp controller candidate plus Python workflow
   binding gives both operator and model effective in-situ edits. This is not
   a Python-versus-Lisp choice for one undifferentiated role. Neither model
   ease of editing Lisp nor the cross-language operation cost has been measured.
2. Whether the next-decision boundary is sufficiently immediate for the operator's
   intended turn changes. If not, identify the exact active-frame modification
   required; do not promise arbitrary stack transplantation by saying “hot.”
3. The supported first set of workflow migration points and state conversions.
   A narrow honest mechanism is preferable to unexplained automatic replay.
4. Default handling of late messages and pending race members. Proposed defaults
   preserve work and permit prompt steering; stronger automatic cancellation
   can be a reusable turn/workflow policy rather than a hidden runtime side effect.
5. What the first model-ergonomics trial must demonstrate to retain or replace the
   proposed Python surface. No runtime benchmark or model trial has run here.

[prime-config]: https://github.com/PrimeIntellect-ai/prime-agent/blob/e75f59efc6f74fcb23e45048f0f23b34490571d0/crates/pa-agent/src/agent.rs#L557
[prime-protocol]: https://github.com/PrimeIntellect-ai/prime-agent/blob/e75f59efc6f74fcb23e45048f0f23b34490571d0/prime-agent-runtime/src/rlm/repl.md#L91
[prime-prune]: https://github.com/PrimeIntellect-ai/prime-agent/blob/e75f59efc6f74fcb23e45048f0f23b34490571d0/crates/pa-core/src/session_engine/ipython_state.rs#L181
[dagger-messages]: https://github.com/dagger/dagger/blob/4129d95c0c43198d11e1b42791d3adbbdb505799/core/agent.go#L152
[dagger-waits]: https://github.com/dagger/dagger/blob/4129d95c0c43198d11e1b42791d3adbbdb505799/core/agent.go#L359
[dagger-reseed]: https://github.com/dagger/dagger/blob/4129d95c0c43198d11e1b42791d3adbbdb505799/core/agent.go#L2233
[dagger-reload]: https://github.com/dagger/dagger/blob/4129d95c0c43198d11e1b42791d3adbbdb505799/core/agents_reload.go#L17
[dagger-tools]: https://github.com/dagger/dagger/blob/4129d95c0c43198d11e1b42791d3adbbdb505799/core/llm_docs.md#L1
[restate-wait]: https://github.com/restatedev/sdk-python/blob/ad4ea7c0a60b5745681a809fe57656bec0939c38/python/restate/asyncio.py#L108
[restate-example]: https://github.com/restatedev/sdk-python/blob/ad4ea7c0a60b5745681a809fe57656bec0939c38/examples/concurrent_greeter.py#L46
[sly-compile]: https://github.com/joaotavora/sly/blob/3ffa216d0818972f7a7fea38a566a6b570349f3b/slynk/backend/sbcl.lisp#L761
