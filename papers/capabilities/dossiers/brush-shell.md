# brush-shell

Native shell composition, custom builtins and named jobs offer a strong OS integration reference, with explicit job-lifetime and timing limits.

Role: embeddable native shell interpreter. Runtime: Rust.

Pinned source: [https://github.com/reubeno/brush.git](https://github.com/reubeno/brush.git); revision/version `f84ae8a74c43cad85824477d67ab3da5de972558`.

Async Rust shell core (Tokio in examples), OS process groups/jobs and mutable shell state.

An application embeds a shell or invokes its executable; no provider or agent governor.

Inspection: Read embedding examples, job states/wait/signal/foreground paths, history representation, timing and background compatibility cases.

Limits of this study: No full parser/Bash conformance audit; reference programs and test harness were not executed.

## Actions

### Shell::run_string / invoke_function

Surface: supplied primitive.

Input: Shell source or named function, argv and FD execution parameters

Result: ExecutionResult/exit code, shell state and output streams

Lifecycle: Same embedded shell across calls.

Authority: Host application authority

Evidence: [embed](#evidence-embed).

### Shell::builder().builtin

Surface: supplied primitive.

Input: Native Command implementation and name

Result: Registered executable builtin with context/IO access

Lifecycle: Host construction; runtime uses registered implementation.

Authority: Embedding program; native change requires compilation

Evidence: [builtin](#evidence-builtin).

### Job::poll_done / wait / kill / move_to_foreground

Surface: supplied primitive.

Input: Job identity, signal or scheduling request

Result: Completion/stopped result, process group effects or error

Lifecycle: Ongoing job lifecycle; no native rebuild continuity.

Authority: Shell/embedding application

Evidence: [job](#evidence-job), [control](#evidence-control).

### History::get_by_id / import

Surface: supplied primitive.

Input: Item ID or line stream

Result: Mutable command-history entry

Lifecycle: Shell-local or persisted history import.

Authority: Embedding application

Evidence: [history](#evidence-history).

## Capabilities

### filesystem

**S — Files** (source): Shell redirections and ordinary commands can read/write filesystem; no model wrapper.

Evidence: [embed](#evidence-embed).

### processes

**S — OS programs** (source): Job/process-group handles and wait/poll/signal/foreground operations supply native process control.

Evidence: [control](#evidence-control).

### code-actions

**S — Code actions** (source): Executable shell scripts/functions plus host-defined native builtins.

Evidence: [embed](#evidence-embed).

### persistent-kernel

**S — Kernel** (source): Variables/functions/history remain in embedded Shell across run_string/invoke_function; OS child memory is separate.

Evidence: [embed](#evidence-embed).

### standing-database

**— — Standing DB** (role): standing_database: this reference supplies embeddable native shell interpreter; an agent policy, model interface and context lifecycle are outside its supplied role.

### workflow-programming

**S — Workflows** (source): Shell pipelines/functions/jobs compose programs; native builtin extension API is available.

Evidence: [builtin](#evidence-builtin).

### multi-model

**— — Models** (role): multi_model: this reference supplies embeddable native shell interpreter; an agent policy, model interface and context lifecycle are outside its supplied role.

### live-collaboration

**— — Peer chat** (role): live_collaboration: this reference supplies embeddable native shell interpreter; an agent policy, model interface and context lifecycle are outside its supplied role.

### concurrent-work

**S — Concurrency** (source): Background jobs have explicit local IDs, task lists and state; implicit waiting on shell exit is a known uncovered boundary.

Evidence: [job](#evidence-job), [cases](#evidence-cases).

### steering-interrupt

**S — Steer/interrupt** (source): Signals, foreground ownership and job resumption are supplied primitives; no provider-turn interrupt.

Evidence: [control](#evidence-control).

### turn-redefinition

**— — Turn program** (role): turn_redefinition: this reference supplies embeddable native shell interpreter; an agent policy, model interface and context lifecycle are outside its supplied role.

### compaction

**— — Compaction** (role): compaction: this reference supplies embeddable native shell interpreter; an agent policy, model interface and context lifecycle are outside its supplied role.

### context-repair

**— — Repair** (role): context_repair: this reference supplies embeddable native shell interpreter; an agent policy, model interface and context lifecycle are outside its supplied role.

### original-audit

**L — Original audit** (source): Mutable/imported command history may skip unreadable text and stores command entries, not complete original IO.

Evidence: [history](#evidence-history).

### audit-query

**S — Audit query** (source): Mutable command history can be queried by ID; does not retrieve all results or streams.

Evidence: [history](#evidence-history).

### hot-change

**S — Hot change** (source): Shell functions can be redefined by scripts during shell life; native builtin executable replacement requires host machinery.

Evidence: [embed](#evidence-embed).

### rebuild-continuity

**? — Rebuild continuity** (inspection scope): Function redefinition is live, but no native host rebuild/handoff preserving jobs or provider identity was established in inspected embedding/job paths.

### remote-services

**— — Remote** (role): remote_services: this reference supplies embeddable native shell interpreter; an agent policy, model interface and context lifecycle are outside its supplied role.

### self-improvement

**— — Self-improve** (role): self_improvement: this reference supplies embeddable native shell interpreter; an agent policy, model interface and context lifecycle are outside its supplied role.

### complaints

**— — Complaints** (role): complaints: this reference supplies embeddable native shell interpreter; an agent policy, model interface and context lifecycle are outside its supplied role.

### authority

**S — Authority** (source): Shell scripts/custom builtins inherit embedding application authority; no per-model approvals.

Evidence: [builtin](#evidence-builtin).

### evaluation

**S — Evaluation** (source): Background cases use side effects after explicit wait; whole suite uses compatibility comparisons, but skipped exit-wait cases cannot prove lifecycle.

Evidence: [cases](#evidence-cases).

### time-order

**L — Time/order** (source): Wall timing uses SystemTime and returns an error if clock goes backward; CPU accounting separate, no restart/paused-time semantics.

Evidence: [time](#evidence-time).

## Inspected test oracles

- [quarantine/brush-shell/brush-shell/tests/cases/compat/background_jobs.yaml](../../../quarantine/brush-shell/brush-shell/tests/cases/compat/background_jobs.yaml): Background side-effect visibility after wait and shell exit. Oracle: Explicit wait runs test -f then echo exists, compared by compatibility runner; implicit exit waits are skipped issue #1079. Read, **not executed**.

## Useful mechanisms

- Native embedding exposes ordinary shell state, function calls and descriptor-level IO.
- Explicit job IDs/process groups make ongoing operations inspectable.

## Material limits

- Shell command history is not original audit.
- Some background transition and implicit exit lifetime paths need targeted validation; wall timing is not monotonic.

## Arconaut design questions

- Can a model receive durable operation handles while using ordinary shell composition?
- How is job identity transferred or deliberately released when the consuming harness refits?

## Evidence

### Evidence embed

[quarantine/brush-shell/brush-core/examples/call-func.rs:1–71](../../../quarantine/brush-shell/brush-core/examples/call-func.rs#L1): A persistent shell defines/invokes a function and redirects file descriptors across calls.

### Evidence builtin

[quarantine/brush-shell/brush-core/examples/custom-builtin.rs:81–143](../../../quarantine/brush-shell/brush-core/examples/custom-builtin.rs#L81): Builtins execute against shell/context streams; builder registers native command implementation.

### Evidence job

[quarantine/brush-shell/brush-core/src/jobs.rs:213–296](../../../quarantine/brush-shell/brush-core/src/jobs.rs#L213): Jobs group tasks, process-group ID, command line, local ID and running/stopped/done state.

### Evidence control

[quarantine/brush-shell/brush-core/src/jobs.rs:343–437](../../../quarantine/brush-shell/brush-core/src/jobs.rs#L343): Poll/wait drain job tasks; background/foreground resume and kill act through process groups; background on nonstopped state is unimplemented.

### Evidence history

[quarantine/brush-shell/brush-core/src/history.rs:16–99](../../../quarantine/brush-shell/brush-core/src/history.rs#L16): History stores mutable command entries with IDs/timestamps and imports line-oriented command text; malformed UTF-8 may be skipped.

### Evidence time

[quarantine/brush-shell/brush-core/src/timing.rs:1–60](../../../quarantine/brush-shell/brush-core/src/timing.rs#L1): Timing uses SystemTime for wall delta and self/child CPU time; backward wall-clock subtraction can error.

### Evidence cases

[quarantine/brush-shell/brush-shell/tests/cases/compat/background_jobs.yaml:1–87](../../../quarantine/brush-shell/brush-shell/tests/cases/compat/background_jobs.yaml#L1): Implicit-exit-wait cases are skipped; explicit wait cases observe filesystem side effects.

