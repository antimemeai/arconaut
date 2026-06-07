use super::device::DeviceInfo;
use super::tombstone;
use chrono::{DateTime, Utc};
use serde::{Deserialize, Serialize};

const KIMI_CODE_CLIENT_ID: &str = "17e5f671-d194-4dfb-9706-5516cb48c098";
const DEFAULT_OAUTH_HOST: &str = "https://auth.kimi.com";
const REFRESH_THRESHOLD_SECONDS: i64 = 300; // 5 minutes

/// An OAuth 2.0 access token with refresh capability.
#[derive(Debug, Clone, Serialize, Deserialize, PartialEq)]
pub struct OAuthToken {
    pub access_token: String,
    pub refresh_token: String,
    #[serde(with = "chrono::serde::ts_seconds_option")]
    pub expires_at: Option<DateTime<Utc>>,
    pub scope: String,
    pub token_type: String,
    pub expires_in: u64,
}

impl OAuthToken {
    /// Check if the token has expired.
    pub fn is_expired(&self) -> bool {
        match self.expires_at {
            Some(expires) => Utc::now() >= expires,
            None => false,
        }
    }

    /// Check if the token should be refreshed (within 5 minutes of expiry).
    pub fn needs_refresh(&self) -> bool {
        match self.expires_at {
            Some(expires) => {
                let threshold = expires - chrono::Duration::seconds(REFRESH_THRESHOLD_SECONDS);
                Utc::now() >= threshold
            }
            None => false,
        }
    }
}

/// Device authorization response from the OAuth server.
#[derive(Debug, Clone)]
pub struct DeviceAuthorization {
    pub user_code: String,
    pub device_code: String,
    pub verification_uri: String,
    pub verification_uri_complete: String,
    pub expires_in: Option<u64>,
    pub interval: u64,
}

/// OAuth flow error.
#[derive(Debug, Clone, PartialEq)]
pub enum OAuthError {
    Network(String),
    Server { status: u16, message: String },
    Unauthorized(String),
    DeviceExpired,
    PollPending,
    Other(String),
}

impl std::fmt::Display for OAuthError {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            OAuthError::Network(msg) => write!(f, "oauth network error: {}", msg),
            OAuthError::Server { status, message } => {
                write!(f, "oauth server error ({}): {}", status, message)
            }
            OAuthError::Unauthorized(msg) => write!(f, "oauth unauthorized: {}", msg),
            OAuthError::DeviceExpired => write!(f, "device authorization expired"),
            OAuthError::PollPending => write!(f, "authorization pending"),
            OAuthError::Other(msg) => write!(f, "oauth error: {}", msg),
        }
    }
}

impl std::error::Error for OAuthError {}

/// Kimi (Moonshot) OAuth flow using Device Authorization Grant (RFC 8628).
#[derive(Clone)]
pub struct KimiOAuthFlow {
    client: reqwest::Client,
    oauth_host: String,
    client_id: String,
    device_info: DeviceInfo,
}

impl KimiOAuthFlow {
    /// Create a new OAuth flow with default settings.
    pub fn new() -> Self {
        Self {
            client: reqwest::Client::new(),
            oauth_host: std::env::var("KIMI_CODE_OAUTH_HOST")
                .or_else(|_| std::env::var("KIMI_OAUTH_HOST"))
                .unwrap_or_else(|_| DEFAULT_OAUTH_HOST.to_string()),
            client_id: KIMI_CODE_CLIENT_ID.to_string(),
            device_info: DeviceInfo::generate(),
        }
    }

    /// Create a new OAuth flow with a custom HTTP client.
    pub fn with_client(client: reqwest::Client) -> Self {
        Self {
            client,
            oauth_host: std::env::var("KIMI_CODE_OAUTH_HOST")
                .or_else(|_| std::env::var("KIMI_OAUTH_HOST"))
                .unwrap_or_else(|_| DEFAULT_OAUTH_HOST.to_string()),
            client_id: KIMI_CODE_CLIENT_ID.to_string(),
            device_info: DeviceInfo::generate(),
        }
    }

