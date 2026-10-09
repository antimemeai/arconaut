# PTY resize/input and cancelled shutdown-control remediation

2026-10-09. Scoped reopening began12:16:37UTC; this report concerns the existing
actual PTY oracle unit. No new review or assurance panel, dependency, production
terminal emulator, or product input/renderer change was added. Journal,
qualification and issue updates remain root-owned.

## Resize/input cause and correction

The retained `context/command-jobs/tsan-bounded-tests.log` fails waiting for the
interactive shell fixture's `pty.size` after resize and `size\r` input. A prior
actual failure retained in `context/command-jobs/pty-green-2/` provides the missing
discriminator: its audit contains the admitted input `size\r` at byte35880 and the
actual six-byte private-terminal echo original `size\r\n` at byte36927, while the
same launched child's independent `pty.effects` says `OTHER:siz`. The full command
reached the private terminal; its final character was lost by the shell's
signal-sensitive read/trap path, rather than command projection/collector transfer.

The old fixture ran `stty size` inside a WINCH trap during `/bin/sh`'s builtin
per-character `read`. Publishing a dimension file inside that trap does not mean
the interrupted reader has resumed. Repeated/adjacent attachment and outer resize
notifications can therefore overlap input. Two standalone shell signal trials
did not reproduce the race; they are not asserted as evidence that it is absent.
The retained actual echo/effect disagreement is the direct evidence.

The existing interactive child was replaced by an owned development Python
fixture in `scripts/check-command-jobs-pty`. It preserves canonical input, ICRNL,
ISIG and a real terminal-generated Ctrl-C, retries interrupted reads, records every
post-line-discipline byte before command parsing in `pty.input`, and records
read/signal/winsize/identity observations in `pty.trace`. Dimension publication is
atomic. The driver still requires actual requested28x100 dimensions, the same
job/PID, independent command effects and exact delivery order; it additionally
compares the entire received byte stream, including literal paste Ctrl-B,
split UTF-8 and standalone ESC. Outer keyboard writes now handle partial writes
instead of assuming one write transfers all bytes. No wait was lengthened or
assertion removed for this correction.

## Shutdown-control cause and correction

The first reopened actual TSAN run, retained as
`context/command-jobs/tsan-resize-before/`, passed resize/input and reached a
different existing assertion: incomplete outer terminal control. Its raw stream
ends a renderer CSI partway through `ESC[1;38`, then contains CAN(0x18), a complete
graphics erase and the complete reset/bracketed-paste-off/alternate-screen-off/
cursor-visible suffix. `TerminalMode` deliberately emits CAN to cancel an
abandoned render control before restoring the terminal. The actual byte stream
was complete for that cancellation/recovery protocol; the Screen oracle's CSI
regular expression did not recognize CAN and retained the valid suffix as
pending. OS termios restoration had already passed in that same failing run.

The existing oracle `scripts/terminal-screen-oracle.py` now accepts CAN(0x18) and
SUB(0x1a) as cancellation of an incomplete supported CSI or APC string, resumes
normal parsing, and executes the recovery suffix. It does not erase arbitrary
pending input: unsupported malformed CSI and unclosed uncancelled CSI/APC remain
pending. Direct fixed-cell/cursor/style cases exercise both controls across every
feed split in the existing terminal-render oracle. The original failing raw
stream also parses completely with the corrected parser.

This behavior is grounded in already quarantined libvterm revision
`934bc2fbf21800ac3458a499df8820ca5fb45fd3`: `src/parser.c:152` handles both CAN and
SUB by clearing escape state and entering normal state; `t/02parser.test:57`,
`:124`, `:165` and `:197` test cancellation of escape, CSI, OSC and DCS. Exact
source/archive provenance is in `papers/2026-10-07-render-reference-acquisition.json`.
The primary [XTerm control-sequence reference](https://invisible-island.net/xterm/ctlseqs/ctlseqs.pdf)
also explains modal control parsing and return to initial state on unexpected
control input. Quarantined source was read, never run or adopted.

## Actual results

- Original resize red: `context/command-jobs/tsan-bounded-tests.log`, with concrete
  complete-input/echo versus truncated-shell-effect discriminator in
  `context/command-jobs/pty-green-2/`.
- Reopened actual shutdown red: `context/command-jobs/tsan-resize-before.log` and
  its raw Screen capture; no product bytes missing in the cancelled CSI protocol.
- Corrected full actual TSAN PTY case: exit0,
  `context/command-jobs/tsan-resize-reader-green.log`, with `pty.input`, `pty.trace`,
  effects, PID/launch counts, actual sizes, raw UI, Screen and native audit retained.
  Resize/input/interrupt/paste/UTF-8/ESC, full-queue background/stop, task separation,
  cleanup, termios restoration and held-open plain stdin all pass.
- Existing terminal-render oracle plus direct cancellation and malformed cases:
  exit0, `context/command-jobs/can-sub-oracle-green.log`.

Root owns the single final affected-profile and full staged-tree gate; these
focused results do not replace it or qualify the candidate themselves.
