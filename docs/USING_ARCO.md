# Use Arco

From this repository, run `./scripts/arco` (the optimized release executable). In an interactive terminal it opens the
TUI. Piped input and `--once` use the plain interface; `--plain` selects it explicitly.
Arco uses your existing Codex ChatGPT sign-in; Codex supplies authentication only.
Arco runs the requests, tools, context and workflows.

```sh
./scripts/arco --session context/live-bootstrap
```

Choose another `--session DIRECTORY` for a separate conversation. Default session:
`~/.local/state/arconaut/default`. The working directory is the tool working
directory. Only one process may hold a session. `--once 'prompt'` runs one turn.
`--model NAME` selects the model; `--effort low|medium|high|xhigh` selects reasoning
(initial default `medium`). Model, effort and absolute workflow path persist with
the session; explicit launch options override and save them. Actor/conversation/
workflow identities also survive restart. Lua request options can override model
request defaults for a particular call. `/session` shows configuration and identities;
`/workflow FILE` validates and saves a different turn program.

## Find and resume sessions

`./scripts/arco --list-sessions [ROOT]` lists direct child session directories
(default `~/.local/state/arconaut`) and ROOT itself if it is a session. `/sessions`
lists the current session's parent. Results have stable absolute paths, Unix-second
last activity from audit mtime (null with explicit unavailable status if stat fails),
saved model/effort/workflow and a shell-quoted
explicit `./scripts/arco --session 'PATH'` resume command. Ordinary reopen does not
consume a pending RRC intent; use `--resume-continue` explicitly when appropriate.

Listing reads bounded `session-info.json` configuration snapshots and stats audits;
it never locks, opens for recovery, creates directories or changes session files.
Snapshots are derived from the owning process's audited settings, not authority.
Legacy sessions have unavailable configuration until normally opened; damaged or
missing snapshots remain listed with explicit status. Snapshot-save failures warn,
so configuration may be stale then; audit stays authoritative. A root name beginning
with `--` should be spelled `./--NAME` or as an absolute path. Listing is local,
nonrecursive, skips symlink children and rejects nonregular metadata files, and
does not maintain a global session catalog.

## Terminal interaction

The conversation, live activity/timing and composer stay visible. Assistant text
streams as it arrives; commands, file paths, process output, exit codes and tool
errors appear during work. Previous user/assistant context is shown on reopening.

Turn and active native tool timers are distinct (`Turn Ns · Tool Ns`); native
completion shows elapsed milliseconds after its retained terminal observation.
Turn end is labeled completed, cancelled or failed. A completed tool is not shown
as still running during later pure-Lua work. Cancellation still records uncertain
outcomes where appropriate; it never invents successful completion.

While PgUp-scrolled, new output keeps the same-width viewport anchored; `Up N`
shows distance from the live bottom. PgDn to bottom resumes following output.
Resize clamps the retained viewport and recomputes composer/cursor cell widths.
Approximately 1 MiB of display is retained even for giant no-newline output, with
an explicit trim notice; full originals remain audited and A2 retrieval still works.

- Enter sends; Alt-Enter (or Ctrl-J) adds a line. Clipboard paste stays multiline
  until you send it. Left/right, Home/End, Backspace/Delete and Ctrl-A/E/U edit.
- Up/down recall persisted submitted-prompt history. PgUp/PgDn scroll the view.
- Type and submit during a turn to queue the next prompt. Commands queue too, so
  configuration changes apply after the current turn.
- Ctrl-Q exits while preserving draft and queued work as recoverable drafts.
- Ctrl-C stops the active turn and clears the queue, preserving the draft you are
  editing. Ctrl-C also preserves the draft when idle; Ctrl-U clears it. `/quit`, `/exit` or Ctrl-D on an empty draft
  exits; an active turn is stopped first. Terminal settings restore on exit.