    /// Override the OAuth host (useful for testing).
    pub fn with_host(mut self, host: impl Into<String>) -> Self {
        self.oauth_host = host.into();
        self
    }

    /// Request device authorization from the OAuth server.
    ///
    /// Returns a `DeviceAuthorization` containing the user code and verification URL.
    pub async fn request_device_authorization(&self) -> Result<DeviceAuthorization, OAuthError> {
        let url = format!("{}/api/oauth/device_authorization", self.oauth_host.trim_end_matches('/'));

        let mut req = self
            .client
            .post(&url)
            .form(&[("client_id", &self.client_id)]);

        for (key, value) in self.device_info.headers() {
            req = req.header(&key, &value);
        }

        let response = req
            .send()
            .await
            .map_err(|e| OAuthError::Network(e.to_string()))?;

        let status = response.status();
        let data: serde_json::Value = response
            .json()
            .await
            .map_err(|e| OAuthError::Network(e.to_string()))?;

        if !status.is_success() {
            let message = data
                .get("error_description")
                .and_then(|v| v.as_str())
                .unwrap_or("device authorization failed")
                .to_string();
            return Err(OAuthError::Server { status: status.as_u16(), message });
        }

        Ok(DeviceAuthorization {
            user_code: data["user_code"]
                .as_str()
                .unwrap_or_default()
                .to_string(),
            device_code: data["device_code"]
                .as_str()
                .unwrap_or_default()
                .to_string(),
            verification_uri: data["verification_uri"]
                .as_str()
                .unwrap_or_default()
                .to_string(),
            verification_uri_complete: data["verification_uri_complete"]
                .as_str()
                .unwrap_or_default()
                .to_string(),
            expires_in: data["expires_in"].as_u64(),
            interval: data["interval"].as_u64().unwrap_or(5),
        })
    }

    /// Poll the token endpoint for an access token.
    ///
    /// Returns `OAuthError::PollPending` if the user has not yet authorized.
    /// Returns `OAuthError::DeviceExpired` if the device code has expired.
    pub async fn poll_token(
        &self,
        auth: &DeviceAuthorization,
    ) -> Result<OAuthToken, OAuthError> {
        let url = format!("{}/api/oauth/token", self.oauth_host.trim_end_matches('/'));

        let mut req = self.client.post(&url).form(&[
            ("client_id", self.client_id.as_str()),
            ("device_code", auth.device_code.as_str()),
            (
                "grant_type",
                "urn:ietf:params:oauth:grant-type:device_code",
            ),
        ]);

        for (key, value) in self.device_info.headers() {
            req = req.header(&key, &value);
        }

        let response = req
            .send()
            .await
            .map_err(|e| OAuthError::Network(e.to_string()))?;

        let status = response.status();
        let data: serde_json::Value = response
            .json()
            .await
            .map_err(|e| OAuthError::Network(e.to_string()))?;

        if status.is_success() && data.get("access_token").is_some() {
            return Self::token_from_response(&data);
        }

        let error = data["error"].as_str().unwrap_or("unknown_error");
        match error {
            "authorization_pending" => Err(OAuthError::PollPending),
            "expired_token" => Err(OAuthError::DeviceExpired),
            _ => {
                let message = data["error_description"]
                    .as_str()
                    .unwrap_or(error)
                    .to_string();
                if status.as_u16() == 401 || status.as_u16() == 403 {
                    Err(OAuthError::Unauthorized(message))
                } else {
                    Err(OAuthError::Server {
                        status: status.as_u16(),
                        message,
                    })
                }
            }
        }
    }

