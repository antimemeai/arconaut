# First-core implementation plan

2026-10-02 operator correction: reach a useful Arconaut coding session first, then
improve Arconaut from within it. This sequencing supersedes the original requirement
that every U1 continuation/custody feature be finished before any U2 work, and that
refit/standing-worker preservation be part of the first usable milestone. The full
first-core design remains the destination; these are successive deliverables.
The operator explicitly deferred another Kimi challenge of this course correction.
Do the work now; independent review can attack concrete code later.

## Milestone one: develop Arconaut inside Arconaut

**Accepted by the operator2026-10-07; arconaut-uaa.3 is closed.** Sustained
source/test/build and inhabited RRC now deliver useful Arconaut changes. Remaining
usability, resilience and broader core capabilities are separately tracked work.

A real model, driven by a Lua coding workflow, can read this repository, edit source,
run its build/tests, inspect failures, and continue the conversation through a thin
terminal client. CLM is included: inspect/edit/reorder context, run a transformation,
and restore retained originals after a bad edit. Actual model requests, responses,
context changes, tool inputs and outputs go into the existing audit machinery.
This is the first milestone. Native hot replacement and independent-outpost refit
follow it; initial executable upgrades may use an explicit restart.

Use the existing qualified single-journal RetainedState/admission path. It already
has committed original bytes, typed identities, exact dedup and recovered-attempt
non-redispatch. A full journal or uncertain write closes new admission visibly;
bootstrap does not require automatic continuation, repairing a damaged journal or
reconnecting to surviving workers. Partial selected-chain/capture machinery stays
available for later completion and is not the bootstrap consumer API. Do not spend
another phase finishing it before a provider loop exists.

Build one vertical coding path: basic retained CLM → local file/process tools → Lua
workflow → OpenAI Responses → thin terminal interaction. Implement shared interfaces
where they serve that path; postpone the standalone custodian IPC/control plane,
standing-worker recovery, compiled generation migration, outpost refit and complaint
DB delivery until after milestone one. Shared services stay externally owned.

Direct tests must attack the behavior being added: stale context edits, preserving
originals, exact file edits, actual child-process output/cancellation, final request
bytes and a real source-edit/build/tool-result continuation. Run relevant local
checks and fix real defects. Linux access, mutation campaigns, exhaustive unrelated
fault matrices and another plan tribunal do not block the local bootstrap.

## Current work and dependencies

U0 and the single-journal portion of U1 supply the bootstrap foundation. Full U1
remains unfinished, with advanced continuation deferred. Stop adding capture cases
and custody machinery now. Existing review reports/fault tests remain evidence;
unfinished capture rereview is later work, not a prerequisite for CLM.

OpenAI is selected for the first provider (operator, 2026-10-02). See
[bootstrap implementation notes](BOOTSTRAP_NOTES.md) for the direct implementation.

The local vertical slice is demonstrated (2026-10-02): a live model edited
Arconaut, demonstrated an actual failing regression, corrected it, rebuilt the
executable and passed its test; the rebuilt binary resumed the same conversation.
A live model-authored Lua transformation edited context and repaired its original
with exact equality. Debug19/19, affected release10/10 and ASan/UBSan10/10, complete
owned-source C++ analysis and Lua diagnostics pass. See [using Arco](USING_ARCO.md).

Operator feedback after trying Arco supersedes the readiness claim: hello-world
and one self-edit/build exercise do not yet unlock sustained self-development.
The current immediate objective is docs/INTERACTIVE_LOOP_SUBPLAN.md: a basic TUI,
streaming, execution visibility, reasoning controls and clean interruption.
The transition milestone remains in progress; whole U2-U7 units and the full core
are also unfinished. Linux and independent integrated code challenge remain pending. Use the
working agent for subsequent development: native reload (U5), full
custodian/continuation (remaining U1/U3), outpost refit (U8), and complaint
sink/autoresearch expansion (U9) follow. Preserve the operator's deferred-review
sequence; do not retroactively invent a gate on using this bootstrap.

The table below retains the full destination and test families. It is no longer a
requirement to complete each whole row before starting the next bootstrap slice.

