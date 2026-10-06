# Science workbench acquisition

2026-09-30. Frumentarius acquisition lane for Arconaut. Reference acquisition does
not adopt dependencies, select an architecture, or begin product implementation.

## Written subplan

1. Read the current doctrine, reconstruction instructions, quarantine manifest,
   and existing Claude Science findings.
2. Acquire exact bytes of the official documentation index, announcement, and
   all relevant Claude Science documentation reachable through the official index.
   Inspect linked official examples, templates, skills, and distribution endpoints.
3. Preserve publicly obtainable source or distribution artifacts when available,
   without installing or executing acquired material, logging in, accepting terms,
   using credentials, or spending money. Coordinate repository acquisitions with
   the agent-discovery owner rather than downloading duplicate source archives.
4. Keep raw bodies, response metadata, source URLs, retrieval times, SHA-256
   identities, and clearly labeled readable extracts in clean ignored quarantine.
   Preserve an intact restorable ZIP on the shared archive shelf.
5. Compare all restored file bytes directly with the ZIP and report obtained
   material, exact coverage, unavailable material, and operator shopping needs.
   The root owner integrates top-level manifests, journal, and beads.

## Acquired material

Claude Science is now represented by actual public **product distributions**, its
complete indexed Science documentation, and published example outputs. The source
of the Claude Science application itself was not acquired. These categories must
stay distinct when studying its behavior.

