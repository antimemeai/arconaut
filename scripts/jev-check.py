#!/usr/bin/env python3
"""Snapshot an explicit Jev review case; --live calls the owned workspace client."""

import argparse
from datetime import datetime, timezone
import hashlib
import json
from pathlib import Path
import re
import subprocess
import sys
import time
import uuid


ROOT = Path(__file__).resolve().parents[1]
CLIENT = ROOT.parent / "codex-tools" / "jev.py"
MAX_REQUEST_BYTES = 65_536  # Local byte bound, not a provider token-budget claim.


def digest(data):
    return hashlib.sha256(data).hexdigest()


def read_bounded(path):
    with path.open("rb") as source:
        data = source.read(MAX_REQUEST_BYTES + 1)
    if len(data) > MAX_REQUEST_BYTES:
        raise ValueError("Input exceeds the local byte bound; select a bounded review packet.")
    return data


def section_snapshot(description):
    relative = Path(description["path"])
    if relative.is_absolute() or any(part.startswith(".env") for part in relative.parts):
        raise ValueError("Sources must be repository-relative noncredential files.")
    path = (ROOT / relative).resolve()
    if not path.is_relative_to(ROOT) or any(part.startswith(".env") for part in path.parts):
        raise ValueError("Source resolves outside the repository or to a credential file.")
    data = read_bounded(path)
    lines = data.decode("utf-8").splitlines(keepends=True)
    wanted = description["section"]
    headings = []
    fence = None
    for index, line in enumerate(lines):
        marker = re.match(r"^ {0,3}(`{3,}|~{3,})(.*)$", line.rstrip("\r\n"))
        if fence:
            if marker and marker[1][0] == fence[0] and len(marker[1]) >= fence[1] and not marker[2].strip():
                fence = None
            continue
        if marker and (marker[1][0] != "`" or "`" not in marker[2]):
            fence = (marker[1][0], len(marker[1]))
            continue
        heading = re.match(r"^(#{1,6}) (.+?)\s*$", line)
        if heading:
            headings.append((index, len(heading[1]), heading[2]))
    matches = [(index, level) for index, level, title in headings if title == wanted]
    if len(matches) != 1:
        raise ValueError("Source section must exist exactly once.")
    start, level = matches[0]
    end = len(lines)
    for index, next_level, _ in headings:
        if index > start and next_level <= level:
            end = index
            break
    text = "".join(lines[start:end])
    return {"path": str(relative), "section": wanted, "first_line": start + 1,
            "last_line": end, "file_sha256": digest(data),
            "section_sha256": digest(text.encode("utf-8")), "text": text}


def save(path, value):
    path.write_text(json.dumps(value, indent=2, allow_nan=False) + "\n")


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument("case", type=Path)
    parser.add_argument("--live", action="store_true")
    parser.add_argument("--run-dir", type=Path, help="New directory; existing paths reject.")
    args = parser.parse_args()
    phase = "preparation"
    record = None
    run_dir = None
    try:
        if any(part.startswith(".env") for part in args.case.resolve().parts):
            raise ValueError("Credential paths cannot be used as cases.")
        case_bytes = read_bounded(args.case)
        case = json.loads(case_bytes)
        if not isinstance(case["questions"], dict) or not case["questions"]:
            raise ValueError("A nonempty questions map is required.")
        if not isinstance(case["model"], str) or not case["model"].startswith("jev-"):
            raise ValueError("An explicit Jev model is required.")
        sources = {name: section_snapshot(desc) for name, desc in case["sources"].items()}
        request = {"model": case["model"], "state": {"case": case["state"], "sources": sources},
                   "questions": case["questions"]}
        encoded = json.dumps(request, allow_nan=False).encode("utf-8")
        if len(encoded) > MAX_REQUEST_BYTES:
            raise ValueError("Request exceeds the local byte bound.")
        stamp = datetime.now(timezone.utc).strftime("%Y%m%dT%H%M%SZ")
        run_dir = args.run_dir or ROOT / "context" / "jev" / (stamp + "-" + uuid.uuid4().hex[:8])
        run_dir.mkdir(parents=True, exist_ok=False, mode=0o700)
        save(run_dir / "request.json", request)
        record = {"case_sha256": digest(case_bytes), "live": args.live,
                  "request_bytes": len(encoded), "disposition": "prepared",
                  "wrapper_sha256": digest(Path(__file__).read_bytes())}
        save(run_dir / "run.json", record)
        if args.live:
            phase = "transport"
            record["client_sha256"] = digest(CLIENT.read_bytes())
            started = time.monotonic()
            completed = subprocess.run([sys.executable, str(CLIENT), str(run_dir / "request.json")],
                                       capture_output=True, timeout=120, check=False)
            record["elapsed_seconds"] = time.monotonic() - started
            if completed.returncode:
                raise ValueError("Shared Jev client failed; no response accepted.")
            response = json.loads(completed.stdout)
            save(run_dir / "response.json", response)
            record["actual_model"] = response["model"]
            if response["model"] != case["model"]:
                phase = "model_mismatch"
                raise ValueError("Response model differs from the pinned request.")
            if set(response["answers"]) != set(request["questions"]):
                raise ValueError("Response question IDs differ from the snapshot.")
            record.update(disposition="answered", actual_model=response["model"],
                          usage=response["usage"])
            print(json.dumps(response["answers"], indent=2))
        save(run_dir / "run.json", record)
        print(f"Jev {record['disposition']}; snapshot: {run_dir}")
        return 0
    except (OSError, ValueError, KeyError, TypeError, AttributeError, subprocess.TimeoutExpired):
        if record is not None:
            record["disposition"] = phase + "_failed"
            save(run_dir / "run.json", record)
        print(f"Jev {phase} failed; inspect the case/source paths or shared client setup.", file=sys.stderr)
        return 1


if __name__ == "__main__":
    sys.exit(main())
