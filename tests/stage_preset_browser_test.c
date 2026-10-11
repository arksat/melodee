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

    press(B_OCTUP);
    failed |= !ui.home;

    printf("stage PRESETS: %s\n", failed ? "FAILED" : "passed");
    return failed;
}
