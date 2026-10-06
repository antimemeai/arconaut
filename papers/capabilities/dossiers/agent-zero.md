# agent-zero

Broad editable extension machinery changes arguments/results/continuation around core functions; persistent terminal sessions and parallel workers coexist with nonpersistent Python subprocess actions.

Role: extensible persistent Python agent and workbench. Runtime: Python, JavaScript (web UI).

Pinned source: [https://github.com/agent0ai/agent-zero](https://github.com/agent0ai/agent-zero); revision/version `e3051fb584b1a36be2b0a0c90606f1c2c2d356ec`.

Python async monologue with background event-loop threads, extension middleware, local/SSH persistent PTY shells and background worker contexts.

Owns local contexts, shells, vector memory and snapshots; consumes model/MCP/A2A/SSH/host bridge services. Local shells are agent-owned, not shared compute consumption.

Inspection: Read actual loop/provider/tools, implicit/explicit extension loader, code execution/PTTY lifetime, parallel/delegation/A2A, memory, history/persistence/time-travel and direct lifetime/compression test oracles.

Limits of this study: No acquired code executed. Not every built-in/community plugin or host bridge traced. Extension watchdog registration function exists but startup wiring not established in inspected paths; core provider adapter internals not exhaustive.

## Actions

### code_execution_tool / input / output / reset

Surface: model tools.

Input: code, terminal/python/node runtime, numbered session/reset

Result: terminal output and session-running state

Lifecycle: persistent shell, new Python/node process per code action; output timeout need not terminate command

Authority: model code execution plugin, local or configured SSH authority

Evidence: [code](#evidence-code), [session](#evidence-session), [terminal](#evidence-terminal).

### parallel start/background/await/cancel

Surface: model tool.

Input: tool_calls or job_ids, timeout/wait

Result: job handles, partial/final results/status

Lifecycle: background contexts, result collection; terminal cancellation label precedes cleanup settlement

Authority: model orchestrates ordinary tools/child contexts

Evidence: [parallel](#evidence-parallel), [cancel](#evidence-cancel), [defer](#evidence-defer).

### call_subordinate / a2a_chat

Surface: model tools.

Input: profile/context_id or remote URL, message/attachments

Result: subordinate reply/context ID or remote task result

Lifecycle: local awaited monologue or remote wait; busy child reuse refused

Authority: model delegates profile/remote agent; no general local busy peer mailbox established

Evidence: [delegate](#evidence-delegate), [busy](#evidence-busy), [a2a](#evidence-a2a).

### Explicit/implicit extension programs

Surface: programmable extension API.

Input: Python files at hook/function hierarchy; mutable arguments/results

Result: rewritten context/model/tool/result/exception

Lifecycle: per call; fresh local tools; cached extensions invalidated by watchdog primitive

Authority: ordinary extension code; host APIs broad, model can author via file/program actions

Evidence: [wrapper](#evidence-wrapper), [prompt](#evidence-prompt), [model](#evidence-model), [dispatch](#evidence-dispatch), [toolload](#evidence-toolload), [extload](#evidence-extload).

### memory save/load/delete/forget family

Surface: model tools.

Input: text/document metadata or similarity query/filter

Result: document identity and retrieved memories

Lifecycle: standing vector DB persists; not general SQL or interpreter heap

Authority: model memory plugin

Evidence: [memory](#evidence-memory), [memwrite](#evidence-memwrite).

### Time Travel snapshot/list/travel/revert

Surface: operator/API.

Input: workspace and commit identity

Result: snapshot/diff/affected files

Lifecycle: pre-change snapshot and preserved references; file restoration only

Authority: operator/API workspace scope

Evidence: [travel](#evidence-travel).

### notify_user

Surface: model tool.

Input: message/title/detail/priority/timeout

Result: display notification

Lifecycle: nonterminal notification

Authority: model invoke; no grievance-state/table mechanism

Evidence: [notify](#evidence-notify).

## Capabilities

### filesystem

**S — Files** (source): Model terminal programs and workspace snapshots edit/read files; dedicated all-plugin file tool families not exhaustively traced.

Evidence: [code](#evidence-code), [travel](#evidence-travel).

### processes

**I — OS programs** (source): Persistent numbered local/SSH PTY shells, output/input/reset and parallel lifetime handling; bounded kill settlement loses handle on final wait errors.

Evidence: [session](#evidence-session), [terminal](#evidence-terminal), [pty](#evidence-pty), [code](#evidence-code).

### code-actions

**I — Code actions** (source): Model Python/node/shell programs compose ordinary computing; native hooks receive full mutable objects.

Evidence: [code](#evidence-code), [session](#evidence-session), [wrapper](#evidence-wrapper).

### persistent-kernel

**L — Kernel** (source): Persistent shell holds cwd/process state; Python action invokes ipython -c each time, so Python variable namespace not persistent in this path.

Evidence: [session](#evidence-session), [code](#evidence-code).

### standing-database

**I — Standing DB** (source): Persisted vector documents with IDs/metadata and model similarity/filter query; scope is memory retrieval, not SQL research workbench.

Evidence: [memory](#evidence-memory), [memwrite](#evidence-memwrite).

### workflow-programming

**I — Workflows** (source): Implicit/explicit Python hooks may short circuit/replace functions and rewrite model/tool/continuation, plus parallel job tool.

Evidence: [implicit](#evidence-implicit), [wrapper](#evidence-wrapper), [dispatch](#evidence-dispatch), [parallel](#evidence-parallel).

### multi-model

**I — Models** (source): Per-call chat/utility model resolution and middleware can replace model/message programmatically; profiles/delegation retained.

Evidence: [model](#evidence-model), [delegate](#evidence-delegate).

### live-collaboration

**L — Peer chat** (source): Remote A2A conversational tasks with cached context; local child call awaited, separate busy child cannot be reused without await/cancel. No traced open local peer room.

Evidence: [a2a](#evidence-a2a), [delegate](#evidence-delegate), [busy](#evidence-busy).

### concurrent-work

**I — Concurrency** (source): Parallel tool/child background jobs and persistent multiple shells; ordinary tool loop distinct from parallel tool.

Evidence: [parallel](#evidence-parallel), [code](#evidence-code), [session](#evidence-session).

### steering-interrupt

**L — Steer/interrupt** (source): Cooperative pause/interventions at stream/tool checkpoints; cancellation marks job terminal after future.cancel rather than confirmed cleanup completion.

Evidence: [intervene](#evidence-intervene), [cancel](#evidence-cancel), [defer](#evidence-defer), [pty](#evidence-pty).

### turn-redefinition

**I — Turn program** (source): Actual mutable function-start/end and result hooks can replace prompt/model/function/continuation behavior; activation not default settled-work staging.

Evidence: [wrapper](#evidence-wrapper), [prompt](#evidence-prompt), [loop](#evidence-loop), [dispatch](#evidence-dispatch).

### compaction

**I — Compaction** (source): Nested topic/bulk summarization and token-decrease/pass-budget guard; provider state clearing test checks ID retention.

Evidence: [compress](#evidence-compress), [wait](#evidence-wait), [compact-test](#evidence-compact-test).

### context-repair

**L — Repair** (source): History may preserve nested summary records but old bulk fallback removes; only long nonbackground results archived separately. Time Travel is workspace files, not model-context repair operation.

Evidence: [compress](#evidence-compress), [large](#evidence-large), [travel](#evidence-travel).

### original-audit

**L — Original audit** (source): Logs capped1000, background excluded, tool-result extra archives only>=500; transformed/masked text and omitted runtime fields, not all original provider/program attempts.

Evidence: [serialize](#evidence-serialize), [save](#evidence-save), [large](#evidence-large), [terminal](#evidence-terminal).

### audit-query

**S — Audit query** (source): Model can read long-result archive paths and memory retrieval; full original IO absent and file time-travel query scoped separately.

Evidence: [large](#evidence-large), [memory](#evidence-memory), [travel](#evidence-travel).

### hot-change

**I — Hot change** (source): Tool classes imported from file per get_tool; prompt/model resolved per call. Cached extension invalidation primitive exists but startup watchdog wiring not established here.

Evidence: [toolload](#evidence-toolload), [module](#evidence-module), [alias](#evidence-alias), [prompt](#evidence-prompt), [model](#evidence-model), [extload](#evidence-extload).

### rebuild-continuity

**L — Rebuild continuity** (source): Chat reconstruction excludes underscore runtime state/shell handles/background contexts; source extension loading and file travel do not implement executable/outpost refit.

Evidence: [serialize](#evidence-serialize), [save](#evidence-save), [travel](#evidence-travel).

### remote-services

**I — Remote** (source): SSH interactive shell and remote A2A task consumer with cached chat ID; external standing service lifecycle not governed by these client actions.

Evidence: [session](#evidence-session), [a2a](#evidence-a2a).

### self-improvement

**S — Self-improve** (source): Editable broad extension programs plus versioned file snapshots support model self-change; independent experiment/evaluate/promote governance not traced.

Evidence: [wrapper](#evidence-wrapper), [code](#evidence-code), [travel](#evidence-travel).

### complaints

**L — Complaints** (source): Model notify_user gives nonterminal details/priority; automatic state capture, issue plus external complaint table not established.

Evidence: [notify](#evidence-notify).

### authority

**I — Authority** (source): Native code plugin dispatch runs model program and extension objects with broad access; precise all-plugin approvals/host deployment boundaries not fully studied.

Evidence: [code](#evidence-code), [wrapper](#evidence-wrapper), [dispatch](#evidence-dispatch).

### evaluation

**S — Evaluation** (source): Command-once/poll/cancellation-close mocks and exact stalled-compression/ID state oracles; OS termination/provider settlement not tested here.

Evidence: [lifetime-test](#evidence-lifetime-test), [compact-test](#evidence-compact-test).

### time-order

**S — Time/order** (source): Monotonic stream cadence, wall completed-job time and ordered history; paused callbacks do not establish pauseable logical deadlines.

Evidence: [loop](#evidence-loop), [cancel](#evidence-cancel), [intervene](#evidence-intervene).

## Inspected test oracles

- [quarantine/agent-zero/tests/test_parallel_code_lifetime.py](../../../quarantine/agent-zero/tests/test_parallel_code_lifetime.py): side-effect single execution and owned shell cleanup Oracle: Exact execute once/poll rules and close mocked on cancellation; no actual OS termination proof. Read, **not executed**.
- [quarantine/agent-zero/tests/test_history_compression_wait.py](../../../quarantine/agent-zero/tests/test_history_compression_wait.py): compaction progress and provider-context reset Oracle: No-progress stops once, max-pass bound and response IDs preserved while active IDs removed; fake history, not summary quality oracle. Read, **not executed**.

## Useful mechanisms

- Substantial extensible function machinery with mutable args/results/exceptions.
- Real ordinary terminal/SSH integration and explicit parallel result handles.
- Atomic chat persistence includes file and directory fsync.

## Material limits

- Persistent shell does not make ipython -c actions a persistent Python kernel.
- Cancellation status can precede cleanup, and terminal close forgets handle after suppressed final wait errors.
- Selective chat/log/results omit background work and runtime state.

## Arconaut design questions

- Give model executable turn programs with explicit version/activation semantics, beyond ad hoc callback freshness.
- Keep wait timeout, cancel request and confirmed resource settlement distinct.
- Separate terminal session, interpreter namespace and shared service lease contracts.

## Evidence

### Evidence loop

[quarantine/agent-zero/agent.py:431–538](../../../quarantine/agent-zero/agent.py#L431): Provider stream hooks, interventions, model-result hook may skip default processing; tool break result ends monologue.

### Evidence prompt

[quarantine/agent-zero/agent.py:564–626](../../../quarantine/agent-zero/agent.py#L564): Before/after prompt extensions rewrite system/history; latest formatted context stored separately.

### Evidence model

[quarantine/agent-zero/agent.py:887–959](../../../quarantine/agent-zero/agent.py#L887): Per-call model resolved and call-data/model/messages/callbacks exposed to before/after middleware.

### Evidence implicit

[quarantine/agent-zero/helpers/extension.py:80–103](../../../quarantine/agent-zero/helpers/extension.py#L80): Implicit function extension contract allows argument/result replacement, short circuit and exception replacement.

### Evidence wrapper

[quarantine/agent-zero/helpers/extension.py:167–189](../../../quarantine/agent-zero/helpers/extension.py#L167): Actual async decorator executes start/original/end with mutable data.

### Evidence extload

[quarantine/agent-zero/helpers/extension.py:326–396](../../../quarantine/agent-zero/helpers/extension.py#L326): Agent-path extension precedence and cached classes; watchdog callbacks clear class caches, startup wiring not established here.

### Evidence toolload

[quarantine/agent-zero/agent.py:1581–1616](../../../quarantine/agent-zero/agent.py#L1581): Tool class located in profile hierarchy then freshly loaded and instantiated.

### Evidence module

[quarantine/agent-zero/helpers/modules.py:12–24](../../../quarantine/agent-zero/helpers/modules.py#L12): Tool module file executes via importlib spec per load.

### Evidence alias

[quarantine/agent-zero/helpers/extract_tools.py:1–4](../../../quarantine/agent-zero/helpers/extract_tools.py#L1): Agent loader alias resolves to actual modules loader.

### Evidence dispatch

[quarantine/agent-zero/agent.py:1491–1522](../../../quarantine/agent-zero/agent.py#L1491): Tool before/after and argument/result extension hooks; returned break_loop determines continuation.

### Evidence code

[quarantine/agent-zero/plugins/_code_execution/tools/code_execution_tool.py:55–112](../../../quarantine/agent-zero/plugins/_code_execution/tools/code_execution_tool.py#L55): Terminal/Python/Node/output/reset actions; parallel workers keep polling then close shells in finally.

### Evidence session

[quarantine/agent-zero/plugins/_code_execution/tools/code_execution_tool.py:134–185](../../../quarantine/agent-zero/plugins/_code_execution/tools/code_execution_tool.py#L134): Persistent per-agent shell state; local/SSH connect cleanup; Python action is ipython -c subprocess, not persistent namespace.

### Evidence terminal

[quarantine/agent-zero/plugins/_code_execution/tools/code_execution_tool.py:206–261](../../../quarantine/agent-zero/plugins/_code_execution/tools/code_execution_tool.py#L206): Existing shell command/output polling; one closed-PTY retry can resubmit command; printable command masked/clipped.

### Evidence pty

[quarantine/agent-zero/plugins/_code_execution/helpers/tty_session.py:69–145](../../../quarantine/agent-zero/plugins/_code_execution/helpers/tty_session.py#L69): TERM then KILL bounded waits and FD release-once; final wait errors suppressed before dropping process handle.

### Evidence intervene

[quarantine/agent-zero/agent.py:1092–1115](../../../quarantine/agent-zero/agent.py#L1092): Cooperative pause at checkpoints, intervention preserves partial text and raises; does not globally suspend subprocess/provider work.

### Evidence parallel

[quarantine/agent-zero/tools/parallel.py:23–77](../../../quarantine/agent-zero/tools/parallel.py#L23): Start/background/await/cancel model tool with job IDs and wait semantics.

### Evidence cancel

[quarantine/agent-zero/helpers/parallel_tools.py:650–676](../../../quarantine/agent-zero/helpers/parallel_tools.py#L650): Calls deferred kill then immediately marks job terminal; no awaited settlement in this function.

### Evidence defer

[quarantine/agent-zero/helpers/defer.py:169–186](../../../quarantine/agent-zero/helpers/defer.py#L169): Ordinary deferred kill cancels future without awaiting task cleanup; optional thread drain separate.

### Evidence delegate

[quarantine/agent-zero/tools/call_subordinate.py:197–243](../../../quarantine/agent-zero/tools/call_subordinate.py#L197): Subordinate monologue awaited and context persisted, existing context_id continuation supported.

### Evidence busy

[quarantine/agent-zero/tools/call_subordinate.py:144–155](../../../quarantine/agent-zero/tools/call_subordinate.py#L144): Reusing separate subordinate while running refused; must await or cancel parallel job.

### Evidence a2a

[quarantine/agent-zero/tools/a2a_chat.py:91–131](../../../quarantine/agent-zero/tools/a2a_chat.py#L91): Remote task send/wait and cached provider context ID; awaited RPC, not local peer chat room.

### Evidence memory

[quarantine/agent-zero/plugins/_memory/tools/memory_load.py:8–27](../../../quarantine/agent-zero/plugins/_memory/tools/memory_load.py#L8): Model standing memory similarity query with limit/threshold/filter.

### Evidence memwrite

[quarantine/agent-zero/plugins/_memory/helpers/memory.py:470–494](../../../quarantine/agent-zero/plugins/_memory/helpers/memory.py#L470): Document IDs/timestamps/area and persisted vector insert/update.

### Evidence compress

[quarantine/agent-zero/helpers/history.py:525–627](../../../quarantine/agent-zero/helpers/history.py#L525): Topic/bulk summaries and nested records with progress guard; old bulk fallback may remove records.

### Evidence wait

[quarantine/agent-zero/extensions/python/message_loop_prompts_before/_90_organize_history_wait.py:12–66](../../../quarantine/agent-zero/extensions/python/message_loop_prompts_before/_90_organize_history_wait.py#L12): Compaction wait exits on no token reduction or max passes and logs stall.

### Evidence save

[quarantine/agent-zero/helpers/persist_chat.py:50–118](../../../quarantine/agent-zero/helpers/persist_chat.py#L50): Background contexts excluded; chat atomic write flush/file+directory fsync, later load reconstructs contexts.

### Evidence serialize

[quarantine/agent-zero/helpers/persist_chat.py:186–246](../../../quarantine/agent-zero/helpers/persist_chat.py#L186): Underscore runtime state excluded, history serialized and log capped1000; not live shell/parallel/provider continuation image.

### Evidence large

[quarantine/agent-zero/extensions/python/hist_add_tool_result/_90_save_tool_call_file.py:9–44](../../../quarantine/agent-zero/extensions/python/hist_add_tool_result/_90_save_tool_call_file.py#L9): Only large>=500 nonbackground tool results separately saved and file path attached.

### Evidence travel

[quarantine/agent-zero/plugins/_time_travel/helpers/time_travel.py:875–925](../../../quarantine/agent-zero/plugins/_time_travel/helpers/time_travel.py#L875): Workspace Git snapshot/travel/revert changes files with preserved reference; not captured model context/runtime refit.

### Evidence notify

[quarantine/agent-zero/tools/notify_user.py:7–37](../../../quarantine/agent-zero/tools/notify_user.py#L7): Model notification title/detail/priority/display-time, no agent-state complaint database linkage.

### Evidence lifetime-test

[quarantine/agent-zero/tests/test_parallel_code_lifetime.py:11–73](../../../quarantine/agent-zero/tests/test_parallel_code_lifetime.py#L11): Mocks prove command once and poll/close rules even cancellation; followup-session jobs rejected, actual OS death not oracle.

### Evidence compact-test

[quarantine/agent-zero/tests/test_history_compression_wait.py:89–130](../../../quarantine/agent-zero/tests/test_history_compression_wait.py#L89): Literal compression pass limits and retained response ID list after active state clearing.

