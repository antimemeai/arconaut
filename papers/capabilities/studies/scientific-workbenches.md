# Scientific workbenches: actions and execution boundaries

Pinned-corpus study begun 2026-10-01 UTC. Fourteen references, with revisions and
artifact versions in `../registry.json`. No acquired program, test, launcher,
installer or binary will be executed; no dependency or product is adopted.

## Written subplan

Study each reference as its actual conceptual unit: application/agent engine,
scientific workflow, skill/integration pack, or produced review artifact. Trace
entry through model/action dispatch and results; persistence, concurrency,
interruption, context transformation, audit, configuration/change, and evaluation
boundaries. Read actual tests and identify the assertions and fault classes they
cover without running them. Save each completed 23-axis row incrementally, with
native actions and direct local line-range evidence. Combine findings here and
preserve distinctions between authored notebook entries and original observed IO,
shared external services and owned kernels, and selectable models and peer messaging.
For Claude Science use only pinned official contracts; its source is unavailable.

## Progress

Complete: all 14 registered references have 23-axis rows, 74 concrete action records, 175 local evidence records and 21 inspected test/check-file records. These counts describe study material, not coverage or runtime conformance. Every original/source boundary remains explicit. No acquired program, test, notebook, launcher, installer or binary was executed. Structural checks verified row identity, all axes, evidence references, existing files/line ranges and unexecuted-test flags.


## claude-science

The official 0.1.55 distribution and captured documentation supply contracts, not an inspectable application engine. Python/R code cells retain variables only within kernels that expire on idle/session/environment boundaries (`raw/tools-and-environments.md:9–36`); remote Slurm/detached jobs and Modal have separate lifetimes (`raw/remote-compute-clusters.md:23–39`, `raw/compute-providers.md:21–33`). Shared named environments are reusable installations, not evidence of kernels shared by independent Arconauts.

Artifacts have explicit saved versions, while scratch files expire. The authoritative command log takes precedence over a generated reproducibility script (`raw/artifacts.md:9–43`). Neither this nor remembered facts in an internal database establishes original provider/process/compaction IO retention or a generic model-visible DB. The reviewer reads records and citations but does not rerun analysis or assess the research-method choice (`raw/the-reviewer.md:9–32`). There are no available implementation tests to inspect; the matrix therefore keeps claims documented/limited and records unresolved program/repair/rebuild/collaboration semantics honestly.

Native control names and their inputs, outputs, authority and lifetimes are in the completed row, including code cells, artifact versions, remote jobs, MCP configuration, skills, reviewer controls and daemon CLI. They are documented product controls, not guessed RPC names. No installer, binary, HTML example or live session ran.


## k-dense-byok

This is a source-visible scientific coding workbench built around Pi, rather than a new owned execution engine. Session construction wires specialist, MCP, codemode and scientific tools (`server/src/agent/session-registry.ts:385–446`); the chat route selects model/thinking and launches a detached `executeRun(session.prompt)` (`server/src/api/sessions.ts:810–889`). Browser loss does not own the run. A session claim remains held through provenance/accounting, and message gating waits for that release. The integration test deliberately stalls provenance flush while a settled-handler follow-up is queued, using a fake provider on real Pi sessions. This tests a meaningful activation boundary, not a live provider or crash guarantee.

The native remote family distinguishes `modal_run` (abort cancels), `modal_submit`/`modal_submit_batch` (durable asynchronous handles), and status/wait/cancel/results (`server/src/agent/modal-tool.ts:270–437`). Records persist before scheduling. Remote jobs can outlive chat abort/backend reconnect, while ordinary model turns end on host restart; this is remote service continuity, not harness rebuild continuity. Specialists share project files, may choose different models and use scoped RPC control/supervisor questions. Schedules retain policy ceilings but require the backend and skip overlapping fires. The catalogue's JSON “workflows” are prompt templates; specialist workflow scripts and durable remote batch primitives have different execution semantics.

Scientific compaction preserves bounded notebook corrections, execution uncertainty, result/environment IDs and literal pending remote/child controls, then adds model narrative (`compaction-bridge.ts:38–325`). Failure returns to Pi's ordinary summary. Source-linked recall provides source/digest/correction/coverage checks useful for repairing lost context, without promoting recalled text into a permanent verified fact store. Notebook entries are authored reports; observed/inferred/declared provenance is separate. Opaque commands, scan/hash budgets, nested children, overwritten bytes and retrospective child hashes prevent a complete audit. Particularly, matching a harvest-time hash stays **unknown**, not current; tests construct this fault. Flush/accounting failures warn but do not prevent `done` (`run-pipeline.ts:303–440`), a material contrast with an audit commit requirement.

