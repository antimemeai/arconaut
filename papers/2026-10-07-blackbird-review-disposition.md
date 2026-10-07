# Blackbird rename / sprite / profiler tribunal

One round with three independent reviewers: rename compatibility, profiling
lifetime/provenance, and native sprite/board rendering. Reports retained unchanged.

Confirmed findings and fixes:
- Compatibility targets could succeed with their promised executable missing.
  Targets now restore aliases even when primary target is already up to date.
  Move aside arco artifact, build arco target, and execute new help: pass. A
  suspected ETXTBSY finding was directly refuted by the reviewer, not implemented.
- Failed profiler captures discarded fixture counters/stderr. Fixture writes
  directly to private capture files through teardown; failed and interrupted
  fault cases retain actual counters and reap the child. Ctrl-C now records
  interrupted, not an eternal recording state.
- Offset glyph blits left intersecting wide leaders/continuations intact. Clear
  old intersecting footprints before placement. Overlapping CJK/ASCII cases pass;
  cell geometry stays consistent with the painter.
- Board refresh period aliased flame phase, and attached supervisor state differed
  between header/card. Use nonaligned phase cadence and shared observed-running/
  attached status. Static/wide header space and encoder checks found no defects.

Renamed checkout/C++ includes and namespace/current product APIs to Blackbird;
old checkout symlink, launcher/build aliases and same-table Lua alias preserve
self-dev programs. Audit format, session identity/data, issue IDs and historical
quarantine/papers remain unchanged. Explicitly reuse existing legacy default;
new default wins when it exists. Core/provider-free checks exercise these paths.

Actual capture findings: concurrent Instruments recordings conflicted; serialize
recordings. CPU serial capture and source-resolved sample work. Allocation attach
failed until ad-hoc signing profile binaries with get-task-allow; ordinary release
is unchanged. Actual allocation trace then succeeded. Scoped profile signing does
not change machine-wide Developer mode. No profiler library/runtime dependency.

Local full release first run41/42: only old resume-command expectation failed;
updated for the authorized canonical name. Targeted final recheck6/6 in33.27s
including coding API alias, session default selection, RRC, native/VT/PTY rendering.
Board actual installed alias PTY verifies title, SR71 cells and quit. Initial board
quit probe stopped draining output; correct that fixture, not product behavior.
Profile counters describe whole workload time, not just the recorded sample window.
Linux platform checks and final publication status are recorded in JOURNAL.

No second tribunal. Remaining profiler investigation is cost/behavior study using
real captures, not certification. Previously recorded general lint debt stays backlog.
