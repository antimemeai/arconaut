# Comprehensive Synthesis: LLM Token Efficiency in Agents and Coding Contexts

**Acquired:** 55 core papers across 6 research pillars  
**Date:** 2026-06-07  
**Coverage:** Prompt compression, KV cache optimization, speculative decoding, agent systems, code generation, economic optimization  

---

## 1. Executive Summary

Token efficiency for LLMs is a multi-layer optimization problem spanning **input** (prompt/context compression), **state** (KV cache management), **compute** (speculative/accelerated decoding), **architecture** (structured generation, early exit), and **economics** (model routing, cascades). In agent and coding contexts — where trajectories grow quadratically and repository contexts routinely exceed 100K tokens — the problem is acute.

**Key quantitative findings from the literature:**

| Technique | Token/Cost Reduction | Latency Speedup | Domain |
|-----------|---------------------|-----------------|--------|
| LLMLingua (prompt compression) | up to 20× compression | — | General |
| LongLLMLingua | ~4× fewer tokens, 94% cost reduction | 1.4–2.6× end-to-end | Long context |
| H2O (KV eviction) | 80% KV cache reduction | 1.9× latency, 29× throughput | Inference |
| StreamingLLM | constant memory | 22.2× vs sliding window | Streaming/agents |
| Speculative Decoding | lossless | 2–3× | General |
| EAGLE-2 (feature-level drafting) | lossless | 3–5× | General |
| Lookahead Decoding | lossless | 1.5–2× | General, no draft model |
| ReWOO (agent decoupling) | 64% token reduction avg, 5× on HotpotQA | — | Agents |
| AgentDiet (trajectory reduction) | 39.9–59.7% input tokens, 21.1–35.9% total cost | — | Coding agents |
| Prefix caching (agent workloads) | 51.7% avg KV memory reduction | 58.6% prefill latency | Agents |
| SGLang RadixAttention | 85–95% cache hit (RAG) | 3–5× throughput | Serving |
| BatchLLM prefix sharing | 92.6% token reuse ratio | 10.8× vs vLLM | Batch inference |
| KVCOMM (multi-agent sharing) | 70%+ reuse rate | 7.8× prefill speedup (5 agents) | Multi-agent |
| Cache-to-Cache (cross-model) | avoids text generation entirely | 2.5× latency | Multi-LLM |
| CodePromptZip | 23.4%, 28.7%, 8.7% improvement over baselines | — | Code RAG |
| LongCodeZip | up to 5.6× compression | — | Code context |
| Token Sugar | 15.1% token reduction (22.4% with SimPy) | 11.2% gen reduction | Code |
| XGrammar (structured decoding) | near-zero overhead | up to 100× vs prior CFG | Code/JSON |
| CodeStruct (AST action spaces) | 12–38% token consumption | 33% inference cost | Code agents |
| FrugalGPT (model cascade) | up to 98% cost reduction | — | API economics |
| RouteLLM (query routing) | 85% cost at 95% GPT-4 quality | — | API economics |

---

## 2. Prompt & Context Compression

### 2.1 Hard Compression: Token Pruning

The foundational insight, dating to Shannon (1951), is that natural language is inherently redundant. LLMLingua (Jiang et al., EMNLP 2023) operationalizes this with a **coarse-to-fine pipeline**:

1. **Budget controller**: dynamically allocates compression ratios across prompt components (instruction, demonstrations, question) to maintain semantic integrity under high compression
2. **Token-level iterative compression**: models conditional dependencies between tokens using a small LM's perplexity scores — tokens with lower perplexity contribute less entropy and can be pruned with minimal impact
3. **Distribution alignment**: instruction-tunes the small compressor LM to align with the target LLM's distribution

**Results**: up to 20× compression with little performance loss on GSM8K, BBH, ShareGPT, and ArXiv summarization.

**LongLLMLingua** (ACL 2024) extends this to long contexts (4K–32K) with **position-aware importance estimation** that explicitly addresses the "lost in the middle" phenomenon (Liu et al., TACL 2024 — see §6.6). It reorders documents by relevance and applies question-aware coarse-to-fine compression, achieving **94% cost reduction** on LooGLE benchmark.

