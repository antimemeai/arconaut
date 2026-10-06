# Empirical methods for Arconaut's self-driven evolution

Frumentarius lane, 2026-10-06. Research and source inspection only: no reference program was executed, no dependency selected, no product code changed, and no empirical result reproduced here. Numerical outcomes below are authors' results, not Arco results. Acquisition pins, exact URLs, hashes and restoration are in [acquisition.json](acquisition.json) and [MANIFEST.md](MANIFEST.md). Existing autoresearch material was reread rather than acquired twice.

## What matters first

Arco can improve useful software while improving itself. Those are two valuable but different outcomes. A completed feature, a stronger result on the development tasks, and an increased capacity to make the *next* useful change need separate claims. The most pertinent new negative result is yesterday's revision of **Rethinking the Evaluation of Harness Evolution**: repeated attempts with a fixed harness often beat harness evolution at comparable budgets, and the latter's mean held-out coding score was unchanged. The strongest positive comparator in this lane, **ModularRSI**, uses benchmark-disjoint experience and contrasts successful/failed traces to localize reusable changes. Neither conclusion licenses assuming that Arco's actual C++/Lua/RRC experience will follow those systems.

The recommendation is to run a small useful coding campaign with a real comparison, rather than start by constructing a grand evolutionary scheduler. Let the campaign ship an audit explorer, original-context retrieval ergonomics, or Lua workflow improvements. Make its traces usable for diagnosis. Then ask whether its resulting harness saves effort on fresh changes, compared with spending the same effort retrying in the preceding harness.

## Eight methods worth distinguishing

| Method | What changes | Useful signal | Main trap | First Arco consequence |
| --- | --- | --- | --- | --- |
| Narrow edit/run/keep | A bounded program or Lua workflow | Independent outcome under fixed resource budget | Candidate can falsify the metric producer | Retain hypothesis, actual output and rejected variants |
| Archive instead of greedy chain | Multiple historical candidate lineages | Outcomes plus lineage/exploration | Complexity and incomparable scores | Keep useful near misses without activating them |
| Adaptive evaluation/search allocation | Parent/model choice and expansion frequency | Improvement and uncertainty | Early noise steals later budget | Start fixed allocation; instrument before adding bandits |
| Evaluation cascade | Cheap rejection before costly evaluation | Distinct fault-class checks | Smoke checks presented as full qualification | Build/direct regression first, fresh workload next |
| Matched-budget and held-out evaluation | Experimental protocol | Fresh task outcome and total effort | More attempts masquerade as better harness | Compare fixed retries to actual refits |
| Contrastive localized evolution | One behavioral module at a time | Success/failure differences across tasks | Task solution leaks into general policy | Diagnose several real traces before touching core |
| Online proxy selection/evolving judges | Stream policy and, optionally, reviewer | Execution evidence plus external anchors | Plausible execution mistaken for correctness | Judge disagreement creates a test/complaint lead |
| Real operator productivity experiment | Mode of collaborating on real issues | End-to-end work, review and repair | Felt speed substituted for measured speed | Count your interventions and review labor |

### 1. Narrow autonomous edit/run/keep: autoresearch

