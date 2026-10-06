# ipykernel

Persistent code execution, rich replies, history queries and separate control channel; restart continuity is not live-object persistence.

Role: Python execution kernel service. Runtime: Python.

Pinned source: [https://github.com/ipython/ipykernel](https://github.com/ipython/ipykernel); revision/version `cfa461d8afac05cefc3671128d00adaa3c0720f4`.

Jupyter messages over ZMQ with separate control thread; Python/IPython shell, asyncio execution and optional threaded subshells.

Kernel is an independent computation service; a harness should consume its protocol rather than infer ownership from locality.

Inspection: Read namespace initialization, execute/result handling, SIGINT and shutdown, history dispatch, subshell manager and selected tests.

Limits of this study: IPython/Jupyter client dependencies are not independently audited; arbitrary native/foreign code cancellation and serialization not established.

## Actions

### execute_request

Surface: supplied primitive.

Input: Code, silent/store_history/allow_stdin, cell metadata and user expressions

Result: execute_reply status, counter, values/errors and IOPub payloads

Lifecycle: Persistent shell; async or blocking execution.

Authority: Authorized kernel client; host-process code authority

Evidence: [execute](#evidence-execute), [reply](#evidence-reply).

### history_request

Surface: supplied primitive.

Input: Tail/range/search, raw/output flags, session/range/pattern

Result: Stored code/history tuples

Lifecycle: Synchronous manager lookup exposed through reply.

Authority: Kernel client

Evidence: [history](#evidence-history).

### interrupt_request / shutdown_request

Surface: supplied primitive.

Input: Interrupt or restart flag

Result: Addressed control reply; loops stop on shutdown

Lifecycle: Interrupt best effort; shutdown settles protocol but does not snapshot values.

Authority: Kernel control client

Evidence: [interrupt](#evidence-interrupt), [shutdown](#evidence-shutdown).

### create/list/delete subshell requests

Surface: supplied primitive.

Input: Subshell identity and control operation

Result: Thread-backed execution route or unknown-subshell error

Lifecycle: Independent route, shared kernel lifetime.

Authority: Kernel client

Evidence: [subshell](#evidence-subshell).

## Capabilities

### filesystem

**— — Files** (role): filesystem: this reference supplies Python execution kernel service; an agent policy, model interface and context lifecycle are outside its supplied role.

### processes

**— — OS programs** (role): processes: this reference supplies Python execution kernel service; an agent policy, model interface and context lifecycle are outside its supplied role.

### code-actions

**S — Code actions** (source): Python cells and user expressions are executable request payloads with structured values/errors.

Evidence: [execute](#evidence-execute).

### persistent-kernel

**S — Kernel** (source): Same shell retains namespace across requests until process/environment restart; no arbitrary-object serialization promised.

Evidence: [namespace](#evidence-namespace).

### standing-database

**? — Standing DB** (inspection scope): IPython history_manager is accessible through Python, but a standing application/evidence DB service contract was not established by kernel protocol paths.

### workflow-programming

**S — Workflows** (source): Client programs can compose execution/history/control requests; agent turns and workflow ownership belong to consumer.

Evidence: [execute](#evidence-execute).

### multi-model

**— — Models** (role): multi_model: this reference supplies Python execution kernel service; an agent policy, model interface and context lifecycle are outside its supplied role.

### live-collaboration

**— — Peer chat** (role): live_collaboration: this reference supplies Python execution kernel service; an agent policy, model interface and context lifecycle are outside its supplied role.

### concurrent-work

**S — Concurrency** (source): Optional subshell threads permit overlapping execution in the same kernel; shared namespace is not isolated agent ownership.

Evidence: [subshell](#evidence-subshell).

### steering-interrupt

**L — Steer/interrupt** (source): Separate control path sends signal and main-thread async cancellation; subshells cannot install the same signal handler, native blocking code need not stop promptly.

Evidence: [interrupt](#evidence-interrupt), [signal_async](#evidence-signal_async).

### turn-redefinition

**— — Turn program** (role): turn_redefinition: this reference supplies Python execution kernel service; an agent policy, model interface and context lifecycle are outside its supplied role.

### compaction

**— — Compaction** (role): compaction: this reference supplies Python execution kernel service; an agent policy, model interface and context lifecycle are outside its supplied role.

### context-repair

**— — Repair** (role): context_repair: this reference supplies Python execution kernel service; an agent policy, model interface and context lifecycle are outside its supplied role.

### original-audit

**L — Original audit** (source): Store-history is optional and history queries expose shell code/output history; this is not all original message bytes, stream outputs or provider activity.

Evidence: [execute](#evidence-execute), [history](#evidence-history).

### audit-query

**S — Audit query** (source): History tail/range/search of stored code and optional outputs; not complete original provider/IO corpus.

Evidence: [history](#evidence-history).

### hot-change

**S — Hot change** (source): Executed Python can change functions and variables in the existing namespace; transactionally activating harness programs after a turn is consumer machinery.

Evidence: [namespace](#evidence-namespace), [execute](#evidence-execute).

### rebuild-continuity

**L — Rebuild continuity** (source): Shutdown/restart flag stops loops; namespace/data migration and harness re-inhabitation are not supplied by this path.

Evidence: [shutdown](#evidence-shutdown).

### remote-services

**S — Remote** (source): Independent Jupyter message kernel is a service boundary; remote transport security and reconnect client state are not fully traced here.

Evidence: [subshell](#evidence-subshell).

### self-improvement

**— — Self-improve** (role): self_improvement: this reference supplies Python execution kernel service; an agent policy, model interface and context lifecycle are outside its supplied role.

### complaints

**— — Complaints** (role): complaints: this reference supplies Python execution kernel service; an agent policy, model interface and context lifecycle are outside its supplied role.

### authority

**S — Authority** (source): Python code executes with kernel process authority; exposed client/model approvals must be designed outside the kernel.

Evidence: [execute](#evidence-execute).

### evaluation

**S — Evaluation** (source): Inspected tests assert exported Unicode history and concurrent shared-variable relations; interrupt test primarily asserts eventual addressed reply.

Evidence: [reply](#evidence-reply).

### time-order

**— — Time/order** (role): time_order: this reference supplies Python execution kernel service; an agent policy, model interface and context lifecycle are outside its supplied role.

## Inspected test oracles

- [quarantine/ipykernel/tests/test_kernel.py](../../../quarantine/ipykernel/tests/test_kernel.py): Unicode history export and signal/message interruption of input(). Oracle: History file must contain exact source including abcþ; interrupt test validates execute_reply parent ID, not completeness of stdout or foreign-call termination. Read, **not executed**.
- [quarantine/ipykernel/tests/test_subshells.py](../../../quarantine/ipykernel/tests/test_subshells.py): Overlapping subshell execution with shared variables. Oracle: Barrier/sleep assertions establish expected overlap and execute_reply status; time-sensitive test is skipped under coverage. Read, **not executed**.

## Useful mechanisms

- An explicit computation consumer/service boundary and structured execution identity.
- Separate control traffic and history retrieval are useful even without agent machinery.

## Material limits

- Kernel history is narrower than a complete original audit.
- Interrupt acknowledgement and restart flags cannot be treated as universal cancellation or memory continuity.

## Arconaut design questions

- How does a harness pause only its requests and standing programs while other clients continue using a shared kernel?
- What values/handles must be serialized or reacquired independently of the kernel process?

## Evidence

### Evidence namespace

[quarantine/ipykernel/ipykernel/ipkernel.py:98–152](../../../quarantine/ipykernel/ipykernel/ipkernel.py#L98): User namespace is assigned to a persistent IPython shell.

### Evidence execute

[quarantine/ipykernel/ipykernel/ipkernel.py:374–439](../../../quarantine/ipykernel/ipykernel/ipkernel.py#L374): Code is transformed and dispatched to blocking or asynchronous IPython cell runner with optional history.

### Evidence reply

[quarantine/ipykernel/ipykernel/ipkernel.py:470–507](../../../quarantine/ipykernel/ipykernel/ipkernel.py#L470): Reply includes error traceback/type/value, execution counter, user expressions and partial payloads.

### Evidence interrupt

[quarantine/ipykernel/ipykernel/kernelbase.py:1097–1132](../../../quarantine/ipykernel/ipykernel/kernelbase.py#L1097): Interrupt sends SIGINT to process or process group on POSIX; Windows message interrupt unsupported here.

### Evidence shutdown

[quarantine/ipykernel/ipykernel/kernelbase.py:1134–1170](../../../quarantine/ipykernel/ipykernel/kernelbase.py#L1134): Shutdown reply carries restart flag, then stops control and shell loops; not namespace snapshot.

### Evidence history

[quarantine/ipykernel/ipykernel/ipkernel.py:612–646](../../../quarantine/ipykernel/ipykernel/ipkernel.py#L612): Tail/range/search dispatch to history manager, with raw/output flags.

### Evidence subshell

[quarantine/ipykernel/ipykernel/subshell_manager.py:36–113](../../../quarantine/ipykernel/ipykernel/subshell_manager.py#L36): Manager owns subshell thread/socket lifetime and serializes cache mutations/sends; control thread routes separately.

### Evidence signal_async

[quarantine/ipykernel/ipykernel/ipkernel.py:320–368](../../../quarantine/ipykernel/ipykernel/ipkernel.py#L320): SIGINT cancels async future in main thread; subshells cannot use that signal handler.

