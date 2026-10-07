# Blackbird's SR-71

The sprite is [the exact supplied PNG](../assets/sr71.png), preserved byte-for-byte.
The second supplied attachment looked like ASCII, but was also a PNG. The earlier
hand redraw is no longer the startup artwork.

Ghostty and Kitty display the supplied image directly. At startup it is large,
alongside ASCII BLACKBIRD lettering or below it on narrower screens. Standard
80x24 terminals show it. First operator input moves the cached image into a
12x6-cell square in the upper-right corner. Retained chat stays intact; the startup
presentation never enters model context or audit history. Automatic continuation
proceeds directly to chat.

The indicator beside the image carries state: muted idle, bright cyan active turn,
amber active tool, red failed turn. Text wraps outside its gutter. Short terminals
hide the image instead of overlapping the composer or command palette.

The native renderer sends the local filename once through the Kitty graphics
protocol. The terminal decodes/caches it; the harness has no image decoder or
pixel buffer. Moving/resizing sends placement commands, not PNG data. Cursor
save/restore, quiet commands, named-image deletion and the existing synchronized
single-writer frame preserve ordinary terminal behavior. Editor handoff/exit
release the image; editor return reloads it.

Known local Ghostty/Kitty terminals select this path automatically.
`BLACKBIRD_GRAPHICS=off` selects text rendering; `BLACKBIRD_GRAPHICS=kitty` explicitly
selects local-file graphics. Automatic selection is disabled over SSH and terminal
multiplexers because filesystem/escape transport needs its own treatment.
`BLACKBIRD_SPRITE_ASSET=/absolute/path.png` selects another local asset.
`BLACKBIRD_MASCOT=0` hides the aircraft while keeping lettering.

The external board uses the same asset and its own image ID. Observed fleet
activity is cyan, paused admission amber, inactive blocked work red, idle muted;
explicit status text remains alongside these colors.

Protocol sources: [Kitty specification](https://sw.kovidgoyal.net/kitty/graphics-protocol/)
and [Ghostty features](https://ghostty.org/docs/features).
