# Phase Next Conformance Specification

**Issue:** arconaut-61g  
**Companion:** Phase Next Subplan

---

## Track B: Login UX

| # | Assertion | Test | Oracle |
|---|-----------|------|--------|
| B.1 | `--no-browser` flag exists and suppresses browser open | `cli::tests::no_browser_flag` | Gold |
| B.2 | Spinner renders without panicking during poll loop | `cli::tests::spinner_no_panic` | Gold |
| B.3 | Device code expiry restarts flow once, then fails | `cli::tests::expiry_restart_then_fail` | Gold |
| B.4 | Max polling timeout (10 min) aborts with clear message | `cli::tests::polling_timeout` | Gold |
| B.5 | Model discovery parses response into ModelInfo list | `oauth::tests::model_discovery_parse` | Gold |
| B.6 | vars.toml backup is created before modification | `cli::tests::vars_backup_created` | Silver |
| B.7 | vars.toml preserves unrelated sections after update | `cli::tests::vars_preserve_other_sections` | Silver |

## Track A: Background Refresh

| # | Assertion | Test | Oracle |
|---|-----------|------|--------|
| A.1 | FileLock::try_lock returns Some when uncontended | `lock::tests::lock_uncontended` | Silver |
| A.2 | FileLock::try_lock returns None when held by another fd | `lock::tests::lock_contended` | Silver |
| A.3 | RejectedTokenTombstone blocks retry for 5 minutes | `oauth::tests::tombstone_blocks_retry` | Gold |
| A.4 | Tombstone is cleared when on-disk token changes | `oauth::tests::tombstone_cleared_on_new_token` | Gold |
| A.5 | Adaptive interval: active (<5m) → 60s | `refresh::tests::interval_active` | Gold |
| A.6 | Adaptive interval: idle (≥2h) → stops background checks | `refresh::tests::interval_idle_stops` | Gold |
| A.7 | Sleep/wake detection forces immediate refresh check | `refresh::tests::sleep_wake_forces_check` | Gold |

---

## Oracle Tiers

- **Gold:** Deterministic, no external deps, no network I/O, no process spawning. Mock clocks, in-memory structures, temp files.
- **Silver:** Requires filesystem operations or process spawning.

**All Gold tests must pass before merge.** Silver tests must pass before release.

## Test Inventory Summary

| Component | Gold | Silver | Total |
|-----------|------|--------|-------|
| Login UX | 5 | 2 | 7 |
| Background Refresh | 5 | 2 | 7 |
| **Total** | **10** | **4** | **14** |
