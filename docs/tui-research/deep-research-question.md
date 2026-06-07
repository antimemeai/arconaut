# Deep Research Brief: The rocket_surgeon TUI

## For Sky-Claude — A Request for Magnum Opus

---

## I. What You're Being Asked

Design a terminal user interface for a neural network debugger that marries two things nobody has married before: the information density, keyboard supremacy, and trading-floor reliability of the Bloomberg Terminal with high-performance computational geometry rendering of tensor data and network topology — all inside a terminal emulator.

This is not a request for a wireframe or a feature list. We need a comprehensive design document that resolves the deep architectural and interaction-design tensions inherent in building a hybrid character-grid / pixel-graphics terminal interface for high-dimensional scientific data. We need you to think about this harder and more completely than anyone has thought about a TUI design before.

---

## II. The System: rocket_surgeon

rocket_surgeon is a debugger and in-situ surgery tool for multi-GPU transformer forward passes. It lets researchers step through transformer internals one tick at a time (forward and backward), inspect every tensor at every layer boundary, and surgically intervene (ablate heads, scale activations, patch tensors, clamp values) between steps.

### Architecture (Three-Process Model)

```
Client (TUI / LLM)
    |  JSON-RPC 2.0 (stdio / Unix socket)
    v
Daemon (Rust)           ← session state machine, dispatch, event delivery, Perfetto sink
    |  JSON-RPC (stdin/stdout pipes)
    v
Host (Rust + PyO3)      ← per-rank, embeds Python, runs forward pass
    |  PyO3 FFI
    v
Python + PyTorch        ← model loading, hooks, barriers, captures
    |
    v
GPU                     ← actual computation
```

Tensors move via POSIX shared memory ring buffer with BLAKE3 content-addressable IDs.

### Protocol Surface

11 verbs (initialize, attach, detach, step, inspect, intervene, probe, checkpoint, replay, status, subscribe) + 5 event types (tick.stopped, tick.heartbeat, probe.fired, replay.divergence, error). Every response carries a SessionState envelope with full position, active probes, available actions. The TUI is a pure view layer — all state lives in the daemon.

### What Exists

- 10 Rust crates, ~16.5K Rust LOC, ~4K Python LOC
- 24 TCK Gherkin feature files (227 scenarios), 499 passing Rust unit tests
- 7 ADRs, 12 design specs
- Complete protocol implementation (Phase 1)
- Shared memory tensor transport with BLAKE3 content-addressable IDs
- Perfetto trace sink for .pftrace output
- DTrace-inspired probe system (point grammar: `model:rank:layer:component:event`)
- Built-in views (residual_stream_norm, attention_pattern)
- Subscribe + event delivery (tick.stopped, heartbeat, probe.fired)
- The TUI crate is a scaffold: ratatui 0.30, crossterm 0.29, one `println!`

### Dual Interface

The TUI is one of two client interfaces. The other is the structured JSON-RPC protocol consumed directly by LLMs (and eventually wrapped as an MCP server). Same verbs, same state, same capabilities — different rendering targets. The protocol was designed LLM-first (state in every response, actionable errors, composable primitives). The TUI must make this same protocol feel native to human hands.

---

## III. The Vision

### The Bloomberg Side

The Bloomberg Terminal is the apex predator of information-dense, keyboard-driven professional interfaces. 40 years of accumulated wisdom. Our TUI must hit the same bar:

**Command model:** Bloomberg's `[SECURITY] <SECTOR> [FUNCTION] <GO>` maps to our probe grammar. Navigate to a component (`layer.12 → attn → head.7`), then operate on it (`inspect`, `intervene`, `step`). Context carries forward until explicitly changed. Every interaction is a navigation command.

**Information density:** Show everything. Use visual hierarchy to manage attention, not hiding mechanisms to reduce complexity. Bloomberg displays 280,000+ data items across 2,000 securities per monitor. Our equivalent: 32 layers × 32 heads × multiple tensor statistics × multiple views, all simultaneously visible at appropriate levels of detail.

**Keyboard supremacy:** Every action has a short mnemonic. Expert users navigate at the speed of thought. The keyboard is the primary interface; the mouse is an escape hatch. Typing is faster than clicking for professionals who have internalized the command vocabulary.

