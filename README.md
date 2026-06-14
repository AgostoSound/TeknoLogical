# TeknoLogical Module Collection

<img width="309" height="124" alt="tecnologo" src="https://github.com/user-attachments/assets/8c090297-3238-41ae-8a1e-a2d54ca1af6a" />
<img width="1766" height="1067" alt="Screenshot 2026-06-14 133955" src="https://github.com/user-attachments/assets/3b7ce652-3ecc-4526-8562-3168eb00a090" />


## Overview

**TeknoLogical** is a focused set of VCV Rack 2 modules shaped for **live minimal techno**. The goal is simple: a compact, coherent instrument that feels immediate and reliable on stage.

Each module is streamlined for **rhythmic and melodic pattern work** - sequencing, tone shaping, and controlled variation - so you can build grooves quickly and stay in the musical flow. The set is curated to work as a whole, enabling complete performances with minimal external dependencies.

> **Heads-up:** TeknoLogical is a **purpose-built instrument**, not a general-purpose toolkit. Some modules use fixed samples or intentionally narrowed controls to maintain a **consistent sonic character** and a fast, performance-first workflow.

This project favors **musical constraint, immediacy, and coherence** - qualities that help keep live techno tight, expressive, and dependable. If that focus resonates with you, welcome aboard.

---

## Included Modules

- [TL-Drum5](docs/TL-Drum5.md) – 5-Voice Polyphonic Drum Machine.
- [TL-Seq4](docs/TL-Seq4.md) – 4-8-16 Dual channel step sequencer.
- [TL-Reseter](docs/TL-Reseter.md) – Dual channel reset expander.
- [TL-Bass](docs/TL-Bass.md) - Trigger-driven bass voice.
- [TL-Mixes](docs/TL-Mixes.md) – 7-channel stereo mixer with cutoff.
- [TL-Odd5](docs/TL-Odd5.md) – Triple odd-step voltage sequencer.
- [TL-Bool](docs/TL-Bool.md) – Dual-input logic gate processor with probability.
- [TL-Pump](docs/TL-Pump.md) – Sidechain-style stereo ducker with selectable pump curves.
- [TL-VuLR](docs/TL-VuLR.md) – Stereo VU Meter.
- [TL-LowF](docs/TL-LowF.md) – Quad-phase control LFO with waves and multipliers.
- [TL-Shape](docs/TL-Shape.md) - ADSR envelope generator with stereo VCA.
- [TL-Deck](docs/TL-Deck.md) - Stereo WAV player with waveform display.
- [TL-Static](docs/TL-Static.md) - Four-color noise source with XY mix and shared low-pass filter.

## Testing Branch Module

This branch also includes [TL-tester](docs/TL-tester.md), a development panel for checking Rack components and custom UI assets. It is intentionally present on `with-tester-module` and is not part of the main release branch.

## Installation

1. Clone this repository into your VCV Rack plugins folder:
   ```bash
   git clone https://github.com/AgostoSound/TeknoLogical.git
   ```

2. Build it using the VCV Rack Plugin Development Toolchain:
   See the official guide: [VCV Rack Plugin Development](https://vcvrack.com/manual/PluginDevelopmentTutorial)
