# Protected settlement credit: native seam, not workflow completion

## Implemented claim

A native `RetainedState::protect_settlement` scope owns volatile byte/physical-record
credits. Its owner must outlive it. Nesting cannot reset the allowance. Normal
append paths—including originals, issuer allocation and rejected-proposal diagnostics—
leave the allowance untouched using exact framed batch cost. Only validated adapter
receipts and terminal attempt observations through `submit_settlement` may spend it;
dispatch routes its receipt through that seam automatically. New acknowledged writes
debit exact cost. Scope release removes a RAM constraint only: no file mutation,
persistent reservation, recovered effect permission or extra physical capacity.

Caller chooses adequate obligations before admission. Deliberately undersized receipt
credit can fail after effect and closes admission, as existing recording failures do.
This unit does NOT enable a guessed live workflow reserve. It does not bound context
cancellation, pending management, call linkage, output overflow or nested obligations.
Atomic triple admission, live handoff and independent old-original resolution remain.
No library adoption, cap increase or old audit modification. `.14.3` stays active.

## Direct fault observations

Simulation uses the actual retained dispatch/callback/receipt path, independently
4096-byte vs7-record caps. Callback exhausts normal source writes precisely, verifies
prewrite refusal and earlier retained originals, then reports capacity. Reserved
receipt and unknown terminal fit exactly; each consumes measured credit and the
adapter is not called twice. This is not a live provider/process overflow test.

Holding all normal room prevents open before adapter invocation; issuer reservation
and rejected-proposal capture cannot steal the floor. Native nested grant before and
inside callback is busy. Grant extent/allocation failure leaves no held budget.
Undersized terminal refuses prewrite without debit and remains live; undersized
receipt after effect closes admission. Reserved receipt partial write and sync failure
retain pending/provisional originals, poison writer, leave credit undebited and do
not publish a committed receipt. Provisional RAM can contain an uncertain receipt;
that is not acknowledged history.

Two initial test mistakes were corrected, not product findings: CHECK of optional
needed explicit `has_value`; the sync-failure oracle initially incorrectly demanded
no provisional receipt, instead of uncertain RAM plus no committed receipt.

## Independent review and consequential disposition

A separate fresh native ChatGPT/gpt-6.1-sol session performed one read-only request
with exact owner/append/submit/dispatch code and tests; no tool dispatch.8259 input,
2277 output tokens;62465ms. Private capture `context/resilience/protect-review-*`.
No demonstrated floor-accounting/publication bug. Review correctly identified an
ambiguity in the stated duplicate guarantee: public append is physical publication,
while submit/submit_settlement deduplicate exact facts at zero cost. Clarified native
API comment and added direct same-fact test under fully held room. Do NOT remove
repeated batch drafts: callers compute source sequences against physical positions.
The zero-cost promise is submit-specific, not uniform across arbitrary batch append.
No repeated review gate, pilot rerun or B1 recertification.

## Affected verification

Mac debug/release/ASan+UBSan initially5/5: retained_state, retained_environment,
journal_batch, coding, context. Final expanded direct faults and formatting pass
retained_state in all3 profiles, with release/arco rebuilt. Linux original snapshot
same5/5 each profile, capture `context/linux/run-owzojrhf`. Final expanded tests still
need the final fresh snapshot result recorded below. An attempted reuse of that
remote build was explicitly refused ENOENT: the ordinary runner had already cleaned
its temporary source; no tests claimed from that attempt. Distinct fresh final run
uses the ordinary qualified runner filtered to retained_state, not a repeated effect
with unknown outcome. Initial checks remain settled; only changed test unit repeated.
Native quiet replacement will follow committed build; successful activation is not
yet claimed. No live workflow exhaustion recovery/promotion readiness implied.
