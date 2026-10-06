# Kimi colleague setup

2026-10-01. The operator requested the Kimi colleague skill and authorized using
their Kimi subscription for independent code review and potentially test work.

## Available runtime and subscription route

The existing executable is `/Users/patrickbeam/.kimi-code/bin/kimi`, version
2.1.1. No CLI installation, upgrade, interactive login or new subscription was
needed. `kimi doctor` validated its existing `config.toml` and `tui.toml`.
`kimi provider list` reports `managed:kimi-code`, type `kimi`, source `oauth`,
four model aliases, with default `kimi-code/k3`. Credentials were not printed or
read directly.

A no-tool connection check ran from a fresh temporary directory using prompt
mode and returned `KIMI_READY`, exit 0. This establishes working inference through
the existing subscription route; it does not establish code review quality.
The prompt explicitly prohibited tools, file reads and modifications. No Arconaut
material was sent during the connection check.

Relevant CLI commands, inspected locally:

```sh
kimi --version
kimi --help
kimi doctor
kimi provider list
kimi -p 'Connection check only. Do not call any tools, read files, or modify anything. Reply exactly: KIMI_READY' --output-format text
```

Prompt mode accepts `--model`, `--agent-file`, `--skills-dir` and structured
`stream-json` output. Its existing default model can conduct the requested review
without installing a delegation wrapper. Read-only review should receive a
self-contained brief and concrete paths; independent findings must still be
checked and integrated by the owning engineer.

## Skill installation status

The exact skill called `kimi-colleague` has not been identified. Searches covered current Codex, Claude and Kimi skill
directories, the local codex-tools project, Arconaut material, the OpenAI curated
skill listing, GitHub repository searches and web searches. The curated listing
does not contain that name. No claim is made that an unindexed or private skill
does not exist.

