# Single tribunal: disposition and measured limits

Exactly one round: three independent reviewers examined the candidate for native
memory/representation, terminal I/O/recovery, and rendering semantics/oracles.
Their reports remain unchanged. This is the implementer's disposition, not a
second review or a certification of the reviewers.

- M1/M2, R1: syntax consumes UTF-8 scalars and coalesces equal styles. Wrapping
  and rasterization use one cluster scanner for combining/ZWJ/flags/variation.
  Narrow fenced CJK and boundary emoji regression cases pass.
- M3/M4: oversized clusters become a replacement glyph; writing a cell resets its
  previous tail. Direct oversized/overwrite regressions pass.
- M5, R2/R3: choose a bounded display projection rather than an incremental
  unbounded-line parser. Retain at most 64KiB source, 2048 logical lines before
  batch eviction, and about 2048 bytes per logical line (finish trailing UTF-8).
  Evict in batches. Once an unfinished line is clipped, later bytes cause no
  layout invalidation. Adjacent syntax runs coalesce. Display announces clipping;
  retained audit originals are authoritative. Newline flood and growing line
  cases pass. This is intentionally a smaller presentation window, not lost audit.
- I/O P1: one nonblocking frame owner. Editor drains with a 150ms bound, restores
  descriptor flags before handoff, then reattaches afterward. Control sequences
  use checked bounded writes; shutdown cancels abandoned CSI with CAN and exits
  synchronized output. Raw mode restoration remains unconditional. Destructor
  control bytes are best effort if the terminal remains unreadable. Real undrained
  PTY quit returned in 177ms, restored configured terminal mode and file flags.
  macOS adds kernel PENDIN; exact whole-termios equality was an invalid assertion.
- I/O P2: Escape flush respects an absolute 50ms deadline despite unrelated wakes.
  Tiny packets never commit the stale normal grid. SIGWINCH wakes the poll through
  the self-pipe; the 1s probe remains a fallback for geometry changes without a
  signal. Actual PTY palette/editor/resize/cancel/reopen checks pass.
- R4/R5: completion conveys AttemptDisposition through a typed callback. Success,
  failure and unknown choose their presentation without matching English strings.
  Tool completion is activity, not a TURN footer. Restore only user/assistant roles.
- R6: bounded policy adjustment: eviction explicitly returns a scrolled reader to
  the live tail and displays the clipping notice, rather than silently pretending
  an anchor survived. Preserving surviving source anchors remains backlog; it is
  not claimed in this unit.
- R7: compare streamed/cold span text AND Ink. Independent VT oracle compares full
  and delta glyphs, styles and cursor, with byte-fragment delivery, stale content,
  resize, CJK, combining, ZWJ and flag fixtures. This models our emitted subset,
  not universal Unicode grapheme conformance or every terminal's emoji behavior.

Focused release checks: chat_view, chat_render_oracle, terminal, terminal_pty,
coding: 5/5, 32.93s. ASan chat_view passes. The actual native, provider-free preview
is docs/design/chat-native-preview.svg, unlike the earlier illustrative concept.

Release microfixture: 1000 120x40 counter updates, 34,981us and 67,932 bytes;
12-byte cells. Whole test (including adversarial inputs) peak RSS 8,912,896 bytes.
These are local fixture measurements. They do not establish end-to-end input
latency or superiority to another harness. Normal composition still resets/scans
and copies the viewport, though output is incremental and history layout cached.

No dependency adoption. No second tribunal. Full Markdown, tables/diffs, folded
activity groups, theme/copy controls, universal grapheme segmentation and surviving
scroll anchors remain subsequent bounded work. Blackbird rename waits for operator
visual acceptance; campaigns remain paused.

Configured `scripts/rigor check release` built all targets and passed all 42 tests
in115.40s, then stopped at pre-existing formatting violations in retained_events,
backstop/candidate/tool files and their tests. Changed files pass formatting.
Targeted clang-tidy passes main and chat_view_test; chat_view's new unnecessary
copy and enum sentinel warnings were fixed. Terminal and coding also report
pre-existing editor-cleanup dead-store/schema-parameter warnings. Keep this
repository-wide lint debt explicit; the full rigor command is not green.
