# pi

A small default coding tool set with unusually broad executable extension hooks, actual source-code tool composition, branch-local JSON state and provenance-preserving context projections; experimental durable runtime surfaces require distinct study.

Role: interactive coding agent and separately exported agent runtimes. Runtime: TypeScript, JavaScript, QuickJS WebAssembly.

Pinned source: [https://github.com/earendil-works/pi](https://github.com/earendil-works/pi); revision/version `8ce69e9d2b171d173fe4b6b2b6256f1f4411e69d`.

Ordinary CLI uses AgentSession over Agent/agent-loop in Node/Bun; codemode creates a fresh QuickJS worker sandbox per action. Separate exported durable/Pico3 harnesses are experimental and not transferred to baseline CLI claims.

Owns local conversation tree, extension contexts and launched shell/stdio children; consumes provider runtimes, HTTP MCP services and injectable remote filesystem/process backends. Chord/durable runtime APIs exist separately.

Inspection: CLI SDK/session/loop/provider/tool boundaries, built-in code mode and nested calls, compaction/context edits/history access, reload/cancellation, MCP lifecycle and local test oracles read; experimental harness API/lifecycle and status documentation read separately.

Limits of this study: No runtime/test execution. Durable operation state machine, Chord scheduler, full transport internals and all optional extensions not exhaustively traced. Do not apply experimental-harness documented recovery or plugin test behavior to the default CLI.

## Actions

### AgentSession.prompt / steer / followUp

Surface: operator/program API.

Input: text/images, source and streamingBehavior

Result: persisted/queued input; events and eventual assistant/tool results

Lifecycle: steer is delivered after current tools; followUp after current run

Authority: Model uses launcher OS authority; extension hooks may block or replace behavior.

Evidence: [entry](#evidence-entry), [steer-abort](#evidence-steer-abort), [loop](#evidence-loop).

### read

Surface: model tool.

Input: path, optional offset/limit

Result: bounded text or image content

Lifecycle: single cancellable read through injectable operations

Authority: Model uses launcher OS authority; extension hooks may block or replace behavior.

Evidence: [read](#evidence-read).

### write

Surface: model tool.

Input: path, content

Result: write confirmation

Lifecycle: serialized file mutation holds lock through in-flight IO settlement

Authority: Model uses launcher OS authority; extension hooks may block or replace behavior.

Evidence: [write](#evidence-write).

### edit

Surface: model tool.

Input: path, edits:[{oldText,newText}]

Result: diff/edit result

Lifecycle: unique nonoverlapping replacements against original content; file queue

Authority: Model uses launcher OS authority; extension hooks may block or replace behavior.

Evidence: [edit](#evidence-edit).

### bash

Surface: model tool.

Input: command, optional timeout seconds

Result: stream updates; structured output/truncated/full_output_path/exit_code/wall_time_seconds

Lifecycle: fresh spawn; awaits exit; abort/timeout invoke tree termination

Authority: Model uses launcher OS authority; extension hooks may block or replace behavior.

Evidence: [bash](#evidence-bash), [spawn](#evidence-spawn).

### powershell

Surface: model tool.

Input: command and shell-tool options

Result: registered shell operation; complete dispatch not inspected

Lifecycle: optional exported native tool, distinct from default bash

Authority: Model uses launcher OS authority; extension hooks may block or replace behavior.

Evidence: [catalog](#evidence-catalog).

### grep / find / ls

Surface: model tool.

Input: grep regex/path/glob/case/literal/context/limit; find glob/path/limit; ls path/limit

Result: bounded search/file/directory results

Lifecycle: registered optional tools; ls execution and search schemas/abort excerpts inspected

Authority: Model uses launcher OS authority; extension hooks may block or replace behavior.

Evidence: [catalog](#evidence-catalog), [search-schema](#evidence-search-schema), [find](#evidence-find), [ls](#evidence-ls).

### codemode

Surface: model tool.

Input: raw JavaScript source or {code}, optional source options deadline/output budget

Result: explicit text/image/return output, nested call metadata/errors; successful branch-local JSON store writes

Lifecycle: fresh sandbox per action; nested tools inherit pipeline; outstanding calls canceled on sandbox end; earlier side effects remain

Authority: Model uses launcher OS authority; extension hooks may block or replace behavior.

Evidence: [codemode-load](#evidence-codemode-load), [codemode](#evidence-codemode), [codemode-contract](#evidence-codemode-contract), [store](#evidence-store).

### codemode tools.*, searchTools, describeTool, describeNamespace, models.*

Surface: model tool.

Input: named nested arguments or catalog/classifier request

Result: structured content/text; model catalog/classifier response and usage

Lifecycle: tool discovery plus bounded concurrent classifier calls; IDs nest under source call

Authority: Model uses launcher OS authority; extension hooks may block or replace behavior.

Evidence: [codemode](#evidence-codemode), [models](#evidence-models).

### appendContextEdit / buildSessionProjection / getBranch / getEntries / getTree

Surface: extension/program API.

Input: entry target, replacement/null or branch identity

Result: projection with source provenance; unchanged original entries/tree

Lifecycle: append edit lineage and project active branch; no destructive original rewrite

Authority: Model uses launcher OS authority; extension hooks may block or replace behavior.

Evidence: [repair](#evidence-repair), [projection](#evidence-projection), [query](#evidence-query), [runner-query](#evidence-runner-query).

### compact / session_before_compact

Surface: operator/extension API.

Input: instructions, history/preparation and optional replacement summary/firstKeptEntryId

Result: persisted managed summary and compaction events

Lifecycle: manual/automatic cancellable transform; extension may cancel/replace

Authority: Model uses launcher OS authority; extension hooks may block or replace behavior.

Evidence: [compact](#evidence-compact), [auto-compact](#evidence-auto-compact).

### /reload / AgentSession.reload

Surface: operator/extension API.

Input: reload options

Result: new extension contexts/resources; retained chat/session

Lifecycle: interactive command requires not streaming/compacting; old extension handles become stale

Authority: Model uses launcher OS authority; extension hooks may block or replace behavior.

Evidence: [reload](#evidence-reload), [reload-ui](#evidence-reload-ui).

### registerTool / registerCommand / setActiveTools / setModel / registerProvider

Surface: extension API.

Input: tool/command code, loadout, model/provider definition

Result: new declared tools/commands/provider hooks

Lifecycle: API includes immediate bound provider changes and next-request loadout refresh; generic extensions govern their own work

Authority: Model uses launcher OS authority; extension hooks may block or replace behavior.

Evidence: [extensions-api](#evidence-extensions-api), [boundary](#evidence-boundary).

### mcp__<server>__<tool> and MCP resource operations

Surface: model/program tools.

Input: configured service names/schemas or resource URI

Result: MCP content/resources

Lifecycle: HTTP service or owned stdio connection; lazy reconnect and explicit reconnect

Authority: Model uses launcher OS authority; extension hooks may block or replace behavior.

Evidence: [mcp](#evidence-mcp), [mcp-call](#evidence-mcp-call), [mcp-close](#evidence-mcp-close).

### Pico3 Harness / ConversationHandle.send, fork, collapse, abort, waitForTask

Surface: experimental supplied runtime.

Input: taskKinds, storage/models/tools; context plus addressed input/task ID

Result: durable input/task handles, conversation watch/query APIs

Lifecycle: separate experimental exported runtime; close delegates suspension

Authority: Model uses launcher OS authority; extension hooks may block or replace behavior.

Evidence: [pico-scope](#evidence-pico-scope), [pico-api](#evidence-pico-api), [pico-lifecycle](#evidence-pico-lifecycle).

## Capabilities

### filesystem

**I — Files** (source): Default read/write/edit and optional search tools use injectable operation backends; local mutations serialize until in-flight IO settles.

Evidence: [catalog](#evidence-catalog), [read](#evidence-read), [write](#evidence-write), [edit](#evidence-edit), [ls](#evidence-ls).

### processes

**I — OS programs** (source): bash streams detached shell execution, has optional deadline and explicit abort/tree-kill requests, exit code/full-output metadata. Persistent public PTY/process handles were not established.

Evidence: [bash](#evidence-bash), [spawn](#evidence-spawn).

### code-actions

**I — Code actions** (source): Built-in, initially inactive codemode exposes model-authored JavaScript that composes actual tools through nested validation/hooks with structured results and explicit output filtering.

Evidence: [codemode-load](#evidence-codemode-load), [codemode](#evidence-codemode), [codemode-contract](#evidence-codemode-contract).

### persistent-kernel

**L — Kernel** (source): Each codemode action creates/closes a fresh QuickJS sandbox. Successful JSON store writes persist on the branch, but functions/live interpreter state do not persist through that mechanism.

Evidence: [codemode](#evidence-codemode), [store](#evidence-store).

### standing-database

**S — Standing DB** (source): Branch-local store/load reconstructs structured JSON values from session custom entries and extensions have history APIs. It supplies key-value state, not a separately owned standing SQL/query service.

Evidence: [store](#evidence-store), [codemode](#evidence-codemode), [query](#evidence-query).

### workflow-programming

**I — Workflows** (source): Codemode provides actual loops/chains/parallel calls; extension tools/commands and tool/turn/context hooks compose normal operation. No default general graph scheduler is inferred.

Evidence: [codemode](#evidence-codemode), [boundary](#evidence-boundary), [nested](#evidence-nested), [extensions-api](#evidence-extensions-api).

### multi-model

**I — Models** (source): Current model refreshes at next assistant request; extension model/provider mutation and script catalog/classifier calls are exposed. Classifier fan-out is distinct from independently working chat peers.

Evidence: [boundary](#evidence-boundary), [models](#evidence-models), [extensions-api](#evidence-extensions-api).

### live-collaboration

**? — Peer chat** (inspection-limit): Inspected baseline prompt/queue/extension/code-mode paths address one session and nested tool calls. No arbitrary addressed messages between independently running chat participants were established; experimental multiple conversations alone are insufficient.

### concurrent-work

**I — Concurrency** (source): Agent tool batches and codemode Promise composition run independent calls concurrently while sequential tools serialize; nested calls have meaningful parent IDs. This does not establish a standing multi-agent room.

Evidence: [dispatch](#evidence-dispatch), [codemode](#evidence-codemode), [nested-test](#evidence-nested-test).

### steering-interrupt

**I — Steer/interrupt** (source): Steering waits for current assistant tools and follow-up waits for run completion. abort cancels agent/summary/retry work and waits idle; shell cancellation invokes termination.

Evidence: [steer-abort](#evidence-steer-abort), [abort](#evidence-abort), [spawn](#evidence-spawn).

### turn-redefinition

**I — Turn program** (source): Actual finishTurn/prepareNextTurn/prepareRequest decisions can change continuation, model, tools and context; CLI wires extension turn-end continuation and post-run before-settle continuation. Scope is these boundaries, not arbitrary replacement of active stack frames.

Evidence: [loop](#evidence-loop), [boundary](#evidence-boundary).

### compaction

**I — Compaction** (source): Manual/auto compaction is managed and cancellable, with extension replacement, retained entry ID and persisted metadata; active projection selects summary/tail from immutable branch entries.

Evidence: [compact](#evidence-compact), [auto-compact](#evidence-auto-compact), [projection](#evidence-projection).

### context-repair

**S — Repair** (source): Append-only branch-local context edits can omit/restore content while retaining original entry IDs/content. Extensions can retrieve originals/tree and project lineage, but a dedicated model repair workflow is composed rather than a default native tool.

Evidence: [repair](#evidence-repair), [projection](#evidence-projection), [query](#evidence-query), [runner-query](#evidence-runner-query), [context-test](#evidence-context-test).

### original-audit

**L — Original audit** (source): Append-only message/context-edit lineage and provider hooks are useful. Nested records deliberately omit/drop/clip data under budgets, script nested results do not all reach model transcript, and exact forced prompt rendering is not recorded; full original audit is not established.

Evidence: [query](#evidence-query), [audit-limits](#evidence-audit-limits), [forced-prompt](#evidence-forced-prompt), [codemode-contract](#evidence-codemode-contract), [sdk](#evidence-sdk).

### audit-query

**S — Audit query** (source): Extensions receive sessionManager and can fetch branch/all-entry/tree/source projection; models may compose access through extension tools. Inspected scope is session history, not an established complete provider/program audit query corpus.

Evidence: [query](#evidence-query), [runner-query](#evidence-runner-query), [projection](#evidence-projection).

### hot-change

**I — Hot change** (source): Reload rebuilds resource/provider/extension runtime with stale-context invalidation while preserving session. CLI refuses reload during streaming or compaction; tool/model refresh is next-request. Programmatic reload does not itself show the same guard or Arconaut's affected-workflow deferred activation.

Evidence: [reload](#evidence-reload), [reload-ui](#evidence-reload-ui), [boundary](#evidence-boundary).

### rebuild-continuity

**L — Rebuild continuity** (source): Resource reload preserves conversation and invalidates old extension handles, but it does not replace the harness executable. Separate experimental suspend/close APIs do not establish a CLI outpost compile-and-reinhabit protocol or survival of live OS effects.

Evidence: [reload](#evidence-reload), [reload-ui](#evidence-reload-ui), [pico-scope](#evidence-pico-scope), [pico-lifecycle](#evidence-pico-lifecycle).

### remote-services

**I — Remote** (source): MCP consumes HTTP services or launches configured stdio servers; lazy getClient/explicit reconnect and resource/tool APIs are wired. Filesystem/bash operations can be supplied by remote backends; shared service governance is not inferred.

Evidence: [mcp](#evidence-mcp), [mcp-call](#evidence-mcp-call), [mcp-close](#evidence-mcp-close), [read](#evidence-read), [bash](#evidence-bash).

### self-improvement

**? — Self-improve** (inspection-limit): Model code actions and reloadable extensions allow substantial alteration, but the inspected paths do not establish a dedicated model-governed candidate evaluation/promotion/autoresearch loop.

### complaints

**? — Complaints** (inspection-limit): Inspected native tools, extensions and session lifecycle do not establish a default model complaint operation that captures state and tracks follow-up. Custom extension composition remains possible.

### authority

**I — Authority** (source): Local tools directly operate with launcher authority; before-tool hooks can block/terminate and nested calls use the same pipeline. README documents no built-in restrictive permission system; extensions can add policy.

Evidence: [write](#evidence-write), [spawn](#evidence-spawn), [validate](#evidence-validate), [nested](#evidence-nested), [authority-doc](#evidence-authority-doc).

### evaluation

**I — Evaluation** (source): Read tests attack projection/original separation, nested ID/usage/concurrency and explicit record truncation, plus separate experimental facet cutover. Fake tools/in-memory sessions/stub lane limit integration claims; tests not run.

Evidence: [context-test](#evidence-context-test), [nested-test](#evidence-nested-test), [reload-test](#evidence-reload-test).

### time-order

**S — Time/order** (source): Append-only entries carry parent IDs/wall timestamps; nested durations use performance.now, shell seconds deadlines and script hard deadlines. Restart/paused-time guarantees for default CLI were not established.

Evidence: [query](#evidence-query), [audit-limits](#evidence-audit-limits), [bash](#evidence-bash), [codemode](#evidence-codemode).

## Inspected test oracles

- [quarantine/pi/packages/coding-agent/test/session-context-edit.test.ts](../../../quarantine/pi/packages/coding-agent/test/session-context-edit.test.ts): Original/projection separation and branch-local content repair. Oracle: In-memory assertions preserve original message while latest edit omits/restores projected content; no provider summary-faithfulness oracle. Read, **not executed**.
- [quarantine/pi/packages/coding-agent/test/nested-tool-calls.test.ts](../../../quarantine/pi/packages/coding-agent/test/nested-tool-calls.test.ts): Nested identity, usage, concurrency and explicit audit clipping. Oracle: Fake tools assert sequential=1 versus parallel=3 and recorder caps/incompleteness; live services and durable crashes not tested in read cases. Read, **not executed**.
- [quarantine/pi/packages/coding-agent/test/experimental-plugin-reload.test.ts](../../../quarantine/pi/packages/coding-agent/test/experimental-plugin-reload.test.ts): Experimental facet activation/disposal. Oracle: Stub lane/loader records generations; proves that cutover contract locally, not baseline CLI executable continuity. Read, **not executed**.

## Useful mechanisms

- Executable code actions compose tools through the ordinary pipeline instead of bypassing hooks.
- Original entry and current context projection are explicit separate concepts with append-only repair lineage.
- Next-turn/settled boundaries expose continuation decisions and model/tool refresh.
- Reload marks prior extension contexts stale, making invalid authority reuse explicit.

## Material limits

- Fresh code-mode sandbox persists JSON values only; it is not a live persistent kernel.
- Normalized bounded nested-call records are not exhaustive original audit.
- Default CLI and exported experimental durable/Pico3 runtimes must be kept distinct.
- Interactive reload guards response/compaction but does not establish executable refit or model-governed experiment promotion.

## Arconaut design questions

- Can Arconaut retain Pi's executable nested-tool ergonomics with an external shared kernel rather than owning interpreter lifetime?
- Which context edit/compaction lineage should be model-addressable as normal agency, and how do we retain every original beside the projection?
- Should explicit stale-context generations also fence extension/service authority across a quiescent refit?
- What scope of pending workflows must delay a configuration change beyond a next-provider-request boundary?

## Evidence

### Evidence entry

[quarantine/pi/packages/coding-agent/src/core/agent-session.ts:1895–1952](../../../quarantine/pi/packages/coding-agent/src/core/agent-session.ts#L1895): prompt intercepts extension commands/input and queues steer/followUp during a run.

### Evidence sdk

[quarantine/pi/packages/coding-agent/src/core/sdk.ts:387–453](../../../quarantine/pi/packages/coding-agent/src/core/sdk.ts#L387): CLI/SDK creates ordinary Agent and AgentSession with provider stream, context and provider event hooks.

### Evidence loop

[quarantine/pi/packages/agent/src/agent-loop.ts:175–292](../../../quarantine/pi/packages/agent/src/agent-loop.ts#L175): Loop applies prepareNextTurn/prepareRequest, steering, tool-call settlement and finishTurn continuation decisions; truncated tool-call arguments are failed.

### Evidence provider

[quarantine/pi/packages/agent/src/agent-loop.ts:388–407](../../../quarantine/pi/packages/agent/src/agent-loop.ts#L388): Context is transformed/converted and current model stream called with key and AbortSignal.

### Evidence dispatch

[quarantine/pi/packages/agent/src/agent-loop.ts:508–654](../../../quarantine/pi/packages/agent/src/agent-loop.ts#L508): Tool batches choose sequential or parallel execution; parallel outcomes materialize in assistant source order.

### Evidence validate

[quarantine/pi/packages/agent/src/agent-loop.ts:710–766](../../../quarantine/pi/packages/agent/src/agent-loop.ts#L710): Tool arguments are validated, beforeToolCall can block/terminate, and abort is checked before effects.

### Evidence boundary

[quarantine/pi/packages/coding-agent/src/core/agent-session.ts:852–909](../../../quarantine/pi/packages/coding-agent/src/core/agent-session.ts#L852): Extension turn-end continuation and next-request context/tool/model refresh are wired to Agent callbacks.

### Evidence nested

[quarantine/pi/packages/coding-agent/src/core/agent-session.ts:618–667](../../../quarantine/pi/packages/coding-agent/src/core/agent-session.ts#L618): Nested/direct tool call and result interception uses the current extension runner and parent call identity.

### Evidence catalog

[quarantine/pi/packages/coding-agent/src/core/tools/index.ts:95–201](../../../quarantine/pi/packages/coding-agent/src/core/tools/index.ts#L95): Eight native tool names are exported; default coding tools are read/bash/edit/write.

### Evidence bash

[quarantine/pi/packages/coding-agent/src/core/tools/bash.ts:27–65](../../../quarantine/pi/packages/coding-agent/src/core/tools/bash.ts#L27): bash accepts command and optional seconds timeout; structured results include capped combined output, full path, exit code and wall time.

### Evidence spawn

[quarantine/pi/packages/coding-agent/src/core/tools/bash.ts:115–166](../../../quarantine/pi/packages/coding-agent/src/core/tools/bash.ts#L115): Local shell spawns detached where available, streams stdout/stderr and invokes process-tree kill on abort/deadline.

### Evidence read

[quarantine/pi/packages/coding-agent/src/core/tools/read.ts:15–47](../../../quarantine/pi/packages/coding-agent/src/core/tools/read.ts#L15): read has path/offset/limit and injectable filesystem operations backed by local reads.

### Evidence write

[quarantine/pi/packages/coding-agent/src/core/tools/write.ts:44–85](../../../quarantine/pi/packages/coding-agent/src/core/tools/write.ts#L44): write resolves path, serializes file mutation and retains the queue until in-flight IO settles even after abort.

### Evidence edit

[quarantine/pi/packages/coding-agent/src/core/tools/edit.ts:145–198](../../../quarantine/pi/packages/coding-agent/src/core/tools/edit.ts#L145): edit validates path/edits and applies unique disjoint replacements to original content inside the mutation queue.

### Evidence search-schema

[quarantine/pi/packages/coding-agent/src/core/tools/grep.ts:21–33](../../../quarantine/pi/packages/coding-agent/src/core/tools/grep.ts#L21): grep takes pattern, path, glob, ignoreCase, literal, context and limit.

### Evidence find

[quarantine/pi/packages/coding-agent/src/core/tools/find.ts:26–35](../../../quarantine/pi/packages/coding-agent/src/core/tools/find.ts#L26): find takes pattern/path/limit; registered execution has abort/child stop handling.

### Evidence ls

[quarantine/pi/packages/coding-agent/src/core/tools/ls.ts:65–115](../../../quarantine/pi/packages/coding-agent/src/core/tools/ls.ts#L65): ls resolves a directory, reads entries and sorts/formats a bounded result.

### Evidence codemode-load

[quarantine/pi/packages/coding-agent/src/extensions/codemode/index.ts:1–43](../../../quarantine/pi/packages/coding-agent/src/extensions/codemode/index.ts#L1): Built-in codemode registers inactive; explicit activation or MCP integration exposes it.

### Evidence codemode

[quarantine/pi/packages/coding-agent/src/extensions/codemode/execute.ts:224–319](../../../quarantine/pi/packages/coding-agent/src/extensions/codemode/execute.ts#L224): A fresh bounded QuickJS sandbox routes nested calls through ctx.executeTool, closes after execution, persists successful store writes and returns clipped script output/errors.

### Evidence codemode-contract

[quarantine/pi/packages/coding-agent/src/extensions/codemode/tool.ts:85–157](../../../quarantine/pi/packages/coding-agent/src/extensions/codemode/tool.ts#L85): codemode takes source code and documents tools/discovery/output/JSON store helpers, no direct Node/FS/network/timers and script deadline options.

### Evidence store

[quarantine/pi/packages/coding-agent/src/extensions/codemode/execute.ts:116–126](../../../quarantine/pi/packages/coding-agent/src/extensions/codemode/execute.ts#L116): JSON store is reconstructed from custom entries on the selected branch.

### Evidence models

[quarantine/pi/packages/coding-agent/src/extensions/codemode/execute.ts:417–472](../../../quarantine/pi/packages/coding-agent/src/extensions/codemode/execute.ts#L417): Script model catalog and classifier calls are wired, with four-call concurrency limiting and attributed usage.

### Evidence compact

[quarantine/pi/packages/coding-agent/src/core/agent-session.ts:2704–2808](../../../quarantine/pi/packages/coding-agent/src/core/agent-session.ts#L2704): Manual compaction can be canceled/replaced by extension, persists summary/firstKeptEntryId and refreshes active projection.

### Evidence auto-compact

[quarantine/pi/packages/coding-agent/src/core/agent-session.ts:3037–3132](../../../quarantine/pi/packages/coding-agent/src/core/agent-session.ts#L3037): Automatic compaction uses the same replaceable summary path, checks abort and emits managed events.

### Evidence projection

[quarantine/pi/packages/coding-agent/src/core/session-manager.ts:480–570](../../../quarantine/pi/packages/coding-agent/src/core/session-manager.ts#L480): Active context selects compaction/retained entries and overlays latest context edits while exposing sourceEntry provenance.

### Evidence repair

[quarantine/pi/packages/coding-agent/src/core/session-manager.ts:1359–1385](../../../quarantine/pi/packages/coding-agent/src/core/session-manager.ts#L1359): appendContextEdit validates branch-local targets and replacement shape without rewriting original message content.

### Evidence query

[quarantine/pi/packages/coding-agent/src/core/session-manager.ts:1464–1564](../../../quarantine/pi/packages/coding-agent/src/core/session-manager.ts#L1464): Branch/tree/all-entry retrieval is separate from compaction-aware context projection; append-only entry history remains accessible.

### Evidence runner-query

[quarantine/pi/packages/coding-agent/src/core/extensions/runner.ts:889–892](../../../quarantine/pi/packages/coding-agent/src/core/extensions/runner.ts#L889): Extensions receive the sessionManager with stale-runner checks.

### Evidence steer-abort

[quarantine/pi/packages/coding-agent/src/core/agent-session.ts:2100–2176](../../../quarantine/pi/packages/coding-agent/src/core/agent-session.ts#L2100): Steering is queued until after current assistant tools; follow-up waits until tool/steering work finishes.

### Evidence abort

[quarantine/pi/packages/coding-agent/src/core/agent-session.ts:2361–2377](../../../quarantine/pi/packages/coding-agent/src/core/agent-session.ts#L2361): abort cancels retry, compaction, branch summary and agent execution then waits for idle.

### Evidence reload

[quarantine/pi/packages/coding-agent/src/core/agent-session.ts:3587–3623](../../../quarantine/pi/packages/coding-agent/src/core/agent-session.ts#L3587): Reload shuts down/invalidate old extension contexts, reloads resources/providers/settings and builds a new extension runtime while keeping session state.

### Evidence reload-ui

[quarantine/pi/packages/coding-agent/src/modes/interactive/interactive-mode.ts:6230–6285](../../../quarantine/pi/packages/coding-agent/src/modes/interactive/interactive-mode.ts#L6230): Interactive /reload refuses streaming/compaction, then rebuilds chat and keybindings around session reload.

### Evidence audit-limits

[quarantine/pi/packages/coding-agent/src/core/nested-tool-calls.ts:21–99](../../../quarantine/pi/packages/coding-agent/src/core/nested-tool-calls.ts#L21): Nested-call records omit arguments beyond byte budgets, drop calls past 256, clip errors and mark incompleteness; durations use performance.now.

### Evidence forced-prompt

[quarantine/pi/packages/coding-agent/src/core/agent-session.ts:1685–1734](../../../quarantine/pi/packages/coding-agent/src/core/agent-session.ts#L1685): Forced provider head prompt is projected without recording that exact rendering in the transcript.

### Evidence extensions-api

[quarantine/pi/packages/coding-agent/src/core/extensions/types.ts:1618–1762](../../../quarantine/pi/packages/coding-agent/src/core/extensions/types.ts#L1618): Extension contract declares tool/command registration, messages, appendEntry, exec, active tools, model choice and immediate post-bind provider registration.

### Evidence mcp

[quarantine/pi/packages/coding-agent/src/extensions/mcp/runtime.ts:97–120](../../../quarantine/pi/packages/coding-agent/src/extensions/mcp/runtime.ts#L97): Configured MCP URL selects HTTP transport; command selects a stdio transport with cwd/env.

### Evidence mcp-call

[quarantine/pi/packages/coding-agent/src/extensions/mcp/runtime.ts:249–264](../../../quarantine/pi/packages/coding-agent/src/extensions/mcp/runtime.ts#L249): MCP getClient lazily reconnects and tool/resource calls consume service APIs.

### Evidence mcp-close

[quarantine/pi/packages/coding-agent/src/extensions/mcp/runtime.ts:314–349](../../../quarantine/pi/packages/coding-agent/src/extensions/mcp/runtime.ts#L314): Explicit reconnect drops old client and obtains a new one.

### Evidence pico-scope

[quarantine/pi/packages/agent/src/harness/pico3/index.ts:1–39](../../../quarantine/pi/packages/agent/src/harness/pico3/index.ts#L1): Pico3 is explicitly an experimental separate subpath, not the ordinary package-root CLI Agent loop.

### Evidence pico-api

[quarantine/pi/packages/agent/src/harness/pico3/harness.ts:58–109](../../../quarantine/pi/packages/agent/src/harness/pico3/harness.ts#L58): Experimental harness exposes task kinds, conversations, commit/fork/collapse/reset/abort/watches and input handles.

### Evidence pico-lifecycle

[quarantine/pi/packages/agent/src/harness/pico3/harness.ts:588–630](../../../quarantine/pi/packages/agent/src/harness/pico3/harness.ts#L588): Experimental harness exposes task marking/waiting and close via suspend; these do not establish CLI executable replacement.

### Evidence context-test

[quarantine/pi/packages/coding-agent/test/session-context-edit.test.ts:36–103](../../../quarantine/pi/packages/coding-agent/test/session-context-edit.test.ts#L36): Tests assert projection omission/restoration and original content retention with in-memory sessions.

### Evidence nested-test

[quarantine/pi/packages/coding-agent/test/nested-tool-calls.test.ts:164–231](../../../quarantine/pi/packages/coding-agent/test/nested-tool-calls.test.ts#L164): Tests assert sequential/parallel concurrency and explicit argument/call/error recording caps using fake tools.

### Evidence reload-test

[quarantine/pi/packages/coding-agent/test/experimental-plugin-reload.test.ts:7–93](../../../quarantine/pi/packages/coding-agent/test/experimental-plugin-reload.test.ts#L7): Experimental plugin test asserts new facet activation/old disposal using a stub lane, not default CLI continuity.

### Evidence authority-doc

[quarantine/pi/README.md:42–51](../../../quarantine/pi/README.md#L42): Official repository README states no built-in filesystem/process/network/credential permission system and inheritance of launcher authority.

