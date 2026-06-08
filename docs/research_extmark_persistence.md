# Research: Semantic Marker Persistence in Editors

How do editors persist bookmarks, breakpoints, and semantic annotations across sessions? A survey of Neovim, Emacs, VS Code, JetBrains, and Eclipse reveals a convergent pattern: **runtime markers are ephemeral; only the *intent* is serialized.** The recommended strategy for arconaut is a **versioned JSON intent file** (`.arconaut/marks.json`) with client-generated UUIDs, contextual anchoring, and lazy recreation on buffer load.

## Patterns Across Editors

**Neovim** — `nvim_buf_set_extmark` provides no built-in serialization. Extmark IDs are session-scoped integers. Plugins like `notebook.nvim` store state externally and recreate extmarks after `BufRead`. The extmark is a runtime artifact; the source of truth lives in plugin state.

**Emacs** — Bookmarks persist to `~/.emacs.d/bookmarks`, a flat Lisp-readable file. Each bookmark records `(file . position)` plus surrounding text context (`bookmark-search-size` chars on each side). On jump, Emacs searches the context to re-anchor if the file has drifted. Auto-save on change or exit via `bookmark-save-flag`.

**VS Code** — Decorations are purely ephemeral. Extensions use `workspaceState` / `globalState`—SQLite-backed `Memento` key-value stores. The pattern: persist minimal JSON descriptors, then re-emit decorations during `activate()`. The runtime never stores handles.

**JetBrains / Eclipse** — JetBrains serializes via `PersistentStateComponent` into `.idea/workspace.xml` (file URLs, line numbers, conditions). Eclipse markers declare persistence via `org.eclipse.core.resources.markers`. Both keep metadata separate from source in workspace-scoped, git-ignored files.

**Research Literature** — Sulír’s thesis distinguishes **in-code** annotations from **external** metadata. Horvath et al. (UIST 2022) introduce *ephemeral* annotations (notes that do not modify source) versus *permanent* ones saved alongside files. For a CLI agent that must not pollute `git` history, external ephemeral storage is correct.

## Recommended Format: `.arconaut/marks.json`

```json
{
  "version": 1,
  "marks": [
    {
      "id": "550e8400-e29b-41d4-a716-446655440000",
      "ns": "soul/decision",
      "file": "src/soul/engine.rs",
      "type": "decision",
      "anchor": {
        "line": 42,
        "col": 8,
        "context_before": "fn decide_next_action(&mut self",
        "context_after": ") -> Action {"
      },
      "metadata": { "reasoning_id": "r-128" }
    }
  ]
}
```

**Key fields:**
- `id`: Client-generated UUID. Never reuse Neovim’s runtime extmark ID.
- `ns`: Logical namespace (e.g., `soul/decision`) to avoid collisions between subsystems.
- `anchor`: Line/col are *hints*. The `context_before`/`context_after` strings enable fuzzy re-anchoring when files shift.
- `type`: Arbitrary tag consumed by the recreating subsystem.

## Handling Hard Problems

**ID Conflicts** — Eliminated by design. Arconaut generates UUIDs; Neovim’s integer IDs are used only ephemerally. On load, map `uuid → nvim_extmark_id` in a per-buffer HashMap.

**Stale Markers** — On buffer open, if `context_before/after` no longer exists at the claimed line, search within ±10 lines. If still unmatched, mark `stale: true` and surface in the TUI for review rather than silently deleting.

**File Renames** — Store paths relative to the project root. On `git mv` or external rename, arconaut’s file-watcher should update paths in `marks.json`. If a mark references a missing file, resolve via `git diff --name-status` or fuzzy matching.

## Recommended Persistence Strategy

1. **Write-through with debounce.** Every extmark mutation updates an in-memory `MarkRegistry`. A 5-second debounced task serializes to `.arconaut/marks.json`. Flush immediately on `SIGTERM` or `nvim_detach`.
2. **Lazy recreation.** When Neovim opens a buffer, `arconaut.lua` calls back to Rust with the buffer name. Rust queries `MarkRegistry` for that file and issues `nvim_buf_set_extmark` for each mark, storing the returned nvim ID in the session map.
3. **Partition by namespace.** Use separate `nvim_create_namespace` instances for each `ns` field. This allows bulk clearing (e.g., wipe all `soul/decision` marks) without affecting others.
4. **Treat `.arconaut/marks.json` as ephemeral intent.** Git-ignore it by default. If users want permanent annotations, a future feature can export marks as inline comments or `CLAUDE.md` entries—following the research distinction between ephemeral and permanent persistence.

This gives arconaut the resilience of Emacs’ contextual bookmarks, the isolation of VS Code’s Memento pattern, and Neovim’s runtime efficiency—without polluting source history.
