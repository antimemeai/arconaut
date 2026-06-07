# Phase 5 Conformance Specification

**Issue:** arconaut-nfv (Phase 5)
**Companion:** Phase 5 Subplan

---

## 1. Provider System

### 1.1 Provider Factory

| # | Assertion | Test | Oracle |
|---|-----------|------|--------|
| 1.1.1 | `ProviderFactory::create` returns correct provider type for each `ProviderKind` | `providers::tests::factory_creates_all_kinds` | Gold |
| 1.1.2 | `ProviderFactory::create_named` resolves config from VariableStore by name | `providers::tests::factory_from_config` | Gold |
| 1.1.3 | Invalid config returns `ProviderBuildError` with descriptive message | `providers::tests::factory_rejects_invalid_config` | Gold |
| 1.1.4 | `ProviderRegistry` stores and retrieves named providers | `providers::tests::registry_round_trip` | Gold |
| 1.1.5 | Duplicate provider name in registry returns error | `providers::tests::registry_rejects_duplicate` | Gold |

### 1.2 Brand-Specific Providers

| # | Assertion | Test | Oracle |
|---|-----------|------|--------|
| 1.2.1 | `AnthropicProvider` retains existing behavior after refactor | `providers::anthropic::tests::provider_model_info` | Gold |
| 1.2.2 | `AnthropicProvider::with_model` overrides default model | `providers::anthropic::tests::provider_with_model` | Gold |
| 1.2.3 | `AnthropicProvider` debug output redacts API key | `providers::anthropic::tests::provider_debug_redacts_key` | Gold |
| 1.2.4 | `OpenAiProvider` constructs with correct default base_url | `providers::openai::tests::default_base_url` | Gold |
| 1.2.5 | `MoonshotProvider` constructs with correct default base_url | `providers::moonshot::tests::default_base_url` | Gold |
| 1.2.6 | `OpenRouterProvider` injects required headers | `providers::openrouter::tests::headers_present` | Gold |
| 1.2.7 | `GeminiProvider` constructs with correct default base_url | `providers::gemini::tests::default_base_url` | Gold |
| 1.2.8 | All providers implement `ChatProvider` and return non-empty capabilities | `providers::tests::all_providers_have_capabilities` | Gold |
| 1.2.9 | Provider error classification maps HTTP status to correct `ProviderError` variant | `providers::tests::error_classification` | Gold |

### 1.3 OpenAI-Compatible Client

| # | Assertion | Test | Oracle |
|---|-----------|------|--------|
| 1.3.1 | `OpenAiCompatClient` serializes `ChatRequest` to correct JSON shape | `openai_compat::tests::request_serialization` | Gold |
| 1.3.2 | `OpenAiCompatClient` deserializes OpenAI response to `ChatResponse` | `openai_compat::tests::response_deserialization` | Gold |
| 1.3.3 | Tool calls in response are parsed into `ContentPart::ToolCall` | `openai_compat::tests::tool_call_parsing` | Gold |
| 1.3.4 | Message conversion handles system/user/assistant/tool roles | `openai_compat::tests::role_conversion` | Gold |

### 1.4 HTTP Round-Trip (Silver)

| # | Assertion | Test | Oracle |
|---|-----------|------|--------|
| 1.4.1 | `AnthropicProvider::chat` against mock server returns valid `ChatResponse` | `providers::anthropic::tests::mock_chat` | Silver |
| 1.4.2 | `OpenAiProvider::chat` against mock server returns valid `ChatResponse` | `providers::openai::tests::mock_chat` | Silver |
| 1.4.3 | Rate limit response returns `ProviderError::RateLimit` | `providers::tests::mock_rate_limit` | Silver |
| 1.4.4 | Auth failure returns `ProviderError::Auth` | `providers::tests::mock_auth_failure` | Silver |

---

## 2. Auth & Config System

| # | Assertion | Test | Oracle |
|---|-----------|------|--------|
| 2.1 | `VariableStore` parses `[provider.{name}]` TOML table into structured config | `vars::tests::provider_config_parsing` | Gold |
| 2.2 | Missing provider config returns `None` without panic | `vars::tests::missing_provider_config` | Gold |
| 2.3 | Provider config with missing `api_key` is rejected by factory | `providers::tests::config_missing_api_key` | Gold |
| 2.4 | CLI `--provider <name>` selects named provider from config | `cli::tests::provider_flag` | Gold |
| 2.5 | CLI `--model <name>` overrides provider model | `cli::tests::model_flag` | Gold |
| 2.6 | CLI `--assistant-provider <name>` configures secondary provider | `cli::tests::assistant_provider_flag` | Gold |

