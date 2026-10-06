# LLM Memory Systems & Long-Context Augmentation: Landscape Report
## For the Antimeme Project Family — CLI Agent Memory Architecture

**Date:** 2026-06-07  
**Scope:** Agent memory architectures (2023–2026), model-as-memory, session persistence, hierarchical memory, code/research task memory, empirical benchmarks.  
**Sources:** arXiv literature, open-source implementations, production framework documentation, local library (`../neurotic_library/lib/`).  

---

## Executive Summary

The field of LLM agent memory has matured from simple vector-RAG appendages into a rich ecosystem of cognitive architectures. For CLI-based agents like Kimi and Claude Code— which start each session with zero context—memory is the single biggest lever for long-horizon productivity.  

**Bottom line:** The most practical near-term path for antimeme-style CLI agents is a **tiered, file-first memory architecture** (human-readable markdown + git versioning) augmented by a **local hybrid-retrieval engine** (SQLite + vector + full-text) exposed through MCP. This gives cross-session persistence with zero cloud dependencies, low latency, and full human auditability. More exotic approaches—model-as-memory, temporal knowledge graphs, or RL-driven memory policies—are promising but should be treated as Phase 2+ enhancements.

---

## (a) Taxonomy of Memory Approaches

### 1. Parametric vs. Non-Parametric

| Paradigm | What it is | Examples | Trade-off |
|----------|-----------|----------|-----------|
| **Parametric** | Knowledge encoded in model weights or learnable memory tokens | MemoryLLM, M+, MemAgent, MemGen | Fast retrieval, no external infra; limited capacity, hard to update, requires fine-tuning |
| **Non-parametric** | External stores (vector DB, KG, files) retrieved into context | Mem0, Zep, Letta, HippoRAG, vstash | Scalable, auditable, easy to update; adds latency and operational complexity |
| **Hybrid** | Learnable retrieval policies over external stores; or latent + external | MemoRAG, MemTier, MemVerse | Best of both worlds; highest implementation complexity |

### 2. By Storage Architecture

| Architecture | Mental Model | Representative Systems | Strengths | Weaknesses |
|--------------|--------------|------------------------|-----------|------------|
| **Flat Vector + Extraction** | Structured notes with a vector index for fuzzy lookup | Mem0, Memobase, LightMem | Simple API, fast lookup, low token overhead | Weak relational reasoning; chronology is metadata, not first-class |
| **Tiered Context / OS Paging** | LLM manages its own memory via tool calls (core ↔ archival ↔ recall) | Letta (MemGPT), MemOS, MemoryOS | Agent feels genuinely stateful; self-editing | Opinionated runtime; core-memory budget is a hard constraint |
| **Temporal Knowledge Graph** | Every fact is a chronologically-scoped edge in a graph | Zep (Graphiti), Hindsight, AriGraph | Chronological correctness; multi-hop reasoning | Graph backend operational overhead; extraction burns LLM credits |
| **Hierarchical Compression** | Multi-stage consolidation (episodes → facts → patterns) | SimpleMem, MELODI, Agent Context | High compression ratios (5–9×); bounded growth | Lossy; retrieval quality depends on consolidation quality |
| **Local-First Hybrid Retrieval** | SQLite + vector + FTS5, zero cloud deps | vstash, engram, codemem, palinode | Privacy, zero infra, offline | Single-user; scaling past ~100K docs requires tuning |
| **Model-as-Memory** | Fixed-size latent memory pool inside the transformer | MemoryLLM, M+ | No external DB; updates in one forward pass | Requires custom model weights; capacity bounded by memory pool size |
| **File-Based Convention** | Human-readable markdown files loaded into context | CLAUDE.md, AGENTS.md, MEMORY.md, agent-context | Zero code, git-tracked, cross-agent portable | No semantic search unless augmented; manual curation |

### 3. By Cognitive Type (CoALA Framework)

The **CoALA** taxonomy (Sumers et al., Princeton, TMLR 2024) has become the canonical reference:

| Memory Type | Stores | AI Analog | Typical Implementation |
|-------------|--------|-----------|------------------------|
| **Working** | Active context for current decision cycle | Context window + scratchpad | In-context messages, CoT reasoning, tool outputs |
| **Episodic** | Records of past events/interactions | Conversation logs, session histories | Append-only logs, retrieval-augmented conversation history |
| **Semantic** | Facts, definitions, accumulated knowledge | User preferences, project conventions | Vector DB of extracted facts, knowledge graphs, CLAUDE.md |
| **Procedural** | Skills, rules, behavioral instructions | How to run tests, deploy, review code | System prompts, tool definitions, skill libraries (Voyager-style) |

