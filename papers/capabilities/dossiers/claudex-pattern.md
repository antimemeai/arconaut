# claudex-pattern

Small local historical runner exposes commander-controlled briefs, per-agent options/workdirs, captured headless outputs and tmux watch/say; it is not a general collaboration engine.

Role: unversioned local external-Claude platoon runner. Runtime: Python (stdlib), generated zsh.

Pinned source: Origin URL unrecorded; local material identified by the registry catalog; revision/version `unversioned`.

CLI invokes external Claude sequentially headless or launches independent tmux/macOS terminal sessions.

Owns brief/roster/launch artifacts and terminal channel; child harness owns provider, tools and context.

Inspection: Selected source excerpts trace exposed entry/loop/request/action/result/state boundaries and relevant capability mechanisms; listed tests are read as source only.

Limits of this study: No acquired code executed, dependencies installed or live providers invoked. Claims concern this pinned snapshot; untraced catalog tools and dependency internals are not inferred.

## Actions

### list / render <brief> <callsign>

Surface: operator/model CLI.

Input: config path, roster/brief IDs

Result: roster or rendered index+assignment prompt

Lifecycle: read-only invocation; full config reread

Authority: operator/commander owns TOML; prompt constraints no filesystem enforcement

Evidence: [e2](#evidence-e2), [e8](#evidence-e8).

### dispatch <brief> [callsigns|all] [--dry-run] [--live] [--stream-json] [--max-budget-usd]

Surface: operator/model CLI.

Input: config, brief, callsigns, optional run dir and dispatch options

Result: prompt/stdout/stderr/meta and child return code

Lifecycle: headless sequential blocking; live tee threads have 2s join; no timeout/cancel API

Authority: config permission mode/auth inherited to external Claude

Evidence: [e3](#evidence-e3), [e5](#evidence-e5), [e8](#evidence-e8).

### dispatch --windows

Surface: operator/model CLI.

Input: configured terminal/workdir/roster/brief

Result: runner script and launcher metadata, independent terminal sessions

Lifecycle: same rt-CALLSIGN tmux killed before new; launcher success does not prove worker finished

Authority: operator/model shell command; generated child permission flags

Evidence: [e4](#evidence-e4), [e8](#evidence-e8).

### status [brief] --json

Surface: operator/model CLI.

Input: config and run limit

Result: run/callsign/rc/dry/output-size rows

Lifecycle: queries files for current roster, not live child health

Authority: local metadata read; no remote provider query

Evidence: [e6](#evidence-e6).

### channel list / watch CALLSIGN / say CALLSIGN MESSAGE

Surface: operator/model CLI.

Input: action, callsign, optional lines or literal message

Result: tmux sessions/screen/text-submit rc

Lifecycle: direct terminal injection without busy/idle fence; ignored literal-text error can yield misleading success

Authority: bypasses config lookup and addresses predictable rt-CALLSIGN

Evidence: [e7](#evidence-e7), [e8](#evidence-e8).

## Capabilities

### filesystem

**S — Files** (source): Writes run artifacts/scripts and assigns child workdirs; underlying Claude tools supply actual coding operations.

Evidence: [e3](#evidence-e3), [e4](#evidence-e4), [e5](#evidence-e5).

### processes

**I — OS programs** (source): Headless external child and independent terminal/tmux workers; replacing windows kills existing callsign session.

Evidence: [e4](#evidence-e4), [e5](#evidence-e5).

### code-actions

**S — Code actions** (source): Config and CLI can be composed by commander; generated shell launcher, not a native model-authored tool-program execution runtime.

Evidence: [e2](#evidence-e2), [e4](#evidence-e4), [e8](#evidence-e8).

### persistent-kernel

**— — Kernel** (inspection-limit): This unversioned Python control script launches external Claude rather than providing a persistent language interpreter.

### standing-database

**— — Standing DB** (inspection-limit): Inspected whole five-file implementation uses prompt/meta/output files, not a standing query database.

### workflow-programming

**S — Workflows** (source): Roster, doctrine, briefs and CLI are editable by commander; no dependency scheduler or workflow resume interpreter. Headless all is sequential.

Evidence: [e2](#evidence-e2), [e8](#evidence-e8), [e9](#evidence-e9).

### multi-model

**L — Models** (source): Per-worker --model and effort configure different Claude CLI invocations; no heterogeneous provider adapter or intra-turn collaboration engine.

Evidence: [e5](#evidence-e5), [e9](#evidence-e9).

### live-collaboration

**S — Peer chat** (source): Commander can watch/say to window workers via terminal; no mailbox, shared peer room or model-consumption acknowledgment.

Evidence: [e4](#evidence-e4), [e7](#evidence-e7).

### concurrent-work

**L — Concurrency** (source): Windowed launches coexist; headless list comprehension waits each worker before next. Tee reader threads only stream one process.

Evidence: [e3](#evidence-e3), [e4](#evidence-e4), [e8](#evidence-e8).

### steering-interrupt

**L — Steer/interrupt** (source): Channel say sends literal text plus Enter without idle test or queue; replacing rt-CALLSIGN kills it. No owned cancel/interrupt lifecycle.

Evidence: [e4](#evidence-e4), [e7](#evidence-e7).

### turn-redefinition

**S — Turn program** (source): Brief/system/model/options tune external invocation; inner model/provider turn unchanged and unavailable here.

Evidence: [e2](#evidence-e2), [e5](#evidence-e5).

### compaction

**— — Compaction** (inspection-limit): No context-manager/compactor in inspected complete runner; external Claude responsibility.

### context-repair

**? — Repair** (inspection-limit): Prompt/stdout artifacts exist; runner has no traced restore/projection repair surface.

### original-audit

**L — Original audit** (source): Prompt/stdout/stderr/meta optionally includes Claude JSON stream. Window metadata is launch only, screen not archived; run IDs second-resolution and files overwritten on reused run dir; not full audit.

Evidence: [e3](#evidence-e3), [e4](#evidence-e4), [e5](#evidence-e5).

### audit-query

**L — Audit query** (source): Status projects current-roster file metadata/output size, watch reads live pane; no complete event/search query.

Evidence: [e6](#evidence-e6), [e7](#evidence-e7).

### hot-change

**L — Hot change** (source): Config reread each CLI command; running child keeps passed options. Channel bypasses config; no affected-turn/workflow activation fence.

Evidence: [e5](#evidence-e5), [e8](#evidence-e8).

### rebuild-continuity

**— — Rebuild continuity** (inspection-limit): No compiled harness swap, outpost/context transfer or refit lifecycle in inspected full runner.

### remote-services

**S — Remote** (source): Consumes external Claude CLI/provider using inherited auth; no remote agent/fabric transport built into script.

Evidence: [e5](#evidence-e5).

### self-improvement

**? — Self-improve** (inspection-limit): Commanding model is instructed to edit roster and doctrine; no governed experiment/evaluation/improvement loop established.

### complaints

**? — Complaints** (inspection-limit): Brief artifacts/stdout can report blockers; no state-capturing tracked complaint operation established.

### authority

**L — Authority** (source): External Claude permission-mode option configurable (example bypassPermissions); brief limits are prompt instructions. Channel can target sessions without consulting roster.

Evidence: [e2](#evidence-e2), [e5](#evidence-e5), [e7](#evidence-e7), [e9](#evidence-e9).

### evaluation

**? — Evaluation** (inspection-limit): Entire available implementation and example files read; no test files present in acquired five-source-file tree. Dry-run writes/prints invocation, not behavioral oracle.

### time-order

**L — Time/order** (source): Second-resolution run IDs and start/complete timestamps; no sequence/causality IDs, cancellation fence or child resume.

Evidence: [e3](#evidence-e3), [e5](#evidence-e5).

## Inspected test oracles

No relevant populated test source was recorded in this inspection; capability claims remain source/document traces.


## Useful mechanisms

- Compact editable commander control surface.
- Per-worker workdir/model/permission config with prompts and return code preserved.

## Material limits

- Local unversioned snapshot, no canonical revision or test suite established.
- Headless dispatch is sequential; window launch metadata is not completion.
- Direct terminal channel lacks turn-boundary queue and can report Enter success after literal send failed.

## Arconaut design questions

- What owned small process API would preserve this directness while making handles/cancel/output/receipt semantics precise?
- How should commander programs edit live orchestration without killing existing callsign processes or overwriting run evidence?

## Evidence

### Evidence e1

[quarantine/claudex-pattern/README.md:95–167](../../../quarantine/claudex-pattern/README.md#L95): Documented CLI/terminal coordination and per-run artifacts; external Claude supplies agent engine.

### Evidence e2

[quarantine/claudex-pattern/claudex.py:38–136](../../../quarantine/claudex-pattern/claudex.py#L38): Config validation, callsign roster/brief selection and rendered prompt; instructions are prompt constraints, not enforcement.

### Evidence e3

[quarantine/claudex-pattern/claudex.py:139–173](../../../quarantine/claudex-pattern/claudex.py#L139): Second-resolution run directory IDs, streaming reader threads and per-agent workdir; no execution timeout/cancel handle.

### Evidence e4

[quarantine/claudex-pattern/claudex.py:176–235](../../../quarantine/claudex-pattern/claudex.py#L176): Window launch generates executable shell runner, kills same-named tmux session, starts new and opens terminal; metadata tracks launcher rc, not worker completion.

### Evidence e5

[quarantine/claudex-pattern/claudex.py:238–299](../../../quarantine/claudex-pattern/claudex.py#L238): Headless dispatch calls Claude -p with model/effort/permissions/optional JSON stream/budget and inherited auth; captures stdout/stderr/meta.

### Evidence e6

[quarantine/claudex-pattern/claudex.py:302–339](../../../quarantine/claudex-pattern/claudex.py#L302): Status reads current-roster metadata and stdout sizes; invalid JSON recorded but removed callsigns excluded.

### Evidence e7

[quarantine/claudex-pattern/claudex.py:342–366](../../../quarantine/claudex-pattern/claudex.py#L342): Channel lists all tmux sessions, watches captured pane and injects literal text then Enter. Text-send error ignored; only Enter rc returned.

### Evidence e8

[quarantine/claudex-pattern/claudex.py:369–415](../../../quarantine/claudex-pattern/claudex.py#L369): Exposed CLI; dispatch list comprehension blocks each headless worker sequentially; windows launch independent sessions; reload is per CLI invocation.

### Evidence e9

[quarantine/claudex-pattern/claudex.toml.example:8–90](../../../quarantine/claudex-pattern/claudex.toml.example#L8): Editable roster/model/workdir/doctrine/permissions/terminal; example has commented agents, so lacks required agents key until configured.

