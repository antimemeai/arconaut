# Blackbird's SR-71

Startup shows ASCII block lettering and a large front-three-quarter aircraft,
facing down-left, with twin nacelles, fins and swept wings. The geometry follows
the operator's supplied visual reference. Both drawings live in src/sprite.cpp.
The layout puts lettering beside the aircraft on wide screens and above it on
tall screens; small terminals keep the lettering. Any operator keystroke reveals
the retained chat. Automatic restart/resume/continue proceeds directly to chat.
Startup art is presentation only and never enters model context or audit history.

The live avatar is a 24x24 bit drawing encoded as 12x6 Braille cells, approximately
square with a normal 1:2 terminal font. A reserved right gutter prevents overlap
with chat/status. Below 90 usable columns, or when the avatar cannot fit above
the composer/palette, it disappears. It carries live state:

- Muted: idle.
- Bright cyan: active turn/model work.
- Amber: active tool.
- Red: failed turn, until the next turn starts.

Active work animates two exhaust trails on the existing clock. Idle has no mascot
timer or redraw. Four cached UTF-8 frames; no raster asset, decoder, graphics
protocol or new library. `BLACKBIRD_MASCOT=0` hides the aircraft in startup and chat
while preserving block lettering.

The external board uses the same square geometry, with observed fleet activity
in cyan, paused admission in amber, inactive blocked work in red, and idle muted.
It retains explicit text labels alongside those colors.

Actual terminal cell captures:
[startup](../papers/2026-10-07-blackbird-startup.svg),
[idle avatar](../papers/2026-10-07-blackbird-avatar.svg).
