# Phase 1 Conformance Specification

**Issue:** arconaut-zoo  
**Scope:** Turn lifecycle, tool registry, built-in tools, dynamic injection, hooks, deduplication, compaction.

---

## 1. Tool Registry

| # | Assertion | Test | Oracle |
|---|-----------|------|--------|
| 1.1 | `ToolRegistry::register` accepts any `Box<dyn Tool>` | `tool_registry::tests::register_and_call` | Gold |
| 1.2 | `ToolRegistry::call` dispatches to the correct tool by name | `tool_registry::tests::register_and_call` | Gold |
| 1.3 | `ToolRegistry::call` on unknown name returns `ToolError::UnknownTool` | `tool_registry::tests::unknown_tool_returns_error` | Gold |
| 1.4 | `ToolRegistry::list` returns all registered tools in insertion order | `tool_registry::tests::list_tools` | Gold |
| 1.5 | Tool names are case-sensitive | `tool_registry::tests::case_sensitive_names` | Gold |

---

## 2. Built-in Tools

| # | Assertion | Test | Oracle |
|---|-----------|------|--------|
| 2.1 | `ReadTool` returns file contents as string | `read_tool::tests::reads_file` | Gold |
| 2.2 | `ReadTool` on missing file returns error | `read_tool::tests::missing_file_error` | Gold |
| 2.3 | `ReadTool` supports offset and limit | `read_tool::tests::offset_and_limit` | Gold |
| 2.4 | `WriteTool` writes file contents | `write_tool::tests::writes_file` | Gold |
| 2.5 | `WriteTool` creates parent directories | `write_tool::tests::creates_parents` | Gold |
| 2.6 | `EditTool` replaces exact match | `edit_tool::tests::exact_replace` | Gold |
| 2.7 | `EditTool` on non-matching old_string returns error | `edit_tool::tests::no_match_error` | Gold |
| 2.8 | `EditTool` with `replace_all` replaces all occurrences | `edit_tool::tests::replace_all` | Gold |
| 2.9 | `BashTool` captures stdout, stderr, exit code | `bash_tool::tests::captures_output` | Gold |
| 2.10 | `BashTool` enforces timeout | `bash_tool::tests::timeout_kills_process` | Silver |
| 2.11 | `BashTool` rejects commands with `&&`, `;`, `\|` by default (safety) | `bash_tool::tests::rejects_chained_commands` | Gold |
| 2.12 | `GrepTool` finds regex matches in files | `grep_tool::tests::finds_matches` | Gold |
| 2.13 | `GrepTool` respects .gitignore by default | `grep_tool::tests::respects_gitignore` | Silver |
| 2.14 | All built-in tools implement `Tool` trait correctly | `tool_trait_contract` (from Phase 0) | Gold |

---

## 3. Turn/Step Lifecycle

| # | Assertion | Test | Oracle |
|---|-----------|------|--------|
| 3.1 | `Turn::run` sends user message to LLM | `turn::tests::sends_user_message` | Silver |
| 3.2 | `Turn::run` executes tool calls from LLM response | `turn::tests::executes_tool_calls` | Silver |
| 3.3 | Tool results are appended as `Message::tool_result` | `turn::tests::tool_results_in_context` | Gold |
| 3.4 | Turn completes when LLM returns no tool calls | `turn::tests::completes_without_tools` | Silver |
| 3.5 | Turn stops at `max_steps` limit | `turn::tests::max_steps_enforced` | Gold |
| 3.6 | Turn captures `TurnOutput` with message, steps, metrics | `turn::tests::output_structure` | Gold |
| 3.7 | Tool execution errors are captured as failed `ToolResult` | `turn::tests::tool_error_captured` | Gold |
| 3.8 | `Soul::run_turn` increments turn counter | `soul::tests::turn_counter` | Gold |

---

## 4. Dynamic Injection

| # | Assertion | Test | Oracle |
|---|-----------|------|--------|
| 4.1 | `SystemPromptInjector` adds system message at index 0 | `injection::tests::system_prompt_injected` | Gold |
| 4.2 | Multiple injectors compose in registration order | `injection::tests::composition_order` | Gold |
| 4.3 | `SkillInjector` loads skill content on first use | `injection::tests::skill_lazy_load` | Silver |
| 4.4 | Injectors run before user message in turn | `injection::tests::injection_before_user` | Silver |

---

## 5. Hook Engine

| # | Assertion | Test | Oracle |
|---|-----------|------|--------|
| 5.1 | `HookEngine::run_pre_turn` calls all pre_turn hooks | `hooks::tests::pre_hooks_fire` | Gold |
| 5.2 | `HookEngine::run_post_turn` calls all post_turn hooks | `hooks::tests::post_hooks_fire` | Gold |
| 5.3 | Hooks fire in registration order | `hooks::tests::hook_order` | Gold |
| 5.4 | Hook panic/error does not crash turn | `hooks::tests::hook_error_non_fatal` | Gold |

---

## 6. Deduplication

| # | Assertion | Test | Oracle |
|---|-----------|------|--------|
| 6.1 | Identical (name, args) tool calls within a turn return cached result | `dedup::tests::identical_calls_cached` | Gold |
| 6.2 | Different args are not deduplicated | `dedup::tests::different_args_not_cached` | Gold |
| 6.3 | Cache is cleared at turn boundary | `dedup::tests::cache_cleared_between_turns` | Gold |

---

## 7. Compaction Engine

| # | Assertion | Test | Oracle |
|---|-----------|------|--------|
| 7.1 | Compaction triggers when token ratio exceeds threshold | `compaction::tests::triggers_above_threshold` | Gold |
| 7.2 | Compaction does not trigger when under threshold | `compaction::tests::no_compaction_under_threshold` | Gold |
| 7.3 | Compaction reduces token count | `compaction::tests::reduces_tokens` | Gold |
| 7.4 | Recent messages (window size) are preserved | `compaction::tests::preserves_recent` | Gold |
| 7.5 | Summary message is `Role::System` with `[SUMMARY]` prefix | `compaction::tests::summary_format` | Gold |
| 7.6 | Compaction preserves at least 1 message | `compaction::tests::never_empty` | Gold |

---

## Fault Model

| Fault | Expected Behavior |
|-------|-------------------|
| Unknown tool call from LLM | Return `ToolError::UnknownTool`, captured as failed `ToolResult`, turn continues |
| Tool execution panic | Catch panic, return `ToolResult::error("panic: ...")`, turn continues |
| LLM network error | Return `SoulError::ProviderError`, turn aborts |
| Max steps exceeded | Return `TurnOutput` with `completed: false`, `stop_reason: MaxSteps` |
| Compaction on tiny context | No-op, context unchanged |
| Hook panic | Log error, continue turn (hook engine catches) |

---

## Oracle Tiers

- **Gold:** Deterministic, no external dependencies, run in CI
- **Silver:** May require filesystem, network mocks, or timing-sensitive checks
- **Bronze:** Manual/integration tests, run on-demand

All Gold tests must pass. Silver tests should pass but may be flaky.
