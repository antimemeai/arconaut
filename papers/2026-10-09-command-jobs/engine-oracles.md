# Command jobs engine fault oracles

2026-10-09. Independent test implementation in
`tests/command_jobs_engine_test.cpp`; no product/CMake edits in this lane.
The test binary is its own controlled child fixture. It records launch count and
actual PID, emits specification-selected binary bytes and waits on an explicit
filesystem gate. No provider account, shell timing heuristic, dependency or external
reference executable is used.

Five cases define expected behavior independently of the registry implementation:

- Identity: initial zero-yield exec is nonterminal with output_ref equal to job ID;
  three foreground waits preserve PID and one launch; unrelated successful and
  failed workflows leave custody/reserve intact; restart/switch refuse active
  command; eventual native exit7 settles original attempt as failure exactly once;
  exact bytes `41 00 ff c3 a9 5a` survive; model-authored task snapshot stays equal.
- Deadline: a one-second command lifetime survives foreground wait boundaries
  without renewal, settles unknown once, and never reaches the gated external
  completion effect.
- PTY input: same request identity does not duplicate delivery; different bytes
  conflict; exact request is present in retained invocation before gate release.
  Child configures private PTY, receives input and additionally observes remaining
  queued input for a terminal-driver timeout, so duplicate kernel writes cannot
  hide behind consuming only the first newline. Exact retained stdout includes
  one input sequence and the binary prefix/suffix.
- Capacity: 2MiB produced output against an actual256KiB audit causes capacity
  refusal and reserved bounded unknown terminal exactly once, never replay or
  a provider call. Prior retained sources remain authoritative.
- Quiet provider: a fake request polls its actual provider cancellation callback
  with no streamed bytes; background completion becomes retained during that
  request; context head does not change inside the request; exact output and
  one-launch marker remain intact.

The existing `tests/participant_recovery_test.cpp` already supplies the direct
synthetic unresolved-exec reopen refusal and zero-replay fence oracle. It should
run as a relevant existing test rather than be duplicated here.

Initial red qualification: qualified clang-format ran; Clang23 syntax inspection
with the actual Xcode SDK reports only missing planned CodingEngine
poll_commands/shutdown_commands declarations. Full target/runtime validation waits
for the product owner to land the declared APIs and add the target. Initial plain
clang invocation lacked the environment's Xcode sysroot; corrected immediately
to the configured SDK. No runtime outcome is claimed from a syntax check.

First integrated runtime surfaced a fixture setup error: nested audit directories
created with ordinary permissions correctly failed the native0700 requirement
with EPERM. Fixed the test directories and added scenario-labeled exceptions.
After rebuild, the product owner's run passes identity, deadline and PTY cases;
capacity failure reports that its reserved terminal outcome was not retained.
The capacity oracle remains unchanged and now prints the exact poll Error and
job projection at failure. Teardown explicitly accepts the existing sticky
capacity error while independently requiring the actual child PID to be gone.
Product owner identified and is correcting a temporary-lifetime bug in capture
failure enumeration. Full suite outcome remains pending that concrete correction.
