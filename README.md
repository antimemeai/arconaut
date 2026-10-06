# Arconaut

Current target: a useful coding session inside Arconaut. The operator has moved
advanced journal continuation, full custodian recovery and outpost refit after that
first milestone. The [current plan](docs/IMPLEMENTATION_PLAN.md) uses the existing
single-journal audit/admission path and proceeds with CLM, local tools, Lua, one
provider and a thin terminal client. No new plan-review wait precedes this work.

Current focus is making the coding loop comfortable enough for actual development.
The first vertical slice demonstrated self-edit/build and context repair, but the
operator reports it was not ready to replace Codex. RRC now supports retained session settings/identities and a quiet restart into
the rebuilt executable with a continuation note. A live C++ self-edit/build/restart
exercise passed; operator transition acceptance remains open. The terminal now has a
conversation view, multiline composer, streaming and clean interruption, alongside
retained CLM, file/process tools and Lua workflows. Run `./scripts/arco` from this
repository; see [using Arco](docs/USING_ARCO.md) for session, context and program
controls. A live model has edited its own tool implementation, demonstrated a
failing regression, corrected it and rebuilt Arco.

The OpenAI bootstrap uses the operator's existing Codex ChatGPT sign-in. Installed
native Codex supplies authentication/refresh only; Arco owns the Responses request
and stream handling. No API key is required. See
[bootstrap notes](docs/BOOTSTRAP_NOTES.md) for implementation and qualification scope.

Arconaut is a personal coding agent for an advanced, technically sophisticated
operator who wants to compose models, coordination styles, and computing tools,
including unusual experiments such as multi-model IRC-style live collaboration.
Routine command execution should proceed with almost no approval friction.
The operator wants to direct the work and use the machine fully.

Expressive workflows belong to both operator and model. Model ergonomics,
redefinable turns, managed compaction, comprehensive audit, hot reload, and
collaborative self-improvement in situ are core design concerns. Rust is optional;
JS/TS and Go face strong distrust, JVM approaches are generally forbidden, and
Python is excluded from production.
The compiled/dynamic boundary must serve continuity and self-improvement.

The operator selected **C++ with Lua scripting** on 2026-10-01: live compiled
native components plus Lua for tools, workflows and experimental interfaces.
RCC++, Godot-related mechanisms and similar reload approaches remain candidates;
no framework or library has been adopted. See the [starting design](docs/STARTING_DESIGN.md),
[C++ rigor seed](papers/2026-10-01-cpp-rigor-seed.md) and
[Lua rigor study](papers/2026-10-01-lua-rigor-stack.md).

CLM is part of the first-pass core: the model edits its live context, writes and
evolves its own transformations, and repairs from retained originals. Context
revisions and final provider requests remain distinct from the original audit.
See the [CLM source study](papers/2026-10-01-context-language-models-study.md).
The [research readiness assessment](papers/2026-10-01-design-readiness.md) maps
first-core requirements to literature, acquired source and qualification obligations.

