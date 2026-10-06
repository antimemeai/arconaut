# Persistent agents and gateways

Source study, 2026-10-01. Seven independent implementations were read: Nanobot,
Agent Zero, Hermes, NullClaw, IronClaw, ZeroClaw and OpenClaw. These names do not imply
shared implementation or transferable guarantees. Linked dossiers contain the complete
23-axis decisions, representative action inventories, exact source ranges, read files
and test-oracle limitations. No acquired program or provider request was executed.

| Reference | Most useful inspected mechanism | Material boundary |
| --- | --- | --- |
| [Nanobot](../dossiers/nanobot.md) | Captured immutable provider runtime; checkpointed stop versus shutdown recovery | Model self-control is opt-in/allowlisted; selective transcripts and bounded IO |
| [Agent Zero](../dossiers/agent-zero.md) | Editable extensions change arguments/results/continuation; terminal/SSH handles | Persistent shell does not preserve Python namespace; cancellation can precede cleanup |
| [Hermes](../dossiers/hermes-agent.md) | Real persistent Python; non-destructive SQL compaction; child steering | Tool-worker abandonment and destructive kernel timeouts |
| [NullClaw](../dossiers/nullclaw.md) | Actual read-only SQL tool; explicit hot-setting subset; native Zig core | Final-pair persistence, destructive context trimming and signature dedup risk |
| [IronClaw](../dossiers/ironclaw.md) | Typed lifecycle/projection boundaries and atomic swaps of individual provider wrappers | Sealed turn strategy; default model subagent spawning disabled; detached sandbox work |
| [ZeroClaw](../dossiers/zeroclaw.md) | Coupled config generations; model pipeline interpreter; peer turn admission | Recipient gets a fresh turn; durable Cancelled can precede worker shutdown |
| [OpenClaw](../dossiers/openclaw.md) | Parked executable cells retain custody through cleanup; addressed receiver steering | Fresh cell namespace; bounded diagnostics; channel reload can proceed active |

## State has different owners and lifetimes

