# Optimal Compression Stack for LLM Coding Agents: Arconaut Recommendation

**Date:** 2026-06-07  
**Context:** Arconaut (200K context window, 40–100 turn trajectories, Rust codebase)  
**Sources:** Literature synthesis of 55 papers; targeted arXiv and web searches.

---

## 1. What Combination Is Most Promising?

For arconaut’s long-trajectory, repository-heavy workload, the evidence points to a **three-layer stack** rather than a single technique:

| Layer | Technique | Expected Impact | Evidence |
|-------|-----------|-----------------|----------|
| **Input** | AgentDiet + SWE-Pruner | 40–60% input token reduction | AgentDiet: 39.9–59.7% input tokens (Xiao et al., PACMSE 2026); SWE-Pruner: 23–38% on SWE-Bench Verified while *improving* pass rate +1.2–1.4pp (arXiv:2601.16746) |
| **State** | Prefix caching + StreamingLLM / KeyDiff | 50%+ KV memory reduction, constant-memory streaming | Prefix caching: 51.7% avg KV memory reduction, 58.6% prefill latency cut (Kim et al., 2025); StreamingLLM: 22.2× speedup vs sliding window, stable to 4M+ tokens (Xiao et al., ICLR 2024); KeyDiff: 1.5%/0.04% accuracy drop under strict budgets (arXiv:2504.15364) |
| **Compute** | EAGLE-2/3 speculative decoding | 3–5× generation speedup, lossless | EAGLE-2: 3–5× (Li et al., EMNLP 2024); EAGLE-3 further improves acceptance rates (NeurIPS 2025) |

**Why this stack?** AgentDiet tackles trajectory bloat—the dominant cost driver. SWE-bench trajectories average 48.4K tokens across 40 steps and can reach 1M per issue. AgentDiet strips *useless, redundant, and expired* information without performance loss (-1.0% to +2.0%). SWE-Pruner then compresses code observations via line-level, task-aware pruning—unlike token-level methods (LLMLingua-2) that break syntax and degrade agent performance (54% vs 64% success on SWE-Bench). Prefix caching is the highest-leverage KV optimization because agents share long prefixes (system prompt, tool schemas). StreamingLLM or KeyDiff cap memory during the 40–100 turn decoding phase. Speculative decoding sits orthogonally at the decoding layer.

---

## 2. What Should We Prototype First?

**Priority 1: AgentDiet-style trajectory reduction.** Lowest-hanging fruit: no model access or inference-stack changes needed. Add a cheap reflection LLM (e.g., GPT-4o-mini or local 4B) that periodically rewrites step `s-a` using a sliding window of recent context. Skip reflection if the step is below a token threshold θ. The paper used a reflection model 12× cheaper than the agent model, keeping overhead to 5–10%.

**Priority 2: SWE-Pruner for repository context.** Integrate a lightweight 0.6B “neural skimmer” as middleware on file-read tool outputs. It performs goal-conditioned line selection, preserving AST structure. On SWE-Bench it reduced peak prompt length by 30% and cut agent rounds by 18–35%. If training a custom skimmer is too heavy, start with LongCodeZip (training-free, up to 5.6× compression) as a baseline.

**Priority 3: Prefix caching via SGLang or BatchLLM.** Enable RadixAttention or global prefix sharing. Agent workloads see 70–90% prefix hit rates, and BatchLLM reports 92.6% token reuse with 16K shared prefixes.

**Priority 4: KV cache eviction.** For self-hosted long sessions, integrate StreamingLLM (constant memory, minimal overhead) or KeyDiff (better accuracy retention, moderate compute cost). A llama.cpp fork already ships TurboQuant + H2O + StreamingLLM, proving production feasibility.

---

## 3. What Is Speculative / Not Yet Proven?

**The full three-layer integration is unproven.** The literature explicitly notes: *“No paper has systematically evaluated combinations”* of prompt compression + KV cache compression + speculative decoding for coding agents (Synthesis §9, Open Question #1). Specifically:

- **Interaction effects unknown:** If SWE-Pruner removes prompt tokens and StreamingLLM evicts different KV entries, the model may lose information neither layer deemed essential. No study measures this compositional risk.
- **Speculative decoding + grammar constraints:** EAGLE-2/3 accelerates general decoding, but parallel verification must respect grammar constraints for code/tool JSON. XGrammar (Dong et al., MLSys 2025) achieves near-zero overhead structured generation, yet combining it with speculative verification remains an open engineering problem.
- **KV eviction in multi-agent settings:** KVCOMM (Ye et al., NeurIPS 2025) shows 70%+ reuse across 5 agents, but combining shared caches with eviction policies risks prefix divergence.
- **Dynamic compute allocation:** No system automatically shifts token budget between exploration and exploitation phases.

---

## 4. Recommendation for Arconaut

1. **Immediate (this month):** Implement AgentDiet reflection + LongCodeZip fallback for file reads. Target: 35–50% input token reduction with <5% overhead.
2. **Short-term (next quarter):** Evaluate SWE-Pruner integration or train a domain-specific skimmer on Rust code. Target: structure-preserving 20–30% additional code-context compression.
3. **Medium-term:** Deploy prefix caching (SGLang/BatchLLM) and prototype StreamingLLM for 100-turn sessions. Measure end-to-end latency and cache hit rates on real arconaut trajectories.
4. **Research track:** Design an ablation study combining SWE-Pruner → StreamingLLM → EAGLE-3 to measure compositional accuracy drop and latency wins. Publish if results are positive.

The core insight: **trajectory reduction (AgentDiet) beats prompt compression for agents**, and **prefix caching beats KV eviction for shared-prefix workloads**. Layer code-specific compression (SWE-Pruner) on top, and treat speculative decoding and KV eviction as orthogonal accelerators rather than cost saviors.