The row covers all 23 axes with these boundaries. Optional watchdog review sees bounded transcript/diffs and does not execute scientific analyses; it can fail silently. No full peer room, shared generic database, universal hot turn replacement, executable refit handoff or dedicated autoresearch-of-the-harness loop is established by the inspected source. Pi dependency internals are not vendored in this reference. No tests were run.


## openai4s

OpenAI4S is an independent implementation, not source for Claude Science. Its Python `AgentEngine` composes model, context, executor, interceptor, events, cancellation and completion ports (`agent/engine.py:42–145`). Native calls retain raw/parsed arguments, wire ID and ordinal; batches win over cells, and completion is a distinct validated action. The manager multiplexes synchronous Python host RPC with cell results, binds generation/action/sandbox authority and correlates responses (`kernel/manager.py:928–988`); the worker executes code in persistent `_NS` (`worker.py:1095–1140`). R is a separate persistent slot and does not supply the same inner Python RPC.

Concurrency has several different lifetimes. Leading read-only/non-conflicting native calls may run in parallel; the first mutation is a barrier. Foreground session writers use exact tickets/generations, while background code gets a separate worker and in-memory `exec_id`; that is not a durable shared kernel. Delegated child requests distinguish attempts, queue steering messages, publish subtree cancellation before signaling and require explicit continuation after restart (`agent/delegation.py:2069–2196`). Provider cancellation is monotonic per call so reuse of a session event cannot revive late output, but a blocked network request can remain detached until timeout (`agent/runtime.py:462–505`). This matters directly to the operator's refit rule: user-visible stop alone is not proof of no active provider request.

The standing SQLite Store is real and exposed through `host.query` with read-only SELECT/CTE, scoped views, a real authorizer and a default five-second statement timeout (`store.py:5826–5898`). Output-row limiting is optional: the default `limit=None` (or another falsy limit) takes `fetchall()`, so this is not a mandatory bounded-result interface. The action ledger records routed actions/results/terminals; compaction stores a transactional covered-through cursor and digest-addressed original summarized slice (`ledger.py:693–729`, `compaction.py:1474–1549`). Redaction and bounded output mean it is still not every original byte or OS effect. Session checkpoints/forks provide exact cursor primitives, while kernel recovery builds a separate candidate, verifies source/replay safety and returns Partial rather than promoting an invalid reconstruction (`kernel/recovery.py:996–1141`). Tests with known changed sidecar bytes and wrong ticket identities attack real failure classes; tests were only read.

Dynamic `execute(args)` tool definitions, schemas/smoke arguments and TTL are model extension primitives. Promotion is human-governed; this is not live engine replacement. The pinned updater has discovery/verification, with apply/relaunch modules absent despite narrative future transaction references (`update/README.md:5–16`). Opt-in auto-mode documentation distinguishes storage-only, shadow and gated reviewer stages; those contracts remain documented here rather than inferred default behavior. Remote SSH/BYOC compute is consumed and explicitly poll-driven, with future provider families excluded. All 23 axes reflect these scopes. Strong candidates for further Arconaut experiments are immutable context originals, explicit attempt/request/generation identity and quiescence that accounts for detached provider calls.


## finch

Finch supplies an Aviary notebook environment consumed by an LDP rollout, not its own provider engine. `reset` discovers `edit_cell`, `list_workdir`, `submit_answer` and optional bucket transfer; `step` executes calls sequentially, then returns a full notebook projection (`notebook_env.py:150–185`). `edit_cell` changes code and in **finally** saves/reruns the entire notebook (`215–244`). Local mode keeps a Jupyter kernel for the episode while rerunning all cells; Docker uses a fresh nbconvert execution with `--allow-errors`. Local error handling stops at the first cell error. Thus an “edit” is also an execution and can repeat external effects or accumulate live local state.

