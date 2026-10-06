# Kimi independent rigor review

2026-10-01. Actual subscription-backed Kimi Code 2.1.1, configured model alias
`kimi-code/k3`, session `session_26abf263-6a3b-4b7f-a2b0-18f949732307`.
The owned `scripts/kimi-review` bound a `Read`/`Grep`/`Glob`-only profile with no
sub-agent access. Child exit 0. Raw prompt, tool exchanges, response and status
remain in ignored `context/kimi-review/run-j3004_sd/`.

The following is Kimi's actual final response, retained without editorial
rewriting. These are independent findings to be reproduced and classified by the
owning engineer; Kimi executed no shell commands or tests and changed no code.
The known LLVM 21 to 23.1.2 transition was supplied explicitly as an existing
qualification issue, not offered as a new finding. Review read live files while
that tool profile was changing, so line locations refer to the inspected state.

---

# Independent review — Arconaut rigor configuration (fail-closed profiles, truthful attribution)

Scope inspected: `CMakeLists.txt`, `CMakePresets.json`, `scripts/rigor`, `scripts/clangd`, `scripts/jev-check.py`, `tooling/local-profile.json`, `.clang-format`, `.clang-tidy`, `.clangd`, `.luarc.json`, `.stylua.toml`, `tooling/probes/*`, `tooling/JEV.md`, `tooling/jev-cases/review-foundation.json`, `docs/RIGOR_SETUP_SUBPLAN.md`, `docs/TESTING_PLAN.md`. Verified case-section headings exist exactly once in `docs/CORE_DESIGN.md:66`, `:222` and `docs/TESTING_PLAN.md:10`. Per your note I did not flag the LLVM 21→23.1.2 transition strings (CMakeLists.txt:5, CMakePresets.json:8, scripts/clangd:2, local-profile.json:3).

## Findings

### F1 (medium): `sources()` is an open allowlist — future owned source trees silently escape format/lua gates
`scripts/rigor:66-72`. Only `src`, `include`, `lua`, `tests`, `tooling/probes` are scanned. Triggering state: owner adds a new top-level directory (e.g. `runtime/`, `outpost/`) with `.cpp`/`.lua` files. Consequences:
- If not wired into CMake: no clang-format, no clang-tidy, no compile-db check, no stylua/luac — `rigor check`/`qualify` exit 0 with the files entirely unqualified, and `latest-qualification.json` still claims green.
- If wired into CMake: clang-tidy does see it (via compile-db `units`, line 98-99), but the "missing from compilation database" check (lines 103-107) only iterates files `sources()` found, so the fail-closed tripwire never fires for the unregistered tree; clang-format/stylua/luac still skip it.
Required: fail closed — enumerate owned files (e.g. `git ls-files` by extension) and refuse to pass when any owned source file is outside the registered roots, or derive the roots from the build and reject unclassified source-bearing directories. Red case: `mkdir runtime && printf 'int f(){return 0;}\n' > runtime/a.cpp` → run `scripts/rigor check` → currently passes; must fail naming `runtime/a.cpp`.

### F2 (medium): orphan headers are never clang-tidy analyzed
`scripts/rigor:103-105` — the missing-from-database check covers only `.cpp/.cc/.cxx`; headers are checked only transitively. Trigger: a future `include/foo.hpp` not (yet) included by any TU, containing e.g. an unchecked `std::optional::value()`. clang-format checks it, but clang-tidy never runs on it and no gate notices — silent green despite `.clang-tidy` naming `include/` in `HeaderFilterRegex`. Required: for each owned header, either require inclusion reachability from the compile db or run a per-header tidy pass with a synthetic TU; fail when a header is analyzable by neither. Red case as above: tidy reports nothing, `rigor check` passes; it must fail.

