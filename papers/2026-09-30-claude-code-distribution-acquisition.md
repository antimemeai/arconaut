# Official Claude Code distribution acquisition

## Written sub-plan

The official public repository is already preserved and lacks a complete CLI engine
checkout. Inspect the official setup/download material and public release metadata
as data, then preserve an actual freely downloadable official CLI distribution.
Prefer the current macOS ARM64 and Linux x64 native distributions, or the canonical
official package artifact if that is the available public distribution. Preserve
original installer, metadata and distribution bytes, publisher version/URLs,
cryptographic identities and a restorable ZIP; extract only owned archival material
into clean quarantine and compare retained bytes directly with the archive.

Do not install, execute or import the distribution or installer, invoke a model,
provide credentials, accept terms, or resolve/install dependencies. The completed
36-reference source acquisition catalog stays unchanged. Record any real availability
limit promptly; this follow-up does not become a proprietary-product survey.

## Status

Complete: official Claude Code 2.1.286 native distributions and public installer/
release/npm metadata are preserved. Exact artifact identities and restoration are
in [the distribution catalog](2026-09-30-claude-code-distribution-acquisition.json).


## Acquired official distribution

The preserved official installer identifies `https://downloads.claude.ai/claude-code-releases`
as its release service. Public `latest` returned **2.1.286** and `stable` returned
**2.1.285**. The selected latest manifest reports build date
`2026-09-30T15:55:40Z`, build commit
`f344a08993bbba4fc1ea9b295245324610f02c58`, and mods commit
`732e167ee9d71296b4b63d6f529ac1334513826a`. These are publisher-reported identifiers,
not claims that the corresponding private engine source is available.

The [official setup documentation](https://code.claude.com/docs/en/setup)
connects the native installer and npm distribution. The canonical
`@anthropic-ai/claude-code` package is now a small launcher/postinstall wrapper
whose manifest names platform-native optional packages; it is not the entire native
CLI on its own. The canonical wrapper tarball and raw package manifest were also
preserved. Its public registry SHA-512 integrity and SHA-1 digest match the bytes.
No dependencies or postinstall steps were installed or invoked.

| Actual CLI artifact | Official download | Bytes | SHA-256 |
| --- | --- | ---: | --- |
| macOS ARM64, Mach-O 64-bit | [2.1.286 native binary](https://downloads.claude.ai/claude-code-releases/2.1.286/darwin-arm64/claude) | 225,167,728 | `75e3016e9d2570767b08e43a7467d4817a4f149232c169ca295f2c95fef21433` |
| Linux x64, ELF 64-bit | [2.1.286 native binary](https://downloads.claude.ai/claude-code-releases/2.1.286/linux-x64/claude) | 241,667,256 | `fe503f65c6289d59c23e5b21ae44f03583f997dd33a2cbfc75ab4f96fb8fc73f` |

Both native sizes and SHA-256 hashes match the retained
[official release manifest](https://downloads.claude.ai/claude-code-releases/2.1.286/manifest.json).
Binary headers were read as data and identify the stated formats/architectures.
The official detached manifest signature and public signing key are retained;
signature/platform code-signing verification was not performed. The checksum match
is the integrity claim made here.

The source shelf includes 12 files: two actual native CLIs, the intact canonical
npm wrapper tarball, the extracted raw npm package manifest, and original installer,
setup Markdown, latest/stable indicators, public npm metadata, native manifest,
detached signature and signing key. Original downloads remain intact under
`../quarantine_proj/archives/arconaut-known-agents-2026-09-30/claude-code-official-distribution/`.

The aggregate restorable ZIP is **215,102,537 bytes**, SHA-256
`f6d77b2a50e2637ba81a9c2dd1194c5bfd4a017058e403106dc7f84a720cff7c`.
The clean shelf `quarantine/claude-code-official-distribution` retains **12 files /
466,913,367 bytes**. The existing importer restored into a fresh staged directory;
all retained file sets, byte sequences and executable classifications were compared
directly with the ZIP before publishing it. No file omissions or failed downloads
remain. Artifacts are preserved as data with no executable file bits.

## Scope and restoration

Actual official product distributions are now available for study alongside the
separately preserved public repository and unverified unofficial source mirror.
This does not supply a complete official engine source checkout or evidence from
running the authenticated product. No installer, binary, npm code or wrapper was
executed, imported or installed; no credentials, login, terms acceptance or provider
calls were involved. The completed 36-source catalog remains unchanged.

From the Arconaut repository, with the destination absent:

```sh
python3 scripts/ingest_zip.py ../quarantine_proj/archives/arconaut-known-agents-2026-09-30/claude-code-official-distribution/claude-code-2.1.286-official-distribution.zip quarantine/claude-code-official-distribution --root claude-code-official-distribution --skip-symlinks
```

The JSON preserves each exact vendor URL, artifact hash/size, package version and
publisher-reported build identity. No new helper architecture was added; the bounded
acquisition reused the current owned importer/direct-comparison machinery.
