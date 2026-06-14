# TL-Deck

<img width="203" height="857" alt="Screenshot 2026-05-15 234853" src="https://github.com/user-attachments/assets/b8f7f54f-a1fd-4484-8796-1a8e6265603e" />

TL-Deck is a compact stereo WAV player for VCV Rack 2. It loads a local WAV file, displays a vertical waveform view, and plays the file from the beginning when triggered.

The module is intended for stems, loops, one-shots, texture beds, and live performance material that should be launched reliably from a patch.

---

## Controls

| Control | Description |
| --- | --- |
| **Volume** | Sets playback level from 0 to 100%. |
| **Load** | Opens a file dialog to choose a WAV file. The same action is available from the context menu. |
| **Play** | Starts playback from the beginning. |
| **Stop** | Stops playback and resets the playhead. |
| **Display** | Shows a cached waveform window around the current playback position. |

---

## Inputs

- **Play CV** - Rising edge starts playback from the beginning.
- **Stop CV** - Rising edge stops playback and resets the playhead.
- **Volume CV** - 0..10 V control that scales inside the maximum set by the Volume knob.

---

## Outputs

- **Left Out** - Left audio output, limited to normal Rack audio levels.
- **Right Out** - Right audio output, limited to normal Rack audio levels.

Mono WAV files are copied to both output channels. Stereo WAV files keep their left/right channels.

---

## Behavior

- Playback always starts from the beginning.
- The module stores the selected sample path in the patch.
- If the file still exists when the patch is loaded, TL-Deck reloads it automatically.
- The playback rate follows the WAV file sample rate and the current Rack engine sample rate.
- The output is scaled for nominal +/-5 V Rack audio.

---

## Typical Use

1. Click **Load** and choose a WAV file.
2. Patch **Play CV** from a transport pulse, sequencer, or manual trigger source.
3. Patch **Stop CV** if you need deterministic resets.
4. Use **Volume** or **Volume CV** to place the stem in the mix.
5. Send **Left Out** and **Right Out** to TL-Mixes or another stereo mixer.

---

[Back to module index](../README.md)
