# Tight-WFS — Architecture & Roadmap

> Tight-WFS = dense-array Wave Field Synthesis. A sibling of
> [WFS-DIY](https://github.com/pob31/WFS-DIY) built on the shared
> [spatcore](https://github.com/pob31/spatcore) engine. spatcore's boundary
> docs name Tight-WFS as a consumer: "Tight-WFS (dense arrays → hundreds of
> outputs)".

## 1. What Tight-WFS is (and how it differs from XOA)

Tight-WFS renders the **same** Wave Field Synthesis as WFS-DIY — delay-and-sum
driving of a loudspeaker array so the superposed wavefronts reconstruct a
target sound field. The difference from WFS-DIY is array **density**: tight
inter-driver spacing raises the spatial-aliasing frequency \(f_{alias} \approx
c / (2\,\Delta x)\), giving sharper, more stable virtual sources — at the cost
of arrays with **hundreds of output channels**.

Contrast with its Ambisonics sibling XOA:

| | XOA | Tight-WFS |
|---|---|---|
| Method | Ambisonics (spherical harmonics) | Wave Field Synthesis |
| DSP vs spatcore | **new** (SH encode/rotate/decode — none in spatcore) | **reuses** `spatcore/wfs/` delay-and-sum as-is |
| App layer borrows from | mostly generic shell | **WFS-DIY** (calc engine, geometry, WFS schema) |
| Stresses | 121-channel SH bus | `maxOutputs` → hundreds |

So Tight-WFS is the *easier* of the two new consumers: it is "WFS-DIY on the
clean CMake base, with the sparse-array assumptions removed and the output
bound raised." The scaffold is shared 1:1 with XOA; the app content leans on
WFS-DIY.

## 2. The spatcore seams Tight-WFS plugs into

Same contract WFS-DIY already uses (reference checkout `d:/dev/WFS_DIY_v1`):

1. **Renderer "algorithm" (reused directly)** — `spatcore/wfs/`
   `InputBufferAlgorithm` / `OutputBufferAlgorithm` (CPU gather/scatter) and
   `NativeGpuWfsAlgorithm` / `NativeGpuOutputBufferAlgorithm` (GPU). Duck-typed
   contract: `prepare(numIn, numOut, sr, block, const float* delay/level/HF
   matrices..., enabled)` · `processBlock(AudioSourceChannelInfo&, const
   AudioBuffer<float>& in, numIn, numOut)`. **No new renderer needed** — the
   scatter (`OutputBufferAlgorithm`, one worker per output) path is the natural
   fit for high output counts.
2. **Parameter delivery**: app-owned flat `[in × out]` delay/level/HF matrices,
   raw `const float*` handed to the renderer; message-thread timer rewrites in
   place. For dense arrays these matrices get large (in × hundreds) — memory
   and the 50 Hz recompute cost are the things to watch (see §5).
3. **Driving-function brain**: WFS-DIY's `Source/DSP/WFSCalculationEngine`
   turns source + speaker geometry into those matrices. Port it, then
   generalize for dense linear/planar arrays and add the WFS **prefilter**
   (the \( \sqrt{jk} \) / 3 dB-per-octave driving-function filter and spatial
   anti-aliasing) that spatcore's open-questions doc flags as wanted by a
   rigorous WFS renderer.
4. **Parameter store / control / MCP / GPU**: identical seams to XOA/WFS-DIY —
   `TreeParameterStore`, `spatcore/control/{osc,mcp}`, the existing WFS GPU
   family. Reuse wholesale.

## 3. Where the dense-array DSP lives

Prototype app-local (`Source/DSP/`), extract shared bits to spatcore only once
stable — same discipline that produced spatcore. The **WFS prefilter** and any
**field-correction** stage are explicitly called out in
`spatcore/docs/open-questions-audio.md` as candidates that belong in spatcore
(both a rigorous Tight-WFS and XOA want them), so design them with that
extraction in mind.

## 4. Roadmap

- **P0 — Bootstrap (this repo state).** Submodules (JUCE 8.0.14, spatcore
  v0.1.1, hidapi 0.15.0), vendored juce_simpleweb + roli_blocks_basics, CMake
  build (via spatcore's SpatcoreConsumer.cmake helper) of a minimal JUCE app
  linking the three spatcore libs, GPLv3, 3-OS CI. Scaffold shared 1:1 with XOA.
- **P1 — Parameter store & schema.** Port WFS-DIY's `WFSValueTreeState`
  pattern; keep the WFS parameter surface (source position, per-output
  delay/level/HF, master, network) but make **max-outputs a runtime bound**
  (spatcore boundary-doc §4) and add **array-geometry** definitions for dense
  linear / arc / planar arrays (driver pitch, count, curvature).
- **P2 — Wire the WFS renderer at scale.** Port the calc-engine → matrix →
  `spatcore/wfs` audio-callback path from WFS-DIY; validate with hundreds of
  outputs; confirm the scatter path and metering hold up. Offline render
  baselines from day one.
- **P3 — Dense-array driving functions & prefilter.** Proper WFS
  driving-function prefilter + spatial anti-aliasing for tight arrays; array
  auto-layout wizard for common dense geometries.
- **P4 — Control plane.** OSC manager + address map (reuse WFS-DIY's), tracking
  receivers, OSCQuery.
- **P5 — MCP server.** Port `MCPServer` + author the Tight-WFS parameter CSV;
  reuse the WFS-DIY `tools/generate_mcp_tools.py` generator.
- **P6 — GUI.** Port the WFS-DIY GUI framework and WFS-domain tabs
  (Inputs/Outputs/Map/System Config) with dense-array-aware editors; the WFS
  tabs XOA rewrote are reused here largely intact.
- **P7 — GPU.** Reuse the existing WFS GPU family (`NativeGpuWfsAlgorithm` +
  CUDA/HIP/Metal backends) — no new kernel family needed; validate at high
  channel counts.
- **P8 — Ship.** Installers, release CI, docs.

## 5. Open questions (decide when reached)

- Matrix memory / recompute at hundreds of outputs: `[in × out]` delay/level/HF
  matrices scale with output count; is the 50 Hz full recompute still cheap, or
  is dirty-region / incremental update needed?
- WFS prefilter realization and whether it lands in `spatcore` (shared with
  XOA) or stays app-side initially.
- Array-geometry schema: how much of WFS-DIY's `ArrayGeometryCalculator` and
  array-wizard preset catalog transfers to dense arrays vs needs new presets.
- Reverb: reuse spatcore FDN/IR; the SDN 32-node cap is irrelevant to the dense
  *output* array but relevant if room modeling scales with nodes.
- GPU latency/throughput at hundreds of outputs — validate the async pipeline
  depth and per-block launch cost hold.
