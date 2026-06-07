# GPU & CPU 2D Rendering in Rust — Exhaustive Research Report

**Date:** 2026-05-19  
**Context:** rocket_surgeon TUI intermission — evaluating the Rust 2D rendering landscape for generating pixel-perfect visualizations (heatmaps, graph diagrams, attention maps) to be pushed to terminals via Kitty/Sixel graphics protocols.

**Architecture under evaluation:**
```
Tensor data (GPU/CPU)
  -> Layout (compute positions, sizes)
  -> Render (rasterize to pixel buffer)
  -> Encode (PNG for Kitty, Sixel, raw RGBA for shared memory)
  -> Transmit (escape sequences to terminal)
  -> Display (terminal GPU renders)
```

---

## 1. The Rust 2D Rendering Landscape

### 1.1 tiny-skia — CPU-Based Skia Subset

**Repository:** [linebender/tiny-skia](https://github.com/linebender/tiny-skia)  
**Current version:** 0.11.4  
**Size:** ~14 KLOC, compiles in <5s, adds ~200 KiB to binary  
**License:** BSD-3-Clause

tiny-skia is a minimal, CPU-only 2D rendering library ported from Skia's raster pipeline. It provides:

- Path filling and stroking with anti-aliasing
- Solid colors, gradients (linear, radial, two-point conical), and image patterns
- Porter-Duff blending modes
- Clipping (anti-aliased path clips)
- Pixmap (RGBA8 pixel buffer) as the primary render target

**Performance characteristics:**

- 20-100% slower than Skia on x86-64
- 100-300% slower than Skia on ARM (NEON optimization gap)
- Faster than cairo and raqote in many benchmarks
- SIMD-optimized raster pipeline for x86 (SSE2/AVX2), AArch64 (NEON), and WebAssembly (SIMD128)
- No GPU acceleration — purely CPU-bound
- No multithreading built in (but the pixel buffer can be split for external parallelism via rayon)

**Relevance to rocket_surgeon:** tiny-skia is the backend for resvg and is used by many Rust projects. For our use case, it is a solid choice for straightforward rendering tasks (heatmaps, simple charts) where GPU overhead is not justified. Its pixel buffer output (Pixmap) maps directly to what the Kitty protocol needs (RGBA8). The lack of text rendering is a limitation — you must composite text separately using fontdue or ab_glyph.

### 1.2 Vello — GPU Compute-Centric 2D Renderer

**Repository:** [linebender/vello](https://github.com/linebender/vello)  
**License:** Apache-2.0 OR MIT

Vello is the Linebender project's flagship 2D renderer, distinguished by its compute-shader-first architecture. Rather than traditional triangle rasterization, Vello dispatches the entire rendering pipeline (path flattening, tiling, compositing) as GPU compute shaders via wgpu.

**Three implementations as of late 2025:**

1. **Vello (GPU):** Full GPU compute pipeline. Requires wgpu with compute shader support. Best performance on scenes with many paths, gradients, and complex compositing. Scenes with 100K+ path segments render at interactive framerates.

2. **Vello CPU:** A CPU-only renderer using the "sparse strips" algorithm, developed as part of a Master's thesis at ETH Zurich by Laurenz Stampl (2025). The sparse strips method avoids the traditional scanline approach by processing only the strips of the image that contain geometry, skipping empty regions entirely. Benchmarks from July 2025 on Apple M1 Pro show Vello CPU taking second place behind Blend2D in many tests, often beating Skia, Cairo, and tiny-skia — especially as geometry size grows. Multithreaded mode available and particularly effective for large geometries with curves or complex paints (gradients, patterns).

3. **Vello Hybrid:** A GPU/CPU hybrid that uses compute shaders for some stages and CPU for others.

**Recent developments (2025-2026):**

- Features to switch between u8 and f32 pipelines in Vello CPU
- Eliminated overdraw for opaque image fills
- Reduced memory usage of wide tile commands
- New clipping algorithm for non-layer-based clipping
- Gaussian blur, drop shadow, and flood effects in Vello CPU

**Performance verdict:** Vello CPU is likely the fastest CPU-only 2D renderer in the Rust ecosystem as of late 2025. Vello GPU is the fastest option when a GPU is available. For rocket_surgeon, the GPU variant is appealing because tensor data may already be on GPU, but the requirement for headless/offscreen wgpu rendering adds complexity.

### 1.3 lyon — Path Tessellation Library

**Repository:** [nical/lyon](https://github.com/nical/lyon)  
**License:** MIT / Apache-2.0

lyon does not render pixels — it converts vector paths (Bezier curves, arcs, lines) into triangle meshes (vertex + index buffers) suitable for GPU rasterization via OpenGL, Vulkan, Metal, or wgpu. This is the classic "tessellation then rasterization" approach.

**Core capabilities:**

- Fill tessellation: converts filled paths to triangles using monotone decomposition (single-pass, >2x faster than libtess2)
- Stroke tessellation: converts stroked paths to triangle geometry with configurable line caps, joins, and miter limits
- Path flattening: converts Bezier curves to polylines for downstream consumption
- Output: vertex buffer (positions + custom attributes) + index buffer (triangle indices)

**Relevance to rocket_surgeon:** lyon is useful if we choose to render via a wgpu render pipeline with custom vertex/fragment shaders. For heatmaps this is overkill — heatmaps are axis-aligned rectangles, not complex paths. For graph/network diagrams with curved edges, lyon could tessellate the edge splines into triangles for GPU rendering. However, Vello's compute-shader approach achieves the same result with less user-side complexity.

### 1.4 wgpu — Low-Level GPU Abstraction

**Repository:** [gfx-rs/wgpu](https://github.com/gfx-rs/wgpu)  
**License:** MIT / Apache-2.0

wgpu is the Rust implementation of the WebGPU standard, abstracting over Vulkan (Linux/Windows/Android), Metal (macOS/iOS), DX12 (Windows), and OpenGL (fallback). It provides:

- Render pipelines (vertex + fragment shaders, rasterization)
- Compute pipelines (compute shaders for GPGPU)
- Texture management, buffer management, bind groups
- Headless/offscreen rendering (no window required)

**Using wgpu directly for 2D rendering:**

For rocket_surgeon's specific needs (heatmaps, graph diagrams), wgpu enables:

1. **Custom compute shaders:** A WGSL compute shader can read a matrix buffer, apply a colormap, and write directly to a storage texture — all on GPU with zero CPU involvement in the pixel generation step.

2. **Custom fragment shaders:** For more complex visualizations, render a quad with a fragment shader that samples data textures and applies coloring logic.

3. **Headless readback pipeline:** Create a texture with `RENDER_ATTACHMENT | COPY_SRC`, render to it, copy to a staging buffer with `MAP_READ | COPY_DST`, and map the buffer to CPU memory. The Learn wgpu tutorial documents this pattern extensively.

**The zero-copy dream:** If tensor data is already on GPU (coming from PyTorch), and the visualization is rendered on GPU, and the result goes to a terminal via shared memory — we could theoretically avoid CPU touching the pixel data entirely. In practice, wgpu and PyTorch use different GPU memory allocators, so some copying is unavoidable, but the compute shader approach still avoids the CPU-bound colormap application.

### 1.5 femtovg — NanoVG Port for Rust

**Repository:** [femtovg/femtovg](https://github.com/femtovg/femtovg)  
**License:** MIT / Apache-2.0

femtovg is a GPU-accelerated 2D vector drawing library ported from NanoVG. It provides a Canvas-style API loosely modeled on HTML5 Canvas. Features include:

- Anti-aliased path fill and stroke
- Solid color, image pattern, and gradient fills (box, linear, radial)
- Various stroke caps and joins
- Composition modes
- Text rendering (built-in)
- Backends: OpenGL (ES) 3.0+ and wgpu

**Relevance to rocket_surgeon:** femtovg's built-in text rendering is an advantage over tiny-skia (which has none). The Canvas API is familiar and productive. However, femtovg requires a GPU context (OpenGL or wgpu), which means dealing with surface creation even for offscreen rendering. The wgpu backend is relatively new. For our use case, if we are already using wgpu for compute shaders, femtovg could layer on top as a convenience API for the diagram-drawing parts of the visualization.

### 1.6 resvg — SVG Rendering

**Repository:** [linebender/resvg](https://github.com/linebender/resvg)  
**License:** MPL-2.0

resvg renders SVG documents to pixel buffers using tiny-skia as its rasterizer. Architecture:

1. **usvg** parses and simplifies SVG into a normalized tree (resolves styles, transforms, references)
2. **resvg** walks the usvg tree and rasterizes to a tiny-skia Pixmap

**Performance:** Fast enough for static SVG rendering, producing pixel-identical results across platforms (deterministic). The total library is ~75K LOC including usvg. The CLI binary is <3 MB with no external dependencies.

**Relevance to rocket_surgeon:** The appeal is defining visualizations as SVG and rendering to pixels. This would give us a declarative visualization format. However, for interactive 60fps rendering of heatmaps, SVG parsing overhead is unnecessary — we would be generating SVG strings only to immediately parse and rasterize them. This makes sense only for complex, infrequently-updated visualizations like static architecture diagrams. For dynamic heatmaps and attention maps, direct pixel generation is faster.

### 1.7 plotters — Rust Plotting Library

**Repository:** [plotters-rs/plotters](https://github.com/plotters-rs/plotters)  
**License:** MIT

plotters provides a high-level charting API with multiple backends:

- **BitMapBackend:** Renders to an in-memory RGBA pixel buffer
- **SVGBackend:** Renders to SVG
- **CanvasBackend:** For web (WASM)

**Supported chart types:** Line series, point series, candlestick series, histogram, area charts, and custom drawing elements. The API allows composing charts from drawing primitives (lines, rectangles, text, circles).

**Performance considerations:** The default BitMapBackend is CPU-based and performs all rasterization on the CPU. For high-FPS real-time plots, this can be a bottleneck. The plotters-conrod backend offloads rendering to the GPU via OpenGL primitives, but this requires a window context.

**Relevance to rocket_surgeon:** plotters could be useful for generating standard charts (loss curves, metric time series) to pixel buffers. The BitMapBackend output is directly usable with the Kitty protocol. However, for heatmaps (our primary use case), plotters' `MatrixChart` is basic and adds unnecessary abstraction. For graph diagrams, plotters is not suited at all. Verdict: potentially useful for ancillary chart types, but not as the primary rendering engine.

### 1.8 image Crate — Pixel Buffer Manipulation

**Repository:** [image-rs/image](https://github.com/image-rs/image)  
**License:** MIT / Apache-2.0

The `image` crate is the standard Rust library for image I/O and manipulation. It provides:

- `ImageBuffer<P, Container>`: Generic pixel buffer with typed pixel formats (Rgba, Rgb, Luma, etc.)
- PNG encoding/decoding via the `png` crate
- JPEG, BMP, GIF, WebP, and other format support
- Basic image operations (resize, crop, rotate, flip)

**PNG encoding performance:** The Rust `png` crate includes a fast encoding mode powered by `fdeflate` that is dramatically faster than libpng's fastest mode while providing better compression ratios. Benchmarks show 1.8x improvement over libpng on x86, 1.5x on ARM. This is relevant because the Kitty protocol's `f=100` mode uses PNG.

**Relevance to rocket_surgeon:** This crate is essential regardless of which rendering library we choose — it is the final step before encoding for terminal transmission. Every rendering pipeline terminates in an RGBA pixel buffer that must be encoded (PNG for Kitty, or consumed raw for shared memory transfer).

### 1.9 ab_glyph / fontdue — Text Rasterization

**ab_glyph** ([alexheretic/ab-glyph](https://github.com/alexheretic/ab-glyph)): Fast API for loading, scaling, positioning, and rasterizing OpenType font glyphs. A rewrite of rusttype focused on performance. Supports both .ttf and .otf fonts.

**fontdue** ([mooman219/fontdue](https://github.com/mooman219/fontdue)): Claims to be "the fastest font renderer in the world." Pure Rust, no dependencies. Designed as a replacement for rusttype and ab_glyph with the lowest end-to-end latency for glyph rasterization.

**Performance comparison:** fontdue has the lowest rasterization latency per glyph. ab_glyph is faster than rusttype for layout operations and especially fast for .otf fonts. Both output alpha coverage bitmaps that must be composited onto the target pixel buffer.

**Relevance to rocket_surgeon:** Text labels on heatmaps, axis labels on charts, node labels on graph diagrams — all require glyph rasterization. fontdue is the performance winner for isolated glyph rendering. For layout-heavy text (paragraphs, wrapped text), ab_glyph's layout capabilities are more mature. Our use case is mostly short labels, so fontdue is the better fit.

---

## 2. The Rendering Pipeline for Terminal Graphics

### 2.1 Pipeline Stages and Bottlenecks

```
Stage 1: DATA PREPARATION
  Tensor data (f32/f16 on GPU) -> normalize to [0,1] range
  Bottleneck: GPU->CPU copy if data is on GPU
  Budget: ~1-2ms for 128x128 f32 matrix (64 KiB)

Stage 2: LAYOUT
  Compute positions, sizes, margins, axis labels
  Bottleneck: negligible for typical visualizations
  Budget: <0.1ms

Stage 3: COLORMAP APPLICATION
  Map normalized [0,1] values to RGBA colors
  Bottleneck: CPU cache misses if naive; SIMD-friendly if done right
  Budget: <0.5ms for 128x128 values (see Section 3)

Stage 4: RASTERIZATION / UPSCALING
  Scale from source resolution to target pixel resolution
  Bottleneck: interpolation compute for bilinear; trivial for nearest-neighbor
  Budget: <1ms for 400x200 target

Stage 5: COMPOSITING
  Overlay text labels, axes, borders onto pixel buffer
  Bottleneck: text rasterization, alpha blending
  Budget: <2ms

Stage 6: ENCODING
  PNG encoding for Kitty (f=100), or raw RGBA (f=32), or Sixel
  Bottleneck: PNG compression is the dominant cost
  Budget: PNG ~2-5ms for 400x200; raw ~0ms; Sixel ~5-20ms

Stage 7: TRANSMISSION
  Base64 encode (for Kitty over pipe) or shared memory (local)
  Bottleneck: base64 encoding + TTY write throughput
  Budget: ~1-2ms base64 for 400x200 PNG; ~0ms shared memory

Stage 8: TERMINAL RENDERING
  Terminal's GPU textures and displays the image
  Bottleneck: out of our control; kitty is GPU-accelerated (fast)
  Budget: <1ms on kitty
```

**Total budget for 60fps: 16.67ms per frame.** With the above estimates, we are well within budget even with PNG encoding. The critical optimization is choosing the right encoding path.

### 2.2 Encoding Path Selection

**Kitty raw RGBA (f=32, no PNG):** The fastest encoding path. No compression overhead. For a 400x200 RGBA image, that is 320,000 bytes of raw pixel data. With base64 encoding (4/3 overhead), that becomes ~427 KB transmitted via escape sequences. This is the fastest local option if bandwidth is not a concern.

**Kitty PNG (f=100):** PNG compression reduces the 320 KB to typically 20-80 KB for heatmap-style images (large uniform color regions compress well). The Rust `png` crate's fast encoder can handle 400x200 in ~2-5ms. Over SSH, this is strongly preferred due to reduced bandwidth.

**Kitty shared memory (t=s):** On a local machine, write raw RGBA to a POSIX shared memory object, pass the shm name to kitty. Zero base64 overhead, zero compression overhead. This is the absolute fastest path: write 320 KB to shared memory, send a ~100 byte escape sequence. Kitty deletes the shm after reading. **This is the path we should optimize for in the local case.**

**Kitty file transfer (t=f):** Write to a temp file, pass the path. Similar to shared memory but with filesystem overhead.

**Sixel:** Notoriously slower than Kitty. Sixel encodes images as a series of six-row-high bands, with RLE compression per band. Color registers are limited (typically 256-1024, terminal-dependent). A 400x200 image requires quantizing to the color register limit, then encoding each band. Modern fast sixel encoders use "branchless bit twiddling" hacks with SIMD for the encoding step, and kmiya's sixel format (designed for terminal emulators rather than dot-matrix printers) reduces transport overhead. Even so, sixel encoding of a 400x200 image typically takes 5-20ms, and the encoded output is often larger than PNG.

### 2.3 Latency Budget Analysis

For a 128x128 attention heatmap rendered at 400x200 pixels with labels, targeting the Kitty protocol on a local machine:

| Stage | Optimistic | Conservative |
|-------|-----------|--------------|
| Data prep (CPU copy) | 0.1ms | 2ms |
| Layout | 0.05ms | 0.1ms |
| Colormap | 0.1ms | 0.5ms |
| Upscale | 0.2ms | 1ms |
| Compositing (text) | 0.5ms | 2ms |
| Encoding (shm path) | 0.1ms | 0.5ms |
| Transmission | 0.05ms | 0.2ms |
| **Total** | **~1.1ms** | **~6.3ms** |

Both cases are well under the 16ms budget. Even with PNG encoding instead of shared memory (add 2-5ms), we remain under budget. **The bottleneck is not rendering speed — it is making the architecture clean and maintainable.**

---

## 3. Heatmap Rendering — The Critical Path

### 3.1 The Fastest Heatmap Pipeline

For rendering an NxN matrix (e.g., 128x128 attention weights) as a colored pixel image:

```
Step 1: Normalize matrix to [0.0, 1.0] — in-place f32 ops
Step 2: Apply colormap via LUT — 256-entry RGBA lookup table
Step 3: Upscale to target resolution — nearest-neighbor or bilinear
Step 4: Composite labels/axes — fontdue + alpha blend
```

### 3.2 Colormap Application: LUT vs. Interpolation

**Lookup Table (LUT) approach — recommended:**

Pre-compute a 256-entry `[u8; 4]` array for the chosen colormap (Viridis, Inferno, Magma, Plasma, etc.). To map a float value to a color:

```rust
let index = (value.clamp(0.0, 1.0) * 255.0) as usize;
let color = LUT[index]; // [r, g, b, a]
```

This is a single float multiply, a clamp, a cast, and an array lookup. For 128x128 = 16,384 values, this is trivially fast — the entire LUT fits in L1 cache (1 KiB for 256 RGBA entries).

The `colorous` crate provides pre-computed LUTs for all standard scientific colormaps (Viridis, Inferno, Magma, Plasma, Turbo, Cividis, and many more). Each colormap is a function from `f64 -> Color`, but internally uses polynomial approximations or LUT interpolation.

**SIMD-accelerated colormap application:**

With Rust's portable SIMD (nightly) or manual SSE2/AVX2 intrinsics, we can process 4-8 float values simultaneously:

1. Load 8 f32 values into an AVX2 register
2. Multiply by 255.0, clamp, convert to i32
3. Gather from the LUT using `_mm256_i32gather_epi32`
4. Store the 8 RGBA values

This yields approximately 3-4x speedup over scalar code for the colormap step specifically. However, since the scalar version for 16K values is already sub-microsecond, SIMD is only relevant for very large matrices (1024x1024+) or when batching many heatmaps.

**Interpolation approach:**

Instead of a 256-entry LUT, use a smaller LUT (e.g., 16 control points) and linearly interpolate between entries. This provides smoother color transitions at the cost of more computation per pixel. For scientific visualization, the 256-entry LUT approach is standard and produces perceptually smooth results with all standard colormaps.

### 3.3 Upscaling: Source 128x128 to Target 400x200

**Nearest-neighbor:** Each source pixel maps to a rectangular block of target pixels. For 128->400 horizontal (3.125x), each source column maps to 3 or 4 target columns. This preserves exact source values — every target pixel's color corresponds to exactly one source cell. Fast (no interpolation math), and appropriate when the user wants to see exact per-cell values. Essentially: `target[x][y] = source[x * src_w / tgt_w][y * src_h / tgt_h]`.

**Bilinear interpolation:** Samples the 4 nearest source pixels and blends based on fractional position. Produces smoother results but "invents" intermediate values that do not exist in the source data. Appropriate for producing visually smooth heatmaps where the exact cell boundaries are not important. 2-3x slower than nearest-neighbor but still sub-millisecond for our target sizes.

**Recommendation:** Default to nearest-neighbor for attention maps (users want to see exact attention weights per head/position). Offer bilinear as a user-togglable option for aesthetic preference.

### 3.4 Performance Estimate

For a 128x128 -> 400x200 heatmap with Viridis colormap, nearest-neighbor upscale, no text:

- Normalize 16,384 f32 values: ~5us
- LUT colormap application (16,384 lookups): ~10us
- Nearest-neighbor upscale to 400x200 (80,000 pixels): ~30us
- Write to RGBA buffer: included in upscale step
- **Total: ~45us (0.045ms)**

This is effectively instantaneous. Even with text compositing adding 0.5-2ms, the entire heatmap render is under 3ms. **Heatmap rendering is not a performance bottleneck.**

---

## 4. Graph / Network Diagram Rendering

### 4.1 Box-and-Line Diagrams for Model Architecture

Rendering transformer architecture as box-and-line diagrams requires:

- Rectangular nodes (layer boxes) with text labels
- Directed edges (arrows) connecting outputs to inputs
- Edge routing that avoids crossing boxes
- Optional: grouping (encoder/decoder sections), MoE expert routing indicators

### 4.2 Layout Algorithms

**Hierarchical / Layered (Sugiyama-style):** The standard for DAG visualization. Assigns nodes to layers (ranks), orders nodes within layers to minimize crossings, positions nodes vertically. This is what Graphviz's `dot` uses. The `layout-rs` crate implements a Graphviz-compatible layout engine in pure Rust.

**Force-directed (Fruchterman-Reingold, etc.):** Treats edges as springs and nodes as repelling charges. Produces organic layouts suitable for undirected graphs. Not ideal for DAG-structured neural networks.

**Recommendation for rocket_surgeon:** Hierarchical layout for the main model architecture view. The data flows top-to-bottom (or left-to-right), matching the mental model of a forward pass. Use `layout-rs` or implement a simplified Sugiyama layout (our architectures are relatively simple DAGs).

### 4.3 Edge Routing

Graphviz's orthogonal edge routing (`splines=ortho`) uses a three-stage approach:

1. Construct an orthogonal visibility graph from node bounding boxes
2. Find shortest paths in the visibility graph (optimal routes)
3. Center and nudge routes to avoid overlap

Known limitations: Graphviz's ortho routing fails with port-specific edge connections and cannot handle edge labels well. For rocket_surgeon, orthogonal routing is visually clean for architecture diagrams and aligns with the rectilinear aesthetic of technical diagrams.

For simpler cases (and our architecture diagrams are relatively simple), direct polyline routing with obstacle avoidance is sufficient: route edges as L-shaped or Z-shaped paths between connection points, offsetting parallel edges to avoid overlap.

### 4.4 Rendering to Pixels

Once layout is computed, rendering the actual pixels requires:

- **Filled rectangles:** trivial — write RGBA values directly to the pixel buffer
- **Anti-aliased lines:** This is where a 2D renderer (tiny-skia, vello_cpu) earns its keep. Anti-aliased diagonal and curved lines at small pixel scales require subpixel coverage computation.
- **Text labels at small sizes:** fontdue rasterizes glyphs at the requested pixel size. Below ~10px, most fonts become unreadable. At 8px, only a few fonts (bitmap fonts like Terminus, or specifically hinted fonts) remain legible. For pixel-rendered diagrams at typical terminal DPI, node labels should be at minimum 11-12px.
- **Arrows / arrowheads:** Small triangles at edge endpoints. Can be rendered as filled paths via tiny-skia or as direct pixel manipulation for axis-aligned arrows.

### 4.5 Rust Graph Layout Libraries

| Library | Approach | Output | Notes |
|---------|----------|--------|-------|
| `layout-rs` | Graphviz-compatible | SVG or custom | Parses DOT format, renders layout |
| `petgraph` | Data structure | DOT export | No rendering, but standard graph data structure |
| `egui_graphs` | Interactive | egui widget | Requires egui context |
| `egraph-rs` | Force-directed | Custom | Python bindings, network viz focus |

**Recommendation:** Use `petgraph` for the graph data structure and implement a simplified hierarchical layout algorithm ourselves (in keeping with the "no dependencies" philosophy). Render the final diagram using tiny-skia or vello_cpu for anti-aliased output.

---

## 5. Performance Optimization Techniques

### 5.1 SIMD for Bulk Pixel Operations

Rust's SIMD capabilities (stable `std::arch` intrinsics, nightly `std::simd` portable SIMD) can accelerate:

- **Colormap application:** Gather-based LUT lookup (AVX2 `_mm256_i32gather_epi32`) processes 8 values per instruction.
- **Alpha blending:** SSE2/NEON can process 4 RGBA pixels simultaneously for compositing text overlays.
- **Normalization:** Bulk f32 multiply-add to normalize a matrix to [0,1] range.

In image processing applications, portable SIMD has demonstrated 3x speedup for pixel manipulation operations compared to scalar code. The `fast_image_resize` crate demonstrates SIMD-accelerated image resizing in Rust.

tiny-skia already uses SIMD internally (SSE2, AVX2, NEON, WASM SIMD128) for its raster pipeline. Vello CPU also leverages SIMD extensively in its sparse strips renderer.

### 5.2 Parallel Rendering with rayon

For pixel buffer generation, rayon enables data-parallel rendering:

- **Row-parallel heatmap:** Each row of the output image can be computed independently. `pixel_buffer.par_chunks_mut(row_stride)` splits the buffer into row-sized chunks processed in parallel.
- **Tile-parallel compositing:** Split the image into rectangular tiles (e.g., 64x64) and render each tile on a separate thread.
- **Batch image generation:** When rendering multiple visualizations (e.g., all attention heads), process them in parallel with `heads.par_iter().map(render_heatmap)`.

For our typical image sizes (400x200 = 80K pixels), the parallelism overhead of rayon's work-stealing scheduler may exceed the computation time. Parallelism pays off at 1000x1000+ pixels or when compositing many layers. For batch rendering of multiple attention heads (e.g., 12-96 heads), rayon parallelism across heads is highly effective.

### 5.3 Incremental Rendering

For interactive debugging where a single tensor value changes:

- **Dirty region tracking:** Maintain a bounding box of changed cells in the source matrix. Only re-render the pixel region corresponding to those cells.
- **Double buffering:** Maintain two pixel buffers. Render changes into the back buffer, then swap. The Kitty protocol supports image replacement by ID, so we can update in-place.
- **Layer compositing:** Render the heatmap base layer once, keep text labels and axes as separate layers. When data changes, only re-render the base layer and composite.

The Kitty protocol's animation support (frame-based updates with `a=f` for frame composition) enables efficient incremental updates where only changed regions are transmitted.

### 5.4 Caching

- **Colormap LUT:** Computed once at startup, reused for all renders. 1 KiB per colormap.
- **Font glyphs:** fontdue caches rasterized glyphs by (character, size) pair. For fixed-size labels, the glyph cache saturates after the first render.
- **Layout:** If the model architecture hasn't changed, cache the node/edge positions. Only re-render if the graph structure changes.
- **Rendered images:** If the underlying data hasn't changed (e.g., user is scrolling through layers but a particular layer's attention hasn't been re-computed), serve the cached pixel buffer.

### 5.5 Memory Layout

RGBA pixel buffers should be stored in row-major order (standard for image formats). This ensures:

- Sequential memory access during row-parallel rendering
- Cache-friendly access patterns for horizontal scanline operations
- Direct compatibility with PNG encoders and Kitty protocol (which expect row-major RGBA)

For the source matrix, ensure f32 values are in row-major contiguous memory. If coming from PyTorch tensors, call `.contiguous()` before copying to Rust to avoid stride-based scattered reads.

---

## 6. Graceful Degradation: One Pipeline, Multiple Targets

### 6.1 The Output Hierarchy

From best to worst quality:

| Target | Resolution | Colors | Quality |
|--------|-----------|--------|---------|
| Kitty (f=32 raw RGBA) | Full pixel | 16M (truecolor) | Highest |
| Kitty (f=100 PNG) | Full pixel | 16M (truecolor) | Highest (compressed) |
| Sixel | Full pixel | 256-1024 registers | High (quantized) |
| iTerm2 inline images | Full pixel | 16M (truecolor) | High |
| Half-block + truecolor | 1x2 per cell | 2 colors/cell | Medium |
| Sextant + truecolor | 2x3 per cell | 2 colors/cell | Medium-low |
| Braille + truecolor | 2x4 per cell | 1 color/cell (binary) | Low (binary) |
| Block elements + 256 color | 1x1 per cell | 256 palette | Low |
| Plain ASCII | 1x1 per cell | None | Minimal |

### 6.2 Unified Rendering Architecture

The key insight from ratatui-image is the **Picker** pattern: detect terminal capabilities at startup, then select the best available rendering backend. The rendering pipeline should be:

```
                          +--> Kitty encoder --> escape sequences
                          |
Source data --> Renderer --+--> Sixel encoder --> escape sequences
   (always produces       |
    RGBA pixel buffer)    +--> Half-block encoder --> styled characters
                          |
                          +--> Braille encoder --> styled characters
                          |
                          +--> ASCII encoder --> plain characters
```

**The renderer always produces an RGBA pixel buffer.** The encoder stage adapts to the target protocol. This means:

1. **For pixel protocols (Kitty, Sixel, iTerm2):** Render at full target resolution (e.g., 400x200 pixels). Encode as PNG, raw RGBA, or Sixel respectively.

2. **For half-block:** Render at 2x the character grid resolution vertically (e.g., 80x48 for an 80x24 area). Each pair of vertically adjacent pixels becomes one half-block character with fg/bg colors.

3. **For Braille:** Render at 2x horizontal, 4x vertical character grid resolution (e.g., 160x96 for 80x24). Threshold each pixel to on/off, pack into Braille codepoints.

4. **For ASCII:** Map pixel intensity to ASCII characters (` .:-=+*#%@` or similar density ramp).

### 6.3 ratatui-image's Approach

The `ratatui-image` crate implements exactly this pattern:

1. **Picker::from_query_stdio():** Queries the terminal for Kitty, Sixel, and iTerm2 support. Falls back to halfblocks if nothing is detected.
2. **Font size detection:** Queries the terminal for cell dimensions in pixels (needed to map image pixels to character cells accurately).
3. **Protocol backends:** Each protocol implements a common trait for encoding `DynamicImage` to terminal output.
4. **Halfblock fallback:** Always available. Uses `▄` with fg/bg truecolor. Works in every modern terminal.

For rocket_surgeon, we should follow this pattern but with our own rendering pipeline feeding into the protocol encoders. We do not need ratatui-image's DynamicImage dependency chain — we generate pixel buffers directly.

---

## 7. GPU Compute for Visualization Rendering

### 7.1 The Case for GPU Rendering

The tensor data is on the GPU (coming from PyTorch). The visualization is a simple transformation of that data (normalize, apply colormap, upscale). Why copy to CPU at all?

**GPU compute shader heatmap pipeline:**

```wgsl
@group(0) @binding(0) var<storage, read> matrix: array<f32>;
@group(0) @binding(1) var<storage, read> colormap: array<vec4<f32>, 256>;
@group(0) @binding(2) var output: texture_storage_2d<rgba8unorm, write>;

@compute @workgroup_size(16, 16)
fn main(@builtin(global_invocation_id) id: vec3<u32>) {
    let src_x = id.x * src_width / tgt_width;
    let src_y = id.y * src_height / tgt_height;
    let value = matrix[src_y * src_width + src_x];
    let index = u32(clamp(value, 0.0, 1.0) * 255.0);
    textureStore(output, vec2<i32>(i32(id.x), i32(id.y)), colormap[index]);
}
```

This shader runs entirely on GPU: reads the matrix, applies the colormap, upscales via nearest-neighbor, and writes the result to a texture. For a 400x200 output, that is 80,000 invocations — trivial for any GPU.

### 7.2 The GPU->CPU Readback Cost

The problem: after the GPU renders the visualization, we need the pixels on the CPU to send via the Kitty protocol (even shared memory requires CPU-accessible memory). The readback path:

1. GPU compute shader writes to a storage texture
2. `encoder.copy_texture_to_buffer()` copies texture to a staging buffer
3. `staging_buffer.map_async(MapMode::Read)` maps the staging buffer
4. CPU reads the mapped data and transmits to terminal

The readback latency is dominated by GPU-CPU synchronization (pipeline flush + DMA transfer). For a 400x200x4 = 320 KB buffer, the DMA transfer is fast (~0.1ms), but the synchronization overhead can add 0.5-2ms depending on GPU driver behavior and any outstanding GPU work.

### 7.3 Is GPU Rendering Faster for Our Use Case?

**For small heatmaps (128x128 source, 400x200 target):** Probably not. The CPU can generate this in ~0.05ms. The GPU readback overhead alone (0.5-2ms) exceeds the total CPU rendering time. GPU rendering wins only when the computation is heavy enough to justify the synchronization cost.

**For large heatmaps (1024x1024 source, 2000x2000 target):** GPU rendering starts to win. The CPU time scales with pixel count (4M pixels at ~2-5ms), while the GPU time barely changes (the shader is trivially parallel) and readback for 16 MB is ~1-2ms.

**For multi-head batch rendering (96 heads, each 128x128):** GPU wins decisively. A single compute dispatch can render all 96 heatmaps in parallel. On CPU, even with rayon, this is 96 * 0.05ms = ~5ms serialized or ~1ms with 8-thread parallelism. On GPU, the total compute time is near-zero; readback of 96 * 320KB = 30 MB is ~2-3ms.

**For the tensor-already-on-GPU case:** GPU rendering avoids the GPU->CPU copy of the raw tensor data (which would be 128*128*4 = 64 KB per head). This is a small win but simplifies the data pipeline.

### 7.4 wgpu Headless Compute Architecture

wgpu supports fully headless operation (no window surface needed):

```rust
let instance = wgpu::Instance::new(Default::default());
let adapter = instance.request_adapter(&wgpu::RequestAdapterOptions {
    power_preference: wgpu::PowerPreference::HighPerformance,
    ..Default::default()
}).await;
let (device, queue) = adapter.request_device(&Default::default()).await;
// Create compute pipeline, dispatch, readback — no surface needed
```

This works on all backends (Vulkan, Metal, DX12). The output texture is created with `TextureUsages::STORAGE_BINDING | TextureUsages::COPY_SRC`, and a staging buffer with `BufferUsages::MAP_READ | BufferUsages::COPY_DST`.

### 7.5 The PyTorch-wgpu Interop Problem

PyTorch uses CUDA (or ROCm, or MPS) for GPU memory. wgpu uses Vulkan/Metal/DX12. These are different GPU APIs with different memory allocators. Sharing GPU buffers between them requires:

- **Vulkan external memory extensions** (VK_KHR_external_memory): Export CUDA memory as a Vulkan external handle. This is possible but complex and NVIDIA-specific.
- **CUDA-Vulkan interop:** NVIDIA provides `cudaImportExternalMemory` and `vkGetMemoryFdKHR` for sharing memory between CUDA and Vulkan on Linux.
- **Simpler path:** Copy tensor to CPU (PyTorch `.cpu().numpy()`), then upload to wgpu buffer. This is the pragmatic approach and adds ~0.1ms for small tensors.

For rocket_surgeon's architecture (Rust process communicates with Python/PyTorch worker process via IPC), the tensor data crosses a process boundary anyway. The simplest path is: PyTorch copies tensor to shared memory (CPU), Rust reads from shared memory, renders on CPU or uploads to GPU. Direct GPU-GPU interop across processes is possible (via CUDA IPC or Vulkan external memory) but adds enormous complexity.

**Pragmatic recommendation:** Start with CPU rendering (tiny-skia or direct pixel buffer manipulation). It is fast enough for all our target sizes. Reserve GPU rendering via wgpu compute shaders for the future optimization of batch rendering large visualizations.

---

## 8. Recommendations for rocket_surgeon

### 8.1 Primary Rendering Stack

| Component | Choice | Rationale |
|-----------|--------|-----------|
| Heatmaps | Direct pixel buffer manipulation | Sub-millisecond for our sizes; no library needed |
| Colormaps | `colorous` crate or hand-rolled 256-entry LUT | Fast, correct, perceptually uniform |
| Graph diagrams | tiny-skia or vello_cpu | Anti-aliased lines and paths |
| Text labels | fontdue | Fastest glyph rasterization |
| Pixel buffer type | `Vec<u8>` (RGBA8 row-major) | Direct compatibility with all encoders |
| PNG encoding | `png` crate (via `image`) | Fastest Rust PNG encoder |
| Image manipulation | `image` crate | Resize, format conversion |
| Terminal protocol | Custom Kitty/Sixel/halfblock encoders | Following ratatui-image's Picker pattern |

### 8.2 Rendering Pipeline Architecture

```rust
// The core abstraction
pub struct PixelBuffer {
    data: Vec<u8>,      // RGBA8, row-major
    width: u32,
    height: u32,
}

// Rendering produces pixel buffers
fn render_heatmap(matrix: &[f32], rows: usize, cols: usize,
                  colormap: &ColorMap, target_w: u32, target_h: u32)
    -> PixelBuffer;

fn render_graph(graph: &Graph, layout: &Layout,
                target_w: u32, target_h: u32)
    -> PixelBuffer;

// Encoding adapts to terminal capabilities
fn encode_kitty_raw(buf: &PixelBuffer) -> Vec<u8>;    // escape sequences
fn encode_kitty_png(buf: &PixelBuffer) -> Vec<u8>;    // escape sequences
fn encode_kitty_shm(buf: &PixelBuffer) -> ShmHandle;  // shared memory
fn encode_sixel(buf: &PixelBuffer) -> Vec<u8>;         // escape sequences
fn encode_halfblock(buf: &PixelBuffer) -> StyledText;  // ratatui content
fn encode_braille(buf: &PixelBuffer) -> StyledText;    // ratatui content
```

### 8.3 Phase Strategy

**Phase 1 (immediate):** Direct pixel buffer heatmaps + Kitty PNG encoding. No rendering library dependency. Handles 90% of our visualization needs.

**Phase 2 (when graph diagrams are needed):** Add tiny-skia or vello_cpu for anti-aliased path rendering. Add fontdue for text labels.

**Phase 3 (optimization, if needed):** wgpu compute shaders for batch rendering of large attention maps. Shared memory Kitty protocol path for zero-copy local display.

---

## 9. Bibliography

### Rendering Libraries

1. tiny-skia — Linebender. "A tiny Skia subset ported to Rust." [GitHub](https://github.com/linebender/tiny-skia). [Docs](https://docs.rs/crate/tiny-skia/latest).
2. Vello — Linebender. "A GPU compute-centric 2D renderer." [GitHub](https://github.com/linebender/vello). [Docs](https://docs.rs/vello).
3. Stampl, Laurenz. "High-performance 2D graphics rendering on the CPU using sparse strips." Master's thesis, ETH Zurich, 2025. [PDF](https://ethz.ch/content/dam/ethz/special-interest/infk/inst-pls/plf-dam/documents/StudentProjects/MasterTheses/2025-Laurenz-Thesis.pdf). [GitHub](https://github.com/LaurenzV/master-thesis).
4. lyon — Nicolas Silva. "2D graphics rendering on the GPU in Rust using path tessellation." [GitHub](https://github.com/nical/lyon). [Introduction](https://nical.github.io/posts/lyon-intro.html).
5. wgpu — gfx-rs team. "Safe and portable GPU abstraction in Rust." [Docs](https://docs.rs/wgpu/). [Learn wgpu tutorial — Headless rendering](https://sotrh.github.io/learn-wgpu/showcase/windowless/).
6. femtovg — "Antialiased 2D vector drawing library written in Rust." [GitHub](https://github.com/femtovg/femtovg). [Docs](https://docs.rs/femtovg).
7. resvg — Linebender. "An SVG rendering library." [GitHub](https://github.com/linebender/resvg). [Docs](https://docs.rs/crate/resvg/latest).
8. plotters — Hao Hou. "A Rust drawing library for high quality data plotting." [GitHub](https://github.com/plotters-rs/plotters). [Docs](https://docs.rs/plotters/latest/plotters/).

### Image and Text

9. image — image-rs. "Encoding and decoding images in Rust." [GitHub](https://github.com/image-rs/image). [Docs](https://docs.rs/image/latest/image/).
10. png — image-rs. "PNG decoding and encoding library in pure Rust." [GitHub](https://github.com/image-rs/image-png). [Performance discussion](https://github.com/image-rs/image-png/discussions/416).
11. "Rust-Based, Memory-Safe PNG Decoders Vastly Outperform C-Based PNG Libraries." [Phoronix](https://www.phoronix.com/news/Rust-PNG-Outperforms-C-PNG).
12. ab_glyph — Alex Sheretic. "Rust API for loading, scaling, positioning and rasterizing OpenType font glyphs." [GitHub](https://github.com/alexheretic/ab-glyph).
13. fontdue — Joe Osborne. "The fastest font renderer in the world, written in pure Rust." [GitHub](https://github.com/mooman219/fontdue).
14. colorous — David Tolnay. "Color schemes for Rust." [Docs](https://docs.rs/colorous).

### Terminal Graphics

15. "Terminal graphics protocol." Kitty documentation. [Kitty graphics protocol](https://sw.kovidgoyal.net/kitty/graphics-protocol/).
16. "Performance." Kitty documentation. [Kitty performance](https://sw.kovidgoyal.net/kitty/performance/).
17. libsixel — Hayaki Saito. "The new standard of SIXEL development." [GitHub](https://saitoha.github.io/libsixel/).
18. at-lib/sixel — "Fast sixel encoder for indexed color bitmap terminal graphics." [GitHub](https://github.com/at-lib/sixel).
19. ratatui-image — "Ratatui widget for rendering image graphics in terminals." [GitHub](https://github.com/ratatui/ratatui-image). [Crates.io](https://crates.io/crates/ratatui-image).
20. "Terminal Graphics Protocols: Kitty, Sixel, iTerm2, and Beyond." [Akmatori Blog](https://akmatori.com/blog/terminal-graphics-protocols).

### Graph Layout

21. layout-rs — Nadav Rotem. "A Rust library and tool that renders Graphviz dot files." [GitHub](https://github.com/nadavrot/layout).
22. petgraph — "Graph data structure library for Rust." [GitHub](https://github.com/petgraph/petgraph). [Docs](https://docs.rs/petgraph/latest/petgraph/).
23. "Regarding Graphviz's orthogonal edge routing." [Graphviz Forum](https://forum.graphviz.org/t/regarding-graphvizs-orthogonal-edge-routing/1889).
24. "Orthogonal Connector Routing." Wybrow, Marriott, Stuckey. [PDF](https://people.eng.unimelb.edu.au/pstuckey/papers/gd09.pdf).

### Performance and SIMD

25. fast_image_resize — Cykooz. "Rust library for fast image resizing with SIMD instructions." [GitHub](https://github.com/Cykooz/fast_image_resize).
26. "Optimizing image processing in Rust with parallelism and Rayon." [Transloadit](https://transloadit.com/devtips/optimizing-image-processing-in-rust-with-parallelism-and-rayon/).
27. "Rust WebGPU Example: Getting Started with GPU Programming in Rust." [Medium](https://medium.com/@aleksej.gudkov/rust-webgpu-example-getting-started-with-gpu-programming-in-rust-fc36dace37d6).
28. "High Performance GPGPU with Rust and wgpu." [Dev.to](https://dev.to/jaysmito101/high-performance-gpgpu-with-rust-and-wgpu-4l9i).

### Linebender Blogs

29. "Linebender in July 2025." [Blog](https://linebender.org/blog/tmil-19/). (Vello CPU benchmarks against Blend2D, tiny-skia, Skia, Cairo)
30. "Linebender in September 2025." [Blog](https://linebender.org/blog/tmil-21/).
31. "Linebender in October 2025." [Blog](https://linebender.org/blog/tmil-22/). (Sparse strips thesis published)
32. "Linebender in December 2025." [Blog](https://linebender.org/blog/tmil-24/).

### Terminal Rendering Techniques

33. "Terminal Pixel Art." [Medium](https://lucamug.medium.com/terminal-pixel-art-ad386d186dad).
34. "(Almost) square pixels in the terminal." [uninformativ.de](https://www.uninformativ.de/blog/postings/2016-12-17/0/POSTING-en.html).
35. "Terminal graphics with Braille characters." [danieledapo](https://danieledapo.github.io/post/terminal-graphics-braille/).
36. "State of Terminal Emulators in 2025." Jeff Quast. [Article](https://www.jeffquast.com/post/state-of-terminal-emulation-2025/).
