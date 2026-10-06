# U1 continuation prerequisite review resolutions

2026-10-02. Scope: maintenance wire formats/root refusal and exact owned RAM proposals.
The selected-chain owner, placement/capture-set checks, namespace entropy and full U1
remain unfinished. Original independent replies are preserved unchanged:
[wire review](2026-10-02-u1-maintenance-wire-kimi-review.md),
[wire rereview](2026-10-02-u1-maintenance-wire-kimi-rereview.md),
[proposal review](2026-10-02-u1-owned-proposal-kimi-review.md),
[proposal rereview](2026-10-02-u1-owned-proposal-kimi-rereview.md).

Wire findings1/2/4 resolved: trailing material pins Corrupt, successful allocator-cut
decoding compares the complete expected event, and unknown14 Unsupported is distinguished
from malformed13 Corrupt. Finding3 is the explicitly pending environmental placement
scope, not a missing codec check. The review prose's8..107=108 arithmetic was wrong;
that span is100, and both u32 count fields make the fixed body108 (empty vector116).
Kimi explicitly corrected this; no production change. Final rereview's remaining
cosmetic repetition of error-code assertions shares already-pinned paths and finds
no new fault class; no extra testing/review loop is justified solely for repetition.

Proposal D1 is incorrect and explicitly withdrawn: max_records counts source/semantic
records, while commits additionally consume sequence numbers. Existing chunks>=remaining
is the right chunks+marker capacity check. Direct max_records5/6 cases prove clean
one-short refusal versus exact fit (four chunks+marker+one prior record, cursor10/end5598).
The proposed replacement frames>remaining would incorrectly reject the legal fit.

G1/G2 resolved by pre-write extent failure after the first committed group and explicit
Blocked-versus-Poisoned assertions. G4 resolved by refusing subsequent ordinary append
without writes or changes to the retained outer packet. G3 now has a direct global
allocation failure armed inside acknowledged sync: root/physical publication succeeds
without a subsequent allocation. Existing prepublication heap cuts remain. G5 withdrawn
as a redundant variant of the same count/length mechanism already directly exercised.
Final scoped rereview has no outstanding confirmed production/oracle defect.

Kimi transport was restored without reading credentials or new login: this host's
Node address-attempt timing is too short at defaults; a qualified3000ms setting now
applies only to the wrapper child when NODE_OPTIONS is absent. Explicit inherited
options and read-only reviewer permissions are unchanged. Terminal status/child0 and
exact session IDs verified; earlier failures remain genuine failures in context.
Mac and Neuroses qualification captures are recorded in JOURNAL; no stronger power-loss,
cryptographic history, live U3 custody or full-chain acceptance claim follows.
