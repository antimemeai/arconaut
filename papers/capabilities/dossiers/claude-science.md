# claude-science

Versioned scientific artifacts, code execution, specialist delegation and reviewed research in a local workbench.

Role: scientific desktop workbench (official contracts; unavailable engine source). Runtime: .

Pinned source: [https://claude.com/product/claude-science](https://claude.com/product/claude-science); revision/version `0.1.55`.

Local app/daemon and sandboxed Python/R kernels; operator browser or Windows app; remote jobs have external lifetimes.

Owns local project/session/workspace/package environments and ephemeral kernels; consumes Claude, MCP, SSH/Slurm, Modal and scientific HTTP model services.

Inspection: pinned official CLI, kernel, artifact, permissions, connector, compute, model and reviewer documentation

Limits of this study: Application source unavailable; documentation snapshots captured alongside 0.1.55 artifacts need not be version-identical to every shipped build. No binary or example execution; no source tests available.

## Actions

### Run Python / Run R / Run shell / Run PowerShell

Surface: model-proposed code cell.

Input: Executable code plus environment; access card grants

Result: Cell outputs/errors and saved artifact references; native tool wire names undisclosed

Lifecycle: Persistent interpreter state for Python/R; kernel expires/restarts; shell lifetime/control unspecified

Authority: Sandbox/project/folder/host grants; scoped standing code approval

Evidence: [e1](#evidence-e1), [e2](#evidence-e2).

### Save artifact / Edit content / Save / Provenance / Export Metadata

Surface: model save; operator artifact commands.

Input: Filename/content or artifact-version selection

Result: New immutable-history version, version-bound link, code/log/environment/review tabs

Lifecycle: Saved versions retained; scratch files removed hours after session end; session deletion retains artifacts, project deletion removes

Authority: Operator/model workspace access; these are product control names, not invented public RPCs

Evidence: [e4](#evidence-e4).

### Delegation / Stop

Surface: operator session settings; model delegation.

Input: Enable independent tracks; stop current session

Result: Per-track transcripts/status; whole-session stop

Lifecycle: Parallel tracks; no individual-track stopping contract

Authority: Session operator; work permitted under existing access grants

Evidence: [e3](#evidence-e3).

### Request review / Auto-review / Reviewer Instructions

Surface: operator review controls.

Input: Recent work and added criteria

Result: Finding cards/status/reasoning fed to next model response

Lifecycle: Runs after responses/periodically; disabling prevents new reviews but pending finish

Authority: Operator customizes additive criteria; model addresses or contests findings

Evidence: [e5](#evidence-e5).

### SSH remote job / Modal job

Surface: model proposal with scoped operator grants.

Input: Script/command, host/resource/time limit and staged inputs

Result: Logs, outputs or remote paths; concrete provider handle API undisclosed

Lifecycle: Slurm/detached SSH work survives disconnect; Modal survives app close and times out externally

Authority: Remote work uses operator account outside local sandbox; standing remote grant allowed

Evidence: [e8](#evidence-e8), [e9](#evidence-e9).

### Add connector / Always allow / Ask each time / Block

Surface: operator connector configuration; model discovered MCP tools.

Input: Remote SSE/HTTP URL or local program plus args/env; individual tool policy

Result: Discovered external tools/results; wire APIs remain unavailable

Lifecycle: Local connector subprocess within sandbox; configured remote service consumed

Authority: Custom connector organization policy and standing tool authority

Evidence: [e11](#evidence-e11).

### / skill / Add skill / Import from GitHub / distill workflow

Surface: operator and model skill authoring.

Input: Instruction/resources or repository; session workflow description

Result: Loaded procedural instructions, reusable skill

Lifecycle: Task-selected or explicit loading; automatic context/activation implementation unobserved

Authority: Operator/custom-skill organizational policy; model can distill a workflow

Evidence: [e12](#evidence-e12).

### claude-science serve/open/url/status/logs/stop/update/import

Surface: operator CLI.

Input: Data dir/config, build ID for update, source dir/database for import

Result: Daemon, login URL, JSON status, logs, atomic replacement or merged data

Lifecycle: One daemon per data dir; update requires restart to inhabit new build; no main/outpost handoff contract

Authority: Operator OS account; update/import may mutate owned app data

Evidence: [e6](#evidence-e6), [e7](#evidence-e7).

## Capabilities

### filesystem

**D — Files** (documentation): Read/write granted folders in place, attachment copies and artifact-version saves; scope grants are persistent.

Evidence: [e2](#evidence-e2), [e4](#evidence-e4).

### processes

**D — OS programs** (documentation): Shell/PowerShell code and remote detached/Slurm work; no exposed general PTY/ongoing local-program handle protocol in inspected contracts.

Evidence: [e1](#evidence-e1), [e9](#evidence-e9).

### code-actions

**D — Code actions** (documentation): Model-authored executable Python/R/shell is the analysis surface; MCP tools supplement it.

Evidence: [e1](#evidence-e1), [e11](#evidence-e11).

### persistent-kernel

**L — Kernel** (documentation): Documented Python/R variables survive steps but kernel ends after about 30 minutes idle, session end or environment restart; no serialization/reconnect contract for shared external kernels.

Evidence: [e1](#evidence-e1).

### standing-database

**L — Standing DB** (documentation): Local memory database holds remembered facts and feeds model context; no documented generic model/program SQL interface. Scientific source connectors are distinct external databases.

Evidence: [e3](#evidence-e3), [e11](#evidence-e11).

### workflow-programming

**L — Workflows** (documentation): Reusable skills and distilled workflows are written instructions; executable turn/workflow scheduling replacement is not established by those controls.

Evidence: [e12](#evidence-e12).

### multi-model

**D — Models** (documentation): Claude model selection and safeguard-driven switching, plus distinct scientific model endpoints; independent conversational providers not established.

Evidence: [e13](#evidence-e13), [e8](#evidence-e8).

### live-collaboration

**? — Peer chat** (inspection): Not established in the explicitly inspected pinned official CLI, kernel, artifact, permissions, connector, compute, model and reviewer documentation; no universal absence claim.

### concurrent-work

**D — Concurrency** (documentation): Parallel delegated tracks, multiple per-session kernels, Modal/SSH job limits and remote independent lifetime.

Evidence: [e3](#evidence-e3), [e8](#evidence-e8), [e9](#evidence-e9).

### steering-interrupt

**L — Steer/interrupt** (documentation): Whole-session Stop rather than individually controllable tracks; disabling auto-review leaves active/waiting reviews to finish. Remote cancellation details not exposed.

Evidence: [e3](#evidence-e3), [e5](#evidence-e5).

### turn-redefinition

**? — Turn program** (inspection): Not established in the explicitly inspected pinned official CLI, kernel, artifact, permissions, connector, compute, model and reviewer documentation; no universal absence claim.

### compaction

**? — Compaction** (inspection): Not established in the explicitly inspected pinned official CLI, kernel, artifact, permissions, connector, compute, model and reviewer documentation; no universal absence claim.

### context-repair

**L — Repair** (documentation): Past-session references and saved artifact-version messages can recover some evidence; scratch cleanup and no documented compaction-original lineage prevent claiming comprehensive repair.

Evidence: [e3](#evidence-e3), [e4](#evidence-e4).

### original-audit

**L — Original audit** (documentation): Artifact provenance records command history/code/environment/messages, but temporary files are removed and no raw provider/process/transform IO-retention contract is exposed.

Evidence: [e4](#evidence-e4).

### audit-query

**L — Audit query** (documentation): Human searchable artifacts and Provenance tabs are documented; comprehensive model/program audit query API is not exposed in inspected contracts.

Evidence: [e4](#evidence-e4).

### hot-change

**L — Hot change** (documentation): Review toggles affect future/pending review behavior; skills/connectors adjustable; config.toml is startup-only and binary update inhabitation is restart.

Evidence: [e5](#evidence-e5), [e10](#evidence-e10), [e6](#evidence-e6).

### rebuild-continuity

**L — Rebuild continuity** (documentation): Atomic vendor update preserves data with schema compatibility checks; no self-compile/outpost transfer, active-work quiescence or restored live-kernel guarantee.

Evidence: [e6](#evidence-e6), [e1](#evidence-e1).

### remote-services

**D — Remote** (documentation): Consumer of operator SSH/Slurm/Modal account and scientific HTTP/MCP endpoints; job survival differs from local ephemeral kernels.

Evidence: [e8](#evidence-e8), [e9](#evidence-e9), [e11](#evidence-e11).

### self-improvement

**? — Self-improve** (inspection): Not established in the explicitly inspected pinned official CLI, kernel, artifact, permissions, connector, compute, model and reviewer documentation; no universal absence claim.

### complaints

**? — Complaints** (inspection): Not established in the explicitly inspected pinned official CLI, kernel, artifact, permissions, connector, compute, model and reviewer documentation; no universal absence claim.

### authority

**D — Authority** (documentation): Folder/network/tool/code/remote grants can stand per conversation/project/global; skip approvals and sandbox flags have documented exceptions and organizational precedence.

Evidence: [e2](#evidence-e2), [e7](#evidence-e7), [e11](#evidence-e11).

### evaluation

**L — Evaluation** (documentation): Reviewer compares claims to recorded work and cited material, does not rerun analyses or judge research-method choice; no source/runtime test oracle inspected.

Evidence: [e5](#evidence-e5).

### time-order

**D — Time/order** (documentation): Kernel idle expiry, one-use login expiry, remote timeout bounds and pending-review activation semantics; no global causal audit clock exposed.

Evidence: [e1](#evidence-e1), [e7](#evidence-e7), [e8](#evidence-e8), [e9](#evidence-e9), [e5](#evidence-e5).

## Inspected test oracles

No relevant populated test source was recorded in this inspection; capability claims remain source/document traces.


## Useful mechanisms

- A code-first scientific workbench with bounded persistent kernels and independently surviving remote computation.
- Saved artifact versions bind messages, command log, code/environment and review, distinguishing actual execution log from generated reproduction code.
- Scoped standing authority and custom scientific instructions/reviewer support sophisticated domain work.

## Material limits

- Every positive finding is a captured official contract, not traced application implementation or executed behavior.
- Internal remembered-fact database is not a generic standing data service; Lecoq workflow and OpenAI4S do not establish app internals.
- Kernel memory and temporary files have explicit expiration; artifact provenance does not promise Arconaut original-audit completeness.
- Turn program replacement, live addressed peers, dedicated autoresearch governance and captured-state model complaints remain unestablished in inspected contracts.

## Arconaut design questions

- How do we keep service references and resumable client work independent of local interpreter lifetime without governing shared service lifetimes?
- What exact originals survive context transformations and failed attempts, beyond artifact-centered provenance?
- Can both operator and model query/repair context from preserved originals and adjust reviewer/workflow behavior at our chosen turn/workflow conclusion boundary?

## Evidence

### Evidence e1

[quarantine/claude-science/raw/tools-and-environments.md:9–36](../../../quarantine/claude-science/raw/tools-and-environments.md#L9): Python/R state persists within a kernel; idle/session/environment termination boundaries; named environments and source builds.

### Evidence e2

[quarantine/claude-science/raw/core-concepts.md:9–38](../../../quarantine/claude-science/raw/core-concepts.md#L9): Projects/sessions own workspaces; filesystem and code/remote permissions have standing scopes.

### Evidence e3

[quarantine/claude-science/raw/core-concepts.md:40–64](../../../quarantine/claude-science/raw/core-concepts.md#L40): Plan approval, sandbox, parallel delegation with whole-session stop, local remembered facts and session/file/skill references.

### Evidence e4

[quarantine/claude-science/raw/artifacts.md:9–43](../../../quarantine/claude-science/raw/artifacts.md#L9): Temporary scratch cleanup differs from artifact retention; saved versions and authoritative execution/provenance tabs.

### Evidence e5

[quarantine/claude-science/raw/the-reviewer.md:9–32](../../../quarantine/claude-science/raw/the-reviewer.md#L9): Reviewer challenges recorded claims without rerunning methods; findings feed next response; changes affect future reviews while pending finish.

### Evidence e6

[quarantine/claude-science/raw/command-line-settings.md:14–42](../../../quarantine/claude-science/raw/command-line-settings.md#L14): Daemon management, JSON status/logs/import, atomic update and restart/downgrade boundaries.

### Evidence e7

[quarantine/claude-science/raw/command-line-settings.md:49–90](../../../quarantine/claude-science/raw/command-line-settings.md#L49): Per-data-dir app, login expiry and approval/sandbox overrides constrained by organization.

### Evidence e8

[quarantine/claude-science/raw/compute-providers.md:9–49](../../../quarantine/claude-science/raw/compute-providers.md#L9): Consumer-controlled Modal account, observable bounded jobs, independent lifetime and scientific HTTP endpoints.

### Evidence e9

[quarantine/claude-science/raw/remote-compute-clusters.md:9–39](../../../quarantine/claude-science/raw/remote-compute-clusters.md#L9): Existing SSH authority, Slurm or detached processes survive connection loss; timeout and output-size boundaries.

### Evidence e10

[quarantine/claude-science/raw/configuration-file-reference.md:9–15](../../../quarantine/claude-science/raw/configuration-file-reference.md#L9): TOML configuration loaded at startup; organization precedence.

### Evidence e11

[quarantine/claude-science/raw/custom-connectors.md:9–21](../../../quarantine/claude-science/raw/custom-connectors.md#L9): Remote/local MCP discovery and per-tool standing permission; sandboxed connector subprocesses.

### Evidence e12

[quarantine/claude-science/raw/connectors-and-skills.md:55–69](../../../quarantine/claude-science/raw/connectors-and-skills.md#L55): Skills are instructions loaded by task or composer; create/write/upload/import and distill session workflows.

### Evidence e13

[quarantine/claude-science/raw/safeguards.md:9–35](../../../quarantine/claude-science/raw/safeguards.md#L9): Model safeguards and operator model-switch behavior are official contracts, not engine traces.

