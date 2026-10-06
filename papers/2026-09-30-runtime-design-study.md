# Runtime design study: live control and a replaceable main harness

**Later operator clarification supersedes the computational ownership premise.**
Arconaut consumes kernels/databases/computation potentially shared by many Arconauts;
the future meta-project fabric governs them and supplies control-plane/scaling.
The recommendation below predates that correction. Source observations remain
evidence, but resident-kernel custody is not an Arconaut responsibility. Any local
process custodian must be justified by the harness's own work. See the
[current scope decision](../docs/FOUNDATION.md#discussion-decision-arconaut-consumes-the-computational-fabric).

2026-09-30. Recommendation for integrated design review, not an adopted product
specification or permission to implement. Owns the runtime/replacement question;
it does not design the outpost system, audit database, provider protocol, or UI.
No reference product, installer, compiler, benchmark, provider request, or mutation
was executed. Only this paper was written.

## Recommendation

**Use a live compiled Common Lisp environment on SBCL for the main control
program, with resident programs held in separate processes and a small independent
OS-resource owner. Prefer Rust provisionally for that narrow owner.** Keep
model/provider selection, turn policy, context construction, compaction, and
workflow control in the programmable layer. Make full refit replace the main
controller at a verified quiescent boundary, while the resource owner keeps
standing program hosts alive and paused. An outpost carries the conversation
and build/repair activity during that interval.

This combines the first two alternatives in the brief for a specific reason:
live policy changes should be ordinary programming, while the lifetime of a
kernel, terminal, or pipe should not depend on the lifetime of the controller
being edited. SLY/Slynk is a reference for compilation, inspection, and recovery
interaction, not a decision to embed an Emacs protocol or depend on an editor.
The recommendation selects neither a Lisp framework nor third-party source for
shipping. It remains conditional on the four bounded checks below.

**A rebuildable Rust harness is the strongest fallback.** The clarified refit
rules make it substantially more credible than the earlier broad migration
scenario suggested. It must still provide executable, hot-changeable policy;
provider setters and prompt configuration are insufficient. C++ can occupy that
native role, but current Arconaut evidence supplies no advantage sufficient to
prefer its additional ownership/representation obligations. BEAM is a serious
comparison for concurrency and supervision, rather than a straw alternative.

Confidence is higher in the **process replacement boundary** than in the choice
of SBCL or Rust. The boundary follows the preservation requirement directly.
SBCL's model ergonomics and operational fit have source support but no local
behavioral evidence. Rust's advantage for the owner is a design judgment about
explicit ownership, not a measured performance or reliability ranking.

## Governing requirement and corrected scope

Read current [AGENTS](../AGENTS.md), [FOUNDATION](../docs/FOUNDATION.md), entry
points, journal, the frumentarii synthesis/review, and relevant sections of the
runtime, workflow, standing-state, and autodroit reports. Also read Rhizome's
[current C++ rigor study](../../rhizome/papers/cpp-rigor-stack.md) read-only.
Checked the subsequently written [behavioral brief](../docs/BEHAVIOR.md) and
[workflow design study](2026-09-30-workflow-design-study.md) against this proposal.
Historical source instructions supplied no authority.

Current operator rules override earlier research scenarios:

- At refit, the main harness has **no active programs or provider requests**.
- Standing harness programs remain **alive and paused**; restart-from-description
  or serialize-and-recreate is not equivalent.
- Uninterrupted work belongs in independently managed OS daemons, deployed by
  the operator, outside the harness pause scope.
- The outpost handles conversation and compilation while the main harness is
  quiescent. Concurrent peers and model-free capability services are other uses;
  they do not require further outpost design in this study.

Thus the refit question is **quiesce → pause → hand over → replace → return →
resume**, not how to transplant active provider streams or arbitrary active
execution stacks. The earlier reports' active-job replacement scenarios remain
dated inquiry; they are not acceptance conditions for refit. Hot policy change
during ordinary active work is a different operation, with its own activation
boundary. A stopped provider client is not automatically a settled request: its
external outcome may remain uncertain and must remain represented as such.

## The recommended boundary

The following are proposed design contracts, not implementation modules already
present in the repository.

| Role | Owns | Ordinary main refit |
| --- | --- | --- |
| Main control environment | Participant working state, executable control policies, provider exchanges, interpretation of events, context/compaction decisions | Quiesces and is replaced; restores explicit working data and reconnects |
| Resource owner | Resident process identities and lifecycle, PTY/pipe endpoints, stream collection, pause/resume observations | Remains available; keeps standing hosts alive and paused |
| Resident program hosts | User program/kernel heaps and their resident state; possibly Lisp, Python, or other programs | Stay alive and paused; they are not reconstructed as a substitute |
| Outpost for this refit | Handed-off conversation, build observation, repair, and return data | Works independently of the controller being rebuilt |
| Independent OS daemons | Their operator-selected service lifecycle | Continue outside refit pause scope |

The key negative constraint is physical: **no standing program whose survival is
promised may depend exclusively on a thread or heap inside the process being
replaced.** A live Lisp image does not exempt us from this constraint. Host a
standing workflow in a retained program process, or explicitly retain its host;
do not call a newly recreated workflow the same living program. This is about
ownership, not constraining the language or expressive power available to models.

The resource owner should not accumulate turn semantics, provider routing,
conversation management, experiment selection, or approval policy. Its limited
job is to own resources and expose truthful observations/controls. Ordinary
administrative work such as collecting buffered output and serving attachment
requests is distinct from executing a paused user program. The main controller
and outpost can use the same programmatic operations as the operator.

The owner is **not permanently unchangeable**. Updating it requires another
quiescent handoff of its live resources to a successor or temporary custodian.
That is a bounded additional replacement case below, not justification for a
general distributed orchestrator. Until it is demonstrated, the claim is main
controller refit, not replacement of every component without interruption.
Transferring a descriptor does not itself transfer child parentage or the right
to collect its exit status; the owner check must cover those separately. The
ordinary child-wait contract is explicitly parent/child scoped.
[Apple wait documentation](https://developer.apple.com/library/archive/documentation/System/Conceptual/ManPages_iPhoneOS/man2/wait.2.html).

## Why SBCL is the leading control environment

**Observed pinned source.** Slynk's SBCL backend compiles submitted source and
loads the compiled output into the existing Lisp. Its RPC evaluator preserves
request identity and routes conditions through its debugger. SBCL's function
definition machinery explicitly replaces named function bindings. These provide
real building blocks for a model to inspect, program, repair, and continue within
a live compiled environment.
[compile/load](https://github.com/joaotavora/sly/blob/3ffa216d0818972f7a7fea38a566a6b570349f3b/slynk/backend/sbcl.lisp#L761),
[evaluation](https://github.com/joaotavora/sly/blob/3ffa216d0818972f7a7fea38a566a6b570349f3b/slynk/slynk.lisp#L1966),
[binding replacement](https://github.com/sbcl/sbcl/blob/e0b6f381c2abb096fc2c4b00d1caead9b6f968be/src/code/fdefinition.lisp#L328).

**Design judgment.** These mechanisms align unusually well with autodroit and
hot turn redefinition. They let policy be ordinary executable code with values,
functions, introspection, and meaningful conditions, rather than an ever-growing
configuration grammar. Choosing Lisp does not establish that models find it
easier than Rust or Python; that is a deciding check, not a presumed benchmark
result. Lisp also does not dictate a serial user/assistant loop.

**Actual limits.** Loading code is not atomic migration of every object or stack.
Captured functions and active frames can retain old definitions. Compiling a
file and loading it may execute top-level forms; a failed load can have effects
before failure. The inspected Slynk backend loads an output file when available
and only then returns the compilation failure status. It is useful reference
machinery, not an atomic policy publication protocol.
[same compile/load path](https://github.com/joaotavora/sly/blob/3ffa216d0818972f7a7fea38a566a6b570349f3b/slynk/backend/sbcl.lisp#L785).

For supported hot policy changes, build a new policy generation and activate it
at a named decision boundary. Each decision records the generation it actually
used. Old calls can finish under old code; an incompatible state representation
needs explicit conversion or quiescent refit. This is a proposed execution
contract, not a restriction to a predetermined set of policies. Direct live
experimentation remains powerful, but its effects must not be mislabeled as a
transactional, reversible policy update.

SBCL's save facility does not preserve arbitrary live execution: its source
requires one remaining application thread after save hooks and reinitializes
streams on load. Its asynchronous thread interruption documentation identifies
unwind and reentrancy hazards. Therefore use explicit main state export and
fresh startup for full refit; do not make saving a running image or terminating
arbitrary threads the preservation strategy.
[save boundary](https://github.com/sbcl/sbcl/blob/e0b6f381c2abb096fc2c4b00d1caead9b6f968be/src/code/save.lisp#L193),
[interruption limits](https://github.com/sbcl/sbcl/blob/e0b6f381c2abb096fc2c4b00d1caead9b6f968be/src/code/target-thread.lisp#L2391).

### Controller language is not the only model-facing language

The workflow colleague recommends Python as the first candidate workflow surface
for practical ecosystem and research reasons, while explicitly acknowledging
the absence of comparative language-ergonomics evidence. That is compatible with
an SBCL controller: expose the same operation semantics through Lisp, Python,
CLI, and model tools as useful. A Python workflow can branch, loop, call models,
exchange messages, and await handles while the systems controller owns admission,
version applicability, and interpretation of lifecycle observations. Python
kernels and provider adapters can be separate processes. This does not place
independent environment control under the Python kernel's event loop.

For an initial design, favor Lisp for the controller's own live decision policy
and Python as an early ordinary-workflow binding; retain a path for policies in
other executable hosts through the same explicit decision/state contract. Test
that distinction rather than presenting it as already proven economical. Multiple
bindings must not become different meanings for cancellation, message delivery,
or pause. The decisive question is whether the model can discover and use those
meanings, edit the responsible policy, and repair its failure. An attractive
Lisp REPL is evidence of a mechanism, not evidence of superior model performance.

Standing Lisp workflows are subject to exactly the same lifetime rule as Python
workflows. Their suspended frames, closures, and values live in a retained host
if they must survive main refit. A short controller decision may return explicit
state for the next call; it must not conceal a promised standing continuation in
the controller heap. Arbitrary ordinary workflow programs need not be rewritten
as a step function solely to accommodate controller replacement.

## Why a separate owner, and why not just exec the new binary?

An `exec` replacement can retain non-close-on-exec descriptors and process
identity. It destroys the old process image and other threads; it does not
preserve their locks, heap, or protocol objects. Under quiescence, carefully
prepared descriptor inheritance is a plausible simpler path. It must not be
dismissed as impossible because the implementation is compiled.
[Linux exec documentation](https://man7.org/linux/man-pages/man2/execve.2.html).

I prefer a separate owner because a bad successor startup should leave the
standing programs **and their observation endpoints** available for diagnosis
and another attempt. With direct replacement, the new controller becomes the
only owner of inherited descriptors; its immediate failure can lose them.
An outpost can repair code, but that does not recreate a closed PTY or consumed
pipe data. Retaining an owner addresses this particular fault without requiring
active-computation migration.

Rust's `OwnedFd`/`BorrowedFd` interfaces expose ownership distinctions suitable
for such a component; raw descriptor adoption remains an unsafe obligation.
These types do not establish process-group membership, signal completion, stream
ordering, or persistence. Tokio also documents that dropping a child handle
normally leaves the process running: memory/resource ownership and child
lifecycle require separate contracts in native code too.
[owned descriptors](https://doc.rust-lang.org/std/os/fd/struct.OwnedFd.html),
[raw adoption](https://doc.rust-lang.org/std/os/fd/trait.FromRawFd.html),
[child lifecycle](https://docs.rs/tokio/latest/tokio/process/struct.Child.html).

The separate owner adds an IPC contract and failure mode. It earns that cost
only if its surface remains small and the same owner supports terminals,
standing kernels, attachment, and complete byte capture. Do not add a second
general agent runtime there. The recommendation does not yet select Tokio,
an IPC encoding, or a logging database.

## Pause must be an observed state

**New source finding.** s6's `killp`/`killP` sends SIGSTOP to a PID/group and
immediately sets `status.flagpaused`; the return value is not checked in those
functions. This status is not evidence that every required process has stopped.
s6 is good ownership prior art, but that flag cannot be reused as Arconaut's
quiescence oracle.
[pinned stop paths](https://github.com/skarnet/s6/blob/67254f0c147f9b9f71aabc7988e52bdaeb9576ab/src/supervision/s6-supervise.c#L252).

Linux cgroup v2 provides a stronger candidate: freezing is asynchronous and
completion appears in `cgroup.events`; descendants are included. Access and
delegation must be available, and moving a process out of the group changes its
state. This is Linux-specific evidence, not a portable pause implementation.
[kernel documentation](https://www.kernel.org/doc/html/latest/admin-guide/cgroup-v2.html).
Apple documents stopped-child observation with `waitpid`/`WUNTRACED`, but that
does not by itself establish an entire arbitrary descendant tree is stopped.
[Apple wait documentation](https://developer.apple.com/library/archive/documentation/System/Conceptual/ManPages_iPhoneOS/man2/wait.2.html).

Proposed contract: stop new workload admission; settle provider/program activity;
ask standing hosts to reach an idle boundary; then establish and observe the
chosen pause condition for all managed work. Sending a signal or receiving a
cooperative “idle” reply alone must not overstate the condition. If it cannot be
established, refit has not begun. Exact mechanisms remain a platform feasibility
question, not another permission dialog.

Pausing a process preserves its existence and resident state, not the whole
world around it. Remote sockets may time out, external files can change, and
wall/monotonic time continues. Resume needs explicit treatment of stale
connections and expired deadlines. Work requiring uninterrupted external service
belongs in the operator's separately managed daemons, as already directed.

## Alternatives weighed against the same contract

| Alternative | Strongest case | Reason it is not the leading choice here |
| --- | --- | --- |
| SBCL main plus independent resource ownership | Live compiled definitions, inspectable values/conditions, ordinary programmable policies; fresh-controller refit without saving resident heaps | Leading proposal, but Lisp model ergonomics, provider integration and packaging are untested; two-language owner adds cost |
| Thin native owner plus Python or Scheme control | Same preservation boundary; Python connects naturally to current model/tool ecosystems; Scheme offers composable continuations/concurrency | Credible alternative if it wins the bounded ergonomics test. The inspected Python/Guile systems still have scheduling, foreign-call, and live-state limits; no evidence justifies selecting one merely because it is dynamic |
| Rebuildable Rust main harness | Explicit typed ownership and state boundaries; refit now avoids arbitrary active-stack migration | Strong fallback. Needs a real hot policy surface; a separate replaceable native policy worker is possible, with compilation and state-transfer costs to test. Rebuilding the whole main harness for every policy experiment would conflate reload with refit |
| Rebuildable C++ main harness | Direct native OS/library integration; expressive compiled code; powerful tooling; incremental compilation also exists | No Arconaut-specific performance/library need currently outweighs ownership/UB and tooling obligations. Not rejected categorically; Rhizome's eventual findings can change this judgment |
| BEAM/OTP main environment | Mature message/process supervision, scheduler isolation, explicit live code/state transitions | Strongest actor alternative. OS processes and their pause/attachment still need ownership machinery; mailbox pressure, native calls, and node replacement remain real boundaries. Selecting an actor-oriented product core now supplies more structure than the present task requires |

The previous [workflow study](2026-09-30-programmable-workflows-frumentarii.md)
shows why Prime's Rust/CPython split is only partial prior art: kernel teardown
is owner-bound, snapshots omit unrepresentable state, and the traced reload
handler does not replace the executable. The [systems study](2026-09-30-systems-runtime-frumentarii.md)
likewise records Guile continuation barriers and Jido queue limits. Neither is
evidence against its entire language; both warn against mistaking an API label
for the preservation contract.

BEAM can retain current and old module versions, while state conversion through
`sys:change_code` is separate and requires suspension. OTP release handling
specifies deliberate upgrade instructions; it does not imply that arbitrary
application objects or external resources migrate automatically. These remain
valuable mechanisms even if BEAM is not selected.
[code loading](https://www.erlang.org/doc/system/code_loading.html),
[state conversion](https://www.erlang.org/doc/apps/stdlib/sys.html#change_code/5),
[release handling](https://www.erlang.org/doc/system/release_handling.html).

Do not dismiss C++ as inherently non-interactive: current Clang-Repl supports
incremental compilation through Clang and LLVM JIT. That establishes a useful
execution mechanism, not safe arbitrary redefinition, object migration, or
unloading. We have not inspected or run a complete C++ live harness.
[Clang-Repl documentation](https://clang.llvm.org/docs/ClangRepl.html).

Rhizome's transferable contribution is concrete: ownership/view contracts,
selected static checks, fault-specific instrumented builds, and direct
semantic/recovery oracles. Its current local study demonstrates a simple Clang
nonallocation guard, while some desired tools/flags remain unavailable. It does
not establish a complete C++ verification stack or select C++ for Arconaut.
Those facts support a fair later native comparison, not borrowing Rhizome's
unsettled language choice.

## Refit and hot-change contracts to carry into the design

1. **No admission after quiescence begins.** Main requests and running programs
   reach recorded settled states before refit entry. Cancellation is a request;
   an unknown external outcome remains an obligation, not a fabricated failure.
2. **Pause preserves identities and resources.** Standing hosts retain their
   actual processes/heaps and attachment endpoints. Their owner remains able to
   observe exit or failure while they are paused. No destruction/recreation is
   silently substituted for preservation.
3. **One owner for the handed-off conversation.** Main and outpost do not both
   act as its execution owner. Incoming messages during transfer are retained
   and assigned once. Independent peers keep their distinct identities; no
   general peer orchestration protocol is selected here.
4. **Return is conditional on usable state, not just startup.** The successor
   identifies its actual artifact and loaded policy generations, accepts the
   working-data representation, obtains retained program handles, and receives
   the outpost additions before workload resumption. Failure leaves the outpost
   able to repair and the standing work paused. A rollback does not undo external
   effects performed by the outpost.
5. **Hot policy change is separate.** It names the next applicable decision
   boundary and records old/new effective generations. Existing operations keep
   explicit old semantics unless a designed conversion changes them. A code
   reload does not pretend to replace an in-flight provider request.
6. **Audit survives both paths.** Retain captured originals, build input/output,
   activation and handoff events, context/compaction transformations, and outpost
   activity independently of active context. Preserve input/output bytes before
   model-facing truncation. Actual loaded definitions matter alongside source
   revision; a successful build message is insufficient executable lineage.

The audit is not a second system whose job is certifying the first. It is the
primary study record. Resource capture belongs at the resource boundary;
controller and outpost decisions must enter the same interpretable history.
This study does not select one writer, one database, or a distributed total
order. It requires direct retained observations and honest gaps.

## Four bounded feasibility checks before committing to the runtime

These are proposed next research subjects, each requiring its own reviewed plan;
none was executed here. They are not an invitation to restart a framework survey.

1. **Live control and model ergonomics.** Use one small policy problem: compose
   independent work, accept an addressed message, change continuation policy,
   inspect a failure, and recover. Exercise old function captures, a rejected
   compilation, partial load failure, and an incompatible state change. Decide
   whether SBCL gives human/model understandable control and whether the active
   generation is unambiguous. Compare one native-policy-worker formulation only
   if this exposes a deciding problem; do not assume Lisp wins by taste.
2. **Quiescent main replacement on macOS and Linux.** A resident kernel holds a
   distinguishable value and a terminal/process produces known bytes. Settle
   activity, observe pause, replace the controller, inject a bad successor
   startup, recover, and resume. The direct oracle checks the same living host,
   state, endpoints, exit behavior, and complete emitted/captured bytes. An
   external daemon continues as a contrasting control. No active main request
   is migrated during this check.
3. **Replacement of the owner itself.** At the same quiescent pause boundary,
   transfer custody of the retained endpoints/identities and resume through a
   successor owner. Determine actual macOS/Linux descriptor, child-wait, and
   terminal limits. If it cannot preserve the promised resources, narrow the
   claim explicitly or revise the ownership mechanism before claiming full
   harness re-inhabitation; do not freeze the owner permanently by definition.
4. **SBCL operational slice.** Build/package one fresh controller with an
   ordinary streaming HTTP provider fixture, inspectable event handling, one
   concurrent process stream, and persistent working-data round-trip. Use known
   inputs and outputs, no paid/live model calls. Check toolchain/dependency fit,
   condition/error ergonomics, actual provider cancellation/settlement, and
   capture completeness. This is a finite compatibility check, not a performance
   tournament or a production harness implementation.

If SBCL fails the control/operational checks materially, prefer the rebuildable
Rust controller with a separately replaceable executable policy surface while
keeping the same resident-resource boundary. If actor scheduling/supervision
dominates the observed control problem, revisit BEAM against those concrete
failures. If a C++ mechanism or Rhizome result changes the native ownership/cost
tradeoff, compare that component directly. No option should be retained solely
because its source was acquired or the inherited project used its language.

The next decision is therefore bounded: accept or overturn **live compiled SBCL
control over independent resident-resource ownership**, using those four checks
and adversarial design review. The outpost handoff makes native rebuilding
possible; it does not remove the separate reason to make everyday control policy
live and programmable.
