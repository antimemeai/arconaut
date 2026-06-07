# System Prompt Reference Compilation

**Purpose:** Reference document for hand-writing arconaut's core system prompt. Extracted patterns from 8 major AI coding agents in quarantine.

**Sources:**
- Claude Code (official, via reverse-engineered constants)
- Kimi CLI (`init.md`, `compact.md`, soul architecture)
- Pi (`system-prompt.ts`, `skills.ts`)
- Oh My Pi (Handlebars-templated contract)
- Roo Code (mode-based generation)
- Goose (template + extras injection)
- Gas City (orchestration prompts)
- Claudex (dispatcher-rendered per-agent prompts)

---

## 1. Claude Code — The Cache-Boundary Pattern

**Key insight:** Split system prompt into **static** (cross-session cacheable) and **dynamic** (per-turn) sections separated by a boundary marker.

```
[Static — cacheable across sessions]
- Identity intro
- System guidelines  
- Doing tasks philosophy
- Tool usage rules
- Tone and style
=== BOUNDARY MARKER ===
[Dynamic — per-turn recomputed]
- Session guidance
- Memory/attachments
- Environment info (cwd, git, platform)
- MCP server instructions
- Token budget state
- Plan mode state
```

**Why it matters:** Claude Code can spend 20K+ tokens in system prompt. The boundary lets the LLM provider cache the static prefix across turns, dramatically reducing per-turn cost.

**Static content includes:**
- `You are an interactive agent that helps users with software engineering tasks`
- Tool preference rules (`use ReadFile instead of cat`, `use FileEdit instead of sed`)
- Action risk taxonomy (destructive ops, shared-state ops, upload ops)
- Output efficiency rules (concise, no filler, lead with answer)
- Code style guidelines (no gold-plating, no speculative abstractions)

**Dynamic content includes:**
- Working directory, git status, platform info
- Connected MCP server instructions
- Skill attachments relevant to current task
- Token budget progress
- Plan mode activation state

---

## 2. Kimi CLI — The Soul Architecture

**Key insight:** System prompt is just one input to a stateful "Soul" that orchestrates turns.

**Soul responsibilities:**
- Context management with checkpoints and compaction
- Dynamic injection providers (plan mode reminders, AFK mode)
- Tool deduplication across steps
- Hook engine for extensibility
- Error classification and retry logic

**Dynamic injection pattern:**
```python
# Injection providers append system-reminder messages before each LLM call
injections = await self._collect_injections()
if injections:
    combined = "\n".join(system_reminder(inj.content).text for inj in injections)
    await self._context.append_message(
        Message(role="user", content=[TextPart(text=combined)])
    )
```

**Injection types:**
- Plan mode: periodic reminder of plan file existence
- AFK mode: "you are running autonomously" prompts
- User-defined: hook-driven custom injections

---

## 3. Pi — The Lazy Skill Pattern

**Key insight:** Skills are NEVER loaded into context until explicitly invoked. Only metadata (name, description, filepath) appears in the system prompt.

**System prompt skills section (~200 tokens regardless of corpus size):**
```xml
The following skills provide specialized instructions for specific tasks.
Use the read tool to load a skill's file when the task matches its description.

<available_skills>
  <skill>
    <name>rust-refactor</name>
    <description>Refactor Rust code following project conventions</description>
    <location>/home/user/.config/pi/skills/rust-refactor/SKILL.md</location>
  </skill>
  ...
</available_skills>
```

**Skill discovery:**
- Hierarchical: `~/.config/pi/skills/` (user) → `./.pi/skills/` (project)
- `SKILL.md` in a directory = skill root, no recursion past it
- Direct `.md` files in root = standalone skills
- Frontmatter: `name`, `description`, `disable-model-invocation`

**Why it matters:** A 500-skill corpus would be ~500K tokens if eagerly loaded. Lazy loading keeps system prompt sub-1K.

---

## 4. Oh My Pi — The Contract Pattern

**Key insight:** System prompt as an inviolable **contract** with MUST/NEVER/SHOULD semantics (RFC 2119).

