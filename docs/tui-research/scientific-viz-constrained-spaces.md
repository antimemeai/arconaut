# Scientific Visualization in Constrained Display Spaces

Research report for the rocket_surgeon TUI intermission.
Covers principles, techniques, and concrete patterns for rendering high-dimensional
neural network data (tensors, attention matrices, activation distributions, routing
decisions) inside a terminal that might be 200x50 characters with optional pixel
graphics support.

---

## 1. Edward Tufte's Principles Applied to Terminals

### 1.1 Data-Ink Ratio

Tufte defines the data-ink ratio as (data-ink) / (total ink used in graphic). The
ideal is 1.0: every mark on the display represents data, nothing is wasted on
decoration. Terminals are naturally strong here. A character cell showing a Braille
scatter point or a colored block is almost entirely data. There are no anti-aliased
bezier borders, no drop shadows, no gradient fills consuming bandwidth for zero
informational gain. The constraint of the medium enforces the principle.

Practical implications for rocket_surgeon:

- **Borders and chrome**: Box-drawing characters around panels are non-data-ink. They
  consume an entire row/column per edge. A single-character vertical separator (a thin
  pipe `|` or a thin Unicode line `│`) costs one column. A blank-line separator costs
  zero columns. Prefer whitespace separation over drawn borders wherever panels are
  visually distinct by color or content type.
- **Axis labels**: Traditional axis ticks and labels consume 5-8 characters of width
  and 2-3 lines of height. For a sparkline showing 32 layer norms, that overhead
  dwarfs the data. Instead, embed the scale in the data: print the min and max values
  at the endpoints, use the sparkline itself as the axis.
- **Grid lines**: Never draw grid lines in a terminal visualization. The character grid
  itself provides implicit alignment. If you need reference lines (e.g., zero-crossing
  in a gradient plot), use a single distinct character or color change within the data
  area.

### 1.2 Small Multiples

Small multiples show the same visualization repeated across a partitioning variable,
using identical scales and axes so the eye can compare by position alone. Tufte
popularized the term and called them "the best design solution" for a wide range of
data presentation problems. They enforce both local and global comparisons without
context switching.

For neural network inspection, small multiples are the natural idiom:

- 32 layers of residual stream norms = 32 identical sparklines stacked vertically
- 32 heads per layer = 32 tiny attention heatmaps in a row
- 8 experts in an MoE layer = 8 identical load-distribution bars

The key design question is: how small can each multiple be before information is
destroyed? This is addressed in Section 2.

### 1.3 Sparklines

Sparklines are Tufte's invention: word-sized graphics embedded inline with text. They
achieve a data-ink ratio of 1.0 by eliminating all non-data elements---no frames, no
tick marks, no axes. The entire graphic is data.

Tufte's three overlapping principles for sparklines: maximize data density, minimize
or zero-out non-data, and the shrink principle (make the graphic as small as it can
be while remaining legible).

Terminal sparklines using Unicode block characters (`_`, `▁`, `▂`, `▃`, `▄`, `▅`,
`▆`, `▇`, `█`) give 8 vertical levels per character cell. A 32-character sparkline
can show all 32 layer norms in a single line of text, consuming exactly 32 columns
and 1 row. This is the most space-efficient visualization pattern available for
sequential scalar data.

Applications in rocket_surgeon:

- **Layer-wise norms**: `▁▂▃▄▅▆▇█▇▆▅▄▃▂▁▁▂▃▄▅▆▇█▇▆▅▄▃▂▁▁` --- 32 layers, 1 row
- **Gradient magnitudes**: same encoding, inline next to each layer's label
- **Expert load over time**: one sparkline per expert, stacked vertically
- **Training loss**: a running sparkline at the bottom of the screen

### 1.4 Micro/Macro Readings

Tufte's micro/macro principle states that well-designed information graphics support
hierarchical reading: a macro-level glance reveals aggregate patterns, while micro-level
inspection reveals individual data points. As Tufte put it: "to clarify, add detail."

This is directly relevant to the semantic zoom problem in a terminal debugger:

