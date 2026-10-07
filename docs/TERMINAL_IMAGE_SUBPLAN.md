# Display the supplied SR-71 directly

Operator correction2026-10-07: use the supplied artwork, do not redraw it.
The two supplied attachments are PNGs, including the ASCII-looking version.
Store the first exact PNG in assets/sr71.png. Native terminal image placement
preserves it: no decoder/library in the harness, no pixel data compiled into it.

Use the Kitty protocol supported by Ghostty and Kitty. Known local terminals
(default detection) or explicit BLACKBIRD_GRAPHICS=kitty select it; graphics=off
and remote/multiplexed sessions retain text rendering. Transfer the local filename
once (t=f,f=100,q=2); place/resize the cached image only when geometry changes.
Always preserve the cursor; suppress replies; erase only our named image. Merge
commands into the existing single-writer, synchronized frame. Clean up on terminal
handoff/exit and re-upload after editor return. The fallback keeps block lettering
and the textual status; stop showing the invented startup airplane.

Startup gets a large square beside lettering or under it; standard80x24 fits.
On first input the same image moves to a12x6-cell corner square with a state-colored
indicator in the existing gutter. Text never flows beneath that gutter. No new
animation/timer or repeated PNG transfer per frame. Board uses its own image ID.

Direct checks: exact asset bytes, protocol framing/path/quiet/cursor preservation,
actual launcher PTY at80x24 and120x40, relocation on first input, erase on resize
and editor/exit. One bounded adversarial review of the change.

References:
- https://sw.kovidgoyal.net/kitty/graphics-protocol/
- https://ghostty.org/docs/features
