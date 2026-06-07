# TUI Design Literature Catalog

Literature survey for rocket_surgeon TUI intermission. Collected 2026-05-19.

Rating key:
- **ESSENTIAL** -- Must read before designing anything
- **HIGH PRIORITY** -- Strongly recommended, fills a critical knowledge gap
- **USEFUL** -- Good reference material, read when tackling specific area
- **BACKGROUND** -- Worth skimming, provides context

---

## 1. TUI Design Philosophy & Patterns

### 1.1 Learning From Terminals to Design the Future of User Interfaces
- **URL:** https://brandur.org/interfaces
- **Format:** Blog post (web)
- **Rating:** ESSENTIAL
- **Summary:** Brandur argues that terminal interfaces embody design principles that modern GUIs have lost: composability, information density, keyboard-first interaction, and optimizing for experienced users over first-timers. Critiques superfluous animations and whitespace in modern UIs, contending that speed and productivity should trump aesthetics. Directly relevant to our "power user first" philosophy.

### 1.2 The Terminal Renaissance: Designing Beautiful TUIs in the Age of AI
- **URL:** https://dev.to/hyperb1iss/the-terminal-renaissance-designing-beautiful-tuis-in-the-age-of-ai-24do
- **Format:** Blog post (web)
- **Rating:** HIGH PRIORITY
- **Summary:** Surveys the current TUI renaissance driven by AI tools (Gemini CLI, Claude Code) and frameworks (Ghostty, Bubble Tea, Ratatui). Discusses how modern TUIs balance beauty with function and how AI integration is reshaping terminal design expectations. Good landscape overview of where the ecosystem is in 2025-2026.

### 1.3 7 Things I've Learned Building a Modern TUI Framework (Will McGugan / Textualize)
- **URL:** https://www.textualize.io/blog/7-things-ive-learned-building-a-modern-tui-framework/
- **Format:** Blog post (web)
- **Rating:** ESSENTIAL
- **Summary:** Hard-won lessons from building Rich and Textual, the most ambitious TUI framework of the past decade. Key insights include: immutability simplifies everything (state, caching, testing), API design is the bottleneck, and terminal rendering is deceptively hard. McGugan has more experience building production TUI frameworks than almost anyone alive. Must-read for avoiding pitfalls.

