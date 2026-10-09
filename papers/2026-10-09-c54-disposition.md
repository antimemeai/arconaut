# C54 colleague disposition repairs

Operator scope2026-10-09: finish the two colleague defects while working on native
instrumentation; evaluations wait for further discussion. Unit starts22:31:23UTC,
ends no later than00:01:23UTC. Hardening is at most25minutes inside that allowance:
remediation plus one owner code review/recheck and fixes, never a third layer.
Root coordinates shared six-worker builds and owns CodingEngine/instrumentation,
commits, issue tracking and journal. This lane owns colleague implementation and
its direct native contract oracle. No provider calls or new dependencies.

## Source and contract

Read current AGENTS/BLACKBIRD, README/JOURNAL, `docs/COLLEAGUE.md`, the G5 source
study, current colleague/native transport and tests, CodingEngine classification,
and RetainedState ordinary/reserved append behavior before edits. The earlier
Claude review is a list of assertions to triage, not an authority to copy fixes.

[Official CLI reference](https://code.claude.com/docs/en/cli-reference) defines
print mode and JSON output. The published
[official Agent SDK result types](https://app.unpkg.com/@anthropic-ai/claude-agent-sdk@0.3.193/files/sdk.d.ts)
define error results with `errors`, usage and modelUsage, and no `result` field;
success results carry `result`. These are source evidence about the envelope,
not dependency selection or a promise that every installed CLI follows a future
package version. Existing captured CLI observations and owned cases retain the
legacy success/auth-error envelope behavior.

## Plan and direct oracles

1. Add direct failing native cases for a type=result/is_error=true response with
   no result, explicit errors and usage/modelUsage; missing diagnostics must not
   erase reported failure. Keep malformed is_error unknown, single-model actual
   attribution observed, and multi-model attribution null.
2. Preserve received native usage/model metadata before outcome/refusal validation,
   and Claude metadata across success/error/nonzero branches. Never derive actual
   model from requested aliases. Originals retain exact metadata; root's audit
   instrumentation independently bounds its projection and marks unavailable or
   malformed fields.
   `model_metadata_status` explicitly distinguishes observed, unavailable,
   ambiguous and malformed; `actual_model` remains a string or null. No decoded
   response from a failed transport means unavailable metadata.
3. Repair reported error decoding without requiring a success-only result field.
   Preserve structured reported error evidence. No automatic replay or new call.
4. Root adds ErrorCode::allocation to uncertain effect classification and a direct
   engine oracle: counted dispatch throws bad_alloc, resulting invocation terminal
   is unknown, no retry/replay. This lane does not edit coding.cpp or coding tests.
5. Coordinate red/green targeted colleague builds/tests, formatting and required
   analysis with root; root reviews the concrete diff once. Record actual outcomes
   and remaining scope before completion.

## Initial finding triage

- Finding1 valid: known is_error=true is overwritten as unknown when success-only
  result is absent. Current code also treats malformed nonboolean is_error as a
  reported error; exact boolean classification is part of the same repair.
- Finding2 valid: Boundary maps bad_alloc to allocation, absent from the uncertain
  effect gate. Root owns that integration repair and engine oracle.
- Finding3 is not a replay/durability defect. Ordinary and reserved submit share
  the retained event transaction/semantic path. They differ in capacity accounting;
  CodingEngine already falls back to bounded unknown reserved terminal on ordinary
  capacity refusal. No new settlement mode is selected here.
- Finding4 describes safe pre-dispatch capacity refusal, not lost custody. The
  request's64KiB bound is not a guarantee that expanded admission fits any journal.
  Capture failure prevents dispatch; no new unrelated capacity policy is added.
- Finding5 is intentional: mixed text/refusal is refused without presenting text
  as a completed answer. Exact upstream bytes remain retrievable; preserve the
  available usage/actual model in the refusal envelope.
- Findings6/7 require no repair: native transport cancellation remains conservative,
  and prepared context stays explicitly selected. No ambient context export added.

## Results

Root coordinated the debug red build. Its first attempt rejected a range-loop Value
copy under `-Wrange-loop-construct`; changed it to a const reference, without
suppression. Corrected red build succeeded; `colleague` then failed in0.42seconds
with "reported CLI error without result must remain a single known failure".
Evidence: ignored `context/nls-afk-colleague-red{,-build}.log`.

Owned implementation and direct cases are written and qualified clang-format has
run on the changed colleague source/header/test. Root coordinated the green debug
build of `colleague_test` and `provider_colleague_test`: CTest `colleague` passed
in1.62seconds, `provider_colleague` passed in0.41seconds,2.04seconds total. Evidence:
ignored `context/nls-afk-colleague-green{,-build}.log`. No duplicate test run was
added by this lane. Root's one concrete diff review and shared required lint/profile
gate remain integration work; no completed-review or activation claim is made here.
No provider calls, evaluations, new dependency, mutation campaign or unrelated
suite rerun occurred in this lane.

Implemented source fixes reside in `src/colleague.cpp`, with the result contract
documented in `include/blackbird/colleague.hpp`; direct expectations reside in
`tests/colleague_test.cpp`. Root owns the allocation gate and actual invocation
terminal UNKNOWN/no-replay engine oracle. No commits, issue changes, root docs or
CodingEngine product/test edits were made by this lane.

Metadata limitations are explicit: available decoded response metadata survives
failure/refusal/incomplete-answer classification, and ambiguous model maps never
select the requested alias. A transport that throws before returning a decoded
result has no parsed metadata to project; retained raw captures remain evidence.
Usage/model_usage preserve exact reported Values, including malformed ones, while
native actual_model stays string/null and attribution status names the distinction.
The root's instrumentation must bound and validate its own derived rows; these
provider fields are neither calibrated costs nor a claim of task correctness.
