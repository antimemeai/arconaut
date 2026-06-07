# Tensor Visualization: Exhaustive Research Report

**Date:** 2026-05-19
**Context:** rocket_surgeon TUI intermission -- understanding the full landscape of tensor visualization techniques before designing our own approach for a Rust-based terminal debugger targeting multi-GPU transformer forward passes.

---

## 1. How Existing Tools Visualize Tensors

### 1.1 TensorBoard

TensorBoard, TensorFlow's visualization toolkit (also usable with PyTorch via `torch.utils.tensorboard`), offers several distinct tensor visualization modalities:

**Histogram Dashboard.** Records arbitrary tensors via `tf.summary.histogram` and compresses them into bucketed histogram data structures. Displays a 3D stacked-histogram view showing how the distribution of tensor values evolves over training steps. Each "slice" is a histogram at a particular step, stacked along a time axis. This is the primary way TensorBoard exposes raw tensor value distributions. The bucketing is done server-side; the client renders pre-aggregated data.

**Distributions Dashboard.** Plots probability density of tensor values over time as a set of overlaid curves (percentile bands). While histograms show local bucket counts, distributions show global statistical properties -- median, interquartile range, and tails. This is particularly useful for diagnosing whether parameters are updating, whether activations are saturating, or whether gradients are vanishing.

**Embedding Projector.** Visualizes high-dimensional data (embedding vectors) by reducing to 2D or 3D via PCA, t-SNE, or UMAP. Reads from checkpoint files, optionally annotated with metadata (vocabulary labels, sprite images). Interactive: rotate, zoom, select clusters, search for specific points. This is a fundamentally different kind of tensor visualization -- it treats rows of a 2D tensor as points in a high-dimensional space rather than visualizing the raw values.

**Image Summaries.** Logs 2D tensors (or slices of higher-dimensional tensors) as images. Commonly used for attention maps, feature maps, generated images. The tensor is mapped through a colormap and rendered as a bitmap. No interactivity beyond choosing which step to view.

**Scalars.** Single values over time -- loss curves, learning rates. Not tensor visualization per se, but the most heavily used dashboard.

**Key insight for us:** TensorBoard's strength is temporal evolution of aggregate statistics. Its weakness is inspecting a specific tensor at a specific moment -- there is no "show me this 4D tensor right now" view.

### 1.2 Weights & Biases (wandb)

W&B's approach to tensor visualization is primarily through logged media and custom charts:

**Built-in Presets.** Line plots, scatter plots, bar charts, histograms, PR curves, ROC curves. These operate on scalar or 1D data logged from training scripts.

**Custom Charts.** Uses Vega/Vega-Lite as the visualization grammar. Users log arbitrary tables of data with `wandb.log()` and build custom Vega specs to visualize them. This is maximally flexible but requires the user to define the visualization. W&B provides a GraphQL query layer to pull logged data into Vega specs.

**Media Logging.** Images, audio, video, 3D objects, HTML. Tensors can be logged as images (e.g., attention heatmaps rendered by matplotlib before logging). W&B does not natively render arbitrary tensors -- the user must convert to a visual format first.

**TensorBoard Integration.** W&B can ingest TensorBoard event files directly, re-rendering TensorBoard's histogram and scalar data in W&B's native charts.

**Key insight for us:** W&B punts on direct tensor visualization. It provides infrastructure for logging and a flexible grammar for rendering, but the user must bridge from "tensor" to "visual representation" themselves.

### 1.3 PyTorch's torchviz / make_dot

The `torchviz` package (and the newer `torchview`) creates Graphviz DOT representations of PyTorch autograd computation graphs. Given an output tensor, `make_dot()` walks the autograd graph backward, producing nodes for operations and edges for data flow. Options include `show_attrs=True` (show operation attributes) and `show_saved=True` (show tensors saved for backward).

This is **graph topology** visualization, not tensor value visualization. It shows which operations produced which tensors and how gradients flow, but does not show the actual numbers inside any tensor. It is useful for understanding model architecture, not for debugging tensor values.

### 1.4 Treescope (Google DeepMind)

Treescope is the most innovative tensor visualization tool in the current landscape. Originally developed as the pretty-printer for the Penzai neural network library, it is now a standalone project.

**Core Idea: Faceted N-dimensional Array Visualization.** Treescope renders arbitrary-dimensional arrays as faceted 2D pixel grids directly inline in notebook output (HTML). Each cell in the pixel grid maps to one element of the array. For arrays with more than 2 dimensions, Treescope creates a grid of facets -- small multiples -- where each facet is a 2D slice, and the facets are arranged along additional axes.

