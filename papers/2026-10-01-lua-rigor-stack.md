# Lua rigor for Arconaut's C++ host

2026-10-01. Research and proposed qualification work, not an adopted dependency
set. The operator has selected C++ with Lua scripting. Lua should make tool
definitions, workflows, turn policy and experimental interfaces easy to change;
live native recompilation and outpost refit cover deeper evolution. This report
does not install a runtime, execute reference code or prescribe a scripting framework.

## Recommendation for the first self-development milestone

Use ordinary Lua source and a small owned binding to the native host as the
leading candidate. Propose **PUC Lua 5.5.1**, with **LuaLS annotations and batch
diagnostics** as development tools; retain **5.4.9** as a compatibility fallback
if qualification exposes a concrete blocker. Avoid building an alternate language,
a general package manager or a new property-testing framework before Arconaut can
develop itself. The operator's exclusion of Python from production is compatible
with this proposal.

The official download area lists 5.5.1 dated July 24, 2026, and 5.4.9 dated August
10, 2026. Version numbers and build configuration should be recorded with each
program generation. [Official releases](https://www.lua.org/ftp/).

Read Rhizome's [C++ rigor proposal](../../rhizome/papers/cpp-rigor-stack.md) as seed
evidence: carry over restricted ownership vocabulary, useful compiler/static
diagnostics, separate sanitizer builds, semantic oracles and deterministic fault
simulation. Rhizome's engine-specific allocation and representation requirements
do not automatically become Arconaut requirements. Its stack is partly operational
and partly proposed; this report does not upgrade it to a qualified Arconaut stack.

## Runtime and type choices

| Candidate | Concrete benefit | Cost and disposition |
|---|---|---|
| PUC Lua 5.5.1 | Current reference language and embedding implementation; no extra source transformation. | Leading proposal. Need to qualify our exact C++ build, embedding and diagnostics together. |
| PUC Lua 5.4.9 | Maintained predecessor supported by older tooling. | Fallback for an actual compatibility failure, rather than silently constraining new source to an older dialect. |
| LuaJIT 2.1 | Native JIT and direct FFI can benefit computation-heavy scripting. | Production branch uses rolling releases and Lua 5.1 API/ABI with partial later-language features. It is a different contract, not a faster drop-in 5.5 runtime. Revisit only with a measured Arconaut workload. [Status](https://luajit.org/status.html), [compatibility](https://luajit.org/extensions.html). |
| Lua plus LuaLS annotations | Types, signatures, navigation and diagnostics in executable ordinary Lua. | Best initial ergonomics proposal. Annotations are advisory, not a sound guarantee or runtime enforcement. Current repository advertises Lua 5.5 support. [LuaLS](https://github.com/LuaLS/lua-language-server). |
| Teal | A typed dialect compiles to Lua, retaining a plain-Lua deployment target. | Adds source/generated-source correspondence, a compiler and another language contract. Current compiler README lists Lua 5.1–5.4 and LuaJIT, not 5.5. Reserve for a demonstrated need. [Compiler](https://github.com/teal-language/tl). |
| Luau | Gradual types and a C++ embedding implementation. | A separate Lua-derived language/runtime with intentional compatibility differences. It removes ordinary Lua facilities, including most `io`/`os`/`package`/`debug`, and lacks several later-Lua features. Host APIs could restore needed computing access, but this would be a deliberate runtime choice. [Compatibility](https://luau.org/compatibility/), [types](https://luau.org/types/). |

LuaJIT's FFI has additional reload obligations: callback resources need explicit
lifetimes; implicit callback conversions are permanent, and a collected library
namespace may release the library while a function pointer remains. Those are
unhelpful defaults for pervasive hot replacement. [FFI semantics](https://luajit.org/ext_ffi_semantics.html).

## Embedding contract: the important rigor is at this seam

Documented Lua facts relevant here: coroutines are not OS threads; sibling
coroutines share globals. C embedding normally uses nonlocal jumps for errors
and yields; resumable C calls require the continuation APIs. Finalizers cannot
yield. GC can be controlled, but stopping collection does not stop allocation.
Registry roots retain objects. Text-only loading rejects binary chunks whose
consistency Lua does not validate. Lua 5.5 also supplies explicit global
declarations. [Reference manual](https://www.lua.org/manual/5.5/manual.html).

The exact 5.5.1 implementation uses C++ exceptions when compiled as C++ without
`LUA_USE_LONGJMP`; its protected boundary catches Lua's internal exception and
also catches foreign exceptions. **Propose compiling the selected Lua runtime as
C++ and qualifying this configuration.** A C-built runtime can jump across C++
frames containing RAII owners. Also, a binding's broad `catch (...)` must not
swallow Lua's own control-transfer exception: isolate exception conversion around
native business calls, before subsequent Lua API operations. Yielding still
requires explicit continuations and durable continuation state; it does not retain
a native stack frame. [Actual error machinery](https://www.lua.org/source/5.5/ldo.c.html).

Proposed host rules:

1. Give a VM family one explicit execution owner at a time. Native workers enqueue
   completions for that owner; they do not call Lua concurrently. Parallel scripting
   can use independent VMs and explicit exchanged values. Coroutines express
   orchestration, while the native host owns admission, scheduling and operation custody.
2. Keep provider calls, child processes, service clients and original audit data in
   the native foundation. Lua holds checked operation handles and composes them.
   A handle carries an identity and lifetime, rather than a raw native pointer.
   This mediates observation and lifecycle without adding routine approval steps.
3. Define every binding's stack inputs/results, exact accepted value shapes,
   failure status, yieldability, ownership transfer and callback lifetime. Check
   these at the real boundary. Keep asynchronous buffers in native owners; copy
   Lua byte strings before their native use outlives the call. The conversion API
   can accept numeric strings and strings containing NUL, so incidental conversion
   is not a schema. [Conversion rules](https://lua.org/manual/5.4/manual.html#lua_tolstring).
4. Treat finalization as local cleanup, with explicit settlement/cancellation for
   live operations. Collection of a scripting handle must not invent successful
   command cancellation, stop a shared service or discharge an unknown outcome.
5. Keep error payloads, source/program version and traceback when available. Lua
   error objects need not be strings, and allocation failures skip normal message
   handling. Reserve a native minimal failure path that does not need another large
   Lua allocation. [Error handling](https://www.lua.org/manual/5.5/manual.html#2.3).
6. Model cancellation as host-owned state. Admission checks prevent further effects
   after cancellation even if a Lua program catches errors. Count hooks can help
   reach a cooperative scheduling boundary; blocking native calls need their own
   cancellation mechanism. Do not advertise a Lua hook as universal preemption.

This initial discipline does not forbid an expert from extending computing access.
It requires deliberate attribution when direct native/OS access bypasses mediated
capture; “audit everything” cannot imply observing arbitrary internals automatically.

## Hot replacement: definitions are ephemeral; operation identity is not

Propose loading a candidate under a new program identity with its own environment
and module cache, validating its exports and exercising its relevant direct tests.
Prefer modules whose initialization returns declarations and closures; effectful
workflow execution begins through the host after publication. A failed candidate
does not replace the current registry. In-process environments are organization
boundaries, not a proof against arbitrary hostile initialization or external effects.

Publish the new generation after the current turn and affected workflows finish,
as the operator chose. Current invocations retain their closures, imports, callbacks
and native component generation through conclusion. An explicit interrupt-and-apply
settles the affected work before publication. Editing a file or overwriting a shared
`package.loaded` entry cannot establish that boundary.

Standing workflows may pin an old generation indefinitely. Deliberately ending one
is a separate operator/model action, not a refit preservation strategy. Refit must
pause and preserve its live execution in a surviving worker or use an explicitly
supported checkpoint/state handoff; never silently transplant a suspended coroutine
into new code. Keep continuation data
inspectable and versioned. Such workflows remain Arconaut's own work; shared
databases/kernels remain consumed services under their independent governance.

Native libraries need generation custody too. A Lua callback, userdata finalizer or
closure can still invoke an old library even after its registry entry is gone. Pin
native code until its last callable/object/thread obligation is settled. Changes to
the stable foundation still use the outpost refit protocol. Lua heap snapshots and
source edits alone cannot preserve a live VM across foundation replacement.

## Development feedback and direct oracles

Propose one LuaLS diagnostic lane for the selected source language, with explicit
host API declarations and version configuration. Check undefined globals, nil
handling, argument/return types and missing fields where useful. LuaLS supports
`--check` with report output; its diagnosis documentation specifies `check.json`.
Inspect the report for required diagnostics rather than assuming an exit code proves
the workspace clean. [CLI](https://luals.github.io/wiki/usage/),
[report](https://luals.github.io/wiki/diagnosis-report/),
[settings](https://luals.github.io/wiki/settings/).

Use the real selected runtime to compile source before activation. LuaLS does not
replace the parser or binding checks. Luacheck currently lists syntax support only
through 5.4; adding it alongside LuaLS must buy a distinct fault class and qualify
the chosen dialect. [Luacheck](https://github.com/lunarmodules/luacheck).

| Failure class | Direct oracle for our host/scripts |
|---|---|
| Wrong scheduling or program version | A small independent state model consumes generated sequences of start/yield/complete/reload/interrupt events; compare actual dispatches, versions and results. Include overlapping workflows and pending changes. |
| Binding shape or conversion faults | Exact expected values/errors for wrong types, embedded NUL, nonfinite floats, integer limits, nil versus absence, cyclic tables and stale handles. Verify the dispatch received precisely the admitted bytes/arguments. |
| Error/unwind leaks | Native destructor and ownership counters observed across Lua error, host exception, allocation failure, yield and cancellation; assert exact ownership balance and disposition of any dispatched effect. |
| Premature code retirement | Old-generation closure, callback and finalizer fixtures exercised during replacement; assert actual callable version and reclamation only after the last obligation concludes. |
| GC/lifetime bugs | Force collection between ownership transitions, retain/release roots deliberately, complete native work after scripting handles disappear, and check concrete values and resource counts. |
| Duplicate or unattributed effects | Fake providers/process seams independently count dispatches and supply known streams; compare actual effects and original capture, including interrupted/failed attempts. A self-consistent log alone is insufficient. |
| Native memory, UB or concurrency faults | ASan plus selected UBSan on the actual host and Lua runtime; separate TSan build for worker completion queues, admission and retirement. Strong semantic checks remain in these runs. |

LLVM's tools support coverage-guided fuzzing and these instrumentation families.
Use a native libFuzzer entry point that constructs bounded binding values and
operation schedules, asserts the semantic oracle and surfaces sanitizer failures.
Instrument the VM and bindings together where feasible. Do not equate fuzzing
arbitrary Lua text without a behavioral oracle with workflow verification.
[libFuzzer](https://llvm.org/docs/LibFuzzer.html),
[ASan](https://clang.llvm.org/docs/AddressSanitizer.html),
[UBSan](https://clang.llvm.org/docs/UndefinedBehaviorSanitizer.html),
[TSan](https://clang.llvm.org/docs/ThreadSanitizer.html).

Start pure-script tests with an owned small runner and seeded domain-specific case
generators; use the same host operation seam for model tests and live execution.
That is a proposal for a narrow harness, not permission to reinvent every testing
library. Busted is a useful reference for error-raising tests; Lua-QuickCheck supplies
generators, shrinking and state-machine testing, but their exact 5.5 compatibility
and adoption value remain unqualified. [Busted](https://github.com/lunarmodules/busted),
[Lua-QuickCheck](https://github.com/luc-tielen/lua-quickcheck).

The upstream release-matched Lua suite can check our unusual compiler/build
configuration once runtime adoption and execution are authorized. It cannot establish
Arconaut lifecycle semantics. Mutation work stays on the fleet; choose actual
activation, stale-handle and unwind faults rather than superficial code edits.
[Official test suites](https://www.lua.org/tests/).

## What needs qualification next

No runtime/tool combination in this report was installed or executed. Documented
availability is distinct from a working host integration. The near-term feasibility
work should settle four concrete questions alongside the bootstrap design:

- Can the pinned C++-built Lua 5.5.1 and selected native reload mechanism share the
  intended exception/ABI contract on this Mac, and retain old callable generations?
- Does the chosen LuaLS build diagnose our small annotated host API and the actual
  5.5 source constructs correctly in batch mode?
- Can our owned seam handle error, yield, cancellation and allocation failure while
  preserving exact dispatch/capture and ownership outcomes?
- What is the smallest explicit standing-work checkpoint protocol needed for the
  first outpost refit, rather than promising general VM or coroutine migration?

The useful first stack is ordinary Lua plus good host contracts and direct oracles.
More dialects, runners and compiler machinery can earn their place from defects or
ergonomic limits encountered while building Arconaut inside itself.

Subsequent [embedding source grounding](2026-10-01-lua-embedding-grounding.md)
acquires official Lua, matching tests and one binding reference, and traces the
actual unwind/linkage/continuation/finalizer mechanisms. Source acquisition and
read tests do not establish that the proposed runtime configuration works locally.
