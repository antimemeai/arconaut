# Phase 3 Conformance Specification

**Issue:** arconaut-dsc

---

## 1. Skill Loader

| # | Assertion | Test | Oracle |
|---|-----------|------|--------|
| 1.1 | `SkillLoader` discovers `SKILL.md` files | `skills::tests::discovery_skill_md` | Gold |
| 1.2 | `SkillLoader` recurses into subdirectories | `skills::tests::discovery_recursive` | Gold |
| 1.3 | Skill metadata exposed without loading content | `skills::tests::lazy_metadata` | Gold |
| 1.4 | Skill content loaded on demand | `skills::tests::load_content` | Gold |
| 1.5 | `skill` tool registered and callable | `skills::tests::skill_tool_call` | Gold |

## 2. Variable Storage

| # | Assertion | Test | Oracle |
|---|-----------|------|--------|
| 2.1 | System variables loaded from TOML | `vars::tests::system_load` | Gold |
| 2.2 | Project variables loaded from TOML | `vars::tests::project_load` | Gold |
| 2.3 | Session variables are ephemeral | `vars::tests::session_ephemeral` | Gold |
| 2.4 | Variable precedence: session > project > system | `vars::tests::precedence` | Gold |
| 2.5 | Template substitution works | `vars::tests::substitution` | Gold |

## 3. Document/Report List

| # | Assertion | Test | Oracle |
|---|-----------|------|--------|
| 3.1 | `DocumentIndex` scans directories | `docs::tests::scan` | Gold |
| 3.2 | Documents filtered by kind | `docs::tests::filter_kind` | Gold |
| 3.3 | Documents sorted by date | `docs::tests::sort_date` | Gold |

## 4. PDF Generation

| # | Assertion | Test | Oracle |
|---|-----------|------|--------|
| 4.1 | Markdown parsed into events | `pdf::tests::parse_markdown` | Gold |
| 4.2 | PDF file created and valid | `pdf::tests::generate_pdf` | Silver |
| 4.3 | Headings and paragraphs rendered | `pdf::tests::render_headings` | Silver |

## 5. Utilities Bin

| # | Assertion | Test | Oracle |
|---|-----------|------|--------|
| 5.1 | `uuid` utility generates valid UUID | `utils::tests::uuid_valid` | Gold |
| 5.2 | `timestamp` utility returns ISO string | `utils::tests::timestamp_iso` | Gold |
| 5.3 | `hash` utility produces consistent output | `utils::tests::hash_consistent` | Gold |
| 5.4 | `base64` encode/decode roundtrip | `utils::tests::base64_roundtrip` | Gold |
| 5.5 | `json_format` pretty-prints JSON | `utils::tests::json_format` | Gold |

---

## Fault Model

| Fault | Expected Behavior |
|-------|-------------------|
| Missing vars.toml | Default to empty store, no panic |
| Malformed TOML | Log error, skip file, continue |
| Skill file not found | Tool returns error, brief "not found" |
| PDF write failure | Tool returns error with path |
| Document directory missing | Empty index, no panic |

---

## Oracle Tiers

- **Gold:** Deterministic, no external deps, run in CI
- **Silver:** Requires filesystem I/O

All Gold tests must pass. Silver tests should pass but may be skipped in sandboxed environments.
