# Phase 3 Subplan: Scaffolding

**Issue:** arconaut-dsc

## Components

### 1. Lazy Skill Loader (`arconaut-machine/src/skills.rs`)
- `Skill` struct: name, description, file_path, source
- `SkillLoader` discovers skills from:
  - `~/.config/arconaut/skills/` (user)
  - `./.arconaut/skills/` (project)
- Discovery rules: `SKILL.md` in root = skill; otherwise recurse for `.md` files
- Pi pattern: only metadata (name, description, path) exposed to LLM; content loaded on demand via `skill://{name}` or `/skill:{name}`
- `skill` tool for LLM to load and read skill content

### 2. Variable Storage (`arconaut-core/src/vars.rs`)
- Three scopes: System (`~/.config/arconaut/vars.toml`), Project (`./.arconaut/vars.toml`), Session (in-memory HashMap)
- `VariableStore` with get/set for each scope
- System/project loaded from disk on startup; session is ephemeral
- Template substitution: `{var:system.key}` syntax

### 3. Document/Report List (`arconaut-core/src/docs.rs`)
- `Document` struct: name, path, kind (markdown, pdf, etc.), created_at
- `DocumentIndex` scans `docs/` and `reports/` directories
- List/filter documents by kind or date

### 4. PDF Generation (`arconaut-core/src/pdf.rs`)
- `PdfGenerator` converts markdown → PDF
- Uses `pulldown-cmark` for markdown parsing, `pdf-writer` for PDF output
- Minimal formatting: headings, paragraphs, code blocks as mono text
- `report` tool for LLM to generate PDFs from markdown content

### 5. Utilities Bin (`arconaut-cli/src/utils.rs`)
- Framework for small utility commands exposed to LLM
- Each utility is a Rust function with name, description, and JSON-schema args
- Built-in utilities: `uuid`, `timestamp`, `hash`, `base64`, `json_format`

## Dependencies

- `toml` — TOML parsing for variables and config (lightweight, pure Rust)
- `pulldown-cmark` — Markdown parsing for skills and PDF generation (lightweight, pure Rust)
- `pdf-writer` — Low-level PDF writer (lightweight, pure Rust, minimal deps)

## Tests

All Gold tier: deterministic, no external deps, run in CI.

## Open Questions

- Should skills be hot-reloaded? (Deferred to Phase 5)
- Should PDF support images? (Deferred — text-only for Phase 3)
