I inspected docs/U1_CONTINUATION_SUBPLAN.md lines 150–196 (plus owning-plan context lines 1–149), include/arconaut/journal.hpp, include/arconaut/journal_writer.hpp, src/journal.cpp, and include/arconaut/retained_events.hpp. The codecs confirm the premises: the 112-byte header carries only JournalLimits (max_payload, max_batch_bytes; src/journal.cpp lines 100–104, 148–149), and RecoveryChoiceEvent carries old_limits + old/new RetainedCapacityDeclaration with the explicit comment that wire capacities never choose replay allocations (retained_events.hpp lines 131–149). Findings below, in severity order.

## Findings

### 1. High — Header framing limits are wire values that do choose allocation size, contradicting "wire values never choose reserve sizes"
- Location: docs/U1_CONTINUATION_SUBPLAN.md lines 162–166 vs src/journal.cpp lines 28–31, 212–214.
- Trigger: The configured-bounds list (line 162–163) covers segment-file bytes, records per segment, segment count, and indexed history — but no bound on frame payload/batch bytes. `valid_limits` (journal.cpp line 28) accepts any max_payload up to max_batch_bytes − 88 with no upper cap; a corrupt or hostile segment header can declare max_payload near 2^32. `decode_journal_frame` then treats a length ≤ that limit as admissible, and scan/reserve sizing follows it.
- Consequence: A wire-declared framing limit silently drives scan/replay allocation outside all configured bounds — exactly what line 166 forbids ("wire values never choose reserve sizes"). This is also a DoS/false-refusal asymmetry: a garbage historical header forces either huge allocation or an unbounded-allocation acceptance.
- Minimal fix/oracle: Add configured maximum frame-payload and batch-byte bounds to the independent-bounds list, and state that every selected segment header's JournalLimits (root and children, including any changed framing on continuation) is validated against them before any scan, with Capacity/unsupported refusal. Oracle: header with valid_limits-passing but configured-bound-exceeding max_payload refused before candidate I/O.

### 2. Medium — Root-only reopen has no anchor for "wrong supplied root refused"; the requirement is unenforceable in that exact case
- Location: lines 154–160, 170, 193–194.
- Trigger: The chain rule anchors the root declaration via the first child choice's old_capacity (line 155–156), so a 3-segment history can refuse a wrong supplied root. But in a root-only environment there is no RecoveryChoiceEvent and the header does not encode capacity — nothing persisted exists to compare the reopened declaration against.
- Consequence: Line 194 ("Root-only reopening must use its explicit declaration") plus line 159–160 ("A different root declaration on reopen cannot silently reinterpret an existing linked history") are jointly silent about root-only: a larger supplied declaration on root-only reopen silently raises the effective append policy over existing bytes with no possible refusal. As written, "wrong supplied root … refused" (line 190–191) is only realizable for chains.
- Minimal fix/oracle: State explicitly that the root declaration becomes cross-checkable only once a first continuation choice persists it; root-only reopen validates the supplied declaration solely against configured bounds and is the trust root. If that is not intended, a persisted anchor must be named (that would be a format change, which the section forbids — so the honest-spec option is the former).

### 3. Medium — Chain-consistency rule omits old_limits and the new segment's framing limits
- Location: lines 154–158 vs retained_events.hpp lines 142–144.
- Trigger: RecoveryChoiceEvent carries `JournalLimits old_limits` and only new *capacity*, not new limits. The subplan chains only capacities ("old capacities must equal … validated new declaration"). It never says old_limits must equal the predecessor segment header's actual JournalLimits, nor what constrains the new segment header's framing limits (which may differ from old_limits).
- Consequence: A choice with matching old/new capacities but fabricated old_limits passes the stated acceptance rule; the event's self-description of history goes unchecked, weakening the exact-history guarantees the surrounding replay section (lines 127–141) depends on. Also, unconstrained new framing limits feed finding 1.
- Minimal fix/oracle: Add: each choice's old_limits must equal the immediately preceding segment's immutable header limits (first child: root header limits); each new segment header's limits are validated per finding 1. Oracle: choice with correct capacities but mismatched old_limits refused; continuation adopting larger framing limits accepted only within configured bounds.

### 4. Low-medium — Host-width conversion hazard is named but its refusal semantics and bound types are not pinned
- Location: line 165 ("all conversions") vs journal_writer.hpp lines 8–11 vs retained_events.hpp lines 133–137.
- Trigger: JournalCapacity.max_records is `std::size_t` (host width) while RetainedCapacityDeclaration.max_records is `uint64_t`. On a 32-bit host a wire-declared record policy can exceed size_t. "Validate … all conversions" does not state refusal-not-truncation, and the configured-bound types (e.g., "physical records per segment") are unspecified, so the comparison operands are undefined width.
- Consequence: Unspecified truncation could silently shrink a declared policy (silent reinterpretation, violating line 159–160's spirit) or a wrap could inflate it.
- Minimal fix/oracle: State that any wire/configured value not representable in the host operand type is a Capacity refusal, never clamped; give the configured bounds concrete types. Oracle: 32-bit host (or emulated narrow size_t) refuses a 2^32+ record declaration.

### 5. Low — "Physical records per segment" bound and the policy-fit check don't restate the commit-record counting rule
- Location: lines 162–163, 172 vs line 118 ("Noncommit source/semantic records consume max_records; commits consume additional sequence numbers only").
- Trigger: Line 172 requires the selected committed prefix to fit its segment's declared file/record policy, and line 163 configures "physical records per segment," but this section doesn't say whether commit frames count toward either the declared policy or the configured bound. Line 118 answers it for max_records, elsewhere in the document.
- Consequence: An implementer of the next-layer owner could count commit records in one check and not the other, producing false refusals (prefix exactly at boundary rejected) or off-by-one acceptance.
- Minimal fix/oracle: Cross-reference the line-118 counting rule explicitly in this section for both the declared-policy fit check and the configured records-per-segment bound, and add a boundary oracle (prefix with records exactly at declared max_records, commits interleaved).

## Non-findings (checked, consistent)
- available_end exceeding the predecessor's declared append policy without authorizing the suffix (lines 175–177) is coherent with JournalRecoveryReport.available_end (journal_writer.hpp lines 39–43) and the header's geometric predecessor validation (journal.cpp lines 43–53); suffix boundedness is covered by the independent scan limits (line 173–174).
- Snapshot bounded independently of one segment's record limit (lines 180–186) is sound; exact counted entries and owner API are correctly deferred, per scope.
- "Insufficient bounds → Capacity, no reduced history/older branch" (line 169–170) is consistent with the chain-selection rules at lines 120–125.

Limitations: review is design-level per scope; no owner implementation exists to test, and I did not execute anything. The required-history test list (lines 188–194) is adequate except that it should add oracles for findings 1, 3, and 5.