**LLMLingua-2** (ACL 2024 Findings) replaces the perplexity-based heuristic with a **BERT-level binary classifier trained via GPT-4 distillation**, predicting which tokens are essential. This is 3–6× faster than v1 and achieves comparable compression.

**500×Compressor** (Li et al., 2024) pushes to extreme ratios via learned compression strategies, though practical utility at 500× remains limited to specific domains.

**PISCO** (Louis et al., 2025) takes a **sentence-level** approach for RAG, achieving state-of-the-art for long-context RAG compression with 4–12×+ compression rates while maintaining strong grounding.

### 2.2 Soft Compression: Learned Embeddings

**Gisting** (Mu et al., NeurIPS 2023) fine-tunes a decoder Transformer to compress prompts into dense "gist tokens" via modified causal attention masks. Achieves up to **26× compression** with minor performance loss. The key insight: gist tokens are trained end-to-end on the target task, so the compressed representation is optimized for the specific downstream use.

**ICAE** (Ge et al., ICLR 2024) uses a **learnable encoder with LoRA** adapted from the target LLM to compress long contexts into "memory slots." The fixed LLM serves as decoder. Achieves 4× context compression; extended to Mistral-7B.

**xRAG** (Cheng et al., NeurIPS 2024) pushes this to the extreme: compresses retrieved context into a **single dense embedding token**. Evaluated on HotpotQA and arXiv summarization.

**Trade-off**: Soft compression requires model access (not API-only), gradient flow, or fine-tuning. Hard compression is plug-and-play for any LLM API.

### 2.3 Code-Specific Compression

**CodePromptZip** (He et al., 2025) is the first code-specific prompt compressor. Key insight: **not all token types are equally important in code**. Using program analysis, tokens are categorized (Identifier, Keyword, Operator, etc.) and ablation analysis ranks removal priorities. Identifiers are ranked *highest* for removal — counterintuitively, because the LLM can often infer them from context. The compressor (CodeT5 775M) is augmented with a **copy mechanism** so compressed code is fully derived from the original.

**Results**: outperforms LLMLingua and RECOMP by 23.4%, 28.7%, and 8.7% on Assertion Generation, Bugs2Fix, and Code Suggestion respectively.

**LongCodeZip** (Shi et al., 2025) is training-free and plug-and-play: coarse-grained function-level selection + fine-grained perplexity-based block detection. Achieves **up to 5.6× compression** without performance degradation.

**Token Sugar** (Sun et al., 2025) operates at the **semantic level** rather than syntactic. Mines frequent, token-heavy code patterns from generalized ASTs and replaces them with reversible shorthands. Achieves **15.1% token reduction** on LeetCode, **12.9%** on HumanEval; combined with syntax simplification (SimPy) reaches **22.4%** total savings without Pass@1 degradation. The critical finding: only **25.5%** of tokens in Python code correspond to syntax elements — the rest are semantic verbosity (long identifiers, API names, design patterns).

---

## 3. KV Cache & Memory-Efficient Attention

The KV cache is the dominant memory bottleneck in LLM inference. For a 30B-parameter model with batch size 128 and sequence length 1024, the KV cache alone is **180GB** (H2O paper). In agent and coding contexts where trajectories accumulate tool outputs, error messages, and file contents, this scales catastrophically.

### 3.1 Eviction Strategies

**H2O** (Zhang et al., NeurIPS 2023) formulates KV cache eviction as a **dynamic submodular problem**. Key insight: a small portion of tokens — "Heavy Hitters" (H2) — contributes most of the value when computing attention scores. H2 tokens naturally emerge from frequent co-occurrence patterns. H2O dynamically retains a balance of recent tokens and H2 tokens.

**Results**: 20% heavy hitters → **29× throughput** improvement over DeepSpeed/HuggingFace on OPT-6.7B/30B, **1.9× latency** reduction.

**StreamingLLM** (Xiao et al., ICLR 2024) discovers the **"attention sink"** phenomenon: initial tokens (even semantically unimportant ones) absorb disproportionate attention. Keeping sink tokens + a sliding window of recent tokens achieves **constant-memory infinite-length generation** without fine-tuning. Demonstrated stable generation up to **4M+ tokens** with **22.2× speedup** over sliding-window recomputation.

