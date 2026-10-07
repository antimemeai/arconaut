# Native decision models — 2026-10-07

Candidate: candidate/native-jev-2026-10-07, based on master20d3006.
Issue: arconaut-1ma. Written subplan and current API/reference grounding are in
[DECISION_MODELS](../../docs/DECISION_MODELS.md). No dependency adoption or production
Python. Existing C++ Child/curl machinery carries one native admitted audited
batch, exposed through model tools, Lua, and operator slash discovery.

## One independent review

The independent native_jev_review colleague inspected the concrete implementation
read-only in one bounded review. Its three findings were:

1. Response capture occurred only after success, losing failure/partial bodies.
2. A separate quiet stderr pipe added a 100 ms polling delay before stdout chunks.
3. Score legend validation allowed contradictory or nonstring level descriptions.

Fixed by capturing observed stdout in Child's observer before outcome parsing,
using the existing quiet-curl stderr disposition, and checking string legend
values against string criteria. Added direct failure-body, 128 KiB/one-second
transport, and contradictory-legend checks. This is a summary of that review,
not a verbatim transcript. No second review or review of the recheck.

## Direct evidence

Release build, BLACKBIRD_DEBUG=OFF, <=j2 succeeded. Existing affected coding,
tools and terminal suites passed (13.08s, 0.40s, 0.47s). The new decision-model
suite initially exposed a test lifetime error: iterating a subobject of a temporary
Json. Keeping the owner alive fixed the test. Final decision_models CTest passed
in 1.77s. No repeat of unaffected suites.

Checks cover valid/invalid batches, answer identities/types/options/levels, pinned
model versus aliases, raw probabilities and usage, malformed values, credentials
absent from argv/audit, own fake-transport stdin, HTTP/redirect/oversize/invalid
response failures, cancellation/deadline, retained failure bytes, and native Lua
admission/effect/settlement linkage. Focused clang-tidy found an unnecessary string
parameter copy; changed to const reference. Its affected recheck exited zero.
These tests exercise transport and structural correctness, not model calibration.

One live native /decision batch succeeded with Choice, Score and Noul answers
from jev-1.13.0: 385 input tokens, 63 output tokens, adapter elapsed288234us.
The native operation took338ms; the whole isolated CLI process took0.756s.
This was one observation, not a latency distribution. The live valid-response
call preceded the review corrections; deterministic direct checks cover those
changes. Request, raw output and its private audit remain in ignored context.
No credential content was inspected or printed by development orchestration.

Remaining scope: explicit master integration/activation; additional decision-model
adapters and calibrated policies as separately planned work. No hidden retries,
thresholds, cache, automatic approvals, or startup credential I/O were added.
