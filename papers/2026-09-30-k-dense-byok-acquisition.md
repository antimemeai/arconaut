# K-Dense BYOK acquisition

2026-09-30. Bounded scientific-workbench follow-up from the official Scientific
Agent Skills README. This reference acquisition adopts no dependency or design.

## Written subplan

1. Verify the product/source link in the publisher's own scientific-agent-skills
   README; coordinate existing source ownership with the discovery frumentarius.
2. Follow official K-Dense BYOK source, product documentation, public examples, and
   freely accessible distributions. Preserve exact downloaded bodies and source
   archives with source URLs, timestamps, headers, and SHA-256 identities.
3. Strip nested Git metadata and filesystem detritus from the clean reference.
   Install or execute nothing, use no credentials or account, and spend nothing.
4. Preserve a restorable intact ZIP and directly compare restored file sets,
   every file byte, and executable bits. Record actual unavailable items and scope
   limits. Stop at the product's relevant public material; do not open an
   unbounded survey of commercial scientific agents.
5. Return acquisition results to root for manifest, journal, beads, and shopping
   list integration. Keep the completed Claude Science catalog unchanged.

## Acquired source and public material

The publisher's [Scientific Agent Skills README](https://github.com/K-Dense-AI/scientific-agent-skills)
directly links [K-Dense-AI/k-dense-byok](https://github.com/K-Dense-AI/k-dense-byok).
This is a public application source repository, with an MIT LICENSE retained in
the snapshot, rather than a documentation-only product reference.

Acquired the complete pinned source at
`ef60be84a27bd2096851ab307514ac3de734214f`: **766 files, 15,745,826 source bytes**,
including backend/frontend implementation, startup scripts, repository tests,
all **27 repository documentation Markdown files**, and the actual catalog of
**326 workflow templates** in `web/src/data/workflows.json`. The source importer
omitted nothing; exact regular-file sets, all bytes, and executable bits matched
the intact codeload ZIP directly. No nested Git metadata was introduced.

Also preserved exact HTTP bodies for the official repository page, the upstream
README that supplied the discovery link, latest-five GitHub release metadata,
and one directly linked [vendor CompBioBench article](https://www.k-dense.ai/blog/compbiobench-three-runs).
Response headers, source/final URLs, request times, sizes and hashes are in the
[separate acquisition catalog](2026-09-30-k-dense-byok-acquisition.json).
Selected HTML bodies have labeled derived text extracts alongside untouched
originals. Actual UTC retrieval times are October 1; the operator's local date
is September 30.

The current official installation guide distributes source with `start.sh`,
`start.cmd`, and `start.mjs`. The five latest inspected releases — `v0.14.2`,
`v0.14.1`, `v0.14.0`, `v0.13.0`, `v0.12.0` — list no separate binary assets.
Source is the acquired product distribution here. Nothing was installed, built,
launched, authenticated, or tested, and no dependency packages were downloaded.

## Useful entry points and scope

Start with the [local reference index](../quarantine/k-dense-byok/REFERENCE_INDEX.md),
[upstream README](../quarantine/k-dense-byok/README.md), and
[complete docs index](../quarantine/k-dense-byok/docs/README.md). Acquisition
metadata and supplementary originals are under `reference-acquisition/`; those
files and `REFERENCE_INDEX.md` are clearly identified acquisition additions.
All other retained root source files remain exactly upstream bytes.

The acquired [architecture document](../quarantine/k-dense-byok/docs/architecture.md)
maps ordinary model/tool turns, event streaming, session identity, and per-project
state to concrete source entry points. It describes a TypeScript/Fastify backend
embedding Pi with a React/Next.js frontend. Multiple chat tabs share project
files; browser reconnect can continue a live turn while the backend remains
running, but a backend restart ends ordinary active turns. Remote Modal jobs have
a separate saved lifecycle. This is a relevant mechanism reference, with no
language or dependency adoption for Arconaut.

The [provenance documentation](../quarantine/k-dense-byok/docs/provenance.md)
distinguishes observed, inferred, and declared file links; bounded opaque-command
scans and child-session harvesting are explicit limits. Append-only tool records,
version-sensitive lineage, hashes, and environment snapshots are inspectable in
`server/src/provenance/`. They do not constitute complete raw execution IO, and
nested child work is not harvested. These are documented boundaries, not findings
from an executed conformance check.

The [lab notebook](../quarantine/k-dense-byok/docs/lab-notebook.md) separately holds
model-authored hypotheses, observations, corrections, evidence links and human
annotations. Its documented context-compaction path adds a bounded notebook/
provenance/pending-work state block to the summary. Workflow templates, specialists,
automation, and source-linked research memory are all present in the acquired
source. The source is available for deeper execution-model study; this acquisition
has not reviewed its entire implementation or validated its advertised outcomes.

Scientific Agent Skills itself remains discovery's independent source acquisition
at `91497e335489dcb544ec8ddc8f6b7ce5fd6d1121` in
[quarantine/scientific-agent-skills](../quarantine/scientific-agent-skills/README.md).
No duplicate skill-source archive was fetched in this product lane. The completed
Claude Science catalog and preserved bundle were left unchanged.

## Preservation and direct restoration check

The exact upstream codeload ZIP remains intact at
`../quarantine_proj/archives/arconaut-science-workbenches-2026-09-30/k-dense-byok/k-dense-byok-ef60be84a27b-upstream.zip`,
SHA-256 `d971e1e9764cb0224bc1fd871e9250d0dc1ee5c82412fe8e92fb047ab6c7a63f`.

The complete source-plus-public-material reference bundle is:

```sh
python3 scripts/ingest_zip.py ../quarantine_proj/archives/arconaut-science-workbenches-2026-09-30/k-dense-byok/k-dense-byok-ef60be84a27b-public-reference.zip quarantine/k-dense-byok --root k-dense-byok
```

Restore to an absent destination. The bundle was restored with the actual current
stdlib importer in an isolated temporary directory, then every file, byte, and
executable bit was directly compared with the acquired shelf. The temporary copy
was removed afterward. No symlinks or filesystem detritus were retained.

The complete bundle contains **774 files**, **16,486,200 retained bytes**;
its ZIP contains 10,013,403 bytes and has SHA-256
`4a0131678e3f5bc25b80b29a3f5f1d80a8708213e2da5612bb50ba47dc02be1b`.

## Limits and shopping needs

No missing public source or documentation blocked this bounded task. Runtime
package dependencies and live provider/compute behavior are outside the acquired
snapshot; reference launchers and tests have not run. Linked tutorial videos remain
recorded upstream links rather than downloaded media. Hosted K-Dense commercial
products and their internals were outside this follow-up. No account, credentials,
terms acceptance, paid service, or purchase was used.
