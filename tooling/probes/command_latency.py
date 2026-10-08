#!/usr/bin/env python3
"""Bounded native launcher/PTTY local-command probe; no provider calls.
The marker is absent from the submitted source and follows a successful audited
workflow_registry call. Temporary test sessions are discarded, metrics retained.
"""
import argparse
import ctypes
import errno
import fcntl
import hashlib
import json
import os
from pathlib import Path
import pty
import resource
import select
import shutil
import signal
import struct
import subprocess
import tempfile
import time

ROOT = Path(__file__).resolve().parents[2]
FRAME = b"\x1b[?2026l"
MARKER = b"LATENCY_LOCAL_OK"
COMMAND = b'/lua local r=blackbird.call("workflow_registry",{}); assert(not r.error); print("LATENCY_".."LOCAL_OK")\r'


def process_cpu(pid):
    """Cumulative user/system CPU of this live process, excluding child/tool CPU."""
    if os.uname().sysname == "Darwin":
        library = ctypes.CDLL("/usr/lib/libproc.dylib", use_errno=True)
        # rusage_info_v0: uuid[16], then user/system Mach absolute-time ticks.
        data = ctypes.create_string_buffer(256)
        if library.proc_pid_rusage(pid, 0, ctypes.byref(data)) != 0:
            raise OSError(ctypes.get_errno(), "proc_pid_rusage failed")
        user, system = struct.unpack_from("QQ", data.raw, 16)
        timebase = (ctypes.c_uint32 * 2)()
        if ctypes.CDLL(None).mach_timebase_info(ctypes.byref(timebase)) != 0 or not timebase[1]:
            raise RuntimeError("mach_timebase_info failed")
        return (user + system) * timebase[0] / timebase[1] / 1e9
    fields = Path(f"/proc/{pid}/stat").read_text().rsplit(")", 1)[1].split()
    return (int(fields[11]) + int(fields[12])) / os.sysconf("SC_CLK_TCK")


