# TL-Bass

<img width="318" height="937" alt="Captura de pantalla 2025-09-17 215833" src="https://github.com/user-attachments/assets/2a1dc064-011c-424b-a049-c16185f33c59" />

A **minimal / psy techno bass voice** for VCV Rack. Single-hit (one-shot) **triggered bass** with **1 V/Oct** pitch, a **dual timbre** selector (1/2), and a **bipolar filter** where **0 = bypass**, **negative = low-pass**, **positive = high-pass**.

---

## 🎛️ Controls

| Control | Description |
|---|---|
| **Trigger button (w/ LED)** | Fires a one-shot note. Same behavior as a rising edge at **TRIG**. |
| **Filter** | Bipolar filter macro (**−10…0 = LP**, **0…+10 = HP**, **0 = bypass**). CV adds 1:1. |
| **Decay** | One-shot decay time for the VCA envelope (≈ **30 ms → 2 s**). CV adds 1:1. |
| **1 / 2** | **Timbre selector**: <br>**1 – DeepSaw:** polyBLEP saw + subtle sub-sine, moderate drive. <br>**2 – AcidSquare:** polyBLEP square with a tiny pitch transient and stronger drive. |

---

## 🔌 Inputs

- **TRIG** – Rising edge starts the note and **restarts phase** + envelope (consistent attacks).
- **V/OCT** – 1 V/Oct pitch, **clamped to ±2 octaves** around the base note.  
  If the cable is **not connected** (or gets unplugged), the module **re-triggers** and plays the **default note**.
  - Default pitch with no cable: **C2 ≈ 65.4 Hz** (internally derived from a 0 V = 440 Hz reference with an offset).
- **FILTER (CV)** – Adds to the **Filter** knob in the same **−10…+10** range (negative adds LP, positive adds HP).
- **DECAY (CV)** – Adds to the **Decay** knob.

---

## 🔈 Output

- **OUT** – Mono output **±5 V** (10 Vpp), DC‑blocked, with a gentle pre‑filter saturation for character.

---

## 💡 Indicators

- **Trigger LED** – Flashes on each hit.

---

## 🎚️ Pitch & Range

- **Default note** (with V/OCT unpatched): **C2 ≈ 65.4 Hz** (techno-friendly low range).  
- **V/Oct range**: input is clamped to **±2 octaves** (total **4 octaves** span). Any incoming voltage beyond this is **hard-limited**.
- Auto **re-trigger** when **unplugging** V/Oct to return to the default note.

---

## 🧪 Bipolar Filter

The **Filter** knob and its CV share the same contract:

- **0** → **Bypass** (no filtering).  
- **−10…0** → **Low‑Pass** (cutoff decreases toward the negative end).  
- **0…+10** → **High‑Pass** (cutoff increases toward the positive end).

> Implementation mirrors other modules in the series: combined **LowPass** + **HighPass** cached filters, controlled by the sign of the macro value. At exactly **0**, both are bypassed.

---

## 🛠️ Notes

- **Envelope**: one‑shot **decay** driving the VCA; an internal env‑to‑filter transient adds punch for percussive basslines.
- **Retrigger behavior**: a new **TRIG** restarts **phase** and **envelope** for tight, consistent attacks.
- **Timbres**:  
  - **1 – DeepSaw**: band‑limited saw plus a **sub‑sine (−1 octave)** blend; solid and deep.  
  - **2 – AcidSquare**: square with a **micro pitch transient** at onset and a hotter drive; edgy/acid/psy.

---

## 🔁 Typical Use

1. Patch a **clock → TRIG** (or tap the button to audition).  
2. For melodic lines, patch **V/OCT** (input is clamped to ±2 oct).  
3. Set **Decay** to taste (short for tight minimal; longer for drones/bombs).  
4. Choose **1/2** then sculpt with **Filter** (negative = round LP; positive = snappy HP).  
5. Send **OUT** to your mixer/FX; external compression/saturation pairs well.

---

## 📌 Tips

- For **rolling psy** patterns, feed **TRIG** with 1/8 or 1/16 notes and modulate **Filter** subtly with a slow LFO or an accent envelope.  
- In **mode 1**, keep **Filter** near **0 (bypass)** to maximize sub weight; or nudge into negative for darker tones.  
- In **mode 2**, a mild **HP** (+1…+3) tightens the low end and leaves space for the kick.

---

[⬅ Back to modules index](../README.md)

