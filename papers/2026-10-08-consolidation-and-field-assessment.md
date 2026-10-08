# Blackbird assessment and campaign priorities 2026 10 08

Blackbird is a useful, unusually programmable native coding harness. The strongest
next investment is making its existing capabilities work together: a model and an
operator should be able to recruit colleagues, watch their work, steer it, and
continue without manually assembling several CLIs and JSON files. Startup work has
removed a major distraction. Collaboration and everyday continuity now offer more
value than another optimization campaign.

## Consolidated state

`55cd85d` publishes the consolidation on the repository's default branch, `master`.
Forty of 42 other local heads were already ancestors. The other two held duplicate
deliveries, preserved by existing archive tags. All 13 remote candidate heads were
already included. These heads are retired; five clean historical worktrees are
detached in place with their build/evidence files preserved. There was no new source
to squash, and no reason to rewrite published main-line history. Exact dispositions
are in [the branch inventory](../docs/BRANCH_INVENTORY.md).

The assessment uses the delivered source through `44defd7`, including tasks,
slash-selection styling, and `/new`. Consolidation changes branch refs and docs;
it does not change the installed executable. Existing operator sessions and
untracked artwork stay intact.

## Against the operator's expectations

The standard is the [foundation brief](../docs/FOUNDATION.md): an expert's
programmable working environment, low routine approval friction, model agency,
context repair, self-development, heterogeneous live collaboration, and eventual
independent network peers. These judgments concern that standard, not a feature
count or percentage of completion.

| Expectation | Present behavior | Assessment and merit of remaining work |
| --- | --- | --- |
| Immediate, comfortable daily use | Streaming native TUI, composer/history/editor, responsive local commands, task pane below sprite, clean slash selection, `/new` and retained sessions. | Substantial progress. The recent missing `/new` and ugly selection are evidence that everyday interaction still needs operator use. Session picking/naming and narrow-window correctness are worthwhile small units. |
| Model and operator use the same machinery | Shared task edits, callable Lua workflow registry, dynamic tools/modules/configuration, Jev decisions and native Beads adapter. | A real foundation. Some common actions still require raw Lua/JSON or a separate executable. Improve discovery and consistent invocation before adding more isolated tools. |
| Context can be changed and repaired | Retained originals, structured edits, managed compaction, original retrieval/restore, exact request recording and task rereading after compaction. | One of Blackbird's strongest design choices. A readable context/compaction view would make it easier to use and diagnose; another general context rewrite has weak immediate merit. |
| Sustained useful self-development | Actual source edits, builds, regression fixes and restart/continuation have been delivered and used in campaigns. | The first milestone is real. Manual native restart is still the boundary; uninterrupted compiled refit and surviving arbitrary workers remain later work. |
| Heterogeneous colleagues work together | OpenAI coding loop; separate selected-context OpenAI/Claude colleague library/CLI; synchronous Lua orchestration. | Largest gap relative to the brief. A standalone answer helper and inline callbacks are not live collaboration. Common invocation, visible runs, bounded concurrency and addressed steering have high merit. |
| Recover without pretending effects vanished | Provider retries, cooperative backstop, workflow repair, retained unknown outcomes and supported compact session reopening. | Useful but finite. Unsupported unresolved/station/pending-restart/multisegment states can require full recovery; linked histories are refused by this launcher. Improve the specific failure the operator encounters, not a universal recovery platform first. |
| Optional services and automation fit ordinary computing | Local-file native station, boundary controls, opt-in trusted Lua packages/feeds and external phux integration. | More is shipped than parts of README suggested. Wider connectors and station attachment have merit after common run/control semantics. Shared services stay independently owned. |
| Autodroit learns from work | Local rageshake capture/advisory Beads delivery, candidate leases and retained experiment records; a finite useful-work pilot. | Machinery exists; improved coding outcomes are not established by having it. Complaint triage and a real sustained-work comparison are valuable. External complaint database delivery remains missing. |
| Multiplayer independent Blackbirds | Literature/source research; no P2P/E2EE product implementation. | Central destination, expensive next step. First settle local participants, addressed messages, observation and ownership; then apply the networking research to two independent peers. |
| Easy independent installation | Native executable and documented build; OpenAI login/refresh still depends on Codex scaffolding. | Important before distributing Blackbird. Lower priority for this operator's installed workspace than coherent collaboration. |

A powerful internal API is not automatically good model ergonomics. Discoverable
schemas, concise current state, clear conflict repair, and useful operator controls
have to meet at the same operation. The [task feature](../docs/TASKS.md) is the
best current example: durable IDs/version checks, compact model reads, and immediate
pane updates over one owner. Use that pattern for participant work without adding
subsubtasks or making the task list a second execution authority.

## What the measurements support

