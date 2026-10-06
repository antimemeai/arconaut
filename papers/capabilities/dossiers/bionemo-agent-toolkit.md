# bionemo-agent-toolkit

scientific service skills, generated catalog and concrete request/result preservation helpers

Role: scientific skill/API client and workflow toolkit. Runtime: Markdown, Python.

Pinned source: [https://github.com/NVIDIA-BioNeMo/bionemo-agent-toolkit](https://github.com/NVIDIA-BioNeMo/bionemo-agent-toolkit); revision/version `16a373c2070993b43d4ca6bbb9d794306bb767a5`.

Host-loadable skills and normal Python NIM HTTP clients; aggregate plugin generator and evaluation scaffolding.

Consumes NVIDIA hosted or independently deployed NIMs; prompts also describe Docker setup if service absent. Client does not implement NIM inference or host agent loop.

Inspection: source-skill discovery/sync/scanner, ProteinMPNN mode/input/HTTP/result/failure paths and exact-response/concurrent-output tests; local Harbor grader and pipeline guidance

Limits of this study: Other NIM adapters/catalog listed, not all manually traced; live GPU/hosted evals unavailable and not run.

## Actions

### discover_source_skills() / plugin_sync.py --check|--write

Surface: Executable catalog packaging.

Input: Source roots and skills.sh groupings

Result: Leaf name/path map; coverage/freshness diagnostics or rebuilt aggregate

Lifecycle: Offline packaging; signature/eval artifacts preserved by generator contract

Authority: Contributor/CI; grouping changes intentionally explicit

Evidence: [e1](#evidence-e1).

### design.py --pdb --mode --num-sequences --output-dir

Surface: Executable ProteinMPNN service client.

Input: PDB, parameters, endpoint mode, credential and new output directory

Result: request.json, response.raw/response.json, designed_sequences.fa and summary with native/design score alignment

Lifecycle: One synchronous POST; atomic directory reservation prevents duplicate output collisions; non200 incl202 fails, no async job handle

Authority: Client invokes existing selected NIM; hosted key/local noauth, prompt transfer consent

Evidence: [e2](#evidence-e2), [e3](#evidence-e3), [e4](#evidence-e4), [e5](#evidence-e5), [e6](#evidence-e6).

### design_results(result, expected_count)

Surface: Executable response normalization.

Input: MultiFASTA and score array/header fallback

Result: Validated design sequences/scores and native_count

Lifecycle: Pure bounded result interpretation; no invented missing scores

Authority: Callable client helper

Evidence: [e3](#evidence-e3).

### drug-discovery-pipeline

Surface: Authored host workflow/code examples.

Input: Target, starting scaffold, scoring objective and service mode/ports

Result: Generated molecules, docking and affinity prediction stages

Lifecycle: External service sequence; no autonomous scheduler or shared namespace guarantee

Authority: Host tools; operator-selected destinations

Evidence: [e7](#evidence-e7).

### scan_skills.py

Surface: Executable contributor scanner orchestration.

Input: Source skills, optional changed paths/base ref and report directory

Result: Per-skill JSON risk reports and aggregate exit/status summary

Lifecycle: Subprocess scan each selected skill; completes even after one failure

Authority: CI/contributor process authority; external SkillSpector composition

Evidence: [e13](#evidence-e13).

## Capabilities

### filesystem

**S — Files** (source): Catalog packaging and scientific helpers read/write explicit artifacts; not general host file tools.

Evidence: [e1](#evidence-e1), [e5](#evidence-e5).

### processes

**S — OS programs** (source): Scanner invokes external skillspector; skills describe Docker deployment, while existing URL is consumed without duplicate service start.

Evidence: [e13](#evidence-e13), [e2](#evidence-e2).

### code-actions

**S — Code actions** (source): Python clients/code examples compose scientific services and validate domain result shapes; host execution required.

Evidence: [e3](#evidence-e3), [e5](#evidence-e5), [e7](#evidence-e7).

### persistent-kernel

**— — Kernel** (inspection): Outside this scientific skill/API client and workflow toolkit role: the reference supplies scientific service skills, generated catalog and concrete request/result preservation helpers rather than an agent execution engine.

### standing-database

**— — Standing DB** (inspection): Outside this scientific skill/API client and workflow toolkit role: the reference supplies scientific service skills, generated catalog and concrete request/result preservation helpers rather than an agent execution engine.

### workflow-programming

**L — Workflows** (documentation): Pipeline is authored ordered workflow with code examples and helper primitives; no executable host-agent scheduler implementation.

Evidence: [e7](#evidence-e7).

### multi-model

**— — Models** (inspection): Outside this scientific skill/API client and workflow toolkit role: the reference supplies scientific service skills, generated catalog and concrete request/result preservation helpers rather than an agent execution engine.

### live-collaboration

**— — Peer chat** (inspection): Outside this scientific skill/API client and workflow toolkit role: the reference supplies scientific service skills, generated catalog and concrete request/result preservation helpers rather than an agent execution engine.

### concurrent-work

**S — Concurrency** (source): Atomic output directory reservation prevents two invocations issuing requests to the same run destination; independent runs remain caller-managed.

Evidence: [e5](#evidence-e5), [e11](#evidence-e11).

### steering-interrupt

**— — Steer/interrupt** (inspection): Outside this scientific skill/API client and workflow toolkit role: the reference supplies scientific service skills, generated catalog and concrete request/result preservation helpers rather than an agent execution engine.

### turn-redefinition

**— — Turn program** (inspection): Outside this scientific skill/API client and workflow toolkit role: the reference supplies scientific service skills, generated catalog and concrete request/result preservation helpers rather than an agent execution engine.

### compaction

**— — Compaction** (inspection): Outside this scientific skill/API client and workflow toolkit role: the reference supplies scientific service skills, generated catalog and concrete request/result preservation helpers rather than an agent execution engine.

### context-repair

**— — Repair** (inspection): Outside this scientific skill/API client and workflow toolkit role: the reference supplies scientific service skills, generated catalog and concrete request/result preservation helpers rather than an agent execution engine.

### original-audit

**L — Original audit** (source): Per-run request payload and exact HTTP response bytes (including error statuses) plus exact FASTA are retained; credentials excluded; connection failure/provider/model/host context not a core audit.

Evidence: [e5](#evidence-e5), [e6](#evidence-e6), [e10](#evidence-e10).

### audit-query

**— — Audit query** (inspection): Outside this scientific skill/API client and workflow toolkit role: the reference supplies scientific service skills, generated catalog and concrete request/result preservation helpers rather than an agent execution engine.

### hot-change

**— — Hot change** (inspection): Outside this scientific skill/API client and workflow toolkit role: the reference supplies scientific service skills, generated catalog and concrete request/result preservation helpers rather than an agent execution engine.

### rebuild-continuity

**— — Rebuild continuity** (inspection): Outside this scientific skill/API client and workflow toolkit role: the reference supplies scientific service skills, generated catalog and concrete request/result preservation helpers rather than an agent execution engine.

### remote-services

**S — Remote** (source): Explicit hosted/local existing-NIM client, parameters and response handling; respects independently managed configured endpoint. No remote reconnect/job lifecycle in inspected client.

Evidence: [e2](#evidence-e2), [e4](#evidence-e4), [e5](#evidence-e5).

### self-improvement

**— — Self-improve** (inspection): Outside this scientific skill/API client and workflow toolkit role: the reference supplies scientific service skills, generated catalog and concrete request/result preservation helpers rather than an agent execution engine.

### complaints

**— — Complaints** (inspection): Outside this scientific skill/API client and workflow toolkit role: the reference supplies scientific service skills, generated catalog and concrete request/result preservation helpers rather than an agent execution engine.

### authority

**D — Authority** (documentation): Skill frontmatter and explicit hosted transfer authorization/service-mode guidance; actual authority enforcement remains host responsibility.

Evidence: [e2](#evidence-e2).

### evaluation

**L — Evaluation** (source): Strong literal parser/request/failure/concurrency oracles. Hosted lift combines trajectory+LLM judgments; local grader source exists but README says unsupported, replay measures reproduction and shape, not experimental biological efficacy.

Evidence: [e8](#evidence-e8), [e9](#evidence-e9), [e10](#evidence-e10), [e11](#evidence-e11), [e12](#evidence-e12).

### time-order

**L — Time/order** (source): HTTP connect/read timeouts bound client waits; no monotonic total deadline, remote cancellation or restart identity exposed in this synchronous helper.

Evidence: [e5](#evidence-e5), [e6](#evidence-e6).

## Inspected test oracles

- [quarantine/bionemo-agent-toolkit/tests/test_proteinmpnn_design.py](../../../quarantine/bionemo-agent-toolkit/tests/test_proteinmpnn_design.py): FASTA native/design alignment, exact IO/failure, secret omission, atomic concurrency Oracle: Literal expected sequences/scores, rejection arrays, preserved bytes and single mocked request under a controlled race. Does not test NIM science. Read, **not executed**.
- [quarantine/bionemo-agent-toolkit/nim-skills/proteinmpnn-nim/evals/harbor/proteinmpnn-local-design/tests/grader.py](../../../quarantine/bionemo-agent-toolkit/nim-skills/proteinmpnn-nim/evals/harbor/proteinmpnn-local-design/tests/grader.py): Shipped local grader source; README support remains TODO Oracle: Shape/parameter/endpoint substring and seeded replay; partial credit possible. Reproduction is not design efficacy, and trajectory string presence is weak execution evidence. Not run. Read, **not executed**.

## Useful mechanisms

- Request/result artifacts preserve exact response bytes and score identities; failures do not fabricate completion.
- Atomic new-run directory plus direct literal/concurrent tests address meaningful client fault classes.

## Material limits

- Client timeout does not prove server quiescence, and202 is rejected rather than tracked.
- Scientific inference quality and supported native local-agent evaluations are not established by helper tests or replay.

## Arconaut design questions

- How can a consumer expose exact service IO, remote identity and quiescence independently of the scientific service governor?
- Which domain transformations need literal identity/alignment oracles, and which require independent biological evaluation?

## Evidence

### Evidence e1

[quarantine/bionemo-agent-toolkit/scripts/plugin_sync.py:1–93](../../../quarantine/bionemo-agent-toolkit/scripts/plugin_sync.py#L1): Generated payload coverage/freshness contract; recursively discover leaf skills, reject duplicate names, exclude vendor/evals.

### Evidence e2

[quarantine/bionemo-agent-toolkit/nim-skills/proteinmpnn-nim/SKILL.md:1–75](../../../quarantine/bionemo-agent-toolkit/nim-skills/proteinmpnn-nim/SKILL.md#L1): Host tool frontmatter, hosted/local authority/data transfer and existing service consumer boundary.

### Evidence e3

[quarantine/bionemo-agent-toolkit/nim-skills/proteinmpnn-nim/scripts/design.py:24–98](../../../quarantine/bionemo-agent-toolkit/nim-skills/proteinmpnn-nim/scripts/design.py#L24): FASTA/result parser checks finite scores, design/native separation and rejects count/score mismatches.

### Evidence e4

[quarantine/bionemo-agent-toolkit/nim-skills/proteinmpnn-nim/scripts/design.py:101–131](../../../quarantine/bionemo-agent-toolkit/nim-skills/proteinmpnn-nim/scripts/design.py#L101): Explicit mode/key/baseURL and numeric/PDB validation before request.

### Evidence e5

[quarantine/bionemo-agent-toolkit/nim-skills/proteinmpnn-nim/scripts/design.py:142–173](../../../quarantine/bionemo-agent-toolkit/nim-skills/proteinmpnn-nim/scripts/design.py#L142): Atomic output reservation, request JSON and exact HTTP response bytes; only200 produces valid complete summary and exact FASTA.

### Evidence e6

[quarantine/bionemo-agent-toolkit/nim-skills/proteinmpnn-nim/scripts/design.py:176–198](../../../quarantine/bionemo-agent-toolkit/nim-skills/proteinmpnn-nim/scripts/design.py#L176): CLI options and separate request/failure exit statuses; read timeout is not server cancellation.

### Evidence e7

[quarantine/bionemo-agent-toolkit/nim-skills/meta-skills/drug-discovery-pipeline/SKILL.md:20–50](../../../quarantine/bionemo-agent-toolkit/nim-skills/meta-skills/drug-discovery-pipeline/SKILL.md#L20): Prompt workflow composes GenMol→DiffDock→Boltz2; port/lifetime assumptions explicitly distinguished.

### Evidence e8

[quarantine/bionemo-agent-toolkit/README.md:62–88](../../../quarantine/bionemo-agent-toolkit/README.md#L62): Hosted skill-lift evaluation with external trajectory+LLM grader; local tasks described TODO/unsupported and some timeouts explicitly pending.

### Evidence e9

[quarantine/bionemo-agent-toolkit/tests/test_proteinmpnn_design.py:42–89](../../../quarantine/bionemo-agent-toolkit/tests/test_proteinmpnn_design.py#L42): Literal score/native/chain validation and malformed result rejection.

### Evidence e10

[quarantine/bionemo-agent-toolkit/tests/test_proteinmpnn_design.py:104–148](../../../quarantine/bionemo-agent-toolkit/tests/test_proteinmpnn_design.py#L104): Mock HTTP exact request/result/failure bytes and secret omission oracles.

### Evidence e11

[quarantine/bionemo-agent-toolkit/tests/test_proteinmpnn_design.py:169–201](../../../quarantine/bionemo-agent-toolkit/tests/test_proteinmpnn_design.py#L169): Directory rejection and controlled concurrent reservation test only one request.

### Evidence e12

[quarantine/bionemo-agent-toolkit/nim-skills/proteinmpnn-nim/evals/harbor/proteinmpnn-local-design/tests/grader.py:102–212](../../../quarantine/bionemo-agent-toolkit/nim-skills/proteinmpnn-nim/evals/harbor/proteinmpnn-local-design/tests/grader.py#L102): Shipped grader checks artifact/request/response shape, trajectory endpoint strings and seeded local replay with partial credit; not scientific design efficacy.

### Evidence e13

[quarantine/bionemo-agent-toolkit/scripts/scan_skills.py:36–95](../../../quarantine/bionemo-agent-toolkit/scripts/scan_skills.py#L36): Scanner finishes selected skills and aggregates risk/suppression/completeness reports; risk threshold is not scientific validity.

