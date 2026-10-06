# Declared successor seed: direct implementation review

Unit: explicit bounded fresh-session selected context seed, not automatic rollover
or a capacity-reservation solution. Written dependency subplan is in
[HISTORY_CAPACITY_SUBPLAN](../docs/HISTORY_CAPACITY_SUBPLAN.md). Native CLI
--seed-session accepts model/operator authored source locators/status and selected
entries; no old namespace is read, moved, increased or reconciled. Declared lineage
is deliberately not authoritative predecessor history. Context and mapping publish
in one packet; fresh IDs, no inherited admissions or imported-effect dispatch.

Independent ChatGPT/gpt-6.1-sol through a separate fresh native session reviewed exact
validator/append implementation and CLI diff. Read-only one-request Lua workflow
never dispatched tools. Actual usage3923input/816output tokens,24742ms provider.
Private prompt/raw review under context/resilience/seed-review*. No credentials
included. One medium real finding: pairing/role checks accepted missing message
content/role and tool name/arguments/output, or tool item with a user role; seeded
context could fail provider protocol after acceptance. Fixed required-field/type
and role/type checks, current text-content/refusal and encrypted reasoning schemas.
Direct negative cases added. Review did not find a concrete atomicity defect;
transport/physical publication is the existing mechanism, not new certification.
No repeated review gate or B1/pilot recertification.

Direct tests assert malformed seeds reject before mutation, exact context/mapping,
fresh original IDs, preserved unsettled declaration, nonempty destination refusal,
native replay reconstruction and small-capacity failure without context publication.
Initial test wrongly assumed a single fact, omitting the existing issuer reservation;
corrected oracle to issuer+context. Initial128byte fixture violated journal minimum;
512byte fixture permits issuer but refuses context, which remains unpublished.
Test header typo and overbroad mechanical schema insertion into unrelated existing
protocol_complete caught by compiler; restored that function unchanged. These are
actual failed probes, not claimed passes. Mac/Linux debug/release/ASan affected4/4
then final changed successor_seed/context2/2 each. Initial native CLI smoke seeded
and quit without a provider, refused existing unchanged audit and invalid manifest
without creating destination; subsequent tightened-schema cases covered directly.

Remaining .14.3: workflow/output reservation, live handoff, honest selected source
export and original-entry resolution without replaying full parent history. Seed
alone does not prevent ongoing exhaustion. No significance/economy generalization.
