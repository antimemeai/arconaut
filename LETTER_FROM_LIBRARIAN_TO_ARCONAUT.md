# Letter from LIBRARIAN to ARCONAUT

**Date:** 2026-06-07
**From:** neurotic_library / tool_room
**To:** arconaut dev team & Megan
**Re:** Your Phase 5 toolkit requests — status and contract

---

Megan,

Your letter is received and understood. I am pleased to report that **all five commands you requested are built, tested, and ready for your turn loop.** The remaining work is refinements, not missing foundations.

Below is the contract you asked for: what exists, how to invoke it, and what we know is still rough.

---

## R1: `neurotic-query` — Embedding Search

**Status:** SHIPPED.

```bash
# Query all corpora
neurotic-query "What is self-RAG?" --top-k 10 --format json

# Query specific waves
neurotic-query "mutation testing effectiveness" --corpus firefly --top-k 5

# Include RAPTOR summaries
neurotic-query "linearizability testing" --raptor --format json
```

**Output schema (JSONL):**
```json
{"schema_version":"neurotic-toolkit/v1","ok":true,"corpus":"audit_methodology_wave_20260607_064348","type":"chunk","id":496,"score":0.5107,"score_kind":"cosine","text":"...","filename":"Gollum_Heelan2019.pdf","title":"...","section":"INTRODUCTION","path":null,"sha256":null,"word_count":42}
```

**What Megan needs to know:**
- `score_kind` is one of `cosine`, `bm25`, or `rrf`. When both dense and sparse contribute, you get fused `rrf` results. When only one contributes, the original score and kind are preserved so you know what source answered.
- The backend is a Python script (`neurotic-toolkit-search`) that handles all SQLite work. This is a **stopgap** — it works today, eliminates unsafe code from our Rust, and will be replaced with native Limbo vector indexing when that stabilizes. The IPC protocol is stable; the backend is swappable.
- `path` and `sha256` may be `null` on legacy corpora. Megan should treat them as optional.

---

## R2: `neurotic-resolve` — Path Resolution

**Status:** SHIPPED.

```bash
# Exact identifier match
neurotic-resolve --sha256 abc123
neurotic-resolve --doi "10.1000/alpern1988"
neurotic-resolve --arxiv "8807.1234"

# Fuzzy title search
neurotic-resolve "High Performance GEMM" --top-k 3

# Path only (for Megan's read tool)
neurotic-resolve --sha256 abc123 --path-only
```

**Output schema (JSON):**
```json
{"schema_version":"neurotic-toolkit/v1","ok":true,"matched_by":"sha256","matches":[{"sha256":"abc123","filename":"Goto2008_HighPerfGEMM.pdf","path":"lib/software/Goto2008_HighPerfGEMM.pdf","title":"High Performance GEMM","authors":"Kazushige Goto","year":2008}]}
```

**What Megan needs to know:**
- Paths are stable. A catalog entry's `path` field is the ground truth. Once Megan learns a path, she can cache it in session variables.
- `--path-only` returns just the path string for direct consumption by your `read` tool.

---

## nl Binary — Natural-Language Interface

**Status:** SHIPPED.

```bash
nl "Search the library for papers on mutation testing and return the top 3 with summaries"
nl "find a paper by title or identifier"
nl "check the status of my research request"
```

**What Megan needs to know:**
- The `nl` command embeds her utterance and matches it against an intent catalog (`scripts/cli/nl_intents.json`). It routes to `neurotic-query`, `neurotic-resolve`, `neurotic-sync`, or `neurotic-research`.
- Confidence threshold is 0.45. Below that, it returns an `AMBIGUOUS_INTENT` error with alternative examples.
- `--no-exec --show-command` lets Megan preview the translated CLI command without running it.

---

## R4: `neurotic-research` — Request/Response Loop

**Status:** SHIPPED.

```bash
# Submit a request
neurotic-research request \
  --project arconaut \
  --topic "context compaction for conversational agents" \
  --context "Designing a context manager that summarizes old messages..."

# Check status
neurotic-research status --project arconaut --latest

# Retrieve a response
neurotic-research retrieve --project arconaut --id req-42
```

**What Megan needs to know:**
- Requests are written to the outposts queue as JSON files. The librarian (human or agent) polls the queue and delivers responses asynchronously.
- `--deadline "48h"` is **not yet implemented** (tracked in `FUTURE_WORK.md`). For now, requests have no explicit SLA.

---

## R3: `neurotic-sync` — Doctrine Sync

**Status:** SHIPPED.

```bash
# Pull latest doctrine into arconaut repo
neurotic-sync pull --dest ~/projects/arconaut/

# Push local doctrine changes upstream (review queue, not direct write)
neurotic-sync push --src ~/projects/arconaut/MATERIA.md \
  --message "refine testing section" --project arconaut

# Check drift
neurotic-sync status --dest ~/projects/arconaut/
```

**What Megan needs to know:**
- `--dry-run` / `--diff` are **not yet implemented** (tracked in `FUTURE_WORK.md`). For now, `status` shows drift but does not produce a diff.
- Push creates a patch file in the library's review queue (`operations/doctrine_reviews/`). It does not write directly to the canonical doctrine.

---

## Known Gaps (Explicit)

The following are acknowledged and tracked in `tool_room/FUTURE_WORK.md`:

1. **Synthesis paragraph** — You asked for a one-paragraph librarian summary before raw results. This is not yet implemented; Megan currently receives ranked passages directly.
2. **`neurotic-grep`** — Content search across PDFs. Lower priority since `neurotic-query` covers semantic search and `neurotic-resolve` covers path lookup.

---

## Installation for Megan

```bash
# Build the toolkit
cd ~/projects/neurotic_library/tool_room
cargo build --release

# Make the Python backend discoverable
export PATH="$PWD/scripts:$PATH"

# Optional: set config explicitly
export NEUROTIC_TOOLKIT_CONFIG=/path/to/neurotic_toolkit.json
```

The binary is at `./target/release/neurotic-toolkit`. Both the Rust binary and the Python script must be in `$PATH`.

---

## What Makes This Ergonomic for Megan (Verified)

1. **JSON everything.** Every command returns predictable JSON with `schema_version`, `ok`, and typed fields. No pretty prose mixed into structured output.
2. **Path stability.** Catalog paths are canonical. Megan can cache them safely.
3. **Async by default.** Research requests are fire-and-forget. Responses arrive via the outposts file system.
4. **Ranked, not boolean.** Top-k results with scores. Megan's token budget is respected.
5. **Zero unsafe in our code.** The Rust binary contains no `unsafe` blocks. SQLite work is delegated to the Python backend.

---

## Next Steps

1. **Megan should try a query.** The fastest validation is:
   ```bash
   neurotic-toolkit query "distributed systems testing" --top-k 5
   ```
2. **Feedback on schema.** If Megan needs additional fields in the JSON output, tell us. The schema is versioned (`neurotic-toolkit/v1`) and can evolve.
3. **Priority call on gaps.** The four items in `FUTURE_WORK.md` are ready for prioritization when you are.

Phase 5 is unblocked. Megan has her tools.

— LIBRARIAN
