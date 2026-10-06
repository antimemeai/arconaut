# Native C++ replacement: source grounding for Arconaut

2026-10-01. Focused foundation research, following the operator's C++/Lua decision.
No framework adoption, imported execution, build, installer, provider call, or
runtime qualification occurred. Python was only existing acquisition machinery.

## Readiness judgment

**Enough to start designing the replacement contract; insufficient to select or
claim a qualified native reload implementation.** The references give concrete
mechanisms and failure paths. None supplies Arconaut's definition-generation,
workflow-settlement, Lua-reference, audit, or outpost contract. Those must be owned
and specified. Apple Silicon operational qualification remains a bounded experiment,
not a consequence of a README's macOS support claim.

The important correction to the starting sketch: RCC++ retaining old modules does
not preserve old objects. Its ordinary swap destroys replaced objects immediately.
Arconaut cannot simply hand a Lua workflow an RCC++ object pointer and assume its
old generation remains usable until that workflow ends. We need indirection through
owned stable handles, explicit generation-isolated objects, or activation only after
all affected roots settle. Choosing among these belongs in the design.

## Exact acquired material

[Source catalog](2026-10-01-native-reload-acquisition.json) records URLs, exact
revisions, byte counts, SHA-256, roots, omissions and direct archive comparisons.

| Reference | Revision | Preserved archive | Clean reference |
| --- | --- | --- | --- |
| RCC++ | `005c05145d98b87d974c36fc885003ea88bf3932` | `../quarantine_proj/archives/arconaut-native-reload-2026-10-01/runtime-compiled-cplusplus-005c05145d98.zip` | `quarantine/runtime-compiled-cplusplus/` — 884 files |
| fungos/cr | `1c3f8302320dee8206cf85d6aeb9e9a9cd78b527` | `../quarantine_proj/archives/arconaut-native-reload-2026-10-01/fungos-cr-1c3f8302320d.zip` | `quarantine/fungos-cr/` — 34 files |

Both are intact immutable codeload ZIPs; retained bytes and executable bits were
compared directly using the existing owned importer. No entries were omitted in
these archives. Submodule content is not included in GitHub ZIPs: RCC++'s GLFW
submodule remains absent, not silently downloaded. Core reading does not require it.
RCC++ initial HTTP/2 download was cancelled; a fresh HTTP/1.1 download succeeded,
and only the complete validated ZIP was ingested.