### 1.4 SE Radio 669: Will McGugan on Text-Based User Interfaces
- **URL:** https://se-radio.net/2025/05/se-radio-669-will-mcgugan-on-text-based-user-interfaces/
- **Format:** Podcast (audio, ~1 hour)
- **Rating:** HIGH PRIORITY
- **Summary:** In-depth technical interview covering design idioms for TUIs, practical rendering strategies, performance considerations, and the subtle idiosyncrasies of building performant TUI frameworks. Goes deeper than the blog post on specific implementation decisions in Textual and Rich. Also available on IEEE Xplore (https://ieeexplore.ieee.org/document/11119101/).

### 1.5 Ratatui Rendering Concepts: Immediate Mode
- **URL:** https://ratatui.rs/concepts/rendering/
- **Format:** Documentation (web)
- **Rating:** ESSENTIAL
- **Summary:** Official documentation explaining Ratatui's immediate-mode rendering model with intermediate diff buffers. The entire UI is rebuilt every frame from application state, but only changed cells are actually written to the terminal. This is the rendering model we will be building on -- understanding its trade-offs (simplicity and predictability vs. potential inefficiency for complex UIs) is non-negotiable.

### 1.6 Immediate Mode vs Retained Mode (Dimitri Glazkov)
- **URL:** https://glazkov.com/2021/11/25/retained-and-immediate-mode/
- **Format:** Blog post (web)
- **Rating:** HIGH PRIORITY
- **Summary:** Clear technical comparison of immediate vs retained mode rendering paradigms. Immediate mode redraws everything each frame (predictable timing, simpler state), retained mode caches scene state (bursty updates, potential jank). Directly applicable to understanding the fundamental architectural choice Ratatui makes and its implications for our real-time debugger display.

### 1.7 The Elm Architecture (TEA) for TUI Applications
- **URL:** https://ratatui.rs/concepts/application-patterns/the-elm-architecture/
- **Format:** Documentation (web)
- **Rating:** ESSENTIAL
- **Summary:** Official Ratatui documentation on the Model-View-Update pattern adapted for terminal UIs. The three pillars: Model (application state), Update (handle events, produce new state), View (pure function from state to UI). This is the dominant architecture pattern in modern TUI development (used by Ratatui, Bubble Tea, and Textual). We need to understand it deeply to decide whether to adopt, adapt, or reject it.

### 1.8 The Bubbletea State Machine Pattern
- **URL:** https://zackproser.com/blog/bubbletea-state-machine
- **Format:** Blog post (web)
- **Rating:** USEFUL
- **Summary:** Practical guide to structuring complex TUI applications as state machines within the Elm Architecture (Bubble Tea/Go, but the pattern is language-agnostic). Shows how to manage multi-screen flows, modal dialogs, and complex state transitions. Useful as a pattern reference even though we are building in Rust, not Go.

### 1.9 Beyond Vim and Emacs: A Scalable UI Paradigm (EmacsConf 2020)
- **URL:** https://emacsconf.org/2020/talks/07/
- **Video:** https://www.youtube.com/watch?v=jBUurG3f_aM
- **Format:** Conference talk (18 min video)
- **Rating:** HIGH PRIORITY
- **Summary:** Sid Kasivajhula proposes a third paradigm beyond modal (Vim) and modeless (Emacs) editing: treating UI elements as conceptual entities reasoned about through a standard language of general "epistemic" habits, rather than memorizing keybindings per action. Directly relevant to our command model design -- we need to decide if rocket_surgeon is modal, modeless, or something new.

### 1.10 CLI UX Best Practices: 3 Patterns for Progress Displays (Evil Martians)
- **URL:** https://evilmartians.com/chronicles/cli-ux-best-practices-3-patterns-for-improving-progress-displays
- **Format:** Blog post (web)
- **Rating:** USEFUL
- **Summary:** Detailed comparison of spinner, X-of-Y counter, and progress bar patterns with clear guidance on when each is appropriate. Directly useful for our forward-pass stepping UI where we need to communicate progress through layers/ticks.

---

## 2. Debugger UI Design

### 2.1 GDB TUI Mode Documentation
- **URL:** https://sourceware.org/gdb/current/onlinedocs/gdb.html/TUI.html
- **Format:** Official documentation (web)
- **Rating:** ESSENTIAL
- **Summary:** Reference documentation for GDB's curses-based TUI mode. Shows the source, assembly, register, and command windows. Important to study both for its capabilities and its well-known limitations: fragile screen state when output mixes with TUI, fixed layout inflexibility, and poor handling of terminal resize. Understanding GDB TUI's failure modes is essential for avoiding them.

### 2.2 nnd: A TUI Debugger Alternative to GDB/LLDB
- **URL:** https://github.com/al13n321/nnd
- **HN Discussion:** https://news.ycombinator.com/item?id=43905185
- **Format:** Source code + community discussion (web)
- **Rating:** ESSENTIAL
- **Summary:** A from-scratch Linux TUI debugger written in Rust, inspired by RemedyBG. Key design decisions: no dependency on GDB/LLDB, obsessive performance focus (handles 2.5GB ClickHouse binaries), async/multi-threaded debug info loading with progress bars, and a 6MB standalone binary. The HN discussion contains deep insights on debugger TUI design trade-offs. This is the closest prior art to what we are building in spirit.

### 2.3 ChatDBG: Augmenting Debugging with Large Language Models
- **URL:** https://arxiv.org/abs/2403.16354
- **PDF:** https://arxiv.org/pdf/2403.16354
- **Format:** Academic paper (PDF -- for ../papers/)
- **Rating:** HIGH PRIORITY
- **Summary:** Integrates LLMs into GDB, LLDB, and Pdb, letting users ask natural-language questions ("why is x null?") and granting the LLM autonomous control of the debugger. Achieves 67-85% root cause identification. Directly relevant to our dual-interface design where LLMs are first-class users of the debugger. Study their protocol for LLM-debugger communication.

### 2.4 Comgra: A Tool for Analyzing and Debugging Neural Networks
- **URL:** https://arxiv.org/abs/2407.21656
- **PDF:** https://arxiv.org/pdf/2407.21656
- **Format:** Academic paper (PDF -- for ../papers/)
- **Rating:** ESSENTIAL
- **Summary:** PyTorch library that extracts internal activations and organizes them in a GUI. Shows summary statistics, individual data points, early-vs-late training comparisons, and gradient flow visualization. This is the closest existing tool to what rocket_surgeon does -- but GUI-only and single-GPU. Study their data model, visualization choices, and what they found useful for NN debugging workflows.

### 2.5 libdebug: Build Your Own Debugger
- **URL:** https://arxiv.org/abs/2506.02667
- **PDF:** https://arxiv.org/pdf/2506.02667
- **Format:** Academic paper / poster (PDF -- for ../papers/)
- **Rating:** USEFUL
- **Summary:** Python library providing programmatic debugger building blocks (registers, memory, breakpoints, watchpoints, syscalls). Median latency 3-4x lower than GDB. Relevant as a reference for the "debugger as library" approach and its API design, though it targets security/reverse engineering rather than ML.

### 2.6 LLDB TUI Documentation and Limitations (Peeter Joot)
- **URL:** https://peeterjoot.com/2019/08/26/the-lldb-tui-text-user-interface/
- **Format:** Blog post (web)
- **Rating:** USEFUL
- **Summary:** Documents LLDB's simplistic TUI mode: fixed pane sizes that don't resize, no console pane (must set breakpoints before entering GUI), and unmaintained code. A catalog of what not to do in a debugger TUI.

---

## 3. Data Visualization in Constrained Spaces

### 3.1 Edward Tufte: The Visual Display of Quantitative Information
- **URL:** https://www.edwardtufte.com/book/the-visual-display-of-quantitative-information/
- **Format:** Book (physical/PDF)
- **Rating:** ESSENTIAL
- **Summary:** The foundational text on data visualization. Core principles directly applicable to terminal displays: maximize data-ink ratio (every pixel should convey information), eliminate chartjunk, use small multiples for comparing across dimensions, and pursue high data density. Terminal displays are the ultimate constrained space -- Tufte's minimalism maps perfectly to our problem.

### 3.2 Tufte's Principles Summarized (thedoublethink.com)
- **URL:** https://thedoublethink.com/tuftes-principles-for-visualizing-quantitative-information/
- **Format:** Blog post (web)
- **Rating:** HIGH PRIORITY
- **Summary:** Comprehensive summary of Tufte's core principles: data-ink ratio, chartjunk elimination, graphical integrity, data density, small multiples. Useful as a quick reference if you have not read the book, though the book itself is strongly recommended.

### 3.3 Small Multiples: Visual Explorations (Morphocode)
- **URL:** https://morphocode.com/small-multiples-visual-explorations-in-architecture-and-information-design/
- **Format:** Blog post (web)
- **Rating:** HIGH PRIORITY
- **Summary:** Deep dive into Tufte's "small multiples" concept: postage-stamp-sized charts indexed by category or time, using the same visual grammar to enable rapid comparison. Directly applicable to displaying attention heads, expert activations, or per-layer statistics side-by-side in the TUI.

### 3.4 Plotille: Terminal Plotting with Braille Dots
- **URL:** https://github.com/tammoippen/plotille
- **HN Discussion:** https://news.ycombinator.com/item?id=40255611
- **Format:** Source code + documentation (web)
- **Rating:** HIGH PRIORITY
- **Summary:** Python library that renders plots, scatter plots, histograms, and heatmaps in the terminal using Unicode Braille characters (2x4 dot grid per cell = 8x resolution boost). No dependencies. The Braille rendering technique is directly applicable to our activation and attention heatmaps. Study their implementation of heatmap rendering.

### 3.5 Drawille: Pixel Graphics in Terminal with Unicode Braille
- **URL:** https://github.com/asciimoo/drawille
- **Format:** Source code (web)
- **Rating:** USEFUL
- **Summary:** The original library for Braille-based terminal pixel graphics. Implements Bresenham line drawing, ellipses, and basic 3D support using Unicode Braille characters (U+2800 to U+28FF). Reference implementation for understanding the Braille rendering primitive.

### 3.6 Uniplot: Lightweight Terminal Plotting with 4x/8x Resolution
- **URL:** https://pypi.org/project/uniplot/
- **Format:** Python package documentation (web)
- **Rating:** USEFUL
- **Summary:** Terminal plotting library offering 4x resolution via Unicode block characters and 8x via Braille. Designed for ML CI/CD pipelines where plots are part of automated workflows. Relevant to our use case of visualizing training metrics and activation distributions in terminal.

### 3.7 Plotting in the Terminal: An Unconventional Approach (Medium)
- **URL:** https://medium.com/geekculture/plotting-in-the-terminal-an-unconventional-approach-to-data-visualization-dd36ec6515d0
- **Format:** Blog post (web)
- **Rating:** BACKGROUND
- **Summary:** Overview of terminal plotting approaches and their trade-offs. Good introduction to the space for someone unfamiliar with terminal data visualization techniques.

---

## 4. Accessibility in Terminal Applications

### 4.1 The Text Mode Lie: Why Modern TUIs Are a Nightmare for Accessibility (xogium)
- **URL:** https://xogium.me/the-text-mode-lie-why-modern-tuis-are-a-nightmare-for-accessibility
- **OSnews mirror:** https://www.osnews.com/story/144892/the-text-mode-lie-why-modern-tuis-are-a-nightmare-for-accessibility/
- **HN Discussion:** https://news.ycombinator.com/item?id=48002938
- **Format:** Blog post (web)
- **Rating:** ESSENTIAL
- **Summary:** Devastating critique from a blind developer: modern TUI frameworks (Ink, Bubble Tea, tcell) that treat the terminal as a reactive 2D canvas are actively hostile to screen readers. Every redraw triggers audio spam; cursor tracking overrides character echo; "modern" TUI stacks optimize for developer aesthetics at the expense of accessibility. Lists specific anti-patterns to avoid and names rare accessible examples (irssi, huh). This is the most important accessibility reference in our catalog -- read it before writing a single line of rendering code.

### 4.2 WCAG 2.1 Contrast Requirements (W3C Understanding SC 1.4.3)
- **URL:** https://www.w3.org/WAI/WCAG21/Understanding/contrast-minimum.html
- **Format:** W3C specification (web)
- **Rating:** HIGH PRIORITY
- **Summary:** The authoritative standard for color contrast accessibility. Regular text needs 4.5:1 contrast ratio (AA) or 7:1 (AAA). Large text needs 3:1 (AA) or 4.5:1 (AAA). Non-text UI components need 3:1. These ratios must inform our color palette design, especially since we will be rendering dense data with color coding.

### 4.3 Developing a Keyboard Interface (W3C WAI ARIA APG)
- **URL:** https://www.w3.org/WAI/ARIA/apg/practices/keyboard-interface/
- **Format:** W3C specification (web)
- **Rating:** HIGH PRIORITY
- **Summary:** Authoritative guidelines for keyboard interface design from the WAI-ARIA Authoring Practices Guide. Covers focus management, keyboard navigation patterns, and consistent use of standard keyboard conventions. Since our TUI is entirely keyboard-driven, this is our primary reference for keyboard interaction design.

### 4.4 SparkBraille: Braille Line Charts for Refreshable Displays
- **URL:** https://fizzstudio.github.io/sparkbraille/
- **Format:** Open source project (web)
- **Rating:** USEFUL
- **Summary:** Enables blind users to quickly grasp chart trends using a single-line refreshable braille display. Uses Unicode Braille (U+2800 to U+28FF) to encode data. Interesting intersection of our data visualization needs and accessibility requirements -- could inform a mode where our heatmaps/sparklines work on braille displays.

---

## 5. Dual-Interface Design (Human + Machine)

### 5.1 GDB/MI: The Machine Interface
- **URL:** https://sourceware.org/gdb/current/onlinedocs/gdb.html/GDB_002fMI.html
- **Format:** Official documentation (web)
- **Rating:** ESSENTIAL
- **Summary:** The canonical example of dual-interface debugger design. GDB/MI provides machine-parseable structured output alongside human-readable CLI output, both running over the same debugger instance. Key design: variable objects (named handles for expressions/memory/registers with tree-structured complex types), async notifications, and multi-target support. Study this as the proven approach to our human+LLM dual interface, while noting its limitations (text-based protocol, no schema, ambiguous parsing).

### 5.2 Debug Adapter Protocol (DAP) -- Overview
- **URL:** https://microsoft.github.io/debug-adapter-protocol/overview
- **DeepWiki:** https://deepwiki.com/microsoft/debug-adapter-protocol
- **Format:** Specification + documentation (web)
- **Rating:** ESSENTIAL
- **Summary:** The modern answer to GDB/MI. DAP introduces an intermediary "Debug Adapter" that translates between a generic debugger UI and specific debugger implementations, reducing M*N integrations to M+N. Wire protocol (JSON over stdin/stdout), not a library API. Each debug session spawns a separate adapter process. This architecture directly informs our design: rocket_surgeon's structured protocol could be DAP-compatible or DAP-inspired.

### 5.3 Neovim RPC Architecture: Channels and MessagePack-RPC
- **URL:** https://deepwiki.com/neovim/neovim/4.4-rpc-and-job-management
- **API Layer:** https://deepwiki.com/neovim/neovim/4.1-api-layer
- **Original RFC:** https://github.com/neovim/neovim/pull/509
- **Format:** Documentation + source code (web)
- **Rating:** ESSENTIAL
- **Summary:** Neovim's three-layer RPC architecture (Channel, MessagePack-RPC, API Dispatch) is the gold standard for editor/tool dual-interface design. Key innovations: auto-generated API bindings from metadata, LIFO response ordering for nested RPC calls, sequential event processing. Neovim embeds headless and any UI (TUI, Qt, web) connects as an RPC client. This is the closest architectural precedent for rocket_surgeon's "TUI for humans, protocol for LLMs" vision.

---

## 6. Ratatui/tui-rs Ecosystem

### 6.1 Ratatui Official Documentation
- **URL:** https://ratatui.rs/
- **Format:** Documentation site (web)
- **Rating:** ESSENTIAL
- **Summary:** The comprehensive official documentation covering concepts (rendering, layout, widgets, application patterns), tutorials (counter app, async patterns), recipes, and API reference. This is the starting point for all Ratatui development. Key sections: Layout (constraint-based with nested rects), Widgets (trait-based immediate-mode rendering), and Application Patterns (TEA, Component Architecture).

### 6.2 Ratatui ARCHITECTURE.md
- **URL:** https://github.com/ratatui/ratatui/blob/main/ARCHITECTURE.md
- **Format:** Markdown in source repo (web)
- **Rating:** HIGH PRIORITY
- **Summary:** Documents Ratatui's modular workspace reorganization (from v0.30.0). The monolithic crate was split into specialized sub-crates for modularity, faster compilation, flexible dependency management, and API stability for third-party widget libraries. Understanding this architecture is important for knowing which crates to depend on and how to structure our own widget code.

### 6.3 The Basic Building Blocks of Ratatui (kdheepak, 5-part series)
- **URLs:**
  - Part 1 (immediate mode): https://kdheepak.com/blog/the-basic-building-blocks-of-ratatui-part-1/
  - Part 2 (Rect + Layout): https://kdheepak.com/blog/the-basic-building-blocks-of-ratatui-part-2/
  - Part 3 (text primitives): https://kdheepak.com/blog/the-basic-building-blocks-of-ratatui-part-3/
  - Part 4 (widgets + blocks): https://kdheepak.com/blog/the-basic-building-blocks-of-ratatui-part-4/
  - Part 5 (custom widgets): https://kdheepak.com/blog/the-basic-building-blocks-of-ratatui-part-5/
- **Format:** Blog post series (web)
- **Rating:** ESSENTIAL
- **Summary:** The best independent deep-dive into Ratatui's primitives. Covers the rendering model (immediate mode with diff buffers), layout system (Rect + constraints), text primitives (Span, Line, Text), widget trait system, and custom widget construction. Written May 2024. This series explains the "why" behind Ratatui's design, not just the "how."

### 6.4 Ratatui Component Architecture Pattern
- **URL:** https://ratatui.rs/concepts/application-patterns/component-architecture/
- **Format:** Documentation (web)
- **Rating:** HIGH PRIORITY
- **Summary:** Official guide to the trait-based component pattern where each component encapsulates its own state, event handlers (key, mouse), and rendering logic. Components have lifecycle methods (init, handle_events, render). This is the pattern we will likely use for structuring our debugger panels.

### 6.5 Ratatui Async Template
- **URL:** https://github.com/ratatui/async-template
- **Documentation:** https://ratatui.github.io/async-template/
- **Format:** Source code template + documentation (web)
- **Rating:** HIGH PRIORITY
- **Summary:** Official opinionated template for async Ratatui applications using Tokio + Crossterm. Demonstrates channel-based async event handling (avoiding blocking the render loop), component lifecycle management, and the Action pattern for decoupled component communication. This is the starting point for our application structure.

### 6.6 RustLab 2024: Cooking up TUIs with Ratatui (Orhun Parmaksiz)
- **URL:** https://github.com/orhun/rustlab2024-ratatui-workshop
- **Talk page:** https://rustlab.it/talks/cooking-up-with-tuis-with-ratatui
- **Format:** Workshop materials + presentation (web)
- **Rating:** USEFUL
- **Summary:** Hands-on workshop from the Ratatui maintainer. Incrementally builds a terminal chat application (messages, files, images). Good for understanding the canonical way to structure a Ratatui application, though the complexity level is below what we need.

### 6.7 Crossterm: Cross-Platform Terminal Abstraction
- **URL:** https://github.com/crossterm-rs/crossterm
- **Docs:** https://docs.rs/crossterm/
- **Format:** Source code + documentation (web)
- **Rating:** HIGH PRIORITY
- **Summary:** The terminal backend library Ratatui uses. Pure Rust, cross-platform (all UNIX + Windows 7+). Key architecture: Command API with queuing (batch terminal operations to minimize syscalls), modular features (events, styling, cursor). Understanding Crossterm's command model is necessary for performance optimization in our render path.

---

## 7. Terminal Emulator Evolution & Philosophy

### 7.1 Ghostty: Reflecting on Reaching 1.0 (Mitchell Hashimoto)
- **URL:** https://mitchellh.com/writing/ghostty-1-0-reflection
- **About page:** https://ghostty.org/docs/about
- **Format:** Blog post (web)
- **Rating:** HIGH PRIORITY
- **Summary:** Mitchell Hashimoto's design philosophy for Ghostty: "fast, feature rich, and platform native together" should not require trade-offs. Zero-configuration philosophy (great defaults out of the box). Key insight: terminal emulators are built on a shaky historical foundation of in-band signaling and legacy escape sequences that hurts capability, performance, and security. Understanding where terminal emulators are going informs what capabilities we can eventually rely on.

### 7.2 Libghostty Is Coming (Mitchell Hashimoto)
- **URL:** https://mitchellh.com/writing/libghostty-is-coming
- **Format:** Blog post (web)
- **Rating:** USEFUL
- **Summary:** The long-term vision: libghostty as a reusable terminal emulator library, with the Ghostty app being just a "flagship tech demo." Envisions embedded terminals in editors, web-based terminals, and new multiplexers all built on the same core. Relevant to understanding the terminal ecosystem's direction and potential future embedding options for rocket_surgeon.

### 7.3 Kitty Terminal: Design Philosophy and Graphics Protocol
- **URL:** https://sw.kovidgoyal.net/kitty/graphics-protocol/
- **Overview:** https://sw.kovidgoyal.net/kitty/overview/
- **Format:** Specification + documentation (web)
- **Rating:** HIGH PRIORITY
- **Summary:** Kitty's graphics protocol enables rendering arbitrary pixel graphics in the terminal with alpha blending and scroll integration. Key design goals: terminal emulators should not need to understand image formats; graphics should integrate with text at individual pixel positions. GPU-accelerated via OpenGL. This protocol could enable rich tensor visualizations (heatmaps, attention plots) beyond what Unicode characters can express.

### 7.4 Why Terminal Multiplexers Are an Anti-Pattern (Jon Roosevelt / Kovid Goyal)
- **URL:** https://jonroosevelt.com/blog/terminal-design-philosophy-rethinking-multiplexers
- **Format:** Blog post (web)
- **Rating:** USEFUL
- **Summary:** Kovid Goyal's argument that terminal multiplexers (tmux, screen) are architecturally flawed: they implement a terminal inside a terminal, doubling processing cost and requiring two enormous state machines to be bridged in real-time. Modern terminals should offer composable primitives (remote control APIs, session management) instead. Relevant to our decision about how rocket_surgeon interacts with the user's terminal environment.

### 7.5 VT100 Terminal History (Columbia University)
- **URL:** https://www.columbia.edu/cu/computinghistory/vt100.html
- **Format:** Historical reference (web)
- **Rating:** BACKGROUND
- **Summary:** The DEC VT100 (1978) was among the first terminals to support ANSI escape codes, establishing the de facto standard that all terminal emulators still implement today. Understanding this heritage explains why terminal capabilities are what they are and why certain limitations persist.

---

## 8. Keyboard & Command Interface Design

### 8.1 The UX of Keyboard Shortcuts: Designing for Speed and Efficiency
- **URL:** https://medium.com/design-bootcamp/the-art-of-keyboard-shortcuts-designing-for-speed-and-efficiency-9afd717fc7ed
- **Format:** Blog post (web)
- **Rating:** HIGH PRIORITY
- **Summary:** Comprehensive treatment of keyboard shortcut design: the discoverability problem (90% of users never discover useful shortcuts per Google research), strategies for making shortcuts learnable (tooltips, key-to-verb mnemonics, conflict avoidance), and the three traits of good shortcuts (discoverable, memorable, conflict-free). Directly applicable to our keybinding design.

### 8.2 Command Palette UX Patterns (Alicja Suska)
- **URL:** https://medium.com/design-bootcamp/command-palette-ux-patterns-1-d6b6e68f30c1
- **Format:** Blog post (web)
- **Rating:** HIGH PRIORITY
- **Summary:** Deep dive into the command palette pattern: hotkey invocation, fuzzy matching, shortcut display for learning. Traces the history from Sublime Text's Ctrl+Shift+P through VS Code, Figma, Notion, and modern tools. The command palette is likely our primary discoverability mechanism for the LLM-friendly command set.

### 8.3 How to Design Great Keyboard Shortcuts (Knock)
- **URL:** https://knock.app/blog/how-to-design-great-keyboard-shortcuts
- **Format:** Blog post (web)
- **Rating:** USEFUL
- **Summary:** Practical guide to keyboard shortcut design covering mnemonic mapping, avoiding conflicts with OS/browser shortcuts, and progressive disclosure (simple shortcuts for common actions, chord sequences for advanced). Good checklist for our keybinding design review.

### 8.4 Guidelines for Keyboard User Interface Design (Microsoft)
- **URL:** https://learn.microsoft.com/en-us/previous-versions/windows/desktop/dnacc/guidelines-for-keyboard-user-interface-design
- **Format:** Technical guidelines (web)
- **Rating:** USEFUL
- **Summary:** Microsoft's authoritative guidelines for keyboard UI design covering focus management, tab order, access keys, and keyboard navigation patterns. While Windows-centric, the underlying principles (consistent conventions, visible focus indicators, logical tab sequences) apply universally.

---

## 9. Information Theory & Developer Cognition

### 9.1 Information Foraging Theory Applied to Developer Tools
- **URL:** https://dl.acm.org/doi/10.1145/2430545.2430551
- **IEEE:** https://ieeexplore.ieee.org/document/7739675/
- **Format:** Academic paper (PDF -- for ../papers/)
- **Rating:** HIGH PRIORITY
- **Summary:** Applies Pirolli & Card's Information Foraging Theory to software engineering: developers "forage" for information in code, debuggers, and documentation following "information scent" cues. The paper provides design patterns for developer tools based on IFT, explaining how to minimize "foraging cost" and maximize "information gain" at each interaction. Directly applicable to designing our debugger's information hierarchy -- what do we show first, what requires drilling down?

### 9.2 Debugging Neural Networks (Towards Data Science)
- **URL:** https://towardsdatascience.com/debugging-neural-networks-abdc6273a3f1/
- **Format:** Blog post (web)
- **Rating:** USEFUL
- **Summary:** Practical guide to neural network debugging workflows: monitoring activations, gradient flow analysis, weight distribution checks, and systematic hypothesis testing. While not about UI design per se, understanding the debugging workflow we need to support is prerequisite to designing the UI that supports it.

---

## 10. Charm Ecosystem (Cross-Reference for Patterns)

### 10.1 Bubble Tea: The Elm Architecture for Terminal UIs
- **URL:** https://github.com/charmbracelet/bubbletea
- **Format:** Source code + documentation (web)
- **Rating:** HIGH PRIORITY
- **Summary:** The most popular TUI framework in Go, implementing the Elm Architecture (Model-View-Update). While we build in Rust with Ratatui, Charm's design decisions are heavily studied and influential. Key patterns to study: Cmd/Msg system for side effects, nested model composition, and how they handle the view-is-a-pure-function constraint. Also study Lipgloss (CSS-like declarative styling) and Huh (accessible form components with screen reader support).

### 10.2 Beyond the GUI: Ultimate Guide to Modern TUI Applications (BrightCoding)
- **URL:** https://www.blog.brightcoding.dev/2025/09/07/beyond-the-gui-the-ultimate-guide-to-modern-terminal-user-interface-applications-and-development-libraries/
- **Format:** Blog post (web)
- **Rating:** USEFUL
- **Summary:** Comprehensive survey of the TUI landscape in 2025: frameworks (Ratatui, Textual, Bubble Tea, Ink), applications (lazygit, k9s, btop), and emerging trends (AI integration, collaborative features). Good for understanding the competitive landscape and user expectations for modern TUIs.

---

## Summary Statistics

| Category | Count | ESSENTIAL | HIGH PRIORITY | USEFUL | BACKGROUND |
|---|---|---|---|---|---|
| 1. TUI Design Philosophy | 10 | 4 | 3 | 2 | 1 |
| 2. Debugger UI Design | 6 | 3 | 1 | 2 | 0 |
| 3. Data Visualization | 7 | 1 | 3 | 2 | 1 |
| 4. Accessibility | 4 | 1 | 2 | 1 | 0 |
| 5. Dual-Interface Design | 3 | 3 | 0 | 0 | 0 |
| 6. Ratatui Ecosystem | 7 | 3 | 3 | 1 | 0 |
| 7. Terminal Evolution | 5 | 0 | 2 | 2 | 1 |
| 8. Keyboard & Commands | 4 | 0 | 2 | 2 | 0 |
| 9. Info Theory & Cognition | 2 | 0 | 1 | 1 | 0 |
| 10. Cross-Reference | 2 | 0 | 1 | 1 | 0 |
| **Total** | **50** | **15** | **18** | **14** | **3** |

## Recommended Reading Order

For the ESSENTIAL items, suggested sequence:

1. **xogium -- "The Text Mode Lie"** (accessibility constraints inform everything)
2. **Tufte -- Visual Display of Quantitative Information** (information design principles)
3. **Brandur -- "Learning from Terminals"** (design philosophy)
4. **McGugan -- "7 Things I've Learned"** (practical TUI framework lessons)
5. **Ratatui docs -- Rendering, TEA, Component Architecture** (our framework)
6. **kdheepak -- Building Blocks series** (Ratatui deep dive)
7. **GDB/MI documentation** (the dual-interface precedent)
8. **DAP overview** (the modern dual-interface standard)
9. **Neovim RPC architecture** (the gold standard for tool RPC)
10. **GDB TUI documentation** (what to avoid)
11. **nnd debugger** (what to aspire to)
12. **Comgra paper** (closest prior art for NN debugging)
13. **ChatDBG paper** (LLM-augmented debugging)

## Papers for ../papers/ Directory

These should be downloaded as PDFs:
- ChatDBG (arxiv 2403.16354)
- Comgra (arxiv 2407.21656)
- libdebug (arxiv 2506.02667)
- Information Foraging Theory for Developer Tools (ACM 10.1145/2430545.2430551)
- Tufte -- The Visual Display of Quantitative Information (book, acquire separately)
