# Phase Next: OAuth Polish & Token Refresh

**Status:** Planning  
**Scope:** Hard polish on the OAuth implementation shipped in the auth module. Background refresh, login UX, model discovery, credential security.

---

## 0. Strategic Overview

The OAuth Device Authorization Grant is wired and working, but it operates at "it compiles and the tests pass" quality. Phase next brings it to "I would trust this with my production credentials" quality. The gaps are all in the long-tail of OAuth: what happens when a session runs for 6 hours, when two terminals are open, when the user sleeps their laptop, when the network flakes during refresh.

The architectural thesis: **OAuth is a distributed systems problem dressed up as auth**. Token state lives in three places: the OAuth server, the filesystem, and process memory. Keeping them consistent requires locks, tombstones, and defensive timeouts.

---

## 1. Background Token Refresh

### 1.1 Problem

Current behavior: token is refreshed at request time (before each LLM call). If the network is slow or the refresh fails, the user waits.

### 1.2 Design

```rust
pub struct TokenRefreshTask {
    flow: KimiOAuthFlow,
    storage: Arc<dyn CredentialStorage>,
    interval: Duration,          // check every 60s
    threshold: Duration,         // refresh when < 5 min to expiry
}

impl TokenRefreshTask {
    pub async fn run(&self, shutdown: tokio::sync::watch::Receiver<bool>);
}
```

**Integration into `run_soul`:**
- Spawn background task on session start if OAuth is configured
- Task sleeps, checks expiry, refreshes if needed
- On sleep/wake detection (elapsed >> interval), force refresh
- Graceful shutdown on session end

### 1.3 Cross-Process Coordination

Two `arconaut` terminals should not race to refresh the same token. kimi-cli solves this with `fcntl.flock` on a lockfile. We need the same in Rust:

```rust
pub struct FileLock {
    path: PathBuf,
    #[cfg(unix)]
    fd: Option<std::os::fd::RawFd>,
}

impl FileLock {
    pub fn try_lock(path: PathBuf) -> Result<Option<Self>, io::Error>;
    pub fn unlock(self);
}
```

**Refresh protocol:**
1. Check in-memory cache
2. Load from disk (another process may have refreshed)
3. Acquire file lock
4. Re-load from disk (double-check)
5. If still expired, perform HTTP refresh
6. Save to disk, release lock, update cache

### 1.4 Tombstones for Rejected Refresh Tokens

If the server rejects a refresh token (401), we must not retry with the same token for 5 minutes. Otherwise we hammer the server and shadow any fallback API key.

```rust
pub struct RejectedTokenTombstone {
    refresh_token_hash: String,
    retry_after: Instant,
}
```

Process-wide `HashMap<String, RejectedTokenTombstone>`, cleared when the on-disk token changes.

---

## 2. Login UX Polish

### 2.1 Current Behavior

```
Starting Kimi OAuth login...
Please visit the following URL to authorize arconaut:
  https://auth.kimi.com/activate?user_code=ABCD-EFGH

............
✓ Logged in to Kimi successfully.
```

### 2.2 Target Behavior

```
╭────────────────────────────────────────╮
│  arconaut login kimi                   │
│                                        │
│  1. Opening browser...                 │
│  2. Waiting for authorization...       │
│     (visit https://... if it doesn't)  │
│                                        │
│  [spinner] Polling...                  │
│                                        │
│  ✓ Authorized as patrick@massmind.ai   │
│  ✓ Fetched 12 models                   │
│  ✓ Saved credentials                   │
│  ✓ Updated ~/.config/arconaut/vars.toml│
╰────────────────────────────────────────╯
```

### 2.3 Specific Improvements

| # | Improvement | File |
|---|-------------|------|
| 1 | Spinner / progress indicator during polling | `cli/src/main.rs` |
| 2 | Show user identity after successful auth (if API provides it) | `cli/src/main.rs` |
| 3 | Fetch and display available models post-login | `cli/src/main.rs` + `auth/oauth.rs` |
| 4 | Auto-update `vars.toml` with `oauth = true` (backup old file first) | `cli/src/main.rs` |
| 5 | Better error messages: distinguish network vs auth vs expired | `auth/oauth.rs` |
| 6 | Timeout handling: device code expiry, max polling duration | `cli/src/main.rs` |
| 7 | `--no-browser` flag for headless environments | `cli/src/main.rs` |

