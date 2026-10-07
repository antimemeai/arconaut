# Autodev performance capture

Immediate development unit: attach a read-only collector to an existing campaign
manager, persist bounded process-tree resource observations and storage growth,
and stop at its deadline or manager exit. No model requests, production dependency,
global permission change, or modification of the observed campaign. Twenty-minute
implementation allowance; direct workload checks and one recheck only.

The collector uses Darwin libproc declarations from the installed SDK, and reads
Apple XNU's `fill_task_rusage`, `task_power_info_locked`, and `fill_task_io_rusage`
to distinguish Mach CPU ticks from nanoseconds, byte counters from page counts,
and physical footprint from resident memory. Existing `scripts/profile` remains
the finite stack/Instruments recorder; continuous collection does not continuously
run Instruments. A fresh private directory retains metadata, newline-delimited
observations, errors, profiler output, and final status, including interruptions.

Direct checks exercise real CPU work, committed memory, filesystem writes,
child discovery/replacement, manager exit, interrupted collection, and refusal
to overwrite an existing capture. Sampling cannot observe every short-lived child;
persisted observations are not complete lifetime accounting.

Attach without restarting the active research campaign:

```sh
./scripts/profile-autodev --pid 15168 --run context/workflows-campaign
```

For new campaigns, launch through the development wrapper so collection starts
automatically, rather than remembering a later attachment:

```sh
./scripts/autodev-profiled context/my-campaign -- /absolute/path/to/run.sh
```

The campaign directory must contain `launch.json` with a future absolute Unix
`deadline`. Put this invocation in the native candidate runner's argv. It retains
each launch in a fresh private `performance/launch-*` directory, records the
launched supervisor and its descendants at one-second cadence, and requests a
five-second native stack window every five minutes when a harness is observed.
`launcher.json` retains both command and collector exit outcomes. Collection
failure is reported and retained without replacing the workload's exit status.
Explicit interruption is forwarded to the owned command; descendant settlement
remains the native runner's responsibility. A reused PID is never polled in a
shell existence loop. Both utilities run with the installed system Python3.9;
development tooling is separate from the C++/Lua production harness.

The one launch review led to opening capture files before starting the command,
guarding later telemetry failures while still waiting the command, and retaining
terminal launcher outcomes. Direct real commands exercised exit7 and SIGTERM143
with persisted observations and manager-exited capture status. An integration
check exposed Python3.9's missing `hashlib.file_digest`; bounded128KiB streamed
hashing replaces it. Source/version and earlier captures remain unchanged.

`launch.json` supplies the absolute deadline; `--seconds N` can shorten it.
The default cadence is one second, with session/audit/output/manager-log size
observations every ten seconds. `--interval` is bounded to 0.25–60 seconds.
Do not sum the session tree with the separately reported audit file: they overlap.
Directory observations stop at 10,000 entries and report truncation/errors. Logical
file sizes and allocated filesystem bytes are separate. No file content is copied.

Output is a fresh mode-0700 directory under ignored `context/profiles/` (or a new
`--out` directory). `capture.json` records the OS/kernel, Git state, campaign
launch configuration, collector/wrapper hashes, timing configuration, and terminal
status. `resources.jsonl` retains one complete observation per line, flushed every
tick and fsynced every ten seconds and at exit. New executable fingerprints are
stored once as `new_binaries`; processes refer to their `binary_id`. A path hash
is streamed in 128KiB chunks (including the system Python 3.9 interpreter), rather
than loading a whole executable into collector memory. It
identifies the bytes at observation time, not a promise that loaded mappings still
match that path after an in-place rebuild. No arguments or environment are logged.

Each process has PID plus birth time identity, parent PID and a heuristic role:
manager, harness, shell/Lua supervisor, or tool. CPU Mach ticks and converted
nanoseconds; resident, wired and physical-footprint bytes; task virtual bytes;
disk read/write bytes; pageins, faults, wakeups, thread counts, Mach IPC messages,
syscalls and context switches are persisted. `descriptor_slots` is the allocated
file-descriptor-table capacity, **not an open-FD count**. Raw child CPU/wakeup/pagein
accounting is kept separately where supplied by rusage; never add those cumulative
child CPU fields to the sampled descendants. Shared memory can also be counted
more than once across process gauges. No lifetime totals are manufactured.

