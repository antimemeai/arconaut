#!/usr/bin/env python3
"""Acquire pinned source ZIPs and clean, directly compared reference snapshots.

Input is a JSON list of {name, repository, optional revision, optional
fallback_path/fallback_revision}. A local_tree item preserves an unversioned
source tree. No reference programs, Git clones, installers, or providers run.
"""

import argparse
from concurrent.futures import ThreadPoolExecutor, as_completed
from datetime import datetime, timezone
import hashlib
import json
import os
from pathlib import Path, PurePosixPath
import re
import shutil
import stat
import subprocess
import sys
import tempfile
from urllib.parse import urlsplit
import uuid
from zipfile import ZipFile, ZipInfo

from ingest_zip import SKIP_PARTS


SCRIPT_DIR = Path(__file__).resolve().parent


def validate_item(item):
    name = item.get("name", "")
    if not re.fullmatch(r"[A-Za-z0-9][A-Za-z0-9_.-]*", name):
        raise ValueError(f"invalid reference name: {name!r}")
    repository = item.get("repository")
    if repository:
        url = urlsplit(repository.removesuffix(".git"))
        if url.scheme != "https" or url.netloc != "github.com" or url.query or url.fragment:
            raise ValueError("repository must be a public canonical GitHub HTTPS URL")
        if not re.fullmatch(r"/[A-Za-z0-9_.-]+/[A-Za-z0-9_.-]+/?", url.path):
            raise ValueError("repository URL must name one owner and repository")
    elif not item.get("local_tree"):
        raise ValueError("reference requires repository or local_tree")
    for key in ("revision", "fallback_revision"):
        if item.get(key) and not re.fullmatch(r"[0-9a-fA-F]{40}", item[key]):
            raise ValueError(f"{key} must be a full immutable Git commit")


def run(argv, *, timeout=180):
    environment = dict(os.environ, GIT_TERMINAL_PROMPT="0", GIT_ASKPASS="")
    completed = subprocess.run(argv, capture_output=True, text=True, timeout=timeout, env=environment)
    if completed.returncode:
        raise RuntimeError(f"{argv[0]} failed ({completed.returncode}): {completed.stderr.strip()[:1500]}")
    return completed.stdout.strip()


def archive_root(archive_path):
    with ZipFile(archive_path) as archive:
        roots = {PurePosixPath(info.filename).parts[0] for info in archive.infolist() if PurePosixPath(info.filename).parts}
    if len(roots) != 1:
        raise ValueError("source ZIP must have exactly one top-level directory")
    return roots.pop()


def compare_zip(archive_path, destination):
    root = archive_root(archive_path)
    omitted = []
    with ZipFile(archive_path) as archive:
        retained = {}
        for info in archive.infolist():
            path = PurePosixPath(info.filename)
            if path.is_absolute() or ".." in path.parts or "\\" in info.filename:
                raise ValueError(f"unsafe archive path: {info.filename}")
            mode = info.external_attr >> 16
            if any(p in SKIP_PARTS or p.startswith("._") for p in path.parts) or stat.S_ISLNK(mode):
                if not info.is_dir():
                    omitted.append(info.filename)
                continue
            if info.is_dir() or len(path.parts) == 1:
                continue
            if stat.S_IFMT(mode) not in {0, stat.S_IFREG}:
                raise ValueError(f"unsupported special file: {info.filename}")
            relative = Path(*path.parts[1:])
            if relative in retained:
                raise ValueError(f"duplicate archive path: {info.filename}")
            retained[relative] = info
        actual = {path.relative_to(destination) for path in destination.rglob("*") if path.is_file() or path.is_symlink()}
        if actual != set(retained):
            raise ValueError(f"file set differs: missing={len(set(retained)-actual)} extra={len(actual-set(retained))}")
        total = 0
        for relative, info in retained.items():
            path = destination / relative
            if path.is_symlink():
                raise ValueError(f"unexpected symbolic link: {relative}")
            with archive.open(info) as source, path.open("rb") as extracted:
                while True:
                    original = source.read(1024 * 1024)
                    local = extracted.read(1024 * 1024)
                    if original != local:
                        raise ValueError(f"bytes differ: {relative}")
                    if not original:
                        break
                    total += len(original)
            expected_executable = 0o111 if (info.external_attr >> 16) & 0o111 else 0
            if path.stat().st_mode & 0o111 != expected_executable:
                raise ValueError(f"executable bits differ: {relative}")
    return {"archive_root": root, "retained_files": len(retained), "retained_bytes": total,
            "omitted_entries": omitted,
            "comparison": "Exact retained file set, file bytes, and executable bits compared directly with source ZIP"}


def ingest_and_compare(archive_path, destination):
    if destination.exists() or destination.is_symlink():
        raise FileExistsError(f"existing reference left unchanged: {destination}")
    destination.parent.mkdir(parents=True, exist_ok=True)
    stage = Path(tempfile.mkdtemp(prefix=f".acquire-{destination.name}-", dir=destination.parent))
    extracted = stage / "snapshot"
    try:
        run([sys.executable, str(SCRIPT_DIR / "ingest_zip.py"), str(archive_path), str(extracted),
             "--root", archive_root(archive_path), "--skip-symlinks"], timeout=600)
        comparison = compare_zip(archive_path, extracted)
        if destination.exists() or destination.is_symlink():
            raise FileExistsError(f"existing reference left unchanged: {destination}")
        extracted.rename(destination)
        return comparison
    finally:
        shutil.rmtree(stage)