The analysis submit closes its owned runtime; temporary workspaces are deleted at close. Notebook/Markdown and trajectory callbacks offer useful artifacts but not immutable versions or all original IO. The agent state reduction is hide-old-environment-state, not archival conversational compaction. Integration assertions use known literal hello/Goodbye/plot outputs, with optional Docker/R skips; they do not test effect idempotence, durable restart or generic scientific validity. All 23 cells retain the difference between temporary owned computation and Arconaut's shared consumed service. No downloaded tests or notebooks were executed.


## paper-qa

PaperQA owns a domain agent loop around literature, not arbitrary coding execution. `run_agent` selects a fake deterministic, Aviary or LDP composition; the traced selector loop appends observations, requests a tool action, invokes environment step and continues (`agents/main.py:91–178,275–323`). Configured discovery binds `paper_search`, `gather_evidence`, `gen_answer`, `reset`, `complete` and optional clinical-trial search to distinct answer/summary/embedding model roles (`agents/env.py:47–140`). Generic provider transport remains a dependency.

Persistent corpus/index is a retrieval service primitive, not a generic model SQL DB. Evidence retrieval summarizes bounded matches in parallel; `gather_evidence` swaps the shared question temporarily and consequently cannot parallelize with itself (`tools.py:255–281`). Reset deliberately clears unsuitable current evidence and lets the model reacquire it. This is useful context repair composition, but no original conversational reconstruction or hot-turn replacement is demonstrated. Recorded token/tool-name history and cited contexts are not a complete audit. Timeout/max timestep may still generate a fallback answer with truncated status; `complete` records model-declared answer sufficiency, not an independent oracle.

The index-crash test injects failure after a known number of files and asserts completion without reparsing all sources. Agent tests instead demand ordered tools in a debug prompt and assert nonempty answer/context plus accounting/formatting; these cannot establish scientific correctness. All 23 cells distinguish callable literature operations, retrieval memory and provider roles from kernel execution, live peers and refit. No tests or providers were run.


## robin

Robin's public code is orchestration around local model calls and consumed Edison agents. `Step` names prompt/generator, inputs/outputs, UID, parallel count, remote runtime limits and postprocess; `run_pipeline` uploads, awaits task batches, stores task IDs, downloads and advances stage-by-stage (`multitrajectory_runner.py:20–74,199–289`). Original hosted analysis/literature execution is not included: the official README requires an Edison account/credits for that portion. This is a service boundary, not missing public code that can be inferred from Finch or PaperQA.

The analysis workflow runs multiple remote R trajectories, then a remote consensus stage reading their CSV outputs (`analyses.py:17–83`). Pairwise hypothesis comparisons use a direct model over authored candidate reasoning with bounded semaphore concurrency; they are ranking, not experimental verification. Remote success_rate counts task status. Timeouts in the public create/poll helper return submitted task IDs without cancellation (`utils.py:31–128`), so a local stopped wait is not quiescence. Downloads/save failures log and continue; process-local results become timestamped JSON at the end, not a write-ahead audit/restart journal. No first-party test suite appeared in the inspected file inventory. All 23 axes preserve the hosted/public boundary and do not invent a model-native loop or shared kernel implementation.


## agent-laboratory

Agent Laboratory is a fixed phased research workflow: literature → plan → data → experiment solver → interpretation/report/refinement. Each subtask chooses its configured model and updates role objects (`ai_lab_repo.py:60–204`). `BaseAgent.inference` reconstructs prompt from phase state/history/notes, then expires bounded history (`agents.py:204–277`). There is no archival managed compaction. The latest workflow is an overwritten pickle, useful state but not original audit or proven restart of active work.

The actual coding actions are `REPLACE` and inclusive-range `EDIT`; dataset plus candidate code is executed in a **fresh** multiprocessing child, output/error strings are inspected, repairs are attempted and failed candidate text is rolled back (`mlesolver.py:55–82,99–162,329–374`). External effects are not rolled back. Timeout terminates the direct process; descendant quiescence is unproven (`tools.py:292–325`). This is neither a shared persistent scientific kernel nor process isolation.

Reward parses an LLM SCORE from plan/code/output, while three report reviewers are sequential persona prompts to the same model. They evaluate research artifacts, not the harness, and do not supply an independent correctness oracle. Phase human rejection resets history and adds notes; optional review overrides can force completion. No first-party test suite was present in the inspected inventory. All 23 axes retain limits of evaluation, phase programmability, historical-state retention and context/compiled refit continuity.