**Trading-floor reliability:** 100% of user actions produce immediate visual feedback. Keybindings NEVER fail, NEVER lag, NEVER conflict. The input loop is sacred — never blocked by rendering, data fetching, or anything else. If a render can't complete in the frame budget, degrade gracefully — drop resolution, show a coarser LOD, render a placeholder — but never stop responding.

**Color semantics:** Every color means something. Green/red for direction, yellow for commands, orange for headers, white for data. We need an equivalent color language for neural network data: activation magnitude, gradient health, routing confidence, anomaly detection, intervention state.

**Progressive disclosure, Bloomberg-style:** Never hide the mechanism for going deeper. Everything is visible. Drill-down is always one keystroke away. The user always knows there is more data available and exactly how to get it.

### The Computational Geometry Side

The data is inherently geometric. Transformers are towers of tensors with networks flowing through them. Numbers-only display misses the structure. We need genuinely high-performance visualization — not toy sparklines bolted onto a text grid, but real rendered geometry that shows the shape of the data:

**Attention patterns** as heatmaps — 32×32 to 128×128 matrices, color-coded, zoomable, comparable across heads and layers.

**The model as a tower** — a stratigraphic column where each layer is a stratum with visible properties (activation norms, gradient magnitudes, routing entropy). The tower IS the navigation structure. Zoom into a stratum to see its internal components.

**Network flow visualization** — data flowing through the architecture, color-coded by magnitude, animated tick-by-tick as the forward pass progresses. Interventions visible as modifications to the flow.

**Tensor landscapes** — activation distributions, weight structure, sparsity patterns rendered as actual visual fields, not just summary statistics.

### The Marriage

These two sides coexist in the same terminal frame. Character-grid panels for Bloomberg-style data density (tensor stats, probe tables, command bar, event log). Pixel-rendered panels for computational geometry (attention heatmaps, tower views, network flow). The two interact: navigate in the data grid, the geometry view updates; focus on a region in the geometry, the data grid follows. Coordinated multiple views, Bloomberg's 4-panel model, with a mix of text and pixel content.

---

## IV. What We Know (Synthesized Research)

We have conducted exhaustive research across 11 reports. Here are the synthesized findings that constrain and inform the design.

### Terminal Capabilities

**The emulator landscape is stratified.** At the top: Kitty, Ghostty, WezTerm — GPU-accelerated, supporting Kitty graphics protocol, Kitty keyboard protocol, synchronized output, truecolor, OSC 52 clipboard. In the middle: iTerm2, foot, Windows Terminal — varying subsets of modern features. At the bottom: macOS Terminal.app (256-color only, no graphics), mosh (drops all custom sequences), GNU Screen (limited passthrough).

**Capability detection is unreliable.** No single mechanism works. The practical approach is layered: environment variables → DA1/DA2 queries with timeout → DECRQM mode probing → XTGETTCAP where supported → terminfo fallback → user config override. Cache everything. Sticky failure.

**tmux is a terminal-in-terminal** that filters escape sequences through its own VT parser. Anything it doesn't understand gets dropped. DCS passthrough exists but is fragile. tmux is ubiquitous in ML/HPC environments, so we must handle it gracefully.

**VS Code Remote SSH** is a major deployment target (ML researchers on GPU clusters) but silently breaks OSC 52 and can leak Kitty keyboard sequences as literal text.

**The minimum viable terminal** is 256-color, basic CSI/OSC, alternate screen, UTF-8. The enhanced tier adds truecolor, synchronized output, Kitty keyboard protocol, OSC 52, graphics protocols.

### Graphics Protocols

**Kitty graphics protocol is the primary target** for pixel rendering. Four transmission methods: direct/base64, temp file, shared memory, stream. The **shared memory path (t=s)** is fastest — write raw RGBA to POSIX shm, send a ~100-byte escape sequence, zero encoding overhead. The **Unicode placeholder mechanism** (U+10EEEE with diacritics) lets pixel graphics live as regular cells in ratatui's layout grid.

