# open-interpreter

Current Rust Open Interpreter distribution emulates provider-recommended harness surfaces on a Codex-derived core; distinct from the Python community lineage.

Role: coding agent. Runtime: Rust, JavaScript (optional V8 code actions).

Pinned source: [https://github.com/openinterpreter/openinterpreter](https://github.com/openinterpreter/openinterpreter); revision/version `9acdb707400234bebe31e672b808235495916b95`.

Codex-derived Tokio session/turn/executor runtime with Rust-native harness-specific request/tool adapters; optional feature-gated V8 cell runtime and ACP/app-server clients.

Owns local sessions, terminal tasks, adapters and scheduled cron state; consumes model/MCP/remote executor services; does not execute the original Python Open Interpreter runtime.

Inspection: Fork identity; own submission/turn continuation and request transport routes; full alias dispatch catalog plus concrete shell/task/context/agent/cron actions; byte-identical inherited handler/audit files verified against inspected Codex; routing and shell oracle.

Limits of this study: Source read only, reference not executed. No reference execution. Adapter names are harness emulation, not adoption/equivalence to upstream products. ReadSessionContext alias is explicitly unavailable; TaskStop emits Ctrl-C then announces success without proving process death. Optional V8 build availability is not distribution runtime validation.

## Actions

### exec_command; write_stdin; apply_patch

Surface: model tool.

Input: Native command/session ID/patch/environment arguments

Result: Terminal output/exit/ongoing handle or patch results

Lifecycle: Native executor and cancellable task lifecycle inherited; direct schema depends on harness

Authority: Model with standing sandbox/approval profile

Evidence: [e6](#evidence-e6), [e7](#evidence-e7), [e12](#evidence-e12).

### Bash; bash; Read/read; Write/write; Edit/edit; Glob/glob; Grep/grep; ReadMediaFile

Surface: model tool.

Input: Harness-specific named command/path/content/offset/pattern schemas

Result: Adapter-shaped file/text/media/error results

Lifecycle: Awaited adapters; Bash background returns task ID/output path

Authority: Model with native underlying execution authority; emulated surface, not original upstream program

Evidence: [e6](#evidence-e6), [e7](#evidence-e7).

### Agent; Task/task; TaskList; TaskOutput; TaskStop

Surface: model tool.

Input: Description/prompt/model/role/background; task_id

Result: Child IDs/results; background process output; Ctrl-C acknowledgement

Lifecycle: Child spawn or task registry; stop acknowledgement does not prove death

Authority: Model; named native handler translations

Evidence: [e6](#evidence-e6), [e8](#evidence-e8), [e9](#evidence-e9).

### TodoRead/TodoWrite; checklist_add/list/update/write; EnterPlanMode; ExitPlanMode

Surface: model tool.

Input: Typed todos/checklist; plan text

Result: Checklist/todo state or plan guidance

Lifecycle: In-memory/adapter state; plan methods return guidance text rather than enforcing read-only transition

Authority: Model; surface selected by harness

Evidence: [e6](#evidence-e6), [e10](#evidence-e10).

### ReadSessionContext

Surface: model tool.

Input: sessionId/query/strategy/maxTokens

Result: Unavailable-for-provider-transport error

Lifecycle: No working dispatcher retrieval in inspected handler despite prompt builder/tests

Authority: Model advertised alias; limitation explicit

Evidence: [e10](#evidence-e10).

### CronCreate; CronDelete; CronList

Surface: model tool.

Input: cron expression,prompt,recurring; id

Result: Created/listed/deleted cron rows

Lifecycle: Session-owned standing scheduled prompt triggers

Authority: Model under Kimi Code surface

Evidence: [e11](#evidence-e11).

### spawn_agent; send_message; followup_task; list_agents; wait_agent; interrupt_agent

Surface: model tool.

Input: Native child/target/message/fork/model/time arguments

Result: Agent status/mail/results

Lifecycle: Independent child loops; message queue versus turn triggering distinct

Authority: Configured model delegation rules

Evidence: [e9](#evidence-e9), [e13](#evidence-e13), [e14](#evidence-e14).

### /harness; /model; interpreter acp/exec

Surface: operator command.

Input: Named compiled harness/provider/model; prompt or ACP JSON-RPC

Result: Configuration/UI or protocol events

Lifecycle: Runtime config next-step activation; no user-code strategy compilation

Authority: Operator/program client

Evidence: [e1](#evidence-e1), [e4](#evidence-e4), [e15](#evidence-e15), [e20](#evidence-e20).

### functions.exec/wait (optional code mode)

Surface: model tool.

Input: Raw JS; yield/cell ID/terminate

Result: Text/media or yielded cell results

Lifecycle: Feature-gated fresh V8 isolate with explicit serialized session storage

Authority: Nested enabled tool authority; no persistent Python namespace

Evidence: [e16](#evidence-e16).

## Capabilities

### filesystem

**I — Files** (source): Read/write/edit/glob/grep/media aliases dispatch native FS handlers; patch/native shell also available.

Evidence: [e6](#evidence-e6).

### processes

**I — OS programs** (source): Native exec/stdin and adapter background task IDs/output paths; TaskStop semantics are limited to Ctrl-C acknowledgement.

Evidence: [e7](#evidence-e7), [e8](#evidence-e8), [e12](#evidence-e12).

### code-actions

**I — Code actions** (source): Optional feature-gated V8 code-mode runtime; ordinary shell strings and aliases are separate actions.

Evidence: [e16](#evidence-e16).

### persistent-kernel

**L — Kernel** (source): Optional V8 uses fresh isolate per cell and explicit stored values; no original Python persistent namespace in this lineage.

Evidence: [e1](#evidence-e1), [e16](#evidence-e16).

### standing-database

**S — Standing DB** (source): Task/todo/checklist registries and durable cron rows are structured state primitives, not standing shared evidence database.

Evidence: [e6](#evidence-e6), [e7](#evidence-e7), [e11](#evidence-e11).

### workflow-programming

**S — Workflows** (source): Compiled request shaping/response adapters plus code-mode composition; names can switch, arbitrary model-authored turn workflow replacement not established.

Evidence: [e4](#evidence-e4), [e5](#evidence-e5), [e16](#evidence-e16), [e20](#evidence-e20).

### multi-model

**I — Models** (source): Provider wire routes/model switches and per-Agent model fields; emulated harness selector distinct from provider.

Evidence: [e4](#evidence-e4), [e9](#evidence-e9), [e20](#evidence-e20).

### live-collaboration

**I — Peer chat** (source): Inherited V2 queue-only addressed messages and trigger-turn follow-ups; own turn boundary accepts mailbox before continuation.

Evidence: [e3](#evidence-e3), [e13](#evidence-e13), [e14](#evidence-e14).

### concurrent-work

**I — Concurrency** (source): Background terminal task IDs and native child sessions; model loop waits its own sampling/tool batch.

Evidence: [e3](#evidence-e3), [e7](#evidence-e7), [e9](#evidence-e9), [e12](#evidence-e12).

### steering-interrupt

**L — Steer/interrupt** (source): Core submission interruption and boundary mailbox delivery exist; adapter TaskStop announces success on Ctrl-C write without confirming exit.

Evidence: [e2](#evidence-e2), [e3](#evidence-e3), [e8](#evidence-e8).

### turn-redefinition

**S — Turn program** (source): Selectable compiled harnesses alter request/tools/postprocessing; active next-step configuration, not arbitrary interpreted scheduling replacement.

Evidence: [e4](#evidence-e4), [e5](#evidence-e5), [e15](#evidence-e15), [e20](#evidence-e20).

### compaction

**S — Compaction** (source): Inherited turn pre-sampling compaction and context-window rollover; harness-specific summary prompts/formatting. Detailed per-harness replacement strategy not fully traced.

Evidence: [e3](#evidence-e3), [e5](#evidence-e5), [e18](#evidence-e18).

### context-repair

**L — Repair** (source): ReadSessionContext alias explicitly errors; cleaned-transcript prompt helper does not make exposed repair implementation.

Evidence: [e10](#evidence-e10).

### original-audit

**L — Original audit** (source): Inherited rollout/optional trace captures selected payloads; raw inference stream deltas explicitly absent, adapter shaping further transforms tool outputs.

Evidence: [e7](#evidence-e7), [e17](#evidence-e17), [e18](#evidence-e18).

### audit-query

**S — Audit query** (source): Readable retained rollout and adapter task output files support program inspection; working model ReadSessionContext lookup absent.

Evidence: [e7](#evidence-e7), [e10](#evidence-e10), [e18](#evidence-e18).

### hot-change

**I — Hot change** (source): Operator harness/model changes validated for next step; strategy implementations remain compiled Rust, no executable hot refit.

Evidence: [e15](#evidence-e15), [e20](#evidence-e20).

### rebuild-continuity

**? — Rebuild continuity** (source): Inherited transcript/thread persistence exists; actual self-compile/outpost/quiescent executable handoff not established in inspected paths.

### remote-services

**I — Remote** (source): Wire routes consume compatible model APIs; inherited executor/MCP protocols and ACP clients provide service boundaries, not remote-governance plane.

Evidence: [e1](#evidence-e1), [e4](#evidence-e4), [e12](#evidence-e12).

### self-improvement

**? — Self-improve** (source): Model file-editing and skill aliases are present; no dedicated executable experiment/evaluate/refit governance traced.

### complaints

**? — Complaints** (source): No model state-capturing complaint action established in inspected compiled action catalog and session paths.

### authority

**I — Authority** (source): Bash adapters translate escalation requests and underlying exec permissions; plan-mode aliases return instructions rather than enforced authority changes.

Evidence: [e7](#evidence-e7), [e10](#evidence-e10), [e12](#evidence-e12).

### evaluation

**I — Evaluation** (source): Routing tests assert supported/rejected wire-harness combinations; mock SSE shell test inspects exact second-request tool output. These do not establish vendor-harness behavioral equivalence.

Evidence: [e4](#evidence-e4), [e19](#evidence-e19).

### time-order

**I — Time/order** (source): Cron expression/prompt lifecycle, task IDs and core turn/call identities; not shared causal original ledger.

Evidence: [e7](#evidence-e7), [e11](#evidence-e11), [e15](#evidence-e15).

## Inspected test oracles

- [quarantine/open-interpreter/codex-rs/core/src/harness/routing.rs](../../../quarantine/open-interpreter/codex-rs/core/src/harness/routing.rs): Wrong wire/harness routing Oracle: Exact routing enum and unsupported messages-wire error assertions; cannot prove model performance or complete vendor parity. Read, **not executed**.
- [quarantine/open-interpreter/codex-rs/core/tests/suite/tool_harness.rs](../../../quarantine/open-interpreter/codex-rs/core/tests/suite/tool_harness.rs): Tool result continuation transport Oracle: Mock SSE emits shell call; next captured provider request must carry exit0/output text; skipped without local networking and not run here. Read, **not executed**.

## Useful mechanisms

- Provider-specific ergonomics change tool schemas, request shaping and response interpretation on owned native core.
- Portable ACP/exec interfaces coexist with harness adapters.

## Material limits

- No reference execution. Adapter names are harness emulation, not adoption/equivalence to upstream products. ReadSessionContext alias is explicitly unavailable; TaskStop emits Ctrl-C then announces success without proving process death. Optional V8 build availability is not distribution runtime validation.

## Arconaut design questions

- How much turn-program variation should be genuinely model editable versus compiled named strategies?
- Can our action success oracles distinguish acknowledgement from actual process death?
- How should unavailable advertised context actions be caught before active model use?

## Evidence

### Evidence e1

[quarantine/open-interpreter/README.md:48–105](../../../quarantine/open-interpreter/README.md#L48): Fork identifies harness emulation and ACP/exec compatibility; Rust lineage distinct from Python successor.

### Evidence e2

[quarantine/open-interpreter/codex-rs/core/src/session/handlers.rs:413–439](../../../quarantine/open-interpreter/codex-rs/core/src/session/handlers.rs#L413): Submission loop accepts interruption independently of turn tasks.

### Evidence e3

[quarantine/open-interpreter/codex-rs/core/src/session/turn.rs:537–571](../../../quarantine/open-interpreter/codex-rs/core/src/session/turn.rs#L537): After sampling/tools it accepts mailbox input and continues for tool requests or pending steering.

### Evidence e4

[quarantine/open-interpreter/codex-rs/core/src/harness/routing.rs:57–155](../../../quarantine/open-interpreter/codex-rs/core/src/harness/routing.rs#L57): Wire API/harness combination selects native/chat/messages transport or explicit unsupported error.

### Evidence e5

[quarantine/open-interpreter/codex-rs/core/src/harness/request.rs:46–137](../../../quarantine/open-interpreter/codex-rs/core/src/harness/request.rs#L46): Selected compiled strategy shapes prompts/request tools and response postprocessing.

### Evidence e6

[quarantine/open-interpreter/codex-rs/core/src/tools/handlers/harness_aliases.rs:151–260](../../../quarantine/open-interpreter/codex-rs/core/src/tools/handlers/harness_aliases.rs#L151): Native catalog and real alias dispatch for file/shell/agent/task/todo/plan/context operations.

### Evidence e7

[quarantine/open-interpreter/codex-rs/core/src/tools/handlers/harness_aliases.rs:553–642](../../../quarantine/open-interpreter/codex-rs/core/src/tools/handlers/harness_aliases.rs#L553): Bash maps command/background to native exec, stores task ID/output path and shapes output by harness.

### Evidence e8

[quarantine/open-interpreter/codex-rs/core/src/tools/handlers/harness_aliases.rs:1161–1196](../../../quarantine/open-interpreter/codex-rs/core/src/tools/handlers/harness_aliases.rs#L1161): TaskStop writes Ctrl-C and reports successful stop without checking returned process death.

### Evidence e9

[quarantine/open-interpreter/codex-rs/core/src/tools/handlers/harness_aliases.rs:297–343](../../../quarantine/open-interpreter/codex-rs/core/src/tools/handlers/harness_aliases.rs#L297): Agent tool translates model/role/background request into native child spawn.

### Evidence e10

[quarantine/open-interpreter/codex-rs/core/src/tools/handlers/harness_aliases.rs:2566–2610](../../../quarantine/open-interpreter/codex-rs/core/src/tools/handlers/harness_aliases.rs#L2566): Plan mode returns guidance strings; ReadSessionContext handler explicitly unavailable.

### Evidence e11

[quarantine/open-interpreter/codex-rs/core/src/tools/handlers/kimi_code_cron.rs:18–92](../../../quarantine/open-interpreter/codex-rs/core/src/tools/handlers/kimi_code_cron.rs#L18): CronCreate/Delete/List mutate session-owned scheduler through awaited handler.

### Evidence e12

[quarantine/open-interpreter/codex-rs/core/src/tools/handlers/unified_exec/write_stdin.rs:58–122](../../../quarantine/open-interpreter/codex-rs/core/src/tools/handlers/unified_exec/write_stdin.rs#L58): Ongoing session-ID terminal IO with yield/output limits and permission errors; byte-identical inspected inherited implementation.

### Evidence e13

[quarantine/open-interpreter/codex-rs/core/src/tools/handlers/multi_agents_v2/send_message.rs:32–52](../../../quarantine/open-interpreter/codex-rs/core/src/tools/handlers/multi_agents_v2/send_message.rs#L32): Inherited V2 queue-only send handler.

### Evidence e14

[quarantine/open-interpreter/codex-rs/core/src/tools/handlers/multi_agents_v2/followup_task.rs:32–52](../../../quarantine/open-interpreter/codex-rs/core/src/tools/handlers/multi_agents_v2/followup_task.rs#L32): Inherited V2 trigger-turn follow-up handler.

### Evidence e15

[quarantine/open-interpreter/codex-rs/core/src/session/step_activation.rs:287–336](../../../quarantine/open-interpreter/codex-rs/core/src/session/step_activation.rs#L287): Task/snapshot identity checked before atomic next-step configuration activation.

### Evidence e16

[quarantine/open-interpreter/codex-rs/code-mode-runtime/src/runtime/mod.rs:221–264](../../../quarantine/open-interpreter/codex-rs/code-mode-runtime/src/runtime/mod.rs#L221): Optional v8-runtime feature starts fresh isolate; state includes serialized stored values.

### Evidence e17

[quarantine/open-interpreter/codex-rs/rollout-trace/src/inference.rs:1–96](../../../quarantine/open-interpreter/codex-rs/rollout-trace/src/inference.rs#L1): Inherited optional request-attempt traces explicitly omit raw stream deltas.

### Evidence e18

[quarantine/open-interpreter/codex-rs/rollout/src/policy.rs:10–65](../../../quarantine/open-interpreter/codex-rs/rollout/src/policy.rs#L10): Inherited retained transcript/checkpoint/tool-response persistence filters unknown items.

### Evidence e19

[quarantine/open-interpreter/codex-rs/core/tests/suite/tool_harness.rs:65–136](../../../quarantine/open-interpreter/codex-rs/core/tests/suite/tool_harness.rs#L65): Mock SSE tool-call then second request oracle checks actual shell output format and successful continuation.

### Evidence e20

[quarantine/open-interpreter/codex-rs/tui/src/app/event_dispatch.rs:2530–2574](../../../quarantine/open-interpreter/codex-rs/tui/src/app/event_dispatch.rs#L2530): Operator harness/model selection stored in config and UI; entry point for changing compiled strategy.

