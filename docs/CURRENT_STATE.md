# Current Blackbird stock — 2026-10-08

The operator explicitly directed merging all remaining source. All12 candidate
branch tips are now included in master, including cold history, physical recovery
hints, the recovery index and OpenAI/Claude colleague code. Native Release binaries
are locally published; DEBUG/profiling OFF. Twenty-one affected checks passed
(20 initially, corrected checkpoint boundary test on direct recheck). Actual
launcher/Lua smoke passed. No autodev or new review campaign was started.

## What is on master

| Area | Current behavior | Material limit |
| --- | --- | --- |
| Native harness | C++20/Lua, terminal composer, command palette and external editor; retained sessions; restart/resume/continue. | OpenAI authentication still uses installed Codex scaffolding. |
| Context | CLM edits, managed compaction, archived original recovery and context budget policy. | Read selector now has one lines/bytes mode; malformed calls return correction guidance. Historical Lua ranges stay compatible. |
| Recovery | Cooperative backstop, bounded provider recovery, workflow repair, retained unknown effects. | Arbitrary exec and escaped/remote effects prevent automatic claims of containment; general crash recovery unfinished. |
| Programmability | Retained dynamic tool definitions, named modules, staged model/effort defaults and effective/pending inspection. | Workflow-selector staging and atomic interrupt/apply remain deferred. |
| Orchestration | Lua sequence, branch, explicit safe retry and selected join. | Synchronous composition; no native parallel participant scheduler or general cancellation/messaging. |
| Station | Durable-before-dispatch local source admission, duplicates, boundary inspect/steer/pause/resume/stop. | Local-file source; unknown effects remain unknown; no live TUI attachment or crash-containment claim. |
| Integrations/HUD | Opt-in trusted Lua packages; public GitHub releases and thematic text feed used in real work. | Feed display/context/action are separate; no broad integration catalog or graphical HUD. |
| Multiplexing | Supported external phux launcher, actual TUI turn, detach/reattach and RRC checked. | Shell run probe timed out; native lifecycle projection is backlog. Relay TLS does not establish application E2EE. |
| Decision models | Native Jev model tool, Lua `blackbird.decide`, `/decision JSON`; batched Choice/Score/Noul, lazy credentials, audited requests/results. | Jev only; workflows compose judgments and explicit retries. |
| Callable workflows | Retained Lua definitions, configurable slash prefix/aliases and bare invocation, operator POWERWORDS and terminal colors; model/Lua discovery/invocation. | Sequential same-turn execution; no parallel or durable scheduler. |
| Local efficiency | Saved current-state and suffix recovery for settled single-segment sessions; paged checked archive queries and original repair; checkpoint renewal prunes resident history. Heavy TUI startup p90 79ms in100runs. | Unresolved/station/pending-restart/multisegment states fall back to full recovery; bulk metadata renewal and audited command latency remain work. See [delivery](../papers/2026-10-08-saved-state-delivery.md) and [measurements](../papers/2026-10-08-startup-300.md). |
| Colleagues | `blackbird-colleague` CLI/library, selected-context OpenAI/Claude requests, explicit captures/results and fake-transport tests. `arco-colleague` remains compatible. | One synchronous context-only call; broader provider coverage and useful live collaboration remain unfinished. |
| Presentation | MIT, Patrick Beam copyright, GitHub About, supported phux link; BLACKBIRD wordmark and supplied SR-71 startup/status image. | Ghostty/Kitty image support; terminal fallback. |

## Remaining design and delivery

The colleague source is merged; this pass checked adapter behavior with existing
fixtures, without another live provider campaign. Prior review/auth timeouts are
historical evidence, not an integration gate. Kimi/MiMo/Grok are not implemented
by this colleague adapter.

G9 is a security discussion only. Its original OpenSSL/TLS proposal is not a
selected multiplayer architecture. Transport, room/message security, peer identity,
membership, enrollment/revocation, artifact/job authority and reconnect semantics
need design together. Frumentarii delivered primary literature/refimpl studies; read the
[source-grounded synthesis](../papers/networking-2026-10-07/SYNTHESIS.md). No networking dependency adopted.

[Branch inventory](BRANCH_INVENTORY.md) records all12 candidate tips included in
master. Branch refs and worktrees remain; no source history was rewritten.

## Remaining work

Saved current-state/suffix recovery is delivered for the supported settled,
single-segment population. The earlier recovery continuation notes describe
historical incomplete work; current behavior and limits are in the
[delivery report](../papers/2026-10-08-saved-state-delivery.md).

Five subsequent performance commits deliver bounded provider-stream capture,
two fewer retained request bodies, context append deltas and removal of historical
index clones, bounded publication-scratch cleanup, and one fewer unchanged UI-state
save on final-byte submission. Their six performance/reopen issues remain open or
in progress: native invocation/admission duplication, remaining live/history copies,
capture sync/CPU/latency and diagnostic lifetime, catalog/resident-tail bounds,
launch-to-command latency and the supported reopen/fallback policy still need work.
The broad coding test has a recorded unresolved `unsupported` failure; focused
affected checks passed. No overall latency speedup is established by these slices.

At the 2026-10-08 assessment, the rebuilt `build/release/blackbird` contains these
five follow-ups, but ordinary interactive `scripts/blackbird` still selects the
older published `blackbird-ui` measured in the 300-start report. Explicit executable
selection overrides that choice. The interactive binary has not been republished
by this assessment; source delivery and interactive activation remain distinct.

Other backlog: scoped Linux
checks, useful heterogeneous colleagues, richer terminal presentation, asynchronous
participant orchestration, and multiplayer design. Giga and autonomous development
remain stopped. No new campaign is launched by this stock report.
