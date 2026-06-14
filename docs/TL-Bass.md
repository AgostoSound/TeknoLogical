# TL-Bass

<img width="136" height="581" alt="docu_bass" src="https://github.com/user-attachments/assets/5289203c-840d-498c-b15e-f5150527e039" />

TL-Bass is a minimal trigger-driven bass voice for VCV Rack. It provides one-shot triggering, a 1 V/Oct input, two timbre modes, and a bipolar macro filter where 0 is bypass, negative values are low-pass, and positive values are high-pass.

The output is treated as normal Rack audio: nominal +/-5 V after DC blocking and soft limiting.

---

## Controls

| Control | Description |
| --- | --- |
| **TRIG** | Fires a one-shot note. Same behavior as a rising edge at the TRIG input. |
| **FILTER** | Bipolar macro filter. -10..0 is low-pass, 0 is bypass, and 0..+10 is high-pass. FILTER CV sums with the knob. |
| **DECAY** | VCA envelope decay time. DECAY CV sums with the knob and is clamped to the same -10..+10 range. |
| **1 / 2** | Timbre selector. Mode 1 is a round near-sine voice. Mode 2 blends polyBLEP square and saw with sub, drive, and fixed tone shaping. |

---

## Inputs

- **TRIG** - Rising edge trigger input.
- **V/OCT** - 1 V/Oct input clamped to +/-2 octaves around the base note.
- **FILTER CV** - Adds to the FILTER knob.
- **DECAY CV** - Adds to the DECAY knob.

If V/OCT is unplugged after being connected, the module retriggers and returns to its default note.

---

## Output

- **OUT** - Mono audio output, DC-blocked and soft-limited to nominal +/-5 V.

---

## Tuning and Range

- Default unpatched note is around C2, derived from 0 V = 440 Hz with an internal offset.
- V/OCT input is clamped to +/-2 octaves.
- Triggers do not reset oscillator phase, which keeps repeated bass hits slightly more natural.

---

## Typical Use

1. Patch a clock or trigger to **TRIG**, or tap the button to audition.
2. Patch a sequencer to **V/OCT** for bass lines.
3. Set **DECAY** for percussive or longer notes.
4. Choose timbre **1** or **2**.
5. Use **FILTER** to round the sound with low-pass or tighten the low end with high-pass.

---

[Back to module index](../README.md)