## coscientist

Coscientist's repository is supporting chemistry data and a **simple** implementation (`README.md:1–17`), not the complete experimental system. The entire visible loop injects callable tool objects, presents their descriptions, requests an OpenAI message, counts command substrings and invokes one command-leading line (`simple_implementation/coscientist.py:5–91`). Output is appended as a user message. There is no typed tool-call identity or durable action lifecycle.

The actual launch roster is only `CALCULATE` (unrestricted Python eval), `RANDOM` (two integer bounds) and `STOP` (custom exception). This is not shipped robot/web/kernel orchestration. Eval confers broad host authority without designed process/file handles, and synchronous logging/history is not original audit or rebuild continuity. The small full-source inspection establishes these limits; it does not infer universal features from the paper's empirical artifacts. No first-party tests appeared in the inventory and no reference code ran. All 23 cells are scoped to this reference's actual role.


## ai-scientist-v2

AI Scientist v2's core is branching research-code search, not a generic live coding chat. Launcher writes idea/config, then a stage manager creates substage search agents, runs candidate steps and performs multi-seed evaluation (`agent_manager.py:692–751`). Worker dispatch chooses draft/debug/improve, hyperparameter or ablation based on node/stage; generated code executes in per-process workspace (`parallel_agent.py:1410–1527`). Distinct code/feedback/summary/review models are pipeline roles.

The Interpreter has reusable globals in principle, but normal search calls `run(code, True)` then cleanup. Optional interactive reuse cannot satisfy timeout: source asserts reset mode before interrupt (`interpreter.py:130–161,196–293`). Direct-child TERM/KILL is not descendant quiescence. More concerning, future.result timeout in ParallelAgent logs and finally releases GPU bookkeeping without cancelling that future (`parallel_agent.py:2124–2190`). Neither stopped local wait nor freed bookkeeping proves work stopped.

Candidate nodes retain UUID/time/parent, plan/code/output/exception/metric/plot feedback. Scientific memory is projected through truncation and generated journal summaries; overwritten pickle checkpoints are state, not full original audit or active-work continuity. Metrics parsing is model-generated code run against actual numpy artifacts, followed by model interpretation (`1534–1638`); seed reuse and real execution are useful, but parser/evaluator can share the researcher's errors. Paper review is another authored artifact assessment, not independent validation of all experiments. No first-party test suite appeared; runtime protocol/function-shape assertions were read and explicitly distinguished from tests. All 23 axes keep autoresearch of **experiment code** separate from Arconaut improving its own harness.


## computational-review-template

This is a skill-defined workflow plus static viewer, not a coding-agent loop. The v29 coordinator prescribes independent actor/critic/validator frames, gates, bounded new-frame continuations and canonical artifact versions (skills/comprev-orchestrator-v29.md:6–48,289–407). Host API enforcement is unavailable. Building complete delegation tasks in the same Python cell is a workaround for host compaction failures, not a managed compaction engine.

Evidence retains claims and verbatim source sentences, DOIs, access labels, conflicts and gaps (evidence/README.md:1–72). MyST loads only section 02–13, skips unavailable files and serializes records into an in-browser widget; filters render first 200 matches (plugins/evidence-explorer-plugin.mjs:130–215; content/evidence-explorer-widget.mjs:202–219). Optional evidence_database.json is not consumed; this does not expose the Science application DB.

Authored validators give literal metadata/schema oracles but weak scientific proxies: source absent from abstract does not establish presence in fulltext; uniform counts are a fabrication heuristic; sizes/counts/DOIs do not prove entailment (skills/comprev-evidence-validator.md:12–95). No acquired programs or validations were run. All 23 cells separate supporting artifacts/documented calls from engine behavior. The Arconaut question is how to execute these contracts while retaining sources and uncertainty.


## computational-review-vip

This produced v27 review is not an agent implementation or the template current v29. It retains 13 evidence JSONs, actual gates, Methods/request, a 45-entry selected version/path/hash manifest and before/after remediation (README.md:1–50; provenance/artifact_manifest.json:1–24; provenance/phase18_remediation_log.json:1–19). Findings distinguish generated claim, source sentence, access, cluster and cite key (evidence/section_02_evidence_package.json:1–58).

