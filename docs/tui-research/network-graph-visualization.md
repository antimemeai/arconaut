# Network and Graph Visualization for Transformer Architectures

Research report for rocket_surgeon TUI intermission.
Date: 2026-05-19

---

## 1. Neural Network Architecture Visualization

### 1.1 How Existing Tools Show Network Structure

The landscape of neural network visualization is dominated by a handful of tools, each
taking a distinct approach to the same fundamental problem: how to render a computational
graph that may contain tens of thousands of operations into something a human can reason
about.

**Netron** is the de facto standard for static model inspection. Built on Electron, it
parses serialized model formats (ONNX, TorchScript, SafeTensors, Core ML, OpenVINO, and
roughly a dozen others) and renders the computational graph as a vertical node-link
diagram. Each operation becomes a node; edges represent tensor flow. Clicking a node
reveals its properties (kernel size, stride, weight shape, dtype) in a sidebar. Netron's
strength is breadth of format support and the one-click "load and see" workflow. Its
weakness: for large models, it renders the entire graph at once. A 70B parameter model
produces a graph so deep that scrolling becomes the primary interaction, with no structural
compression. Every MatMul, every Add, every Reshape gets its own box [1][2].

**TensorBoard's Graph View** introduced a critical innovation: hierarchical grouping via
name scopes. TensorFlow's `tf.name_scope` creates a nested namespace, and TensorBoard
uses "/" delimiters in node names to construct a collapsible hierarchy. Nodes within a
scope collapse into a single "MetaNode" with "MetaEdges" synthesized from the internal
connectivity. Users expand and collapse groups interactively, navigating from "the model"
down to "layer_3/attention/query_projection/MatMul". This hierarchical approach makes
TensorBoard the best existing solution for large model navigation, though its layout
algorithm struggles with graphs exceeding several thousand visible nodes [3][4].

**torchviz** takes the computational graph approach: given a PyTorch model and a dummy
input, it performs a forward pass, walks the autograd graph, and emits a Graphviz DOT
file. The resulting diagram shows the actual computation rather than the architectural
intent. This makes it excellent for understanding gradient flow but poor for
architectural overview---every backward-pass operation clutters the view [5].

**ONNX visualization** (via Netron or dedicated ONNX tools) operates on the intermediate
representation after export. Since ONNX flattens control flow and unrolls loops, the
graph for a transformer with 32 layers contains 32 near-identical subgraphs with no
structural sharing. This is the "flat graph explosion" problem.

### 1.2 The Transformer-Specific Challenge

Transformers expose a unique visualization problem: they are simultaneously repetitive
and internally complex. A GPT-style model has the structure:

```
embedding --> [layer_0 --> layer_1 --> ... --> layer_N] --> ln_final --> lm_head
```

Each layer is:

```
input --> ln1 --> attn(q_proj, k_proj, v_proj --> scaled_dot_product --> o_proj) --> residual_add
      --> ln2 --> mlp(gate_proj, up_proj --> activation --> down_proj) --> residual_add
```

The 32 layers are structurally identical but semantically distinct (early layers learn
syntax, late layers learn semantics). Showing all 32 expanded is overwhelming. Showing
them collapsed hides the interesting internal structure. The visualization must support
both: a collapsed "stack" view showing the tower of layers, with the ability to expand
any single layer to see its internal attention/MLP wiring [6].

### 1.3 MoE Routing Visualization

Mixture of Experts layers introduce a fundamentally different topology: the fan-out/fan-in
pattern. Where a dense transformer has a single MLP per layer, an MoE layer contains N
experts (often 8-64) with a router that assigns each token to a subset of experts.

The visual challenge is threefold:

1. **Router decision**: Show which tokens go where. Typically visualized as a matrix
   (tokens x experts) with color intensity indicating routing weight, or as a Sankey-like
   flow diagram where path width represents routing probability [7].

2. **Load balancing**: Show whether experts receive roughly equal token counts. Unbalanced
   routing creates "hot" experts and "cold" experts. Bar charts per expert showing token
   counts, or color-coded expert nodes (red = overloaded, blue = underutilized) convey
   this at a glance [7].

3. **Expert activation patterns**: Over a sequence, different tokens activate different
   expert subsets. Visualizing this across time reveals whether the router has learned
   meaningful specialization or is effectively random.

MixtureKit [8] provides both high-level visualization (each token colored by its dominant
expert) and low-level visualization (per-token expert-specific percentage breakdowns).
The KeepTopK selection mechanism is typically shown by graying out or zeroing
non-selected expert pathways, making the "active subnetwork" visually pop.

### 1.4 Multi-GPU Partitioning Overlay

For rocket_surgeon, the network graph must carry a second layer of information: device
placement. There are three partitioning strategies to visualize:

- **Tensor Parallelism (TP)**: A single layer is split across GPUs. The attention heads
  might be sharded: heads 0-15 on GPU 0, heads 16-31 on GPU 1. The visualization must
  show a single logical layer with internal partition boundaries [9].

- **Pipeline Parallelism (PP)**: Consecutive layers are assigned to different GPUs.
  Layers 0-7 on GPU 0, 8-15 on GPU 1, etc. The visualization naturally segments the
  vertical tower into colored bands per device [9].

