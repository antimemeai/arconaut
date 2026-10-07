# Supplied SR-71 terminal-image review

One bounded adversarial source review of the uncommitted native integration and
external board diff. Scope: exact supplied PNG, Kitty local-file transfer, frame
ownership, placement bounds, cache lifetime, and terminal handoff. No production
edits or second review. Protocol grounding: [Kitty graphics protocol](https://sw.kovidgoyal.net/kitty/graphics-protocol/).

Two concrete native defects were sent to the implementer:

- The compact-frame branch (`rows < 8 || columns < 12`) bypassed image lifecycle
  entirely. Shrinking from a displayed image left its placement visible over the
  compact composer. Erase the named placement in that branch and reset placement
  geometry while retaining loaded pixel storage. The implementer reports this
  fixed; its direct PTY resize check should exercise a shrink below eight rows.
- The inherited `width >= 90` guard removed the PNG after first input at standard
  80x24 rather than moving it to the promised upper-right square. A graphics-only
  lower width threshold can reserve the 14-column gutter and retain a useful
  chat width. The implementer is changing the threshold to 70. Check startup and
  first-input relocation at both 80x24 and 120x40.

The screen oracle also skipped DEC save/restore cursor commands (`ESC 7`/`ESC 8`).
It could not substantiate a graphics-mode composer-cursor check. The implementer
will add those state transitions before the graphics PTY checks. This concerns
its direct cursor oracle, not a second-order test certification layer.

Other inspected mechanics are coherent: PNG data remains outside the compiled
program; the filename is base64 encoded; `q=2` suppresses replies; named-image
lowercase deletion retains cached data while uppercase releases it; `C=1` plus
DEC save/restore preserves the cursor. Native image commands join the existing
synchronized packet. `image_frame` outlives its `ChatOutput` string view, and
redrawing is blocked while output is pending. Editor handoff drains output before
release, and editor return resets cache/placement before the next full paint.
The startup placement math keeps its square above the composer and palette.
Cleanup targets only image 72171.

The board uses image 72172, base64 filename transfer, one cached load, bounded
12x6 placement, and release on normal/error exit. Its header has one aircraft
placement, so `p=1` does not collide with additional cards. Its existing full-screen
clear necessitates erase/replacement on each redraw; no new refresh timer or PNG
transfer cadence was added. Narrow redraw erases the previous image before
returning. No additional concrete defect found in this board diff.

Qualification is bounded to the source and direct protocol/PTY cases above.
This report does not claim that a PTY decodes PNGs or qualifies every terminal.
