# Phase 1 Sub-Plan: Soul

**Issue:** arconaut-zoo  
**Scope:** Turn lifecycle, tool registry, built-in tools, dynamic injection, hooks, deduplication, compaction.

---

## Sub-Task 1: Turn/Step Lifecycle

**What:** The core agent loop — receive input, send to LLM, parse response, execute tool calls, loop until done.

**Deliverables:**
- `Turn` struct representing one user turn
- `Step` struct representing one LLM call + tool execution
- `Soul` (or `Agent`) struct owning provider, context, tool registry
- `Soul::run_turn(user_input) -> Result<TurnOutput, SoulError>`
- Max steps per turn enforced (configurable, default 50)
- Tool call → tool execution → tool result appended → next step

**Tests:**
- Turn completes when LLM returns no tool calls
- Turn stops at max_steps
- Tool results are appended to context
- Errors in tool execution are captured as tool results

---

## Sub-Task 2: Tool Registry

**What:** Register tools by name, dispatch tool calls to the right implementation.

**Deliverables:**
- `ToolRegistry` struct: `HashMap<String, Box<dyn Tool>>`
- `register(name, tool)` and `call(name, args) -> Result<ToolResult, ToolError>`
- `list_tools() -> Vec<&dyn Tool>` for building LLM tool descriptions

**Tests:**
- Register and call a tool
- Call unknown tool returns error
- List returns all registered tools

---

## Sub-Task 3: Built-in Tools

**What:** Five essential file/shell tools.

**Deliverables:**
- `ReadTool` — read file contents, with line offset and limit
- `WriteTool` — write file (overwrite)
- `EditTool` — targeted string replacement (exact match, one occurrence default)
- `BashTool` — execute shell command, capture stdout/stderr/exit code
- `GrepTool` — regex search in files (ripgrep-like, workspace-scoped)

**Constraints:**
- All tools are read-only or write with no confirmation in Phase 1 (yolo mode)
- BashTool has a timeout (default 30s)
- GrepTool respects .gitignore by default
- All return structured `ToolResult` with success/error distinction

**Tests:**
- Each tool has at least 2 tests (success + error path)
- BashTool timeout works
- EditTool fails gracefully on non-matching old_string

---

## Sub-Task 4: Dynamic Injection Framework

**What:** Inject content into the context window dynamically (system prompts, skills, rules).

**Deliverables:**
- `Injector` trait: `fn inject(&self, context: &mut Context)`
- `SystemPromptInjector` — injects system message at start
- `SkillInjector` — injects skill instructions (lazy, loaded on first use)
- `RuleInjector` — injects project rules from `.arconaut/rules/`
- Injection happens at turn start, before user message

**Tests:**
- Injector adds messages to context
- Multiple injectors compose in order
- Skill loads lazily

---

## Sub-Task 5: Hook Engine

**What:** Pre/post turn hooks for instrumentation, logging, metrics.

**Deliverables:**
- `Hook` trait with `pre_turn(&mut self, ctx: &TurnContext)` and `post_turn(&mut self, ctx: &TurnContext, outcome: &TurnOutcome)`
- `HookEngine` that runs all registered hooks
- Built-in `MetricsHook` tracking token usage, step count, latency

**Tests:**
- Hooks fire in registration order
- Pre and post both called
- Hook errors don't crash the turn (logged but not fatal)

---

## Sub-Task 6: Deduplication

**What:** Prevent duplicate tool calls within a turn.

**Deliverables:**
- `Deduplicator` that tracks (tool_name, arguments) pairs in a turn
- If LLM issues identical tool call, return cached result instead of re-executing
- Cleared at turn boundary

**Tests:**
- Identical tool calls deduplicated
- Different args are not deduplicated
- Dedup cleared between turns

---

## Sub-Task 7: Compaction Engine

**What:** Summarize old context when approaching token limit.

**Deliverables:**
- `CompactionEngine` with `compact(&mut self, context: &mut Context)`
- Triggered when `context.token_count() / context.max_tokens > 0.8` (configurable)
- Strategy: summarize oldest N messages into a single summary message
- Summary is a `Message::system` with "[SUMMARY] ..." prefix
- Preserves recent messages (configurable window, default 10)

**Tests:**
- Compaction reduces token count
- Recent messages preserved
- Summary message inserted correctly
- No compaction if under threshold

---

## Dependency Graph

```
Sub-Task 2 (Tool Registry)
    ↓
Sub-Task 3 (Built-in Tools) ──→ Sub-Task 1 (Turn Lifecycle)
                                    ↓
                        Sub-Task 4 (Injection) ──→ Sub-Task 5 (Hooks)
                                    ↓
                        Sub-Task 6 (Deduplication)
                                    ↓
                        Sub-Task 7 (Compaction)
```

**Parallelizable:** Sub-Task 2 and Sub-Task 3 can start immediately. Sub-Task 4, 5, 6, 7 depend on Sub-Task 1.

---

## Order of Attack

1. Sub-Task 2 + 3 together (registry + built-ins)
2. Sub-Task 1 (turn lifecycle, uses registry)
3. Sub-Task 4 (injection, uses turn lifecycle)
4. Sub-Task 6 (deduplication, uses turn lifecycle)
5. Sub-Task 5 (hooks, uses turn lifecycle)
6. Sub-Task 7 (compaction, uses context)
