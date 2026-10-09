#!/usr/bin/env python3
"""Development-only direct native station process oracle; no provider request."""
import json
from pathlib import Path
import importlib.util as packet_import
packet_spec = packet_import.spec_from_file_location('native_packet', Path(__file__).resolve().parent.parent / 'scripts/native-packet.py')
native_packet = packet_import.module_from_spec(packet_spec)
packet_spec.loader.exec_module(native_packet)
import subprocess
import sys
import tempfile
import time


def wait(predicate):
    end = time.monotonic() + 12
    while time.monotonic() < end:
        if predicate():
            return
        time.sleep(.05)
    raise AssertionError("station observation deadline")


def put(path, packet):
    tmp = path.with_suffix(".tmp")
    tmp.write_text(json.dumps(packet))
    tmp.replace(path)


with tempfile.TemporaryDirectory(prefix="arco-station-driver-") as tmp:
    root = Path(tmp)
    adapter = root / "adapter"
    adapter.mkdir()
    session = root / "session"
    effect = root / "effects"
    workflow = root / "workflow.lua"
    # The counter is a real audited native tool effect, not a mock dispatch flag.
    workflow.write_text("""
local p = %s
local n = 0
local ok, v = pcall(arco.call, 'read_file', {path=p})
if ok then n = tonumber(v.content) or 0 end
arco.call('write_file', {path=p,content=tostring(n+1)})
local c=arco.context()
arco.call('write_file',{path=%s,content=arco.json.encode(c)})
if tostring(arco.json.encode(c)):find('FAIL_EVENT',1,true) then error('unknown after effect') end
""" % (json.dumps(str(effect)), json.dumps(str(root / "context.json"))))
    # read_file content field is qualified below by actual counter observation.
    put(adapter / "control.json", {"id": "initial-pause", "action": "pause"})
    event = {"source": "repo", "id": "source-1", "cursor": "commit-1", "prompt": "useful source inventory"}
    put(adapter / "events.json", [event, event])
    output = (root / "output").open("w")
    process = subprocess.Popen([sys.argv[1], "--session", str(session), "--workflow", str(workflow),
                               "--station", str(adapter)], stdout=output, stderr=output)
    def status():
        try:
            return native_packet.read_packet((session / "station-status.bbm"))
        except (FileNotFoundError, json.JSONDecodeError):
            return {}
    try:
        wait(lambda: status().get("phase") == "paused")
        before = status()
        audit_size = (session / "audit").stat().st_size
        time.sleep(.8)
        assert not effect.exists()
        assert (session / "audit").stat().st_size == audit_size, "idle generated audit/inference"
        put(adapter / "control.json", {"id": "steer-1", "action": "steer", "text": "narrow station ownership"})
        wait(lambda: status().get("station", {}).get("steer") == "narrow station ownership")
        put(adapter / "control.json", {"id": "resume-1", "action": "resume"})
        wait(lambda: (status().get("station", {}).get("events") or [{}])[0].get("outcome") == "workflow_returned")
        assert effect.read_text() == "1"
        assert "narrow station ownership" in (root / "context.json").read_text()
        time.sleep(.7)
        assert effect.read_text() == "1", "duplicate dispatched"
        assert status()["pid"] == before["pid"] == process.pid and status()["actor"] == before["actor"]
        assert status()["context_head"] != before["context_head"]
        failed = dict(event, id="source-2", prompt="FAIL_EVENT")
        put(adapter / "events.json", [event, failed, failed])
        wait(lambda: status().get("phase") == "paused")
        assert effect.read_text() == "2", "actual uncertain effect missing"
        put(adapter / "control.json", {"id": "resume-2", "action": "resume"})
        wait(lambda: status().get("phase") == "idle")
        time.sleep(.7)
        assert effect.read_text() == "2", "unknown admission replayed"
        put(adapter / "control.json", {"id": "stop-1", "action": "stop"})
        assert process.wait(timeout=12) == 0
        assert status()["phase"] == "stopped"
        print("actual station: idle/no inference, pause/steer stable owner, duplicate and unknown no replay passed")
    finally:
        if process.poll() is None:
            process.send_signal(2)
            try:
                process.wait(timeout=5)
            except subprocess.TimeoutExpired:
                process.kill()
                process.wait()
        output.close()
        if process.returncode != 0:
            print((root / "output").read_text())

# Real RRC scheduling restoration: do not turn the successor's continue into a
# new event. Preserve station actor, retained control and admission identities.
with tempfile.TemporaryDirectory(prefix="arco-station-rrc-") as tmp:
    root = Path(tmp)
    adapter = root / "adapter"
    adapter.mkdir()
    session = root / "session"
    effect = root / "effect"
    workflow = root / "restart.lua"
    workflow.write_text("arco.call('write_file',{path=%s,content='once'}); arco.call('restart',{note='station RRC oracle'})" % json.dumps(str(effect)))
    event = {"source": "build", "id": "rrc-event", "cursor": "build-1", "prompt": "record actual RRC event"}
    put(adapter / "events.json", [event, event])
    first = subprocess.run([sys.argv[1], "--session", str(session), "--workflow", str(workflow),
                            "--station", str(adapter)], capture_output=True, text=True, timeout=15)
    assert first.returncode == 75, first.stdout + first.stderr
    before = native_packet.read_packet((session / "station-status.bbm"))
    size = effect.stat().st_mtime_ns
    output = (root / "output").open("w")
    second = subprocess.Popen([sys.argv[1], "--session", str(session), "--resume-continue"], stdout=output, stderr=output)
    try:
        def resumed():
            try:
                s = native_packet.read_packet((session / "station-status.bbm"))
                return s if s.get("pid") == second.pid and s.get("phase") == "idle" else None
            except (FileNotFoundError, json.JSONDecodeError):
                return None
        wait(resumed)
        assert resumed()["actor"] == before["actor"]
        time.sleep(.8)
        assert second.poll() is None and effect.stat().st_mtime_ns == size, "RRC replayed/continued"
        # Malformed source must stop visibly and retain paused scheduling, not infer.
        put(adapter / "events.json", [{}])
        assert second.wait(timeout=12) == 1
        s = native_packet.read_packet((session / "station-status.bbm"))
        assert s["phase"] == "blocked" and s["station"]["paused"]
        print("station RRC: stable actor, no implicit continue/replay, malformed source blocked passed")
    finally:
        if second.poll() is None:
            second.send_signal(2)
            try:
                second.wait(timeout=5)
            except subprocess.TimeoutExpired:
                second.kill()
                second.wait()
        output.close()
