#!/usr/bin/env python3
"""Owned bounded platform experiments, independent of Blackbird's implementation."""
import errno, os, platform, pty, select, signal, subprocess, sys, time

def read_until(fd, suffix, timeout=5):
 out=bytearray(); deadline=time.monotonic()+timeout
 while not out.endswith(suffix):
  remain=deadline-time.monotonic()
  if remain<=0 or not select.select([fd],[],[],remain)[0]:raise AssertionError(('read timeout',bytes(out),suffix))
  data=os.read(fd,4096)
  if not data:raise AssertionError(('early EOF',bytes(out),suffix))
  out.extend(data)
 return bytes(out)

def launch(code):
 return subprocess.Popen([sys.executable,'-u','-c',code],stdin=subprocess.PIPE,stdout=subprocess.PIPE,stderr=subprocess.PIPE,start_new_session=True)

def cleanup(p):
 try:os.killpg(p.pid,signal.SIGKILL)
 except ProcessLookupError:pass
 for pipe in (p.stdin,p.stdout,p.stderr):
  if pipe:pipe.close()
 if p.poll() is None:p.wait(timeout=5)

print('platform',platform.platform(),flush=True)
p=launch("import os,sys; print('READY',os.getpid(),flush=True); a=sys.stdin.buffer.read(1); print('STEP',os.getpid(),a.hex(),flush=True); b=sys.stdin.buffer.read(1); print('FINAL',os.getpid(),b.hex(),flush=True)")
try:
 first=read_until(p.stdout.fileno(),b'\n');pid=p.pid
 assert first==f'READY {pid}\n'.encode()
 # Releasing this caller's wait executes no signal, close, or process replacement.
 p.stdin.write(b'a');p.stdin.flush();second=read_until(p.stdout.fileno(),b'\n')
 assert second==f'STEP {pid} 61\n'.encode() and p.poll() is None
 p.stdin.write(b'b');p.stdin.flush();tail=read_until(p.stdout.fileno(),b'\n')
 assert tail==f'FINAL {pid} 62\n'.encode() and p.wait(timeout=5)==0
 print('PASS same-process progression while caller releases and resumes wait',flush=True)
finally:cleanup(p)
p=launch("import os,sys; pid=os.fork();\nif pid==0:\n os.write(1,b'CHILD_READY\\n'); sys.stdin.buffer.read(1); os.write(1,b'CHILD_TAIL\\n'); os._exit(0)\nos._exit(23)")
try:
 assert read_until(p.stdout.fileno(),b'\n')==b'CHILD_READY\n'
 assert p.wait(timeout=5)==23
 os.set_blocking(p.stdout.fileno(),False)
 try:os.read(p.stdout.fileno(),1);raise AssertionError('pipe wrongly reached EOF')
 except BlockingIOError:pass
 p.stdin.write(b'x');p.stdin.flush();os.set_blocking(p.stdout.fileno(),True)
 assert read_until(p.stdout.fileno(),b'\n')==b'CHILD_TAIL\n'
 assert select.select([p.stdout.fileno()],[],[],5)[0] and os.read(p.stdout.fileno(),1)==b''
 print('PASS root exit 23 precedes descendant-held stdout EOF and retained tail',flush=True)
finally:cleanup(p)
p=launch("import os,sys; os.close(1); os.close(2); sys.stdin.buffer.read(1); os._exit(31)")
try:
 assert select.select([p.stdout.fileno()],[],[],5)[0] and os.read(p.stdout.fileno(),1)==b''
 assert p.poll() is None
 p.stdin.write(b'x');p.stdin.flush();assert p.wait(timeout=5)==31
 print('PASS stdout EOF precedes root exit 31',flush=True)
finally:cleanup(p)
p=launch("import os,sys; print('READY',flush=True); sys.stdin.buffer.read(1); print('DONE',flush=True)")
try:
 assert read_until(p.stdout.fileno(),b'\n')==b'READY\n'
 os.kill(p.pid,signal.SIGSTOP);got,status=os.waitpid(p.pid,os.WUNTRACED)
 assert got==p.pid and os.WIFSTOPPED(status) and os.WSTOPSIG(status)==signal.SIGSTOP
 os.kill(p.pid,signal.SIGCONT);p.stdin.write(b'x');p.stdin.flush()
 assert read_until(p.stdout.fileno(),b'\n')==b'DONE\n' and p.wait(timeout=5)==0
 print('PASS stop is a distinct observed child state; continue same PID',flush=True)
finally:cleanup(p)
pid,master=pty.fork()
if pid==0:
 code="import os,sys; print('READY',os.getpid(),os.getpgrp(),os.getsid(0),os.tcgetpgrp(0),os.isatty(0),flush=True); line=sys.stdin.buffer.readline(); print('INPUT',line.hex(),flush=True); sys.exit(17)"
 os.execv(sys.executable,[sys.executable,'-u','-c',code])
try:
 first=read_until(master,b'\r\n')
 assert first==f'READY {pid} {pid} {pid} {pid} True\r\n'.encode(),first
 os.write(master,b'hello\n');data=read_until(master,b'INPUT 68656c6c6f0a\r\n')
 got,status=os.waitpid(pid,0);assert got==pid and os.WIFEXITED(status) and os.WEXITSTATUS(status)==17
 assert select.select([master],[],[],5)[0]
 try:
  end=os.read(master,100);assert end==b'';result='zero-byte EOF'
 except OSError as exc:
  assert exc.errno==errno.EIO;result='EIO'
 print('PASS own PTY session/foreground; canonical input, echo, CRLF; final read',result,flush=True)
finally:
 os.close(master)
 try:os.kill(pid,signal.SIGKILL)
 except ProcessLookupError:pass
 try:os.waitpid(pid,0)
 except ChildProcessError:pass
print('All five bounded primary-platform probes passed.',flush=True)