| Unit | Whole purpose and deliverable | Red checks / completion |
| --- | --- | --- |
| U0: executable foundation | Owned build/profile and small test driver; typed identities, clock domains, bytes/checked results; injected storage/clock/effect interfaces | T0; debug/optimized and supported sanitizer probes; actual tool/profile qualification, reviewed interfaces |
| U1: retained state and admission | Single-writer framed journal, commit/recovery/index rebuilding, semantic ledger, decision/action identity and effect admission; emergency control and complaint capture | T1 plus changed-input dedup/restart cuts from T3; exact durable/publication behavior and recovery qualified for declared host scope |
| U2: context and continuation lineage | Structured revisions, actual edit bases/CAS, immutable request assembly binding, continuation branches, retrieval/repair and experiment records | T2 with deterministic transformations and fake request sink; no provider or Lua needed to prove this data invariant |
| U3: custodian-mediated effects | Local versioned IPC with identity/authorship/epoch and bounded messages; process launch/input/streams/wait/cancel, retained step recovery, safe file read/write/edit and owned standing-worker protocol | T3 and initial T8 pause/capture cases; externally counted effects and bytes, writer/controller custody on reconnect |
| U4: programmable participant | Qualified Lua embed, checked host bindings, owned async continuation, generation source snapshots/module caches, root/descendant activation and interrupt controls; discoverable default workflow and context APIs | T4; real Lua-authored tool/context program used/revised, allocation/error/yield/finalizer paths and old-root progress reviewed |
| U5: live native components | Own compiler/loader boundary, retained artifacts, versioned ABI, candidate state/activation, explicit generation pins and retirement | T5; real compiled behavior changes, failed candidates leave truthful old-state disposition, delayed old-code obligations actually run before unload |
| U6: useful headless agent | Selected provider's real transport/codecs, captured final requests/streams, Lua default coding turn with ordinary file/process operations, incoming steering and context evolution | T6 plus T2/T3; real source edit/check via a model with successful linked tool continuation, meaningful errors and inspection |
| U7: baby terminal client | Owned terminal input/render/control client, Lua config, composer/history/output/status, CLM inspection/edit/repair, queued/interrupt inputs and reconnect/drafts | T7; declared terminal profile and restoration, useful coding conversation while commands continue independently |
| U8: compiled foundation refit | Independent retained outpost, drain/park/handoff/build/start/return/resume using custodian epochs and surviving capture/worker custody | T8; successful and failed refit, same paused process/VM/state, symmetric return fencing and outpost recovery |
| U9: self-development completion | Complaint bead plus external DB delivery/reconciliation, experiment/keep/reverse ergonomics, integrated Arconaut-in-Arconaut milestone | T9 and full acceptance exercise; direct outcome, both sinks, failed trials and usable continuing conversation |

The table sets dependency and acceptance boundaries, not separate redundant status
systems. Beads tracks current work; sub-plans and journal carry concrete execution
decisions/results. No schedule or duration claim is inferred from the number of units.

## Qualifications are part of the unit

U0 selects a supported compiler/C++ profile from actual installed tooling and
validates owned primitives. It requires no unselected Lua/provider/library and can
begin immediately after plan review. Avoid choosing the whole dependency graph before
this useful work. Proposed build/test tools are discussed if adoption is needed.

U1 fixes the journal framing/commit/segment and host sync details before its red cuts,
then exercises actual process crash plus modeled persistence faults. Strong power-loss
qualification needs the appropriate disposable fault environment. The release statement
must name the achieved fault domain; a failed or absent stronger experiment cannot be
renamed success. Query/index and complaint capture must consume this same retained
state rather than adding an independent durability system.

U3 fixes IPC framing, stale-controller dispatch fencing, submitted-input identity and
disconnect semantics as one admission/custody boundary. Independent clients prove
dedup at the actual effect seam, including fragmented/coalesced/oversized/unsupported
wire frames, incomplete-request disconnect and admission-reply loss; malformed requests
cannot dispatch and message limits bound allocation. Define supported standing-worker membership and park
protocol; do not advertise arbitrary descendant preservation on macOS. File edits
retain inputs/results, reject ambiguous match operations and expose partial failure.

