# Integrated local collaboration campaign — 2026-10-08

The authorized campaign shipped named session selection, a common colleague call
and bounded native participants. A live OpenAI colleague identified two actual
small-viewport defects; Blackbird's main model made the source edits. Direct
physical-size PTY checks, managed compaction/original restoration and the actual
release launcher restart completed the focused repair. The live Claude answer
remains outstanding: its one request timed out with unknown remote disposition.

## Delivered behavior

| Work | Result |
| --- | --- |
| Session continuity | Retained128-byte names, searchable picker, explicit resume; independent context/settings/tasks/drafts. Busy or damaged destination returns once to the previous session. Restored working state dismisses the welcome screen. |
| Common colleague | Model tool `colleague`, Lua `blackbird.colleague` and `/colleague JSON` use the same admitted operation/result. `/colleagues` discovers installed transports without pretending to validate accounts. Requested/actual models and remote disposition remain separate. |
| Participants | Process-lifetime workers, configured concurrency1..16 (default2),32 current slots, addressed direction at request boundaries, exact-value message dedup, observe/await/join/cancel/archive and task badges. Join returns declared order; waiting and cancelled/unknown are explicit. |
| Retention/recovery | Exact prepared request retained before launch/enqueue. Worker capture buffering is bounded to8MiB of payload/1024 packets and drained by one engine owner. Unfinished pool/control admissions reopen unknown with no replay. General command/file custody remains fenced. |
| Useful repair | Small fallback clears the viewport instead of only row1, and cursor uses physical columns rather than reserved text width. Source edits came through Blackbird's main model/tools. |

Each message permits2048bytes,16 pending and64 lifetime deliveries per run.
Current results are bounded projections; detail stays in retained originals.
Archive frees settled current slots; it does not expire the plan or originals.
Existing diagnostics keep their30-day TTL. Workers have selected context and no
tools; arbitrary heap/child resurrection, P2P and remote tool-using agents remain
outside this implementation. Changes require normal native replacement to activate.

## Direct checks and actual design

Session_store/terminal cases check retained names/invalid input and picker
selection. The real new_session_pty driver switches independent sessions, verifies
context/tasks/drafts/settings/identity, busy queue boundaries, corrupt/busy
fallback and repeated restart routing. Mac debug/release and Linux debug pass.

The colleague_engine case sends the same request through operator, Lua and a
scripted model tool call. It checks only selected context is exported, result and
capture linkage, admission refusal before transport, one dispatch and unknown
settlement. Existing colleague decoding tests also pass. Mac debug/release and
Linux debug pass; coding's affected Mac debug case passes.

Participants uses deterministic transport barriers to establish actual overlap,
capacity refusal, accepted direction order/dedup/backpressure, cancellation
requested before worker return, declared join order, bounded capture failure,
failed durable admission before launch and saved unfinished-run unknowns. Engine
cases cover common controls, restart refusal, checkpoint retention and task/run
separation. Participant_recovery reopens a dispatched incomplete control admission,
checks unknown with zero provider calls, and confirms exec custody still refuses.

The real participants_pty case puts a local fake Claude first on PATH; it uses no
credentials. From the TUI it starts two tasks/runs, observes active work, cancels
one, joins the other, verifies neither task was automatically completed, switches
via /new and checks native restart/result retention. Native/PTY cases pass on Mac
debug/release, ASan/UBSan and Linux debug. The late field-order retry fix has its
direct Release case: reversed keys deduplicate, changed text conflicts. Native
participants source lint passes after removing two redundant parameter copies.

The new tiny_terminal_pty case populates a real32x120 PTY and a VT cell oracle,
enters `abc界`, then resizes to20x8. It checks rows below the composer are blank,
that the seven-cell draft puts the cursor at physical column8, and that subsequent
editing clears old wide cells. It also exercises9x90 and return to32x120, physical
cell/cursor bounds and terminal-mode restoration. The old binary fails the stale-
row assertion; the rebuilt binary passes on Mac debug/release and Linux debug.
This checks the emitted VT frame, not Ghostty image pixels or every terminal.

## Live calls, versions and interventions

Native C++20/Lua5.4.8; Mac Clang23.1.2, Linux Clang18.1.3/libstdc++13 development
profile. Codex CLI0.161.0 supplied existing authentication. Claude Code2.1.163 was
installed. No new runtime, framework, provider SDK or library was adopted.

| Actual call | Observed result |
| --- | --- |
| Claude/sonnet selected-source review through `/colleague` |90-second deadline; status/remote disposition unknown, error io; no answer, actual model or usage. No retry. Native command exit0 describes completed control handling, not remote success. |
| OpenAI participant viewport review | Requested/actual gpt-6.1-sol; completed in21.333s at join,6134 input/527 output tokens reported. It identified stale rows and cursor bounds from selected source. |
| Blackbird main model repair | Configured gpt-6.1-sol; four successful provider requests, one rg command and two native edit_file calls;16.205s for the native turn. Reported20632 input/376 output tokens across those requests. |

Usage above excludes the unavailable Claude consumption; it is not zero. No price
or general coding-uplift estimate is derived. Roles used the same OpenAI model;
this demonstrates the local working loop, not heterogeneous model cooperation.

Operator/Codex interventions were explicit: select review source and purpose,
brief the main model with the two concrete changes, author the independent PTY
case, format/build/check, manage/restore context, mark the task done after checks,
and arrange actual native replacement. This was a focused supervised repair,
not unattended sustained development or a matched solo comparison.

The same retained session summarized the colleague entry, then restored its exact
original. An explicit unknown-Claude note was carried forward without replay.
After rebuild and release publication, the actual scripts/blackbird launcher
restarted that session. The continuation program checked the same conversation,
done task, completed participant, restored selected-source original and explicit
unknown effect. It made no provider call. The operator's running process was not
interrupted.

## Source and remaining work

Deliveries: c3af6bc sessions, b312c23 common colleague, be9ba76 participants,
ecdb911 field-order retry/lifetime refinement, followed by the viewport repair
and these results. Scripts and cases are tracked; raw live request/result/program
and continuation artifacts remain under ignored context/campaign-selfdev and
context/campaign-colleague-live. Test sources and governing programs provide the
repeatable deterministic cases; re-running the live artifact would be a new call,
not recovery permission.

Orders1/3/4 are delivered. Order2's interface is delivered; its live non-OpenAI
outcome remains arconaut-3sm.2. The timeout is not an authentication diagnosis.
No further provider probe, benchmark, review campaign or resumption of the old
Giga was launched. Next broader scopes remain remote tool-using colleagues,
station attachment, multiplayer and independent login/distribution, as described
in docs/NEXT_CAMPAIGN.md.
