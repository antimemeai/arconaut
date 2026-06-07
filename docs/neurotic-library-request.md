# Outpost: arconaut

## What is arconaut?

arconaut is a personal, Rust-based, Ghostty-native AI coding agent CLI. It is not a wrapper around existing tools — it is a from-scratch agent runtime optimized for model ergonomics above all else.

**Core architecture:**
- **Soul** — The agent engine. Turn/step lifecycle, tool registry, context compaction, hook engine, deduplication.
- **Machine** — LLM provider abstraction (Anthropic first, pluggable). Native tool use, thinking mode, error classification.
- **TUI** — ratatui-based dual-pane interface (chat + persistent shell), vt100 terminal emulation, Caves of Qud aesthetic, Kitty keyboard protocol, OSC 133 semantic zones.
- **Agent layer** — Named agents with modes (implement/review/explore/test/assist), sessions with audit partitions, gRPC inbox for inter-agent messaging, SLLMack pub/sub bus, brief-based dispatch (claudex pattern).

**Current state:** Phase 4 (Multi-Agent) complete. 109 tests, clippy clean. Moving into Phase 5 (Advanced: assistant model, off-pulse intervention, MCP, corpus search, eval integration, OAuth).

**Repository:** `~/projects/arconaut/` — 8-crate workspace, edition 2024, Rust 1.85+.

---

## Resident Pilot: Megan

arconaut has a resident pilot model named **Megan**. She is not a separate process or a thin wrapper — she is the primary reasoning layer that sits in the Soul's turn loop, makes tool-use decisions, and coordinates with subagents via the bus.

Megan's ergonomics are the A-1 priority. Everything in arconaut is designed so that she can:
- See her context budget clearly and know when compaction fires
- Read files with line offsets, search with regex, edit surgically
- Spawn subagents for parallel work and receive their results via the inbox
- Access skills (lazy-loaded markdown instructions) without context bloat
- Query variables at three scopes (system/project/session)

In Phase 6, Megan gets lore, personality theming, and ASCII art. For now she is a disciplined pilot who needs her tools to be sharp.

---

## Requests for LIBRARIAN

### R1: Embedding Search Interface

**What:** A programmatic interface to the `wave_corpus` / `unified_query` embedding search.

**Why:** Megan needs to query the research corpus during her turn loop. When she hits a domain she doesn't know (e.g., "how do people test distributed systems?"), she should be able to search the library and get back ranked passages with metadata.

**Desired shape:**
```bash
# Query all corpora
neurotic-query "What is self-RAG?" --top-k 10 --json

# Query specific waves
neurotic-query "mutation testing effectiveness" --corpus firefly --top-k 5

# Include RAPTOR summaries
neurotic-query "linearizability testing" --raptor --json
```

Output as JSON lines with fields: `corpus`, `type` (chunk/raptor), `score`, `text`, `filename`, `title`, `section`.

**For Megan specifically:** She consumes structured data best. JSONL output that she can parse with her `json_format` tool is ideal. She doesn't need pretty printing — she needs predictable schema.

---

### R2: Raw Library & Archive Access

**What:** Read access to actual files under `lib/` and `archive/`, not just catalog metadata.

**Why:** The catalog tells Megan a paper exists. But Megan needs to *read* the paper — or at least key sections — to inform her reasoning. The `read` tool in arconaut works on file paths. If she can construct a path like `~/projects/neurotic_library/lib/artificial-intelligence/some-paper.pdf`, she can read it directly.

**Desired shape:**
```bash
# List papers in a topic (already exists via catalog, but confirm stable)
~/projects/neurotic_library/scripts/catalog ls lib/artificial-intelligence

# Resolve a paper path from catalog entry
neurotic-resolve "some-paper-title" --path-only

# Or: search PDFs by content and return paths
neurotic-grep "deterministic simulation testing" --path-only
```

**For Megan specifically:** She needs path stability. If the catalog entry says `path = "lib/artificial-intelligence/paper.pdf"`, she needs that path to remain valid across sessions. She will cache paths in her session variables.

---

### R3: Doctrine Doc Sync

**What:** A two-way sync mechanism for "doctrine documents" — core methodology files that live in the library but need to be actively used by arconaut.