---

## 3. Off-Pulse Intervention

| # | Assertion | Test | Oracle |
|---|-----------|------|--------|
| 3.1 | `ChurnDetector::detect_message_churn` returns true for trigger phrases | `intervention::tests::detects_trigger_phrases` | Gold |
| 3.2 | `ChurnDetector::detect_message_churn` returns false for non-trigger text | `intervention::tests::ignores_normal_text` | Gold |
| 3.3 | `ChurnDetector::detect_tool_loop` returns true at threshold (5 identical calls) | `intervention::tests::detects_tool_loop` | Gold |
| 3.4 | `ChurnDetector::detect_stall` returns true when steps exceed threshold without progress | `intervention::tests::detects_stall` | Gold |
| 3.5 | `InterventionInjector` injects churn prompt only when churn detected | `intervention::tests::injects_on_churn` | Gold |
| 3.6 | `InterventionInjector` does not duplicate intervention prompts | `intervention::tests::no_duplicate_interventions` | Gold |
| 3.7 | Soul turn loop hard-stops after 10 identical tool calls | `soul::tests::hard_stop_on_churn` | Gold |

---

## 4. Assistant Model

| # | Assertion | Test | Oracle |
|---|-----------|------|--------|
| 4.1 | `AssistantModel::check_triggers` fires on `OnToolFailure` | `assistant::tests::trigger_tool_failure` | Gold |
| 4.2 | `AssistantModel::check_triggers` fires on `OnMaxStepsWarning` | `assistant::tests::trigger_max_steps` | Gold |
| 4.3 | `AssistantModel::check_triggers` does not fire on unrelated events | `assistant::tests::trigger_no_false_positive` | Gold |
| 4.4 | `TriggerEvent::Periodic` fires only after interval elapsed | `assistant::tests::periodic_respects_interval` | Gold |
| 4.5 | Soul with assistant model queries assistant on trigger | `soul::tests::assistant_query_on_trigger` | Gold |
| 4.6 | Soul without assistant model skips assistant logic | `soul::tests::no_assistant_when_disabled` | Gold |

---

## 5. Audit Logging

| # | Assertion | Test | Oracle |
|---|-----------|------|--------|
| 5.1 | `AuditEvent` serializes to valid JSON | `audit::tests::event_serialization` | Gold |
| 5.2 | `AuditEvent` deserializes from JSON with all fields preserved | `audit::tests::event_deserialization` | Gold |
| 5.3 | `AuditLogger::log` appends to file without truncation | `audit::tests::append_preserves_history` | Gold |
| 5.4 | `AuditLogger` creates session directory if missing | `audit::tests::creates_session_dir` | Gold |
| 5.5 | `AuditHook::pre_turn` logs `TurnBegin` event | `audit::tests::hook_logs_turn_begin` | Gold |
| 5.6 | `AuditHook::post_turn` logs `TurnEnd` with correct `steps_taken` | `audit::tests::hook_logs_turn_end` | Gold |
| 5.7 | `AuditHook::pre_tool_use` logs `ToolCall` with tool name and args | `audit::tests::hook_logs_tool_call` | Gold |
| 5.8 | `AuditHook::post_tool_use` logs `ToolResult` with outcome | `audit::tests::hook_logs_tool_result` | Gold |
| 5.9 | Compaction events logged with before/after token counts | `audit::tests::hook_logs_compaction` | Gold |
| 5.10 | Provider switch events include from/to provider names | `audit::tests::provider_switch_event` | Gold |

---

## Oracle Tiers

- **Gold:** Deterministic, no external deps, no network I/O, no process spawning. Run in CI. Uses mock data, in-memory structures, temp files.
- **Silver:** Requires network I/O (mock HTTP server or real API), process spawning, or filesystem operations beyond temp files. Run on-demand or in extended CI.

**All Gold tests must pass before merge.** Silver tests must pass before release.

---

## Test Inventory Summary

| Component | Gold | Silver | Total |
|-----------|------|--------|-------|
| Provider Factory | 5 | 0 | 5 |
| Brand Providers | 9 | 4 | 13 |
| OpenAI-Compat Client | 4 | 0 | 4 |
| Auth & Config | 6 | 0 | 6 |
| Off-Pulse Intervention | 7 | 0 | 7 |
| Assistant Model | 6 | 0 | 6 |
| Audit Logging | 10 | 0 | 10 |
| **Total** | **47** | **4** | **51** |
