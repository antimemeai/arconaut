# Adversarial design/plan review: command jobs platform lane

2026-10-09. Reviewed `docs/COMMAND_JOBS.md` (137-line initial draft) and
`docs/COMMAND_JOBS_SUBPLAN.md` (27-line initial draft), current owned spawn/wait
sites, and the primary evidence in [platform.md](platform.md). This is pre-code
review within the research/design phase, not another implementation-hardening
layer. No product edits or new runtime experiments were performed for this review.
Line references identify the reviewed draft; later integration can move them.

## Judgment

The major architecture is appropriate: job custody outlives wait calls, capture
has one owner, the operator tty is retained, child exit differs from stream closure,
restart/switch are fenced, and crash does not authorize saved-PID adoption/replay.
The draft is **not ready to guide the PTY/spawn implementation unchanged**. Its
helper bootstrap and cleanup rules omit conditions that can produce immediate
startup failure, descriptor leaks, indefinite shutdown or false containment claims.
These are concrete corrections to the existing design, not proposals for a new
architecture or assurance layer.

## Required findings

### P1 — Define a PTY bootstrap that can actually create its session

**Locations:** COMMAND_JOBS 101–107; SUBPLAN 12–13.

The current `detail::Child::start` sets POSIX_SPAWN_SETPGROUP with pgroup zero,
which makes the spawned child a group leader. Reusing that launch path to start
the proposed helper makes its `setsid()` fail with EPERM: a group leader cannot
create a new session. The design names a helper but does not distinguish its
spawn attributes from ordinary pipe commands.

**Correction:** define a separate PTY bootstrap. The parent spawns the helper
without making it a group leader; the helper creates its session, establishes the
controlling slave, sets stdio/configuration and executes the requested program.
Declare the readiness/error channel, inherited fd whitelist and status publication.
Until session/group setup has been established, the parent owns a child PID but
must not assume `pid` already names its intended isolated process group. Early
stop/timeout/failure must safely handle this starting phase. Pipe commands retain
the isolated group attribute.

