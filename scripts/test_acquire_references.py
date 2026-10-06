#!/usr/bin/env python3
"""Small local oracles for reference acquisition; no network or imported code."""

from pathlib import Path
import stat
import tempfile
import unittest
from unittest.mock import patch
from zipfile import ZipFile, ZipInfo

import acquire_references as acquisition


def fixture(path, unsafe=False):
    with ZipFile(path, "w") as archive:
        for name, data, mode in [
            ("fixture/src/program", b"direct bytes\n", stat.S_IFREG | 0o755),
            ("fixture/plain", b"plain\x00data", stat.S_IFREG | 0o644),
            ("fixture/.git/config", b"historical metadata", stat.S_IFREG | 0o644),
            ("fixture/link", b"plain", stat.S_IFLNK | 0o777),
        ]:
            info = ZipInfo(name)
            info.create_system = 3
            info.external_attr = mode << 16
            archive.writestr(info, data)
        if unsafe:
            archive.writestr("fixture/../escape", b"escape")


class AcquisitionTests(unittest.TestCase):
    def test_exact_set_bytes_modes_and_omissions(self):
        with tempfile.TemporaryDirectory() as temporary:
            base = Path(temporary)
            archive = base / "fixture.zip"
            fixture(archive)
            destination = base / "reference"
            result = acquisition.ingest_and_compare(archive, destination)
            self.assertEqual(result["retained_files"], 2)
            self.assertEqual(set(result["omitted_entries"]), {"fixture/.git/config", "fixture/link"})
            self.assertEqual((destination / "src/program").read_bytes(), b"direct bytes\n")
            self.assertEqual((destination / "src/program").stat().st_mode & 0o111, 0o111)
            (destination / "plain").write_bytes(b"corrupted")
            with self.assertRaisesRegex(ValueError, "bytes differ"):
                acquisition.compare_zip(archive, destination)
            (destination / "plain").write_bytes(b"plain\x00data")
            (destination / "src/program").chmod(0o644)
            with self.assertRaisesRegex(ValueError, "executable bits differ"):
                acquisition.compare_zip(archive, destination)
            (destination / "extra").write_bytes(b"extra")
            with self.assertRaisesRegex(ValueError, "file set differs"):
                acquisition.compare_zip(archive, destination)

    def test_unsafe_archive_leaves_no_snapshot_or_stage(self):
        with tempfile.TemporaryDirectory() as temporary:
            base = Path(temporary)
            archive = base / "unsafe.zip"
            fixture(archive, unsafe=True)
            with self.assertRaises(Exception):
                acquisition.ingest_and_compare(archive, base / "reference")
            self.assertFalse((base / "reference").exists())
            self.assertEqual(sorted(p.name for p in base.iterdir()), ["unsafe.zip"])

    def test_existing_reference_not_overwritten(self):
        with tempfile.TemporaryDirectory() as temporary:
            base = Path(temporary)
            archive = base / "fixture.zip"
            fixture(archive)
            destination = base / "reference"
            destination.mkdir()
            (destination / "original").write_bytes(b"keep")
            with self.assertRaises(FileExistsError):
                acquisition.ingest_and_compare(archive, destination)
            self.assertEqual((destination / "original").read_bytes(), b"keep")

    def test_names_and_repository_urls_are_bounded(self):
        for name in ["../elsewhere", ".", "nested/path", ""]:
            with self.assertRaises(ValueError):
                acquisition.validate_item({"name": name, "repository": "https://github.com/owner/repo"})
        for url in ["https://token@github.com/owner/repo", "https://example.com/owner/repo", "https://github.com/owner/repo?token=x"]:
            with self.assertRaises(ValueError):
                acquisition.validate_item({"name": "valid", "repository": url})

    def test_historical_fallback_resolves_archive_path_outside_repository(self):
        with tempfile.TemporaryDirectory(dir=".") as temporary:
            base = Path(temporary)
            source = base / "historical"
            source.mkdir()
            acquisition.run(["git", "-C", str(source), "init"])
            acquisition.run(["git", "-C", str(source), "config", "user.name", "Local acquisition oracle"])
            acquisition.run(["git", "-C", str(source), "config", "user.email", "oracle@example.invalid"])
            (source / "source").write_bytes(b"historical source\n")
            acquisition.run(["git", "-C", str(source), "add", "source"])
            acquisition.run(["git", "-C", str(source), "commit", "-m", "Fixture"])
            revision = acquisition.run(["git", "-C", str(source), "rev-parse", "HEAD"])
            original_run = acquisition.run

            def reject_remote(argv, **kwargs):
                if "ls-remote" in argv:
                    raise RuntimeError("unavailable upstream fixture")
                return original_run(argv, **kwargs)

            with patch.object(acquisition, "run", side_effect=reject_remote):
                result = acquisition.acquire({"name": "fixture", "repository": "https://github.com/owner/repo",
                    "fallback_path": str(source), "fallback_revision": revision}, base / "archives", base / "quarantine")
            self.assertEqual(result["status"], "acquired", result)
            self.assertEqual(result["provenance"], "historical local Git archive fallback")
            self.assertEqual((base / "quarantine/fixture/source").read_bytes(), b"historical source\n")


if __name__ == "__main__":
    unittest.main()