- **FSDP/Data Parallelism**: Each GPU has a full copy. The overlay shows which parameters
  are currently materialized vs. sharded.

The partition overlay is best rendered as background coloring or border styling on nodes.
A subtle color wash (GPU 0 = blue tint, GPU 1 = green tint) behind the graph nodes
communicates device placement without obscuring the structural information. Boundary edges
(cross-device tensor transfers) should be visually distinct---dashed lines, different
color, or annotated with transfer cost.


## 2. Graph Layout Algorithms

### 2.1 Hierarchical/Layered Layout (Sugiyama)

The Sugiyama framework [10][11] is the gold standard for directed acyclic graph layout
and is overwhelmingly the right choice for neural network computational graphs. The
algorithm proceeds in four phases:

1. **Cycle removal**: Reverse a minimal set of edges to make the graph acyclic. Neural
   network forward-pass graphs are already DAGs (residual connections are still
   forward-flowing), so this phase is typically a no-op for our use case.

2. **Layer assignment**: Assign each node to a horizontal layer such that all edges point
   downward (or in the chosen rank direction). Nodes are placed at the minimum rank
   consistent with their dependencies. For a transformer, this naturally produces the
   tower structure: embedding at rank 0, layer_0 components at ranks 1-6, layer_1 at
   ranks 7-12, etc.

3. **Crossing minimization**: Within each layer, order nodes to minimize edge crossings.
   This is NP-hard in general, but the **barycenter heuristic** (position each node at
   the average x-coordinate of its neighbors in the adjacent layer) and **median
   heuristic** work well in practice. Multiple passes (alternating top-down and
   bottom-up sweeps) converge quickly [11].

4. **Coordinate assignment**: Compute exact x,y positions. The Brandes-Kopf algorithm
   produces compact, balanced layouts with good edge alignment.

**Why Sugiyama is right for rocket_surgeon**: Neural network forward passes are DAGs with
a dominant flow direction. The layer assignment phase naturally captures the "tower"
metaphor. The algorithm handles thousands of nodes efficiently when combined with
hierarchical grouping (collapse repeated layers into single nodes for layout, then
expand on demand).

Graphviz's `dot` engine [12] is the canonical implementation. Its `rankdir` attribute
controls flow direction (TB/BT/LR/RL). `cluster` subgraphs group related nodes. The
`rank=same` constraint forces nodes onto the same horizontal layer. These features map
directly onto transformer visualization needs.

### 2.2 Force-Directed Layout

Force-directed algorithms (Fruchterman-Reingold, Kamada-Kawai, ForceAtlas2) simulate a
physical system where nodes repel each other and edges act as springs. They produce
aesthetically pleasing layouts for undirected graphs with community structure, but they
are fundamentally wrong for neural network visualization [13]:

- They destroy the flow direction. A transformer's data flows top-to-bottom; force-
  directed layout places nodes in arbitrary positions.
- They produce the "hairball" problem at scale. With thousands of nodes, the algorithm
  converges to a tangled mess.
- They are computationally expensive: O(n^2) per iteration for naive implementations,
  O(n log n) with Barnes-Hut approximation. For our 1000+ node graphs, this is slow.
- They are non-deterministic: different runs produce different layouts, which is
  disorienting for a debugging tool where spatial memory matters.

Force-directed layout has exactly one use case in rocket_surgeon: visualizing attention
patterns as a graph where tokens are nodes and attention weights are edges. Here, the
lack of inherent directionality and the community-detection properties of force-directed
layout could reveal attention clustering patterns. But this is a secondary visualization,
not the primary architecture view.

### 2.3 Orthogonal Layout

Orthogonal layout restricts all edges to horizontal and vertical segments, producing
circuit-diagram-style drawings [14]. Nodes are placed on a grid, and edges are routed
along grid lines with 90-degree bends. This is visually clean and maximally readable but
space-inefficient for deep graphs.

For rocket_surgeon, orthogonal layout is appropriate for the "expanded single layer"
view: showing the internal wiring of one transformer layer (attention projections,
softmax, MLP gates) as a box-and-wire schematic. The small node count (10-20 operations
per layer) keeps the layout compact, and the right-angle routing makes data flow paths
unambiguous.

### 2.4 The "32 Identical Layers" Problem

This is the critical challenge. A naive graph with 32 identical layer subgraphs produces
a visualization that is 32x taller than necessary and provides no additional information
after the first layer.

Solutions from the literature and practice:

- **Isomorphic subgraph stacking** (Pan et al. [6]): Detect structurally identical
  subgraphs and collapse them into a single representative with a "x32" multiplicity
  indicator. This achieves up to 60% element reduction. The stacked representation shows
  one layer's structure with an annotation indicating repetition.

- **Elision with summary**: Show layer_0 expanded, then a compressed "... x30 identical
  layers ..." band, then layer_31 expanded. This prioritizes the first and last layers
  (which often differ slightly: different attention mask, final layer norm).

