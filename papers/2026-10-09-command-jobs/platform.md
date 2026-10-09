# Running command jobs: platform contracts and source mechanisms

2026-10-09. Platform research lane for the operator's explicit priority: background
an already running command and foreground that **same** command. Steering and
trajectory work are out of scope. Research informs the reviewed design; it neither
adopts a dependency nor qualifies a Blackbird implementation. No imported program,
build, test or installer was executed. The five probes below are owned experimental
instruments, run on both supported host families.

## Principal result

Releasing a caller's wait does not require stopping, restarting or moving the
command between terminal process groups. A job must own its live child, stream
endpoints, capture and lifecycle observations independently of each wait call.
Foregrounding can then resume observation of the same job. Terminal attachment,
process execution state and parent-owned exit collection are separate contracts.

This separation is an inference from the contracts and source mechanisms below.
It directly excludes an implementation that abandons a local `Child` object when
returning a background handle: Blackbird's current `Child` destructor closes its
pipes and kills its process group. Lifetime must change before an API can honestly
promise background execution. Merely saving a PID cannot provide that lifetime.

## Evidence and reading limits

| Evidence | Acquired identity and actual reading | Limits |
| --- | --- | --- |
| POSIX.1-2024 Issue 8 | Terminal chapter §§11.1–11.2 relevant access, input, signal and close rules; job-control rationale; `tcsetpgrp`, `setpgid`, `setsid`, `fork`, `posix_spawn`, `read`, `write`, `poll`, `wait`, `kill`, `exec`, `fg`, `bg` contracts | Normative portable semantics, not a claim that every Issue 8 API/flag exists on the installed hosts; entire rationale/chapter not read linearly |
| GNU C Library manual 2.44 | Implementing a shell overview and complete launch, foreground/background, stopped/terminated example sections | Educational reference implementation; single-threaded shell assumptions, illustrative error handling, and signal-policy caveats |
| Apple Xcode macOS SDK 26.2 | Retained `forkpty`/`openpty`/`login_tty`, `pty`, relevant `termios`, `setsid`, `setpgid`, `read`, `wait`, `sigaction`, `tcsetpgrp`, `kqueue` manuals | Target API documentation; some manuals retain old BSD device naming; SDK version differs from runtime macOS 26.6 |
| Linux man-pages 6.19 | `pipe`, `pty`, `openpty`, `setsid`, `wait`, `pidfd_open`, child-subreaper, relevant tty ioctl contracts | Kernel/library version prerequisites apply; runtime probe host uses Linux 6.8/glibc 2.39 |
| Linux kernel v6.19 | `drivers/tty/pty.c`, close/open/hangup and flow-control paths | Inspected tag differs from probe host kernel; no kernel source executed or adopted |
| tmux upstream | `82abcd175cca43c671af3690cd0af74c4c75621c`; spawn setup, server child collection, job close/death state machine, pane capture callbacks, client buffer offsets/read controls, detach-related defaults, `fdforkpty` adapter | Portable server-custody mechanisms; no whole-source or runtime qualification; screen history differs from original-byte retention |
| Blackbird current source | `src/native_process.hpp` spawn, descriptor normalization and destructor; prior custody study | Baseline consequences, not an independent oracle for new implementation |

Exact URLs, source paths, acquired byte hashes and archive identities are in
[platform-documents.json](platform-documents.json) and
[platform-references.json](platform-references.json). Intermediate transformed
text, acquisition scripts and probe captures live in ignored
`context/command-jobs/`. Original external HTML/manual bytes are preserved in the
intact platform-documents ZIP; the complete tmux source ZIP is intact too. The
[restoration instructions](PLATFORM_RESTORE.md) state mutable-document limits.

The initial GNU unversioned paths returned the manual index rather than the
requested chapter. Those files were replaced with the actual versioned primary
chapters from the glibc project's sourceware host **before** interpreting them.
HTTP success alone was not accepted as acquisition of the requested material.

## Three independent meanings of foreground

