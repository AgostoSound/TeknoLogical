# TL-Shape

<img width="176" height="762" alt="TL-Shape panel" src="../res/TL_Shape.svg" />

A **compact ADSR envelope generator with a built-in stereo VCA** for VCV Rack 2.  
TL-Shape creates a 0-10 V envelope from a trigger or gate, then provides a simple stereo audio shaping stage for controlling level, accents, and gated movement.

The module is designed as a fast performance utility: one side generates the envelope, the other side applies voltage-controlled gain to stereo or mono audio.

---
---

## Controls

| Control | Description |
| --- | --- |
| **ATTACK** | Sets the envelope attack time. Range is exponential, from very fast transients to long fades. |
| **DECAY** | Sets the time it takes to fall from the peak level to the sustain level. |
| **SUSTAIN** | Sets the held envelope level while the gate remains active. |
| **RELEASE** | Sets the time it takes to return to 0 V after the gate is released. |
| **TRIG (button with LED)** | Manual gate/trigger. Starts the ADSR envelope and keeps it active while held. |
| **VOL** | Sets the final VCA gain from **0-100%**. Default = **100%**. |

---

## Inputs

- **TRIG** - Gate/trigger input. A rising edge starts the envelope; holding the gate keeps the envelope in the sustain stage.
- **CV** - VCA control input. Expected range is **0-10 V**. When patched, this CV controls the VCA gain before the **VOL** control.
- **L / R** - Stereo audio inputs.

If only one audio input is connected, TL-Shape copies it to both outputs, making it usable as a mono-to-stereo VCA.

---

## Outputs

- **CV** - Envelope output, scaled from **0 to 10 V**.
- **L / R** - Stereo audio outputs after VCA gain.

---

## Envelope Behavior

The envelope follows a standard ADSR cycle:

1. **Attack** rises from 0 to full level.
2. **Decay** falls from full level to the selected sustain level.
3. **Sustain** holds while the trigger button or input gate remains active.
4. **Release** falls back to 0 V when the gate is released.

Time controls use an exponential response from approximately **1 ms to 10 s**, giving fine control over short percussive shapes while still allowing slow fades.

---

## VCA Behavior

- With **CV** unpatched, the VCA stays open and is controlled only by **VOL**.
- With **CV** patched, incoming CV is scaled as **0 V = closed** and **10 V = fully open**.
- **VOL** always acts as a final level trim after the CV stage.
- Audio is passed as stereo when both **L** and **R** are connected.
- A single connected audio input is copied to both outputs.

---

## Indicators

- **TRIG LED** follows the current manual or input gate state.
- The paired side LEDs show envelope/VCA activity in seven stepped levels.
- When **CV** is patched, the LEDs follow the VCA CV level.
- When **CV** is unpatched, the LEDs follow the internal ADSR envelope level.

---

## Typical Use

1. Patch a trigger or gate into **TRIG**, or use the front-panel **TRIG** button.
2. Take **CV OUT** to control another module, or patch audio through **L/R** for built-in VCA shaping.
3. Set **ATTACK**, **DECAY**, **SUSTAIN**, and **RELEASE** for the desired contour.
4. Use **VOL** as the final audio level control.
5. Patch an external envelope, sequencer, or modulation source into **CV** when you want the VCA stage to follow a signal other than the internal ADSR.

---

## Tips

- Use short **ATTACK** and **RELEASE** settings for tight rhythmic gates.
- Raise **SUSTAIN** for held notes or drones, and lower it for plucked shapes.
- Patch **CV OUT** into the module's own **CV** input when you want the internal envelope to drive the built-in VCA directly.
- Use the mono-to-stereo normalization to quickly place a mono synth or drum voice into a stereo chain.

---

[<- Back to Module Index](../README.md)
