# Reopened command bug unit

Operator explicitly reopened2026-10-09 at12:16:37UTC. Budget60minutes total,
including at most25minutes hardening, remediation/fixes then one affected recheck
and fixes. Prior direct review remains useful; no additional panel.

## Linux descriptor-exhaustion fixture

The retained report's generic closed-stdio assertion actually labels both child
modes. Offline symbolization of the actual sanitized binary locates the failure
in resource_failure -> Evidence::start -> arguments -> Value string construction.
It occurs before any command starts. The resource child lowered RLIMIT_NOFILE to3,
leaving no descriptors for the sanitizer. LLVM18.1.3 Itanium getVtablePrefix calls
IsAccessibleMemoryRange, which allocates a temporary pipe and returns false when
that pipe fails. A minimal valid Live object succeeds at normal limit and raises
invalid-vptr at limit3, with identical runtime inability to inspect memory. No
Value lifetime defect is established by this diagnostic.

Primary source:
https://github.com/llvm/llvm-project/blob/llvmorg-18.1.3/compiler-rt/lib/ubsan/ubsan_type_hash_itanium.cpp
and sanitizer_common/sanitizer_posix_libcdep.cpp at the same tag. Official diagnostic
instructions: https://clang.llvm.org/docs/UndefinedBehaviorSanitizer.html . Captures:
context/command-jobs/fd-sanitizer-probe.log, linux-resource-symbolized.log,
context/linux/run-8a36vrhl/checks.log. Original failure is retained.

Fix plan: prepare fixture values before limiting descriptors. Set the real soft
limit to6: stdio plus CTest's inherited fd3 log leave two temporary slots for
sanitizer memory checks. A direct pipe probe checks this headroom before starting.
The complete normalized exec pipes still cannot fit this limit. Limit5 passed
in a direct shell run but failed under CTest because its fd3 log consumed a slot;
readlink census identified the actual inherited LastTest.log.tmp descriptor.
Start then join the worker before restoring the limit and pumping
Value observations, preserving the real syscall refusal. Require exact EMFILE,
unknown outcome, no invented child exit, one terminal, no launch side effect and
no child custody. Distinguish child modes in failure diagnostics. Do not suppress
vptr checking, skip the resource case, or replace the real OS failure with injection.

The whole PTY lane's causal findings, exact audit/reader traces, CAN/SUB reference
semantics and direct TSAN/parser results are in tsan-pty-remediation.md. Both fixes
preserve and strengthen independent oracles. Linux direct ASAN native green is
context/command-jobs/linux-resource-green.log (2.38seconds). No production source
change was indicated by the two reopened defects. This focused result does not
replace the complete required staged-tree gate.
