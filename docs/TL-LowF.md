# TL-LowF

<img width="1880" height="697" alt="TL-LowF" src="https://github.com/user-attachments/assets/778ff262-2037-4067-a99c-ea0050b1794c" />

A **4-channel quadrature LFO** for VCV Rack. Each output is phase-shifted **90°** from the previous one, with **per-channel wave selection** and **frequency multiplier** controls. Designed to animate filters, VCAs, panning, and modulation targets with coordinated motion from a single master rate.

---

## Controls

| Control | Description |
| --- | --- |
| **AMP** | Sets the global output amplitude from **0 to 5 V**. At minimum, all outputs stay at **0 V**. |
| **FREQ** | Sets the global base frequency. The range matches the native VCV LFO-style exponential range, from very slow cycles to audio-rate modulation. |
| **MULT 1–4** | Per-channel frequency multiplier. Each channel can run at **x1**, **x2**, or **x3** relative to the base frequency. |
| **WAVE 1–4** | Per-channel waveform selector: **Sine**, **Triangle**, or **Square**. |

---

## Inputs

- **AMP CV** — Global amplitude control input. When patched, it **replaces** the **AMP** knob.
- **FREQ CV** — Global frequency control input. When patched, it **replaces** the **FREQ** knob.

---

## Outputs

- **OUT 1** — LFO output at **0°**
- **OUT 2** — LFO output at **90°**
- **OUT 3** — LFO output at **180°**
- **OUT 4** — LFO output at **270°**

All outputs are **bipolar** and follow the current global amplitude setting, up to **±5 V**.

---

## Indicators

- **LED 1–4** — Per-channel activity indicators. Each LED follows the **absolute output voltage** of its channel:
  - **0 V** → LED off
  - **±5 V** → maximum brightness
  - intermediate voltages → proportional brightness

This means the LEDs reflect the **real output level**, not just the waveform shape. Reducing **AMP** also reduces LED brightness.

---

## Behavior

- The four outputs are permanently spaced by **90°**.
- The **MULT** controls multiply the global base frequency independently for each channel.
- The **WAVE** controls change only the waveform of their own channel.
- If **AMP CV** or **FREQ CV** is connected, the corresponding knob is ignored.
- With amplitude at **0**, all outputs stay silent and all LEDs remain off.

---

## Waveforms

Each channel can be set to one of these three waveforms:

- **Sine** — Smooth cyclic modulation
- **Triangle** — Linear rise/fall motion
- **Square** — Hard switching between positive and negative extremes

---

## Typical Use

1. Set a master rate with **FREQ**.
2. Set the global modulation depth with **AMP**.
3. Choose a different **MULT** and **WAVE** for each channel.
4. Patch the outputs to filters, VCAs, panning inputs, pitch modulation, or effect parameters.
5. Use the built-in **90° phase offsets** to create evolving motion across multiple destinations.

---

## Tips

- Use all four outputs on stereo movement targets for wide, coordinated modulation.
- Set channels to different multipliers for more complex rhythmic interactions while preserving a shared timing base.
- Combine **Sine** and **Triangle** outputs for smooth but non-identical motion.
- Use **Square** on one channel to create stepped modulation against smoother waveforms on the others.

---

[⬅ Back to Module Index](../README.md)
