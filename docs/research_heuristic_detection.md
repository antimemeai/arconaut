# Heuristic Detection of Agent Failure Modes

**Date:** 2026-06-07  
**Scope:** arconaut Heuristic Engine design  
**Sources:** Claude Code SDK issues, aider/OpenHands/SWE-agent source analyses, arXiv (Mehtiyev & Assunção 2026; Pro2Guard/ProMAS; tool-hallucination surveys; SEC-bench; REXBENCH), production tooling (Agent Orchestrator, ACP Bridge, agent-token-meter), neurotic_library outposts.

---

## 1. What Production Agents Already Do

| System | Stuck/Loop Detection | Hard Limits | Recovery Behavior |
|--------|---------------------|-------------|-------------------|
| **Claude Code** | Closed-source `cli.js` watchdogs: idle-timeout, retry wrappers, 529 recovery, kill switches. | `--max-turns`; session token caps. | Recovery nudges; permission-mode escalation. |
| **aider** | None proactive. Reflection loop only on validation failure. | 3 retries per stage (edit/lint/test). | Structured "did you mean?" error feedback. |
| **OpenHands** | `MAX_ITERATIONS=100`; `SANDBOX_TIMEOUT=120s`. | Iteration cap; sandbox timeout. | History condenser at 32K context; no auto-reset. |
| **SWE-agent** | Max-turn / cost limits in eval harness. | 80–160 steps typical. | Trajectory terminates. |
| **Codex** | `--yolo` bypass; reasoning-effort controls; tiered timeouts. | API timeout; max turns via harness. | Non-interactive `--print` mode. |

Open frameworks rely on **hard limits** (turns, tokens, time). Only Claude Code’s closed-source layer has a real *watchdog* architecture. No open framework implements real-time behavioral heuristics beyond coarse iteration caps.

---

## 2. Failure Modes from the Literature

- **Explicit errors:** empty diffs, syntax errors, execution timeouts, tool schema violations. SEC-bench shows SWE-agent fails mostly on compilation errors (CE) and failed patches (FP); OpenHands on incorrect patches (IP); aider on no-patch (NP) due to token limits.
- **Implicit errors:** code executes but is wrong. *Stronger models produce more of these* (REXBENCH: Claude 4 produced 4× the implicit errors of Claude 3.7). Silent and dangerous.
- **Trajectory length is confounded.** Mehtiyev & Assunção (2026) analyzed 9,374 trajectories and found that, controlling for task difficulty, *successful* trajectories are *longer* (44.0 vs 39.6 steps). Length alone cannot detect failure.
- **Structure predicts outcome.** Agents that **gather context before editing** and **invest in validation** succeed more. Premature patching in the first ~10 steps is a reliable early-warning signature.
- **Architectural reasoning gap:** Agents correctly localize bugs but patch the *symptom* layer (caller/display) instead of the *root-cause* layer (callee/serialization).
- **Tool hallucinations:** Boundary/grounding errors (wrong tool, wrong arguments) dominate over syntax errors. Error propagation in multi-turn loops is intrinsic.
- **Latent policy failures:** Agents bypass required checks yet reach correct outcomes. Trajectory-level verification detects ~7% near-miss rate even on strong agents.

---

## 3. Proactive / Predictive Approaches

| Approach | Mechanism | Fit for arconaut |
|----------|-----------|------------------|
| **Pro2Guard** | DTMC over symbolic states; probabilistic reachability. | Medium: needs coding-task state abstraction. |
| **ProMAS** | Vector Markov Space + Causal Deltas; "velocity" of reasoning. | High: turn embeddings + transition dynamics map well. |
| **Doom-Loop Detection** (Lita) | MD5 fingerprint of `(tool, args)`; 3 reps in 20-turn window → warning → pause. | **High:** trivial to implement; catches loops fast. |
| **Agent Token Meter** | Burn rate, acceleration, compaction prediction from usage logs. | **High:** read-only telemetry; drive TUI alerts. |

---

## 4. Heuristics arconaut Should Implement

### Tier 1: Rule-Based (Immediate)

| Heuristic | Trigger | Action |
|-----------|---------|--------|
| **Doom-Loop Detector** | Same `(tool, args)` fingerprint ≥ 3× in last 20 turns. | Inject warning on 3rd; `Intervention::Pause` on 4th. |
| **Premature Patching Detector** | `Edit`/`Write` before ≥2 `Read`/`Search` in first 10 turns. | Nudge: "Gather more context before editing." |
| **Test-Fix Loop Detector** | `edit → test_fail → edit` ≥3× with no new reads/search. | Flag `Intervention::Narrow`; force broader review. |
| **Token Burn Alert** | Turn tokens >2σ above session mean, or cumulative >80% context window. | Pre-compaction warning; suggest handoff. |
| **Idle / Stalled Detector** | No tool call in last N turns, or >10 min wall-clock since last successful tool execution. | Prompt agent to act or indicate completion. |
| **Wrong-File Bounce** | Edit, revert, and re-edit the same file within 5 turns without touching related files. | Flag architectural reasoning gap. |
| **Validation Avoidance** | Declares done without running tests/linter after edits. | Block submission; force validation. |
| **Syntax Error Cascade** | ≥2 syntax/compilation errors in consecutive edits. | Halt edits; force read-only review. |

