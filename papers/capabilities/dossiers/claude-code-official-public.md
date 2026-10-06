# claude-code-official-public

Official extension source exposes context middleware, per-action rules and stop continuation; this repository is not the full coding-agent implementation.

Role: official public plugin/mod source and product index. Runtime: TypeScript, Python, shell (extensions only).

Pinned source: [https://github.com/anthropics/claude-code](https://github.com/anthropics/claude-code); revision/version `525d3b35312636cf8c001ecb3df2be0324a48b43`.

Extension callbacks and hook subprocesses; host engine source unavailable in this reference.

Examples consume host APIs; no ownership claim about the closed engine.

Inspection: Read official index, extension catalog, context hook implementation/test, Hookify rule loading and Ralph continuation program.

Limits of this study: Host engine and plugin loader wiring unavailable; tests depend on unavailable claude-code/testing host. No product binary or hook executed.

## Actions

### Context and Read middleware

Surface: extension program.

Input: instructionFiles options, prompt.context and Read event

Result: merged instruction list / additional context

Lifecycle: per request/read; per-agent dedup inherited on fork

Authority: extension program receives host FS/session/env APIs

Evidence: [context](#evidence-context), [read](#evidence-read).

### Hookify editable action rules

Surface: hook subprocess and operator commands.

Input: Markdown rule files and hook JSON stdin

Result: warning/block response or nonblocking diagnostic

Lifecycle: files reloaded at each hook invocation; no restart claimed

Authority: operator/model can author rule files; host hook execution authority remains external

Evidence: [hook](#evidence-hook), [rules](#evidence-rules).

### Ralph stop continuation

Surface: extension stop hook.

Input: state Markdown, iteration limit, completion promise, transcript path

Result: repeat prompt or allow stop

Lifecycle: stop event drives new continuation; state atomically replaced

Authority: operator starts/stops loop; model self-declares promise, no independent performance oracle

Evidence: [ralph](#evidence-ralph).

### /bug

Surface: documented operator command.

Input: operator complaint

Result: product feedback with associated conversation data

Lifecycle: documented submission; capture details unavailable

Authority: operator command; model-wide state capture not established

Evidence: [index](#evidence-index).

## Capabilities

### filesystem

**S — Files** (source): Context plugin uses host ancestor-file APIs; full read/edit/search implementation belongs to unavailable engine.

Evidence: [context](#evidence-context), [read](#evidence-read).

### processes

**D — OS programs** (documentation): Product advertises routine terminal/Git work; supplied hooks are subprocess programs, not the engine process-lifetime implementation.

Evidence: [index](#evidence-index), [hook](#evidence-hook).

### code-actions

**? — Code actions** (inspection scope): Not established for the closed host engine by the inspected public extension/index paths.

### persistent-kernel

**? — Kernel** (inspection scope): Not established for the closed host engine by the inspected public extension/index paths.

### standing-database

**? — Standing DB** (inspection scope): Not established for the closed host engine by the inspected public extension/index paths.

### workflow-programming

**S — Workflows** (source): Commands/agents/MCP/hooks and real stop continuation supply composable extension programs, not a traced full host scheduling language.

Evidence: [catalog](#evidence-catalog), [ralph](#evidence-ralph).

### multi-model

**? — Models** (inspection scope): Not established for the closed host engine by the inspected public extension/index paths.

### live-collaboration

**? — Peer chat** (inspection scope): Not established for the closed host engine by the inspected public extension/index paths.

### concurrent-work

**? — Concurrency** (inspection scope): Not established for the closed host engine by the inspected public extension/index paths.

### steering-interrupt

**? — Steer/interrupt** (inspection scope): Not established for the closed host engine by the inspected public extension/index paths.

### turn-redefinition

**S — Turn program** (source): Middleware changes prompt context and stop hook requests another continuation; does not establish arbitrary replacement of host turn loop.

Evidence: [context](#evidence-context), [ralph](#evidence-ralph).

### compaction

**? — Compaction** (inspection scope): Not established for the closed host engine by the inspected public extension/index paths.

### context-repair

**? — Repair** (inspection scope): Not established for the closed host engine by the inspected public extension/index paths.

### original-audit

**? — Original audit** (inspection scope): Ralph consumes a transcript and README describes feedback retention; neither inspected path establishes full original provider/program IO.

### audit-query

**? — Audit query** (inspection scope): Not established for the closed host engine by the inspected public extension/index paths.

### hot-change

**S — Hot change** (source): Hookify reads editable rules every action and Read checks current env switches; host code/plugin reload boundary remains unavailable.

Evidence: [hook](#evidence-hook), [read](#evidence-read), [rules](#evidence-rules).

### rebuild-continuity

**? — Rebuild continuity** (inspection scope): Not established for the closed host engine by the inspected public extension/index paths.

### remote-services

**? — Remote** (inspection scope): Not established for the closed host engine by the inspected public extension/index paths.

### self-improvement

**L — Self-improve** (source): Ralph repeatedly requests work, with model-written promise/iteration limit; no independent candidate evaluation/promotion mechanism established.

Evidence: [ralph](#evidence-ralph).

### complaints

**D — Complaints** (documentation): Operator /bug reports feedback with conversation data; universal model-invokable state snapshot plus external complaint table is unestablished.

Evidence: [index](#evidence-index).

### authority

**S — Authority** (source): Hookify editable rules can warn/block but errors fail open; host ordinary execution grants and operator bypass defaults unavailable.

Evidence: [hook](#evidence-hook), [rules](#evidence-rules).

### evaluation

**S — Evaluation** (source): Context tests assert exact instruction contents and failed-walk behavior; host testing package unavailable, not an executed engine test.

Evidence: [test](#evidence-test).

### time-order

**? — Time/order** (inspection scope): Not established for the closed host engine by the inspected public extension/index paths.

## Inspected test oracles

- [quarantine/claude-code-official-public/mods/agents-md/tests/register.test.ts](../../../quarantine/claude-code-official-public/mods/agents-md/tests/register.test.ts): instruction precedence and failed ancestor walking Oracle: Exact injected path/content and unchanged context on failed walk; supplied host fixtures, not runtime validation. Read, **not executed**.

## Useful mechanisms

- Public programmable middleware receives context, tools and agent-fork events.
- Action rules are ordinary editable local data with next-use activation.

## Material limits

- Engine unavailable: advertised product features cannot be source-validated here.
- Ralph stopping depends on model text rather than an independent result oracle.

## Arconaut design questions

- Expose composition of turn/context/stop hooks without making hook errors silent.
- Separate next-action extension semantics from Arconaut settled-turn default activation.

## Evidence

### Evidence index

[quarantine/claude-code-official-public/README.md:7–70](../../../quarantine/claude-code-official-public/README.md#L7): Terminal coding-agent product contract and operator /bug with associated conversation feedback; core unavailable.

### Evidence catalog

[quarantine/claude-code-official-public/plugins/README.md:3–59](../../../quarantine/claude-code-official-public/plugins/README.md#L3): Commands, specialized agents, hooks and MCP configurations are public extension surfaces.

### Evidence context

[quarantine/claude-code-official-public/mods/agents-md/hooks/register.ts:40–167](../../../quarantine/claude-code-official-public/mods/agents-md/hooks/register.ts#L40): Source registers session and prompt.context middleware, optionally merges ancestor instructions.

### Evidence read

[quarantine/claude-code-official-public/mods/agents-md/hooks/register.ts:169–280](../../../quarantine/claude-code-official-public/mods/agents-md/hooks/register.ts#L169): Fork inherits seen-instruction state; Read adds unseen nested instructions and reads attachment switches every action.

### Evidence hook

[quarantine/claude-code-official-public/plugins/hookify/hooks/pretooluse.py:35–70](../../../quarantine/claude-code-official-public/plugins/hookify/hooks/pretooluse.py#L35): Every hook loads rules, evaluates input and emits JSON; hook errors permit operation with a diagnostic.

### Evidence rules

[quarantine/claude-code-official-public/plugins/hookify/README.md:18–128](../../../quarantine/claude-code-official-public/plugins/hookify/README.md#L18): Documented editable rule shape and next-tool-use activation, warnings/blocks and event scopes.

### Evidence ralph

[quarantine/claude-code-official-public/plugins/ralph-wiggum/hooks/stop-hook.sh:50–177](../../../quarantine/claude-code-official-public/plugins/ralph-wiggum/hooks/stop-hook.sh#L50): Stop callback reads last transcript assistant text, compares model-written completion promise or iteration budget, updates state and blocks stop with same prompt.

### Evidence test

[quarantine/claude-code-official-public/mods/agents-md/tests/register.test.ts:17–87](../../../quarantine/claude-code-official-public/mods/agents-md/tests/register.test.ts#L17): Tests assert exact context injection, no double insertion for CLAUDE.md and failed-walk unchanged context; host is fixture supplied.

