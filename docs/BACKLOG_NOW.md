# Useful open work — 2026-10-07

Operator asks what the flat backlog actually means, especially "byte audit".
This is current triage against source, reports and candidate commits, not a new
assurance hierarchy. Historical issue prefixes remain for identity.

## Immediate performance work

| Issue | Actual deliverable | Evidence and priority |
| --- | --- | --- |
| arconaut-fsp | Native timing surfaces and real history measurements | Merged; debug-only build machinery, defaultOFF, runtime recording also explicit. Issue closed. |
| arconaut-02g | Reduce capture/reservation/synchronization work per streamed chunk | Pushed e0e0b98:14610 IDs require15 reservations/syncs versus14610; five affected checks and one review/fix/recheck passed. Candidate awaits integration; stream batching remains separate. |
| arconaut-n11 | Startup below100ms with large retained history/context | Pushed35413f3: warm335.8MB-history fixture audited readiness1394.550→265.961ms, prompt180.675ms. Target missed; replay160.891ms + engine/admission85.965ms remain. Large live context unmeasured. Candidate awaits integration. |
| arconaut-kut | Continue unfinished units until completion or same deadline | Fixed shared driver with direct six-segment/deadline/pause checks, no budget renewal or blind replay; closed. |
| arconaut-1v0 | Store invocation input once and reference it from semantic records | Old census measured46,559,372 duplicated blob bytes between invocation/admission alone; decision repeats logicalinput. Requires versioned references and historical readability, not history deletion. |
| arconaut-3x2 | Stop copying complete metadata/context history on small appends | Current append copies Snapshot vectors; live lookup scans facts; ContextStore copies/serializes cumulative state. Split retained suffix storage and context indexing at their real ownership boundaries. |
| arconaut-ayg | Scoped Linux check of already-delivered startup changes | Useful portability check on Neuroses; no new productfeature, blanket recertification or blocker on unrelated work. |

The three implementation byte tasks come from the actual
[waste audit](../papers/2026-10-07-waste-audit.md). Native profile windows now also
catch sync and retained metadata work. P1 is small instrumentation serving those
fixes; it must not grow into a laboratory that postpones the fixes.

Both renewed performance autodevs completed within their allowances; zero active.
Next useful step is serial integration of their delivered changes and a matched
combined startup measurement. The allocator may reduce admission cost; that
interaction has not been measured. No additional review panel is queued.

## Other useful product work

- **Native Beads integration (arconaut-jim):** operator requests sketch/design then
  BB autodev. docs/BEADS_DESIGN.md defines shared C++ model/Lua/slash operations,
  lazy canonical project binding and truthful mutation outcomes. Existing bd CLI
  is the first external backend; direct Dolt is not silently adopted. B1 merged
  with affected checks and native installed readonly ready/show smoke; issue closed.

- **Workflow registry delivery (arconaut-3oz.2):** implemented/tested/pushedc8498f0;
  first substantive review found two repairs: bound palette selection after hot
  registry shrink, and latch caught invocation-limit failures. Candidate remains
  inactive; report specifies direct regression checks. Fix these without restarting
  implementation or adding another tribunal.
- **Real workflow execution:** structured concurrent tasks/joins, restartable
  segments/effect reconciliation, then station signals/timers and artifact reuse.
  These are W2–W5 in WORKFLOW_CAMPAIGN, not delivered by registry invocation.
- **Interrupted command/file recovery (arconaut-05n):** provider-only recovery is
  delivered; broader non-provider uncertain effects need inspect/reconcile/pivot
  behavior without blind replay. Original startup-error wording is historical.
- **Heterogeneous colleagues (arconaut-7iy.5):** independent candidate retained;
  finish useful cross-provider operation/integration. Old auth notes predate restored
  Claude and installed/authenticated Grok; refresh exact current obstacles before
  asking the operator for credentials or treating every backend as blocked.
- **Conversation ergonomics (arconaut-os3):** rich chat/slash shell delivered;
  remaining concrete features are Markdown, folding, theme/copy and anchors after
  display eviction. Old rename/activation text is stale. Small-terminal issue
  arconaut-uaa.13's clamp is removed in current source; its original defect is not
  current evidence of bad behavior. A narrowly missing PTY check must not become
  another generic TUI implementation task.
- **phux lifecycle integration (arconaut-nmm):** external launch/detach/RRC support
  exists; shell completion semantics and optional lifecycle projection remain.
- **Independent OpenAI login (arconaut-uaa.5):** distribution work; current
  Codex sign-in/refresh scaffolding remains explicitly accepted for now.

## Housekeeping and historical scope

Closed delivered editor/palette issues arconaut-buc/cqp after confirming source
commits are ancestors of master and current UI supports them. Closed remote-branch
retirement arconaut-suv against aa1d63a; local archive refs remain intentional.
Closed W0 arconaut-3oz.1 as superseded; its failed standalone BB report is not
claimed complete. No useful reason to rerun the full survey before further work.

Formatting/static-analysis debt arconaut-lh1 is concrete low-priority cleanup.
Old bootstrap/U1 qualification wrappers arconaut-uaa.2/.4 and top-level epics
need descriptions aligned to remaining distinct runtime capabilities; they do not
justify rerunning old qualification campaigns. Complaint arconaut-pvl is untriaged:
its title alone does not establish severity or a separate implementation task.
