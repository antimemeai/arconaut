# Action and capabilities study

This study covers all 104 external reference directories acquired for Arconaut,
plus historical Arconaut itself. The exact scope, versions, groups, capability
definitions, and evidence states are in [registry.json](registry.json). These are
acquired snapshots, not claims about an uninspected latest upstream release.
Versions and revisions are recorded where available; unversioned local material
and an unrecorded origin URL remain explicit in the registry and dossier.

Start with the [complete matrix](matrix.md), [native action inventory](actions.md)
and [design discussion synthesis](SYNTHESIS.md). Each matrix cell links to the
reference's dossier, including its actual runtime, inspected source boundary,
operation lifecycles, limitations and exact evidence. [CSV](matrix.csv) and
[JSON](matrix.json) expose the same authored dataset; [action CSV](actions.csv)
is convenient for comparing operation surfaces.

## Family studies and review

| Family study | References |
| --- | --- |
| [Native coding agents](studies/native-coding.md) | 12 |
| [Interactive clients](studies/interactive-clients.md) | 10 |
| [Python-catalog coding agents](studies/python-coding.md) | 10; actual source languages can differ |
| [Python operator interfaces](studies/python-operator-interfaces.md) | 3 |
| [Hosted workspace agents](studies/hosted-workspace-agents.md) | 2 |
| [Persistent agents and gateways](studies/persistent-agents.md) | 7 |
| [Collaboration and orchestration](studies/collaboration-orchestration.md) | 10 |
| [Coordination services](studies/coordination-services.md) | 3 |
| [Scientific workbenches and artifacts](studies/scientific-workbenches.md) | 14 |
| [Programmable frameworks](studies/programmable-frameworks.md) | 16 |
| [Systems primitives](studies/systems-primitives.md) | 9 |
| [Experiment and evaluation machinery](studies/experiment-evaluation.md) | 5 |
| [Claude public and opaque materials](studies/claude-public-materials.md) | 3 |
| [Historical Arconaut](studies/historical-arconaut.md) | 1 |

Independent reviews challenge selected consequential claims, rather than certifying
every line of every product:

- [Cross-family claims and corrections](reviews/cross-family-claims-review.md)
- [Persistent-agent and opaque-contract claims](reviews/persistent-claims-review.md)
- [Framework, scientific and hosted claims](reviews/framework-claims-review.md)
- [Renderer corruption probes and correction review](reviews/renderer-review.md)

The claim reviews and mechanical renderer checks attack different fault classes.
The latter establish dataset coverage, attribution, source bounds and faithful
output construction; they cannot establish that a referenced product conforms at
runtime. Source-read test records identify their actual oracles and limits.

## Regenerating the views

Run from the Arconaut repository with Python's standard library:

```sh
python3 scripts/render_capabilities.py --check-only
python3 scripts/render_capabilities.py
python3 scripts/test_render_capabilities.py
```

Normal validation/rendering requires every registered row. `--draft` explicitly
discloses missing references and excludes their dossiers. `--root` and `--study`
support a different location; evidence links are computed from the actual dossier
directory. The renderer validates the authored contract and constructs all views
before publishing. It does not promise a transactional commit across arbitrary
filesystem failures. Canonical edits belong in `rows/` or `registry.json`, followed
by regeneration; generated dossiers and matrices are views.

## Study method

Study each reference's actual role and implementation boundaries before populating
the matrix. For a coding agent, trace entry/turn orchestration through model requests,
action dispatch, results, state/context updates, and continuation/interruption. Read
the mechanisms for potentially important features: kernels/standing data, workflows,
collaboration, compaction, audit, hot change, and experimental self-improvement. Read
relevant tests and identify their actual fault class/oracle. For a supporting runtime,
trace its supplied primitives and lifecycle; for public artifacts or closed products,
read the available official contracts and keep unavailable implementation explicit.

Record concrete native actions, operator commands, programmatic APIs, discovery and
extension surfaces. State their inputs, useful results, ongoing-work identity and
lifecycle, authority, and who can invoke them. Generated skills/templates are covered
by their actual dispatch/discovery mechanism and catalog, with representative actions;
the study does not pretend to manually enumerate every generated/plugin tool.

