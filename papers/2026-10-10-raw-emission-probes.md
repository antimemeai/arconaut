# Raw handoff and retained-byte primitive experiments

2026-10-10. Owned, isolated research code; no Blackbird source or acquired source
was built/executed. At most one producer and one reader ran at a time. No product
suite, eval, provider call, daemon activation, mutation or sustained CPU campaign.
The experiments answer narrow transport questions, not implementation approval.

## Producer work

Three modes copy the same prepared raw body to a separate process: a pipe with
one write/read per record; a shared buffer with acquire reclaim load, copy and
release publication; and that shared path plus an unconditional armed RMW.
The latter measures the selected protocol's extra exchange on an awake reader,
not the doorbell syscall/cold wake. Ring capacity exceeds the entire run, so this
is deliberately **not** full-buffer or durable-recorder throughput. Pages are
faulted before timing; the reader compares every body byte to the independent
prepared fixture. Bodies are constant in this timing test; it is not an order/
unique-identity oracle. The separate custody test uses a known128-byte pattern.

Time covers the producer loop only. Sizes/counts:64B/100000,256B/32768,
4096B/4096, three trials each. The child runs concurrently and can cause scheduler/
cache interference. No affinity, tail-percentile claims, runtime call intercepts,
clock acquisition, static description emission, audit framing or disk sync in
this timing. Report all trials, not the best one.

| Host/compiler,64-byte body | Load/store shared baseline | Shared plus armed RMW | Pipe |
| --- | --- | --- | --- |
| macOS ARM, Apple Clang17 | 8.97–13.92 ns/event | 13.13–21.77 ns/event | 655.10–722.43 ns/event |
| macOS ARM, owned LLVM23.1.2 profile | 9.34–21.65 ns/event | 10.77–25.26 ns/event | 641.45–725.36 ns/event |
| Neuroses Linux x86, Clang18.1.3 | 25.17–31.47 ns/event | 51.35–66.87 ns/event | 672.97–709.79 ns/event |

The ordinary-copy/RMW result supports investigating retained shared memory;
it does not claim full pipeline latency or general machine-independent ratios.
Larger-body trials are in the raw CSVs; the macOS256B trials varied materially.
They are not used to make precision or tail claims.

Inspected optimized assembly: macOS acquire/release compiled to native ordered
loads/stores; armed exchange to a native atomic operation. Linux used direct
loads/stores plus locked exchange. No libatomic/local-lock helper appeared in the
owned publication path. The generic modulo compiled to division; copy calls and
register spill work are real. This probe is not magically instruction-free.
Actual source includes no allocation/clock/syscall inside `publish_raw`; the
outer timing/fixture harness intentionally allocates and reads its clock.

The first direct LLVM23 compile used its stale default SDK path and failed.
Rebuilding with the actual `xcrun --show-sdk-path`, as the project's tooling
requires, succeeded with warnings treated as errors. One scoped LLVM23
ASan/UBSan run of1000 armed publications completed with exact byte checks and no
diagnostic. That covers this research copy path, not multiprocess memory-order
qualification. TSan is not asserted as a process-shared oracle.

## Named-memory retention and recorder death

An independent prepare process creates named shared memory, then exits and
unmaps. A second process publishes a known128-byte binary pattern and exits.
The recorder reopens it after all previous mappings are gone; a controller kills
the recorder with SIGKILL at each requested stage. Inspection/recovery are new
processes reopening by name, at different observed mapping addresses. The object
is unlinked only after the case ends.

| Recorder killed at | Expected D before restart | Direct result on both hosts |
| --- | --- | --- |
| Read into private buffer, before write | 0 | Original exact in named memory; recovery writes/syncs then D=128 |
| Half raw write | 0 |64-byte tail unchanged; recovery refuses acknowledgement; D stays0 |
| Complete write, before sync | 0 | Original exact; recovery validates visible bytes and resyncs before D=128 |
| Sync succeeded, before acknowledgement | 0 | Existing file reused, resynced; exact128 bytes, no duplicate append; D=128 |
| After acknowledgement | 128 | Exact disk/body prefix retained; recovery leaves D=128 |

Mac recorder uses successful `F_FULLFSYNC`; Linux uses successful `fsync`.
File contents are independently compared to the known byte pattern, not a hash
returned by the recorder. These tests demonstrate named-object retention and the
ack ordering in this tiny model. They do not exercise production audit frames,
directory durability, storage corruption, host power loss, broken doorbells,
multiple lanes, reverse waits, wrap/full queues or actual owner cancellation.
Those remain direct implementation tests in the
[plan](../docs/INSTRUMENTATION_PLAN.md).

## Reproduction and raw results

Text snapshots preserve the owned research sources without adding them to the
production build. Copy the selected `.cpp.txt` to an isolated scratch `.cpp`:

* [Initial handoff source](raw-emission-probes-2026-10-10/handoff-baseline.cpp.txt).
* [Handoff plus exchange source](raw-emission-probes-2026-10-10/handoff-armed.cpp.txt).
* [Named retention/crash source](raw-emission-probes-2026-10-10/custody.cpp.txt).
* [LLVM23 raw producer results](raw-emission-probes-2026-10-10/mac-llvm23-handoff.csv)
  and [actual compiler/SDK flags](raw-emission-probes-2026-10-10/mac-llvm23-config.txt).
* [Apple17 baseline](raw-emission-probes-2026-10-10/mac-handoff.csv) and
  [armed](raw-emission-probes-2026-10-10/mac-armed.csv).
* [Linux baseline](raw-emission-probes-2026-10-10/linux-handoff.csv) and
  [armed](raw-emission-probes-2026-10-10/linux-armed.csv).
* [Mac crash cuts](raw-emission-probes-2026-10-10/mac-custody.txt) and
  [Linux crash cuts](raw-emission-probes-2026-10-10/linux-custody.txt).
* [Scoped diagnostic run](raw-emission-probes-2026-10-10/mac-sanitized.txt) and
  [artifact hashes](raw-emission-probes-2026-10-10/sha256.json).

Compile handoff with `-std=c++20 -O3 -Wall -Wextra -Wconversion -Werror`, custody
with `-O2` and the same language/warnings. Add actual macOS sysroot for LLVM23.
Linux uses `/usr/bin/clang++-18`. Generate assembly with `-O3 -S`.
Run handoff as `./handoff ring|armed|pipe SIZE COUNT` using the three size/count
pairs above. The original source accepts ring/pipe; the exchange source also
accepts armed. Recreate each trial in a new process.

For custody choose a unique short POSIX name and absent scratch file. Run
`prepare NAME FILE STAGE`, then `publish NAME FILE STAGE`, then start
`record NAME FILE STAGE`; it prints/stops at drain/partial/write/sync/ack.
Kill that process with SIGKILL, inspect, recover, inspect. Partial recovery must
exit13 and preserve the exact tail; others must exit0 and preserve exactly128B.
Finish with `clean NAME FILE STAGE`. The actual runs imposed5-second case and
10-second timing timeouts. Remote scratch directories and named objects were
removed; binaries/assembly/working logs remain only in ignored context.