Commands: `/help`, `/model NAME`, `/effort LEVEL`, `/context`, `/stats`, `/originals`,
`/restore ENTRY`, `/lua CODE`, `/clear` (view only), `/drafts`, `/draft N`, `/quit, `/exit`, `/session`, `/sessions`, `/workflow FILE`, `/restart NOTE`. Unknown slash commands report a local error without a model request.
In plain mode `/paste` collects a block until `/send`.
`--audit-last` inspects recent original records without a provider request.

This first TUI uses ANSI terminals with bracketed paste and at least 12 columns
by 10 rows. Editing uses UTF-8 codepoints; terminal wcwidth determines wrapping.
Complex emoji/grapheme editing is later work. Drafts are limited to 1 MiB, screen
history to approximately 1 MiB and prompt history to 128 entries. These presentation
limits do not trim retained context or original audit bytes. Queued prompts remain
UI drafts until their turn begins. Composer, cursor and submitted-prompt history
persist in replaceable `ui-state.json` within the private locked session directory;
this is UI state, not provider context. History and queued/recovered draft shelves
also have aggregate 1 MiB limits and at most 128 entries. On reopen, prior queued
prompts become **recovered drafts**, never runnable work. `/drafts` lists them;
`/draft N` loads one into the empty composer, and a separate Enter submits it.
Use Ctrl-U first if retaining a current draft elsewhere is intended. Automatic RRC
continuation runs first; restored drafts stay available behind it. A prompt is
removed from pending UI state before dispatch, so it cannot be replayed on reopen.
Invalid/unreadable UI state reports a warning without repairing audit. Atomic
replacement preserves the preceding saved state on write failure; a visible warning
keeps unsaved drafts in RAM and prevents new work dispatch until a save succeeds.
Recovered cursor offsets inside UTF-8 continuation bytes move back to a boundary.
No power-loss
or arbitrary-crash exact-keystroke durability is claimed.

## Opt-in cooperative backstop

`--backstop MISSION_JSON_FILE --once 'PROMPT'` supplies a small author-declared
mission packet for recovery after that workflow fails. The file contains an object
with a nonempty `mission` string; optional fields retain steer/resource observations.
`--workflow FILE` can select the failed source workflow. This is same-lifetime native
custody, not authority inferred from a parent exit, old lease, or reopened session.
Explicit cancellation stays paused; arbitrary exec makes containment unavailable.
The fresh `SESSION/backstop/audit` preserves declared lineage and old unknowns,
without inheriting attempt admissions. Two independent assessment/pivot attempts and
a two-minute deadline bound recovery. Selected actions use native file/context APIs,
not exec/restart or additional provider calls. Old-session mutations (including
aliases) are refused. Success observes a declared changed file artifact; it does not
settle remote effects or certify semantic usefulness. See
[BACKSTOP_OPERATING](BACKSTOP_OPERATING.md) for examples and limits.

## Lua programs and context

`programs/turn.lua` runs the request/tool loop. Edit it to change the next turn or
select another with `--workflow FILE`. Each turn reads and retains its source and
creates a fresh Lua VM. Globals are ephemeral; use context, files or services for
persistent state. The model has a `lua` tool and the same APIs:

- `arco.request(options)` assembles from live context, with optional model,
  instructions, input, tools, reasoning and other overrides. This transport
  currently requires `store=false` and `stream=true`.
- `arco.call(name, arguments)` runs a tool with an object or JSON string.
  `read_file`, `write_file`, `edit_file` access files. `exec` accepts `command` or
  `argv` and a timeout in seconds. Nonempty argv takes precedence; empty argv uses
  the command. File/process effects run without command-approval prompts.
  `read_file` also accepts optional `byte_start`/`byte_end` (zero-based,
  half-open) or `line_start`/`line_end` (one-based, inclusive). Do not mix modes.
  Missing start means first byte/line; missing end means EOF. Beyond-EOF ranges
  clip or return empty; reversed, negative, noninteger ranges and line zero fail.
  LF is retained, CR is ordinary data, and a trailing LF creates no extra line.
  Full original file bytes remain audited; invalid UTF-8 slices return hex.
  Reads still have the existing 16 MiB full-file capacity, even for small ranges.
  `exec` accepts `output_max_bytes` to bound the raw prefix bytes in its
  model-facing result (JSON encoding overhead is not included). With a budget,
  `output_bytes` and `omitted_bytes` describe the full and omitted counts. Normal
  `exec` results include `output_ref`; use
  `arco.call("read_process_output", {output_ref=REF, byte_start=N, byte_end=M})`
  to retrieve original combined stdout/stderr from retained audit without replay.
  Ranges use the same byte semantics as file reads, and survive reopen. Original
  process chunks and terminal streaming are not clipped by this model budget.
  Timeout/cancellation still interrupt the turn; partial chunks and their attempt
  references remain in audit for explicit later inspection/retrieval. The existing
  16 MiB process collection capacity remains; this is not a memory/stream budget.
- `arco.context()` returns `{base, entries}`; each entry has stable `id` and `item`.
  Transform the presentation and use `arco.edit(candidate)`. Stale bases and invalid
  IDs produce retained rejections. Edits/removals preserve originals.
- `arco.stats()` (also `/stats` and `context_stats`) reports entry count,
  per-entry item bytes, serialized input-array bytes and full view bytes. All are
  compact UTF-8 JSON byte counts, **not tokens**. `last_request_bytes` measures the
  exact serialized payload after request overrides; authentication headers are not
  part of it. Request size is shown before provider transport; actual returned
  token usage is shown after response. `usage` contains only numeric input/output/
  total counts and cached/reasoning details, never arbitrary extension strings.
  Last request/usage are scoped to this process; null means unavailable. No token
  estimates or model-limit guesses are made.
- `arco.originals()` lists originals; `arco.restore(id)` repairs an item.
  `arco.append(items)` adds synthetic items with fresh original IDs.
- `arco.json.decode/encode` preserve null, empty arrays and exact numeric lexemes.
  Use `arco.array({})` for a new empty array.
- `arco.present(item)` displays a completed assistant message without duplicating
  matching streamed text. `arco.display(text)` and `print(...)` retain explicit
  display output. Streaming preview is derived from already-retained raw bytes.

Base/coroutine/table/string/math/utf8 Lua facilities are available. Host I/O goes
through `arco.call`; raw io/os/package/debug and file loaders are absent. Evaluate
modules with `load` on audited `read_file` content. This keeps effects in the audit;
it is not a hostile-code sandbox or an approval system.

Preserve Responses call/result linkage when editing context. Opaque items stay
originals even after presentation edits. Final requests, raw provider streams,
input/results, file prior/proposed bytes, process chunks, context revisions and
program source are retained. Credentials are excluded from the audit interface.
If final text differs from the preview, the UI marks the correction and shows
the authoritative final text. Deltas without item IDs wait for the final item.
Incomplete streamed calls never dispatch. Ctrl-C closes interrupted effects with an
unknown outcome and supplies interruption results for pending calls so later turns
can continue. It never automatically repeats a side effect. Pure Lua loops stop
through the instruction hook too.

## Live Lua-defined tools

`tool_define` (or `arco.define_tool(definition)`) stages a session tool schema and
Lua function body; `tool_registry` inspects effective versus pending definitions.
Successful workflow boundaries publish them. Later default requests expose the
tools and `arco.call` dispatches them through retained operation/native-effect
paths. Invalid definitions or failed workflows preserve effective configuration.
See [LUA_TOOLS](LUA_TOOLS.md) for the supported scalar-object schema, exact bounds,
source/body example, persistence and unknown-effect limits.

## Restart/resume/continue (RRC)

Build `cmake --build build/release --target arco`, then use `/restart NOTE` or let
the model call `restart` with `{note="changes, checks, next steps"}`. Launch through
`./scripts/arco`: it waits for exit code 75, then starts the replacement executable
with the same session. Settings, identities and context survive; the new process
injects `continue` with the note and runs the saved workflow. The original prompt
is not submitted again. `ARCO_EXECUTABLE` selects a different executable when needed.

A model restart request is deferred until the current tool batch and turn finish.
`arco.restarting()` lets custom Lua programs return at that boundary; another provider
request after scheduling restart is refused. Failed/interrupted turns cancel their
restart request. Pending call/result linkage or unfinished admitted effects blocks
restart. There are no current standing programs to pause; consumed services remain
outside this process's lifecycle.

After a launcher failure, reopen with `./scripts/arco --session DIRECTORY
--resume-continue` to consume a pending intent. Ordinary reopen does not run it.
`--resume-once` runs the continuation and exits, useful for controlled exercises.
Injection is deduplicated across the append/consume crash gap. The intent is
consumed before requesting a provider: a subsequent crash or failed request requires
explicit operator continuation rather than an automatic retry. Terminal drafts and history survive RRC; queued prompts restore only as drafts
requiring explicit submission after continuation, never automatic execution.
Native hot replacement and outposts remain separate later capabilities.

### Explicit failed-workflow linkage repair (B4)

A custom Lua error after a call is appended but before its result can leave
context linkage incomplete. The next provider request refuses that presentation;
ordinary failure does not automatically repair it or replay effects. Interruption
and capacity stops already supply their native stop placeholders.

After inspecting the retained failed program, operation results and unknowns, use
an explicit recovery workflow:

```text
/lua local r=arco.call('context_repair',{base=arco.context().base}); print(arco.json.encode(r))
/workflow programs/turn.lua
<your next useful request, with instructions not to replay uncertain effects>
```

`context_repair` requires the current context `base` (strict CAS). It adds one
`workflow_result_missing` placeholder per still-unanswered call that existed at
this workflow's start, with `effect_outcome: unknown` and `replayed: false`.
Even a tool that completed locally can have a missing presentation result; the
placeholder does not replace its retained actual result or establish its outcome.
Originals, existing outputs and source/audit paths remain intact. Repeating repair
on complete context adds nothing. Duplicate calls, orphan outputs, stale base,
new calls introduced by the active workflow and live owned native children are
refused. Repair is audited through ordinary native operation/context records.

This is **presentation repair**, not backstop recovery, effect reconciliation,
process containment or a new attempt authority. General exec may have escaped
children; remote effects may remain unknown. No native lifetime reset, automatic
request, retry, restart or unpause occurs. Explicit cancellation stays paused until
the operator submits work. Reopen still refuses unresolved non-provider admissions;
this tool does not bypass that recovery gate. A new explicit workflow can use
this repair path on retained incomplete context after an ordinary allowed reopen.

On reopening, unfinished provider requests are recorded as unknown without replay
or acceptance of late response data. Context and raw streams remain retained.
Unfinished command/file operations still block ordinary resumption; keep their audit
for diagnosis. Full recovery/custody remains unfinished. Multi-model concurrency, external complaint
storage and replacing the Codex auth scaffold are also subsequent work. Scoped
Mac/Linux persistence and RRC checks pass; broader integrated qualification remains
pending. The earlier vertical-slice demonstration
is mechanism evidence; readiness for daily self-development depends on operator use.

### Explicit managed compaction (B1)

`/compact JSON` accepts `{base,mode,ids,reason,source}`; mode is `archive`, `select`,
`summarize` or `restore`. Summary also requires `summary:{role:"assistant",content:...}`.
Use `/context` for entry IDs/revision. Complete call/result intervals are atomic;
all live user/system/developer instructions are mandatory. Restore merges exact
originals in capture order and repairs selected edited/reordered presentations.
Archive only changes presentation. Summaries have fresh IDs and source ancestry;
structural acceptance cannot establish fact accuracy. No automatic thresholds.

Lua uses `arco.manage(proposal)` and `arco.inspect(query)`. Model tools are
`context_manage {proposal}` and `context_inspect {query}`. A supplied base is strict
CAS. Model tool proposals may omit base to explicitly bind the invocation snapshot
(the concrete resolved proposal is audited), avoiding the provider-call append gap.
Managed requests during a workflow stage until successful completion, retaining the
current tool exchange. Failed/interrupted workflows cancel. One pending request;
o overlapping last-writer-wins. Generic CLM edit/append remain available.

`/inspect {"kind":"index"}` returns bounded hex-encoded serialized UTF-8 JSON.
Kinds: `index`, `history`, `originals`; originals may specify `entry`. Optional
`offset` and `limit` (default4096, max65536) page exact source bytes without dumping
old history into a request. Decode hex locally; ranges may split UTF-8 codepoints.
Audit/originals grow even when presentation shrinks. No automatic effect replay.

A tool deadline returns `timed_out:true`, `effect_outcome:"unknown"` and an
`output_ref` for retained partial output. The workflow continues so the model can
inspect that output and choose a new attempt with a larger timeout, split the work,
or take another approach. Arco closes/reaps the local child group before returning;
already-performed external effects remain possible and are not replayed automatically.
Operator cancellation still stops the turn. Provider transport failures remain
separate from this tool-result continuation.

The Arconaut sprite appears at the upper right in terminals at least 90 columns
wide and 20 rows tall. He gently bobs and his plume shimmers during a turn, then
settles when idle. Smaller windows use the full width for the conversation.
The sprite uses Unicode half blocks and ANSI truecolor; no image protocol is needed.

## Local audit explorer and advisory complaints

Model tools `audit_inspect({query:...})` and `rageshake({...})` are also available
from Lua as `arco.call("audit_inspect", {query=...})` and
`arco.call("rageshake", {...})`. No command approvals or immediate repair obligation.

Start with `{cursor:0,count:32}`; retain returned `end` for later pages, advance
`cursor` to returned `next`. Each row names its committed fact index, physical
journal/sequence, causal IDs and up to64 source dependency references. Kind numbers
are RetainedKind in include/arconaut/retained_events.hpp. Decision → invocation →
attempt IDs link operation input/context/program to observed disposition; admission
is not success. Application channel1 captures original labels/metadata, channel2
context revisions, channel3 program activation. Separate occurrences stay separate.

For exact retained bytes use `{record:INDEX,offset:0,limit:4096}` for event payload,
or add `source:0` to read that dependency's immutable original. Returned hex can
split UTF8/binary arbitrarily; concatenate/decode bytes, never rerun the effect.
`total_bytes`/`next` expose EOF. Count1..64; byte limit1..65536. Metadata is small
and selected; context packets are not serialized by index listing. Detail source
reads may copy one existing document (ordinary document bound); there is no global
full-history reconstruction or second output store. Prefix `end` remains usable
while new facts append. Fact indices are local to this journal lineage, not portable
between unrelated sessions. Only committed facts appear; provisional/crash evidence
is not silently asserted to have settled.

Complaint example:

```lua
local result = arco.call("rageshake", {
  observation="Inspection required a repeated full-history scan",
  references={attempt="EXACT_ATTEMPT", record=123, hypothesis="optional suspicion"}
})
return result
```

Observation/references are model assertions, not a correctness verdict. The native
capture adds context revision, effective program generation, model/effort,
actor/conversation/workflow, reporting attempt and audit watermark. `complaint.detail`
is retained before bead delivery; `complaint.delivery` records the result. Beads
receives only constant advisory text and local identity locators, not private raw
provider history or the observation. Delivery failure/timeout remains local; inspect
the actual exec output reference before any explicit new delivery attempt. Never
automatically replay an unknown side effect. An external DB sink is unconfigured;
a separately configured consumer can inspect these records without Arco provisioning
or governing a database. Keep raw audit/context ignored, not in commits or pushes.

### Explicit capacity successor (declared lineage)

`arco --session NEW_DIRECTORY --seed-session SEED.json --plain` initializes a
fresh audit with selected context; `/quit` exits without a provider request.
Do not combine seeding with inspection, discovery or restart-resume switches.
Existing audits are refused, not overwritten. Input is bounded to1MiB/4096 entries.
Example shape (replace IDs/observations with actual source locators):

```json
{"version":1,"source":{"session":"/absolute/source/session",
 "environment":"11000000000000000000000000000000",
 "journal":"22000000000000000000000000000000",
 "prefix_sequence":17,"prefix_end":8192,
 "context_revision":"33000000000000000000000000000000",
 "status":"unsettled","reason":"explicit capacity successor; no settlement established"},
 "entries":[{"id":"33000000000000000000000000000000.0",
 "item":{"role":"developer","content":"Retain current instructions and working state"}}]}