A fifth type, **organizational context memory** (governed definitions, lineage), is emerging in enterprise settings but is less relevant for CLI research agents.

---

## (b) What’s Practical for CLI Agents vs. Web-Service Agents

### Web-Service Agents (ChatGPT, enterprise assistants)
- **Constraints:** High scale, multi-tenancy, sub-200ms latency budgets, cloud-native.
- **Typical choice:** Mem0 (YC-backed, AWS Agent SDK memory provider) or Zep/Graphiti for temporal reasoning. These run as managed services with vector stores (Qdrant, Pinecone, Neo4j) and LLM-based extraction pipelines.
- **Trade-off:** Cloud dependency, cost per LLM extraction call, data residency concerns.

### CLI Agents (Kimi, Claude Code, Codex CLI, local LLMs)
- **Constraints:** Stateless by default; session ends → context vaporizes. No background server unless user runs one. Preference for local-first, zero-API-key operation. Must work across multiple agent brands.
- **Current state-of-the-art practice:**
  1. **File-based persistent context:** `CLAUDE.md` / `AGENTS.md` / `MEMORY.md` — loaded at session start. Agent-agnostic, human-editable, version-controlled.
  2. **MCP-based local memory servers:** `engram` (Go + SQLite + FTS5), `codemem` (Rust + graph-vector hybrid), `vstash` (Python + sqlite-vec + FTS5 + adaptive RRF). These expose `mem_save`, `mem_search`, `mem_consolidate` as MCP tools the agent can call.
  3. **Hybrid:** File-based conventions for procedural/semantic memory + local vector DB for episodic retrieval.

### Key Insight for CLI Agents
> "Tool complexity matters less than reliable retrieval." — Letta’s filesystem agents scored **74% on LoCoMo** using basic file operations, beating Mem0’s specialized tools at **68.5%** (under matched conditions). For CLI agents, **start simple, add structure only when retrieval fails.**

---

## (c) Empirical Results on Memory Effectiveness

### Canonical Benchmarks

| Benchmark | What it tests | Scale |
|-----------|--------------|-------|
| **LoCoMo** (Maharana et al., ACL 2024) | Very long-term conversational memory (300-turn, ~9K tokens) | 1,540 QA pairs across 10 conversations |
| **LongMemEval / LongMemEval-S** (Wu et al., ICLR 2025) | Cross-session memory: extraction, multi-session synthesis, temporal reasoning, knowledge updates, abstention | 500 questions across ~53 sessions; 100K+ token avg context |
| **BEIR** | Zero-shot information retrieval across domains | Multiple datasets (SciFact, NFCorpus, FiQA, etc.) |
| **SWE-bench Verified** | Real-world software engineering task completion | 500+ GitHub issues |

### Head-to-Head Results (LoCoMo — LLM-as-Judge Accuracy %)

| System | Architecture | Overall | Multi-Hop | Temporal | Notes |
|--------|-------------|---------|-----------|----------|-------|
| **BYTEROVER** | Custom context tree | **96.1** | **93.3** | **97.8** | Best published result |
| **MemMachine** | Ground-truth-preserving retrieval | **91.7** | — | — | Leading open framework result |
| **True Memory Pro** | SQLite-only, no vector/GPU | **93.0** | — | — | 6-layer retrieval pipeline |
| **HonCho** | Commercial (?) | 89.9 | 84.0 | 88.2 | — |
| **Hindsight** | Graph-native cognitive memory | 89.6 | 70.8 | 83.8 | Four-network architecture |
| **MAGMA** | Multi-graph agentic memory | 70.0 | 52.8 | 65.0 | Strong adversarial robustness |
| **Nemori** | Predict-calibrate graph | 59.0 | 56.9 | 64.9 | — |
| **Zep** | Temporal KG | 75.1 | 66.0 | 79.8 | Strong temporal reasoning |
| **Memobase** | Structured fact extraction | 75.8 | 46.9 | 85.1 | — |
| **Mem0** | Vector + entity graph | 66.9 | 51.2 | 55.5 | Fast, production-grade |
| **A-MEM** | Zettelkasten-inspired notes | 58.0 | 49.5 | 47.4 | Self-evolving links |
| **Letta / MemGPT** | OS-style tiered memory | 74.0 | — | — | Filesystem agent variant |
| **OpenAI Memory** | Commercial embedding | 52.9 | 42.9 | 21.7 | Baseline |
| **Full Context** | No memory system | 48.1 | 46.8 | 56.2 | Lost-in-the-middle penalty |

