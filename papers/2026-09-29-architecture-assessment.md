# Inherited Arconaut: architecture and executable behavior

Date: 2026-09-29. Scope: the inherited implementation at `3bf2056`, as exposed by
the new assessment checkout. This is an independent source review, not approval
of the inherited architecture and not a repair plan. Current AGENTS, Blackbird,
README, JOURNAL, and document-status instructions were read. No product code was
edited; no application, live provider, credential store, SSH target, or operator
configuration was exercised. Execution and test-oracle assessment belong to the
separate oracle review.

Source location updated 2026-09-30: source paths are within `quarantine/arconaut/`.
Crate-qualified citations such as `arconaut-agent/src/...` abbreviate
`crates/arconaut-agent/src/...`; shorter paths inherit the containing crate or
module discussed. The implementation has been moved there intact; these
observations and candidate reuse suggestions confer no current implementation
standing. Reconstruction begins with the time and foundation design.

The useful base is a collection of small Rust interfaces and implementations,
plus a readable prototype turn loop. It is not an established working coding
harness. Crucial interfaces are lost at provider conversion, TUI task ownership
prevents terminal-tool completion, and named sessions/modes do not control the
runtime. The newer pilot/reactor architecture is mostly a separately compiled
proposal, with substantial editor/shell experiments but no implemented turn
orchestrator.

Here, **observed** means directly present in source; **inference** means the
consequence of that source/control flow, not an execution result; **design
choice** identifies a restart decision that the code cannot settle for us.

## What the executable actually connects

The binary is `crates/arconaut-cli/src/main.rs`; `repl.rs` is included only under
`cfg(test)` (`main.rs:1`). The CLI imports `arconaut_agent::Soul` (`main.rs:6`),
not the newer pilot Soul.

```text
CLI run_main
  ├─ --no-tui → run_single_turn → old agent::Soul::run_turn
  └─ TUI → App ⇄ channels ⇄ run_soul → old agent::Soul::run_turn
                              ├─ PersistentShell (piped interactive bash)
                              ├─ TerminalBridge → channel back to run_soul
                              └─ Bus tool (separate from gRPC InboxServer)

old Soul
  ├─ Context + per-turn dedup cache
  ├─ ChatProvider → Anthropic or OpenAI-compatible wrappers or Gemini stub
  ├─ ToolRegistry → file, shell, grep, skill, utility tools
  ├─ intervention + optional assistant + hooks
  └─ optional injection/compaction (not configured by either CLI path)

pilot DefaultSoul → one provider call
reactor ArconautLogic → unimplemented turn method
optional agent::p5_7_wiring → constructs both and discards result
```

Both CLI paths register read/write/edit/bash/grep/skill and utility tools, set
50 maximum steps and 200,000 context tokens, and enable intervention
(`main.rs:431-453`, `595-619`). TUI also registers terminal and bus tools. Neither
path configures an injector or compactor. The old Soul appends user input, calls
the provider, appends the assistant response, executes tool calls sequentially,
appends results, and repeats (`arconaut-agent/src/soul.rs:127-224`). That control
flow is real, but its provider and tool dependencies do not preserve its implied
contracts.

The only current use of pilot in the old loop is the re-export of its dedup
implementation (`arconaut-agent/src/dedup.rs:1`). Reactor integration is behind
the agent crate's non-default `p5_7` feature (`arconaut-agent/Cargo.toml`;
`arconaut-agent/src/lib.rs:16-17`). The CLI has no feature wiring or calls into
`run_minimal_turn`.

## Consequential findings

### Provider adapters do not carry a coding-agent conversation end to end

**Observed.** Anthropic constructs its URL using
`format!("{}messages", base_url.trim_end_matches('/'))`
(`arconaut-machine/src/providers/anthropic.rs:95`). Its default is
`https://api.anthropic.com/v1` (`:10`), producing `/v1messages`. The compatible
client applies the same concatenation to `chat/completions`
(`providers/openai_compat.rs:123`), producing `/v1chat/completions` for its
default OpenAI base (`providers/openai.rs:5`). Adding a trailing slash to the
configured base cannot fix this because the code removes it.

**Observed.** Beyond URL construction, the Anthropic outbound message type holds
only a string body. Conversion drops `ContentPart::ToolCall` and flattens tool
results into ordinary user text, removing the call ID and error flag
(`providers/anthropic.rs:292-329`). It can parse incoming tool calls
(`:149-169`), so the second request after a tool execution loses the information
that connects a result to its request.

