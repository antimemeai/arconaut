# C++/Lua embedding grounding

2026-10-01. One foundation research gap: lifetime, error/yield control transfer,
and program/native-generation boundaries. This extends the
[Lua rigor proposal](2026-10-01-lua-rigor-stack.md) with acquired source and actual
release-matched tests. No acquired program, build, installer or test ran. Lua
5.5.1 remains a runtime proposal; sol2 is a mechanism reference, not an adoption.

**Enough evidence to begin specifying this seam; not enough to call an embedding
or reload stack qualified.** The source supplies concrete constraints and useful
oracles. Compiler/ABI interoperability and real host behavior still need bounded
experiments after the reviewed design and experiment plan.

## Acquired sources

| Reference | Exact identity | Study scope |
| --- | --- | --- |
| Official Lua | 5.5.1, published 2026-07-24 | Protected calls, C++ error transfer, resumable C calls, allocation, registry roots, finalization and shutdown. |
| Official tests | Exact 5.5.1 suite, same release date | Actual assertions for continuation results/status/context, coroutine close, finalization and injected allocator failures. |
| sol2 | `c1f95a773c6f8f4fde8ca3efe872e7286afe4444` | One C++ binding reference: protected invocation, exception trampoline, registry-reference ownership and separate coroutine stacks. |

