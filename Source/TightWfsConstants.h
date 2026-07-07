#pragma once

//==============================================================================
// Tight-WFS — project-wide constants.
//
// Tight-WFS is dense-array Wave Field Synthesis: the same delay-and-sum
// renderer as WFS-DIY (spatcore/wfs), but driving *tight* loudspeaker arrays
// (small inter-driver spacing → high spatial-aliasing frequency), which pushes
// the output channel count into the hundreds.
//
// spatcore's boundary docs (core-boundary-proposal-audio.md §4) call out that
// the max-inputs / max-outputs bounds must be runtime prepare()/ctor args
// precisely so dense-array consumers like this one can exceed WFS-DIY's
// defaults. So channel bounds live in the parameter store as runtime values,
// not as compile-time constants here — this header holds only fixed identity.
//==============================================================================

namespace tightwfs
{

/** Provisional upper bound for the output-channel UI/allocation hint.
    NOT a hard DSP limit — the renderer takes the live count at prepare().
    Tune once the array-geometry schema lands (roadmap P1/P2). */
constexpr int kOutputChannelSoftCap = 512;

} // namespace tightwfs
