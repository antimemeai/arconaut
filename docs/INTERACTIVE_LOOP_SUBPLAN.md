# Make the bootstrap usable for development

Operator feedback supersedes the earlier readiness claim: the vertical slice proves
mechanisms, but does not yet unlock daily self-development. Immediate objective is
responsive interaction with enough execution visibility to work and steer.

First slice: show assistant text as its retained provider stream arrives; show actual
tool paths/commands and process output; make reasoning effort configurable; Ctrl-C
stops the current request/process/Lua turn and returns to the prompt with originals
and an explicit uncertain/cancelled outcome. Preserve completed item authority: a
preview is presentation only, never a partial tool invocation or published context.

Incremental SSE framing reads fragmented bytes/CRLF/multiline data, bounds
input, and presents only complete output_text.delta events. Raw stream capture
precedes preview. Completed Responses validation remains authoritative. Default Lua
workflow presents final text without duplicating the matching preview; incomplete
streams remain visibly incomplete and never authorize tool calls.

Native subprocess polling checks a cancellation predicate at bounded intervals.
SIGINT handler sets only a lock-free atomic flag; normal C++ teardown kills/reaps the owned
child group. Lua instruction hook interrupts pure Lua too. The sync engine stops
between effects and commits its observed disposition before returning. No async
session fabric, automatic retries or recovery fabrication are introduced.

Direct checks: every byte boundary of an SSE text event, CRLF/multiline framing,
preview-before-provider-return and no duplicate final display, failed stream after
preview cannot invoke a tool, actual child interruption/reaping and Lua-loop
interruption. CLI subprocess/pseudo-terminal checks exercise the real interface.
Use current OpenAI event handling and acquired provider literature as source
baseline; no libraries or new runtime adoption. Local debug and sanitizer checks
plus focused analysis cover changed boundaries. The basic TUI uses a conversation viewport, status and a multiline composer with
UTF-8 codepoint editing, bracketed paste, history and scrolling. Rendering uses
terminal wcwidth, with a minimum supported size of 12 columns by 10 rows.
The terminal thread handles input/rendering while one worker owns the entire turn;
messages transfer presentation only. Queue prompts while work runs; Ctrl-C clears
the queue and stops the active turn while preserving the composer draft, including
when idle; Ctrl-U clears a draft. Pasted control bytes are data, never shortcuts. Terminal mode is restored on normal exit
and C++ error paths. Further speed work follows measured operator use; do not trade away model capability silently.

## Exit-command correction

Operator hit unexpected /exit behavior and forced shutdown, followed by a restart
error. Support /exit alongside /quit in both clients; reject unknown slash commands
locally instead of starting a provider turn. Validate actual PTY exit and plain
unknown-command/reopen behavior. Investigate interrupted-session recovery separately
without deleting originals or assuming unknown effects failed.
