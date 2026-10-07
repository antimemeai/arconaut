# P2P study acquisition manifest

2026-10-07. Study-only reference snapshots; no build or executable invocation. Source ZIPs remain intact in shared `quarantine_proj/archives/arconaut-p2p-2026-10-07/`. Extracted references live in ignored `arconaut/quarantine/p2p-study/`. Archives do not recursively materialize submodules. Historical instructions in references have no authority. Extraction omitted Git metadata, archive detritus and symlinks. No dependency adoption.

## Immutable references

### microsoft/msquic

Commit `8249f718b9f0018a9e151367fbea11b5e86adb8c`; commit date 2026-10-05T16:41:39Z. Archive 8,914,266 bytes; 1027 files extracted.

Source: [microsoft/msquic at pinned commit](https://codeload.github.com/microsoft/msquic/zip/8249f718b9f0018a9e151367fbea11b5e86adb8c).

Archive: `quarantine_proj/archives/arconaut-p2p-2026-10-07/msquic-8249f718b9f0018a9e151367fbea11b5e86adb8c.zip`.
SHA256: `bc180ab94294848c0484a16a282e20655c0545fc7093048f5c4a48fb6d8fafdf`.

Restore from workspace root into a fresh destination:

```sh
curl -fL 'https://codeload.github.com/microsoft/msquic/zip/8249f718b9f0018a9e151367fbea11b5e86adb8c' -o 'quarantine_proj/archives/arconaut-p2p-2026-10-07/msquic-8249f718b9f0018a9e151367fbea11b5e86adb8c.zip'
printf '%s  %s\n' 'bc180ab94294848c0484a16a282e20655c0545fc7093048f5c4a48fb6d8fafdf' 'quarantine_proj/archives/arconaut-p2p-2026-10-07/msquic-8249f718b9f0018a9e151367fbea11b5e86adb8c.zip' | shasum -a 256 -c -
python3 arconaut/scripts/ingest_zip.py 'quarantine_proj/archives/arconaut-p2p-2026-10-07/msquic-8249f718b9f0018a9e151367fbea11b5e86adb8c.zip' 'arconaut/quarantine/p2p-study/msquic' --root 'msquic-8249f718b9f0018a9e151367fbea11b5e86adb8c' --skip-symlinks
```

### ngtcp2/ngtcp2

Commit `b9fc4d55ce6035a4cc76dc456ced6371e98f9214`; commit date 2026-10-04T05:08:16Z. Archive 1,112,383 bytes; 472 files extracted.

Source: [ngtcp2/ngtcp2 at pinned commit](https://codeload.github.com/ngtcp2/ngtcp2/zip/b9fc4d55ce6035a4cc76dc456ced6371e98f9214).

Archive: `quarantine_proj/archives/arconaut-p2p-2026-10-07/ngtcp2-b9fc4d55ce6035a4cc76dc456ced6371e98f9214.zip`.
SHA256: `bd20095704da854dd6797bf26060ed2ebb5ff57b5f3f4f0b9e53628a8abd4185`.

Restore from workspace root into a fresh destination:

```sh
curl -fL 'https://codeload.github.com/ngtcp2/ngtcp2/zip/b9fc4d55ce6035a4cc76dc456ced6371e98f9214' -o 'quarantine_proj/archives/arconaut-p2p-2026-10-07/ngtcp2-b9fc4d55ce6035a4cc76dc456ced6371e98f9214.zip'
printf '%s  %s\n' 'bd20095704da854dd6797bf26060ed2ebb5ff57b5f3f4f0b9e53628a8abd4185' 'quarantine_proj/archives/arconaut-p2p-2026-10-07/ngtcp2-b9fc4d55ce6035a4cc76dc456ced6371e98f9214.zip' | shasum -a 256 -c -
python3 arconaut/scripts/ingest_zip.py 'quarantine_proj/archives/arconaut-p2p-2026-10-07/ngtcp2-b9fc4d55ce6035a4cc76dc456ced6371e98f9214.zip' 'arconaut/quarantine/p2p-study/ngtcp2' --root 'ngtcp2-b9fc4d55ce6035a4cc76dc456ced6371e98f9214' --skip-symlinks
```

### paullouisageneau/libjuice

Commit `cb523763255f241442da1fbdd72c498f33a54b35`; commit date 2026-10-06T08:14:37Z. Archive 163,892 bytes; 81 files extracted.

Source: [paullouisageneau/libjuice at pinned commit](https://codeload.github.com/paullouisageneau/libjuice/zip/cb523763255f241442da1fbdd72c498f33a54b35).

Archive: `quarantine_proj/archives/arconaut-p2p-2026-10-07/libjuice-cb523763255f241442da1fbdd72c498f33a54b35.zip`.
SHA256: `d5489bcd1707e2a43bcb58e24d144ed582c5cf7af19d5a4e53007118eda4a219`.

Restore from workspace root into a fresh destination:

```sh
curl -fL 'https://codeload.github.com/paullouisageneau/libjuice/zip/cb523763255f241442da1fbdd72c498f33a54b35' -o 'quarantine_proj/archives/arconaut-p2p-2026-10-07/libjuice-cb523763255f241442da1fbdd72c498f33a54b35.zip'
printf '%s  %s\n' 'd5489bcd1707e2a43bcb58e24d144ed582c5cf7af19d5a4e53007118eda4a219' 'quarantine_proj/archives/arconaut-p2p-2026-10-07/libjuice-cb523763255f241442da1fbdd72c498f33a54b35.zip' | shasum -a 256 -c -
python3 arconaut/scripts/ingest_zip.py 'quarantine_proj/archives/arconaut-p2p-2026-10-07/libjuice-cb523763255f241442da1fbdd72c498f33a54b35.zip' 'arconaut/quarantine/p2p-study/libjuice' --root 'libjuice-cb523763255f241442da1fbdd72c498f33a54b35' --skip-symlinks
```

### paullouisageneau/libdatachannel

Commit `773e5b3de2d6c6501fa74cc9aa5c9e6aab1d9e12`; commit date 2026-09-26T22:32:00Z. Archive 53,653,338 bytes; 2785 files extracted.

Source: [paullouisageneau/libdatachannel at pinned commit](https://codeload.github.com/paullouisageneau/libdatachannel/zip/773e5b3de2d6c6501fa74cc9aa5c9e6aab1d9e12).

Archive: `quarantine_proj/archives/arconaut-p2p-2026-10-07/libdatachannel-773e5b3de2d6c6501fa74cc9aa5c9e6aab1d9e12.zip`.
SHA256: `e0e1c97f5b8abba8af38e6c2850a3a836f181be68c59503f77860d7f578ffff7`.

Restore from workspace root into a fresh destination:

```sh
curl -fL 'https://codeload.github.com/paullouisageneau/libdatachannel/zip/773e5b3de2d6c6501fa74cc9aa5c9e6aab1d9e12' -o 'quarantine_proj/archives/arconaut-p2p-2026-10-07/libdatachannel-773e5b3de2d6c6501fa74cc9aa5c9e6aab1d9e12.zip'
printf '%s  %s\n' 'e0e1c97f5b8abba8af38e6c2850a3a836f181be68c59503f77860d7f578ffff7' 'quarantine_proj/archives/arconaut-p2p-2026-10-07/libdatachannel-773e5b3de2d6c6501fa74cc9aa5c9e6aab1d9e12.zip' | shasum -a 256 -c -
python3 arconaut/scripts/ingest_zip.py 'quarantine_proj/archives/arconaut-p2p-2026-10-07/libdatachannel-773e5b3de2d6c6501fa74cc9aa5c9e6aab1d9e12.zip' 'arconaut/quarantine/p2p-study/libdatachannel' --root 'libdatachannel-773e5b3de2d6c6501fa74cc9aa5c9e6aab1d9e12' --skip-symlinks
```

## Literature bytes

Restore text with `curl -fL URL -o PATH` and verify the SHA256 listed below. PDFs are shared under ignored `papers_proj/arconaut-networking-2026-10-07/p2p/`; text lives in the ignored project quarantine. This is a source/restoration manifest, not an executed interoperability report.

| Source URL | Local path from projects root | Bytes | SHA256 |
|---|---|---:|---|
| https://www.rfc-editor.org/rfc/rfc7675.txt | `arconaut/quarantine/p2p-study/rfc7675.txt` | 22346 | `301ef56c53015c8e10ba42aac959d467bfc4e784cd2b5dbd59db41b835b949dd` |
| https://www.rfc-editor.org/rfc/rfc8445.txt | `arconaut/quarantine/p2p-study/rfc8445.txt` | 239713 | `60bb913e8c8b2007f284105eef2faa2ea7e359dd4b73d7992557b3b11180065e` |
| https://www.rfc-editor.org/rfc/rfc8489.txt | `arconaut/quarantine/p2p-study/rfc8489.txt` | 166237 | `3a1c97ec36576fda83b5f39fbe1d033c6c0b934e2209a29bb21b9ace57ef4289` |
| https://www.rfc-editor.org/rfc/rfc8656.txt | `arconaut/quarantine/p2p-study/rfc8656.txt` | 222692 | `271f4386bcf95cf94a31710dda6816a89cefdc2557581451e6a36d79b93b12b0` |
| https://www.rfc-editor.org/rfc/rfc8827.txt | `arconaut/quarantine/p2p-study/rfc8827.txt` | 92422 | `ac905792a914f61fa5ff02c92ec9ff93ac8841bc41c0d4d163a71440f123ec52` |
| https://www.rfc-editor.org/rfc/rfc8831.txt | `arconaut/quarantine/p2p-study/rfc8831.txt` | 35623 | `86bec13ce9ebb28dfea37ad2ea2ccd771ab7adb9a95668ed8736eee2b028ea5c` |
| https://www.rfc-editor.org/rfc/rfc8832.txt | `arconaut/quarantine/p2p-study/rfc8832.txt` | 27616 | `64f9addf6ff135f37c5ff8ec5b9b905ad882d6288400af144425cac3df507df5` |
| https://www.rfc-editor.org/rfc/rfc9000.txt | `arconaut/quarantine/p2p-study/rfc9000.txt` | 403442 | `f88aae47f8b18e102024916e975e919201d8dde689cba79b01079eaedd402e22` |
| https://www.rfc-editor.org/rfc/rfc9001.txt | `arconaut/quarantine/p2p-study/rfc9001.txt` | 126175 | `3bbaecdf5afd278052a2c48348ce118c4ff8d0cf6b9915549858171b3f98a591` |
| https://www.rfc-editor.org/rfc/rfc9221.txt | `arconaut/quarantine/p2p-study/rfc9221.txt` | 18624 | `ee8c04c5228fd120030ba7a8f6725c2ca609da107ad2ba8c44fdd44f73edb3b4` |
| https://www.ietf.org/rfc/rfc9000.pdf | `papers_proj/arconaut-networking-2026-10-07/p2p/rfc9000.pdf` | 1535216 | `24f411581702fea968f554264a629a80aa5a03a2a959733063391575256edcc7` |
| https://www.ietf.org/rfc/rfc8831.pdf | `papers_proj/arconaut-networking-2026-10-07/p2p/rfc8831.pdf` | 200135 | `6518eed024a10253ff89a331b43fa5f26ea70be0f365ed9b35f5883cabc5b9ee` |

RFC 8445 PDF was unavailable at `https://www.ietf.org/rfc/rfc8445.pdf` and the initially attempted rfc-editor pdfrfc path (404); its complete text was acquired. No operator shopping item required.

Existing phux snapshot `0fd4e511621f50c70c857ef0300b90fcd34a295f` was read in place from `arconaut/quarantine/phux/`; restoration is already in project QUARANTINE.md. No duplicate archive acquired.

## Research action record

Read workspace doctrine/Arconaut instructions and multiplayer intent; inspected the G9 security candidate and phux study; fetched current primary protocol/repository pages; resolved HEAD to immutable commit before fetching each archive; acquired four source archives and ten RFC texts plus two PDFs; inspected concrete queue, verifier, relay and clock/packet callback paths; wrote TRANSPORT.md and this manifest. No source/build/branch/commit, credential or network service modified. Root owns integration/journal entry.

Source archive total: 63,843,879 bytes. No performance, billing or memory measurement made.