Reset the inherited signal mask and default/ignored job-control dispositions
before helper setup, not merely sometime before the final exec. The helper must
not inherit the harness's operator terminal as its controlling terminal.
[POSIX setsid](https://pubs.opengroup.org/onlinepubs/9799919799/functions/setsid.html),
[POSIX posix_spawn](https://pubs.opengroup.org/onlinepubs/9799919799/functions/posix_spawn.html).

**Direct oracles:** helper observes PID=SID=PGID=slave foreground PGID; target stdin
is the private tty; operator tcgetpgrp/modes do not change. Gate helper before
session creation and inject stop/deadline; root is collected and no unrelated
process receives a group signal. Missing helper, slave-open/setup failure and
failed target exec each produce one launch with an explicit failed startup
observation, without a bogus successful attach or retry.

### P1 — A jobs-only spawn mutex cannot satisfy the stated fd contract

**Locations:** COMMAND_JOBS 101–102; SUBPLAN 12.

The existing non-atomic pipe creation is in `src/native_process.hpp:118`.
Existing spawns also occur in `src/terminal.cpp:399` (editor). Notification pipe
creation at terminal.cpp:1210 and `mkstemp` followed by CLOEXEC in the editor also
have descriptor-creation windows. A mutex used only by new jobs leaves these
spawns able to inherit transient job descriptors, or new job spawns able to
inherit transient terminal/editor descriptors. That can delay output EOF or
retain child stdin even after its intended owner closes it.

**Correction:** identify a shared, process-wide spawn/descriptor-creation lock and
all its call sites. Include legacy Child creation/spawn and editor spawn. For each
non-atomic transient descriptor, cover creation until normalization/CLOEXEC/closure,
or document and test an ordering that makes it pre-concurrency. Do not hold this
lock over waits, pipe I/O, helper readiness or editor execution. Normalize all new
pipe, PTY and bootstrap endpoints above 0/1/2 before constructing dup/close actions;
otherwise closed parent stdio creates collisions and accidental endpoint closure.
Atomic CLOEXEC primitives are an alternative only when qualified on both hosts.

**Direct oracles:** controlled concurrent legacy command, new pipe/PTY job and
editor spawn see only their declared descriptors; EOF arrives when the fixture's
actual writers close. Run each with parent fd 0, 1 and 2 closed, singly and in
combinations, and assert correct target stdio, independent bootstrap reporting and
no kept-alive master/slave/error pipe. Error injection during each creation step
leaves no open endpoint and releases the spawn lock.

### P1 — Define bounded cleanup/drain rather than infer group disappearance

**Locations:** COMMAND_JOBS 16–21, 39–43, 54–58, 106–116.

WNOWAIT keeps the leader's PID occupied until reap, which protects against stale
numeric-identity reuse. It does not establish that all group members stopped or
that the group is empty. A zombie leader itself can keep a group existence probe
positive before reap. After reap, `kill(-pgid, 0)` can inspect a recycled group and
violates the design's no-group-signal-after-reap rule. Existing Child destruction
currently does such a post-reap probe; the new unit must not accidentally inherit
that pattern. Group-signal success is not all-member completion evidence.

A descendant that retains stdout can prevent stream closure after leader exit.
A stopped process will not act on ordinary termination until continued or killed.
The draft says lifecycle deadlines and stoppable collectors exist, but specifies
neither finite post-exit drain nor the cleanup escalation/close disposition.
Shutdown could consequently wait indefinitely or publish a clean result after
abandoning output.

**Correction:** state the finite shutdown/stop/drain policy, terminal-event priority
under a full queue, root-specific signaling and error dispositions. Retain the
zombie until the final allowed group action, then collect it exactly once. A child
can move out of its original group, so root termination may also require direct
PID signaling; never substitute its current PGID when that could be the harness's
own group. Do not use a post-reap group probe as verification. Describe settlement
as known root status, sealed/incomplete channel capture and completed bounded
cleanup attempts; escaped or unobserved descendants remain outside any universal
termination claim. Decide how those limits affect the existing quiescence fence.
[POSIX kill](https://pubs.opengroup.org/onlinepubs/9799919799/functions/kill.html),
[POSIX wait](https://pubs.opengroup.org/onlinepubs/9799919799/functions/wait.html).

**Direct oracles:** root exits while a gated descendant holds its writer, and
foreground accurately reports leader exit with incomplete capture until the gate
or specified drain deadline. A SIGSTOPped root and pipe-filling producer terminate
through the declared escalation within bounds even while delivery queues are full.
A root that moves group is still reaped without signaling the harness group.
Instrument the signal adapter to reject every group action after reap. Exact final
bytes or exact incomplete-drain disposition must match the fixture, not merely a
successful wait or absence of the old PID.

### P1 — Raw attach must preserve raw *operator* input while replacing rendering

**Locations:** COMMAND_JOBS 82–95.

The private PTY must receive Ctrl-C as input. If raw attachment reuses
`TerminalMode::suspend()` as the editor does, that restores the original terminal
line discipline including ISIG; Ctrl-C is then consumed by the operator tty and
sent to Blackbird's process group, rather than forwarded to the private PTY.
The existing method also changes alternate-screen/bracketed-paste state, so mode
restoration and screen restoration are not the same operation.

**Correction:** explicitly retain the operator tty's raw no-ISIG/no-echo input
configuration during relay. Suspend TUI rendering without enabling host-terminal
signal processing. On detachment restore the established TUI terminal and emulator
modes and redraw its saved composer. All ordinary TUI writes must cease during raw
relay except the deliberate detach/restoration transition. Define Ctrl-B as a
reserved local action, including how buffered input around it is handled. Job
emitted alternate-screen/paste/keyboard-protocol changes must not corrupt the
returning composer; resetting supported emulator modes is part of restoration.
[POSIX terminal interface](https://pubs.opengroup.org/onlinepubs/9799919799/basedefs/V1_chap11.html).

**Direct oracles:** outer real PTY has ISIG off during relay; Ctrl-C reaches the
inner canonical tty's actual foreground child while Blackbird remains alive and
its workflow cancellation flag stays unchanged. Raw inner mode instead receives
literal byte 03. After Ctrl-B, job exit, child display-mode changes and failed
attach/output, the composer text, intended TUI screen modes and host tcgetpgrp
match their declared states. Output from another background job does not interleave
with the attached raw terminal.

## Required clarifications before selecting tests

### P2 — State-dependent Linux PTY EIO, not a blanket success conversion

**Location:** COMMAND_JOBS 111–112.

The wording acknowledges the platform difference but omits its predicate. Declare
what distinguishes expected last-slave closure from pre-bootstrap failure or an
unrelated read error. A first EIO before custody readiness cannot be an ordinary
successful stream completion. A target may close all slave descriptors while
continuing to run; sealing the captured channel does not certify root exit. Root
failure/exit status must survive any normalized terminal EOF.

**Direct oracle:** on both hosts, child writes exact tail and closes its slave
while remaining alive behind a gate; capture seals but process state remains
running. Linux close yields qualified EIO, Mac zero read. Inject an unrelated EIO
and assert an error/incomplete capture. Failed bootstrap never presents a ready
terminal merely because its error channel closed.

### P2 — Preserve signal/stop cause and consume nonterminal child events

**Locations:** COMMAND_JOBS 41–42, 54–55, 106, 112.

Specify how stopped signal, continued state and signal termination are represented.
`exit_code=128+signal` alone collapses an actual exit 143 with SIGTERM termination.
WNOWAIT on a stop/continue event can present that same event repeatedly if never
consumed; mixing exit-reaping `waitpid` with pinned-exit observation can accidentally
reap before the planned cleanup. Per-job workers should wait only on their owned
PID, not WAIT_ANY, which could steal another job/editor/provider child's status.

**Correction/oracle:** preserve native termination kind and signal alongside any
compatibility exit_code. Consume only nonterminal wait events while independently
observing terminal exit without reap, under the same worker. Gate repeated
STOP/CONT, a normal exit 143 and actual SIGTERM; each state transition/cause must
be exact and there must be one final reap. Concurrent other child statuses must
remain available to their respective owners.

### P2 — Name bounds and input delivery stages in the specification

**Locations:** COMMAND_JOBS 31, 60–65, 78–80, 113–116.

Finite queues alone are not a full capacity contract. Name chosen per-job/aggregate
job count, input bytes, live output bytes, terminal-event reserve and cleanup/drain
bounds in the written subplan before tests are authored. Specify whether input
results mean queued, partially kernel-written or completely kernel-written;
application consumption cannot be inferred. Duplicate input must resume/report
its original delivery position, never resend its written prefix. Signal delivery
acknowledges an attempted OS action, not the resulting stop/exit.

**Direct oracles:** saturate every chosen bound and assert pre-effect refusal or
its exact documented disposition; deduplicate at each partial-write boundary;
queue-full cancellation must wake cleanup without owner consumption. A duplicate
signal uses the original result and never causes a second syscall.

## Ninety-minute sequencing assessment

SUBPLAN 24–26 explicitly prevents budget reset and permits an unsafe partial to
remain inactive; retain those rules. The proposed sequence correctly puts red
oracles before code and a concrete review before the recheck layer. Its claimed
scope is nevertheless much larger than a credible complete 90-minute unit:

- Platform registry/collector/helper and fault-bearing spawn/cleanup mechanics.
- Existing audit admission/retention/reservation changes and owner pumping through
  provider/tool/Lua/idle execution without reentrancy.
- Shared native/model/Lua/operator controls with ordered partial-input delivery.
- Raw terminal handoff, display restoration, asynchronous UI attribution, restart
  and shutdown fences.
- Independent transition cases, real process/PTY fixtures and four local build
  modes plus Linux actual behavior, followed by review and the required clean hook.

This is a planning risk inferred from the number of coupled contracts, not an
assertion that a timed implementation cannot succeed. The plan has no timing
measurements showing that its complete required checks fit inside the allowance.
With at most 25 minutes for hardening, unfinished helper/session/signal mechanics
cannot be deferred into assurance or excused by a passed happy path.

**Recommended correction:** keep the authorized full goal and the declared
allowance unchanged, but give the owner explicit acceptance milestones inside it:
(1) safe same-job process/PTY custody and native tests, (2) integrated audited
lifetime/control, (3) operator attach and complete required validation. These are
progress checkpoints, not separately restarted allowances or independent
implementation owners. If the bound expires before all three are satisfied,
retain the candidate inactive, record the concrete remainder and continue only
under the existing applicable allowance policy. Do not activate a partially
verified UI or silently narrow the operator's requested completed behavior.

The required platform corrections belong in the reviewed design/subplan now;
none requires adopting a library, moving custody to a daemon, or reopening the
steering/trajectory work.
