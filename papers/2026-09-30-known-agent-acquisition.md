# Known agent acquisition

## Written sub-plan

Reconcile the 53-entry historical reference catalog with current clean snapshots,
then acquire missing coding agents, coding assistants, and agent collaboration
systems. Existing snapshots and the original checkouts remain unchanged. Acquire
the separate agent-relevant SDK/runtime references already cited by the fresh
frumentarii as supporting execution-model evidence. Studying these implementations
does not select dependencies or adopt their languages or designs.

For public repositories, resolve upstream HEAD to an immutable revision and preserve
an intact source ZIP in the shared archive directory. If public acquisition fails,
use a precisely labeled historical Git archive where one exists. An unversioned
local collaboration pattern can be preserved as such without inventing an upstream.
Reuse the existing ZIP importer, omit Git/filesystem detritus and symbolic links
from clean snapshots, and directly compare the retained file set, bytes, and
executable bits with the archive. Publish each successful result incrementally so
interruptions can resume without replacing existing references. Record unavailable
sources and explicit classification of non-agent support tools.

The acquisition utility is owned, uses Python's standard library and argv-based
subprocesses, and downloads only source. Check path rejection, direct comparison,
and failed-stage cleanup using tiny local archives; run no imported programs,
installers, live model calls, credentials, or laptop mutation testing. Another
frumentarius independently discovers new agents and reuses this utility.

## Status

Complete: 36 references acquired. Exact archive identities, revisions, provenance,
file counts, omissions, and source boundaries are in
[the acquisition catalog](2026-09-30-known-agent-acquisition.json).

## Selection and classification

The historical catalog has 53 entries. Twenty-six missing entries are coding
agents, coding assistants, or worker orchestration patterns. Mini-SWE-Agent and
OpenCode already have preserved active snapshots and remain unchanged. One additional
legacy entry, `chat-system`, is a multi-protocol messaging crate rather than an agent;
it is acquired as a supporting reference for the proposed IRC-style collaboration.

The other 24 historical entries are general libraries, tools, benchmarks, databases,
or interface programs. They are not part of this agent acquisition; their old
checkouts remain live in `projects_old`. Brush is already an active supporting
reference. Explicitly unselected here:

- `Figment` — https://github.com/SergioBenitez/Figment.git
- `PolarisDB` — https://github.com/hugoev/PolarisDB.git
- `SWE-bench` — https://github.com/princeton-nlp/SWE-bench.git
- `arbitrary` — https://github.com/rust-fuzz/arbitrary.git
- `bashkit` — https://github.com/everruns/bashkit.git
- `beads` — https://github.com/steveyegge/beads.git
- `brush-shell` — https://github.com/reubeno/brush.git
- `cargo-fuzz` — https://github.com/rust-fuzz/cargo-fuzz.git
- `cargo-mutants` — https://github.com/sourcefrog/cargo-mutants.git
- `criterion.rs` — https://github.com/bheisler/criterion.rs.git
- `flamegraph` — https://github.com/flamegraph-rs/flamegraph.git
- `halloy` — https://github.com/squidowl/halloy.git
- `hyperfine` — https://github.com/sharkdp/hyperfine.git
- `markdown2pdf` — https://github.com/theiskaa/markdown2pdf.git
- `pgvector` — https://github.com/pgvector/pgvector.git
- `printpdf` — https://github.com/fschutt/printpdf.git
- `proptest` — https://github.com/proptest-rs/proptest.git
- `qdrant` — https://github.com/qdrant/qdrant.git
- `quickcheck` — https://github.com/BurntSushi/quickcheck.git
- `ratatui` — https://github.com/ratatui/ratatui.git
- `rust-bash` — https://github.com/shantanugoel/rust-bash.git
- `tarpc` — https://github.com/google/tarpc.git
- `tonic` — https://github.com/hyperium/tonic.git
- `usearch` — https://github.com/unum-cloud/usearch.git

Previously cited supporting references newly selected are Windmill, the Restate
Python and Rust SDKs, and Erlang/OTP. Jido AI, the OpenAgents SDK, and the Letta
server were investigated as agent framework/runtime sources. The current Letta
repository proved to be a public landing page; its retired V1 server is acquired
separately from the upstream `archive` branch. Official `anthropics/claude-code`
public materials are preserved separately from the unofficial mirror; the scope
of publicly included source is described below. No dependency adoption
is implied by bringing these references home.

`claudex-pattern` has no upstream or Git revision in the historical catalog.
Its seven-file local source tree is preserved as an unversioned snapshot; no
upstream identity is invented. The unofficial Claude Code mirror describes itself
as leaked source. That is an upstream claim, not verified authenticity or a claim
that it matches the official running product. Archived instructions remain history.

## Utility checks

The direct local oracles initially failed because the acquisition module did not
exist. After implementation, all initial four checks passed: retained bytes/set/executable
bits and omissions, corrupted outputs rejected by direct comparison, unsafe archive
paths leaving no snapshot or stage, existing references unchanged, and bounded
reference names/public repository URLs. No network or imported source runs in these
checks. The parent is reviewing the utility independently during acquisition.


