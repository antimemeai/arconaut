# Rich chat tribunal: rendering, semantics, and direct oracles

2026-10-07. One adversarial review round of the working rich-chat diff. Read-only
review apart from this report. No provider requests, dependency adoption, second
tribunal, or broad certification. Reviewed `src/chat_view.cpp`, its public types,
the changed terminal/main/coding event paths, `tests/chat_view_test.cpp`, and the
PTY screen oracle. Line references describe the candidate at review time.

The cell packet ownership and commit-after-flush approach are useful foundations.
The candidate is not yet acceptable under its own stated renderer contract. The
principal problems are upstream of the damage writer: inconsistent grapheme
geometry, unchecked derived expansion, an expensive unfinished-line fast path,
and presentation events that lose actual outcome/role information.

## R1 — High: wrapping splits clusters that the grid promises to preserve

`src/chat_view.cpp:133-160` wraps individual runes using `wcwidth`, whereas
`ChatGrid::line` later joins ZWJ, variation, modifier and flag sequences into cells.
Those two different geometry models cannot compose. A direct compiled probe:

```
ChatView v;
v.append(ChatKind::assistant, "aa👩‍💻x");
v.rows(8);
```

produces `aa👩‍` on one content row and `💻x` on the next. The emoji becomes two
different glyphs. The same defect appears at ordinary widths when a cluster lands
near the content edge. Direct-grid emoji tests do not exercise this path.

Remedy: segment/measure display clusters before wrapping, and use the same cluster
footprints for rows, grid placement and composer/cursor geometry. Never break
inside a cluster. Establish a deterministic replacement policy if a cluster is
wider than the available content area. One direct test should check rendered
rows/cells for ZWJ, flags, combining text and variation sequences at boundaries.

## R2 — High: the source-byte limit permits enormous derived displays

`src/chat_view.cpp:179-208, 241-295` bounds source bytes and block count, but not
rows, spans, derived bytes, or work. One 1 MiB block of newlines creates
**1,048,577 retained ChatRows**. Each row has a vector and generally a separately
allocated rail span. A direct probe containing this case and the R3 fixture reached
115,130,368 bytes maximum RSS; that is a process measurement, not an isolated row
allocation count. The object sizes alone show substantial expansion.

`syntax` at lines 96-129 creates a separate owning span for each punctuation
character before wrapping coalesces it again. A punctuation-heavy code line has a
second large temporary expansion. Both are ordinary model/process output inputs.

Remedy: bound the derived projection, not merely its source. Retain a bounded row
window with an explicit omitted-history indicator, or index source ranges and
materialize only the bounded viewport/cache. Coalesce adjacent equal-style syntax
runs before allocating spans. Keep originals authoritative and inspectable. Check
newline-heavy output and punctuation-heavy code against explicit row/span budgets.

## R3 — High for the performance objective: streaming a long line rescans its prefix

`src/chat_view.cpp:263-293` advances `committed` only at newline. Every partial
chunk sanitizes, parses, allocates and wraps the entire unfinished source line.
Growing that line in fixed-size chunks therefore causes quadratic cumulative work.
This is a realistic tool case: minified output, JSON and long progress lines.

A release-linked direct probe appended 256 chunks of 1 KiB ASCII to one assistant
line and called `rows(120)` after each chunk. It took 871 ms on its first execution
and 739 ms on a repeat. The combined R1/R2/R3 probe retired about 10.9 billion
instructions. These are local fixture observations, not portable latency claims.

Remedy: preserve incremental sanitized/lexed line state and append cheap runs;
reparse only when a delimiter actually changes interpretation, with a bounded
fallback for pathological lines. Alternatively bound active-line presentation and
offer full source inspection. Measure the real growing-line workload, not just a
counter changing in an already materialized grid.

## R4 — High: failed/unknown tools get a success-shaped completion