The compatible adapter similarly drops outbound tool-call identities and treats
tool results as plain user text (`providers/openai_compat.rs:271-304`). Its
response parser consumes text only and explicitly leaves tool calls unimplemented
(`:183-191`, `:333-337`). OpenAI, Moonshot, and OpenRouter delegate to this client
(`providers/openai.rs:41-42`, `moonshot.rs:131-136`, `openrouter.rs:56-57`). Gemini's
`chat` always returns “not yet implemented” (`providers/gemini.rs:83-89`).

**Inference.** The source does not support a claim of a working multi-provider
coding loop. Even if transport paths were corrected, a tool-only compatible
response becomes an empty assistant message, which the old Soul considers a
completed turn (`arconaut-agent/src/soul.rs:178-187`). Anthropic's continuation
loses the required semantic relationship between calls and results. This is
broader than a choice of current model name or endpoint version.

**Design choice / discriminating experiment.** Choose an explicit conversation
contract including identity, errors, stop reasons, and unsupported content. A
credential-free local server should capture the actual URL and full second
request for a tool-call/result round trip and return a provider-shaped response.
The oracle must inspect the conversation, not merely successful construction of
provider objects. Current API conformance still needs separate primary-source
research; this report makes no claim to have validated live APIs.

### The TUI terminal tool cannot finish under its current owner

**Observed.** `run_soul` awaits `soul.run_turn` inside the user-input arm of one
`tokio::select!` (`arconaut-cli/src/main.rs:635-663`). A `terminal_send` call sends
to `bridge_rx` and awaits its oneshot reply (`terminal_bridge.rs:51-63`). The only
consumer of that channel is another arm of the same select loop (`main.rs:640-647`).

**Inference.** When the model requests `terminal_send`, the turn waits for a reply
that its own suspended owner must produce. No timeout exists on that reply. This
is a deterministic wait cycle in the wired execution path, even though the
terminal bridge unit test supplies a separate receiver task
(`terminal_bridge.rs:79-84`).

Interrupt is likewise not processed during an awaited turn; when eventually
processed it only sends a status string (`main.rs:672-674`). The UI can display
“Interrupted” immediately (`arconaut-tui/src/app.rs:145-148`) while work continues.
Quitting the UI subsequently waits for the Soul task (`main.rs:530-531`).

**Design choice / discriminating experiment.** Define ownership of model work,
terminal I/O, user input, cancellation, and subprocess completion before choosing
an async arrangement. A bounded, fake-provider test must exercise the actual
CLI task arrangement and prove both terminal completion and interruption during
a turn. Testing the bridge in isolation is a different claim.

Even with scheduling repaired, the CLI treats 200 ms of elapsed time as command
completion and returns the shared shell buffer (`main.rs:644-646`). The shell
uses pipes, not a PTY, and reads lines (`persistent_shell.rs:23-29`, `50-85`).
Consequently interactive terminal fidelity, command boundaries, and which output
belongs to which command remain design questions; the visible terminal pane does
not establish those properties.

### Tool deduplication changes program meaning and defeats its loop counter

**Observed.** Every tool result, including errors, is cached by name and serialized
arguments. Cache hits return before execution and before hooks
(`arconaut-agent/src/soul.rs:282-296`). The cache persists for the entire turn
(`:128`). Tool contracts have no purity, freshness, or idempotence declaration
(`arconaut-core/src/tool.rs:5-10`).

**Inference.** `read(file)` → `write(file)` → `read(file)` returns the first read;
rerunning an identical test command after an edit returns its earlier result.
Repeated side effects such as bus sends can be silently suppressed. These are
normal coding workflows, not unusual malformed inputs.

The dedup consecutive counter advances only on insertion
(`arconaut-pilot/src/dedup.rs:54-73`), while repeated calls in Soul hit the cache
and skip insertion. After clearing at turn start, a repeated identical call
therefore does not accumulate toward intervention's 5/10 duplicate thresholds
(`arconaut-agent/src/intervention.rs:43-56`). Tests that manually insert the same
entry repeatedly model a path the loop does not take (`intervention.rs:159-170`).

**Design choice / discriminating experiment.** Separate observation of repeated
requests from any reuse of tool results. Test against a stateful temporary
resource and an effect count. Decide whether any caching is justified and what
invalidates it; it is not a generally valid harness optimization.

### Context limits and compaction are labels rather than preservation guarantees

**Observed.** The default Soul has no compactor or injector
(`arconaut-agent/src/soul.rs:75-86`); neither CLI builder supplies one. Provider
usage is received but not integrated into Context in the turn loop (`:143-149`).
Context's token estimator examines only top-level text
(`arconaut-core/src/context.rs:104-109`); `as_text` excludes ToolCall and ToolResult
(`arconaut-core/src/message.rs:101-105`). A large read-tool result therefore adds
zero estimated tokens. Appending is unconditional (`context.rs:44-47`).

