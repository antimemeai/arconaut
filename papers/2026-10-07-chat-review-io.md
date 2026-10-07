# Rich chat tribunal: terminal transport and lifecycle

2026-10-07. One adversarial review round of the working candidate. Read-only
review except this report; no provider calls, production edits or commits.
Scope: packet lifetime, partial writes, terminal/editor handoff, wakeup lifecycle,
resize, cancellation and cache recovery. Source locations refer to the candidate
before findings are integrated.

## P1 — Terminal control bypasses the nonblocking writer

Locations: src/terminal.cpp:210–223, 960–963, 1150–1153;
src/chat_view.cpp:414–425.

ChatOutput changes stdout's open-file-description flags to O_NONBLOCK. Entry,
editor handoff and teardown still use unchecked std::cout writes. More seriously,
quit and native restart break even when a frame packet remains pending. Mode
cleanup runs before ChatOutput restores stdout's original flags. Cleanup therefore
uses a different output path while the data path may have left a partial CSI in
the terminal parser. std::cout neither tracks partial progress nor reports failure
here. Failure, buffering or blocking during cleanup can leave alternate-screen,
synchronized-output, cursor or paste modes inconsistent. Editor subprocesses also
inherit the modified stdout flags; a successful frame flush does not restore them.

Bounded actual-PTY observation using build/release/arco, no model calls: start an
80x120 or 240x4096 terminal, stop reading its master, then send Ctrl-Q. The process
did not exit within one second; after draining the master it exited zero. Captured
output contained only 1,058 bytes, with a truncated frame followed by terminal
cleanup. This confirms shutdown is not independent of terminal backpressure. It
is not evidence that every such shutdown loses cleanup bytes.

Remedy: give terminal control sequences the same explicit packet/partial-write
ownership as frame output. Define graceful exit and interrupted-packet recovery,
with a finite drain allowance; do not append cleanup blindly to an arbitrary
partial escape. Restore descriptor flags before launching the editor and restore
nonblocking operation deliberately afterward. Check handoff writes. Keep termios
restoration independent of whether output succeeds. A focused PTY case should
stop master reads, request exit, then resume reading and inspect both emitted
cleanup and native terminal flags.

## P2 — Unrelated readiness prematurely resolves an Escape key

Locations: src/terminal.cpp:551–558, 1323–1344.

The previous loop resolved pending Escape only on poll timeout. The new loop
calls Composer::flush_escape whenever stdin lacks readiness, including immediate
wakeup-pipe readiness and stdout POLLOUT. If a slash menu/palette has received the
first ESC byte of an arrow sequence, a worker notification or writable-output
notification can close the menu before the remaining `[B` bytes arrive. Those
bytes then become draft text instead of selection motion. This is a real
interleaving supported by both the composer and poll loop, not an exotic invalid
terminal sequence; terminal and network transports may split key sequences.

Remedy: record an Escape deadline and resolve only after that deadline, retaining
it across non-input wakeups. The deadline must be based on input arrival, not a
fresh timeout after every notification. A focused oracle interleaves ESC, a worker
wakeup, and `[B` before the deadline and requires selection movement with no
literal bytes in the draft.

## P2 — Tiny-terminal packet completion validates an unrelated grid

Locations: src/terminal.cpp:1086–1087, 1187–1197.

The small-terminal branch writes small_frame directly and invalidates the painter.
If that packet is incomplete, the common completion path later calls
painter.commit(grid). grid still describes the previous normal-sized screen;
the tiny packet never represented it. This marks a false screen state as valid.
Returning to the earlier geometry can then use a partial delta against that false
state instead of repairing the top row replaced by the tiny composer. The common
completion path needs to know whether a pending packet belongs to a grid frame or
the fallback presentation.

Remedy: explicitly tag packet ownership/type. Only a successfully emitted grid
packet may commit its corresponding grid. Fallback completion must leave the
painter invalid. Clear stale physical rows on entering the tiny fallback. Exercise
normal -> tiny under forced short writes -> normal with an independent screen
oracle and compare to a cold full frame.

## P2 — Idle resize now waits nearly a second

Locations: src/terminal.cpp:1167–1180, 1323–1330; src/main.cpp:288–291.

Geometry is checked only between polls. Idle poll timeout changed from 50 ms to
1,000 ms, and no SIGWINCH handler or equivalent resize wakeup exists. SIGWINCH's
normal ignored disposition does not wake this poll. A terminal resize therefore
leaves the old layout on screen for up to a second, directly contradicting the
requested responsive rendering.

Bounded actual-PTY observation: drain the initial 24x80 screen, resize to 30x90
without keyboard/worker traffic, measure until the first output: 0.844 seconds.
The child subsequently exited normally. No inference or production edits.

Remedy: make resize an explicit wakeup (signal-safe notification into the poll
loop, or the platform's suitable event mechanism). Keep an idle timer only as a
fallback. Test the actual no-input/no-worker resize path; a busy-stream resize
cannot detect this defect.

## Properties that survived this review

The frame packet's string storage is retained while pending; prepare/grid reset
are suppressed until it finishes. Ordinary grid completion commits only after
flush success. EINTR, EAGAIN and positive short writes preserve the packet offset.
The wakeup fd is set/cleared under the same mutex used by producers; worker join
precedes wakeup destruction. Input and worker processing continue while a normal
frame is pending. These are useful foundations, but do not resolve the four
specific lifecycle failures above. No broad certification or second review is
requested.