Final PASS preserves bibliography shortfall; 403 insufficient-evidence citations remain in authored verification results. These are inspectable workflow outputs, not independent proof of validity. Static viewer loads section 02–13 althoughsection01 is also retained (plugins/evidence-explorer-plugin.mjs:138–158). Selective provenance cannot reconstruct host session or original DB/kernel. No reference code/tests ran; all 23 cells are scoped to the artifact role.


## scientific-agent-skills

This 181-skill pack supplies host-loadable descriptions, references, and ordinary Python helpers. Frontmatter and plugin metadata are discovery input; autoskill loads name/description from */SKILL.md and ranks cosine matches (plugin.json:1–22; skills/autoskill/scripts/match_skills.py:5–46). No provider turn loop or host reload enforcement is supplied. Public database lookup is a bounded endpoint/filter/pagination/provenance contract, not a standing app DB (skills/database-lookup/SKILL.md:1–38).

Autoskill is a concrete improvement proposal primitive: fetch an explicitly requested observation window from separately managed screenpipe, redact, cluster, match existing skills, call a selected backend, then stage reuse/compose/novel outcomes and report (skills/autoskill/scripts/run.py:68–132). Synthesis validates verdict/name/body; promotion merely moves a folder and refuses overwrites (synthesize.py:57–79; promote.py:14–32). No performance experiment evaluates the draft, and no harness refit occurs. The fake-service end-to-end tests check expected patterns and output locations, not skill usefulness (tests/autoskill/test_e2e.py:146–199).

Representative DICOM UID validation checks bounded schema, collisions, structural UID protection and keyed derivation, emits aggregate failures and explicitly limits its result: consistency does not prove deidentification or referential completeness (skills/pydicom/scripts/uid_mapping_validator.py:57–143). All 23 cells respect this support role; no acquired code/tests ran. Arconaut can study proposal generation and domain oracles while adding a separate model-governed evaluation/activation program.


## bionemo-agent-toolkit

This toolkit supplies host skills and scientific service clients, not a scientific agent engine. Its generator discovers leaf SKILL.md directories, excludes vendor/evaluation containers and checks coverage/freshness of the generated aggregate (scripts/plugin_sync.py:1–93). The pipeline skill orders GenMol→DiffDock→Boltz2, while existing configured services are consumed directly and deployment remains separate (nim-skills/meta-skills/drug-discovery-pipeline/SKILL.md:20–50; proteinmpnn-nim/SKILL.md:26–75).

The concrete ProteinMPNN helper validates request mode and input, reserves a new output directory atomically, saves request JSON and exact HTTP response bytes, distinguishes native/design rows and aligned scores, and saves exact FASTA plus a summary only after valid 200 completion (scripts/design.py:24–173 within proteinmpnn-nim). A 202 response is a failure rather than a tracked job; connect/read timeout does not cancel server computation. Exact-byte/error/count/native-alignment and controlled concurrent-reservation tests have direct literal oracles (tests/test_proteinmpnn_design.py:42–89,104–148,169–201).

Evaluation boundaries matter: hosted skill lift mixes trajectory checks with LLM judgments. A shipped local Harbor grader checks request/output shapes, endpoint strings and seeded local replay with partial credit, while README still labels local coverage TODO/unsupported and some models pending after timeouts (README.md:62–88; local grader:102–212). Neither replay nor client conformance proves biological design efficacy. No downloaded code/tests ran; all 23 cells retain these boundaries. Arconaut should study exact service IO and alignment oracles while separately specifying remote identity, cancellation and independent outcome evaluation.


## allen-openai-tools

This experimental publication-processing library supplies text/PDF summaries, embeddings and a local diskcache, not a coding agent. LocalDatabase exposes keyed save/load/list/reset and class-dictionary restore (src/papers_extractor/database_parser.py:33–123). That is a useful program-accessible storage primitive but not a model standing SQL service or Science engine implementation. The restore guard checks the outer key instead of the attribute key when intending to skip the database pointer. LongText cache identity includes text/chunk size but no final-summary target or model/prompt version (long_text.py:16–62,141–212).

Chunk chat requests run in an asyncio worker pool with fixed model, slot-ordered results, retries and increasing per-attempt timeouts. On exhausted/other failures, branches set abort and acknowledge the queue but fall through into response processing; workers are canceled without being awaited (openai_parsers.py:153–288). These source failure paths do not establish quiescence suitable for refit. Summarization repeats until a chunk target with no reduction-round/progress bound; this transforms literature, not active agent context. Optional files save assembled chunk prompts and text results rather than original provider attempts/failures/raw envelopes (openai_parsers.py:358–394).

