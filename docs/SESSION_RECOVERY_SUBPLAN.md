# Interrupted provider-session recovery

The bootstrap audit contains one admitted/opened provider request without a receipt
or terminal observation. Ordinary startup rejects it. A current provider request
produces response data only; local tool effects dispatch later through separate
admissions. Late data from the previous process cannot reach the newly locked
journal or acquire tool authority. Its completion remains unknown.

On reopen, accept reconciliation only when every unresolved attempt is a coding
provider operation identified by its retained decision metadata and linked admission.
Bind retained input, generation and revision to the linked invocation/admission;
reserve the provider operation name from Lua tool calls. Then retain terminal unknown
observations in one atomic batch before accepting new turns. Each observation
payload explains the recovery; there is no separately committed summary. Any
settlement failure closes admission until reopen. Reconciliation makes the
physical writer live internally; it grants no recovered attempt dispatch authority
and no new turns run before settlement succeeds. Never redispatch recovered attempts or reconstruct partial calls.
Unfinished exec/file/unknown operations remain blocked. Preserve original streams
and context. Inspection must not perform this settlement.

Direct test: create provider and exec admissions with no terminal, reopen actual
journals; provider resumes with unknown and zero provider calls, old attempt cannot
dispatch, second reopen is idempotent, exec remains blocked. Verify a bootstrap-audit
copy reopens, original unchanged. Ordinary launcher uses optimized executable to
avoid debug replay cost; no fallback to a stale debug image.
