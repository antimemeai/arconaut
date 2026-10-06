# Agent discovery frumentarius — 2026-09-30

Acquisition complete: 46 unique references, including 28 agent implementations/harnesses, 17 runtime/control/integration/evaluation support references and one produced review artifact. This source corpus informs the Arconaut execution-model discussion; it adopts no implementation, library or dependency.

## Written acquisition sub-plan

Compare canonical origins against the 53 legacy references, sixteen fresh sources and known remote-studied sources; search official upstream repositories and the ACP agent list for practical coding agents, programmable workflows, live collaboration, persistent computation and scientific agents. Resolve exact upstream revisions, preserve intact source archives, ingest through the owned ZIP importer with symbolic links and nested Git/filesystem detritus omitted, and directly compare retained file sets, bytes and executable bits against the ZIP. Existing snapshots and original source archives remain unchanged. Never execute imported code, installers or automation. Root integrates the project journal, manifests and beads.

Known-agent acquisition proceeds in a separate lane. The science-workbench lane owns Claude Science and hosted/proprietary documentation; this lane owns public scientific implementations and execution/evaluation support sources, distinctly categorized.

## Search coverage

Primary upstream repositories plus [ACP’s agent list](https://agentclientprotocol.com/get-started/agents) supplied the discovery path. Search covers terminal/IDE agents, native persistent-agent runtimes, Python code-action harnesses, multi-agent coding communication/orchestration and scientific workbenches. This is a substantial finite acquisition pass, not a claim to exhaust every agent or fork on the internet. Canonical-origin checks avoid treating renamed repositories as additional paradigms. Imported instructions remain historical material.

## Interim findings

- [Bub](https://github.com/bubbuild/bub) exposes every turn stage as an overridable hook and reconstructs context from append-only tape; it directly bears on turn programmability and audit/context separation.
- [Hermes](https://github.com/NousResearch/hermes-agent/blob/main/website/docs/user-guide/features/code-execution.md) documents persistent Python execution state and programmable tool calls.
- [gptme](https://github.com/gptme/gptme) supplies persistent autonomous workspaces, cross-harness memory and shell/Python/computer tools.
- [Crush](https://github.com/charmbracelet/crush) currently documents Bash-programmed configuration and shared backend sessions.
- [Kaagum](https://git.systemreboot.net/kaagum/about/) is a small Guile agent with ACP; its strong sandbox policy is a comparison point, not Arconaut policy adoption.
- [Station](https://github.com/dualverse-ai/station) provides autonomous scientific participants and shared literature; [Finch](https://github.com/Future-House/finch) is a notebook data-science agent.

The current Open Interpreter Rust lineage derives from Codex; its original Python lineage is now maintained separately. They will be labeled as related lineages rather than claimed as independent origins. Directly opening Antinomy Forge resolves its canonical rename to `tailcallhq/forgecode`; the initial organization listing alone did not establish availability. The renamed public source is included separately from unrelated Forge products. Root’s science-workbench lane also located the primary author’s Claude Science evidence workflow, `AllenNeuralDynamics/ComputationalReviewTemplate`, and its produced `ComputationalReviewVIP`; this source is included, while output artifacts are explicitly distinguished from harness source.

## Source boundaries and corrected leads

[Open Interpreter](https://github.com/openinterpreter/openinterpreter) currently derives from Codex and offers selectable model-specific harness emulation. [Its original Python successor](https://github.com/endolith/open-interpreter) is a related lineage, not a second independent origin. [AgentPool](https://github.com/phil65/agentpool) is the canonical successor of `llmling-agent`; [Forgecode](https://github.com/tailcallhq/forgecode) is the canonical successor of `antinomyhq/forge`.

[AgentRxiv’s author site](https://agentrxiv.github.io/) links Code to AgentLaboratory, whose README explicitly includes AgentRxiv support. The speculative standalone `SamuelSchmidgall/AgentRxiv` candidate is rejected as a duplicate/incorrect-origin lead; no separate source is claimed.

[Coscientist](https://github.com/gomesgroup/coscientist) supplies paper supporting data and a `simple_implementation`, not a complete current scientific workbench. [AutoHarness](https://github.com/aiming-lab/AutoHarness) currently exposes configurable governance/tool pipelines and an agent loop; its source description does not substantiate the initial model-generated-harness hypothesis. Its rationale is corrected to programmable context/control/audit comparison. Neither restrictive governance nor claimed performance is adopted as Arconaut policy.

The Claude Science custom [ComputationalReviewTemplate](https://github.com/AllenNeuralDynamics/ComputationalReviewTemplate) consists of workflow skills, schemas, evidence/provenance and rendering machinery. Its `evidence/README.md` specifies per-section JSON evidence packages, which the explorer consumes; the optional combined `evidence_database.json` is not read by that widget. [ComputationalReviewVIP](https://github.com/AllenNeuralDynamics/ComputationalReviewVIP) is produced review material with thirteen per-section evidence JSON packages. These directly expose the authored persistent-evidence workflow, while the Anthropic harness and its underlying database/kernel machinery remain separate. Root’s science-workbench lane acquired the proprietary product’s official public documentation/examples/distribution.

[OpenAI4S](https://github.com/PKU-YuanGroup/OpenAI4S) is a separate public implementation inspired by Claude Science. Its README describes native JSON orchestration alongside persistent Python/R kernels, Python-to-host RPC and an append-only action ledger. It is useful inspectable machinery, not Anthropic source. NVIDIA BioNeMo toolkit and K-Dense scientific skills are separately labeled scientific integration/skills support, not stand-alone harnesses.

## Acquisition results

All 46 selected unique references acquired successfully. The preserved ZIPs contain 1,302,288,259 bytes; the clean snapshots retain 131,391 regular files and 2,734,388,987 bytes. Every retained file set, byte stream and executable bit was compared directly to its source ZIP before publishing the snapshot. No imported program, installer, provider API or automation ran. Existing references were left unchanged.

Exact full revisions, download URLs, SHA-256 identities, archive roots, destinations and omission lists are in [the acquisition catalog](2026-09-30-discovered-agent-acquisition.json). Source ZIPs remain intact in `../quarantine_proj/archives/arconaut-agent-discovery-2026-09-30/`. Git metadata and filesystem detritus are omitted from clean snapshots; symbolic links are omitted explicitly and remain in the ZIP. Kaagum’s ZIP was generated with `git archive` from the official upstream at `3d286e6896a64d2857b947500a485a36393724c2`; its temporary bare clone was removed.

### Agent implementations and harnesses

| Reference | Category | Revision prefix | Files | Concrete study use |
| --- | --- | --- | ---: | --- |
| [agent-laboratory](https://github.com/SamuelSchmidgall/AgentLaboratory) | scientific-agent | `d9017d90e329` | 35 | End-to-end research workflow with human researcher interaction |
| [agent-zero](https://github.com/agent0ai/agent-zero) | general-agent | `e3051fb584b1` | 3,081 | Explicit editable prompts/tools, subordinate agents and Linux workbench |
| [ai-scientist-v2](https://github.com/SakanaAI/AI-Scientist-v2) | scientific-agent | `96bd51617cfd` | 68 | Autonomous experiment manager and progressive tree search over research code |
| [aider](https://github.com/Aider-AI/aider) | coding-agent | `5dc9490bb35f` | 685 | Repository-map and edit-format pair-programmer |
| [coscientist](https://github.com/gomesgroup/coscientist) | scientific-agent | `417e82b85743` | 14 | Paper supporting data plus a simple Coscientist implementation; historical scientific planner/tool comparison, not a complete current workbench |
| [crush](https://github.com/charmbracelet/crush) | coding-agent | `76cc5c574e15` | 1,218 | Executable Bash configuration, shared backend sessions and model switching |
| [deepagents](https://github.com/langchain-ai/deepagents) | coding-agent-harness | `dd0e2f5366da` | 1,773 | Programmable filesystem/planning/subagent harness and CLI |
| [finch](https://github.com/Future-House/finch) | scientific-agent | `aea66fdf2dd2` | 63 | Jupyter notebook data-science agent and observable computational state |
| [forge-adulari](https://github.com/Adulari/forge) | coding-agent | `263b45d60548` | 1,780 | Rust multi-provider routing and common session/audit surfaces |
| [forgecode](https://github.com/tailcallhq/forgecode) | coding-agent | `571a28902b9c` | 936 | Canonical successor of antinomyhq/forge; native CLI, Zsh integration and YAML-customized agent behavior |
| [gptme](https://github.com/gptme/gptme) | coding-agent | `1f8d73638a25` | 1,763 | Persistent autonomous workspaces, Python/shell tools and cross-harness memory |
| [hermes-agent](https://github.com/NousResearch/hermes-agent) | general-agent | `cfdcea4f2226` | 16,891 | Persistent Python tools, learned skills and background/channel operation |
| [ironclaw](https://github.com/nearai/ironclaw) | general-agent | `b0b999d96781` | 4,942 | Native self-expanding agent with model-built WASM tools and reloadable plugins |
| [kaagum](https://git.systemreboot.net/kaagum) | coding-agent | `3d286e6896a6` | 27 | Small Guile implementation of an ACP coding agent, contrasting native dynamic execution and controlled filesystem tools |
| [mistral-vibe](https://github.com/mistralai/mistral-vibe) | coding-agent | `7c19608af06f` | 3,880 | Provider-native Python CLI coding-agent execution model |
| [nanobot](https://github.com/HKUDS/nanobot) | general-agent | `d3d70f20655b` | 1,621 | Small Python persistent-agent implementation useful against larger native agents |
| [nullclaw](https://github.com/nullclaw/nullclaw) | general-agent | `55907af88e51` | 398 | Zig small native agent with explicit vtable extensibility |
| [open-interpreter](https://github.com/openinterpreter/openinterpreter) | coding-agent | `9acdb7074002` | 8,681 | Current Rust Codex lineage with selectable provider-specific harness emulation |
| [open-interpreter-python](https://github.com/endolith/open-interpreter) | general-agent | `e77c93612380` | 384 | Original Python interpreter lineage preserved as community successor |
| [open-swe](https://github.com/langchain-ai/open-swe) | coding-agent | `9613f663bbff` | 1,907 | Asynchronous coding-agent orchestration and external issue/chat integration |
| [openai4s](https://github.com/PKU-YuanGroup/OpenAI4S) | scientific-agent | `9f20ef8d89c1` | 4,092 | Independent Claude Science-inspired Python/R code-action agent with persistent kernels, host RPC, action ledger, managed compaction and delegated scientific work |
| [openclaw](https://github.com/openclaw/openclaw) | general-agent | `646c42b4c630` | 50,936 | Persistent multi-channel agent and broad local tool/skill integration |
| [paper-qa](https://github.com/Future-House/paper-qa) | scientific-agent | `57e89f7223b0` | 182 | Agentic query/evidence generation over local scientific document corpus |
| [robin](https://github.com/Future-House/robin) | scientific-agent | `4a5cce310f3b` | 888 | Scientific multi-agent hypothesis/experiment pipeline; hosted Edison dependency remains external |
| [stakpak-agent](https://github.com/stakpak/agent) | coding-agent | `760cd2b5984d` | 491 | Rust coding/DevOps agent with continuous background operation |
| [station](https://github.com/dualverse-ai/station) | scientific-collaboration | `782088e56199` | 511 | Open scientific multi-agent environment with shared literature and scored research |
| [vtcode](https://github.com/vinhnx/VTCode) | coding-agent | `fc68c9e1f454` | 3,295 | Native Rust terminal agent with explicit core/runtime implementation |
| [zeroclaw](https://github.com/zeroclaw-labs/zeroclaw) | general-agent | `83f0ff3805ad` | 2,166 | Rust native persistent-agent runtime contrasted with Zig reimplementation |

### Runtime, collaboration, science support and produced artifacts

| Reference | Category | Revision prefix | Files | Concrete study use |
| --- | --- | --- | ---: | --- |
| [agent-orchestrator-rust](https://github.com/c9r-io/orchestrator) | collaboration-control | `c4098a7d14d5` | 1,601 | Rust daemon with declarative durable coding-agent workflows |
| [agentchat](https://github.com/tjamescouch/agentchat) | collaboration-service | `f845e472a95f` | 203 | Real-time cross-harness agent communication |
| [agentpool](https://github.com/phil65/agentpool) | collaboration-control | `b6ddbea9cb66` | 1,062 | YAML orchestration of native and external ACP agents; formerly llmling-agent |
| [allen-openai-tools](https://github.com/AllenInstitute/openai_tools) | scientific-integration-support | `161dbb400441` | 56 | Primary author-linked earlier scientific tool/workflow integration reference |
| [auto-harness](https://github.com/aiming-lab/AutoHarness) | agent-runtime | `3561e468f9ca` | 240 | Configurable governance/tool pipelines and agent loop; programmable context/control/audit comparison, no model-generated-harness claim |
| [aviary](https://github.com/Future-House/aviary) | evaluation-support | `7167342915e6` | 111 | Scientific agent environments with explicit state and tool observation contracts |
| [bionemo-agent-toolkit](https://github.com/NVIDIA-BioNeMo/bionemo-agent-toolkit) | scientific-integration-support | `16a373c20709` | 605 | Official Claude Science-linked model skills and scientific workflow integration; not harness source |
| [bub](https://github.com/bubbuild/bub) | agent-runtime | `0fe2fd51ffd8` | 264 | Every turn stage replaceable; append-only context tape in shared conversations |
| [computational-review-template](https://github.com/AllenNeuralDynamics/ComputationalReviewTemplate) | scientific-workflow-support | `7312d15c1244` | 55 | Author-linked Claude Science evidence/provenance database workflow with actor/critic phases and skills; directly relevant operator example |
| [computational-review-vip](https://github.com/AllenNeuralDynamics/ComputationalReviewVIP) | scientific-workflow-artifact | `a04f01d37994` | 159 | Produced scientific review from the Claude Science workflow; observe retained evidence/provenance rather than confusing output with harness source |
| [docker-agent](https://github.com/docker/docker-agent) | agent-runtime | `83fca5b2577d` | 3,103 | Declarative agent teams, execution/toolsets and container-oriented runtime |
| [fast-agent](https://github.com/evalstate/fast-agent) | agent-runtime | `5e3ad9a78d9e` | 2,347 | Coding/workflow toolkit with model, tools, hooks and compaction configuration |
| [forge-workflow](https://github.com/ForgeAILab/forge) | collaboration-control | `cf652b1aa1c5` | 2,545 | Native control plane for durable assistants and isolated task-scoped coding agents |
| [ldp](https://github.com/Future-House/ldp) | evaluation-support | `7220ca1e0629` | 241 | Modular agent/environment/optimizer interchange and evaluation machinery |
| [mcp-agent-mail](https://github.com/Dicklesworthstone/mcp_agent_mail) | collaboration-service | `3fad5ec67286` | 299 | Durable identities/messages and advisory file reservation for coding agents |
| [ntm](https://github.com/Dicklesworthstone/ntm) | collaboration-control | `ee589e7e7f95` | 2,966 | Multi-CLI tmux orchestration, durable work coordination and robot interface |
| [scientific-agent-skills](https://github.com/K-Dense-AI/scientific-agent-skills) | scientific-skills-support | `91497e335489` | 2,641 | Scientific agent tools and operating procedures; formerly claude-scientific-skills, not Anthropic Science source |
| [smolagents](https://github.com/huggingface/smolagents) | agent-runtime | `c30b115286e0` | 185 | Python CodeAgent actions and alternative interpreter/executor backends |

## Availability, coverage and next study

No selected unique source remains unavailable. The attempted standalone AgentRxiv URL failed with Git exit 128, `could not read Username for https://github.com: terminal prompts disabled`; its actual primary author code link is AgentLaboratory, already acquired. The catalog retains the failed attempted URL and its corrected disposition under `rejected_leads`, without adding it to the acquired-reference count. The main acquisition CLI therefore exited 1 before this canonical duplicate was reconciled; every final selected record has status `acquired`.

The finite pass favors useful, materially different implementations and explicit related lineages, avoiding repetitive forks, empty wrapper repositories and unrelated generic frameworks. The current ACP list also advertises hosted/commercial agents; acquiring public metadata does not provide their proprietary implementation. Claude Science’s official docs/examples/distribution and the directly linked K-Dense BYOK follow-up belong to the separate [science-workbench acquisition](2026-09-30-science-workbench-acquisition.md), not this source count. Additional adjacent agents remain possible future corpus expansion; this pass does not claim the entire internet is exhausted.

GitHub source exports do not recover private services, external dependencies, all submodule contents, every Git LFS object or live process state. `fast-agent` and `zeroclaw` contain `.gitmodules`, identified in the catalog. Published capabilities and performance remain claims until direct source study and appropriate experiments. No library or architecture was selected by acquisition.

The execution-model discussion can now compare ordinary coding loops with replaceable turn stages (Bub, fast-agent, DeepAgents), code-action/persistent computation (OpenAI4S, Finch, Hermes, smolagents), live communication and durable collaboration (AgentChat, MCP Agent Mail, NTM, AgentPool), and longitudinal evidence/research workflows (the Claude Science template/output, PaperQA, Station). Arconaut’s own consumer boundary and agreed refit rules remain the governing product intent.

## Restoration

From the Arconaut repository root, obtain the source ZIPs with the catalog’s identities and require the destination snapshots to be absent. Each record supplies the importer arguments; the following uses the preserved relative archive paths:

```sh
python3 - <<'RESTORE_DISCOVERED_REFERENCES'
from pathlib import Path
import hashlib, json, subprocess
catalog = json.loads(Path('papers/2026-09-30-discovered-agent-acquisition.json').read_text())
for source in catalog['references']:
    archive = Path(source['archive'])
    digest = hashlib.sha256()
    with archive.open('rb') as handle:
        for chunk in iter(lambda: handle.read(1024 * 1024), b''):
            digest.update(chunk)
    if digest.hexdigest() != source['sha256']:
        raise SystemExit(f'Archive identity mismatch: {archive}')
    subprocess.run(['python3', 'scripts/ingest_zip.py', str(archive),
                    source['destination'], '--root', source['archive_root'],
                    '--skip-symlinks'], check=True)
RESTORE_DISCOVERED_REFERENCES
```

If a GitHub archive must be reacquired, use the record’s exact `download_url`; the original preserved ZIP remains the artifact with the recorded SHA-256 identity. To regenerate Kaagum’s source export, clone `https://git.systemreboot.net/kaagum` into a temporary bare repository, fetch the recorded revision if needed, and use its recorded `archive_generation_argv` with real temporary clone/output paths. Remove that temporary Git metadata after archiving. Imported instructions, installers and automation are historical reference, never current workspace instructions.
