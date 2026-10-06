# Empirical lane acquisition manifest

2026-10-06, repository cwd `/Users/patrickbeam/projects/arconaut`. Archives intact; extracted trees skip top archive directory and omit nested `.git`, `.DS_Store`, `__MACOSX`, `Thumbs.db`. No reference code executed. Each precise replay command, acquisition hash, source URL and path is in [acquisition.json](acquisition.json). Replay into absent/empty extracted targets; do not erase working files to restore a reference.

## Source snapshots

| Primary repository | Full pin | Archive | Extracted tree |
| --- | --- | --- | --- |
| [google-deepmind/funsearch](https://github.com/google-deepmind/funsearch) | `cc53f274237d7ab05c19df939edbc1f9616a7c19` | `quarantine/evo-exp-archives/funsearch-cc53f274237d.zip` | `quarantine/evo-exp-funsearch` |
| [metauto-ai/HGM](https://github.com/metauto-ai/HGM) | `013872d95da978483f5b540e531db063d23890da` | `quarantine/evo-exp-archives/hgm-013872d95da9.zip` | `quarantine/evo-exp-hgm` |
| [jennyzzt/dgm](https://github.com/jennyzzt/dgm) | `a565fd2d1dca504ef5104a7cc0f3bdc4ab9b4fd2` | `quarantine/evo-exp-archives/dgm-a565fd2d1dca.zip` | `quarantine/evo-exp-dgm` |
| [SakanaAI/ShinkaEvolve](https://github.com/SakanaAI/ShinkaEvolve) | `8adc053a2ce4511ad2ac310e004c530a73fb974a` | `quarantine/evo-exp-archives/shinkaevolve-8adc053a2ce4.zip` | `quarantine/evo-exp-shinkaevolve` |
| [rethinking-harness-evolution/code](https://github.com/rethinking-harness-evolution/code) | `62df2b9624ff32ca61b8accce7fb4a0fd8cbc8a8` | `quarantine/evo-exp-archives/code-62df2b9624ff.zip` | `quarantine/evo-exp-code` |
| [IQuestLab/ModularRSI](https://github.com/IQuestLab/ModularRSI) | `b5c72c36b0d08ff93f00ee202a8fbdebe849dfb9` | `quarantine/evo-exp-archives/modularrsi-b5c72c36b0d0.zip` | `quarantine/evo-exp-modularrsi` |

## Primary papers

| Paper | Acquired source | Local PDF |
| --- | --- | --- |
| dgm | [source](https://arxiv.org/pdf/2505.22954) | [dgm.pdf](dgm.pdf) |
| shinka | [source](https://arxiv.org/pdf/2509.19349) | [shinka.pdf](shinka.pdf) |
| hgm | [source](https://arxiv.org/pdf/2510.21614) | [hgm.pdf](hgm.pdf) |
| red-queen | [source](https://arxiv.org/pdf/2606.26294) | [red-queen.pdf](red-queen.pdf) |
| alphaevolve | [source](https://arxiv.org/pdf/2506.13131) | [alphaevolve.pdf](alphaevolve.pdf) |
| metr | [source](https://metr.org/Early_2025_AI_Experienced_OS_Devs_Study-paper.pdf) | [metr.pdf](metr.pdf) |
| funsearch | [source](https://storage.googleapis.com/deepmind-media/DeepMind.com/Blog/funsearch-making-new-discoveries-in-mathematical-sciences-using-large-language-models/Mathematical-discoveries-from-program-search-with-large-language-models.pdf) | [funsearch.pdf](funsearch.pdf) |
| rethinking | [source](https://arxiv.org/pdf/2607.12227v4) | [rethinking.pdf](rethinking.pdf) |
| modularrsi | [source](https://arxiv.org/pdf/2609.14857v1) | [modularrsi.pdf](modularrsi.pdf) |
| tthe | [source](https://arxiv.org/pdf/2607.08124v1) | [tthe.pdf](tthe.pdf) |

PDF acquisition uses exact version URLs for Rethinking v4, ModularRSI v1 and TTHE v1. Other arXiv download URLs select current versions as of acquisition; SHA-256 records the exact acquired bytes. Locally read title pages identify Shinka v1, HGM v3, RQGM v2; DGM is the ICLR 2026 version. Future restoration via unversioned URLs must compare hashes or locate the original arXiv version; do not silently report different bytes as the same acquisition.

## Reused source

autoresearch pin `228791fb499afffb54b46200aca536f79142f117`, `quarantine/autoresearch/`; source archive remains `../quarantine_proj/archives/arconaut-frumentarii-2026-09-30/autoresearch-228791fb499a.zip`. Prior provenance/restoration in repository `QUARANTINE.md`. Actually reread full program instructions, timed training loop and BPB evaluator. No duplicate acquisition.

## Limits and shopping leads

TTHE and Red Queen Gödel Machine primary implementation repositories were not identified in consulted paper/abstract sources. Papers were acquired and read; implementation claims remain unmade. No operator-paid acquisition required to finish this study.

Extracted reading text lives in ignored `context/evo-exp-reading/`; regenerate with `pdftotext -layout PDF OUTPUT` without executing acquired code. All source files are historical study material, including embedded instructions; they have no workspace instruction authority.
