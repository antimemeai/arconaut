# U0: executable foundation

2026-10-01. First product unit of the reviewed IMPLEMENTATION_PLAN, implementing
CORE_DESIGN's identity/time boundary and TESTING_PLAN T0. The qualified local
LLVM 23.1.2/C++20 profile is reused; no new library, Lua/provider dependency,
filesystem durability claim or effect-admission engine is introduced here.

## Purpose and API boundary

The custodian and participants need values that cannot silently mix identity,
clock or ownership domains. Build a small owned `arconaut_foundation` target and
one direct-test executable. Keep mechanism vocabulary small; this is not a
generic framework. APIs live under `include/arconaut/foundation.hpp` and
`src/foundation.cpp`; direct cases are `tests/foundation_test.cpp`.

IDs are opaque, nonzero 128-bit values with distinct C++ tag types for environment,
participant, conversation, workflow, invocation, attempt, process incarnation,
context revision, definition generation, audit stream, registry and clock. Checked
construction preserves the exact supplied bytes and rejects zero. They are not
PID/FD aliases. U0 does not claim to mint globally unique IDs: the custodian's
retained issuer in U1 must assign fresh identities and reject reuse; an operator
supplying identical bytes is supplying the same identity, not a new incarnation.
Audit sequence is a separate checked nonzero uint64 value with checked advance.

An ephemeral typed handle carries environment, process incarnation, registry
identity, slot and generation. A single-owner bounded registry validates that
whole domain before exposing a valid slot. Release invalidates the generation
before reuse; exhausted generations permanently retire their slot. Release validates
the complete handle first; stale/foreign release leaves the current occupant unchanged.
Registry identity is caller-supplied and freshness is the custodian issuer's U1
obligation, including within the same process. Supplying the same domain twice means
reusing one identity, not constructing two independently owned domains. U0 validates
domains; it does not claim to detect forged caller identity reuse.
Generation advances exactly once on release; UINT64_MAX retires to the unusable
zero state. This implementation uses heap-backed storage, without inline-buffer
optimization; allocation-failure cases target its actual allocation calls.
No payload/OS resource or concurrent dereference is owned by this unit. Bounds
and allocation failure return explicit errors. An explicit nonzero initial
generation permits constructing a registry at a retained generation boundary;
it does not excuse domain reuse. Tests exercise UINT64_MAX without billions of
iterations. Copying a registry is forbidden; a move transfers its identity/state.

`Result<T>` is nodiscard and holds either a value or a small allocation-free
error code/detail. Invalid value/error accessor use is a programming error;
fallible APIs return errors, not silent fallback. Error text uses fixed literals.
An owned byte buffer checks its configured size limit before copying and converts
allocation failure. Borrowed spans retain exact binary bytes and require a living
owner; checked slices reject offset/length overflow using subtraction. Obtaining
a view from a temporary owned buffer is prohibited. Async retention is later
operation-owned custody, not a property inferred from a span.

Monotonic instants carry environment/process/clock domain and signed 64-bit
nanoseconds. Durations are nonnegative uint64 nanoseconds. Elapsed arithmetic
rejects unequal domains and backward time; full-range valid differences are
allowed. Deadline construction rejects overflow and retains start/due. Observation
before its start rejects clock regression; due-at-equality and zero duration are
specified: equality returns true and zero duration is due at its start. Elapsed
arithmetic uses checked unsigned differences, never overflowing signed subtraction.
Wall observations are separately typed signed Unix nanoseconds with
their origin domain, never an elapsed/deadline input. Changing wall time cannot
alter monotonic arithmetic. Pause does not freeze this clock.

A `ClockSource` returns both wall and monotonic observations with matching origin.
The native clock uses system_clock/steady_clock under the qualified libc++ profile,
checks conversion bounds and statically asserts its supported period/representation
in every build. It makes
no atomic-simultaneous or cross-process comparison claim. Injected storage exposes
bounded read/write-at, extent and explicit data/full synchronization requests;
backend support and directory publication are U1 obligations. Injected effects
carry resolved identity and exact bytes. Their adapter result describes local
dispatch observation, never proof of no external effect on error. These seams do
not bypass the future retained admission/epoch checks or invent deduplication.

## Direct red and green cases

| Claim | Concrete expected outcome |
| --- | --- |
| Type separation | Participant/conversation and wall/monotonic implicit mixing fail compilation; temporary-owned-buffer view fails a deleted-function diagnostic; discarded Result fails the required warning profile |
| ID fidelity | A specified 16-byte identity is preserved exactly; all-zero ID and sequence zero reject |
| Sequence bounds | UINT64_MAX advance rejects rather than wrapping; ordinary advance returns exact next sequence |
| Handle custody | A live slot validates; wrong environment/incarnation/registry reject distinctly; two registries with distinct caller identities reject each other's handles; stale release cannot invalidate a reused live slot |
| Registry bounds | Zero capacity rejects, configured exhaustion returns capacity error; release at generation maximum retires the slot; moving transfers state and invalidates the source registry |
| Bytes | NUL/0xff/CR/LF copy exactly; source modification leaves the owned copy unchanged; legal empty/end slices succeed; oversized/overflow-shaped slices and configured allocation limits reject |
| Time | Named pairs yield exact durations/deadlines; foreign/restarted clock rejects; backward observation rejects; INT64_MIN to INT64_MAX elapsed is UINT64_MAX; impossible addition rejects |
| Wall separation | Injected wall jumps backward/forward while monotonic advances 25ns; elapsed remains 25ns and deadline status follows only monotonic values |
| Native clock | Its returned domain is exact; monotonic samples are bracketed by independent steady_clock reads, including after real work; wall samples are bracketed independently (a concurrent wall adjustment is reported as inconclusive, not hidden) |
| Injectable seams | Scripted short read/write, interrupt/sync/dispatch uncertainty return exact values/errors without hidden retries; fake effect observes exact identity/input and count |

First write the direct cases and observe the missing production foundation header/
target as red. Implement, then run debug and optimized plus ASan/UBSan and separate
TSan for actual concurrent reads of immutable owned bytes/IDs, not concurrent registry
mutation. Add bounded generated legal/illegal slice and registry reuse histories with
specification-derived expected state; do not derive expectations from the registry.
Compiler-negative API cases use real target flags and required diagnostic classes.
Native time assertions do not assume a timer advances within a fixed duration.

## Review and remaining units

Kimi independently attacks the sub-plan/cases, then actual code and oracles in a
fresh review, with explicit-session rereview of integrated findings. Jev may flag
semantic mismatch in proposed cases; owner reasoning and execution supply acceptance.
Record source grounding, red/green results and limitations in journal/papers/beads.
Only then may U1 depend on this foundation. Operator explicitly postponed mutation
campaigns in favor of working Arconaut; no mutation is a prerequisite for this
unit or the current U0–U9 path. No mutants here.

Grounding: the current CORE_DESIGN/TESTING_PLAN contracts, the read C++ rigor seed
and Rhizome owner/domain proposals, and archived source counterexamples: bare wall
timestamps in core context/message and shell timeout logic in persistent_shell.
Their useful concepts are re-expressed; no quarantined source is translated/imported.
