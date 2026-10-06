# Independent Review: U1 Retained Semantic Event Codecs

**Scope inspected:** `docs/U1_RETAINED_STATE_SUBPLAN.md` (semantic body layout, lines 150–208), `include/arconaut/retained_events.hpp`, `src/retained_events.cpp`, `tests/retained_events_test.cpp`, `include/arconaut/foundation.hpp` (Id/Result/ByteView/ErrorCode contracts), `context/u1-events-red.log` (missing-header red, as planned), `context/u1-events-batch-debug-second.log` (local 8/8 green including `retained_events`). I did not execute anything; the green log is the author's evidence, not mine.

## Verification against the specification (all confirmed correct)

- **Body layout exactness** (subplan 157–170): field order and widths match for all nine kinds — reservation counter u64; decision six 16-byte identities in ID/actor/conversation/workflow/definition/context order + u32 planned count + IDs + u32 continuation length + bytes; invocation/admission triples + length-prefixed input; open single attempt ID; observation attempt + phase u8 + disposition u8 + 2 reserved zero bytes + length-prefixed bytes; retry decision/invocation/attempt; conflict disputed-kind u16 + 16-byte ID + length-prefixed proposal; complaint two IDs + length-prefixed detail. Encode and decode orders are symmetric (src lines 135–187 vs 289–389), and the test pins exact bytes (test lines 58–62, 76–87, 115–117).
- **Schema/kind mapping**: `retained_kind` is an explicit `if constexpr` chain (src 206–233) with a comment noting variant reorder cannot shift wire kinds; schema u16=1 and kinds 1–9 match the enum; unknown schema → `unsupported` (src 270–272), unknown kind → `unsupported` (src 390–391).
- **Identity/count/length bounds before allocation**: dependency count bounded by `remaining()/24` before `reserve` (src 267, 274); planned-invocation count bounded by `remaining()/16` before `reserve` (src 91); blob lengths bounded by `take()` against remaining before any vector construction (src 52–59, 84–88). No unchecked length drives allocation.
- **Nonzero rules**: all body identities via `Id::from_bytes` (rejects all-zero, foundation.hpp 86–93); reservation counter nonzero on encode (src 193) and decode via `valid_body` in `completed` (src 284); conflict disputed identity nonzero + disputed kind ∈ {2,3,4} both directions (src 127–134, 372); dependency sequences nonzero both directions (src 238–239, 278).
- **Disposition/reserved rules**: only terminal phase permits non-none disposition, terminal requires 1–4 (src 114–126); phase range 1–3 and reserved == 0 checked on decode (src 349–351); encoder writes reserved zeros (src 170).
- **Trailing bytes**: `decoder.complete()` requires `remaining()==0` (src 105, 284) — tested for every body kind (test 147–149).
- **Length-limit symmetry**: encode enforces `max_payload` cumulatively (src 11–17), decode rejects `size > max_payload` (src 259–261).
- **Allocation errors**: both entry points catch `bad_alloc` → `ErrorCode::allocation` (src 253–255, 393–395); tested via replaced global `operator new` (test 15–26, 159–166).
- **Lifetime safety**: decode copies all identities/blobs into owning vectors; no `ByteView` escapes into `RetainedEvent`. No struct memcpy; field-by-field LE.
- **Isolation**: nothing outside `src/retained_events.cpp` + its test includes the header, so no caller currently mistakes decoding for replay validation; the header comment (hpp 104–105) and `journal_writer.hpp` (45–47, 60–61) state this explicitly.

## Findings

**No correctness defects found in the codec contract.** Minor observations only:

1. **Error-code ordering (cosmetic, not a spec violation)** — src/retained_events.cpp:267–272: the dependency-count bound check runs before the schema check, so an unknown-schema record with an oversized count reports `corrupt` instead of `unsupported`, and an unknown-kind record with malformed dependencies reports `corrupt` before the kind switch. The subplan mandates rejection, not specific codes; note only in case RetainedState later branches on these codes.

2. **Oracle gaps versus subplan-mandated cases** (test file, not product):
   - No decode-negative tests for phase=0/4 or disposition=5 (src 349), terminal+`none` disposition (the "only terminal has a non-none disposition" inverse), all-zero conflict disputed identity (src 128–130), or encode rejection of `disputed_kind` outside {decision, invocation, attempt_admitted} (src 131–133). These are the subplan's "wrong reserved fields / invalid combinations" cases (lines 230–232) and the code paths are currently unexercised.
   - No test of the dependency-count bound (`count > remaining()/24`, src 267) — the analog of the tested decision-count bound (test 93–94).
   - No exact-boundary test: `encode_retained_event(event, 16)` accepted for the 16-byte reservation, or decode rejection via `max_payload` (src 259–261) — only the 15-reject side is tested (test 71).
   - OOM is injected only at the first allocation; a mid-decode failure (e.g., blob allocation after dependencies succeed) is untested, though the single try/catch covers it structurally.

## Explicitly unfinished owning-unit checks (not defects here)

Per the task framing and subplan: dependency authority (references vs. earlier authoritative/same-batch sources, cross-journal chain rules; subplan 68–73), ledger transitions/checkpoint membership, identical-resubmission dedup and identity-conflict recording, issuer reservation/commit/counter-rebuild/burn semantics (195–208), chain continuation, admission gating, and emergency slots (210–224) are all absent and belong to the unbuilt `RetainedState`. Successful decoding here is a wire fact only, as documented.

## Inspection limits

- No execution tools: I did not build or run tests; green claims rest on `context/u1-events-batch-debug-second.log`. The red log shows only the planned missing-header failure.
- Reviewed framing/journal interfaces only as needed (`journal_writer.hpp` comments); frame CRC/sequence/commit wiring into semantic frames is the journal layer and was out of this review's depth.
- Fixed-byte verification was by hand-derivation from the layout table (offsets 8/104/123/124/128–130 for decision, 23/24/32 for dependency encoding, 26 for observation reserved byte); all matched.
