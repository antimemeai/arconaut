# bub

Operator/model tool symmetry, replaceable turn stages, tape-derived context, code-mode tool orchestration and channel admission/steering.

Role: plugin-composable coding/chat agent runtime. Runtime: Python, TypeScript (website).

Pinned source: [https://github.com/bubbuild/bub](https://github.com/bubbuild/bub); revision/version `0fe2fd51ffd83d352361488caec366507a3826c5`.

Asyncio turn/channel loop, Pluggy stage/interception callbacks, any-llm provider streams and native shell/fresh Python subprocess code actions.

Owns default append-style file tape, sidecars, scoped shell manager and session environment lifecycle; accepts caller-owned tape stores/environment plugins and consumes providers/channels.

Inspection: framework/stage hooks, model/tool loop, tape projection/fork/merge/storage/query/overflow/spill, native shell/code/subagent and inspected mutation/lifecycle/archive test assertions

Limits of this study: No upstream execution. Provider SDK/channel/plugin implementations not fully inspected; default FileTapeStore/LocalEnvironment specifically traced and not generalized to all plugin-backed services.

## Actions

### process_inbound / Agent.run_stream

Surface: operator/program/channel turn API.

Input: Envelope/session prompt/state/model/allowed tools/skills

Result: TurnResult/outbounds or AsyncStreamEvents usage/error

Lifecycle: framework resolve→state→prompt→model→finally save→render→dispatch; SDK stream holds fork until consumed/closed

Authority: registered plugins and caller-owned explicit stores; same-session stream serialization caller concern

Evidence: [e2](#evidence-e2), [e3](#evidence-e3), [e4](#evidence-e4), [e12](#evidence-e12).

### resolve_session / load_state / build_prompt / run_model(_stream) / save_state / render_outbound / dispatch_outbound

Surface: programmable stage plugin contract.

Input: message/session/state/model prompt or output

Result: modified/replaced stage result/outbound

Lifecycle: priority stage selection each call; save finally and lifespans close

Authority: operator/program entry-point plugin can replace whole turn; no live refit barrier implied

Evidence: [e1](#evidence-e1), [e2](#evidence-e2), [e3](#evidence-e3).

### before_llm_call / before_tool_call / after_tool_call

Surface: programmable agent interception.

Input: request/model/messages or named call/effective args/outcome

Result: changed request/finish/proceed/replace/deny or changed result

Lifecycle: ordered transformations; exceptions logged/skipped; cancellation not terminal after-LLM observation

Authority: plugin has substantial normal-turn agency; LLM tool_names observational

Evidence: [e4](#evidence-e4), [e6](#evidence-e6), [e7](#evidence-e7), [e26](#evidence-e26).

### bash / bash.output / bash.kill

Surface: model and operator command.

Input: command/cwd/background/timeout then shell_id/offset/limit

Result: text/exit/status/cursor or background handle

Lifecycle: foreground timeout continues background; explicit kill; scoped shutdown drains pending spawn

Authority: selected Environment host groups/program authority; commands share tooling

Evidence: [e16](#evidence-e16), [e17](#evidence-e17), [e27](#evidence-e27).

### fs.read / fs.write / fs.edit; web.fetch

Surface: model and operator callable tools.

Input: path/offset/limit/text replacement; URL/headers/timeout

Result: contents/write/edit or fetched text

Lifecycle: await configured environment/file or HTTP request

Authority: default host path authority; caller controls plugins and tool filters

Evidence: [e16](#evidence-e16), [e17](#evidence-e17), [e22](#evidence-e22).

### run_code / code_mode

Surface: model Python action and operator setting command.

Input: Python/top-level await, timeout; enable bool

Result: printed output or typed error with prior output; persisted next-turn setting

Lifecycle: fresh process each action, full builtins + tools RPC; pending RPC canceled/gathered on done, kill finally

Authority: enabled code tools/context; structured tool results bypass spill/post-render; not sandbox proof

Evidence: [e18](#evidence-e18), [e19](#evidence-e19), [e20](#evidence-e20), [e21](#evidence-e21).

### subagent

Surface: model callable delegation.

Input: prompt/model/session inherit,temp,named/allowed tools/skills

Result: session_id/output/errors

Lifecycle: await child stream; temp does not merge back; inherited named tape can share context

Authority: instance tool filters exclude recursive subagent; state shallow copied

Evidence: [e22](#evidence-e22).

### tape.info / tape.search / tape.anchors / tape.handoff / tape.reset

Surface: model/operator data/context commands.

Input: query/kinds/dates/limit; anchor/summary; archive bool

Result: entry matches/info/anchors/new boundary or archive/reset result

Lifecycle: handoff appends anchor/event, default context AFTER anchor; reset destructive unless archive

Authority: same native command/tool runtime; original tape distinct projected context

Evidence: [e8](#evidence-e8), [e9](#evidence-e9), [e10](#evidence-e10), [e12](#evidence-e12), [e22](#evidence-e22).

### spill.read / SpillStore.spill_tool_result

Surface: model retrieval + support sidecar API.

Input: oversized rendered result; handle/cursor/count/from_end

Result: preview/manifest/chunk pages/complete cursor or incomplete storage error

Lifecycle: UTF8 chunk archive, bounded retrieval; temporary fork discarded; failure returns truncated preview

Authority: mounted sibling tape authority, structured code values bypass spill

Evidence: [e15](#evidence-e15), [e18](#evidence-e18).

### admit_message / quit / steering inbox

Surface: programmable channel/operator control.

Input: session turn snapshot/message; session ID

Result: process/drop/followup/steer or canceled task count

Lifecycle: steer active else queue; quit clears pending and awaits canceled sibling tasks

Authority: channel plugin/operator policy, current caller excluded from self-cancel

Evidence: [e23](#evidence-e23), [e24](#evidence-e24).

### ,model / ,reasoning_effort / tool decorator

Surface: operator settings and discovery registration.

Input: provider:model/effort; annotated callable/context/agent_use/preserve metadata

Result: next-turn persisted settings or registered named tool/schema

Lifecycle: tool registry snapshotted per Agent, plugins/later callbacks; no module hotreload contract

Authority: operators and model share callable primitives; model-use filters differ by tool

Evidence: [e25](#evidence-e25), [e6](#evidence-e6), [e18](#evidence-e18).

## Capabilities

### filesystem

**I — Files** (source): fs.read/write/edit delegate configured Environment; default native host paths allow absolute/expanded paths, no project-only sandbox inferred.

Evidence: [e16](#evidence-e16), [e17](#evidence-e17).

### processes

**I — OS programs** (source): Native bash/group-spawn/output/kill and lifetime cleanup; timeout advertises still-running background handle, environment plugin can replace host.

Evidence: [e16](#evidence-e16), [e17](#evidence-e17), [e24](#evidence-e24), [e27](#evidence-e27).

### code-actions

**I — Code actions** (source): Model run_code full Python/top-level await with structured async tools RPC, default fresh subprocess always killed afterward; code-mode chosen operator next-turn.

Evidence: [e18](#evidence-e18), [e19](#evidence-e19), [e20](#evidence-e20), [e21](#evidence-e21).

### persistent-kernel

**L — Kernel** (source): Default code action fresh child namespace and teardown; shell handles resident manager, not kernel heap checkpoint/shared service. Environment plugin supports alternate composition.

Evidence: [e3](#evidence-e3), [e16](#evidence-e16), [e17](#evidence-e17), [e20](#evidence-e20), [e21](#evidence-e21).

### standing-database

**I — Standing DB** (source): Queryable tape/sidecar file records standing across turns; injectable caller-owned store protocol. Default file journal not general transactional shared DB.

Evidence: [e8](#evidence-e8), [e9](#evidence-e9), [e10](#evidence-e10), [e13](#evidence-e13), [e15](#evidence-e15).

### workflow-programming

**I — Workflows** (source): All turn stages replaceable, code tools async orchestration, subagents with inheritance/temp/named session and model override; actual callable/discovery paths.

Evidence: [e1](#evidence-e1), [e2](#evidence-e2), [e3](#evidence-e3), [e18](#evidence-e18), [e19](#evidence-e19), [e22](#evidence-e22).

### multi-model

**I — Models** (source): Per-turn/session model and subagent override with inherited/temp/named tapes; before-LLM model rewrite, provider candidates consume SDK.

Evidence: [e4](#evidence-e4), [e7](#evidence-e7), [e22](#evidence-e22), [e25](#evidence-e25).

### live-collaboration

**I — Peer chat** (source): Channel turn admission supports shared session process/steer/followup and subagent inherited/named sessions; SDK run_stream does not itself serialize concurrent same-session turns.

Evidence: [e2](#evidence-e2), [e22](#evidence-e22), [e23](#evidence-e23), [e24](#evidence-e24).

### concurrent-work

**I — Concurrency** (source): Tool gather, code RPC tasks, channel active tasks/subagents. Owned-work cleanup and temporary tape merge boundaries distinct from shared service lifecycle.

Evidence: [e5](#evidence-e5), [e19](#evidence-e19), [e22](#evidence-e22), [e24](#evidence-e24), [e27](#evidence-e27).

### steering-interrupt

**L — Steer/interrupt** (source): Channel steer/followup and quit cancels/awaits siblings; code finally kills runner and gathers pending RPC, shell cleanup waits in-flight spawn. Hook terminal observations skip cancellation/consumer close.

Evidence: [e4](#evidence-e4), [e19](#evidence-e19), [e20](#evidence-e20), [e23](#evidence-e23), [e24](#evidence-e24), [e27](#evidence-e27).

### turn-redefinition

**I — Turn program** (source): Stage plugins replace full flow; before-provider message/model transform/short circuit and tool argument/result transform are wired. Tool names observational in LLM request hook, not toolset mutation.

Evidence: [e1](#evidence-e1), [e2](#evidence-e2), [e3](#evidence-e3), [e4](#evidence-e4), [e6](#evidence-e6), [e7](#evidence-e7), [e26](#evidence-e26).

### compaction

**L — Compaction** (source): Model manual handoff and overflow anchor move context boundary, originals remain tape; default after-anchor selection excludes stored anchor.state summary, overflow only records reason/error without substantive summary.

Evidence: [e8](#evidence-e8), [e9](#evidence-e9), [e10](#evidence-e10), [e11](#evidence-e11), [e22](#evidence-e22).

### context-repair

**S — Repair** (source): Queryable originals, anchor/selector plugin and tape.search can recover previous material; default malformed read/fork redaction/default anchor exclusion limit faithful automatic repair.

Evidence: [e8](#evidence-e8), [e9](#evidence-e9), [e10](#evidence-e10), [e12](#evidence-e12), [e13](#evidence-e13), [e22](#evidence-e22).

### original-audit

**L — Original audit** (source): Append-style tape originals/spill-sidecar text retained when writes succeed; fork strips nontext inputs, posthooks may replace before record, cancellation skips after-LLM and completed-turn records, malformed rows ignored, reset destructive unless archive requested.

Evidence: [e4](#evidence-e4), [e6](#evidence-e6), [e12](#evidence-e12), [e13](#evidence-e13), [e14](#evidence-e14), [e15](#evidence-e15), [e22](#evidence-e22).

### audit-query

**L — Audit query** (source): Native text/date/kind/limit/anchor query and tape tools; selected ForkTapeStore child path applies kind/anchor but omits text/date filters unlike parent store, so in-flight fork query semantics differ.

Evidence: [e10](#evidence-e10), [e12](#evidence-e12), [e22](#evidence-e22).

### hot-change

**L — Hot change** (source): Model/reasoning/code-mode settings explicitly next-turn persisted tape events; dynamic plugin manager/stage selection supports composition but no safe executable source reload/refit activation barrier.

Evidence: [e1](#evidence-e1), [e3](#evidence-e3), [e18](#evidence-e18), [e25](#evidence-e25).

### rebuild-continuity

**L — Rebuild continuity** (source): File tape/settings survive process restart and rebuild context; fresh code namespace/owned shell Maps and provider stream not restored. Exact shell cleanup test addresses shutdown race, not full compiled harness outpost.

Evidence: [e13](#evidence-e13), [e18](#evidence-e18), [e20](#evidence-e20), [e25](#evidence-e25), [e27](#evidence-e27).

### remote-services

**I — Remote** (source): Environment/store/provider/channel hooks consume alternate services; caller supplied store lifecycle belongs caller, default native environment owned by framework.

Evidence: [e2](#evidence-e2), [e3](#evidence-e3), [e17](#evidence-e17), [e22](#evidence-e22).

### self-improvement

**S — Self-improve** (source): Replaceable stage/code workflows and shared tools allow model/operator experiments; no inspected candidate harness evaluation/promotion or managed autoresearch program.

Evidence: [e1](#evidence-e1), [e3](#evidence-e3), [e18](#evidence-e18), [e22](#evidence-e22).

### complaints

**S — Complaints** (source): on_error hooks, tape error/events/run correlation and plugin init diagnostics can underpin complaint filing; no dedicated external state-snapshot/bead-table contract.

Evidence: [e1](#evidence-e1), [e2](#evidence-e2), [e7](#evidence-e7), [e14](#evidence-e14).

### authority

**I — Authority** (source): Operator commands share native tool implementations, model allowed tool/skill filters, plugin explicit proceed/replace/deny; hook exceptions skipped rather than denied. Default full host Python/filesystem authority.

Evidence: [e6](#evidence-e6), [e7](#evidence-e7), [e17](#evidence-e17), [e18](#evidence-e18), [e21](#evidence-e21), [e22](#evidence-e22).

### evaluation

**S — Evaluation** (source): Exact hook argument propagation, shell cleanup race, monotonic entry IDs and spill archive tests validate mechanisms; social evaluation docs not treated as implemented truth/harness optimizer.

Evidence: [e26](#evidence-e26), [e27](#evidence-e27).

### time-order

**L — Time/order** (source): Per-run correlation/date/IDs, tool results gather order and monotonic append IDs within tape object; sidecar/main fork merge nontransactional, effects happen before persisted record and cancellation can leave gaps.

Evidence: [e5](#evidence-e5), [e12](#evidence-e12), [e13](#evidence-e13), [e14](#evidence-e14), [e19](#evidence-e19).

## Inspected test oracles

- [quarantine/bub/tests/test_agent_hooks.py](../../../quarantine/bub/tests/test_agent_hooks.py): phase transform ordering and actual execution Oracle: Hooks chain LIFO request/argument changes; exact ran:safe-ls proves modified args reached handler; deny asserts later hooks not called. Includes mutable after-result semantics. No full audit originals or refit oracle. Read, **not executed**.
- [quarantine/bub/tests/test_shell_manager.py](../../../quarantine/bub/tests/test_shell_manager.py): spawn/shutdown/cancellation ownership races Oracle: Actual source test gates delayed spawn, checks lifespan unresolved until spawn released, then closed-runtime rejection, child terminated and Maps empty. Read only; external daemons/provider settlement outside oracle. Read, **not executed**.
- [quarantine/bub/tests/test_spill.py](../../../quarantine/bub/tests/test_spill.py): oversized rendered result preservation/archive and code mode boundary Oracle: Archive checks main/spill handle/file presence then reset no-result; code-mode exact structured large value remains unchanged/unspilled. Handle containment alone is weaker than complete archive byte recovery, other page tests compare original text. Read, **not executed**.
- [quarantine/bub/tests/test_file_tape_store_entry_ids.py](../../../quarantine/bub/tests/test_file_tape_store_entry_ids.py): append identity on repeated fork merge Oracle: Exact IDs [1,2] and names prove monotonic parent IDs across sequential merges, not multi-process writer/crash transaction correctness. Read, **not executed**.

## Useful mechanisms

- Full turn-stage replacement plus actual effective tool input/result transforms, with operator/model native callable symmetry.
- Queryable original tape distinct from model projection; large text retained as UTF8 chunks when spill succeeds.
- Next-turn settings and scoped native process cleanup have explicit activation/lifetime semantics and useful race oracle.

## Material limits

- Default after-anchor context excludes anchor-state summary; overflow handoff lacks substantive summary.
- Fork strips nontext inputs; posthooks/temporary fork/spill failure/cancellation and malformed rows bound original audit.
- File append/fork merge no complete cross-process transaction; fresh code action and resident shell Maps not rebuild shared-kernel continuity.

## Arconaut design questions

- Can tape-like exact original custody and configurable projections preserve media, pre/post transformations and canceled/late events without default data loss?
- What contract makes handoff summary actually visible after its boundary and allows model-governed repair?
- Can stage/environment plugins act as consumers of shared services while owned work inventory enforces refit quiescence?

## Evidence

### Evidence e1

[quarantine/bub/src/bub/framework.py:79–123](../../../quarantine/bub/src/bub/framework.py#L79): Loads builtins then entry-point plugins; failed init diagnostics; stage implementation precedence and caller-configurable manager.

### Evidence e2

[quarantine/bub/src/bub/framework.py:150–208](../../../quarantine/bub/src/bub/framework.py#L150): Build fresh state from reversed hook contributions, then resolve/load/build/run/save/render/dispatch turn; save finally even model failure.

### Evidence e3

[quarantine/bub/src/bub/hooks/specs.py:40–151](../../../quarantine/bub/src/bub/hooks/specs.py#L40): Turn stages replaceable, model changes next turn, environment supplied/cached per session and closed on runtime exit.

### Evidence e4

[quarantine/bub/src/bub/builtin/model_runner.py:187–294](../../../quarantine/bub/src/bub/builtin/model_runner.py#L187): Build request then before-LLM transforms/finish; after once completed/failure but cancellation/consumer close bypasses; tools execute then record_chat.

### Evidence e5

[quarantine/bub/src/bub/tools.py:278–309](../../../quarantine/bub/src/bub/tools.py#L278): ToolExecutor gathers independent invocations, preserves results order, failures structured; unexpected BaseException propagated.

### Evidence e6

[quarantine/bub/src/bub/tools.py:360–424](../../../quarantine/bub/src/bub/tools.py#L360): Mutable effective argument pipeline reaches actual handler; after hooks may replace result, errors normalized. Sync callable not automatically offloaded.

### Evidence e7

[quarantine/bub/src/bub/hooks/interception.py:124–211](../../../quarantine/bub/src/bub/hooks/interception.py#L124): LLM request chain change/model/messages/finish and tool argument chain/proceed/replace/deny; plugin exceptions skipped, not veto; after mutable result shared.

### Evidence e8

[quarantine/bub/src/bub/tape.py:148–177](../../../quarantine/bub/src/bub/tape.py#L148): Context selection default LAST_ANCHOR builds after-last query; explicit selector/anchor=None can reconstruct all history.

### Evidence e9

[quarantine/bub/src/bub/tape.py:291–329](../../../quarantine/bub/src/bub/tape.py#L291): read_messages filters context:false then projection; handoff appends anchor(state) and event without modifying old entries.

### Evidence e10

[quarantine/bub/src/bub/store.py:180–213](../../../quarantine/bub/src/bub/store.py#L180): Default store query slices AFTER anchor, then date/query/kind/limit filters; anchor itself and its state summary are excluded from default messages.

### Evidence e11

[quarantine/bub/src/bub/builtin/agent.py:350–380](../../../quarantine/bub/src/bub/builtin/agent.py#L350): Overflow auto-handoff writes reason/error state but no substantive summary, then retries original prompt once.

### Evidence e12

[quarantine/bub/src/bub/store.py:307–377](../../../quarantine/bub/src/bub/store.py#L307): Fork parent read fallback silently empty on errors, child query filters kinds/anchor but not query/date; strips nontext prompt parts; merges sidecars then main nontransactionally.

### Evidence e13

[quarantine/bub/src/bub/store.py:502–560](../../../quarantine/bub/src/bub/store.py#L502): File store incremental read skips malformed JSON/entries; append locks per cached tape object and assigns monotonic local IDs; no process lock/fsync contract in inspected writer.

### Evidence e14

[quarantine/bub/src/bub/tape.py:337–384](../../../quarantine/bub/src/bub/tape.py#L337): Chat persists selected input/system/text/calls/results/usage event after tools; full provider response object not persisted raw.

### Evidence e15

[quarantine/bub/src/bub/builtin/spill.py:121–189](../../../quarantine/bub/src/bub/builtin/spill.py#L121): Oversized rendered results chunked UTF8 in sidecar with manifest; failure returns bounded truncation notice/preview and diagnostic event rather than original.

### Evidence e16

[quarantine/bub/src/bub/builtin/tools.py:242–337](../../../quarantine/bub/src/bub/builtin/tools.py#L242): Native bash returns background handle even foreground timeout, output/kill tools; environment read/write/edit delegated, direct operation no blanket routine prompt.

### Evidence e17

[quarantine/bub/src/bub/builtin/environment.py:54–133](../../../quarantine/bub/src/bub/builtin/environment.py#L54): Local environment host filesystem/processes own Unix process groups, TERM/KILL; run_code delegates fresh child; paths resolve but no project-only sandbox.

### Evidence e18

[quarantine/bub/src/bub/builtin/codemode/__init__.py:254–310](../../../quarantine/bub/src/bub/builtin/codemode/__init__.py#L254): Model run_code gets structured async tool RPC without text spill; timeout/error output; operator code_mode command recorded next-turn persistent setting.

### Evidence e19

[quarantine/bub/src/bub/builtin/codemode/code_runner.py:45–86](../../../quarantine/bub/src/bub/builtin/codemode/code_runner.py#L45): Code runner spawns tool-call tasks per RPC; on done/failure cancels/gathers outstanding calls, cannot transactionally undo provider/external work.

### Evidence e20

[quarantine/bub/src/bub/builtin/codemode/code_runner.py:107–142](../../../quarantine/bub/src/bub/builtin/codemode/code_runner.py#L107): Each code action new subprocess; always force stop/reap in finally, no persistent interpreter namespace.

### Evidence e21

[quarantine/bub/src/bub/builtin/codemode/code_runner_child.py:145–172](../../../quarantine/bub/src/bub/builtin/codemode/code_runner_child.py#L145): Child compiles Python top-level await with full builtins and async tools namespace; no state transfer between actions.

### Evidence e22

[quarantine/bub/src/bub/builtin/tools.py:361–455](../../../quarantine/bub/src/bub/builtin/tools.py#L361): Model tape info/search/reset/handoff/anchors/web.fetch/subagent; inherited/temp/named sessions, model override, filters prevent recursive subagent tool.

### Evidence e23

[quarantine/bub/src/bub/channels/manager.py:239–272](../../../quarantine/bub/src/bub/channels/manager.py#L239): Admission process/drop/follow-up/steer; steer running turn else queue pending.

### Evidence e24

[quarantine/bub/src/bub/channels/manager.py:130–148](../../../quarantine/bub/src/bub/channels/manager.py#L130): Quit clears pending and cancels/awaits active sibling tasks, avoids canceling current caller.

### Evidence e25

[quarantine/bub/src/bub/builtin/tools.py:493–513](../../../quarantine/bub/src/bub/builtin/tools.py#L493): Operator model/reasoning settings recorded next-turn, persist across restarts; invalid model error next turn with recover command.

### Evidence e26

[quarantine/bub/tests/test_agent_hooks.py:203–216](../../../quarantine/bub/tests/test_agent_hooks.py#L203): Exact native-handler result ran:safe-ls proves modified arguments execute, unlike decision-only transform.

### Evidence e27

[quarantine/bub/tests/test_shell_manager.py:221–259](../../../quarantine/bub/tests/test_shell_manager.py#L221): Delayed spawn shutdown test asserts lifespan remains unsettled until spawn completes, closed-start rejection and processes terminated/Maps empty.

