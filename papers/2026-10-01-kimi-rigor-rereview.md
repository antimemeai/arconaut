# Independent Kimi rigor rereview

2026-10-01. Direct CLI subscription session, read-only Read/Grep/Glob profile.
Capture: `context/kimi-review/run-ykrurmm9`; session
`session_26abf263-6a3b-4b7f-a2b0-18f949732307`. CLI exit 0.
The response below is retained unchanged; owner dispositions are separate.

# Rereview — corrections to findings F1–F6 plus kimi-review custody invariant

Inspected: `papers/2026-10-01-kimi-rigor-review.md`, `scripts/rigor` (current, 235 lines), `scripts/jev-check.py` (140 lines), `scripts/kimi-review` (148 lines), `CMakeLists.txt`, `CMakePresets.json`, `tooling/local-profile.json`. Read-only; no executions, so owner's demonstrated red runs are taken as claims consistent with the code paths I traced, not independently observed.

## Per-finding verdicts

**F1 (open allowlist of source trees) — RESOLVED.** `sources()` (scripts/rigor:67-77) now walks the entire checkout with `os.walk`, excluding only research/capture/build roots (`quarantine`, `papers`, `context`, `.context`, `.git`, `.beads`, `build`, `target`, `.dolt`, `__pycache__`) at the top level. A new `runtime/a.cpp` is now picked up for clang-format/stylua/luac and hits the compile-db missing check (lines 111-115). Direction of any residual surprise is fail-closed (stray `.cpp` in an unexcluded dot-dir now fails the gate rather than escaping it).

**F2 (orphan headers) — RESOLVED.** `lint()` (scripts/rigor:116-143) replays each debug compile entry with `-MM -MT arconaut-deps`, parses the depfile (continuation-joining, shlex-aware for escaped spaces, format validated at line 136-137), resolves dependencies against `entry["directory"]`, and rejects any owned `.h/.hpp` not reached by a real analyzed TU (lines 141-143). `-o/-MT/-MF/-MQ` argument pairs are correctly skipped when reconstructing flags. Dep paths that are absolute survive `Path(dir) / p` unchanged. This is a genuine compiler-oracle attachment, not a heuristic.

**F3 (silently selected build drivers) — RESOLVED.** `local-profile.json:3-5` pins cmake/ctest 4.3.0 and the anaconda ninja `1.13.0.git.kitware.jobserver-pipe-1`; `verify_tools()` (scripts/rigor:50-51) version-checks all three; `build()` and ctest invocations use the pinned paths (lines 63-64, 170, 224); and every compile-db entry must invoke the pinned `clang++` as argv[0] (lines 118-120), which also closes my earlier "wrong/stale compilation database" oracle attack.

**F4 (fenced-code headings) — RESOLVED.** `section_snapshot` (scripts/jev-check.py:44-53) now tracks fence state: openers with ≤3-space indent, backtick info strings may not contain backticks (CommonMark), closers require same char, length ≥ opener, empty info. Section boundaries (lines 62-65) also use the fence-filtered heading list, so a fenced subheading can no longer truncate a real section. Unclosed fences swallow later headings → "must exist exactly once" error → fail-closed. Indented-code-block content can't match because the heading regex anchors `^#{1,6}` at column 0 (which also means a legitimately 1–3-space-indented ATX heading would hard-fail rather than misattribute — acceptable).

**F5 (wrong-model response accepted) — RESOLVED.** scripts/jev-check.py:118-122: `response.json` is saved unchanged, `actual_model` recorded, then mismatch sets `phase = "model_mismatch"` and raises — exit 1, disposition `model_mismatch_failed` in `run.json`. Exactly the requested loud failure with retained material.

