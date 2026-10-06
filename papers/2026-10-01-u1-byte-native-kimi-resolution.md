## Disposition of the F5 read-only flock flag: retracted as a defect

### Reassessment against the primary source

I re-derived my claim's basis and it does not hold for the declared target. My concern rested on the access-mode check that POSIX **fcntl record locking** performs (`F_SETLK` with `F_WRLCK` requires a writable descriptor, EBADF otherwise). The cited XNU `sys_flock` implementation (`bsd/kern/kern_descrip.c`, lines 5506–5555) shows `flock(2)` on XNU is a distinct path: it obtains the vnode, maps `LOCK_EX` to `F_WRLCK`, and calls `VNOP_ADVLOCK` with `F_FLOCK` — without the FWRITE pre-check present in the fcntl-locking code elsewhere in that same file. The installed SDK `flock(2)` manual lists EBADF only for an invalid descriptor, not for read-only mode. So the "EBADF before contention on read-only descriptors" behavior I attributed to XNU belongs to POSIX fcntl record locking (and to some other BSD flock implementations), not to the XNU flock syscall on the declared Mac profile. I conflated the two locking families; that was my error, not a nuance.

### Evidence

`context/u1-review-debug-second.log` is a named-host Mac run (arm64-apple-darwin25.6.0, Homebrew clang 23.1.2, debug build): test #4 `journal_native` passes (line 36), with the exact `error_is(read_only->lock_writer(), ErrorCode::busy)` assertion at `tests/journal_native_test.cpp:95` in the binary. Combined with the syscall-source argument, the busy result is explained: on XNU the lock request reaches the vnode layer, contends against the lock already held by `contender` (acquired at line 93 on an independent open-file description), and returns EWOULDBLOCK → busy. The oracle is not merely "currently passing"; its expected value follows from the primary source.

### Consequence for the fixture

The strict `busy` assertion stays exactly as written. Weakening it to "busy-or-io" would, as you note, dissolve a demonstrated valid oracle into an unfalsifiable one and would hide genuine regressions in lock-contention mapping on both declared profiles (Mac/APFS and Neuroses/Linux, where flock likewise performs no mode check). No change recommended.

### Residual scope statement (not a conformance defect)

The only remaining truth in the original flag is out-of-scope portability: on platforms outside the two declared local profiles — other BSDs whose flock enforces a write-mode check, network filesystems with incomplete flock emulation, or hypothetical adapters using fcntl record locking instead of flock — an exclusive lock on a read-only descriptor could fail with EBADF/io rather than contend. The subplan explicitly scopes support to "one restricted local directory" on the named Mac/Linux profiles and disclaims unqualified configurations, so this is a scope limit of the adapter's contract, not a defect in it. If a future unit ever widens supported storage, `lock_writer`'s read-only-descriptor semantics should be re-stated then; no action now.

### Final status of the byte/native boundary review

- F1 (creation durability vs. publication contract): resolved in spec text, verified.
- F2 (predecessor extent bound): resolved, overflow-safe, exact boundary tests verified.
- F3 (payload ≤ batch − 88): resolved, effective 112-byte minimum batch derivation verified.
- F4 (move-only `NativeJournalFile`): resolved, descriptor-ownership lifecycle (exchange/self-move/moved-from-guard/double-close) verified.
- F5: symlink ELOOP, truncated short-read with untouched remainder, and read-only EBADF-write oracles verified; the read-only flock contention assertion is correct on both declared profiles per primary source and observed run — **flag withdrawn**.

No outstanding findings remain in the reviewed scope (journal codec, native storage adapter, and their test oracles). Batch commit/recovery/dependency replay, continuation orchestration, issuer ledger, and emergency capture remain deliberately unimplemented and unassessed, to be reviewed when they land.
