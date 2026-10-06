## Final scoped verdict (recheck after oracle resolutions)

I re-read the actual `tests/retained_events_test.cpp::continuation_wire()` (lines 272–477). Production codecs unchanged; no new production faults found. Kinds 1–11 untouched; no full-chain acceptance implied.

### Oracle findings — resolution confirmed

1. **Trailing-byte error code**: now pinned to `ErrorCode::corrupt` for all three vectors (`choice`, `capture`, `full`) at lines 358–362. Resolved.
2. **Decode allocator sweep compares decoded value**: the success branch resets the hook (`fail_allocation.store(-1)`, line 461) before `CHECK(parsed.value() == populated)` (line 462), so the comparison itself cannot be poisoned by the fault injector. Resolved.
4. **Unknown vs malformed distinguished**: kind 14 on the choice body yields `unsupported` (lines 430–434); purpose-0 on the capture body yields `corrupt` (lines 435–439). Resolved, and the `default:` decode branch is now exercised for the new schema.
3. **Placement coverage**: agreed — correct pending-owner scope limitation, not a defect; not counted as a finding.

Also accepted: my "offsets 8–107 = 108" phrasing was wrong — that span is 100 fixed bytes; the 108-byte fixed body includes the two zero u32 count fields, already pinned by the 116-byte empty vector and the 228-byte populated vector. No test change needed.

### Remaining minor observations (non-blocking, no production impact)

- A few negative wire checks still assert only `!has_value()` without pinning `corrupt`: `duplicate_descriptor` (line 400), `dependency_wire` (416), `missing_chunk_wire` (424). The analogous encode-side rejections (404, 408, 411, 419) don't pin `invalid_range`. Given the envelope-rule code paths are shared with the ones that *are* pinned, this is cosmetic; a specified-red improvement would assert the codes.
- The decode allocator sweep's `cut < 64` bound is empirical; if allocation count ever exceeded 63 the sweep would fail loudly (`CHECK(success)`), so it is self-guarding — no action needed.

### Verdict

**Approved within scope.** The exact 12/13 codecs (108+20A+36C bodies, canonical ordering/uniqueness/nonzero IDs, boolean/phase/disposition/evidence/error/purpose/reserved validation, count bounds before allocation, envelope dependency rules) are correctly implemented and now well-oracled, including full truncation sweeps, capacity on both directions, allocator-cut evidence with content verification, and unknown-vs-malformed error distinction. Root refusal of maintenance both live and replay stands via the kind-based `apply()` gate. Unresolved by design: whole selected-chain owner semantics (placement, required-set cross-check, custody/source routing, reconciliation-set equality) remain pending owner work and are neither claimed nor validated here.
