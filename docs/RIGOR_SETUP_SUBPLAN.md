# Operational rigor setup

2026-10-01. Implements the operator's request to configure the reviewed testing
stack before beginning core implementation. Parent contracts are TESTING_PLAN T0
and IMPLEMENTATION_PLAN U0. This unit configures development tools and qualifies
their actual diagnostics; it does not implement the agent or qualify its future
Lua embedding, provider or journal.

Use Homebrew LLVM 23.1.2 as a pinned coherent development profile,
C++20, CMake/Ninja and owned CTest fixtures. Build debug/optimized, ASan+UBSan,
separate TSan and libFuzzer profiles. Pin tool paths/versions in a local profile
and reject drift/missing instrumentation. Emit a real compilation database; configure
clangd, format, focused clang-tidy and analyzer diagnostics. No production dependency
or testing framework is adopted. LuaLS and StyLua are development executables;
install their official Homebrew bottles and record versions. Existing Lua 5.4.8
can parse/run compatibility probes; Lua 5.5 production/runtime embedding remains
an explicit future adoption/qualification, not an invisible fallback.

Before accepting setup, demonstrate intended red signals: absent configuration
initially fails to configure; known native memory/UB/race errors trigger the actual
instrumented profiles; unformatted source fails formatting; a focused tidy defect
and analyzer defect diagnose; a Lua annotation/type error appears in check.json
and fails the owned gate. Clean fixtures then pass debug/optimized/instrumented
execution and focused analysis. A bounded native fuzzer checks an explicit byte
identity property to qualify the fuzz entry/runtime, not future agent semantics.
Capture real versions and results under ignored context; describe achieved scope
in tooling/README.md. Add local commands, clean target behavior and ignore outputs.

Fleet mutation and Linux-only profiles use an operator-supplied host/launcher and
must never run mutants on this laptop. Missing fleet entry remains a named missing
qualification; do not claim full stack success by silently skipping it. Kimi reviews
the resulting setup independently once authenticated, and can challenge its cases.
JEV supplies advisory semantic checks via the existing owned workspace client;
it does not replace deterministic test oracles or become an authorization gate.

After green checks, adversarial review examines fail-closed profile/diagnostic gates,
tool attribution, useful negative oracles and scope claims. Integrate findings and
reread. Root journals actions/decisions and updates beads; no global editor/shell
configuration or unrelated project files are changed.

Qualification revision: installed LLVM 21.1.0 ASan entered an initialization
deadlock inside dyld/allocator callbacks on macOS 26.6. Retained the process sample
and rejected that runtime profile. Installed the current Homebrew LLVM 23.1.2
with required Z3 update, preserving the LLVM 21 keg; disabled automatic unrelated
dependent upgrades. Requalify the complete profile rather than disabling ASan.

Review integration: enumerate all owned source roots, reject ungoverned directory
symlinks, and require headers to be actual compiler dependencies of analyzed
translation units. Pin and cross-check preset/profile build drivers. Avoid basename
exemptions. Jev section provenance must ignore fenced examples and reject wrong
actual-model attribution while retaining its response. Kimi child custody extends
through post-spawn errors as well as timeout/signals. Direct fault cases and
independent rereviews establish these corrections; achieved scope is in the
tooling README and papers/2026-10-01-rigor-review-resolution.md.
