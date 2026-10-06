# EDG compiler: independent language checking, not a replacement native toolchain

2026-10-01. Bounded primary-source study at revision
`89e67e07c7f6d0fc38622f9778d8e79f40deaa0f`. The source, build configuration,
testing documentation and selected implementation paths were read. No imported
build, developer initializer, test, container or compiler binary was executed.
Acquisition does not select a dependency.

EDG is a strong candidate for a separate C++ language-qualification lane. It
offers an independently implemented parser, lookup/template machinery, constant
evaluation and diagnostics, with inspectable implementation and a substantial
public test corpus. It does not supply a replacement for our native Clang code
generator, linker, sanitizers, debugger, platform SDK or hot-reload ABI checks.

## Opening date and source identity

The [first-party transition announcement](https://edgcpp.org/announcement/)
states that source became public on **September 30, 2026**. Relative to this
workspace's October 1 date, that is yesterday. GitHub's repository API reports
creation on September 22; repository creation is not its public-opening date.
The pinned HEAD has a September 30 commit timestamp. The release API returned no
tagged GitHub releases at acquisition time. These observations do not establish
an independently released binary or a tested stable distribution.

Repository/head/release API responses are retained alongside the
[acquisition catalog](2026-10-01-edg-compiler-acquisition.json). The public code
history includes older EDG work, rather than beginning at the public-opening
date. Professional history does not yet establish the activity or responsiveness
of the new public contributor community.

## What the actual machinery provides

The front end consumes C/C++ and produces a high-level intermediate language,
making implicit source operations explicit. It does not perform an optimizing
native backend's work. The repository includes two source-generating backends,
IL read/write/display tools, a template prelinker, demangler, runtime support and
development utilities. The C++ backend is designed for source transformations;
its output expands preprocessing and drops comments, so it is not a source-text
preserving rewrite tool. See the pinned
[internal overview](https://github.com/edgcpp/compiler/blob/89e67e07c7f6d0fc38622f9778d8e79f40deaa0f/doc/source/int_overview.rst),
[C backend](https://github.com/edgcpp/compiler/blob/89e67e07c7f6d0fc38622f9778d8e79f40deaa0f/doc/source/c_gen_be.rst) and
[C++ backend](https://github.com/edgcpp/compiler/blob/89e67e07c7f6d0fc38622f9778d8e79f40deaa0f/doc/source/cp_gen_be.rst).

`src/cmd_line.c` registers `--c++20`, `--c++23`, `--c++26`, compatibility modes
including version-selected Clang emulation, and `--no_code_gen`/`-n`. The latter
suppresses backend execution. Mode flags are real implemented controls, not a
claim that every standard/library/platform facility is supported. The repository's
own build uses **C++14**; that is the implementation language used to build EDG,
not the maximum language version EDG can inspect.

There is a particularly relevant backend distinction: `src/host_envir.h` around
line 3880 says coroutine lowering is incomplete and the C backend cannot handle
coroutines, whereas the C++ backend can. The default `cpfe` configuration turns
on C lowering. `cpfe-cp` selects the C++ backend. Consequently a coroutine-aware
independent checking experiment should qualify **the non-lowering `cpfe-cp`
configuration**, rather than treating a rejection by default `cpfe` as evidence
that our source is invalid. Neither path proves native coroutine execution or
the C++/Lua lifetime contracts.

Arconaut could eventually use the semantic representation for analysis or model
assistance. Embedding EDG or using it for an autoresearch transformation tool is
a separate architectural/library decision, with a large implementation surface
and source/IL coupling. It has no demonstrated role in making a live shared
library safe to unload or in preserving Lua continuations through native reload.

## Apple arm64 and current qualification gaps

The pinned [BUILD.md](https://github.com/edgcpp/compiler/blob/89e67e07c7f6d0fc38622f9778d8e79f40deaa0f/BUILD.md)
and `CMakePresets.json` explicitly provide macOS arm64 Clang debug/release
configurations. Their existence resolves the question of whether upstream intends
an Apple arm64 host build; it does not qualify our compiler/SDK/runtime combination.

The target configuration specifies little endian, 64-bit pointers/long, and
8-byte long double for Apple arm64. The native base can scrape system include
directories and Clang's version, select compatibility mode and link generated
objects through Clang/libc++/System. That is evidence of platform adaptation,
not a guarantee of compatibility with the current Apple SDK's headers/builtins
or every C++23 library facility. Release CMake configuration requests both
arm64 and x86_64; a qualification experiment should deliberately choose its
architecture, rather than inherit a surprising universal build.

[HACKING.md](https://github.com/edgcpp/compiler/blob/89e67e07c7f6d0fc38622f9778d8e79f40deaa0f/HACKING.md)
identifies Linux x86_64 as the best supported developer environment. Its macOS
section documents a native procedure and warns about stable test recordings
with Apple arm64/Clang/libc++. The inspected build-and-test workflow exercises
Linux GCC x86_64/i686 with both source backends; it is not Apple qualification.
Native Windows test infrastructure still depends on Bash.

Official development tooling requires Python and CMake; the driver/testing
tools also use Bash. These would be development tools, not Python in Arconaut
production. Initial native CMake defaults also expect an `EDG_BASE` runtime
configuration. A frontend-only build can be investigated independently of the
runtime/source-to-C path; that has not been built here. EDG's own warning and
ASan settings do not become our rigor profile merely by being read.

## Tests: availability and oracle boundaries

The intact source archive has **45,447 single-/multi-file test entry files**
with `.sft.c`, `.sft.cpp`, `.mft.c` or `.mft.cpp` suffixes. This is a file count,
not the number of test invocations, unique language properties, passing cases or
independent EDG-authored tests. The archive contains imported Clang/GNU suites,
source/support files, recorded outputs and configuration variants. Before any
deduplication, `tests/tests/imported/` accounts for 95,755 entries; EDG-named,
changes, core-working-group, modules, reflections and library suites also exist.

The [testing framework documentation](https://github.com/edgcpp/compiler/blob/89e67e07c7f6d0fc38622f9778d8e79f40deaa0f/doc/source/testing.rst)
defines positive/negative frontend, compile, link and runtime outcomes, optional
required-output regular expressions and captured-output comparisons. Some tests
encode direct semantic expectations, such as valid access versus required
diagnostics. Much of the broad suite also depends on prior recorded output.
This is valuable compiler-regression evidence, with the ordinary limits of a
regression oracle. It does not supply behavioral oracles for our operations,
context revisions, audit durability or refit authority.

`edgy` and `edg-run-test` share Python test machinery. Tests can be filtered by
suite/directory/individual case and run in configuration variants. The first-test
tutorial includes distinct positive/negative cases and manual review before
recording output. The repository's `--no-expectations`, recording and
crash-avoidance options must not be confused with conformance evidence. No suite
was run in this study, and imported tests are not automatically adopted here.

## Proposed use in the rigor stack

Keep the existing Clang native build/sanitizer/runtime lane operational. Add EDG
only as an explicit independently qualified **language-checking lane**:

1. Build a pinned standalone frontend in a disposable development checkout,
   outside the read-only reference. Qualify architecture, configuration and
   exact compiler identity without running the entire imported test suite.
2. Use a small owned dialect fixture set with specification-derived pass/fail
   expectations: templates/concepts/constexpr and the exception/linkage/ABI
   declarations that our core actually uses; include coroutines if adopted.
   Distinguish an unsupported configuration from invalid source.
3. Prove a known invalid fixture is rejected with the expected diagnostic
   class, not merely any nonzero exit status. Prove valid fixtures are accepted
   under the intended standard. Keep vendor-compatibility and strict-standard
   results distinct; compatibility emulation can deliberately accept extensions.
4. Replay selected real translation units with explicit source include paths,
   defines, system header strategy and language version. A command adapter must
   translate native flags deliberately, not pass a Clang compilation database
   straight to EDG and report failure as a product defect.
5. Investigate compiler disagreement as evidence to resolve against the
   applicable language contract. Compiler majority voting cannot adjudicate
   correctness, and acceptance does not establish runtime safety.

This can improve independence from a Clang-only parser/static-analysis stack.
It should not hold up establishment of the first owned native unit. The same
lane is transferable to Rhizome if it selects C++; this report makes no change
to Rhizome's decisions or files.

## License and adoption boundary

The [top-level license](https://github.com/edgcpp/compiler/blob/89e67e07c7f6d0fc38622f9778d8e79f40deaa0f/LICENSE.txt)
is Apache 2.0 with LLVM exceptions, matching the source SPDX declarations.
The tree explicitly carries separately licensed third-party material:
`tests/tests/imported/gnu/LICENSE.txt` is GPLv3, and the imported Clang subtree
has its own LLVM license. Therefore the whole source/test archive must not be
described as uniformly licensed merely by reading its root license. This is
license identification for the study, not legal advice or authorization to ship
compiler/test source. A development-tool adoption and any embedding would need
their own concrete scope decision.

## Acquisition and restoration

The original immutable source ZIP is preserved at
`../quarantine_proj/archives/arconaut-edg-compiler-2026-10-01/edg-compiler-89e67e07c7f6.zip`
(244,854,866 bytes). SHA-256:

```text
66df655bf9a518932787e265d04b680f3180bd7ee36cc2da15528352029e0947
```

The clean reference is `quarantine/edg-compiler/`: **111,049 regular files plus
12 symlinks**. Initial owned ingestion compared retained file sets, bytes and
executable bits directly with the ZIP. Its generic `._`-prefix and symlink
exclusion omitted valid data: 46 compiler expectation filenames derived from
identifiers such as `_Pragma`, plus 12 functional links. At the root owner's
direction those were restored directly from the same archive, with file bytes,
executable bits and every symlink's target string compared to archive entries.
Every link resolves within the reference root. Three links deliberately target
`Local_file.c`, which `edgtest.checkout_at_directory` creates only in the test's
execution checkout. No unsafe link or detritus was retained; no source archive
or shared ingestion policy was changed. Full override paths/roles are in the
catalog. The snapshot is study material, not a locally qualified runnable build.

Restore using the commands in [QUARANTINE.md](../QUARANTINE.md#edg-compiler--2026-10-01),
including the catalog-driven fixture override after generic ingestion. Sources,
archive identity, exact revision, initial omissions, override and counts remain
explicit so a future study does not mistake legitimate compiler test names for
filesystem detritus.
