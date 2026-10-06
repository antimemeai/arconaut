# aider

A specialized editable-file conversation with model edit formats, Git commits/undo, sequential architect/editor models and lint/test reflection. Its compact scope exposes several useful contrasts with a general programmable harness.

Role: interactive repository editing agent. Runtime: Python.

Pinned source: [https://github.com/Aider-AI/aider](https://github.com/Aider-AI/aider); revision/version `5dc9490bb35f9729ef2c95d00a19ccd30c26339c`.

Synchronous CLI/provider stream and bounded reflection loop; separate history-summary thread; fresh shell process per command

Owns selected repository edits/Git integration and immediate commands; consumes provider via model adapter, no shared computational fabric traced

Inspection: Read run/input/provider/edit/result/reflection flow, concrete edit-block writes, shell/PTY dispatch, history thread/summarizer/fallback, model/mode/command script operations, diagnostics and benchmark test oracle.

Limits of this study: No acquired code executed. Repo-map internals, all edit formats, GUI/watch paths, provider dependency transport and full benchmark evaluator not exhaustively inspected.

## Actions

### model edit blocks

Surface: model response format.

Input: selected filename plus SEARCH/REPLACE code block

Result: written files, partial edit failure/reflection or commit bookkeeping

Lifecycle: dry-run/prepare before writes; multiple edits are not a transaction

Authority: model edits selected files under coder policy

Evidence: [edit](#evidence-edit), [apply](#evidence-apply), [effects](#evidence-effects).

### suggested shell script

Surface: model response format.

Input: model shell blocks

Result: merged stdout/stderr and command exit result shared into chat

Lifecycle: explicit-required yes, dedup within batch, fresh sequential shell; no live task handle

Authority: model suggestion requires operator approval; --yes declines this prompt

Evidence: [shell-authority](#evidence-shell-authority), [yes](#evidence-yes), [shell](#evidence-shell), [pty](#evidence-pty).

### /run /test

Surface: operator commands/programmatic hook.

Input: command or supplied callable test hook

Result: streamed output; nonzero error report for reflection

Lifecycle: synchronously run then optionally append output; no process custody API

Authority: operator/embedding author

Evidence: [commands](#evidence-commands), [shell](#evidence-shell).

### architect/editor handoff

Surface: agent mode.

Input: architect text and configured editor model/edit format

Result: editor writes/commits and merged cost/commit IDs

Lifecycle: sequential editor run with fresh conversation; no live peer mailbox

Authority: selected two-model workflow with optional auto_accept_architect

Evidence: [architect](#evidence-architect).

### /model /editor-model /weak-model

Surface: operator commands.

Input: provider model name

Result: SwitchCoder/new model config

Lifecycle: operator input boundary; loaded command script skips transition

Authority: operator

Evidence: [models](#evidence-models), [scripts](#evidence-scripts).

### history summary/restore

Surface: host context lifecycle.

Input: completed messages, token budget and selected models/Markdown log

Result: replacement summary+tail or warning; originals absent from active list

Lifecycle: separate thread snapshot/join; failed result can still adopt empty output

Authority: host automatic summarizer; no model repair transaction

Evidence: [summary-init](#evidence-summary-init), [summary-thread](#evidence-summary-thread), [summary](#evidence-summary).

### /load /save

Surface: operator commands.

Input: command file path or output file

Result: sequential command outcomes or reconstructed selected file list

Lifecycle: load catches/skips SwitchCoder; saved file selection not runtime snapshot

Authority: operator

Evidence: [scripts](#evidence-scripts).

### benchmark task tests

Surface: research evaluation API.

Input: candidate workspace and original test paths/language

Result: runner exit-derived outcome and test stdout

Lifecycle: original test recopy, 180-second subprocess bound, sequential task attempts

Authority: researcher benchmark program; not live model harness promotion

Evidence: [benchmark](#evidence-benchmark).

## Capabilities

### filesystem

**I — Files** (source): Selected repository files are read and model edit blocks dry-run/prepare/apply into actual writes; partial successful edits can precede later failure.

Evidence: [edit](#evidence-edit), [apply](#evidence-apply).

### processes

**L — OS programs** (source): Model shell suggestions require explicit approval; operator run/test supported. Fresh piped/interactive commands stream merged output, no reusable task handle/timeout/process-tree settlement traced.

Evidence: [shell](#evidence-shell), [pty](#evidence-pty), [shell-authority](#evidence-shell-authority), [commands](#evidence-commands).

### code-actions

**S — Code actions** (source): Model-generated shell scripts can run sequentially after explicit yes, with output added to chat. No general nested-tool code-action kernel.

Evidence: [edit](#evidence-edit), [shell-authority](#evidence-shell-authority), [shell](#evidence-shell).

### persistent-kernel

**? — Kernel** (inspection scope): persistent_kernel: not established in the inspected synchronous CLI/edit/history/shell surfaces; no universal absence implied.

### standing-database

**? — Standing DB** (inspection scope): standing_database: not established in the inspected synchronous CLI/edit/history/shell surfaces; no universal absence implied.

### workflow-programming

**S — Workflows** (source): Programmatic Coder run and operator command files compose bounded reflection/edit/test stages. Loaded scripts skip model/mode SwitchCoder rather than redefine active turn.

Evidence: [run](#evidence-run), [effects](#evidence-effects), [commands](#evidence-commands), [scripts](#evidence-scripts).

### multi-model

**I — Models** (source): Main/weak/editor roles and sequential architect→editor run are implemented; selecting models is not concurrent dialogue.

Evidence: [models](#evidence-models), [architect](#evidence-architect), [summary](#evidence-summary).

### live-collaboration

**L — Peer chat** (source): Architect/editor exchange is a sequential fresh editor run, not independently working addressed participants; no busy-target colleague messaging established.

Evidence: [architect](#evidence-architect).

### concurrent-work

**L — Concurrency** (source): History summarization runs in a separate thread; main joins before adopting result. Interactive/tool work is synchronous and no model-exposed ongoing process/peer handle traced.

Evidence: [summary-thread](#evidence-summary-thread), [run](#evidence-run), [shell](#evidence-shell).

### steering-interrupt

**L — Steer/interrupt** (source): KeyboardInterrupt ends a provider reply and records a synthetic interruption message; synchronous input loop waits for run_one, no busy guidance mailbox. Shell path has no explicit descendant stop/join guarantee.

Evidence: [request](#evidence-request), [effects](#evidence-effects), [run](#evidence-run), [shell](#evidence-shell).

### turn-redefinition

**S — Turn program** (source): Embedding author can supply Coder classes/summarizer/test callables; operator switches compiled Python coder/model modes. No model hot replacement of the governing loop established.

Evidence: [run](#evidence-run), [commands](#evidence-commands), [models](#evidence-models).

### compaction

**L — Compaction** (source): Background summary replaces done_messages after snapshot equality. Head can be truncated before summary. Source-inferred all-model failure causes caught ValueError with empty summary subsequently adopted; no successful outcome guard.

Evidence: [summary-init](#evidence-summary-init), [summary-thread](#evidence-summary-thread), [summary](#evidence-summary), [summary-test](#evidence-summary-test).

### context-repair

**S — Repair** (source): Optional Markdown chat restore and current repository context allow reacquisition, but summary replacement loses original working messages and source transformation lineage not retained there.

Evidence: [summary-init](#evidence-summary-init), [summary-thread](#evidence-summary-thread), [scripts](#evidence-scripts).

### original-audit

**L — Original audit** (source): Formatted optional request/assistant diagnostics and Markdown history/Git edits are useful, but write failure disables logging and original provider envelopes/attempts/transformation IO are not captured comprehensively.

Evidence: [audit](#evidence-audit), [audit-write](#evidence-audit-write), [summary-thread](#evidence-summary-thread).

### audit-query

**S — Audit query** (source): Input/chat history can be restored/read as files; selected file list scripts do not reconstruct original provider/program audit. No dedicated model audit query surfaced.

Evidence: [summary-init](#evidence-summary-init), [scripts](#evidence-scripts), [audit-write](#evidence-audit-write).

### hot-change

**L — Hot change** (source): Operator changes model/editor/weak model through SwitchCoder; sequential loaded command scripts explicitly skip SwitchCoder. No active-stack code reload or affected-workflow pending generation traced.

Evidence: [models](#evidence-models), [scripts](#evidence-scripts).

### rebuild-continuity

**L — Rebuild continuity** (source): Restore selected Markdown chat/files and Git bookkeeping, not executable compile handoff or in-flight program/provider continuity.

Evidence: [summary-init](#evidence-summary-init), [scripts](#evidence-scripts), [architect](#evidence-architect).

### remote-services

**? — Remote** (inspection scope): remote_services: not established in the inspected synchronous CLI/edit/history/shell surfaces; no universal absence implied.

### self-improvement

**S — Self-improve** (source): Bounded lint/test reflection is real code repair driven by tool results, not harness autoresearch or an independent promotion experiment.

Evidence: [run](#evidence-run), [effects](#evidence-effects), [benchmark](#evidence-benchmark).

### complaints

**? — Complaints** (inspection scope): complaints: not established in the inspected synchronous CLI/edit/history/shell surfaces; no universal absence implied.

### authority

**L — Authority** (source): Automatic yes still declines explicit-required model shell prompts. Standing selected-file/read-only policy and edits exist, but this is a concrete friction limit for Arconaut operator intent.

Evidence: [yes](#evidence-yes), [shell-authority](#evidence-shell-authority), [edit](#evidence-edit), [effects](#evidence-effects).

### evaluation

**I — Evaluation** (source): Lint/test return errors feed bounded reflection; benchmark restores original test files and checks runner exit. Tests inspect literal summary fallback/output; no complete scientific correctness or harness-quality oracle implied.

Evidence: [effects](#evidence-effects), [commands](#evidence-commands), [benchmark](#evidence-benchmark), [summary-test](#evidence-summary-test), [shell-test](#evidence-shell-test).

### time-order

**L — Time/order** (source): Synchronous request/edit stages have explicit order; retry uses sleep, interrupt debounce and diagnostic timestamps use wall clock; benchmark has subprocess timeout, shell path lacks one.

Evidence: [request](#evidence-request), [run](#evidence-run), [audit-write](#evidence-audit-write), [benchmark](#evidence-benchmark), [shell](#evidence-shell).

## Inspected test oracles

- [quarantine/aider/tests/basic/test_history.py](../../../quarantine/aider/tests/basic/test_history.py): literal summary shaping and provider fallback Oracle: Mock provider asserts exact summary prefix and second-model fallback; tests do not invoke Coder background worker on all-model failure. Read, **not executed**.
- [quarantine/aider/tests/basic/test_run_cmd.py](../../../quarantine/aider/tests/basic/test_run_cmd.py): basic command output and exit Oracle: Actual echo must return exact Hello, World! and zero; no cancellation/cleanup oracle. Read, **not executed**.
- [quarantine/aider/tests/basic/test_coder.py](../../../quarantine/aider/tests/basic/test_coder.py): file selection/read-only and ambiguous names Oracle: Inspected literal selected-file sets and empty read-only selection; repository listing mocked, not concurrent shared-writer proof. Read, **not executed**.

## Useful mechanisms

- Compact edit-format→apply→lint/test reflection flow.
- Architect/editor roles and bounded reflections are concrete separate policies.
- Benchmark test source restoration improves independence from candidate edits.

## Material limits

- Suggested shell command --yes declines explicit-required prompts.
- Source-inferred summary failure can discard completed history; not executed.
- Formatted optional logs, synchronous commands and chat restoration do not establish audit/refit continuity.

## Arconaut design questions

- Keep general program actions as operations, not ambiguous prose requiring shell approval.
- Inject all-summary-provider failure at worker publication and require original history remain selected.
- Use independent immutable evaluator material and explicit promotion outcome for autodroit, beyond bounded repair reflections.

## Evidence

### Evidence run

[quarantine/aider/aider/coders/base_coder.py:859–943](../../../quarantine/aider/aider/coders/base_coder.py#L859): Interactive/single-message run initializes edit/result state and repeats reflected errors to bounded max_reflections.

### Evidence request

[quarantine/aider/aider/coders/base_coder.py:1419–1534](../../../quarantine/aider/aider/coders/base_coder.py#L1419): Assembles selected request, retries transient provider exceptions with sleep, handles interrupt and partial output; no live input mailbox in this path.

### Evidence effects

[quarantine/aider/aider/coders/base_coder.py:1560–1623](../../../quarantine/aider/aider/coders/base_coder.py#L1560): Applies edits, commits, moves completed conversation, runs lint/shell/test and asks before error reflection.

### Evidence apply

[quarantine/aider/aider/coders/base_coder.py:2296–2336](../../../quarantine/aider/aider/coders/base_coder.py#L2296): Edit pipeline dry-run/prepare/apply; malformed/general exceptions report reflected messages, not atomic edit transaction.

### Evidence edit

[quarantine/aider/aider/coders/editblock_coder.py:21–79](../../../quarantine/aider/aider/coders/editblock_coder.py#L21): Model blocks separate file replacements and shell suggestions; searches other selected files on failed match, writes successful edits before reporting remaining failures.

### Evidence shell

[quarantine/aider/aider/run_cmd.py:42–86](../../../quarantine/aider/aider/run_cmd.py#L42): Fresh subprocess merges stderr/stdout, streams one character at a time then waits; no timeout or process-group cleanup in inspected path.

### Evidence pty

[quarantine/aider/aider/run_cmd.py:89–132](../../../quarantine/aider/aider/run_cmd.py#L89): TTY path transfers user interaction to pexpect, captures output, closes then returns exit status; no standing process handle.

### Evidence shell-authority

[quarantine/aider/aider/coders/base_coder.py:2434–2475](../../../quarantine/aider/aider/coders/base_coder.py#L2434): Suggested shell command batches require explicit_yes_required and sequential run_cmd, dedup within batch.

### Evidence yes

[quarantine/aider/aider/io.py:866–869](../../../quarantine/aider/aider/io.py#L866): yes=True chooses n for explicit_yes_required prompts; automatic yes does not grant suggested command execution.

### Evidence architect

[quarantine/aider/aider/coders/architect_coder.py:11–48](../../../quarantine/aider/aider/coders/architect_coder.py#L11): Architect output becomes a separate selected editor-model run with cleared chat, then bookkeeping merges; sequential not live peers.

### Evidence models

[quarantine/aider/aider/commands.py:87–136](../../../quarantine/aider/aider/commands.py#L87): Operator main/editor/weak model changes raise SwitchCoder after constructing model candidate.

### Evidence summary-init

[quarantine/aider/aider/coders/base_coder.py:510–523](../../../quarantine/aider/aider/coders/base_coder.py#L510): Summary worker output starts empty; restore parses selected Markdown history and starts summary.

### Evidence summary-thread

[quarantine/aider/aider/coders/base_coder.py:1002–1038](../../../quarantine/aider/aider/coders/base_coder.py#L1002): Worker catches ValueError without publishing a replacement; join assigns summarized_done_messages if source snapshot still matches. Empty default can replace original history on failure.

### Evidence summary

[quarantine/aider/aider/history.py:33–123](../../../quarantine/aider/aider/history.py#L33): Summary keeps bounded tail and truncates head to model input budget, can recursively summarize; only user/assistant roles supplied to model; all models failing raises ValueError.

### Evidence audit

[quarantine/aider/aider/coders/base_coder.py:1783–1834](../../../quarantine/aider/aider/coders/base_coder.py#L1783): Optional request diagnostic is formatted messages; finally records reconstructed assistant content/function prose, not original provider wire/deltas.

### Evidence audit-write

[quarantine/aider/aider/io.py:754–765](../../../quarantine/aider/aider/io.py#L754): Optional appended formatted LLM log uses wall timestamp; write failure warns and disables future logging.

### Evidence commands

[quarantine/aider/aider/commands.py:993–1053](../../../quarantine/aider/aider/commands.py#L993): Operator run/test executes command; output enters chat by explicit choice or nonzero test exit; callable test hook supported.

### Evidence scripts

[quarantine/aider/aider/commands.py:1465–1522](../../../quarantine/aider/aider/commands.py#L1465): Loads sequential slash commands, skipping SwitchCoder changes; save reconstructs selected file list only.

### Evidence benchmark

[quarantine/aider/benchmark/benchmark.py:981–1049](../../../quarantine/aider/benchmark/benchmark.py#L981): Benchmark recopies original tests, invokes language runner with timeout and checks actual return code, appends test output; only task test suite oracle.

### Evidence summary-test

[quarantine/aider/tests/basic/test_history.py:44–120](../../../quarantine/aider/tests/basic/test_history.py#L44): Mock summary tests assert literal prefix/result and second-model fallback; no background worker all-model failure/history retention oracle.

### Evidence shell-test

[quarantine/aider/tests/basic/test_run_cmd.py:6–11](../../../quarantine/aider/tests/basic/test_run_cmd.py#L6): Echo test asserts actual exit zero and literal stdout; not cancellation or descendant cleanup.

