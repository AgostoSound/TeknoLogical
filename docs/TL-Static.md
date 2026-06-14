# TL-Static

<img width="215" height="905" alt="Screenshot 2026-06-14 121943" src="https://github.com/user-attachments/assets/567ab78f-3dab-4653-b242-f71536208b03" />

**Four-color noise source for VCV Rack 2.** TL-Static generates **White**, **Pink**, **Brown**, and **Blue** noise simultaneously, each available on its own output, plus an **XY Mix** output driven by the custom selector in the center of the panel. A shared **Volume** control and a shared **low-pass filter** shape all four colors together.

---

## Controls

| Control | Description |
| --- | --- |
| **XY selector** | 2D mix control for the **MIX** output. Each corner represents one noise color: **top-left = White**, **top-right = Pink**, **bottom-left = Brown**, **bottom-right = Blue**. Intermediate positions crossfade bilinearly between the four. Center position gives an even mix of all colors. |
| **VOLUME** | Global output level for all noise generators. The knob sets the maximum level. **VOL** CV applies an absolute 0..1 control inside that maximum, expected range **0..10 V**. |
| **FILTER** | Global low-pass filter cutoff for all noise generators. With no CV patched, the knob controls the cutoff. If **FILTER** CV is connected, it replaces the knob as the cutoff source. Cutoff is mapped roughly **20 Hz .. 20 kHz** on a logarithmic curve. |

---

## Inputs

- **VOL** - Global volume CV, **0..10 V**. Scales inside the maximum set by the **VOLUME** knob.
- **FILTER** - Global cutoff CV, **0..10 V**. When connected, it replaces the **FILTER** knob.

---

## Outputs

- **WHITE** - Independent white noise output, around **+/-5 V** nominal.
- **PINK** - Independent pink noise output, around **+/-5 V** nominal.
- **BROWN** - Independent brown noise output, around **+/-5 V** nominal.
- **BLUE** - Independent blue noise output, around **+/-5 V** nominal.
- **MIX** - XY-controlled blend of the four colors after the same global volume and low-pass processing.

---

## Noise Mapping

- **Top-left** -> White
- **Top-right** -> Pink
- **Bottom-left** -> Brown
- **Bottom-right** -> Blue

The selector behaves as a continuous 2D crossfader. Moving horizontally morphs between left and right colors; moving vertically morphs between upper and lower colors.

---

## Notes

- All four colors are generated with separate internal state so they do not collapse into the same signal with different filtering.
- Each output has its own low-pass filter state, even though cutoff is shared globally.
- The **MIX** output is built from the already-generated, already-filtered individual colors.
- Brown noise includes a leaky/integrated stage plus DC control to keep it stable.
- Blue noise is derived from its own brightened noise path and remains bounded at audio-rate output levels.

---

## Quick start

1. Patch any of the four direct outputs if you want a fixed noise color.
2. Patch **MIX** if you want to morph between colors from the XY selector.
3. Set **VOLUME** first, then use **VOL** CV if you want dynamic control.
4. Sweep **FILTER** to tame the highs or patch **FILTER** CV for animated cutoff movement.
5. Move the XY handle toward a corner for a pure color, or keep it near the center for blended textures.

---

[Back to module index](../README.md)
