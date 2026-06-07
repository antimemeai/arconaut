# Terminal Graphics Protocols — Exhaustive Research Report

**Date:** 2026-05-19  
**Context:** rocket_surgeon TUI intermission — evaluating every approach to rendering graphics and data visualization in a terminal for a Rust-based multi-GPU transformer debugger/surgery tool.

---

## 1. Character-Based Graphics

### 1.1 Unicode Block Elements (U+2580–U+259F)

The simplest approach to terminal graphics uses the 32 block element characters. The most important for visualization:

**Vertical eighth blocks (sparklines):** `▁▂▃▄▅▆▇█` (U+2581–U+2588) — 8 vertical levels per character cell. Used directly by ratatui's `Sparkline` widget with `NINE_LEVELS` bar set (includes empty as the ninth level). Effective vertical resolution: **8 levels per cell**.

**Half blocks:** `▀` (upper half, U+2580) and `▄` (lower half, U+2584). The critical trick: set the **background color** to one pixel's RGB and the **foreground color** to the other pixel's RGB, effectively encoding **two vertically-stacked pixels per character cell**. This doubles vertical resolution. On an 80x24 terminal, this yields 80x48 effective color pixels. This is the universal fallback used by ratatui-image's "halfblocks" backend and tools like TerminalImageViewer (tiv).

**Quadrant blocks:** `▖▗▘▙▚▛▜▝▞▟` (U+2596–U+259F) — divide a cell into four quadrants (2x2). Combined with fg/bg color, this gives 2x2 spatial resolution per cell but only 2 colors per cell.

**Effective resolution per cell:**
| Technique | Spatial | Colors per cell |
|-----------|---------|-----------------|
| Full block + color | 1x1 | 1 |
| Half block + fg/bg | 1x2 | 2 |
| Quadrant blocks | 2x2 | 2 |

### 1.2 Unicode Sextant Characters (U+1FB00–U+1FB3B)

Added in Unicode 13.0 (March 2020), the 64 sextant characters divide each cell into a **2x3 grid** of independently toggleable sub-blocks. Originally from TRS-80 "pseudopixel" / teletext / Minitel systems. Combined with fg/bg color, each cell encodes a 2x3 binary bitmap — 6 sub-pixels per cell. On an 80x24 terminal: **160x72 monochrome pixels**, or 160x72 with 2-color freedom per cell.

**Font support caveat:** Sextant characters are in the "Symbols for Legacy Computing" block and are NOT universally supported by terminal fonts. Noto Sans Symbols 2, JetBrains Mono, and some Nerd Font patches include them. ratatui added a `Marker::Octant` (from "Symbols for Legacy Computing Supplement") as an alternative to Braille with the same 2x4 resolution but denser visual appearance.

### 1.3 Braille Characters (U+2800–U+28FF)

The 256 Braille pattern characters encode a **2x4 dot matrix** per character cell. Each of the 8 dots is independently on or off, addressed as a 2-column x 4-row grid. The bit positions within the byte map to dot positions:

```
Dot 1 (bit 0)  Dot 4 (bit 3)
Dot 2 (bit 1)  Dot 5 (bit 4)
Dot 3 (bit 2)  Dot 6 (bit 5)
Dot 7 (bit 6)  Dot 8 (bit 7)
```

The character codepoint = `0x2800 + bitmask`. For example, all dots on = `0x2800 + 0xFF = 0x28FF` = `⣿`.

**Effective resolution:** On an 80x24 terminal, Braille yields **160x96 addressable dots** (binary — on/off only). Each dot is monochrome (the foreground color of that cell). This is the highest resolution character-based technique that is widely supported.

**ratatui integration:** The `Canvas` widget defaults to `Marker::Braille` for drawing arbitrary shapes (lines, circles, rectangles). The `Chart` widget can use `Marker::Braille` for datasets. The drawille library (Python) and its ports (Rust, Nim, Lua, Zig) popularized this technique.

**Limitation:** Braille dots are inherently binary (on/off) and monochrome per cell. You cannot have two different colors within one Braille cell. For heatmaps requiring color gradients, Braille is unsuitable — use colored half/full blocks instead.

### 1.4 Box Drawing Characters (U+2500–U+257F)

128 characters covering single-line, double-line, and heavy-line variants for horizontal, vertical, corners, T-junctions, and crosses. Essential for:

- **Tree visualization:** `├── branch` / `│   └── leaf` patterns for model architecture display
- **Table borders:** Light (`─│┌┐└┘├┤┬┴┼`), heavy (`━┃┏┓┗┛┣┫┳┻╋`), double (`═║╔╗╚╝╠╣╦╩╬`), and rounded (`╭╮╰╯`)
- **Layout frames:** Dividing the TUI into panes

ratatui's `Block` widget uses these natively with `BorderType::Plain`, `Rounded`, `Double`, `Thick`.

### 1.5 Mathematical Symbols, Arrows, Geometric Shapes