[Official release index](https://www.lua.org/ftp/),
[official test instructions/index](https://www.lua.org/tests/),
[sol2 exact source](https://github.com/ThePhD/sol2/tree/c1f95a773c6f8f4fde8ca3efe872e7286afe4444).
The [acquisition catalog](2026-10-01-lua-embedding-acquisition.json) contains the
URLs, SHA-256 identities, roots, direct comparisons and destinations. Lua's two
original tarballs match the publisher's checksums and remain intact. Derived
ZIPs permit the existing owned importer; retained sets, bytes and executable bits
also match the original tarballs directly. Clean extractions contain 75, 42 and
669 regular files respectively, with no omitted entries. Archived instructions
are reference material only.

## What actual control transfer requires

[ldo.c](../quarantine/lua-5.5.1/src/ldo.c), lines 69–171, implements three modes:
C++ exceptions by default under C++ compilation unless `LUA_USE_LONGJMP`, POSIX
nonlocal jumps, otherwise standard nonlocal jumps. In C++ mode Lua throws its
private `lua_longjmp*` for both errors and ordinary coroutine yield. The protected
boundary catches the matching pointer, rethrows mismatched outer levels, and
catches foreign exceptions by setting an internal `-1` status. That does **not**
preserve native exception details as a structured Lua error. Status storage is an
unsigned byte ([llimits.h](../quarantine/lua-5.5.1/src/llimits.h):43–50).

Design consequences:

- A C-built runtime can nonlocally jump across native RAII owners. C++ compilation
  is a plausible way to obtain native unwinding, requiring exact qualification.
- A broad binding `catch (...)` surrounding Lua API calls can intercept Lua's own
  yield/error transfer. Native failure conversion must surround the native
  business call only; later Lua error/result operations occur outside it.
- A `noexcept` callback or destructor must not let a Lua API call's control
  transfer escape. Conversely, assuming every API operation is nonthrowing
  because invocation eventually uses `lua_pcall` is wrong: preparation can allocate
  before that protected call. Put allocating preparation behind an appropriate
  protected shim, and give low-memory failure capture a native path.
- C++ compilation and symbol linkage are separate decisions. The actual
  [lua.hpp](../quarantine/lua-5.5.1/src/lua.hpp) wraps declarations in `extern "C"`,
  while the ordinary headers deliberately do not. A C++-compiled runtime and host
  using those wrappers need consistent exported linkage; merely changing the
  source compiler can break linking across host/plugins.

[sol2's trampoline](../quarantine/sol2/include/sol/trampoline.hpp):103–180 makes
exception conversion and optional catch-all behavior explicit. Its
[exception documentation](../quarantine/sol2/documentation/source/exceptions.rst):53–66
has a safe-propagation configuration. The
[exception example](../quarantine/sol2/examples/source/exception_handler.cpp)
returns a protected result checked for failure. These are useful patterns;
configuration names and broad catches are not contracts to copy uncritically into
our different Lua build. No sol2/Lua-5.5.1 compatibility claim was tested.

## Yield is a continuation boundary, not a saved native frame

Actual [lua_yieldk](../quarantine/lua-5.5.1/src/ldo.c):1008–1037 stores a native
continuation function and `lua_KContext`, then transfers control. On resume,
[finishCcall](../quarantine/lua-5.5.1/src/ldo.c):839–862 calls that stored function;
[lapi.c](../quarantine/lua-5.5.1/src/lapi.c):1093–1114 records the equivalent
protected-call continuation. Yield across a nonyieldable C boundary fails.

Continuation state therefore belongs in rooted/owned durable storage or an
identified native operation, never a pointer to a vanished stack local. Root the
coroutine, distinguish yielded results from resumed inputs and final results, and
preserve the callback's native generation until completion or explicit closure.
Native workers enqueue completions; one owner resumes the VM. This is an Arconaut
scheduling proposal, not a feature supplied by Lua's default locking macros.

The sol2 [multiple-stack example](../quarantine/sol2/examples/source/coroutine_multiple_stacks.cpp)
creates distinct coroutine stacks in one state. They do not establish independent
heaps or native-thread safety. A suspended coroutine cannot be transplanted into
a freshly compiled foundation merely by serializing Lua tables.

## Roots and deferred cleanup are part of native retirement

[restartcollection and traversal](../quarantine/lua-5.5.1/src/lgc.c):440–448,
631–711 mark registry, reachable stacks, metatables and closure/upvalue references.
[lua_newthread](../quarantine/lua-5.5.1/src/lstate.c):278–300 initially roots the
new thread on its parent's stack; storing only its native pointer after popping
that value does not retain it. [luaL_ref/unref](../quarantine/lua-5.5.1/src/lauxlib.c):702–739
provide table references and recycle integer slots. A registry reference number
is VM-local and reusable, so it is not a globally durable operation identity.
Sol2's [reference owner](../quarantine/sol2/include/sol/reference.hpp):632–636
unrefs on destruction: this also means reference owners must die before their VM.

[lua_pushcclosure](../quarantine/lua-5.5.1/src/lapi.c):609–635 stores the native
function pointer directly. GC traces its Lua upvalues, not the operating system's
loaded library or an external callback queue. A closure, continuation, userdata
method/finalizer, registered hook or external callback can still call old native
code after publication removes its registry entry. An owned stable trampoline
with generation/operation handles is a promising boundary; direct plugin callbacks
require explicit generation custody too.

[GCTM](../quarantine/lua-5.5.1/src/lgc.c):968–993 invokes finalizers without yielding,
disables hooks during them, protects the call, and reports errors through warnings.
[VM shutdown](../quarantine/lua-5.5.1/src/lgc.c):1529–1540 runs all pending finalizers.
[Coroutine closure](../quarantine/lua-5.5.1/src/lstate.c):314–340 executes protected
close handling and returns a status. Cleanup may run code and fail; closing a VM
is not necessarily a passive memory-release step.

Arconaut should keep finalizers local and nonblocking, explicitly settle live
operations, capture warnings, and block new effect admission while interrupt/refit
settlement runs. Shutdown cleanup does not prove a provider request or child process
was cancelled. Preserve callback code through finalizer/close execution; schedule
library retirement on the stable owner **after** returning from old code. Releasing
the last code pin inside that library's own finalizer is not a safe unload point.
Finalizer resurrection also defeats naive one-collection retirement assumptions.

## Real tests that inform our oracles

The official suite's mechanisms are useful study inputs, not a substitute for an
Arconaut contract. Its internal C API assertions require the testC debug build;
basic `_U` runs deliberately omit them. We have acquired the matching suite but
have not run either configuration.

| Source read | Actual upstream check | Arconaut oracle to specify |
| --- | --- | --- |
| [coroutine.lua](../quarantine/lua-5.5.1-tests/coroutine.lua):1053–1243 | Exact status/context, yielded/resumed values, preserved upvalues, repeated continuations and error recovery; unreachable instructions after yield. | Independent expected event/result sequence, exact generation and ownership counts across native error, yield, resume and cancellation. |
| [api.lua](../quarantine/lua-5.5.1-tests/api.lua):1293–1350 | Failed close, VM shutdown without allocations, GC with allocations denied and preserved concrete stack/string-table observations. | Fail selected allocation points in real bindings; assert owned resources, original effect capture and known/unknown effect outcomes. |
| [gc.lua](../quarantine/lua-5.5.1-tests/gc.lua):650–692 | Shutdown finalizers, resurrection, created objects and ordered warning observations. | Retain an old callable/finalizer through replacement, observe its actual version, and retire native code only after all callable and cleanup obligations settle. |

Force GC between ownership transitions; retain/release registry roots; deliver a
native completion after a scripting handle disappears. Assert exact operation
identity and dispatch/capture, rather than merely no crash. Run the real VM/binding
seam under the selected sanitizer configurations once authorized qualification
begins; native destructor counters and independent effect seams supply semantic
oracles in addition to sanitizer fault detection.

## Design readiness and remaining qualification

This seam is sufficiently grounded for design: define protected native entry,
strict value conversion, owner thread, operation identity, coroutine rooting,
continuation storage, generation pinning and explicit publication/settlement.
CLM context programs use these same mechanics; editable context revisions are
values, not mutable pointers into retired code.

Before production implementation depends on the choice, a reviewed qualification
experiment must demonstrate the selected C++/Lua linkage and error configuration
on this host; correct unwinding and low-memory failure behavior; generation
retention for yielded/finalizer/native callbacks; and the selected reload
mechanism's actual ABI/lifetime behavior. Refitting preserved standing work needs
a surviving VM/worker or an explicit checkpoint protocol. This study does not
qualify arbitrary coroutine migration, process continuity, sol2, or any reload
framework, and it does not select libraries.

## Restoration

From the Arconaut root, with destinations absent and preserved archives available:

```sh
python3 - <<'PY'
from pathlib import Path
import hashlib, json, subprocess
catalog = json.loads(Path('papers/2026-10-01-lua-embedding-acquisition.json').read_text())
for record in catalog['references']:
    archive = Path(record['archive'])
    assert hashlib.sha256(archive.read_bytes()).hexdigest() == record['sha256']
    subprocess.run(['python3', 'scripts/ingest_zip.py', str(archive),
                    record['destination'], '--root', record['archive_root'],
                    '--skip-symlinks'], check=True)
PY
```

Original Lua tarball URLs/checksums are in the catalog; sol2's codeload URL pins
its full revision. To reacquire originals, download those exact URLs without
executing their contents and verify the catalog SHA-256. The Lua ingest ZIPs are
derived copies of the original tarball regular members with names and modes
preserved; the original tarballs remain the authoritative source archives.
