# Session persistence and restart/resume/continue

Persist model, effort and absolute workflow path as audited program records.
Explicit CLI selections override saved settings; changes become durable before
activation. Stable actor/conversation/workflow identities survive restart; migrate
legacy sessions from their latest coding decision without rewriting history.

A restart tool schedules a note; the current tool batch completes, its call results
are retained, and the turn exits without another provider request. A successful
turn commits a restart intent. Error/interruption cancels the in-memory request.
No process replacement occurs inside an active provider/tool operation.

The shell launcher observes exit 75 only after the entire old executable exits,
then starts the current release image with the same session and resume flag.
Persisted settings govern the new process. No original once-prompt is replayed.
Resume injects a user 'continue' with the retained note once, then consumes the
intent before requesting a provider. The context append origin deduplicates a
crash between injection and consumption. A crash or failed request after consumption does not retry
requests automatically; the retained continue/note remains for explicit continuation. Ordinary reopen never auto-consumes a pending intent.

Test actual sessions/launcher with a Lua fixture: persist settings, execute a
command/build, request restart, compare old/new PIDs and stable identity, inspect
continuation and exactly one restart token injection. Invalid settings must not
persist, failed/cancelled turns must not restart, pending linkage blocks restart.
Then a bounded real-provider self-development exercise edits/builds a temporary
workflow artifact, asks for restart, and verifies continuity from the new process.
No dependency adoption, production Python or outpost is required.
