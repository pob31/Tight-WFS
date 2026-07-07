# Tight-WFS

A dense-array Wave Field Synthesis processor built with the JUCE framework —
a sibling of [WFS-DIY](https://github.com/pob31/WFS-DIY), sharing the same
real-time core.

## Project overview

Tight-WFS renders Wave Field Synthesis on **tight** loudspeaker arrays: small
inter-driver spacing raises the spatial-aliasing frequency for sharper,
more stable virtual sources, at the cost of driving arrays with hundreds of
output channels.

It uses the same delay-and-sum renderer as WFS-DIY — provided by
[spatcore](https://github.com/pob31/spatcore), the shared spatial-audio core —
consumed here as a git submodule and built natively with CMake
(`spatcore-audio`, `spatcore-control`, `spatcore-controllers`). Where WFS-DIY
is Projucer-built and compiles the spatcore sources directly, Tight-WFS is the
CMake-native consumer spatcore was extracted to serve.

> **Status: bootstrap.** The repository currently builds a minimal JUCE
> application shell linked against all three spatcore libraries. The
> dense-array driving-function layer, parameter schema, control surface and
> GUI are being developed on top of it — see
> [Documentation/TIGHT-WFS-PLAN.md](Documentation/TIGHT-WFS-PLAN.md).

## Building

### Prerequisites

**Windows**
1. [Git for Windows](https://git-scm.com/download/win)
2. [Visual Studio 2026 Community](https://visualstudio.microsoft.com/) with the
   **"Desktop development with C++"** workload (includes CMake).

**macOS**
1. [Xcode](https://apps.apple.com/app/xcode/id497799835) (includes git)
2. [CMake](https://cmake.org/download/) ≥ 3.22 (`brew install cmake`)

**Linux (Debian / Ubuntu)**
```bash
sudo apt install build-essential cmake pkg-config libasound2-dev libfreetype6-dev \
    libfontconfig1-dev libgl1-mesa-dev libcurl4-openssl-dev libgtk-3-dev \
    libwebkit2gtk-4.1-dev libudev-dev
```

### Clone and bootstrap

```bash
git clone --recurse-submodules https://github.com/pob31/Tight-WFS.git
cd Tight-WFS
./tools/setup.sh   # idempotent; fixes a clone made without --recurse-submodules
```

Submodules: `ThirdParty/JUCE` (JUCE 8), `ThirdParty/hidapi` (HID controller
support) and `spatcore` (the shared engine, pinned to a released tag).
`juce_simpleweb` and `roli_blocks_basics` are vendored under `ThirdParty/`.

### Configure and build

```bash
cmake -S . -B build
cmake --build build --config Release
```

On Windows this generates a Visual Studio solution in `build/` (open
`build/TightWFS.sln`). On macOS add `-G Xcode` for an Xcode project. The binary
lands under `build/TightWFS_artefacts/<config>/`.

## License

This project is licensed under the GNU General Public License v3.0 (GPL-3.0).

Copyright (c) 2026 Pierre-Olivier Boulant

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.

Third-party components are listed in
[THIRD_PARTY_NOTICES.md](THIRD_PARTY_NOTICES.md).
