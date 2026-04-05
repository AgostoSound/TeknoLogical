# TL-VuLR

<img width="110" height="905" alt="Screenshot 2026-04-05 181655" src="https://github.com/user-attachments/assets/e851bcbc-5cb3-4459-887e-05438bf40565" />


A **compact stereo VU meter** for VCV Rack 2. TL-VuLR monitors the signal level of independent **Left** and **Right** audio channels with two vertical **14-segment LED meters**. It is designed as a simple visual level reference for checking stereo balance and peak activity in a patch.

---

## Controls

TL-VuLR has **no knobs or switches**.  
It is a dedicated display module: patch audio in, and read the level from the LEDs.

---

## Inputs

- **L** – Left channel audio input.
- **R** – Right channel audio input.

Each input drives its own meter independently.

---

## Notes

- TL-VuLR reads the **absolute signal level**, so both positive and negative waveform peaks contribute to the display.
- The two channels are fully independent, which makes the module useful for checking **stereo balance** as well as overall signal presence.
- The top segment is intended to represent a signal close to typical VCV Rack / Eurorack full audio level.

---

## Typical Use

1. Patch your stereo signal into **L** and **R**.
2. Watch both LED columns while the patch is running.
3. Compare the two sides to check stereo balance.
4. Use the top LEDs as a quick warning that the signal is reaching a very strong level.

---

## Quick Start

- Patch any audio source into **L** and/or **R**.
- Send a louder signal to light more LEDs.
- Use it anywhere in the rack where you want a compact visual monitor.

---

[Back to module index](../README.md)