- **Arrows:** U+2190–U+21FF (← ↑ → ↓ ↔ ↕ ⇐ ⇒ etc.) — useful for flow visualization
- **Geometric shapes:** U+25A0–U+25FF (■ □ ▪ ▫ ● ○ ◆ ◇ ▲ △ etc.) — scatter plot markers
- **Mathematical operators:** U+2200–U+22FF (∀ ∂ ∃ ∅ ∇ ∈ ∏ ∑ etc.) — annotation
- **Miscellaneous technical:** U+2300–U+23FF — gauge/meter characters

### 1.6 Combining Characters for Overlays

Unicode combining diacritical marks (U+0300–U+036F, U+20D0–U+20FF) are zero-width characters that overlay the preceding base character. Theoretically usable for:

- Adding strikethrough, underline, or overline to visualization elements
- Overlaying marks on existing characters

**Practical verdict:** Terminal support for combining characters is inconsistent. Many terminals misalign combining characters, produce visual artifacts, or ignore them entirely. **Not recommended as a primary visualization technique.** The Kitty graphics protocol's Unicode placeholder mechanism (U+10EEEE) uses combining diacritics in a controlled way, but that is a protocol-level feature, not a general-purpose overlay technique.

---

## 2. Sixel Graphics Protocol

### 2.1 Protocol Mechanics (Byte Level)

Sixel is a DEC bitmap graphics format from the 1980s (VT240/VT340 terminals). It encodes images as sequences of ASCII characters within a Device Control String.

**Entry sequence:** `ESC P p1;p2;p3;q` (or `0x90 p1;p2;p3;q`)
- `ESC P` = DCS (Device Control String)
- `p1` = pixel aspect ratio (deprecated; typically 0)
- `p2` = background select (0 = background color, 1 = keep current, 2 = device default)
- `p3` = horizontal grid size (typically omitted)
- `q` = sixel mode identifier

**Color register definition:** `#N;T;R;G;B`
- `N` = register number (0–255 typical, up to 1024 on xterm)
- `T` = type: `1` for HLS, `2` for RGB
- `R;G;B` = component values, each 0–100 (percentage, NOT 0–255)

Example: `#0;2;100;0;0` = register 0, RGB, pure red.

**Pixel data encoding:** Each character encodes a **column of 6 vertical pixels** (a "sixel"). The 6 bits map to 6 vertical pixels (top = bit 0, bottom = bit 5). The bit pattern is offset by 63 (ASCII `?`), so:
- `?` (63) = all 6 pixels off (000000)
- `~` (126) = all 6 pixels on (111111)
- `@` (64) = bottom pixel only (000001)

**Row advancement:** `$` returns to the start of the current 6-pixel-high strip. `-` advances to the next strip.

**Repeat:** `!N<char>` repeats `<char>` N times (run-length encoding).

**Termination:** `ESC \` (String Terminator) exits sixel mode.

**Multi-color rendering:** Within one 6-pixel strip, you select a color register with `#N`, draw that color's pixels, then `$` (carriage return within strip), select the next color, and draw its pixels. Colors composite via overwrite within the strip.

### 2.2 Resolution and Color Limitations

- **Resolution:** Pixel-level, limited only by terminal window size in pixels. A 1920x1080 terminal window can theoretically display a 1920x1080 sixel image.
- **Color registers:** Typically 256 (palette-based). xterm supports up to 1024. Some terminals default to only 16 until reconfigured. WezTerm supports 256+ with palette optimization. This is the critical limitation: sixel is **palette-based**, not truecolor.
- **Query:** Terminals supporting XTSMGRAPHICS can report their color register count. Fallback assumption: 256.

### 2.3 Terminal Support (2025–2026)