**F6 (basename exemptions) — RESOLVED.** The fault-probe exemption is now the exact resolved path `tooling/probes/instrumentation_fault.cpp` (scripts/rigor:106-107, 112). The `bytes_fuzz.cpp` basename exemption is gone entirely: `bytes_fuzz_probe` is an OBJECT library in all profiles (CMakeLists.txt:44-45), so the probe is compiled and clang-tidy-analyzed in ordinary builds, with the fuzzer entrypoint linked only in the fuzz profile (CMakeLists.txt:46-50). A future stray file sharing either basename is fully checked.

## kimi-review custody invariant (new, inspected as requested)

Correct: handlers installed for SIGINT/SIGTERM/SIGHUP before spawn; pre-spawn and post-`Popen` signal arrival both funnel into the same cleanup (lines 82-90); cleanup ignores further signals, killpg TERM (valid because `start_new_session=True` makes pgid == child.pid), 10 s grace, then KILL with a final unbounded wait that is safe after SIGKILL; timeout maps to 124/"timeout", interruption to 128+sig/"interrupted"; actual child exit (including negative signal-coded returncodes) is recorded as `child_exit_code` (line 114); the `128 - code` mapping at line 144 correctly re-encodes a signal-killed child (e.g. −15 → 143). Resume pinning is sound: profile hash + cwd equality, strict session-ID regex, ambiguous or changed captured IDs force `session_id: None` with `session_capture_error` rather than guessing (lines 130-134). `result.json` is written in `finally`, so even an unhandled exception leaves a `failed` record.

**Residual (low) — OSError after a successful `Popen` leaks the child.** If an `OSError` is raised inside the `with` block after spawn (e.g. from `child.wait` itself or a filesystem event on the capture files), control jumps to `except OSError` at line 135 with no killpg, leaving the Kimi process group running while the wrapper exits 127 and records `failed`. The common OSError cases (unwritable capture dir, missing `kimi` binary) occur pre-spawn and are handled; this is a narrow path but a child-custody hole on a custody invariant. Fix: in the `except OSError` handler, if `child is not None and child.poll() is None`, killpg TERM/KILL mirroring lines 96-106 before recording.

**Minor note:** a signal arriving after the child has exited but before `result` is finalized is silently ignored in the record (status stays `completed`) — truthful about the child, just unattributed about the signal. Not worth changing.

## Consequential remaining defects

1. **Low — preset/profile pins can drift apart silently.** `CMakePresets.json:8-9` hardcode the compiler and `CMAKE_MAKE_PROGRAM` independently of `local-profile.json`. The compiler side is backstopped by the CMakeLists.txt:4-7 version FATAL_ERROR, but nothing verifies the preset's ninja path against the profile pin: edit only `local-profile.json` (or only the preset) and `verify_tools` qualifies one ninja while the build drives another, with `latest-qualification.json` attributing the wrong one. Fix: `verify_tools()` (or `build()`) should parse `CMakePresets.json` and require `CMAKE_MAKE_PROGRAM` and `CMAKE_CXX_COMPILER` to equal the profile pins.
2. **Low — symlinked source trees escape enumeration.** `os.walk` (scripts/rigor:71) runs with default `followlinks=False`; an owned source directory reachable only via a top-level symlink is silently skipped by format/lua/db-missing gates (compiled files would still be tidied via the db). Fix: reject directory symlinks under ROOT, or walk with `followlinks=True` plus loop detection.
3. **Nit-grade, noting for completeness:** the new tidy probes invoke unpinned `xcrun --show-sdk-path` (scripts/rigor:186) to obtain `-isysroot`; the SDK path is printed via `run()` so it is visible, but it is outside the profile pins. Acceptable as an OS locator; flag only if you want the SDK itself pinned.

No fleet mutation or Lua 5.5 embedding is claimed in the changed code, and `latest-qualification.json`'s scope wording ("tool capability only; no agent-runtime qualification") remains accurate. I did not execute any of the demonstrated red cases; the code paths for all four (unregistered probe-basename source, orphan header, fenced-only heading, wrong-model stub) reach their respective rejections as traced above.