## Final acquisition and source scope

All 36 selected references are acquired. Thirty-four are pinned current upstream
HEAD source ZIPs, one is the deliberately historical upstream Letta V1 `archive`
branch (`56ba9c25552605eec89de8ed3dc6394b625c1993`), and one is the unversioned
local Claudex source tree. No historical local-Git fallback was needed in the final
corpus, and no known selected source-download failure remains.

The final selection consists of 26 legacy agent/assistant/collaboration entries,
two agent runtime SDKs (Jido AI and OpenAgents SDK), one historical agent framework
(Letta V1), two official public-material/index references (Claude Code and current
Letta), and five supporting chat/workflow/runtime references. Existing active
references and original checkouts remain unchanged.

The 36 intact ZIPs total **1,644,203,505 bytes**. Clean snapshots retain
**114,960 files / 2,808,160,757 bytes**. Ninety-eight source file entries are omitted
as symbolic links or repository/filesystem detritus and remain preserved inside the
archives. Every retained file set, file byte sequence, and executable classification
was compared directly to its source ZIP before publishing the snapshot. No hidden
partial archive or failed extraction stage remains.

Gas City's first acquisition encountered a transient upstream problem and then
exposed a utility defect: `git archive --output` used a relative path interpreted
inside the fallback checkout selected by `git -C`. A new direct local-Git fallback
oracle failed red; changing the archive output to an absolute path made all five
oracles pass. The utility also now retains the original upstream failure before
attempting fallback. A separate Gas City retry succeeded at current upstream HEAD
`0addde8a1e17a7b4c51c11b563c817a527dc4b3b`; its successful record replaces the
initial failed attempt, which published no snapshot. The parent independently
reviewed the helper's staging/comparison and URL/name bounds during acquisition.

The current official Claude Code repository contains public documentation,
examples, plugins, mods and scripts; it does not provide a complete CLI engine
source checkout. The unofficial mirror remains separately labeled and unverified.
Current `letta-ai/letta` has 12 landing/legal/instruction files and directs the current
agent harness and App Server to the already-preserved `letta-code` source. The
retired V1 server is available in the distinct historical snapshot. These are source
scope distinctions, not failed downloads or dependency selections.

Git source archives do not populate nested Git submodules or resolve Git LFS assets.
The catalog explicitly records declarations, effective paths, URLs, and the 217
unresolved LFS pointer paths. Observed boundaries:

| Reference | Submodule declaration / unresolved LFS scope |
| --- | --- |
| Cline | `evals/cline-bench` → `https://github.com/cline/cline-bench.git`, no populated files; one demo GIF pointer |
| Pi Agent Rust | Nested npm conformance artifact declares `tests/ext_conformance/artifacts/npm/vaayne-agent-kit/mcphub` → `https://github.com/vaayne/mcphub`; no populated files; this is a test artifact rather than a root engine module |
| Tabby | `crates/llama-cpp-server/llama.cpp` → `https://github.com/ggml-org/llama.cpp.git`, no populated files; 129 pointers, including prebuilt tree-sitter WASM and documentation/UI assets |
| Roo Code | One demo GIF pointer |
| Sweep | 86 documentation/demo image pointers |

No benchmark, external inference runtime, or large image dependency was recursively
acquired as an agent merely because a source archive refers to it. Source inspection
can use the captured pointer declarations; importing these references does not
install their dependencies or make their runtime assets usable.

## Acquired references

Full commits, SHA-256 archive identities and restoration roots are in the JSON.
The abbreviated commits below are for navigation only.