- **Accordion/expand-on-demand**: All layers start collapsed to single nodes in the
  tower view. The user expands any specific layer to see its internals. Multiple layers
  can be expanded simultaneously for comparison.

- **Indexed tower**: The tower is shown as a vertical stack of labeled bars (like a
  geological column). Each bar has a fixed height regardless of internal complexity.
  Selecting a bar opens a detail panel showing that layer's internal graph.

### 2.5 Scaling to Thousands of Nodes

Attention heads x layers = potentially 1000+ nodes. Layout algorithms must be carefully
chosen:

- **Sugiyama**: O(|V| + |E|) for layer assignment, O(|V|^2) for crossing minimization
  per sweep. With hierarchical grouping, the visible node count stays manageable (tens,
  not thousands) at any zoom level.

- **ELK (Eclipse Layout Kernel)** [15]: Purpose-built for compound hierarchical graphs.
  Supports ports (explicit edge attachment points on node borders), nested layout (child
  graphs laid out independently, then composed), and cross-hierarchy edges. ELK's layered
  algorithm is a sophisticated Sugiyama variant with five distinct phases. Its JavaScript
  transpilation (elkjs) is widely used in web-based diagram editors.

- **Virtualized rendering**: Only compute layout and render nodes that are visible in the
  current viewport. Off-screen nodes get approximate bounding boxes for scroll/zoom
  calculations but skip detailed rendering.


## 3. Dataflow Visualization on Graphs

### 3.1 Overlaying Activation Data

The architecture graph provides structure; the data overlay provides semantics. For
rocket_surgeon, every node in the graph has associated runtime data: tensor shapes,
activation magnitudes, gradient norms, numerical health (NaN/Inf counts).

**Node coloring by magnitude**: Map activation L2-norm (or max absolute value) to a
color scale. Blue-to-red diverging palettes work well: blue = small activations,
white = typical, red = large/potentially problematic. This immediately highlights which
layers are "hot" (large activations, potential overflow) vs. "cold" (vanishing signal).

**Edge thickness by data volume**: Make edges thicker in proportion to the tensor size
flowing through them. A [batch, seq_len, hidden_dim] tensor is shown as a thick pipe;
a [batch, seq_len, 1] attention mask is a thin wire. This conveys the "data volume"
through the network.

**Sparklines in nodes**: Embed a tiny histogram or sparkline of activation values
directly inside each node's bounding box. At a glance, the user sees both the
operation type and the distribution of its output.

### 3.2 Color-Coding Edges by Gradient Flow

During backward-pass debugging, gradient magnitude per edge is critical. The gradient
flow overlay replaces the forward-pass coloring with gradient norms:

- **Healthy gradients**: Green edges, moderate thickness.
- **Vanishing gradients**: Fading to transparent/gray, thin.
- **Exploding gradients**: Bright red, thick, possibly with a warning icon.

This turns the architecture diagram into a diagnostic tool: gradient pathology is
visible as a color gradient across the tower. Vanishing gradients show as a fade from
green (near the loss) to gray (near the embedding). Exploding gradients show as
intensifying red.

### 3.3 Tick-by-Tick Animation

rocket_surgeon's core abstraction is the "tick"---one forward-pass step. The graph
visualization should support animating data flow:

- **Wavefront highlighting**: As the tick advances through the network, a bright
  highlight sweeps from top to bottom, showing which layer is currently executing.
  Completed layers dim slightly; pending layers remain at base brightness.

- **Progressive edge activation**: Edges "light up" as data flows through them. Before
  a layer executes, its incoming edges are dim; after execution, they glow with a
  color indicating the output magnitude.

- **Ghost trails**: After several ticks, fading "ghost" colors on nodes show the
  trajectory of data values over time (increasing = warming color, decreasing = cooling).

### 3.4 Sankey Diagrams for Routing

Sankey diagrams [16] are flow diagrams where edge width is proportional to flow
magnitude. They are ideal for two specific rocket_surgeon visualizations:

1. **MoE token routing**: Tokens on the left, experts on the right, flow widths showing
   how many tokens each expert receives. This instantly reveals load imbalance.

2. **Activation magnitude flow**: Input tokens on the left, output logits on the right,
   intermediate layers as vertical bands. Flow width shows how much of the total
   activation "energy" passes through each pathway. Residual connections become thick
   bands that bypass the attention/MLP processing, visually demonstrating the "residual
   stream" hypothesis.

In a terminal context, Sankey rendering must be simplified: use half-block characters
and Braille dots to approximate curved flows, or use a rectilinear approximation with
box-drawing characters where flow width is indicated by character density or color
intensity rather than actual geometric width.

### 3.5 Focus+Context

The fundamental tension: the user needs to see one layer in detail while maintaining
awareness of position within the 32-layer tower. Three approaches from the information
visualization literature [17]:

- **Overview+Detail**: A minimap in one panel shows the entire tower with a viewport
  rectangle; the main panel shows the expanded view of the selected region. This is
  the Bloomberg terminal pattern: a persistent small overview, a large detail area.

