use arconaut_core::{Context, Message, Role, ToolCall, ToolRegistry, ToolResult};
use arconaut_machine::{
    ChatProvider, ChatRequest, ChatResponse, ProviderError, TokenUsage, ToolDescriptor,
};
use async_trait::async_trait;
use std::sync::{Arc, Mutex};

/// The agent core. Owns the LLM provider, conversation context, and tool registry.
/// Executes turns: user input → LLM → tool calls → results → loop.
pub struct Soul {
    provider: Box<dyn ChatProvider>,
    context: Context,
    registry: ToolRegistry,
    max_steps: usize,
}

/// Outcome of a single turn.
pub struct TurnResult {
    /// The final assistant message (may be empty if max_steps reached).
    pub message: Message,
    /// Number of LLM call + tool execution cycles performed.
    pub steps_taken: usize,
    /// Whether the turn completed normally (no tool calls in final response).
    pub completed: bool,
    /// Why the turn stopped.
    pub stop_reason: StopReason,
}

/// Reason the turn stopped.
#[derive(Debug, Clone, PartialEq)]
pub enum StopReason {
    /// LLM returned no tool calls.
    Completed,
    /// Hit the max_steps limit.
    MaxStepsReached,
    /// Provider returned an error.
    ProviderError(ProviderError),
}

/// Errors that abort a turn.
#[derive(Debug, Clone, PartialEq)]
pub enum SoulError {
    Provider(ProviderError),
    /// Failed to parse tool arguments JSON.
    ToolArgsParse { tool_name: String, message: String },
}

impl std::fmt::Display for SoulError {
    fn fmt(&self, f: &mut std::fmt::Formatter<'_>) -> std::fmt::Result {
        match self {
            SoulError::Provider(e) => write!(f, "provider error: {}", e),
            SoulError::ToolArgsParse { tool_name, message } => {
                write!(f, "failed to parse args for tool '{}': {}", tool_name, message)
            }
        }
    }
}

impl std::error::Error for SoulError {}

impl Soul {
    pub fn new(provider: Box<dyn ChatProvider>, registry: ToolRegistry) -> Self {
        Self {
            provider,
            context: Context::new(200_000),
            registry,
            max_steps: 50,
        }
    }

    pub fn with_max_steps(mut self, max_steps: usize) -> Self {
        self.max_steps = max_steps;
        self
    }

    pub fn with_context_size(mut self, max_tokens: usize) -> Self {
        self.context = Context::new(max_tokens);
        self
    }

    /// Run one user turn.
    ///
    /// Appends the user message, then loops up to `max_steps`:
    ///   1. Call the LLM with current context + registered tools.
    ///   2. Append the assistant response.
    ///   3. If no tool calls, return completed.
    ///   4. Execute each tool call and append results.
    pub async fn run_turn(&mut self, input: &str) -> Result<TurnResult, SoulError> {
        self.context.append_message(Message::user(input));

        for step in 0..self.max_steps {
            let request = self.build_request();
            let response = self
                .provider
                .chat(request)
                .await
                .map_err(SoulError::Provider)?;

            self.context.append_message(response.message.clone());

            let tool_calls = extract_tool_calls(&response.message);
            if tool_calls.is_empty() {
                return Ok(TurnResult {
                    message: response.message,
                    steps_taken: step + 1,
                    completed: true,
                    stop_reason: StopReason::Completed,
                });
            }

            for call in tool_calls {
                let result = self.execute_tool_call(&call).await;
                self.context
                    .append_message(Message::tool_result(&call.id, result));
            }
        }

        // Max steps reached — grab the last assistant message if present.
        let last_message = self
            .context
            .history()
            .iter()
            .rev()
            .find(|m| m.role == Role::Assistant)
            .cloned()
            .unwrap_or_else(|| Message::assistant(""));

        Ok(TurnResult {
            message: last_message,
            steps_taken: self.max_steps,
            completed: false,
            stop_reason: StopReason::MaxStepsReached,
        })
    }

