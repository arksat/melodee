/* SPDX-License-Identifier: GPL-3.0-only
 * Minimal browser host for Melodee. Uses the existing host UI harness and real DSP.
 * Panel/AudioWorklet protocol adapted from hugelton/Felucca (Leo Kuroshita).
 * No hardware flash, USB update or multicore emulation. */
#define UI_TEST_NO_MAIN 1
#define MELODEE_USB_AUDIO 0
#include "../../tests/ui_test.c"
#undef __attribute__
#define EXPORT __attribute__((used, visibility("default")))

static float out_l[1024], out_r[1024];
static uint32_t samples, last_frame, draws;
static uint8_t nor_stub[4096];
EXPORT void web_nor_erase(void) { memset(nor_stub, 255, sizeof nor_stub); }
EXPORT uint8_t *web_nor(void) { return nor_stub; }
EXPORT uint32_t web_nor_size(void) { return sizeof nor_stub; }
EXPORT uint32_t web_flash_writes_count(void) { return 0; }
EXPORT void web_boot(void)
{
    uint32_t i;
    ui_power_on();
    for (i = 0; i < UI_NPALETTES; i++) {
        if (str_eq(UI_PALETTES[i].name, "CONTRAST")) {
            settings.palette = (uint8_t)i;
            palette_set(i);
            break;
        }
    }
    song.master_q12 = 2048;
    ui_draw(); draws++;
}
EXPORT void web_master(uint32_t v) { if (v > 1023) v = 1023; song.master_q12 = v * v >> 8; }
EXPORT float *web_out_l(void) { return out_l; }
EXPORT float *web_out_r(void) { return out_r; }
EXPORT void web_render(uint32_t n)
{
    int32_t block[CTL * 2];
    uint32_t pos, k;
    if (n > 1024) n = 1024;
    for (pos = 0; pos < n; pos += CTL) {
        uint32_t count = n - pos < CTL ? n - pos : CTL;
        fm1_ms = (uint32_t)((uint64_t)samples * 1000 / FS);
        host_ticks = fm1_ms * 1000;
        ui_input();
        mix_block(block, count);
        for (k = 0; k < count; k++) {
            out_l[pos+k] = block[k*2] / 65536.0f;
            out_r[pos+k] = block[k*2+1] / 65536.0f;
            scope_buf[scope_w++ & (SCOPE_N-1)] = (int16_t)block[k*2];
        }
        samples += count;
        if (fm1_ms - last_frame >= 16) {
            last_frame = fm1_ms;
            ui_leds(); ui_draw(); draws++;
        }
    }
}
EXPORT void web_buttons(uint32_t labels)
{
    uint32_t m = 0, b;
    for (b = 0; b < NB; b++) if (labels >> b & 1) m |= 1u << panel.btn[b];
    host_pressed |= m & ~fm1_in.buttons;
    fm1_in.buttons = m;
}
EXPORT void web_keys(uint32_t mask)
{
    mask &= (1u << 27)-1;
    host_notes |= mask & ~fm1_in.notes;
    fm1_in.notes = mask;
}
EXPORT void web_enc(uint32_t role, int32_t n)
{ if (role < NE) host_enc[panel.enc[role] % 7] += n * panel.dir[role]; }
EXPORT uint32_t web_midi(uint32_t s, uint32_t a, uint32_t b)
{ return s >= 128 && s < 240 ? midi_enqueue((s >> 4) | (s << 8) | ((a & 127) << 16) | ((b & 127) << 24), 1) : 1; }
EXPORT uint16_t *web_screen(void) { return host_screen; }
EXPORT uint32_t web_screen_draws(void) { return draws; }
EXPORT uint32_t web_now_ms(void) { return fm1_ms; }
EXPORT uint32_t web_playing(void) { return song.playing; }
EXPORT uint32_t web_sample_rate(void) { return FS; }
/* The minimal panel omits LED animation; audio and LCD use the firmware sources. */
EXPORT uint32_t web_lit_buttons(void) { return song.playing ? 1u << 14 : 0; }
EXPORT uint32_t web_lit_keys(void) { return fm1_in.notes; }
EXPORT uint32_t web_dim_buttons(void) { return 0; }
EXPORT uint32_t web_dim_keys(void) { return 0; }
EXPORT uint32_t web_dim_level(void) { return 0; }
EXPORT uint32_t web_breath_buttons(void) { return 0; }
EXPORT uint32_t web_breath_keys(void) { return 0; }
EXPORT uint32_t web_mid_keys(void) { return 0; }
EXPORT uint32_t web_anim_levels(void) { return 0; }
EXPORT uint32_t web_test_home(void) { return ui.home; }
EXPORT uint32_t web_test_browser(void) { return !ui.home && cur_page()->graph == GR_BROWSE; }
EXPORT uint32_t web_test_preset(void) { return trk[song.sel].preset; }
EXPORT uint32_t web_test_list(void) { return list_mode(); }
EXPORT uint32_t web_test_track(void) { return song.sel; }