def local_tree_zip(source, archive_path, root):
    with ZipFile(archive_path, "w") as archive:
        for path in sorted(source.rglob("*")):
            relative = path.relative_to(source)
            if ".git" in relative.parts:
                continue
            if path.is_symlink():
                info = ZipInfo(f"{root}/{relative.as_posix()}")
                info.create_system = 3
                info.external_attr = (stat.S_IFLNK | 0o777) << 16
                archive.writestr(info, os.readlink(path).encode())
            elif path.is_file():
                archive.write(path, f"{root}/{relative.as_posix()}")


def acquire(item, archive_directory, quarantine_directory):
    validate_item(item)
    result = dict(item)
    name = item["name"]
    destination = quarantine_directory / name
    result["destination"] = str(destination)
    if destination.exists() or destination.is_symlink():
        result.update(status="existing_unchanged", note="Pre-existing snapshot was not replaced or reassessed")
        return result
    archive_directory.mkdir(parents=True, exist_ok=True)
    partial = archive_directory / f".{name}-{uuid.uuid4().hex}.part"
    try:
        upstream_failure = None
        if item.get("repository"):
            repository = item["repository"].removesuffix(".git").rstrip("/")
            result["repository"] = repository
            try:
                revision = item.get("revision") or run(["git", "-c", "credential.helper=", "ls-remote", repository, "HEAD"], timeout=90).split()[0]
                if not re.fullmatch(r"[0-9a-fA-F]{40}", revision):
                    raise ValueError("upstream HEAD did not resolve to a full commit")
                owner_repo = urlsplit(repository).path.strip("/")
                download_url = f"https://codeload.github.com/{owner_repo}/zip/{revision}"
                archive_path = archive_directory / f"{name}-{revision[:12]}.zip"
                if not archive_path.exists():
                    run(["curl", "--disable", "--fail", "--location", "--silent", "--show-error", "--retry", "2",
                         "--max-time", "600", "--connect-timeout", "30", "--output", str(partial), download_url], timeout=1900)
                    archive_root(partial)
                    partial.rename(archive_path)
                result.update(revision=revision, download_url=download_url, provenance="immutable upstream source ZIP")
            except Exception as error:
                upstream_failure = str(error)
                result["upstream_failure"] = upstream_failure
                if not item.get("fallback_path"):
                    raise
                revision = item.get("fallback_revision") or run(["git", "-C", item["fallback_path"], "rev-parse", "HEAD"])
                if not re.fullmatch(r"[0-9a-fA-F]{40}", revision):
                    raise ValueError("local fallback did not resolve to a full commit")
                archive_path = archive_directory / f"{name}-{revision[:12]}-historical.zip"
                if not archive_path.exists():
                    run(["git", "-C", item["fallback_path"], "archive", "--format=zip", f"--prefix={name}/",
                         f"--output={partial.resolve()}", revision])
                    archive_root(partial)
                    partial.rename(archive_path)
                result.update(revision=revision, provenance="historical local Git archive fallback", upstream_failure=upstream_failure)
        else:
            archive_path = archive_directory / f"{name}-local-unversioned.zip"
            if not archive_path.exists():
                local_tree_zip(Path(item["local_tree"]), partial, name)
                archive_root(partial)
                partial.rename(archive_path)
            result.update(revision=None, provenance="unversioned local source tree snapshot")
        result.update(archive=str(archive_path), archive_bytes=archive_path.stat().st_size)
        digest = hashlib.sha256()
        with archive_path.open("rb") as source:
            for chunk in iter(lambda: source.read(1024 * 1024), b""):
                digest.update(chunk)
        result["sha256"] = digest.hexdigest()
        result.update(ingest_and_compare(archive_path, destination))
        result["status"] = "acquired"
    except Exception as error:
        result.update(status="failed", error=str(error))
    finally:
        partial.unlink(missing_ok=True)
    return result


def write_catalog(path, records, input_path):
    path.parent.mkdir(parents=True, exist_ok=True)
    payload = {"recorded_at": datetime.now(timezone.utc).isoformat(), "input": str(input_path),
               "references": sorted(records.values(), key=lambda entry: entry["name"])}
    temporary = path.with_name(f".{path.name}-{uuid.uuid4().hex}.tmp")
    temporary.write_text(json.dumps(payload, indent=2) + "\n")
    temporary.replace(path)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("--input", type=Path, required=True)
    parser.add_argument("--catalog", type=Path, required=True)
    parser.add_argument("--archive-dir", type=Path, required=True)
    parser.add_argument("--quarantine-dir", type=Path, default=Path("quarantine"))
    parser.add_argument("--jobs", type=int, default=4, choices=range(1, 9))
    args = parser.parse_args()
    items = json.loads(args.input.read_text())
    if not isinstance(items, list):
        parser.error("input must be a JSON list")
    for item in items:
        validate_item(item)
    if len({item["name"] for item in items}) != len(items):
        parser.error("duplicate reference names")
    records = {}
    if args.catalog.exists():
        records = {record["name"]: record for record in json.loads(args.catalog.read_text())["references"]}
    pending = []
    for item in items:
        old = records.get(item["name"], {})
        if old.get("status") == "acquired" and Path(old.get("archive", "")).is_file() and Path(old.get("destination", "")).is_dir():
            print(f"{item['name']}: retained previous acquisition", flush=True)
        else:
            pending.append(item)
    with ThreadPoolExecutor(max_workers=args.jobs) as executor:
        futures = {executor.submit(acquire, item, args.archive_dir, args.quarantine_dir): item for item in pending}
        for future in as_completed(futures):
            result = future.result()
            records[result["name"]] = result
            write_catalog(args.catalog, records, args.input)
            print(f"{result['name']}: {result['status']} ({result.get('retained_files', 0)} retained files)", flush=True)
    if not pending:
        write_catalog(args.catalog, records, args.input)
    return 1 if any(record["status"] == "failed" for record in records.values()) else 0


if __name__ == "__main__":
    raise SystemExit(main())
