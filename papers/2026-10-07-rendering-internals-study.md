# Renderer internals: source findings and proposed application

2026-10-07. Operator requires serious reference-driven rendering work: C-programmer
economy, critical-systems failure discipline, buttery motion and no filler. This
report examines concrete source paths; it does not qualify unbuilt Arco behavior.

## Source findings

**Notcurses, C**, pin `b26048eebc74d5d254717d3332fa484718f9efe6`.
`src/lib/render.c:postpaint_cell` compares composited cells against lastframe,
marks damage, and accounts for trailing cells of wide glyphs. Resize restripes the
framebuffer and releases displaced grapheme storage. `src/lib/egcpool.h` keeps short
graphemes inline and spills longer ones into pooled storage addressed by offsets;
storage is reusable and bounded. `src/lib/fbuf.h` reuses contiguous byte capacity,
grows geometrically, and checks overflow. Its finalization uses a blocking write:
that is source evidence for batching, not a solution to our input-latency problem.
We do not need its entire plane/media/compositor system to learn these techniques.

**libvterm, C**, pin `934bc2fbf21800ac3458a499df8820ca5fb45fd3`.
`src/screen.c` uses contiguous screen cells, checked row/column lookup, lazily
allocated alternate storage, reusable scrollback-row storage and explicit damage
rectangles. `damagerect` merges row damage or larger rectangles according to policy;
pending scroll state is distinct from ordinary damage. It is a terminal emulator,
not a Markdown viewer or a terminal-byte emitter. Its fixed-size character array
is not permission to silently truncate modern Unicode clusters in Arco.

**FTXUI, C++**, pin `d807cbca267a5ed994604439efa6a4b8cb411f54`.
`src/ftxui/component/app.cpp:Draw` skips valid frames, sizes the cell buffer on
resize, computes layout, renders and writes via `Screen::ToString`. The latter
walks every screen cell, tracks style transitions and skips continuation columns
of full-width characters. It is a useful reference for canvas/easing/style handling;
its whole-screen serialization is not our intended frequent-update path.
`src/ftxui/dom/canvas.cpp` maps Braille and block masks to cell graphics.

**Codex, Rust**, existing pin in terminal-ux-study.
`history_cell/markdown_render_cache.rs` keys cache validity on width, list spacing,
theme revision, terminal colors and color level. Its return path clones rows:
learn invalidation, measure copying, do not copy the tradeoff uncritically.
`history_cell/activity_group.rs` preserves call positions through completion.

**Oh-my-pi, TypeScript**, existing pin in terminal-ux-study.
`packages/tui/src/components/markdown.ts` bounds its LRU by aggregate size, entry
size and count, counting source-bearing keys as well as rows. Streaming tests
compare every prefix to a cold render with shared cache bypassed, preventing the
oracle from reading the same wrong cached result. Tool-preview tests reject late
updates after final results. These are useful direct tests regardless of runtime.
The source also documents quadratic streaming lexer paths: language choice alone
does not eliminate algorithmic waste.

## Arco's present cost centers

`TerminalUI::run` wraps the flat transcript on each redraw, formats a whole frame,
clears rows and flushes through std::cout. Each input batch requests a redraw.
Status-driven redraws currently repeat conversation work. Composer text and cursor
prefix are independently wrapped. Unicode decoding/cell width computation is
repeated during layout. Provider prose, tool bytes and diagnostics share a string,
preventing targeted invalidation. There is no measured CPU/allocation baseline in
this unit; these are identified source operations, not invented timings.

## Proposed fast path

1. Producers append typed events/source revisions. Audit retention happens in its
   existing owner; UI updates carry IDs/spans, not repeated entire histories.
2. Parse/reflow only changed semantic blocks. Decode UTF-8 once per changed span,
   preserve cluster boundaries and source mapping, cache widths. Maintain explicit
   dependencies where Markdown can restyle earlier content.
3. Reusable contiguous viewport cells; compact glyph/style identifiers with a
   cheap ASCII path and pooled complex clusters. No string or heap object per cell.
   Explore a 16-byte cell; two 120×40 buffers would then occupy 153,600 bytes,
   excluding pools/maps/caches. This is arithmetic for a candidate, not a measured
   or fixed ABI claim. Record actual sizeof and all retained capacities.
4. Track damaged row intervals; compare only affected cells, extending damage to
   complete old/new wide-glyph footprints. Group adjacent output spans and retain
   current cursor/style state to avoid redundant escape sequences. Pick cursor
   movement versus writing a short unchanged gap by actual byte cost.
5. Reuse a bounded output buffer. Submit complete escape/text units with a saved
   offset across short writes; handle EINTR/EAGAIN. Do not block input indefinitely
   on a slow terminal. A partially emitted frame drains safely before a replacement
   delta; only entirely unstarted obsolete frames can be discarded.
6. Advance the assumed terminal state after successful output. Failure invalidates
   that assumption; recover with a full repaint from the current projection. Never
   treat a failed write as a presented frame. Physical display acknowledgement is
   unavailable: measured write completion is not photon latency.

No event-loop heartbeat during settled idle: wait for input, producer notification
or the next active effect deadline. Input/stop has priority over optional effects.
Batch streaming bursts at an explicit frame cadence (candidate 60Hz maximum);
activity indicators can run slower. Under overload skip obsolete presentation
frames, not original audit bytes or semantic final state. Reuse capacity, but cap
retained capacities and release pathological peaks at safe boundaries.

## Critical-systems lessons applied narrowly

Make state transitions, byte ownership, capacities and failure outcomes explicit.
Use checked size arithmetic, generation-tagged references and typed lifecycles.
No cache is authoritative. A bounded cache miss takes a correct slower path.
Source remains available if rich parsing fails; basic text remains available if
optional features fail; full repaint recovers screen-state uncertainty. Terminal
handoff restores cursor, input flags and optional modes on normal/error exits.
Application recovery cannot guarantee terminal cleanup after SIGKILL; startup
must establish its modes deliberately.

Preserve a full-paint reference path as the direct correctness oracle for the
incremental renderer. Exercise partial writes, backpressure, Unicode wide-to-narrow
replacement, resize/collapse during output, interrupted frames and replay. This is
one independent comparison mechanism, not a stack of certification wrappers.
The product remains one authoritative state machine with derived presentation.

Benchmark workloads must include typing while a tool floods output, long retained
history, narrow/wide resize, incomplete Markdown, scrolling while streaming and
motion-only ticks. Record CPU time, allocations, touched cells, emitted bytes,
syscalls and input-to-write latency. Compare the current Arco baseline and new
renderer on identical captured workloads. Cross-harness measurements require
matched workloads and settings; do not claim superiority from language or demos.

## Acquisition and adoption boundary

Intact source archives and clean extraction/restoration are recorded in
`2026-10-07-render-reference-acquisition.json` and
`2026-10-07-chat-native-reference.json`. No references executed or libraries adopted.
Notcurses/FTXUI demonstrate techniques, not a selected production dependency.
libvterm could be considered as independent development-only screen oracle, but
that would require the library-adoption discussion and is not selected here.

The presentation and renderer proposals are combined in
[CHAT_PRESENTATION_PROPOSAL.md](../docs/CHAT_PRESENTATION_PROPOSAL.md).