`src/terminal.cpp:1118-1122` always adds `✓` and `ChatKind::summary` to every
non-provider completion. `src/coding.cpp:1088-1103` already knows whether an attempt
completed, failed, or has an unknown outcome, but exports that distinction only
inside a display string. Consequently a timeout can appear as `✓ exec timed out;
outcome unknown` under a muted `TURN` heading. Failed tools are not red errors,
and a tool completion is mislabeled as a turn.

Remedy: send a structured operation completion containing operation identity/type,
disposition and elapsed time. Choose success/failure/unknown styling from that
disposition; never infer it by matching English prose. Distinguish a tool
completion from the final turn summary. Directly exercise exit-nonzero and unknown
timeout as well as success, asserting the visible outcome and semantic style.

## R5 — Medium: session restoration relabels instructions as assistant speech

`src/main.cpp:563-579` maps every role except `user` to `ChatKind::assistant`.
This includes real `developer`/`system` context entries; developer summaries and
backstop instructions are supported by the repository. Restart can consequently
show internal instructions under `ARCO`, even though they were never conversation
speech. This is a new consequence of making the label semantic.

Remedy: map known roles explicitly. Restore user/assistant conversation messages;
represent context summaries distinctly or keep internal instructions in inspection.
Do not guess a speaker for an unknown role. A restored fixture should contain user,
assistant and developer entries and assert the resulting speaker labels.

## R6 — Medium: trimming loses the reading anchor

`src/terminal.cpp:1209-1215` passes the same post-trim count as `before_trim` and
`after_trim`. `terminal_scroll_after_output` needs the true pre-trim growth to keep
the viewed content fixed. Once the display cap is reached and old rows are removed
while new rows arrive, the count may stay constant; the current offset then follows
the replacement content forward instead of holding the user's reading position.
`ChatView::trim` also provides no visible omitted-history notice.

Remedy: expose stable source/block anchors or an explicit removed/appended row
accounting result; clamp only when the anchor itself is gone, and say so. A direct
PTY/model check should append past the cap while viewing surviving older rows.
The current streaming scroll test exercises growth below the cap only.

## R7 — High evidence gap: the advertised cell-equivalence oracle does not exist

`tests/chat_view_test.cpp:37-44` compares flattened text, discarding Ink/style,
and uses an ASCII-only Markdown fixture. That does not check styled cold/streaming
equivalence. Its diagnostic assertion at line 31 never submits a diagnostic at
all. The damage test checks packet size/touched cells, not the terminal's resulting
cells. A writer can touch the right number of cells and still emit the wrong bytes.

`scripts/terminal-screen-oracle.py:25-48` discards SGR, treats ZWJ/variation
selectors as independent spacing characters, and has no terminal dimensions or
autowrap. It is useful for the current ASCII PTY command assertions, but cannot
establish the promised Unicode/styling/resize cell agreement. Do not present it as
that stronger oracle.

Remedy: compare styled row sequences for cold/streaming equivalence. Feed actual
incremental and full paint packets into an independent screen model, then compare
cells, styles and final cursor for a bounded fixture set: narrowing/growing text,
wide-to-narrow replacement, combining tails, resize and interrupted packet delivery.
The oracle must support exactly the subset being claimed; unsupported Unicode
cases need explicit limitation rather than false conformance. Check diagnostic
separation at the engine/UI callback boundary with real diagnostic events.

## Evidence and limits

The existing release `chat_view_test` passed. It reported 33,383 microseconds and
67,932 bytes for 1,000 120x40 counter changes. That fixture bypasses the terminal's
full grid reset/reconstruction and ChatView streaming layout, so it supports a
small painter observation only. It does not support general input latency, CPU,
allocation or comparative TypeScript-harness claims.

The direct review probe was compiled against the candidate release static
libraries, with the configured Clang 23 and explicit MacOSX15.5 SDK. The first
standalone compile omitted that SDK and failed; those compiler diagnostics are
not product findings. No operator session was stopped or changed. No changes to
production/test source were made by this reviewer. Fix valid findings and run
their targeted direct checks within the operator's two-layer bound; this report
does not request another review round.
