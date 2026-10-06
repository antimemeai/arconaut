## Rereview: F1–F3 resolution in current `tests/environment_head_test.cpp`

**Scope:** full re-read of the current test file (649 lines). Production files (`src/environment_head.cpp`, `src/journal_storage.cpp`) were not re-diffed; per the task statement no production logic changed, and the test file's expectations are consistent with the implementation I previously reviewed.

### F1 — CRC-valid crafted negatives: **resolved, and stronger than requested**

- `recalculate` (lines 66–76) is an *independent* bitwise CRC32C (reflected poly `0x82f63b78`, init/xorout `0xffffffff`), not a call into production `crc32c` — so a production CRC bug can't collude with the oracle.
- Critically, `recalculate`'s correctness is proven *by the test itself*: the owner-binding cases at lines 468–481 mutate environment/root bytes, recompute, and expect `wrong_environment`/`conflict` — errors only reachable *past* a passing CRC check and successful decode. If `recalculate` were wrong these would surface as `corrupt` and go red. This closes the masking concern completely.
- The vector set (95–113) covers: bad magic (at=0), reserved≠0 (at=64), zero environment/root/active IDs (8/24/40), zero generation (at=56), generation-1-with-non-root-active (107–109), and generation-2-with-active==root (110–113). Each previously-masked semantic check in `decode_head_selector` (`environment_head.cpp:116-134`) now has a direct red oracle: deleting the reserved check, the `from_bytes` validity check, or the `valid_selector` check turns a specific case red. Verified by tracing each vector against the decoder.
- Owner environment/root binding (468–481) additionally exercises the `wrong_environment`/`conflict` split in `read_selector` with CRC-valid on-disk bytes — previously only authority-header mismatches were tested.

### F2 — native seam oracles: **resolved**

Lines 581–600 now cover: `../a` and same-name rejection (`invalid_range`), symlink as source *and* as destination rejected, hardlink same-inode → `conflict`, successful rename preserving the open FD's contents (binary verification through `a->extent()`/`read_at`), and permissive-mode (0644) source rejected. The destination-side `restricted()` branch (`journal_storage.cpp:227-229`) is exercised via the symlink destination. The only untestable branch remains `st_uid != geteuid`, which requires another UID — acceptable for this profile.

### F3 — read-side/EINTR bounds: **resolved**

- Read zero → `incomplete`, read impossible-count → `corrupt` (435–447), hitting the previously dead branches at `environment_head.cpp:62-65`.
- Eight-consecutive-interruption exhaustion now bounded for lock, read, extent, and sync (439–446, 455–462), matching the `n == 7` bound in `interrupted()`; single-interruption recovery succeeds afterward (448–454, 463–466), proving the retry counter resets after progress.
- Native SIGKILL after directory-sync completion (cut 2, `CrashDirectory::synchronize_directory` guarded by `replacing_`, lines 549–556) is correctly ordered: the `open()`-time dirsync doesn't trip it since `replacing_` is only armed by `replace_file`. Reopen observes generation 2/active 4 for cuts 1 and 2, old identity for cut 0 (630–632).

### Residual observations (nits, not defects)

1. Destination-file permissive-mode rejection is covered only via the symlink-destination branch; a 0644 *regular* destination case would be marginally more direct but hits the same code.
2. In the fault loop (435–447), `read_interrupts` exhaustion consumes the budget on the *authority* read, so the head-selector read path's 8-interruption limit is covered by code identity (`read_exact`) rather than by a distinct case. Acceptable.
3. Sized `operator delete`/`operator delete[]` overloads (31, 34) are the correct hook-side fix for the libstdc++ sized-delete mismatch; no sanitizer suppression involved.

### Conclusion

F1, F2, and F3 are each genuinely resolved with direct red oracles traced against the production checks they protect; the F1 fix is self-validating through the post-CRC `wrong_environment`/`conflict` assertions. F4 (trusted-directory TOCTOU) and F6 (no power-loss claim) stand as explicit scoped limitations with no contradictory claim in code or docs. **Within the reviewed scope (selection-metadata component, its seam, and this test file), I find no remaining real component or oracle defect.** Remaining qualifications (final Mac/Linux profile runs of the F1-only additions, U2/U3 owner integration) are outside this component's claim, consistent with the subplan's own statement that this primitive alone neither accepts U1 nor enables U2 dependencies.