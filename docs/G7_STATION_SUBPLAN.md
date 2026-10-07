# G7 local file station and boundary control

Unit arconaut-7iy.7. Start2026-10-07T06:27:04.372475Z;
whole deadline07:57:04.372475Z (90 minutes, including activation).
No predecessor context/admission authority imported.

Purpose: opt-in native idle station that admits identified local source events
into the ordinary persistent participant/context/workflow engine. No external
board/service governance, no integration platform or concurrent scheduler.

Source: PARTICIPANTS_AND_MODES station/D1 requirements; main.cpp's native
session lock, settings and engine.turn; session.cpp/AuditLog retained program
records; G6's useful operating map. NTM primary source pinned ee589e7e (see
papers/capabilities/dossiers/ntm.md and coordination-services.md):
pipeline/executor.go1738–1847 persists sending before paste,987–1007 refuses
ordinary uncertain resend; submission_verification_test.go277–308 counts pastes.
Consequence here: admission durable before dispatch, unknown never replayed;
externally count actual workflow writes. No reference code/dependency adopted.
Crash-Only/Microreboot primary literature in backstop-recovery-study separates
persistent state and local lifetime from remote outcomes: reopening our store is
not proof of process containment or external success. Unknown reopen pauses.

Narrow adapter: atomically replaced bounded events.json snapshot (source,id,
cursor,prompt), bounded latest control.json command (id,action,text). Source
owns cursor/replay. Pending events stay at source while busy/paused. Retain full
admitted input in native audit; repeated source/id is ignored, not dispatched.
Status file allows attach inspection with stable PID/actor/context head; no
second session owner. Steer/pause/resume/stop apply at workflow boundaries.
Idle polls use zero model requests. SIGINT pauses; stop closes worker, not any
consumed service. Admission cap4096 is explicit, exhausted station stops visibly.

Direct failing oracle: previous native executable rejects --station; no source
admission/control exists. New station tests count completed and unknown dispatch
across reopen, duplicate commands, pause, steering and explicit independent work.
Process oracle counts actual native workflow file writes; observes idle audit
size, stable PID/actor, paused source retention and failed effect nonreplay.
Useful delivery: actual source event creates station operating/source map for
later queued work. Native release replacement and actual same-session RRC.

Implementation allowance: up to65 minutes within whole deadline; hardening at
most25 minutes inside it. Before hardening start declare concrete tasks/checks,
then remediation and one recheck/fix only. At exhaustion retain inactive candidate
and exact blocker; no assurance reset. Unrelated dirty startup-replay source in
main checkout is preserved; use existing empty pilot pool slot for clean builds.

Hardening actual start06:33:37UTC, deadline06:58:37UTC (25minutes), not a
budget reset. Initial commentary rounded the minute; these process-clock values
are the allowance. Layer1 added retained RRC profile with no implicit continue,
accepted control originals, bounded input rejection/pause and actual RRC process
oracle. Mac release and Linux debug/release/ASan station checks passed.
Layer2 single source recheck06:37–06:38 found exhausted operator-command table
could block native error pause. Fixed with a separate native station.pause fact;
actual4096-command exhaustion then successful native pause/no-dispatch passed.
Only affected final tests follow this fix; no third review or old campaign rerun.
