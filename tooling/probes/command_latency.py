#!/usr/bin/env python3
"""Bounded native launcher/PTTY local-command probe; no provider calls.
The marker is absent from the submitted source and follows a successful audited
workflow_registry call. Temporary test sessions are discarded, metrics retained.
"""
import argparse
import errno
import fcntl
import hashlib
import json
import os
from pathlib import Path
import pty
import resource
import select
import signal
import struct
import subprocess
import tempfile
import time

ROOT = Path(__file__).resolve().parents[2]
FRAME = b"\x1b[?2026l"
MARKER = b"LATENCY_LOCAL_OK"
COMMAND = b'/lua local r=blackbird.call("workflow_registry",{}); assert(not r.error); print("LATENCY_".."LOCAL_OK")\r'


def sample(executable, idle, tail, sync_probe, direct):
    with tempfile.TemporaryDirectory(prefix="blackbird-latency-") as directory:
        master, slave = pty.openpty()
        fcntl.ioctl(slave, 0x80087467 if os.uname().sysname == "Darwin" else 0x5414,
                    struct.pack("HHHH", 35, 110, 0, 0))
        env = dict(os.environ, BLACKBIRD_EXECUTABLE=str(executable), TERM="xterm-256color")
        sync_log = Path(directory) / "sync-count.log"
        if sync_probe:
            env["DYLD_INSERT_LIBRARIES"] = str(sync_probe)
            env["BLACKBIRD_UI_SYNC_LOG"] = str(sync_log)
        usage_before = resource.getrusage(resource.RUSAGE_CHILDREN)
        started = time.monotonic()
        command = [str(executable) if direct else str(ROOT / "scripts/blackbird"), "--session", directory]
        process = subprocess.Popen(command,
                                   stdin=slave, stdout=slave, stderr=slave, env=env, start_new_session=True)
        os.close(slave)
        raw = bytearray()
        frame = sent = marker = complete = None
        failure = None
        deadline = started + 8
        try:
            while time.monotonic() < deadline:
                now = time.monotonic()
                if frame is not None and sent is None and now >= frame + idle:
                    sent = now
                    os.write(master, COMMAND + tail.encode())
                wait = min(0.02, max(0, deadline - now))
                if frame is not None and sent is None:
                    wait = min(wait, max(0, frame + idle - now))
                if not select.select([master], [], [], wait)[0]:
                    continue
                try:
                    data = os.read(master, 65536)
                except OSError as error:
                    if error.errno == errno.EIO:
                        break
                    raise
                if not data:
                    break
                raw.extend(data)
                if len(raw) > 4 * 1024 * 1024:
                    raise RuntimeError("probe output bound exceeded")
                now = time.monotonic()
                if frame is None and FRAME in raw:
                    frame = now
                if sent is not None and marker is None and MARKER in raw:
                    marker = now
                if marker is not None and b"Turn completed" in raw:
                    complete = now
                    break
            if complete is None:
                raise RuntimeError("missing successful local command/completion endpoint")
            # UI state must preserve any bytes delivered after Enter in the same batch.
            saved = json.loads((Path(directory) / "ui-state.json").read_text())
            if bytes.fromhex(saved["draft"]).decode() != tail:
                raise RuntimeError("trailing input was not persisted")
            os.write(master, b"\x01\x0b\x04")  # clear trailing draft, quit while idle
            code = process.wait(timeout=3)
            if code != 0:
                raise RuntimeError(f"launcher exited {code}")
        except Exception as error:
            failure = str(error)
            # Private probe process group only; no provider or user command effects.
            try:
                os.killpg(process.pid, signal.SIGKILL)
            except ProcessLookupError:
                pass
            process.wait(timeout=3)
        finally:
            os.close(master)
        usage_after = resource.getrusage(resource.RUSAGE_CHILDREN)
        ms = lambda endpoint, origin: None if endpoint is None or origin is None else (endpoint - origin) * 1000
        result = dict(first_frame_ms=ms(frame, started), command_marker_ms=ms(marker, sent),
                      command_complete_ms=ms(complete, sent), spawn_to_marker_ms=ms(marker, started),
                      idle_seconds=idle, trailing_input=tail, error=failure,
                      process_cpu_ms=((usage_after.ru_utime + usage_after.ru_stime) -
                                      (usage_before.ru_utime + usage_before.ru_stime)) * 1000)
        if sync_probe:
            result["ui_state_fsyncs"] = len(sync_log.read_bytes().splitlines()) if sync_log.exists() else 0
        if failure:
            result["failure_output_hex"] = raw.hex()
        return result


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--executable", type=Path, default=ROOT / "build/release/blackbird")
    parser.add_argument("--baseline", type=Path)
    parser.add_argument("--samples", type=int, default=6)
    parser.add_argument("--idle-seconds", type=float, default=0)
    parser.add_argument("--tail", default="")
    parser.add_argument("--sync-probe", type=Path)
    parser.add_argument("--direct", action="store_true", help="native diagnostic, excludes shell launcher")
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    if not 1 <= args.samples <= 30 or not 0 <= args.idle_seconds <= 1:
        parser.error("samples must be1..30 and idle-seconds0..1")
    if len(args.tail) > 128 or any(not (c.isascii() and (c.isalnum() or c in "_ ")) for c in args.tail):
        parser.error("tail must be at most128 ASCII alphanumeric/underscore/space characters")
    if args.sync_probe:
        args.sync_probe = args.sync_probe.resolve()
        if not args.direct:
            parser.error("sync-probe requires --direct; protected system shells strip dyld injection")
        if os.uname().sysname != "Darwin" or not args.sync_probe.is_file():
            parser.error("sync-probe requires a macOS measurement dylib")
    binaries = [("candidate", args.executable.resolve())]
    if args.baseline:
        binaries.insert(0, ("baseline", args.baseline.resolve()))
    report = dict(binary_sha256={label: hashlib.sha256(path.read_bytes()).hexdigest() for label, path in binaries},
                  samples=[], load_before=os.getloadavg(), platform=os.uname().sysname,
                  direct_native=args.direct,
                  endpoints="Fresh private sessions, ordinary cache. No provider. CPU includes all phases, not idle CPU.")
    for index in range(args.samples):
        # Alternate pair order to reduce monotonic host-load bias.
        for label, path in binaries[::1 if index % 2 == 0 else -1]:
            result = sample(path, args.idle_seconds, args.tail, args.sync_probe, args.direct)
            report["samples"].append(dict(binary=label, index=index, **result))
            args.output.parent.mkdir(parents=True, exist_ok=True)
            report["load_after"] = os.getloadavg()
            args.output.write_text(json.dumps(report, indent=2) + "\n")
            print(label, index, json.dumps(result), flush=True)
            if result["error"] or (args.sync_probe and not result["ui_state_fsyncs"]):
                return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
