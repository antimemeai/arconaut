# Command jobs qualification status

The reopened bugs are fixed. Candidate implementation checkpoint `6fa7c38`
passed the complete installed clean-staged-tree hook:84/84 debug tests, all117
owned C++ translation units/headers, Lua formatting/syntax and Lua language-server
analysis. This qualifies the candidate's described command behavior and tested
fault classes; main integration and operator activation remain separate.

| Final changed-case recheck | Actual result | Capture |
| --- | --- | --- |
| Mac release |5/5 pass,26.29s | context/command-jobs/release-reopened-tests.log |
| Mac ASAN/UBSan |5/5 pass,38.91s | context/command-jobs/asan-reopened-tests.log |
| Mac TSAN |5/5 pass,54.94s | context/command-jobs/tsan-reopened-tests.log |
| Linux debug |5/5 pass,9.17s | context/linux/run-8itb2wba/checks.log |
| Linux release |5/5 pass,8.64s | same capture |
| Linux ASAN/UBSan |5/5 pass,11.37s | same capture |
| Full debug plus all owned C++/Lua lint |84/84 pass,95.97s tests; full lint pass; hook exit0 | context/command-jobs/checkpoint-reopened.log |

The five changed cases are command_jobs, command_jobs_engine, command_jobs_pty,
command_jobs_station and chat_render_oracle. Existing unrelated debug cases ran
because the operator requires the complete hook before every commit. No new
suppression, dependency, provider call or additional review was introduced.

The Linux diagnostic originated in the resource fixture before process dispatch:
RLIMIT_NOFILE3 starved LLVM18's temporary pipe used to inspect a valid vptr. A
valid-object reproducer, primary runtime source, symbolized original frame and
CTest's inherited log descriptor discriminate this from a demonstrated Value
lifetime defect. The corrected fixture leaves two checked sanitizer slots, still
forces real EMFILE, joins before observations, and requires no launch effect,
no fabricated exit, no custody debt and exactly one terminal.

The resize case retained complete admitted input and private tty echo while the
shell fixture recorded a truncated command. Its replacement preserves terminal
semantics and records every actual reader byte before parsing. The screen oracle
now handles CAN/SUB cancellation of a partial control before terminal restoration,
while continuing to reject uncancelled malformed/truncated streams. See
[causal fixes](bug-fixes.md) and [PTY evidence](tsan-pty-remediation.md).

Source hardening concluded12:46:51UTC,19m30s after its declared12:27:21UTC start,
within the existing25minute allowance. The evidence/issue-closure documentation
checkpoint follows the same mandatory per-commit hook; it introduces no product
change or new qualification campaign. The operator's explicitly reopened60minute
total scope remains unchanged. Running operator executable and supplied art are
untouched; no master merge, activation, steering or additional trajectory work.

Earlier stopped-unit results below remain historical failures, not final status.

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