If enabled, CompactionEngine replaces the entire old prefix with only
“N previous messages summarized (T tokens)” and keeps a suffix
(`arconaut-agent/src/compaction.rs:53-72`). It retains no facts from that prefix
and cuts by message count, without preserving call/result groups. It runs only
at turn entry (`soul.rs:130-132`).

**Inference.** The 200,000 setting is neither a request-size limit nor a reliable
usage measurement. Existing “compaction” is deletion accompanied by a count, not
semantic summarization. Enabling it in the CLI would not establish a usable
long-running memory system and could orphan tool results.

**Design choice / discriminating experiment.** Establish what must survive
context reduction and what may be recovered from disk, and define accounting for
the entire serialized request. A direct oracle should put a decision and a
call/result group in the reduction boundary and verify their specified survival
or explicit omission. There is no value in a second receipt around a token-count
decrease.

### Session, authorization mode, and agent messaging are disconnected surfaces

**Observed.** CLI constructs `_session` and does not use it (`main.rs:114`).
Agent name/mode are explicitly ignored in each runtime entry (`:396`, `:484`).
Both receive the same write/edit/bash registry. The source describes AgentMode
as what an agent is authorized to do (`arconaut-agent/src/agent.rs:5-13`), but no
runtime mode enforcement follows. Both audit builders use the literal session
name `default` (`main.rs:455`, `:621`). AgentRegistry has independent JSON
load/save methods (`agent.rs:82-93`), but the CLI does not invoke them.

The TUI creates a fresh Bus and a separate InboxServer with separate state
(`main.rs:486-492`; `inbox_server.rs:13-29`). Nothing in the CLI joins bus topics,
registers a mailbox/presence, or consumes a bus/inbox stream. Broadcast/whisper
silently do nothing without destinations (`bus.rs:85-101`), while BusTool reports
success (`bus_tool.rs:69-73`, `:86-90`). The assistant's deliver method drops the
broadcast future without polling it (`assistant.rs:147-151`). Assistant advice
still enters Soul context directly (`soul.rs:237-242`), so the secondary-provider
path is partial, not wholly absent.

**Inference.** `--session` does not resume state, `--mode review` does not restrict
effects, and the presence of bus/gRPC code does not constitute coordinated
multi-agent operation. Process-local pub/sub and a separately launched server
are two unconnected transports. The executable cannot be trusted to honor the
operator-facing meanings of those names.

**Design choice / discriminating experiment.** Decide which of persistent agent
identity, resumable conversation, tool authority, and multi-agent communication
belong in the restart. Each needs an observable contract. A two-turn restart
test, a forbidden-effect test, and actual addressed-message delivery answer
different claims; none is implied by serializable structs or successful sends.

### Configuration errors and provider failures can look like successful work

**Observed.** `build_provider` erases factory errors with `.ok()?`
(`main.rs:713`). Single-turn mode substitutes an empty MockProvider on failure
without a warning (`:414-415`); TUI calls this “echo mode” (`:585-590`). That mock
actually responds `done` (`arconaut-agent/src/soul.rs:329-337`). Single-turn
provider errors are printed, then the function returns `Ok(())`
(`main.rs:468-474`). VariableStore ignores read/parse failures
(`arconaut-core/src/vars.rs:31-45`).

**Inference.** A missing/invalid provider setup can yield `done` and successful
exit despite no real provider call; an actual provider error also has successful
process status. This invalidates headless automation's obvious success oracle.

**Design choice / discriminating experiment.** Define the difference between
demo, failed initialization, failed turn, and completed work. Invoke the binary
under an isolated configuration root with an invalid provider and with a local
error response; inspect exit status and user-visible output together.

### OAuth machinery exists, but its credential resolution is bypassed

**Observed.** Moonshot builds its compatible client with the static key
(`providers/moonshot.rs:25-33`). Its `resolve_api_key` method loads/refreshes a
token and sets that client's key (`:66-94`), but is unused and explicitly marked
dead code. `chat` records activity then calls the original client directly
(`:131-136`). Background refresh receives storage, flow, and activity, not the
client (`:101-116`). ProviderFactory still requires an `api_key` configuration
entry before considering `oauth` (`providers/mod.rs:148-160`).

**Inference.** Successful device login or background refresh does not establish
that chat uses the resulting access token. The operator can have a valid token
on disk while requests continue with the original static key. This finding came
entirely from source; no credential material was read.

**Design choice / discriminating experiment.** Decide the supported authentication
paths, then test request authentication with synthetic storage and a local
server. Do not infer the outgoing credential from storage tests alone.

