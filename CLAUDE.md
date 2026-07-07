# Tight-WFS — Claude Code notes

Dense-array Wave Field Synthesis processor. A sibling of WFS-DIY
(`d:/dev/WFS_DIY_v1`) and XOA (`d:/dev/XOA/XOA`), consuming the same shared
engine — **spatcore** — as a git submodule, **CMake-native** (no Projucer;
spatcore's CMakeLists names Tight-WFS as exactly this kind of consumer).

**Key relationship:** Tight-WFS is *still WFS*. Unlike XOA (which needed brand
new Ambisonics/spherical-harmonic DSP), Tight-WFS **reuses the existing
`spatcore/wfs/` delay-and-sum renderer as-is** — its differentiator is dense
arrays (small driver spacing → hundreds of outputs), which stresses the
channel-count bounds spatcore's boundary docs already flagged for runtime
parameterization. Its app layer will borrow WFS-specific pieces from **WFS-DIY**
(calculation engine, array geometry, WFS parameter schema) — the pieces XOA
dropped.

## Build

```
cmake -S . -B build
cmake --build build --config Release     # or Debug
```

Binary: `build/TightWFS_artefacts/<config>/Tight-WFS.exe`. After a fresh clone
run `./tools/setup.sh` (initialises submodules).

## Repo shape

- `Source/` — the Tight-WFS application (JUCE gui app). App layer only.
- `spatcore/` — **submodule**, pinned to **v0.1.1**. The CMake wiring comes
  from spatcore's `cmake/SpatcoreConsumer.cmake` helper (see CMakeLists.txt).
  Dependency direction is strictly app → spatcore; never modify spatcore from
  here — changes go to the spatcore repo and arrive via a pin bump.
- `ThirdParty/JUCE` — submodule (JUCE 8, same pin as WFS-DIY / XOA).
- `ThirdParty/hidapi` — submodule (headers for spatcore-controllers; static
  lib linked into the app via hidapi's own CMake).
- `ThirdParty/juce_simpleweb`, `ThirdParty/roli_blocks_basics` — vendored JUCE
  modules, required by spatcore-control / -controllers.

## Conventions

- C++17; compile flags from `spatcore_apply_compile_flags()` (pins the
  optimization/FP flags spatcore's bit-exactness gates were baselined with).
- License GPLv3. Third-party inventory in `THIRD_PARTY_NOTICES.md`.
- This repo shares its scaffold 1:1 with XOA (same submodules, CMake wiring,
  vendored modules). If the scaffold changes in one, mirror it in the other
  (and in WFS-DIY where applicable) — there is no shared template repo yet.

## Where things are decided

- Roadmap and architecture decisions: `Documentation/TIGHT-WFS-PLAN.md`.
- The renderer/engine seams (algorithm method contract, `RtSnapshot`,
  raw-pointer matrix hand-off): spatcore docs (`spatcore/docs/*.md`).
- Reference app for the WFS driving functions, array geometry and parameter
  schema to port: the WFS-DIY checkout at `d:/dev/WFS_DIY_v1`.
