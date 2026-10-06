# Verification: revised docs/U1_ENVIRONMENT_HEAD_SUBPLAN.md

Re-read the revised sub-plan in full and the claimed native oracle (`tests/journal_native_test.cpp:58–66`). All nine prior findings are resolved in the concrete protocol text, not deferred to commentary. Details and residual checks below.

## Resolution verification

1. **Generation/active consistency (was P1-1)** — Resolved. Line 35: "Reject CRC-valid generation/active mismatches as corrupt too," adjacent to the reject list so both directions (gen=1/active≠root, gen>1/active==root) are decode-time failures.

2. **Exhaustion (was P1-2)** — Resolved. Line 39: "UINT64_MAX refuses without I/O and leaves health intact" — refusal point and health consequence both explicit.

3. **After-rename uncertainty (was P1-3)** — Resolved. Lines 66–68: after successful rename followed by directory-sync failure or SIGKILL, same-host reopen observes the **new** identity though never acknowledged. This closes the competing-branch ambiguity and matches "never roll back an uncertain rename."

4. **Bounded EINTR + short I/O (was P1-4)** — Resolved, lines 72–75: retry interrupted lock/read/write/sync up to eight *consecutive* interruptions, resetting after progress; short reads/writes accumulate to the exact known size; zero-before-completion is Incomplete; impossible counts are corrupt; no failure becomes fallback. This correctly distinguishes accumulating legal short positional reads (normal pread behavior) from premature EOF (zero), and caps retry so a signal storm terminates in a visible error rather than a livelock. Excluding rename from the retry list is correct — renameat EINTR has an unknown result, and the existing "any error after the first attempted selector write closes replace" rule handles it conservatively.

5. **Mac full directory sync (was P1-5)** — Verified as claimed. `tests/journal_native_test.cpp:60` directly requires `directory.synchronize_directory(SyncStrength::full)` to succeed on the actual native directory descriptor, alongside `file->synchronize(SyncStrength::full)` (line 59). So F_FULLFSYNC-on-directory behavior is already a direct, non-injected oracle on the Mac profile, and resolution text commits to rerunning it with the new replacement tests. renameat/getentropy SDK declarations were checked per the resolution note. Acceptable as an oracle obligation.

6. **Reader semantics (was P2-6)** — Resolved. Lines 50–51: busy applies to every ordinary open; a diagnostic reader is explicitly out of component scope. No half-specified second path.

7. **Existing-destination requirement (was P2-7)** — Resolved more strongly than requested. Lines 47–49: replace first rereads the on-disk selector and requires equality with the last acknowledged selection; missing, corrupt, or changed head closes switching and is never reconstructed. This both guarantees `head` exists at mutation time and adds a genuine disk-level CAS beneath the in-memory one, closing the window where the acknowledged cache and disk diverge (e.g. prior uncertain rename). Read errors close switching until successful reopen — conservative and consistent.

8. **Typed CAS (was P2-8)** — Accepted; the resolution is sound, not evasive. Since the API accepts a typed selector struct carrying no reserved/CRC fields, and decode rejects any non-canonical encoding while encode is deterministic, decoded-field equality *is* equality over the single legal encoding. A raw-byte token would add a second representation of the same fact with no additional discriminative power. The previously suggested red vector (differing reserved/CRC encoding) is now covered at the decode layer by oracle #2's "bad magic/CRC/reserved" cases, which is where it belongs.

9. **Collision (was P2-9)** — Resolved. Lines 57–59 plus resolution note: collision is Conflict before any selector write with health intact, evidence preserved, continuation mints a fresh identity.

## Residual observations (non-blocking)

- **Open-path EINTR is outside the retry rule.** The bounded-retry list (line 72) covers lock/read/write/sync, but `openat`/`fstat` during create/open (journal_storage.cpp:174) can still return EINTR → `interrupted` → spurious create/open failure. A one-line note that interrupted native open is retried at the owner (or surfaced as transient-retryable, distinct from a real failure) would close it. Low severity: the failure is fail-closed, just noisy.
- **Extent-first vs read-first on selector load** is implicit rather than stated: "reject unknown sizes" plus accumulating reads to "the exact known size" reads naturally as check-extent-72-then-read, and oracle #2's truncation/trailing-byte vectors force whichever interpretation is chosen to reject both. Not a defect; the red cases pin it.
- Oracle #3's "wrong expectations ... cause no I/O" composes correctly with the new reread rule only if expectation-vs-cache mismatch refuses *before* the reread (expected≠acknowledged ⇒ refuse, no I/O; expected==acknowledged ⇒ reread disk). The plan text supports this reading; an implementer doing the reread unconditionally would still be safe, just noisier against the scripted oracle. Worth one clause in the red-case spec, not a plan change.

## Suitability verdict

No remaining protocol blockers. The failure model is closed at every I/O boundary (temp create/write/sync, rename, dir sync, publication), closure-vs-health-preservation is now partitioned precisely at "first attempted selector write," reopen semantics are defined for every crash cut including the previously ambiguous rename-done/dir-sync-failed case, and the claimed primitive/owner split is unchanged and still fair. The native seam (default-Unsupported `replace_file`, no fallback) plus existing natives suffice; no additional proof or metadata layer is needed. The plan is suitable to proceed to red API cases and implementation, subject to the two non-blocking notes above.

Limitations: read-only inspection of the listed scope plus the named test file; I did not execute the Mac/Linux profiles, so finding 5 remains an oracle rerun obligation, and I did not re-survey legacy quarantine material.