The [eight-tool trial](2026-10-08-agent-startup-total.md) measured 100 fresh launches
per tool, across two serial batches, with a common PTY driver. Blackbird's configured
input-frame p50 was **102.231 ms**, p90 **115.428 ms**; marker response after typing
p50 **0.721 ms**. It was fastest in that measured population. These are terminal
readiness and input observations from a specific earlier executable, not coding
quality, painted Ghostty pixels, or a new measurement of the task-pane generation.

The separate [300-start wrap-up](2026-10-08-startup-300-wrapup.md) includes fresh,
heavy, and unusual retained sessions. Heavy first-frame p90 was **93.355 ms**;
fresh first-frame p90 **111.610 ms**, maximum **310.207 ms**. Audited local command
response after input had medians **33.336–34.798 ms**. These distinguish presentation
latency from durable command work. Diagnostics TTL remains 30 days; task state and
original history do not expire with it. The excluded stream-capture measurement
remains excluded.

My judgment: responsiveness is now a strength worth preserving, while successful
work, operator interventions, model-call reliability, and coherent continuation
should guide the next campaign. Faster startup alone cannot establish preference
over another coding agent.

## Against the current field

Primary sources checked 2026-10-08 describe available surfaces. They are not a
matched coding trial or independent verification of vendors' reliability claims.
The ecosystem moves: earlier acquired versions and today's docs are separate.

| Reference | Relevant current behavior | Consequence for Blackbird |
| --- | --- | --- |
| Codex | Parallel delegated agents with inspectable threads and collected results; progressive skill discovery. [Subagents](https://learn.chatgpt.com/docs/agent-configuration/subagents), [skills](https://learn.chatgpt.com/docs/build-skills). | Inspecting and steering delegated work is part of ordinary agent ergonomics. Blackbird's common colleague interface should be usable without leaving the conversation. |
| Claude Code | Experimental teams with separate contexts, direct messages and shared tasks; lifecycle hooks. Teams are disabled by default and add coordination/token overhead. [Teams](https://code.claude.com/docs/en/agent-teams), [hooks](https://code.claude.com/docs/en/hooks). | Learn from the shared work view and direct teammate interaction. More agents are useful only when work separates cleanly; they should not multiply review ceremony. |
| Pi | Extensions can add tools, commands, providers, session state and UI; session picker/tree/fork/clone, context management, broad provider authentication. [Extensions](https://github.com/earendil-works/pi/blob/main/packages/coding-agent/docs/extensions.md), [sessions](https://pi.dev/docs/latest/sessions), [providers](https://pi.dev/docs/latest/providers). | Closest challenge to our expert-programmability premise. Blackbird needs equally usable extension/session surfaces, while retaining its native ownership and recorded effects. Old descriptions of Pi as lacking built-in MCP are not a current baseline. |
| OpenCode | Primary/subagent roles with per-agent models and tool permissions; event plugins. [Agents](https://opencode.ai/docs/agents/), [plugins](https://docs.opencode.ai/docs/plugins/). | Model choice and optional integrations should be ordinary configuration rather than new core forks. |
| Kimi Code | Current CLI documents parallel subagents, skills/MCP/plugins, hooks and ACP editor integration. The former Python Kimi CLI is archived. [Current source](https://github.com/MoonshotAI/kimi-code), [legacy disposition](https://github.com/MoonshotAI/kimi-cli). | A convenient binary does not imply a C++ implementation or equivalent architecture. Study current interfaces, not legacy skill assumptions or startup advertising. |
| Hermes | Project documents persistent memory, skill creation, session search, subagents, gateway delivery and scheduled automation. [Primary source](https://github.com/NousResearch/hermes-agent). | Long-lived learning and useful external delivery matter. Memory/skill claims do not by themselves establish improved work; our complaints/experiment machinery needs actual useful follow-through. |
| Temporal as workflow reference | Workflows distinguish commands/awaitables, event history, replay and cancellation observations. [Execution semantics](https://docs.temporal.io/workflow-execution). | Async collaboration needs explicit state and effects. A saved transcript or Lua callback list is not durable execution; borrowing the concepts does not adopt its service or SDK. |

There is no honest overall league-table position yet. Blackbird leads this local
startup sample and has a substantial programmable/retained-state design. Several
references expose broader everyday collaboration and extension surfaces. Their
languages are not our adoption choices. Their ergonomics and failure semantics
are useful study material.

## Recommended direction

[The proposed next campaign](../docs/NEXT_CAMPAIGN.md) builds an integrated local
collaboration loop: practical session/task controls, one shared colleague surface,
bounded concurrent participants and visible intervention, then a useful sustained
self-development run. Multiplayer follows a usable local participant design.
Keep implementation units bounded and integrate each completed unit promptly.

The remaining backlog has different merit. Tiny-window bugs, actionable complaint
triage and coherent run controls deserve direct repairs. Formatting/analyzer debt
should get a finite mechanical pass. Broader authentication/distribution, connector
catalogs, live-worker custody and network peers are consequential separate units.
A new broad performance or certification campaign would distract from the most
valuable missing behavior.
