# Dense Visualization at Scale: How the Best Tools Render Millions of Data Points

Research report for rocket_surgeon TUI intermission.
Date: 2026-05-19

---

## 1. Perfetto UI Architecture

Perfetto is the gold standard for rendering millions of trace events in a scrollable timeline. Its architecture solves the exact problem rocket_surgeon faces: dense, hierarchical, time-ordered data that must be interactively explorable.

### 1.1 The Trace Processor: Query Engine

The heart of Perfetto is the Trace Processor, a portable C++17 library that ingests traces of various formats and stores them in a **custom, in-memory, columnar database** backed by SQLite's query engine. The data pipeline flows:

```
Raw Trace -> ForwardingTraceParser -> Format-Specific ChunkedTraceReader
          -> TraceSorter -> TraceStorage -> SQL Query Engine
```

The columnar storage is purpose-built for trace data. Core tables include `SliceTable` (time intervals with metadata), `CounterTable` (continuous values over time), `ProcessTable`, and `ThreadTable`. Events are fundamentally two types: **slices** (intervals of time with associated data) and **counters** (continuously varying values). The system processes traces in chunks for memory efficiency with large files [1].

In the browser, the Trace Processor runs as a **WebAssembly module**. This is fast enough for moderate traces but has limitations: WASM parsing is slower than native, and large files can freeze or crash the browser. For large traces (multi-GB), Perfetto supports offloading to a **native TraceProcessor server** running locally, which leverages full RAM and SSE instructions on x86_64 [2].

The critical insight for rocket_surgeon: **the query layer is separate from the rendering layer**. The UI issues SQL queries against the trace processor to fetch only the data needed for the current viewport. This decoupling means the rendering pipeline never sees millions of events -- it sees only the query result for the visible time window.

### 1.2 Canvas Rendering Pipeline

Perfetto renders trace timelines using HTML5 Canvas (2D context), not WebGL. The rendering architecture is track-based:

- Each row in the timeline is a **track** (thread track, counter track, etc.)
- `DatasetSliceTrack` is the core renderer for slice-based tracks, providing fine-grained control over geometry, layout, padding, and row height [3]
- A `TimeScale` object converts between trace timestamps (nanoseconds) and horizontal pixel coordinates via `timescale.timeToPx(time)` [3]
- Overlays are drawn on top of the canvas for interactive DOM elements (selections, tooltips, annotations) that would be inefficient to render in canvas directly

### 1.3 Level-of-Detail and Aggregation

When viewing a 60-second trace containing millions of slices, individual events may be sub-pixel width. Perfetto handles this through:

1. **Viewport-scoped SQL queries**: Only events overlapping the visible time window are fetched from the trace processor
2. **Pixel-width thresholding**: When a slice is narrower than ~1 pixel at the current zoom level, it is either merged with adjacent slices or rendered as a thin line
3. **Progressive detail**: Zooming in triggers re-queries that fetch more granular data for the narrower time window
4. **Summary tracks**: At very high zoom-out levels, some tracks switch to aggregate representations (histograms, density plots) rather than individual slices

### 1.4 Timeline Scrubbing Interaction

Perfetto's zoom and pan model uses WASD keyboard navigation plus mouse drag. The viewport is defined by a `(visStart, visEnd)` time range. Pressing 'F' centers the selected entity in the viewport; pressing 'F' again fits the slice to fill the viewport. Deep-linking supports passing `visStart` and `visEnd` parameters to control the initial viewport [4].

The interaction model is deliberately **not** a traditional scrollbar. Time is the primary axis, and the viewport is a window sliding over the full trace duration. This avoids the problem of mapping millions of events to a scroll position.

## 2. Chrome DevTools Performance Panel