    fn build_request(&self) -> ChatRequest {
        let tools: Vec<ToolDescriptor> = self
            .registry
            .list()
            .iter()
            .map(|t| ToolDescriptor {
                name: t.name().to_string(),
                description: t.description().to_string(),
                parameters: t.parameters().clone(),
            })
            .collect();

        ChatRequest {
            messages: self.context.history().to_vec(),
            tools,
            system_prompt: None,
        }
    }

    async fn execute_tool_call(&self, call: &ToolCall) -> ToolResult {
        let args = match serde_json::from_str(&call.function.arguments) {
            Ok(v) => v,
            Err(e) => {
                return ToolResult::error(
                    format!("invalid JSON arguments: {}", e),
                    "parse error",
                );
            }
        };

        match self.registry.call(&call.function.name, args).await {
            Ok(result) => result,
            Err(e) => ToolResult::error(e.message, e.brief),
        }
    }
}

fn extract_tool_calls(message: &Message) -> Vec<ToolCall> {
    message
        .content
        .iter()
        .filter_map(|part| match part {
            arconaut_core::ContentPart::ToolCall { tool_call } => Some(tool_call.clone()),
            _ => None,
        })
        .collect()
}

// ---------------------------------------------------------------------------
// MockProvider — deterministic responses for tests
// ---------------------------------------------------------------------------

pub struct MockProvider {
    responses: Arc<Mutex<Vec<ChatResponse>>>,
}

impl MockProvider {
    pub fn new(responses: Vec<ChatResponse>) -> Self {
        Self {
            responses: Arc::new(Mutex::new(responses)),
        }
    }
}

#[async_trait]
impl ChatProvider for MockProvider {
    async fn chat(&self, _request: ChatRequest) -> Result<ChatResponse, ProviderError> {
        let mut responses = self.responses.lock().unwrap();
        if responses.is_empty() {
            Ok(ChatResponse {
                message: Message::assistant("done"),
                usage: TokenUsage { input: 0, output: 0 },
                id: "mock".to_string(),
            })
        } else {
            Ok(responses.remove(0))
        }
    }

    fn model_name(&self) -> &str {
        "mock"
    }

    fn max_context_size(&self) -> usize {
        200_000
    }

    fn capabilities(&self) -> std::collections::HashSet<arconaut_machine::ModelCapability> {
        let mut caps = std::collections::HashSet::new();
        caps.insert(arconaut_machine::ModelCapability::Text);
        caps.insert(arconaut_machine::ModelCapability::ToolUse);
        caps
    }

    fn thinking_effort(&self) -> Option<&str> {
        None
    }
}

// ---------------------------------------------------------------------------
// Tests
// ---------------------------------------------------------------------------

#[cfg(test)]
mod tests {
    use super::*;
    use arconaut_core::{ContentPart, FunctionCall};
    use serde_json::Value;

    fn text_response(text: &str) -> ChatResponse {
        ChatResponse {
            message: Message::assistant(text),
            usage: TokenUsage { input: 10, output: 5 },
            id: "test-1".to_string(),
        }
    }

    fn tool_response(calls: Vec<ToolCall>) -> ChatResponse {
        let content = calls
            .into_iter()
            .map(|tc| ContentPart::ToolCall { tool_call: tc })
            .collect();
        ChatResponse {
            message: Message::new(Role::Assistant, content),
            usage: TokenUsage { input: 10, output: 5 },
            id: "test-2".to_string(),
        }
    }

    #[tokio::test]
    async fn turn_completes_without_tools() {
        let provider = MockProvider::new(vec![text_response("hello")]);
        let registry = ToolRegistry::new();
        let mut soul = Soul::new(Box::new(provider), registry);

        let result = soul.run_turn("hi").await.unwrap();
        assert!(result.completed);
        assert_eq!(result.steps_taken, 1);
        assert_eq!(result.stop_reason, StopReason::Completed);
        assert_eq!(result.message.content[0].as_text().unwrap(), "hello");
    }

