# OAuth Subplan: Kimi Device Authorization Grant

**Scope:** Implement browser-less OAuth login for Kimi (Moonshot) using RFC 8628 Device Authorization Grant.
**Reference:** `quarantine/kimi-cli/klips/klip-14-kimi-code-oauth-login.md` and `quarantine/kimi-cli/src/kimi_cli/auth/oauth.py`
**MATERIA discipline:** JSMNTL cycle — subplan → conformance spec → red tests → implementation → green tests → review → repeat.

---

## 0. Strategic Overview

arconaut currently authenticates to LLM providers via plaintext API keys in TOML. The Kimi (Moonshot) platform supports OAuth 2.0 Device Authorization Grant, which is safer (no API key to leak) and more convenient (browser login, auto-refresh). This subplan implements the full flow: device authorization request, user browser interaction, token polling, secure storage, refresh, and CLI integration.

The architectural thesis: **auth is a first-class machine concern**. The `arconaut-machine` crate owns the OAuth module, token storage, and refresh logic. The CLI provides the `login`/`logout` commands. Providers consume OAuth tokens transparently.

---

## 1. OAuth Flow (RFC 8628)

### 1.1 Endpoints

| Endpoint | Method | Body |
|----------|--------|------|
| `https://auth.kimi.com/api/oauth/device_authorization` | POST | `client_id=17e5f671-d194-4dfb-9706-5516cb48c098` |
| `https://auth.kimi.com/api/oauth/token` | POST | `client_id=...&device_code=...&grant_type=urn:ietf:params:oauth:grant-type:device_code` |
| `https://auth.kimi.com/api/oauth/token` | POST | `client_id=...&refresh_token=...&grant_type=refresh_token` |

### 1.2 Device Headers

All token requests must include device identification headers:

```
X-Msh-Platform: arconaut
X-Msh-Version: 0.1.0
X-Msh-Device-Name: {hostname}
X-Msh-Device-Model: {os} {version} {arch}
X-Msh-Os-Version: {os_version}
X-Msh-Device-Id: {stable_uuid}
```

Device ID is a stable UUID persisted to `~/.config/arconaut/device_id` (perms 0600).

### 1.3 Flow

```
1. CLI: arconaut login kimi
2. POST device_authorization → receive user_code, device_code, verification_uri_complete
3. Print URL to terminal; optionally open browser
4. Poll POST token every `interval` seconds
   - 200 + access_token → success
   - error=authorization_pending → continue polling
   - error=expired_token → restart flow
5. Save tokens to storage
6. Update vars.toml: [provider.moonshot] oauth = true
```

---

## 2. Module Design

### 2.1 Crate Layout

```
arconaut-machine/src/
├── auth/
│   ├── mod.rs          # Public API: OAuthFlow, OAuthToken, CredentialStorage
│   ├── oauth.rs        # Device Authorization Grant implementation
│   ├── storage.rs      # Token persistence (file + keychain)
│   └── device.rs       # Device info headers, stable device ID
```

### 2.2 Types

```rust
pub struct OAuthToken {
    pub access_token: String,
    pub refresh_token: String,
    pub expires_at: Option<DateTime<Utc>>,
    pub scope: String,
    pub token_type: String,
}

pub struct DeviceAuthorization {
    pub user_code: String,
    pub device_code: String,
    pub verification_uri: String,
    pub verification_uri_complete: String,
    pub expires_in: Option<u64>,
    pub interval: u64,
}

pub struct KimiOAuthFlow {
    client: reqwest::Client,
    oauth_host: String,
    client_id: String,
}

pub trait CredentialStorage: Send + Sync {
    fn load(&self, key: &str) -> Result<Option<OAuthToken>, StorageError>;
    fn save(&self, key: &str, token: &OAuthToken) -> Result<(), StorageError>;
    fn delete(&self, key: &str) -> Result<(), StorageError>;
}
```

### 2.3 Storage Strategy

**Phase 1 (this subplan):** File-based storage only.
- Directory: `~/.config/arconaut/oauth/`
- File: `{key}.json` with permissions 0600
- JSON format: `{"access_token":"...","refresh_token":"...","expires_at":"...","scope":"...","token_type":"..."}`

**Future:** Add `KeychainStorage` using `security-framework` (macOS) / `secret-service` (Linux).

### 2.4 Token Refresh

**Phase 1 (this subplan):** Refresh at provider request time.
- Before each LLM call, check if token is expired or within 5 min of expiry
- If so, refresh and save new token
- Update provider's API key in-place

**Future:** Background refresh task, cross-process file locking.

---

## 3. Provider Integration

### 3.1 MoonshotProvider with OAuth

```rust
pub struct MoonshotProvider {
    inner: OpenAiCompatClient,
    oauth: Option<KimiOAuthFlow>,
    storage: Option<Arc<dyn CredentialStorage>>,
}
```

When OAuth is configured:
1. Load token from storage
2. If expired/close-to-expiry, refresh
3. Use `access_token` as Bearer token for LLM calls

### 3.2 VariableStore Integration

```toml
[provider.moonshot]
oauth = true
# api_key is optional when oauth = true; used as fallback
```

`ProviderFactory::create_named` checks for `oauth = true` in the config. If set, wraps the provider with OAuth support.

---

## 4. CLI Integration

### 4.1 Subcommands

```bash
arconaut login <provider>     # Start OAuth device flow
arconaut logout <provider>    # Clear OAuth credentials
```

### 4.2 UX

```
$ arconaut login kimi
Please visit the following URL to authorize arconaut:
https://auth.kimi.com/activate?user_code=ABCD-EFGH

Waiting for authorization...
✓ Logged in to Kimi successfully.
```

---

## 5. Implementation Order

| # | Task | Files | Complexity |
|---|------|-------|-----------|
| 1 | Device info module | `auth/device.rs` | Low |
| 2 | OAuth token types + serialization | `auth/oauth.rs` (types) | Low |
| 3 | File-based credential storage | `auth/storage.rs` | Low |
| 4 | Device Authorization Grant flow | `auth/oauth.rs` (flow) | Medium |
| 5 | Token refresh | `auth/oauth.rs` (refresh) | Medium |
| 6 | Wire OAuth into MoonshotProvider | `providers/moonshot.rs` | Low |
| 7 | CLI login/logout commands | `cli/src/main.rs` | Medium |
| 8 | Tests | `auth/*` | Medium |

---

## 6. Dependencies

**New dependencies for `arconaut-machine`:**
- `chrono` (already in workspace) — for `expires_at` timestamps
- `dirs` — for `~/.config/arconaut` path resolution
- `uuid` (already in workspace) — for device ID generation

**No keyring dependency for Phase 1** — file storage only.

---

## 7. Open Questions

1. Should we support OAuth for providers other than Kimi? (Decision: Kimi only for now.)
2. Should `api_key` in vars.toml be removed after OAuth login? (Decision: keep as fallback, mark with comment.)
3. Should we auto-detect OAuth vs API key at provider creation? (Decision: explicit `oauth = true` flag.)
