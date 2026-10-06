## Final review — A3/A4 scope (terminal persistence, coding stats/usage)

Inspected: `src/terminal.cpp` (validate_state/load/save, Composer::feed/draft, TerminalUI::run lines 333–586), `src/coding.cpp` lines 640–792, both subplan docs.

### Findings

**No blocking faults found.** Specifically:

1. **Duplicate restored queued effects — not present.** `recovered` (shelf) and `queued` (runnable) are disjoint; persist merges them (terminal.cpp:362–363) but on reload both land only in `recovered` (line 350), and `/draft N` erases from the shelf on load (line 562), so a recovered prompt can execute only via explicit `/draft`+Enter, exactly once. Matches A4 line 26.

2. **Save-before-dispatch — documented intent, not a fault.** Lines 425–429 erase+persist before `start()`, and the immediate-submit path (578–579) persists before dispatch, so a crash mid-turn loses the in-flight prompt. A4 line 19 explicitly states "Dequeue and save before dispatch; previously dispatched prompts are not pending state." Consequence (lost in-flight prompt on crash) is accepted by the plan; flagging only so the owner confirms this tradeoff consciously.

3. **RRC continuation-first — handled.** `load_terminal_state` snaps `state.cursor` forward past UTF-8 continuation bytes (lines 134–135) after bounded validation; hex-v1 encoding tolerates incomplete keystroke bytes. Conforms to A4 lines 10, 26.

4. **B1 (prior tentative) — confirmed invalid.** Enter (feed, line 242–252) returns the whole composer text and clears it before the handler; `starts_with("/draft ")` (line 553) can only match when the composer held exactly that command, so no unsent user text exists to be overwritten by `composer.draft` (line 561). Not reachable via real input flow.

5. **Minor doc/code mismatch (non-blocking):** A4 line 17 says `/draft N` loads "only when current draft is empty"; code loads unconditionally (lines 553–564). As shown in (4), the composer is always empty at that point in the real flow, so there is no data-loss trigger; either add the empty check for defense-in-depth or amend the doc.

6. **Usage allowlist / no fabrication — correct.** `usage_` reset to null per request (coding.cpp:652); `select` accepts only nonempty all-digit lexemes (rejects negatives, decimals, strings — matches "nonnegative integral numeric lexemes", lines 679–687); allowlist limited to input/output/total + cached/reasoning detail groups (688–699), excluding extension/credential strings; absent usage yields JSON null in stats (788), never a zero. `last_request_bytes_` is the exact compact JSON size captured pre-dispatch (650–651) with `request_scope` documenting "last request in this process" (789–791). Failure path leaves usage null since the exception escapes before line 676.

7. **Bounded UI state — enforced.** draft ≤1MiB, history/queued ≤128 entries and ≤1MiB aggregate (validate_state, lines 71–84), state file read capped at 7MiB (line 120), corrupt/oversized state rejected with visible warning and audit untouched (lines 351–353); queue-full retains the prompt in the composer instead of dropping it (573–575). Transcript independently trimmed at 1MiB (432–436).

### Limitations
Read-only, bounded-line inspection; worker-thread `perform` internals, `ResponsePreview`, and `ContextStore::stats()` byte-count internals were not opened. Persistence failure propagation and cursor-mid-codepoint are under independent owner evaluation and were not re-adjudicated here.
