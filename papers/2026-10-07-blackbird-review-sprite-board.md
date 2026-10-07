# Sprite and board adversarial review — 2026-10-07

One bounded code-review round. Scope: the original two-row SR-71 encoder, native
header insertion, the new ChatGrid column-offset blit, board rename/sprite/fleet
changes, and the blackbird-board alias. Settled renderer behavior was not reopened.
No product edits, provider calls, fleet launches, or operator session mutations.

## Findings

1. **Medium — offset blit can violate wide-cell representation.**
   `src/chat_view.cpp:388–415` writes a new cell over a prior continuation without
   clearing the wide leader at its left. Reproduction linked against the current
   native libraries: reset a 6×2 grid; `line(0, {{"界", Ink::normal}})`;
   `line(0, {{"X", Ink::normal}}, 1)`. Cell widths become `2,1,1`, and the painter
   emits `界X    `, seven terminal columns for the six-cell row. Overwriting a
   wide leader with a narrow cell also leaves its old continuation unresolved.
   The current reserved header rectangle prevents this collision for the mascot,
   but the newly exposed general offset API does not uphold its cell invariant.
   Clear both halves of every existing wide glyph intersecting the incoming blit,
   then place the replacement. Direct checks should cover writing into a tail and
   replacing a leader; no new renderer review round is required.

2. **Low — board activity frame period aliases its idle refresh interval.**
   `codex-tools/scripts/arco-board.lua:389` samples `os.time()%3`; the watch loop
   configures `time 30` at line 516, a three-second terminal read timeout. With
   quick collections, ordinary unattended refreshes repeatedly sample the same
   phase. The sprite then appears frozen while work is active, advancing mostly
   when input or collection latency shifts the phase. Advance a draw counter for
   successive observed-active refreshes, or select a phase/cadence that cannot
   alias the refresh period. Do not solve this by tripling expensive data polling
   just to animate a tiny exhaust.

3. **Low — header and fleet disagree on attached-supervisor activity.**
   New header logic at board line 401 animates an observed `running` supervisor
   or worker. Existing `fleet_state()` treats an observed `attached` supervisor
   as UNDER WAY as well; the new fleet sprite therefore animates that job while
   the header remains static if no separate worker is observed. Reuse one activity
   predicate or explicitly distinguish the meanings. In either case, observation
   proves process identity/presence, not model progress; retain that distinction.

## Checks and limits

The encoder indices are bounded by fixed 36×8 input, 18 columns, 2 rows and the
four-by-two Braille dot map. The C++ static assertion checks every silhouette row
width; frame selection is modulo three and idle selection is constant. UTF-8
encoding is restricted to U+2800–U+28FF. Native insertion reserves 20 columns,
starts at width−18, and uses the existing two header rows, leaving chat/composer
height intact. Narrow layouts omit it. Frames are cached once; idle redraws do
not advance the native mascot.

The board inserts trusted Braille directly rather than into its source-text
ASCII clipping helper; coordinates leave the terminal's final column unused.
Timestamp/title/header reservation arithmetic is nonoverlapping at the 96-column
threshold. `blackbird-board` is a relative symlink to `arco-board.lua`, preserving
one implementation and old command compatibility. No out-of-bounds access or
allocation-growth defect was found in the new sprite encoder/insertion paths.

The direct native overlap probe is the evidence for finding 1. Findings 2 and 3
follow directly from phase/timeout and branch predicates; no live fleet was
started to manufacture a demonstration. Artistic recognizability and terminal
font legibility remain operator visual judgments, not memory-safety claims.