### F3 (medium): cmake and the build driver are silently selected
`scripts/rigor:48-50, 62-63`; `tooling/local-profile.json`; `CMakePresets.json:9`. `verify_tools()` pins clang*/LuaLS/StyLua/Lua versions, but `cmake` is resolved from `PATH` with no path or version check, and the pinned ninja (`/opt/homebrew/anaconda3/bin/ninja` — a different toolchain family than the Homebrew-pinned rest) has its version never verified. The subplan (docs/RIGOR_SETUP_SUBPLAN.md:11-12) requires "Pin tool paths/versions in a local profile." Trigger: PATH changes to a different cmake (e.g. anaconda's) → builds proceed under an unrecorded, unqualified driver; `latest-qualification.json` versions map omits it, so the attribution is untruthful by omission. Required: add `cmake`/`ninja` entries (path + version) to `local-profile.json`, verify both in `verify_tools()`, and invoke cmake via the pinned path.

### F4 (medium): Jev section snapshotter counts Markdown headings inside fenced code blocks
`scripts/jev-check.py:44-56`. Heading matching never tracks ``` / ~~~ fence state. Two failure modes:
- **Silent misattribution**: a review packet whose only occurrence of the wanted heading is inside a code fence (e.g. an example spec excerpt) yields `matches == 1`; the wrapper snapshots the fence content and records `section_sha256`/line range attributing it as the named specification section — exactly the "truthful capture attribution" invariant broken, and those bytes are what Jev judges against.
- **Spurious hard failure**: a file containing the real heading plus a fenced example of it errors with "Source section must exist exactly once" although the real section is unique.
Red case: packet containing only ```` ```markdown ```` + `## Operation lifecycle` + filler → run preparation for section "Operation lifecycle" → currently succeeds with fence bytes captured as the spec; must reject or skip fenced text. Fix: track fenced-code state while scanning lines before applying the heading regex.

### F5 (low-medium): live Jev response from a different model is accepted silently
`scripts/jev-check.py:108-114`. `actual_model` is recorded (good retention), but never compared to the pinned `case["model"]`; disposition becomes `answered` and exit status is 0 even if the shared client routed to `jev-2.x` or a fallback. "Pinned model" (JEV.md:46,61) plus "libraries and runtimes must not be silently selected" argues this must be a loud failure, not a quiet record field. Required: if `response["model"] != case["model"]`, save `response.json` unchanged (as now) but set a mismatch disposition and exit nonzero. Red case: stub client returning valid answers with `"model": "jev-9.9.9"` → currently exit 0 "answered"; must fail.

### F6 (low): exclusion of intentional-defect files is by basename
`scripts/rigor:99, 104`. Any future owned file named `instrumentation_fault.cpp` or `bytes_fuzz.cpp` anywhere escapes clang-tidy / the db-missing check. Anchor the exclusions to `tooling/probes/` paths.

## Oracle attacks proposed (not currently demonstrated)

- **Compiler-pin rejection**: `qualify()` never demonstrates the CMakeLists.txt:4-7 gate fails closed. Configure once with a non-pinned compiler (`cmake -B /tmp/x -DCMAKE_CXX_COMPILER=/usr/bin/clang++`) and require `FATAL_ERROR`. The subplan (lines 19-22) demands demonstrating intended red signals; this one is absent.
- **Wrong/stale compilation database**: `lint()` (scripts/rigor:97-109) trusts `build/debug/compile_commands.json` unconditionally. Verify each entry's `command`/`compiler` begins with the pinned `clang++` path from `local-profile.json`; otherwise a debug tree configured by hand with a different compiler is tidied under wrong flags and reported as qualified.
- **compile_commands staleness**: re-run configure is implicit; consider checking db mtime ≥ CMakeCache, or reconfiguring debug inside `lint()` before reading the db.

## What checked out (no fault found)

- `run()` child lifetime: `start_new_session` + `killpg` TERM→KILL with bounded waits; no orphaned grandchildren (scripts/rigor:22-35). jev-check `subprocess.run(timeout=120)` kills the direct child.
- No silent success after rejected checks: all gates raise `RuntimeError`/`ValueError`; `main` catches and exits 1 (rigor:196-200); jev-check records `*_failed` disposition into `run.json` before exiting 1.
- Compile-db emptiness is fail-closed ("no actual translation units", rigor:100-101); LuaLS report freshness is enforced via a fresh, exclusively-created log dir and explicit `check.json` presence plus schema validation (rigor:76-90).
- Sanitizer probes are direct oracles: `expect_fault` requires nonzero exit *and* the sanitizer's own marker text (rigor:125-128, 137-140); probes contain genuine defects (heap overflow, signed overflow, data race).
- jev-check: credential-path rejection (`.env*` on case and resolved source paths), repo-containment after `resolve()`, byte bound on case/sources/request, `exist_ok=False` run dirs with mode 0700, `allow_nan=False`, response question-ID set equality, response.json saved before validation, wrapper/client SHA-256 attribution.
- Case file's referenced sections exist exactly once in the real docs; `.luarc.json` excludes `context`/`build` so probe temp dirs can't contaminate the real gate; fuzz probe checks a real byte-identity property with an explicit abort oracle.

## Limits

I did not execute anything (read-only per instructions), so I did not observe an actual `qualify()` run, ASan behavior under LLVM 23.1.2 on macOS 26.6, or the shared `../codex-tools/jev.py` client (outside the checkout; its credential handling and retry behavior were taken from JEV.md's description, lines 70-73). LuaLS/clang-tidy version-specific behaviors (e.g. `check.json` schema stability at 3.19.1) were assessed from the code's schema validation only.