- **Semantic Zoom**: At low zoom, each layer is a single labeled bar. At medium zoom,
  the selected layer shows its major components (attn block, MLP block). At high zoom,
  individual operations and tensor shapes appear. The transition between zoom levels
  is smooth, with elements fading in/out. The ZMLT (Zoomable Multilevel Tree) algorithm
  [18] provides the theoretical foundation: viewport intersection determines which
  nodes are visible at each zoom level, strictly bounding per-view complexity.

- **Fish-eye distortion**: The focused layer is rendered at full size; neighboring
  layers are progressively compressed. This keeps context visible without a separate
  minimap panel but may be disorienting if overdone.

For a TUI, overview+detail is the most practical. The minimap can be a single-column
vertical strip showing the layer tower, with a highlight indicating the currently
viewed region.


## 4. Graph Visualization in Constrained Spaces

### 4.1 The 80x24 Challenge

Fitting a 32-layer transformer with 32 attention heads per layer into an 80x24
character terminal requires aggressive abstraction. The math: 80 characters wide, 24
lines tall, minus chrome (borders, status bar, header) leaves roughly 76x20 usable
characters.

**Column layout**: Devote one column (1-2 characters wide) per attention head. With 32
heads and 2 chars each = 64 characters, leaving 12 for labels and borders. Each layer
gets one row. 32 layers fit in 32 rows---but we only have 20. Solution: show a viewport
of ~18 layers with scroll indicators, or use half-block characters to fit 2 layers per
character row (40 layers in 20 rows).

**Collapsed tower**: Each layer is a single row: `[L00 attn:0.42 mlp:0.38 grad:ok]`.
This fits 20 layers on screen with room for header/footer. The numbers are summary
statistics (activation norms). Color-coding conveys health at a glance: green = healthy,
yellow = elevated, red = pathological.

**ASCII sparklines**: Using Unicode block elements (U+2581-U+2588), each layer's
activation distribution can be rendered as an 8-character sparkline:
`L05 [_-=##=-_]`. This is maximally information-dense.

### 4.2 Collapsible/Expandable Nodes

The accordion pattern in a TUI:

```
  Embedding [768 -> 768]
+ Layer  0  [attn: 0.34  mlp: 0.41]   <-- collapsed, shows summary
- Layer  1                             <-- expanded
  |  ln1 -> q_proj [768x768]
  |       -> k_proj [768x768]
  |       -> v_proj [768x768]
  |       -> sdpa -> o_proj
  |  residual_add
  |  ln2 -> gate_proj [768x3072]
  |       -> up_proj [768x3072]
  |       -> silu -> down_proj
  |  residual_add
+ Layer  2  [attn: 0.29  mlp: 0.35]
  ...
+ Layer 31  [attn: 0.18  mlp: 0.22]
  LM Head [768 -> vocab]
```

The expanded layer uses tree-drawing characters (|, ->, indentation) to show internal
structure. The +/- toggles are keyboard-driven (Enter to expand/collapse, j/k to
navigate). This pattern fits naturally in 80 columns and allows the user to expand
multiple layers simultaneously for comparison.

### 4.3 Minimap/Overview+Detail

In a split TUI layout:

```
+-----+------------------------------------------+
|TOWER|  Layer 5: Attention Detail                |
|     |                                           |
| L00 |  q_proj ----+                             |
| L01 |  k_proj ----+---> sdpa ---> o_proj        |
| L02 |  v_proj ----+       |                     |
| L03 |                     v                      |
| L04 |              attn_weights                  |
|[L05]|  [head 0] [head 1] ... [head 31]          |
| L06 |   0.12     0.45         0.03              |
| L07 |                                           |
| ... |  Activation histogram:                    |
| L31 |  ___--##--___   mean=0.34 std=0.12        |
+-----+------------------------------------------+
```

The left panel is the minimap: one row per layer, the currently selected layer
highlighted. The right panel shows the detail view for the selected layer. This is the
overview+detail pattern adapted for character-cell rendering.

### 4.4 Treemap Representations

Treemaps [19] use nested rectangles where area encodes size. For neural networks,
"size" could be parameter count, activation memory, or FLOP cost. A treemap of a
transformer would show:

- The entire model as the bounding rectangle
- Each layer as a sub-rectangle (all roughly equal for a uniform transformer)
- Within a layer, attention and MLP as sub-sub-rectangles
- Within attention, the projection matrices as leaves

In a terminal, treemaps are rendered with box-drawing characters for borders and
fill characters for area. The challenge is label placement: nested rectangles quickly
become too small for text. Treemaps work best as a "parameter budget" or "memory
budget" overview rather than as a structural/flow diagram.

### 4.5 Flame Graph Style

Brendan Gregg's flame graphs [20] stack horizontal bars representing hierarchical
data, with bar width proportional to the metric of interest. For neural networks:

```
|==================== Model (100%) ====================|
|========= Attention (45%) =========|==== MLP (55%) ===|
|= q (11%) =|= k (11%) =|= v (11%) =|= o (12%) =|....|
```

The flame graph naturally handles the hierarchy (model -> layer -> component -> op) and
communicates relative cost through width. In a terminal, this renders beautifully with
simple characters. Gregg's comparison [20] concludes that flame graphs should be the
default for hierarchical data because "long labeled rectangles tell the big picture by
comparing their lengths"---length comparison is cognitively easier than area comparison
(treemaps) or angle comparison (sunburst).

