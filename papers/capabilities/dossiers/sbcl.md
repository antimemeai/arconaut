# sbcl

Live compilation and global-state core images are powerful primitives, with explicit thread, stream and runtime compatibility limits.

Role: native Common Lisp compiler/runtime. Runtime: Common Lisp, C.

Pinned source: [https://github.com/sbcl/sbcl](https://github.com/sbcl/sbcl); revision/version `e0b6f381c2abb096fc2c4b00d1caead9b6f968be`.

Compiled Lisp in a long-lived OS process; OS threads and child processes are explicit.

A runtime used by an application, not an agent or shared-computation governor.

Inspection: Read save/deinit, source/FASL load, compile-file interface, process execution/control, thread interruption and save rejection tests.

Limits of this study: No complete compiler/runtime audit or execution of SBCL; no agent protocol or harness rebuild coordinator supplied.

## Actions

### LOAD / COMPILE-FILE

Surface: supplied primitive.

Input: Source/FASL path and compiler options

Result: Loaded globals or FASL path, diagnostics and errors

Lifecycle: Live process; definitions may change future calls.

Authority: Host application process authority

Evidence: [load](#evidence-load), [compile](#evidence-compile).

### SB-EXT:RUN-PROGRAM

Surface: supplied primitive.

Input: Executable, literal argv, directory, environment, PTY and IO options

Result: PROCESS handle and streams/status hook

Lifecycle: Wait by default; :wait nil gives ongoing process.

Authority: Host OS authority

Evidence: [process](#evidence-process).

### SB-THREAD:INTERRUPT-THREAD

Surface: supplied primitive.

Input: Thread and callback

Result: Asynchronous execution or thread error

Lifecycle: FIFO interrupts; critical sections defer delivery.

Authority: Application thread control

Evidence: [interrupt](#evidence-interrupt).

### SB-EXT:SAVE-LISP-AND-DIE

Surface: supplied primitive.

Input: Core path, toplevel, executable flag and hooks

Result: Core or combined executable; current process exits

Lifecycle: Quiescent image snapshot; later startup at toplevel.

Authority: Application owns image and thread cleanup

Evidence: [save](#evidence-save), [deinit](#evidence-deinit).

### GET-INTERNAL-REAL-TIME / GET-INTERNAL-RUN-TIME

Surface: supplied primitive.

Input: No arguments

Result: Elapsed startup/CPU time units

Lifecycle: Current runtime instance.

Authority: Calling Lisp program

Evidence: [time](#evidence-time).

## Capabilities

### filesystem

**S — Files** (source): LOAD reads executable source/FASL; ordinary filesystem work must be composed by an application.

Evidence: [load](#evidence-load).

### processes

**S — OS programs** (source): Child-process handles with PTY/streams, nonwaiting launch, status hooks and separate control APIs; no agent wrappers.

Evidence: [process](#evidence-process).

### code-actions

**S — Code actions** (source): Lisp source/FASL can execute in the live runtime; exposure to a model remains application work.

Evidence: [load](#evidence-load).

### persistent-kernel

**S — Kernel** (source): Lisp global state persists for process life; image save retains globals but not stacks or open streams.

Evidence: [save](#evidence-save).

### standing-database

**— — Standing DB** (role): standing_database: this reference supplies native Common Lisp compiler/runtime; an agent policy, model interface and context lifecycle are outside its supplied role.

### workflow-programming

**S — Workflows** (source): Arbitrary Lisp programs and threads can compose computation; no agent workflow semantics supplied.

Evidence: [compile](#evidence-compile).

### multi-model

**— — Models** (role): multi_model: this reference supplies native Common Lisp compiler/runtime; an agent policy, model interface and context lifecycle are outside its supplied role.

### live-collaboration

**— — Peer chat** (role): live_collaboration: this reference supplies native Common Lisp compiler/runtime; an agent policy, model interface and context lifecycle are outside its supplied role.

### concurrent-work

**S — Concurrency** (source): OS-thread interruption and child-process handles provide mechanisms, not workflow ownership.

Evidence: [interrupt](#evidence-interrupt).

### steering-interrupt

**S — Steer/interrupt** (source): Asynchronous thread interrupts exist, with explicit cleanup hazards; the operator queue semantics are application work.

Evidence: [interrupt](#evidence-interrupt).

### turn-redefinition

**— — Turn program** (role): turn_redefinition: this reference supplies native Common Lisp compiler/runtime; an agent policy, model interface and context lifecycle are outside its supplied role.

### compaction

**— — Compaction** (role): compaction: this reference supplies native Common Lisp compiler/runtime; an agent policy, model interface and context lifecycle are outside its supplied role.

### context-repair

**— — Repair** (role): context_repair: this reference supplies native Common Lisp compiler/runtime; an agent policy, model interface and context lifecycle are outside its supplied role.

### original-audit

**— — Original audit** (role): original_audit: this reference supplies native Common Lisp compiler/runtime; an agent policy, model interface and context lifecycle are outside its supplied role.

### audit-query

**— — Audit query** (role): audit_query: this reference supplies native Common Lisp compiler/runtime; an agent policy, model interface and context lifecycle are outside its supplied role.

### hot-change

**S — Hot change** (source): Source/FASL load and compilation in the same process; compiled direct references may affect redefinition and active frames are not refitted automatically.

Evidence: [compile](#evidence-compile).

### rebuild-continuity

**L — Rebuild continuity** (source): Core save/restart preserves globals only, requires thread quiescence, changes streams and cannot transfer images across rebuilt runtimes; this does not supply an executable refit/outpost.

Evidence: [save](#evidence-save), [deinit](#evidence-deinit).

### remote-services

**— — Remote** (role): remote_services: this reference supplies native Common Lisp compiler/runtime; an agent policy, model interface and context lifecycle are outside its supplied role.

### self-improvement

**— — Self-improve** (role): self_improvement: this reference supplies native Common Lisp compiler/runtime; an agent policy, model interface and context lifecycle are outside its supplied role.

### complaints

**— — Complaints** (role): complaints: this reference supplies native Common Lisp compiler/runtime; an agent policy, model interface and context lifecycle are outside its supplied role.

### authority

**S — Authority** (source): Loaded programs and child processes run with host process authority; no model approval policy exists here.

Evidence: [process](#evidence-process).

### evaluation

**S — Evaluation** (source): Save tests assert a specific multiple-thread error and continued finalizer identity; they do not validate harness/provider continuity.

Evidence: [deinit](#evidence-deinit).

### time-order

**S — Time/order** (source): Separate startup real-time and process CPU time; no durable agent causal clock or paused-work policy.

Evidence: [time](#evidence-time).

## Inspected test oracles

- [quarantine/sbcl/tests/save.impure.lisp](../../../quarantine/sbcl/tests/save.impure.lisp): Attempt to image-save while another thread is waiting. Oracle: assert-error expects save-with-multiple-threads-error; second case checks finalizer remains a thread after rejected save. Read, **not executed**.

## Useful mechanisms

- Live native compilation and interactive redefinition without a JS host.
- The image contract explicitly distinguishes globals from stacks, threads and OS streams.

## Material limits

- An image is tied to the exact runtime build; rebuilding the native runtime needs a separate data handoff.
- Interrupts can disrupt cleanup; a disciplined quiescence protocol remains application work.

## Arconaut design questions

- Can a data handoff preserve agent identity and context while leaving compiler/runtime images disposable?
- Which mutable definitions are indirectly dispatched so activation can occur after turn/workflow conclusion?

## Evidence

### Evidence save

[quarantine/sbcl/src/code/save.lisp:96–223](../../../quarantine/sbcl/src/code/save.lisp#L96): Images preserve global state, unwind stacks, change streams and require single-thread quiescence; even separately built runtimes are incompatible.

### Evidence deinit

[quarantine/sbcl/src/code/save.lisp:340–380](../../../quarantine/sbcl/src/code/save.lisp#L340): Save hooks precede checked live/starting/joinable-thread rejection and foreign teardown.

### Evidence load

[quarantine/sbcl/src/code/target-load.lisp:205–267](../../../quarantine/sbcl/src/code/target-load.lisp#L205): LOAD evaluates source or loads FASL into the current Lisp environment.

### Evidence compile

[quarantine/sbcl/src/compiler/main.lisp:1879–1920](../../../quarantine/sbcl/src/compiler/main.lisp#L1879): COMPILE-FILE returns FASL; block compilation may resolve function references at compilation time.

### Evidence process

[quarantine/sbcl/src/code/run-program.lisp:718–870](../../../quarantine/sbcl/src/code/run-program.lisp#L718): RUN-PROGRAM accepts literal argv, optional PTY/streams, wait flag, status hook and preserved descriptors.

### Evidence interrupt

[quarantine/sbcl/src/code/target-thread.lisp:2391–2421](../../../quarantine/sbcl/src/code/target-thread.lisp#L2391): Thread interrupts are asynchronous and FIFO but nonlocal transfer during cleanup may skip cleanup.

### Evidence time

[quarantine/sbcl/src/code/time.lisp:14–24](../../../quarantine/sbcl/src/code/time.lisp#L14): Startup real-time and process CPU time are separate interfaces.