    /// Refresh an access token using a refresh token.
    ///
    /// If the refresh token has been tombstoned (previously rejected by the
    /// server), this returns `OAuthError::Unauthorized` immediately without
    /// making an HTTP request.
    pub async fn refresh_token(&self, refresh_token: &str) -> Result<OAuthToken, OAuthError> {
        if tombstone::is_tombstoned(refresh_token) {
            return Err(OAuthError::Unauthorized(
                "refresh token rejected; retry blocked".to_string(),
            ));
        }

        let url = format!("{}/api/oauth/token", self.oauth_host.trim_end_matches('/'));

        let mut req = self.client.post(&url).form(&[
            ("client_id", self.client_id.as_str()),
            ("refresh_token", refresh_token),
            ("grant_type", "refresh_token"),
        ]);

        for (key, value) in self.device_info.headers() {
            req = req.header(&key, &value);
        }

        let response = req
            .send()
            .await
            .map_err(|e| OAuthError::Network(e.to_string()))?;

        let status = response.status();
        let data: serde_json::Value = response
            .json()
            .await
            .map_err(|e| OAuthError::Network(e.to_string()))?;

        if !status.is_success() {
            let message = data["error_description"]
                .as_str()
                .unwrap_or("token refresh failed")
                .to_string();
            if status.as_u16() == 401 || status.as_u16() == 403 {
                tombstone::tombstone(refresh_token);
                return Err(OAuthError::Unauthorized(message));
            }
            return Err(OAuthError::Server {
                status: status.as_u16(),
                message,
            });
        }

        tombstone::remove_tombstone(refresh_token);
        Self::token_from_response(&data)
    }

    fn token_from_response(data: &serde_json::Value) -> Result<OAuthToken, OAuthError> {
        let access_token = data["access_token"]
            .as_str()
            .ok_or_else(|| OAuthError::Other("missing access_token".to_string()))?
            .to_string();
        let refresh_token = data["refresh_token"]
            .as_str()
            .unwrap_or("")
            .to_string();
        let expires_in = data["expires_in"].as_u64().unwrap_or(0);
        let expires_at = if expires_in > 0 {
            Some(Utc::now() + chrono::Duration::seconds(expires_in as i64))
        } else {
            None
        };
        let scope = data["scope"].as_str().unwrap_or("").to_string();
        let token_type = data["token_type"].as_str().unwrap_or("Bearer").to_string();

        Ok(OAuthToken {
            access_token,
            refresh_token,
            expires_at,
            scope,
            token_type,
            expires_in,
        })
    }

    /// Fetch available models from the Kimi API.
    ///
    /// Uses the access token to authenticate. Returns a list of models
    /// the user has access to.
    pub async fn fetch_models(&self, access_token: &str) -> Result<Vec<ModelInfo>, OAuthError> {
        let url = "https://api.kimi.com/coding/v1/models";

        let response = self
            .client
            .get(url)
            .bearer_auth(access_token)
            .send()
            .await
            .map_err(|e| OAuthError::Network(e.to_string()))?;

        let status = response.status();
        let data: serde_json::Value = response
            .json()
            .await
            .map_err(|e| OAuthError::Network(e.to_string()))?;

        if !status.is_success() {
            let message = data["error"]["message"]
                .as_str()
                .unwrap_or("failed to fetch models")
                .to_string();
            return Err(OAuthError::Server {
                status: status.as_u16(),
                message,
            });
        }

        let mut models = Vec::new();
        if let Some(data_array) = data["data"].as_array() {
            for item in data_array {
                if let Some(id) = item["id"].as_str() {
                    models.push(ModelInfo {
                        id: id.to_string(),
                        context_length: item["context_length"].as_u64().unwrap_or(0) as usize,
                        display_name: item["display_name"].as_str().map(|s| s.to_string()),
                    });
                }
            }
        }

        Ok(models)
    }
}

/// Information about a single model from the provider.
#[derive(Debug, Clone, PartialEq)]
pub struct ModelInfo {
    pub id: String,
    pub context_length: usize,
    pub display_name: Option<String>,
}