| Reference | Classification | Revision | Retained files |
| --- | --- | --- | ---: |
| [OxideAgent](https://github.com/Juan-LukeKlopper/OxideAgent) | coding-agent-or-assistant | `4ad71bdb1d8c` | 88 |
| [chat-system](https://github.com/rexlunae/chat-system) | supporting-chat-transport | `8fefcaea4f51` | 54 |
| [claude-code-official-public](https://github.com/anthropics/claude-code) | official-agent-public-materials | `525d3b353126` | 1,524 |
| [claude-code-unofficial](https://github.com/codeaashu/claude-code) | coding-agent-or-assistant | `eec3692193a3` | 2,163 |
| `claudex-pattern` | agent-collaboration | `unversioned local tree` | 7 |
| [cline](https://github.com/cline/cline) | coding-agent-or-assistant | `9fe17595de3b` | 4,150 |
| [codex](https://github.com/openai/codex) | coding-agent-or-assistant | `08e2b58b07b8` | 8,864 |
| [continue](https://github.com/continuedev/continue) | coding-agent-or-assistant | `5522c6f44ca0` | 3,058 |
| [devika](https://github.com/stitionai/devika) | coding-agent-or-assistant | `80bb343cbe4a` | 169 |
| [erlang-otp](https://github.com/erlang/otp) | supporting-systems-runtime | `495ce0b626f2` | 11,839 |
| [gascity](https://github.com/gastownhall/gascity) | agent-collaboration | `0addde8a1e17` | 6,608 |
| [gastown](https://github.com/gastownhall/gastown) | agent-collaboration | `649b832b7672` | 1,563 |
| [gemini-cli](https://github.com/google-gemini/gemini-cli) | coding-agent-or-assistant | `c6bccb7ecbf6` | 3,021 |
| [goose](https://github.com/aaif-goose/goose) | coding-agent-or-assistant | `bab8ff641039` | 2,488 |
| [gpt-engineer](https://github.com/AntonOsika/gpt-engineer) | coding-agent-or-assistant | `a90fcd543eed` | 182 |
| [jido-ai](https://github.com/agentjido/jido_ai) | agent-runtime-sdk | `01b7dca0f889` | 453 |
| [kimi-cli](https://github.com/MoonshotAI/kimi-cli) | coding-agent-or-assistant | `9ab1286b8fe4` | 990 |
| [letta](https://github.com/letta-ai/letta) | official-agent-public-index | `5bcdd177d70f` | 12 |
| [letta-v1-historical](https://github.com/letta-ai/letta) | historical-agent-framework | `56ba9c255526` | 1,156 |
| [metagpt](https://github.com/geekan/MetaGPT) | agent-collaboration | `11cdf466d042` | 1,255 |
| [oh-my-pi](https://github.com/can1357/oh-my-pi) | coding-agent-or-assistant | `8b25ad4a0562` | 8,471 |
| [openagents-sdk](https://github.com/openagents-org/openagents-sdk) | agent-runtime-sdk | `faf416fca40b` | 1,315 |
| [opendev](https://github.com/opendev-to/opendev) | coding-agent-or-assistant | `d32c660e4eed` | 1,184 |
| [openhands](https://github.com/All-Hands-AI/OpenHands) | coding-agent-or-assistant | `953c944eee62` | 2,422 |
| [pi](https://github.com/earendil-works/pi) | coding-agent-or-assistant | `8ce69e9d2b17` | 2,145 |
| [pi-agent-rust](https://github.com/Dicklesworthstone/pi_agent_rust) | coding-agent-or-assistant | `38d4f8835a06` | 21,811 |
| [plandex](https://github.com/plandex-ai/plandex) | coding-agent-or-assistant | `e2d772072efa` | 696 |
| [qwen-code](https://github.com/QwenLM/qwen-code) | coding-agent-or-assistant | `310f4ba3ab95` | 10,266 |
| [restate-sdk-python](https://github.com/restatedev/sdk-python) | supporting-workflow-sdk | `ad4ea7c0a60b` | 109 |
| [restate-sdk-rust](https://github.com/restatedev/sdk-rust) | supporting-workflow-sdk | `5cd9a3d24d7d` | 127 |
| [roo-code](https://github.com/RooVetGit/Roo-Code) | coding-agent-or-assistant | `b867ec914575` | 3,022 |
| [swe-agent](https://github.com/princeton-nlp/SWE-agent) | coding-agent-or-assistant | `3ea751c087f3` | 407 |
| [sweep](https://github.com/sweepai/sweep) | coding-agent-or-assistant | `a8b8b67bda4f` | 527 |
| [tabby](https://github.com/TabbyML/tabby) | coding-agent-or-assistant | `21b29048d7bc` | 2,098 |
| [trae-agent](https://github.com/bytedance/trae-agent) | coding-agent-or-assistant | `e839e559ac61` | 108 |
| [windmill](https://github.com/windmill-labs/windmill) | supporting-workflow-runtime | `8953ca670aac` | 10,608 |


## Restoration and utility

Archives are in `../quarantine_proj/archives/arconaut-known-agents-2026-09-30/`.
From the Arconaut repository, with the named destination absent, use each record's
`archive`, `destination`, and `archive_root`:

```sh
python3 scripts/ingest_zip.py ARCHIVE quarantine/NAME --root ARCHIVE_ROOT --skip-symlinks
```

The owned utility is [acquire_references.py](../scripts/acquire_references.py), with
small local direct oracles in [test_acquire_references.py](../scripts/test_acquire_references.py).
It accepts a structured JSON list of `name` and public GitHub `repository`, optional
immutable `revision`, and optional historical `fallback_path`/`fallback_revision`;
`local_tree` records an unversioned source snapshot. Example invocation:

```sh
python3 scripts/acquire_references.py --input INPUT.json --catalog CATALOG.json --archive-dir ARCHIVE_DIRECTORY --quarantine-dir quarantine --jobs 4
```

The catalog is incremental and successful acquisitions resume without replacing
existing destinations. Skipping a prior successful acquisition does not claim a
new direct comparison. Filesystem changes and downloads are staged; reference
programs and installers are never run by this utility. Ordinary original checkouts
are not deleted or stripped. No library is adopted into Arconaut.

## Official Claude Code distribution follow-up

The separate [official distribution acquisition](2026-09-30-claude-code-distribution-acquisition.md)
preserves actual Claude Code 2.1.286 macOS ARM64 and Linux x64 binaries, canonical
npm wrapper tarball, installer and release metadata without executing or installing
them. The completed 36-reference source catalog remains unchanged.
