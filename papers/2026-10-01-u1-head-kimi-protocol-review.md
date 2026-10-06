# Independent review: docs/U1_ENVIRONMENT_HEAD_SUBPLAN.md

Inspected: AGENTS.md, the two U1 sub-plans, `include/arconaut/journal_storage.hpp`, `include/arconaut/journal.hpp`, `src/journal_storage.cpp`, `include/arconaut/foundation.hpp` (Storage/SyncStrength/ErrorCode). Format arithmetic checks out: selector = 8+16+16+16+8+4+4 = 72 bytes, CRC over bytes 0..67; `head.<32 lowercase hex>` matches a 16-byte journal ID. The plan's failure-closure, no-fallback, and lock-before-read discipline is internally consistent with the existing U1 storage primitives (exclusive create, busy/conflict codes, checked `valid_io`, directory sync seam).

## Prioritized findings

### P1 — protocol defects / ambiguities to fix before red cases

1. **Generation/active-ID consistency is not an explicit reject rule.** Lines 33–34 state "Generation 1 selects root; later generations select non-root" as *selection semantics*, but the reject list ("unknown sizes/magic/bits/CRC, zero fields, wrong environment/root") never requires `generation==1 ⇒ active==root` or `generation>1 ⇒ active!=root`. Trigger: on-disk selector with generation=1 and active≠root (or generation=2, active==root) — is it a corrupt-head failure or accepted? Consequence: two implementers diverge on whether a manufactured or damaged selector is honored. Fix: add both mismatches to the reject list. Red vectors: `gen=1, active≠root` and `gen=2, active==root`, both CRC-valid, must fail closed with no I/O.

2. **Generation exhaustion overflow is implied, not specified.** "checked generation+1" (line 39) plus "IDs/generation are nonzero" (line 33) together imply refusal at `UINT64_MAX`, since +1 wraps to 0. Make it explicit that expected-generation `UINT64_MAX` refuses replace before any I/O. Red vector: head with `generation=0xFFFF…FF`, valid CRC → replace refuses, no temp create, health intact.

3. **Outcome of "rename succeeded, directory sync failed/absent" is unspecified.** Lines 53–58 cover "any error after the first attempted selector write closes transitions" and "reopen reads the actual selector", but oracle #4 (line 87) only states SIGKILL *directly before/after replacement* selects exactly old/new. The dir-sync-failure cut (rename durable-bytes renamed, dir entry durability unknown) has no declared expectation: reopen must select the **new** identity (the bytes on disk) while the old cache stays unavailable, and the owner stays closed. Without stating this, an implementer could treat dir-sync failure as "rename didn't count" and try to preserve the old identity — a competing-branch violation. Add this cut explicitly to oracles #3 and #4.

4. **EINTR policy on lock acquisition and selector reads is unspecified.** Native `lock_writer` maps EINTR → `interrupted` (journal_storage.cpp:116–124), and `read_at`/`pread` can return EINTR or a short count. The plan mandates "checked short/EINTR I/O" only for the temp *write* (line 47). Trigger: SIGALRM/other signal during `flock` or the 72-byte selector read on open → spurious create/open failure against a healthy environment, or an unhandled short read misinterpreted as corruption. Fix: state that open/create retries EINTR on lock and read (bounded), and that extent==72 with a short read is an error, never a fallback. Red inputs: injected EINTR once then success; extent 72, read returns 40.

5. **Mac full-strength directory sync is assumed, not qualified.** Line 48 requires directory sync "at that strength"; journal_storage.cpp:28–44 maps `full` → `F_FULLFSYNC` via `fcntl` on whatever fd it is given, including the directory fd. `F_FULLFSYNC` behavior on directory fds on the installed Apple SDK is exactly the kind of grounding gap the plan elsewhere says to check (line 15: "Check the installed Apple SDK declarations"). If it returns EINVAL on directories, full-strength replace is broken on a declared profile. Add an explicit native check item under #4/#5: full-strength `synchronize_directory` on Mac must be observed succeeding, or the strength mapping for directories re-specified.

