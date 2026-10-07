# Sources and restoration

Recorded 2026-09-29; reconstruction updated 2026-09-30. References teach patterns;
their code is not incorporated into the fresh implementation. Historical
instructions inside them are not current instructions.

## EDG compiler — 2026-10-01

Official [EDG compiler](https://github.com/edgcpp/compiler) source at
`89e67e07c7f6d0fc38622f9778d8e79f40deaa0f` is retained in
`quarantine/edg-compiler/`: 111,049 regular files (3,549,149,165 bytes) and 12
confined symlinks. The intact 244,854,866-byte ZIP is
`../quarantine_proj/archives/arconaut-edg-compiler-2026-10-01/edg-compiler-89e67e07c7f6.zip`,
SHA-256 `66df655bf9a518932787e265d04b680f3180bd7ee36cc2da15528352029e0947`.
The [exact catalog](papers/2026-10-01-edg-compiler-acquisition.json) records
immutable source, direct archive comparisons and an explicit ingestion override.
The [study](papers/2026-10-01-edg-compiler-study.md) distinguishes a proposed
independent language-checking lane from native compiler/runtime qualification.
No imported build, test, initializer, container or binary ran; no dependency adopted.

Generic ingestion initially omitted 46 legitimate compiler expectation filenames
beginning `._`, plus 12 functional symlinks. All were restored from the preserved
ZIP and compared directly: regular bytes/executable bits and symlink target
strings. Symlink destinations remain within the reference root; three point to
`Local_file.c`, created later by the upstream test checkout. The catalog retains
the initial exclusions and each restoration role. Neither source archive nor
shared ingester was modified.

Restore from Arconaut with the destination absent, then apply these exact
catalog-listed overrides (the generic ingest command alone is incomplete):

```sh
python3 scripts/ingest_zip.py ../quarantine_proj/archives/arconaut-edg-compiler-2026-10-01/edg-compiler-89e67e07c7f6.zip quarantine/edg-compiler --root compiler-89e67e07c7f6d0fc38622f9778d8e79f40deaa0f --skip-symlinks
python3 - <<'PY'
import json
from pathlib import Path
from zipfile import ZipFile
record = json.loads(Path('papers/2026-10-01-edg-compiler-acquisition.json').read_text())['references'][0]
root = Path(record['destination']).resolve()
with ZipFile(record['archive']) as archive:
    for entry in record['ingestion_override']['restored']:
        destination = root / entry['path']
        if destination.exists() or destination.is_symlink():
            raise RuntimeError(f'existing override destination: {destination}')
        destination.parent.mkdir(parents=True, exist_ok=True)
        archived = archive.getinfo(record['archive_root'] + '/' + entry['path'])
        payload = archive.read(archived)
        if entry['kind'] == 'confined symlink':
            target = payload.decode()
            if not (destination.parent / target).resolve().is_relative_to(root):
                raise RuntimeError(f'escaping symlink: {destination}')
            destination.symlink_to(target)
        else:
            destination.write_bytes(payload)
            destination.chmod((archived.external_attr >> 16) & 0o777 or 0o644)
PY
```

After the 2026-09-30 acquisition pass, the shelf has **104 external reference
directories** plus historical `quarantine/arconaut/`. These are references of
different kinds, not 104 independently implemented agents:

| Acquisition | Reference directories |
| --- | ---: |
| Initial three legacy imports and sixteen fresh frumentarii sources | 19 |
| Known agent/assistant/collaboration and supporting source pass | 36 |
| Newly discovered agents, support sources, and produced review material | 46 |
| K-Dense BYOK application source and public material | 1 |
| Claude Science public documentation/examples/distributions | 1 |
| Official Claude Code public distributions and release material | 1 |

## Quarantined Arconaut source

`quarantine/arconaut/` contains the entire assessed source at `3bf2056` plus the
previously untracked `lemonnotes.md`: 182 files. The implementation, Cargo/build/
lint configuration, June design and research documents, original instructions,
old `.beads/` records, and CI have their original source layout. The current
doctrine, assessments, journal, and reconstruction documents remain outside it.
No module has been selected for the new implementation.

The restorable ZIP is
`../quarantine_proj/archives/arconaut-3bf2056-source.zip`. It was generated from
the preserved Git source, with the unchanged untracked note added before
preservation. Its SHA-256 identity is:

```text
01fd58cc02d53de967e0be2ad69feee151efbcbdec46c799c65145c3af39ff03
```

Restore from this repository, with `quarantine/arconaut` absent:

```sh
python3 scripts/ingest_zip.py ../quarantine_proj/archives/arconaut-3bf2056-source.zip quarantine/arconaut --root arconaut
```

On another host, obtain the archive with that identity and use its path. The
reference contains no nested Git metadata or filesystem detritus. All retained
file bytes and executable bits were compared directly with the preserved ZIP.
The large original ZIP and source-history bundle below remain intact.

## Arconaut lineage

| Source | Observed state | Preservation |
| --- | --- | --- |
| `https://github.com/antimemeai/arconaut.git` | Public master `76ddf000657fcaf7feebeccddd20625881a3aa8c` | `origin/master` in this clone |
| `~/projects_old/miscellaneous/arconaut` | Master `3bf2056565eff537b380f4fd649d725f36d1c493`, 15 commits ahead, no configured remote | `legacy-local/master`, ancestor of assessment branch; portable bundle below |
| Same checkout, untracked `lemonnotes.md` | Design proposal not present in Git | `quarantine/arconaut/lemonnotes.md` and the new source ZIP |
| `~/projects_old/miscellaneous/arconaut.zip` | 211,400 ZIP entries; recorded master also `3bf2056` | Original archive left intact in place; not exhaustively compared with checkout |

The complete local master history was saved as
`../quarantine_proj/archives/arconaut-3bf2056.bundle`. This is an intact source
archive, not a reference checkout containing nested Git metadata.

To reconstruct the assessed source on another host, obtain that bundle and run
from a directory where `arconaut` does not exist:

```sh
git clone https://github.com/antimemeai/arconaut.git arconaut
git -C arconaut fetch /path/to/arconaut-3bf2056.bundle master:refs/remotes/legacy-local/master
git -C arconaut switch -c assessment-source legacy-local/master
```

The assessment reports and refreshed instructions are local reconstruction
artifacts, outside this source bundle. The original checkout remains live and intact.

## Reference corpus

[The catalog](papers/legacy-reference-catalog.json) records the locations,
origins, and revisions of all 53 legacy reference directories. Fifty-two contain
Git metadata in the original shelf; those originals are unchanged. Three clean
snapshots were imported here because this assessment directly studies them:

| Directory | Origin | Revision | Imported regular files |
| --- | --- | --- | --- |
| `quarantine/brush-shell/` | `https://github.com/reubeno/brush.git` | `f84ae8a74c43cad85824477d67ab3da5de972558` | 669 |
| `quarantine/mini-swe-agent/` | `https://github.com/SWE-agent/mini-swe-agent.git` | `2afd0fb81bacbf0aacfac9ded6f093c5acd0bf7c` | 220 |
| `quarantine/opencode/` | `https://github.com/sst/opencode.git` | `2006259a02a87edf9e37f253cbddf3188309026b` | 5,663 |

The corresponding ZIPs are in `../quarantine_proj/archives/arconaut-references/`,
named `<directory>-<first-12-revision-characters>.zip`. They were created with
`git archive` from clean original reference checkouts, preserving those selected
revisions. The ZIPs remain intact. Restore from this repo, with destinations absent:

```sh
python3 scripts/ingest_zip.py ../quarantine_proj/archives/arconaut-references/brush-shell-f84ae8a74c43.zip quarantine/brush-shell --root brush-shell --skip-symlinks
python3 scripts/ingest_zip.py ../quarantine_proj/archives/arconaut-references/mini-swe-agent-2afd0fb81bac.zip quarantine/mini-swe-agent --root mini-swe-agent --skip-symlinks
python3 scripts/ingest_zip.py ../quarantine_proj/archives/arconaut-references/opencode-2006259a02a8.zip quarantine/opencode --root opencode --skip-symlinks
```

If a ZIP is unavailable, clone its origin into a temporary directory, fetch the
exact revision if needed, then recreate it with:

```sh
git -C /path/to/temporary-clone archive --format=zip --prefix=NAME/ --output=/path/to/NAME-REV.zip FULL_REVISION
```

The importer was reused from the workspace's Rhizome utility with an explicit
`--skip-symlinks` option. It strips `.git`, `.entire`, macOS detritus, and Python
bytecode-cache directories; symbolic links are omitted for these study snapshots.
It retains regular files and executable bits and refuses an existing destination.
Root license files are retained; omitted symlinks remain in the preserved ZIPs.
No reference automation has been run.

Original reference checkouts remain unchanged on the original shelf. The broader
known-agent acquisition below supplements these three initial imports; the old
catalog still records historical locations and revisions, not current upstream HEAD.
Shared wiki: `../quarantine_proj/xx_wiki/`. Acquisition gaps belong in
`papers/SHOPPING_LIST.md` when a selected source cannot be obtained.


## Fresh frumentarii corpus — 2026-09-30

Sixteen pinned upstream source snapshots were acquired into ignored quarantine
for the expert-operator and self-improvement inquiry. No implementation is adopted.
All 33,036 retained regular files and executable bits match the preserved source
ZIPs directly. The ZIPs are intact GitHub codeload archives at exact commits.
Nested Git and filesystem detritus are omitted by the importer; symbolic links
are explicitly omitted and remain in the ZIPs. Exact omissions are recorded.

[The acquisition catalog](papers/2026-09-30-reference-acquisition.json) records
full revisions, original download URLs, archive identities and roots, file counts,
and omissions. Archives live in
`../quarantine_proj/archives/arconaut-frumentarii-2026-09-30/`.

| Reference | Revision prefix | Retained files | Research use |
| --- | --- | ---: | --- |
| [autoresearch](https://github.com/karpathy/autoresearch) | `228791fb499a` | 10 | Legible experiment intervention/evaluation loop |
| [dagger](https://github.com/dagger/dagger) | `4129d95c0c43` | 16,360 | Agent messaging, step scheduling, stateful recomposition (Go mechanism reference) |
| [gepa](https://github.com/gepa-ai/gepa) | `3f160c295000` | 603 | Shared evaluation API, code/policy candidates, search engines |
| [guile-fibers](https://github.com/wingo/fibers) | `f08253bafc30` | 80 | Scheme concurrency, channels, ports and scheduler lifetime |
| [ipykernel](https://github.com/ipython/ipykernel) | `cfa461d8afac` | 114 | Persistent execution, multiple clients and subshells |
| [jido](https://github.com/agentjido/jido) | `90b163eef87a` | 374 | BEAM strategies, directives, supervised signal processing |
| [letta-code](https://github.com/letta-ai/letta-code) | `96eb977b9477` | 2,427 | Memory versioning and managed compaction (TS mechanism reference) |
| [prime-agent](https://github.com/PrimeIntellect-ai/prime-agent) | `e75f59efc6f7` | 1,626 | Rust/Python partition, execution handles and continuity limits |
| [restate](https://github.com/restatedev/restate) | `2180b55410f6` | 1,519 | Queryable durable execution and deployment/version boundaries |
| [rlm](https://github.com/alexzhang13/rlm) | `d04208afbad2` | 179 | Executable context inspection and recursive model computation |
| [s6](https://github.com/skarnet/s6) | `67254f0c147f` | 367 | Small native process supervision and controller replacement |
| [sbcl](https://github.com/sbcl/sbcl) | `e0b6f381c2ab` | 2,127 | Compiled live Lisp, process primitives and image boundaries |
| [self-harness](https://github.com/qzzqzzb/Self-Harness) | `2720dbb3f522` | 24 | Failure-grounded candidate changes and evaluation/merge rules |
| [sly](https://github.com/joaotavora/sly) | `3ffa216d0818` | 98 | Live Lisp evaluation, compilation and loading |
| [spacetimedb](https://github.com/clockworklabs/SpacetimeDB) | `629e8c1e809a` | 6,987 | Transactional live state, subscriptions and durability boundaries |
| [squirreling](https://github.com/hyparam/squirreling) | `249f5bb27ed4` | 141 | Semantic query algorithms for studying an audit corpus (JS mechanism reference) |

Restore selected references from the Arconaut root after obtaining the preserved
archives with the catalog's SHA-256 identities. Each destination must be absent.
This example restores all sixteen using the existing importer:

```sh
python3 - <<'PY'
from pathlib import Path
import hashlib, json, subprocess
catalog = json.loads(Path("papers/2026-09-30-reference-acquisition.json").read_text())
for source in catalog["sources"]:
    archive = Path("..") / source["archive"]
    if hashlib.sha256(archive.read_bytes()).hexdigest() != source["sha256"]:
        raise SystemExit(f"Archive identity mismatch: {archive}")
    subprocess.run(["python3", "scripts/ingest_zip.py", str(archive),
                    source["destination"], "--root", source["archive_root"],
                    "--skip-symlinks"], check=True)
PY
```

On another host, adjust the archive location. To restore one reference, select
its catalog record and use the same importer arguments. Download URLs identify
the original source acquisition; the preserved ZIP identity identifies our artifact.
License files and historical instructions are retained as reference material.
No imported automation, installer, or program has been run.

## Complete known-agent pass and new discovery — 2026-09-30

At the operator's request, the source corpus now expands beyond the initial imports.
The catalogs distinguish actual agents, collaboration/orchestration machinery,
supporting runtimes, public product materials, and produced artifacts. Historical
fallbacks and unversioned local material are labeled explicitly. Existing snapshots
are not silently refreshed or replaced. Acquisition and source inspection do not
adopt a dependency or confer design authority on archived instructions.

- [Known-agent acquisition report](papers/2026-09-30-known-agent-acquisition.md)
  and [exact source catalog](papers/2026-09-30-known-agent-acquisition.json).
- [New discovery frumentarius](papers/2026-09-30-agent-discovery-frumentarii.md)
  and [exact source catalog](papers/2026-09-30-discovered-agent-acquisition.json).
- [Claude Science and scientific workbench acquisition](papers/2026-09-30-science-workbench-acquisition.md)
  and [artifact/documentation catalog](papers/2026-09-30-science-workbench-acquisition.json).
- [K-Dense BYOK source, documentation, and workflow templates](papers/2026-09-30-k-dense-byok-acquisition.md)
  and [exact reference catalog](papers/2026-09-30-k-dense-byok-acquisition.json).
- [Official Claude Code distribution acquisition](papers/2026-09-30-claude-code-distribution-acquisition.md)
  and [release/artifact catalog](papers/2026-09-30-claude-code-distribution-acquisition.json).
- [Unavailable acquisition material](papers/SHOPPING_LIST.md).

Known-agent ZIPs live in `../quarantine_proj/archives/arconaut-known-agents-2026-09-30/`;
discovered-source ZIPs in `../quarantine_proj/archives/arconaut-agent-discovery-2026-09-30/`;
scientific workbench bundles in
`../quarantine_proj/archives/arconaut-science-workbenches-2026-09-30/`.
Official Claude Code distributions have their own subdirectory under the known-agent
archive shelf, rather than being represented as vendor engine source.
These archives remain intact. Clean source snapshots omit nested Git metadata,
filesystem detritus, and symbolic links; omitted entries remain in their archives.
The source catalogs record direct retained-set/byte/executable-bit comparisons.
Science distinguishes vendor distributions, original documentation/examples,
derived readable material, and separately acquired public workflow source.

The owned `scripts/acquire_references.py` utility resolves immutable public source
revisions, preserves archives, stages ingestion, compares retained material directly,
and publishes an incremental catalog. Its input is a JSON list of `name`/`repository`
records, optionally with an immutable `revision` or explicitly labeled local fallback.
It leaves existing destinations unchanged and runs no imported code.

Restore acquired source snapshots from the Arconaut root after obtaining the exact
archives named in the catalogs; each selected destination must be absent. Select
one record instead of iterating all records to restore a single reference:

```sh
python3 - <<'PY'
from pathlib import Path
import hashlib, json, subprocess
catalogs = ("papers/2026-09-30-known-agent-acquisition.json",
            "papers/2026-09-30-discovered-agent-acquisition.json")
for catalog_path in catalogs:
    for source in json.loads(Path(catalog_path).read_text())["references"]:
        if source["status"] != "acquired":
            continue
        archive = Path(source["archive"])
        digest = hashlib.sha256()
        with archive.open("rb") as stream:
            for chunk in iter(lambda: stream.read(1024 * 1024), b""):
                digest.update(chunk)
        if digest.hexdigest() != source["sha256"]:
            raise SystemExit(f"Archive identity mismatch: {archive}")
        subprocess.run(["python3", "scripts/ingest_zip.py", str(archive),
                        source["destination"], "--root", source["archive_root"],
                        "--skip-symlinks"], check=True)
PY
```

On another host, adjust archive paths to the copied shelf. For Claude Science's
documentation/distribution bundle, use its catalog's archive/root identity and
the same importer into absent `quarantine/claude-science/`. The Science report
records exact restoration instructions and the scope of what was obtainable.
K-Dense BYOK's report supplies the corresponding source/public-material bundle
command into absent `quarantine/k-dense-byok/`; its intact upstream source ZIP is
also preserved separately. Its 27 repository documents and 326 workflow templates
are acquired source, rather than authenticated product exports.
The official Claude Code distribution report gives the corresponding bundle command
into absent `quarantine/claude-code-official-distribution/`. It preserves macOS ARM64
and Linux x64 native programs, installer/release material, and the canonical npm
launcher artifact without executing them. Full vendor engine source remains a
separate availability limit.

The known-source pass acquired 36 references and new discovery acquired 46.
Current Letta's public index and its deliberately historical V1 source share an
origin and remain distinct dated references; renamed aliases are not counted as
additional agents. Source exports do not automatically resolve Git submodules,
Git LFS assets, private services, or live state. The exact catalogs/report record
those boundaries. Official Claude Code public repository material and its
unofficial mirror remain distinct; mirror authenticity is unverified.

## Context Language Models — 2026-10-01

Added the operator-supplied [official CLM source](https://github.com/facebookresearch/context-language-models)
at `18dc11115f50f261233c5bba7937834491e307e8` to
`quarantine/context-language-models/`: 75 retained files, no omissions. The intact
ZIP and direct archive comparison are recorded in the
[acquisition catalog](papers/2026-10-01-context-language-models-acquisition.json).
The [source study](papers/2026-10-01-context-language-models-study.md) records archive
identity, restoration commands and design implications. No acquired code was executed
or adopted. This later addition is outside the completed 104-external-reference survey.

## C++ and Lua foundation grounding — 2026-10-01

Six additional references ground the selected stack and refit/audit obligations.
They are outside the completed agent-survey snapshot. No imported source, test,
installer or build was executed; no library was adopted.

| Reference | Identity | Extracted destination | Intact archive shelf |
| --- | --- | --- | --- |
| RuntimeCompiledCPlusPlus | `005c05145d98b87d974c36fc885003ea88bf3932` | `quarantine/runtime-compiled-cplusplus/` | `../quarantine_proj/archives/arconaut-native-reload-2026-10-01/` |
| fungos/cr | `1c3f8302320dee8206cf85d6aeb9e9a9cd78b527` | `quarantine/fungos-cr/` | Same native-reload shelf |
| Official Lua | 5.5.1, publisher archive checksum matched | `quarantine/lua-5.5.1/` | `../quarantine_proj/archives/arconaut-lua-embedding-2026-10-01/` |
| Official Lua tests | Exact 5.5.1 suite, publisher checksum matched | `quarantine/lua-5.5.1-tests/` | Same Lua-embedding shelf |
| sol2 | `c1f95a773c6f8f4fde8ca3efe872e7286afe4444` | `quarantine/sol2/` | Same Lua-embedding shelf |
| LevelDB | `7ee830d02b623e8ffe0b95d59a74db1e58da04c5` | `quarantine/leveldb/` | Shelf recorded in custody catalog below |

Exact source/download URLs, SHA-256 identities, archive roots, byte/file counts and
direct retained-byte/executable-bit comparisons are in the
[native catalog](papers/2026-10-01-native-reload-acquisition.json),
[Lua catalog](papers/2026-10-01-lua-embedding-acquisition.json), and
[custody catalog](papers/2026-10-01-refit-custody-acquisition.json).
Lua's original tarballs stay intact alongside derived ingestion ZIPs, with direct
comparison to the originals. RCC++'s GLFW Git submodule is absent from its upstream
source ZIP; this source-export boundary is recorded, not silently populated.

Restoration from the Arconaut root, with destinations absent:

```sh
python3 - <<'PY'
from pathlib import Path
import hashlib, json, subprocess
catalogs = ('papers/2026-10-01-native-reload-acquisition.json',
            'papers/2026-10-01-lua-embedding-acquisition.json',
            'papers/2026-10-01-refit-custody-acquisition.json')
for catalog in catalogs:
    for ref in json.loads(Path(catalog).read_text())['references']:
        if ref.get('status') != 'acquired':
            continue
        archive = Path(ref['archive'])
        if hashlib.sha256(archive.read_bytes()).hexdigest() != ref['sha256']:
            raise SystemExit(f'Archive identity mismatch: {archive}')
        subprocess.run(['python3', 'scripts/ingest_zip.py', str(archive),
                        ref['destination'], '--root', ref['archive_root'],
                        '--skip-symlinks'], check=True)
PY
```

The [native study](papers/2026-10-01-native-reload-grounding.md),
[Lua study](papers/2026-10-01-lua-embedding-grounding.md), and
[custody study](papers/2026-10-01-refit-custody-grounding.md) distinguish mechanisms
read from runtime qualification still required. Native author literature and
Godot/JENOVA documentation are cataloged separately; custody platform material
likewise records its exact source and reading limits. Archived instructions are inert.

## Lua 5.4.8 Linux qualification source

Intact archive: `../quarantine_proj/archives/lua-5.4.8.tar.gz`. Source:
https://www.lua.org/ftp/lua-5.4.8.tar.gz. SHA-256:
`4f18ddae154e793e46eeab727c59ef1c0c0c2b744e7b94219710d76f530629ae`.
Restore with `curl --fail --proto '=https' https://www.lua.org/ftp/lua-5.4.8.tar.gz
-o ../quarantine_proj/archives/lua-5.4.8.tar.gz`, then verify the digest.
`scripts/check-linux` builds this already-selected runtime in its disposable remote
workspace. No system installation or new runtime/version adoption is involved.

## Self-driven harness evolution frumentarii — 2026-10-06

Primary source/paper study for autodroit, useful-work campaigns and acceleration
measurement. Three acquisition catalogs provide original URLs, revision pins,
intact archive hashes/paths, restoration and actual reading limits:

- [Empirical acquisition/restoration](papers/frumentarii-2026-10-06/empirical/MANIFEST.md)
  and its machine-readable acquisition.json (repository cwd).
- [Harness acquisition/restoration](papers/frumentarii-2026-10-06/harness/manifest.json)
  and safe_extract.py (repository cwd). HELIX101 absolute fixture symlinks omitted
  from extraction, preserved in intact archive; omission is a reproduction limit.
- [Orchestration acquisition/restoration](papers/frumentarii-2026-10-06/orchestration/manifest.json)
  (paths/restoration relative to projects workspace).

Extracted sources are study-only, nested Git/filesystem metadata stripped; original
archives remain intact. No reference agents/tests were executed, no accounts used,
no dependencies adopted. Existing sources/pins explicitly reused in catalogs.
The synthesis and lane reports distinguish author benchmark claims, inspected code
mechanisms and unacquired implementation leads. Archived instructions remain inert.

## Optional integrations, thematic HUD and multiplayer — 2026-10-06

Study catalogs preserve pinned original archives, extracted source identities,
restoration commands and primary-document reading limits:

- [Mods manifest](papers/integrations-2026-10-06/mods/manifest.json): official
  Claude Code and playground sources; restoration uses repository cwd.
- [Feeds manifest](papers/integrations-2026-10-06/feeds/MANIFEST.md) and
  acquisition.json: MCP, CISA KEV and service documentation; repository cwd.
  Existing gptme plugin reference explicitly reused.
- [Teams manifest](papers/integrations-2026-10-06/teams/manifest.json): ICE,
  Commonly, Agent Room and official Claude changelog; projects workspace cwd.

Archives remain intact; extracted Git/filesystem metadata stripped. Acquired
instructions are historical reference. No reference programs/tests executed,
accounts authenticated, integrations installed or dependencies adopted. The
[synthesis](papers/integrations-2026-10-06/SYNTHESIS.md) separates inspected
mechanisms from proposed Arconaut design and future runtime qualification.

## G9 cryptographic transport decision study — 2026-10-07

Ignored `quarantine/g9-security/` contains RFC8446, OpenSSL3.5.0 verification-mode
API documentation and libsodium key-exchange documentation. Primary URLs, exact
SHA256 identities, restoration limitations and design/testing consequences are in
[papers/2026-10-07-peer-security-decision.md](papers/2026-10-07-peer-security-decision.md).
Acquired as study-only text, never executed or adopted. No production dependency
selected; G9 awaits explicit operator decision.

## phux — 2026-10-07

Operator-selected general multiplexing study: https://github.com/no-phux/phux,
commit `0fd4e511621f50c70c857ef0300b90fcd34a295f`. Extracted2626 reference files to ignored
`quarantine/phux/`; archive retained intact at
`../quarantine_proj/archives/arconaut-phux-2026-10-07/phux-0fd4e511621f50c70c857ef0300b90fcd34a295f.zip`
(20122943bytes; SHA256 `6ba142aaf70be41391dbc1684d017836aef32cba4fa8a9293cbb9a13598ead33`).
No nested Git history acquired; generic ingestion excludes metadata/detritus and
symlinks. Archived instructions carry no authority. No build/install/runtime executed.

Restore with absent destination:

```sh
curl -fL https://codeload.github.com/no-phux/phux/zip/0fd4e511621f50c70c857ef0300b90fcd34a295f -o ../quarantine_proj/archives/arconaut-phux-2026-10-07/phux-0fd4e511621f50c70c857ef0300b90fcd34a295f.zip
python3 scripts/ingest_zip.py ../quarantine_proj/archives/arconaut-phux-2026-10-07/phux-0fd4e511621f50c70c857ef0300b90fcd34a295f.zip quarantine/phux --root phux-0fd4e511621f50c70c857ef0300b90fcd34a295f --skip-symlinks
```

Study: papers/2026-10-07-phux-multiplexing.md. External optional service proposal,
not embedded dependency adoption or production qualification.

## P2P, multiplayer and E2EE frumentarii — 2026-10-07

Primary standards/literature and immutable reference archives acquired and studied
for the deferred multiplayer design. Source archives remain intact in shared
quarantine_proj; extracted refs are ignored and stripped of Git metadata/detritus.
No reference build/execution, networking dependency adoption or account use.
Sources, hashes, exact paths, restoration and acquisition gaps are recorded in:

- [P2P manifest](papers/networking-2026-10-07/p2p/MANIFEST.md)
- [Multiplayer references](papers/networking-2026-10-07/multiplayer/reference-manifest.json),
  [literature](papers/networking-2026-10-07/multiplayer/literature-manifest.json),
  [restoration](papers/networking-2026-10-07/multiplayer/RESTORE.md)
- [E2EE manifest](papers/networking-2026-10-07/e2ee/manifest.json),
  [restoration](papers/networking-2026-10-07/e2ee/RESTORE.md)

[Source-grounded synthesis](papers/networking-2026-10-07/SYNTHESIS.md) separates
identity, reachability, encryption, custody and shared-work authority.
