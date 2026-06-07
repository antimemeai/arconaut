# Code Review Fixes Subplan

**Source:** `docs/code-review-2026-06-07.md` (comprehensive review, ~12,749 lines)
**Scope:** Only findings still valid in current codebase. Critical/medium findings #1-5 and #8 are already fixed.
**MATERIA discipline:** JSMNTL cycle.

---

## Findings Status

| # | Finding | Status in Current Code | Action |
|---|---------|------------------------|--------|
| 1 | AnthropicMessage drops ToolResult | **FIXED** — handles ToolResult, test passes | None |
| 2 | TUI chat scroll inverted | **FIXED** — Up=sub, Down=add | None |
| 3 | BashTool safety gaps | **FIXED** — rejects `\|\|`, `\n`, `\r` | None |
| 4 | GrepTool blocks async runtime | **FIXED** — uses `tokio::fs` | None |
| 5 | CompositeInjector single prompt | **CORRECT** — tests pass for composed distinct prompts | None |
| 6 | CompactionEngine wipes checkpoints | **BUG** — `clear()` preserves vec but indices stale after rebuild | **Fix** |
| 7 | TerminalSendTool 100ms race | **IMPROVED** — `poll_buffer` with stability detection | Minor |
| 8 | InboxServer subscriber leak | **FIXED** — removes old subscriber before insert | None |
| 9 | estimate_tokens rough | **VALID** — `text_len / 4` underestimates CJK | **Fix** |
| 10 | GrepTool gitignore naive | **VALID** — substring containment, false positives | **Fix** |
| 11 | SkillLoader frontmatter fragile | **VALID** — no quoted YAML, multiline, indentation | **Fix** |
| 12 | DocumentIndex no recurse | **VALID** — `scan_dir` only top-level | **Fix** |
| 13 | PdfGenerator single-page | **VALID** — truncates at y < 50.0 | **Fix** |
| 14 | run_single_turn stripped down | **VALID** — no compaction, injection, hooks, skills, bridge | **Fix** |

---

## Implementation Order

### Track A: Critical Correctness
| # | Task | File | Lines | Complexity |
|---|------|------|-------|------------|
| A.1 | CompactionEngine: clear stale checkpoints | `arconaut-agent/src/compaction.rs` | 64 | Low |
| A.2 | Add test: revert after compaction is invalid | `arconaut-agent/src/compaction.rs` | tests | Low |

### Track B: Tool Quality
| # | Task | File | Lines | Complexity |
|---|------|------|-------|------------|
| B.1 | GrepTool: use `glob`/`ignore` crate for gitignore | `arconaut-machine/src/tools.rs` | 423-433 | Medium |
| B.2 | SkillLoader: document frontmatter restriction | `arconaut-machine/src/skills.rs` | — | Low |

### Track C: Core Polish
| # | Task | File | Lines | Complexity |
|---|------|------|-------|------------|
| C.1 | DocumentIndex: recursive scan | `arconaut-core/src/docs.rs` | 65-96 | Low |
| C.2 | estimate_tokens: add CJK-aware heuristic | `arconaut-core/src/context.rs` | 99-109 | Low |
| C.3 | PdfGenerator: document single-page limit | `arconaut-core/src/pdf.rs` | — | Low |

### Track D: CLI Parity
| # | Task | File | Lines | Complexity |
|---|------|------|-------|------------|
| D.1 | run_single_turn: add compaction, injection, hooks | `arconaut-cli/src/main.rs` | 388-424 | Medium |

---

## Conformance Spec

| # | Assertion | Test | Oracle |
|---|-----------|------|--------|
| A.1 | `compact()` clears all pre-existing checkpoints | `compaction::tests::checkpoints_cleared` | Gold |
| A.2 | Revert to checkpoint created before compaction returns error | `compaction::tests::revert_after_compaction_fails` | Gold |
| B.1 | `target/` in gitignore does NOT match `not-target/foo` | `tools::tests::gitignore_no_false_positive` | Gold |
| C.1 | `scan()` finds files in nested directories | `docs::tests::scan_recursive` | Gold |
| C.2 | CJK text token estimate ≥ `text_len / 2` | `context::tests::cjk_token_estimate` | Gold |
| D.1 | `run_single_turn` uses Soul with compaction + hooks | cli integration | Silver |
