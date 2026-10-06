# squirreling

Lazy cell materialization makes expensive API/model calls query demand-driven; strong idea independent of its JS runtime.

Role: streaming SQL query engine. Runtime: JavaScript.

Pinned source: [https://github.com/hyparam/squirreling](https://github.com/hyparam/squirreling); revision/version `249f5bb27ed4d685e13bd23bf67bfc8053117eab`.

Host JS event loop, async row/cell generators and pluggable sources/UDFs.

A read/query composition library over supplied data; not a standing storage engine or agent.

Inspection: Read public exports, SQL normalization/planning/scan/abort path, UDF dispatch, collect and expensive-cell/abort tests.

Limits of this study: No full SQL conformance or join/window algorithm audit; no persistent DB, agent scheduling or original audit supplied.

## Actions

### executeSql / executePlan

Surface: supplied primitive.

Input: SQL/plan, tables, asynchronous functions and AbortSignal

Result: QueryResults with columns and async rows/cells

Lifecycle: Demand-driven stream; interruption can expose partial values plus error.

Authority: Host-provided source/UDF authority

Evidence: [query](#evidence-query), [scan](#evidence-scan).

### collect

Surface: supplied primitive.

Input: QueryResults

Result: Fully materialized record array

Lifecycle: Collects rows then evaluates cells concurrently; may consume substantial memory.

Authority: Calling program

Evidence: [collect](#evidence-collect).

### UDF apply / data-source scan

Surface: supplied primitive.

Input: Evaluated SQL arguments or pushdown hints/signal

Result: Awaited scalar or async scan rows

Lifecycle: Caller-defined IO lifetime and side effects.

Authority: Caller supplied callback

Evidence: [udf](#evidence-udf), [scan](#evidence-scan).

## Capabilities

### filesystem

**— — Files** (role): filesystem: this reference supplies streaming SQL query engine; an agent policy, model interface and context lifecycle are outside its supplied role.

### processes

**— — OS programs** (role): processes: this reference supplies streaming SQL query engine; an agent policy, model interface and context lifecycle are outside its supplied role.

### code-actions

**S — Code actions** (source): Host-supplied asynchronous UDFs execute as demanded cells; arbitrary model code installation is outside engine.

Evidence: [udf](#evidence-udf).

### persistent-kernel

**— — Kernel** (inspection scope): This is a query engine over host sources/UDFs, not an executable persistent language kernel.

### standing-database

**L — Standing DB** (source): Queryable sources are supplied by caller and README limits SQL to read-only; no standing write/durable database engine.

Evidence: [contract](#evidence-contract), [query](#evidence-query).

### workflow-programming

**S — Workflows** (source): Declarative SQL composes filtering, projection and asynchronous sources/UDFs with lazy evaluation.

Evidence: [query](#evidence-query), [udf](#evidence-udf).

### multi-model

**— — Models** (role): multi_model: this reference supplies streaming SQL query engine; an agent policy, model interface and context lifecycle are outside its supplied role.

### live-collaboration

**— — Peer chat** (role): live_collaboration: this reference supplies streaming SQL query engine; an agent policy, model interface and context lifecycle are outside its supplied role.

### concurrent-work

**S — Concurrency** (source): collect materializes gathered cells concurrently; execution is JS async, not independent agent/process ownership.

Evidence: [collect](#evidence-collect).

### steering-interrupt

**S — Steer/interrupt** (source): AbortSignal is threaded through scans and checked after cooperative stream exhaustion; arbitrary UDF/external call termination is not established.

Evidence: [scan](#evidence-scan).

### turn-redefinition

**— — Turn program** (role): turn_redefinition: this reference supplies streaming SQL query engine; an agent policy, model interface and context lifecycle are outside its supplied role.

### compaction

**— — Compaction** (role): compaction: this reference supplies streaming SQL query engine; an agent policy, model interface and context lifecycle are outside its supplied role.

### context-repair

**— — Repair** (role): context_repair: this reference supplies streaming SQL query engine; an agent policy, model interface and context lifecycle are outside its supplied role.

### original-audit

**— — Original audit** (role): original_audit: this reference supplies streaming SQL query engine; an agent policy, model interface and context lifecycle are outside its supplied role.

### audit-query

**S — Audit query** (source): SQL can query a supplied audit-shaped source; audit capture, retention and persistence are not part of this engine.

Evidence: [query](#evidence-query).

### hot-change

**S — Hot change** (source): Tables/functions are passed per query so later queries can receive new definitions; no transaction for active queries or harness replacement.

Evidence: [query](#evidence-query).

### rebuild-continuity

**— — Rebuild continuity** (role): rebuild_continuity: this reference supplies streaming SQL query engine; an agent policy, model interface and context lifecycle are outside its supplied role.

### remote-services

**S — Remote** (source): A supplied async source/UDF can consume APIs; transport/session/authority is host-supplied.

Evidence: [udf](#evidence-udf).

### self-improvement

**— — Self-improve** (role): self_improvement: this reference supplies streaming SQL query engine; an agent policy, model interface and context lifecycle are outside its supplied role.

### complaints

**— — Complaints** (role): complaints: this reference supplies streaming SQL query engine; an agent policy, model interface and context lifecycle are outside its supplied role.

### authority

**S — Authority** (source): Caller supplies sources/functions; engine executes those with host authority, without model-specific approvals.

Evidence: [query](#evidence-query).

### evaluation

**S — Evaluation** (source): Demand-sensitive call counts and explicit abort/result-count tests attack waste and truncated-success errors.

Evidence: [tests](#evidence-tests), [scan](#evidence-scan).

### time-order

**— — Time/order** (role): time_order: this reference supplies streaming SQL query engine; an agent policy, model interface and context lifecycle are outside its supplied role.

## Inspected test oracles

- [quarantine/squirreling/test/execute/expensive.test.js](../../../quarantine/squirreling/test/execute/expensive.test.js): Unnecessary expensive cell/API work. Oracle: Counters expect zero for unselected columns and exact 1/2/5 demand-dependent calls; not cost/latency benchmark. Read, **not executed**.
- [quarantine/squirreling/test/execute/abort.test.js](../../../quarantine/squirreling/test/execute/abort.test.js): Abort before/during a streamed scan. Oracle: Rejects with aborted error; exactly Alice/Bob and two produced rows remain when aborted after second row. Read, **not executed**.

## Useful mechanisms

- Lazy structured queries can orchestrate expensive computations while avoiding unused work.
- Abort surfaces truncation rather than silently treating partial scan as success.

## Material limits

- JS event-loop dependency conflicts with core preference; mechanism study does not select this library.
- No durable storage or agent action lifecycle supplied.

## Arconaut design questions

- Can a standing evidence/query interface lazily call remote computation without obscuring cost, identity and cancellation?
- How is partial streamed data represented separately from completed query success?

## Evidence

### Evidence query

[quarantine/squirreling/src/execute/execute.js:24–62](../../../quarantine/squirreling/src/execute/execute.js#L24): SQL executes over supplied tables/functions/signal and returns planned lazy results; host event-loop yield interval is 4000.

### Evidence scan

[quarantine/squirreling/src/execute/execute.js:380–417](../../../quarantine/squirreling/src/execute/execute.js#L380): Scan receives pushdown hints/signal, invalid partial pushdown errors and cooperative abort is surfaced after scan completion.

### Evidence udf

[quarantine/squirreling/src/expression/evaluate.js:679–685](../../../quarantine/squirreling/src/expression/evaluate.js#L679): User-defined functions are awaited after case-insensitive name lookup.

### Evidence collect

[quarantine/squirreling/src/execute/utils.js:84–101](../../../quarantine/squirreling/src/execute/utils.js#L84): collect first gathers rows then materializes cells concurrently; stream interface avoids whole-result collection.

### Evidence contract

[quarantine/squirreling/README.md:13–24](../../../quarantine/squirreling/README.md#L13): Documented scope is streaming async read-only SQL and lazy async cells, not storage durability.

### Evidence tests

[quarantine/squirreling/test/execute/expensive.test.js:50–113](../../../quarantine/squirreling/test/execute/expensive.test.js#L50): Call-count assertions distinguish zero unselected work from selected/filter/limit-driven expensive calls.