**SnapKV** (Li et al., NeurIPS 2024) observes that the LLM "knows what it's looking for" during the prefill phase. By analyzing attention patterns from the last few prompt tokens, it identifies salient KV cache entries and compresses statically before generation begins.

**PyramidKV** (Cai et al., 2024) allocates KV cache budget **unevenly across layers**: higher layers get smaller budgets because attention becomes sparser in deeper layers (pyramidal information funneling).

### 3.2 Quantization

**KIVI** (Liu et al., ICML 2024) proposes **asymmetric 2-bit quantization**: per-channel for keys, per-token for values. Tuning-free; achieves aggressive compression without retraining.

**KVQuant** (Hooper et al., NeurIPS 2024) achieves **4-bit per-channel quantization** combined with pre-RoPE positional embedding quantization, scaling to **10M context lengths**.

### 3.3 Serving Systems

**PagedAttention** (Kwon et al., SOSP 2023) — the foundation of vLLM — stores KV cache in **non-contiguous memory blocks**, eliminating fragmentation and enabling much larger batch sizes. vLLM v1 enables prefix caching by default (`--enable-prefix-caching`), but uses block-level hashing which is less flexible than token-level approaches.

**SGLang** (Zheng et al., OSDI 2024) introduces **RadixAttention**: token-level radix tree KV cache management. Enables automatic prefix caching with typical hit rates of **85–95%** (RAG), **70–90%** (few-shot), **60–80%** (multi-turn chat).

**BatchLLM** (Zheng et al., 2024) addresses implicit prefix caching limitations in vLLM with **global prefix sharing and throughput-oriented token batching**. Achieves **92.6% token reusing ratio** vs 6.3% for vLLM when shared prefix length is 16K with share degree 16. Throughput improvements of **10.8×/9.5×** over vLLM/SGLang in large-batch scenarios.

---

## 4. Speculative & Accelerated Decoding

### 4.1 Draft-Model Approaches

**Speculative Decoding** (Leviathan et al., ICML 2023) uses a small draft model to propose tokens, verified in parallel by the target model. Guarantees **exact output distribution** while accelerating generation by 2–3×.

**SpecInfer** (Miao et al., ASPLOS 2024) replaces linear draft chains with **tree-structured speculative inference**, verifying multiple paths in parallel.

### 4.2 No-Draft Approaches

**Medusa** (Cai et al., 2024) adds **multiple shallow decoding heads** to the target model itself, predicting future tokens without a separate draft model.

**Lookahead Decoding** (Fu et al., ICML 2024) uses **Jacobi-style parallel decoding**: maintains n-gram pools from the decoding trajectory for parallel token verification. Eliminates the draft model entirely.

### 4.3 Feature-Level Drafting

**EAGLE** (Li et al., ICML 2024) drafts at the **feature level** (auto-regressive head over hidden states) rather than token level, dramatically improving acceptance rates. **EAGLE-2** (EMNLP 2024) adds dynamic draft trees. **EAGLE-3** (NeurIPS 2025) modifies training to better align the model with speculative decoding.

### 4.4 Integration with Code/Structured Generation

An open question: can speculative decoding be combined with grammar-constrained decoding without breaking constraints? The verification step must respect the grammar, which complicates parallel verification. XGrammar's co-design with inference engines (§5.3) is a promising direction.

---

## 5. Code Generation & Structured Decoding

### 5.1 Repository-Level Context Management

Code generation is uniquely token-hungry because:
- Repository contexts routinely exceed 100K tokens
- Each file, function, and dependency adds tokens
- Tool outputs (compiler errors, test failures, grep results) are verbose and accumulate in trajectories

**RepoCoder** (Zhang et al., EMNLP 2023) uses iterative RAG that progressively enriches prompts with repository context, explicitly measuring input token lengths across dialogue rounds.

**RepoFormer** (Wu et al., 2024) introduces **self-selective RAG**: special tokens trigger retrieval only when necessary, reducing unnecessary context overhead.