Chrome DevTools flame charts share DNA with Perfetto (both originate from Google's tracing infrastructure) but are optimized for web developer workflows.

### 2.1 Architecture

The flame chart uses a **multi-layered, dual-chart architecture** [5]:

- Two `FlameChart` instances with separate data providers (network track and main thread track)
- A `SplitWidget` managing the vertical split
- An `Overlays` system for interactive DOM elements on top of canvas
- Data providers convert `ParsedTrace` data into `FlameChartTimelineData`

### 2.2 Rendering Optimization

The flame chart rendering is heavily optimized for traces with hundreds of thousands of events [5]:

- **Viewport culling**: Only entries within the visible time window are rendered
- **Group/level visibility**: Collapsed tracks are skipped entirely -- no iteration over hidden entries
- **Batched drawing**: Multiple entries at the same level with the same color are drawn in a **single `fillRect()` call**, dramatically reducing draw call overhead
- **Canvas + DOM hybrid**: The flame chart is canvas-rendered, but interactive overlays (selections, tooltips, breadcrumbs) are DOM elements positioned over the canvas

### 2.3 Deep Call Stacks

DevTools has a practical limitation of **64 stack frames per sample** [6]. For very deep stacks, the flame chart becomes vertically unwieldy. DevTools addresses this with:

- Click-and-drag panning in any direction
- WASD keyboard navigation (same as Perfetto)
- **Entry hiding**: Since Chrome 124, users can hide irrelevant entries (e.g., framework internals) via ignore lists, reducing visual clutter [7]
- **Long task highlighting**: Tasks over 50ms are highlighted with red corners, drawing the eye to bottlenecks

### 2.4 Summary vs. Detail Pattern

The Performance panel uses a consistent pattern:

1. **Overview strip**: A miniature timeline at the top showing CPU activity, network activity, and screenshots
2. **Flame chart**: The detailed view showing individual function calls
3. **Summary tab**: Aggregated statistics for the selected time range (self time, total time, by category)
4. **Bottom-up / Call tree / Event log**: Alternative detail views

This **overview + detail + summary** pattern is directly applicable to rocket_surgeon's layer/head/expert views.

## 3. Flame Graph Techniques

### 3.1 Brendan Gregg's Original Design

Flame graphs, created by Brendan Gregg and published in the ACM Queue (2016), are among the most successful dense data visualizations ever designed [8] [9]. The key design decisions:

**Width = proportion of total time.** The x-axis represents 100% of sampled time. Each rectangle's width shows the fraction of time that function was on-stack. This is the fundamental insight: by encoding the most important metric (time) as the most perceptually salient visual channel (width), rare events are thin but still visible, and dominant events are immediately obvious.

**X-axis is NOT time.** Gregg deliberately abandoned time-ordering on the x-axis. Since profiling uses sampling (not tracing), the function call flow is unknown between samples. Instead, samples are **reordered to maximize frame merging** -- identical call stacks are merged, and siblings are sorted alphabetically. This makes the visualization a *summary* of what happened, not a timeline.

**Y-axis = call depth.** Stack depth grows upward (flame metaphor). The topmost frames are the functions actually executing; wider top frames are the CPU bottlenecks.

**Color is deliberately low-information.** Gregg chose a warm color palette ("hot" = busy CPU) with random hue variation. Color carries no semantic meaning beyond broad categorization -- this is intentional, to avoid overloading the visual channel. The warm palette draws attention to width differences, not color differences.

### 3.2 Differential Flame Graphs

Differential flame graphs compare two profiles (before/after) using color to encode the delta [10]:

- **Red**: Function's execution time *increased* in the second profile
- **Blue**: Function's execution time *decreased*
- **Color saturation**: Proportional to the magnitude of the delta
- **Width**: Set by the second ("after") profile, showing current resource consumption
- **Colorization**: Based on the `(profile2 - profile1)` delta

The process: generate a flame graph using the second profile (which sets frame widths), then colorize by the delta. This answers "how did we get here?" while showing "where are we now?"

At Netflix, differential flame graphs are generated nightly for microservices to catch performance regressions [8].

### 3.3 Icicle Graphs

Icicle charts are inverted flame graphs -- stack depth grows *downward*. Some analysts prefer this orientation because it matches the mental model of "drilling down" into code. Gregg's `flamegraph.pl` supports this via `--inverted` [8]. The data and layout algorithm are identical; only the y-axis direction changes.

### 3.4 Inferno (Rust)

Inferno is a Rust port of the flamegraph toolkit focused on performance [11]:

- `inferno-collapse-perf` is **~20x faster** than `stackcollapse-perf.pl`
- Two-stage pipeline: stack collapsing (parsing profiler output) and plotting (SVG generation)
- Library interface via the `inferno` crate for integration with tools like `cargo-flamegraph`
- **Diffusion-based coloring**: Wider frames are rendered more red, visually pulling the eye toward the functions most likely to need optimization

### 3.5 Why Flame Graphs Work for Dense Data

The secret is **merging + width encoding**. A profile with 10 million samples and 50,000 unique functions collapses into a visualization with at most 50,000 rectangles. Of those, most are narrow (rare functions) and a few are wide (hot functions). The human eye immediately identifies the wide rectangles. The visualization degrades gracefully: even if the SVG contains thousands of thin rectangles, the visual message is dominated by the few wide ones.

Interactive features reinforce this: **search** highlights all matching frames with magenta; **zoom** expands a subtree to fill the viewport; **hover** shows exact percentages.

## 4. Bloomberg-Style Dense Data Display

Bloomberg Terminal's Launchpad monitors handle up to **2,000 securities per monitor** with ~30 columns of real-time data per security, yielding 60,000+ actively updating cells [12].

### 4.1 Architecture

Bloomberg's architecture is unusual [13]:

- The front-end is **server-side rendered in JavaScript** using SpiderMonkey
- Real-time elements require a completely different rendering path because the JavaScript environment restricts timers and callbacks (to prevent rendering stalls)
- The system ingests **millions of updates per second** without data loss
- Data is continuously validated and time-synchronized with centralized normalization

### 4.2 Cell-Level Update Strategy

The Bloomberg approach to dense real-time grids:

1. **Cell-level diffing**: Only cells whose values have changed are redrawn. With 60,000 cells and ~100 updates/second arriving, perhaps 200-500 cells change per frame. Only those cells are touched.
2. **Flash-on-update**: Changed cells briefly highlight (green for up, red for down) then fade. This draws the trader's eye to activity without requiring them to scan the entire grid.
3. **Magnitude color coding**: Background colors encode magnitude ranges. A bond yielding 4.5% might be yellow; 5.0% might be orange. This turns the grid into a heatmap at a glance.
4. **Sorting and grouping**: Securities can be grouped by sector, country, rating, etc., with group headers showing aggregate statistics. Sorting by any column instantly reorders rows.

### 4.3 Lessons for rocket_surgeon

The Bloomberg pattern maps directly to rocket_surgeon's needs:

- **Layer/head grid**: 96 layers x 96 heads = 9,216 cells. Each cell shows activation magnitude, gradient norm, or routing weight.
- **Cell-level diffing**: Between ticks, most cells change slightly. Only the cells with significant changes need visual update.
- **Flash-on-change**: When a surgeon modifies an activation, the affected cells flash to show propagation.
- **Magnitude color coding**: Blue-white-red diverging palette for activations; log-scale color for gradient norms.

## 5. Large Matrix Visualization

### 5.1 The 32768x32768 Attention Matrix Problem

A 32768x32768 attention matrix contains ~1 billion values. No display can show this at full resolution. The solution is **hierarchical aggregation with semantic zoom**.

### 5.2 Hierarchical Aggregation

The quadtree approach is the standard for 2D hierarchical aggregation [14]:

1. **Full zoom-out**: Divide the matrix into large blocks (e.g., 256x256). Each block is represented by its mean value (or max, or variance). The 32768x32768 matrix becomes 128x128 -- easily displayable.
2. **Intermediate zoom**: As the user zooms in, the quadtree subdivides. A 256x256 block splits into four 128x128 blocks, each with their own aggregate. The visible region shows finer blocks; regions outside the viewport remain coarse.
3. **Full zoom-in**: Individual cells become visible. The system fetches raw values for the visible region from the underlying data.

This is analogous to how map tiling works (Google Maps, OpenStreetMap). The key invariant: **at any zoom level, the number of visible cells is bounded by the viewport pixel count**.

### 5.3 Semantic Zoom for Matrices

Semantic zoom changes the **representation**, not just the scale [15]:

- **Zoom level 0 (overview)**: 128x128 block-averaged heatmap. Colors show which regions of the attention matrix are "hot."
- **Zoom level 1**: Transition to showing individual head structure. Block boundaries align with attention head boundaries.
- **Zoom level 2**: Individual token-to-token attention weights visible. Cell annotations show numeric values.
- **Zoom level 3**: Comparison mode -- show this attention pattern alongside the previous tick's pattern, with a diff overlay.

The critical principle: **each zoom level shows a qualitatively different kind of information**, not just a scaled version of the same data. At the overview level, the user sees "which heads are active." At the detail level, the user sees "which tokens attend to which."

### 5.4 Comparison and Diff Views

Side-by-side matrix comparison for rocket_surgeon surgery:

- **Before/after matrices**: Show the attention matrix before and after an activation modification
- **Diff matrix**: `abs(after - before)`, with a diverging colormap (blue = decreased attention, red = increased)
- **Threshold highlighting**: Only show cells where `abs(delta) > epsilon`, dimming everything else

This is the 2D equivalent of differential flame graphs: width (position) shows structure, color shows change.

## 6. Real-Time Streaming Data Visualization

### 6.1 Update Rate Tiers

Different data types demand different update strategies:

| Data Type | Update Rate | Strategy |
|-----------|------------|----------|
| Activation snapshots | ~1-10 Hz (per tick) | Snapshot, full redraw |
| Gradient norms | ~1-10 Hz | Snapshot, incremental diff |
| Routing weights (MoE) | ~1-10 Hz | Snapshot, cell-level diff |
| Loss curve | ~0.1-1 Hz | Append to ring buffer |
| GPU utilization | ~10-60 Hz | Rolling sparkline |

### 6.2 Ring Buffer Display Pattern

For time-series data that scrolls (loss curves, GPU metrics, activation norms over time):

1. Maintain a **ring buffer** of N most recent values (e.g., 1000)
2. Render as a **sparkline** that scrolls leftward as new values arrive
3. The ring buffer overwrites the oldest value, so memory is constant
4. Drawing: iterate from the current write position backward, mapping each value to a y-coordinate within the sparkline's bounding box

Ring buffers are thread-safe with a single writer: the writer advances the write pointer atomically; the reader reads from a snapshot of the pointer and works backward [16].

### 6.3 Backpressure

When data arrives faster than it can be rendered (e.g., 1000 activation snapshots/second but rendering takes 50ms per frame) [17]:

1. **Drop intermediate frames**: Keep only the latest snapshot. The UI always shows the most recent state, skipping intermediate states entirely.
2. **Aggregate on arrival**: Instead of storing every snapshot, maintain running statistics (min, max, mean, variance) and render those.
3. **Rate-limit at the source**: Signal the data producer to reduce update frequency.
4. **Buffered batch rendering**: Accumulate N snapshots, render the aggregate, then accept the next batch.

For rocket_surgeon, the appropriate strategy depends on the mode:
- **Step-through mode** (one tick at a time): No backpressure needed. Each tick is a discrete event.
- **Continuous run mode** (watching the forward pass stream by): Drop intermediate frames. Show latest state + rolling statistics.

## 7. Aggregation and LOD Strategies

### 7.1 M4 Algorithm

M4 (Jugel et al.) is the standard for **visualization-preserving time series downsampling** [18]. For each pixel-width bucket of data:

1. Take the **first** value in the bucket (preserves left edge)
2. Take the **last** value in the bucket (preserves right edge)
3. Take the **minimum** value (preserves valleys)
4. Take the **maximum** value (preserves peaks)

This yields exactly 4 points per pixel column, guaranteeing that the rendered line chart is **pixel-identical** to what you would see if you rendered all points. No visual information is lost. The algorithm is O(n) and scales linearly to billions of points.

The key insight: **you cannot display more information than you have pixels**. M4 exploits this by reducing data to exactly the information content of the display.

### 7.2 LTTB (Largest-Triangle-Three-Buckets)

LTTB (Steinarsson, 2013) takes a different approach to perceptual downsampling [19]:

1. Divide N points into M buckets (where M is the desired output size)
2. For each bucket, select the point that **maximizes the triangle area** formed with the selected points from the adjacent buckets
3. This preserves visual features: sharp turns, peaks, valleys, and trend changes

LTTB is O(n), deterministic, and produces visually excellent results. It is better than M4 when you want a *smooth* representation rather than a pixel-exact one (e.g., for sparklines where aesthetics matter).

### 7.3 MinMaxLTTB (Hybrid)

The `tsdownsample` library (Rust-backed, from predict-idlab) combines both approaches [20]:

1. **First pass**: MinMax preselection -- select `n_out * minmax_ratio` min and max values
2. **Second pass**: Apply LTTB to the preselected points, reducing to `n_out` output points

This hybrid is faster than pure LTTB on large datasets because the MinMax pass dramatically reduces the input size for the more expensive triangle-area computation. Performance: f16 argminmax is **200-300x faster than numpy**. Scales to 1 billion data points with sub-linear runtime increase.

### 7.4 Pixel-Aligned Aggregation for 2D Data

For 2D data (attention matrices, activation grids), extend M4/LTTB to two dimensions:

1. Determine the viewport in data coordinates (which rows/columns are visible)
2. Determine the pixel resolution of the viewport
3. For each pixel, aggregate all data values that map to that pixel
4. Aggregate function choices: mean (smooth overview), max (show hotspots), variance (show heterogeneity)

This is equivalent to **mipmapping** in GPU texture rendering. Precompute aggregates at multiple resolutions (like a quadtree or image pyramid), and select the appropriate level for the current zoom.

### 7.5 Progressive Refinement

Progressive refinement shows coarse data immediately and refines as computation time allows [21]:

1. **Frame 0**: Show the lowest-resolution aggregate (e.g., 32x32 block-averaged matrix)
2. **Frame 1-N**: Replace blocks with higher-resolution versions, starting from the center of the viewport
3. **Interaction interrupt**: If the user pans or zooms, restart from the coarse level for the new viewport

This is critical for rocket_surgeon because computing a 32768x32768 attention matrix visualization takes time. The user should see *something* immediately (the coarse aggregate), with detail filling in progressively. If they pan before refinement completes, the system responds instantly with the coarse view of the new region.

## 8. Performance Techniques for Terminal Rendering

### 8.1 Double Buffering with Diff (Ratatui)

Ratatui already implements the core optimization [22]:

1. The `Terminal` struct maintains **two buffers**: current and previous
2. During `terminal.draw(|frame| ...)`, widgets render to the current buffer
3. After drawing, the two buffers are **diffed**
4. Only **changed cells** generate ANSI escape sequences sent to the terminal
5. Buffers are swapped for the next frame

This achieves 60+ FPS even with complex layouts and real-time data. The diff eliminates redundant terminal writes, which are the primary bottleneck (each ANSI escape sequence requires terminal processing).

### 8.2 Dirty Region Tracking

Beyond cell-level diffing, rocket_surgeon can track dirty regions at a higher level:

1. **Region invalidation**: When an activation value changes, mark only the widget containing that value as dirty
2. **Skip clean regions**: Don't even compute the buffer contents for unchanged regions
3. **Partial render**: Only invoke the rendering logic for dirty widgets

This is especially valuable for the Bloomberg-style grid: if only 50 of 9,216 cells changed, skip the rendering computation for the other 9,166 entirely.

### 8.3 Async Rendering Pipeline

For expensive visualizations (attention matrix heatmaps, large flame graphs):

1. **Render thread**: Compute the visualization buffer in a background thread
2. **Display thread**: Show the most recently completed buffer
3. **Swap on completion**: When the render thread finishes, atomically swap the buffer into the display
4. **Cancel on interaction**: If the user pans/zooms before rendering completes, cancel the current render and start a new one for the updated viewport

This ensures the UI never blocks on expensive computation. The user always sees a responsive interface, even if the displayed data is slightly stale.

### 8.4 Frame Budgeting

Target: 16.67ms per frame for 60fps. In practice, budget 14-15ms to absorb spikes [23]:

```
Frame budget breakdown:
  - Input handling:     1ms
  - Data query/fetch:   3ms
  - Layout computation: 2ms
  - Widget rendering:   5ms
  - Buffer diff:        1ms
  - Terminal write:     2ms
  - Headroom:           2ms
  -------------------------
  Total:               16ms
```

When the budget is exceeded:

1. **Reduce resolution**: Increase the aggregation block size (show 64x64 blocks instead of 32x32)
2. **Skip non-essential widgets**: Render the primary view but defer sparklines, status bars
3. **Temporal amortization**: Spread expensive updates across multiple frames (update the left half of the heatmap this frame, the right half next frame)
4. **Adaptive quality**: Monitor frame times and automatically adjust aggregation level to maintain target FPS

### 8.5 Memory-Mapped Data Access

For large trace files and activation dumps [24]:

- **mmap** maps file contents directly into virtual address space
- Pages are loaded on demand -- only accessed regions consume physical RAM
- Enables working with files larger than available RAM
- Random access patterns (jumping to a specific tick) are efficient because the OS handles page faults
- Sequential scans benefit from kernel readahead

For rocket_surgeon, activation snapshots at each tick could be stored in a memory-mapped file. The TUI reads only the data for the current viewport from the mapped region. Jumping to tick N is an O(1) seek operation.

### 8.6 Terminal-Specific Pixel Graphics

For heatmaps and matrices in the terminal, rocket_surgeon has several options beyond text characters:

- **Block characters**: Unicode half-block (`▀`, `▄`) gives 2 vertical pixels per cell. Quarter-block characters give 2x2 = 4 pixels per cell.
- **Braille characters**: 2x4 dot matrix per cell = 8 pixels per character cell. Effective resolution: terminal columns * 2 by terminal rows * 4.
- **Kitty graphics protocol / Sixel**: Send actual pixel data to the terminal. Resolution limited only by the terminal's pixel dimensions (typically 1920x1080 or higher).
- **Full-width color cells**: Use background color on space characters. Each cell is one "pixel." With 256 columns and 64 rows, this gives 16,384 "pixels."

The choice depends on the terminal and the data density requirements. Braille characters offer the best balance of density and compatibility for most terminals.

## 9. Synthesis: Architecture for rocket_surgeon

The research points to a layered architecture:

```
Data Layer:
  - Memory-mapped activation/gradient snapshots
  - Ring buffers for time-series metrics
  - Columnar storage for trace events (tick metadata)

Query Layer:
  - Viewport-scoped queries: "give me data for ticks 100-200, layers 5-10"
  - Pre-aggregated LOD pyramid: coarse aggregates precomputed at ingest time
  - M4/LTTB downsampling for time-series views

Rendering Layer:
  - Ratatui double-buffered diff rendering (already available)
  - Cell-level dirty tracking for grid views
  - Async rendering for expensive visualizations
  - Frame budget monitoring with adaptive quality

Visualization Modes:
  - Bloomberg grid: layer x head cells with flash-on-change
  - Flame graph: function/module time breakdown per tick
  - Attention heatmap: semantic zoom from overview to individual weights
  - Sparkline dashboard: rolling metrics with ring buffers
  - Differential views: before/after comparisons for surgery verification
```

---

## Bibliography

[1] "Trace Processor Architecture," Perfetto Tracing Docs. https://perfetto.dev/docs/design-docs/trace-processor-architecture

[2] "Visualising large traces," Perfetto Tracing Docs. https://perfetto.dev/docs/visualization/large-traces

[3] "UI Plugins," Perfetto Tracing Docs. https://perfetto.dev/docs/contributing/ui-plugins

[4] "Perfetto UI," Perfetto Tracing Docs. https://perfetto.dev/docs/visualization/perfetto-ui

[5] "Flame Chart Visualization," DeepWiki ChromeDevTools. https://deepwiki.com/ChromeDevTools/devtools-frontend/5.1.2-flame-chart-visualization

[6] "Is flame graph call stack truncated?" Google Chrome Developer Tools Discussion. https://groups.google.com/g/google-chrome-developer-tools/c/ISxkm3hJAYg

[7] "3 new features to customize your performance workflows in DevTools," Chrome for Developers Blog. https://developer.chrome.com/blog/devtools-customization

[8] B. Gregg, "The Flame Graph," ACM Queue, vol. 14, no. 2, 2016. https://queue.acm.org/detail.cfm?id=2927301

[9] B. Gregg, "Flame Graphs," brendangregg.com. https://www.brendangregg.com/flamegraphs.html

[10] B. Gregg, "Differential Flame Graphs," brendangregg.com, 2014. https://www.brendangregg.com/blog/2014-11-09/differential-flame-graphs.html

[11] J. Gjengset, "Inferno: A Rust port of FlameGraph," GitHub. https://github.com/jonhoo/inferno

[12] "Bloomberg Terminal Essentials: IB, Worksheets & Launchpad," Bloomberg Professional Services. https://www.bloomberg.com/professional/insights/technology/bloomberg-terminal-essentials-ib-worksheets-launchpad/

[13] "The Bloomberg Terminal, Explained," Hacker News Discussion. https://news.ycombinator.com/item?id=21821327

[14] "QuadTree Visualizer," ResearchGate. https://www.researchgate.net/publication/360242672_QuadTree_Visualizer

[15] "Semantic Zoom: Interactive Multi-Level Visualization," EmergentMind. https://www.emergentmind.com/topics/semantic-zoom

[16] "Backpressure Mechanisms in High-Throughput Data Streams," Dev3lop. https://dev3lop.com/backpressure-mechanisms-in-high-throughput-data-streams/

[17] "What is backpressure in streaming data systems," Design Gurus. https://www.designgurus.io/answers/detail/what-is-backpressure-in-streaming-data-systems-and-how-can-a-system-design-handle-it-to-avoid-being-overwhelmed

[18] Jugel et al., "M4: A Visualization-Oriented Time Series Data Aggregation," ResearchGate. https://www.researchgate.net/publication/262763696_M4_A_Visualization-Oriented_Time_Series_Data_Aggregation

[19] S. Steinarsson, "Downsampling Time Series for Visual Representation," MSc Thesis, University of Iceland, 2013. Referenced via https://rajnandan.com/posts/largest-triangle-three-buckets-downsampling/

[20] Van den Bossche et al., "tsdownsample: high-performance time series downsampling for scalable visualization," SoftwareX, 2025. https://www.sciencedirect.com/science/article/pii/S2352711025000123

[21] "Progressive refinement," HandWiki. https://handwiki.org/wiki/Progressive_refinement

[22] "Rendering under the hood," Ratatui Docs. https://ratatui.rs/concepts/rendering/under-the-hood/

[23] "What Is a Frame Time Budget in Optimization?" PulseGeek. https://pulsegeek.com/articles/what-is-a-frame-time-budget-in-optimization/

[24] "Memory-Mapped I/O for Handling Files Larger Than RAM," DEV Community. https://dev.to/kherld/memory-mapped-io-for-handling-files-larger-than-ram-4o7k

[25] "Rendering One Million Datapoints with D3 and WebGL," Scott Logic Blog. https://blog.scottlogic.com/2020/05/01/rendering-one-million-points-with-d3.html

[26] "A Multiscale Visualization of Attention in the Transformer Model," Vig, 2019. https://arxiv.org/pdf/1906.05714

[27] J. Gjengset, "Inferno Flamegraph Options," Docs.rs. https://docs.rs/inferno/latest/inferno/

[28] "Clustergrammer: a web-based heatmap visualization and analysis tool," Nature Scientific Data. https://www.nature.com/articles/sdata2017151

[29] B. Gregg, "Visualizing Performance with Flame Graphs," USENIX ATC 2017. https://www.usenix.org/conference/atc17/program/presentation/gregg-flame

[30] "Virtual Scrolling for Billions of Rows -- Techniques from HighTable," RedNegra Blog. https://rednegra.net/blog/20260212-virtual-scroll/

[31] MinMaxLTTB paper, "MinMaxLTTB: Leveraging MinMax-Preselection to Scale LTTB," arXiv, 2023. https://arxiv.org/pdf/2305.00332

[32] "M4 Scalable Time Series Visualization," UW Interactive Data Lab, Observable. https://observablehq.com/@uwdata/m4-scalable-time-series-visualization

[33] "Accelerated 2D visualization using adaptive resolution scaling and temporal reconstruction," Journal of Visualization, Springer, 2023. https://link.springer.com/article/10.1007/s12650-023-00925-3