[The author chapter](https://www.gameaipro.com/GameAIPro/GameAIPro_Chapter15_Runtime_Compiled_C%2B%2B_for_Rapid_AI_Development.pdf),
Doug Binks, Matthew Jack and Will Wilson, *Runtime Compiled C++ for Rapid AI
Development*, Game AI Pro chapter 15, was read and preserved at
`papers/native-reload/gameaipro-ch15-runtime-compiled-cpp.pdf` (17 pages).
The publisher's `www` endpoint reset the local download; the same path without
`www` succeeded. The
[literature/documentation catalog](2026-10-01-native-reload-literature-acquisition.json)
records that provenance and captures official Godot/JENOVA documentation.

## RCC++: replacement is explicit reconstruction

The chapter's sections 15.4.2, 15.5–15.7 explain virtual interfaces, factory
constructors, object identity, property serialization and reinitialization.
Nonruntime interfaces stay stable. This is a reconstructed-object mechanism,
not preservation of arbitrary executing C++ stacks or an arbitrary heap.
The chapter is historical; current behavior below comes from the acquired source.

Evidence paths are relative to `quarantine/runtime-compiled-cplusplus/`; links pin
upstream source where the mechanism matters.

1. [`RuntimeObjectSystem.cpp`](https://github.com/RuntimeCompiledCPlusPlus/RuntimeCompiledCPlusPlus/blob/005c05145d98b87d974c36fc885003ea88bf3932/Aurora/RuntimeObjectSystem/RuntimeObjectSystem.cpp),
   lines 386–434: `LoadCompiledModule` loads a compiled artifact with
   `dlopen(..., RTLD_NOW)` or `LoadLibraryA`, locates `GetPerModuleInterface`,
   retains the module handle, then calls `SetupObjectConstructors`.
   A missing export returns failure without closing that already loaded handle.
   Successful module load and successful semantic activation need separate outcomes:
   this method marks load success after setup, while `AddConstructors` returns void
   even when its protected swap has reverted.
2. [`ObjectFactorySystem.cpp`](https://github.com/RuntimeCompiledCPlusPlus/RuntimeCompiledCPlusPlus/blob/005c05145d98b87d974c36fc885003ea88bf3932/Aurora/RuntimeObjectSystem/ObjectFactorySystem/ObjectFactorySystem.cpp),
   lines 85–255, 280–406: serialize old objects, replace constructors, create new
   objects, deserialize, initialize, optionally serialize-test, destroy replaced
   old objects, notify listeners. External holders reacquire pointers from IDs.
   There is no active-call/reference admission barrier in this swap path.
   It does not keep old workflow objects alive until their callers retire.
3. [`ISimpleSerializer.h`](https://github.com/RuntimeCompiledCPlusPlus/RuntimeCompiledCPlusPlus/blob/005c05145d98b87d974c36fc885003ea88bf3932/Aurora/RuntimeObjectSystem/ISimpleSerializer.h),
   lines 28–54, 93–161: typed copied property values, keyed by property names;
   loading uses `static_cast` to the expected serialized-value type, then assignment.
   Array forms copy bytes. This is not a versioned migration format with checked
   field types. Changing a field's type while reusing its name needs deliberate
   migration, not reliance on that cast. `SimpleSerializer.cpp` lines 91–150 keys
   the property groups by object IDs and invokes each object's own serializer.

Retirement observation: `m_Modules.push_back` at loader line 427 retains modules;
the destructor at lines 75–85 deletes subsystem objects without unloading those
modules. Searching the acquired RuntimeObjectSystem and RuntimeCompiler trees
finds no `dlclose`/`FreeLibrary` path. Constructor-history limits do not change that.
This avoids invalidating old code addresses by unloading, but is not a bounded
long-lived code-generation retirement strategy. It also does not repair dangling
object pointers after replaced-object destruction.

Rollback observation: factory completion restores old constructors/state after
protected failure before the delete-old phase. Restore itself is unprotected and
can fail; deletion-phase failure is treated as a completed swap with leaks.
Constructor/initialization/destructor side effects are not transactions. Object
reconstruction must not duplicate owned provider/process effects.

## RCC++: fault protection and compiler seams

[`RuntimeObjectSystem_PlatformPosix.cpp`](https://github.com/RuntimeCompiledCPlusPlus/RuntimeCompiledCPlusPlus/blob/005c05145d98b87d974c36fc885003ea88bf3932/Aurora/RuntimeObjectSystem/RuntimeObjectSystem_PlatformPosix.cpp),
lines 34–79, 85–167: thread-local current protector, process signal-handler
installation, `setjmp`/`longjmp`, and Apple task exception-port changes. Its
thread-local protector is not a workflow retirement barrier. Signal handlers and
exception-port settings still involve process-wide state. The shown function has
no C++ `catch` around `ProtectedFunc`.

**Inference:** treating this as general recovery from native memory corruption or
as C++ RAII/Lua-error unwinding would be unsound. A caught fault cannot establish
unmodified heap, restored locks, undone external effects, or a recoverable Lua VM.
Design ordinary checked failure separately from process crash containment; consider
an isolated candidate worker if a failed candidate must leave live chat usable.
This inference must be tested against our actual binding and ownership contract,
not by intentionally faulting the reference application now.

[`Compiler_PlatformPosix.cpp`](https://github.com/RuntimeCompiledCPlusPlus/RuntimeCompiledCPlusPlus/blob/005c05145d98b87d974c36fc885003ea88bf3932/Aurora/RuntimeCompiler/Compiler_PlatformPosix.cpp),
lines 140–146, 172–203, 231–314: chooses clang++/g++ unless configured, forks,
pipes stdout/stderr, constructs a shell compile command, and invokes `/bin/sh -c`.
It adds PIC/hidden visibility/shared-module flags and accepts extra options. This
is a separate compile invocation; our C++20, include, standard-library, SDK,
architecture and sanitizer profile must reach runtime compilation too. The owned
operation/audit system should control and capture compilation as an ordinary
process attempt, rather than relying on a framework's log output as the audit.

`Aurora/CMakeLists.txt` has separate RuntimeCompiler/RuntimeObjectSystem targets
and optional examples. Graphical examples pull unrelated graphics/UI machinery;
`.gitmodules` pins a GLFW dependency location. Nothing requires adopting those
examples for an Arconaut qualification. No CMake was run and no present Clang or
macOS arm64 compatibility is claimed.

## fungos/cr: narrow lifecycle contrast

Source: [`cr/cr.h`](https://github.com/fungos/cr/blob/1c3f8302320dee8206cf85d6aeb9e9a9cd78b527/cr/cr.h).

The C entry `cr_main(context, operation)` supplies load/step/unload/close phases;
`userdata` carries explicitly external state. That narrow protocol is useful
inspiration for owned boundaries. Static persistence copies selected state/BSS
sections; section address/size policies do not validate object graphs, pointers,
vtable compatibility, or field meaning. Default mode permits section growth.

Actual loader lines 1738–1809 unload the current plugin before loading/validating
the candidate. Lines 1936–2001 run unload/store/load/rollback hooks; lines 1632–1650
call `dlclose`. There is no reference census for callbacks or Lua closures.
A retained rollback file can reload code later; it does not preserve concurrent
executing old-generation frames. Compilation is external.

POSIX crash support lines 1662–1734 uses one global `sigjmp_buf` and installs
process signal handlers. **Inference:** it is unsuitable as a concurrent per-plugin
fault-containment guarantee without additional isolation. Do not transplant its
signal recovery into an RAII-heavy C++/Lua core. Its macOS branch inspects Mach-O
sections and handles 64-bit headers; that is source support, not arm64 qualification.

The acquired tests `tests/test.cpp` and `tests/test_basic.cpp` check static counters,
heap values through external userdata, version changes, and faults in load/unload/
step. Their reload trigger touches the same binary's timestamp. They are useful
semantic oracles for those behaviors, but not changed-layout migration,
held Lua callbacks, concurrent retirement, or actual recompilation conformance.
They were read, not run.

## Godot and JENOVA contrast

[Godot GDExtensionManager](https://docs.godotengine.org/en/stable/classes/class_gdextensionmanager.html)
documents `reload_extension` as editor-only; release builds fail that call.
This limitation concerns reload, not whether release builds can load extensions.
The engine was not acquired; importing an engine lifecycle solely for loader
convenience requires a separate concrete benefit.

[JENOVA/Sakura's hot-reload documentation](https://jenova-framework.github.io/docs/pages/Advanced/Hot-Reload/)
describes runtime reload enabled explicitly and demonstrated during a debug game
session. Allocated script objects/variables are lost on reload unless explicitly
preserved using cross-reload storage. Retained raw pointers/variables do not establish
callable lifetime or ABI compatibility. Nested extensions require compatible forks.
[Getting started](https://jenova-framework.github.io/docs/pages/Getting-Started/)
documents Windows/Linux 64-bit. A Mac library-name example is not supported-platform
evidence. The [current pinned README](https://github.com/Jenova-Framework/Jenova-Runtime/blob/084e02830d07e6d3dfdb309dc9df9d3a165b9056/ReadMe.md) also names Windows/Linux x64 and says its 0.4.0.0 builder requires Godot 4.7, whereas getting-started documentation still says 4.2+. Preserve that version disagreement rather than selecting the older minimum. These documentation observations neither qualify exported production
reload nor make JENOVA a present macOS arm64 bootstrap choice.

## Contract and bounded qualification needed before commitment

Specify first: who owns candidate and active generations; which ABI crosses the
foundation boundary; who owns state/resources/allocations; whether objects are
isolated per generation or migrated only at quiescence; activation atomicity and
failure outcomes; complete Lua/callback/finalizer/thread roots; bounded retirement;
and escalation to outpost refit for incompatible native interfaces.

Then one owned, reviewable fixture can discriminate candidates. Its independent
oracle checks a counter plus an owned resource identity across changed behavior,
new state fields, rejected migration, and failed compile/load. Suspend a Lua
workflow holding an old callable, stage a candidate, and verify old behavior until
workflow conclusion, new behavior afterward, and no retirement before the last
callback/finalizer can fire. Require observable destruction and bounded loaded
artifacts rather than loader success alone. Exercise separate sanitizer profiles
and the actual macOS arm64 compiler/library/SDK profile, then intended fleet targets.

These qualification subjects are a research/design seam, not permission to skip
review or to execute acquired applications. An owned narrow loader may be preferable
if the required RCC++ integration exceeds the machinery it saves. No dependency
has been selected.

## Ready-to-append quarantine manifest entry

Native replacement grounding — 2026-10-01. Preserve RCC++ and fungos/cr immutable
source ZIPs under `../quarantine_proj/archives/arconaut-native-reload-2026-10-01/`;
clean references under `quarantine/runtime-compiled-cplusplus/` (884 files) and
`quarantine/fungos-cr/` (34 files). Exact sources, revisions, SHA-256, counts and
comparison are in `papers/2026-10-01-native-reload-acquisition.json`; mechanism and
qualification findings in `papers/2026-10-01-native-reload-grounding.md`.
No imported source ran. GLFW submodule content is absent from the RCC++ ZIP.

```text
991f6ee33d2b4fd413df0f87b264c5e2772d41888b74156e9b484a93fa546793  runtime-compiled-cplusplus-005c05145d98.zip
978f1e1717b5e969564a5a9790a17c2412b05284984cad4d1cedd1bb0de3dc3b  fungos-cr-1c3f8302320d.zip
```

Restore from `~/projects/arconaut/`, with destinations absent:

```sh
python3 scripts/ingest_zip.py ../quarantine_proj/archives/arconaut-native-reload-2026-10-01/runtime-compiled-cplusplus-005c05145d98.zip quarantine/runtime-compiled-cplusplus --root RuntimeCompiledCPlusPlus-005c05145d98b87d974c36fc885003ea88bf3932 --skip-symlinks
python3 scripts/ingest_zip.py ../quarantine_proj/archives/arconaut-native-reload-2026-10-01/fungos-cr-1c3f8302320d.zip quarantine/fungos-cr --root cr-1c3f8302320dee8206cf85d6aeb9e9a9cd78b527 --skip-symlinks
```

To reacquire archives, use the catalog's immutable codeload URLs, retaining exact
archive bytes and SHA-256 before extraction. For RCC++, the successful local fetch
used `curl --disable --http1.1 --fail --location`; preserve archives intact and use
the owned importer to omit nested Git metadata, detritus and symbolic links.
The historical instructions inside references remain inert.