The current [outpost discussion](docs/FOUNDATION.md#outposts-working-environments-and-collaborating-peers)
explores independent working environments: temporary conversation handoff during
refit and remote peers with selected context and communication back to main chat.
Refit requires main-harness quiescence; its own standing work pauses and remains alive.
Consumed shared services are outside that pause scope.

This is the fresh reconstruction scaffold. Start with
[Blackbird](BLACKBIRD.md), [foundation reconstruction](docs/FOUNDATION.md), and
the [journal](JOURNAL.md).

The [core specification](docs/CORE_DESIGN.md) now develops the agreed brief from
the ground up; two adversarial reviews found eight interaction gaps, all integrated
and reread as resolved. The [testing plan](docs/TESTING_PLAN.md) and
[implementation plan](docs/IMPLEMENTATION_PLAN.md) have also been adversarially
reviewed, with all three findings integrated and reread as resolved. Runtime,
provider and library choices receive qualification before their owning unit depends
on them. Studies do not select production dependencies. The first product unit,
[U0 foundation](docs/U0_FOUNDATION_SUBPLAN.md), now has typed identities, checked
handles/bytes/results and clock domains, qualified on Mac and Neuroses and
independently reviewed with findings resolved. The bootstrap coding loop is now usable; the full core remains unfinished.
The [retained-state unit](docs/U1_RETAINED_STATE_SUBPLAN.md) is in progress:
physical journal batches, staged recovery, typed semantic codecs and the root
semantic ledger have direct Mac/Linux evidence and independent reviews. The root
owner retains decisions/attempts, prevents repeat dispatch, validates dependencies
and burns uncertain issuer reservations. Environment-wide head authority and bounded original-byte diagnostics now have
direct Mac/Linux qualification and resolved independent code/oracle reviews. Exact
continuation maintenance codecs and complete RAM proposal ownership are implemented
with direct Mac/Linux checks and resolved independent code/oracle review. Kimi
transport is restored through a child-local address-attempt setting.
Same-descriptor physical restaging and exact historical-prefix selection now have
resolved independent review and Mac/Linux qualification. The next capacity-policy
refinement and concrete owner plan are reviewed. Initial head creation phases have
resolved independent review and Mac checks; their new Linux qualification is pending
after Neuroses SSH resets before tests. The selected-chain semantic owner is under
construction with three-segment replay/source/namespace/budget tests and resolved
independent code/oracle review of those initial paths. Provisional capture integration, live continuation, namespace entropy
and emergency controls remain unfinished; whole U1 is not accepted.

The [local development rigor](tooling/README.md) now has qualified LLVM 23.1.2
builds, sanitizer/fuzz profiles, focused diagnostics and Lua 5.4 checks. Direct
subscription-backed [Kimi review](tooling/KIMI.md) and advisory [Jev](tooling/JEV.md)
are configured. Earlier foundation Linux debug/release/ASan+UBSan checks passed on
Neuroses using Clang 18.1.3 and libstdc++ 13; current environment changes still
require Linux qualification, with SSH presently failing before tests. Mutation campaigns are explicitly deferred
until the agent works and its suite matures. Owned Lua 5.4.8 production bindings now have direct local coding/context checks. The [EDG study](papers/2026-10-01-edg-compiler-study.md)
adds a candidate independent language checker, not a selected dependency.

The [working brief](docs/BEHAVIOR.md) preserves the discussion and scenarios.
[Foundation design discussion](papers/2026-09-30-foundation-design-discussion.md)
preserves an earlier runtime candidate. The subsequent service-ownership clarification
supersedes its assumption that Arconaut owns resident kernels/databases; see FOUNDATION.
C++ and Lua are selected; the starting architecture remains its earlier discussion sketch.

The first milestone is building Arconaut inside Arconaut as soon as possible.
Programmability and substantial model agency are available during normal work.
Autodroit is tunable from a few manual experiments to model-governed autoresearch
with operator steering.

Kernels, databases, and computational services may serve many Arconauts. Arconaut
is their consumer/customer; control-plane, scaling, and governance belong to the
eventual meta-project fabric. Early self-development can consume available services.

The June code, Cargo workspace, design notes, instructions, old issues, and CI
now live together in ignored `quarantine/arconaut/`. Current work starts from
research and design. The foundation must support this operator intent, including
live concurrency and changing coordination styles. Time and foundation work are
part of that inquiry. No legacy module has been selected for the new implementation.

- [Assessment baseline](docs/RESTART.md) records source findings and research leads.
- [Action/capabilities survey](papers/capabilities/README.md) covers the 104 external
  references and historical Arconaut with native operations, evidence-linked
  matrices, family comparisons and independent review. Its
  [synthesis](papers/capabilities/SYNTHESIS.md) feeds design discussion.
- [Fresh frumentarii](papers/2026-09-30-frumentarii-synthesis.md) compares current
  programmable agents, runtime evolution, standing state, and self-improvement.
- [papers/](papers/README.md) holds independent reports and recovered literature.
- [QUARANTINE.md](QUARANTINE.md) records exact sources and restoration commands.
- `context/` holds ignored working material; current issues use beads.

The public repository stopped at `76ddf00`; the existing local repository contains
15 later commits through `3bf2056`. This assessment branch starts from the latter.
Both histories are preserved. A complete source ZIP restores the quarantined tree;
the original checkout, large ZIP, and source-history bundle remain intact.
