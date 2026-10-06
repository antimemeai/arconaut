# C++ rigor seed for Arconaut

2026-10-01. Adapted from Rhizome research, read-only. C++ with Lua is now the
operator's language decision for Arconaut. This proposes a verification environment;
it adopts no reload framework, runner, binding library, or other dependency and
does not establish an operationally qualified build stack.

## What transfers from Rhizome

Read the [C++ rigor proposal](../../rhizome/papers/cpp-rigor-stack.md),
[representations study](../../rhizome/papers/cpp-types-and-refactoring.md),
[oracle and harness survey](../../rhizome/papers/cpp-oracle-harness-survey.md),
[formal-methods study](../../rhizome/papers/executable-design-and-formal-methods.md),
and [editor preliminaries](../../rhizome/tooling/README.md).
Their observations and candidate qualifications are retained as research evidence;
Rhizome's engine contracts and tool selections do not become Arconaut's contracts.

Transfer explicit RAII ownership, checked construction, distinct domain types,
fallible results, real compilation databases, focused compiler/static diagnostics,
separate instrumented builds, and independent semantic/history oracles. For Arconaut,
the important domains are operation IDs, process incarnations, context IDs,
definition generations, byte lengths, and participant/environment identities.
A string alias is not type separation; a span is not ownership.

Transfer fault-directed simulation at actual OS/provider/storage/reload seams.
Do not import Rhizome's SIMD, numeric encoding, or kernel-performance machinery
without a corresponding Arconaut claim. Do not introduce lock-free structures
merely to justify a weak-memory checker.

## Proposed everyday stack

Use a deliberately pinned Clang/compiler-library profile, CMake/Ninja build presets,
the real compile_commands.json, clangd, clang-format, focused clang-tidy and Clang
Static Analyzer checks. C++20 is a proposed starting baseline for discussion;
needed features and reload compatibility determine the final choice. Select warning
flags on the actual compiler, with unknown required flags/attributes rejected.
The proposal avoids a universal command copied from a newer LLVM documentation page.

Use strong owner/handle boundaries and tagged lifecycle states. Explicitly define
exception/error paths, destructor behavior, cross-module allocation ownership,
ABI/layout compatibility, and which code may call Lua error/yield APIs. Compile
guards are valuable when they constrain an actual contract; optional tooling must
fail if its promised instrumentation is unavailable.

Locally reobserved on 2026-10-01: Apple Clang, clang-format and clangd 17 via xcrun;
CMake 4.3.0; Ninja 1.13.0.git.kitware.jobserver-pipe-1. Standalone clang-tidy was not
on PATH. These version queries do not qualify analysis, sanitizers, reload, or a
build. Rhizome's prior syntax-only effect/flag experiments remain bounded findings,
not an Arconaut runtime qualification. Nothing was installed or acquired code run.
The installed lua/luac report 5.4.8; this is not the runtime version proposed by the
Lua study and does not qualify newer Lua syntax or embedding behavior.

## Oracles for this harness

| Fault or claim | Direct evidence to build | Candidate mechanism |
| --- | --- | --- |
| Dangling handles, stale Lua references, native callback retirement | Actual ownership transitions and invalidation cases; correct values/errors as well as lifetime failures | ASan/UBSan; focused static analysis |
| Concurrent capture, completion, interruption and reclamation | Actual concurrent runs plus an independent permitted-history model | Separate TSan build; bounded weak-memory checker only for a real atomic unit |
| Lua reload affects ongoing work too early | A suspended old workflow must complete with its old definitions; subsequent work uses the new ones | Host/Lua conformance exercise with generation oracle |
| Native swap loses state or calls retired code | Observable state before/after swap; held callback/object/closure prevents retirement; rejected migration leaves promised old behavior | Real module integration fixture, not loader-success-only test |
| Cancellation is confused with settlement | Exit, descendant/stream completion, provider terminal response and unknown effect outcomes checked separately | Owned deterministic operation-history model plus real OS/provider-fake conformance |
| Audit drops or alters emitted data | Independently known input/output compared byte-for-byte with retained originals, including failures and interleaving | Bounded emitting fixture, faultable writer and real filesystem fixture |
| Refit duplicates ownership or loses standing work | Single conversation execution owner, unchanged preserved worker state, actual resume and continuous observations through failed build/start | Owned deterministic state model plus real multiprocess refit exercise |
| Corrupt frame, provider input or component state | Spec-derived accepted/rejected values and bounded work, with sanitized execution | libFuzzer/AFL++ candidates; owned semantic/error oracle |
| Compaction corrupts requirements | A seeded omitted/misstated requirement is recovered from originals and honored by the continued task | Direct context-repair scenario |