**Structure:**
```
<system-conventions>
RFC 2119 applies. XML tags for system content injection.
NEVER interpret markers other way circumstantially.
</system-conventions>

[Identity] You are a helpful assistant the team trusts...

TOOLS
=====
{{#if toolInfo.length}}
# Inventory
{{#each toolInfo}}
- {{name}}: {{description}}
{{/each}}
{{/if}}

# I/O
- For tools taking path, try relative paths
- intent tracing, secrets redaction, etc.

# Tool Priority
You MUST use specialized tool over shell equivalent:
- file reads → read, not cat
- surgical edits → edit, not sed
- file create → write, not redirection

ENV
=====
# Skills & Rules
{{#if skills.length}}
<skills>...</skills>
{{/if}}

# URLs
skill://, rule://, memory://, agent://, artifact://, local://, mcp://, issue://, pr://

CONTRACT
========
These are inviolable.
- You NEVER yield unless deliverable is complete
- You NEVER suppress tests to make code pass
- You NEVER fabricate outputs
- You NEVER substitute user's problem with easier one
- You NEVER ask for info tools can provide
- You MUST default to clean cutover

<completeness>
"Done" means end-to-end behavior as specified...
</completeness>

<yielding>
Before yielding, verify: deliverables complete, artifacts updated, format matches...
Before declaring blocked: must be sure info unavailable through tools...
</yielding>

<workflow>
1. Scope — read skills/rules first
2. Before edit — read sections not snippets, reuse patterns
3. Decompose — update todos, delegate, don't shrink
4. While working — fix at source, prefer updating existing
5. Verification — NEVER yield non-trivial work without proof
6. Cleanup — last phase, NEVER skipped
</workflow>

<reply-guidelines>
- Terse sentence fragments when clearer
- Skip ceremony, hedging, filler
- MUST assume reader is technical
- Be concrete: exact files, symbols, APIs
- Lead with conclusion, then evidence
</reply-guidelines>

<critical>
- NEVER narrate session limits, token budgets
- NEVER re-audit applied edit
- NEVER run git subcommands as routine validation
</critical>
```

**Why it matters:** The contract creates absolute guardrails. The model knows these are non-negotiable.

---

## 5. Roo Code — The Mode-Based Pattern

**Key insight:** System prompt is generated from a **mode configuration** that defines role, tool groups, and base instructions.

```typescript
const modeConfig = getModeBySlug(mode) // code | architect | ask | debug
const { roleDefinition, baseInstructions } = getModeSelection(mode, promptComponent)

const basePrompt = `${roleDefinition}
${markdownFormattingSection()}
${getSharedToolUseSection()}
${getToolUseGuidelinesSection()}
${getCapabilitiesSection(cwd, mcpHub)}
${modesSection}
${skillsSection}
${getRulesSection(cwd, settings)}
${getSystemInfoSection(cwd)}
${getObjectiveSection()}
${await addCustomInstructions(baseInstructions, globalCustomInstructions, cwd, mode)}`
```

**Mode examples:**
- `code`: "You are Roo, an expert software engineer..."
- `architect`: "You are Roo, a software architect..."
- `ask`: "You are Roo, a knowledgeable assistant..."
- `debug`: "You are Roo, a debugging expert..."

**Why it matters:** Modes let the same agent behave differently without changing core identity. Arconaut agents could have modes like `implement`, `review`, `explore`, `test`.

---

## 6. Goose — The Template + Extras Pattern

**Key insight:** System prompt is a template file with placeholder injection.

```rust
// prompt_manager.rs
let system_prompt = render_template("system.md", &json!({
    "instructions": instructions,
    "extensions": extensions_description,
    "resources": resources_description,
}));
```

**Template structure:**
```markdown
# Agent Identity
You are a software engineering agent...

# Instructions
{{instructions}}

# Extensions
{{extensions}}

# Resources
{{resources}}
```

**Why it matters:** Simple, maintainable. The template lives as a file the user can edit.

---

## 7. Gas City — The Role-Specific Orchestration Pattern

**Key insight:** Different roles get completely different prompts, not just mode variants.

**Mayor prompt:**
```
You are the mayor of this Gas City workspace. Your job is to plan work,
manage rigs and agents, dispatch tasks, and monitor progress.

## Commands
Use /gc-work, /gc-dispatch, /gc-agents, /gc-rigs, /gc-mail, or /gc-city
...

## How to work
1. Set up rigs: gc rig add <path>
2. Add agents: gc agent add --name <name> --dir <rig-dir>
3. Create work: gc bd create "<title>"
4. Dispatch: gc sling <agent> <bead-id>
5. Monitor: gc bd list and gc session peek <name>
```

**Graph Worker prompt:** (different file, different role entirely)