### LongMemEval-S Results (Accuracy %)

| System | Overall | Key Strength |
|--------|---------|--------------|
| **BYTEROVER** | **92.8** | Cross-session synthesis |
| **Chronos-High** | 95.6 | Claude Opus 4.6 backbone (stronger model) |
| **Hindsight** | 87.2 | — |
| **True Memory Pro** | **87.8** | CPU-only, SQLite |
| **Zep** | ~58–71 | Temporal queries |
| **Mem0** | ~68–93 | Vendor-reported numbers vary by config |
| **Full Context** | ~44–78 | Degrades with scale |

### Efficiency Metrics

| System | Ingestion Rate | Search Latency | Token Cost vs. Full-Context |
|--------|---------------|----------------|----------------------------|
| Mem0 | 1.28 items/sec | ~18 ms | ~91% p95 latency reduction |
| MemVerse | 0.22 items/sec | — | 3 LLM calls per item |
| SimpleMem | — | — | ~45% less tokens than Mem0 on LoCoMo |
| Zep | — | **<200 ms** | 90% latency reduction vs. vector-only |
| vstash | — | **~21–73 ms** @ 50K chunks | Zero LLM calls for storage |
| engram | — | FTS5-speed | Zero LLM calls |

### Domain-Specific: Code Agents

| System / Approach | Benchmark | Result |
|-------------------|-----------|--------|
| **Reflexion** (verbal reinforcement) | HumanEval | 91% pass@1 vs. 80% GPT-4 baseline (+11 points) |
| **Git-Context-Controller (GCC)** | SWE-Bench-Lite | **48%** bug resolution (SOTA tier); 79% file-level localization |
| **MemCoder** | SWE-Bench Verified | SOTA; DeepSeek-V3.2 boosted from 68.4% → **77.8%** with human-commit memory |
| **ExpeRepair** (dual-memory program repair) | Repo-level repair | Demonstrates episodic + semantic memory split for SE tasks |
| **Agent Context** (file-based) | N/A (qualitative) | Claims **9:1 compression** of daily logs → topic files |

---

## (d) Recommendations for Antimeme Architecture

### Design Principles
1. **File-first, retrieval-second.** Human-readable, git-tracked files are the CLI agent’s native memory format. Treat them as the source of truth.
2. **Agent-agnosticism.** Avoid lock-in to a single agent runtime (Letta owns the loop; Mem0 is cloud-first). Use MCP or file conventions that Kimi, Claude Code, and local LLMs can all consume.
3. **Deterministic state ≠ episodic memory.** Code, configs, and project structure are already persistent. Don’t duplicate them into a memory system. Instead, memory should capture *discoveries, decisions, failures, and conventions* that aren’t in the codebase.
4. **Retrieval failure is the default.** Design for empty results: always have a fallback (broader search, human prompt, or full-context scan).
5. **Garbage in, garbage out.** Automated memory extraction accumulates noise. Prefer human-curated or agent-validated writes. Use consolidation and decay to prevent unbounded growth.

### Proposed Phased Architecture

#### Phase 0: Immediate (Zero Infrastructure)
- **Procedural + Semantic memory:** Maintain `AGENTS.md` / `CLAUDE.md` at project root and `~/.claude/CLAUDE.md` for user preferences.
- **Episodic memory:** Append-only `memory/YYYY-MM-DD.md` daily logs written by the agent at session end.
- **Consolidation:** Weekly manual or scripted merge of daily logs into `memory/<topic>.md` files.
- **Rationale:** Works today with Kimi and Claude Code. Zero latency. Fully version-controlled.

#### Phase 1: Structured Local Retrieval (Low Complexity)
- Add a **local hybrid-retrieval layer** via MCP:
  - **Option A:** `engram` (Go binary, SQLite + FTS5, 19 MCP tools, git-sync, TUI). Best for coding-agent workflow with branch awareness.
  - **Option B:** `vstash` (Python, sqlite-vec + FTS5 + adaptive RRF, self-supervised embedding tuning). Best for document-heavy research tasks.
  - **Option C:** `codemem` (Rust, graph-vector hybrid, single binary, MCP). Best if you want knowledge-graph relationships for code.