impl Default for KimiOAuthFlow {
    fn default() -> Self {
        Self::new()
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn token_from_response_parses_fields() {
        let data = serde_json::json!({
            "access_token": "abc123",
            "refresh_token": "def456",
            "expires_in": 3600,
            "scope": "all",
            "token_type": "Bearer"
        });
        let token = KimiOAuthFlow::token_from_response(&data).unwrap();
        assert_eq!(token.access_token, "abc123");
        assert_eq!(token.refresh_token, "def456");
        assert_eq!(token.scope, "all");
        assert_eq!(token.token_type, "Bearer");
        assert_eq!(token.expires_in, 3600);
        assert!(token.expires_at.is_some());
    }

    #[test]
    fn token_from_response_missing_access_token_fails() {
        let data = serde_json::json!({
            "refresh_token": "def456",
            "expires_in": 3600,
        });
        assert!(KimiOAuthFlow::token_from_response(&data).is_err());
    }

    #[test]
    fn error_response_parsing() {
        let data = serde_json::json!({
            "error": "invalid_client",
            "error_description": "client_id is invalid"
        });
        let result = KimiOAuthFlow::token_from_response(&data);
        // This should succeed parsing the JSON but the caller would check status first
        assert!(result.is_err());
    }

    #[test]
    fn expired_token() {
        let token = OAuthToken {
            access_token: "a".to_string(),
            refresh_token: "b".to_string(),
            expires_at: Some(Utc::now() - chrono::Duration::minutes(1)),
            scope: "all".to_string(),
            token_type: "Bearer".to_string(),
            expires_in: 3600,
        };
        assert!(token.is_expired());
        assert!(token.needs_refresh());
    }

    #[test]
    fn fresh_token() {
        let token = OAuthToken {
            access_token: "a".to_string(),
            refresh_token: "b".to_string(),
            expires_at: Some(Utc::now() + chrono::Duration::hours(1)),
            scope: "all".to_string(),
            token_type: "Bearer".to_string(),
            expires_in: 3600,
        };
        assert!(!token.is_expired());
        assert!(!token.needs_refresh());
    }

    #[test]
    fn device_auth_request_shape() {
        // Verify the flow struct exists and has the right client_id
        let flow = KimiOAuthFlow::new();
        assert!(!flow.client_id.is_empty());
        assert!(!flow.oauth_host.is_empty());
    }

    #[test]
    fn oauth_token_snapshot() {
        // Use a fixed timestamp so the snapshot is deterministic.
        let expires_at = DateTime::parse_from_rfc3339("2025-01-01T00:00:00Z")
            .unwrap()
            .with_timezone(&Utc);
        let token = OAuthToken {
            access_token: "test_access_token".to_string(),
            refresh_token: "test_refresh_token".to_string(),
            expires_at: Some(expires_at),
            scope: "all".to_string(),
            token_type: "Bearer".to_string(),
            expires_in: 3600,
        };
        let json = serde_json::to_string_pretty(&token).unwrap();
        insta::assert_snapshot!(json);
    }

    mod wiremock_tests {
        use super::*;
        use wiremock::{
            matchers::{method, path},
            Mock, MockServer, ResponseTemplate,
        };

        #[tokio::test]
        async fn request_device_authorization_success() {
            let server = MockServer::start().await;
            Mock::given(method("POST"))
                .and(path("/api/oauth/device_authorization"))
                .respond_with(ResponseTemplate::new(200).set_body_json(serde_json::json!({
                    "user_code": "ABCD-EFGH",
                    "device_code": "dc123",
                    "verification_uri": "https://auth.kimi.com/verify",
                    "verification_uri_complete": "https://auth.kimi.com/verify?code=ABCD-EFGH",
                    "expires_in": 600,
                    "interval": 5
                })))
                .mount(&server)
                .await;

            let flow = KimiOAuthFlow::with_client(reqwest::Client::new())
                .with_host(server.uri());
            let auth = flow.request_device_authorization().await.unwrap();

            assert_eq!(auth.user_code, "ABCD-EFGH");
            assert_eq!(auth.device_code, "dc123");
            assert_eq!(auth.verification_uri, "https://auth.kimi.com/verify");
            assert_eq!(auth.interval, 5);
        }

        #[tokio::test]
        async fn poll_token_success() {
            let server = MockServer::start().await;
            Mock::given(method("POST"))
                .and(path("/api/oauth/token"))
                .respond_with(ResponseTemplate::new(200).set_body_json(serde_json::json!({
                    "access_token": "at_123",
                    "refresh_token": "rt_456",
                    "expires_in": 3600,
                    "scope": "all",
                    "token_type": "Bearer"
                })))
                .mount(&server)
                .await;

            let flow = KimiOAuthFlow::with_client(reqwest::Client::new())
                .with_host(server.uri());
            let device_auth = DeviceAuthorization {
                user_code: "ABCD".to_string(),
                device_code: "dc123".to_string(),
                verification_uri: "https://auth.kimi.com/verify".to_string(),
                verification_uri_complete: "https://auth.kimi.com/verify?code=ABCD".to_string(),
                expires_in: Some(600),
                interval: 5,
            };
            let token = flow.poll_token(&device_auth).await.unwrap();
            assert_eq!(token.access_token, "at_123");
            assert_eq!(token.refresh_token, "rt_456");
        }

        #[tokio::test]
        async fn poll_token_pending() {
            let server = MockServer::start().await;
            Mock::given(method("POST"))
                .and(path("/api/oauth/token"))
                .respond_with(ResponseTemplate::new(200).set_body_json(serde_json::json!({
                    "error": "authorization_pending",
                    "error_description": "User has not yet authorized"
                })))
                .mount(&server)
                .await;

            let flow = KimiOAuthFlow::with_client(reqwest::Client::new())
                .with_host(server.uri());
            let device_auth = DeviceAuthorization {
                user_code: "ABCD".to_string(),
                device_code: "dc123".to_string(),
                verification_uri: "https://auth.kimi.com/verify".to_string(),
                verification_uri_complete: "https://auth.kimi.com/verify?code=ABCD".to_string(),
                expires_in: Some(600),
                interval: 5,
            };
            let result = flow.poll_token(&device_auth).await;
            assert!(matches!(result, Err(OAuthError::PollPending)));
        }

        #[tokio::test]
        async fn refresh_token_success() {
            let server = MockServer::start().await;
            Mock::given(method("POST"))
                .and(path("/api/oauth/token"))
                .respond_with(ResponseTemplate::new(200).set_body_json(serde_json::json!({
                    "access_token": "new_at_789",
                    "refresh_token": "new_rt_abc",
                    "expires_in": 3600,
                    "scope": "all",
                    "token_type": "Bearer"
                })))
                .mount(&server)
                .await;

            let flow = KimiOAuthFlow::with_client(reqwest::Client::new())
                .with_host(server.uri());
            let token = flow.refresh_token("old_rt").await.unwrap();
            assert_eq!(token.access_token, "new_at_789");
            assert_eq!(token.refresh_token, "new_rt_abc");
        }

        #[tokio::test]
        async fn refresh_token_unauthorized() {
            let server = MockServer::start().await;
            Mock::given(method("POST"))
                .and(path("/api/oauth/token"))
                .respond_with(ResponseTemplate::new(401).set_body_json(serde_json::json!({
                    "error": "invalid_grant",
                    "error_description": "Refresh token revoked"
                })))
                .mount(&server)
                .await;

            let flow = KimiOAuthFlow::with_client(reqwest::Client::new())
                .with_host(server.uri());
            let result = flow.refresh_token("bad_rt").await;
            assert!(matches!(result, Err(OAuthError::Unauthorized(_))));
        }

    }
}
