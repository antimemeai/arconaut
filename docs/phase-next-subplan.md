# Phase Next Subplan: OAuth Polish & Adaptive Token Refresh

**Issue:** arconaut-61g  
**Scope:** Login UX polish, background token refresh with adaptive intervals, cross-process locking, rejected-token tombstones.  
**MATERIA discipline:** JSMNTL cycle — subplan → conformance spec → red tests → implementation → green tests → review → repeat.

---

## 0. Strategic Overview

The OAuth Device Authorization Grant shipped in the auth module is functionally complete but rough around the edges. This subplan polishes it to production quality across two tracks:

- **Track B (Login UX):** Make `arconaut login kimi` feel professional. Spinner, timeouts, model discovery, auto-config.
- **Track A (Background Refresh):** Make tokens stay fresh without blocking the user. Adaptive intervals, cross-process locks, tombstones.

**Execution order:** Track B first (fast visible wins), then Track A (load-bearing infrastructure).

---

## 1. Track B: Login UX Polish

### 1.1 Spinner / Progress Indicator

Replace dot-polling with a proper spinner:

```
$ arconaut login kimi
Opening browser... ✓
Waiting for authorization... ⠋
```

Use a simple spinner: `['⠋', '⠙', '⠹', '⠸', '⠼', '⠴', '⠦', '⠧', '⠇', '⠏']`
Render on a single line, overwrite in place with `\r`.

### 1.2 `--no-browser` Flag

Skip `open` command. Just print the URL.

```
$ arconaut login kimi --no-browser
Please visit the following URL to authorize arconaut:
  https://auth.kimi.com/activate?user_code=ABCD-EFGH
```

### 1.3 Timeout Handling

- **Device code expiry:** If `poll_token` returns `DeviceExpired`, restart the flow once. If it expires a second time, fail with a clear message.
- **Max polling duration:** Cap total polling time at 10 minutes (generous; most users authorize within 30s). Fail with "Authorization timed out. Please try again."

### 1.4 Post-Login Model Discovery

After receiving the access token:

1. `GET https://api.kimi.com/coding/v1/models` with `Authorization: Bearer {token}`
2. Parse response into `ModelInfo { id, context_length, display_name }`
3. Print count and default model

```
✓ Logged in to Kimi successfully.
✓ Fetched 12 models (default: kimi-k2-32k)
```

### 1.5 Auto-Update vars.toml

After successful login:

1. Locate `~/.config/arconaut/vars.toml`
2. If file exists, backup to `vars.toml.bak.{timestamp}`
3. Read existing content
4. Insert or update `[provider.moonshot]` section:
   ```toml
   [provider.moonshot]
   oauth = true
   api_key = ""  # fallback if OAuth fails
   model = "moonshot-v1-8k"  # or discovered default
   ```
5. Preserve all other config
6. Write atomically (temp file + rename)

---

## 2. Track A: Background Token Refresh

### 2.1 Adaptive Refresh Intervals

Refresh check frequency drops with user inactivity:

| Time Since Last Prompt | Check Interval |
|------------------------|---------------|
| < 5 minutes | 60 seconds |
| < 1 hour | 180 seconds |
| < 2 hours | 600 seconds |
| ≥ 2 hours | Stop background checks; refresh on-demand |

The `last_activity` timestamp is updated on every `SoulCommand::UserInput`.

### 2.2 Cross-Process File Lock

Prevent two arconaut processes from racing to refresh the same token.

```rust
pub struct FileLock {
    path: PathBuf,
    #[cfg(unix)]
    fd: std::os::fd::RawFd,
}
```

- `try_lock(path)` — acquire exclusive lock via `fcntl(fd, F_SETLK, ...)`
- `unlock()` — release lock, close fd
- Non-blocking: if lock is held, caller retries after a short delay

### 2.3 Rejected Token Tombstones

If the server returns 401 on refresh, mark the refresh token as rejected for 5 minutes.

```rust
pub struct RejectedTokenTombstone {
    refresh_token_hash: String,
    retry_after: Instant,
}
```

Process-wide static `HashMap<String, RejectedTokenTombstone>`. Cleared when the on-disk token's refresh_token differs from the rejected one (another process successfully rotated, or user re-logged in).

### 2.4 Sleep/Wake Detection

If the background task wakes and finds that significantly more time has elapsed than the expected interval (e.g., 2x), force an immediate refresh check. This handles laptop sleep/wake cycles.

### 2.5 Integration into Soul Runtime

- Spawn background task in `run_soul` if the primary provider is a MoonshotProvider with OAuth enabled
- Pass `last_activity: Arc<Mutex<Instant>>` — updated by the user input handler
- Graceful shutdown via `tokio::sync::watch::channel`

---

## 3. Dependencies

No new workspace dependencies. All work uses existing crates:
- `tokio` — async tasks, channels, sleep
- `reqwest` — model discovery HTTP call
- `toml` — vars.toml parsing and writing
- `chrono` — timestamps for backup files

---

## 4. Implementation Order

| # | Track | Task | Files | Complexity |
|---|-------|------|-------|-----------|
| 1 | B | `--no-browser` flag | `cli/src/main.rs` | Low |
| 2 | B | Spinner / progress indicator | `cli/src/main.rs` | Low |
| 3 | B | Timeout handling | `cli/src/main.rs` | Low |
| 4 | B | Model discovery | `auth/oauth.rs` + `cli/src/main.rs` | Low |
| 5 | B | Auto-update vars.toml | `cli/src/main.rs` | Low |
| 6 | A | FileLock (cross-process) | `auth/lock.rs` | Medium |
| 7 | A | Rejected token tombstones | `auth/oauth.rs` | Low |
| 8 | A | Adaptive background refresh task | `auth/refresh.rs` + `cli/src/main.rs` | Medium |

---

## 5. Open Questions

1. **vars.toml format preservation:** We parse as `toml::Table`, modify, and re-serialize. This may lose comments and formatting. Acceptable for now; comment-preserving rewrite is future work.
2. **Model discovery endpoint:** `https://api.kimi.com/coding/v1/models` (from kimi-cli reference). Verify at runtime if different.
