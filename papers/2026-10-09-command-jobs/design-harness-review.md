# Adversarial harness review of command jobs design and subplan

2026-10-09. Reviewed `docs/COMMAND_JOBS.md` and `docs/COMMAND_JOBS_SUBPLAN.md` against `harnesses.md` and the current-source/platform integration reports. No product edit, imported execution, or dependency selection. This is design/plan challenge before implementation; it does not spend or start the code hardening layers.

The permanent owner/collector and separate attachment/wait state are the right foundation. The design directly avoids Codex's silent live-job LRU kill, Hermes's decoder handoff, Qwen's temporary ownership gap, and Kimi's fabricated terminality from an observation snapshot. The original launch-attempt/open-effect contract and WNOWAIT lifetime are explicit. The following obligations remain underspecified enough that different implementations could all claim conformance while producing materially different operator behavior.

## Findings to settle before code

### H1 — Bounded raw replay does not restore a terminal

The shared-interface section promises raw foreground return with bounded retained live chunks, replay and a visible gap for evicted prefix. A stream suffix can start inside an escape sequence, UTF-8 character, graphics payload or application protocol; missing history can contain the alternate-screen switch, cursor/mode changes and the screen contents. A gap label is sufficient for a log reader but does not reconstruct an interactive program's terminal. Even complete replay can provoke side effects: replayed terminal status/device queries cause fresh terminal replies which, if blindly relayed, become new input to a child that may no longer be expecting them.

Gemini supplies terminal-emulator snapshots for this reason. Its reconnect path replaces full screen snapshots rather than pretending byte suffixes reconstruct screen state. oh-my-pi similarly binds its overlay to a terminal emulator; neither source validates arbitrary raw replay.

Resolve the promised behavior explicitly. Options include an owned terminal-state representation with replayable screen/mode snapshots; replay of all original output from a defined baseline with live input/replies suppressed until catch-up; or a deliberately limited byte-relay attachment that does not claim to reconstruct lost terminal state and has an explicit redraw protocol for supported applications. The latter limitation must be visible to the operator and tests. Do not quietly infer that resize guarantees every application redraws. State which bytes are rendered, where the attach cursor begins, when input becomes enabled, and how a replay/query reply is distinguished from live input.

Direct cases: child enters alternate screen, disables echo with private termios, sets cursor positioning and mouse/bracketed-paste modes, prints a known screen, then remains quiet while the operator detaches/re-attaches. Require defined screen/modes after return and intact composer on detach. Evict history inside CSI and multibyte sequences, and issue DSR/device queries before detach; replay must not create extra child input. Verify exact audit bytes separately from the operator representation.

### H2 — Workflow cancellation, waiter cancellation and job stop need one policy

Background is clearly separate from stop, and Ctrl-C in raw PTY goes through its driver. The design does not say what current workflow cancellation does to an attached ordinary exec, attached PTY, a yielded job from that workflow, a foreground wait admitted in a later workflow, or an operator-launched job. Existing synchronous cancellation terminates commands; Codex instead keeps its retained terminal alive when a turn is interrupted. Both are coherent, materially different policies.

Publish a small policy table, including Ctrl-C outside PTY, cancellation during launch-before-PID, cancellation during a wait, turn end, background release, explicit stop, timeout and shutdown. Define whether a stopped PTY job auto-continues on foreground: platform study says observation/attachment alone must not send CONT. Keep job stop sticky across turn cancellation resets. Input Ctrl-C is a write/effect through the PTY driver, not evidence of process termination.

Direct cases: background A, run B, cancel B, attach A; cancel a waiter for A and then wait again; cancel workflow during admitted launch before helper spawn; Ctrl-C child foreground subgroup while root shell survives; SIGSTOP then attach and confirm no implicit CONT. Observe actual process survival/status and original attempt dispositions.

### H3 — Raw input's admission and delivery guarantees are unresolved

Ownership says every input/signal operation has its own admitted attempt, while the busy UI uses a native mailbox and only engine owner may mutate retained state. The exact boundary between keystroke receipt, admission, queue acceptance, partial write and completion is unspecified. A worker must not send unaudited raw input merely because the UI enqueued it. Conversely the engine cannot block waiting for input delivery while the worker blocks on output capacity which only the engine can drain.

“Duplicate requests cannot repeat writes/signals” also needs explicit partial-write/failure semantics. Marking delivery consumed before write avoids retries but can lose bytes; marking it consumed after partial write can duplicate a prefix. The design must retain request identity/content and actual accepted/written extent; unknown effect after failure must fence retry rather than imply no bytes reached the child. The caller needs a clear distinction between accepted, fully delivered, partial/unknown and rejected. Define deduplication lifetime and capacity, including identical request after job completion.

