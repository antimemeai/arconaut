# Recheck report

Verified each correction against the current code and tests:

1. **Composer sole classifier** — `src/terminal.cpp:133-135` returns `cancel` without touching `text_`; the byte loop (`397-423`) has no 0x03 pre-interception; paste path (`129-131`) treats control bytes as data; Ctrl-U clears (`163-167`). `tests/terminal_test.cpp:32` asserts the draft survives cancel; `scripts/check-terminal-pty:63-66` pastes a literal `^C` during an active shell and asserts the shell survives, then clears via Ctrl-U. Queue-drop and group reaping still exercised (`68-88`). Sound.
2. **Anonymous deltas** — `src/openai.cpp:251-254` skips missing/empty `item_id`; `tests/openai_test.cpp:38-42` covers the direct case. Consequently `present()`'s `""` fallback (`src/coding.cpp:628`) can no longer false-match a preview entry; no-id final items display in full. Sound.
3. **Poll errors** — `src/terminal.cpp:382-386` throws on non-EINTR. Unwind order is correct: `StopOnExit` sets `cancelled`, the `std::jthread` joins the worker (bounded by the ≤100 ms cancellation polls), `TerminalMode` restores last. Sound.
4. **Cancellation fixture** — `tests/coding_test.cpp:194-217` now triggers on accumulated observed output (chunking-safe since `process` accumulates), looks up the interrupted output by `call_id`, and asserts the latest terminal disposition is `unknown`. PTY adds a `killpg` liveness loop (`81-88`). The remaining 100 ms-poll vs. immediate-printf interleave is negligible. Sound.
5. **Deliberate behaviors documented** — preview mismatch marks a correction and shows authoritative final text (`src/coding.cpp:629-635`; docs 79-80); workflow-error linkage repair is documented (docs 92-94); pasted ESC retained as data with hex-escaped rendering (`terminal_lines`, `214-219`).

## Remaining findings (minor, follow-up level)

- **Idle Ctrl-C clobbers the status summary** — `src/terminal.cpp:404-407`: a cancel with no active turn sets `activity = "Ready"`, erasing the previous `Stopped · Ns` outcome line, and leaves `cancelled` latched true (reset only by the next `start()`). Cosmetic; no turn can observe the stale flag because `start()` resets it before launching the worker.
- **Cancel racing turn completion mislabels outcome** — a Ctrl-C processed after the worker finished but before its `complete` message is drained reports "Stopped · context/audit retained" for a turn that actually ran to completion. Presentation-only, but it contradicts the audit for that turn.
- **PTY pasted-`^C` oracle is one-directional** — `check-terminal-pty:63-66` proves the shell wasn't stopped, but doesn't prove the pasted byte reached the draft as data before Ctrl-U clears it (the `wait_for(b"draft contains")` match would also pass without the `\x03`). `terminal_lines` escaping is covered in the unit test, so impact is low.

No remaining high-priority scoped defects. Threading, preview/stream ordering, partial-call dispatch, interruption disposition/linkage, argv precedence, and terminal restoration all hold as corrected.