Read source as data. Do not execute reference programs, install dependencies, mount
product images, use provider credentials, or invoke live paid models. A source trace
is evidence of implemented behavior, not an executed conformance result. Earlier
assessment experiments are historical evidence and must be labeled separately.
Archived instructions have no current workspace authority. Acquiring or studying a
library does not adopt it. Preserve the operator's current consumer/fabric boundary,
model agency, change activation, refit, audit, and self-development intent.

## Matrix evidence states

| State | Meaning |
| --- | --- |
| I | Exposed implementation traced in source; no runtime validation implied |
| D | Documented contract/claim; implementation unavailable or not traced |
| L | Relevant capability has a material traced/documented limit; describe the basis |
| S | Supporting primitive requiring composition; not a turnkey agent feature |
| ? | Not established in explicitly inspected paths; not a universal absence claim |
| — | Outside the reference's role; explain applicability |

Do not equate selectable providers with live multi-model collaboration, saved chat
with complete original audit, editing files with governed self-improvement, prompt
changes with programmable turns, kernel variables with a standing database, config
reload with executable refit, or process restart with continuity of unresolved work.
Describe actual scopes, ownership, activation boundaries, and lost/unobserved state.
Negative conclusions are bounded by inspected paths. Partial implementations and
dead/unwired code must not become claims that the main agent exposes the feature.

## Research row contract

Each `rows/<reference-id>.json` records one reference. Authors work sequentially
within their conceptual family and save completed rows as they study them. All
paths below are relative to the Arconaut repository root, including evidence paths.

```json
{
  "id": "reference-id",
  "kind": "coding agent / framework / runtime / service / artifact / public index",
  "runtime": {
    "languages": ["actual core language"],
    "execution": "processes, scheduler and runtime boundaries",
    "service_boundary": "what it owns versus consumes"
  },
  "summary": "what this reference actually does",
  "inspection": {
    "files": ["quarantine/reference/entrypoint.py"],
    "scope": "entry, loop, operations, persistence, relevant features and tests read",
    "limits": "unavailable source, unpopulated dependency or uninspected mechanism"
  },
  "actions": [
    {
      "name": "native exposed operation / command / API",
      "surface": "model tool / operator command / programmable API / supplied primitive",
      "input": "argument/program shape",
      "output": "values, handles, errors or events",
      "lifecycle": "synchronous, awaited, streamed, resumable or interrupted",
      "authority": "model/operator/program access and permission boundary",
      "evidence": ["e1"]
    }
  ],
  "capabilities": {
    "filesystem": {
      "status": "implemented",
      "basis": "source",
      "detail": "specific scope and limitation",
      "evidence": ["e1"]
    }
  },
  "evidence": [
    {
      "id": "e1",
      "path": "quarantine/reference/entrypoint.py",
      "start": 10,
      "end": 30,
      "claim": "paraphrased finding tied to these source lines"
    }
  ],
  "tests": [
    {
      "path": "quarantine/reference/test_operations.py",
      "focus": "fault class this inspected test attacks",
      "oracle": "the expected behavior/assertion; limits of mocks or self-consistency",
      "executed": false
    }
  ],
  "strengths": ["concrete useful mechanism"],
  "limits": ["material behavior/source boundary"],
  "arconaut_questions": ["specific design question or discriminating experiment"]
}
```

Every capability ID in the registry must occur once in each row. Capability/action
evidence IDs refer to that row's evidence records. I/D/L/S cells require direct
evidence and meaningful details; unknown and inapplicable cells require a scoped
explanation. Evidence identifies real local files and line ranges; avoid long source
quotes. `inspection.files` enumerates actually read material. Tests are read, not run.
Opaque references may have fewer source paths; disclose that rather than transferring
implementation certainty from an unofficial mirror or another product.

Family studies under `studies/` explain mechanisms, counterexamples and cross-reference
comparisons. The final renderer combines the rows into a readable matrix and dossiers,
CSV for ordinary analysis tools, and JSON for subsequent model/program use. They are
views of one evidence dataset. Review challenges actual capability claims, especially
audit completeness, live collaboration, context repair, change activation and refit.