- **Macro reading** (glance at the full screen from arm's length): Is the model
  healthy? Are any layers anomalous? Is attention concentrated or diffuse? A grid of
  colored blocks, one per layer, gives this reading.
- **Micro reading** (lean in and read individual values): What is the exact norm of
  layer 17? What is the attention weight from token 5 to token 12 in head 7? When the
  user navigates to a specific element, the display should reveal numeric precision.

The design implication is that every visualization should have at least two
representations: a dense/compressed one for macro reading and an expanded one for
micro reading, switchable by navigation or zoom.

### 1.5 Chartjunk Elimination

Chartjunk is non-data visual ornamentation: 3D effects, excessive gridlines, moiré
patterns, decorative fills. In a terminal, most chartjunk is impossible by default.
But terminal-specific chartjunk exists:

- Unnecessary box-drawing borders (see 1.1)
- ASCII art logos or decorative headers consuming screen lines
- Redundant labels (labeling something "Attention Heatmap" when the context makes it
  obvious)
- Animated transitions that consume time without conveying information
- Color used for decoration rather than data encoding

The terminal's natural austerity is an asset. The principle: if a visual element does
not encode data or provide essential navigation context, delete it.

### 1.6 "Above All Else, Show the Data"

Tufte's foundational commandment. In rocket_surgeon, this means:

- The default view should be data, not menus or chrome
- When the user opens the debugger at a breakpoint, they should immediately see tensor
  values, not a help screen
- Every pixel of the terminal should be working to show the state of the model
- Navigation and command interfaces should be minimal and out of the way (a single
  status line, a command prompt that appears on demand)

---

## 2. Small Multiples for Neural Networks

### 2.1 The Scale Problem

A 32-layer, 32-head transformer produces 1,024 distinct attention matrices. Each
attention matrix is NxN where N is the sequence length (potentially 512, 2048, or
more). Displaying all of them simultaneously is impossible in any medium, let alone a
terminal.

The design response is hierarchical small multiples:

- **Level 0 (full overview)**: 32 layers x 32 heads = 1,024 single-character cells.
  Each cell is one colored character encoding a summary statistic (entropy, max
  attention weight, sparsity). This fits in 32 rows x 32 columns = a 32x32 block.
- **Level 1 (layer zoom)**: One layer selected, showing 32 heads as small attention
  thumbnails. Each thumbnail might be 5x5 characters (using Braille for 10x20 dot
  resolution). 32 thumbnails in a 8x4 grid = 40 columns x 20 rows.
- **Level 2 (head zoom)**: One head selected, showing the full NxN attention matrix at
  maximum available resolution, using Braille characters for 2x the horizontal and 4x
  the vertical resolution of raw character cells.

### 2.2 Grid Layout Strategies

**Flat grid**: All multiples arranged in a rectangular grid. Simple but can waste space
if the grid doesn't tile evenly into the terminal dimensions. Works well when multiples
are uniform and the count is a nice composite number (32 = 8x4, 4x8, 16x2).

**Faceted grid**: Two categorical variables mapped to rows and columns. For attention:
rows = layers, columns = heads. This makes it natural to compare across heads within a
layer (scan horizontally) or across layers for the same head (scan vertically).

**Hierarchical grouping**: Group by some structural variable first. For a model with
4 pipeline stages across GPUs, group layers by GPU, then by layer within GPU. This
aligns the visual structure with the computational structure the user is debugging.

### 2.3 Minimum Viable Multiple Size

How small can a single multiple be while retaining useful information?

- **1 character**: Encodes a single scalar summary (entropy, norm, sparsity) via color
  or symbol. Useful for Level 0 overviews.
- **3x3 characters (6x12 Braille dots)**: Can show coarse structure of an attention
  pattern---diagonal dominance, uniform attention, a few bright spots. Barely useful but
  surprisingly informative when the alternative is nothing.
- **5x5 characters (10x20 Braille dots)**: Can distinguish between diagonal, block, and
  sparse attention patterns. This is the practical minimum for small multiples of
  attention matrices.
- **10x10 characters (20x40 Braille dots)**: Can show meaningful detail for moderate
  sequence lengths. Individual token positions become resolvable for sequences up to ~40
  tokens.
- **20x20 characters (40x80 Braille dots)**: Full detail for short sequences, excellent
  macro reading for longer ones.

### 2.4 Visual Encoding Dimensions

Within a small multiple grid, additional information dimensions can be mapped to:

- **Color hue**: Which expert (categorical), which GPU (categorical)
- **Color intensity/lightness**: Magnitude of a value (sequential)
- **Border/outline**: Selected vs. unselected, anomalous vs. normal
- **Position within grid**: Layer index (row), head index (column)
- **Symbol/character**: Different states or categories using distinct Unicode characters
- **Sorting order**: Arrange multiples not by structural position but by a computed
  metric (entropy, norm deviation from mean) to surface anomalies. The most unusual
  heads appear first.

### 2.5 Sorting to Surface Anomalies

A grid of 32x32 attention patterns sorted by layer/head index is structurally correct
but visually monotonous---the eye has no entry point. Re-sorting by a divergence
metric (e.g., KL divergence from the layer mean, entropy, or norm) puts the most
interesting heads first. The user's eye is drawn to the top-left corner where the
outliers live. This is a crucial interaction: toggle between structural order (for
understanding position) and anomaly order (for finding problems).

---

## 3. Focus+Context Techniques

### 3.1 Fisheye Distortion

Fisheye views magnify the region around the user's focus while compressing peripheral
regions, keeping the entire dataset visible. The original formulation by Furnas (1986)
defined a degree-of-interest (DOI) function: DOI(x) = Intrinsic_Importance(x) -
Distance(x, focus).

In a terminal, fisheye distortion maps to variable row/column allocation:

- The focused layer gets 10 rows of detail.
- Adjacent layers get 2 rows each.
- Distant layers get 1 row each or are elided to a single summary line.
- The total still fits in the terminal height.

This is particularly natural for the layer-stack view: all 32 layers remain visible,
but the focused layer expands to show per-head detail or full tensor statistics while
distant layers collapse to a single sparkline.

### 3.2 Overview+Detail

The overview+detail pattern separates the overview and the detail into distinct
spatial regions, unlike fisheye which integrates them. A terminal implementation:

- **Left panel (20% width)**: Full model architecture minimap. Every layer is a single
  row. Color encodes a health metric. The currently focused layer is highlighted.
- **Right panel (80% width)**: Detailed view of the selected layer---per-head attention
  patterns, activation distributions, routing decisions.

This is the Bloomberg Terminal model applied to neural network inspection (see
Section 5.5). The overview panel acts as both a navigation aid and a macro reading
surface.

### 3.3 Semantic Zooming

Semantic zoom changes the representation qualitatively as the user zooms, rather than
just magnifying. Unlike geometric zoom (which scales all elements uniformly), semantic
zoom reveals different information at each level:

- **Zoom level 0 (full model)**: Each layer is one colored character. Color = health
  metric. This is a single column, 32 rows.
- **Zoom level 1 (layer detail)**: The selected layer expands to show 32 heads as
  small colored blocks plus aggregate statistics (mean norm, max gradient, routing
  balance).
- **Zoom level 2 (head detail)**: The selected head expands to show the attention
  matrix as a Braille-dot heatmap, with actual weight values on hover/selection.
- **Zoom level 3 (element detail)**: Individual attention weights shown as numbers,
  with context about source/target tokens.

The key design constraint: semantic zoom must maintain geometric stability. Nodes that
appear at a coarse level must persist (in the same relative position) at finer levels.
The user should never lose their spatial orientation during a zoom transition.

### 3.4 Brushing and Linking

Brushing and linking is the interaction pattern where selecting elements in one view
highlights corresponding elements in all other views. This is the foundational
interaction for multi-view coordinated visualization.

For rocket_surgeon:

- Select layer 17 in the architecture overview -> the tensor view shows layer 17's
  residual stream, the distribution view shows layer 17's activation histogram, the
  timeline view highlights layer 17's forward pass timing.
- Select tokens 5-10 in the sequence view -> the attention matrix highlights rows
  5-10, the activation view highlights positions 5-10, the routing view shows which
  experts handled tokens 5-10.
- Select a specific expert in the routing view -> all tokens routed to that expert are
  highlighted across all views.

Implementation requirement: a global selection state that all views subscribe to,
propagating selection changes as events. This aligns with rocket_surgeon's existing
subscribe+events architecture.

### 3.5 Degree-of-Interest Trees

Card and Nation's DOI trees (2002) extend Furnas's DOI function specifically for
hierarchical data. The DOI of each node combines intrinsic importance with distance
from the user's focus, and the tree layout dynamically allocates screen space based on
DOI values. Sibling nodes receive fractional DOI offsets based on their distance from
the focus sibling, so nearby siblings get more space than distant ones.

This is directly applicable to navigating a model hierarchy:

```
Model
  └─ Stage 0 (GPU 0)
      └─ Layer 0
          └─ Self-Attention
              └─ Head 0 ... Head 31
          └─ MLP / Expert Layer
              └─ Expert 0 ... Expert 7
      └─ Layer 1 ...
  └─ Stage 1 (GPU 1) ...
```

A DOI tree view allocates screen space proportional to interest: the focused expert
gets 10 lines of detail, its layer gets 5 lines of context, the enclosing stage gets
2 lines, and other stages collapse to 1 line each.

---

## 4. Dimensionality Reduction for Visualization

### 4.1 PCA, t-SNE, and UMAP

These are the three workhorses of embedding visualization:

**PCA** (Principal Component Analysis): Linear projection onto the directions of
maximum variance. Fast, deterministic, preserves global structure (distances between
distant points). No hyperparameters beyond the number of components. The go-to choice
for a quick "what does this activation space look like" view.

**t-SNE** (t-distributed Stochastic Neighbor Embedding): Non-linear, focuses on
preserving local neighborhood structure. Reveals clusters that PCA misses. But:
O(n^2) complexity limits it to ~10,000 points in real-time; perplexity hyperparameter
significantly affects the output; global distances between clusters are meaningless.

**UMAP** (Uniform Manifold Approximation and Projection): Non-linear like t-SNE but
faster, better at preserving global structure, and with fewer hyperparameters. The
current default choice for interactive embedding exploration.

### 4.2 The Lie of Dimensionality Reduction

It is mathematically impossible to map high-dimensional data to 2D without losing
information. The specific distortions to be aware of:

- **Cluster distances are not meaningful**: Two clusters that appear close in a t-SNE
  or UMAP plot may be far apart in the original space. Global relationships are
  sacrificed for local structure preservation.
- **Cluster sizes are not meaningful**: A tight cluster in the projection might
  correspond to a diffuse region in high-dimensional space, and vice versa.
- **Hyperparameter sensitivity**: Minor changes to perplexity (t-SNE) or n_neighbors
  (UMAP) can dramatically reshape the visualization. Results are not stable across
  parameter choices.
- **Density artifacts**: Densely grouped high-dimensional points may appear scattered,
  or scattered points may appear clustered.

For rocket_surgeon, this means:

- Always label DR plots with the method and parameters used.
- Provide controls to adjust DR parameters interactively so the user can probe
  stability.
- Offer PCA as a "honest" baseline alongside UMAP for "interesting structure."
- Never present DR output as ground truth about the activation space.

### 4.3 Interactive Dimensionality Reduction

The TensorFlow Embedding Projector demonstrated the value of interactive DR: users
can rotate 3D PCA projections, adjust t-SNE perplexity in real-time, and search for
specific points. For a terminal-based tool:

- Allow toggling between PCA and UMAP with a keybinding.
- Show explained variance ratio for PCA so the user knows how much information is
  captured.
- For UMAP, allow adjusting n_neighbors (local vs. global structure emphasis).
- Animate transitions between DR projections to maintain spatial context.

### 4.4 Terminal Rendering of DR Output

A 2D scatter plot in the terminal using Braille characters:

- Each Braille character cell is a 2x4 dot matrix, giving 2x horizontal and 4x
  vertical resolution compared to character cells.
- A 40-column x 20-row plotting area gives 80x80 effective dot resolution.
- With 256 colors or true color, each dot can encode a categorical or continuous
  variable via color.
- Overlapping points can use intensity (brighter = more points) or the character
  itself (Braille dots for sparse regions, full blocks for dense regions).

This is sufficient to render a meaningful UMAP projection of per-layer activations
(32 points) or per-token embeddings (up to several hundred tokens, with density
encoding for overlap).

---

## 5. Multi-View Coordinated Visualization

### 5.1 The Core Concept

Multi-view coordinated visualization presents the same underlying data through
multiple simultaneous visual representations, linked by interaction. Selecting or
filtering in one view propagates to all others. The power is that different views
expose different aspects of the data: structure, distribution, magnitude, temporal
evolution.

### 5.2 Views for Neural Network Debugging

For rocket_surgeon, the canonical view set:

- **Architecture view**: The model as a hierarchical structure. Layers, heads, experts.
  Navigation target. Shows which component is selected.
- **Tensor view**: The actual numeric content of the selected tensor. Heatmap,
  histogram, or raw values depending on zoom level.
- **Distribution view**: Statistical summary of the selected tensor(s). Histogram,
  box plot, or summary statistics (mean, std, min, max, percentiles).
- **Timeline view**: The forward pass as a sequence of ticks. Shows which tick is
  current, where breakpoints are, timing information. Perfetto trace data lives here.
- **Diff view**: Before/after comparison for surgical interventions. Diverging
  colormap centered on zero, showing what changed.

### 5.3 Selection Propagation

The interaction model:

1. User selects "Layer 12, Head 7" in the architecture view.
2. The tensor view updates to show Head 7's attention matrix.
3. The distribution view updates to show the distribution of attention weights for
   Head 7.
4. The timeline view highlights the tick corresponding to Layer 12's forward pass.
5. If a diff is active, the diff view shows the change in Head 7's attention matrix
   from the intervention.

This is a publish-subscribe pattern: the selection state is a shared observable,
and each view subscribes to selection changes. This maps directly onto rocket_surgeon's
existing event architecture.

### 5.4 Layout Strategies for Constrained Space

**Tiling**: All views visible simultaneously, each in a fixed region. Best when the
terminal is large enough (e.g., 200+ columns). The Bloomberg model (see 5.5).

**Tabbing**: Only one primary view visible at a time, with tab-key switching. Best for
very small terminals. Loses the "coordinated" aspect since views can't be compared
side by side.

**Stacking**: Views stacked vertically, each occupying the full width but a fraction
of the height. Natural for terminals, which are often wider than they are tall.
The architecture view might be 5 rows at the top, the tensor view 30 rows in the
middle, and the distribution view 10 rows at the bottom.

**Hybrid**: A persistent minimap/overview in a narrow side panel (15-20 columns), with
the remaining space dedicated to a single detailed view that can be switched. This is
the overview+detail pattern from Section 3.2.

**Adaptive**: Layout changes based on terminal dimensions. A 200x50 terminal gets
tiling; an 80x24 terminal gets stacking with a minimap; a tiny terminal gets tabbing.
The layout engine must be responsive.

### 5.5 The Bloomberg Terminal as Multi-View Archetype

The Bloomberg Terminal is the canonical example of extremely dense, multi-view,
coordinated data display in a constrained space. Key design patterns:

- **Information density over aesthetics**: Every pixel carries data. No whitespace
  purely for breathing room. This matches Tufte's data-ink principle.
- **Color as data, not decoration**: Colors encode asset classes, directional changes
  (green/red), alerts. Never used for visual appeal.
- **Keyboard-driven navigation**: All interaction through keyboard shortcuts and
  command-line function codes, not mouse pointing. This is directly relevant to TUI
  interaction design.
- **Persistent context panels**: Key reference data (positions, indices, alerts) is
  always visible. The equivalent in rocket_surgeon: model architecture and current
  tick position should always be visible.
- **Multiple synchronized data views**: Price chart, order book, news feed, and
  position summary all update in response to the same underlying security selection.
  This is exactly the coordinated multi-view pattern.

---

## 6. Color Encoding for Scientific Data

### 6.1 Sequential Colormaps

Sequential colormaps map a single continuous variable from low to high. They progress
from dark to light (or vice versa) through a color ramp. The perceptually uniform
options:

- **viridis**: Dark purple to yellow through blue and green. The current gold standard.
  Perceptually uniform, colorblind-safe, prints well in grayscale.
- **magma**: Dark to light through purple and orange. Higher contrast at the bright end.
- **inferno**: Similar to magma, darker overall. Good for dark terminal backgrounds.
- **plasma**: Blue to yellow through purple and pink. Slightly less uniform than
  viridis but higher saturation.
- **cividis**: Specifically optimized for deuteranopia (red-green colorblindness) by
  adjusting viridis.

For rocket_surgeon: viridis is the default for attention weights, activation
magnitudes, and any other non-negative scalar field. Inferno may be preferred for
dark terminal backgrounds where viridis's dark purple end disappears.

### 6.2 Diverging Colormaps

Diverging colormaps have a neutral midpoint (typically white or light gray) with two
distinct color progressions extending in each direction. Essential for data centered
on zero:

- **RdBu (Red-Blue)**: The standard for "positive vs. negative" data. Red for positive,
  blue for negative, white for zero.
- **coolwarm**: Similar to RdBu but with smoother perceptual properties.
- **BrBG (Brown-Blue-Green)**: An alternative that avoids red-green confusion.

Applications in rocket_surgeon:

- **Gradient visualization**: Diverging colormap centered on zero. Red = positive
  gradient, blue = negative.
- **Tensor diffs**: Before/after intervention. The diverging colormap instantly shows
  what changed and in which direction.
- **Attention deviation**: Difference between actual attention and uniform attention.
  Highlights tokens receiving more or less attention than the baseline.

### 6.3 Categorical Colormaps

For discrete, unordered categories (which GPU, which expert, which attention head
type), use colormaps with maximum perceptual distance between colors:

- **Colorbrewer qualitative palettes**: Set1, Set2, Set3 (up to 12 distinct colors).
- **Tableau 10**: 10 maximally distinct colors.
- **For more categories**: Use a combination of hue and brightness or pair colors with
  distinct symbols.

For rocket_surgeon: MoE expert assignment (up to 8-16 experts) maps well to a
categorical palette. GPU assignment (2-8 GPUs) similarly.

### 6.4 Perceptual Uniformity

A perceptually uniform colormap ensures that equal steps in data value produce equal
steps in perceived color difference. Non-uniform colormaps (like the infamous "jet"
or "rainbow") create visual artifacts---bands of apparent discontinuity where the
colormap has steep perceptual gradients, and regions of apparent uniformity where it
has flat perceptual gradients. These artifacts mislead the user into seeing structure
that doesn't exist in the data.

This matters for scientific debugging: if a gradient visualization shows a "hot spot"
that's actually a colormap artifact, the user wastes time investigating a phantom.
Always use perceptually uniform colormaps for quantitative data.

### 6.5 The 256-Color Constraint

XTerm-256color provides:

- Colors 0-15: Standard ANSI (system-dependent, unreliable for data encoding).
- Colors 16-231: A 6x6x6 RGB color cube (216 colors). Each channel has 6 levels:
  0, 95, 135, 175, 215, 255.
- Colors 232-255: A 24-step grayscale ramp.

Mapping a perceptually uniform colormap to this palette requires nearest-neighbor
matching in a perceptual color space (CIELAB or similar). The 6-level channels mean
only 6 distinct lightness levels per hue, which is coarse. Strategies:

- **Use the grayscale ramp** (24 levels) for luminance-only encoding. This is
  surprisingly effective for attention heatmaps where the user only needs to see
  relative magnitude.
- **Map viridis to the nearest 256-color entries**: Pre-compute a lookup table. The
  result is coarser than true-color but preserves the essential ordering and
  perceptual uniformity.
- **Detect true-color support**: Most modern terminals support 24-bit color (16.7M
  colors). If available, use it. Fall back to 256-color mapping only when necessary.
- **Graceful degradation**: Design the visualization to work at 24-step grayscale,
  look better at 256-color, and look best at true-color. The information content
  should be preserved at all levels.

### 6.6 Colorblind-Safe Palettes

Approximately 8% of males and 0.5% of females have some form of color vision
deficiency, predominantly deuteranopia (red-green). Design requirements:

- Never use red-green as the only distinguishing dimension. Pair color with luminance,
  shape, or pattern.
- Viridis, cividis, inferno, and magma are all colorblind-safe by design.
- For diverging colormaps, BrBG (brown-blue-green) is safer than RdBu for
  deuteranopes, but RdBu is more intuitive for sighted users. Provide both as
  options.
- For categorical palettes, the "Okabe-Ito" palette (8 colors designed for universal
  accessibility) is the gold standard.
- Always ensure that luminance alone carries the primary data dimension. Color hue
  should be a secondary encoding that adds richness but is not required for basic
  reading.

---

## 7. Text-Based Scientific Visualization History

### 7.1 The FORTRAN Era

Scientific visualization in text began with line printers and FORTRAN `WRITE`
statements in the 1960s-70s. Scientists plotted functions by computing which character
position on each line corresponded to the Y value and printing an asterisk there. The
"overprint" technique---printing multiple characters on the same line by suppressing
the carriage return---allowed crude density encoding. These plots were ugly but
functional: they showed the data.

Printer plots had one virtue that modern graphical systems lost: they were
self-documenting. The plot was text, so it could be pasted into a report, emailed, or
logged without any format conversion. This is directly relevant to terminal
visualization: terminal output is text (with ANSI escape codes), which means it can
be captured, logged, piped, and processed by LLMs.

### 7.2 Gnuplot's Dumb Terminal

Gnuplot's `set terminal dumb` mode renders plots as ASCII characters in the terminal.
Default size is 79x24 characters. The aspect option controls the ratio of horizontal
to vertical tick marks (default 2:1, matching typical character cell aspect ratios).
The `block` terminal mode uses Unicode block or Braille characters for higher
resolution.

Gnuplot dumb terminal is the standard reference for "terminal plotting that works
everywhere." It demonstrates that useful scientific plots can be rendered in as few as
40x12 characters. Limitations: no color encoding, crude resolution without Unicode,
no interactivity.

### 7.3 Modern Terminal Plotting Libraries

**plotext** (Python): Full-featured terminal plotting---scatter, bar, histogram,
heatmap---using Unicode characters. Color support via ANSI codes. Syntax mirrors
matplotlib for familiarity. No external dependencies.

**plotille** (Python): Plotting in the terminal using Braille dots with foreground and
background colors. Lightweight, focused on Braille-resolution scatter and line plots.

**UnicodePlots.jl** (Julia): The state of the art in terminal plotting. Multiple
canvas types: BrailleCanvas (2x4 dot resolution per character), BlockCanvas
(half-block resolution), HeatmapCanvas (color-fill per character). Supports scatter,
line, histogram, heatmap, contour, and density plots with full 24-bit color.

**uniplot** (Python): Lightweight plotting for CI/CD pipelines, using Unicode for 4x
resolution. Designed for machine learning workflows where a quick visual check is
needed without launching a GUI.

**Textual/Rich** (Python): Textual's plotting capabilities via Rich integration offer
sparklines and bar charts in TUI applications with full color support.

### 7.4 Lessons from Text-Mode Visualization History

What worked:

- **Simplicity**: A sparkline or bar chart in the terminal is immediately readable.
  No learning curve.
- **Data density**: Braille characters achieve 80x80 effective dot resolution in a
  40x20 character area. Sufficient for scatter plots of hundreds of points.
- **Color as a multiplier**: Adding 256 or true-color to text-mode plots dramatically
  increases information capacity without changing the spatial layout.
- **Inline embedding**: Sparklines and small plots can be embedded within textual
  output (log lines, status bars), keeping data near the context that explains it.

What didn't work:

- **ASCII-only rendering**: Without Unicode, resolution is too coarse for anything
  beyond the simplest plots. ASCII scatter plots using `*` and `.` are barely readable.
- **Mouse-based interaction**: Terminal mouse support is inconsistent and awkward.
  Keyboard-driven interaction is essential.
- **Complex chart types**: Pie charts, treemaps, and complex network graphs don't
  translate well to character cells. The medium favors: bar charts, line charts,
  sparklines, heatmaps, scatter plots.

---

## 8. Specific Visualization Patterns for rocket_surgeon Data

### 8.1 Layer-Wise Norms Plot

**Data**: 32 scalar values, one per layer, representing the L2 norm of the residual
stream after each layer.

**Visualization**: A horizontal sparkline, 32 characters wide. Each character uses the
8-level block character set (`▁▂▃▄▅▆▇█`) to encode the norm value, scaled between the
observed min and max. The min and max values are printed at the left and right ends.

**Enhancements**:

- Color each bar by its deviation from the mean (diverging colormap): red for
  unusually high, blue for unusually low, neutral for typical.
- Place a reference line at the mean using a different character or color.
- When a norm exceeds a threshold (potential numerical instability), highlight it with
  a bright/blinking indicator.
- Stack a second sparkline below showing the gradient norm for comparison.

**Space cost**: 32 columns x 1-2 rows. Fits in a status bar.

### 8.2 Attention Matrix Heatmap

**Data**: An NxN matrix of attention weights (N = sequence length), one per head.

**Small (5x5 characters, macro reading)**: Use Braille dots for 10x20 resolution.
Downsample the NxN matrix to 10x20 using max-pooling (to preserve bright spots) or
mean-pooling (to preserve overall density). Apply viridis colormap. This shows:
diagonal dominance (local attention), vertical stripes (attention to key tokens),
blocks (segment-level patterns).

**Medium (20x20 characters, mid reading)**: Braille dots give 40x80 resolution.
Sufficient for sequences up to ~80 tokens with direct mapping. For longer sequences,
downsample. Add axis labels for token positions at regular intervals.

**Large (full panel, micro reading)**: Each character cell encodes one attention weight
using color intensity. For an NxN matrix where N < terminal columns, this gives
per-token resolution. Include numeric values on hover/selection. Show row and column
labels (token text).

**Interactive enhancement**: Brushing---hover over a position in the heatmap, and the
source/target tokens are highlighted in a separate sequence view.

### 8.3 Token-Level Activations

**Data**: For each token position, a vector of activations (dimension d_model, e.g.,
768 or 4096). Across L layers, this forms an L x N matrix of summary statistics
(e.g., L2 norm at each position in each layer).

**Visualization**: A 2D heatmap with layers on the Y-axis and token positions on the
X-axis. Color encodes the summary statistic. This is the "residual stream over time"
view.

**Anomaly highlighting**: Compute the z-score of each cell relative to its row (layer)
or column (position). Overlay markers on cells that exceed a threshold. This
immediately surfaces tokens where a specific layer produces unusually high or low
activations.

**Space cost**: L rows (32) x N columns (sequence length). For sequences longer than
the terminal width, horizontally scroll with the focused token always visible, and
show a minimap at the top.

### 8.4 Routing Decisions (MoE)

**Data**: For each token, which K experts (out of E total) were selected, and with
what routing weights.

**Matrix view**: A tokens x experts matrix where each cell's color encodes the
routing weight (0 = not selected, bright = high weight). This is a sparse heatmap---
most cells are zero for K << E. Use a sequential colormap for non-zero weights and
leave zero cells as background color.

**Flow view**: A simplified Sankey-like diagram showing token groups flowing to experts.
In the terminal, render as a two-column layout: tokens on the left (grouped by their
primary expert assignment), experts on the right (sized by total load), with
connecting lines drawn using Unicode box-drawing characters. Width of connection
indicates routing weight.

**Summary view**: A bar chart of expert load (total routing weight received by each
expert). Deviations from uniform load indicate routing imbalance---a common problem in
MoE training. A sparkline of load-balance entropy over training steps shows whether
the problem is getting better or worse.

### 8.5 Gradient Magnitude

**Data**: A scalar per layer representing the gradient norm during backward pass.

**Visualization**: Identical to the layer-wise norms sparkline (Section 8.1) but for
gradients. The key difference is the health interpretation:

- Gradient norms that decrease monotonically from output to input = potential vanishing
  gradients.
- Gradient norms that increase monotonically = potential exploding gradients.
- Gradient norms that are roughly uniform across layers = healthy training.

**Enhancement**: Overlay a reference band showing the "healthy" range. Use the
diverging colormap not centered on zero but centered on the mean gradient norm, to
highlight layers that deviate from the average.

### 8.6 Tensor Diff (Before/After Intervention)

**Data**: Two tensors of the same shape---the original and the modified version after
surgical intervention. The diff is their element-wise difference.

**Visualization**: A heatmap using a diverging colormap (RdBu or BrBG) centered on
zero. Blue = decreased, red = increased, white = unchanged. The colormap's scale
should be symmetric around zero, set to the maximum absolute change.

**Enhancements**:

- Show the diff alongside the original and modified tensors (three-panel layout).
- Compute summary statistics of the diff: mean absolute change, max change, fraction
  of elements changed by more than threshold.
- Highlight the top-K most changed elements with distinct markers.
- For attention matrices: show how the intervention redistributed attention weight
  (where did the probability mass move from and to?).

**Space cost**: Same as the tensor view (Section 8.2) but using a diverging colormap.
The three-panel layout (original / modified / diff) requires 3x the horizontal space,
so this benefits from tabbing or sequential display.

---

## 9. Synthesis: A Unified Design Philosophy

The research converges on a unified approach for rocket_surgeon's TUI visualization:

1. **Sparklines as the atomic unit**: Every scalar-per-layer metric gets a sparkline.
   These are the cheapest visualization in the system---1 row, 32 columns---and they
   provide immediate macro reading of model health.

2. **Small multiples as the comparison unit**: When comparing across heads, experts,
   or layers, use small multiples with identical scales. Sort by anomaly metrics to
   surface problems.

3. **Semantic zoom as the navigation model**: Every visualization has 3-4 levels of
   detail, from single-character summary to full numeric precision. Zooming in changes
   the representation, not just the scale.

4. **Coordinated views as the interaction model**: All views share a selection state.
   Selecting a layer, head, token, or expert propagates across all visible views.
   This is brushing and linking.

5. **DOI-weighted layout as the space allocation model**: Available terminal space is
   allocated proportional to user interest. The focused element gets the most space,
   context elements get proportionally less, and distant elements collapse.

6. **Perceptually uniform color as the encoding layer**: Viridis for sequential,
   RdBu/BrBG for diverging, Okabe-Ito for categorical. Graceful degradation from
   true-color to 256-color to grayscale.

7. **Data-ink ratio approaching 1.0**: Minimize all non-data elements. No decorative
   borders. No chartjunk. Every character cell carries information.

---

## Bibliography

### Books

1. Tufte, E. R. (1983). *The Visual Display of Quantitative Information*. Graphics
   Press.
2. Tufte, E. R. (1990). *Envisioning Information*. Graphics Press.
3. Tufte, E. R. (2006). *Beautiful Evidence*. Graphics Press. (Introduces sparklines.)
4. Wilke, C. O. (2019). *Fundamentals of Data Visualization*. O'Reilly Media.
5. Munzner, T. (2014). *Visualization Analysis and Design*. CRC Press.
6. Ware, C. (2012). *Information Visualization: Perception for Design*, 3rd edition.
   Morgan Kaufmann.

### Papers

7. Furnas, G. W. (1986). Generalized fisheye views. *Proceedings of CHI '86*,
   pp. 16-23. ACM. (Original fisheye / degree-of-interest formulation.)
8. Card, S. K. and Nation, D. (2002). Degree-of-Interest Trees: A Component of an
   Attention-Reactive User Interface. *Proceedings of AVI '02*, pp. 231-245. ACM.
9. Cockburn, A., Karlson, A., and Bederson, B. B. (2009). A Review of
   Overview+Detail, Zooming, and Focus+Context Interfaces. *ACM Computing Surveys*,
   41(1), Article 2.
10. van der Maaten, L. and Hinton, G. (2008). Visualizing Data using t-SNE. *Journal
    of Machine Learning Research*, 9, pp. 2579-2605.
11. McInnes, L., Healy, J., and Melville, J. (2018). UMAP: Uniform Manifold
    Approximation and Projection for Dimension Reduction. *arXiv:1802.03426*.
12. Moreland, K. (2009). Diverging Color Maps for Scientific Visualization.
    *Proceedings of the 5th International Symposium on Visual Computing*, pp. 92-103.
13. Nuñez, J. R., Anderton, C. R., and Renslow, R. S. (2018). Optimizing colormaps
    with consideration for color vision deficiency to enable accurate interpretation
    of scientific data. *PLOS ONE*, 13(7), e0199239.
14. Roberts, J. C. (2007). State of the Art: Coordinated & Multiple Views in
    Exploratory Visualization. *Proceedings of CMV '07*, pp. 61-71. IEEE.
15. Yeh, C., Czajka, B., Engel, N., et al. (2023). AttentionViz: A Global View of
    Transformer Attention. *arXiv:2305.03210*.
16. Vig, J. (2019). Visualizing Attention in Transformer-Based Language
    Representation Models. *arXiv:1904.02679*.
17. Clark, K., Khandelwal, U., Levy, O., and Manning, C. D. (2019). What Does BERT
    Look At? An Analysis of BERT's Attention. *Proceedings of the 2019 ACL Workshop
    BlackboxNLP*, pp. 276-286.
18. Bernstein, M. N. (2024). Assessing the utility of data visualizations based on
    dimensionality reduction. Blog post / technical note.

### Software and Tools

19. gnuplot, "dumb" and "block" terminal drivers. http://gnuplot.info/docs_6.1/loc20421.html
20. plotille: Plot in the terminal using Braille dots. https://github.com/tammoippen/plotille
21. UnicodePlots.jl: Unicode-based scientific plotting for Julia.
    https://github.com/JuliaPlots/UnicodePlots.jl
22. plotext: Plotting on the terminal (Python). https://github.com/piccolomo/plotext
23. uniplot: Lightweight terminal plotting (Python). https://pypi.org/project/uniplot/
24. Term::Colormap: Colormaps for ANSI 256 color terminals.
    https://metacpan.org/pod/Term::Colormap
25. TensorFlow Embedding Projector. https://projector.tensorflow.org/

### Web Resources

26. Tufte's Principles of Data Visualization.
    https://thedoublethink.com/tuftes-principles-for-visualizing-quantitative-information/
27. Sparklines History by Tufte: 1324 to now.
    https://www.edwardtufte.com/notebook/sparklines-history-by-tufte-1324-to-now/
28. Color Map Advice for Scientific Visualization (Kenneth Moreland).
    https://www.kennethmoreland.com/color-advice/
29. Xterm 256 colors reference. https://www.ditig.com/256-colors-cheat-sheet
30. Okabe, M. and Ito, K. Color Universal Design (CUD): How to make figures and
    presentations that are friendly to colorblind people.
    https://jfly.uni-koeln.de/color/