* Four official Claude Science **0.1.55** artifacts, release **3848a04a**: Linux x64
  binary, macOS ARM64 and Intel DMGs, and Windows x64 EXE. Together they contain
  457,009,634 downloaded bytes. All four match the SHA-256 values in the exact
  preserved vendor release manifest. They are every product download linked from
  the captured [official product page](https://claude.com/product/claude-science).
* All **29** Claude Science Markdown documents listed in the exact captured
  [official documentation index](https://claude.com/docs/llms.txt), including
  execution environments, artifacts, reviewer, memory, remote jobs, CLI,
  configuration, administration, network requirements, and changelog. All returned
  HTTP 200. Crosslinks inside those documents revealed no additional unindexed
  Science document. This is complete coverage of that index's Science section,
  not every generic Claude support, legal, API, or forum page.
* Exact official announcement and product/landing page bodies, Modal and NVIDIA
  integration announcements, Linux and Windows installer scripts, the public
  release pointer, and the release manifest.
* Seven public HTML artifact examples linked by the official product page:
  literature review, SAMOSA viewer, scVI screen, KRAS inhibitor series,
  rhodopsin spectral tuning, Tabula Sapiens atlas, and protein dashboard. These are
  published output bodies, not complete copies of the generating analysis or
  its execution environment. Referenced network assets have not all been mirrored.
* Jérôme Lecoq's own public
  [Computational Reviews article](https://www.linkedin.com/pulse/computational-reviews-building-ai-agentic-workflows-doubt-lecoq-9gurc),
  which reveals the actual published source for the custom evidence-database
  workflow discussed in our earlier research.

There are 52 exact HTTP bodies. Source/final URLs, request time, headers, lengths,
SHA-256 identities, scope, and the vendor-manifest comparisons are retained in the
[acquisition catalog](2026-09-30-science-workbench-acquisition.json). Actual UTC
retrieval times fall on October 1, while the operator's local date is September 30.

The readable entry point is [quarantine/claude-science/INDEX.md](../quarantine/claude-science/INDEX.md).
Original Markdown bodies are immediately readable under `raw/`; five selected
HTML pages also have explicitly labeled derived plain-text extracts. Original
bodies remain unchanged. Historical reference instructions have no current
workspace authority.

## The standing evidence-database workflow is available

The earlier frumentarii report correctly distinguished the announcement's account
of a custom workflow from a public API to the app's internal database. This
acquisition adds the missing primary implementation reference. Lecoq publicly
links [AllenNeuralDynamics/ComputationalReviewTemplate](https://github.com/AllenNeuralDynamics/ComputationalReviewTemplate)
and [ComputationalReviewVIP](https://github.com/AllenNeuralDynamics/ComputationalReviewVIP)
from his article. The template's current README describes role-specific skills,
a staged actor/critic pipeline, evidence packages, citation validation, and
provenance directories. These are study claims and instructions, not results we
have verified by executing the pipeline. The author explicitly describes the
reviews as early and largely unvalidated in his article.

The discovery frumentarius acquired both exact sources and compared them directly
with their preserved ZIPs:

* [ComputationalReviewTemplate](../quarantine/computational-review-template/README.md),
  revision `7312d15c12443031d9806a0550a4e665bd45ca92`: 55 retained files.
* [ComputationalReviewVIP](../quarantine/computational-review-vip/README.md),
  revision `a04f01d37994c6a14e4bc7ec1b3d0a869144e98d`: 159 retained files.

Direct inspection of the acquired [evidence format](../quarantine/computational-review-template/evidence/README.md)
shows persistent per-section JSON packages containing source sentences, effect
sizes, study systems, replication/conflict information, and figure inputs. The
Evidence Explorer consumes those per-section files; it does not directly read
the optional combined `evidence_database.json`. The acquired VIP example contains
13 section evidence JSON files. This makes the authored workflow and its actual
structured outputs inspectable; it does not expose the Science application's
internal standing database, host RPC implementation, or kernel machinery.

That lane also owns the older linked
[AllenInstitute/openai_tools](https://github.com/AllenInstitute/openai_tools),
[NVIDIA BioNeMo Agent Toolkit](https://github.com/NVIDIA-BioNeMo/bionemo-agent-toolkit),
and the independently implemented
[OpenAI4S](https://github.com/PKU-YuanGroup/OpenAI4S). Their pinned source results
and restoration records belong to that lane's catalog. None is the proprietary
Claude Science application source; the template is the directly relevant custom
scientific workflow. The independently implemented [OpenAI4S source](../quarantine/openai4s/README.md)
is also now acquired at `9f20ef8d89c195487e41d39942a146159f190c0c`, with 4,092
retained files directly compared by the discovery owner; it provides a separate
harness implementation to study, without claiming it reproduces the vendor app.

## Read-only behavior findings for later execution-model study

The current [tools and environments documentation](https://claude.com/docs/claude-science/tools-and-environments.md)
explicitly says Python and R execute in persistent kernels retaining variables
between session steps. Kernel life ends after approximately 30 minutes idle,
a relevant environment restart, or session end. Named package environments are
shared across local projects. This is stronger evidence for a persistent execution
surface than an ordinary shell-call loop, but it does not establish crash recovery
of kernel memory or shared multi-client kernels.

The [artifact documentation](https://claude.com/docs/claude-science/artifacts.md)
distinguishes saved versioned outputs from temporary session files. Its execution
log takes precedence when a generated reproducibility script disagrees, and
artifact versions retain conversation, code, environment, execution and review
information. Those retention boundaries are useful study material for Arconaut's
original audit and managed context requirements; they do not demonstrate a complete
raw provider/process audit.

The [core concepts](https://claude.com/docs/claude-science/core-concepts.md) describe
local remembered facts, per-session delegation, and standing access grants.
[Remote compute](https://claude.com/docs/claude-science/compute-providers.md) runs
jobs on the consumer's own Modal account and notes that closing the app leaves
remote jobs running. The [CLI](https://claude.com/docs/claude-science/command-line-settings.md)
and [configuration reference](https://claude.com/docs/claude-science/configuration-file-reference.md)
make several controls inspectable outside the UI, while the configuration file
is read at startup. These are documented contracts; no runtime conformance check
was performed.

## Preservation and restoration

The intact ZIP is on the shared archive shelf:
`../quarantine_proj/archives/arconaut-science-workbenches-2026-09-30/claude-science-0.1.55-3848a04a-public-reference.zip`.
Its identity, size, retained-file count, exact source metadata, and the restoration
check result are in the acquisition catalog. Restore with an absent destination:

```sh
python3 scripts/ingest_zip.py ../quarantine_proj/archives/arconaut-science-workbenches-2026-09-30/claude-science-0.1.55-3848a04a-public-reference.zip quarantine/claude-science --root claude-science
```

The ZIP was restored through the actual current stdlib importer into an absent
isolated temporary directory. All **59 files**, **465,078,050 retained bytes**,
and executable bits matched the acquired shelf directly. The temporary
restoration was removed afterward. The compressed ZIP contains 344,942,737 bytes
and has SHA-256 `9818caede4dc1845956083aa2872992d8a2db610573bc97c1d794784f838947e`. No nested Git metadata, symlinks, or filesystem
detritus belongs in this reference. The original downloads stay intact.

No acquired installer, script, binary, or example was executed. No DMG was mounted.
Nothing was installed, no product session was started, and no account, credentials,
terms acceptance, subscription purchase, or live paid model call was used.

## Acquisition limits and shopping needs

No official app source repository was found in the inspected official product
page, documentation, announcement, or targeted search. If application internals
become necessary, the operator would need vendor source/architecture access;
there is no claim that the downloaded distribution is an open source checkout.
The app's bundled Example project, installed Featured skills, local database
schema, and active session state have not been exported or inspected at runtime.
A later isolated authenticated product session could obtain relevant exports if
we decide they materially answer a design question. Those are conditional study
needs, not blockers to bringing the publicly available product home now.
