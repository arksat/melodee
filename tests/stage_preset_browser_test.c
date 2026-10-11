/* SPDX-License-Identifier: GPL-3.0-only */
/* Regression: Stage PRESETS opens the browser without changing the sound. */
#define UI_TEST_NO_MAIN
#include "ui_test.c"

int main(void)
{
    track_t before;
    int failed = 0;

    ui_power_on();
    before = *TSEL;

    turn(EN_PRESET, 1);
    failed |= ui.home || cur_page()->graph != GR_BROWSE || !same_sound(TSEL, &before);

    turn(EN_K2, 1);
    failed |= same_sound(TSEL, &before);

    /* ALGORITHM filters the browser list without switching tracks. */
    before = *TSEL;
    list_set(LM_CAT);
    turn(EN_ALGO, 1);
    failed |= song.sel != 0 || list_mode() != LM_CAT + 1 || !same_sound(TSEL, &before);
    turn(EN_ALGO, -1);
    failed |= song.sel != 0 || list_mode() != LM_CAT;
    list_set(LM_ALL);
    turn(EN_PRESET, 1);
    failed |= same_sound(TSEL, &before);

    press(B_OCTUP);
    failed |= !ui.home;
    turn(EN_ALGO, 1);
    failed |= song.sel != 1; /* Stage retains track selection. */

    printf("stage PRESETS: %s\n", failed ? "FAILED" : "passed");
    return failed;
}
