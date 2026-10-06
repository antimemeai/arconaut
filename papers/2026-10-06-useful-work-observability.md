# Useful-work interstitial: common observability

Unit1 of arconaut-uaa.14, 2026-10-06. This does not reopen closed B1.
Plan: docs/USEFUL_WORK_INTERSTITIAL_SUBPLAN.md. APIs/use documented in
USING_ARCO.md. Subsequent diagnosis/candidate/pilot work is not yet complete.

## Delivered mechanism

`audit_inspect` via ordinary model tool or `arco.call` pages committed native facts
and exact event payload/original source bytes. A fixed `end` pins an append-only
prefix across subsequent activity. Metadata includes physical journal/sequence,
application identity/channel, Decision actor/conversation/workflow/generation/context,
Invocation decision/generation, Attempt admission links and explicit Observation
phase/disposition. Provider request/stream, tool input/result/output and effective
program originals retain separate occurrence IDs; no lossy merge or new audit store.
Context payloads are recovered on demand rather than parsed for each index page.
Pages: count1..64, byte limit1..65536, hex preserves arbitrary binary/UTF8 splits.
Metadata exposes only selected bounded strings and first64 dependencies plus total.

`rageshake` retains local complaint.detail before attempting bead creation through
an independently audited exec attempt. Complaint captures model observation and
references, effective context/program/model/effort, participant/workflow/conversation,
reporting attempt and audit watermark. Native complaint.delivery records result.
Bead text carries only local identity locators, not observation or raw private
provider/context history. Failed/unknown delivery stays local with no automatic
retry. Reporting does not interrupt productive work or require immediate repair.
External sink unconfigured, available to a separately configured audit consumer;
no DB service/library was provisioned.

## Evidence and failures

Direct red: unsupported audit implementation failed audit_test at runtime.
Direct green independently expects binary `41 00 ff 5a`, a middle `00 ff` slice,
two separate original occurrences and exact recovery after physical reopen.
A test initially omitted native confirm_recovery and saw invalid_range because
committed facts were not yet published; confirmed recovery fixes the test, not
weakened production admission. Out-of-bound byte limits rejected; fixed prefix
survives append. CodingEngine's actual timeout trace exposes unknown disposition
and its decision/invocation links. Existing timeout marker/recovery checks still
assert once-only writes. Failed bead executable PATH case preserves private
observation/attempt in immutable local original, emits bead_created:false and
continues with no provider dispatch or obligation to repair. One brace typo was
caught by compiler before runtime verification.

Mac debug/release/ASan+UBSan audit+coding **2/2 each**. Focused clang-tidy audit TU
and direct audit test clean. Build/release/arco built. Portable batch and one narrow
independent Kimi review launched; final outcome/replacement verification to append.
Raw local evidence under ignored context/evolution-pilot and context/linux;
never publish raw provider history. Git source checkpoint follows activation.

## Grounding and limits

Firsthand Inspect AI event/_pool.py strict JSON identity warning and
log/test_condense_linear.py counting/reopen/divergent-lineage oracles inform exact
bytes and occurrence preservation. We do not adopt Python pooling/cache mechanisms
before measurement. AdaMAST core/evidence.py record_reflection informed advisory
observation/reference capture; its accumulated JSON rewrite and blocking verdict
gate are deliberately not copied. Local native retained event/source contracts
supply the stronger immutable retention authority. Rhizome's rigor-stack research
watch reinforces direct lifetime/ownership fault checks over an expanded ritual.

Index work bounded by requested fact count; parsing a selected log metadata packet
is skipped above64KiB. Exact source read may copy one existing document, not the
whole history. Linear search by causal ID is still a model/Lua policy atop pages;
no indexed search or replay speedup claimed yet. Record indices are session-lineage
local, not globally portable. Only committed facts exposed; unfinished provisional
or corrupt physical journal diagnosis remains existing native diagnostics. Bead
exit0 is delivery acknowledgement, not evidence a model acted on complaint.
Model-supplied references/causes are assertions, not validated conclusions.

## Final unit1 qualification checkpoint

Completed independent Kimi session4317a089-108a-46dc-adb6-9358a528faa6,
wrapper0/child0; final report preserved unchanged alongside this report. No
consequential correctness fault found. Integrated bounded references (object,
serialized≤64KiB), schema documentation of single-document source copy, and a
successful fake-bd argv oracle that independently rejects private observation in
issue delivery. Complaint now also names up to64 unsettled admission IDs plus
actual count. Count/limit validated globally is intentionally consistent strict
query validation, not a data fault. Review's multi-GB source example exceeds
ordinary document limits; copy cost remains real and documented. Broader minor
oracle suggestions do not reopen B1 or grow qualification scope.

A Ruby edit script failed before writing; its compound trailing green therefore
checked unchanged code and is not counted as qualification of that intended change.
Used exact edits instead; actual changed-source debug/release/ASan **2/2 each**
passed, then final Neuroses debug/release/ASan **2/2 each** passed at
context/linux/run-4k4nl3e3. First Linux pass run-nea3pi_d applies to pre-followup
source, retained separately. No further reviews requested. All local review/build
programs stopped; release/arco rebuilt. Quiet RRC/live tool and real bead delivery
verification next; executable activation is not implied by this source checkpoint.
