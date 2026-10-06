# Independent Kimi review

Unchanged assistant replies. Read-only reasoning; no reviewer execution.

## Independent review: docs/U1_RETAINED_STATE_SUBPLAN.md

**Inspected:** docs/U1_RETAINED_STATE_SUBPLAN.md against docs/CORE_DESIGN.md (operation lifecycle, original audit/durability, complaints), docs/TESTING_PLAN.md (T1, T3 excerpts), docs/IMPLEMENTATION_PLAN.md (U1 row and U1 paragraph), and include/arconaut/foundation.hpp (U0 interfaces). No files changed; no commands run; review only, no execution evidence.

**Verified sound (not findings):** the byte arithmetic is internally consistent — file header 8+16+16+4+4+4+8+4 = 64 with CRC over the first 60 bytes (lines 39–43); frame header 4+2+2+8+8+4+4 = 32 with CRC over bytes 0..27 + payload (lines 46–48); commit payload 8+8+4+4 = 24 (lines 53–54). Batch-first-sequence in every frame cross-checks the commit payload. Capacity-close-instead-of-rotation, the weak power-loss release statement, U3/U9 deferrals, and the admission/dedup/retry semantics all match CORE_DESIGN and T1/T3 as directed.

### Finding 1 — Missing disposition rule for a complete-but-unacknowledged tail batch, and for reopening a clean journal after crash
**Where:** lines 60–66 and 76–83.
**Trigger:** crash after the commit frame's bytes are written but before (or during) the sync call — one of the sub-plan's own enumerated red cuts (lines 153–156).
**Problem:** CORE_DESIGN line 127–128 defines committed as "crossed the selected successful sync boundary"; T1 lines 78–80 require that a recovered committed-but-unacknowledged admission "reconcile rather than automatically replay." The sub-plan says the batch "can be discovered" and that ack/dispatch are "unknowable from disk," but never states the recovery-time disposition: is the complete unacknowledged tail batch (a) published into rebuilt indexes as committed, (b) exposed as recovered-pending and withheld from publication pending explicit reconciliation, or (c) diagnostic-only? Each choice has different dedup/dispatch-count consequences. Relatedly, line 81 closes admission only for an "incomplete/damaged tail"; a file ending in a structurally complete batch of unknown sync status is neither, so the sub-plan is silent on whether a reopened clean-tail journal may resume appending/admission.
**Required behavior:** state explicitly that such a batch is recovered-unacknowledged (never silently published as committed), that index rebuild excludes it, that its attempt records enter the reconciliation set, and whether/when normal admission may resume on that file.
**Direct red case:** scripted storage retains commit bytes but reports sync never confirmed; kill child at that cut; reopen; assert the batch is not in committed output, dedup of the same invocation returns the recovered-pending state (not "not found," which would permit a duplicate effect), and external effect count stays ≤ 1.

### Finding 2 — After any crash-at-write, admission is permanently closed with no owning unit for continuation
**Where:** lines 79–83 ("Choosing a trustworthy new linked continuation is explicit later recovery work"), line 21–23 (no rotation in this unit).
**Trigger:** any torn tail or failed sync in normal operation — the expected outcome of the sub-plan's own process-crash tests.
**Problem:** "never truncate originals or silently repair" plus "no append/admission" on the damaged file plus "no automatic rotation" means the first write-side fault bricks the only journal forever. CORE_DESIGN line 151 requires admission closed "until a trustworthy continuation is chosen," but no unit in IMPLEMENTATION_PLAN (U1–U9 rows) owns choosing/creating that linked continuation. As written, a working agent cannot survive its own qualified fault model — the exact delay risk the review is asked to find.
**Required behavior:** either scope a minimal explicit continuation protocol into U1 (new journal file, directory-synced publication, header link to predecessor journal ID/final committed sequence, declared reconciliation of the old pending tail), or name the unit that owns it and state the interim operator-visible disposition.
**Direct red case:** poison the writer via injected sync failure; perform the specified continuation procedure; assert the new journal's header lineage fields, that old bytes remain readable, that dedup/issuer state carries across the link, and that admission reopens only after the new file's directory publication succeeds.