    #[tokio::test]
    async fn turn_executes_single_tool() {
        let provider = MockProvider::new(vec![
            tool_response(vec![ToolCall {
                id: "call-1".to_string(),
                function: FunctionCall {
                    name: "echo".to_string(),
                    arguments: "{\"msg\":\"world\"}".to_string(),
                },
            }]),
            text_response("done"),
        ]);

        let mut registry = ToolRegistry::new();
        registry.register(Box::new(TestEchoTool::new()));
        let mut soul = Soul::new(Box::new(provider), registry);

        let result = soul.run_turn("call echo").await.unwrap();
        assert!(result.completed);
        assert_eq!(result.steps_taken, 2);

        // Verify tool result was appended to context
        let history = soul.context.history();
        let tool_result_msg = history.iter().find(|m| m.role == Role::Tool).unwrap();
        match &tool_result_msg.content[0] {
            ContentPart::ToolResult { tool_result } => {
                assert_eq!(tool_result.tool_call_id, "call-1");
                assert!(!tool_result.is_error);
            }
            _ => panic!("expected ToolResult"),
        }
    }

    #[tokio::test]
    async fn turn_max_steps_enforced() {
        // Provider always returns a tool call, so we should hit max_steps.
        let provider = MockProvider::new(vec![
            tool_response(vec![ToolCall {
                id: "call-1".to_string(),
                function: FunctionCall {
                    name: "echo".to_string(),
                    arguments: "{}".to_string(),
                },
            }]),
            tool_response(vec![ToolCall {
                id: "call-2".to_string(),
                function: FunctionCall {
                    name: "echo".to_string(),
                    arguments: "{}".to_string(),
                },
            }]),
            tool_response(vec![ToolCall {
                id: "call-3".to_string(),
                function: FunctionCall {
                    name: "echo".to_string(),
                    arguments: "{}".to_string(),
                },
            }]),
        ]);

        let mut registry = ToolRegistry::new();
        registry.register(Box::new(TestEchoTool::new()));
        let mut soul = Soul::new(Box::new(provider), registry).with_max_steps(3);

        let result = soul.run_turn("loop").await.unwrap();
        assert!(!result.completed);
        assert_eq!(result.steps_taken, 3);
        assert_eq!(result.stop_reason, StopReason::MaxStepsReached);
    }

    #[tokio::test]
    async fn turn_provider_error_aborts() {
        let mut soul = Soul::new(Box::new(ErrorProvider), ToolRegistry::new());
        let result = soul.run_turn("fail").await;
        assert!(result.is_err());
    }

    struct TestEchoTool {
        params: Value,
    }

    impl TestEchoTool {
        fn new() -> Self {
            Self {
                params: serde_json::json!({
                    "type": "object",
                    "properties": {
                        "msg": { "type": "string" }
                    }
                }),
            }
        }
    }

    #[async_trait]
    impl arconaut_core::Tool for TestEchoTool {
        fn name(&self) -> &str {
            "echo"
        }
        fn description(&self) -> &str {
            "Echoes input"
        }
        fn parameters(&self) -> &Value {
            &self.params
        }
        async fn call(&self, args: Value) -> Result<ToolResult, arconaut_core::ToolError> {
            let msg = args["msg"].as_str().unwrap_or("");
            Ok(ToolResult::success(vec![ContentPart::text(msg)]))
        }
    }

    struct ErrorProvider;

    #[async_trait]
    impl ChatProvider for ErrorProvider {
        async fn chat(&self, _request: ChatRequest) -> Result<ChatResponse, ProviderError> {
            Err(ProviderError::Network {
                message: "boom".to_string(),
            })
        }
        fn model_name(&self) -> &str {
            "error"
        }
        fn max_context_size(&self) -> usize {
            0
        }
        fn capabilities(&self) -> std::collections::HashSet<arconaut_machine::ModelCapability> {
            std::collections::HashSet::new()
        }
        fn thinking_effort(&self) -> Option<&str> {
            None
        }
    }
}
