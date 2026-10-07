# Inline slash menu and terminal frame

2026-10-07: operator supplies actual Ghostty screenshot and requests implementation
now. Deliver automatic slash discovery with arrows/filter/Tab/Enter, preserving
modal palette, paste, history and cancellation. Add composed borders, readable
selected rows, compact title and adaptive composer. This is a concrete first
slice of TUI_PLAN, not completion of Markdown/tool/animation work.

Direct oracles: composer tests for exact/selected dispatch, argument fill, Escape,
literal paste and restored draft; existing terminal PTY test for interactive
regressions. Build release and install local launcher binary. Inspect an actual
PTY frame for borders/menu, cursor and physical bounds. No provider request needed.
