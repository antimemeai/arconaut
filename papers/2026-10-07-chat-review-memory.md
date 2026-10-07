# Rich chat tribunal: native storage, Unicode, and work

2026-10-07. One requested adversarial review round. Read-only review of the
working tree; this report is the only file written. No provider requests, secondary
reviewers, code edits, or certification exercise. Findings below follow explicit
source counterexamples; these counterexamples were not compiled or executed in
this reviewer lane. Locations refer to the reviewed working tree.

Read the repository instructions, Blackbird doctrine, README, recent journal,
`include/arconaut/chat_view.hpp`, all of `src/chat_view.cpp`,
`tests/chat_view_test.cpp`, and the terminal/runtime integration diff.

## Findings requiring remediation

### M1 — P1: Code wrapping can split a UTF-8 scalar

`src/chat_view.cpp:96–129` (`syntax`) emits a separate one-byte span for every
non-ASCII byte outside quoted strings. `wrap`, lines 144–160, calls `rune` on each
span independently. A partial UTF-8 sequence therefore gets width one per byte,
instead of the scalar's real width. Adjacent same-style spans are coalesced into
output strings, but that does not repair the wrap decisions: a boundary can split
the scalar between rows. `ChatGrid::line` then displays replacement question marks.

Concrete counterexample: append assistant text `````cpp\n// 界\n``` `` and request
`rows(8)`. The content budget is four cells. The three ASCII characters consume
three cells, the first byte of 界 consumes the fourth, and its remaining bytes
move to a second row. Actual input contains one valid CJK scalar; the resulting
grid does not. Existing CJK tests bypass Markdown, syntax, and wrapping entirely.

Minimal remedy: never emit spans ending inside a UTF-8 scalar; scan/coalesce
ordinary code runs and measure decoded scalars. Add a narrow fenced-code test
checking both exact original text preservation and cell footprints.

### M2 — P1: Layout and grid disagree about grapheme widths

`src/chat_view.cpp:151–160` wraps by individual `wcwidth` values, while
`ChatGrid::line`, lines 319–353, combines ZWJ, regional indicators, modifiers,
variation selectors, and keycap sequences. Different width models cannot produce
a coherent screen. Layout can insert a line break inside a grapheme and therefore
change its visible glyph. Variation selectors can also increase the grid footprint
after layout has already declared that the row fits, clipping following text.

Counterexample: prose `aa👩‍💻x` with `rows(8)`. The first woman scalar fits the
four-cell content budget; the laptop scalar triggers a wrap after the ZWJ. The
single intended emoji becomes two pieces on different lines. Flag pairs have the
same class of defect. Existing direct-grid tests demonstrate clustering only
after layout has already completed; they do not exercise this failure.

Minimal remedy: one bounded cluster iterator/width policy used by wrap and grid;
never divide a cluster at a row or style boundary. Test a cluster just before, at,
and beyond a narrow row edge. Leading unattached combining marks need an explicit
visible replacement/dotted-circle policy rather than pretending they occupy one
cell while emitting a zero-width terminal glyph.

### M3 — P1: Valid display input can terminate the harness

`src/chat_view.cpp:325–326` throws when a cluster exceeds 4096 tail bytes. A model,
tool, or operator can supply `a` followed by 2049 U+0301 combining accents. This
input is valid UTF-8 and well below the one-MiB source limit. The throw occurs on
the UI thread during `grid.line`, outside the worker's catch boundary, so a display
limit becomes an application-ending exception. It must not kill a running turn.

Minimal remedy: enforce the cluster bound as a display policy, with a visible
replacement or truncated cluster and an omission indicator; retain original text
in audit. The same cluster policy must run before wrapping. A direct over-bound
cluster test should check bounded storage and continued rendering of the next
ordinary character.

### M4 — P2: Repeated line updates preserve obsolete grapheme tails

`src/chat_view.cpp:342–345` assigns scalar, ink, and width without clearing tail or
length. On the same grid, `line(0, {{"é", ...}})` followed by
`line(0, {{"h", ...}})` leaves the acute accent attached to `h`. This is relevant
to the advertised reusable grid and the motion tests, which call `line` repeatedly
without reset. Current full-frame reset in TerminalUI hides the defect there.

Minimal remedy: replace the destination cell completely when placing a new
cluster; define what happens to old wide-glyph continuation cells on narrower
replacement. Test Unicode-to-ASCII and wide-to-narrow updates, not only ASCII
motion. This is a concrete API defect, not an immediate crash in the current
reset-every-frame integration.

### M5 — P1: Source bound does not bound useful render work or display storage

`ChatView::rows`, lines 264–293, restarts from the last newline. A long unfinished
line is sanitized, allocated, reparsed, and rewrapped in full on every streamed
append. Work is quadratic in a bytewise streamed line, even though the stable-line
cache works well for newline-delimited text. At the one-MiB source cap, every new
append invokes `trim` (lines 186–197), prefix-erases the string and resets width,
forcing a complete reflow on every batch. Tool output without newlines is an
ordinary workload, not exotic malformed input.

The byte limit also permits roughly one million newline rows. Each produces a
vector-owned rail span: at least `sizeof(ChatRow) + sizeof(ChatSpan)` per row,
before allocation overhead/capacity. The reviewed layout can thus expand one MiB
of input to tens of MiB and perform roughly one million allocations before
displaying a few dozen rows. No rendered-row/span budget or visible omission
marker exists. The current large-input test checks source size only; it never
calls `rows` on adversarial newline input.

Minimal first-pass remedy: explicit derived row/span budget and unfinished-line
budget with visible omission, plus chunked amortized source eviction instead of
prefix erasure on every token. For the normal path, preserve stable wrapped rows
and dirty only the final affected rows; reparse a bounded suffix where Markdown
delimiters can change interpretation. Do not claim arbitrary streamed-line work
is incremental on the present implementation. Measure this workload directly,
not just an ASCII spinner.

## Follow-up: performance limits, not an additional acceptance gate

The changed-cell output is useful and the 12-byte scalar/arena-tail cell is a good
base. However, every spinner frame still resets the whole grid, rebuilds all
visible rows, compares the entire screen, and copies the entire grid in `commit`.
Relevant locations: `src/terminal.cpp:1220` and subsequent `grid.line` calls;
`src/chat_view.cpp:306–308`, `ChatPainter::prepare`, and `ChatPainter::commit`.
Thus terminal output damage is incremental; CPU composition and screen comparison
remain O(screen cells) per animated frame. This is not inherently unacceptable at
120×40, but it must be described accurately. A future region-damage compositor
and swapping retained grids would remove redundant work; do not turn that into
another review round or block remediation behind a new rendering architecture.

Existing tests exercise typed roles, cold-versus-streamed ASCII Markdown, direct
CJK/combining/ZWJ/flag grids, delta bytes, pipe backpressure, and source size. They
do not cover the above layout/grid interactions or adversarial derived-size
expansion. Remediate the findings and do one direct recheck within the existing
bounded-hardening rule.