### Finding 3 — U0 `Storage` seam cannot express U1's native storage contracts
**Where:** sub-plan lines 44, 85–93 vs include/arconaut/foundation.hpp lines 304–311.
**Problem:** the sub-plan requires exclusive creation with restrictive mode, regular-file/permission checks, an exclusive writer lock, parent-directory sync on creation, and close-on-exec move-only descriptors. `Storage` exposes only `read_at`/`write_at`/`extent`/`synchronize` on an already-open file; there is no open/create/lock/directory-publication seam, so the T1-required "directory publication under the chosen backend" (TESTING_PLAN lines 71–72) and the lock-contention red case (sub-plan line 159–160) have no interface to be red against. Sub-plan line 145 promises red cases "against missing production APIs," but which APIs are missing is not pinned.
**Required behavior:** the sub-plan should name the concrete additional interface (creation flags, lock acquisition, directory-sync call) as a U0-review resolution item or a U1-owned adapter, with its error mapping onto `ErrorCode`.
**Direct red case:** open with an existing file (exclusive-creation failure must be an error, never truncation), two openers contending the lock, and creation where the file sync succeeds but the parent-directory sync fails — all asserted through the named seam, not an ad-hoc test helper.

### Finding 4 — Issuer counter rebuild rule is asserted but not specified
**Where:** lines 118–125.
**Trigger:** restart after issuing identities.
**Problem:** "counter … rebuilds on restart" does not say from what. The only available source is scanning committed semantic reservation records for the maximum counter, but that rule (and its interaction with Finding 1's unacknowledged reservations) is unstated. A rebuild that misses a committed-but-unacknowledged reservation reissues an identity, violating "non-reuse within that retained environment is exact."
**Required behavior:** state that rebuild takes max counter over all committed reservation records, and define the treatment of recovered-unacknowledged reservations (counter must be treated as consumed, since the identity may have escaped before the crash).
**Direct red case:** reserve ID counter = k, commit written, crash before sync/ack; restart; issue a new ID; assert counter > k and that byte-equal re-presentation of the escaped identity is recognized as existing, never reissued.

### Finding 5 — Cross-batch dependency check has no specified check point or encoding
**Where:** lines 56–58 vs lines 96–100.
**Problem:** "Dependencies reference only an earlier committed sequence or a source in the same batch" is a journal-level invariant, but dependency references live inside semantic payloads whose codecs are deferred ("their own versioned bounded codecs"). Recovery's report causes (line 61–62) are framing-level plus "semantic conflict," yet nothing states where the missing-earlier-source check executes (framing recovery vs ledger index rebuild) or which payload field carries the reference. The red case "valid later commit behind damaged earlier source" (line 148–149) cannot have a defined oracle until the check's location and the withheld-publication rule are named.
**Required behavior:** declare that dependency references are a required, decodable field of semantic frames (not opaque payload), checked during committed-record replay before publication, with failure yielding "semantic conflict" and the dependent batch withheld while the source bytes stay readable.
**Direct red case:** damage an earlier source frame while preserving a structurally valid dependent batch and commit; reopen; assert the dependent batch publishes nothing, the report names the missing sequence, and no scan-forward adopts later material.

### Minor
- **Capacity vs poison asymmetry (lines 22–23, 69–74, 79):** a clean at-capacity rejection should be distinguished from I/O-error poisoning (the former need not poison the writer or require continuation); currently only the error path's poison is described. Red case: batch that exactly fills and one that exceeds the 512 MiB limit — admission closes visibly, prior committed state fully readable, no false corruption report.
- **Conceptual fragmentation is otherwise well controlled:** complaint/emergency scope (lines 127–141), no second persistence system, and the single-journal bounded design all match the plan; no unnecessary-work findings beyond Finding 2's missing continuation owner.

**Limitations:** review of documents and the U0 header only; no implementation exists to execute; the referenced grounding papers (refit-custody, audit-state studies) were not re-derived here, so format choices were checked for internal and cross-document consistency, not against those external sources.