Resolve ordering between model input and operator raw input, and whether resize is an admitted effect or a coalesced best-effort presentation operation. Define Ctrl-B interception for fragmented terminal input, bracketed paste, and a literal Ctrl-B forwarding escape. A blanket byte deletion would alter pasted program input.

Direct cases: actual input exceeding PTY/input-queue capacity interleaved with a child producing more than output-queue capacity; short write then EAGAIN; duplicate identity during queue/in-flight/after acknowledgement; same identity with changed contents; child exits midway through write. Independently count bytes received by the child and audit their offsets/identities. Paste a string containing Ctrl-B and fragmented escape sequences; assert declared detach/literal rules exactly.

### H4 — Attachment has no specified lease or control arbitration

“Exact job/wait identity under registry lock” correctly prevents stale control from affecting a later job, but the public selection/arbitration rules are absent. Can a model foreground wait coexist with raw operator attachment? Can two waiters coexist? Which is released by `/bg JOB`, `/bg` or Ctrl-B? What happens to a second `/fg` while another PTY is attached? Does attached child exit terminate only the matching attachment and restore composer once, or can late output from that child affect a newer attachment?

Use a distinct attachment/wait identity and state the policy for replacing/rejecting attachments, independent observation cursors and raw-input ownership. Selection by optional JOB must capture its exact active waiter at admission. Do not defer resolving an omitted job target until control executes. Keep signal/input identity tied to the job, not a globally current selection.

Direct schedule: A attached; enqueue detach-A; A exits; attach B; process delayed detach/output/resize-A. B retains focus/input; stale request has a declared finished/stale result; terminal restoration runs once for A without clobbering B. Race model waits with raw operator attach and completion; require exactly one launch settlement and correctly attributed wait results.

### H5 — Capture/read/retention extents must be observable separately

“Full originals remain available through output_ref” and “retained live chunks” leave unclear whether UI may render worker-received bytes before the audit owns them, and whether cursors address received, audited or live-buffer extents. A queued byte observed by the operator is not necessarily retained if the next append fails. A raw viewer silently advancing its cursor past an unaudited/evicted prefix could falsely make later reads look complete.

Define one absolute offset space per raw stream or one explicitly merged stream; identify how stdout/stderr ordering is preserved. Expose received versus retained high-water marks where they differ. A read's bytes and sealed/caught-up status must refer to a consistent snapshot. Prefix gaps must name exact offsets; retention error must leave an explicit last committed extent. Originals must remain original bytes, including NUL/invalid UTF-8/ANSI.

Direct cases: deterministic binary blocks across repeated independent cursor reads; output capture accepted by worker then audit append fails; empty read racing last output and terminal commit; bounded live-prefix eviction; two jobs finish while another synchronous operation is active. Check exact source bytes and retained dispositions, not just display text.

### H6 — Full-queue stop and shutdown need a progress mechanism

The design requires bounded worker queues and stoppability while full. That is a requirement, not yet an algorithm. A worker blocked pushing output cannot reach its control mailbox, timeout check or wait/cleanup if all progress occurs on that same worker. During shutdown the engine must keep draining while it requests stop and joins; joining first can deadlock. Input writes, helper-startup status pipes and terminal finalization require the same scrutiny.

State the worker's nonblocking read/write/control/wait multiplexing, reserved terminal-event capacity, stop wakeup and producer enqueue failure behavior. Engine pumping must be bounded but fair across jobs; a quiet provider, plain stdin wait and uncooperative Lua must not destroy the promised timeout/stop progress. Code can stop independently while retention/presentation catches up. The subplan's separate registry and integration stages must not commit a supposedly useful candidate with lifetime/deadline/stop unqualified.

Direct cases: fill actual output queue, block engine consumption at a gate, then request stop/deadline and establish the child's kernel exit; resume owner and require bounded final retention. Quit while queue full, child stopped, input pending and renderer blocked. Require no remaining worker/owned descriptor/zombie and no callback after UI destruction. Failure fixture cleanup must save PGID before parent exits.

## Plan implications

Steps 2–4 are a coherent unit, but Step 2 should explicitly include red end-to-end raw-attachment/input tests and bounded-queue stop/shutdown tests before selecting mechanics. The raw-return scope from H1 must be resolved before implementation; a permanent job manager alone cannot qualify it. Tests should distinguish model wait functionality from operator interactive return and stop/continue semantics.

The 90-minute implementation envelope is a bound, not evidence that every promised capability can fit. Keep any incomplete candidate inactive and report exact unsatisfied behavior at the bound. Do not spend that envelope constructing an unsupported arbitrary-terminal claim. Static analysis and sanitizer/Linux runs remain direct fault-class checks; the listed source-study tests do not constitute Blackbird qualification or a new assurance gate.

No finding asks to expand into provider steering, trajectories, external dependencies or a restart-surviving daemon. All findings constrain the already selected command custody and foreground/background unit.
