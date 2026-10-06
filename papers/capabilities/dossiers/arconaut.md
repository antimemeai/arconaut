# arconaut

Useful small interfaces and fault counterexamples; exposed implementation does not establish a working coding harness or current baseline.

Role: historical coding-agent prototype. Runtime: Rust.

Pinned source: [https://github.com/antimemeai/arconaut](https://github.com/antimemeai/arconaut); revision/version `3bf2056565eff537b380f4fd649d725f36d1c493`.

CLI/TUI uses old Soul sequential provider/tool loop under Tokio; newer reactor turn method is a separate stub.

Inherited local shell/tool ownership and compiled policies; historical reference only, no current reconstruction authority.

Inspection: Re-read wired CLI/Soul/provider conversion/tool paths, bus/hooks/context/compactor/logger/utility time and selected tests; consulted prior independent assessment.

Limits of this study: No inherited code executed this study. Earlier 2026-09-29 build/70 selected tests/probes are historical evidence in the separate oracle report; no live provider conformance established.

## Actions

### read / write / edit / grep

Surface: model tools.

Input: Path, text/range or pattern

Result: File text/edit confirmation/matches/errors

Lifecycle: Awaited fixed tools; turn-wide identical-call cache can stale results.

Authority: Model under host filesystem authority

Evidence: [wiring](#evidence-wiring), [files](#evidence-files), [write](#evidence-write), [edit](#evidence-edit), [dedup](#evidence-dedup).

### bash

Surface: model tool.

Input: Single command string and timeout_ms

Result: Captured lossy stdout/stderr and exit code

Lifecycle: Awaited; timeout returns error without established child termination.

Authority: Model; composition syntax blanket forbidden

Evidence: [bash](#evidence-bash).

### terminal_send

Surface: model tool.

Input: Terminal input string

Result: Shared shell buffer via reply channel

Lifecycle: Owner wait cycle in wired TUI; 200ms treated as completion.

Authority: Model TUI tool

Evidence: [bridge](#evidence-bridge), [tui](#evidence-tui).

### bus broadcast / whisper / presence

Surface: model tool.

Input: Topic/recipient/message or presence name

Result: Send/presence text

Lifecycle: No traced incoming-message continuation path in main turn.

Authority: Model send surface

Evidence: [bus](#evidence-bus), [tui](#evidence-tui).

### uuid / timestamp / hash / base64 / JSON utility family

Surface: model tools.

Input: No-arg ID/time or input data

Result: Text values

Lifecycle: Fixed registered tools; cached within turn by name/args.

Authority: Model utility calls

Evidence: [time](#evidence-time), [dedup](#evidence-dedup).

## Capabilities

### filesystem

**L — Files** (source): Read/write/edit implementations are exposed, but unconditional turn-wide dedup makes read after write and identical retries stale; no purity contract.

Evidence: [wiring](#evidence-wiring), [files](#evidence-files), [write](#evidence-write), [edit](#evidence-edit), [dedup](#evidence-dedup).

### processes

**L — OS programs** (source): Bash capture is exposed with timeout/composition blacklist; terminal bridge has actual owner wait cycle and 200ms fake completion boundary.

Evidence: [bash](#evidence-bash), [bridge](#evidence-bridge), [tui](#evidence-tui).

### code-actions

**L — Code actions** (source): Model can author shell command string, but banned operators prevent ordinary multi-command/pipeline composition; no wired general persistent code action kernel.

Evidence: [bash](#evidence-bash).

### persistent-kernel

**L — Kernel** (source): Persistent piped shell exists for TUI, but terminal_send waits on its own blocked turn owner; not an established usable language kernel.

Evidence: [tui](#evidence-tui), [bridge](#evidence-bridge).

### standing-database

**? — Standing DB** (inspection scope): standing_database: not established in re-read CLI/Soul/provider/tool/context/audit paths; other compiled historical proposals do not establish active capability.

### workflow-programming

**S — Workflows** (source): Compiled Hook/old Soul APIs are useful small orchestration interfaces; model cannot redefine its turn programs through these exposed tools.

Evidence: [hook](#evidence-hook), [loop](#evidence-loop).

### multi-model

**L — Models** (source): Selectable adapters exist, but compatible tool response is TODO and Anthropic request loses tool protocol/endpoint slash; no working multi-provider coding conversation established.

Evidence: [anthropic](#evidence-anthropic), [endpoint](#evidence-endpoint), [compat](#evidence-compat).

### live-collaboration

**L — Peer chat** (source): Bus has addressed send/presence surface, but wired TUI does not consume peer messages into active/queued turns; separate inbox proposals do not establish one collaboration lifecycle.

Evidence: [bus](#evidence-bus), [tui](#evidence-tui).

### concurrent-work

**L — Concurrency** (source): Whole turn serially awaits tools and blocks select servicing bridge/user commands; runtime tasks alone do not establish useful concurrent agent work.

Evidence: [loop](#evidence-loop), [tui](#evidence-tui).

### steering-interrupt

**L — Steer/interrupt** (source): Interrupt only posts Interrupted status in select command handler; that handler cannot run while awaited Soul turn is active.

Evidence: [tui](#evidence-tui).

### turn-redefinition

**L — Turn program** (source): Fixed compiled old loop is wired; newer reactor continuation remains a stub, no model-authored hot turn policy.

Evidence: [loop](#evidence-loop), [reactor](#evidence-reactor).

### compaction

**L — Compaction** (source): An unconfigured compactor replaces old content with counts only, clears checkpoints and slices by message count; no semantic preservation or pairing guarantee.

Evidence: [compact](#evidence-compact), [wiring](#evidence-wiring), [tui](#evidence-tui).

### context-repair

**L — Repair** (source): Checkpoints store lengths/counts; revert cannot reconstruct deleted originals, and clear leaves checkpoints permitting inconsistent token count.

Evidence: [context](#evidence-context), [compact](#evidence-compact).

### original-audit

**L — Original audit** (source): Hook logger omits provider requests/failures, transforms error output to brief, dedup skips hooks and logger errors are swallowed; not comprehensive originals.

Evidence: [audit](#evidence-audit), [logger](#evidence-logger), [dedup](#evidence-dedup), [loop](#evidence-loop).

### audit-query

**? — Audit query** (inspection scope): Append files exist, but no model-facing original audit query/retrieval tool established in wired registry/hook paths.

### hot-change

**? — Hot change** (inspection scope): No active-program generation/activation protocol established in wired CLI/fixed Hook/Soul paths; source editing alone is not hot change.

### rebuild-continuity

**? — Rebuild continuity** (inspection scope): No outpost/quiescence/build/activation/re-inhabitation path established; historical runtime stubs confer no continuity.

### remote-services

**? — Remote** (inspection scope): remote_services: not established in re-read CLI/Soul/provider/tool/context/audit paths; other compiled historical proposals do not establish active capability.

### self-improvement

**? — Self-improve** (inspection scope): No dedicated live harness experiment/evaluation/governance loop established; legacy source-edit tools are insufficient.

### complaints

**? — Complaints** (inspection scope): complaints: not established in re-read CLI/Soul/provider/tool/context/audit paths; other compiled historical proposals do not establish active capability.

### authority

**L — Authority** (source): No routine command approval loop shown, but blanket forbidden shell syntax and fixed exposed tools materially limit expert/model programming.

Evidence: [bash](#evidence-bash), [wiring](#evidence-wiring).

### evaluation

**L — Evaluation** (source): Local mock checks ID pairing, while compaction recent-window test never triggers and endpoint test checks only string trimming; earlier passing selected tests do not validate broken boundaries.

Evidence: [test](#evidence-test), [compact](#evidence-compact), [endpoint](#evidence-endpoint).

### time-order

**L — Time/order** (source): UTC timestamp/UUID utilities exposed, but no-argument timestamp is deduplicated within turn, returning stale time; checkpoint wall timestamps do not establish monotonic/refit ordering.

Evidence: [time](#evidence-time), [dedup](#evidence-dedup), [context](#evidence-context).

## Inspected test oracles

- [quarantine/arconaut/crates/arconaut-agent/src/soul.rs](../../../quarantine/arconaut/crates/arconaut-agent/src/soul.rs): Sequential tool-call/result identity under mock provider. Oracle: Asserts two steps and matching call-1 result, while MockProvider ignores actual outbound request; not provider roundtrip oracle. Read, **not executed**.
- [quarantine/arconaut/crates/arconaut-agent/src/compaction.rs](../../../quarantine/arconaut/crates/arconaut-agent/src/compaction.rs): Recent-window preservation. Oracle: Fixture ten msgN messages gives 10 estimated tokens against 50 threshold; compact return ignored, so expected msg7 can pass without transformation. Read, **not executed**.
- [quarantine/arconaut/crates/arconaut-machine/src/providers/anthropic.rs](../../../quarantine/arconaut/crates/arconaut-machine/src/providers/anthropic.rs): Endpoint slash construction. Oracle: Inspected test checks trimmed base property rather than actual generated request URL and notes defective concatenation; cannot find endpoint failure. Read, **not executed**.

## Useful mechanisms

- Small explicit tool/message/context interfaces and local byte-edit tests provide bounded research examples.
- Broken ownership, stale dedup and inert oracle cases are useful discriminating counterexamples.

## Material limits

- Historical implementation remains quarantined, not adopted baseline.
- A build and green local suites do not establish a usable whole harness, complete audit or native rebuild continuity.

## Arconaut design questions

- Which tool effects may ever be reused, and what explicit purity/freshness policy would justify it?
- Actual wired-loop tests must observe cancellation, tool settlement, provider roundtrip bytes and original audit across failure paths.
- Should all time observations bypass result reuse, and how does refit preserve time-domain identity?

## Evidence

### Evidence wiring

[quarantine/arconaut/crates/arconaut-cli/src/main.rs:431–458](../../../quarantine/arconaut/crates/arconaut-cli/src/main.rs#L431): CLI registers fixed file/shell/skill/utility tools and Soul limits/intervention; no compactor or injector configured.

### Evidence tui

[quarantine/arconaut/crates/arconaut-cli/src/main.rs:592–679](../../../quarantine/arconaut/crates/arconaut-cli/src/main.rs#L592): TUI registers bridge/bus and awaits whole turn in same select that must service bridge; interrupt only sends status after this await.

### Evidence loop

[quarantine/arconaut/crates/arconaut-agent/src/soul.rs:127–224](../../../quarantine/arconaut/crates/arconaut-agent/src/soul.rs#L127): Old loop calls provider, appends result, sequentially dispatches tools and continues until no calls or step limit.

### Evidence dedup

[quarantine/arconaut/crates/arconaut-agent/src/soul.rs:282–299](../../../quarantine/arconaut/crates/arconaut-agent/src/soul.rs#L282): All matching name/argument results reuse turn cache before hooks or actual execution, without purity/freshness contract.

### Evidence anthropic

[quarantine/arconaut/crates/arconaut-machine/src/providers/anthropic.rs:292–329](../../../quarantine/arconaut/crates/arconaut-machine/src/providers/anthropic.rs#L292): Outbound conversion drops tool calls and flattens tool-result identity/error to user text.

### Evidence endpoint

[quarantine/arconaut/crates/arconaut-machine/src/providers/anthropic.rs:91–104](../../../quarantine/arconaut/crates/arconaut-machine/src/providers/anthropic.rs#L91): Messages endpoint concatenates after stripping slash, yielding v1messages for default base.

### Evidence compat

[quarantine/arconaut/crates/arconaut-machine/src/providers/openai_compat.rs:180–192](../../../quarantine/arconaut/crates/arconaut-machine/src/providers/openai_compat.rs#L180): Compatible response parser handles text and leaves tool_calls parsing TODO.

### Evidence files

[quarantine/arconaut/crates/arconaut-machine/src/tools.rs:14–76](../../../quarantine/arconaut/crates/arconaut-machine/src/tools.rs#L14): Read tool loads text then returns line subset.

### Evidence write

[quarantine/arconaut/crates/arconaut-machine/src/tools.rs:122–154](../../../quarantine/arconaut/crates/arconaut-machine/src/tools.rs#L122): Write creates parent directories and writes exact content.

### Evidence edit

[quarantine/arconaut/crates/arconaut-machine/src/tools.rs:201–263](../../../quarantine/arconaut/crates/arconaut-machine/src/tools.rs#L201): Edit rejects no/ambiguous matches then replaces and writes text.

### Evidence bash

[quarantine/arconaut/crates/arconaut-machine/src/tools.rs:298–381](../../../quarantine/arconaut/crates/arconaut-machine/src/tools.rs#L298): Bash forbids composition syntax then captures output under await timeout without kill_on_drop handling.

### Evidence bridge

[quarantine/arconaut/crates/arconaut-cli/src/terminal_bridge.rs:51–69](../../../quarantine/arconaut/crates/arconaut-cli/src/terminal_bridge.rs#L51): terminal_send sends channel request then awaits oneshot reply.

### Evidence bus

[quarantine/arconaut/crates/arconaut-agent/src/bus_tool.rs:15–101](../../../quarantine/arconaut/crates/arconaut-agent/src/bus_tool.rs#L15): Model can broadcast/whisper/query presence; delivery-to-turn consumption not wired in these paths.

### Evidence hook

[quarantine/arconaut/crates/arconaut-agent/src/hooks.rs:1–54](../../../quarantine/arconaut/crates/arconaut-agent/src/hooks.rs#L1): Compiled Hook trait registers turn/tool callbacks, not model-defined continuation program.

### Evidence audit

[quarantine/arconaut/crates/arconaut-agent/src/hooks.rs:112–156](../../../quarantine/arconaut/crates/arconaut-agent/src/hooks.rs#L112): Audit observes hooks; tool errors retain brief, no provider originals; dedup bypasses hooks.

### Evidence logger

[quarantine/arconaut/crates/arconaut-audit/src/logger.rs:15–63](../../../quarantine/arconaut/crates/arconaut-audit/src/logger.rs#L15): Append logger flushes but returns void after printing IO/serialization errors; not all-outcome completeness.

### Evidence context

[quarantine/arconaut/crates/arconaut-core/src/context.rs:55–109](../../../quarantine/arconaut/crates/arconaut-core/src/context.rs#L55): Checkpoints retain lengths/counts only; revert truncates and top-level text-only estimation omits nested outputs.

### Evidence compact

[quarantine/arconaut/crates/arconaut-agent/src/compaction.rs:36–77](../../../quarantine/arconaut/crates/arconaut-agent/src/compaction.rs#L36): Compactor deletes old messages and inserts only message/token count, not semantic summary; CLI path does not install it.

### Evidence reactor

[quarantine/arconaut/crates/arconaut-reactor/src/logic.rs:59–78](../../../quarantine/arconaut/crates/arconaut-reactor/src/logic.rs#L59): New reactor run_turn is explicitly unimplemented.

### Evidence time

[quarantine/arconaut/crates/arconaut-cli/src/utils.rs:12–75](../../../quarantine/arconaut/crates/arconaut-cli/src/utils.rs#L12): Registered timestamp returns current UTC and uuid returns random ID; these fixed no-arg actions meet the turn cache.

### Evidence test

[quarantine/arconaut/crates/arconaut-agent/src/soul.rs:394–438](../../../quarantine/arconaut/crates/arconaut-agent/src/soul.rs#L394): Local mock tool test asserts call/result ID pairing, not actual provider second request.