The POSIX rationale describes a shell foreground job through both shell waiting
and terminal-driver foreground membership. These happen together in an interactive
shell because the shell deliberately coordinates them. The native harness is not
required to tie its own waiting to the operator terminal's foreground group.
[POSIX job-control rationale](https://pubs.opengroup.org/onlinepubs/9799919799/xrat/V4_xbd_chap01.html).

The reviewed Blackbird contract should distinguish:

1. **Wait attachment:** a model/tool/operator is currently waiting for an existing
   job's observations. Releasing this wait changes who is blocked.
2. **Execution state:** the owned process is running, observed stopped, observed
   exited, or no longer authoritatively observable. Background does not imply STOP.
3. **Terminal foreground:** a process group owns input and terminal-generated
   signals for a particular controlling terminal. This concerns one terminal and
   one session, not whether the model currently waits.

A private PTY permits a command to retain terminal-foreground membership inside
its own session while Blackbird continues owning the operator terminal. Apple
`login_tty` explicitly creates a session, acquires its terminal and binds stdio;
`forkpty` combines allocation, fork and this setup. These mechanisms explain the
separation; they do not select PTYs for every command.
[Apple openpty family](https://developer.apple.com/library/archive/documentation/System/Conceptual/ManPages_iPhoneOS/man3/openpty.3.html),
[POSIX setsid](https://pubs.opengroup.org/onlinepubs/9799919799/functions/setsid.html).

`fg`/`bg` are shell-environment job operations. Invoking a fresh `sh -c 'fg …'`
cannot foreground a native job owned by Blackbird or another shell. The shell's
job ID and Blackbird's durable job identity need separate namespaces.
[POSIX fg](https://pubs.opengroup.org/onlinepubs/9799919799/utilities/fg.html),
[POSIX bg](https://pubs.opengroup.org/onlinepubs/9799919799/utilities/bg.html).

## Terminal and signal consequences

A background group reading its controlling terminal normally receives SIGTTIN.
Background writes depend on TOSTOP; operations that change terminal parameters
have stricter SIGTTOU treatment. Terminal signal characters, when enabled, target
the terminal's foreground group. These are reasons to retain Blackbird's own
terminal custody and make any command-terminal bridge explicit. Sharing the
operator tty also obliges restoration of the operator's terminal modes.
[POSIX terminal interface](https://pubs.opengroup.org/onlinepubs/9799919799/basedefs/V1_chap11.html).

`tcsetpgrp` requires the caller's controlling terminal and a target group in its
session. A background caller can itself provoke SIGTTOU unless appropriately
blocked/ignored. It is not a universal operation for attaching arbitrary PIDs.
[POSIX tcsetpgrp](https://pubs.opengroup.org/onlinepubs/9799919799/functions/tcsetpgrp.html).

The GNU example restores saved job modes, sets the foreground group and sends
SIGCONT when continuing a stopped job; after the wait, it restores shell custody
and modes. It gives no reason to send SIGCONT merely because observation resumes
for an already running job. Its launch example calls `setpgid` in both parent and
child to close scheduling races and restores default job-control signal actions.
[GNU foreground/background example](https://sourceware.org/glibc/manual/2.44/html_node/Foreground-and-Background.html),
[GNU launch example](https://sourceware.org/glibc/manual/2.44/html_node/Launching-Jobs.html).

An interactive shell inside a private PTY can create additional process groups
and transfer that PTY's foreground to its current command. Therefore a terminal
interrupt, a signal to the root PID, and a signal to the root process group can
have different effects. The UI/API must name the intended action. Sending a
control character to a PTY respects its line discipline; with signal processing
disabled, that character is ordinary input. Signaling a selected group has a
different, explicit contract. Successful `kill` is dispatch evidence, not observed
termination; groups are not arbitrary descendant trees.
[POSIX kill](https://pubs.opengroup.org/onlinepubs/9799919799/functions/kill.html),
[POSIX setpgid](https://pubs.opengroup.org/onlinepubs/9799919799/functions/setpgid.html).

## Child status, EOF and completion must not collapse

`waitpid` gives a parent an owned child's status. WNOHANG means a poll can return
without a status; WUNTRACED permits observed stopped state. Stops are not exits,
and errors are not successful outcomes. Multiple competing waiters introduce
collection ambiguity; an authoritative lifecycle collector should publish to
observers. Explicit SIGCHLD ignore or SA_NOCLDWAIT can discard waitable status.
GNU's historical example discusses ignoring SIGCHLD; that policy must not be
copied where current POSIX child-status preservation is required.
[POSIX wait](https://pubs.opengroup.org/onlinepubs/9799919799/functions/wait.html),
[GNU status example](https://sourceware.org/glibc/manual/2.44/html_node/Stopped-and-Terminated-Jobs.html).

Pipe EOF arrives only when every writer reference is closed. A root process may
exit while a grandchild retains a writer. A child may close output and continue
running. A stray inherited descriptor can delay EOF indefinitely. Closing all
readers can instead cause the writer's SIGPIPE/EPIPE. Consequently capture needs
independent status for each output channel, and exit status cannot certify either
complete output or disappearance of every descendant.
[Linux pipe contract](https://man7.org/linux/man-pages/man7/pipe.7.html).

POLLHUP and readable bytes can coexist. Drain available bytes before classifying
stream completion; HUP alone must not discard the final buffered tail. Readiness
can yield zero bytes or an error and should not be mistaken for payload.
[POSIX poll](https://pubs.opengroup.org/onlinepubs/9799919799/functions/poll.html),
[POSIX read](https://pubs.opengroup.org/onlinepubs/9799919799/functions/read.html).

The tmux `job.c` machine separately tracks JOB_DEAD and JOB_CLOSED. Either event
can happen first; completion and freeing wait until the other occurs. This is a
direct reference for exit/stream orthogonality. Its child-stop path auto-continues
some stopped jobs; that is tmux policy, not a requirement for Blackbird.
[tmux job machine](https://github.com/tmux/tmux/blob/82abcd175cca43c671af3690cd0af74c4c75621c/job.c#L338).

## Capture, backpressure and input

A pipe is a finite byte channel. Backgrounding must leave an active drain/capture
owner, otherwise an output-heavy command can block on a full pipe even though its
job status says running. More than one output pipe and writable stdin create
possible duplex deadlocks if handled serially. Finite retention requires a stated
policy for capacity/storage failure: bounded blocking, explicit truncation/loss,
or termination with an observed disposition. Returning a handle cannot evade the
physical bound.

Writes can be short, interrupted, or refused. Stdin results must distinguish bytes
accepted by the kernel from bytes proven consumed by the application. Closing a
pipe's write end is meaningful EOF; a PTY is bidirectional and does not offer the
same stdin half-close contract. Canonical VEOF is a terminal input operation,
not an irrevocable close. Input queues must remain bounded after wait detachment.
[POSIX write](https://pubs.opengroup.org/onlinepubs/9799919799/functions/write.html),
[Linux PTY contract](https://man7.org/linux/man-pages/man7/pty.7.html).

PTYs change the observable stream: terminal echo, canonical buffering, output
processing such as LF-to-CRLF, and normally merged stdout/stderr. Preserve the
actual captured terminal bytes and do not call them untouched separate pipe
streams. Window-size updates may cause terminal signals. These differences belong
in explicit transport metadata and exact-byte test expectations.
[Apple termios](https://developer.apple.com/library/archive/documentation/System/Conceptual/ManPages_iPhoneOS/man4/termios.4.html),
[Linux tty ioctls](https://man7.org/linux/man-pages/man2/ioctl_tty.2.html).

Tmux pane input is read and parsed independently of any one attached client.
Its client path maintains offsets into buffered data and can disable PTY reads
when attached control consumers cannot accept more. With no attached clients it
allows reading to continue. This offers concrete mechanisms for background drain
and consumer separation; it is not an unlimited-memory design or an original-byte
audit. Server exit-on-unattached defaults off, so client detach alone ordinarily
does not relinquish pane custody.
[tmux client flow control](https://github.com/tmux/tmux/blob/82abcd175cca43c671af3690cd0af74c4c75621c/server-client.c#L1930),
[tmux pane callbacks](https://github.com/tmux/tmux/blob/82abcd175cca43c671af3690cd0af74c4c75621c/window.c#L1649),
[tmux defaults](https://github.com/tmux/tmux/blob/82abcd175cca43c671af3690cd0af74c4c75621c/options-table.c#L402).

## Spawn safety and descriptor inheritance

A threaded parent cannot freely call allocating C++/Lua code in a forked child.
POSIX limits that child to async-signal-safe operations until exec. Introducing
`forkpty` and then using ordinary container/string/logging machinery would violate
this contract. A reviewed approach needs an appropriate executable setup helper,
a strictly qualified child path, or supported spawn machinery. Tmux's fork-based
server does not supply evidence for doing the same inside Blackbird's threads.
[POSIX fork](https://pubs.opengroup.org/onlinepubs/9799919799/functions/fork.html),
[tmux spawn](https://github.com/tmux/tmux/blob/82abcd175cca43c671af3690cd0af74c4c75621c/spawn.c#L450).

`posix_spawn` specifies ordered file actions and explicit signal-mask/default
attributes. New job machinery should state inherited descriptors, mask and ignored
signal disposition rather than assume defaults. Successful spawning and successful
execution are different observations; child setup/exec failure can be represented
by status 127. If precise startup failure distinctions are promised, their reporting
mechanism must survive child setup failure rather than infer it from command output.
[POSIX posix_spawn](https://pubs.opengroup.org/onlinepubs/9799919799/functions/posix_spawn.html),
[POSIX exec](https://pubs.opengroup.org/onlinepubs/9799919799/functions/exec.html).

Current Blackbird uses `pipe()` then `F_DUPFD_CLOEXEC` to normalize descriptors
above stdio. The original descriptors are temporarily not close-on-exec. Concurrent
spawn can inherit these originals before normalization completes. This is a
source-derived risk, not a reproduced defect in the current scheduling. The job
design must either serialize descriptor creation with spawn or use qualified atomic
close-on-exec primitives and controlled child inheritance. Issue 8 declarations
alone do not establish macOS/Linux runtime availability.

## Harness death, restart and custody

Moving a descriptor to another process does not transfer parentage. Apple kqueue's
NOTE_EXITSTATUS is explicitly child-only. Linux pidfds can identify/observe a
process, but parent-only wait status is still conditional; opening a pidfd for a
nonchild does not grant wait authority. Thus saving PIDs and reopening files cannot
recover a vanished parent collector's promised status history.
[Apple kqueue](https://developer.apple.com/library/archive/documentation/System/Conceptual/ManPages_iPhoneOS/man2/kqueue.2.html),
[Linux pidfd_open](https://man7.org/linux/man-pages/man2/pidfd_open.2.html).

Linux's subreaper changes orphan reparenting for descendants of a still-living
ancestor. It is Linux-specific and does not recover status already lost with the
last observer. It cannot stand in for a portable custody design.
[Linux child subreaper](https://man7.org/linux/man-pages/man2/PR_SET_CHILD_SUBREAPER.2const.html).

If commands are promised to survive replacement of Blackbird, a surviving owner
must retain parentage, open capture endpoints and any consumed-but-uncommitted
bytes, or the replacement must preserve them through a specifically qualified
in-place mechanism. The prior [refit custody study](../2026-10-01-refit-custody-grounding.md)
establishes this boundary. A plain restart can instead explicitly terminate owned
jobs or refuse while they run, but must not claim continuity. Whole-host failure
cannot preserve the live process. An unresolved admission must never trigger an
automatic relaunch of arbitrary command effects.

PTY endpoint lifetime matters too: Linux's master close path hangs up the slave.
A UI detachment must release observation/input attachment without closing the last
custodial master descriptor. Master closure during owner death is therefore a
semantic failure, even if a worker happened to remain alive.
[Linux v6.19 PTY close](https://github.com/torvalds/linux/blob/v6.19/drivers/tty/pty.c#L47).

## Direct platform experiments

Owned source: [platform-probes.py](platform-probes.py). No Blackbird component is
called. Handshake pipes/known byte markers gate the cases; bounded polling protects
against hangs. Commands were identical apart from local/remote invocation:

```sh
python3 papers/2026-10-09-command-jobs/platform-probes.py
ssh -o BatchMode=yes -o ConnectTimeout=8 neuroses 'timeout --kill-after=2 25 python3 -' < papers/2026-10-09-command-jobs/platform-probes.py
```

| Direct oracle | macOS 26.6 arm64 | Neuroses Linux 6.8.0-146-generic x86_64, glibc 2.39 |
| --- | --- | --- |
| Release/resume caller wait: one child PID produces READY, STEP and FINAL after independent input gates; exit 0 | Passed | Passed |
| Root exits 23 while descendant holds stdout open: read is EAGAIN before release, then exact CHILD_TAIL and EOF | Passed | Passed |
| Child closes stdout before exit: zero-byte read while `poll()` still reports no root exit; gate releases exit 31 | Passed | Passed |
| SIGSTOP observed through WUNTRACED/WIFSTOPPED; SIGCONT yields same child's DONE and exit 0 | Passed | Passed |
| Private PTY child has PID=PGID=SID=terminal foreground PGID, canonical input/echo and CRLF; exit 17; final master read | Passed: zero-byte EOF | Passed: EIO |

Captured logs: `context/command-jobs/platform-probes-macos.log` and
`context/command-jobs/platform-probes-linux.log`. These are bounded demonstrations
of platform primitives with specification oracles. They establish neither race
freedom nor Blackbird job behavior, resource-failure behavior, arbitrary descendant
containment or restart continuity.

The observed PTY close difference requires a qualified transport adapter. A Linux
PTY-master EIO in the established terminal-close context may normalize to stream
closure; arbitrary EIO on another descriptor or in an unrelated state must remain
an error. A zero-byte read on the Mac is not root-exit evidence. Exit and stream
closure remain separate on both platforms.

## Static and dynamic obligations for the owning design

These attack different fault classes. They are direct oracles, not mechanisms to
certify other tests. Choose the explicit product scope first; then require every
promised property to have its appropriate check.

| Fault class | Static obligation | Direct dynamic oracle |
| --- | --- | --- |
| Job lost when wait returns | Job owner outlives all wait handles; destructors cannot cancel on detach | One effect counter/PID/known stream across repeated foreground/background cycles; no second launch |
| Wait cancellation kills job | Separate wait cancellation from command-stop capability | Cancel one waiter while child advances; later foreground observes exact remaining bytes and status |
| Completion race | Root state and each channel state are independent; publish final status once | Gate both exit-before-EOF and EOF-before-exit; tail bytes, exit code and incomplete state exactly match fixture |
| Missed signal/zombie | One reaper owner; SIGCHLD wakeup not event count; no status-discard policy | Several controlled children exit before/after notification; exact per-child exit/signal statuses, every child collected |
| Lost buffered tail | HUP handling drains readable bytes; errors distinguishable from closure | Child writes known final block then exits; concatenate captured offsets byte-for-byte |
| Background pipe blockage | Drain/capture independent of foreground wait; finite queue/storage policy | Output exceeds actual pipe capacity while no waiter attaches; exact bytes or exact declared loss/disposition; RSS bound |
| Duplex deadlock | Nonblocking or concurrent progress on stdin/stdout/stderr | Fixture interleaves output on both pipes and gated input larger than capacity; checks all bytes and accepted input count |
| Descriptor leak | Atomic/serialized inheritance; normalized stdio; close all unrelated ends | Parallel launches with closed 0/1/2 and descriptor inventories; no unexpected inherited endpoint, timely expected EOF |
| Signal inheritance | Spawn attributes and child setup have explicit defaults/masks | Parent blocks/ignores chosen signals; child reports intended effective mask/disposition and actual response |
| Wrong terminal target | Distinguish root group, PTY foreground group and operator tty | Nested job-control shell; report groups independently; interrupt changes intended child only; composer keeps tty/modes |
| STOP mistaken for background | Run state independent of wait state; do not CONT on mere observation | Observed stopped child stays stopped on inspect/attach unless explicit continuation is requested |
| Platform PTY termination | Target-specific, state-limited EOF/EIO handling | Final bytes plus close on both hosts; unrelated read EIO remains failure; child exit retained separately |
| Raw/canonical mismatch | Transport metadata and terminal config explicit | Known echo/LF/NUL/control input under canonical and raw modes; exact retained actual bytes, not normalized originals |
| Unsafe fork path | Post-fork operation list entirely qualified, or helper avoids threaded path | Repeated spawn while other threads allocate/hold locks; watchdog and startup/exec status oracle; no ignored hung launches |
| PID/PGID reuse | Stable owned identity; no delayed signal through a stale bare number | Deterministic identity-reuse model rejects old handle signal; actual fast-exit races do not signal unrelated fixture |
| Owner death/restart | Named custodian and restart policy; no saved-PID fictional restoration | Kill caller/custodian at selected custody/capture boundaries; surviving same worker/bytes or explicit uncertainty, never replay |
| Timeout semantic drift | Absolute command deadline separate from each wait deadline | Repeated waits cannot reset command deadline; wait timeout leaves command running when contract says it should |
| Capture/storage failure | Disposition distinguishes retained, received-unretained and unknown | Short writes, ENOSPC and failed append while producer active; exact last retained offset and specified action |

No arbitrary process-tree kill/pause guarantee is established for macOS. Any such
claim needs its own chosen containment mechanism and direct evidence, or an honest
scope to known child/groups. The platform primitives support the requested same-job
background/foreground capability; their composition remains a reviewed-design and
implementation task.

## Implementation-time empirical appendix: Darwin waitid surprises

The first native STOP/CONT oracle falsified an assumption that looked reasonable
from the portable contract: `waitid(P_PID, child, ..., WEXITED|WNOHANG|WNOWAIT)`
could be classified as exit merely because `si_pid` matched. On macOS26.6 arm64,
it returned **CLD_STOPPED**, leaving the child alive and stopped. Marking that
observation as leader exit made foreground inspection report draining and the
stopped-state handshake fail. This is direct OS evidence, not a guessed explanation
for an unrelated timeout.

An initial ctypes experiment isolated the behavior; its continued-event fields
were anomalous. The follow-up used actual C++ `siginfo_t`, Apple SDK26.2 headers
and an explicit SDK sysroot, avoiding inference from a foreign structure layout.
Owned [probe source](darwin-waitid-probe.cpp.txt) and its
[unaltered observation](darwin-waitid-observation.txt) are retained here. The source
is text to keep this experiment outside the product compilation database. The
child signals readiness through a pipe; a SIGCONT handler proves resumed execution;
a separate input gate permits actual exit. All displayed fields are SDK members.

| Probe call on known child71525 | rc / errno | Actual SDK fields |
| --- | --- | --- |
| WEXITED\|WNOHANG\|WNOWAIT after SIGSTOP | 0 / 0 | si_code5=CLD_STOPPED, si_pid71525, si_status17=SIGSTOP |
| Repeat same WNOWAIT call | 0 / 0 | Same stopped event, demonstrating it was not consumed |
| WSTOPPED\|WCONTINUED\|WNOHANG, without WEXITED | 0 / 0 | Same stopped event, consumed by this call |
| Repeat nonterminal selector | 0 / 0 | si_code0, si_pid0: no remaining event |
| WEXITED\|WNOHANG\|WNOWAIT after confirmed continuation | 0 / 0 | si_code0, si_pid0 |
| WSTOPPED\|WCONTINUED\|WNOHANG after confirmed continuation | 0 / 0 | si_code6=CLD_CONTINUED, **si_pid40542**, si_status17 |
| Real exit with WEXITED\|WNOHANG\|WNOWAIT | 0 / 0 | si_code1=CLD_EXITED, si_pid71525, si_status0; subsequent waitpid collects exit0 |

The continued-event PID mismatch reproduced with actual SDK types. It must not be
attributed to ctypes ABI or to a kernel cause that has not been studied. This is
one observed macOS/runtime profile, not a universal Darwin theorem. The portable
contract remains useful, but the implementation must classify actual events and
qualify these target behaviors directly.

Consequences for this unit: only CLD_EXITED, CLD_KILLED and CLD_DUMPED are terminal
observations; preserve the exact matching leader PID for that classification.
Consume STOP/CONT separately. For a **specific P_PID selector** on the owned child,
qualify treating nonzero event presence plus CLD_STOPPED/CLD_CONTINUED as that
selected child's nonterminal state; this observation does not authorize accepting
an unrelated PID from P_ALL or using the returned continued-event PID as a signal
target. Continue using the stored, live owned identity for control. Neither stop
nor continuation permits reaping before the final group action.

Reproduce on the stated installed SDK using the qualified local compiler, with a
fresh experiment output path (commands do not modify product source):

```sh
/opt/homebrew/Cellar/llvm/23.1.2/bin/clang++ -std=c++20 -x c++ -isysroot /Applications/Xcode.app/Contents/Developer/Platforms/MacOSX.platform/Developer/SDKs/MacOSX.sdk papers/2026-10-09-command-jobs/darwin-waitid-probe.cpp.txt -o context/command-jobs/darwin-waitid-native-probe
context/command-jobs/darwin-waitid-native-probe
```

The root owner ran this follow-up; the platform lane did not overlap product
builds. The previously described five cross-platform probes remain distinct:
this supplementary experiment answers the newly exposed waitid classification
fault and does not establish any broader implementation qualification.
