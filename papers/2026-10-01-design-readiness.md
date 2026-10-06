# Research readiness for Arconaut design

2026-10-01. The operator asks whether the literature and references are sufficient
to enter design, and explicitly rejects haphazard work in pursuit of moving out of
Codex. This assessment concerns the C++/Lua first core, including CLM and the
self-development milestone. It does not certify a runtime or approve product coding.

## Judgment

There is enough evidence to begin ground-up behavioral and foundation specification.
The agent survey alone did not establish that. The recent stack decision exposed
specific source gaps: native reload, the exact Lua embedding configuration, and
durable custody during refit. Targeted acquisitions and mechanism studies below
address those research gaps. Their remaining qualification questions must shape
the design and experiment plan, not disappear behind a generic confidence claim.

The original starting sketch is a proposal. None of RCC++, cr, sol2, LevelDB or
the other references is selected as a dependency. Historical Arconaut still has
no design authority. A read mechanism, a specified contract and an observed runtime
result are three different kinds of evidence.

## Coverage against the first core

| Design question | Literature and source basis | What is established and what remains |
| --- | --- | --- |
| Ordinary coding operations and expressive workflows | Source-grounded agent survey; CodeAct/RLM literature; Prime Agent, Dagger, Restate, Windmill and live coordination references | Enough contrasting execution, observation and composition mechanisms to specify our own operations. No existing agent supplies the complete intended contract. |
| Model-authored live context and its evolution | CLM paper methods/evolution sections, pinned released source and its source study; RLM and reflective harness research | Direct editing and reusable model-authored transformations are grounded. Define structured editing, publication, conflicts, repair, provider assembly and experiment outcomes ourselves. Released CLM swarm options and source audit are materially limited. |
| C++ native replacement | RCC++ author chapter and acquired exact source; fungos/cr comparator; Godot/JENOVA documented boundaries | Source mechanisms support design of explicit activation/state transfer. Neither candidate provides our complete generation lifetime, retirement, fault recovery or platform contract. |
| Lua ownership and control transfer | Official Lua 5.5.1 source and matching tests, pinned sol2 binding reference, official embedding manual | Concrete error/yield, continuation, roots and cleanup obligations are known. Exact linkage, unwind behavior, allocation failures and native reload interaction remain runtime qualification subjects. |
| Audit persistence and recovery | Earlier audit study; acquired LevelDB log source and failure tests; application crash-consistency literature | Enough to specify admitted-effect recording, raw capture, framing and failure behavior. An owned append log still needs its own protocol and recovery oracle; a mature reference is not its correctness argument. |
| Refit with preserved standing state | Existing s6 source; OS descriptor, wait/stop and execution contracts; focused custody study | Main replacement can be specified against surviving owned workers and observation custody. Descriptor duplication is not authority/parentage transfer. Whole-tree pause and replacement of the custodian itself remain unsupported general claims pending design and qualification. |
| Rigorous implementation feedback | Rhizome C++ studies plus Arconaut C++ seed and Lua rigor report | Compiler/analysis/sanitizer candidates and direct semantic/history oracles are grounded. Actual complete profiles and compatible runtimes are not locally qualified. |
| Initial provider and observation transport | Provider/action mechanisms in the acquired agent corpus; earlier raw-exchange audit study | Enough to state the final-input/raw-output capture contract. Choose one initial provider and inspect its exact current protocol and transport before integration; no credential search or live call is necessary for that design work. |

The seven earlier acquired papers cover code actions, recursive context, reflective
evolution and self-improving harnesses. The dated studies identify their relevant
reading and limits; acquisition is not represented as a cover-to-cover reading claim.
CLM adds directly editable live context, and the RCC++ chapter adds a native mechanism
after the operator's language decision. Corpus size is not the readiness criterion.

## Constraints learned from actual mechanisms

RCC++ retaining loaded modules does not keep its replaced objects alive; its object
swap and native fault protection must be judged against our old-work preservation
and C++/Lua unwind requirements. Lua retains raw native function/continuation
pointers without knowing which shared library owns them. Generation custody must
span those callable and cleanup obligations, including VM shutdown.

Persistence requires a supported fault domain and concrete ordering/durability
points. A successful flush is not necessarily a sync. Resynchronizing a damaged log
can be useful in a database recovery policy while violating a claim of complete
audit history. Our recovery must report gaps and retain available originals.
Passing open descriptors does not hand over sole execution authority or the right
to wait for someone else's children. Pause observations apply to a defined owned
scope; they cannot freeze remote effects, shared services or escaped descendants.

These are design inputs, not reasons to abandon native programmability or introduce
per-command approval. They explain why an apparently convenient demo is insufficient.

## Additional persistence literature

Pillai et al., [All File Systems Are Not Created Equal](https://www.usenix.org/conference/osdi14/technical-sessions/presentation/pillai)
(OSDI 2014), sections 2–3, distinguishes persistence atomicity and ordering and
studies application update protocols through possible crash states and explicit
application checkers. The paper's Linux experiments do not establish current APFS
behavior. Its useful methodological constraint is to state the filesystem assumptions
and check the real recovery invariant across their permitted states. An acknowledged
record and a pending record may have different acceptable recovery outcomes.

The intact publisher PDF is retained at `papers/pdfs/osdi14-paper-pillai.pdf`;
[catalog](2026-10-01-persistence-literature-acquisition.json) records its source/hash
and reading scope. No paper's tools or reference tests were executed.

## How to proceed with discipline

Begin specification with operations, generation ownership and CLM context revisions
as one connected foundation. For each consequential contract, identify the source
mechanism and limits, define our behavior/failure states, and attach a direct oracle.
Use the research to expose design alternatives; do not copy a reference's defaults.

Before implementation relies on the stack, review the specification and a bounded
qualification/implementation plan. That plan must resolve the C++/Lua linkage and
unwind configuration, native object/callback retention, actual pause scope and custody,
and audit commit/recovery semantics on the intended host. Missing support changes
the mechanism or the supported scope; it does not get relabeled success.

Moving into Arconaut quickly narrows breadth: one provider and a modest operator
interface first. It does not defer CLM, replace comprehensive captured originals
with summaries, silently recreate paused programs, or waive review and meaningful
verification. The full model-governed research framework and shared fabric scaling
can grow after the actual self-development loop is usable.

## Focused evidence reports

- [Independent readiness review](2026-10-01-design-readiness-review.md): supports
  entering design, with the corrected restoration-path defect resolved and no
  unresolved readiness findings. This does not qualify an implementation.
- [CLM released-source study](2026-10-01-context-language-models-study.md)
- [Native reload grounding](2026-10-01-native-reload-grounding.md)
- [Lua embedding grounding](2026-10-01-lua-embedding-grounding.md)
- [Audit and refit custody grounding](2026-10-01-refit-custody-grounding.md)
- [C++ rigor seed](2026-10-01-cpp-rigor-seed.md) and [Lua rigor](2026-10-01-lua-rigor-stack.md)
- [Earlier audit study](2026-09-30-audit-state-design-study.md) and
  [runtime study](2026-09-30-runtime-design-study.md), preserving their superseded
  language/service-ownership recommendations as history rather than current authority.