**Sixel** is the fallback for terminals without Kitty support. 256 color registers typical. CPU-bound encoding. Overwrites text cells (can't layer text on Sixel).

**Character-based rendering** covers everything else:
- Half-block characters (▀▄) with foreground+background color: 2 pixels per cell, works in 256-color
- Braille characters (U+2800–U+28FF): 2×4 dots per cell, binary only (no color per dot)
- Sextant characters (U+1FB00): 2×3 grid per cell
- Block elements (▁▂▃▄▅▆▇█): 8-level sparklines in a single row
- Box-drawing characters: universal support for layouts and trees

**Graceful degradation path:** Kitty shm → Kitty base64 → Sixel → half-block truecolor → half-block 256-color → Braille → ASCII. The Bloomberg-style character grid works everywhere. The pixel graphics scale with terminal capability.

### Input & Interaction

**Keyboard encoding is chaotic.** Function keys have different byte sequences across terminal families. Ctrl+key collisions (Ctrl+H=Backspace, Ctrl+I=Tab, Ctrl+M=Enter) eliminate a large chunk of the keyboard space. The **Kitty keyboard protocol** fixes this with unambiguous key reporting, but support is limited to modern terminals (Kitty, Ghostty, WezTerm, foot).

**Modal editing (vim-style) is the only workable approach** for a debugger TUI — separate modes for navigation, command entry, and text input. This matches Bloomberg's command-line-first model.

**Mouse SGR mode (1006)** is the modern standard for mouse reporting — fixes the 223-column limit of older encodings. Pixel-level mouse reporting (mode 1016) exists but is spotty.

**crossterm** (our backend) supports Kitty keyboard protocol via PushKeyboardEnhancementFlags, mouse capture, focus events, and bracketed paste. Known limitations: no pixel mouse, ESC timeout ambiguity, no terminal identification queries.

### Rendering Architecture

**Perfetto's architecture is the model:** query layer decoupled from render layer. The UI never sees raw millions-of-events data. It issues viewport-scoped queries and renders only the result set. For us: the daemon holds the data, the TUI queries only what's visible at the current zoom level.

**ratatui's double-buffer diff** already achieves 60+ FPS for text. It compares frames and only emits escape sequences for changed cells. We layer on top: dirty-region tracking for pixel graphics, async rendering, frame budgeting.

**The rendering pipeline for pixel content:**
```
float[N][M] tensor data
  → quantize to [0,255] (one division + clamp per element)
  → index into precomputed 256-entry colormap LUT (one array lookup per element)
  → write RGBA pixel buffer
  → transmit via Kitty shm (write to POSIX shm + ~100-byte escape sequence)
```
Total for a 128×128 heatmap at 400×200 pixels: well under 2ms on CPU. No rendering library needed.

**GPU compute rendering via wgpu is not worth it** for our typical image sizes. GPU→CPU readback overhead (0.5–2ms) exceeds total CPU rendering time for images under ~2000×2000. Exception: batch rendering 96 attention heads simultaneously might benefit.

### Visualization Techniques

**Tensor visualization:** Treescope (Google) is the closest prior art for faceted N-dimensional array display. CircuitsVis and BertViz are the references for attention pattern heatmaps. The dimensionality problem (showing 4D tensors) is solved by slice selection + faceting + summary aggregation.

**Network visualization:** Sugiyama layout is correct for our DAG dataflow. Force-directed is explicitly wrong (destroys flow direction, nondeterministic). The "32 identical layers" problem has four solutions: isomorphic stacking, elision, accordion, indexed tower. The **stratigraphic tower** metaphor — geological column with per-layer annotations — is the right default view.

**Dense data at scale:** The M4 algorithm (4 points per pixel column: first, last, min, max) guarantees pixel-identical downsampling. LTTB provides perceptually smooth curves. Hierarchical aggregation for 2D data (quadtree/pyramid) with semantic zoom — different representations at different zoom levels.

**Scientific viz principles:** Tufte's sparklines as atomic unit. Small multiples for comparison (1024 attention heads shown as 1024 one-character summaries — anomalies jump out by color). Focus+context via semantic zoom. Coordinated multiple views. DOI-weighted layout (more space to the focused region). Data-ink ratio approaching 1.0.

### The Bloomberg Gospel

Key lessons from 40 years of the most successful professional terminal interface:

1. **Keyboard supremacy over mouse** — typing is faster for experts, period
2. **Information density is a feature** — visible complexity beats hidden simplicity for professional tools
3. **Consistency across all functions** — learn the patterns once, apply everywhere
4. **Security-context model** — load a thing, operate on it, context carries forward
5. **Color as semantic language** — every color means something, learned once
6. **Never hide information** — progressive disclosure means deeper access, not hidden access
7. **The learning curve is a feature** — expertise = speed = competitive advantage
8. **Incremental evolution, never revolution** — change the engine, keep the controls identical
9. **The 2008 font incident** — in expert systems, any visible change is initially perceived as damage
10. **The rejected IDEO redesign** — users took pride in mastering complexity; simplifying it undermined professional identity

---

## V. Design Principles & Constraints (Non-Negotiable)

1. **256-color palette, unimpeachably correct.** We would MUCH rather do 256 colors perfectly than introduce truecolor gradients and calculations. A precomputed 256-entry colormap LUT means colormap application is a single array index. Zero math, zero branching, zero surprises. Truecolor is a stretch goal, not the design target.

2. **Pay in memory/disk, never in compute.** Precompute everything. Cache rendered pixel buffers. Store pre-aggregated LOD tiers at ingest (pyramid: 128², 64², 32², 16², 8² — 1.33× memory, zero-latency zoom). Memoize graph layouts. A 100MB cache is nothing on a machine with 256GB RAM.

3. **Input loop is sacred.** Never block on render, data fetch, or anything else. Rendering is budgeted — if a frame takes too long, skip it and render fresher state. The input→response latency must be sub-frame (<16ms) under all conditions.

4. **C for the hot rendering path.** The rendering core (colormap kernels, heatmap rasterization, Sixel encoding, graph layout inner loops, pixel buffer generation) should be C with hand-tuned SIMD where it matters. Rust calls into C via FFI. The TUI shell, protocol handling, state management stay in Rust. This is the same Rust+C split the project uses for Rust+Python — clean boundaries, right language for each job.

5. **No external dependencies.** Prior art is reference, never a dependency. We study the algorithms, then reimplement. This means no cairo, no skia, no graphviz-the-binary. We build our own rendering pipeline, our own graph layout, our own colormap system.

6. **No OOP.** Functional/procedural. Data through functions, no class hierarchies.

7. **Dual interface is inalienable.** The TUI and the LLM protocol are equal citizens. Same protocol, same capabilities. The TUI renders for human eyes; the protocol emits JSON for LLM consumption. Feature parity is non-negotiable.

8. **Multi-GPU is the endgame.** Every design choice must support multi-rank visualization. Single-GPU is just a checkpoint. The tower view needs to show per-rank data. The heatmaps need to handle sharded tensors.

9. **Accessibility via the protocol.** The TUI's 2D canvas mode is hostile to screen readers (this is a known, fundamental limitation — "The Text Mode Lie"). The accessibility path is the structured protocol — screen reader users interact via the CLI/protocol mode. The TUI may optionally offer a `--simple` linear output mode.

10. **Bloomberg quality bar.** 100% of actions produce feedback. Keybindings never fail. Never stuck, never frozen, never blank. Graceful degradation on every axis. Professional tool reliability.

---

## VI. The Open Questions — What We Need You to Resolve

### A. Rendering Architecture

1. **The hybrid frame composition problem.** ratatui renders text cells via double-buffer diff. Kitty graphics protocol renders pixel images via escape sequences. These are fundamentally different rendering models living in the same terminal frame. How should the frame compositor work? When a text panel and a pixel panel share a border, how do we coordinate their updates? When the terminal is resized, how do we reflow text cells and recompute pixel regions simultaneously? What is the data flow from "state changed" to "frame on screen" for a frame containing both text and pixel content?

2. **The rendering pipeline in C.** We've established that the hot path should be C. Define the exact C API surface: what functions does Rust call into? What data crosses the FFI boundary? What memory ownership model? Propose a concrete `librocket_viz` (or whatever the right name is) C library structure with headers, data types, and function signatures. How does the LUT-based colormap system work at the API level? How does the pixel buffer lifecycle work (allocate, render, transmit, free)?

3. **Sixel encoding performance.** Sixel is notoriously slow to encode. For our 256-color palette approach, can we build a fast Sixel encoder that exploits knowing the palette at compile time? What SIMD optimizations are possible for Sixel encoding specifically? What's the realistic bandwidth for Sixel in a 60fps update loop?

4. **The LOD pyramid.** For an attention matrix, we store precomputed tiers: 128², 64², 32², 16², 8². What aggregation function is correct for each reduction? (Max? Mean? Something perceptually motivated?) How do we handle the transition between LOD tiers during zoom animation? When new data arrives (next tick), how do we incrementally update the pyramid without recomputing everything?

### B. Interaction Design

5. **The command grammar.** Bloomberg has `[SECURITY] <SECTOR> [FUNCTION] <GO>`. We have the probe grammar (`model:rank:layer:component:event`). Design the exact command syntax for the TUI. How does a user navigate from the top-level model view to a specific head's attention pattern? What are the mnemonic commands? How does context carry forward? How does this coexist with vim-style modal editing? Give concrete examples of expert-speed interaction sequences for common tasks (inspect a layer, set an intervention, compare two heads, step and watch a probe).

6. **The view system.** Bloomberg has 4 panels. We have coordinated multiple views. Define: what are the canonical views? (Architecture tower? Tensor inspector? Distribution panel? Event timeline? Command bar?) How many can be visible simultaneously? How are they arranged? How does selection propagate between views? Can views be swapped, resized, detached? What is the data model backing the view system — how does a "view" know what data to display when the user navigates to a new component?

7. **The color language.** Bloomberg has green=up, red=down, yellow=command, orange=header. Define: what does each color mean in rocket_surgeon? Activation magnitude, gradient health, routing confidence, anomaly, intervention state, selection, focus, error. Map these to a 256-color palette. Consider colorblind safety (we cannot use red/green for semantically opposed meanings — Bloomberg learned this the hard way). Provide the actual palette definition — 256 specific RGB values with semantic assignments.

8. **The zoom/drill interaction model.** The user is looking at a 32-layer tower view. They zoom into layer 12. The tower collapses to show layer 12 expanded with its internal components (attention, MLP, norm). They drill into attention. Now they see 32 heads. They focus on head 7. Now they see the full attention matrix. At each level, what do they see? What information is preserved from the outer levels (minimap? breadcrumb? header bar?)? How do they zoom back out? How does this interact with the command grammar?

9. **Keyboard binding map.** Define the complete keybinding scheme. What keys are safe to use across all terminal environments (including tmux, SSH, VS Code)? What keys require Kitty keyboard protocol and degrade gracefully? How do we handle the Ctrl+key collision problem? What are the mode transitions (normal → command → insert → visual)? What keys are reserved for future expansion?

### C. Visualization Design

10. **The attention heatmap at every scale.** Define exactly what an attention pattern looks like at each zoom level: as a 1-character summary (scalar → color?), as a 5×5 thumbnail (what colormap? what aggregation?), as a 20×20 small multiple, and as a full-resolution interactive matrix. What metadata is visible at each level? What interactions are available at each level?

11. **The stratigraphic tower.** Design the tower visualization in detail. What does each "stratum" (layer) show in its collapsed form? (Activation norm bar? Gradient magnitude? Routing entropy if MoE?) What does it show when expanded? How does the tower handle 32 layers in 40 rows of terminal? What about 128 layers (large models)? How does the tower show the forward pass progressing tick-by-tick — color change? Animation? Wavefront marker?

12. **Network flow visualization.** When the user is watching a forward pass step-by-step, how does the data flow through the tower/graph? Color change tracking as activations flow through layers — define the visual encoding. What does an intervention look like when it fires? What does a probe firing look like? What does a divergence (replay mismatch) look like?

13. **Tensor diff view.** Before and after an intervention, the user wants to see what changed. Define the diff visualization. Diverging colormap centered on zero? Side-by-side? Overlay with transparency? What about diffing tensor *distributions* rather than individual values? How do we handle the case where the shapes differ (e.g., different routing in MoE)?

14. **The small multiples array.** 32 layers × 32 heads = 1024 attention patterns. The user wants to see all of them at once to find the anomalous one. At the "all at once" scale, each pattern is at most 1-2 characters. Define the visual encoding: what single character represents a 128×128 attention matrix? (Entropy as a sparkline character? Dominant diagonal as a colored block? Sparsity as brightness?) How does the user scan, sort, filter, and select from 1024 tiny multiples?

### D. Architecture

15. **State management for views.** The TUI needs to track: which views are visible, what each view is focused on, what zoom level, what selection, what filters. This is UI state, separate from the daemon's session state. Define the state model. How does it serialize (for session save/restore)? How does it react to daemon events (tick.stopped, probe.fired)? How does it handle the case where the daemon's state invalidates the view's focus (e.g., model detached while the view is showing a tensor)?

16. **Event-driven architecture.** The daemon sends events (tick.stopped, heartbeat, probe.fired). The user presses keys. The render loop ticks. These are three independent event sources. Define the event loop architecture. How are events prioritized? How does a long-running render interact with incoming daemon events? How does the frame budget system work — what gets cut first when rendering exceeds 16ms?

17. **Plugin/extension model (future-proofing).** Bloomberg has 30,000 functions built by different teams following consistent patterns. We'll grow similarly (Phase 2 adds interventions, Phase 5 adds multi-GPU views, etc.). How should the view/panel system be designed so that new views can be added without modifying the core? What's the interface a new view must implement? How does it register its keybindings, declare its data dependencies, and integrate into the coordinated view system?

18. **Testing strategy for a TUI.** How do we test a visual, interactive terminal application with JSMNTL rigor? Gherkin scenarios for interaction sequences? Snapshot testing for rendered frames? Property-based testing for the rendering pipeline? Integration testing with a headless terminal (crossterm's TestBackend)? What does a TCK for the TUI look like?

### E. The Hard Problems

19. **The tmux problem.** Many of our users will be inside tmux on GPU servers. tmux eats graphics protocols and most advanced sequences. Define the tmux-aware degradation strategy in full detail. What capabilities do we probe for? How do we detect tmux-in-tmux? What is the absolute minimum TUI experience inside tmux, and is it still usable for real work? Can we use DCS passthrough for anything critical?

20. **The SSH latency problem.** GPU servers are often accessed via SSH with 20-100ms latency. The Bloomberg quality bar says "never stuck." How do we maintain sub-frame interaction responsiveness over a 100ms SSH connection? Optimistic local UI updates? Predictive rendering? What is the minimum data that must cross the wire for each user action?

21. **The terminal identification bootstrapping problem.** At startup, the TUI needs to discover what the terminal can do. But capability detection requires sending escape sequences and waiting for responses (or timeouts). This adds latency to startup. How do we bootstrap quickly? Can we start rendering with conservative assumptions and upgrade mid-session as capabilities are confirmed? What's the exact detection sequence and timeout strategy?

22. **Multi-GPU visualization layout.** With 4 or 8 GPUs, the tower becomes a forest. Per-rank data needs to be visible but not overwhelming. How does the layout adapt? Side-by-side towers? Interleaved views? A communication map overlay showing NCCL collectives? How does the command grammar extend to multi-rank (`rank.0:layer.12:attn:head.7`)?

---

## VII. What We Want Back

A complete TUI design document that:

1. Resolves every question in Section VI with concrete, implementable answers
2. Provides ASCII mockups of every major view and interaction state
3. Defines the exact rendering architecture (data flow diagrams, C API surface, memory model)
4. Specifies the command grammar with full syntax and examples
5. Defines the 256-color palette with semantic assignments
6. Maps the complete keybinding scheme
7. Describes the event loop and state management architecture
8. Addresses every degradation scenario (tmux, SSH, 256-color, no graphics, mosh)
9. Proposes a testing strategy compatible with JSMNTL rigor
10. Provides a phased implementation plan (what to build first, what depends on what)

This should be the kind of document you could hand to a team and they could build from it. Not hand-wavy principles — concrete specifications. Not "consider using a heatmap" — what exact colormap, what aggregation at each LOD tier, what pixel dimensions, what encoding. Bloomberg-level design specificity for a Bloomberg-level tool.

---

## VIII. Bibliography & Reference Material

The following research reports have been produced and are available as context. Each represents an exhaustive deep dive on its topic:

### Terminal Fundamentals
- **Terminal Emulator Landscape** (5,237 words) — per-emulator capability profiles for 13 terminals, capability detection mechanisms, escape sequence standards vs. reality, color tiers, Unicode rendering, multiplexer complications, remote access, accessibility
- **Terminal Graphics Protocols** (3,000+ words) — Sixel at the byte level, Kitty graphics protocol (all transmission modes, Unicode placeholders, animation), iTerm2 inline images, character-based graphics (Braille, half-block, sextant), color-as-visualization, graceful degradation tiers
- **Terminal Input & Interaction** (5,199 words) — keyboard encoding pipeline, Kitty keyboard protocol, mouse tracking modes, clipboard (OSC 52), focus events, crossterm specifics, interaction patterns for debugger TUIs
- **Bloomberg Terminal Gospel** (5,693 words) — 40-year history, keyboard and command system, information density philosophy, color semantics, progressive disclosure, what Bloomberg got right, what translates to neural network debugging

### Visualization Domain
- **Tensor Visualization Techniques** (5,284 words) — how 9 existing tools visualize tensors, the dimensionality problem, 7 transformer-specific visualization types, heatmap rendering techniques, histogram/distribution visualization, terminal achievability analysis
- **Network/Graph Visualization for NNs** (5,799 words) — Sugiyama layout for DAGs, the "32 identical layers" compaction problem, dataflow overlay, the "tower of tensors" / stratigraphic metaphor, interactive graph navigation, concrete terminal mockups
- **GPU-Accelerated 2D Rendering in Rust/C** (3,800+ words) — Rust 2D rendering landscape (tiny-skia, vello, lyon, wgpu), the rendering pipeline for terminal graphics, heatmap rendering benchmarks (0.05ms for 128×128), Kitty shm as fastest display path, graceful degradation via unified pixel buffer, GPU compute not worth it for our sizes
- **Dense Visualization at Scale** (4,000+ words) — Perfetto UI architecture (query layer decoupled from render layer), flame graph design, Bloomberg-style dense data display, M4/LTTB downsampling, hierarchical aggregation, semantic zoom, real-time streaming, frame budgeting
- **Scientific Visualization in Constrained Spaces** (6,162 words) — Tufte's principles applied to terminals, small multiples for 1024 attention heads, focus+context techniques, dimensionality reduction, multi-view coordination, color encoding science, text-based viz history, concrete rocket_surgeon patterns

### Literature & References
- **Literature Catalog** — 50 entries across 10 categories (15 ESSENTIAL, 18 HIGH PRIORITY), covering TUI design philosophy, debugger UI design, constrained-space visualization, accessibility, dual-interface design, ratatui ecosystem
- **72 fetched articles/specs as markdown** — including Mitchell Hashimoto on grapheme clusters, Chad Austin's truecolor deep dive, The Text Mode Lie (accessibility), Dan Luu's latency benchmarks, Kitty/Sixel/OSC 8 specs, Bloomberg UX blog posts, Tufte summaries, ratatui architecture docs
- **3 academic PDFs** — Comgra (neural network analysis tool), ChatDBG (LLM-augmented debugging), libdebug (programmatic debugger)
- **XTerm Control Sequences** — 3,835-line complete plaintext reference
- **10 quarantined reference repos** — BugStalker, nnd, helix, bottom, nviwatch, ratatui-image, yazi, gitui, zellij, csvlens

### Existing Project Documentation
- **Protocol README** — full verb table, state machine, error contract, transport plan
- **Capabilities spec** — phase-gated capability flags, client adaptation patterns
- **Architecture spec** — three-layer architecture, state machine, tick model, probe system, intervention system
- **7 ADRs** — language split, protocol design, probe model, three-process architecture, tick model, tensor handling, wire format
- **TUI frameworks lit review** — ratatui vs. textual vs. cursive, dual-interface design patterns (GDB-MI, Neovim RPC, DAP, tmux control mode), terminal graphics capabilities
- **LLM-native UX lit review** — function calling fundamentals, protocol design models, 7 design principles for LLM consumers, anti-patterns, concrete protocol sketches