### Pilot/reactor is an unfinished alternative, not the foundation already adopted

**Observed.** Pilot DefaultSoul executes one provider call; `continue_turn`
unconditionally errors (`arconaut-pilot/src/soul.rs:69-80`), and SoulConfig is
stored as `_config` (`:20-24`). Reactor's `run_turn` unconditionally returns an
unimplemented error (`arconaut-reactor/src/logic.rs:68-76`). ContextAssembler
ignores user input and requested tempo, supplies no tools, and does not use its
stored system prompt (`context/assembler.rs:20-26`, `:39-40`); Beat injection is a
no-op (`:35-37`). Heuristic evaluation returns an empty vector
(`heuristic/engine.rs:49-52`). The optional minimal wiring discards the error
(`arconaut-agent/src/p5_7_wiring.rs:12-15`).

Reactor does contain substantial implementations: Neovim process/RPC and buffer
operations (`runtime/nvim.rs:42-92`, `:148-210`), brush execution and output capture
(`runtime/brush.rs:48-119`), and editor/shell/SSH tools. They are experiments worth
examining on their own merits, not evidence of a connected replacement runtime.
One consequential unresolved brush mechanism is that it waits for command
completion before draining stdout/stderr pipes (`brush.rs:75-108`). Output larger
than pipe capacity can block the child while the parent waits for completion;
this is an inference requiring a bounded subprocess experiment, not a run made
here. OSC633 parsing is a pass-through stub (`brush.rs:123-131`).

**Design choice.** Provider interaction separated from orchestration is a useful
hypothesis, but the restart should evaluate the boundary against cancellation,
streaming, context ownership, recovery, and effects before accepting these crate
names or filling their stubs. Adding code to complete a historical phase would
silently accept the premise under review.

## What is worth carrying forward

These are reuse candidates, not blanket certification. No implementation has
been moved or deleted by this review.

| Disposition | Candidate | Evidence and condition |
| --- | --- | --- |
| Retain as small primitives | Structured Message/ContentPart/ToolCall/ToolResultPart; Tool and ChatProvider interfaces | `arconaut-core/src/message.rs:4-52`, `tool.rs:5-23`, `arconaut-machine/src/provider.rs:6-69`. They expose useful seams for fake providers and effect oracles. Preserve identities end to end and revisit request/response contracts. |
| Retain as a reference loop | The old Soul's explicit sequential tool cycle | `arconaut-agent/src/soul.rs:127-224`. It explains the minimum execution story and is small enough to understand. Its caching, context, hooks, and stopping semantics require redesign. |
| Retain selectively | File-tool mechanics, ordered core registry, scoped variable precedence, lazy skill discovery | `arconaut-machine/src/tools.rs:54-75`, `:201-259`; `arconaut-core/src/tool_registry.rs:29-55`; `vars.rs:48-85`; `arconaut-machine/src/skills.rs:39-60`. Exact-match edit ambiguity errors and explicit precedence are concrete behavior. Authority, freshness, parsing errors, and skill identity need contracts. |
| Retain selectively | Append-only JSONL writer and typed events | `arconaut-audit/src/logger.rs:19-53`. Useful ordinary logging; currently best-effort, not a replay log. Soul provider errors bypass post-turn hooks (`soul.rs:143-147`); usage is not recorded by MetricsHook (`hooks.rs:91-98`); session IDs are not wired. |
| Rework after design | Provider codecs/config/auth; context lifecycle; TUI/shell ownership; interruption; result caching | Findings above directly obstruct ordinary coding. A passing isolated unit suite cannot make these seams correct. |
| Preserve as research prototypes | Neovim, brush, SSH tools; TUI widgets and protocol | Real implementation effort with narrow reusable ideas. It has not earned selection as the core UX or execution substrate. Reuse follows a decision, not the amount of existing code. |
| History unless freshly justified | Phase-completion claims; count-only “summary”; fixed phrase/step churn rules; unconnected session/dispatch/bus promises; unimplemented reactor orchestration | They embody hypotheses and unfinished intentions. The new baseline should describe them as such. `arconaut-corpus/src/lib.rs:1` and `arconaut-eval/src/lib.rs:1` are placeholders. |

The restart's central research question is how a personal harness should preserve
operator intent, conversation/effect identity, and recoverable work while allowing
the desired degree of autonomy. The old code supplies examples and counterexamples,
not the answer. Resolve the smallest useful operator workflow, persistence model,
execution authority, context strategy, and UI/runtime ownership first. Then choose
which primitives and prototypes serve that design and retire the rest with their
history intact.