**InlineCoder** (2026) reframes repo-level generation as function-level by bidirectionally inlining upstream callers and downstream callees, dramatically reducing context waste.

**AgentDiet** (Xiao et al., PACMSE 2026) — the first paper on inference-time trajectory reduction for coding agents — finds that average SWE-bench trajectories contain **48.4K tokens in 40 steps**, with accumulated token usage reaching **1M tokens per issue**. It identifies three categories of waste:
1. **Useless information**: tool outputs never referenced again
2. **Redundant information**: repeated observations across steps
3. **Expired information**: context that was relevant early but no longer needed

Their LLM-based reflection module reduces input tokens by **39.9–59.7%** and total cost by **21.1–35.9%** while maintaining performance parity.

### 5.2 Grammar-Constrained Decoding

**SynCode** (Ugare et al., 2024) enforces CFGs during decoding, reducing syntax errors by **96%** on Python and Go.

**XGrammar** (Dong et al., MLSys 2025) is the state-of-the-art structured generation engine. Key innovations:
- Divides vocabulary into **context-independent tokens** (prechecked) and **context-dependent tokens** (interpreted at runtime)
- Builds transformations to expand grammar context and reduce context-dependent tokens
- Efficient persistent stack for runtime checks
- Co-design with inference engine to **overlap grammar computation with GPU execution**

**Results**: up to **100× speedup** over prior CFG approaches; near-zero overhead in end-to-end serving. Integrated with vLLM, SGLang, MLC-LLM.

**XGrammar-2** (2025) adds tag dispatch, JIT-based cross-grammar caching, and Earley parser support for dynamic schemas in agentic tool-calling.

**Pre3** (ACL 2025) adapts LR(1)-to-DPDA techniques, achieving **40% throughput improvement** over XGrammar at large batch sizes.

### 5.3 AST-Based Optimization

**cAST** (Zhang et al., EMNLP 2025 Findings) uses **AST-based recursive chunking** for RAG that respects syntactic boundaries. Improves Recall@5 by 4.3 points on RepoEval and Pass@1 by 2.67 on SWE-bench over line-based chunking.

**CodeStruct** (2026) demonstrates that exposing **AST-based structured action spaces** instead of raw text interfaces reduces token consumption by **12–38%** and inference cost by up to **33%** for code agents.

---

## 6. Agent Systems & Multi-Turn Efficiency

### 6.1 The Agent Token Problem

In multi-turn agents, token consumption grows **quadratically** with trajectory length because each turn re-encodes the full history. Key data points:
- **Reflexion**: avg 89.4K input / 15.2K output tokens per task (vs 47.2K/8.3K for ReAct-GPT4)
- **OpenRouter daily usage** (Sept 2025): 100B tokens for Claude 4 Sonnet, **99% input** (trajectory accumulation), 1% generated
- **SWE-bench trajectories**: average 48.4K tokens in 40 steps, reaching 1M tokens per issue

### 6.2 Decoupling Reasoning from Observations

**ReWOO** (Xu et al., 2023) proposes modular reasoning detached from tool observations. A Planner generates reasoning steps; Workers execute tool calls independently. The full trajectory is never re-encoded.

**Results**: **5× token efficiency** on HotpotQA with 4% accuracy improvement; **64% token reduction** averaged across six benchmarks. Enables offloading reasoning from 175B GPT-3.5 to 7B LLaMA.

### 6.3 Prefix Caching

Prefix caching is **the single highest-leverage optimization** for agent workloads:

**SGLang RadixAttention**: 85–95% cache hit (RAG), 70–90% (few-shot), 60–80% (multi-turn chat)

**BatchLLM**: 92.6% token reusing ratio with 16K shared prefix

**Cost of Dynamic Reasoning** (Kim et al., 2025): prefix caching reduces prefill latency by **58.6%** and end-to-end latency by **15.7%** for agentic workloads. KV cache memory drops by **51.7%** (avg) and **63.5%** (max). Critically: **agent workloads benefit far more from caching than CoT workloads** because agents have longer shared prefixes (system prompt, task description, tool schemas).