- Use the memory server for **episodic retrieval** ("What did we decide about X last week?") while keeping `AGENTS.md` for **procedural/semantic** constants.

#### Phase 2: Hierarchical Memory (Medium Complexity)
- Implement a **three-tier hierarchy** inspired by Letta but file-backed:
  1. **Working memory:** Current context window + scratchpad.
  2. **Session memory:** Summarized facts from the current session (auto-extracted via lightweight LLM calls at session end).
  3. **Long-term memory:** Consolidated topic files + local vector/KG index.
- Add **consolidation triggers:** Time-based (≥24h) + event-based (≥N new memories).
- Add **temporal validity:** Tag memories with `valid_from` / `valid_until` to handle stale facts (critical for rapidly evolving research projects).

#### Phase 3: Advanced (High Complexity, Experimental)
- **RL-driven memory policies:** Train or adapt a small model to decide what to store/retrieve/forget (Memory-R1, MemTier approach). Only viable if you have enough interaction volume to amortize the cost.
- **Model-as-memory:** If running local LLMs (e.g., via vLLM), experiment with MemoryLLM-style latent memory pools for fast, weight-based knowledge updates. Not practical for API-only agents.
- **Multi-agent shared memory:** If running swarms of subagents, use a shared SQLite-backed memorywire protocol or neo4j-labs/agent-memory for cross-agent episodic context.

### What to Avoid
- **Don’t use cloud-only memory services** (Mem0 cloud, Zep cloud) as the primary store for CLI agents. They add latency, cost, and network dependencies. If used at all, treat them as optional sync targets.
- **Don’t stuff everything into context.** 150–200 instructions is the reliable attention budget for frontier LLMs. Long `CLAUDE.md` files degrade adherence.
- **Don’t ignore memory governance.** Build deletion, PII filtering, and conflict detection from day one. Persistent memory accumulates toxic data.

---

## (e) Key Papers to Acquire

### Essential (Foundational / High Impact)