For rocket_surgeon, a flame graph could show:
- **FLOP distribution**: Which layers and components dominate compute.
- **Activation memory**: Which layers hold the most memory.
- **Time per tick**: How long each component takes to execute (profiling overlay).


## 5. Existing Tools and Libraries

### 5.1 Graphviz/dot

The patriarch of graph visualization [12]. Graphviz's `dot` engine implements the
Sugiyama algorithm with cluster subgraph support. Key features for our use case:

- `rankdir=TB` for top-to-bottom flow (the natural transformer orientation).
- `subgraph cluster_layer_N` for grouping layer components.
- `rank=same` to force nodes onto the same horizontal rank.
- Record-shaped nodes for showing structured data within a node.
- Edge routing with splines, orthogonal routing, or polyline options.

Graphviz is a layout engine, not a rendering engine for terminals. But its layout
algorithms can be studied and reimplemented. The `dot` algorithm's source is well-
documented and has been reimplemented in JavaScript (dagre, d3-dag), Java (ELK), and
Python (grandalf).

### 5.2 D3.js Layout Algorithms

D3's ecosystem includes several layout implementations whose algorithms transfer to
any rendering target:

- **d3-dag** [21]: Modular Sugiyama layout. Small, focused, well-documented. Implements
  layering (longest path, Coffman-Graham), crossing minimization (two-layer),
  and coordinate assignment (quadratic optimization).
- **d3-hierarchy**: Tree layouts (Reingold-Tilford, cluster, treemap, partition, pack).
- **d3-force**: Force-directed simulation (useful for attention pattern visualization).
- **d3-sankey**: Sankey diagram layout with iterative relaxation.

These are JavaScript implementations, but the algorithms are language-independent. The
d3-dag source in particular is clean enough to serve as a reference for a Rust
reimplementation.

### 5.3 ELK (Eclipse Layout Kernel)

ELK [15] deserves special attention for rocket_surgeon because it handles compound
graphs with ports---exactly the structure of neural network layers. Its layered
algorithm supports:

- **Hierarchical ports**: Edges attach to specific points on node borders, not just
  "somewhere on the side." This matters for showing which input feeds which projection.
- **Nested layout**: Child graphs (the internals of a layer) are laid out independently,
  then the parent graph (the layer tower) incorporates them.
- **Cross-hierarchy edges**: Residual connections that skip levels are routed correctly.
- **Port constraints**: Edges can be constrained to enter/exit from specific sides.

ELK is Java-based but has a JavaScript port (elkjs). The algorithms are well-documented
in academic papers and could be reimplemented in Rust.

### 5.4 Rust Graph Layout Crates

The Rust ecosystem for graph layout is thin but growing:

- **petgraph** [22]: The standard graph data structure crate. Supports directed/undirected
  graphs, topological sort, DFS/BFS, DOT export. No layout algorithms.
- **layout-rs** [23]: Parses Graphviz DOT files and renders to SVG. Implements a basic
  topological layout. Does NOT support nested graphs or hierarchical layout. Limited.
- **egui_graphs** [24]: Interactive graph widget for egui. Supports random, hierarchical,
  and force-directed (Fruchterman-Reingold) layouts. GUI-oriented, not TUI.
- **visgraph**: Visualization with various layout algorithms, exports to PNG/SVG.

**Assessment for rocket_surgeon**: None of these crates provide a production-quality
Sugiyama implementation with compound graph support. petgraph provides the graph data
structure foundation. Layout must be implemented from scratch, referencing the d3-dag
or ELK algorithm descriptions. This is consistent with the project's "no dependencies,
reimplement everything" principle.

### 5.5 ASCII/Unicode Graph Rendering

The character repertoire for terminal graph drawing:

**Box drawing (U+2500-U+257F)**: 128 characters covering single-line, double-line, and
mixed thickness. Key characters:
- Horizontal/vertical: `─ │ ═ ║`
- Corners: `┌ ┐ └ ┘ ╔ ╗ ╚ ╝`
- T-junctions: `├ ┤ ┬ ┴ ╠ ╣ ╦ ╩`
- Crossings: `┼ ╬`
- Rounded corners: `╭ ╮ ╯ ╰`

**Block elements (U+2580-U+259F)**: Half-blocks, quarter-blocks, and shading.
Useful for filled rectangles, progress bars, and area-proportional displays.

**Braille patterns (U+2800-U+28FF)**: 256 characters, each a 2x4 dot matrix. Provides
sub-character resolution for sparklines, scatter plots, and fine-grained graph edges.
Ratatui's Canvas widget [25] uses Braille by default, achieving 2x4 resolution per
character cell.

**Arrow characters**: `→ ← ↑ ↓ ↗ ↘ ↙ ↖ ⟶ ⟵` for indicating flow direction.

For graph edge routing in a terminal, the approach is:
1. Compute layout coordinates in a continuous space.
2. Quantize to character cells.
3. For each edge segment, select the appropriate box-drawing character based on entry
   and exit direction.
