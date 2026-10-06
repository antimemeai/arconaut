# A4 — terminal drafts/history are UI state, not runnable work

Grounding: current Composer Enter/history transitions, TerminalUI queue/dequeue
worker ownership, main session lock and RRC exit-before-new-image. Existing
INTERACTIVE_LOOP_SUBPLAN and initial Kimi queue findings govern restoration.

Persist a replaceable ui-state.json inside the already exclusively locked session;
this is bounded UI state, never context or another effect-output/audit store.
Save on input batches, queue transitions and exit. Atomic owned write_file keeps
prior state if replacement fails; warn and hold new work rather than exit or dispatch
without durably removing pending UI work. Invalid cursor boundaries snap back. Lossless hex strings tolerate incomplete UTF-8
keystrokes and pasted bytes. Draft <=1MiB, history <=128 and aggregate1MiB,
queued/recovered drafts <=128 and aggregate1MiB; reject oversized/damaged state
with visible warning, never repair audits. No save from worker/provider thread.

Restore composer+cursor/history; queued submissions become an explicitly labeled
recovered-draft shelf, not the runnable queue. /drafts lists; /draft N loads one
into the composer only when current draft is empty; Enter must submit explicitly.
RRC's injected continue alone enters the runnable queue, before drafts. Dequeue
and save before dispatch; previously dispatched prompts are not pending state.
Ctrl-C deliberately clears runnable queue, preserving current draft. Ctrl-Q exits
while preserving draft and pending queue as recoverable drafts; /exit does likewise.
Ordinary exit/RRC preserves queued work without executing it on reopen.

Direct PTY oracle: sent history recalled after exit, unsent UTF-8/multiline draft
and cursor recovered, queued command not executed on ordinary reopen or RRC,
explicit /draft+Enter executes exactly once, continuation first. Native state tests
attack bounds/damaged data. Build release and affected tests, restart and verify;
A3/A4 narrow review plus native sanitizer/portable checks at this boundary.
