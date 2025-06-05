# TL-Drum5

![Captura de pantalla 2025-06-05 165659](https://github.com/user-attachments/assets/b5115202-4e80-4b0c-99bd-94dc2f96103e)

A 5-channel drum sample player for VCV Rack. Each channel has its own trigger, envelope, filter, panning, and volume control. Ideal for building drum grooves with tight control and a clean stereo mix.

---

## 🎛️ Controls (per channel)

| Control | Description |
|--------|-------------|
| **Push** | Boosts the signal level 50% |
| **Link** | Routes the output **only** to its dedicated output, bypassing the stereo mix |
| **Pan** | Stereo panning for the channel (-1 = Left, 1 = Right) |
| **Vol** | Output volume |
| **Decay** | Controls how long the sound rings out (negative = shorter, positive = longer) |
| **Filter** | Sweeps between high-pass and low-pass filters for tonal shaping |

---

## 🎚️ Triggers

Each trigger fires the corresponding sample with a custom envelope and processing chain.

---

## 🔈 Outputs

- `OUT` – Each channel has its individual mono output.  
- `STEREO OUT` – Stereo mix output (only includes channels with `Link` OFF)

---


## 🛠️ Notes

- If **Link** is active on a channel, its signal **won’t be mixed into the stereo output**.
- The stereo mix includes panning and volume settings for each unlinked channel.

---

## 🔁 Typical Use

Use TL_Drum5 to create quick and customizable drum lines. Connect sequencer triggers, tweak filters for tonal variety, pan your channels for stereo width, and optionally route some voices out individually for external processing or mixing.

---

[⬅ Back to Module Index](../README.md)