### 6.4 KV Cache Sharing Across Agents

**KVCOMM** (Ye et al., NeurIPS 2025) is the first system for **cross-context KV-cache communication** in multi-agent systems. Core challenge: diverging prefixes across agents cause offset variance in KV caches. Solution: anchor-based offset estimation using a pool of cached examples.

**Results**: **70%+ reuse rate**, **7.8× prefill speedup** in 5-agent settings, TTFT from ~430ms to ~55ms on H100.

**Cache-to-Cache** (Fu et al., ICLR 2026) replaces text-based inter-model communication with **direct KV-cache transfer** using neural projectors. Achieves **6.4–14.2% higher accuracy** than individual models, **3.1–5.4%** over text communication, with **2.5× latency speedup**.

**DroidSpeak** (Liu et al., NSDI 2026) enables cache reuse across fine-tuned models by selectively recomputing critical layers.

**LatentMAS** (2025) uses training-free latent collaboration with KV-cache working memory: **83.7% token savings**.

### 6.5 Trajectory & Context Management

**AgentDiet** (§5.1): LLM-based reflection module for trajectory reduction.

**OpenHands LLMSummarizingCondenser**: periodically summarizes old message blocks using a cheaper LLM (GPT-4o-mini). Reduces API costs by **~2×** with no performance degradation.

**Atlassian Rovo Dev**: "Protect the edges" heuristic — keep beginning (task framing) and most recent interactions; compact the middle. Structure-aware pruning is instant and free vs LLM-based compaction.

**Continuum** (Li et al., 2025): introduces **KV cache TTL** (time-to-live) for tool-call pauses. Predicts tool durations and selectively pins KV caches. Reduces delay by **1.12× to 3.66×**, up to **8.18×** on real SWE-agent workloads.

### 6.6 Tool Use Optimization

**SMART** (Qian et al., ACL 2025): reduces unnecessary tool calls by **24%** while improving performance by **37%**. Enables 7B models to match 70B counterparts by balancing parametric knowledge vs tool use.

**Anthropic Token-Efficient Tool Use**: reduces tool output verbosity by **14–70%** without information loss.

**ToolScope** (Liu et al., 2025): reduces tool schema bloat via merging and filtering.

---

## 7. Economic Optimization & Model Routing

### 7.1 FrugalGPT

**FrugalGPT** (Chen et al., TMLR 2023) outlines three strategies:
1. **Prompt adaptation**: reduce prompt length/complexity for simpler queries
2. **LLM approximation**: use smaller models or cached responses when sufficient
3. **LLM cascade**: route easy queries to cheap models, hard queries to expensive ones

**Results**: matches GPT-4 performance with **up to 98% cost reduction**, or improves accuracy by 4% at same cost.

### 7.2 RouteLLM

**RouteLLM** (Ong et al., ICLR 2024) learns query routing from Chatbot Arena preference data. Achieves **85% cost reduction** on MT-Bench at **95% of GPT-4 quality**.

### 7.3 Industry Practices

- **Prompt caching**: 50–90% discount on cached input tokens (Anthropic, OpenAI)
- **Model routing cascades**: 87% cost reduction (Zylos Research)
- **Semantic caching**: ~31% redundant query elimination
- **Batch API discounts**: 50%

---

## 8. Cross-Cutting Insights

### 8.1 The Lost-in-the-Middle Problem

**Lost in the Middle** (Liu et al., TACL 2024) is foundational: LLMs perform best when relevant information is at the beginning or end of context, and degrade when it's in the middle. This has profound implications:
- **Position-aware compression** is essential (LongLLMLingua)
- **Document reordering** by relevance improves performance
- **Attention sink** phenomenon (StreamingLLM) may partially explain why initial tokens are disproportionately important

### 8.2 Composability Challenges

The literature largely studies these techniques in isolation. Critical open questions:
- How does **prompt compression** interact with **KV cache compression**? If you compress the prompt first, the KV cache is already smaller — but does eviction-based KV compression then remove tokens that the prompt compressor deemed essential?
- Can **speculative decoding** work with **grammar-constrained decoding**? Parallel verification must respect grammar constraints.
- Can **KV cache sharing** (KVCOMM) be combined with **KV cache eviction** (H2O, StreamingLLM)? If agents evict different tokens, shared prefixes may diverge.