CRUD and chunk tests contain literal oracles. Model tests require live credentials, expect exact prose, and cache tests compare persisted fields to the same run output without checking call suppression or parameter invalidation (tests/test_database_parser.py:12–65; test_openai_parsers.py:9–127; test_unique_paper_database.py:63–97). Nothing was imported/run. All 23 cells reflect the library role. Arconaut questions concern versioned result identity, original attempted IO, and actual awaited cancellation.


## Family synthesis

The most useful distinction is between an action's scope and its lifetime. A scientific workbench can expose arbitrary code without providing shared service continuity; a notebook can retain values without retaining original executions; a review can retain source sentences without retaining the process that produced them. The completed rows separate those claims rather than treating “AI scientist” as one architecture.

| Reference role | Actual computation or data boundary | Audit and continuation boundary |
| --- | --- | --- |
| Claude Science official product contracts | Temporary Python/R kernels, reusable environments, remembered facts, separately lived remote compute | Documented command/artifact versions; engine source unavailable; reviewer scope excludes rerunning/method-choice assessment |
| K-Dense BYOK workbench | Pi-hosted turns, shared project files, specialists, durable asynchronous Modal handles | Authored notebook plus bounded observed/inferred provenance; terminal completion may proceed after flush failure; ordinary turn restart ends work |
| OpenAI4S independent engine | Owned persistent worker slots, inner Python host RPC, real scoped read-only SQLite query; remote compute consumed separately | Action ledger plus transactional compaction originals/cursors and partial replay recovery; redaction/clipping and detached provider requests remain limits |
| Finch notebook environment | Local episode kernel/full notebook reruns or fresh Docker execution | Latest notebook projection can overwrite history/repeat effects; temporary workspace and kernel close at episode end |
| PaperQA domain agent and corpus | Persistent indexed sources; bounded evidence search/summarization and model-selected tools | Reset/reacquisition repairs task evidence; tool/accounting history is partial; index recovery is not agent refit |
| Robin remote workflow | Local stage/trajectory orchestration consumes hosted Edison tasks by IDs | Hosted execution is unavailable; remote IDs survive local waiting timeout but local result state is saved only at final stage |
| Agent Laboratory / AI Scientist v2 | Fresh experiment processes, role/phase or branching candidate-code search and generated evaluation | Task pickles/journals retain selected history; code repair is not original context repair; direct-child/future cancellation limits matter |
| Computational review template / VIP | Authored role/gate/version protocol and per-section evidence files; VIP is its produced v27 review | Selected source/gate/remediation provenance, static viewer; host enforcement and full original session are unavailable |
| Scientific skills / BioNeMo / Allen tools | Host-loaded instructions and supplied client/data primitives; separately consumed services or local publication cache | Autoskill drafts/promotes without usefulness eval; BioNeMo exact HTTP bytes are narrow original IO; Allen mutable snapshots omit complete attempts |

### Execution, authority and change

K-Dense offers a concrete turn-adjacent activation boundary: its claim/gate stays held through run finalization. That is useful evidence for activating changes after current work finishes, but provenance flush can fail while the terminal state still advances. OpenAI4S provides stronger identities around request attempts, kernel generations and action tickets; source/read-only access does not alone solve provider quiescence because canceled network work can remain detached. AI Scientist's timed-out Future is logged and GPU assignment released without canceling the underlying operation. Agent Laboratory terminates its immediate experiment child rather than proving descendants are stopped. These are distinct counterexamples to “the chat stopped, therefore refit is safe”; see the action lifecycle/evidence records in their rows.

None of the inspected paths establishes the operator's complete compiled outpost refit and re-inhabitation contract. Checkpoints, index recovery, remote job handles, selectable models and tool definitions are useful components, with their actual limits recorded. The study deliberately marks task checkpoint/remote follow-up as limited continuity, and sequential roles/journal aggregation as limited collaboration. They must not become affirmative live-peer or executable-refit claims.

