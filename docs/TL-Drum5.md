# TL-Drum5

<img width="552" height="582" alt="docu_drum5" src="https://github.com/user-attachments/assets/41dd1fff-a9fb-4948-80be-6d3f84c7f06b" />

TL-Drum5 is a compact 5-voice drum sample player for VCV Rack. Each voice has its own trigger input, decay envelope, filter, pan, volume, push, link routing, individual output, and contribution to the stereo mix.

The stereo mix is soft-limited to nominal +/-5 V so stacked drum hits stay in normal Rack audio range.

---

## Controls Per Voice

| Control | Description |
| --- | --- |
| **Push** | Boosts the voice before filtering and volume. |
| **Link** | Removes the voice from the stereo mix and routes it only to its individual output. |
| **Pan** | Places the voice in the stereo mix when Link is off. |
| **Vol** | Voice level. |
| **Decay** | Sets the one-shot decay time. Negative values are shorter, positive values are longer. |
| **Filter** | Bipolar macro filter. Negative values use low-pass, positive values use high-pass, and 0 is bypass. |

---

## Inputs

- **Kick, Snare, Clap, Closed Hat, Open Hat trigger inputs** - Each rising edge restarts the corresponding sample and decay envelope.

---

## Outputs

- **Individual outputs** - One mono output per voice.
- **Stereo L/R outputs** - Panned stereo sum of all voices whose Link control is off.

---

## Notes

- Individual outputs are taken after each voice volume/filter chain.
- Linked voices stay available on their individual outputs but do not enter the stereo bus.
- The stereo bus uses a soft limiter at the final output stage to avoid hard clipping when multiple voices hit together.

---

## Typical Use

1. Patch sequencer triggers into the five trigger inputs.
2. Use **Decay** and **Filter** to shape each drum voice.
3. Use **Pan** and **Vol** to build a quick stereo drum mix.
4. Enable **Link** on voices you want to process externally.
5. Send the stereo output to TL-Mixes or your main Rack mixer.

---

[Back to module index](../README.md)
