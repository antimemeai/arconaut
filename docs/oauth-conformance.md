# OAuth Conformance Specification

**Companion:** OAuth Subplan
**Scope:** Device Authorization Grant, token storage, refresh, provider integration.

---

## 1. Device Info

| # | Assertion | Test | Oracle |
|---|-----------|------|--------|
| 1.1 | `DeviceInfo::generate()` produces non-empty platform, version, name, model, os_version, device_id | `device::tests::info_has_all_fields` | Gold |
| 1.2 | `DeviceInfo::headers()` returns all 6 required headers with ASCII-safe values | `device::tests::headers_are_ascii` | Gold |
| 1.3 | `get_device_id()` returns stable UUID across calls | `device::tests::device_id_is_stable` | Gold |
| 1.4 | Device ID file is created with restrictive permissions | `device::tests::device_id_file_permissions` | Silver (fs ops) |

## 2. Token Storage

| # | Assertion | Test | Oracle |
|---|-----------|------|--------|
| 2.1 | `FileStorage::save` writes valid JSON | `storage::tests::save_load_roundtrip` | Gold |
| 2.2 | `FileStorage::load` returns `None` for missing key | `storage::tests::load_missing_returns_none` | Gold |
| 2.3 | `FileStorage::delete` removes the file | `storage::tests::delete_removes_file` | Gold |
| 2.4 | Saved token JSON contains all fields | `storage::tests::token_fields_preserved` | Gold |
| 2.5 | `OAuthToken::is_expired()` returns true when `expires_at` is in the past | `storage::tests::expired_token_detected` | Gold |
| 2.6 | `OAuthToken::needs_refresh()` returns true when within 5 min of expiry | `storage::tests::needs_refresh_near_expiry` | Gold |

## 3. Device Authorization Grant

| # | Assertion | Test | Oracle |
|---|-----------|------|--------|
| 3.1 | `KimiOAuthFlow::request_device_authorization` serializes correct POST body | `oauth::tests::device_auth_request_shape` | Gold |
| 3.2 | `KimiOAuthFlow::poll_token` sends correct grant_type and device_code | `oauth::tests::poll_request_shape` | Gold |
| 3.3 | `KimiOAuthFlow::refresh_token` sends correct grant_type and refresh_token | `oauth::tests::refresh_request_shape` | Gold |
| 3.4 | All token requests include device info headers | `oauth::tests::requests_have_device_headers` | Gold |
| 3.5 | Token response parsing handles all required fields | `oauth::tests::token_from_response` | Gold |
| 3.6 | Error responses produce descriptive `OAuthError` | `oauth::tests::error_response_parsing` | Gold |

## 4. Provider Integration

| # | Assertion | Test | Oracle |
|---|-----------|------|--------|
| 4.1 | `MoonshotProvider` with OAuth loads token from storage | `moonshot::tests::oauth_loads_token` | Gold |
| 4.2 | `MoonshotProvider` falls back to API key when OAuth token missing | `moonshot::tests::oauth_fallback_to_api_key` | Gold |
| 4.3 | `ProviderFactory::create_named` respects `oauth = true` flag | `providers::tests::factory_oauth_flag` | Gold |

## 5. CLI

| # | Assertion | Test | Oracle |
|---|-----------|------|--------|
| 5.1 | `arconaut login kimi` exists as a subcommand | `cli::tests::login_subcommand_exists` | Gold |
| 5.2 | `arconaut logout kimi` exists as a subcommand | `cli::tests::logout_subcommand_exists` | Gold |

---

## Oracle Tiers

- **Gold:** Deterministic, no external deps, no network I/O. Mock HTTP server or in-memory structures.
- **Silver:** Requires filesystem operations or process spawning.

**All Gold tests must pass before merge.**

## Test Inventory Summary

| Component | Gold | Silver | Total |
|-----------|------|--------|-------|
| Device Info | 3 | 1 | 4 |
| Token Storage | 6 | 0 | 6 |
| Device Auth Grant | 6 | 0 | 6 |
| Provider Integration | 3 | 0 | 3 |
| CLI | 2 | 0 | 2 |
| **Total** | **20** | **1** | **21** |
