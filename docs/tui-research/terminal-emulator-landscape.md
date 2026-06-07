# Terminal Emulator Landscape: Deep Fundamentals for rocket_surgeon TUI Design

**Date:** 2026-05-19
**Purpose:** Exhaustive reference on terminal emulator capabilities, limitations, and runtime detection to inform ratatui-based TUI design decisions for rocket_surgeon.

---

## 1. Terminal Emulator Landscape

### Kitty

GPU-accelerated (OpenGL), cross-platform (macOS, Linux). Originated the Kitty graphics protocol, now adopted by Ghostty, WezTerm, Konsole, and others. Supports ligatures, true color, Unicode grapheme clusters, styled underlines (curly, dotted, dashed), OSC 52 clipboard, OSC 8 hyperlinks, synchronized output (mode 2026), and XTGETTCAP for runtime capability querying. Kitty extends XTGETTCAP with `kitty-query-*` keys exposing runtime metadata (terminal name, version, font family, DPI, clipboard control policy). Idle memory: 60-100 MB. Key-to-screen latency: ~3ms. Kitty has its own keyboard protocol (progressive enhancement) that reports key events with modifiers unambiguously -- this is a significant advancement over the traditional terminal keyboard model and is now also supported by Ghostty, WezTerm, foot, and rio.

### Ghostty

GPU-accelerated via platform-native APIs (Metal on macOS, OpenGL/Vulkan on Linux). Written in Zig. As of 2026, the fastest terminal emulator with ~2ms key-to-screen latency. Supports Kitty graphics protocol, Kitty keyboard protocol, custom GLSL shaders, light/dark mode notifications, OSC 8 hyperlinks, OSC 52 clipboard, synchronized output, true color. Version 1.2.0 (September 2025) brought 2,676 commits from 149 contributors. Version 1.3.0 planned for March 2026 with scrollback search and scrollbars. No Windows support yet. Idle memory: 60-100 MB. Genuinely native on macOS (feels like a first-party app) and Linux.

### WezTerm

GPU-accelerated, Rust-based, cross-platform (macOS, Linux, Windows, FreeBSD). The most feature-complete terminal emulator. Supports ALL THREE graphics protocols (Kitty, Sixel, iTerm2) -- the only terminal to do so. Lua-based configuration (full programming language, not just config). Built-in multiplexer (tabs, panes, workspaces) with its own mux server protocol. Built-in SSH client. Serial port support. Supports ligatures, true color, styled underlines, OSC 52, OSC 8, synchronized output, Kitty keyboard protocol. Idle memory is higher than minimalist terminals due to feature breadth. Built-in multiplexer means you can avoid tmux entirely.

### Alacritty

GPU-accelerated (OpenGL), Rust-based, cross-platform. Philosophy: do one thing (render terminal output) as fast as possible. Deliberately excludes ligatures, tabs, splits, inline images, notifications. Idle memory: ~22 MB (lowest of any GPU-accelerated terminal). Relies on external tools (tmux, window manager) for multiplexing. Supports true color, OSC 52, basic OSC 8. No graphics protocol support. TOML configuration. Was the fastest terminal until Ghostty surpassed it. The "no features" philosophy means it has fewer compatibility concerns but also fewer capabilities for rich TUI applications.

### iTerm2

macOS only. The incumbent macOS terminal for power users, though increasingly displaced by Ghostty. Proprietary inline image protocol (OSC 1337). Sixel support since September 2022. Deep tmux integration (`tmux -CC` control mode creates native iTerm2 windows backed by tmux sessions). Shell integration, Triggers, Profiles, extensive AppleScript/Python API. Supports true color, OSC 52, OSC 8. Proprietary escape sequences for notifications, badges, profile switching, cursor shape, annotations. Higher memory usage and slower throughput than GPU-native terminals. Ghostty benchmarks show 3x throughput advantage and 4x memory advantage over iTerm2.

### foot

Wayland-native, C, minimal. Server/client architecture: one process hosts multiple windows, sharing fonts and glyph cache for reduced memory and instant window spawning. Supports Sixel, true color, OSC 52, OSC 8, synchronized output, styled underlines. Full XTGETTCAP support (entire terminfo queryable). URL detection mode (`ctrl+shift+o`). No tabs, no splits (by design -- use a tiling Wayland compositor). Among the lowest-latency terminals in benchmarks. Wayland-only (no X11, no macOS, no Windows).

### Rio

Rust-based, WebGPU/WGPU rendering. Cross-platform (macOS, Linux, Windows, FreeBSD). Supports Kitty graphics, Sixel, iTerm2 image protocol. Ligatures, true color, Vi mode (from Alacritty heritage). TOML configuration. Split panes. RetroArch shaders for CRT-style effects. Native ARM64 builds. Relatively new entrant, smaller community.

### Windows Terminal