`first-observed`, `not-observed`, and executable-change events expose sampled
restarts and missing observations. Individual errors remain attached to their PID.
The root's birth time prevents following a reused manager PID; zombie manager
exit is checked against rusage Mach start/exit timestamps. The root observation
must match its original birth time again inside traversal; child observations
must match the enumerating parent PID and its birth time. Parent identity is
rechecked after enumerating children before expansion. Every tick reports the
collector's observation CPU/wall cost and its own cumulative resource usage.
Per-tick cost ends before serialization/write; final cumulative collector CPU,
wall duration and resources include collection/persistence overhead. A process
born and gone between ticks remains unobserved. Detached or external services,
provider-side compute and network-byte attribution are outside this capture.

Optional finite native stack samples use the existing wrapper:

```sh
./scripts/profile-autodev --pid 15168 --run context/workflows-campaign \
  --stack-seconds 5 --stack-every 300
```

Each stack window has its own retained status/log/report. At deadline or signal,
only collector-owned profiler processes are stopped; the harness is untouched.
CPU and allocation Instruments recordings stay explicit `scripts/profile capture`
commands. Allocations require the profile binary's `get-task-allow` entitlement;
the current campaign's release snapshot does not have it. Unsupported attachment
is a recorded failure, never a claim that allocation tracing happened.

Unit evidence: a real two-child workload retained >500ms CPU per child, >28MiB
committed footprint, 128MiB disk writes, child replacement/disappearance, and exact
4KiB audit size. SIGTERM retained parseable observations and final interrupted
status without killing the observed process; a same-directory retry was rejected
without changing old data. A first check exposed zombie-root handling; one recheck
passed after the targeted fix. A live one-second native stack sample completed.
Direct workload/evidence lives under ignored `context/profiles/collector-recheck/`.
Installed SDK C structs and field offsets were compared against the actual ctypes
layout, including sizes 136/96/160 bytes for BSD/task/rusage V2. Interrupting a live
five-second sample at the collector's deadline preserved its interrupted status
and left the observed harness alive.

One adversarial code review identified traversal races and terminal-status writes
being bypassed by resource-file open/final-measurement failures. Remediation carries
the expected root identity and parent identity through traversal; direct injected
races reject a reused root, a child moved to another parent, and a reused parent
without expanding unrelated descendants. Resource-file open and fsync faults
retain failed metadata when its destination remains writable; optional final self
metrics cannot block final status. All three fault checks passed and left the
observed process alive. Evidence is in `context/profiles/collector-remediation/`.
If the filesystem is full/unwritable even terminal metadata cannot be guaranteed:
the existing recording metadata and complete flushed lines remain partial evidence,
and the collector reports the write failure on stderr. No second review added.

CPU units were independently checked against `getrusage`: a 0.5 CPU-second fixture
gave 0.480618 user seconds there versus 0.480662 converted libproc seconds. This
host's actual `mach_timebase_info` ratio is 125/3. Apple XNU primary source grounding:

- [fill_task_rusage / fill_taskprocinfo](https://github.com/apple-oss-distributions/xnu/blob/main/osfmk/kern/bsd_kern.c)
  copies the rusage CPU fields from task power accounting and the task-info fields
  from `recount_task_times` Mach-time values.
- [task_power_info_locked](https://github.com/apple-oss-distributions/xnu/blob/main/osfmk/kern/task.c)
  obtains CPU times from `rm_time_mach` / `recount_usage_system_time_mach`.
- The same `bsd_kern.c` source's `fill_task_io_rusage` reports disk byte totals;
  `fill_task_rusage` takes footprint/resident bytes from task ledger accounting.
- [libproc wrappers](https://github.com/apple-oss-distributions/xnu/blob/main/libsyscall/wrappers/libproc/libproc.c)
  establishes that `proc_listchildpids` returns PID count, not byte count.
- [filedesc](https://github.com/apple-oss-distributions/xnu/blob/main/bsd/sys/filedesc.h)
  distinguishes `fd_nfiles` allocated slots from `fd_nfiles_open`.

The installed SDK supplies exact V2/struct layouts; primary source describes their
meaning, and the independent live fixture resolves CPU units on this actual host.
No third-party implementation or runtime dependency was adopted.

Next native units: action spans linked to audit request/effect IDs, using monotonic
wall/thread CPU and byte/allocation counters; exact waited-child resource totals
for short tools; request/response/terminal byte accounting; and Linux `/proc`/perf
collection. These make serialization, context transformations, audit writes,
process execution, message transport, workflow dispatch and terminal painting
comparable to studied implementations under matching workloads. The collector
alone cannot assign a sampled CPU stack to every semantic action, count all
allocations, or establish tail latency/reliability. Those remain concrete orders,
not implied achievements of turning on a profiler.
