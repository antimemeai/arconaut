# Independent Kimi review

Kimi Code 2.1.1 is already installed at `~/.kimi-code/bin/kimi`. Its existing
`managed:kimi-code` OAuth provider and `kimi-code/k3` model alias work through
the operator's subscription. `kimi doctor` passes and a no-tool live prompt
returned the expected answer. No new login, API key, subscription or upgrade was
needed. This is development tooling; no Python or Kimi CLI is an Arconaut
production dependency.

Use the owned wrapper for independent review:

```sh
scripts/kimi-review --new --brief context/review-brief.txt
scripts/kimi-review --resume context/kimi-review/run-EXAMPLE --brief context/review-followup.txt
```

The brief should give Kimi the concrete files, relevant specification, threat or
fault classes, and expected findings format. Kimi sees its own session and files,
without Codex's conversation. Ask for independent analysis before disclosing
the owner's expected findings. Kimi's final answer remains a claim to examine.
For tests, ask her to propose concrete inputs and specification-derived expected
results; the owner implements and executes them through the project's rigor
profiles. Review mode cannot run tests itself.

The wrapper requires either a new session or an explicit prior run directory.
It never selects the latest session implicitly. New sessions bind a custom
profile allowing only `Read`, `Grep`, and `Glob`, with no sub-agent access. The
CLI enforces the allowlist before execution. Continuation checks the prior run's
working directory and profile fingerprint and sends that run's captured session
ID. It rejects missing or mismatched IDs rather than guessing. Prior capture
files are local operator data, not a hostile-input security boundary.

Each invocation writes a new directory under ignored `context/kimi-review/`:

- `brief.txt`: the exact task text.
- `reviewer.md`: the bound profile for new sessions.
- `events.jsonl`: unchanged CLI stdout, including assistant replies and tool events.
- `stderr.txt`: unchanged CLI stderr.
- `result.json`: invocation and actual child exit status, working directory, model, profile fingerprint and
  explicit session ID when the current CLI emits it.

The wrapper uses an argument vector, not a shell command, and defaults to a
15-minute limit. Override with `--timeout SECONDS`; it terminates its child
process group on timeout or SIGINT/SIGTERM/SIGHUP and waits for the child.
The same cleanup runs on post-spawn failures, including an `OSError` from waiting;
it terminates and reaps the actual child before recording failure.
Invocation status and actual child status are recorded separately. `--cd PATH` chooses the target working
directory; `--model ALIAS` chooses an existing configured alias. An existing
review session retains its original profile. Do not manually alter captures to
resume unrelated sessions. Nonzero child status propagates to the caller.

This is a tool-access restriction, not an OS sandbox: read tools can inspect
accessible files. Brief scopes should name the actual material to inspect.
The brief appears in the child process argument list, so reference credentials
through neither inline values nor review tasks. Captures retain provider-visible
material locally; do not publish them wholesale.

Current CLI 2.1.1 emits its session ID as a structured `session.resume_hint`
event. The wrapper preserves unknown events and does not mistake assistant prose
for that metadata. The controlled profile qualification read a previously
unknown file correctly and had no write/shell tool available despite requests.
Controlled child checks established literal argument delivery, unchanged raw
capture, explicit continuation, exit propagation, timeout and rejection of an
unrestricted prior session. These checks qualify the utility's limited behavior,
not Kimi's judgment or general security properties.

## Communicating directly

The owned wrapper gives its Kimi child
`NODE_OPTIONS=--network-family-autoselection-attempt-timeout=3000` when that
variable is absent. An explicitly inherited value is honored unchanged. This
setting is local to the development child: on this Mac, Node's default address
attempts failed with ETIMEDOUT while curl connected after about0.87s; the longer
attempt enabled actual subscription-backed independent review. It changes no
TLS/authentication, shell configuration, reviewer tools or Arconaut runtime.
The installed2.1.1 executable is a Node single-binary distribution. See
[Node24.7 CLI](https://nodejs.org/download/release/v24.7.0/docs/api/cli.html#--network-family-autoselection-attempt-timeout)
and [Kimi network environment](https://moonshotai.github.io/kimi-code/en/configuration/env-vars.html#http-proxy).

For simple turns, the CLI itself suffices:

```sh
kimi -m kimi-code/k3 -p 'Review the specified files' --output-format stream-json
kimi -S SESSION_ID -p 'Here is the follow-up' --output-format stream-json
kimi session list --cwd /Users/patrickbeam/projects/arconaut --json
```

Prompt mode already runs automatically; it cannot be combined with `--auto`,
`--yolo`, or `--plan`. Without the custom reviewer profile it has ordinary editing
and execution tools. An operator can use `kimi --auto` interactively or resume
the chosen session with `kimi -S SESSION_ID`.

For a standing colleague, `kimi acp` offers JSON-RPC over stdin/stdout with
`initialize`, `session/new`, `session/prompt`, streamed `session/update`,
cancellation and session recovery. `kimi web --no-open` offers a local
authenticated REST/WebSocket service plus a browser interface. Its live
`/openapi.json` and `/asyncapi.json` describe the running version; that API is
experimental. Neither transport has been integrated or launched here. The
stored `wire.jsonl` files are internal recovery/debug records, not a writable
messaging interface. See the official [command reference](https://moonshotai.github.io/kimi-code/en/reference/kimi-command.md),
[ACP methods](https://moonshotai.github.io/kimi-code/en/reference/kimi-acp.md),
[custom agents](https://moonshotai.github.io/kimi-code/en/customization/agents.md)
and [server API](https://moonshotai.github.io/kimi-code/en/reference/server-api.md).

## Installed published skill

Following the operator's clarification that “colleague skill” meant direct CLI
collaboration, created the owned `~/.codex/skills/kimi-colleague/SKILL.md`.
Bundled skill-creator validation passes. This is the skill to use for direct
new/resumed reviews and continuing conversation; Codex discovers it on the next
turn.

The exact name `kimi-colleague` was not found. The functionally related published
**kimi-delegate** v0.5.0 (MIT) was installed under its real name using the Codex
skill-installer, from [amElnagdy/delegate-skills](https://github.com/amElnagdy/delegate-skills/tree/6826b363085dcc80875372315fe7d208c4bf733f/skills/kimi-delegate),
revision `6826b363085dcc80875372315fe7d208c4bf733f`, into
`~/.codex/skills/kimi-delegate`. It is available to Codex on the next turn.

It describes bounded implementation delegation and result capture, with the
orchestrator reviewing the result. Its Node relay was documented against older
CLI 0.24.0 and remains unused/unqualified here. The owned wrapper above invokes
the current CLI directly. The skill's commit guidance does not authorize a Git
commit in this project. The setup report records installation chronology and
source ambiguity at [the Kimi setup report](../papers/2026-10-01-kimi-colleague-setup.md).