| Terminal | Sixel Support | Notes |
|----------|--------------|-------|
| xterm | Yes (since patch #359, 2018) | Up to 1024 colors; must enable with `-ti vt340` |
| mlterm | Yes (since 3.1.9) | Full, 256 colors |
| WezTerm | Yes (from initial release) | Dynamic palette, 256+ effective colors |
| foot | Yes | Native Wayland terminal |
| Contour | Yes | |
| Rio | Yes (~2024) | Rust-based, hardware-accelerated |
| Windows Terminal | Yes (Preview 1.22+) | Recent addition |
| DOMTerm | Yes | |
| tmux | Yes (compile with `--enable-sixel`) | Official since ~2023; previously required forks |
| **Kitty** | **No** | Uses its own graphics protocol instead |
| **Ghostty** | **No** | Uses Kitty graphics protocol |
| **Alacritty** | **No** (community fork only) | alacritty-sixel fork exists |
| **iTerm2** | Partial (natively uses OSC 1337) | Some sixel support added |

Reference: https://www.arewesixelyet.com/

### 2.4 Performance Characteristics

- **Bandwidth:** Sixel is verbose. A 256-color 800x600 image can be 500KB+ of ASCII escape sequences over the PTY. For local terminals, PTY bandwidth is rarely a bottleneck, but for remote sessions (SSH) it can be.
- **Rendering:** The terminal must parse ASCII, decode the sixel raster line by line, and composite colors. GPU-accelerated terminals handle this faster.
- **Compared to Kitty:** Over the PTY wire, Sixel data is typically larger than base64-encoded raw pixel data (Kitty protocol). However, Sixel passes through multiplexers and SSH more naturally because it is just escape sequences.

### 2.5 Interaction with Text

Sixel images are rendered at the current cursor position and advance the cursor. They **cannot** coexist with text in the same cell — the image overwrites the cell. You cannot overlay text on a sixel image without re-rendering. This is a fundamental limitation for TUI frameworks that need text labels on top of visualizations.

### 2.6 tmux Sixel Support History

The history is rocky:
- **Pre-2023:** No official support. Multiple community forks: `sixel-tmux` (csdvrx), `tmux-sixel` (ChrisSteinbach). The sixel-tmux project documented extensive frustrations with tmux maintainer resistance.
- **~2023:** tmux added official sixel support behind `--enable-sixel` compile flag.
- **Current:** Works when compiled with the flag, but still has edge cases. Images can cause display artifacts when scrolling or resizing panes. The Kitty protocol's Unicode placeholder approach was specifically designed to solve this class of problems in multiplexers.

---

## 3. Kitty Graphics Protocol

### 3.1 Overview

Developed by Kovid Goyal for the Kitty terminal. The most capable terminal graphics protocol as of 2026. Uses APC (Application Program Command) escape sequences: `ESC _ G <key=value pairs> ; <payload> ESC \`

### 3.2 Transmission Methods

**Direct (escape sequence, `t=d`):** Base64-encoded pixel data sent inline. Chunked into pieces no larger than 4096 bytes each. All chunks except the last use `m=1`; the final chunk uses `m=0`. Suitable for remote sessions.

**Temporary file (`t=f`):** Application writes pixel data to a temp file, sends only the path. Terminal reads and (optionally) deletes the file. The file path must contain `tty-graphics-protocol` and reside in a known temp directory for security. **Zero PTY bandwidth for pixel data.**

**Shared memory (`t=s`):** Application writes pixel data to a POSIX shared memory region, sends only the name. Fastest possible local transfer — **zero copy, zero encoding overhead.** This is why the claim "Kitty is infinitely faster than Sixel" exists: for local usage, the pixel data never traverses the PTY at all.

**Stream (`t=s` for remote):** For SSH, Kitty's `kitten icat --transfer-mode=stream` sends data through the PTY with base64 encoding, similar to direct mode but with Kitty's SSH integration handling the plumbing.

### 3.3 Image Placement

**Absolute placement:** Image placed at current cursor position, occupying specified rows/columns.

**Relative placement:** Image placed relative to a cell, with offsets.

**Virtual placement (`U=1`):** **[HIGH PRIORITY]** The image is not drawn directly. Instead, the application emits Unicode placeholder characters (U+10EEEE, a Private Use Area codepoint) into the terminal's text buffer. The terminal renders the image wherever those placeholder characters appear. This is the breakthrough feature:

- Placeholders move with the text buffer (scrollback, reflow)
- Work inside tmux (because they are just Unicode characters to the multiplexer)
- Can be mixed with text — the image is "behind" the text grid
- ratatui and other TUI frameworks can treat image placement as text cell operations

**Unicode placeholder mechanics:** The character U+10EEEE is written into cells. Combining diacritics encode the row/column within the image that this cell should display. The **foreground color** of the cell encodes the image ID. The terminal maps these to the previously transmitted image data and renders the appropriate image fragment in each cell.

### 3.4 Animation

Added in Kitty 0.20.0. Two animation-specific action modes:

- `a=f` — transmit frame data (supports delta encoding for efficiency)
- `a=a` — control animation playback (start, stop, set frame, set loops)

Frames can be specified as deltas from a base frame, with rectangular regions that differ being transmitted. This makes animation bandwidth-efficient for typical use cases (e.g., updating a small region of a visualization).

### 3.5 Memory Management

- Each image has a numeric ID (32-bit) and optional "number" for client-side tracking
- Images are stored in a disk cache to minimize memory usage
- When the terminal runs out of quota for new images, images without active placements are deleted first
- Explicit deletion commands: delete by ID, delete by number, delete all, delete by cell position, delete by z-layer
- Client can query which images are still alive

### 3.6 Terminal Support

| Terminal | Kitty Graphics | Notes |
|----------|---------------|-------|
| Kitty | Full (originator) | All features including animation, virtual placement |
| Ghostty | Yes | Full support; Mitchell Hashimoto confirmed via libghostty |
| WezTerm | Yes | Also supports Sixel and iTerm2 |
| Konsole | Partial | Basic support |
| **xterm** | **No** | |
| **Alacritty** | **No** | |
| **iTerm2** | **No** | Uses its own protocol |
| **Windows Terminal** | **No** | Only Sixel |

### 3.7 Performance vs Sixel

| Dimension | Kitty | Sixel |
|-----------|-------|-------|
| Local transfer | Shared memory / file — near zero overhead | Always PTY (ASCII encoded) |
| Remote transfer | Base64 over PTY (chunked) | Native escape sequences over PTY |
| Color depth | Full 32-bit RGBA | Palette-based (256 typical) |
| Multiplexer compat | Unicode placeholders work in tmux | Native (escape sequences pass through) |
| Text integration | Unicode placeholders coexist with text | Overwrites text cells |
| Animation | Native support | Not supported |
| Adoption (2026) | ~6 terminals | ~15 terminals |

---

## 4. iTerm2 Inline Images (OSC 1337)

### 4.1 Protocol Details

**Escape sequence:** `ESC ] 1337 ; File = [args] : <base64 data> BEL`

**Arguments (semicolon-separated key=value):**
- `name=<base64 encoded filename>` — optional
- `size=<integer>` — file size in bytes (helps with progress)
- `width=<spec>` — auto, N (cells), Npx (pixels), N% (percent of terminal width)
- `height=<spec>` — same format as width
- `preserveAspectRatio=0|1` — default 1
- `inline=1` — display inline (vs download)

### 4.2 Encoding Overhead

All image data is base64-encoded, adding ~33% overhead. Unlike Kitty, there is no file/shared-memory bypass. For a 1MB PNG, the escape sequence payload is ~1.33MB. iTerm2 v3.5 added chunked transfer for tmux integration mode, splitting the sequence into smaller pieces.

### 4.3 Format Support

Any image format supported by macOS (PNG, JPEG, GIF, TIFF, BMP, PDF, PICT, HEIC, etc.) can be displayed inline. The terminal handles decoding. This is an advantage: you can send a compressed PNG directly rather than raw pixel data.

### 4.4 Terminal Support Beyond iTerm2

| Terminal | OSC 1337 Support |
|----------|-----------------|
| iTerm2 | Full (originator) |
| WezTerm | Yes |
| mintty | Yes |
| Terminology | Partial |
| Konsole | Partial |
| Tabby | Requested (issue #5687) |

### 4.5 Limitations

- No animation support
- No image management (no IDs, no deletion, no update-in-place)
- No virtual/placeholder placement — image at cursor position only
- macOS-centric design (format support depends on host OS image decoders)

---

## 5. Terminal Text-Based Visualization Techniques

### 5.1 Heatmap Rendering with Colored Unicode Blocks

**[HIGH PRIORITY for rocket_surgeon]**

The most practical approach for attention heatmaps:

**Full-block approach:** Each matrix element → one `█` character with foreground color set to the heatmap color. Resolution: 1:1 (one cell per value). A 32x32 attention matrix needs 32x32 cells = 32 columns x 32 rows. Fits easily in an 80x24 terminal if you use the full area.

**Half-block approach:** Use `▄` with fg/bg colors to encode **two vertically adjacent values** in one character cell. A 32x32 matrix needs only 32 columns x 16 rows. A 64x64 matrix needs 64 columns x 32 rows — still fits. A 128x128 matrix needs 128 columns x 64 rows — needs horizontal scrolling or downsampling on most terminals.

**UnicodePlots HeatmapCanvas approach:** Uses foreground and background terminal colors to turn every character into two color pixels. This is the Julia library's technique and is directly applicable in Rust.

### 5.2 Sparklines

ratatui's built-in `Sparkline` widget uses `▁▂▃▄▅▆▇█` (8 vertical levels + empty = 9 levels). Each data point → one character wide. A 4096-dim tensor histogram with 80 bins fits in 80 columns x 1–4 rows. The `SparklineBar` type allows per-bar styling (color coding by value or percentile).

Inline sparklines are the single most space-efficient visualization for time series and distributions.

### 5.3 ASCII/Unicode Art Graph Rendering

**inferno-flamegraph approach:** The `inferno` Rust crate renders flame graphs as SVGs, but the concept can be adapted to terminal output using colored blocks. Each stack frame → a colored rectangle of proportional width.

**graphs-tui crate:** Renders Mermaid and D2 diagrams in the terminal using Unicode/ASCII art, supporting flowcharts, state diagrams, and pie charts.

### 5.4 ratatui Native Capabilities

**Canvas widget:**
- Draws arbitrary shapes using Braille (default), Block, Dot, Bar, HalfBlock, or Octant markers
- `Marker::Braille` — 2x4 resolution per cell (160x96 on 80x24)
- `Marker::Octant` — 2x4 resolution with denser visual appearance than Braille (no visible bands between rows/columns)
- `Context::draw()` accepts any `Shape` trait implementor: `Line`, `Rectangle`, `Circle`, `Map`, or custom
- X/Y coordinate mapping from world coordinates to cell coordinates is built-in

**Chart widget:**
- Renders multiple `Dataset`s as line or scatter graphs
- Supports `Marker::Braille` for high-resolution line plots
- X and Y axis with labels, bounds, configurable styles
- Recent PR (#1466) allows Braille/Dot charts to overlap with Block markers

**Sparkline widget:** As described above.

**Block widget:** Container with borders (plain, rounded, double, thick), title, padding.

**Table widget:** Structured data display with column headers, row selection, scrolling.

**Gauge widget:** Progress bars / percentage indicators.

**What ratatui CANNOT do natively:** Pixel-level graphics, smooth color gradients within a cell, anti-aliasing. For these, you need a graphics protocol (Sixel/Kitty/iTerm2) or the ratatui-image crate.

### 5.5 ratatui-image Crate

**[HIGH PRIORITY]**

The `ratatui-image` crate (v10.0.6 as of Feb 2026) bridges ratatui and terminal graphics protocols:

- **Protocol auto-detection:** Queries terminal via environment variables and control sequences
- **Backends:** Kitty (with Unicode placeholders), Sixel, iTerm2, Halfblocks (universal fallback)
- **Halfblocks fallback:** Uses `▄` with fg/bg colors; works in ALL terminals; assumes 4:8 pixel ratio if font size detection fails
- **Font size query:** Queries terminal for font size in pixels to map image pixels to character cells
- **Two widget types:** `Image` (stateless/immediate-mode) and `StatefulImage` (adapts to render area, more robust)
- **Blacklisting:** v10.0.6 added blacklisting of kitty/sixel detection for terminals that falsely report support

---

## 6. Color as Visualization

### 6.1 Terminal Color Modes

| Mode | Colors | Escape Sequence |
|------|--------|-----------------|
| Basic | 8 | `ESC[30-37m` (fg), `ESC[40-47m` (bg) |
| Bright | 16 | `ESC[90-97m` (fg), `ESC[100-107m` (bg) |
| 256-color | 256 | `ESC[38;5;Nm` (fg), `ESC[48;5;Nm` (bg) |
| Truecolor (24-bit) | 16.7M | `ESC[38;2;R;G;Bm` (fg), `ESC[48;2;R;G;Bm` (bg) |

**Truecolor support** is near-universal in modern terminals (Kitty, Ghostty, WezTerm, iTerm2, Alacritty, Windows Terminal, GNOME Terminal, Konsole, foot, Rio). The only notable holdout is the Linux framebuffer console.

### 6.2 Perceptually Uniform Colormaps

For heatmaps, perceptual uniformity means equal numerical differences produce equal perceived color differences. The key colormaps:

- **Viridis:** Dark purple → blue → green → bright yellow. Perceptually uniform, colorblind-safe (deuteranopia, protanopia). The default recommendation.
- **Cividis:** Specifically designed for ALL forms of color blindness (deuteranopia, protanopia, tritanopia). Blue → yellow. More conservative than viridis.
- **Inferno:** Black → purple → red → orange → bright yellow. High dynamic range, excellent for attention patterns where you need to see both low and high values.
- **Plasma:** Blue → purple → red → orange → yellow. Similar to inferno but with more blue emphasis.
- **Magma:** Black → purple → pink → light yellow. Good for dark-themed terminals.

**Implementation:** These colormaps are defined as 256-entry lookup tables of RGB values. For Rust, the `colorgrad` crate provides these, or you can embed the lookup tables directly (they are just arrays of 256 RGB triplets, published as public domain data).

### 6.3 Foreground + Background = 2x Density

**[HIGH PRIORITY]**

Using the half-block character `▄` (U+2584):
- **Background color** = top pixel
- **Foreground color** = bottom pixel

This is the single most important technique for terminal heatmaps. It doubles vertical resolution while maintaining full truecolor. A 32x32 attention heatmap using this technique needs only 32 columns x 16 rows — easily fitting in a quarter of the screen while leaving room for labels, axes, and other UI elements.

### 6.4 Colorblind-Safe Palette Recommendations

For rocket_surgeon's multi-purpose visualization:

1. **Primary sequential:** Viridis or Cividis (safe for all common color blindness types)
2. **High-contrast sequential:** Inferno (when you need maximum dynamic range)
3. **Diverging:** Blue-White-Red with desaturated midpoint (mark zero-crossings in activations)
4. **Categorical (for layer/expert labels):** IBM Design palette or Okabe-Ito palette (8 colors, universally distinguishable)

---

## 7. Practical Recommendations for rocket_surgeon

### 7.1 Attention Heatmap (32x32 to 128x128)

**Recommended approach: Half-block with truecolor + viridis/inferno colormap.**

| Matrix Size | Cells Needed (half-block) | Fits 80-col? | Fits 120-col? |
|-------------|--------------------------|-------------|---------------|
| 32x32 | 32 cols x 16 rows | Yes | Yes |
| 64x64 | 64 cols x 32 rows | Yes (tight) | Yes |
| 128x128 | 128 cols x 64 rows | No — downsample or scroll | Yes (tight) |

For 128x128, options:
1. **Downsample** to 64x64 or 80x80 for overview, full resolution on demand
2. **Scrollable viewport** with ratatui's scrolling
3. **Kitty/Sixel graphics** for pixel-perfect rendering (but loses text integration)

**Implementation path:**
1. Build a `HeatmapWidget` using half-blocks + fg/bg truecolor as the primary renderer
2. Add viridis/cividis/inferno colormap lookup tables (256-entry RGB arrays)
3. Support axis labels and value annotation
4. Optionally upgrade to ratatui-image with Kitty Unicode placeholders for pixel-perfect mode

### 7.2 Tensor Value Distribution (Histogram, 4096-dim)

**Recommended approach: ratatui Sparkline + Chart widget.**

- **Quick view:** Sparkline with 80–120 bins, 1–4 rows high. Shows shape of distribution at a glance.
- **Detailed view:** ratatui `Chart` with `Marker::Braille` for the histogram envelope, or filled bars using block characters.
- **Statistics overlay:** Text labels for mean, std, min, max, percentiles alongside the chart.

For a 4096-dim tensor:
1. Compute histogram (80–120 bins) on the Rust side
2. Render as sparkline for inline/compact view
3. Render as Chart with Braille markers for detailed view with axes

### 7.3 Model Architecture Tree (32+ layers)

**Recommended approach: ratatui Tree widget with box-drawing characters.**

The `tui-tree-widget` crate (third-party ratatui widget) provides collapsible tree rendering. For a transformer with 32+ layers:

```
╭─ GPT-2 (12 layers, 117M params) ──────────╮
│ ├── Embedding                              │
│ │   ├── token_embedding [50257 x 768]      │
│ │   └── position_embedding [1024 x 768]    │
│ ├── Layer 0                                │
│ │   ├── LayerNorm (pre-attn) [768]         │
│ │   ├── MultiHeadAttention                 │
│ │   │   ├── Q [768 x 768] ◄ BREAKPOINT    │
│ │   │   ├── K [768 x 768]                  │
│ │   │   └── V [768 x 768]                  │
│ │   ├── LayerNorm (pre-ffn) [768]          │
│ │   └── FFN                                │
│ │       ├── fc1 [768 x 3072]               │
│ │       └── fc2 [3072 x 768]              │
│ ├── Layer 1 ... (collapsed)                │
│ ...                                        │
╰────────────────────────────────────────────╯
```

Use box drawing for structure, color for status (breakpoint = red, active layer = yellow, completed = green), and collapse/expand for managing the 32+ layer depth.

### 7.4 Time Series (Loss curves, activation norms)

**Recommended approach: ratatui Chart with Braille markers.**

- Multiple datasets (loss, gradient norm, activation norm) on the same Chart
- Braille markers give 160x96 resolution on an 80x24 canvas — smooth enough for curves
- Color-coded datasets with legend
- Auto-scaling Y axis with configurable bounds
- Sparkline mode for compact inline display in a status bar

### 7.5 Graceful Degradation Strategy

**[HIGH PRIORITY]**

rocket_surgeon should detect terminal capabilities at startup and degrade gracefully:

| Tier | Detection | Visualization |
|------|-----------|---------------|
| **Tier 1: Kitty protocol** | `TERM=xterm-kitty` or Kitty graphics query response | Full pixel graphics via Kitty protocol with Unicode placeholders; animations; ratatui-image StatefulImage |
| **Tier 2: Sixel** | XTSMGRAPHICS query or `DA` response | Pixel graphics via Sixel; static images; ratatui-image |
| **Tier 3: Truecolor** | `COLORTERM=truecolor` or 24-bit color query | Half-block heatmaps, Braille charts, full sparklines — the primary TUI experience |
| **Tier 4: 256-color** | 256-color detection | Same as Tier 3 but with quantized colormaps (256 palette entries from viridis/inferno) |
| **Tier 5: Basic** | Everything else | ASCII-only charts (`#` bars, `*` scatter), no color heatmaps, tree with `+--` instead of box drawing |

**The ratatui-image crate already implements this detection and fallback chain.** The recommendation is to use ratatui-image for any pixel-level graphics and implement the character-based visualization (heatmaps, sparklines, trees) directly in ratatui widgets, which naturally work at all tiers.

### 7.6 Summary Decision Matrix

| Visualization | Primary Technique | Pixel Upgrade | Fallback |
|---------------|-------------------|---------------|----------|
| Attention heatmap | Half-block + truecolor | Kitty/Sixel via ratatui-image | 256-color blocks |
| Tensor histogram | Sparkline + Chart/Braille | N/A (text is fine) | ASCII bars |
| Architecture tree | Box drawing + Tree widget | N/A (text is fine) | ASCII `+--\|` |
| Loss curves | Chart + Braille markers | Kitty/Sixel rendered plot | ASCII `*` scatter |
| Sparklines | ▁▂▃▄▅▆▇█ | N/A | ASCII `_.-'"` |
| Inline images | ratatui-image (auto-detect) | Kitty > Sixel > iTerm2 | Halfblocks |

---

## 8. Notable Libraries and Tools (Rust Ecosystem)

| Crate | Purpose | Relevance |
|-------|---------|-----------|
| `ratatui` | TUI framework | Core framework — Canvas, Chart, Sparkline, Block, Table |
| `ratatui-image` | Terminal image rendering | Kitty/Sixel/iTerm2/Halfblocks with auto-detection |
| `tui-tree-widget` | Collapsible tree widget | Model architecture display |
| `tui-bar-graph` | Bar graph widget | Histogram rendering |
| `graphs-tui` | Diagram rendering | Mermaid/D2 flowcharts in terminal |
| `colorgrad` | Color gradient generation | Viridis, inferno, etc. colormap generation |
| `notcurses` / `libnotcurses-sys` | Low-level terminal graphics | Capability detection, blitter hierarchy, sixel/kitty/halfblock |

---

## Bibliography

### Protocol Specifications
1. [Kitty Terminal Graphics Protocol — Official Specification](https://sw.kovidgoyal.net/kitty/graphics-protocol/)
2. [Kitty Graphics Protocol — GitHub Source](https://github.com/kovidgoyal/kitty/blob/master/docs/graphics-protocol.rst)
3. [iTerm2 Inline Images Protocol](https://iterm2.com/documentation-images.html)
4. [iTerm2 Proprietary Escape Codes](https://iterm2.com/3.2/documentation-escape-codes.html)
5. [All About SIXELs — VT100.net (Chris Siebenmann)](https://vt100.net/shuford/terminal/all_about_sixels.txt)
6. [Sixel — Wikipedia](https://en.wikipedia.org/wiki/Sixel)
7. [Sixel Protocol — Rio Terminal Documentation](https://rioterm.com/docs/features/sixel-protocol)

### Terminal Compatibility
8. [Are We Sixel Yet?](https://www.arewesixelyet.com/)
9. [Terminal Compatibility Matrix — TmuxAI](https://tmuxai.dev/terminal-compatibility/)
10. [Terminal Emulators Comparison Table (2026) — Terminal Trove](https://terminaltrove.com/compare/terminals/)
11. [Terminal Graphics Protocols: Kitty, Sixel, iTerm2, and Beyond — Akmatori Blog](https://akmatori.com/blog/terminal-graphics-protocols)
12. [State of Terminal Emulators in 2025 — Jeff Quast](https://www.jeffquast.com/post/state-of-terminal-emulation-2025/)
13. [Best Terminal for Mac in 2026 — DEV Community](https://dev.to/vibehackers/best-terminal-for-mac-in-2026-ghostty-kitty-wezterm-alacritty-warp-more-4pe6)

### Unicode Graphics Techniques
14. [Drawille — Pixel graphics in terminal with Unicode Braille characters](https://github.com/asciimoo/drawille)
15. [Graph Plotting in the Terminal Using Braille Characters — Lyngvaer](https://lyngvaer.no/log/graph-plotting-terminal-braille)
16. [ASCII Art, But in Unicode — Dernocua](https://dernocua.github.io/notes/unicode-graphics.html)
17. [(Almost) Square Pixels in the Terminal — uninformativ.de](https://www.uninformativ.de/blog/postings/2016-12-17/0/POSTING-en.html)
18. [TerminalImageViewer — Half-block image rendering](https://github.com/stefanhaustein/TerminalImageViewer)
19. [Unicode Block Drawing Characters Guide — SymbolFYI](https://symbolfyi.com/guides/box-drawing-characters/)
20. [Box-drawing Characters — Wikipedia](https://en.wikipedia.org/wiki/Box-drawing_characters)
21. [Unicode Sextant Characters — Symbols for Legacy Computing (U+1FB00)](https://www.unicode.org/charts/PDF/U1FB00.pdf)
22. [Unicode Combining Diacritical Marks (U+0300-U+036F)](https://www.unicode.org/charts/PDF/U0300.pdf)

### Ratatui Ecosystem
23. [ratatui — Official Documentation](https://ratatui.rs/)
24. [ratatui Canvas Widget Documentation](https://docs.rs/ratatui/latest/ratatui/widgets/canvas/struct.Canvas.html)
25. [ratatui Sparkline Widget Documentation](https://docs.rs/ratatui/latest/ratatui/widgets/struct.Sparkline.html)
26. [ratatui Chart Example](https://ratatui.rs/examples/widgets/chart/)
27. [ratatui Built-in Widgets Showcase](https://ratatui.rs/showcase/widgets/)
28. [ratatui Third-Party Widgets Showcase](https://ratatui.rs/showcase/third-party-widgets/)
29. [ratatui v0.30.0 Release Notes](https://ratatui.rs/highlights/v030/)
30. [ratatui-image — GitHub (Ratatui org)](https://github.com/ratatui/ratatui-image)
31. [ratatui-image — crates.io](https://crates.io/crates/ratatui-image)
32. [ratatui-image — API Documentation](https://docs.rs/ratatui-image/latest/ratatui_image/index.html)

### Kitty Protocol Implementations
33. [Kitty Unicode Placeholder Pull Request (#5664)](https://github.com/kovidgoyal/kitty/pull/5664)
34. [Kitty Unicode Placeholder Discussion (#4021)](https://github.com/kovidgoyal/kitty/discussions/4021)
35. [kittytgp — Pure Python Kitty Graphics with Unicode Placeholders](https://github.com/AnswerDotAI/kittytgp)
36. [Ghostty Kitty Graphics Implementation — Instagit Analysis](https://instagit.com/ghostty-org/ghostty/ghostty-kitty-graphics-protocol-implementation/)
37. [Mitchell Hashimoto on libghostty Kitty Graphics support](https://x.com/mitchellh/status/2041253090205249584)
38. [Graphics Protocol — Kitty DeepWiki](https://deepwiki.com/kovidgoyal/kitty/4.2-graphics-protocol)

### tmux Graphics
39. [sixel-tmux — Community fork for reliable graphics](https://github.com/csdvrx/sixel-tmux)
40. [Sixel Support in Tmux — Engineered.at](https://engineered.at/articles/revolutionizing-terminal-productivity-the-exciting-integration-of-sixel-support-in-tmux)

### Color Science
41. [Colorcet — Perceptually Accurate Colormaps](https://colorcet.holoviz.org/index.html)
42. [Introduction to Viridis Color Maps — CRAN](https://cran.r-project.org/web/packages/viridis/vignettes/intro-to-viridis.html)
43. [Viridis Color Palettes for R](https://sjmgarnier.github.io/viridis/)
44. [tinycolormap — C++ Colormap Library (Viridis, Inferno, etc.)](https://github.com/yuki-koyama/tinycolormap)

### Heatmap Implementations
45. [ANSI-Heatmap — Terminal heatmap renderer](https://github.com/richardjharris/ANSI-Heatmap)
46. [terminal-heatmap — Experimental terminal heatmaps](https://github.com/jclulow/terminal-heatmap)
47. [UnicodePlots — Julia terminal plotting library](https://juliapackages.com/p/unicodeplots)
48. [VisiData Terminal Graphics](https://www.visidata.org/docs/graphics/)

### Other Terminal Graphics Libraries
49. [notcurses — Rust bindings documentation](https://docs.rs/notcurses)
50. [libsixel — The new standard of SIXEL development](https://saitoha.github.io/libsixel/)
51. [rasterm — Go library for iTerm/Kitty/Sixel](https://github.com/BourgeoisBear/rasterm)
52. [Nick Black's Sixel wiki page](https://nick-black.com/dankwiki/index.php/Sixel)

### Blog Posts and Analysis
53. [Plotting in the Terminal — Medium (Vikas Negi)](https://medium.com/geekculture/plotting-in-the-terminal-an-unconventional-approach-to-data-visualization-dd36ec6515d0)
54. [Building High-Performance CLIs: Rust & TUI for Monitoring](https://techbytes.app/posts/rust-tui-high-performance-cli-monitoring/)
55. [Will McGugan — A New Way of Drawing Boxes in the Terminal](https://www.willmcgugan.com/blog/tech/post/ceo-just-wants-to-draw-boxes/)
56. [Sixel for Terminal Graphics — konfou.xyz](https://konfou.xyz/posts/sixel-for-terminal-graphics/)

---

## HIGH PRIORITY Items for Follow-Up

1. **Half-block heatmap widget prototype** — Build a ratatui widget that renders a matrix as half-blocks with truecolor viridis/inferno colormaps. This is the single highest-impact visualization for rocket_surgeon.

2. **Kitty Unicode placeholder integration** — Evaluate ratatui-image's Unicode placeholder support for pixel-perfect heatmaps that coexist with text overlays inside tmux.

3. **Graceful degradation testing** — Test the tier 1–5 fallback chain across Kitty, Ghostty, WezTerm, iTerm2, Alacritty, basic xterm, and tmux with/without sixel.

4. **Colormap lookup table embedding** — Embed viridis, cividis, and inferno as static 256-entry RGB arrays in Rust. These are public domain data. Avoid pulling in a color library dependency.

5. **ratatui Canvas/Chart for time series** — Prototype loss curves and activation norm plots using ratatui's Chart widget with Braille markers.

6. **Tree widget evaluation** — Evaluate `tui-tree-widget` or build a custom collapsible tree widget for the model architecture view.
