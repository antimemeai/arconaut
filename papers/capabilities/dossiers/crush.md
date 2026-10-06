# crush

Go coding agent whose useful systems mechanisms include accepted-run cancellation identities, per-step queued steering, background shell handles, config rollback and external push channels.

Role: coding agent. Runtime: Go.

Pinned source: [https://github.com/charmbracelet/crush](https://github.com/charmbracelet/crush); revision/version `76cc5c574e15072b15aaed0f4f843a5711fae0d9`.

Go goroutines/context cancellation; Fantasy dependency handles model/tool loop; per-execution shell interpreter and global background job registry; CLI/server/TUI surfaces.

Owns session SQLite state/background shells/task sessions/LSP; consumes model APIs and MCP services/channels.

Inspection: Entry/coordinator/Fantasy stream callbacks, native registration and file/process actions, accepted run/queue/cancel semantics, task sessions, config/hook mutation, summary retention, logs and external channel routing; read relevant fault tests.

Limits of this study: Source read only, reference not executed. Fantasy dependency scheduler internals not populated/read. Full network/LSP actions and shell exec isolation not exhaustively traced. No persistent code notebook kernel, native peer room, executable rebuild continuity, captured-state complaint or measured autoresearch established.

## Actions

### view / write / edit / multiedit / glob / grep / ls

Surface: model tools.

Input: File/range/content or exact edit/search parameters

Result: Text/images, changes/diff metadata, matches or error

Lifecycle: Awaited with filetracker/history/LSP; write requires reread after observed mtime change

Authority: Allowed agent tools and per-action permission; metadata time check is not byte-atomic file reservation

Evidence: [e5](#evidence-e5), [e6](#evidence-e6), [e7](#evidence-e7).

### bash / job_output / job_kill

Surface: model tools.

Input: Command/cwd/background/auto-background threshold; shell_id + wait

Result: Output and background ShellID; status/error, cancellation result

Lifecycle: Fresh shell per execution; foreground can become detached job after default 60s; process-global registry with no traced per-session handle guard

Authority: Prompt skip/allowlist exists but built-in command blockers remain

Evidence: [e8](#evidence-e8), [e9](#evidence-e9), [e10](#evidence-e10), [e11](#evidence-e11), [e12](#evidence-e12), [e26](#evidence-e26).

### agent / agentic_fetch

Surface: model tools.

Input: Task prompt or agentic web-fetch parameters

Result: Independent child-session text and parent cost; no live messaging handle

Lifecycle: Parallel-designated prompt tool awaited; child uses configured host task agent

Authority: Native agent prompt has no per-call provider/model selection; question/hooks restricted in child

Evidence: [e5](#evidence-e5), [e13](#evidence-e13), [e14](#evidence-e14).

### fetch / web_fetch / web_search / download / sourcegraph / list_mcp_resources / read_mcp_resource

Surface: model tool catalog and external adapter.

Input: URL/query/destination/resource/server schema

Result: Content/search/download/resource results

Lifecycle: Network/MCP tools use per-tool permission and discovered adapter

Authority: Configured allowed native/MCP tools; individual network implementations not all traced

Evidence: [e5](#evidence-e5), [e29](#evidence-e29).

### lsp_diagnostics / lsp_references / lsp_restart / lsp_symbols / lsp_definition / lsp_call_hierarchy / lsp_rename / lsp_replace_symbol

Surface: model tool catalog.

Input: File/symbol/location or edit parameters

Result: Language-server diagnostics/navigation/mutation

Lifecycle: Enabled configured/automatic LSP lifecycle

Authority: Agent AllowedTools and mutation permissions; catalog rather than every LSP implementation traced

Evidence: [e5](#evidence-e5).

### todos / question / crush_info / crush_logs

Surface: model tools.

Input: TODO updates, interactive question, introspection, bounded recent-log line count

Result: Saved plan items/user answer/config info or redacted logs

Lifecycle: Session TODO persists; question operator UI only and absent from child; logs read saved file

Authority: Model according to agent catalog; logs caps and omission prevent original audit completeness

Evidence: [e5](#evidence-e5), [e21](#evidence-e21), [e22](#evidence-e22).

### Run / BeginAccepted / Cancel / Summarize / SetTools / SetModels

Surface: embedding/host program API.

Input: Session/prompt/run id; acceptance reservation; cancellation; provider/tool update

Result: Live persisted assistant/tool messages and terminal RunComplete; summary id

Lifecycle: Same-session serial dispatch, queued steering/turn identities, latest tools per step and current model callback

Authority: Host config/hooks can affect steps; model does not hot-redefine compiled coordinator

Evidence: [e2](#evidence-e2), [e3](#evidence-e3), [e4](#evidence-e4), [e18](#evidence-e18), [e19](#evidence-e19), [e27](#evidence-e27), [e28](#evidence-e28).

### ConfigStore.ReloadFromDisk / SetConfigField(s), executable crushrc

Surface: operator/host configuration API.

Input: Disk config or field values; context for evaluation

Result: Candidate config or error with prior config retained

Lifecycle: Atomic candidate publication, runtime chosen-model overrides maintained; cancellable config program

Authority: Operator/config author; executable config does not establish executable agent refit

Evidence: [e16](#evidence-e16), [e17](#evidence-e17).

### notifications/claude/channel + discovered MCP reply tools

Surface: external service/program surface.

Input: Opted-in validated channel text/meta; outbound reply target/message

Result: New queued/session turn and tool reply

Lifecycle: Backend must-deliver publication/ordered routing; downstream init/dispatch errors drop logged messages

Authority: External MCP channel service, not native multi-agent colleague room

Evidence: [e23](#evidence-e23), [e24](#evidence-e24), [e25](#evidence-e25), [e29](#evidence-e29).

### PreToolUse hooks

Surface: operator-defined shell program surface.

Input: Tool name/input/context and hook result

Result: Allow/deny/halt/input rewrite/context appended

Lifecycle: Top-level invocation only; errors proceed with tool call; subagents skip hook interception

Authority: Configured operator programs, permission preapproval scoped to tool-call id

Evidence: [e15](#evidence-e15), [e26](#evidence-e26).

## Capabilities

### filesystem

**I — Files** (source): Native files/search/edit with reread tracker and history; write uses second-resolution mtime guard, not simultaneous-writer reservation.

Evidence: [e5](#evidence-e5), [e6](#evidence-e6), [e7](#evidence-e7).

### processes

**I — OS programs** (source): Background shell identities, output/wait/kill and foreground auto-background; global registry and detached contexts need separate lifecycle study for shared users.

Evidence: [e8](#evidence-e8), [e9](#evidence-e9), [e10](#evidence-e10), [e11](#evidence-e11), [e12](#evidence-e12).

### code-actions

**S — Code actions** (source): Shell can execute programs and operator pre-tool/config scripts compose behavior; no native unrestricted model code-to-tools cell/kernel traced.

Evidence: [e8](#evidence-e8), [e15](#evidence-e15), [e17](#evidence-e17).

### persistent-kernel

**? — Kernel** (source-inspection): Not established in the inspected entry, model loop, action dispatch and state paths; no universal absence claim.

### standing-database

**S — Standing DB** (source): Internal persisted session/message/todo/summary database; model TODO/log actions do not expose arbitrary standing research DB.

Evidence: [e21](#evidence-e21), [e22](#evidence-e22).

### workflow-programming

**L — Workflows** (source): Config/hooks and prompt-only delegation support composition but concrete Go/Fantasy turn kernel remains fixed; native programmable workflow language not established.

Evidence: [e3](#evidence-e3), [e13](#evidence-e13), [e15](#evidence-e15), [e16](#evidence-e16).

### multi-model

**L — Models** (source): Host large/small model configuration and live ModelProvider callback; task tool has prompt only and uses host-built agent, no per-call model mixing/live colleague protocol.

Evidence: [e4](#evidence-e4), [e13](#evidence-e13), [e14](#evidence-e14).

### live-collaboration

**S — Peer chat** (source): External MCP channels inject ordered text and route replies; ordinary child delegation lacks peer communication. Downstream init/dispatch errors can drop push.

Evidence: [e14](#evidence-e14), [e23](#evidence-e23), [e24](#evidence-e24), [e25](#evidence-e25).

### concurrent-work

**I — Concurrency** (source): Separate sessions, parallel-designated child agent and background job goroutines; same session dispatch serializes active vs queued calls. Fantasy tool scheduler dependency internals not inspected.

Evidence: [e2](#evidence-e2), [e10](#evidence-e10), [e13](#evidence-e13), [e24](#evidence-e24).

### steering-interrupt

**I — Steer/interrupt** (source): Accepted sequence/cancel mark closes pre-active cancellation race; folded steering vs RunID-bearing separate turns preserve terminal events, latest step drains queue.

Evidence: [e2](#evidence-e2), [e3](#evidence-e3), [e27](#evidence-e27), [e28](#evidence-e28).

### turn-redefinition

**L — Turn program** (source): Host hooks rewrite inputs/halt/add context; step updates tools/model. This is controlled behavior change, not model hot redefinition of turn scheduler.

Evidence: [e3](#evidence-e3), [e4](#evidence-e4), [e15](#evidence-e15).

### compaction

**I — Compaction** (source): Automatic/manual provider summary stored as message; outgoing history selects summary tail while prior DB transcript retained.

Evidence: [e18](#evidence-e18), [e19](#evidence-e19), [e20](#evidence-e20).

### context-repair

**S — Repair** (source): Retained session messages and explicit summary pointer allow host recovery composition; no model-native exact restoration/lineage repair protocol traced.

Evidence: [e20](#evidence-e20), [e21](#evidence-e21).

### original-audit

**L — Original audit** (source): Assistant/tool content persists, but retry resets partial stream text and invalid JSON arguments sanitized; diagnostic logs redacted/bounded. Not original every-attempt/wire log.

Evidence: [e4](#evidence-e4), [e22](#evidence-e22).

### audit-query

**L — Audit query** (source): Model crush_logs reads bounded/redacted recent diagnostic file; persistent session DB program service broader but no original-audit query tool.

Evidence: [e21](#evidence-e21), [e22](#evidence-e22).

### hot-change

**I — Hot change** (source): Config candidate reload/rollback, runtime model overrides, live tool set each step and current model callback; executable recompilation continuity not established.

Evidence: [e3](#evidence-e3), [e4](#evidence-e4), [e16](#evidence-e16), [e17](#evidence-e17).

### rebuild-continuity

**? — Rebuild continuity** (source-inspection): Not established in the inspected entry, model loop, action dispatch and state paths; no universal absence claim.

### remote-services

**I — Remote** (source): Model-discovered MCP actions and opted-in push channel callbacks consume external services.

Evidence: [e23](#evidence-e23), [e24](#evidence-e24), [e29](#evidence-e29).

### self-improvement

**S — Self-improve** (source): Files/config/hooks can be changed as programs; no measured experiment governor/executable refit outcome traced.

Evidence: [e6](#evidence-e6), [e15](#evidence-e15), [e16](#evidence-e16).

### complaints

**? — Complaints** (source-inspection): Not established in the inspected entry, model loop, action dispatch and state paths; no universal absence claim.

### authority

**L — Authority** (source): Skip/allowlist/autoapprove make low-approval use possible, but bash installs static command/blocker rules even when prompt requests skipped.

Evidence: [e8](#evidence-e8), [e26](#evidence-e26).

### evaluation

**S — Evaluation** (source): Specific cancellation/terminal-event/config rollback tests provide strong local fault oracles, not live provider coverage.

Evidence: [e17](#evidence-e17), [e27](#evidence-e27), [e28](#evidence-e28).

### time-order

**I — Time/order** (source): Acceptance sequence/cancel coverage, ordered channel routing and RunID terminal events; no all-event distributed audit order.

Evidence: [e2](#evidence-e2), [e23](#evidence-e23), [e24](#evidence-e24), [e28](#evidence-e28).

## Inspected test oracles

- [quarantine/crush/internal/agent/dispatch_cancel_test.go](../../../quarantine/crush/internal/agent/dispatch_cancel_test.go): Cancel racing accepted/active dispatch Oracle: Manual active cancellation function and accepted reservation; asserts both branches covered, cancel-on-entry releases accept and never queues; broader fixture path uses scripted model, not paid provider. Read, **not executed**.
- [quarantine/crush/internal/agent/queued_runid_test.go](../../../quarantine/crush/internal/agent/queued_runid_test.go): Queued turn completion identity Oracle: Gated model holds active turn; enqueue RunID-bearing follow-up, release, assert distinct main/follow terminal events without silent folding. Read, **not executed**.
- [quarantine/crush/internal/config/reload_crushrc_test.go](../../../quarantine/crush/internal/config/reload_crushrc_test.go): Failed/hanging executable configuration retention Oracle: Edited config value changes on reload; invalid/hanging program errors, finishes within bounded cancellation wait and preserves old notifications value. Read, **not executed**.

## Useful mechanisms

- Explicit accepted-versus-active cancellation race handling.
- Separate completion events for queued RunID turns.
- Transactional reload candidate and canceled/invalid-program retention oracle.
- Background command result identities and bounded spill metadata.

## Material limits

- Fantasy dependency scheduler internals not populated/read. Full network/LSP actions and shell exec isolation not exhaustively traced. No persistent code notebook kernel, native peer room, executable rebuild continuity, captured-state complaint or measured autoresearch established.

## Arconaut design questions

- Would an external process service replace global unsessioned shell handles while retaining useful output/wait/cancel semantics?
- Which cancellation receipts must be durable rather than process-local?
- Should original failed stream fragments/unsanitized calls be kept separately from model-visible normalized transcript?
- How should peer messages differ from optional MCP notifications and failure-droppable channel routing?
- Can config programs and provider changes activate at an explicit turn/workflow boundary under model agency?

## Evidence

### Evidence e1

[quarantine/crush/main.go:1–26](../../../quarantine/crush/main.go#L1): Go CLI entry, optional profiling goroutine.

### Evidence e2

[quarantine/crush/internal/agent/agent.go:605–706](../../../quarantine/crush/internal/agent/agent.go#L605): Per-session dispatch/cancel lock, accepted sequence guard, busy queue, registered active request identity.

### Evidence e3

[quarantine/crush/internal/agent/agent.go:844–913](../../../quarantine/crush/internal/agent/agent.go#L844): Fantasy streaming request; latest tools and queued steering prepared per step.

### Evidence e4

[quarantine/crush/internal/agent/agent.go:973–1068](../../../quarantine/crush/internal/agent/agent.go#L973): Model hot lookup, sanitized tool input, paired persisted results and finish callback.

### Evidence e5

[quarantine/crush/internal/agent/coordinator.go:876–969](../../../quarantine/crush/internal/agent/coordinator.go#L876): Actual native catalog, MCP allow filtering and top-level-only tool hooks.

### Evidence e6

[quarantine/crush/internal/agent/tools/write.go:56–142](../../../quarantine/crush/internal/agent/tools/write.go#L56): Write protects files modified since last read, asks permission and writes content.

### Evidence e7

[quarantine/crush/internal/agent/tools/view.go:190–257](../../../quarantine/crush/internal/agent/tools/view.go#L190): view structured image/text range and LSP-aware file result.

### Evidence e8

[quarantine/crush/internal/agent/tools/bash.go:166–314](../../../quarantine/crush/internal/agent/tools/bash.go#L166): Bash installs built-in blockers regardless of prompt skip; explicit background uses detached context and job identity.

### Evidence e9

[quarantine/crush/internal/agent/tools/bash.go:315–388](../../../quarantine/crush/internal/agent/tools/bash.go#L315): Foreground cancellation kills tracked shell; threshold auto-background preserves handle.

### Evidence e10

[quarantine/crush/internal/shell/background.go:45–154](../../../quarantine/crush/internal/shell/background.go#L45): Process-global background manager, bounded count and context/goroutine completion ownership; handles lack session scope.

### Evidence e11

[quarantine/crush/internal/agent/tools/job_output.go:25–92](../../../quarantine/crush/internal/agent/tools/job_output.go#L25): Model job_output handle reads accumulated output or waits with caller context.

### Evidence e12

[quarantine/crush/internal/agent/tools/job_kill.go:32–60](../../../quarantine/crush/internal/agent/tools/job_kill.go#L32): Model job_kill cancels global background handle and waits for completion.

### Evidence e13

[quarantine/crush/internal/agent/agent_tool.go:18–68](../../../quarantine/crush/internal/agent/agent_tool.go#L18): agent prompt-only parallel tool builds task agent with separate child session.

### Evidence e14

[quarantine/crush/internal/agent/coordinator.go:1660–1733](../../../quarantine/crush/internal/agent/coordinator.go#L1660): Child session runs configured task agent and aggregates cost; no live peer message protocol.

### Evidence e15

[quarantine/crush/internal/agent/hooked_tool.go:17–97](../../../quarantine/crush/internal/agent/hooked_tool.go#L17): Top-level pre-tool shell hooks may deny/halt/rewrite input/preapprove/add context; subagents skip hooks.

### Evidence e16

[quarantine/crush/internal/config/store.go:1295–1369](../../../quarantine/crush/internal/config/store.go#L1295): Reload builds candidate, validates hooks and preserves runtime model overrides before provider configuration.

### Evidence e17

[quarantine/crush/internal/config/reload_crushrc_test.go:29–105](../../../quarantine/crush/internal/config/reload_crushrc_test.go#L29): Reload tests edited value, invalid config retention and bounded cancellation of hanging crushrc.

### Evidence e18

[quarantine/crush/internal/agent/agent.go:1411–1464](../../../quarantine/crush/internal/agent/agent.go#L1411): Summary is separate busy-checked provider run with explicit persisted summary message.

### Evidence e19

[quarantine/crush/internal/agent/agent.go:1514–1552](../../../quarantine/crush/internal/agent/agent.go#L1514): Summary stores pointer and usage then releases active request and runs queued prompt.

### Evidence e20

[quarantine/crush/internal/agent/agent.go:1792–1814](../../../quarantine/crush/internal/agent/agent.go#L1792): Outgoing compacted history reads stored summary tail and changes first summary role to user.

### Evidence e21

[quarantine/crush/internal/session/session.go:52–90](../../../quarantine/crush/internal/session/session.go#L52): Internal session DB service and explicit parent/summary/todo/channel metadata.

### Evidence e22

[quarantine/crush/internal/agent/tools/crush_logs.go:20–116](../../../quarantine/crush/internal/agent/tools/crush_logs.go#L20): Model logs action bounded to 50 default/100 lines with sensitive keys redacted.

### Evidence e23

[quarantine/crush/internal/agent/tools/mcp/channel.go:157–205](../../../quarantine/crush/internal/agent/tools/mcp/channel.go#L157): Explicit channel opt-in plus must-deliver validated external notification publication.

### Evidence e24

[quarantine/crush/internal/backend/channels.go:23–95](../../../quarantine/crush/internal/backend/channels.go#L23): Backend routes pushes in order across workspace goroutines; initialization/dispatch failures still drop/log push.

### Evidence e25

[quarantine/crush/internal/agent/channelreply.go:19–46](../../../quarantine/crush/internal/agent/channelreply.go#L19): MCP reply target routing contract and bounded reply timeout.

### Evidence e26

[quarantine/crush/internal/permission/permission.go:181–231](../../../quarantine/crush/internal/permission/permission.go#L181): Skip/allowlist/hook approval and per-session autoapprove paths before serialized permission requests.

### Evidence e27

[quarantine/crush/internal/agent/dispatch_cancel_test.go:71–124](../../../quarantine/crush/internal/agent/dispatch_cancel_test.go#L71): Active and accepted cancellation test; cancel-on-entry never enqueues future work.

### Evidence e28

[quarantine/crush/internal/agent/queued_runid_test.go:78–162](../../../quarantine/crush/internal/agent/queued_runid_test.go#L78): Gated provider test verifies RunID-bearing queued follow-up publishes own terminal completion.

### Evidence e29

[quarantine/crush/internal/agent/tools/mcp-tools.go:24–68](../../../quarantine/crush/internal/agent/tools/mcp-tools.go#L24): Discovered external schemas mapped to mcp_<server>_<tool>.