Before U4 uses it, discuss the exact Lua runtime as a proposed adoption: ordinary Lua
5.5.1 currently leads from the grounding, but the installed 5.4.8 is not qualification.
Compare source-level linkage/unwind/maintenance/tooling against an owned scripting
runtime (a substantial diversion) and the maintained 5.4 alternative. No binding
framework is selected; narrow owned bindings are proposed. Qualify runtime, build mode,
unwind and low-memory behavior together. Lua programs may use ordinary tools and shared
services without needing a resident database/kernel implementation.

U5 directly compares the owned loader's required custody/retirement interface with
RCC++/cr and applicable Godot/JENOVA source evidence. The current grounds favor owned
machinery; do not silently adopt a framework. Specify artifact/compiler compatibility,
symbol ownership and migration in the sub-plan. If the proposed boundary cannot meet
the tests, revise the contract/plan rather than pretend module loading is hot reload.

Before U6 depends on external transport/runtime machinery, settle the initial provider
and concrete dependency choices with the operator. Read current primary protocol
sources, including supported model/stream/tool blocks and credential configuration.
Prefer exceptional transport machinery over casually owning TLS; discuss its benefit,
cost and maintenance against platform facilities. No library, shell curl wrapper,
provider credential path or model name is selected by this plan. Credential-free
conformance comes before a bounded live exercise. The default loop is Lua policy,
not hardwired sole agent topology. Audit and context programming remain available.

U7 selects supported terminals, encoding/width, modifier fallbacks and escape timeout
from primary docs/actual behavior, then specifies PTY oracles. No embedded editor or
terminal framework is needed unless evidence changes the owned-client proposal.
U8 packages a known runnable outpost independently of the candidate build; it can use
the same qualified provider. Existing custodian continues parent/stream duties.
Custodian image replacement remains a separate bounded sub-plan and qualification;
while it has promised live custody, unsupported replacement visibly blocks.

U9 chooses the external complaint database service/table and reads its actual delivery/
idempotency contract. It consumes the service; it does not become its control plane.
Offline/local capture already works when either sink is unavailable. Beads integration
uses the current supported interface and real identifiers, not reconstructed old JSONL.

## What carries forward from the inherited implementation

All old implementation stays intact in quarantine. No source module is imported into
the C++/Lua build. The [source assessment](../papers/2026-09-29-architecture-assessment.md)
and [oracle assessment](../papers/2026-09-29-oracle-assessment.md) supply concrete
mechanisms/counterexamples, not dependency authority.

| Inherited material | Fresh disposition and role |
| --- | --- |
| Time/foundation primitives, message/content/tool IDs and small interfaces | Re-express useful concepts under U0/U2 checked incarnations, clock domains and end-to-end structures; no mechanical Rust translation |
| Registry/variable precedence, exact-match file-edit ambiguity, lazy skill discovery | Adapt only the specified useful behavior when its owning U3/U4 API needs it; new red oracles decide fidelity |
| Old Soul sequential loop | Reference for U6's smallest default workflow; replace execution, recovery, generation/context and stop semantics |
| Provider/auth adapters, TUI/shell ownership, caching, context/compaction, JSONL audit | Replace under the new invariant; carry their known failures as adversarial inputs, not patch targets |
| Neovim/brush/SSH and TUI/pilot/reactor experiments | Retain as historical research; no required editor/shell/remote product adoption for first core |
| Phase claims, old backlog/CI/instructions, count-only summaries, unconnected corpus/eval/multi-agent promises | History; do not drive dependencies, acceptance or supported features |

This satisfies selection against the reviewed design: the useful base is knowledge
and examples; implementation is freshly owned. Later reuse requires a named contract
and unit review, not silent restoration of quarantined files.

## Execution handoff

Proceed with the bootstrap path above. Discuss runtime/provider/dependency choices
when their actual integration needs them; they do not block owned CLM and local-tool
work. Milestone one requires actual coding inside Arconaut, not chat alone. Complete
the remaining U1–U9 contracts from that useful environment; do not describe manual
restart as hot refit or mark the whole destination complete at bootstrap.

2026-10-03: retained session settings and stable identities plus RRC are implemented.
A live Arco edited its C++ instructions, rebuilt, passed coding tests, restarted
and continued in a new PID. Audit inspection confirms one continuation injection,
one pending/consumed intent, stable identities and rebuilt instructions in the
post-restart provider request. This adds autodevelopment machinery; it does not
close operator transition acceptance or full recovery/custody.