Model agency also varies by layer: general executable cells, dynamic host tools, constrained scientific operations, arbitrary generated experiment code, and authored skills are different surfaces. Template plan/signoff requirements and toolkit data-transfer prompts are historical product contracts; they neither bind the current workspace nor become an Arconaut approval design. Scientific skill changes and experiment code replacement are not evidence of universal hot engine replacement. K-Dense distinguishes future-session settings from ongoing frames; other dynamic paths require similarly explicit activation timing.

### Persistent services and context

For Arconaut's consumer boundary, the promising lesson is explicit external identity rather than resident ownership. K-Dense remote handles and Robin Edison task IDs represent separately lived service work. BioNeMo's configured URL contract explicitly tells the client to consume an existing service rather than start another container. Claude Science environment reuse is not evidence of a shared kernel; Finch episode kernels and OpenAI4S session worker slots have narrower lifetimes. The local Allen cache, public database lookup, review evidence JSON and OpenAI4S queryable Store have different schemas, access semantics and owners; none transfers source certainty into Claude Science's proprietary standing-state implementation.

Context repair should preserve the distinction between original retrieval and ordinary regeneration. OpenAI4S covered-through cursors and digest-addressed summarized slices provide specific originals/lineage. K-Dense source/correction/hash/coverage recall provides useful but bounded recovery; opaque commands, nested children and overwritten bytes constrain it. PaperQA can discard/reacquire unsuitable evidence from indexed sources. Finch latest-state projection, Agent Laboratory cleared role histories and AI Scientist code debugging do not reconstruct lost original conversation. The template's same-cell task-construction rule is a studied host compaction failure/workaround, not an implemented general repair policy.

### Audit and oracles

Original audit cannot be inferred from impressive publication provenance. VIP supplies claims/source sentences, gates, edits and selected version/path/hash records; its final PASS preserves both a target exception and residual insufficient evidence. The template builds a static browser projection from section02–13 and ignores the optional combined JSON database. Those outputs are worth studying as projections linked to sources. They are not a complete original provider/action/transformation stream.

BioNeMo's ProteinMPNN helper gives a narrow, direct example: exact HTTP response bytes, request payload, native/design identity, score alignment and failure outcomes are retained, with literal/malformed-input and controlled race oracles. It still lacks a remote job/cancellation handle and rejects202. K-Dense authored notebook statements and bounded harvested provenance can disagree with actual effects; a matching later hash stays unknown and flush loss is possible. OpenAI4S compaction archives/cursor transactions are stronger for reconstruction, while redaction and clipped output still constrain the original-byte claim. Allen optional text files retain chunk prompt/result text rather than every provider attempt or raw envelope.

The main scientific oracle problem is independent outcome validity. Model-generated scores and same-model reviewer personas in Agent Laboratory, generated metric parsers and reward-oriented experiment search in AI Scientist, consensus over Robin trajectory files, PaperQA declared sufficiency and the template's source/count/size heuristics do not become independent scientific truth. BioNeMo seeded replay tests reproducibility and shape, not biological efficacy; endpoint strings in a trajectory are weak execution evidence, and local grader support is explicitly unresolved in its README. Autoskill fake-service tests validate proposal plumbing, not whether a new skill improves work. Allen exact live-model prose tests are brittle and same-run cache equality does not prove invalidation or suppressed provider calls.

### Questions to carry into specification

1. Define operation identity, client ownership and actual quiescence across kernels, provider transports, descendants and separately managed jobs. A client may pause its own activity while a shared service continues; specify what can safely remain outstanding at refit.
2. Define the original audit stream before its notebook, evidence, complaint, workflow and research projections. Keep originals/attempts/transformations and causal links available for repair, with explicit limits for opaque programs and external effects.
3. Give workflows executable role/input/output/activation contracts, including independently inspectable barriers where intended. Prompt-defined role separation and model judgment require different oracles from mechanical transitions.
4. Make autodroit's proposal, experiment, evaluation, adoption and rollback mechanisms separately inspectable. Preserve model governance/operator steer without mistaking code editing, a folder promotion or scientific candidate search for evaluated harness improvement.
5. Treat result/cache identity and uncertainty as first-class data. Model/program/parameter versions, source digests, observed-versus-declared provenance and unresolved evidence should survive recall and publication.

These are discriminating research/specification questions, not a dependency adoption, language selection or implementation plan. The study stays within the pinned fourteen-reference family; the larger corpus matrix is integrated by the parent project.
