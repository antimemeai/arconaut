# Systems runtime frumentarii: continuity while changing the harness

2026-09-30. Bounded primary-source reconnaissance for the foundation research;
not a language choice, product specification, implementation plan, or adoption
recommendation. Read current AGENTS, Blackbird, and FOUNDATION. No downloaded
product was installed or executed; no model/provider API, operator credential,
legacy implementation, Git state, or issue database was touched. Source excerpts
and GitHub revision metadata are in ignored `context/systems-frumentarii/`.

The operator's latest direction governs this report: Rust is optional; JS/TS and
Go are distrusted; JVM is generally excluded. Compiled code is welcome insofar as
the model can rebuild the harness and inhabit its successor with continuity.
Otherwise the compiled component should be small. Hot redefinition of execution
policy, managed compaction, comprehensive study data, live peers, persistent
experiments, and operator/model improvement in situ are first-class research
subjects. None is assumed to mean a fixed user/assistant cycle.

## Principal finding

The useful division is between **what can change and what remains alive**, not
between compiled and interpreted languages. SBCL compiles new functions into a
running process. BEAM can keep old and new modules present while processes move
between them. A native supervisor can keep independent jobs alive while a
controller is replaced. Durable execution can reconstruct computation in another
process. These mechanisms preserve different things and impose different limits.

Seven references below supply materially different mechanisms. The most relevant
next comparison is live image/actor evolution versus a replaceable policy process
over a stable resource owner. That is a research comparison, not a proposed
architecture. A large Rust agent is useful opposing evidence: its source can
still contain fixed policy order, unbounded queues, lossy output handling, and
limited peer messaging. Native code does not remove event loops or schedulers.

Evidence labels: **Observed** means inspected pinned source; **Documented** means
the primary author's specification or description, not locally tested behavior;
**Inference** and **Question** explicitly delimit our conclusions. No throughput,
latency, platform-compatibility, or recovery experiment was performed here.

## Revision record

All revisions were resolved from public upstream default branches on 2026-09-30.
Older commit dates are retained rather than presented as new development.

