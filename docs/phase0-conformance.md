# Phase 0 Conformance Spec

**Issue:** arconaut-6s8  
**Date:** 2026-06-07  

## Implementation Relation

An implementation conforms to this spec if, for every assertion below, the implementation's behavior matches the specified expected behavior under the stated preconditions.

## Fault Model

We assume these classes of faults:
- **Serialization faults** — types cannot roundtrip through JSON
- **API integration faults** — HTTP requests malformed, responses mis-parsed
- **State management faults** — context loses messages, double-counts tokens, fails to checkpoint
- **Trait contract faults** — implementors violate `ChatProvider` or `Tool` invariants

## 1. Core Types Conformance

### 1.1 Message Roundtrip
**Given:** A `Message` with `role=User`, `content=[TextPart("hello")]`, `timestamp=2026-06-07T12:00:00Z`
**When:** Serialized to JSON and deserialized back
**Then:** The resulting `Message` must equal the original (all fields match)

### 1.2 ContentPart Discrimination
**Given:** A JSON payload `{"type":"tool_call","tool_call":{"id":"123","function":{"name":"bash","arguments":"{}"}}}`
**When:** Deserialized to `ContentPart`
**Then:** The result must be `ContentPart::ToolCall` with matching fields, not `TextPart` or any other variant

### 1.3 Timestamp Presence
**Given:** Any `Message` constructed via `Message::new`
**When:** Inspected
**Then:** `timestamp` must be `Some(DateTime<Utc>)` set to construction time (within 1 second tolerance)

### 1.4 Tool Trait Contract
**Given:** A type implementing `Tool` with `name="test_tool"`, `description="A test"`
**When:** `name()` and `description()` are called
**Then:** They must return the exact strings provided at construction, not clones with different lifetimes

### 1.5 ToolResult Success/Failure
**Given:** `ToolResult::success(TextPart("ok"))` and `ToolResult::error("failed", "it broke")`
**When:** Pattern matched
**Then:** Success variant must carry the exact output. Error variant must carry exact message and brief.

## 2. LLM Provider Conformance

### 2.1 Anthropic Provider Basic Chat
**Given:** A valid Anthropic API key in environment, `AnthropicProvider` configured with `model="claude-sonnet-4-6"`
**When:** `chat()` called with a single user message "Say exactly: pong"
**Precondition:** Network available, API key has quota
**Then:** Response must contain assistant message with text "pong" (case-sensitive, exact match)

### 2.2 Provider Error Classification
**Given:** `AnthropicProvider` with invalid API key
**When:** `chat()` called with any message
**Then:** Must return `ProviderError::Auth` with status code 401, not `ProviderError::Network` or `ProviderError::Other`

### 2.3 Provider Model Info
**Given:** `AnthropicProvider` initialized with `model="claude-sonnet-4-6"`
**When:** `model_name()` and `max_context_size()` called
**Then:** Must return `"claude-sonnet-4-6"` and `200000` respectively

### 2.4 Capability Reporting
**Given:** `AnthropicProvider` for Claude Sonnet 4.6
**When:** `capabilities()` called
**Then:** Must contain `ModelCapability::Text`, `ModelCapability::ToolUse`, and `ModelCapability::Thinking`

## 3. Context Manager Conformance

### 3.1 Append and Count
**Given:** Empty `Context` with `max_tokens=1000`
**When:** Append `Message::user("hello world")` (2 tokens estimated)
**Then:** `token_count()` returns 2, `history.len()` returns 1

### 3.2 Clear Resets State
**Given:** `Context` with 5 messages, `token_count=50`
**When:** `clear()` called
**Then:** `history` empty, `token_count` 0, `checkpoints` preserved

### 3.3 Checkpoint Save/Restore
**Given:** `Context` with messages [A, B, C], checkpoint taken after B
**When:** Revert to checkpoint
**Then:** `history` contains [A, B], `token_count` reflects [A, B] only

### 3.4 Token Count Accuracy (Heuristic)
**Given:** `Context` with message containing 400 ASCII characters
**When:** `token_count()` queried
**Then:** Returns 100 (400 / 4), within ±10% of actual token count

## 4. REPL Conformance

### 4.1 Basic Conversation Loop
**Given:** REPL started with valid Anthropic API key
**When:** User inputs "Say exactly: hello" → Enter
**Then:** REPL prints assistant response containing "hello" within 30 seconds

### 4.2 Quit Command
**Given:** REPL running
**When:** User inputs `/quit` → Enter
**Then:** REPL exits with status 0 within 1 second, no further prompts

### 4.3 Empty Input Handling
**Given:** REPL running
**When:** User inputs empty string → Enter
**Then:** REPL re-prompts without sending empty message to API

## 5. Workspace Conformance

### 5.1 Clean Build
**Given:** Fresh checkout, no `target/` directory
**When:** `cargo check --workspace` executed
**Then:** Completes with zero warnings, zero errors

### 5.2 Test Execution
**Given:** Workspace built
**When:** `cargo test --workspace` executed
**Then:** All tests pass (exit code 0)

### 5.3 Crate Isolation
**Given:** Each crate in workspace
**When:** `cargo check -p <crate>` executed for each
**Then:** Each compiles independently (no hidden cross-crate dependencies)

## Test Oracle Summary

| Spec | Oracle Tier | Technique |
|------|------------|-----------|
| 1.1 Message Roundtrip | 5 (Property) | Roundtrip property: `deserialize(serialize(msg)) == msg` |
| 1.2 ContentPart Discrimination | 5 (Property) | Arbitrary JSON → must parse to correct variant |
| 1.3 Timestamp | 6 (Model) | Compare against system clock model |
| 2.1 Anthropic Chat | 7 (Specification) | Exact string match against expected response |
| 2.2 Error Classification | 7 (Specification) | Status code and error variant exact match |
| 3.3 Checkpoint | 6 (Model) | Abstract model: list + pointer, check equivalence |
| 4.1 REPL | 7 (Specification) | End-to-end behavioral specification |
| 5.1 Clean Build | 2 (Implicit) | Compiler success = no crash/no warning |
