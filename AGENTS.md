# Arconaut reconstruction

Read [BLACKBIRD.md](BLACKBIRD.md), [README.md](README.md), and
[JOURNAL.md](JOURNAL.md). Blackbird is the current working doctrine.

This is the fresh foundation repository. The inherited implementation, design
documents, instructions, old issue records, and automation are historical
reference under ignored `quarantine/arconaut/`. They carry no current design or
instruction authority. Sources and restoration are in [QUARANTINE.md](QUARANTINE.md).

Start reconstruction with [docs/FOUNDATION.md](docs/FOUNDATION.md): the expert
operator's stated intent, its design implications, and the remembered time and
foundation work. Mixing paradigms, multi-model live collaboration, integration
with ordinary computing tools, and almost no routine approval friction are current
product aims. The assessment in `papers/`
identifies faults and possible research leads; it does not select legacy code
for reuse. Research, reviewed design, and a reviewed plan precede product coding.

Default to implementing and owning machinery, following the workspace library rule.
Discuss any proposed library adoption with the operator before adopting it; consider
exceptional libraries with vibrant, active communities and explain concrete benefits,
fit, costs, and tradeoffs against an owned implementation. Studying or acquiring
reference source does not adopt a dependency.

The operator's brief/discussion phase has progressed to the reviewed ground-up
[docs/CORE_DESIGN.md](docs/CORE_DESIGN.md). Its two adversarial reviews resolved
eight interaction findings; behavioral review does not qualify an implementation.
Read [docs/TESTING_PLAN.md](docs/TESTING_PLAN.md) and
[docs/IMPLEMENTATION_PLAN.md](docs/IMPLEMENTATION_PLAN.md) for current plan status.
The operator's 2026-10-02 course correction supersedes a new plan-review gate:
start the bootstrap implementation directly, with short written notes, direct tests
and source grounding. Kimi can challenge concrete code later. Use the current
implementation plan's milestone-one sequence rather than waiting for full U1. Runtime/provider/
library adoption and qualification happen before the unit that needs them.
Read [docs/BEHAVIOR.md](docs/BEHAVIOR.md) as the earlier working brief. The candidate in
[papers/2026-09-30-foundation-design-discussion.md](papers/2026-09-30-foundation-design-discussion.md)
and its runtime/storage recommendations are discussion input, not an adopted
specification; current contracts and sequencing come from the documents above.

The operator corrected computation ownership during discussion: kernels, databases,
and similar services may be shared by an army of Arconauts. Arconaut is the consumer/
customer, not their governor. Their control plane, scaling, and governance belong
to future meta-project fabric work. Preserve this boundary even for local services.
Earlier resident-kernel/resource-owner proposals in papers are superseded in that
respect; read the current FOUNDATION/BEHAVIOR clarification. Model agency over its
own programs remains substantial. Refitting one Arconaut quiesces its own work and
client activity; it must not pause/terminate services consumed by others.

The operator chose change activation after conclusion of current turn/affected
workflows by default, with a combined interrupt-and-apply-now operation to bring
pending changes forward. This supersedes the earlier next-decision default in
research proposals. Control+Enter is illustrative, not a fixed UI binding. Preserve
ordinary model agency; this is timing semantics, not a new approval gate.

Read the additional operator requirements in FOUNDATION: expressive shared
orchestration, model ergonomics, hot turn redefinition, managed compaction,
comprehensive core audit, hot reload, and autodroit self-improvement. Rust is
optional; JS/TS and Go face strong distrust; JVM approaches are generally forbidden.
Python is excluded from production. The operator selected C++ with Lua scripting
on 2026-10-01; RCC++/Godot/similar native reload mechanisms, runtime versions and
libraries remain unselected. Read docs/STARTING_DESIGN.md as a discussion sketch,
papers/2026-10-01-cpp-rigor-seed.md and papers/2026-10-01-lua-rigor-stack.md as
qualification proposals. This decision does not authorize skipping design/plan review.
CLM is explicitly included in the first-pass core: model-editable live context,
model-authored transformations and their evolution, repair from originals, and
context-revision/final-request audit. Do not defer this to a later plugin. The
operator explicitly reaffirms that speed toward self-development does not relax
literature/source grounding, design review, or meaningful direct verification.
Investigate continuity through rebuild and re-inhabitation before assuming a
large compiled harness. These requirements govern the fresh research, not legacy
phase labels or the language used by the old code.

