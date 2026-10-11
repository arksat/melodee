# Melodee browser preview

Minimal browser host using Melodee's existing host UI harness and DSP sources.
The panel and AudioWorklet were adapted from hugelton/Felucca (GPL-3.0), itself
based on charlesvestal/fm1-x0x. Original copyright notices are retained.

Run a firmware build once to generate build/gen, then:

```sh
sh web/emu/build.sh
node web/emu/smoke_test.mjs
python3 -m http.server 8000 --directory build/emu
```

Open http://localhost:8000 in a browser and click Switch on. The build uses
local Emscripten or Docker (emscripten/emsdk:4.0.20). Generated output is
build/emu; source lives in web/emu. Serve over localhost or HTTPS for AudioWorklet.

Mouse/touch: drag or wheel a knob; click keys/buttons; Shift-click holds a button.
Keyboard mappings are shown below the panel. Web MIDI input is also supported.

This preview supports the firmware LCD, synthesis, presets, sequencer and UI,
including opening the browser on the first Stage PRESETS turn. User presets and
projects are RAM-only and disappear when reloading. Hardware flash, USB audio,
multicore behavior, exact CPU timing and LED animation are not simulated.
It cannot establish that a hardware build is safe to flash or meets FM-1 deadlines.

To publish, copy build/emu into webapp/try in the GitHub Pages site output.
The build does not push or deploy anything automatically.

The footer's Japanese install button opens the custom firmware installer. This
requires Chrome/Edge, Web MIDI SysEx permission and a USB-connected FM-1. It does
not transfer the preview's RAM-only sounds or patterns. Actual hardware flashing
has not been verified in this environment.

A ready-to-run handoff snapshot lives in `web/player/`; serve it with Python's
HTTP server without building. See `docs/HANDOFF-ja.md` for macOS setup. Rebuilding
updates `build/emu/`, not that snapshot.