```

The author selects context and is responsible for including governing instructions
and honest source observations/unknowns. Native validation checks manifest shapes,
current text-message/self-contained encrypted-reasoning/tool-pair schemas, complete
ordered call/output pairs and unique original-entry locators. Non-text modalities
are not supported by this seed schema. Seeding does not read or verify the source,
claim settlement, dispatch imported calls, or inherit admissions. New session and
entry identities are fresh. One context packet atomically retains new originals
and declared lineage; its `lineage.source_entries` order maps to new `originals`.
Use bounded context history inspection to recover that packet. Old paths/journals/
entry IDs remain old locators; original bytes have not been copied wholesale or
moved. A failed write can leave allocator reservation facts but cannot publish
selected context without lineage. Inspect such a destination; do not silently retry
seeding into it. Automatic handoff and bounded old-entry resolution without full
parent replay remain separately queued; explicit declared continuation is supported.

### Bounded workflow capacity stop

Each coding turn protects byte/record credits for bounded diagnostics, outstanding
nested attempts (maximum depth16), pending managed-proposal cancellation, and call
linkage. Before context/staging publication, the prospective stop packets must fit
the unchanged physical payload/batch limits as well as remaining journal capacity.
Oversized obligations are refused rather than raising caps. A capacity refusal is
sticky for the turn, including Lua `pcall`: no fresh effects or provider retries.

Provider/process capture refusal stops the live adapter/child, retains earlier
committed fragments, records unknown effect outcome and received-but-unretained byte
counts, cancels pending management, and closes open calls with stop outputs. Bounded
Lua error capture reports omitted bytes. This is not a promise that rejected bytes
were retained, that no dispatched effect occurred, or that physical write/sync
failures can be settled in an unavailable audit. Success outputs remain ordinary
bounded writes; post-dispatch output overflow uses a bounded unknown terminal.
Context compaction does not reclaim physical audit bytes. Act on early headroom
warnings and explicitly select a successor; no automatic physical rollover exists.

### Retained named modules and request configuration

`program_config({})` inspects effective and pending program snapshots/revisions,
resolved request defaults and launch fallbacks, tool registry, context policy, and
the current governing workflow generation (its source is retained as
`program.source`). `program_config({proposal=..., base=REV})` stages a **complete**
snapshot; omitted base binds the current effective revision. Example in Lua:

```lua
local view = arco.call("program_config", {})
local source = arco.call("read_file", {path="programs/maintenance.lua"}).content
local pending = arco.call("program_config", {
  base=view.revision,
  proposal={modules=arco.array({{name="maintenance", source=source}}),
            model="", effort=""}
})
assert(pending.staged)
-- This workflow still sees the old snapshot. Import on a later workflow:
-- local maintenance = arco.module("maintenance")
```

Empty model/effort use current launch defaults; nonempty defaults affect later
requests after publication. Effort accepts low/medium/high/xhigh. Request-local
`arco.request` options still override these defaults. Model strings do **not**
select a different provider/adapter, authorize dependencies, or establish provider
capability; unsupported models fail through the existing adapter.

A candidate contains at most32 uniquely named modules, each at most16KiB source,
and at most64KiB serialized configuration. Text-only syntax validation executes
no candidate top-level code. Publication shares the durable successful-workflow
boundary with context/tool/policy changes; workflow failure, cancellation or failed
settlement preserves the effective snapshot and discards pending changes. Invalid
proposals do not replace a prior valid pending proposal. Complete proposals can
remove modules; retained history is not erased. Reopen restores published sources.

`arco.module(name)` obtains source through the audited `module_source` tool,
evaluates it on first import in that workflow, and caches its non-nil returned
value. Late imports resolve the same effective snapshot, never pending/live-file
bytes. Each module has a private assignment environment inheriting the ordinary
Lua globals; shared tables and `_G` are **not hostile-code isolation**. Cycles
fail; failed evaluation clears its loading marker. Evaluation can perform ordinary
audited effects: an error does not roll them back and an explicit retry can repeat
them. Use declarative top levels and put effects in returned functions.

This slice does not stage a governing workflow selector or implement an atomic
interrupt-and-apply UI. `--workflow`/`/workflow` still select the file read at turn
admission. Cancel stays cancelled; it does not publish pending configuration.
Native code still activates by build/restart/resume, not by module publication.

### Bounded synchronous orchestration (G6)

`programs/orchestration.lua` supplies a reusable named-module source. Publish it
through a complete `program_config` proposal, preserving other desired modules,
then use `arco.module("orchestration")` after the successful boundary. There is no
auto-installed module or inherited participant authority in a fresh session.

`new{owner=...,max_participants=N,max_attempts=M}` creates one workflow-local
synchronous scope. `run(id,callback)`, boolean `branch`, explicit `retry` and selected
`join{ids...}` compose ordinary native tool/colleague calls. IDs are unique per
scope, results/history are copied, and finite positive limits count participants
and callback attempts. Callbacks must bound their own tool/request counts and
native timeouts. A callback reports `status` and `remote_disposition`; these are
caller assertions grounded in actual native results, not containment evidence.
Exceptions/malformed results remain unknown. Only explicit `refused` plus
`not_dispatched` permits retry; there is no automatic retry/reconciliation/replay.

`pause(reason)` prevents subsequent callback dispatch and has no resume operation.
It does not interrupt an active native call or establish remote cancellation;
use the owning runtime's existing native cancellation paths for in-flight work.
Join is a snapshot, not a wait: all selected rows must be completed or it reports
unsettled. No parallel workers, detached children, persistent admissions, native
participant messaging or new provider pairing restrictions are introduced.

See [G6_ORCHESTRATION_MAP](G6_ORCHESTRATION_MAP.md) for API example and actual
context-only colleague usage, and [G6_NATIVE_OWNERSHIP_MAP](G6_NATIVE_OWNERSHIP_MAP.md)
for current stop/cancellation anchors. Direct checks: `lua tests/orchestration_test.lua`
with Lua5.4.8. G5's independent colleague candidate is not automatically integrated
by importing this module; select an actually configured adapter/call path explicitly.

### Operator executable during concurrent development

Interactive `scripts/arco` prefers `build/release/arco-ui` when that published
executable exists. Unattended `--once` / `--resume-once` uses `build/release/arco`;
`ARCO_EXECUTABLE=/absolute/path` overrides either. This prevents a campaign build
from silently changing the interactive generation. Native self-development in an
interactive session must install the checked executable atomically to `arco-ui`
before `/restart`, or explicitly select its executable when launching. A restart
continues the selected path. Startup reports the session before reopening history.
