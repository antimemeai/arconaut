# Context Language Models: live context as an editable program surface

Recorded 2026-10-01. Operator supplied the official repository while selecting
C++ with Lua for Arconaut. This is source study, not dependency adoption or a
compaction-policy selection. No acquired program, test, installer, model call or
provider credential was used.

## Design consequence

Expose live context itself to model-authored Lua programs and ordinary file tools.
A model should be able to select, reorder, annotate, replace, remove and restore
material, define its own reusable transformations, and experiment with different
representations. Context repair may increase size. A fixed menu of summarization
commands would miss this paper's useful idea.

Keep three things distinct: original events, editable context revisions, and the
actual provider request produced from a revision. C++ provides operation identity,
original-event capture and consistent publication; Lua supplies transformations,
assembly, policy and model-facing interfaces. The concrete API remains to specify.

## What the acquired source does

All source references below are relative to `quarantine/context-language-models/`
at **18dc11115f50f261233c5bba7937834491e307e8**. The
[immutable upstream tree](https://github.com/facebookresearch/context-language-models/tree/18dc11115f50f261233c5bba7937834491e307e8)
is the public source studied, not an inferred complete experimental harness.

| Mechanism | Source evidence | Implication |
| --- | --- | --- |
| Before each Bash command, write editable history to a file; after the command, read changes and replace the message list. Append the current assistant call and result afterward. | `clm/clm_harness/context_env/env.py:113–125,182–214,260–315`; `clm/clm_harness/clm_agent/harness.py:612–622` | This file edits real subsequent inputs, not merely a memory note. It is a command-boundary mirror, not an always-authoritative asynchronous file. |
| System and initial task stay outside the editable region. Parser ignores displayed turn numbers, drops empty sections and merges adjacent same-role messages. Historical tool structure is converted to text; only assistant roles survive as assistant, all other editable roles become user. | `context_utils/context_string.py:63–168`, under `clm/clm_harness/` | Learn the programmability, avoid copying this lossy role/tool/attachment representation into our canonical context model. |
| Default fit gate permits growth only within a nonzero budget; shrink gate rejects growth. Non-growing edits can remain over budget. Missing/unreadable file silently returns no edit. | `context_env/edit_gate.py:38–64`; `context_env/env.py:120–125,269–315` | Repair should not be intrinsically shrink-only. Make rejected, missing and malformed edits observable; budget policy belongs in editable programs. |
| Next loop evaluates budget, nudges or rolls back history; request snapshots precede the model call. | `clm_agent/harness.py:518–564` | Context data edits can affect the next request under the current program. Replacing the governing program is a different operation, subject to Arconaut's agreed turn/workflow completion default and interrupt-and-apply operation. |

## Original trace and repair limitations

The source has useful context-epoch recording. ATIF-CTX segments carry their
input context and branch relation (`agent_trajectory_format/README.md:3–17`).
This enables studying how the presented context changed rather than pretending
all requests replay one append-only conversation.

It does **not** establish the comprehensive original audit we require:

- `env.py:147–170,231–237` combines stdout/stderr and truncates output before
  it enters history; no separate raw-output persistence was identified in this
  inspected agent path. Harbor may have independent environment logs; that was
  not assessed here.
- `harness.py:374–392` describes competing snapshot writers overwriting names;
  `utils/tokens.py:153–179` makes recording best effort. Final
  `trajectory.json` is the retained, edited message list (`harness.py:648–656`).
- Snapshot capture at `harness.py:559` precedes an Anthropic trailing-assistant
  role conversion inside `_query_with_retry`, lines 750–763. The snapshot is
  therefore not necessarily the final provider-facing message list, much less
  the complete wire request after library transformations or every retry.
- A rolled-back response absent from later snapshots is represented by a
  contentless stand-in (`agent_trajectory_format/harness_export.py:276–302`).
  The protected rollback ledger only retains the last five abbreviated commands
  (`harness.py:418–449`). Those mechanisms preserve accounting or retry hints,
  not original attempts in full.

Consequently, record raw outputs and provider exchanges before reduction;
record context edits, failed edits, repair source references and resulting
revisions separately. An edit changes future presentation; it cannot undo a
command already run. Repair should retrieve originals and previous revisions
instead of trusting an already-damaged summary. Requests need an immutable
context revision plus the exact final request and attempt identity.

## Multi-agent and continuity boundaries

The paper describes multiple context files, swarms and nested workers. Public
ATIF-CTX models include nested subtrajectories and parent spawn/fold links
(`agent_trajectory_format/models.py:215–260`). However, this released harness
explicitly rejects `subagents`, `n_subagent_slots`, `ctx_archive` and other
experimental options (`harness.py:108–124`). A recording schema and paper results
are not an acquired live collaboration implementation.

For Arconaut, each colleague needs its own editable context lineage; selected
material can cross to an outpost with source links. Concurrent edits require
revision checks or serialized publication, rather than silently overwriting one
another. A running request keeps the revision it started with. Refit transfers
context and custody only after the already-agreed quiescence boundary.

Their restart code restores files and conversation while explicitly warning
that processes died (`utils/resume.py:55–89`); it drops unanswered tool calls
rather than inventing results (122–137). Useful honesty, but unlike our paused
standing work and shared-service/outpost design.

## Research value and limits

[The paper, v1](https://arxiv.org/html/2609.37725v1), section 4.1, makes model-authored
context functions central. Sections 4.2 and appendix E describe evolving textual
skills and checking held-out tasks. That is a useful autodroit experiment lane:
optimize task outcomes and actual cost, not edit frequency or bytes removed.
The released evolution utility defaults development tasks to training tasks
when omitted (`clm/clm_icl/evolve.py:7–21`), so separation must be deliberate.

The authors report favorable accuracy/compute comparisons in particular research,
coding and optimization settings; no benchmark was reproduced here. These results
support testing the surface, not prescribing default compaction. ContextBench is
marked forthcoming in the repository. Ordinary provider caching can lose reuse
after an early edit. Suffix Cache Reuse is an optional serving-side research
mechanism that retains stale suffix states (`suffix_cache_reuse/README.md:5–12,
39–63`), changing inference semantics; it is not a transparent client optimization
or an Arconaut requirement.

The code is Python/Harbor/LiteLLM and licensed CC BY-NC 4.0. Study patterns only;
this does not change the selected C++/Lua production stack.

## Acquisition and restoration

[Exact acquisition catalog](2026-10-01-context-language-models-acquisition.json):
75 retained files, 1,686,597 retained bytes, no omitted entries; retained file set,
bytes and executable bits compared directly against the intact source ZIP.

Archive: `../quarantine_proj/archives/arconaut-context-language-models-2026-10-01/context-language-models-18dc11115f50.zip`.
SHA-256: `2a98b3cb3abf226f05d7ba878d121949aa569d74248e67be94b7463ad69f6401`.
Original download: `https://codeload.github.com/facebookresearch/context-language-models/zip/18dc11115f50f261233c5bba7937834491e307e8`.
Restore from Arconaut root, with destination absent:

```sh
python3 scripts/ingest_zip.py ../quarantine_proj/archives/arconaut-context-language-models-2026-10-01/context-language-models-18dc11115f50.zip quarantine/context-language-models --root context-language-models-18dc11115f50f261233c5bba7937834491e307e8 --skip-symlinks
```

Paper preserved intact as ignored `papers/2609.37725v1-context-language-models.pdf`,
from `https://arxiv.org/pdf/2609.37725v1`; SHA-256
`ac5c926814434d8ad2f499183cd2a81e53b4c14f327587f99980ade9157db01b`.
