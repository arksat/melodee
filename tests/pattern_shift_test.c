/* SPDX-License-Identifier: GPL-3.0-only */
#define UI_TEST_NO_MAIN
#include "ui_test.c"
#include <assert.h>
int main(void)
{
    ui_power_on();
    ui.home = 0; ui.page = (uint8_t)page_titled("PATTERN"); page_entered();
    fm1_in.buttons |= 1u << panel.btn[B_HOME];
    host_pressed |= 1u << panel.btn[B_HOME];
    for (uint32_t i = 0; i < 60; i++) frame();
    assert(!ui.menu); /* HOME can be held before turning. */
    turn(EN_K1, 1); assert(TSEL->p[P_SLEN] == 32);
    turn(EN_K1, 1); assert(TSEL->p[P_SLEN] == 48);
    turn(EN_K1, 1); assert(TSEL->p[P_SLEN] == 64);
    turn(EN_K1, 1); assert(TSEL->p[P_SLEN] == 64);
    turn(EN_K1, -1); assert(TSEL->p[P_SLEN] == 48);
    TSEL->p[P_SLEN] = 20;
    turn(EN_K1, -1); assert(TSEL->p[P_SLEN] == 16);
    turn(EN_K1, -1); assert(TSEL->p[P_SLEN] == 16);
    TSEL->p[P_SLEN] = 20;
    turn(EN_K1, 1); assert(TSEL->p[P_SLEN] == 32);
    fm1_in.buttons &= ~(1u << panel.btn[B_HOME]); frame();
    assert(!ui.home && !ui.menu);
    fm1_ms += 1000; host_ticks += 1000000;
    turn(EN_K1, 1); assert(TSEL->p[P_SLEN] == 33);
    puts("pattern HOME + K1: passed");
    return 0;
}