### Tier 2: State-Machine (Medium Term)
- **Progress Velocity Score:** Count new files read, new tests passing, new hypotheses per 5-turn window. Decay = stuck.
- **Trajectory Markov Monitor:** 1st-order transition matrix of action types. Detect anomalous sequences (e.g., `edit→edit→edit` with no `read` or `test`).
- **Context Pressure Predictor:** Fit token growth rate; predict context-window exhaustion N turns ahead.

### Tier 3: ML / Learned (Long Term)
- **Behavioral Signature Classifier:** Train on labeled audit logs to classify trajectories as "on-track" / "meandering" / "fixated."
- **Proactive Failure Prediction:** Adapt ProMAS: embed turns, model Markov transitions, detect drift into failure attractors.
- **Root-Cause Layer Checker:** LLM-judge compares proposed edits against call-graph to detect symptom-layer patching. Use sparingly (expensive).

---

## 5. Metrics to Track Per Turn

Persist these to `arconaut-audit` JSONL every turn:

1. Turn index + wall-clock elapsed.
2. Action type (`read`, `search`, `edit`, `write`, `test`, `bash`, `think`, `submit`).
3. Tool fingerprint (`tool_name + canonical_args_hash`).
4. Token delta (`input`, `output`, `cache_read`, `cache_write`).
5. Cumulative cost estimate.
6. Files touched (cardinality + ordered list).
7. Test/lint outcome (`pass`, `fail`, `not_run`).
8. Syntax/compilation error count introduced.
9. Context window utilization (%).
10. Heuristic flags fired (bitmask).

---

## 6. Recommended Thresholds

| Detector | Threshold | Notes |
|----------|-----------|-------|
| Max turns | 100 default; 25 simple; 160 deep research | Aligns with SWE-rebench norms; tunable per skill. |
| Doom-loop warning | 3 identical fingerprints in 20-turn window | Proven in Lita; low false-positive. |
| Doom-loop pause | 4 identical fingerprints | Genuine execution halt. |
| Premature patch | Edit before 2 reads in first 10 turns | Matches read-first success signature. |
| Test-fix loop | 3 cycles without new reads/search | Indicates fixation. |
| Idle stall | 3 turns text-only, or 10 min wall-clock | Distinguishes thinking from doing. |
| Token burn alert | >2σ above session mean, or >80% context | Prevents death spiral. |
| Syntax cascade | 2 consecutive edits with syntax errors | Forces review before further mutation. |
| Sandbox timeout | 120s default; 300s heavy builds | OpenHands norm. |

---

## 7. Concrete Implementation Recommendations

### Architecture
arconaut already has an **Intervention Detector** in the Soul. Add a `heuristic.rs` module in `arconaut-agent` that feeds into it:

```
Turn Executor ──▶ Heuristic Engine ──▶ Intervention Detector
(bus.rs)         (counters + state)    (escalation)
```

### Steps
1. Maintain `HeuristicState` across turns (sliding windows, counters, transition counts).
2. Fingerprint tool calls in `bus.rs` before execution.
3. Emit audit events every turn to `arconaut-audit` JSONL.
4. Return `Option<Intervention>` (nudge, warning, pause, abort) to `intervention.rs`.
5. Render burn rate, context pressure, and active flags in the TUI status bar.
6. Add `HeuristicConfig` to `arconaut-machine` settings.

### Phasing
- **Phase 1:** Doom-loop + premature-patch + max-turns + token burn. These four catch the majority of observed failure modes with zero ML.
- **Phase 2:** Progress velocity + trajectory Markov monitor.
- **Phase 3:** ML classifier on accumulated audit logs; secondary LLM judge for architectural-gap detection (use the existing Assistant Model slot sparingly).

### Performance Guardrails
- Keep per-turn evaluation **O(1)** (hashes, counters).
- Do not invoke a secondary LLM unless escalating to `Intervention::Pause`.
- Use the Assistant Model only for Phase 3 or rare escalations.

---

## 8. References

1. Mehtiyev, N. & Assunção, W. *Behavioral Drivers of Coding Agent Success and Failure.* arXiv:2604.02547, 2026.
2. Wang, H. et al. *Pro2Guard: Proactive Runtime Enforcement for LLM Agent Safety.* arXiv:2508.00500, 2025.
3. *Proactive Error Forecasting for Multi-Agent Systems Using Markov Transition Dynamics.* arXiv:2603.20260, 2026.
4. *Tool Execution Hallucination: A Survey.* TechRxiv, 2025.
5. Rabinovich, E. et al. *Latent Policy Failure Detection in Agentic Workflows.* arXiv:2603.29665, 2026.
6. SEC-bench (SWE-agent / OpenHands / Aider taxonomy). arXiv:2506.11791, 2025.
7. REXBENCH: aider, Claude Code, OpenHands error patterns. arXiv:2506.22598, 2025.
8. Lita / Extended ReAct: Doom-loop detection. arXiv:2509.25873 / arXiv:2603.05344, 2026.
9. Agent Orchestrator (Composio): JSONL stuck-agent tracking. 2026.
10. ACP Bridge: >10min auto-fail. AWS Samples, 2026.
11. agent-token-meter: Burn-rate telemetry. NPM, 2026.
12. Anthropic SDK #998: Ping-aware streaming watchdog. 2026.