Reuse the same specification-derived oracle across ordinary, generated, instrumented,
and optimized executions. Simulated settlement or fsync cannot qualify the actual
OS/provider/filesystem contract. A seeded schedule is insufficient if uncontrolled
threads still determine outcomes. Scope failures and observations explicitly.

## Tools to qualify rather than accumulate

Rhizome recommends GoogleTest with FuzzTest, with Catch2 as a runner alternative.
Choose one runner only after discussion. FuzzTest's documented Linux/Clang fuzzing
and dependency graph make it a fleet candidate, not an assumed macOS bootstrap tool.
An owned bounded history generator may suffice initially if it directly explores
the needed operations; avoid writing a generic test framework for its own sake.

ASan/selected UBSan and separate TSan builds are useful early subjects. MSan is a
Linux/instrumented-dependency subject, not a promised ordinary Mac configuration.
Sanitizer compatibility with the chosen dynamic modules and Lua build must be
qualified. Required runtimes/checks fail their build profile when unavailable.

Use a small executable state model for generation activation and exclusive refit
ownership if it exposes interleaving defects. Quint is optional; TLA+ tools that
require a JVM conflict with the operator's general JVM prohibition and are not
proposed as the default stack. CBMC/GenMC/Relacy remain conditional bounded-unit
candidates. A model result must connect to actual implementation observations.
Mull and other mutation campaigns run on the fleet only, never this laptop.

## Reload research to carry into qualification

[RCC++](https://github.com/RuntimeCompiledCPlusPlus/RuntimeCompiledCPlusPlus) uses
compiled module/object replacement with explicit serialization and stable interfaces.
Its [author chapter](https://www.gameaipro.com/GameAIPro/GameAIPro_Chapter15_Runtime_Compiled_C%2B%2B_for_Rapid_AI_Development.pdf)
explains the mechanism. Source inspected in the preceding discussion at revision
005c05145d98b87d974c36fc885003ea88bf3932 included object serialization, construction,
restore, initialization, old-object destruction and listener notification. That is
not evidence of arbitrary stack preservation, concurrency settlement, or recovery
from every native fault.

[Godot's current GDExtensionManager contract](https://docs.godotengine.org/en/stable/classes/class_gdextensionmanager.html)
limits reload_extension to the editor; release builds fail that call.
[JENOVA/Sakura](https://jenova-framework.github.io/docs/pages/Advanced/Hot-Reload/)
documents runtime script reload but says script objects/variables are lost unless
explicit cross-reload storage is used. Its
[getting-started requirements](https://jenova-framework.github.io/docs/pages/Getting-Started/)
document 64-bit Windows/Linux, not supported macOS. Example Mac library filenames
do not establish that support. These source observations are not executed qualification.

Proposed first qualification subject: one stateful native component referenced by
a Lua workflow, with a pending generation, held callback, migration failure, failed
compilation, and explicit activation after workflow conclusion. Check semantic state
and actual lifetime/retirement behavior on macOS arm64. This can discriminate an
RCC++ dependency from an owned narrow loader, and expose whether a Godot-related
approach carries enough benefit to justify its coupling. Discuss adoption before
building production on any candidate.

Subsequent [native source grounding](2026-10-01-native-reload-grounding.md) acquires
RCC++/cr and identifies object destruction, retained modules and control-transfer
limits; [custody grounding](2026-10-01-refit-custody-grounding.md) adds LevelDB and
platform mechanisms. These sharpen qualification subjects rather than qualifying
this proposed stack.
