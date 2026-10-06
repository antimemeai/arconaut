# Arconaut terminal mascot

2026-10-06. Integrate operator-supplied arconaut.png unchanged. Its exact three-color
10px grid is represented as 16x24 logical pixels, rendered with Unicode half blocks
and ANSI foreground/background colors using the terminal's existing renderer.
No production image decoder, image protocol, or new dependency. Native terminal
source and its PTY resize/cursor/scroll checks are the implementation reference.

Reserve an 18-column right margin only at >=90 columns and >=20 rows. Keep original
transcript height; title, status, transcript and composer use the remaining width.
Overlay mascot in that margin, then restore composer cursor. Smaller terminals hide
it and retain the existing layout. Busy state alternates a one-pixel bob and subtle
white-plume brightness every500ms; idle is static. Changes are display-only, not
context/audit/provider input. No independent animation thread or provider traffic.

Verify actual PTY frames show original sprite, change while a local tool runs,
settle when idle, and disappear on narrow resize; existing cursor/scroll/cancel/RRC
checks remain. Inspect rendered output visually. Review concrete rendering/layout
code with Kimi, integrate valid findings; affected terminal debug/release checks.