| Paper | Authors | Year | Venue | arXiv / Link | Why acquire |
|-------|---------|------|-------|--------------|-------------|
| **MemGPT: Towards LLMs as Operating Systems** | Packer et al. | 2023 | ICLR 2024 | arXiv:2310.08560 | OS-paging metaphor for agent memory; Letta predecessor |
| **Cognitive Architectures for Language Agents (CoALA)** | Sumers et al. | 2023 | TMLR 2024 | arXiv:2309.02427 | Canonical taxonomy of working/episodic/semantic/procedural memory |
| **Generative Agents** | Park et al. | 2023 | — | arXiv:2304.03442 | Observation stream + reflection + retrieval scoring |
| **MemoryLLM: Towards Self-Updatable Large Language Models** | Wang et al. | 2024 | ICML 2024 | arXiv:2402.04624 | Parametric memory pool inside transformer |
| **M+: Extending MemoryLLM with Scalable Long-Term Memory** | Wang et al. | 2025 | ICML 2025 | OpenReview | Latent-space retrieval + LTM compression |
| **MemoRAG: Boosting Long Context Processing with Global Memory-Enhanced Retrieval** | Qian et al. | 2024 | WWW 2025 | arXiv:2409.05591 | Dual-system memory model; RLGF training |
| **HippoRAG: Neurobiologically Inspired Long-Term Memory for LLMs** | Gutierrez et al. | 2024 | NeurIPS 2024 | arXiv:2405.14831 | Hippocampal indexing theory → KG + PageRank |
| **From RAG to Memory: Non-Parametric Continual Learning for LLMs (HippoRAG 2)** | Gutierrez et al. | 2025 | ICML 2025 | arXiv:2502.14802 | 7% improvement in associative memory |
| **Mem0: The Memory Layer for AI Agents** | Chhikara et al. | 2025 | — | arXiv:2504.19413 | Production vector+graph extraction pipeline |
| **Zep: A Temporal Knowledge Graph Architecture for Agent Memory** | Rasmussen et al. | 2025 | — | arXiv:2501.13956 | Bi-temporal validity model |
| **A-MEM: Agentic Memory with Self-Organizing Notes** | Xu et al. | 2025 | NeurIPS 2025 | arXiv:2502.12110 | Zettelkasten-inspired dynamic linking |
| **MemoryBank: Enhancing Long-term Memory for LLMs** | Zhong et al. | 2024 | — | arXiv:2305.10250 | Ebbinghaus forgetting curve + reinforcement |
| **Reflexion: Language Agents with Verbal Reinforcement** | Shinn et al. | 2023 | — | arXiv:2303.11366 | Verbal reflection + episodic buffer for self-improvement |
| **Voyager: An Open-Ended Embodied Agent with Large Language Models** | Wang et al. | 2023 | NeurIPS 2023 | arXiv:2305.16291 | Ever-growing skill library (procedural memory) |
| **ReadAgent: A Human-Inspired Reading Agent with Gist Memory** | Lee et al. | 2024 | ICML 2024 | OpenReview | Gist memories for 20× context extension |
| **LongMemEval: Benchmarking Chat Assistants on Long-Term Interactive Memory** | Wu et al. | 2025 | ICLR 2025 | arXiv:2407.01528 | Canonical cross-session benchmark |
| **LoCoMo: Evaluating Very Long-Term Conversational Memory** | Maharana et al. | 2024 | ACL 2024 | arXiv:2308.12576 | Canonical single-long-conversation benchmark |
| **Graph RAG** | Edge et al. | 2024 | — | arXiv:2404.16130 | Community detection + global summarization over KGs |
| **LightMem: Lightweight and Efficient Memory-Augmented Generation** | Fang et al. | 2026 | — | arXiv:2510.18866 | Atkinson-Shiffrin-inspired; strong efficiency results |
| **SimpleMem: Efficient Lifelong Memory Management** | Liu et al. | 2026 | — | arXiv:2506.13472 | Memory atomization + adaptive consolidation |
| **MemTier: Tiered Memory Architecture and the Retrieval Bottleneck** | Anonymous / Sun et al. | 2026 | — | arXiv:2605.03675 | RL-based retrieval policy; edge-deployable |
| **Human-Inspired Memory Architecture for LLM Agents** | Anonymous | 2026 | — | arXiv:2605.08538 | Consolidation, forgetting, reconsolidation pipeline |
| **MemVerse: Multimodal Memory for Lifelong Learning Agents** | Liu et al. | 2025/2026 | — | arXiv:2512.03627 | CLS-based fast-slow memory framework |
| **MAGMA: Multi-Graph based Agentic Memory Architecture** | Jiang et al. | 2026 | — | arXiv:2601.03236 | Multi-graph memory with strong LoCoMo results |
| **True Memory / Storage Is Not Memory** | Sauron Labs | 2026 | Tech Report | arXiv:2605.04897 | Retrieval-centered architecture; 93% LoCoMo on SQLite alone |
| **MemMachine: Ground-Truth-Preserving Memory** | Anonymous | 2026 | — | arXiv:2604.04853 | 91.7% LoCoMo; sentence-level indexing; 80% token reduction |
| **BYTEROVER** | Anonymous | 2026 | — | arXiv:2604.01599 | 96.1% LoCoMo; context tree architecture |
| **vstash: Local-First Hybrid Retrieval with Adaptive Fusion** | Steffens | 2026 | — | arXiv:2604.15484 | Self-supervised embedding tuning; beats ColBERTv2 |
| **MemCoder: Repository-Level Code Agent with Human-AI Co-Evolution** | Anonymous | 2026 | — | arXiv:2603.13258 | SWE-bench SOTA using commit memory |
| **ExpeRepair: Dual-Memory Enhanced Repository-Level Program Repair** | Anonymous | 2026 | — | arXiv:2506.10484 | Episodic + semantic memory for code repair |
| **Tiered Memory Architecture and Retrieval Bottleneck (MemTier)** | Anonymous | 2026 | — | arXiv:2605.03675 | PPO-adapted retrieval weights; diagnostic ablations |
| **MemFail: Stress-Testing Failure Modes of LLM Memory Systems** | Anonymous | 2026 | — | arXiv:2605.26667 | Systematic benchmark of memory failure modes |
| **Eywa: Provenance-Grounded Long-Term Memory** | Anonymous | 2026 | — | arXiv:2605.30771 | Immutable evidence + canonical beliefs; deterministic retrieval |
| **Memory in the Age of AI Agents** (Survey) | Hu et al. | 2024/2025 | — | arXiv:2410.08372 | Comprehensive taxonomy: factual / experiential / working |
| **From Human Memory to AI Memory: A Survey** | Wu et al. | 2025 | — | arXiv:2503.01461 | 3D-8Q taxonomy across object, form, and time |

