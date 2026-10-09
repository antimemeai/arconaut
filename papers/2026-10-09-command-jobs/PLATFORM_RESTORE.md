# Platform reference restoration

Run from `/Users/patrickbeam/projects/blackbird` or the corresponding cloned
project root. Study only: no imported source is a dependency or current instruction.

The manifest [platform-references.json](platform-references.json) gives the tmux
repository, immutable revision, codeload URL, archive SHA-256, clean destination and
retained-set/bytes/executable-bit comparison. Download that exact URL to the stated
archive shelf, verify the hash, and restore to an absent destination:

```sh
python3 scripts/ingest_zip.py ../quarantine_proj/archives/blackbird-command-jobs-platform-2026-10-09/command-jobs-tmux-82abcd175cca.zip quarantine/command-jobs-tmux --root tmux-82abcd175cca43c671af3690cd0af74c4c75621c --skip-symlinks
```

[platform-documents.json](platform-documents.json) gives every primary document's
original URL or installed-SDK path, SHA-256 and size. The `snapshot_archive` entry
identifies the intact ZIP containing those original acquired bytes. Copy the
preserved archive to the named shelf, verify its SHA-256 and restore into an absent
destination:

```sh
python3 scripts/ingest_zip.py ../quarantine_proj/archives/blackbird-command-jobs-platform-2026-10-09/platform-documents-2026-10-09.zip quarantine/command-jobs-platform-documents-2026-10-09 --root command-jobs-platform-documents-2026-10-09 --skip-symlinks
```

Source snapshots preserve original bytes; transformed readable text lives only in
ignored context. POSIX and Linux manual HTML can change upstream even at the same
URL. Exact acquired-byte restoration requires the retained ZIP. Reacquisition from
the manifest's URLs must compare hashes and record a different snapshot rather
than overwrite the old archive. Apple manuals require the stated macOS SDK 26.2;
a later SDK is different evidence. Linux `pty.c` is pinned to v6.19. GNU chapters
are pinned to manual 2.44 on the project's sourceware host; the unversioned GNU
paths originally redirected to a manual index and were replaced before study.

The importer removes nested Git metadata/filesystem detritus and omits symbolic
links; preserved archives remain intact. Neither new acquisition contains omitted
entries. No imported program or tests ran. The five owned platform probes are
retained separately as [platform-probes.py](platform-probes.py).
