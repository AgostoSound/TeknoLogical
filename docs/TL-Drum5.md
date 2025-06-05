# TL_Drum5 🥁

A 5-channel drum sample player for VCV Rack. Each channel has its own trigger, envelope, filter, panning, and volume control. Ideal for building drum grooves with tight control and a clean stereo mix.

---

## 🧠 Features

- 5 sample channels: **Kick, Snare, Clap, Closed Hat, Open Hat**
- Individual **trigger inputs**, **volume**, **pan**, **decay**, and **filter** controls per channel
- Optional **stereo mix output** or **individual outputs** per voice
- Built-in **decay envelope** per channel
- Filter control for shaping each voice
- **LED activity indicators** per channel

---

## 🎛️ Controls (per channel)

| Control | Description |
|--------|-------------|
| **Push** | Boosts the output level (adds saturation or presence) |
| **Link** | Routes the output **only** to its dedicated output, bypassing the stereo mix |
| **Pan** | Stereo panning for the channel (-1 = Left, 1 = Right) |
| **Vol** | Output volume |
| **Decay** | Controls how long the sound rings out (negative = shorter, positive = longer) |
| **Filter** | Sweeps between high-pass and low-pass filters for tonal shaping |

---

## 🎚️ Inputs

- `IN_KK` – Trigger for Kick  
- `IN_SN` – Trigger for Snare  
- `IN_CL` – Trigger for Clap  
- `IN_CH` – Trigger for Closed Hat  
- `IN_OH` – Trigger for Open Hat  

Each trigger fires the corresponding sample with a custom envelope and processing chain.

---

## 🔈 Outputs

- `OUT_KK` – Kick individual output  
- `OUT_SN` – Snare individual output  
- `OUT_CL` – Clap individual output  
- `OUT_CH` – Closed Hat individual output  
- `OUT_OH` – Open Hat individual output  
- `OUT_L / OUT_R` – Stereo mix output (only includes channels with `Link` OFF)

---

## 💡 LED Indicators

Each channel includes a small LED that lights up according to the output level, giving visual feedback when a sound is triggered.

---

## 🛠️ Notes

- If **Link** is active on a channel, its signal **won’t be mixed into the stereo output**.
- The stereo mix includes panning and volume settings for each unlinked channel.
- Filters allow creative shaping of each drum voice directly from the module.

---

## 🔁 Typical Use

Use TL_Drum5 to create quick and customizable drum lines. Connect sequencer triggers, tweak filters for tonal variety, pan your channels for stereo width, and optionally route some voices out individually for external processing or mixing.

---