Microsoft's modern terminal for Windows. Runs on top of ConPTY (Console Pseudo Terminal). Supports true color (24-bit), DA2/DA3 device attributes, synchronized output (mode 2026, added via PR #18826). Known ConPTY limitations: escape sequence modification (some sequences get mangled in transit), tab handling bugs, text wrapping interference with escape processing. OSC 52 support was a long-requested feature (issue #2946). Improving rapidly but ConPTY remains a compatibility concern -- it sits between the application and the actual terminal, reinterpreting escape sequences, which can cause subtle bugs. GPU-accelerated rendering via DirectX.

### VS Code Integrated Terminal

Built on xterm.js (a JavaScript terminal emulator running in Electron). Custom shell integration escape sequences. OSC 52 works locally but is silently broken over Remote SSH. Kitty keyboard protocol sequences can leak through as literal text (e.g., `[57358u` appearing when Caps Lock is pressed). The terminal is fundamentally a web technology rendering a terminal in a browser context, which creates an impedance mismatch with native terminal protocols. Limited by what xterm.js implements. XTGETTCAP support is under investigation (issue #4107). Important because many developers use VS Code Remote SSH as their primary terminal.

### macOS Terminal.app

Apple's built-in terminal. Only supports up to 256 colors -- one of the few common terminals that does NOT support true color (24-bit). Limited escape sequence support. No graphics protocols. No OSC 8 hyperlinks. Effectively a legacy terminal that Apple has not invested in modernizing. Any TUI targeting macOS must handle the case where the user is running Terminal.app, which means graceful degradation from 24-bit color to 256-color palettes.

### tmux

Not a terminal emulator but a terminal multiplexer -- it sits between the application and the actual terminal, presenting its own virtual terminal. Uses its own terminfo entry (`tmux-256color` or `screen-256color`). Supports true color (with `terminal-features` or `terminal-overrides` configuration). Sixel support when compiled with `--enable-sixel`. DCS passthrough (`\033Ptmux;\033...\033\\`) allows forwarding escape sequences to the outer terminal, but requires `set -g allow-passthrough on` (tmux 3.2+). Mode 2031 (dark/light theme detection) support added via PR #4353. Synchronized output support. OSC 52 passthrough. The passthrough mechanism is fragile: sequences are truncated after ~60 characters in some versions (issue #4377), and escapes within the payload must be doubled. tmux fundamentally acts as a VT100-ish terminal emulator itself, which means it filters/reinterprets many escape sequences -- anything the tmux VT parser doesn't understand gets dropped.

### GNU Screen

The original terminal multiplexer. Increasingly replaced by tmux. Limited to 1023-byte termcap translations. Less active development, slower to adopt modern features. Does not support true color natively in stable releases. Escape sequence passthrough is more limited than tmux. Uses `Ctrl-a` as its prefix key (configurable). Relevant mainly for legacy environments and servers where it's the only multiplexer installed.

### mosh (Mobile Shell)

UDP-based remote terminal protocol designed for mobile/high-latency connections. Predictive local echo masks network latency (70% of keystrokes predicted correctly, median response <5ms). UTF-8 only (refuses to start without UTF-8 locale). Key limitations: does NOT support X11 forwarding, port forwarding, or any passthrough of custom escape sequences (issue #1135). Mangles certain color escape sequences (issue #519) -- specifically, requires foreground and background color changes in separate escape sequences. No graphics protocol forwarding. Mosh maintains its own model of the terminal state and synchronizes screen diffs over UDP -- this architecture means any escape sequence mosh's internal VT parser doesn't understand is silently dropped. This makes mosh hostile to advanced TUI features.

---

## 2. Capability Detection at Runtime

### Environment Variables

- **`TERM`**: Identifies the terminfo entry to use. Common values: `xterm-256color`, `screen-256color`, `tmux-256color`, `xterm-kitty`, `xterm-ghostty`. Unreliable for feature detection because users frequently override it, and many terminals claim `xterm-256color` despite having capabilities far beyond xterm.
- **`TERM_PROGRAM`**: Set by some terminals to identify themselves. Kitty sets `kitty`, iTerm2 sets `iTerm.app`, WezTerm sets `WezTerm`, VS Code sets `vscode`, Apple Terminal sets `Apple_Terminal`. Not universally set. Useful as a hint but not authoritative.
- **`TERM_PROGRAM_VERSION`**: Version string, set by some terminals alongside `TERM_PROGRAM`.
- **`COLORTERM`**: If set to `truecolor` or `24bit`, indicates 24-bit color support. Set by Kitty, Konsole, libvte-based terminals, and others. Not universally reliable.
- **`TMUX`**: Set when running inside tmux. Contains the socket path and PID.
- **`STY`**: Set when running inside GNU Screen.

### Device Attribute Queries

- **DA1 (Primary Device Attributes)**: `CSI c` or `CSI 0 c`. Response reports terminal class and supported features as a list of feature codes. Feature code 4 indicates Sixel support. Feature code 22 indicates ANSI color. Useful for basic capability probing.
- **DA2 (Secondary Device Attributes)**: `CSI > c` or `CSI > 0 c`. Response format: `CSI > Pp ; Pv ; Pc c` where Pp is terminal type, Pv is firmware version, Pc is hardware options. Can identify the terminal type and version. Windows Terminal added DA2/DA3 support (commit 53b224b).
- **DA3 (Tertiary Device Attributes)**: `DCS ! | <hex-encoded-unit-id> ST`. Returns a unit ID. Less commonly used for feature detection.

### XTGETTCAP

`DCS + q <hex-encoded-cap-name> ST`. Queries the terminal's terminfo database directly via escape sequences -- works over SSH without needing terminfo installed on the remote machine. First introduced by XTerm. Supported by Kitty, foot (full terminfo queryable), iTerm2. Kitty extends it with `kitty-query-*` keys. The query is slow (requires a round-trip to the terminal), so results should be cached. Not widely supported enough to be the sole detection mechanism. Foot exposes its entire terminfo via XTGETTCAP, making it the gold standard for this approach.

### DECRQM (Request Mode)

`CSI ? <mode> $ p`. Queries whether a specific DEC private mode is supported and its current state. Response: `CSI ? <mode> ; <value> $ y` where value is 0 (not recognized), 1 (set), 2 (reset), 3 (permanently set), 4 (permanently reset). Critical for probing synchronized output (mode 2026), dark/light mode notifications (mode 2031), and bracketed paste (mode 2004). If DECRQM itself is not implemented, you get no response (timeout-based detection).

### The Reality of Detection

The fundamental problem: there is no single reliable mechanism. The practical approach is layered:

1. Check environment variables (`TERM_PROGRAM`, `COLORTERM`, `TMUX`, `STY`) for quick hints.
2. Send DA1/DA2 queries with a timeout for basic terminal identification.
3. Use DECRQM to probe specific modes (2026, 2031, etc.).
4. Use XTGETTCAP for terminals known to support it.
5. For color depth, try setting a truecolor value and query it back via DECRQSS.
6. Fall back to terminfo database for baseline capabilities.
7. Cache everything. Sticky failure (if DA1 times out, don't retry DA2).

The timeout-based approach is inherently racy and adds startup latency. This is why many TUI applications offer manual configuration overrides -- because auto-detection will always have edge cases.

---

## 3. Escape Sequence Standards and Reality

### The Standards Hierarchy

- **ECMA-48 / ISO 6429 / ANSI X3.64**: The foundational standard for control functions. Defines CSI (Control Sequence Introducer), OSC (Operating System Command), DCS (Device Control String), and the basic grammar. Most terminals claim ECMA-48 compliance but implement varying subsets.
- **DEC VT series (VT100/VT220/VT320/VT420/VT510/VT520)**: DEC's terminal hardware defined many private-mode sequences (DECSET/DECRST) that became de facto standards. Examples: mode 1 (application cursor keys), mode 25 (cursor visible), mode 1049 (alternate screen buffer), mode 2004 (bracketed paste). These are "private" modes (the `?` in `CSI ?`) which ECMA-48 explicitly leaves to implementors.
- **xterm extensions**: The single most influential implementation. Thomas Dickey's xterm has accumulated decades of extensions that other terminals copy. The canonical reference is `ctlseqs.html` at invisible-island.net. OSC sequences 0-119, mouse tracking modes, selection protocols, focus events, modified key reporting, and more all originate from or are canonized by xterm.

### CSI Sequences (What Matters for TUIs)

- **SGR (Select Graphic Rendition)**: `CSI <params> m`. Colors, bold, italic, underline, strikethrough, reverse, etc. The most frequently used sequence family. 8-color: SGR 30-37/40-47. 16-color: adds SGR 90-97/100-107. 256-color: `CSI 38;5;<n> m`. True color: `CSI 38;2;<r>;<g>;<b> m`. Styled underlines: `CSI 4:<style> m` (1=straight, 2=double, 3=curly, 4=dotted, 5=dashed). Underline color: `CSI 58;2;<r>;<g>;<b> m`. Not all terminals support all SGR attributes.
- **Cursor movement**: CUU/CUD/CUF/CUB/CUP for movement. ED/EL for erasing. Standard and well-supported.
- **Synchronized output**: `CSI ? 2026 h` (begin) / `CSI ? 2026 l` (end). Batches rendering to prevent tearing during full-screen updates. Supported by Kitty, Ghostty, WezTerm, foot, Contour, Windows Terminal. Query support via `CSI ? 2026 $ p`.
- **Kitty keyboard protocol**: Progressive enhancement mode for unambiguous key reporting. `CSI > <flags> u` to enable. Reports keys as `CSI <keycode> ; <modifiers> u`. Solves the decades-old problem of distinguishing Ctrl-I from Tab, Ctrl-M from Enter, etc. Supported by Kitty, Ghostty, WezTerm, foot, rio.

### OSC Sequences (Terminal-Application Integration)

- **OSC 0/1/2**: Window/icon title. Universally supported.
- **OSC 7**: Current working directory. Used by shell integration for `cd` tracking. Format: `OSC 7 ; file://hostname/path ST`.
- **OSC 8**: Hyperlinks. `OSC 8 ; params ; uri ST` ... text ... `OSC 8 ;; ST`. Widely supported in modern terminals. Params can include `id=` for grouping adjacent link cells.
- **OSC 10/11**: Query/set foreground/background color. `OSC 11 ; ? ST` queries background color; response is `OSC 11 ; rgb:RRRR/GGGG/BBBB ST`. Used for light/dark mode detection. Some terminals (e.g., Tabby) return incorrect values.
- **OSC 52**: Clipboard access. `OSC 52 ; c ; <base64-data> ST`. Critical for clipboard operations over SSH. Supported by most modern terminals. Broken in VS Code Remote SSH.
- **OSC 1337**: iTerm2 proprietary. Inline images, notifications, badges, profile switching, annotations, custom marks.

### The Standards-Reality Gap

What terminfo says and what terminals actually do are frequently different. Key issues:

1. Terminfo was designed in the early 1980s for hardware terminals. It cannot natively represent modern features (graphics protocols, hyperlinks, styled underlines, keyboard protocols). The ncurses user-defined capabilities extension partially addresses this.
2. Many terminals set `TERM=xterm-256color` but support far more than what the xterm-256color terminfo entry describes.
3. Terminfo source files are restricted to ISO 8859-1, limiting internationalization metadata.
4. The termcap compatibility layer has a 1023-byte limit per entry, causing truncation of complex entries.
5. User-installed terminfo entries (e.g., Kitty's `xterm-kitty`) may not be present on remote machines, causing fallback to generic entries and loss of capabilities.

---

## 4. Color Support Tiers

### 8-Color (SGR 30-37 / 40-47)

The original ANSI colors: black, red, green, yellow, blue, magenta, cyan, white. Foreground: SGR 30-37. Background: SGR 40-47. Reset: SGR 39/49. Universal support. The actual RGB values vary wildly between terminals and color schemes.

### 16-Color (+ SGR 90-97 / 100-107)

Adds "bright" variants. Originally achieved via SGR bold (1) + color, but SGR 90-97/100-107 provide direct bright color access. The "bright" colors are independently configurable in most terminals. Still universal support.

### 256-Color (SGR 38;5;n / 48;5;n)

Introduced by xterm. Colors 0-7: standard ANSI. 8-15: bright ANSI. 16-231: 6x6x6 color cube. 232-255: grayscale ramp. `CSI 38;5;<n> m` for foreground, `CSI 48;5;<n> m` for background. Nearly universal support. Terminal.app supports this tier.

### True Color / 24-bit (SGR 38;2;r;g;b / 48;2;r;g;b)

16.7 million colors. `CSI 38;2;<r>;<g>;<b> m` for foreground. Supported by: Kitty, Ghostty, WezTerm, Alacritty, foot, Rio, Windows Terminal, iTerm2, tmux (with configuration), and most modern terminals. NOT supported by macOS Terminal.app (only 256-color). Detection: check `COLORTERM=truecolor` or `24bit`; or actively probe by setting a color and querying it back via DECRQSS.

### Theme Detection (Light vs. Dark)

Two mechanisms:

1. **OSC 11 query**: `\033]11;?\a` -- terminal responds with `\033]11;rgb:RRRR/GGGG/BBBB\a`. Compute luminance from the RGB values to classify as light or dark. Works in most modern terminals. Some report incorrect values (e.g., always returning black).
2. **Mode 2031**: `CSI ? 2031 h` subscribes to dark/light mode change notifications. `CSI ? 996 n` explicitly requests current preference. The terminal sends an unsolicited DSR when the system theme changes. Supported by Ghostty, Contour, and increasingly adopted (tmux PR #4353, Helix PR #14356). This is the correct solution for live theme switching.

Rust crate `terminal_colorsaurus` implements OSC 10/11 querying for background/foreground color detection.

### Color Space Considerations

Terminal colors are specified in sRGB. There is no standard mechanism for wide-gamut or HDR color in terminals. The actual rendering depends on the terminal's color management (or lack thereof). Some terminals (notably macOS ones) are color-managed, others are not. For a debugger TUI, this is unlikely to matter -- but if displaying heatmaps or activation visualizations, the color fidelity depends entirely on the terminal and monitor.

---

## 5. Unicode Rendering

### The Width Problem

The fundamental issue: terminals operate on a fixed-width character grid, but Unicode characters have varying visual widths. The traditional `wcwidth()` function (POSIX) assigns width 1 to most characters and width 2 to East Asian Wide characters. This model breaks down for:

- **Emoji**: Many emoji are width 2, but the Unicode Emoji specification and `wcwidth()` often disagree. Emoji presentation sequences (codepoint + U+FE0F) should render as width 2 regardless of their East_Asian_Width property.
- **ZWJ sequences**: "👩‍🚀" is three codepoints (person + ZWJ + rocket) but should render as one width-2 character if the terminal supports the compound glyph. If not, it renders as two separate emoji (width 4). There is no standard way to know which ZWJ sequences a terminal supports.
- **Combining characters**: Diacritics (accents, etc.) that combine with a base character. Should be width 0, but broken implementations exist.
- **Ambiguous-width characters**: Characters classified as East_Asian_Width=Ambiguous (UAX #11). Default: width 1 in Western contexts, width 2 in CJK contexts. Different terminals handle this differently. The `wcwidth()` libraries allow configuring ambiguous width, but the terminal's own opinion may differ.

### Grapheme Clusters vs. Codepoints

Modern text rendering should operate on grapheme clusters (user-perceived characters), not individual codepoints. A grapheme cluster is one or more codepoints that form a single visual unit. Mitchell Hashimoto's article on grapheme clusters in terminals (written during Ghostty development) highlights that most terminals historically operated at the codepoint level, not the grapheme cluster level. Ghostty and foot attempt proper grapheme cluster handling.

The ratatui project uses `unicode-segmentation` crate for grapheme cluster splitting but struggles with the M:N mapping problem: M codepoints map to N terminal cells, and determining N requires knowing both the Unicode properties AND the terminal's rendering behavior. A prototype `grapheme-width` crate was proposed but the problem remains fundamentally unsolvable without terminal cooperation (since the terminal decides the rendered width).

### Terminal Inconsistencies

Windows Terminal renders "woman scientist" emoji as 5 columns when it should be 2. Different terminals disagree on the width of various emoji and CJK characters. The `ucs-detect` tool (updated February 2026) probes terminal Unicode capabilities by rendering test strings and measuring cursor positions, providing empirical data for a specific terminal.

### The State of Unicode in 2026

Unicode 17.0.0 was released in September 2025. Key Rust crates:
- `unicode-width` (updated for Unicode 17.0.0): traditional wcwidth-style width computation.
- `unicode-segmentation`: grapheme cluster boundaries.
- `runefix-core` (2025): attempts to solve the "layout unit" problem, grouping ZWJ sequences and variation selectors into atomic units.

The practical takeaway for rocket_surgeon: avoid emoji and ZWJ sequences in the TUI chrome. Stick to ASCII and basic Unicode box-drawing characters for the interface itself. If displaying user data that contains emoji/CJK, accept that width calculation will be approximate and may misalign in some terminals.

---

## 6. Performance Characteristics

### Throughput

Terminal throughput measures how fast the terminal can process and render incoming data. Kitty includes a built-in benchmark (`kitten benchmark`). The `vtebench` tool from the Alacritty project generates standardized benchmarks. Key insight from Kitty's documentation: the relationship between stdout throughput and perceived speed is non-obvious. A terminal can have high throughput but poor frame rate, or vice versa.

Typical throughput for modern GPU-accelerated terminals: 200-800 MB/s for raw text. This is far more than any TUI application will generate. The bottleneck for TUI applications is not throughput but rather frame composition latency and the PTY round-trip.

### Latency

Key-to-screen latency (typometer measurements, 2026):
- Ghostty: ~2ms
- Alacritty: ~3ms
- Kitty: ~3ms
- foot: ~2-3ms (among the lowest on Wayland)
- WezTerm: ~5-8ms (higher due to feature overhead)
- iTerm2: ~10-15ms

For a TUI debugger, perceived latency below 16ms (60fps) is acceptable. Below 50ms is "instant" for interactive controls. The critical path is: user input -> PTY -> application -> escape sequence output -> PTY -> terminal render. Each PTY crossing adds ~0.1-0.5ms on Linux, more on macOS.

### Full-Screen Redraws vs. Incremental Updates

Full-screen redraws (clearing and rewriting all cells) are expensive and cause visible flicker without synchronized output. Ratatui's rendering model uses a double-buffer diff approach: it compares the current frame with the previous frame and only emits escape sequences for changed cells. This is critical for performance on:
- Large terminal windows (200+ columns x 50+ rows = 10,000+ cells)
- SSH connections with latency
- Terminals without synchronized output support

Synchronized output (mode 2026) eliminates flicker from full redraws by batching the output, but incremental updates are still preferable for bandwidth efficiency over SSH.

### When Terminals Struggle

- **Large scrollback buffers**: Terminals with hundreds of thousands of scrollback lines consume significant memory. Not relevant for TUI applications that use the alternate screen.
- **Rapid updates**: High-frequency output (e.g., `cat /dev/urandom`) stresses the VT parser and renderer. GPU-accelerated terminals handle this much better than CPU-rendered ones.
- **Wide terminals**: Ultra-wide monitors with 300+ column terminals mean more cells to render per frame.
- **Complex Unicode**: Rendering grapheme clusters, emoji, and bidirectional text is CPU-intensive.
- **Graphics**: Sixel rendering is CPU-bound (pixel-by-pixel processing). Kitty graphics protocol offloads to GPU.

---

## 7. Multiplexer Complications

### tmux as Terminal-in-Terminal

tmux implements its own VT parser and emulates a virtual terminal. Applications inside tmux see tmux's terminal, not the outer terminal. tmux's TERM is typically `tmux-256color` or `screen-256color`. This means:

- Features the outer terminal supports but tmux doesn't are invisible to the application.
- tmux adds its own escape sequences (status line, pane borders).
- tmux rewrites cursor position, colors, and attributes as it composites panes.

### What Gets Eaten

By default, tmux silently drops any escape sequence its VT parser doesn't recognize. This kills:
- Graphics protocols (Sixel, Kitty) unless native support is compiled in or passthrough is enabled.
- Proprietary sequences (iTerm2 OSC 1337, etc.).
- Custom keyboard protocols (though Kitty keyboard protocol support is improving in tmux).

### DCS Passthrough

`\033Ptmux;\033<escaped-sequence>\033\\` wraps a sequence for passthrough to the outer terminal. Requirements:
- tmux 3.2+
- `set -g allow-passthrough on` (or `all`)
- All `\033` (ESC) characters within the payload must be doubled
- Known bug: sequences truncated after ~60 characters in some versions (issue #4377)
- Not a complete solution: the sequence is forwarded but tmux doesn't track its effects, so subsequent tmux operations may overwrite the result

### Nested Multiplexers

tmux inside tmux, or tmux inside screen, compounds all the above problems. Each layer adds its own VT emulation, escape sequence filtering, and rendering overhead. Capability detection becomes unreliable because each layer may respond differently to DA1/DA2 queries. The `TMUX` environment variable only indicates the innermost tmux session.

### Practical Implications for rocket_surgeon

Detect tmux/screen presence via environment variables. When inside a multiplexer:
1. Assume reduced capabilities (no graphics, possibly no styled underlines).
2. Use DECRQM to probe for specific modes rather than assuming.
3. Consider DCS passthrough for critical features (like clipboard via OSC 52).
4. Test explicitly with tmux since it's the most common multiplexer in ML/HPC environments.

---

## 8. Remote Access

### SSH Terminal Forwarding

SSH allocates a PTY on the remote host and forwards terminal I/O over the encrypted connection. What survives:
- All standard escape sequences (CSI, OSC, DCS) pass through unmodified.
- OSC 52 clipboard works (the escape sequence reaches the local terminal).
- Sixel/Kitty graphics pass through (the escape sequences are just bytes).
- Latency is the main concern, not capability loss.

What doesn't survive:
- `TERM_PROGRAM`, `COLORTERM`, and other detection env vars are not forwarded by default.
- terminfo entries may not be installed on the remote host (the `TERM` value may not resolve).
- XTGETTCAP queries work because they reach the local terminal, but there's added round-trip latency.

### mosh

As detailed in Section 1: mosh maintains its own terminal model and synchronizes screen state over UDP. It does NOT pass through arbitrary escape sequences. Any escape sequence mosh's VT parser doesn't understand is silently dropped. This means:
- No graphics protocols
- No OSC 52 clipboard
- No OSC 8 hyperlinks
- Color escape sequence mangling (foreground/background must be separate)
- Predictive local echo is great for latency but the protocol is fundamentally incompatible with rich TUI features

### VS Code Remote SSH

Uses its own terminal forwarding (xterm.js on the remote side). OSC 52 clipboard is broken (sequences silently ignored). Kitty keyboard protocol sequences can leak as literal text. Shell integration uses VS Code-specific custom escape sequences. This is a significant deployment target because many ML researchers use VS Code Remote to access GPU clusters.

### JetBrains Gateway

Remote development with a thin client. Terminal capabilities are limited by the IDE's built-in terminal emulator. Linux-only for remote hosts. Debugging and profiling features may not work in remote mode. Terminal is secondary to the IDE's GUI.

### Latency Implications

- Local: <1ms PTY round-trip
- SSH same datacenter: 1-5ms
- SSH cross-region: 20-100ms
- SSH intercontinental: 100-300ms
- mosh: perceived <5ms (predictive echo) but actual state sync matches SSH latency

For interactive TUI with rapid updates (e.g., stepping through a forward pass), latency above ~100ms becomes noticeably uncomfortable. Design should prefer server-side state with minimal client updates rather than chatty protocols.

---

## 9. Accessibility

### The Fundamental Tension

Screen readers (NVDA on Windows, VoiceOver on macOS, Orca on Linux) interact with terminals by reading the character grid and tracking cursor position. The cursor position is the single most important piece of metadata for terminal accessibility. A CLI (linear text stream) is accessible by default. A TUI (2D spatial layout) is fundamentally hostile to screen readers because:

1. Screen readers must parse an unstructured character grid with no semantic information.
2. TUI frameworks treat the terminal as a canvas, redrawing arbitrary regions.
3. Spinners, progress bars, and animation cause screen reader spam (every redraw triggers speech output).
4. Spatial layouts require the user to explore the grid manually, losing the efficiency of linear reading.

### The "Text Mode Lie"

The widespread misconception that terminal = accessible is documented in "The text mode lie: why modern TUIs are a nightmare for accessibility" (OSnews, May 2026). Modern TUI frameworks (Bubble Tea, Ink, tcell, ratatui) create worse accessibility outcomes than web applications with proper ARIA attributes.

### What Terminals Expose

Terminals expose to screen readers:
- The character grid contents
- Cursor position
- (Sometimes) text selection
- (Sometimes) line/cell change notifications

They do NOT expose:
- Semantic structure (this is a button, this is a table header)
- Focus/active element
- Role/state information
- Live region semantics

### Practical Approach for rocket_surgeon

Given that rocket_surgeon has a dual-interface design (TUI for humans, structured protocol for LLMs), the accessibility path is:

1. The structured protocol can serve as the accessibility interface -- screen reader users could interact via the protocol/CLI mode rather than the TUI.
2. If TUI accessibility is required, provide a `--a11y` or `--simple` mode that uses linear output instead of spatial layouts.
3. Avoid spinners and rapid redraws in any mode that might be used with a screen reader.
4. Keep the cursor position meaningful (at the active input point, not jumping around the screen).
5. This is an area where doing it right requires user testing with actual screen reader users -- there is no framework-level solution.

---

## 10. Implications for rocket_surgeon TUI Design

### Minimum Viable Terminal

Based on this research, the baseline terminal target for rocket_surgeon should be:
- 256-color support (covers Terminal.app)
- Basic CSI/OSC support (cursor movement, SGR attributes)
- Alternate screen buffer
- UTF-8 (but ASCII-safe TUI chrome)
- No graphics protocol dependency

### Enhanced Experience Tier

For terminals that support it:
- True color (24-bit) for richer data visualization
- Synchronized output (mode 2026) for flicker-free rendering
- OSC 52 for clipboard (critical for remote SSH workflows)
- OSC 8 for hyperlinks (linking to source files, documentation)
- Kitty keyboard protocol for unambiguous key input
- Mode 2031 for automatic light/dark theme detection

### Detection Strategy

1. Environment variable scan on startup
2. DA1/DA2 with 100ms timeout
3. DECRQM probes for modes 2026, 2031
4. Cache all results, expose via internal capability struct
5. Allow user overrides via configuration

---

## Bibliography

### Specifications and Primary References

- [XTerm Control Sequences (ctlseqs)](https://invisible-island.net/xterm/ctlseqs/ctlseqs.html) -- the canonical reference for terminal escape sequences. **HIGH PRIORITY**
- [ECMA-48: Control Functions for Coded Character Sets](https://ecma-international.org/publications-and-standards/standards/ecma-48/) -- the foundational standard
- [Kitty Graphics Protocol Specification](https://sw.kovidgoyal.net/kitty/graphics-protocol/) -- **HIGH PRIORITY**
- [Kitty Keyboard Protocol](https://sw.kovidgoyal.net/kitty/keyboard-protocol/) -- **HIGH PRIORITY** for unambiguous key handling
- [Synchronized Output Specification](https://gist.github.com/christianparpart/d8a62cc1ab659194337d73e399004036) -- DEC mode 2026
- [Hyperlinks in Terminal Emulators (OSC 8)](https://gist.github.com/egmontkob/eb114294efbcd5adb1944c9f3cb5feda) -- **HIGH PRIORITY**
- [UAX #11: East Asian Width](https://www.unicode.org/reports/tr11/) -- character width classification
- [Terminal Guide: DA1 Primary Device Attributes](https://terminalguide.namepad.de/seq/csi_sc/)
- [Terminal Guide: DA2 Secondary Device Attributes](https://terminalguide.namepad.de/seq/csi_sc__q/)
- [VT510 Reference Manual: DA2](https://vt100.net/docs/vt510-rm/DA2.html)
- [OSC Sequences Reference (terminfo.dev)](https://terminfo.dev/osc)
- [Dark/Light Mode Detection: Mode 2031 Specification (Contour)](https://contour-terminal.org/vt-extensions/color-palette-update-notifications/)
- [OSC 8 Adoption Tracker](https://github.com/Alhadis/OSC8-Adoption)

### Blog Posts and Articles

- [Grapheme Clusters and Terminal Emulators -- Mitchell Hashimoto](https://mitchellh.com/writing/grapheme-clusters-in-terminals) -- **HIGH PRIORITY**, Ghostty author's deep dive on Unicode rendering
- [Ghostty Devlog 004 -- Mitchell Hashimoto](https://mitchellh.com/writing/ghostty-devlog-004) -- XTGETTCAP and capability detection
- [I Just Wanted Emacs to Look Nice -- Chad Austin](https://chadaustin.me/2024/01/truecolor-terminal-emacs/) -- **HIGH PRIORITY**, exhaustive investigation of truecolor in terminals
- [So you want to render colors in your terminal -- Marvin Hagemeister](https://marvinh.dev/blog/terminal-colors/) -- practical color rendering guide
- [XTGETTCAP -- Alexander Gromnitsky](https://sigwait.org/~alex/blog/2025/03/25/XTGETTCAP.html) -- detailed XTGETTCAP analysis
- [Terminal Latency -- Dan Luu](https://danluu.com/term-latency/) -- foundational terminal latency benchmarks
- [Terminal Latency Benchmarks -- beuke.org](https://beuke.org/terminal-latency/) -- updated latency measurements
- [The Text Mode Lie: Why Modern TUIs Are a Nightmare for Accessibility -- OSnews](https://www.osnews.com/story/144892/the-text-mode-lie-why-modern-tuis-are-a-nightmare-for-accessibility/) -- **HIGH PRIORITY**
- [The Inclusive Lens: The Text Mode Lie](https://xogium.me/the-text-mode-lie-why-modern-tuis-are-a-nightmare-for-accessibility/) -- original source for the accessibility article
- [Accessibility of Command Line Interfaces -- ACM CHI 2021](https://dl.acm.org/doi/fullHtml/10.1145/3411764.3445544) -- **HIGH PRIORITY**, academic study on CLI accessibility
- [Why the Text Terminal Cursor is Important for Accessibility -- Blind Guru](https://blind.guru/blog/2021-06-25-brick.html)
- [Terminal Graphics Protocols: Kitty, Sixel, iTerm2, and Beyond -- Akmatori](https://akmatori.com/blog/terminal-graphics-protocols) -- comprehensive protocol comparison
- [Automatic Dark Mode for Terminal Apps, Revisited -- Fatih Arslan](https://arslan.io/2025/06/06/automatic-dark-mode-for-terminal-apps-revisited/) -- mode 2031 practical guide
- [Standard for Dark/Light Preference in Terminals -- Tau's Wiki](https://wiki.tau.garden/cli-theme/)
- [A Comparison of Terminal Emulators (2025) -- randomstring.org](https://blog.randomstring.org/2025/09/26/a-comparison-of-terminal-emulators/)
- [Unexpected Behaviour in Command Line Interfaces -- G-Research](https://www.gresearch.com/news/g-research-the-terminal-escapes/) -- escape sequence edge cases

### Terminal Documentation

- [Ghostty Features](https://ghostty.org/docs/features)
- [Ghostty 1.2.0 Release Notes](https://ghostty.org/docs/install/release-notes/1-2-0)
- [iTerm2 Proprietary Escape Codes](https://iterm2.com/documentation-escape-codes.html)
- [iTerm2 Inline Images](https://iterm2.com/documentation-images.html)
- [iTerm2 tmux Integration](https://iterm2.com/documentation-tmux-integration.html)
- [WezTerm Multiplexing](https://wezterm.org/multiplexing.html)
- [foot Terminal Emulator](https://codeberg.org/dnkl/foot)
- [Windows Console Virtual Terminal Sequences -- Microsoft](https://learn.microsoft.com/en-us/windows/console/console-virtual-terminal-sequences)
- [Kitty Performance Documentation](https://sw.kovidgoyal.net/kitty/performance/)

### Ratatui and Rust Ecosystem

- [Ratatui Backends](https://ratatui.rs/concepts/backends/) -- **HIGH PRIORITY**
- [Ratatui Backend Comparison](https://ratatui.rs/concepts/backends/comparison/)
- [Ratatui Unicode Width Discussion (#1438)](https://github.com/ratatui/ratatui/discussions/1438) -- **HIGH PRIORITY**, emoji/Unicode width challenges
- [Ratatui Unicode Width Bug (#1271)](https://github.com/ratatui/ratatui/issues/1271)
- [Ratatui ANSI Passthrough Request (#1227)](https://github.com/ratatui/ratatui/issues/1227) -- hyperlinks/images in ratatui
- [terminal_colorsaurus Rust crate](https://docs.rs/terminal-colorsaurus/latest/terminal_colorsaurus/) -- light/dark detection
- [runefix-core Rust crate](https://github.com/runefix-labs/runefix-core) -- Unicode display width for terminals

### Tools and Testing

- [vtebench -- Terminal Emulator Benchmarks](https://github.com/alacritty/vtebench)
- [Are We Sixel Yet?](https://www.arewesixelyet.com/) -- Sixel support tracker
- [ucs-detect -- Terminal Unicode Detection](https://pypi.org/project/ucs-detect/2.1.0/)
- [terminal-query -- Node.js Terminal Querying](https://github.com/sindresorhus/terminal-query/)
- [True Colour Terminal Support Gist](https://gist.github.com/kurahaupo/6ce0eaefe5e730841f03cb82b061daa2)
- [Terminal Compatibility Matrix (tmuxai.dev)](https://tmuxai.dev/terminal-compatibility/)
- [Terminal Emulators Comparison Table 2026 (Terminal Trove)](https://terminaltrove.com/compare/terminals/)

### tmux and Multiplexers

- [tmux FAQ](https://github.com/tmux/tmux/wiki/FAQ)
- [tmux Passthrough Truncation Bug (#4377)](https://github.com/tmux/tmux/issues/4377)
- [tmux Mode 2031 PR (#4353)](https://github.com/tmux/tmux/pull/4353)
- [tmux Dark/Light Mode Detection Issue (#4286)](https://github.com/tmux/tmux/issues/4286)
- [sixel-tmux Fork](https://github.com/csdvrx/sixel-tmux)

### Remote Access

- [mosh: Mobile Shell](https://mosh.org/)
- [mosh Custom Escape Passthrough Issue (#1135)](https://github.com/mobile-shell/mosh/issues/1135)
- [mosh Color Escape Mangling Issue (#519)](https://github.com/mobile-shell/mosh/issues/519)
- [VS Code OSC 52 over Remote SSH Issue (#11475)](https://github.com/microsoft/vscode-remote-release/issues/11475)
- [Achieving Low Latency Remote Development -- Coder Blog](https://coder.com/blog/achieving-low-latency-remote-development)