The parent authorized installation of a functionally suitable published skill
under its real name. After reading the skill, dispatch/review guidance and relay
entry points, installed **kimi-delegate** v0.5.0 (MIT), from
[amElnagdy/delegate-skills](https://github.com/amElnagdy/delegate-skills/tree/6826b363085dcc80875372315fe7d208c4bf733f/skills/kimi-delegate),
path `skills/kimi-delegate`, revision
`6826b363085dcc80875372315fe7d208c4bf733f`, into
`/Users/patrickbeam/.codex/skills/kimi-delegate`:

```sh
python3 /Users/patrickbeam/.codex/skills/.system/skill-installer/scripts/install-skill-from-github.py --repo amElnagdy/delegate-skills --ref 6826b363085dcc80875372315fe7d208c4bf733f --path skills/kimi-delegate
```

The skill is aimed at bounded implementation/test delegation, with exact briefs,
resumable sessions and captured structured results. It can support collaboration,
but its name is not `kimi-colleague`, and no alias or fabricated substitute was
created. Its Node relay has no package dependencies and does not handle
credentials or commit. It was documented against older CLI 0.24.0; the relay has
not been run or qualified against the installed 2.1.1. Its unconditional
orchestrator-commit instruction does not override this project's current user
authorization: no commit is authorized here.

Immediately after installation, the parent directed pausing ambiguous skill
installation pending the operator's source clarification. The preceding install
had already completed; this was reported immediately. The installed skill remains
inactive. It will be discoverable by Codex on the next turn.

The operator subsequently clarified that “colleague skill” meant directly
invoking Kimi Code as a colleague rather than a particular published package.
Created the owned `~/.codex/skills/kimi-colleague/SKILL.md` following the read
skill-creator instructions. It codifies the accepted direct CLI wrapper, explicit
review-session continuation, adversarial test proposals, and ACP/web alternatives.
Bundled `quick_validate.py` reports “Skill is valid!” The published
`kimi-delegate` remains installed but unused; its relay was never executed.

Official current Kimi documentation is at
[Kimi Code CLI](https://moonshotai.github.io/kimi-code/), with
[agent skills](https://moonshotai.github.io/kimi-code/en/customization/skills.md)
and [agents and sub-agents](https://moonshotai.github.io/kimi-code/en/customization/agents.md).
These describe Kimi-side extensions and do not establish the identity of a
Codex-side `kimi-colleague` skill.

## Next review

Kimi completed a concrete independent review of the C++/Lua rigor configuration
and Jev attribution utility. The actual response is retained in
[the independent rigor review](2026-10-01-kimi-rigor-review.md). Its six findings
and compilation-database oracle attacks await owner reproduction/classification
and follow-up in the same explicit session.

## Direct communication options

The installed CLI and [current command reference](https://moonshotai.github.io/kimi-code/en/reference/kimi-command.md)
support these alternatives without a colleague skill:

1. **Prompt-mode session:** `kimi -m kimi-code/k3 -p 'brief' --output-format stream-json`.
   Follow-up: `kimi -S <session-id> -p 'follow-up' --output-format stream-json`.
   Each invocation runs one turn; explicit session continuation preserves Kimi's
   local conversation. Prompt mode already runs automatically and rejects
   `--auto`, `--yolo`, or `--plan` combined with `--prompt`.
2. **Persistent ACP subprocess:** `kimi acp` exposes JSON-RPC over stdin/stdout:
   `initialize`, `session/new`, `session/prompt`, streamed `session/update`,
   cancellation, loading, resuming and forking. See the
   [ACP method reference](https://moonshotai.github.io/kimi-code/en/reference/kimi-acp.md).
   A small client can exchange messages with Kimi without emulating terminal
   keystrokes.
3. **Local shared service:** `kimi web --no-open` starts the authenticated REST and
   WebSocket service; the operator can also use its browser interface. The live
   `/openapi.json` and `/asyncapi.json` describe the running version. This is an
   experimental interface, documented in the
   [server API reference](https://moonshotai.github.io/kimi-code/en/reference/server-api.md).
4. **Operator terminal:** `kimi --auto` starts an interactive session without
   routine prompts; `kimi -S <session-id>` resumes an explicitly chosen session.

The current CLI exposes no public `--wire` transport flag. Its stored
`agents/*/wire.jsonl` is an internal recovery/debug event record and should not be
edited as a communication channel.

Current [custom-agent documentation](https://moonshotai.github.io/kimi-code/en/customization/agents.md)
also supports a `--agent-file` profile with a tool allowlist enforced before
execution. A reviewer can have only `Read`, `Grep`, and `Glob`; text-only reasoning
can use `tools: []`. This is stronger than a read-only sentence in a prompt. The
profile is bound at session creation and restored on resume; `--agent-file` cannot
be combined with `--session` or `--continue`. No long-lived service has been
launched. The profile qualification below also ran inference.

## Written sub-plan: direct review wrapper

Purpose: make actual independent Kimi review available now, preserving its raw
exchange without depending on the unqualified third-party relay. This is a
development utility, not Arconaut production machinery.

Build `scripts/kimi-review` with only Python's standard library. Require explicit
new-session or a prior review-run directory for continuation. Bind new sessions
to a custom profile allowing only `Read`, `Grep`, and `Glob`, with no sub-agent
access; continuation must come from a run created with that same profile and
working directory. Use a subprocess argument vector, capture raw stdout/stderr
unchanged under ignored `context/`, record the exact brief and child result, and
propagate nonzero exit. Never use most-recent-session selection, manipulate
credentials, execute code through Kimi, or commit.

Qualification: first check actual CLI tool-list behavior on a controlled file:
read its unknown contents while being asked to write another file and run a
command; only the expected read and content answer may occur. Then directly
check the wrapper's child argv, captured bytes, exit propagation, explicit
continuation and rejection of unrestricted/mismatched sessions with a controlled
fake executable. These are utility behavior checks, not tests for tests. Finally
run the concrete tooling review through the real subscription-backed CLI and
integrate valid findings in the owning lane.

### Implemented and checked

`scripts/kimi-review` now implements this sub-plan and `tooling/KIMI.md` documents
its operation. The direct CLI profile check is retained in ignored
`context/kimi-profile-qualification/`. With only `Read`, `Grep`, and `Glob`
available, Kimi read the unknown token `qualify-read-token-597f1b` and stated that
write/shell tools were unavailable. Raw events contain a single `Read` tool call;
neither requested sentinel file exists. This exercises real tool visibility,
not merely model agreement to a read-only prompt.

The current CLI reports resumability in a stdout meta event, not the stderr
resume banner used by text mode. A direct failing check exposed the initial
wrapper's loss of its session ID. The implementation now reads only the
structured `role=meta`, `type=session.resume_hint` event while keeping raw output
unchanged. It rejects conflicting session hints.

Controlled executable checks passed literal multiline argument delivery,
unchanged non-UTF-8 stderr capture, exact brief preservation, new profile binding,
explicit continuation, child exit 19 propagation, timeout exit 124 and refusal
to resume a run with an unrestricted/mismatched profile. Python syntax inspection
also passed. No tests were run through Kimi and no product code was written.

Timeout metadata now distinguishes invocation status 124 from the actual
terminated child's signal exit (-15 in the controlled case). TERM/KILL races with
a just-exited child do not become a misleading executable-launch error.

The first two real review invocations failed before inference with an OAuth
transport `fetch failed` error, correctly returning exit 1 and retaining stderr.
The public token endpoint was reachable without credentials; a bounded retry
then began actual read-only review. A duplicate default-model diagnostic had also
begun; following the owner's instruction, its verified child was terminated and
exited 143. The single actual rigor review remains in
`context/kimi-review/run-j3004_sd/`; findings are retained separately once returned.

The review subsequently completed successfully, with explicit captured session
`session_26abf263-6a3b-4b7f-a2b0-18f949732307` and exit 0. Kimi used only read
tools, made no code changes and executed no tests.

Owner self-review exposed another direct lifecycle defect: SIGTERM to the initial
wrapper left its detached child running. A controlled live-process red case
reproduced it and cleaned up the owned child. The wrapper now handles SIGINT,
SIGTERM and SIGHUP, signals its process group and waits for the child, ignores
repeated interruption during cleanup, and retains both invocation and actual
child status. Direct green process cases passed for all three signals: invocation
130/143/129 respectively, actual child -15, and no live owned child afterward.
This does not promise cleanup after an uncatchable SIGKILL or machine failure.
