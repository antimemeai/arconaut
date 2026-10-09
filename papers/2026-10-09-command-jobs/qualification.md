# Command jobs qualification status

Operator explicitly reopened2026-10-09 at12:16:37UTC to fix the retained bugs.
Both causal repairs and direct Linux ASAN/TSAN PTY oracles are complete. The
single final affected profile recheck and complete staged-tree debug plus all
owned C++/Lua lint gate are underway. Candidate remains inactive pending explicit
main integration/activation. See bug-fixes.md and tsan-pty-remediation.md for actual
causes; the Linux failure was the resource child, mislabeled closed-stdio by the
old shared assertion. Earlier failures below remain historical evidence.

Mac release and ASAN final five-case rechecks pass (command native/engine/PTY/
station and chat_render_oracle). Final TSAN and Linux debug/release/ASAN results
will be recorded after completion. Required full hook is still outstanding;
focused passes do not substitute for it. Final hardening starts12:27:21UTC and
remains bounded through12:52:21UTC, with no extra review layer.

# First bounded unit results

2026-10-09, stopped within the declared hardening allowance ending12:13UTC.
Candidate branch command-jobs; implementation is retained but INACTIVE and NOT
qualified. Running operator executable, main and supplied art were not changed.

| Check | Actual result and retained capture |
| --- | --- |
| Primary platform primitives | Mac and Linux pass; platform.md records Darwin waitid deviations |
| Concrete independent review | Four consequential findings fixed; code-review.md; Kimi returned403 before reviewing |
| Mac debug command/native-engine/PTY/station |4/4 pass, context/command-jobs/debug-bounded-tests.log |
| Mac release same four cases |4/4 pass, release-bounded-tests.log |
| Mac ASAN native and PTY final oracles |2/2 pass, asan-isolated-tests.log; same final product engine and station passed in asan-recheck-tests.log |
| Mac TSAN final recheck | Native, engine and station pass; PTY fails waiting for complete real tty dimensions after resize, tsan-bounded-tests.log |
| Linux debug and release |4/4 each pass, context/linux/run-1g2qw7mp/checks.log |
| Linux ASAN | Engine, PTY and station pass; native fails with invalid-vptr UBSan diagnostic and closed-stdio subprocess oracle failure |
| Linux ASAN changed native oracle recheck | Failure reproduces, context/linux/run-ijn_zxpx/checks.log; not a saturation timing failure |
| Focused static analysis | Collector, exec helper and final coding/terminal/native-test/engine-test analysis pass; logs in context/command-jobs |
| Required complete commit gate | Last completed full debug run83/84, blocked by PTY pressure writer; final pressure timing/read changes passed focused checks but full debug plus all owned C++/Lua lint has NOT passed |

No assertion was removed or suppression added. Final pressure writer deadline is
30seconds and PTY CTest timeout60seconds; fixture uses actual raw mode and512KiB.
The one-MiB saturation oracle now requests count0 metadata instead of repeatedly
copying output while holding the collector mutex. Earlier failing traces remain.

Next work, if the operator explicitly reopens this bounded unit:

1. Obtain a symbolized Linux UBSan trace for the native closed-stdio subprocess
   failure. Isolate runtime/descriptor interactions and Value lifetime without
   suppressing the diagnostic or skipping the closed-stdio case. Establish the
   cause and implement a direct fix/oracle.
2. Trace resize, actual child read/EINTR, exact admitted input and terminal modes
   in the failing TSAN PTY case. Resolve whether command input is lost or the shell
   fixture has a signal/read handshake race; preserve independent effect checks.
3. Recheck the affected cases and complete the required clean staged-tree full
   debug and owned C++/Lua lint gate. Only then checkpoint/push and close arconaut-st9.
4. Main integration and running operator activation remain explicit separate work.
   Steering and trajectories remain deferred.