def sample(executable, idle, tail, sync_probe, direct, seed=None, timing=False, published=False):
    with tempfile.TemporaryDirectory(prefix="blackbird-latency-") as directory:
        if seed:
            shutil.copytree(seed, directory, dirs_exist_ok=True)
        master, slave = pty.openpty()
        fcntl.ioctl(slave, 0x80087467 if os.uname().sysname == "Darwin" else 0x5414,
                    struct.pack("HHHH", 35, 110, 0, 0))
        env = dict(os.environ, BLACKBIRD_EXECUTABLE=str(executable), TERM="xterm-256color")
        if published:
            env.pop("BLACKBIRD_EXECUTABLE", None)
            env.pop("ARCO_EXECUTABLE", None)
        timing_path = Path(directory) / "local-phases.jsonl"
        if timing:
            env["BLACKBIRD_LOCAL_TIMING"] = str(timing_path)
        sync_log = Path(directory) / "sync-count.log"
        if sync_probe:
            env["DYLD_INSERT_LIBRARIES"] = str(sync_probe)
            env["BLACKBIRD_UI_SYNC_LOG"] = str(sync_log)
        usage_before = resource.getrusage(resource.RUSAGE_CHILDREN)
        started_clock = time.clock_gettime(time.CLOCK_MONOTONIC)
        started = time.monotonic()
        command = [str(executable) if direct else str(ROOT / "scripts/blackbird"), "--session", directory]
        process = subprocess.Popen(command,
                                   stdin=slave, stdout=slave, stderr=slave, env=env, start_new_session=True)
        os.close(slave)
        raw = bytearray()
        frame = sent = marker = complete = None
        frame_clock = sent_clock = marker_clock = complete_clock = None
        failure = None
        idle_cpu_before = idle_cpu_ms = None
        native_pid = process.pid if direct else None
        idle_measured_start = None
        deadline = started + 8
        try:
            while time.monotonic() < deadline:
                now = time.monotonic()
                if frame is not None and sent is None and now >= frame + idle:
                    sent = now
                    sent_clock = time.clock_gettime(time.CLOCK_MONOTONIC)
                    if idle_cpu_before is not None:
                        idle_cpu_ms = (process_cpu(native_pid) - idle_cpu_before) * 1000
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
                    frame_clock = time.clock_gettime(time.CLOCK_MONOTONIC)
                    if idle:
                        if native_pid is None:
                            children = subprocess.check_output(
                                ["pgrep", "-P", str(process.pid)], text=True).split()
                            if len(children) != 1:
                                raise RuntimeError("launcher must have one native child for idle CPU")
                            native_pid = int(children[0])
                        idle_cpu_before = process_cpu(native_pid)
                        idle_measured_start = time.monotonic()
                if sent is not None and marker is None and MARKER in raw:
                    marker = now
                    marker_clock = time.clock_gettime(time.CLOCK_MONOTONIC)
                if marker is not None and b"Turn completed" in raw:
                    complete = now
                    complete_clock = time.clock_gettime(time.CLOCK_MONOTONIC)
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
                      idle_seconds=idle, idle_cpu_ms=idle_cpu_ms,
                      idle_measured_seconds=None if idle_measured_start is None else sent - idle_measured_start,
                      seed=str(seed) if seed else None, trailing_input=tail, error=failure,
                      process_cpu_ms=((usage_after.ru_utime + usage_after.ru_stime) -
                                      (usage_before.ru_utime + usage_before.ru_stime)) * 1000)
        if timing:
            if not timing_path.exists():
                result["error"] = result["error"] or "timing requested but unavailable (requires BLACKBIRD_DEBUG build)"
            else:
                result["clock_monotonic_seconds"] = dict(spawn=started_clock, first_frame=frame_clock,
                                                        submit=sent_clock, marker=marker_clock,
                                                        complete=complete_clock)
                result["native_phases"] = [json.loads(line) for line in timing_path.read_text().splitlines()]
                startup = next((row for row in result["native_phases"] if row.get("action") == "startup.initialize"), None)
                if startup:
                    result["spawn_to_native_entry_ms"] = (startup["start_wall_ns"] / 1e9 - started_clock) * 1000
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
    parser.add_argument("--seed", type=Path, help="private copy of a settled session fixture")
    parser.add_argument("--timing", action="store_true", help="record native phase timings from debug-enabled binary")
    parser.add_argument("--published", action="store_true", help="use the ordinary launcher default without an executable override")
    parser.add_argument("--direct", action="store_true", help="native diagnostic, excludes shell launcher")
    parser.add_argument("--output", type=Path, required=True)
    args = parser.parse_args()
    if not 1 <= args.samples <= 30 or not 0 <= args.idle_seconds <= 3:
        parser.error("samples must be1..30 and idle-seconds0..3")
    if len(args.tail) > 128 or any(not (c.isascii() and (c.isalnum() or c in "_ ")) for c in args.tail):
        parser.error("tail must be at most128 ASCII alphanumeric/underscore/space characters")
    if args.published and (args.direct or args.baseline):
        parser.error("published requires launcher mode without a baseline")
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
                  direct_native=args.direct, published_launcher=args.published,
                  idle_cpu_clock="native PID user+system CPU; macOS Mach ticks converted with mach_timebase_info; Linux ticks/SC_CLK_TCK",
                  endpoints="Private fresh/copied sessions, ordinary cache. No provider. process_cpu_ms covers all phases; idle_cpu_ms only first-frame to submission.")
    for index in range(args.samples):
        # Alternate pair order to reduce monotonic host-load bias.
        for label, path in binaries[::1 if index % 2 == 0 else -1]:
            result = sample(path, args.idle_seconds, args.tail, args.sync_probe, args.direct, args.seed, args.timing, args.published)
            report["samples"].append(dict(binary=label, index=index, **result))
            args.output.parent.mkdir(parents=True, exist_ok=True)
            report["load_after"] = os.getloadavg()
            args.output.write_text(json.dumps(report, indent=2) + "\n")
            print(label, index, json.dumps({key: value for key, value in result.items()
                                           if key not in ("native_phases", "failure_output_hex")}), flush=True)
            if result["error"] or (args.sync_probe and not result["ui_state_fsyncs"]):
                return 1
    return 0


if __name__ == "__main__":
    raise SystemExit(main())
