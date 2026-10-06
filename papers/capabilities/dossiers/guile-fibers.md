# guile-fibers

Composable choice/wrap operations and synchronous channels offer expressive concurrency; cancellation in this snapshot has a source-level coverage concern.

Role: Scheme concurrency scheduler. Runtime: Scheme, C.

Pinned source: [https://github.com/wingo/fibers](https://github.com/wingo/fibers); revision/version `f08253bafc3026408185227f3d2e753a9493484e`.

Delimited-continuation fibers over configurable OS-thread schedulers; IO polling, timers and optional preemption.

Runs inside a Guile application; does not own shared kernels, providers or an agent fabric.

Inspection: Read run/spawn/cleanup, operation choice/suspension/cancellation, timers, concurrency design and channel/preemption/cancel-timer tests.

Limits of this study: No execution or full Guile/foreign-IO audit; distributed participants, persistent checkpoint and harness refit are outside role.

## Actions

### run-fibers / spawn-fiber

Surface: supplied primitive.

Input: Initial thunk, parallelism, preemption hz, drain flag; child closure

Result: Initial return values; scheduled fibers

Lifecycle: Default completion ends scheduler; drain waits for pending work.

Authority: Enclosing Guile program

Evidence: [run](#evidence-run), [spawn](#evidence-spawn).

### choice-operation / wrap-operation / perform-operation

Surface: supplied primitive.

Input: First-class operations and result continuation

Result: One selected result or suspension until ready

Lifecycle: Suspended fiber or blocking foreign thread; losing cancellation has noted defect.

Authority: Application programs

Evidence: [ops](#evidence-ops), [cancel](#evidence-cancel).

### timer-operation / sleep-operation

Surface: supplied primitive.

Input: Internal-time expiry or duration seconds

Result: Operation completing with no values

Lifecycle: Cancelable wheel entry when callback reached.

Authority: Application programs

Evidence: [timer](#evidence-timer).

## Capabilities

### filesystem

**— — Files** (role): filesystem: this reference supplies Scheme concurrency scheduler; an agent policy, model interface and context lifecycle are outside its supplied role.

### processes

**— — OS programs** (role): processes: this reference supplies Scheme concurrency scheduler; an agent policy, model interface and context lifecycle are outside its supplied role.

### code-actions

**S — Code actions** (source): Scheme closures can compose operations and execute arbitrary application logic, with model interface supplied separately.

Evidence: [spawn](#evidence-spawn).

### persistent-kernel

**S — Kernel** (source): In-process closures/dynamic state survive across fiber scheduling; no process-restart serialization is supplied.

Evidence: [spawn](#evidence-spawn).

### standing-database

**— — Standing DB** (role): standing_database: this reference supplies Scheme concurrency scheduler; an agent policy, model interface and context lifecycle are outside its supplied role.

### workflow-programming

**S — Workflows** (source): Choice and wrap are first-class composable operations rather than a fixed graph DSL.

Evidence: [ops](#evidence-ops).

### multi-model

**— — Models** (role): multi_model: this reference supplies Scheme concurrency scheduler; an agent policy, model interface and context lifecycle are outside its supplied role.

### live-collaboration

**S — Peer chat** (source): In-process channels and operation composition supply communication primitives, not independent model participants.

Evidence: [ops](#evidence-ops).

### concurrent-work

**S — Concurrency** (source): Parallel schedulers, fibers and work suspension; drain behavior determines whether background work outlives initial thunk.

Evidence: [run](#evidence-run).

### steering-interrupt

**L — Steer/interrupt** (source): Preemption and losing-operation cancellation are provided, but cancel-other-operations never advances beyond index zero in this snapshot; no general agent turn-cancel protocol.

Evidence: [run](#evidence-run), [cancel](#evidence-cancel), [timer](#evidence-timer).

### turn-redefinition

**— — Turn program** (role): turn_redefinition: this reference supplies Scheme concurrency scheduler; an agent policy, model interface and context lifecycle are outside its supplied role.

### compaction

**— — Compaction** (role): compaction: this reference supplies Scheme concurrency scheduler; an agent policy, model interface and context lifecycle are outside its supplied role.

### context-repair

**— — Repair** (role): context_repair: this reference supplies Scheme concurrency scheduler; an agent policy, model interface and context lifecycle are outside its supplied role.

### original-audit

**— — Original audit** (role): original_audit: this reference supplies Scheme concurrency scheduler; an agent policy, model interface and context lifecycle are outside its supplied role.

### audit-query

**— — Audit query** (role): audit_query: this reference supplies Scheme concurrency scheduler; an agent policy, model interface and context lifecycle are outside its supplied role.

### hot-change

**? — Hot change** (inspection scope): Guile code is hosted dynamically, but executable definition activation and active-fiber migration were not established in inspected scheduler/operation paths.

### rebuild-continuity

**? — Rebuild continuity** (inspection scope): Captured in-process continuations are not evidence of transfer across rebuilt runtime/processes; no such handoff established.

### remote-services

**— — Remote** (role): remote_services: this reference supplies Scheme concurrency scheduler; an agent policy, model interface and context lifecycle are outside its supplied role.

### self-improvement

**— — Self-improve** (role): self_improvement: this reference supplies Scheme concurrency scheduler; an agent policy, model interface and context lifecycle are outside its supplied role.

### complaints

**— — Complaints** (role): complaints: this reference supplies Scheme concurrency scheduler; an agent policy, model interface and context lifecycle are outside its supplied role.

### authority

**S — Authority** (source): Runs with the enclosing Guile program authority; model approvals and service governance are not defined.

Evidence: [design](#evidence-design).

### evaluation

**S — Evaluation** (source): Channel result and preemption tests have explicit expected values; cancel-timer heap test covers only a timer in the first choice position.

Evidence: [cancel](#evidence-cancel), [timer](#evidence-timer).

### time-order

**S — Time/order** (source): Timers use Guile internal real time and scheduler ticks; no persisted deadline/paused-time semantics.

Evidence: [timer](#evidence-timer).

## Inspected test oracles

- [quarantine/guile-fibers/tests/channels.scm](../../../quarantine/guile-fibers/tests/channels.scm): Channel communication/recursive concurrent RPC results. Oracle: Explicit equal-value assertions including rpc-fib 24 = 75025; termination-only pingpong is weaker. Read, **not executed**.
- [quarantine/guile-fibers/tests/preemption.scm](../../../quarantine/guile-fibers/tests/preemption.scm): Busy fibers progressing without cooperative IO yields. Oracle: Two fibers alternate atomic counter parity; expected result 100. Test success is not bounded production cancellation. Read, **not executed**.
- [quarantine/guile-fibers/tests/cancel-timer.scm](../../../quarantine/guile-fibers/tests/cancel-timer.scm): Losing timer continuation accumulation. Oracle: After 200000 choices with timer first, heap must grow no more than 2x; placement does not cover later losing alternatives, allocator behavior affects oracle. Read, **not executed**.

## Useful mechanisms

- First-class operations compose communication, continuations and deadlines.
- Explicit initial-thunk versus drain lifecycle informs honest ongoing-work boundaries.

## Material limits

- No durable restart/continuation transfer or agent policy provided.
- Source concern: cancellation loop only checks index zero; position-specific test can miss later alternatives.

## Arconaut design questions

- Can ordinary programs combine cancellation, IO and result routing without callback boilerplate?
- A three-way choice test must assert every losing cancellation callback fires exactly once for each winner position.

## Evidence

### Evidence run

[quarantine/guile-fibers/fibers.scm:75–163](../../../quarantine/guile-fibers/fibers.scm#L75): Schedulers support configurable parallelism/preemption; default returns when initial thunk finishes, while drain waits for pending work; auxiliary threads stop during unwind.

### Evidence spawn

[quarantine/guile-fibers/fibers.scm:164–190](../../../quarantine/guile-fibers/fibers.scm#L164): Fibers start next scheduler turn, capture dynamic state and may select parallel scheduler.

### Evidence ops

[quarantine/guile-fibers/fibers/operations.scm:100–193](../../../quarantine/guile-fibers/fibers/operations.scm#L100): Choice flattens alternatives; wrap transforms results; suspend resumes captured continuation.

### Evidence cancel

[quarantine/guile-fibers/fibers/operations.scm:140–150](../../../quarantine/guile-fibers/fibers/operations.scm#L140): Named loop visits index zero without recursive advance; later losing alternatives are not traversed in this pinned source.

### Evidence timer

[quarantine/guile-fibers/fibers/timers.scm:45–90](../../../quarantine/guile-fibers/fibers/timers.scm#L45): Timer uses internal elapsed time and cancellation removes wheel entry when its callback is invoked.

### Evidence design

[quarantine/guile-fibers/fibers.texi:309–341](../../../quarantine/guile-fibers/fibers.texi#L309): Blocking IO suspends fibers; foreign threads may block; fibers are in-process delimited continuations.