| Reference | Inspected revision | Commit date UTC |
| --- | --- | --- |
| [Goose](https://github.com/aaif-goose/goose) | `ac15f938151bb8c0efd93ed9c2c1cf61298934bf` | 2026-09-30 |
| [Jido](https://github.com/agentjido/jido) | `90b163eef87ace89a153e94f114c8dc736be2ccd` | 2026-09-28 |
| [Jido AI](https://github.com/agentjido/jido_ai) | `01b7dca0f8897980097260c6cd15515658c56676` | 2026-09-28 |
| [SBCL](https://github.com/sbcl/sbcl) | `e0b6f381c2abb096fc2c4b00d1caead9b6f968be` | 2026-09-30 |
| [SLY/Slynk](https://github.com/joaotavora/sly) | `3ffa216d0818972f7a7fea38a566a6b570349f3b` | 2026-08-01 |
| [Guile Fibers](https://github.com/wingo/fibers) | `f08253bafc3026408185227f3d2e753a9493484e` | 2025-07-18 |
| [Restate server](https://github.com/restatedev/restate) | `2180b55410f6512f8534012167aa536116d75c8a` | 2026-09-29 |
| [Restate Rust SDK](https://github.com/restatedev/sdk-rust) | `5cd9a3d24d7ddc21f29c8c43074d3d67ca67c163` | 2026-09-22 |
| [OpenAgents SDK](https://github.com/openagents-org/openagents-sdk) | `faf416fca40b1cf73f585636478587967bc9f6f5` | 2026-08-06 |
| [s6](https://github.com/skarnet/s6) | `67254f0c147f9b9f71aabc7988e52bdaeb9576ab` | 2026-09-21 |

OpenAgents SDK was extracted from its larger project; the findings here refer to
this exact SDK source, not every component of the current hosted workspace.
Guile core was studied through its official 3.0.11 manual, not a pinned core
checkout. OTP documentation retrieved was 29.1/29.1.1. These version distinctions
matter when acquiring references.

## 1. Goose: native agent with separable operations, but restrictive live boundaries

**Observed.** Goose's state-machine path composes steering, turn limits,
compaction, approvals, tools, retries, and stop hooks as ordered operations. It
coexists with another reply path; this inspection does not establish which path
every frontend enables. This is useful evidence for separating execution policy
from a single monolithic loop, but the operation order is constructed in Rust,
not a model-redefinable policy language. Provider and model configuration are
captured when that machine is created; changing the agent's provider field is
not evidence that an existing stream or machine changes with it.
[construction](https://github.com/aaif-goose/goose/blob/ac15f938151bb8c0efd93ed9c2c1cf61298934bf/crates/goose/src/agents/agent.rs#L1644),
[snapshot boundary](https://github.com/aaif-goose/goose/blob/ac15f938151bb8c0efd93ed9c2c1cf61298934bf/crates/goose/src/agents/agent.rs#L1993),
[provider update](https://github.com/aaif-goose/goose/blob/ac15f938151bb8c0efd93ed9c2c1cf61298934bf/crates/goose/src/agents/agent.rs#L3631).

**Observed.** Compaction has an explicit operation, command, usage accounting,
conversation effect, and error response. It checks unanswered tool requests and
accounts for tool results not included in the last provider usage report. This
is considerably more informative prior art than treating compaction as a token
counter followed by deletion. It does not establish user-selectable compaction
strategies or a complete immutable record of context transformations.
[compaction operation](https://github.com/aaif-goose/goose/blob/ac15f938151bb8c0efd93ed9c2c1cf61298934bf/crates/goose/src/agents/state_machine/ops_compaction.rs#L51).

**Observed.** The built-in shell pipes stdout/stderr, supplies null stdin, and
collects output concurrently through an unbounded channel. The receiver is
drained after waiting for the child; final presentation truncation therefore
does not bound accumulation during execution. Cancellation/timeout kills the
direct child. The inspected constructor creates no process group. Output
collection is aborted after a 500 ms drain timeout. **Inference:** this is neither
a persistent interactive terminal nor a general descendant-ownership mechanism;
background descendants and output bursts require separate experiments. This
does not condemn other Goose extensions whose process behavior was not traced.
[shell lifecycle](https://github.com/aaif-goose/goose/blob/ac15f938151bb8c0efd93ed9c2c1cf61298934bf/crates/goose/src/agents/platform_extensions/developer/shell.rs#L557),
[constructor](https://github.com/aaif-goose/goose/blob/ac15f938151bb8c0efd93ed9c2c1cf61298934bf/crates/goose/src/agents/platform_extensions/developer/shell.rs#L682).

**Observed separate mechanism.** Goose's MCP subprocess helper does create a
process group and, on Linux, configures parent-death signaling. Long-lived MCP
children are spawned from a dedicated thread so their death signal is not tied
to a transient Tokio worker. The built-in shell calls only the window-setting
helper, which is a no-op on Unix. These paths must not be conflated.
[subprocess helpers](https://github.com/aaif-goose/goose/blob/ac15f938151bb8c0efd93ed9c2c1cf61298934bf/crates/goose/src/subprocess.rs#L46).

**Observed surprise.** The orchestrator can start, inspect, interrupt, and message
sessions, yet its message tool rejects a busy target. Delegated sessions can
message only sibling delegated sessions and cannot start other sessions. A
separate steering queue exists in Agent, but this tool does not use it to deliver
to a working peer. Thus “multi-agent” is not evidence of the operator's live
IRC-style interaction.
[authorization and busy-target rejection](https://github.com/aaif-goose/goose/blob/ac15f938151bb8c0efd93ed9c2c1cf61298934bf/crates/goose/src/agents/platform_extensions/orchestrator.rs#L451).

**Acquire for:** operation/effect separation, provider boundaries, compaction
lifecycle, and concrete process/audit counterexamples. **Question:** can policy
change between observable steps while preserving active jobs and clearly naming
the policy version that governed each decision?

## 2. Jido on BEAM/OTP: agent strategies over supervised processes

**Observed.** Jido exposes strategy callbacks `cmd`, `init`, and `tick`, returning
agent state and directives. AgentServer owns directive execution and signal
processing. This is a useful programmable boundary without assuming a provider
turn is the unit of all execution. It is not proof that arbitrary actions are
pure, nor that one server processes its own state concurrently.
[strategy contract](https://github.com/agentjido/jido/blob/90b163eef87ace89a153e94f114c8dc736be2ccd/lib/jido/agent/strategy.ex#L1),
[signal and directive handling](https://github.com/agentjido/jido/blob/90b163eef87ace89a153e94f114c8dc736be2ccd/lib/jido/agent_server.ex#L1230).

**Observed.** Jido AI's ReAct configuration resolves models through ReqLLM;
request transformation supports a model override and validates options against
that runtime provider. Checkpoint fingerprints include model, prompt, tool
names, execution settings, and transformer identity. This is an actual boundary
for exchangeable model configuration, not arbitrary migration of an already
issued request. Fingerprinting tool names does not itself identify tool code.
[configuration](https://github.com/agentjido/jido_ai/blob/01b7dca0f8897980097260c6cd15515658c56676/lib/jido_ai/reasoning/react/config.ex#L175).

**Observed limits.** The directive queue has a size limit, but the inspected
incoming call queue and deferred asynchronous signal queue simply append.
One must not generalize the directive limit to total mailbox backpressure.
Recent debug events are conditional and kept in a bounded ring; they are not a
comprehensive durable audit. The inspected AgentServer has no `code_change`
implementation. Pod mutation uses explicit locking and a mutation lifecycle;
that is topology mutation, not automatic conversion of every in-flight state.
[incoming queues](https://github.com/agentjido/jido/blob/90b163eef87ace89a153e94f114c8dc736be2ccd/lib/jido/agent_server.ex#L1467),
[debug buffer](https://github.com/agentjido/jido/blob/90b163eef87ace89a153e94f114c8dc736be2ccd/lib/jido/agent_server/state.ex#L438),
[pod mutation](https://github.com/agentjido/jido/blob/90b163eef87ace89a153e94f114c8dc736be2ccd/lib/jido/pod/mutable.ex#L20).

**Documented foundation.** OTP permits current and old module versions together;
qualified calls enter current code, while old frames may continue. A further
load/purge can terminate processes still using old code. State conversion is a
separate operation: `sys:change_code` requires suspension and invokes the
conversion callback. This is a serious live evolution mechanism, not unrestricted
replacement of all running computations.
[code loading](https://www.erlang.org/doc/system/code_loading.html),
[state migration](https://www.erlang.org/doc/apps/stdlib/sys.html#change_code/5).

**Documented scheduler limit.** Native functions can monopolize schedulers unless
divided, offloaded, or scheduled as dirty work. Killing the Erlang process does
not forcibly stop an executing dirty NIF; some suspension/GC operations wait for
it. BEAM is not the JVM, but it remains a runtime with explicit scheduling and
foreign-code boundaries.
[OTP NIF documentation](https://www.erlang.org/doc/apps/erts/erl_nif.html#long-running-nifs).

**Acquire for:** message-driven control, supervision, explicit state conversion,
and live topology changes. **Question:** which edits can happen in-place, which
require process replacement, and how do external terminals/kernels survive node
replacement? A BEAM process is not a POSIX process or a durable record.

## 3. SBCL plus SLY/Slynk: compiled, image-based live programming

**Observed.** Slynk runs a server separately from its editor client and exposes
evaluation and compilation RPCs. Its SBCL backend compiles submitted source to a
file and loads the result into the existing Lisp. Compilation and live mutation
are therefore compatible; the relevant distinction is granularity and binding
semantics. A model-facing client need not reproduce an editor UI to study this
mechanism, although the protocol's suitability needs its own assessment.
[server/evaluation](https://github.com/joaotavora/sly/blob/3ffa216d0818972f7a7fea38a566a6b570349f3b/slynk/slynk.lisp#L935),
[compile/load](https://github.com/joaotavora/sly/blob/3ffa216d0818972f7a7fea38a566a6b570349f3b/slynk/backend/sbcl.lisp#L761).

**Documented.** CLOS supports runtime method and class redefinition. This gives
policy experimentation a richer substrate than changing configuration strings.
Existing frames, captured function objects, and compiler optimizations still
make “all code immediately replaced” an unjustified promise.
[SBCL CLOS implementation notes](https://www.sbcl.org/sbcl-internals/Discriminating-Functions.html).

**Observed.** SBCL's process API carries PID, status, stdin/stdout/stderr and PTY
streams, asynchronous waiting, and explicit PID versus process-group signaling.
These are useful OS primitives, not a completed ownership or recovery policy.
[process representation](https://github.com/sbcl/sbcl/blob/e0b6f381c2abb096fc2c4b00d1caead9b6f968be/src/code/run-program.lisp#L169),
[signaling](https://github.com/sbcl/sbcl/blob/e0b6f381c2abb096fc2c4b00d1caead9b6f968be/src/code/run-program.lisp#L291).

**Observed hard boundary.** Saving a core is not snapshotting the entire running
environment. The implementation requires a single remaining application thread
after save hooks and reinitializes streams when loading. Saving involves process
deinitialization. A saved Lisp object containing a process identity is not a
restored OS process, pipe, or database connection.
[save contract and deinitialization](https://github.com/sbcl/sbcl/blob/e0b6f381c2abb096fc2c4b00d1caead9b6f968be/src/code/save.lisp#L193).

**Documented limit.** SBCL uses OS threads; arbitrary asynchronous interruption
and termination can unwind through code that cannot safely handle it. Interactive
debugging facilities are powerful but cannot substitute for designed cancellation.
[current threading manual](https://www.sbcl.org/manual/#Asynchronous-Operations).

**Inference.** This is an unusually relevant substrate for autodroit experiments:
inspect live values, redefine policy, examine a failure, invoke a restart, and
continue in the same image. The unattended path must still record changes in
source and distinguish source revision, loaded definitions, and persistent data.
No model provider ecosystem, crash-complete audit, supervision tree, or robust
hot state migration was established by this reconnaissance.

**Question:** can a model evolve a live policy, intentionally exercise old/new
objects, then recreate the same working state from source plus explicit data in
a clean image? An image-only success would not answer the continuity requirement.

## 4. Guile plus Fibers: live Scheme with composable concurrent operations

**Observed.** Fibers runs parallel schedulers on OS threads, combines suspendable
ports with channels, and includes configurable preemption. Source defaults to
100 Hz and processor-count parallelism. Scheduler lifetime is explicit: the
default `run-fibers` scope ends when its initial thunk returns unless configured
to drain; auxiliary threads are then stopped. This is relevant to programmable
composition and scoped tasks, not evidence of independent durable jobs.
[scheduler lifetime](https://github.com/wingo/fibers/blob/f08253bafc3026408185227f3d2e753a9493484e/fibers.scm#L76).

**Documented and source-backed.** Rendezvous channels wait for a sender and
receiver, providing a concrete backpressure mechanism. Parallel schedulers
steal/share work. Preemption works only where a continuation can be suspended;
C-to-Scheme recursion can introduce continuation barriers. Blocking foreign
operations therefore remain consequential even without JavaScript.
[channels and scheduling](https://github.com/wingo/fibers/blob/f08253bafc3026408185227f3d2e753a9493484e/fibers.texi#L349),
[barriers](https://github.com/wingo/fibers/blob/f08253bafc3026408185227f3d2e753a9493484e/fibers.texi#L1075).

**Documented reload boundary.** Guile supports module reload, but declarative
module optimizations mean redefining a binding need not change every use.
Reloading the module or selecting non-declarative bindings changes that tradeoff;
`define-once` protects precious state from ordinary reload initialization.
This is a concrete example of how optimization, live code, and state interact.
[Guile declarative modules](https://www.gnu.org/software/guile/manual/html_node/Declarative-Modules.html).

**Inference.** Scheme is a serious alternative for a live control language, with
operations that models could compose as programs and inspect as values. Fibers
is not a durable actor system, provider adapter layer, process supervisor, or
automatic audit. Its public HEAD is older than the agent frameworks; that is a
maintenance fact, not by itself a defect. Platform support, terminal handling,
tool-library interruption, and how a new policy reaches already captured
continuations remain untested.

**Acquire for:** composable scheduling/backpressure and explicit continuation
limits. **Question:** can the intended model ergonomics be achieved without
exposing accidental scheduler/C-stack restrictions as mysterious tool failures?

## 5. Restate: reconstructing computations across replaceable workers

**Observed.** The Rust SDK journals nondeterministic results through `ctx.run`,
has configurable retries, and offers durable selection with cancellation
handling. It prohibits nesting context operations inside `run` and warns that
interleaving journaled actions can make replay nondeterministic. The server tracks
pinned deployments; its resume command explicitly distinguishes retaining a
deployment from selecting another.
[journal contract](https://github.com/restatedev/sdk-rust/blob/5cd9a3d24d7ddc21f29c8c43074d3d67ca67c163/src/context/mod.rs#L898),
[durable select](https://github.com/restatedev/sdk-rust/blob/5cd9a3d24d7ddc21f29c8c43074d3d67ca67c163/src/context/select.rs#L1),
[deployment state](https://github.com/restatedev/restate/blob/2180b55410f6512f8534012167aa536116d75c8a/crates/invoker-impl/src/invocation_state_machine.rs#L318),
[resume](https://github.com/restatedev/restate/blob/2180b55410f6512f8534012167aa536116d75c8a/cli/src/commands/invocations/resume.rs#L40).

**Documented.** New invocations use new deployments while existing invocations
remain on their original version. Moving a paused invocation to another
deployment still requires journal-compatible code. In-place changes that alter
operation order or inputs can break replay. Persistent object state also needs
schema compatibility. This is code evolution by explicit execution boundary,
not unrestricted hot mutation of an existing workflow.
[versioning](https://docs.restate.dev/services/versioning).

**Documented limit.** An external side effect can happen repeatedly if failure
occurs before its result becomes durable. Durable return values do not make an
arbitrary shell command, model request, or external database mutation exactly
once. The original architecture explanation is explicit about this boundary.
[external effects](https://restate.dev/blog/why-we-built-restate#side-effects).

**Inference.** This offers useful mechanisms for persistent experiment jobs and
recovery, independent of a model provider. Its HTTP invocation API makes jobs
attachable by identity, but attaching a durable invocation is not attaching its
original PTY, pipe, or process memory. A workflow journal is not a complete
research audit of arbitrary code inside an effect. An implementation could add
those observations; they are not automatic. Full service orchestration may be
too much substrate for the desired harness, and the Rust SDK inspection is not
a recommendation to write the policy in Rust.
[public invocation API](https://docs.restate.dev/services/invocation/http).

**Question:** which experimental units should replay unchanged, and which should
explicitly adopt a new policy? Recoverability and changing the running experiment
must be distinguished rather than hidden inside “resume.”

## 6. OpenAgents: live network participation versus active-task steering

**Observed.** The adapter base dispatches different channels to concurrent tasks,
but messages to a busy channel are appended to an unbounded list and processed
after its current task. This is live arrival with deferred consumption, not
in-flight model participation. It is a valuable counterexample to inferring
execution semantics from an IRC-like presentation.
[channel dispatch](https://github.com/openagents-org/openagents-sdk/blob/faf416fca40b1cf73f585636478587967bc9f6f5/src/openagents/adapters/base.py#L231).

**Observed.** The Goose adapter assigns stable per-workspace/agent/channel session
names and persists the mapping. Each task starts a headless process with piped
streams and a new POSIX session, writes its prompt, then closes stdin. Stop sends
signals to the process group, escalating after a timeout. A silence watchdog can
terminate the task; oversized stream lines can be skipped. These are concrete
controller/child policies, not native-runtime guarantees.
[process creation and stream handling](https://github.com/openagents-org/openagents-sdk/blob/faf416fca40b1cf73f585636478587967bc9f6f5/src/openagents/adapters/goose.py#L555),
[group cancellation](https://github.com/openagents-org/openagents-sdk/blob/faf416fca40b1cf73f585636478587967bc9f6f5/src/openagents/adapters/goose.py#L380).

**Inference.** Stable session lookup is a useful reattachment ingredient, but
this launcher does not establish continuing execution through controller rebuild.
Whole-group cancellation can also kill a kernel or database intentionally meant
to persist. Conversely, descendants that deliberately create another session may
escape that group. The adapter's “every child” docstring is stronger than POSIX
process-group signaling alone establishes. Python's asyncio here is another
event-loop design; escaping JS does not settle scheduling requirements.

**Acquire for:** agent/network identity, channel/context mapping, lifecycle
translation between independent harnesses. **Question:** what precisely counts
as delivered, observed, or applied when a peer speaks during another model call
or tool execution? A public message and each participant's private context are
different objects.

## 7. s6: small native process ownership independent of policy code

**Observed/documented.** s6 is a small C/Unix supervision toolbox, not an agent
framework. `s6-supervise` owns a direct child, runs `./run`, monitors exit and
readiness, and responds to control commands. Source distinguishes PID and
process-group signals. `s6-svscan` creates and retains the logging pipe between a
service and its logger while supervising both; logger and service replacement
need not discard that pipe merely because an endpoint restarts.
[supervisor source](https://github.com/skarnet/s6/blob/67254f0c147f9b9f71aabc7988e52bdaeb9576ab/src/supervision/s6-supervise.c#L172),
[scan and pipes](https://github.com/skarnet/s6/blob/67254f0c147f9b9f71aabc7988e52bdaeb9576ab/src/supervision/s6-svscan.c#L337),
[documented ownership](https://skarnet.org/software/s6/overview.html).

**Inference.** This is strong prior art for separating a stable job owner from a
frequently replaced policy process. If the controller and experimental services
are separately supervised, rebuilding one need not kill the others. That is an
application arrangement, not an automatic property of every s6 tree. Replacing
the actual owner, transferring arbitrary descriptors, or surviving machine
failure is a different problem. s6 does not preserve an agent's heap, model
context, arbitrary terminal state, or causal audit. Standard stream logging can
block under pressure and is not equivalent to recording every semantic event.

**Question:** can a deliberately small owner expose jobs, stream offsets,
terminal attachment, signals, and exit observations to successive policy
processes without taking ownership of the policy itself? s6's portability and
packaging should be checked on the actual macOS/Linux targets before selecting
anything; this report did not run it.

## Continuity dimensions the next study must keep separate

| Change or continuity claim | What would directly demonstrate it |
| --- | --- |
| Configuration reload | A named setting takes effect at a stated boundary; existing work's effective configuration remains inspectable. |
| Live policy redefinition | A new decision rule governs a subsequent decision in the same session; earlier frames/closures are deliberately retained or migrated. |
| Executable refresh | A freshly built executable becomes the running controller; the actual loaded artifact is identified, not merely the latest source checkout. |
| OS-resource continuity | An existing process/PTY/pipe remains usable across replacement, with stable identity, output position, exit status, and ownership. |
| Model continuity | The successor reconstructs the intended participant context, pending messages, tool obligations, and compaction lineage. It is not merely handed the old conversation title. |
| Experiment continuity | A database/kernel keeps its intended state; work is neither silently rerun nor silently discarded when the harness changes. |
| Verified executable lineage | Source/build inputs and actual loaded code are connected to the events they govern. A source hash alone does not identify live definitions in a mutated image. |
| Causal audit | The record relates observation, decision, policy/model/context version, effect request, outcome or unknown outcome, and later interpretation; gaps/truncation are explicit. |

These are proposed discriminating claims, not a prescribed component split or
storage schema. A small stable substrate plus live control is one comparison arm.
An image/actor runtime with explicit migration is another. The parent research
lane's Prime Agent review at `e75f59efc6f74fcb23e45048f0f23b34490571d0`
supplies a Rust/CPython-kernel comparator; its process continuity should be judged
by the same dimensions, not assumed from its split-language design.

“Capturing everything” requires examining loss at the point where it happens:
provider stream fragments, tool bytes, peer messages, context selection,
compaction, failed calls, and policy edits. The references expose bounded debug
buffers, presentation truncation, unknown external effects, and journals that
only see operations passed through them. None inspected establishes the entire
operator requirement. This conclusion calls for studying the actual observation
boundary, not creating receipts that certify other receipts.

## Discriminating scenarios for research design

1. **Rebuild and inhabit the successor.** A participant runs a long-lived kernel,
   a verbose background job, and an interactive terminal. It changes controller
   code, builds it, starts the successor, and continues the same work. Observe
   lost/duplicated effects, terminal behavior, process identity, context, and
   executable lineage. Repeat with the predecessor failing during handoff.
2. **Redefine policy while work is in flight.** Change what constitutes a turn,
   provider selection, tool concurrency, and stop conditions while a model
   stream and independent job are active. Identify the exact point at which
   each change applies; include an incompatible state change that must be
   migrated or reported instead of guessed through.
3. **Live peers under load.** Several peers exchange addressed messages while
   one is streaming and another emits large tool output. Determine whether a
   message is merely queued, actually enters context, or causes a policy action;
   inspect backpressure and the effect of a peer leaving/rejoining.
4. **Compaction as an intervention.** Compare alternative context transformations
   with outstanding tools and newly arriving peer observations. Preserve the
   pre-transformation evidence, chosen inputs, output, effective model, and
   subsequent behavior. Do not treat a smaller token count as its oracle.
5. **Unknown external outcome.** Interrupt between an external mutation and
   recording its result; then change executable or policy. The successor must
   represent uncertainty and apply a chosen reconciliation strategy, rather than
   infer failure from absence of a completion event.

These are study proposals. They require a reviewed design and plan before
product implementation. They can discriminate mechanisms without selecting a
vendor, language, database, or mandatory approval workflow. Model ergonomics
should be measured through the model's ability to inspect, compose, correct, and
continue the work, alongside operator usability and direct behavioral oracles.

## Literature and acquisition priorities

- **Dynamic software updating:** [Kitsune, TOPLAS 2014](https://doi.org/10.1145/2629460)
  explicitly addresses whole-program update points and state transformation for
  compiled C. Abstract inspected; full paper merits acquisition and reading.
  It counters the assumption that native code necessarily requires abandoning a
  running computation, without promising that arbitrary updates are free.
- **Failure isolation and supervision:** [Armstrong's 2003 thesis](https://erlang.org/download/armstrong_thesis_2003.pdf).
  Abstract inspected here; acquire/read the supervision and upgrade material
  alongside current OTP semantics. Language mechanisms and library protocols are
  different responsibilities.
- **Live multi-agent coordination:** [AgentRoom, August 2026](https://arxiv.org/abs/2608.23740).
  Abstract inspected: file claims, status, and broadcasts over CRDT-backed shared
  files; evaluation spans five coding CLIs and four backend tasks. Useful
  contemporary comparison, but its limited tasks and judge-based contrasts do
  not establish general coding correctness or prove a CRDT is the right core.
- **Acquire first for this conceptual unit:** Jido/Jido AI plus OTP upgrade docs;
  SBCL/SLY; Fibers with matching Guile docs; s6 ownership/logging sources; Goose
  operations and shell; Restate SDK plus matching server; OpenAgents adapters.
  Preserve exact source archives and use the root owner's quarantine manifest
  workflow. This report downloaded excerpts only and did not alter quarantine.

Two additional observations deserve follow-up without becoming defaults.
[GitHub's September 2026 Copilot runtime account](https://github.blog/ai-and-ml/generative-ai/migrating-the-github-copilot-runtime-to-rust-using-copilot/)
reports a Rust rewrite and C ABI embedding; the implementation is not established
by its public SDK. The author's migration failures include lifetime/state and
incorrect test-oracle problems: a language rewrite does not validate semantics.
[Temporal's workflow definition](https://docs.temporal.io/workflow-definition)
is useful mechanism literature for deterministic replay and version boundaries,
but its Go substrate does not fit the operator's stated default-language
preferences. Neither report should be turned into an architecture decision.

The reconnaissance found no inaccessible essential source that presently requires
operator purchase. Full-paper reading, platform experiments, source-level audit
coverage, provider interchange conformance, and actual rebuild/reattachment remain
open work. They should be selected by the scenarios above rather than expanded
into an indiscriminate framework survey.