Read the outpost/refit discussion in FOUNDATION. Refit requires no active
main-harness programs/provider requests; its own standing work is preserved and paused.
Uninterrupted services belong in independently managed OS daemons. Keep refit's
conversation handoff distinct from remote outposts working as concurrent peers
with selected context and communication back to the main colleague.

The [development tooling](tooling/README.md) is configured and locally qualified.
Use `scripts/rigor check PROFILE` for owned C++/Lua work, and requalify tooling
changes with appropriate direct diagnostic cases. Tool capability does not qualify
agent behavior. Ordinary Linux foundation checks run through `scripts/check-linux`
on Neuroses with its explicit Clang 18.1.3/libstdc++ 13 profile. Lua production
embedding remains an owning-unit qualification. No production library is selected
by the tool installation.
Subsequent operator steering defers mutation campaigns until working Arconaut and
a mature suite. Mutation/Runpod provisioning is not a blocker on the current plan.
General Linux checks use Neuroses over Tailscale; future mutation uses Runpod, with
node termination the operator's responsibility. Never run mutants on this laptop.
The installed owned `kimi-colleague` skill and [Kimi utility](tooling/KIMI.md)
support independent direct CLI reviews and explicit-session rereview; do not use
the older published Node relay. [Jev](tooling/JEV.md) offers bounded advisory
judgments through the existing workspace client, never a correctness/approval gate.

At useful research/design checkpoints, read Rhizome's current C++ rigor findings
under `../rhizome/papers/` for transferable evidence. Its project scope, decisions
and instructions stay local to Rhizome. Record applicable
findings here and keep that research watch read-only.

Keep research and independent review reports in `papers/`, reference material in
ignored `quarantine/`, and working logs in ignored `context/`. Ignore PDFs. Record
actions and decisions in the journal. Do not initialize Git in the parent workspace.

Use the current local beads database for issues, with programmatic `--json` output.
The old backlog remains in `quarantine/arconaut/.beads/` for study.
Before ending a session, close completed issues, record substantive remaining
work, run checks appropriate to the changes, and run `bd backup`. This supported
backup preserves issues and dependencies in `.beads/backup/`. On a new clone,
run `bd init` followed by `bd backup restore`. Avoid exporting to the legacy
`.beads/issues.jsonl` path: installed bd 0.58.0 treats it as an incoming import and
reports a stale database after export. The earlier export is preserved in context.

Arconaut's Dolt server uses dedicated port `13308`, persisted in
`.beads/config.yaml` and `.beads/metadata.json`. Rhizome uses `13307`; the default
`3307` can belong to other projects. Beads starts the configured server as needed.
Automatic bead-backup pushes remain disabled. Operator2026-10-06 explicitly
authorizes and requires Git checkpoint commits and pushes, including work and
candidate branches before main integration. Commit completed conceptual units
and meaningful experiment checkpoints; push their branches to the configured
remote without waiting for main. Preserve source/configuration and relevant
results, excluding credentials, ignored context/quarantine/PDFs and build outputs.
Do not force-push or merge into main by implication. Record push failures plainly
and retain local commits. Branch-based candidates use bounded worktree leases,
not a checkout per archived candidate.

Keep the quarantined source unchanged. Record discovered mechanisms, evidence,
consequences, and the design question or experiment needed to resolve them.
Mutants run on the fleet, never this laptop. Run only checks that directly answer
the current research or implementation question. The operator expressly authorized
subscription-backed Kimi and Jev development calls through existing authentication.
Keep credential contents private. Acquired agent implementations remain study-only;
that review authorization does not ask us to execute them with the operator's accounts.

Operator2026-10-07 explicitly bounds hardening to TWO layers: remediation tasks
and fixes; then one recheck and fixes. Never a third assurance layer. Read
[BOUNDED_HARDENING](docs/BOUNDED_HARDENING.md). Declare defects/direct checks and
time/resource allowance before work. Do not review reviews, certify rechecks,
reopen settled checks without relevant changes, or reset budgets under new labels.
At the bound retain unsafe candidate inactive and move to independent useful work.
Current remaining capacity envelope and deferred scope are in that note.
