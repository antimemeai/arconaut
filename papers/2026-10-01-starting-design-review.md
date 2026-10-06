# Starting design: focused adversarial review

2026-10-01. Scope: `docs/STARTING_DESIGN.md` and the C++ rigor seed against
FOUNDATION, BEHAVIOR, workspace/project instructions and BLACKBIRD. This reviews
a discussion sketch, not a complete specification or permission to write product
code. Rhizome's referenced rigor proposal and tooling notes were read as evidence,
without changing that repository or executing acquired code.

**Judgment:** sufficient for discussion. The sketch preserves ordinary model agency,
the chosen C++/Lua division, deferred activation, explicit interruption/settlement,
independent outpost recovery, preservation rather than restart of standing work,
and the consumer boundary for shared services. The rigor seed carries useful
ownership/oracle machinery without importing Rhizome's engine scope or claiming
that version queries qualify a build. No contradiction requires restarting the
sketch. Two small obligations identified below have been integrated and reread;
both findings are resolved at sketch scope. This is not approval of an unwritten
specification or implementation plan. Line references below describe the initial
reviewed sketch, before the clarifying additions.

## 1. Resolved: preserving a Lua VM also requires its native dependencies

STARTING_DESIGN lines 122–136 propose a preservable worker for a standing Lua stack;
lines 69–80 correctly retain complete generations and check handles. A worker's
Lua closures, userdata, C callbacks, destructors and outstanding bindings must not
depend on native objects or callable addresses in the main executable that refit
replaces. Keeping the VM/process alive alone cannot establish that preservation.
Keeping a code generation loaded also does not keep every referenced resource alive.

Required clarification: each preserved worker owns the native code/state its live
Lua execution requires, or reaches surviving custodians/services through a compatible
operation protocol. References into the retiring main process must be absent or
explicitly transferred/rebound under a supported boundary before refit. This does
not select separate processes for every Lua workflow or require serializing stacks.

For the later real refit exercise, resume a paused workflow with actual native
userdata/callbacks and a retained operation handle, rather than checking only a Lua
scalar. Check correct behavior and resource identity through failed startup as well
as successful return. The rigor seed already supplies the appropriate multiprocess
exercise; this identifies a concrete case for it.

Resolution reread: the revised preservation section now requires the worker to own
its native bindings/code/objects or reach a surviving owner through a protocol,
and explicitly rejects pointers into the replaced main as preservation. Sufficient
for this sketch; the actual interface/lifetime contract remains specification work.

## 2. Resolved: continuous capture needs an owner outside the replaced main

STARTING_DESIGN lines 92–105 put original capture in the foundation; lines 116–120
hand conversation/build work to the independent outpost and return accumulated
audit. The brief explicitly requires continuous original capture through quiescence,
handoff, build, failure and return. The sketch has not yet identified which surviving
participant records that interval and applies the stated recording-failure contract.
This is an ownership gap, not a request to choose a storage engine now.

Required clarification: while main is unavailable, the outpost or a surviving
capture component retains its original conversation, actual dispatched build inputs,
stdout/stderr and custody transitions under the same audit failure semantics;
return incorporates those originals with source and generation attribution. Also
name mediated client requests/responses to consumed kernels/databases/services in
the capture boundary. Their exchanges are Arconaut observations; their unobserved
internals and other consumers' activity are not captured by assertion.

Use independently known build/output/message material across a failed build/start
to check continuity. An internally consistent imported log alone cannot show that
original observations were not lost. This is already aligned with the seed's audit
oracle; no second audit-certification layer is proposed.

Resolution reread: the revised refit section identifies a surviving custodian or
outpost writer, explicit transfer of writer/observation obligations, actual
provider/service exchanges and final dispatched inputs after adapter transforms.
It excludes unobserved service internals. Sufficient for sketch scope; durability
and failure dispositions remain acknowledged specification obligations.

## Scope respected

Exact ABI, Lua error/yield handling, scheduling, pause mechanism, custody transfer,
durability/overload behavior and tool versions are correctly marked as subsequent
specification work. RCC++/Godot/JENOVA observations are bounded research leads,
not adopted dependencies or guarantees of concurrency, arbitrary stack migration,
fault rollback or macOS compatibility. Direct models and real OS/module exercises
attack different faults; the seed does not add tests whose purpose is to certify
other tests. Fleet-only mutation is retained. The proposed acceptance slice serves
the agreed self-development milestone without requiring fabric governance, a GUI,
or a general autoresearch framework first.