**Actually inspected:** existing pin `228791fb499a`, `program.md` in full, `prepare.py::evaluate_bpb`, and `train.py`'s timed training loop/final results. The protocol confines edits to one training file, asks for an initial baseline, records keep/discard/crash rows, and continues until interrupted. The fixed evaluator calls the editable model's per-token loss; therefore an unchanged evaluator file does not by itself make its returned metric independent. Its five-minute budget excludes early warmup/compilation; the ten-minute kill is an instruction to the hosting agent. [Source](https://github.com/karpathy/autoresearch/tree/228791fb499afffb54b46200aca536f79142f117).

**Consequence:** the compact research loop is highly suitable for Lua policies and measured C++ hot paths. Preserve failed runs and raw outputs in Arco's audit rather than overwrite one log. For speed claims measure both steady-state behavior and compile/restart/resume overhead. Supply an external process oracle that computes expected behavior from inputs, rather than trusting the changing program's printed `score`. A timeout is a failed/unknown attempt with evidence, followed by a reasoned new approach; it is not proof that an idea is bad and not a reason to replay effects blindly.

**Limit:** this snapshot supplies a protocol and GPU experiment, not an autonomous harness or independent evidence of recursive harness improvement. No training replication performed.

### 2. Keep stepping stones: DGM and FunSearch

**Actually inspected:** DGM `DGM_outer.py::choose_selfimproves`, `update_archive`, `get_full_eval_threshold`; `self_improve_step.py` diagnosis, patch check and benchmark invocation. Its default archive keeps candidates and parent sampling can combine score with inverse child count. The paper reports coding gains and cross-benchmark/model transfer, but uses staged subset evaluation during search; same-search-task improvement and genuinely alternate-benchmark transfer must not be conflated. [Paper](https://arxiv.org/abs/2505.22954), [source](https://github.com/jennyzzt/dgm/tree/a565fd2d1dca504ef5104a7cc0f3bdc4ab9b4fd2).

**Actually inspected:** FunSearch `implementation/programs_database.py`: per-test score signatures, island sampling/reset and within-cluster preference for shorter programs; `evaluator.py::Evaluator.analyse` substitutes one evolved function, executes user evaluation, rejects ancestor calls and registers measured candidates. `evaluator_test.py` checks parsing/trimming against exact desired code. The supplied sandbox is abstract; released source is not complete production infrastructure. [Paper](https://pmc.ncbi.nlm.nih.gov/articles/PMC10794145/), [source](https://github.com/google-deepmind/funsearch/tree/cc53f274237d7ab05c19df939edbc1f9616a7c19).

**Consequence:** archive a rejected policy/source variant separately from activation. A failed broad experiment may yield a useful tool definition or test. For Arco, represent a candidate's *outcome vector*: coding result, actual intervention count, context repair, native fault result, latency. Do not collapse different behavior signatures into “score improved.” Start with two or three conceptual variants, not a large population.

**Static source cautions:** DGM's optional `method='best'` sorts accuracy ascending before choosing; its name is not reliable evidence of behavior. This is not the score/child strategy described above and was not executed. Study sources critically rather than adopt their selection routine.

### 3. Allocate scarce experiments: ShinkaEvolve and HGM

**Actually inspected:** Shinka paper §§3/5; `shinka/llm/prioritization.py::AsymmetricUCB.update` shifts reward by parent/initial baseline, clips negative gains in asymmetric mode and separately tracks cost; `core/novelty_judge.py` gates on embedding similarity and optionally asks an LLM whether a close candidate differs meaningfully. `tests/test_novelty_judge.py` uses explicit response/similarity sequences; `test_bandit_persistence.py` checks retained counters/rewards/baseline after reload. The paper's 150-evaluation circle-packing result demonstrates its particular search setting, not faster arbitrary coding. [Paper](https://arxiv.org/abs/2509.19349), [source](https://github.com/SakanaAI/ShinkaEvolve/tree/8adc053a2ce4511ad2ac310e004c530a73fb974a).

**Actually inspected:** HGM paper's metaproductivity distinction; `hgm.py::TS_sample`, nested `expand`/`sample`, and `tree.py::Node.get_decendant_evals`. Expansion uses descendant outcomes while direct evaluation uses agent outcomes; the scheduler interleaves adding candidates and reducing uncertainty. Tool tests inspect actual edited contents, but are not tests of search correctness. [Paper](https://arxiv.org/abs/2510.21614), [source](https://github.com/metauto-ai/HGM/tree/013872d95da978483f5b540e531db063d23890da).

**Consequence:** measure present coding capability and later improvement productivity separately. If a small Lua change makes the next three workflows easier to improve, it has value even before a headline coding score moves. Conversely a one-task shortcut may win today while impairing future changes. Start with equal model/parent allocations and record actual costs before trying adaptive routing among ChatGPT/Kimi/MiMo/Claude/Grok. A minimum exploratory allocation keeps one lucky early win from excluding a model prematurely.

**Static source caution:** HGM's `get_pseudo_decendant_evals` returns `self.utility_measures` itself when count is below `num_pseudo`; `get_decendant_evals` then uses `+=` on that list for descendants. Static reading indicates selection can mutate direct-evaluation records through list aliasing in that branch. No reference code executed to reproduce it. An Arco oracle should assert querying lineage statistics leaves original trial outcomes unchanged. This is exactly why reference implementation is input to design, never authority.

### 4. Reject cheaply, qualify the actual claim: AlphaEvolve

**Actually read:** paper §§2.4 and the production scheduler case. Its evaluation cascade increases difficulty/expense only for promising candidates and parallelizes costly evaluations. The authors report deploying a scheduler heuristic with average recovery of 0.7% fleet-wide compute. The deployed workload matters more than another score on a small search set. The full AlphaEvolve implementation was not acquired or inspected; it is not the same as an unofficial clone. [Primary paper](https://arxiv.org/abs/2506.13131).

**Consequence:** use cheap compile and the direct changed-behavior oracle to eliminate broken candidates, then useful development tasks to decide whether they help. These attack different claims; a second receipt certifying the first adds nothing. For an RRC change, the decisive test is an actual rebuilt child inhabiting the retained session and continuing useful work, plus an external expected effect count—not a model saying “I resumed.” Native sanitizers attack memory faults, not semantic quality. Performance comparison should include startup/refit cost because Arco self-improvement pays it repeatedly.

**Limit:** infrastructure-scale gains and expensive evaluator throughput are not a budget justification for Arco. Begin with a small explicit campaign budget. Stopping an unproductive experiment stream is a valid model-governed decision, not abandonment.

### 5. Distinguish evolution from simply trying harder: Rethinking Evaluation

**Actually read:** latest v4, updated 2026-10-05, §§3–5 and tables 1/3. Under a comparable K=5 rollout protocol, fixed-harness parallel sampling averages 72.3 without tests, shared harness evolution 67.4; initial 68.2. A disjoint 45/10/34 train/validation/test split gives initial and evolved mean 63.2. Long-horizon games show a more promising setting. The authors compare feedback/rollout allocation; equal rollouts do not automatically imply equal tokens or latency. [Paper](https://arxiv.org/abs/2607.12227v4).

**Actually inspected:** pinned source `_copy_blind_rollout_view` copies agent trajectory files, excluding separate verifier/reward files; `audit_blind_rollout_selection.py::audit` measures selected success versus whether any candidate passed, with missing labels explicit. `scripts/eval_held_out_iters.py` evaluates stored harness snapshots on distinct split configs. “Agent-only” traces still require checking for embedded task-test evidence; filename exclusion is not a general nonleakage guarantee. [Source](https://github.com/rethinking-harness-evolution/code/tree/62df2b9624ff32ca61b8accce7fb4a0fd8cbc8a8).

**Consequence:** Arco's comparison needs three arms: fixed harness with the same total attempt/feedback budget; evolving Lua/context policy; evolving native harness with RRC. Count all diagnosis/proposal/review calls, tool time, compiling, restarts and intervention. Measure selected final work, not best-of-all-attempts. Use fresh tasks after candidate selection; a failed final holdout becomes development data next time and cannot remain “unseen.” This does not prevent useful online adaptation—it prevents calling adaptation a demonstrated general improvement prematurely.

### 6. Contrast trajectories and localize the change: ModularRSI

**Actually read:** v1 §§3–6 and appendix interfaces/cases. It groups tasks into all-success/all-failure/mixed trajectories; mixed success/failure on the *same task* gives contrastive clues. Findings receive support counts across distinct tasks, target one of five behavioral modules, and are integrated after separate evolution. The evolution pool is external to downstream benchmarks. Reported Terminal-Bench 2.0 accuracy rises 47.57→52.43, with cross-domain/model results and ablations. Two sampled tasks validate execution after edits, which is a narrow gate rather than broad correctness evidence. [Paper](https://arxiv.org/abs/2609.14857v1).

**Actually inspected:** `self_evo/confirm.py::paired_delta` compares variants on shared tasks and skips missing infrastructure outcomes; comments recognize composer/epoch confounding. `clustering.py` retains causal hypotheses and resolves retired variants to live descendants. `online_evo.py::review_edit` and `tests/unit/test_structured_review_verdict.py` require an executed verdict action: a quoted acceptance inside analysis must not count as completed review. Snapshot contains ongoing implementation evolution beyond the paper; no assumption that every source addition caused the published result. [Source](https://github.com/IQuestLab/ModularRSI/tree/b5c72c36b0d08ff93f00ee202a8fbdebe849dfb9).

**Consequence:** diagnose repeat failures using audit-original trajectories: same task, comparable model/settings, preceding source/context revision. Show both success and failure, preserving inconvenient evidence. A diagnosis should name a mechanism and a falsifying observation. Prioritize a recurring lost tool result or premature stop over one spectacularly misreasoned solution. Change one coherent conceptual unit, then test real interactions with context/tool lifecycle/RRC. The review record should identify actual reviewer completion or failure; do not mine optimistic language as a verdict. Same-task pairing helps but does not eliminate changed-model/epoch/composer confounding.

### 7. Evolve from unlabeled execution, but keep independent anchors: TTHE / RQGM

**Actually read:** TTHE §§4–6/algorithm 1: multiple branches edit an executable harness from trace/proxy feedback, invalid children fall back to parents, and a judge selects a persisted candidate; gold outcomes are only measured after selection. Runtime health, round-trip consistency and public tests are deliberately imperfect signals. The paper reports nonmonotonic gains with search budget and selection regret: good candidates can be proposed but rejected. No TTHE source repository was identified from the consulted paper/abstract links; findings here are paper-level. [Paper](https://arxiv.org/abs/2607.08124v1).

**Actually read:** RQGM v2 §§3.2–3.5/5: separates training feedback, validation selection and final test; evolving reviewers are replaced against evaluator-independent anchors. Replacement invalidates affected historical scores, with later lazy rescoring. It reports useful code-review and other-domain results, with explicit model-judge biases. No implementation was acquired; no empirical/theoretical claims transferred unqualified. [Paper](https://arxiv.org/abs/2606.26294).

**Consequence:** station mode naturally generates unlabeled trace evidence, but “command succeeded,” “model agrees,” and “task complete” are different propositions. A judge should produce a complaint/test lead when uncertain, not decorate uncertainty with a number. Record whether the failure is proposal coverage (no good candidate existed) or selection (good candidate existed, wrong one chosen). When changing a judge/oracle, give it a revision; old scores under it are not comparable. Judge evolution should first target independently checkable examples, including false acceptance and false rejection. Keep executable correctness authoritative wherever available; reviewer agreement covers additional fault classes, not a replacement for it.

### 8. Measure operator labor and useful completion: METR productivity RCT

**Actually read:** study methods, randomization, end-to-end issue time and limitations. Sixteen experienced maintainers worked on 246 real tasks defined before treatment assignment. Early-2025 AI availability increased time by 19% in that setting while participants believed it saved time; implementation time includes subsequent PR-review changes. This is a dated, population-specific result and is not a prediction for Arco or present models. [Primary study](https://metr.org/Early_2025_AI_Experienced_OS_Devs_Study-paper.pdf).

**Consequence:** avoid evaluating a self-driven agent by how busy or reassuring it feels. Define useful changes before choosing the run mode, then record elapsed time, actual operator minutes, review/repair work and residual defects. Count Codex rescue/editing as intervention even when it succeeds. Compare tasks of similar difficulty with randomized assignment or crossover order where feasible; same task repeated is learning, not an independent replication. The operator's attention freed while the model works is valuable in its own right and should remain distinct from reduced total completion time.

## Proposed Arco experiments and direct oracles

These are proposals for the owner's synthesis, not campaigns launched by this lane.

### A. Useful work with matched continuation budget

Choose three fresh practical tasks across audit inspection, Lua workflows and C++ tool behavior. Preserve starting repository/session states per task. Allocate the same capped provider effort and explicit wall-clock budget to fixed retry, Lua evolution and native RRC arms; compare actual accounting afterward because subscriptions may hide monetary cost. A fixed arm can retry/reflect and use the same source material; deny it only the experimental change surface. Do not handicap the baseline's context or tools.

The product oracle is a predeclared behavioral artifact produced from known inputs: e.g., audit explorer reports exact supplied events, including unknown effects; a Lua workflow consumes its actual tool output and follows a specified recovery branch; a file/tool change makes an exact expected filesystem transition. An independent reviewer examines code and supplies concrete counterexamples; its availability is a recorded fact. The experiment outcome is accepted useful work plus effort/interventions—not a model-scored transcript. Failed candidates remain visible. A tiny pilot diagnoses feasibility; it cannot establish statistical acceleration.

### B. Context policies under a useful coding workload

Compare baseline retention, model summary, selection and summary-plus-original repair on fresh equivalent coding tasks. Seed constraints in real original items, including one fact needed after compaction; track its source ID outside the candidate's policy. The oracle checks final program behavior against that fact, actual final provider input, and whether bounded retrieval restored the precise bytes. A small retained view alone is not success. Measure completion/intervention and source retrieval cost along with actual input tokens if provider usage supplies them. Do not use token estimates as actual billing.

This should produce a better context inspector/retrieval interface or workflow while studying the policy. Avoid giving every arm the answer in an “expected fact” file readable during coding. If the model accidentally reads the final oracle, record the exposure and retire that case from generalization claims.

### C. Can the refitted agent make the next change better?

After each accepted change, give the preceding and resulting harness a fresh *subsequent* useful task under the same model/settings. The oracle checks correct code effect and actual post-RRC session continuity, with a once-only external marker for effects around restart. Compare useful output per budget, including compile/restart costs. Report task-specific improvement separately from ability to propose/execute the next improvement. Only after several meaningful lineages exist consider HGM-like parent allocation; query statistics must never mutate retained outcomes.

### D. Judge quality and selection regret

Assemble a small set of independently decided examples: plausible but wrong tool output, a correct implementation with an unconvincing transcript, a quoted but unsubmitted review verdict, and a genuinely completed contrary review. Give the reviewer only intended evidence, preserve its original response/action, and compare against the independent behavioral outcome. When the selected candidate fails, inspect whether any rejected candidate would have passed. Improving judge accuracy is useful even when proposal capability is unchanged. Changed judges receive new revisions; no averaging scores across changed definitions.

### E. Real operation with deliberate stopping/reversal

Run the actual useful campaign within a chosen budget; permit the model to stop a branch when successive attempts fail for the same structural reason, reverse a candidate on a direct regression, and continue another useful task. The direct oracle observes source/session activation and expected effects, not a textual promise of rollback. Record infrastructure outages/timeouts separately from candidate semantic failures. A missing outcome remains missing; it must not silently become either success or failure. Keep one known-good launcher/build path while candidates exercise RRC. This is recovery machinery, not a recurring operator command approval flow.

## Open leads and limits

- Acquired six new source snapshots and ten primary PDFs, plus reused autoresearch. All acquired archives remain intact; no reference code ran. PDFs were read via development-only `pdftotext`; reading derivatives are in ignored `context/evo-exp-reading/`.
- Published implementations are Python-heavy; their orchestration and evaluation ideas transfer, not their runtime/dependencies. C++ build/refit and Lua activation costs are central to our experiment, unlike many cited systems.
- RQGM and TTHE implementation acquisition remains open. Do not silently substitute an unofficial implementation and claim primary-source inspection.
- ModularRSI's original research source is CC BY-NC 4.0 alongside Apache Harbor components. This study selects no adoption; study mechanisms and implement our machinery.
- Existing benchmarks can be contaminated by model training, task familiarity, or search. Benchmark-disjoint external tasks reduce one channel; do not establish universal novelty.
- Online station adaptation and reusable harness improvement are both legitimate, but require different reports. In station mode, changed feeds/model/provider versions make simple before/after averages especially weak.
- Large mutation testing remains deferred by operator and would run on the fleet, never this laptop. No empirical Arco test was executed by this research lane.