4. Handle crossings with `┼` characters.
5. Use color to disambiguate overlapping edges.


## 6. The "Tower of Tensors" Problem

### 6.1 The Fundamental Metaphor

A neural network forward pass is a tower: data enters at the bottom (or top, by
convention), flows upward through stacked transformation layers, and exits as a
prediction. Each layer receives a tensor, transforms it, and passes the result upward.
The tensor's shape may stay constant (hidden_dim -> hidden_dim through most transformer
layers) but its statistical properties change: the distribution of values, the
information content, the gradient magnitude.

This is not merely an analogy. The physical structure of the computation is a literal
tower of matrix operations. The visualization challenge: how to show both the tower
structure (the architecture) and the data transformation (the semantics) simultaneously.

### 6.2 Geological Stratigraphy Analogy

Geological stratigraphic columns show layers of rock with properties annotated per
layer: composition, age, thickness, fossil content. The visual structure is:

```
Surface
|  Sandstone   [grain: fine, age: 65 Ma, color: tan]
|  Shale       [grain: clay, age: 70 Ma, color: gray]
|  Limestone   [grain: crystalline, age: 100 Ma, color: white]
Bedrock
```

Each layer has a fixed position in the column, a name, and annotations. The analogy to
a transformer is direct:

```
Output logits
|  Layer 31  [act_norm: 0.18, grad_norm: 1.2e-4, dtype: bf16]
|  Layer 30  [act_norm: 0.21, grad_norm: 1.5e-4, dtype: bf16]
|  ...
|  Layer  0  [act_norm: 0.45, grad_norm: 2.1e-3, dtype: bf16]
Embedding
```

The stratigraphic column visualization naturally supports:
- Per-layer annotations (statistics, health indicators)
- Color-coding by property (gradient health, activation magnitude)
- Variable-thickness layers (expand interesting layers, compress boring ones)
- Boundary markers (device partition boundaries for multi-GPU)

### 6.3 Signal Processing Waterfall Plots

Waterfall plots [26] show a 3D view of how a signal's frequency spectrum evolves over
time. Each horizontal slice is a spectrum at one time step; successive slices stack
vertically. The axes are: frequency (x), time (y), amplitude (color/height).

For neural networks, the analogous waterfall plot would show:
- **X-axis**: Hidden dimension index (or PCA component).
- **Y-axis**: Layer depth (layer 0 at top, layer 31 at bottom).
- **Color**: Activation magnitude at that position in that layer.

This produces a heatmap where patterns emerge: if certain hidden dimensions consistently
carry large activations, they appear as vertical bright streaks. If a layer suddenly
amplifies or suppresses certain dimensions, it appears as a horizontal band of changed
color.

In a terminal, this waterfall can be rendered with Braille dots or block characters,
using a 256-color or 24-bit color palette. With 80 columns and Braille's 2x horizontal
resolution, we get 160 "pixels" across---enough to show the principal components of the
hidden state evolving through 32 layers in a single screen.

### 6.4 Building Cross-Section Diagrams

Architectural cross-sections show how different systems (structural, mechanical,
electrical) coexist within the same physical space. For a transformer, the "systems" are:

- **Data flow**: The tensor moving through layers.
- **Gradient flow**: The backward-pass signal moving in the opposite direction.
- **Memory usage**: How much activation memory each layer holds.
- **Compute intensity**: How long each layer takes.

A cross-section view would show the tower with multiple parallel columns of information:

```
Layer  | Fwd Act | Bwd Grad | Memory | Time
-------|---------|----------|--------|------
L31    | ===     | =        | ==     | ==
L30    | ====    | ==       | ==     | ==
...
L01    | ======  | ======== | ====   | ===
L00    | =====   | ======== | ====   | ===
```

Each column uses bar length or color intensity to encode its metric. This is
information-dense and fits well in a terminal.


## 7. Interactive Graph Navigation

### 7.1 Zoom Semantics for Hierarchical Graphs

In a conventional graph viewer, "zoom" means geometric magnification. For hierarchical
graphs, **semantic zoom** [18] is more powerful: zooming in reveals more structural
detail, not just bigger pixels.

The zoom levels for a transformer:

1. **Model level**: The entire model as a single box. Shows input/output shapes, total
   parameter count, device placement summary.
2. **Layer level**: The tower of layers. Each layer is a labeled box. Summary statistics
   per layer visible. This is the default view.
3. **Component level**: One layer expanded. Attention block and MLP block visible as
   sub-boxes. Internal edges (residual connections, layer norms) visible.
4. **Operation level**: One component expanded. Individual operations visible (MatMul,
   Softmax, GELU). Tensor shapes on edges. Weight matrix dimensions in nodes.
5. **Head level**: Attention heads visible as individual nodes. Per-head attention
   patterns and statistics. This is the deepest zoom.

The zoom transitions should be keyboard-driven: `z` to zoom in (expand the selected
node), `Z` to zoom out (collapse back to parent). The selected node's children
animate into view (or, in a TUI, simply appear with a redraw). Context (the surrounding
layers in the tower) remains visible at reduced detail.

