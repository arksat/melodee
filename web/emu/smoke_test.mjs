// SPDX-License-Identifier: GPL-3.0-only
import fs from 'node:fs';
import assert from 'node:assert/strict';
const {instance} = await WebAssembly.instantiate(fs.readFileSync('build/emu/melodee.wasm'), {});
const e = instance.exports;
e._initialize(); e.web_boot();
let peak = 0;
function render(ms) {
  for (let n = 0; n < ms * 44.1; n += 128) {
    e.web_render(128);
    for (const v of new Float32Array(e.memory.buffer, e.web_out_l(), 128)) {
      assert(Number.isFinite(v)); peak = Math.max(peak, Math.abs(v));
    }
  }
}
function tap(b) { e.web_buttons(1 << b); render(60); e.web_buttons(0); render(60); }
render(100);
assert(new Set(new Uint16Array(e.memory.buffer, e.web_screen(), 240*240)).size > 2);
e.web_keys(1 << 7); render(500); e.web_keys(0); render(200);
assert(peak > 0.001, 'keyboard produces sound');
const preset = e.web_test_preset();
e.web_enc(2, 1); render(60);
assert(e.web_test_browser(), 'PRESETS opens browser');
assert.equal(e.web_test_preset(), preset, 'first detent keeps the sound');
const track = e.web_test_track();
const list = e.web_test_list();
e.web_enc(1, 1); render(60);
assert.equal(e.web_test_track(), track, 'browser ALGORITHM keeps the track');
assert.equal(e.web_test_list(), list + 1, 'browser ALGORITHM selects next list/category');
tap(10); assert(e.web_playing()); tap(10); assert(!e.web_playing());
console.log(`Web smoke passed: LCD, keyboard audio (peak ${peak.toFixed(3)}), Stage PRESETS, transport`);
