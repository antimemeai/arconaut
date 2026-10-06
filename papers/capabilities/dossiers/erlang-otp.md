# erlang-otp

Message identity, explicit state migration and old/current code semantics are strong hot-change primitives, with sharp purge and runtime-replacement boundaries.

Role: actor runtime and standard systems libraries. Runtime: Erlang, C.

Pinned source: [https://github.com/erlang/otp](https://github.com/erlang/otp); revision/version `495ce0b626f24bc092196ffa46acc0f9f6ace625`.

BEAM processes/mailboxes, distributed nodes, OTP state machines and native runtime.

Application/fabric machinery, not an agent; a harness may consume services without governing all nodes or databases.

Inspection: Read module load/purge contract and entry functions, sys change/suspend dispatch, gen_server messaging, Mnesia transaction interface and soft-purge test.

Limits of this study: Native VM internals, full distributed protocols and Mnesia durability implementation not audited; no agent/provider path supplied.

## Actions

### code:load_binary / soft_purge / purge

Surface: supplied primitive.

Input: Module binary and version-lifetime operation

Result: Loaded module/error; purge/refusal result

Lifecycle: Two code versions; destructive purge can terminate lingering processes.

Authority: Node application authority

Evidence: [load](#evidence-load), [purge](#evidence-purge), [versions](#evidence-versions).

### sys:suspend / change_code / resume

Surface: supplied primitive.

Input: Process, old version, extra migration data and timeout

Result: ok/error and migrated callback state

Lifecycle: Suspend before migration; process identity persists.

Authority: OTP system-control caller

Evidence: [change](#evidence-change).

### gen_server:cast

Surface: supplied primitive.

Input: Process/name/node and message

Result: Immediate ok; later recipient handling

Lifecycle: Asynchronous mailbox delivery, not terminal work result.

Authority: Distributed node/application authority

Evidence: [message](#evidence-message).

### mnesia:transaction

Surface: supplied primitive.

Input: Executable function with table operations and retry policy

Result: atomic value or aborted reason

Lifecycle: Transaction lifetime; independent service ownership must be designed.

Authority: Database-capable application

Evidence: [db](#evidence-db).

## Capabilities

### filesystem

**— — Files** (role): filesystem: this reference supplies actor runtime and standard systems libraries; an agent policy, model interface and context lifecycle are outside its supplied role.

### processes

**— — OS programs** (role): processes: this reference supplies actor runtime and standard systems libraries; an agent policy, model interface and context lifecycle are outside its supplied role.

### code-actions

**S — Code actions** (source): Compiled Erlang modules can be loaded from binaries into a running node; model execution interface is consumer work.

Evidence: [load](#evidence-load).

### persistent-kernel

**S — Kernel** (source): Actor state remains while module code is migrated; VM restart does not automatically preserve it.

Evidence: [change](#evidence-change).

### standing-database

**S — Standing DB** (source): Mnesia transactional table operations are supplied primitives; exposure/ownership and independent durability remain separate design work.

Evidence: [db](#evidence-db).

### workflow-programming

**S — Workflows** (source): Actor callbacks/state migration and custom event hooks support executable orchestration rather than fixed model workflow.

Evidence: [change](#evidence-change), [debug](#evidence-debug).

### multi-model

**— — Models** (role): multi_model: this reference supplies actor runtime and standard systems libraries; an agent policy, model interface and context lifecycle are outside its supplied role.

### live-collaboration

**S — Peer chat** (source): Addressed mailbox/server messaging locally/remotely supplies peer primitives; no model busy-delivery policy comes with it.

Evidence: [message](#evidence-message).

### concurrent-work

**S — Concurrency** (source): Actors can keep executing old/current modules concurrently; application owns request settlement.

Evidence: [versions](#evidence-versions).

### steering-interrupt

**S — Steer/interrupt** (source): Suspension/system messages are provided; suspended state migration is distinct from aborting provider requests.

Evidence: [change](#evidence-change).

### turn-redefinition

**— — Turn program** (role): turn_redefinition: this reference supplies actor runtime and standard systems libraries; an agent policy, model interface and context lifecycle are outside its supplied role.

### compaction

**— — Compaction** (role): compaction: this reference supplies actor runtime and standard systems libraries; an agent policy, model interface and context lifecycle are outside its supplied role.

### context-repair

**— — Repair** (role): context_repair: this reference supplies actor runtime and standard systems libraries; an agent policy, model interface and context lifecycle are outside its supplied role.

### original-audit

**L — Original audit** (source): sys trace/log captures supplied system events and optional formatted file output, not all provider/OS bytes.

Evidence: [debug](#evidence-debug).

### audit-query

**S — Audit query** (source): System event logging/custom handlers are a query/debug building block; complete audit schema/retention is application work.

Evidence: [debug](#evidence-debug).

### hot-change

**S — Hot change** (source): Explicit load plus suspended code_change migrates actor state; activation timing must be programmed, old frames may remain.

Evidence: [load](#evidence-load), [change](#evidence-change).

### rebuild-continuity

**L — Rebuild continuity** (source): Live bytecode update preserves processes with deliberate state migration, but third-version purge can kill old-code users; replacement of the native VM itself is not supplied by module load.

Evidence: [versions](#evidence-versions), [purge](#evidence-purge), [change](#evidence-change).

### remote-services

**S — Remote** (source): Distributed named-node cast enables remote actor service consumption; exactly-once delivery or reconnect is not inferred.

Evidence: [message](#evidence-message).

### self-improvement

**— — Self-improve** (role): self_improvement: this reference supplies actor runtime and standard systems libraries; an agent policy, model interface and context lifecycle are outside its supplied role.

### complaints

**— — Complaints** (role): complaints: this reference supplies actor runtime and standard systems libraries; an agent policy, model interface and context lifecycle are outside its supplied role.

### authority

**S — Authority** (source): Code load/debug/DB APIs operate with node/application authority; no agent approval policy.

Evidence: [load](#evidence-load), [db](#evidence-db).

### evaluation

**S — Evaluation** (source): Soft-purge test explicitly verifies refusal and target survival while code is referenced, then success after target termination.

Evidence: [purge](#evidence-purge).

### time-order

**S — Time/order** (source): System/monotonic APIs distinguish time domains; monotonic bases cannot be compared across VM instances and ties occur.

Evidence: [time](#evidence-time).

## Inspected test oracles

- [quarantine/erlang-otp/lib/kernel/test/code_SUITE.erl](../../../quarantine/erlang-otp/lib/kernel/test/code_SUITE.erl): Old-code purge safety. Oracle: soft_purge returns false and process remains alive while referenced; returns true after process exits; not VM upgrade or agent continuity. Read, **not executed**.

## Useful mechanisms

- Code versions and state migration are explicit inspectable mechanisms.
- Actor messaging, service supervision concepts and DB transactions can support programmable agents without JS.

## Material limits

- Asynchronous cast ok does not mean a recipient processed the message.
- Live module upgrade is not native VM rebuild; purge and state-schema decisions remain dangerous if implicit.

## Arconaut design questions

- Which program definitions need generation identity and explicit old-state migration?
- Can refit require a settled request graph while allowing shared actors/services to continue for other consumers?

## Evidence

### Evidence versions

[quarantine/erlang-otp/lib/kernel/src/code.erl:217–243](../../../quarantine/erlang-otp/lib/kernel/src/code.erl#L217): Old/current versions may coexist; third load can purge and kill old-code processes.

### Evidence load

[quarantine/erlang-otp/lib/kernel/src/code.erl:651–681](../../../quarantine/erlang-otp/lib/kernel/src/code.erl#L651): Binary code preparation and code-server load support in-process module replacement.

### Evidence purge

[quarantine/erlang-otp/lib/kernel/src/code.erl:704–740](../../../quarantine/erlang-otp/lib/kernel/src/code.erl#L704): Purge kills lingering processes; soft_purge refuses when references remain.

### Evidence change

[quarantine/erlang-otp/lib/stdlib/src/sys.erl:516–544](../../../quarantine/erlang-otp/lib/stdlib/src/sys.erl#L516): System code change requires suspension and invokes explicit state migration callback.

### Evidence debug

[quarantine/erlang-otp/lib/stdlib/src/sys.erl:846–884](../../../quarantine/erlang-otp/lib/stdlib/src/sys.erl#L846): Debug event hooks support trace, bounded event log and custom callback state; only supplied events are observed.

### Evidence message

[quarantine/erlang-otp/lib/stdlib/src/gen_server.erl:1810–1833](../../../quarantine/erlang-otp/lib/stdlib/src/gen_server.erl#L1810): Cast dispatch can target named remote-node destination; asynchronous send returns ok.

### Evidence db

[quarantine/erlang-otp/lib/mnesia/src/mnesia.erl:760–804](../../../quarantine/erlang-otp/lib/mnesia/src/mnesia.erl#L760): Transactions compose table writes and return atomic result or abort; not model exposure by itself.

### Evidence time

[quarantine/erlang-otp/erts/preloaded/src/erlang.erl:5230–5276](../../../quarantine/erlang-otp/erts/preloaded/src/erlang.erl#L5230): Monotonic times may tie and have incomparable bases across runtime instances.

