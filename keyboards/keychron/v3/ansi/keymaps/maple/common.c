// Common utilities for keymap scripts.

#include QMK_KEYBOARD_H
#include "common.h"
#include <stdlib.h>

uint32_t random_range(uint32_t interval_ms, uint8_t min_pct, uint8_t max_pct) {
    if (min_pct > max_pct) {
        uint8_t t = min_pct;
        min_pct = max_pct;
        max_pct = t;
    }
    if (min_pct > 100) min_pct = 100;
    if (max_pct > 100) max_pct = 100;

    uint8_t range = max_pct - min_pct + 1;
    uint8_t pct   = min_pct + (uint8_t)(rand() % range);

    return (uint32_t)((uint64_t)interval_ms * (pct / 100.0));
}

// Keycode to char dict
const char *keycodes_to_chars[] = {
    [KC_A] = "A",
    [KC_B] = "B",
    [KC_C] = "C",
    [KC_D] = "D",
    [KC_E] = "E",
    [KC_F] = "F",
    [KC_G] = "G",
    [KC_H] = "H",
    [KC_I] = "I",
    [KC_J] = "J",
    [KC_K] = "K",
    [KC_L] = "L",
    [KC_M] = "M",
    [KC_N] = "N",
    [KC_O] = "O",
    [KC_P] = "P",
    [KC_Q] = "Q",
    [KC_R] = "R",
    [KC_S] = "S",
    [KC_T] = "T",
    [KC_U] = "U",
    [KC_V] = "V",
    [KC_W] = "W",
    [KC_X] = "X",
    [KC_Y] = "Y",
    [KC_Z] = "Z",
    [KC_1] = "1",
    [KC_2] = "2",
    [KC_3] = "3",
    [KC_4] = "4",
    [KC_5] = "5",
    [KC_6] = "6",
    [KC_7] = "7",
    [KC_8] = "8",
    [KC_9] = "9",
    [KC_0] = "0",
    [KC_F1] = "F1",
    [KC_F2] = "F2",
    [KC_F3] = "F3",
    [KC_F4] = "F4",
    [KC_F5] = "F5",
    [KC_F6] = "F6",
    [KC_F7] = "F7",
    [KC_F8] = "F8",
    [KC_F9] = "F9",
    [KC_F10] = "F10",
    [KC_F11] = "F11",
    [KC_F12] = "F12",
    [KC_LALT] = "LALT",
    [KC_RALT] = "RALT",
    [KC_LEFT] = "LEFT",
    [KC_RIGHT] = "RIGHT",
    [KC_UP] = "UP",
    [KC_DOWN] = "DOWN",
    [KC_HOME] = "HOME",
    [KC_END] = "END",
    [KC_PAGE_UP] = "PAGE_UP",
    [KC_PAGE_DOWN] = "PAGE_DOWN",
    [KC_INSERT] = "INSERT",
    [KC_DELETE] = "DELETE",
    [KC_ESC] = "ESC",
    [KC_TAB] = "TAB",
};

const char *keycode_to_string(uint16_t keycode) {
    if (keycode > sizeof(keycodes_to_chars) / sizeof(keycodes_to_chars[0])) {
        return "";
    }
    return keycodes_to_chars[keycode];
}