### 8.3 The Code-Specific Opportunity

Code has unique properties that general NLP compression misses:
- **Syntactic structure** enables AST-based chunking and grammar-constrained generation
- **Type information** enables priority-driven token removal (CodePromptZip)
- **Semantic patterns** enable shorthand substitution (Token Sugar)
- **Tool outputs** (compiler errors, test logs) are highly compressible via structured extraction

The combination of: (1) code-specific prompt compression, (2) AST-based structured action spaces, (3) grammar-constrained decoding, and (4) trajectory reduction represents a **multiplicative** opportunity for coding agents.

### 8.4 The Agent Economics Crisis

The Kim et al. (2025) infrastructure analysis reveals a looming crisis:
- xAI's Colossus: 100K H100 GPUs, 150MW
- Meta's Hyperion, OpenAI's Stargate: projected gigawatt-scale
- Agent workflows multiply per-request compute by **orders of magnitude**
- **53% of developers** cite cost as a barrier to AI agent adoption (StackOverflow 2025 survey)

This makes token efficiency not merely an optimization but a **sustainability imperative**.

---

## 9. Open Questions & Future Directions

1. **Optimal composition**: What is the optimal stack of compression + caching + speculative decoding for coding agents? No paper has systematically evaluated combinations.

2. **Very long agent sessions**: What is the optimal context management strategy for 100K+ token agent sessions? Current approaches (summarization, truncation, KV eviction) have not been compared head-to-head on real coding tasks.

3. **Grammar + speculative decoding**: Can constrained decoding frameworks (XGrammar) be extended to support speculative verification without grammar violations?

4. **Cross-model KV communication**: Cache-to-Cache and KVCOMM open a new paradigm. How does this scale to 10+ agents? What are the security/privacy implications of sharing internal representations?

5. **Dynamic compute allocation**: Can agents self-regulate their token budget? E.g., allocate more tokens to debugging phases, fewer to exploration phases.

6. **Token efficiency vs interpretability**: Trajectory reduction (AgentDiet) and prompt compression both reduce human-readable context. Does this impede debugging and auditability?

7. **Training-time optimization**: Most work is inference-time. Can we train models that are inherently more token-efficient for code and agent tasks? (Token Sugar's pretraining approach is a start.)

---

## 10. Recommended Reading Order

**For practitioners building coding agents:**
1. AgentDiet (trajectory reduction — immediate applicability)
2. CodePromptZip + LongCodeZip (code-specific compression)
3. XGrammar (structured generation for tool calling)
4. Token Sugar (semantic-level code optimization)
5. StreamingLLM + H2O (KV cache for long sessions)
6. SGLang / BatchLLM (prefix caching for serving)

**For researchers:**
1. Lost in the Middle (foundational context understanding)
2. LLMLingua → LongLLMLingua → LLMLingua-2 (compression progression)
3. H2O → StreamingLLM → SnapKV (KV cache evolution)
4. Speculative Decoding → EAGLE → Lookahead (decoding acceleration)
5. KVCOMM → Cache-to-Cache (multi-agent communication)
6. FrugalGPT → RouteLLM (economic optimization)

**For system builders:**
1. PagedAttention / vLLM (serving foundation)
2. SGLang (structured programs + RadixAttention)
3. FlashAttention 1-3 (kernel optimization)
4. KIVI / KVQuant (memory-constrained deployment)
5. Continuum / Autellix (agent-specific scheduling)

---

## Appendix: Acquisition Inventory

All papers acquired from arXiv direct PDF links, text-extracted via `pdftotext -layout`, and organized in:
- `intake/llm_token_efficiency/*.pdf` (55 papers, ~137MB)
- `intake/llm_token_efficiency/text/*.txt` (extracted plain text)
- `intake/llm_token_efficiency/_MANIFEST.md` (full provenance table)
- `intake/llm_token_efficiency/_MASTER_BIBLIOGRAPHY.md` (complete bibliography including Tier 2)