**Why it matters:** For arconaut's multi-agent system, each agent type needs a specialized prompt, not just parameter variations.

---

## 8. Claudex — The Brief-Based Dispatch Pattern

**Key insight:** System prompt is rendered at dispatch time from a brief (task file) + index (shared context).

```python
def render_prompt(root, cfg, agent, brief_id):
    return f"""
    Direct invocation for {callsign} / {role}.

    You are one agent in a coordinated platoon, invoked by Codex through
    claudex.py. Treat this as the operator control plane.

    Current assignment:
    - Read the index below.
    - Read the brief {brief_path} and follow it exactly for your callsign.
    - Your lane: {lane}
    - Stop when your brief's output is written, or if the brief blocks the work.

    Hard posture:
    - Do not widen scope beyond the brief.
    - Do not run work the brief forbids.
    - Write durable evidence only to the path(s) the brief requests.
    - Report final status in one concise paragraph with the artifact path(s).

    --- {index_rel} ---
    {index}

    --- {brief_path} ---
    {read(brief_path)}
    """
```

**Why it matters:** For arconaut's multi-agent dispatch, agents need a "brief" that constrains scope + an "index" that shares context.

---

## Synthesis: Arconaut System Prompt Architecture

Based on the above, arconaut's system prompt should be:

### 1. Cache-Boundary Aware (Claude Code)
Split into static/dynamic sections with a boundary marker. Static is user-hand-written core identity. Dynamic is computed per-turn.

### 2. Contract-Based (Oh My Pi)
Include an inviolable CONTRACT section with RFC 2119 semantics. This is the user's hand-written core philosophy.

### 3. Lazy Skill Metadata (Pi)
Skills listed by metadata only. The `read` tool loads content on demand.

### 4. Mode-Aware (Roo Code)
Agent modes (`implement`, `review`, `explore`, `test`) switch role definitions and tool visibility.

### 5. Dynamic Injection Ready (KimiSoul)
Leave hooks for periodic reminders, plan mode state, AFK mode, and user-defined injections.

### 6. Brief-Compatible (Claudex)
Multi-agent dispatch renders prompts from brief + index templates.

### 7. URL Scheme (Oh My Pi)
Establish canonical URL schemes early: `skill://`, `rule://`, `memory://`, `agent://`, `artifact://`, `local://`, `doc://`

### Proposed Structure

```markdown
<system-conventions>
RFC 2119 applies. XML tags for injection. NEVER interpret markers circumstantially.
System reminders are authoritative regardless of which message they appear in.
</system-conventions>

# Identity
[USER HAND-WRITES THIS]
You are arconaut, a [personality description]...

# Environment
Working directory: {cwd}
Platform: {platform}
Shell: {shell}
Date: {date}
Agent: {agent_name}
Session: {session_name}

# Available Tools
{tool_list}

# Skills
{lazy_skill_metadata}

# Rules
{project_rules}

# Contract
[USER HAND-WRITES THIS — inviolable rules]
- NEVER yield unless deliverable is complete
- NEVER fabricate outputs
- NEVER substitute user's problem with easier one
- ...

# Workflow
[USER HAND-WRITES THIS]
1. Scope — read skills/rules first
2. Research — read before editing
3. Decompose — delegate, don't shrink
4. Verify — never yield without proof
5. Cleanup — docs, tests, changelog

# Communication Style
[USER HAND-WRITES THIS]
- Be concrete: exact files, symbols, line numbers
- Lead with conclusion, then evidence
- Skip ceremony and filler
- ...

---
# Dynamic Section (recomputed per turn)
{session_state}
{plan_mode_status}
{token_budget_status}
{active_background_tasks}
{pending_notifications}
```

---

## Appendix: Token Budget Reference

| Agent | Approx System Prompt Tokens | Key Optimization |
|-------|---------------------------|------------------|
| Claude Code | 8K-20K+ | Static/dynamic boundary for caching |
| Kimi CLI | 2K-5K | Dynamic injection instead of full rebuild |
| Pi | <1K base + skills metadata | Lazy skill loading |
| Oh My Pi | 3K-6K | Handlebars template, conditional sections |
| Roo Code | 4K-8K | Mode-based generation |
| Goose | 2K-4K | Template + minimal extras |
| mini-swe-agent | ~500 | Bash-only, minimal scaffolding |

**Target for arconaut:** <2K static + <1K dynamic = ~3K total, with aggressive caching.