### 2.4 Auto-Config Update

After successful login:

```rust
fn update_vars_toml(home: &Path) -> Result<(), io::Error> {
    let path = home.join(".config").join("arconaut").join("vars.toml");
    // Backup existing file: vars.toml.bak.{timestamp}
    // Read existing, insert/update [provider.moonshot] oauth = true
    // Preserve all other config
    // Write atomically
}
```

---

## 3. Secure Credential Storage

### 3.1 Current

OAuth tokens stored in `~/.config/arconaut/oauth/kimi.json` with `0o600` permissions.

### 3.2 Target

**Tier 1: OS Keychain**
- macOS: `security` CLI or `security-framework` crate
- Linux: `secret-service` crate (D-Bus)
- Windows: `windows` crate (Credential Manager)

**Tier 2: File fallback** (current implementation)

**Tier 3: TOML fallback** (for environments with no keychain and no home dir)

```rust
pub enum StorageBackend {
    Keychain,
    File,
}

pub struct TieredStorage {
    primary: Option<Box<dyn CredentialStorage>>,
    fallback: FileStorage,
}
```

---

## 4. Model Discovery Post-Login

kimi-cli fetches the models list after login and writes it to config. We should do the same:

```rust
async fn fetch_models(access_token: &str) -> Result<Vec<ModelInfo>, ProviderError> {
    // GET https://api.kimi.com/coding/v1/models
    // Authorization: Bearer {access_token}
}
```

This lets us:
- Validate the token works before declaring success
- Show the user which models they have access to
- Auto-set a default model

---

## 5. Implementation Order

| # | Task | Files | Complexity | Blockers |
|---|------|-------|-----------|----------|
| 1 | FileLock (cross-process) | `auth/lock.rs` | Medium | None |
| 2 | Rejected token tombstones | `auth/oauth.rs` | Low | None |
| 3 | Background refresh task | `auth/refresh.rs` + `cli/src/main.rs` | Medium | #1–2 |
| 4 | Login UX: spinner, timeout, --no-browser | `cli/src/main.rs` | Low | None |
| 5 | Post-login model discovery | `auth/oauth.rs` + `cli/src/main.rs` | Low | None |
| 6 | Auto-update vars.toml | `cli/src/main.rs` | Low | None |
| 7 | Secure storage: macOS keychain | `auth/storage.rs` | Medium | None |
| 8 | Secure storage: Linux secret-service | `auth/storage.rs` | Medium | #7 |
| 9 | Secure storage: Windows | `auth/storage.rs` | Low | #7–8 |

**Parallel tracks:**
- Track A (Refresh): #1–3
- Track B (UX): #4–6
- Track C (Security): #7–9

---

## 6. Conformance Spec (Sketch)

| # | Assertion | Test | Oracle |
|---|-----------|------|--------|
| 3.1 | Background refresh updates token before expiry | `refresh::tests::proactive_refresh` | Gold (mock clock) |
| 3.2 | Cross-process lock prevents double-refresh | `lock::tests::exclusive_lock` | Silver (spawn process) |
| 3.3 | Tombstone prevents retry of rejected token | `oauth::tests::tombstone_blocks_retry` | Gold |
| 3.4 | Sleep/wake detection forces refresh | `refresh::tests::sleep_wake_refresh` | Gold (mock clock) |
| 4.1 | `--no-browser` skips browser open | `cli::tests::no_browser_flag` | Gold |
| 4.2 | Login timeout after device code expiry | `cli::tests::login_timeout` | Gold |
| 4.3 | vars.toml backup created on update | `cli::tests::vars_backup` | Silver |
| 7.1 | Keychain round-trip save/load | `storage::tests::keychain_roundtrip` | Silver (macOS only) |

---

## 7. Open Questions

1. **Background refresh interval:** 60s default? kimi-cli uses 60s. Too aggressive for a CLI that may idle for hours.
2. **Keychain crate choice:** `keyring-rs` is cross-platform but adds Linux D-Bus deps. `security-framework` is macOS-only but lighter. Decision: try `keyring-rs` first, fall back to platform-specific crates if binary bloat is bad.
3. **vars.toml backup retention:** Keep 1 backup? 5? Rotate? Decision: keep 1 backup suffixed with `.bak.{timestamp}`.
4. **Model discovery on every login or once?** kimi-cli does it every login. We should too — the user's model access may have changed.