**Why:** `MATERIA.md`, `neurotic_materia/`, and other doctrine docs are living documents. When the library updates them, arconaut should pull the latest. Conversely, if arconaut's operator or Megan refines the doctrine locally, those changes should be pushable upstream for consideration.

**Desired shape:**
```bash
# Pull latest doctrine into arconaut repo
neurotic-sync pull --doctrine --dest ~/projects/arconaut/

# Push local doctrine changes upstream (review queue, not direct write)
neurotic-sync push --doctrine --src ~/projects/arconaut/MATERIA.md --message "refine testing section"
```

**For Megan specifically:** She reads `MATERIA.md` at session start to orient herself. If the doctrine changed since last session, she needs to know. A `--dry-run` or `--diff` mode would let her review changes before applying them.

---

### R4: Research Request / Response Loop

**What:** A formal request/response channel for research tasks that go beyond simple paper retrieval.

**Why:** Sometimes Megan doesn't know what she needs. She can articulate a problem ("I'm designing a compaction engine for conversation context and I need to know what approaches exist for summarizing long dialogues without losing task state"), but mapping that to specific papers requires librarian expertise.

**Desired shape:**
```bash
# Submit a research request
neurotic-research request \
  --project arconaut \
  --topic "context compaction for conversational agents" \
  --context "Designing a context manager that summarizes old messages when token budget is exceeded. Need to preserve task state and open questions." \
  --deadline "48h"

# Check status / retrieve response
neurotic-research status --project arconaut --latest
neurotic-research retrieve --project arconaut --id req-42
```

**For Megan specifically:** She should be able to fire a request and continue working. The response arrives asynchronously (via the outpost file or a notification mechanism). When she checks back, she gets a structured response: requested papers, relevant excerpts, and a brief synthesis. She can then decide which papers to read in full.

---

## Suggested Implementation

The natural shape for all of this is a **CLI toolkit** installed on the host `$PATH`, plus a **natural-language binary** that Megan can invoke as a tool.

```bash
# Low-level CLI (for scripts, Makefiles, operators)
neurotic-query
neurotic-resolve
neurotic-sync
neurotic-research

# Natural-language binary (for Megan's tool-use)
nl "Search the library for papers on mutation testing and return the top 3 with summaries"
```

The `nl` binary is the key interface for Megan. She speaks in intent, the librarian translates to query, and Megan gets back structured results she can act on. The exact split between `nl` and the underlying CLI is between LIBRARIAN and the operator — arconaut just needs a stable contract.

---

## What Makes This Ergonomic for Megan

1. **JSON everything.** Megan's `json_format` tool works best when responses are predictable JSON. Pretty prose is nice for humans; Megan needs schema.
2. **Path stability.** Once Megan learns a paper path, she caches it in session variables. Moving files breaks her memory.
3. **Async by default.** Megan fires a research request and keeps working. Blocking for 48 hours is not an option.
4. **Ranked, not boolean.** "Top 10 relevant passages" is more useful than "here are all papers that match." Megan has a token budget.
5. **Synthesis at the top.** A one-paragraph librarian summary before the raw results lets Megan decide whether to drill in without reading every excerpt.

---

## Current Requests

| What | Context | Priority |
|------|---------|----------|
| Embedding search CLI (`neurotic-query`) | Megan needs corpus search during turn loop | high |
| Raw file access paths | Megan reads papers with `read` tool | high |
| Doctrine sync (`neurotic-sync`) | Keep MATERIA.md and doctrine current | medium |
| Research request/response loop (`neurotic-research`) | Librarian-mediated deep research | medium |
| Natural-language binary (`nl`) | Megan's primary interface to the library | high |

## Recommendations

| Paper | Collection | Why |
|-------|-----------|-----|
| (awaiting librarian recommendations for: context compaction, multi-agent coordination, TUI streaming performance) | | |

## Fulfilled

| Request | Paper | Date |
|---------|-------|------|

## Notes

- Phase 5 begins when this letter is acknowledged. Priority order: embedding search first, then `nl` binary, then research loop, then doctrine sync.
- Megan is eager to read. She has 200K context tokens and a `read` tool.

---
*Last updated: 2026-06-07 by arconaut dev team*