### 7.2 Selection and Highlighting

Graph selection follows a "structural cursor" model:

- **Node selection**: Navigate with j/k (next/previous node in flow order), h/l
  (parallel nodes at the same depth). The selected node is highlighted with a bold
  border or inverse color.
- **Path highlighting**: Select a node, press `p` to highlight all nodes on the
  path from input to the selected node. This traces the data lineage.
- **Subtree highlighting**: Select a layer, press `s` to highlight all operations
  within it. Useful for seeing the scope of a device partition.
- **Cross-reference highlighting**: Select a tensor edge, all other edges carrying
  the same tensor are highlighted. Useful for tracking residual connections.

### 7.3 Linked Views

rocket_surgeon's TUI should implement coordinated multiple views [27]:

- **Graph view <-> Tensor view**: Select a node in the graph, the tensor panel shows
  that node's output tensor data (shape, statistics, histogram, sample values).
- **Graph view <-> Timeline view**: Select a node, the timeline highlights when that
  operation executed. Select a time range in the timeline, the graph highlights which
  operations were active.
- **Graph view <-> Code view**: Select a node, the code panel shows the PyTorch source
  line that generated that operation (requires source mapping from hooks).

The interaction primitive is **brushing**: selecting in one view highlights in all
linked views. In a TUI, this means a shared selection state that all panels observe.
When the selection changes (via keyboard navigation in any panel), all panels update
to reflect the new focus.

### 7.4 Keyboard Navigation Design

Following the Bloomberg terminal philosophy (context carries forward, minimal mode
switching):

- **Arrow keys / vim keys**: Navigate the graph structure.
- **Enter**: Expand/collapse (zoom into/out of selected node).
- **Tab**: Cycle focus between panels (graph, tensor, timeline).
- **/ (slash)**: Search for a node by name (e.g., "/layer_15/attn/q_proj").
- **Space**: Toggle data overlay (forward activations, backward gradients, memory).
- **1-9**: Quick-jump to layer N (1 = layer 0, 2 = layer 3, ... logarithmic or
  configurable mapping).
- **[ / ]**: Step backward/forward one tick (the core rocket_surgeon interaction).

The key principle: the user should never need a mouse. Every interaction---navigation,
zoom, selection, view switching, data overlay---is keyboard-driven with consistent,
memorable bindings. Context carries forward: if you're looking at layer 5's attention
and step to the next tick, you stay looking at layer 5's attention with updated data.


## 8. Synthesis: Recommendations for rocket_surgeon

### 8.1 Primary Architecture: Sugiyama Layout with Semantic Zoom

The main graph view should use a Sugiyama-based hierarchical layout with compound node
support. The graph data structure lives in petgraph. Layout is computed in Rust, not
delegated to an external tool. The algorithm follows ELK's five-phase approach adapted
for terminal rendering.

### 8.2 The Tower View as Default

The default screen shows the stratigraphic tower: one row per layer, collapsible,
with per-layer summary statistics and color-coded health indicators. This fits in 80x24
and provides the "at a glance" picture of the entire model.

### 8.3 Expand-on-Demand Detail

Expanding a layer replaces the single row with 8-12 rows showing internal structure
using tree-drawing characters. Multiple layers can be expanded simultaneously. An
overview minimap in a side panel shows the full tower with the current viewport
highlighted.

### 8.4 Data Overlay System

The graph carries a data layer that can be switched between: forward activations,
backward gradients, memory usage, compute time, device placement. Each overlay uses a
consistent color scheme. The overlay data updates per-tick as the user steps through
the forward pass.

### 8.5 Rendering Pipeline

1. Model topology from hook registration (the "subscribe" system already built).
2. petgraph stores the directed graph with hierarchical grouping.
3. Custom Sugiyama layout computes positions, respecting compound nodes and ports.
4. Terminal renderer quantizes to character cells using box-drawing characters, Braille
   for fine-grained overlays, and 24-bit color for data encoding.
5. ratatui provides the TUI framework; a custom widget wraps the graph renderer.

### 8.6 What NOT to Build

- Do not build force-directed layout for the architecture view. It destroys the flow
  metaphor that makes neural network graphs readable.
- Do not attempt to render the full flat computational graph (like torchviz). It
  produces unusable hairballs for any model beyond toy scale.
- Do not use Graphviz as a runtime dependency. Use it as algorithmic reference only.
  The layout must be native Rust for performance and control.


## Bibliography

[1] L. Roeder, "Netron: Visualizer for neural network, deep learning and machine learning models." GitHub, https://github.com/lutzroeder/netron

[2] L. Kethinedi, "Visualizing Deep Learning models using Netron." Medium, https://medium.com/@lahari.kethinedi/visualizing-deep-learning-models-using-netron-a034dfd72540

[3] TensorFlow, "TensorBoard Graph Plugin README." GitHub, https://github.com/tensorflow/tensorboard/blob/master/tensorboard/plugins/graph/README.md

[4] TensorFlow, "TensorBoard: Graph Visualization." Chromium source, https://chromium.googlesource.com/external/github.com/tensorflow/tensorflow/+/r0.12/tensorflow/g3doc/how_tos/graph_viz/index.md

