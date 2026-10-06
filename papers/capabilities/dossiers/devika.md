# devika

Fixed planner/researcher/coder and classified follow-up application pipeline; named agents are same-model stages, not concurrent colleagues.

Role: coding agent. Runtime: Python, JavaScript/Svelte UI.

Pinned source: [https://github.com/stitionai/devika](https://github.com/stitionai/devika); revision/version `80bb343cbe4a4e5f5a0ba08d2524920139baceb6`.

Flask/gevent-patched service with per-input Python threads; synchronous prompted stages and provider executor; Playwright browser

Owns local project files, SQLite chat/state and browsers/processes; consumes provider/search/deployment APIs. No fabric contract.

Inspection: Entry/thread routing, initial/follow-up stages, model transport, file parser/save, command/repair, SQLite and config, browser integration and available benchmark material.

Limits of this study: Source read only, reference not executed. No assertion-based reference tests found. No dependencies or reference executed. Browser/decision branches have specific unwired/faulty calls; owned stores are not shared standing-service capability.

## Actions

### user-message

Surface: operator socket.

Input: message/base_model/project/search_engine

Result: project messages/state and background thread

Lifecycle: non-cancellable thread, active work can overlap

Authority: operator selects project/model; no command approval

Evidence: [e1](#evidence-e1), [e3](#evidence-e3).

### answer | run | deploy | feature | bug | report

Surface: model classifier.

Input: conversation selects action; host builds code/context

Result: answer text, command results, deployed URL, files or PDF

Lifecycle: fixed one branch after classifier

Authority: model selects branch; host executes automatically

Evidence: [e4](#evidence-e4).

### Coder.execute / save_code_to_project

Surface: model response convention and host API.

Input: ~~~ blocks with File: and code

Result: file dictionaries then filesystem writes

Lifecycle: synchronous generation and writes

Authority: model controls filenames/code, host unguarded join

Evidence: [e6](#evidence-e6), [e7](#evidence-e7).

### Runner commands / rerunner action=command | patch

Surface: model response convention.

Input: commands JSON; failed stdout and generated replacement/patch

Result: terminal stdout and patched files

Lifecycle: blocking sequential processes; no handles/cancel/deadline

Authority: automatic OS execution without confirmation

Evidence: [e8](#evidence-e8), [e9](#evidence-e9).

### search_queries / open_page

Surface: model-generated queries and host API.

Input: queries plus chosen search engine

Result: first-link page text, screenshots, formatted summary

Lifecycle: sequential browser instances close on normal path

Authority: host search/network access

Evidence: [e5](#evidence-e5).

### generate_pdf_document | coding_project | browser_interaction | git_clone

Surface: secondary Decision response grammar.

Input: function/args/reply records

Result: PDF/files; browser branch faulty; git_clone TODO

Lifecycle: not established as primary entry; explicit partial branches

Authority: model-directed branch when host invokes make_decision

Evidence: [e21](#evidence-e21), [e20](#evidence-e20).

### SCROLL UP | SCROLL DOWN | CLICK | TYPE | TYPESUBMIT

Surface: separate browser model grammar.

Input: visible element index and text

Result: page manipulation/screenshot

Lifecycle: owned browser loop; inference call signature mismatch

Authority: partial browser integration, not working primary dispatch claim

Evidence: [e20](#evidence-e20).

### KnowledgeBase.add_knowledge / get_knowledge

Surface: host supporting API.

Input: tag and contents

Result: exact-match persistent record

Lifecycle: owned SQLite; unused in main search

Authority: not a native model SQL surface

Evidence: [e16](#evidence-e16), [e5](#evidence-e5).

### /api/messages | /api/get-agent-state | /api/get-terminal-session | /api/logs

Surface: operator HTTP.

Input: project identifier

Result: saved chat/state/UI/log data

Lifecycle: snapshot read

Authority: operator; no complete audit-query contract

Evidence: [e14](#evidence-e14), [e15](#evidence-e15), [e12](#evidence-e12).

### /api/settings

Surface: operator HTTP.

Input: nested settings

Result: singleton/TOML mutation

Lifecycle: immediate update; existing clients retain some captures

Authority: operator

Evidence: [e17](#evidence-e17), [e18](#evidence-e18).

### /api/run-code

Surface: operator HTTP stub.

Input: project/code

Result: misleading started response

Lifecycle: TODO, no process

Authority: not an implemented execution action

Evidence: [e19](#evidence-e19).

## Capabilities

### filesystem

**I — Files** (source): Model-generated file blocks saved, plus host project reading; model path joins lack resolved containment guard.

Evidence: [e6](#evidence-e6).

### processes

**L — OS programs** (source): Blocking space-split argv commands capture both pipes but drop stderr from repair/UI; no timeout/handles. Falsy success replays via decorator.

Evidence: [e8](#evidence-e8), [e9](#evidence-e9).

### code-actions

**I — Code actions** (source): Generated application files and patcher branch plus arbitrary runner commands.

Evidence: [e4](#evidence-e4), [e6](#evidence-e6), [e8](#evidence-e8).

### persistent-kernel

**? — Kernel** (source): No resident interpreter established in stage/Runner paths; subprocess per command.

### standing-database

**S — Standing DB** (source): Owned SQLite chat/state and exact-tag knowledge API; knowledge calls commented out, no model-facing query surface.

Evidence: [e12](#evidence-e12), [e14](#evidence-e14), [e16](#evidence-e16), [e5](#evidence-e5).

### workflow-programming

**S — Workflows** (source): Python stage objects and fixed dispatch compose workflow; no model-authored hot program surface established.

Evidence: [e2](#evidence-e2), [e3](#evidence-e3), [e4](#evidence-e4).

### multi-model

**L — Models** (source): Provider selector feeds same base model to every stage; no coordinated distinct participants.

Evidence: [e2](#evidence-e2), [e10](#evidence-e10).

### live-collaboration

**? — Peer chat** (source): Named stages are serial prompted roles, no live peer messaging established.

### concurrent-work

**L — Concurrency** (source): HTTP/socket threads overlap even active same-project work; no owned task/session arbitration.

Evidence: [e1](#evidence-e1), [e13](#evidence-e13).

### steering-interrupt

**L — Steer/interrupt** (source): Question polls saved latest user message every five seconds; new input can launch another workflow. Inference timer does not cancel provider worker.

Evidence: [e3](#evidence-e3), [e1](#evidence-e1), [e10](#evidence-e10).

### turn-redefinition

**S — Turn program** (source): Python fixed branches/templates are editable host code; no coordinated model hot turn activation.

Evidence: [e3](#evidence-e3), [e4](#evidence-e4), [e18](#evidence-e18).

### compaction

**? — Compaction** (source): No managed conversation compaction established; stages construct independent prompts.

### context-repair

**S — Repair** (source): Persisted project chat and current file corpus can be rebuilt into follow-up prompt; no targeted archived-context repair.

Evidence: [e4](#evidence-e4), [e15](#evidence-e15).

### original-audit

**L — Original audit** (source): Mutable JSON chat/state plus optional prompt logs; response content only and stderr discarded before capture; no complete original stream audit.

Evidence: [e8](#evidence-e8), [e10](#evidence-e10), [e11](#evidence-e11), [e13](#evidence-e13), [e14](#evidence-e14).

### audit-query

**S — Audit query** (source): Host/operator project/chat/state reads expose snapshots, not comprehensive audit.

Evidence: [e12](#evidence-e12), [e15](#evidence-e15).

### hot-change

**L — Hot change** (source): Config singleton changes immediately while captured stage models/import-time prompt templates persist; no turn/workflow boundary.

Evidence: [e17](#evidence-e17), [e18](#evidence-e18), [e2](#evidence-e2).

### rebuild-continuity

**? — Rebuild continuity** (source): Saved application state is not handoff of active provider/process operations; no quiescent refit traced.

### remote-services

**I — Remote** (source): Provider adapters, first-link search/browser, deployment branch consume remote services.

Evidence: [e5](#evidence-e5), [e10](#evidence-e10), [e4](#evidence-e4).

### self-improvement

**S — Self-improve** (source): Failure prompts invoke application patcher; no governed harness-change experiment.

Evidence: [e8](#evidence-e8), [e4](#evidence-e4).

### complaints

**? — Complaints** (source): No model complaint artifact with captured runtime state traced.

### authority

**L — Authority** (source): Main workflow executes commands/writes automatically; source joins model filenames and splits commands rather than supplying a coherent resource/authority contract.

Evidence: [e6](#evidence-e6), [e8](#evidence-e8).

### evaluation

**L — Evaluation** (source): Inspected benchmarks are not-yet marker; no assertion-based tests found in snapshot file inventory.

Evidence: [e22](#evidence-e22).

### time-order

**L — Time/order** (source): Second-resolution wall timestamps, polling/sleeps and shared JSON replacements do not establish total order or cancellation.

Evidence: [e13](#evidence-e13), [e15](#evidence-e15), [e3](#evidence-e3), [e10](#evidence-e10).

## Inspected test oracles

No relevant populated test source was recorded in this inspection; capability claims remain source/document traces.


## Useful mechanisms

- Explicit separation of application generation, search and repair stages makes workflow dataflow inspectable.
- Stored chat/state supplies operator visibility, with concrete gaps against original audit.

## Material limits

- No assertion-based reference tests found. No dependencies or reference executed. Browser/decision branches have specific unwired/faulty calls; owned stores are not shared standing-service capability.

## Arconaut design questions

- How should a workflow respond to steering while active without launching a second unowned execution?
- Can every command result preserve stderr, exit status, identity and cancellation before model projection?
- Which generated UI states are observations and which are theatrical emulation?

## Evidence

### Evidence e1

[quarantine/devika/devika.py:76–103](../../../quarantine/devika/devika.py#L76): Socket input creates per-message Agent and starts threads even when prior work is active; no owner identity/cancellation.

### Evidence e2

[quarantine/devika/src/agents/agent.py:36–67](../../../quarantine/devika/src/agents/agent.py#L36): Named stages all receive the same selected base model.

### Evidence e3

[quarantine/devika/src/agents/agent.py:270–360](../../../quarantine/devika/src/agents/agent.py#L270): Initial workflow planner/monologue/research/question polling/search/coder/save/completion.

### Evidence e4

[quarantine/devika/src/agents/agent.py:179–268](../../../quarantine/devika/src/agents/agent.py#L179): Follow-up action classifier selects answer/run/deploy/feature/bug/report with no generic tool loop.

### Evidence e5

[quarantine/devika/src/agents/agent.py:79–116](../../../quarantine/devika/src/agents/agent.py#L79): Search results fetched through browser and formatter; standing knowledge read/write commented out.

### Evidence e6

[quarantine/devika/src/agents/coder/coder.py:34–80](../../../quarantine/devika/src/agents/coder/coder.py#L34): File-block parser then host write using model filename joined into project path; no resolved containment guard.

### Evidence e7

[quarantine/devika/src/agents/coder/coder.py:90–134](../../../quarantine/devika/src/agents/coder/coder.py#L90): UI vim commands emulate editing; actual file save is separate.

### Evidence e8

[quarantine/devika/src/agents/runner/runner.py:69–198](../../../quarantine/devika/src/agents/runner/runner.py#L69): Commands split on spaces and synchronous subprocess.run without timeout; only stdout passed to state/repair. Retry counter shared across commands; function has no success return.

### Evidence e9

[quarantine/devika/src/services/utils.py:9–26](../../../quarantine/devika/src/services/utils.py#L9): retry_wrapper repeats entire function on falsy result up to five attempts, including Runner.run_code returning None.

### Evidence e10

[quarantine/devika/src/llm/llm.py:92–156](../../../quarantine/devika/src/llm/llm.py#L92): Inference executes selected provider in ThreadPoolExecutor; timeout raises/sys.exit but context manager waits for worker rather than cancellation.

### Evidence e11

[quarantine/devika/src/llm/openai_client.py:13–24](../../../quarantine/devika/src/llm/openai_client.py#L13): Single user prompt returns message.content, excluding original response metadata.

### Evidence e12

[quarantine/devika/src/state.py:10–23](../../../quarantine/devika/src/state.py#L10): Owned SQLite agent_state table stores JSON state stacks.

### Evidence e13

[quarantine/devika/src/state.py:65–100](../../../quarantine/devika/src/state.py#L65): Stack additions rewrite stored JSON; latest-state updates replace entries.

### Evidence e14

[quarantine/devika/src/project.py:47–79](../../../quarantine/devika/src/project.py#L47): Chat messages persisted as rewritten JSON stack; UI emitted before persistence.

### Evidence e15

[quarantine/devika/src/project.py:81–128](../../../quarantine/devika/src/project.py#L81): Host queries latest user message and formatted project chat.

### Evidence e16

[quarantine/devika/src/memory/knowledge_base.py:10–33](../../../quarantine/devika/src/memory/knowledge_base.py#L10): SQL knowledge table exact-tag add/get primitive; not wired to main search path.

### Evidence e17

[quarantine/devika/src/config.py:182–195](../../../quarantine/devika/src/config.py#L182): Settings update mutates singleton and TOML immediately, no activation coordination.

### Evidence e18

[quarantine/devika/src/agents/runner/runner.py:15–21](../../../quarantine/devika/src/agents/runner/runner.py#L15): Prompt templates captured at module import; per-stage clients capture model.

### Evidence e19

[quarantine/devika/devika.py:154–161](../../../quarantine/devika/devika.py#L154): run-code endpoint returns started message despite TODO no execution.

### Evidence e20

[quarantine/devika/src/browser/interaction.py:485–521](../../../quarantine/devika/src/browser/interaction.py#L485): Browser command grammar exists but LLM.inference call omits required project_name.

### Evidence e21

[quarantine/devika/src/agents/agent.py:128–177](../../../quarantine/devika/src/agents/agent.py#L128): Decision branch git_clone is TODO; browser uses unassigned self.base_model; coding/pdf branches implemented.

### Evidence e22

[quarantine/devika/benchmarks/SWE-bench.md:1–1](../../../quarantine/devika/benchmarks/SWE-bench.md#L1): Benchmark file contains only not-yet marker; no execution/oracle.

