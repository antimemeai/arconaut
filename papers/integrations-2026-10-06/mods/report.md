# Claude Code mods: extension machinery Arco can learn from

2026-10-06. Bounded study of current official documentation, Anthropic's public built-in mods and sample mods. Read-only research: no plugin installed, acquired code executed, account authenticated, production dependency proposed or product edited.

## Recommendation for discussion

Arco should offer optional Lua extensions that can register tools, immediate commands, event observers/middleware, and native UI views. A small C++ extension interface should expose the same journaled operations already available to ordinary workflows. Keep display-only HUD data outside model context until explicitly selected, subscribed to, or submitted by operator/model. Give extension code explicit generation identity and state lifetimes; activate changes at the existing end-of-turn/workflow boundary. External integrations can be ordinary program/service clients, installed independently, without bundling every vendor into the core.

Claude's new mods are a strong concrete reference for the *shape* of this interface. They do not require Arco to adopt JavaScript, JSX, React, Claude's approval system, or its static-analysis restrictions. Its public repository contains extension source and declarations, not the implementation of the closed engine that loads and dispatches them. Engine correctness below is documented behavior, not source-audited behavior.

## Current product status and surfaces

The official overview says terminal mods are enabled by default from v2.1.287; Desktop Code includes them from v2.1.286. `CLAUDE_CODE_ENABLE_FUNCTION_HOOKS` from early access is ignored in current versions. The current reference describes v2.1.289. The acquired public declaration file says **2.1.277, early access**; the replay sample says 2.1.280. Treat exact fields from those source pins as version-scoped, and current official docs as later evidence. Runtime-generated declarations are documented as the authority for a particular installed build. [Overview](https://code.claude.com/docs/en/plugins/mods/overview), [current reference](https://code.claude.com/docs/en/plugins/mods/reference).

| Surface | Hooks | Custom visual views |
| --- | --- | --- |
| Terminal CLI / integrated terminal | Run | Draw |
| Desktop Code local session | Run | Draw; some elements terminal-only |
| Desktop WSL session | Plugins unavailable | No |
| VS Code chat panel | Run | No |
| Headless `claude -p` / Agent SDK | Run | No |
| Remote Control | Run on local machine | Visible in local terminal |
| Cloud session receiving plugin | Run | No |

The original [Oct 1 claude.dev tutorial](https://claude.dev/blog/getting-started-with-claude-code-mods/) is verified; its exact URL differs from a guessed claude.com blog URL, which did not resolve. It presents token-weather, blast-radius and replay-theater as examples, not evidence of a general collaboration or feed platform.

## Event middleware versus classic settings hooks

A mod's handlers are in-process functions. Classic settings hooks are configured shell/HTTP/prompt/agent callbacks; `classic.Stop` and other classic events are also addressable by mods. Middleware receives host API, deeply immutable event input, and `next`. It can observe unchanged input/output, send a changed copy onward, or supply the result without running later handlers/core behavior. Ordering matters because the first handler sees input first and downstream result last. API calls themselves are interceptable named events. [Events guide](https://code.claude.com/docs/en/plugins/mods/events).

The failure policy depends on whether downstream execution happened: failure before `next` skips the extension; failure after a resolved `next` retains that result without automatically running the action again. A handler can attach an explicit error continuation. Tools may deliberately call `next` again; that is an execution decision, not observation. Streaming request hooks preserve response chunks through a generator. These mechanics transfer well to C++/Lua, but Arco's timeout/unknown-effect semantics must remain authoritative when a handler proposes a retry.

The following groups have different powers, and should not be collapsed into one generic “hook”:

| Boundary | Claude mechanism | Arco consequence |
| --- | --- | --- |
| Engine operation | Tool call/check/description, prompt submission/composition, turn start/request/completion | Observe or alter ordinary programs using existing audit and workflow semantics |
| Definitions | Register commands/tools/agents; tool descriptions can defer discovery | Dynamic registry, qualified names, JSON input contracts |
| Context | Prompt sections/attachments; session rows; compaction event | Preserve original and transformed inputs; no accidental UI-to-context coupling |
| UI | Named render sites and control events | Structured native view interface, independent of inference turn |
| Environment | Host filesystem/process/network/timer API | Programs and optional clients, with actual lifetime/cleanup accounting |
| Peers | Send/receive session events | Explicit message delivery and provenance, separate from rendering |

The acquired `TurnStepInput` declaration permits rewriting model/effort but pins message count, turn/index and agent identity; the transcript is not in the event. `session.messages()` is a separate transcript projection. Thus “can hook a model request” does not mean arbitrary managed context revision or full original-provider-request access. Arco's CLM/current audit goes further and should stay available.

## Engine API and model visibility

Commands can execute immediately while the model works. Tools register descriptions and JSON input schemas, with plugin-qualified model names. Model completion uses the session's credentials but no history; fork uses the current conversation snapshot. Timers run between turns. Status/toast/log can show information without inference; a log is explicitly operator-visible only. Prompt submission waits for idle and starts a turn; ordinary submissions identify the extension as sender. Session send resolves on queued delivery, not proof the recipient acted. [API guide](https://code.claude.com/docs/en/plugins/mods/api).

**Arco interpretation:** a thematic HUD feed can update and filter locally without consuming model turns. “Show it to me,” “make it agent context,” and “wake the agent because this item arrived” are distinct actions. A button can configure/select a feed without starting inference. Station scheduling should admit a selected trigger as a journaled prompt when its policy says so. Message sender attribution and delivery state should survive resumed sessions.

## Rendering and state

The UI is a structured element tree at named sites, with native host control of keyboard focus, scroll and layout. Pane/AbovePrompt can carry text, buttons, inputs and selections; terminal Raster supports cell-based charts/animation, while Desktop has SVG. `Client` is a separate render/input region without the engine API, posting messages to extension handlers. Invalid drawings fall back to the engine's view. Render reads reactive state; writes belong to other events/callbacks. Module variables reset on reload; reactive state survives reload but resets on clear/resume/branch; persistent plugin store survives sessions. `session.start` fires on reload but not those resets. [Interface guide](https://code.claude.com/docs/en/plugins/mods/interface).

This separation is valuable for native Arco: presentation code can rebuild a tree from state without rerunning external effects. Use the actual pane width rather than whole terminal width. Dynamic layouts need narrow-terminal alternatives, and display-only animation can repaint cells without touching context or workflow definitions.

The persistent store is explicitly shared by local sessions, and `get` followed by `set` is **not atomic**. More frequent reads reduce, but do not eliminate, lost updates. This is inadequate as a true multiplayer log or authoritative shared work queue. That conclusion follows directly from the documented race, not from a hypothetical security concern.

## Reload and resource lifetimes

Development directories loaded with `--plugin-dir` are watched. Reload re-registers handlers; files the model edits during its own turn load when that turn ends. Installed/updated plugins from outside a session need explicit reload or a later session. Headless long-lived development watching has a separate environment switch. Module timers stop at reload; event cancellation supplies a signal to long work. [Create guide](https://code.claude.com/docs/en/plugins/mods/create).

The transferable issue is generation custody: retired extension timers/callbacks must not mutate the current generation, and unfinished process/provider activity must be settled or preserved according to the same workflow rules as built-in operations. A new callback's state must not overwrite persisted values merely because resume reset its reactive defaults. Arco's RRC/native refit remains separate from cheap Lua reload. Neither source nor docs here establish arbitrary C++ hot reload.

## Actual source and test findings

Both archives are intact, full pins and reproducible restoration in [manifest.json](manifest.json):

- [anthropics/claude-code at 8e60c4c](https://github.com/anthropics/claude-code/tree/8e60c4cac989c0e0cc6d2c49407a5c67f5a4a8e6). Studied `mods/types/claude-code.d.ts`, `mods/diff/hooks/register.ts`, `mods/diff/tests/register.test.ts`, `mods/agents-md/hooks/register.ts` and its register tests.
- [anthropics/claude-code-playground at 569c5283](https://github.com/anthropics/claude-code-playground/tree/569c5283d9a0a7ee7938df85bb32e4f48cbb8c86). Studied token-weather and replay-theater hook modules and README/version annotations; sample source does not carry the same direct test suite as built-ins.

**Token weather:** `takeReading` reads actual context usage, retaining 12 readings and ignoring subagent completion. `band` drops sparkline/trend details below 60 columns; forecast uses strict upper thresholds, so exactly 25/50/75/90 enters the next band. State is plain module data and lost at reload. Failures retain a stale last reading without a displayed stale marker. This is a good small HUD example, not a context-budget controller. Arco should visibly distinguish current, stale and unavailable feed/usage data.

**Replay theater:** `stepsFor` constructs an Edit/Write/MultiEdit diff and the `tool.call` hook pushes it into `state.pending` **before** returning `next(e)`. It does not inspect final refusal/error/result. Its replay can therefore show intended edits that did not happen. It also reads whole files for Write and renders a bounded diff. `openReplay` falls back to AbovePrompt when pane placement fails. Borrow interaction/layout ideas, but derive Arco replay from journaled actual outcomes and immutable originals; do not treat this sample as an audit implementation.

**Built-in diff:** `probeBackend` uses a pin epoch to discard stale async repository results; `fetchBodies` checks a generation/base stamp; `refresh` coalesces overlap; HEAD polling is visibility-aware; command clear/resume resets backend/session-specific state. The model-facing operation is not repeated just to render. Direct tests assert zero Git work at startup, concurrent `/diff` uses one probe, failed/refused edits open nothing, only main-loop edits auto-open, and `/clear` refreshes an existing pane. These are useful independent fault cases for optional HUD extensions: zero unused work, nonduplicated external requests, stale-result rejection and lifecycle-correct display.

**AGENTS.md built-in:** prompt-context middleware inserts actual instruction file contents according to configuration. Tests assert exact loaded paths and original text; an existing CLAUDE.md preserves engine-provided context, and failed directory walk leaves it unchanged. This confirms extensions can alter real model inputs, not merely display instructions. Arco should journal extension definition, source original and resulting provider input when similar augmentation happens.

The official test kit fires events and stubs host answers without network/account. Its drawing helpers press controls and compare rendered content across surfaces; documented reset tests start from default reactive state and deliver the actual reset event. Stub tests qualify extension transformations, not the closed host's process cleanup or provider delivery. [Test guide](https://code.claude.com/docs/en/plugins/mods/test).

## Lean Arco options and tradeoffs

| Option | Concrete benefit | Cost / fit | Recommendation |
| --- | --- | --- | --- |
| Lua event/command/tool modules plus native view descriptors | Low-friction model-authored HUDs and behavior changes; existing C++/Lua stack | Define dispatch/reload/state/controls and native layout contracts | First discussion candidate |
| External program/service feed client, loaded on demand | Integrate existing OS tools and new vendors without bundling runtime | Protocol framing, reconnect/backpressure, explicit selected-data admission | Pair with Lua modules |
| Native shared-library extensions | High-throughput special capability close to C++ core | ABI, build/toolchain and unload/resource lifetime complexity | Only where Lua/client boundary demonstrably inadequate |
| Web/client renderer in core | Rich browser ecosystem | Runtime/dependency footprint, two UI models, JS production conflict | No present reason to adopt |
| Static analyzer that requires literal hooks/API calls | Produces inspectable capability inventory before load | Restricts dynamic metaprogramming; large validator surface | Do not copy as a prerequisite; journal actual execution instead |

No new library is selected. A small initial native view vocabulary can express rows/text/status/actions and leave general layout/graphics until a real extension needs them. Feed adapters remain optional directories/programs, not hardcoded bundled social/media providers. Exposing async native completions to Lua needs clear cancellation and generation accounting; it does not require putting all program scheduling under a JS-style event loop.

## Direct oracles worth carrying into design

1. A disabled/unopened feed performs zero fetches and no model requests; opening it performs one coalesced request. Displaying/refreshing it never inserts model context by itself.
2. Two extensions transform the same tool event in a declared order; post-effect handler failure preserves the actual result and never duplicates the effect. Deliberate retry of unknown effect remains a model/program decision.
3. Reload during a workflow leaves that workflow's current definition intact; the next admitted workflow uses the new generation. Retired timers/results cannot overwrite current display state.
4. Resume/RRC restores intended persistent preferences and feed position; reset defaults never overwrite them. A stale callback from the previous session is rejected by actual identity/version.
5. A failed/refused file edit appears as failed/refused in replay; only actual changed content is shown as a successful edit. Compare replay directly with audit and filesystem effects.
6. A native PTY fixture at narrow/wide widths checks pane fallback, focus and literal typed input; background updates do not steal composer keys or trigger inference.
7. Shared peer/feed state receives concurrent updates without lost messages; test actual transactional or append semantics, not a JSON get/set convenience wrapper. Queued delivery and recipient handling remain distinct.

These are proposed independent fault cases, not launched tests or extra approval gates.

## Acquisition limits

`manifest.json` records two source pins, hashes, intact ignored archives, extracted stripped copies and restoration. Raw docs downloads via Python returned HTTP 403; browser retrieval succeeded, and [docs-manifest.json](docs-manifest.json) preserves that distinction. [official-docs-browser-capture.json](official-docs-browser-capture.json) and [lifecycle-browser-capture.txt](lifecycle-browser-capture.txt) are browser-returned excerpts, not claims to have acquired complete original documentation files. All citations link current primary pages. Static public declarations are older than current docs; no installed runtime/type dump was queried because this is research, not plugin installation or account use. No reference tests were executed.
