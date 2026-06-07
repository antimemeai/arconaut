# Soul Design Notes

## Problem: ChatRequest holds Vec<Box<dyn Tool>>

`ToolRegistry` stores `HashMap<String, Box<dyn Tool>>`. `list()` returns `Vec<&dyn Tool>`.
Passing tools to `ChatRequest` requires cloning `Box<dyn Tool>`, which is not trivial.
The provider only needs `name()`, `description()`, `parameters()` — never `call()`.

## Decision: Introduce ToolDescriptor

```rust
pub struct ToolDescriptor {
    pub name: String,
    pub description: String,
    pub parameters: Value,
}
```

`ChatRequest.tools` becomes `Vec<ToolDescriptor>`. Soul maps `registry.list()`
to descriptors. Anthropic provider updated to consume descriptors directly.
No trait object cloning needed.

## Soul Turn Flow

```
run_turn(user_input):
  1. context.append_message(Message::user(input))
  2. for step in 0..max_steps:
     a. request = ChatRequest {
          messages: context.history().to_vec(),
          tools: registry.list() → map to ToolDescriptor,
        }
     b. response = provider.chat(request).await?
     c. context.append_message(response.message)
     d. tool_calls = extract_tool_calls(&response.message)
     e. if tool_calls.is_empty():
          return TurnResult { completed: true, ... }
     f. for tc in tool_calls:
          args = serde_json::from_str(&tc.function.arguments)?
          result = registry.call(&tc.function.name, args).await
          context.append_message(Message::tool_result(tc.id, result))
  3. return TurnResult { completed: false, stop_reason: MaxStepsReached }
```

## MockProvider (tests only)

Holds `Vec<ChatResponse>`, returns them in order via `Mutex`.
Implements all `ChatProvider` trait methods with dummy values.

## Error Handling

- ProviderError → SoulError::Provider, turn aborts
- ToolError (unknown tool, bad args) → captured as ToolResult::Error, turn continues
- serde_json::Error parsing tool args → captured as ToolResult::Error, turn continues
- Max steps → TurnResult with completed=false
