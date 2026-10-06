# spacetimedb

Standing tables plus reducers and subscriptions, with direct MCP SQL/schema/call tools; schema update and client compatibility limits are explicit.

Role: persistent database and executable module service. Runtime: Rust, C#, TypeScript (SDK/module options).

Pinned source: [https://github.com/clockworklabs/SpacetimeDB](https://github.com/clockworklabs/SpacetimeDB); revision/version `629e8c1e809a3d60bba7c863243a64d1e0dbb7d9`.

Rust server hosts executable modules, relational state, subscription push and durable scheduled rows.

Independent database/computation service; Arconaut is a client, not governor of all tenants/module upgrades.

Inspection: Read client MCP/tool dispatch and identity forwarding, publish controls, database migration implementation, scheduler recovery and selected schema/tool tests.

Limits of this study: No complete transactional/storage/WASM audit or runtime tests; implementation details of native/other module hosts not fully inspected.

## Actions

### MCP ping / list_databases / get_schema / sql / call

Surface: supplied primitive.

Input: Database/name, SQL or reducer and JSON arguments

Result: Schema/rows/reducer result or protocol/in-band error

Lifecycle: Request/reply; reducer uses caller connection identity.

Authority: Caller identity, ownership for SQL writes

Evidence: [tools](#evidence-tools), [identity](#evidence-identity).

### publish/update module

Surface: supplied primitive.

Input: Module artifact, migration policy/token and confirmation timeout

Result: Created/updated result or compatibility/durability error

Lifecycle: Service owns module/data upgrade; not client-harness refit.

Authority: Authorized service operator

Evidence: [publish](#evidence-publish), [update](#evidence-update).

### Scheduled reducer rows

Surface: supplied primitive.

Input: Table row identity, function and ScheduleAt

Result: Queued execution after persisted recovery

Lifecycle: Database row governs schedule; runtime deadline reacquired.

Authority: Module/service scheduling authority

Evidence: [schedule](#evidence-schedule).

## Capabilities

### filesystem

**— — Files** (role): filesystem: this reference supplies persistent database and executable module service; an agent policy, model interface and context lifecycle are outside its supplied role.

### processes

**— — OS programs** (role): processes: this reference supplies persistent database and executable module service; an agent policy, model interface and context lifecycle are outside its supplied role.

### code-actions

**S — Code actions** (source): Executable modules/reducers define custom service actions; SQL is data query/update, not unrestricted client Python kernel.

Evidence: [contract](#evidence-contract), [identity](#evidence-identity).

### persistent-kernel

**L — Kernel** (source): Database/module state is durable structured state, not a retained arbitrary Python/OS process heap across actions.

Evidence: [contract](#evidence-contract), [schedule](#evidence-schedule).

### standing-database

**S — Standing DB** (source): Standing relational tables and model-accessible schema/SQL/reducer tools are implemented service interfaces; durability internals not comprehensively validated.

Evidence: [tools](#evidence-tools), [identity](#evidence-identity).

### workflow-programming

**S — Workflows** (source): Scheduled reducers persisted as rows are an orchestration primitive; full agent continuation/context policy is client work.

Evidence: [schedule](#evidence-schedule).

### multi-model

**— — Models** (role): multi_model: this reference supplies persistent database and executable module service; an agent policy, model interface and context lifecycle are outside its supplied role.

### live-collaboration

**S — Peer chat** (source): Subscription pushes and reducer-written messages can compose shared communication; not turnkey independently working model chat.

Evidence: [contract](#evidence-contract).

### concurrent-work

**S — Concurrency** (source): Multiple clients/subscriptions plus scheduled rows supply shared-work mechanisms; no per-agent refit settlement automatically.

Evidence: [contract](#evidence-contract), [schedule](#evidence-schedule).

### steering-interrupt

**— — Steer/interrupt** (role): steering_interrupt: this reference supplies persistent database and executable module service; an agent policy, model interface and context lifecycle are outside its supplied role.

### turn-redefinition

**— — Turn program** (role): turn_redefinition: this reference supplies persistent database and executable module service; an agent policy, model interface and context lifecycle are outside its supplied role.

### compaction

**— — Compaction** (role): compaction: this reference supplies persistent database and executable module service; an agent policy, model interface and context lifecycle are outside its supplied role.

### context-repair

**— — Repair** (role): context_repair: this reference supplies persistent database and executable module service; an agent policy, model interface and context lifecycle are outside its supplied role.

### original-audit

**? — Original audit** (inspection scope): Database logs and structured state exist, but complete captured-original provider/OS IO retention was not established in MCP/update/scheduler paths.

### audit-query

**S — Audit query** (source): SQL over an application-defined audit schema supplies retrieval; the service does not itself capture all agent originals.

Evidence: [identity](#evidence-identity).

### hot-change

**L — Hot change** (source): Publish/update supports schema-aware module change with compatibility policy and durable confirmation; manual migration explicitly unimplemented, not arbitrary active-frame refit.

Evidence: [update](#evidence-update), [publish](#evidence-publish).

### rebuild-continuity

**S — Rebuild continuity** (source): Standing tables and recovered scheduled rows can outlive client replacement; agent context/provider identity transfer remains external and module/VM replacement is separate.

Evidence: [schedule](#evidence-schedule).

### remote-services

**S — Remote** (source): HTTP/MCP service with forwarded caller identity and database addressing; client consumes shared state.

Evidence: [mcp](#evidence-mcp), [identity](#evidence-identity).

### self-improvement

**— — Self-improve** (role): self_improvement: this reference supplies persistent database and executable module service; an agent policy, model interface and context lifecycle are outside its supplied role.

### complaints

**— — Complaints** (role): complaints: this reference supplies persistent database and executable module service; an agent policy, model interface and context lifecycle are outside its supplied role.

### authority

**S — Authority** (source): Database ownership governs SQL write authority; reducer calls retain caller identity and application policy.

Evidence: [mcp](#evidence-mcp), [identity](#evidence-identity).

### evaluation

**S — Evaluation** (source): Exact MCP exposure/argument tests and table migration expectations inspected; these do not prove complete IO audit or restart correctness.

Evidence: [tooltest](#evidence-tooltest).

### time-order

**S — Time/order** (source): Persisted schedule timestamps are remapped to runtime Instant on recovery; no consumer paused-time policy inferred.

Evidence: [schedule](#evidence-schedule).

## Inspected test oracles

- [quarantine/spacetimedb/crates/client-api/src/routes/mcp.rs](../../../quarantine/spacetimedb/crates/client-api/src/routes/mcp.rs): Tool discovery and required argument contracts. Oracle: Exact ping/get_schema/sql/call list and SQL required field; does not execute the reducers against storage. Read, **not executed**.
- [quarantine/spacetimedb/crates/engine/src/update.rs](../../../quarantine/spacetimedb/crates/engine/src/update.rs): Dropping table produces schema-change identity. Oracle: Inspected test expects removed table lookup and TableRemoved pending change; limited to automatic migration shape. Read, **not executed**.

## Useful mechanisms

- Direct model-friendly action discovery over persistent queryable service state.
- Persisted schedules and explicit compatibility policy clarify server/client continuity.

## Material limits

- Manual schema migration route is unimplemented in the snapshot; module update is not unrestricted kernel continuity.
- Arconaut refit must preserve/reacquire client identities without governing unrelated database consumers.

## Arconaut design questions

- Can complaints and audit views be independent standing services with direct schema/query access?
- What client handles survive reconnect, and how are rejected or incompatible service updates represented?

## Evidence

### Evidence mcp

[quarantine/spacetimedb/crates/client-api/src/routes/mcp.rs:89–135](../../../quarantine/spacetimedb/crates/client-api/src/routes/mcp.rs#L89): MCP dispatch lists tools; contract preserves caller identity and requires ownership for SQL writes.

### Evidence tools

[quarantine/spacetimedb/crates/client-api/src/routes/mcp.rs:226–271](../../../quarantine/spacetimedb/crates/client-api/src/routes/mcp.rs#L226): Exposed ping/list_databases/get_schema/sql/call dispatch with structured protocol vs in-band execution errors.

### Evidence identity

[quarantine/spacetimedb/crates/client-api/src/routes/mcp.rs:358–409](../../../quarantine/spacetimedb/crates/client-api/src/routes/mcp.rs#L358): SQL forwards identity/auth; reducer tool creates a connection identity and calls module reducer.

### Evidence update

[quarantine/spacetimedb/crates/engine/src/update.rs:133–192](../../../quarantine/spacetimedb/crates/engine/src/update.rs#L133): Automatic/manual migration routes are distinct; manual migration unimplemented and drop transactionality noted.

### Evidence publish

[quarantine/spacetimedb/crates/client-api/src/routes/database.rs:1054–1083](../../../quarantine/spacetimedb/crates/client-api/src/routes/database.rs#L1054): Client-breaking update token/policy and bounded durable-confirmation timeout are publish parameters.

### Evidence schedule

[quarantine/spacetimedb/crates/core/src/host/scheduler.rs:92–142](../../../quarantine/spacetimedb/crates/core/src/host/scheduler.rs#L92): Scheduler recovers rows from standing database, maps persisted schedule timestamps to new Instant delays and row identities.

### Evidence contract

[quarantine/spacetimedb/README.md:117–146](../../../quarantine/spacetimedb/README.md#L117): Module tables/reducers and pushed subscription updates are public role.

### Evidence tooltest

[quarantine/spacetimedb/crates/client-api/src/routes/mcp.rs:447–476](../../../quarantine/spacetimedb/crates/client-api/src/routes/mcp.rs#L447): Test expects exact database-scoped tool list and required SQL argument.

