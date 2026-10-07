# Restore this study corpus

Run from the Arconaut repository root. This developer-only Python command reads
the adjacent manifest, verifies each acquired item's recorded SHA256, and restores
study bytes under ignored quarantine. It executes no acquired code. It deliberately
fails if a mutable URL changed; investigate and record a new source rather than
substituting silently. Manifest failures are skipped because no bytes were acquired.

```sh
python3 - <<'PY'
import hashlib, json, pathlib, stat, urllib.request, zipfile

base = pathlib.Path.cwd().resolve()
manifest = base / 'papers/networking-2026-10-07/e2ee/manifest.json'
quarantine = (base / 'quarantine/e2ee-study').resolve()
quarantine.mkdir(parents=True, exist_ok=True)
for row in json.loads(manifest.read_text()):
    if 'path' not in row:
        continue
    target = (base / row['path']).resolve()
    assert target.is_relative_to(quarantine)
    request = urllib.request.Request(row['url'], headers={'User-Agent': 'Arconaut-research'})
    with urllib.request.urlopen(request, timeout=90) as response:
        data = response.read()
    assert hashlib.sha256(data).hexdigest() == row['sha256'], row['url']
    target.parent.mkdir(parents=True, exist_ok=True)
    target.write_bytes(data)
    if 'extracted' not in row:
        continue
    destination = (base / row['extracted']).resolve()
    assert destination.is_relative_to(quarantine)
    destination.mkdir(parents=True, exist_ok=True)
    with zipfile.ZipFile(target) as archive:
        for entry in archive.infolist():
            path = pathlib.PurePosixPath(entry.filename)
            parts = path.parts[1:]
            if not parts or path.is_absolute():
                continue
            if any(part in ('..', '.git', '.DS_Store', '__MACOSX') for part in parts):
                continue
            if stat.S_ISLNK(entry.external_attr >> 16):
                continue
            output = destination.joinpath(*parts)
            assert output.resolve().is_relative_to(destination)
            if entry.is_dir():
                output.mkdir(parents=True, exist_ok=True)
            else:
                output.parent.mkdir(parents=True, exist_ok=True)
                output.write_bytes(archive.read(entry))
print('Study corpus restored; no reference code executed.')
PY
```

Archives remain intact. ZIPs omit Git submodule contents, specifically Mbed TLS's
framework and TF-PSA-Crypto; this restores exactly the acquired study corpus, not
a complete build tree. Its archived instructions have no current authority.

The optional derived PDF text can be recreated with:

```sh
pdftotext quarantine/e2ee-study/pqxdh-analysis.pdf quarantine/e2ee-study/pqxdh-analysis.txt
```