[5] PyTorch, "Visualizing Models, Data, and Training with TensorBoard." PyTorch Tutorials, https://docs.pytorch.org/tutorials/intermediate/tensorboard_tutorial.html

[6] Z. Pan et al., "Towards Efficient Visual Simplification of Computational Graphs in Deep Neural Networks." IEEE TVCG, 2022. https://arxiv.org/abs/2212.10774

[7] M. Grootendorst, "A Visual Guide to Mixture of Experts (MoE)." Newsletter, https://newsletter.maartengrootendorst.com/p/a-visual-guide-to-mixture-of-experts

[8] MixtureKit authors, "MixtureKit: A General Framework for Composing, Training, and Visualizing Mixture-of-Experts Models." https://arxiv.org/abs/2512.12121

[9] NVIDIA, "Parallelisms Guide -- Megatron Bridge." https://docs.nvidia.com/nemo/megatron-bridge/latest/parallelisms.html

[10] K. Sugiyama, S. Tagawa, and M. Toda, "Methods for Visual Understanding of Hierarchical System Structures." IEEE Trans. Systems, Man, and Cybernetics, 1981.

[11] P. Healy and N. Nikolov, "Hierarchical Drawing Algorithms." Ch. 13 in Handbook of Graph Drawing and Visualization, R. Tamassia, Ed., CRC Press. https://cs.brown.edu/people/rtamassi/gdhandbook/chapters/hierarchical.pdf

[12] Graphviz, "Drawing graphs with dot." https://www.graphviz.org/pdf/dotguide.pdf

[13] Wikipedia, "Force-directed graph drawing." https://en.wikipedia.org/wiki/Force-directed_graph_drawing

[14] ResearchGate, "Orthogonal Graph Drawing." https://www.researchgate.net/publication/47842991_Orthogonal_Graph_Drawing

[15] S. Domroes et al., "The Eclipse Layout Kernel." arXiv:2311.00533, 2023. https://arxiv.org/abs/2311.00533

[16] Data to Viz, "Sankey diagram." https://www.data-to-viz.com/graph/sankey.html

[17] A. Cockburn, A. Karlson, and B. Bederson, "A Review of Overview+Detail, Zooming, and Focus+Context Interfaces." ACM Computing Surveys, 2009. https://dl.acm.org/doi/10.1145/1456650.1456652

[18] M. Burch et al., "Multi-level tree based approach for interactive graph visualization with semantic zoom." arXiv:1906.05996, 2019. https://arxiv.org/abs/1906.05996v1

[19] B. Shneiderman, "Tree visualization with tree-maps: 2-d space-filling approach." ACM Trans. Graphics, 1992.

[20] B. Gregg, "Flame Graphs vs Tree Maps vs Sunburst." Blog, 2017. https://www.brendangregg.com/blog/2017-02-06/flamegraphs-vs-treemaps-vs-sunburst.html

[21] E. Brinkman, "d3-dag: Layout algorithms for visualizing directed acyclic graphs." GitHub, https://github.com/erikbrinkman/d3-dag

[22] petgraph authors, "petgraph: Graph data structure library for Rust." GitHub, https://github.com/petgraph/petgraph

[23] layout-rs authors, "layout-rs: Rust data visualization library." https://lib.rs/crates/layout-rs

[24] blitzarx1, "egui_graphs: Interactive graph visualization widget for Rust." GitHub, https://github.com/blitzarx1/egui_graphs

[25] Ratatui authors, "Canvas widget documentation." https://docs.rs/ratatui/latest/ratatui/widgets/canvas/index.html

[26] Wikipedia, "Waterfall plot." https://en.wikipedia.org/wiki/Waterfall_plot

[27] J. C. Roberts, "State of the Art: Coordinated & Multiple Views in Exploratory Visualization." CMV 2007. https://www.researchgate.net/publication/4259731_State_of_the_Art_Coordinated_Multiple_Views_in_Exploratory_Visualization

[28] J. Vig, "A Multiscale Visualization of Attention in the Transformer Model." ACL 2019. https://arxiv.org/abs/1906.05714

[29] J. Vig, "BertViz: Visualize Attention in Transformer Models." GitHub, https://github.com/jessevig/bertviz

[30] yWorks, "Layered Graph Layout." https://www.yworks.com/pages/layered-graph-layout

[31] Wikipedia, "Box-drawing characters." https://en.wikipedia.org/wiki/Box-drawing_characters

[32] Svelte Flow, "Dagre Layout." https://svelteflow.dev/examples/layout/dagre

[33] Tom Sawyer Software, "TensorFlow Graph Visualization." https://blog.tomsawyer.com/tensorflow-graph-visualization

[34] Cosmo authors, "Cosmo: A Graph Visualizer That Lives in Your Terminal." DEV Community, https://dev.to/turutupa/meet-cosmo-a-graph-visualizer-that-lives-in-your-terminal-1p5o

[35] HuggingFace, "Mixture of Experts (MoEs) in Transformers." https://huggingface.co/blog/moe-transformers