### Already Present in Local Library

The following relevant papers are already held in `../neurotic_library/lib/artificial_intelligence/`:

- `Park_2023_Generative_Agents.pdf`
- `Lewis2020_RAG.pdf`
- `Edge2024_GraphRAG.pdf`
- `Gao2024_RAGSurvey.pdf`
- `Qi2024_PipelineControllableMemory.pdf`
- `Sukhbaatar2015_EndToEndMemoryNetworks.pdf`
- `Krotov2021_HierarchicalAssociativeMemory.pdf`
- `KrotovHopfield2021_LargeAssociativeMemory.pdf`
- `Huang2024_ComprehensiveRAGSurvey.pdf`
- `Bietti2023_BirthOfTransformerMemory.pdf`
- `Wang2024_SpeculativeRAG.pdf`
- `Fan2025_ARAG.pdf`
- `Yan2024_CRAG.pdf`
- `Zhang2025_HiPRAG.pdf`
- `Jeong2024_AdaptiveRAG.pdf`
- `Peng2024_GraphRAGSurvey.pdf` (×2)
- `Chen2024_AgentPoison.pdf`
- `COSMIC_2025_Compressed_Semantic_Cache.pdf`
- `MeanCache_2024_Semantic_Cache_LLMs.pdf`
- `ContextParallelism2024_ScalableMillionToken.pdf`
- `Hooper2024_KVQuant_Towards_10_Million_Context_Length_LLM_Infe.pdf`
- `Hu2024_MemServe_Context_Caching_for_Disaggregated.pdf`
- `Wu2024_LoongServe_Efficiently_Serving_LongContext_Large.pdf`
- `LongContextAttentionBenchmark2025.pdf`

**Acquisition priority:** The 2024–2026 memory-specific papers (MemGPT, Mem0, Zep, A-MEM, MemoryLLM/M+, HippoRAG, LoCoMo, LongMemEval, MemoRAG, MemVerse, MemTier, True Memory, MemMachine, BYTEROVER, vstash, MemCoder, ExpeRepair) are **not currently in the library** and should be acquired.

---

## Appendix: Open-Source Implementations to Evaluate

| Project | Language | Architecture | Best For |
|---------|----------|--------------|----------|
| **Letta** (ex-MemGPT) | Python | Tiered OS-style memory | Stateful agent runtime, research |
| **Mem0** | Python | Vector + graph extraction | Production multi-tenant agents |
| **Zep / Graphiti** | Python / Go | Temporal KG | Enterprise temporal reasoning |
| **engram** | Go | SQLite + FTS5 + MCP | CLI coding agents, local-first |
| **vstash** | Python | SQLite + vec + FTS5 + adaptive RRF | Document-heavy research |
| **codemem** | Rust | Graph-vector hybrid + MCP | Codebase memory |
| **agent-context** | Python / bash | File-based consolidation | Zero-infra cross-agent memory |
| **HippoRAG** | Python | KG + Personalized PageRank | Multi-hop QA |
| **MemoryLLM** | Python | Latent memory pool | Custom local LLM deployment |
| **MemoRAG** | Python | Dual-system memory RAG | Long-context RAG tasks |
| **Supermemory** | TypeScript | Chunk-based + relational versioning | Web-app integration |
| **palinode** | ? | Git-native markdown + sqlite-vec | Git-coupled note memory |
| **Stash** | ? | 8-stage consolidation + MCP | Self-hosted persistent memory |

---

## Appendix: Glossary of Memory Operations

Across frameworks, memory primitives are converging on a common vocabulary (see also the emerging **memorywire** protocol, arXiv:2606.01138):

| Operation | Meaning |
|-----------|---------|
| **remember / store / add** | Persist a new memory |
| **recall / search / retrieve** | Find relevant memories by query |
| **forget / delete / expire** | Remove or invalidate a memory |
| **merge / consolidate** | Combine related memories to reduce redundancy |
| **reflect** | Synthesize higher-level insights from raw observations |
| **update / edit** | Modify an existing memory (with or without versioning) |
| **boot / context** | Load initial memory set at session start |

---

*Report compiled by explorer subagent. For questions or deeper dives into any section, delegate a focused research task.*