**Axis Assignment.** The user (or Treescope's defaults) assigns which axes map to the X axis of the pixel grid, which to the Y axis, and which are faceted (arranged as small multiples in rows/columns) or sliced (with slider controls for navigation).

**Color Mapping.** Values are mapped to colors using a diverging or sequential colormap. Hovering over a pixel shows the exact numeric value. Clicking locks the display to a particular element.

**Automatic Embedding.** Treescope automatically embeds these visualizations whenever a tensor is printed, eliminating the need for explicit plotting code. This is the "print debugging but actually useful" approach.

**Model Structure Visualization.** Beyond arrays, Treescope color-codes parts of neural network model trees to show shared structures, and adds "copy path" buttons for easy navigation.

**Supported frameworks:** JAX, NumPy, PyTorch, Equinox, Flax NNX.

**Key insight for us:** Treescope's faceted approach is the closest prior art to what we need. The challenge is adapting its HTML/interactive model to a terminal environment. The core idea -- small-multiple facets with per-element color mapping -- translates well to a pixel-grid rendered via Kitty protocol.

### 1.5 Comgra

Comgra (Computation Graph Analyzer) is a PyTorch debugging tool with a GUI for traversing the computation graph and inspecting tensor data. Published as a paper at ICML 2024 workshops.

**Recording and Playback.** Comgra records network internals during forward and backward passes, then provides a GUI for post-hoc analysis. This is a record-then-analyze workflow, not live inspection.

**Computation Graph Navigation.** The user can move along the computation graph, jumping between operations. At each node, they can view the tensor data from multiple perspectives.

**Multi-Viewpoint Inspection.** Both summary statistics and individual data points are available. The user can compare early vs. late training stages, focus on individual samples, and visualize gradient flow through the network.

**Anomaly Detection.** Comgra can automatically flag outlier values, NaN/Inf occurrences, and irregular distributions.

**Key insight for us:** Comgra's graph-navigation + multi-viewpoint approach is architecturally similar to what rocket_surgeon wants. The difference is that Comgra is a post-hoc GUI tool; we want live, in-situ, terminal-based inspection with surgical intervention.

### 1.6 nnsight / TransformerLens

These are mechanistic interpretability libraries, not visualization tools per se, but they define the interface through which tensors are accessed for visualization:

**TransformerLens** reimplements transformer architectures from scratch with named hook points at every interesting location (attention patterns, residual stream, MLP outputs, etc.). Provides a cache system that stores all intermediate activations from a forward pass. Visualization is done by extracting tensors from the cache and plotting with matplotlib/CircuitsVis.

**nnsight** wraps existing HuggingFace models with a tracing system, allowing intervention on arbitrary internal tensors without reimplementation. More general but requires knowing the model's internal module structure.

**nnterp** bridges the two approaches: a lightweight wrapper around nnsight that provides TransformerLens-like named access across 50+ model architectures.

**Key insight for us:** These tools define *what* to visualize (the hook point taxonomy for transformers) but delegate *how* to visualize to external tools. Our hook/subscription system already covers the "what"; we need the "how."

### 1.7 CircuitsVis

CircuitsVis is a React-based visualization library from the TransformerLens ecosystem (originally based on Anthropic's PySvelte).

**Attention Pattern Grids.** Renders attention matrices as destination-by-source grids (heatmaps). Interactive: hover over tokens to see attention weights, click to lock. Can show multiple heads simultaneously.

**Token-Level Detail.** Hovering over a token in the input sequence highlights its attention distribution across all other tokens.

**Integration.** Designed for Jupyter notebooks, renders as inline HTML/JavaScript widgets. Used extensively in mechanistic interpretability research to identify attention head types (induction heads, previous-token heads, etc.).

**Key insight for us:** CircuitsVis's attention grid is the canonical way researchers look at attention. We need to support this view natively.

### 1.8 BertViz

BertViz provides three levels of attention visualization granularity:

**Head View.** Shows attention for one or more heads in a single layer. Source and destination tokens are listed vertically on left and right sides; colored lines connect them with opacity proportional to attention weight. This is the "bipartite graph" view.

**Model View.** Bird's-eye view across all layers and heads simultaneously. Each layer is a column, each head within a layer gets a colored attention pattern. Useful for seeing global patterns -- which layers attend broadly vs. narrowly.

**Neuron View.** Visualizes individual neurons in query and key vectors, showing how they contribute to the attention computation. This is the deepest level -- actually inspecting the dot-product components.

**Key insight for us:** The three-level hierarchy (head / model / neuron) is a good design pattern. Our system should support similar drill-down: overview of all layers, zoom into a layer's heads, zoom into a single head's query/key decomposition.

### 1.9 AttentionViz

AttentionViz (Yeh et al., IEEE VIS 2023) takes a fundamentally different approach: instead of showing attention weights as matrices, it creates a **joint embedding of query and key vectors** in a shared 2D/3D space. Attention weight between two tokens corresponds to proximity in this embedding space.

This enables global analysis across multiple input sequences simultaneously, unlike BertViz/CircuitsViz which show one input at a time. Users can see clusters of queries and keys, identify attention head specialization patterns, and compare across layers.

**Key insight for us:** This is a dimensionality-reduction approach (like t-SNE/UMAP applied to Q/K vectors). Computationally expensive. May be out of scope for live in-situ debugging but could be a post-hoc analysis feature.

---

## 2. Tensor Representation Challenges

### 2.1 The Dimensionality Problem

A typical transformer attention tensor has shape `[batch, heads, seq_len, seq_len]` -- 4 dimensions. A residual stream activation is `[batch, seq_len, d_model]` -- 3 dimensions. Weight matrices in MoE are `[num_experts, d_model, d_ff]` -- 3 dimensions. Screens are 2D. This is the fundamental problem.

**Strategies used in practice:**

- **Slice selection:** Fix all but 2 dimensions, show a 2D heatmap. The user navigates slices by changing fixed indices. TensorBoard's image summaries and most matplotlib-based approaches use this.
- **Faceting / small multiples:** Treescope's approach. Lay out 2D slices in a grid, where grid position encodes additional dimensions. Works well up to about 4-5 dimensions before the grid becomes unwieldy.
- **Aggregation:** Reduce dimensions via mean, max, norm, etc. Show a summary (e.g., mean attention across all heads) rather than the full tensor. Loses information but fits in 2D.
- **Animation / time axis:** Use one dimension as a time/slider axis, animating through slices. Interactive tools do this; terminal tools could cycle through slices on keypress.

### 2.2 Summary Statistics vs. Full Data

**When a histogram is enough:** Monitoring weight distributions during training. Checking for NaN/Inf. Detecting mode collapse. Comparing gradient magnitudes across layers. These are "is something wrong?" questions answered by aggregate statistics.

**When you need actual values:** Debugging a specific attention pattern. Understanding why a particular token gets misrouted in MoE. Comparing residual stream values before and after an intervention. These are "what exactly is happening here?" questions requiring element-level inspection.

**The practical answer:** Always start with summaries (shape, dtype, min/max/mean/std, histogram) and provide drill-down to full data on demand. This is both a UI design principle and a performance requirement -- transmitting full tensor data over IPC is expensive.

### 2.3 Scale and Dynamic Range

Tensors in practice span many orders of magnitude. Attention logits before softmax might range from -100 to +100. Post-softmax attention weights range 0 to 1 but are highly peaked (most values near 0, a few near 1). Gradient magnitudes can vary by factors of 10^6 across layers.

**Strategies:**
- **Log-scale colormaps:** Map log(|value|) to color. Essential for gradient visualization.
- **Symmetric log:** Linear near zero, logarithmic in tails. Matplotlib's `SymLogNorm`.
- **Percentile-based normalization:** Map the 1st-99th percentile range to the full colormap, clipping outliers. Prevents a single extreme value from washing out the visualization.
- **Adaptive/auto-ranging:** Compute min/max (or percentiles) per-tensor and normalize accordingly. The user should be able to lock the range for comparison across tensors.

### 2.4 Sparsity Visualization

Post-softmax attention is highly sparse (most weights near zero). MoE routing tables are explicitly sparse (each token goes to top-k experts out of many). Weight matrices may have structured sparsity after pruning.

**Strategies:**
- **Non-linear colormaps:** Use a colormap that allocates more perceptual range to near-zero values. E.g., a power-law transform before color mapping.
- **Threshold-based:** Show values above a threshold as colored, below as blank/background. Common for attention visualization where you want to see "who attends to whom" not "what is the exact weight."
- **Sparsity pattern visualization:** Binary view (nonzero vs zero), useful for understanding structural sparsity in weight matrices.

### 2.5 Comparing Tensors

This is critical for surgical debugging: you modify an activation and want to see what changed.

**Strategies:**
- **Diff heatmap:** Compute element-wise difference (or ratio), display as a diverging colormap centered on zero. Blue = decreased, red = increased (or vice versa).
- **Side-by-side:** Show before and after tensors as adjacent heatmaps with the same color scale.
- **Overlay:** Superimpose before/after with different color channels (e.g., red channel = before, green channel = after).
- **Statistical comparison:** Compare histograms of the two tensors, show KL divergence, L2 distance, cosine similarity.

### 2.6 Time Evolution

How a tensor changes across layers (depth) or across training steps (time) or across ticks (in our system).

**Strategies:**
- **Stacked histograms:** TensorBoard's approach. One histogram per timestep, stacked along a z-axis.
- **Line plots of summary statistics:** Mean, std, max, min of the tensor over time.
- **Filmstrip:** Small multiples arranged temporally, like frames of a movie.
- **Animated heatmap:** Cycle through heatmaps at different timesteps. Terminal-native via redrawing.

---

## 3. Visualization Types for Transformer Internals

### 3.1 Attention Patterns

The canonical transformer visualization. Shape: `[heads, seq_len, seq_len]` per layer (ignoring batch).

**Matrix heatmap (CircuitsVis style).** Each head rendered as a seq_len x seq_len grid. Color intensity = attention weight. Rows = destination (query) tokens, columns = source (key) tokens. This is the most information-dense view.

**Bipartite graph (BertViz head view).** Source tokens listed on left, destination on right, connecting arcs with opacity = attention weight. More intuitive for small sequences but does not scale past ~50 tokens.

**Aggregated views.** Mean attention across heads (shows "what the layer pays attention to on average"). Max attention per position (shows "strongest signal per query"). Entropy of attention distribution per position (shows whether attention is focused or diffuse).

**Head-level summary.** For each head, compute a single statistic (e.g., average entropy, fraction attending to previous token, fraction attending to first token) and display as a layers x heads grid. This is how researchers identify head types (induction, previous-token, BOS-attending).

### 3.2 Activation Heatmaps

Residual stream, MLP outputs, layer norm outputs. Shape: `[seq_len, d_model]`.

**Token-by-dimension heatmap.** Rows = token positions, columns = hidden dimensions. Color = activation value. Reveals structured patterns in the residual stream.

**Norm-per-position.** Compute L2 norm of the residual stream at each token position, display as a 1D bar or sparkline. Shows where the model's "energy" is concentrated in the sequence.

**Norm-per-layer.** Track the L2 norm of the residual stream as it passes through layers. Should increase roughly monotonically in a healthy model. Sudden drops or spikes indicate problems.

### 3.3 Weight Matrices

Shape: `[d_out, d_in]` for dense layers, `[num_experts, d_out, d_in]` for MoE.

**Full heatmap.** For small matrices, show every element. For large ones (e.g., 4096x4096), need downsampling.

**SVD spectrum.** Compute singular values, display as a line plot or bar chart. Shows effective rank and conditioning.

**Sparsity pattern.** Binary or threshold view showing which weights are non-negligible. Useful for pruned models.

**Block structure.** Many weight matrices exhibit block-diagonal or block-sparse structure. Visualization should make this apparent.

### 3.4 Embedding Spaces

Token embeddings, positional embeddings. Shape: `[vocab_size, d_model]` or `[max_seq_len, d_model]`.

**Dimensionality reduction.** Apply PCA, t-SNE, or UMAP to project to 2D/3D. Display as scatter plot with labels. This is what TensorBoard's Embedding Projector does.

**Cosine similarity matrices.** Compute pairwise cosine similarity between embedding vectors. Display as a heatmap. Reveals clustering structure without dimensionality reduction.

**For our terminal context:** Dimensionality reduction is computationally expensive (especially t-SNE). PCA is fast and may be sufficient for initial views. Scatter plots are straightforward in terminal via Braille characters or Kitty pixel rendering.

### 3.5 Logit Lens / Tuned Lens

Projects intermediate layer representations into vocabulary space to see "what the model would predict if it stopped here."

**Layer-by-token probability heatmap.** Rows = layers (0 to N), columns = token positions. Color = probability of the correct next token (or entropy of the predicted distribution). Shows how predictions converge across layers.

**Top-k tokens per layer-position.** At each (layer, position), show the top-k predicted tokens as text labels. This is more informative than a heatmap but requires more screen space.

**Logit lens vs. tuned lens.** The standard logit lens applies the final unembedding matrix to intermediate states, producing noisy results in early layers. The tuned lens trains a per-layer affine probe to produce calibrated distributions. Both produce the same visualization format; tuned lens produces cleaner results.

### 3.6 MoE Routing Tables

Shape: `[num_layers, seq_len, top_k]` (which experts were selected) plus `[num_layers, seq_len, num_experts]` (router logits/probabilities).

**Token-expert assignment heatmap.** Rows = token positions, columns = experts. Color = routing probability or binary (selected/not). Shows load balancing and specialization.

**Expert utilization bar chart.** Per expert, show what fraction of tokens are routed to it. Reveals load imbalance.

**Token coloring by expert.** In the input text, color each token by its primary expert assignment. Shows whether routing correlates with token semantics (e.g., all punctuation tokens go to expert 3).

**Cross-layer routing flow.** Alluvial/Sankey diagram showing how tokens' expert assignments change across layers. Reveals whether tokens "stick" to experts or move around.

MixtureKit provides both high-level (token colored by dominant expert across layers) and low-level (per-layer expert percentages) visualization.

### 3.7 Gradient Flow

Shape: one scalar (gradient norm) per parameter tensor per layer, or per-element gradients matching the tensor shape.

**Layer-wise gradient norm bar chart.** The standard diagnostic. X-axis = layer, Y-axis = gradient L2 norm. Vanishing gradients show an exponential decay toward early layers; exploding gradients show exponential growth.

**Gradient histogram per layer.** Distribution of gradient values within each layer's parameters. Healthy training shows gradients concentrated around zero with reasonable spread.

**Gradient-to-weight ratio.** Compute |gradient| / |weight| per parameter. Shows the effective learning rate. Values consistently near zero indicate dead parameters.

**RNNbow approach.** Visualizes gradient flow through time in recurrent networks using color-coded flow diagrams. The temporal unfolding aspect maps onto our tick-by-tick model.

---

## 4. Heatmap Rendering Techniques

### 4.1 Color Mapping Strategies

**Sequential colormaps.** Single hue, varying lightness. For data with a natural ordering from low to high. Examples: viridis, inferno, magma, plasma, cividis.

**Diverging colormaps.** Two hues diverging from a neutral center (usually white or light gray). For data centered on a meaningful midpoint (often zero). Examples: coolwarm, RdBu (red-blue), PiYG (pink-green), seismic. Critical for diff views and signed data (gradients, weight changes).

**Cyclic colormaps.** Wrap around, so the start and end colors match. For phase/angle data. Rarely needed for tensor visualization.

**Key principle:** The colormap must be chosen to match the data semantics. Attention weights (0 to 1, sequential) need a different colormap than weight differences (negative to positive, diverging).

### 4.2 Perceptually Uniform Colormaps

The viridis family (viridis, inferno, magma, plasma, cividis) was designed in CAM02-UCS color space to have constant perceptual change per unit of data change. Properties:

- **Monotonically increasing luminance:** Still informative when printed in grayscale.
- **No perceptual artifacts:** Unlike jet/rainbow, which create false boundaries at yellow and cyan due to luminance non-monotonicity.
- **Colorblind-safe:** Cividis specifically designed for deuteranopia; viridis is reasonably safe for most color vision deficiencies.

For diverging colormaps, Kenneth Moreland's work provides perceptually uniform diverging maps that maintain these properties while diverging from a neutral center.

**For terminal rendering:** We need to implement these colormaps in Rust. The math is a lookup table (256 or 512 entries of RGB triples) with linear interpolation. Minimal computational cost.

### 4.3 Resolution and Downsampling

A 128x128 attention matrix on a terminal might have only 40 columns and 20 rows of available character cells. With half-block rendering, that gives 40x40 effective pixels. With Kitty protocol, you could render at full resolution (128x128 pixels or more) within the same cell area.

**Downsampling strategies when data > pixels:**

- **Nearest-neighbor:** Each pixel shows the value of the nearest data element. Preserves exact values but can miss structure. Aliasing artifacts with regular patterns.
- **Bilinear interpolation:** Each pixel is a weighted average of neighboring data elements. Smoother but blurs sharp features. Appropriate for continuous data.
- **Area averaging (box filter):** Each pixel is the mean of all data elements that map to it. Mathematically correct for showing the "true" value at that resolution. Best default choice.
- **Max pooling:** Each pixel shows the maximum value in its corresponding region. Useful for seeing "is there anything interesting here?" without missing peaks.
- **Min-max envelope:** Show both min and max in each pixel region, perhaps as two heatmaps or as a range indication. Preserves dynamic range information.

**For interactive zoom:** Start with area-averaged overview, zoom to full resolution on demand. The Kitty protocol supports replacing images in-place, so zoom can be implemented by re-rendering at higher resolution and retransmitting.

### 4.4 Large Matrix Strategies

For very large matrices (e.g., 32768 x 32768 attention in long-context models):

- **Hierarchical/pyramidal rendering:** Pre-compute multiple resolution levels (like mipmaps). Show the coarsest level first, zoom into regions of interest.
- **Region of interest (ROI):** Show a zoomed view of a selected subregion alongside a minimap showing the full matrix at low resolution with the ROI highlighted.
- **Statistical summary overlay:** On a low-resolution view, overlay summary statistics (mean, max, variance) for each aggregated block.

---

## 5. Histogram and Distribution Visualization

### 5.1 Log-Scale Histograms

Essential for gradient distributions and weight distributions, which are often heavy-tailed. Linear-scale histograms compress the interesting structure into a few bins near zero. Log-scale Y-axis (or log-scale X-axis for strictly positive values) reveals the full structure.

### 5.2 Kernel Density Estimation (KDE)

KDE places a smooth kernel (typically Gaussian) on each data point and sums to produce a continuous density estimate. Compared to histograms:
- No bin-edge artifacts
- Smoother, more readable
- Requires bandwidth selection (Silverman's rule is a common default)
- More computationally expensive

For terminal rendering, KDE produces a smooth curve that can be rendered as a line plot using Braille characters or Kitty pixels.

### 5.3 Sparkline-Style Inline Distributions

Edward Tufte's sparklines: tiny, word-sized charts embedded in text. In terminals, implemented using Unicode block characters (`_`, `|`, `^`, or the more common `block_elements` set: `_`, `lower_block`, `full_block`, etc.).

**Effective resolution:** Using the 8 Unicode block elements (from U+2581 `lower_one_eighth_block` to U+2588 `full_block`), each character cell provides 8 levels of vertical resolution. A 40-character sparkline gives a 40x8 effective resolution histogram. With Braille characters, resolution increases to 40x4 dots (but only on/off, no intensity).

**Use case:** Inline in status lines, alongside tensor summaries. "Tensor X: shape [64, 128], range [-3.2, 4.1], mean 0.02 +- 0.7 `_-^-_-___`"

### 5.4 Box Plots and Violin Plots

**Box plots** in terminal: straightforward with Unicode box-drawing characters. Show median, quartiles, whiskers, outliers. Very compact (one line per distribution).

**Violin plots** are mirrored KDE curves, harder to render in terminal but possible with Braille or half-block characters. More informative than box plots for multimodal distributions.

### 5.5 Comparing Distributions

- **Overlaid histograms/KDE:** Two distributions on the same axes, different colors. Requires truecolor support for distinguishing overlapping regions.
- **Side-by-side:** Two distributions in adjacent panels. Easier to read in a terminal.
- **QQ-plot:** Quantile-quantile plot comparing two distributions. Renders as a scatter plot (Braille characters work well).
- **Statistical summary comparison:** Side-by-side display of mean, std, percentiles, skewness, kurtosis.

---

## 6. What Is Achievable in a Terminal

### 6.1 Half-Block Characters with Truecolor

Unicode half-block characters (`U+2580 UPPER HALF BLOCK`, `U+2584 LOWER HALF BLOCK`) split each character cell into two independently-colored pixels (foreground and background color). Combined with truecolor (24-bit RGB) escape sequences:

**Effective resolution:** 2 pixels per character cell vertically, 1 per cell horizontally. A terminal 120 columns x 40 rows provides 120x80 effective pixels. Because character cells are typically ~2:1 aspect ratio (taller than wide), half-block pixels are approximately square.

**Performance:** Escape sequences are ~20 bytes per pixel change (CSI color codes). A 120x80 pixel image is ~192KB of escape sequences. At modern terminal throughput (10+ MB/s for local terminals), this renders in under 20ms.

**Ratatui support:** Ratatui's `Canvas` widget supports `Marker::HalfBlock`, which provides this exact capability. The `HalfBlock` grid type supports foreground and background color per cell.

### 6.2 Braille Characters

Unicode Braille patterns (`U+2800` to `U+28FF`) encode a 2x4 grid of dots per character cell. Each dot is independently on or off.

**Effective resolution:** 2 pixels wide x 4 pixels tall per character cell. A 120x40 terminal gives 240x160 effective dots. Much higher resolution than half-block, but binary (on/off only, no color per dot -- only one foreground color per cell).

**Best for:** Line plots, scatter plots, sparklines. Not suitable for heatmaps (which need continuous color).

**Ratatui support:** `Marker::Braille` is a first-class marker type.

### 6.3 Octant Characters

A newer Unicode addition providing 2x4 grid of solid blocks (unlike Braille's dots). Each sub-cell is either the foreground or background color, giving a visual appearance of densely packed pixels without gaps.

**Effective resolution:** Same as Braille (2x4 per cell) but with filled pixels rather than dots.

**Ratatui support:** `Marker::Octant` exists but relies on characters not yet widely supported across terminals and fonts.

### 6.4 Kitty Graphics Protocol

The most capable approach. Programs send raw pixel data (RGBA) to the terminal emulator, which renders it as a raster image within the terminal grid.

**Resolution:** Arbitrary. The image can be any pixel dimension; the terminal allocates character cells to display it. A 400x200 pixel image might occupy 50x25 character cells depending on font size.

**Performance characteristics:**
- Image data is base64-encoded in escape sequences. A 200x100 RGBA image is 80KB raw, ~107KB base64.
- Zlib compression is supported (reduces ~60-70% for typical heatmap data due to spatial correlation).
- On local connections, shared memory transfer avoids base64 overhead entirely.
- Terminal-side rendering uses GPU (OpenGL) -- the bottleneck is transmission, not rendering.
- Estimated throughput for a compressed 200x100 heatmap: ~30-40KB of escape data, renders in <5ms on local terminal. Comfortable for 30+ FPS updates.

**Placement control:** Pixel-level positioning, z-index layering (image behind or above text), virtual placement (reference an already-transmitted image at multiple locations without re-sending data).

**Fallback chain:** Kitty protocol -> Sixel -> iTerm2 inline images -> half-block characters. Sixel supports up to 256 colors (no alpha) but is more widely supported. iTerm2 protocol is macOS-specific.

**Ratatui integration:** The `ratatui-image` crate supports Kitty protocol with fallback to halfblocks when protocol detection fails.

### 6.5 The Rendering Pipeline

For rocket_surgeon, the pipeline is:

```
Tensor data (from GPU via IPC)
  -> Select slice/aggregation
  -> Normalize to [0, 1] (or [-1, 1] for diverging)
  -> Apply colormap (lookup table: value -> RGB)
  -> Generate pixel buffer (RGBA)
  -> Encode for terminal protocol (Kitty base64, Sixel, or half-block escapes)
  -> Transmit to terminal
  -> Terminal renders
```

The colormap application step is embarrassingly parallel and trivially fast (one LUT lookup per element). The bottleneck for large tensors is the slice/aggregation step (which may involve GPU-to-CPU transfer) and the terminal transmission step.

### 6.6 Performance Budget

For interactive use, we target 10+ FPS for tensor view updates (100ms budget per frame):
- **Slice selection and aggregation:** <10ms for pre-cached tensors, potentially 10-50ms for GPU-to-CPU transfer of a new slice.
- **Colormap application:** <1ms for a 200x100 buffer (20K elements, one LUT lookup each).
- **Compression:** <5ms for zlib of ~80KB.
- **Transmission:** <10ms for ~30KB compressed data on local terminal.
- **Rendering:** <5ms (GPU-accelerated terminal).
- **Total:** ~30-80ms. Achievable.

For large tensors requiring GPU-side downsampling, the GPU kernel adds latency but can be pipelined with the previous frame's display.

---

## 7. Synthesis: What This Means for rocket_surgeon

### Priority Visualization Types (must-have)

1. **Tensor summary panel:** Shape, dtype, min/max/mean/std, inline sparkline histogram. This is the "always visible" quick-look.
2. **2D heatmap with colormap:** The workhorse view. Supports attention matrices, activation slices, weight slices. Kitty protocol for pixel-resolution, half-block fallback.
3. **Slice navigator:** For nD tensors, select which 2D slice to view. Keyboard-driven (arrow keys to move through batch/head/layer dimensions).
4. **Diff view:** Before/after comparison with diverging colormap. Critical for surgical intervention feedback.
5. **Distribution view:** Histogram or KDE of tensor values. Log-scale option. For monitoring weight/gradient health.

### Secondary Visualization Types (important)

6. **Attention pattern view:** Specialized heatmap with token labels on axes.
7. **Layer-wise summary:** Norms, statistics, gradient magnitudes across layers. Bar chart or sparkline per layer.
8. **MoE routing table:** Token-expert assignment heatmap plus utilization statistics.
9. **Logit lens view:** Layer-by-position probability heatmap.

### Deferred (future work)

10. **Embedding projector:** Requires dimensionality reduction (PCA feasible, t-SNE/UMAP expensive).
11. **Computation graph topology:** DAG rendering in terminal (possible with box-drawing characters but complex).
12. **Cross-sequence attention analysis:** AttentionViz-style joint embedding.

### Colormap Requirements

- At least one perceptually uniform sequential colormap (viridis).
- At least one diverging colormap (coolwarm or RdBu) for diffs and signed data.
- User-selectable, but sane defaults.
- Implementation: 256-entry RGB lookup tables, linearly interpolated. Ship as compile-time constants.

### Resolution Strategy

- Detect terminal capabilities at startup (Kitty protocol > Sixel > half-block).
- For Kitty: render at native data resolution up to ~400x400 pixels, downsample larger tensors with area averaging.
- For half-block fallback: render at 2x vertical resolution, use area averaging for any tensor larger than the cell grid.
- Always offer a "zoom" interaction that re-renders a subregion at full resolution.

---

## Bibliography

### Tools and Libraries

1. **TensorBoard** -- TensorFlow's Visualization Toolkit.
   GitHub: https://github.com/tensorflow/tensorboard
   Histogram docs: https://github.com/tensorflow/tensorboard/blob/master/docs/r1/histograms.md

2. **BertViz** -- Visualize Attention in Transformer Models (Jesse Vig).
   GitHub: https://github.com/jessevig/bertviz
   Paper: https://debug-ml-iclr2019.github.io/cameraready/DebugML-19_paper_2.pdf

3. **Treescope** -- Interactive HTML pretty-printer for ML research (Google DeepMind).
   GitHub: https://github.com/google-deepmind/treescope
   Docs: https://treescope.readthedocs.io/en/stable/
   Array visualization: https://treescope.readthedocs.io/en/v0.1.6/notebooks/array_visualization.html

4. **CircuitsVis** -- Mechanistic interpretability visualization (TransformerLens).
   GitHub: https://github.com/TransformerLensOrg/CircuitsVis

5. **Comgra** -- Computation Graph Analyzer (Florian Dietz).
   GitHub: https://github.com/FlorianDietz/comgra
   Paper: https://arxiv.org/abs/2407.21656

6. **TransformerLens** -- Mechanistic interpretability library.
   Docs: https://transformerlensorg.github.io/TransformerLens/generated/demos/Main_Demo.html

7. **nnsight** -- Neural network intervention library.
   nnterp paper: https://arxiv.org/abs/2511.14465

8. **AttentionViz** -- Global View of Transformer Attention (Yeh et al., 2023).
   Paper: https://arxiv.org/abs/2305.03210
   Interactive tool: https://catherinesyeh.github.io/attn-docs/

9. **torchviz** -- PyTorch execution graph visualization.
   GitHub: https://github.com/szagoruyko/pytorchviz

10. **Tuned Lens** -- Eliciting Latent Predictions from Transformers (Belrose et al., 2023).
    Paper: https://arxiv.org/abs/2303.08112
    GitHub: https://github.com/AlignmentResearch/tuned-lens
    Docs: https://tuned-lens.readthedocs.io/en/latest/index.html

11. **MixtureKit** -- Framework for MoE model visualization.
    Paper: https://arxiv.org/pdf/2512.12121

12. **MoE Expert Routing Visualization** (Martin Alderson).
    Blog: https://martinalderson.com/posts/moe-expert-routing-visualization/

### Terminal Graphics

13. **Kitty Graphics Protocol** -- Terminal pixel rendering protocol.
    Spec: https://sw.kovidgoyal.net/kitty/graphics-protocol/
    GitHub source: https://github.com/kovidgoyal/kitty/blob/master/docs/graphics-protocol.rst

14. **Terminal Graphics Protocols Comparison** (Akmatori).
    Blog: https://akmatori.com/blog/terminal-graphics-protocols

15. **ratatui** -- Rust terminal UI framework.
    Canvas widget docs: https://docs.rs/ratatui/latest/ratatui/widgets/canvas/struct.Canvas.html
    Marker types: https://docs.rs/ratatui/latest/ratatui/symbols/enum.Marker.html

16. **ratatui-image** -- Image rendering widget for ratatui.
    GitHub: https://github.com/ratatui/ratatui-image
    Crate: https://crates.io/crates/ratatui-image

### Colormaps and Visualization Theory

17. **Perceptually Uniform Colormaps** (matplotlib viridis family).
    Tutorial: https://riptutorial.com/matplotlib/example/11647/perceptually-uniform-colormaps
    Viridis introduction: https://sjmgarnier.github.io/viridis/articles/intro-to-viridis.html

18. **Color Map Advice for Scientific Visualization** (Kenneth Moreland).
    https://www.kennethmoreland.com/color-advice/

19. **Diverging Color Maps for Scientific Visualization** (Kenneth Moreland).
    https://www.kennethmoreland.com/color-maps/

20. **Choosing Colormaps in Matplotlib** (matplotlib docs).
    https://matplotlib.org/stable/users/explain/colors/colormaps.html

### Gradient Visualization

21. **Visualizing Gradients** (PyTorch Tutorials).
    https://docs.pytorch.org/tutorials/intermediate/visualizing_gradients_tutorial.html

22. **RNNbow: Visualizing Learning via Backpropagation Gradients in RNNs**.
    Paper: https://arxiv.org/pdf/1907.12545

### Dimensionality Reduction

23. **t-SNE vs. UMAP for High-Dimensional Embeddings**.
    Overview: https://eureka.patsnap.com/article/t-sne-vs-umap-visualizing-high-dimensional-embeddings-for-model-debugging

24. **TensorFlow Embedding Projector**.
    https://projector.tensorflow.org/

### Sparklines and Terminal Charts

25. **sparklines** -- Text-based sparklines for the command line (Edward Tufte concept).
    GitHub: https://github.com/deeplook/sparklines

26. **termgraph** -- Terminal graph drawing.
    GitHub: https://github.com/mkaz/termgraph