### P2 — gaps worth a decision line

6. **Reader semantics under the lifetime-exclusive lock.** "Fresh open under the permanent lock" (line 56) with the lock held "for its entire lifetime" (line 23) means any second process — including a pure diagnostic reader — gets `busy`, not the current selector. That may be intended, but oracle #4 only tests *writer* exclusion. State whether a read-only non-locked inspection path exists (reading `head` is safe against rename atomicity) or that busy-for-readers is the contract.

7. **`replace_file` allows absent destination; the owner should require it.** The seam (lines 65–70) permits renaming onto a missing `head`, which would silently *create* a selector in a directory whose head was lost — precisely the "missing head never falls back" case, reintroduced through the side door. One line: the EnvironmentHead owner must require an existing restricted regular `head` before replace. Primitive generality is fine; owner responsibility should be assigned.

8. **Full-selector CAS byte-exactness.** "Exact expected selector" should be defined as exact 72-byte equality (including reserved and CRC bytes), not decoded-field equality, so a caller-constructed selector with re-serialized-but-different reserved bits cannot pass. Red vector: expected selector differing only in CRC/reserved encoding → refusal, no I/O.

9. **Stale temp liveness after refusal.** Collision refusal (lines 49–51) is correct, but note the consequence explicitly: a stale temp from a crashed prior attempt permanently blocks that journal ID for replace, and continuation must mint a fresh identity. Oracle #3 covers "preexisting temporary files remain untouched"; add the assertion that the refusal carries `conflict` and leaves health/cache intact.

### Fairness assessment (primitive vs. owner)

The responsibility split is fairly drawn. The primitive owns structural validation, bounded filenames, ordered I/O, uncertainty-closure and no-rollback; the continuation owner owns journal-before-replace ordering, predecessor validation and namespace freshness — and the plan honestly states the primitive *cannot* enforce that ordering (lines 40–43), accepting the misuse window rather than pretending otherwise. The trusted-local-directory profile (lines 17–19) legitimately scopes out adversarial rename-by-others that would defeat the authority-lock CAS; no extra ceremony needed there. Existing natives suffice: `create_exclusive` (conflict), `lock_writer` (busy), `synchronize_directory`, `valid_io` bounds checks, and close-after-EINTR discipline (journal_storage.cpp:64) all match the plan's needs. The new `replace_file` with default `Unsupported` is the right seam; its check-then-rename TOCTOU is acceptable within the declared profile since `renameat` itself never follows a destination symlink.

### Suggested red inputs (consolidated)

- CRC-valid selectors: gen=1/active≠root; gen=2/active==root; gen=UINT64_MAX (replace attempt); generation=0; environment/root mismatch vs. authority; trailing byte; size 71/73.
- Scripted byte-file: fail at temp-create (EEXIST collision), temp-write short/EINTR, temp-sync, rename error, dir-sync error → after-rename cuts must reopen to the **new** identity; pre-rename cuts reopen to old; health preserved only for pre-I/O failures (bad expectation, invalid IDs, collision, pre-I/O bad_alloc).
- Injected single EINTR then success on flock and on selector read; extent 72 with short read.
- Native: SIGKILL (a) after temp sync before rename, (b) after rename before dir sync, (c) after dir sync — reopen selects old, new, new respectively; stale temp preserved; writer exclusion held before and after replace; Mac full-strength directory sync observed.
- Preexisting `head.<hex>` evidence file and an unrelated `head.*` foreign file: untouched, refusal is `conflict`, no selector write.

### Limitations

I did not read the quarantined legacy code, JOURNAL.md/CORE_DESIGN.md, or any existing tests; findings are scoped to the listed files. I did not execute anything; the Mac `F_FULLFSYNC`-on-directory question (finding 5) is flagged as an oracle obligation, not a confirmed defect.