Agent Zero's terminal session persists, but its Python action executes `ipython -c` in a
new subprocess. Hermes instead has a cross-cell Python namespace: ordinary exceptions
preserve it, while timeout/interrupt tears it down. Hermes' remote path can fall back
to per-call scripts if persistent startup fails. OpenClaw preserves one live JS cell
across waits, but each new exec constructs a fresh namespace and its continuation
explicitly lacks a serialized VM image. These are three different capabilities.
[Agent Zero execution](../../../quarantine/agent-zero/plugins/_code_execution/tools/code_execution_tool.py#L206),
[Hermes kernel](../../../quarantine/hermes-agent/tools/code_kernel.py#L781),
[remote fallback](../../../quarantine/hermes-agent/tools/code_execution_tool.py#L659),
[OpenClaw fresh cell](../../../quarantine/openclaw/src/agents/code-mode-node.worker.ts#L220),
[live continuation](../../../quarantine/openclaw/src/agents/code-mode-node.ts#L221).

NullClaw's registered sqlite_query opens an existing workspace database read-only,
applies statement restrictions and bounds returned rows/bytes. It is useful standing
data access, not a writable shared experiment database. Nanobot's remembered files,
Agent Zero's FAISS store and OpenClaw's bounded run-scoped JSON results also cannot
be conflated with a shared SQL service.
[NullClaw registration](../../../quarantine/nullclaw/src/tools/root.zig#L495),
[SQL implementation](../../../quarantine/nullclaw/src/tools/sqlite_query.zig#L55),
[Nanobot memory](../../../quarantine/nanobot/nanobot/agent/memory.py#L59),
[Agent Zero memory](../../../quarantine/agent-zero/plugins/_memory/helpers/memory.py#L470),
[OpenClaw results lifetime](../../../quarantine/openclaw/src/agents/code-mode-results.ts#L12).

The service boundary differs from Arconaut's current brief. Several references own
kernels, sandboxes and processes as local agent resources. We can learn their client
handles and failure semantics without making Arconaut governor of shared computation.
Physical locality does not change that boundary.

## Interrupted is not settled

Hermes interrupts tool execution, gives a grace period and can abandon a daemon worker;
parallel execution has a related timeout path. Agent Zero marks deferred work cancelled
before stop/cleanup, and a terminal close can suppress the final wait failure while
forgetting the process handle. IronClaw deliberately lets an owned sandbox task survive
the caller dropping its await. These behaviors defeat an inference that a finished turn
has no remaining effects.
[Hermes abandonment](../../../quarantine/hermes-agent/agent/tool_executor.py#L942),
[Agent Zero cancellation](../../../quarantine/agent-zero/helpers/parallel_tools.py#L650),
[terminal close](../../../quarantine/agent-zero/plugins/_code_execution/helpers/tty_session.py#L69),
[IronClaw owned task](../../../quarantine/ironclaw/crates/lanes/ironclaw_sandbox/src/sandbox_process.rs#L888).

ZeroClaw commits the durable Cancelled transition before signalling its worker token;
that test intentionally checks storage-before-token ordering, not OS death. A failed
storage transition leaves the worker live. Its shell timeout also requests cancellation
without proving every external effect settled. OpenClaw's process kill answer is more
precise: “Termination requested.” Polling/finalization remains a separate operation.
[ZeroClaw transition](../../../quarantine/zeroclaw/crates/zeroclaw-runtime/src/tools/delegate.rs#L2213),
[ordering test](../../../quarantine/zeroclaw/crates/zeroclaw-runtime/src/tools/delegate.rs#L5996),
[OpenClaw kill result](../../../quarantine/openclaw/src/agents/bash-tools.process.ts#L578).

OpenClaw has a valuable narrower ownership rule: code-mode close revokes dispatch
immediately but retains the runtime-refresh lease until late executions and continuation
disposal settle. Failed cleanup remains owned for retry. Its controlled-promise test
actually checks that ordering. This does not establish universal foreign-effect
settlement. Channel reload can proceed while work remains active on timeout or lifecycle
lease demand, so it is not Arconaut's strict refit rule.
[Cell ownership](../../../quarantine/openclaw/src/agents/code-mode-state.ts#L142),
[cleanup oracle](../../../quarantine/openclaw/src/agents/code-mode-state.test.ts#L60),
[reload escape](../../../quarantine/openclaw/src/gateway/server-reload-active-work.ts#L85).

Nanobot distinguishes completing an interrupted turn checkpoint from preserving an
unfinished checkpoint during shutdown. Its cleanup failure reinserts the execution
session handle rather than losing custody. These are useful recovery choices; bounded
process-group cleanup still cannot certify all foreign effects.
[Stop/shutdown](../../../quarantine/nanobot/nanobot/agent/loop.py#L1579),
[retained owner](../../../quarantine/nanobot/nanobot/agent/tools/exec_session.py#L407),
[checkpoint test](../../../quarantine/nanobot/tests/agent/test_stop_preserves_context.py#L46).

## Model agency and definition activation

Agent Zero provides broad extension wrappers over actual core functions, including
mutable arguments/results/exceptions and short-circuit behavior. Tools import fresh
modules per invocation, while extension class caches and watcher activation have a
different boundary. IronClaw instead seals planner strategy slots and allows additive
prompt mutations; self-authored hook types are restriction-only and their production
activation was not established. The default composition explicitly denies model
subagent spawning despite the presence of implementation and an ignored test.
[Agent Zero wrapper](../../../quarantine/agent-zero/helpers/extension.py#L167),
[tool import](../../../quarantine/agent-zero/agent.py#L1581),
[IronClaw planner](../../../quarantine/ironclaw/crates/loop/ironclaw_agent_loop/src/default_planner.rs#L1),
[default denial](../../../quarantine/ironclaw/crates/loop/ironclaw_turn_runner/src/runtime.rs#L294).

Hermes' actual context-engine hook selects a copied conversation context and fails open
on invalid results. OpenClaw invokes configured context assembly in the provider-request
path with budget, available tools and transcript read fences. These make model ergonomics
and context construction explicit extension points, but their existence alone does not
show ordinary model authority to replace every governing program.
[Hermes selection](../../../quarantine/hermes-agent/agent/conversation_loop.py#L1280),
[OpenClaw assembly](../../../quarantine/openclaw/src/agents/embedded-agent-runner/run/attempt-history-prepare.ts#L172).

ZeroClaw adopts a coupled model-routing/context-limit config generation at supported
turn boundaries. Nanobot captures immutable admitted runtime configuration, so future
settings do not rewrite an already admitted request. IronClaw builds a provider candidate
before swapping wrappers separately. Each wrapper has an atomic request snapshot;
the primary/cheap pair is not one atomic generation. An initially absent cheap-model
wrapper needs restart.
NullClaw explicitly reports supported hot paths versus skipped or restart-required
changes, and locks skill prompt invalidation against session work. These details are
more useful than a single “reload supported” flag.
[ZeroClaw generation](../../../quarantine/zeroclaw/crates/zeroclaw-runtime/src/agent/agent.rs#L1664),
[Nanobot runtime](../../../quarantine/nanobot/nanobot/agent/loop.py#L520),
[IronClaw swap](../../../quarantine/ironclaw/crates/domains/ironclaw_llm/src/runtime.rs#L315),
[NullClaw application](../../../quarantine/nullclaw/src/agent/commands.zig#L4706).

## Peer work and original history

Hermes child steering injects guidance at a subsequent child iteration; stop honestly
returns interrupt_requested. OpenClaw separates notify, active steering and new-turn
admission, and gives accepted input receiver-owned custody through settlement. ZeroClaw's
peer operation registers a task then starts a fresh one-shot recipient turn. NullClaw's
channel message queues output and its delegate can be one provider completion. These
are not interchangeable versions of an IRC-style live colleague room.
[Hermes steering](../../../quarantine/hermes-agent/tools/delegate_tool_registry.py#L124),
[OpenClaw receiver](../../../quarantine/openclaw/src/agents/tools/sessions-send-tool.steering.ts#L1),
[ZeroClaw recipient](../../../quarantine/zeroclaw/crates/zeroclaw-runtime/src/tools/send_message_to_peer.rs#L318),
[NullClaw delegate](../../../quarantine/nullclaw/src/tools/delegate.zig#L108).

Hermes soft-archives SQL message ranges instead of deleting originals, and fences the
commit so concurrent arrivals are not silently archived. Its test checks exact content,
ordering and watermark behavior. Nanobot also retains conversation source when summary
projection changes. NullClaw's normal durable turn save instead contains user/final
assistant messages, and compaction frees old working history. ZeroClaw overwrites its
current-history file. IronClaw stores separate range summary artifacts, but complete
original retention through every backend was not traced.
[Hermes range commit](../../../quarantine/hermes-agent/hermes_state_messages.py#L1074),
[watermark oracle](../../../quarantine/hermes-agent/tests/hermes_state/test_compression_watermark_commit.py#L43),
[Nanobot summary](../../../quarantine/nanobot/nanobot/session/manager.py#L219),
[NullClaw persistence](../../../quarantine/nullclaw/src/agent/turn_persistence.zig#L18),
[ZeroClaw save](../../../quarantine/zeroclaw/crates/zeroclaw-runtime/src/agent/history.rs#L599),
[IronClaw summary](../../../quarantine/ironclaw/crates/loop/ironclaw_loop_host/src/compaction_task.rs#L578).

None of these inspected audit paths establishes Arconaut's full capture of original
provider/program/transformation IO, failed attempts and abandoned work. Redacted metadata,
bounded output, query snippets, current chat and summaries each lose different material.
Agent Zero notify_user is a display operation, not a state-captured complaint/bead/database
transaction. Hermes' curator is a model-driven skills-only fork with restricted tools,
not a general performance-gated harness optimizer.
[IronClaw safe metadata](../../../quarantine/ironclaw/crates/events/ironclaw_event_log/src/runtime_event.rs#L72),
[OpenClaw diagnostic copy](../../../quarantine/openclaw/src/agents/anthropic-payload-log.ts#L97),
[Agent Zero notice](../../../quarantine/agent-zero/tools/notify_user.py#L7),
[Hermes curator](../../../quarantine/hermes-agent/agent/curator.py#L1085).

## Specific source concerns and useful oracles

NullClaw reuses successful IDless XML tool calls by signature without a purity or external
state invalidation condition. A read-edit-read trace or repeated effect can therefore
produce stale output/suppressed effects. Native calls with explicit IDs have a different
dedup meaning; the claim is scoped to the IDless path. This is inferred from source,
not a reproduced runtime result.
[Signature cache](../../../quarantine/nullclaw/src/agent/root.zig#L2981).

The proposed Arconaut tests should target the actual claims: refit after an uncooperative
worker with a live observable process; peer admission followed by sender termination
and receiver reincarnation; context compaction racing a new message followed by original
retrieval/repair; a config change during a workflow that must retain its admitted
generation; and a failed cleanup that must retain custody. Inspecting an expected status
string or using the same model to proclaim success would not answer these questions.
