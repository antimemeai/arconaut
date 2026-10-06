# scientific-agent-skills

181 scientific skills, references, scripts and a workflow-derived skill drafting primitive

Role: scientific skill pack with executable helpers. Runtime: Markdown, Python.

Pinned source: [https://github.com/K-Dense-AI/scientific-agent-skills](https://github.com/K-Dense-AI/scientific-agent-skills); revision/version `91497e335489dcb544ec8ddc8f6b7ce5fd6d1121`.

Host agent discovers SKILL.md; selected helper scripts are normal Python processes. Autoskill explicitly composes local observation, embedding and LLM services.

Consumes host execution/skills, public databases and user-managed screenpipe/LLM endpoints; supplies no coding-agent turn loop.

Inspection: plugin metadata, database-lookup contract, autoskill discovery/provider/synthesis/staging/promotion and deterministic fake-service tests; representative DICOM UID validation

Limits of this study: Catalog breadth inventoried, not every helper manually traced. Host dispatch/compaction/audit enforcement are outside pack.

## Actions

### SKILL.md frontmatter / load_skill_descriptions(skills_dir)

Surface: Host skill catalog and autoskill Python discovery.

Input: Directory of */SKILL.md name/description

Result: Selected skill guidance or list used for cosine similarity

Lifecycle: On file read/load; no host reload scheduling contract

Authority: Host loader and autoskill client filesystem authority

Evidence: [e1](#evidence-e1), [e2](#evidence-e2), [e3](#evidence-e3).

### database-lookup retrieval contract

Surface: Authored public API workflow.

Input: Named database, entity IDs, filters, completeness/count/pagination requirements

Result: Bounded API results plus endpoints/params/access date/count reconciliation

Lifecycle: External API request/response; no persistent interpreter or standing app DB

Authority: Read/Bash frontmatter; authored confirmation thresholds; host enforces authority

Evidence: [e2](#evidence-e2).

### autoskill.run(config, start_time, end_time, ...)

Surface: Executable observation→proposal helper.

Input: Screenpipe client/token, time window, embedding/LLM backend and skills/output directories

Result: Timestamped plan/report and staged new-skills/composition-recipes folders

Lifecycle: On-demand synchronous run; dry-run stops after clustering; no resident auto loop

Authority: User-triggered in skill contract; calls separately managed services

Evidence: [e4](#evidence-e4), [e8](#evidence-e8).

### autoskill.synthesize(cluster, top_k_skills, backend)

Surface: Executable model request and parse.

Input: Redacted cluster and candidate descriptions

Result: reuse target or compose/novel valid-name nonempty SKILL.md draft

Lifecycle: One selected backend call per cluster, response schema validation

Authority: Backend credential/endpoint selected by invoking client

Evidence: [e5](#evidence-e5), [e7](#evidence-e7).

### autoskill.promote(proposed_path, skills_dir, name)

Surface: Executable staged file move.

Input: Proposal directory, target catalog and name

Result: Moved skill folder; existing target rejected

Lifecycle: Filesystem promotion; no quality eval, rollback or in-flight host redefinition

Authority: Explicit CLI/client invocation; host activation unspecified

Evidence: [e6](#evidence-e6).

### uid_mapping_validator.validate_mapping(...)

Surface: Executable scientific-data validator.

Input: Bounded map, optional key/scope and2.25 requirement

Result: Aggregate error_codes/counts/ok; no UIDs emitted

Lifecycle: One call, no network; validates map consistency, not dataset completeness

Authority: Caller files/key; host invocation needed

Evidence: [e9](#evidence-e9).

## Capabilities

### filesystem

**S — Files** (source): Helper machinery reads skill directories and writes/moves staged drafts; not a general model file dispatcher.

Evidence: [e3](#evidence-e3), [e4](#evidence-e4), [e6](#evidence-e6).

### processes

**— — OS programs** (inspection): Outside this scientific skill pack with executable helpers role: the reference supplies 181 scientific skills, references, scripts and a workflow-derived skill drafting primitive rather than an agent execution engine.

### code-actions

**S — Code actions** (source): Executable Python helpers and authored code workflows compose external APIs; underlying host grants execution.

Evidence: [e4](#evidence-e4), [e7](#evidence-e7), [e9](#evidence-e9).

### persistent-kernel

**— — Kernel** (inspection): Outside this scientific skill pack with executable helpers role: the reference supplies 181 scientific skills, references, scripts and a workflow-derived skill drafting primitive rather than an agent execution engine.

### standing-database

**L — Standing DB** (documentation): Public database lookup guidance consumes external corpora; it supplies neither model-owned standing DB nor generic query engine.

Evidence: [e2](#evidence-e2).

### workflow-programming

**S — Workflows** (source): Autoskill composes an executable pipeline with injectable clients/backend/embedder; broader science workflows are skill instructions, not a new agent scheduler.

Evidence: [e4](#evidence-e4).

### multi-model

**S — Models** (source): Autoskill chooses local OpenAI-compatible, Anthropic or Foundry backend; no live multi-model peer coordination.

Evidence: [e7](#evidence-e7).

### live-collaboration

**— — Peer chat** (inspection): Outside this scientific skill pack with executable helpers role: the reference supplies 181 scientific skills, references, scripts and a workflow-derived skill drafting primitive rather than an agent execution engine.

### concurrent-work

**— — Concurrency** (inspection): Outside this scientific skill pack with executable helpers role: the reference supplies 181 scientific skills, references, scripts and a workflow-derived skill drafting primitive rather than an agent execution engine.

### steering-interrupt

**— — Steer/interrupt** (inspection): Outside this scientific skill pack with executable helpers role: the reference supplies 181 scientific skills, references, scripts and a workflow-derived skill drafting primitive rather than an agent execution engine.

### turn-redefinition

**— — Turn program** (inspection): Outside this scientific skill pack with executable helpers role: the reference supplies 181 scientific skills, references, scripts and a workflow-derived skill drafting primitive rather than an agent execution engine.

### compaction

**— — Compaction** (inspection): Outside this scientific skill pack with executable helpers role: the reference supplies 181 scientific skills, references, scripts and a workflow-derived skill drafting primitive rather than an agent execution engine.

### context-repair

**— — Repair** (inspection): Outside this scientific skill pack with executable helpers role: the reference supplies 181 scientific skills, references, scripts and a workflow-derived skill drafting primitive rather than an agent execution engine.

### original-audit

**L — Original audit** (source): Database skill prescribes provenance; autoskill writes reduced reports/proposals after redaction/clustering, not all original observations/model IO.

Evidence: [e2](#evidence-e2), [e4](#evidence-e4).

### audit-query

**— — Audit query** (inspection): Outside this scientific skill pack with executable helpers role: the reference supplies 181 scientific skills, references, scripts and a workflow-derived skill drafting primitive rather than an agent execution engine.

### hot-change

**— — Hot change** (inspection): Outside this scientific skill pack with executable helpers role: the reference supplies 181 scientific skills, references, scripts and a workflow-derived skill drafting primitive rather than an agent execution engine.

### rebuild-continuity

**— — Rebuild continuity** (inspection): Outside this scientific skill pack with executable helpers role: the reference supplies 181 scientific skills, references, scripts and a workflow-derived skill drafting primitive rather than an agent execution engine.

### remote-services

**S — Remote** (source): Consumes independently managed screenpipe and LLM endpoints, plus documented public database APIs; does not control their continuity.

Evidence: [e2](#evidence-e2), [e4](#evidence-e4), [e7](#evidence-e7), [e8](#evidence-e8).

### self-improvement

**L — Self-improve** (source): Dedicated observe→draft→promote skill machinery exists; output shape and staging are checked, but no performance experiment, harness modification or autonomous acceptance criterion.

Evidence: [e4](#evidence-e4), [e5](#evidence-e5), [e6](#evidence-e6), [e10](#evidence-e10).

### complaints

**— — Complaints** (inspection): Outside this scientific skill pack with executable helpers role: the reference supplies 181 scientific skills, references, scripts and a workflow-derived skill drafting primitive rather than an agent execution engine.

### authority

**D — Authority** (documentation): Per-skill allowed-tools and authored user-triggered observation/remote choices; runtime enforcement belongs to host.

Evidence: [e2](#evidence-e2), [e8](#evidence-e8).

### evaluation

**S — Evaluation** (source): Offline fake-service tests verify pipeline artifacts and promotion; DICOM helper checks domain mapping invariants. No oracle establishes proposed skill usefulness.

Evidence: [e9](#evidence-e9), [e10](#evidence-e10), [e11](#evidence-e11).

### time-order

**S — Time/order** (source): Autoskill converts observation timestamps and creates UTC-named proposal windows; no event-clock/paused-time semantics.

Evidence: [e4](#evidence-e4).

## Inspected test oracles

- [quarantine/scientific-agent-skills/tests/autoskill/test_e2e.py](../../../quarantine/scientific-agent-skills/tests/autoskill/test_e2e.py): Offline observation/redaction/clustering/matching/draft pipeline Oracle: Deterministic fake OCR/LLM/embedding check known surviving patterns and exact staged draft paths. Does not establish real-world detection or generated skill quality. Read, **not executed**.
- [quarantine/scientific-agent-skills/tests/autoskill/test_promote.py](../../../quarantine/scientific-agent-skills/tests/autoskill/test_promote.py): Proposal move and content preservation Oracle: Literal path/content/presence and explicit failure checks; no host activation or effectiveness oracle. Read, **not executed**.

## Useful mechanisms

- Concrete proposal pipeline with injectable dependencies and offline literal oracles.
- Scientific helpers can expose aggregate invariant failures rather than plausible prose.

## Material limits

- Skill discovery does not imply host execution/reload/authority enforcement.
- Promotion is a file move; no comparative evaluation of proposed skills or harness self-refit.

## Arconaut design questions

- Can workflow-derived proposal machinery feed model-governed autodroit experiments with independent performance oracles?
- How should scientific helper provenance be captured at original request/response scope while consumer services remain independent?

## Evidence

### Evidence e1

[quarantine/scientific-agent-skills/plugin.json:1–22](../../../quarantine/scientific-agent-skills/plugin.json#L1): Plugin metadata identifies scientific skill pack.

### Evidence e2

[quarantine/scientific-agent-skills/skills/database-lookup/SKILL.md:1–38](../../../quarantine/scientific-agent-skills/skills/database-lookup/SKILL.md#L1): Frontmatter discovery and bounded public database retrieval/provenance contract, not a resident DB.

### Evidence e3

[quarantine/scientific-agent-skills/skills/autoskill/scripts/match_skills.py:5–46](../../../quarantine/scientific-agent-skills/skills/autoskill/scripts/match_skills.py#L5): Directory frontmatter description discovery and cosine top-k matching.

### Evidence e4

[quarantine/scientific-agent-skills/skills/autoskill/scripts/run.py:68–132](../../../quarantine/scientific-agent-skills/skills/autoskill/scripts/run.py#L68): Fetch/redact/segment/cluster/match/synthesize writes proposals and report; dry-run avoids synthesis.

### Evidence e5

[quarantine/scientific-agent-skills/skills/autoskill/scripts/synthesize.py:57–79](../../../quarantine/scientific-agent-skills/skills/autoskill/scripts/synthesize.py#L57): LLM output validates verdict/name/nonempty body, not skill behavior.

### Evidence e6

[quarantine/scientific-agent-skills/skills/autoskill/scripts/promote.py:14–32](../../../quarantine/scientific-agent-skills/skills/autoskill/scripts/promote.py#L14): Promotion moves a staged folder, refuses existing target; no evaluation/host activation mechanism.

### Evidence e7

[quarantine/scientific-agent-skills/skills/autoskill/scripts/backends.py:50–116](../../../quarantine/scientific-agent-skills/skills/autoskill/scripts/backends.py#L50): Single-request Claude/Foundry and OpenAI-compatible local LLM adapters; selection is not collaboration.

### Evidence e8

[quarantine/scientific-agent-skills/skills/autoskill/SKILL.md:28–51](../../../quarantine/scientific-agent-skills/skills/autoskill/SKILL.md#L28): User-triggered observation/draft workflow, local screenpipe, redacted summaries and opt-in remote backend.

### Evidence e9

[quarantine/scientific-agent-skills/skills/pydicom/scripts/uid_mapping_validator.py:57–143](../../../quarantine/scientific-agent-skills/skills/pydicom/scripts/uid_mapping_validator.py#L57): Bounded UID mapping checks syntax, uniqueness, standard UID protection and optional keyed derivation; aggregate result explicitly not deidentification proof.

### Evidence e10

[quarantine/scientific-agent-skills/tests/autoskill/test_e2e.py:146–199](../../../quarantine/scientific-agent-skills/tests/autoskill/test_e2e.py#L146): Offline fake services check specific cluster/report and staged draft outcomes; do not test improvement effectiveness.

### Evidence e11

[quarantine/scientific-agent-skills/tests/autoskill/test_promote.py:16–60](../../../quarantine/scientific-agent-skills/tests/autoskill/test_promote.py#L16): Literal folder move/content preservation/missing proposal oracles.

