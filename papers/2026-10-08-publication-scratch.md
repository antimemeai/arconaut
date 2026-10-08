# Bounded publication scratch reclamation

First delivery for arconaut-m9l addresses abandoned scan-checkpoint, saved-state and archive-catalog temporary files. Bulk catalog renewal and ledger-only resident-tail bounds are unchanged.

## Mechanism

NativeJournalDirectory::reclaim_publication_scratch requires the caller's exclusive journal writer lease and no publication in flight. Native callers invoke it after FramedJournal reopen completes physical recovery, before scan-checkpoint publication, and before saved-current-state publication. Cleanup failure is best-effort and does not undo or gate recovery/publication. Other directory adapters default to unsupported.

One pass inspects at most128 directory entries and removes at most16 files. It does not traverse children or collect an unbounded filename list. Eligible names are exactly this journal's reserved numeric temporary forms:

- <journal>.scan.tmp.<sequence>.<clock>.<attempt>
- <journal>.state.[01].tmp.<clock>
- <journal>.archive.[01].tmp.<clock>

Every numeric component is nonempty, decimal and at most20characters. No other prefix, suffix or selected-generation name qualifies.

Candidates must be regular, single-link, owned by the effective user and inaccessible to group/other users. Symlinks, directories, hardlinks, permissive files and other journal names are preserved. Cleanup opens with no-follow, takes a nonblocking exclusive candidate lock, checks descriptor/path identity, and skips the ARCOJ001 journal signature even if the remainder is damaged. This protects a live or idle actual journal whose name happens to resemble scratch. The primary journal lease excludes a conforming publication from concurrently using its scratch namespace; this is not containment of hostile same-user filesystem mutation.

Deletion has no foreground directory fsync: a crash may restore a scratch directory entry, which a later pass can reclaim. Selected generation/catalog/scan files and journal payloads are not removed. There is no historical-byte preservation requirement behind this cleanup.

## Direct checks

Release build/release/blackbird built. scratch_cleanup, journal_checkpoint and saved_state pass on the one recheck; git diff --check passes. Direct native cases cover precise name matching, repeat cleanup, scan/removal caps, invalid limits/path, selected filenames, unrelated names, other active journals, locked scratch, active and idle journals with scratch-looking names, symlink/directory/hardlink/mode exclusions, and a forked process that creates a temporary then exits without C++ cleanup. The subsequent actual FramedJournal reopen removes that abandoned temporary under its new writer lease.

The one source recheck found that filename matching alone could misclassify a different journal with a reserved-looking filename. Nonblocking candidate locking and journal-signature preservation were added and those cases tested within that same recheck. No third assurance layer or unrelated full suite.

## Explicit remaining limits

This is bounded best-effort reclamation, not a complete disk-growth bound. Every pass begins a fresh directory enumeration; debris beyond the128-entry window can remain indefinitely behind unrelated entries. Removal errors and unsupported adapters do not expose a native operator warning yet. The counters/complete flag are available from the directory API but routine callers currently discard them. A subsequent bounded-maintenance design should choose a resumable cursor, explicit maintenance action or a fixed-name publication strategy rather than an unbounded startup sweep.

Catalog renewal still bulk-merges metadata, and arbitrary ledger-only workloads or persistent publication failures can still grow resident tails. arconaut-m9l remains in progress. No claim of full failure-publication storage cleanup or arbitrary-history constant maintenance cost.

Unit bound35minutes; hardening at most20minutes, implementation/direct checks then one recheck and its fixes. Previous context/request/capture units were not re-reviewed.

## Remaining maintenance implementation (2026-10-08)

Native cleanup now retains its directory stream between calls, closes at EOF, and
starts a fresh cycle on the next call. Changing journals resets the cursor. Each
pass keeps the 128-entry/16-removal limits and prior candidate safety checks. The
last pass counters and error are available on NativeJournalDirectory; a changed
cleanup failure emits a concise native warning. Directory enumeration progress is
process-local: restarting after fewer than a full cycle restarts its scan.

Catalog publication now merges the selected immutable catalog with sorted suffix
locators one entry at a time; it retains one old entry and one 128-entry output
batch. The suffix map remains proportional to the resident suffix. Catalog renewal
still rewrites O(total keys) disk bytes: this change removes the history-sized RAM
map and extra vector, not all historical I/O. Publication preserves old generation
reader ownership and validates monotonic keys and locator bounds while streaming.

For compact recovery, ContextStore supplies a checkpoint callback before fresh
ordinary admission once the suffix reaches 128 resident records or 8MiB of journal
bytes. Failed maintenance is visible through maintenance_error(); fresh admission
stops at 1024 records or 32MiB, including admitted settlement headroom. Existing
settlement remains permitted. Ledger-only ordinary workloads therefore trigger the
same maintenance as context edits. Explicit ContextStore::checkpoint() retries
after repair. Legacy full-replay sessions retain their configured physical ceiling
until migrated into a supported compact session; no unlimited-history behavior is
introduced for them.

Direct checks added: old/new duplicate locator merge and immutable readers, debris
beyond 300 unrelated entries across bounded passes, ledger-only auto checkpoint,
and persistent checkpoint failure refusing a fresh append without cursor advance.
