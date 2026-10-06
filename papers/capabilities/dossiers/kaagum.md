# kaagum

Read/search ACP agent; configured tools exclude writes and arbitrary command execution.

Role: coding agent. Runtime: Guile Scheme.

Pinned source: [https://git.systemreboot.net/kaagum](https://git.systemreboot.net/kaagum); revision/version `3d286e6896a64d2857b947500a485a36393724c2`.

One Guile ACP process; synchronous HTTP and serial Linux-container child calls in a TEA reducer/effects loop.

Owns in-memory sessions/transient tool containers; consumes remote OpenAI-compatible/forge/Kagi services.

Inspection: CLI→ACP reducer→HTTP→validated tools→container output→continuation; config/permission/cancel and sole type-test.

Limits of this study: Source read only, reference not executed. HTTP/tool effects block input; stderr not captured. No exposed kernel/workflows or persisted session restoration established.

## Actions

### read

Surface: model tool.

Input: path; start-line/end-line

Result: Raw text/errors

Lifecycle: Synchronous container/call ID

Authority: ACP permission; read-only cwd

Evidence: [e3](#evidence-e3), [e4](#evidence-e4), [e5](#evidence-e5).

### list

Surface: model tool.

Input: root; POSIX pattern

Result: type/size/path rows

Lifecycle: Synchronous container

Authority: Named-tool permission

Evidence: [e3](#evidence-e3), [e4](#evidence-e4).

### search

Surface: model tool.

Input: pattern, files-root, files-pattern

Result: file:line:match

Lifecycle: Synchronous container

Authority: Named-tool permission

Evidence: [e3](#evidence-e3), [e4](#evidence-e4).

### github-issue

Surface: model tool.

Input: host, owner, repo, number

Result: Issue discussion

Lifecycle: Synchronous network-enabled container

Authority: ACP permission

Evidence: [e10](#evidence-e10).

### kagi-search; kagi-extract

Surface: model tool.

Input: query/lens/date bounds; URL

Result: Search results/Markdown/errors

Lifecycle: Synchronous optional network tools

Authority: Operator supplies key command

Evidence: [e1](#evidence-e1), [e11](#evidence-e11).

### /cwd; /tools

Surface: operator command.

Input: No useful args

Result: cwd/tool-permission table

Lifecycle: Immediate end turn

Authority: ACP client

Evidence: [e13](#evidence-e13).

### initialize; session/new; session/prompt; session/set_config_option; session/cancel

Surface: programmable API.

Input: JSON-RPC, session ID/cwd/prompt/model

Result: Session/config/update/stopReason

Lifecycle: In-memory state; cancel waits for input boundary

Authority: ACP client

Evidence: [e6](#evidence-e6), [e2](#evidence-e2).

## Capabilities

### filesystem

**L — Files** (source): Read/list/search only; write explicitly TODO.

Evidence: [e3](#evidence-e3).

### processes

**S — OS programs** (source): Harness-owned tool thunks in containers; no general shell/PTY/ongoing process tool.

Evidence: [e4](#evidence-e4), [e5](#evidence-e5).

### code-actions

**? — Code actions** (source-inspection): Not established in the inspected entry, model loop, action dispatch and state paths; no universal absence claim.

### persistent-kernel

**? — Kernel** (source-inspection): Not established in the inspected entry, model loop, action dispatch and state paths; no universal absence claim.

### standing-database

**? — Standing DB** (source-inspection): Not established in the inspected entry, model loop, action dispatch and state paths; no universal absence claim.

### workflow-programming

**? — Workflows** (source-inspection): Not established in the inspected entry, model loop, action dispatch and state paths; no universal absence claim.

### multi-model

**I — Models** (source): Model discovery/per-session model switch; single active request, not colleagues.

Evidence: [e6](#evidence-e6), [e9](#evidence-e9).

### live-collaboration

**? — Peer chat** (source-inspection): Not established in the inspected entry, model loop, action dispatch and state paths; no universal absence claim.

### concurrent-work

**L — Concurrency** (source): Logical sessions/pending calls exist; execution is serial/blocking.

Evidence: [e2](#evidence-e2), [e6](#evidence-e6).

### steering-interrupt

**L — Steer/interrupt** (source): Cancel clears pending state; input cannot arrive during a blocking HTTP/container effect.

Evidence: [e2](#evidence-e2), [e6](#evidence-e6).

### turn-redefinition

**? — Turn program** (source-inspection): Not established in the inspected entry, model loop, action dispatch and state paths; no universal absence claim.

### compaction

**? — Compaction** (source-inspection): Not established in the inspected entry, model loop, action dispatch and state paths; no universal absence claim.

### context-repair

**? — Repair** (source-inspection): Not established in the inspected entry, model loop, action dispatch and state paths; no universal absence claim.

### original-audit

**L — Original audit** (source): Optional fsynced ACP trace; no comprehensive provider/program stderr/transformation corpus.

Evidence: [e2](#evidence-e2), [e5](#evidence-e5), [e8](#evidence-e8).

### audit-query

**? — Audit query** (source-inspection): Not established in the inspected entry, model loop, action dispatch and state paths; no universal absence claim.

### hot-change

**L — Hot change** (source): Session model config changes subsequent requests; startup tool set has no exposed executable reload.

Evidence: [e1](#evidence-e1), [e6](#evidence-e6).

### rebuild-continuity

**? — Rebuild continuity** (source-inspection): Not established in the inspected entry, model loop, action dispatch and state paths; no universal absence claim.

### remote-services

**I — Remote** (source): Remote provider/forge/Kagi consumption, no remote service lifecycle.

Evidence: [e9](#evidence-e9), [e10](#evidence-e10), [e11](#evidence-e11).

### self-improvement

**? — Self-improve** (source-inspection): Not established in the inspected entry, model loop, action dispatch and state paths; no universal absence claim.

### complaints

**? — Complaints** (source-inspection): Not established in the inspected entry, model loop, action dispatch and state paths; no universal absence claim.

### authority

**I — Authority** (source): Tools default pending approval; allow/reject always affects later named calls; per-tool mounts/namespaces.

Evidence: [e4](#evidence-e4), [e6](#evidence-e6).

### evaluation

**L — Evaluation** (source): One invalid argument-type oracle; no inspected cancel/container/concurrency oracle.

Evidence: [e12](#evidence-e12).

### time-order

**? — Time/order** (source-inspection): Not established in the inspected entry, model loop, action dispatch and state paths; no universal absence claim.

## Inspected test oracles

- [quarantine/kaagum/tests/tools.scm](../../../quarantine/kaagum/tests/tools.scm): Tool argument type mismatch Oracle: Exact synthetic invalid-type error; no container/transport/permission execution. Read, **not executed**.

## Useful mechanisms

- Small explicit state/effects separation.
- Draining stdout before wait prevents saturated-pipe deadlock.

## Material limits

- HTTP/tool effects block input; stderr not captured. No exposed kernel/workflows or persisted session restoration established.

## Arconaut design questions

- Can reducer/effect separation support independent scheduling and steering while effects run?
- How should tool standing authority and mount scope be configured without per-call ceremony?

## Evidence

### Evidence e1

[quarantine/kaagum/bin/kaagum:103–140](../../../quarantine/kaagum/bin/kaagum#L103): Constructs base/forge/optional Kagi tool set and trace port.

### Evidence e2

[quarantine/kaagum/kaagum/tea.scm:964–1032](../../../quarantine/kaagum/kaagum/tea.scm#L964): Effects executed serially before further ACP input; blocking HTTP/tools.

### Evidence e3

[quarantine/kaagum/kaagum/tools/base.scm:89–231](../../../quarantine/kaagum/kaagum/tools/base.scm#L89): Read/list/search and explicit write TODO.

### Evidence e4

[quarantine/kaagum/kaagum/tools.scm:179–305](../../../quarantine/kaagum/kaagum/tools.scm#L179): Typed arguments, per-tool approvals and call/result pairing.

### Evidence e5

[quarantine/kaagum/kaagum/container.scm:33–55](../../../quarantine/kaagum/kaagum/container.scm#L33): Transient container captures stdout, drains then waits; stderr TODO.

### Evidence e6

[quarantine/kaagum/kaagum/tea.scm:376–528](../../../quarantine/kaagum/kaagum/tea.scm#L376): ACP initialize/new/prompt/model configuration/cancel; cancel clears state.

### Evidence e7

[quarantine/kaagum/kaagum/tea.scm:202–237](../../../quarantine/kaagum/kaagum/tea.scm#L202): Pending-call barrier and removal of nonprotocol/reasoning fields.

### Evidence e8

[quarantine/kaagum/kaagum/trace.scm:23–36](../../../quarantine/kaagum/kaagum/trace.scm#L23): Optional textual trace fsyncs each line.

### Evidence e9

[quarantine/kaagum/kaagum/openai.scm:33–72](../../../quarantine/kaagum/kaagum/openai.scm#L33): OpenAI-compatible completion/model discovery HTTP consumer.

### Evidence e10

[quarantine/kaagum/kaagum/tools/forges.scm:139–185](../../../quarantine/kaagum/kaagum/tools/forges.scm#L139): github-issue reads public GitHub/Forgejo discussion.

### Evidence e11

[quarantine/kaagum/kaagum/tools/kagi.scm:62–136](../../../quarantine/kaagum/kaagum/tools/kagi.scm#L62): Optional Kagi search/extraction tools.

### Evidence e12

[quarantine/kaagum/tests/tools.scm:24–44](../../../quarantine/kaagum/tests/tools.scm#L24): One invalid-type test with specific error oracle.

### Evidence e13

[quarantine/kaagum/kaagum/tea.scm:273–339](../../../quarantine/kaagum/kaagum/tea.scm#L273): Only cwd/tools slash commands; immediate end-turn.

