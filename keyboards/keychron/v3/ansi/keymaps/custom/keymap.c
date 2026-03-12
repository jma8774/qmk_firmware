/* Copyright 2021 @ Keychron (https://www.keychron.com)
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include QMK_KEYBOARD_H
#include "print.h"

#include "common.h"
#include "engine/script_engine.h"
#include "engine/timing.h"
#include "scripts/marksman/rotation_script.h"
#include "scripts/marksman/setup_script.h"
#include "scripts/marksman/loot_script.h"
#include "scripts/marksman/buff_script.h"
#include "scripts/marksman/random_human_script.h"

// clang-format off

enum layers{
    MAC_BASE,
    MAC_FN,
    WIN_BASE,
    WIN_FN
};

#define KC_TASK LGUI(KC_TAB)
#define KC_FLXP LGUI(KC_E)

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    [MAC_BASE] = LAYOUT_tkl_ansi(
        KC_ESC,             KC_BRID,  KC_BRIU,  KC_NO,    KC_NO,    RM_VALD,  RM_VALU,  KC_MPRV,  KC_MPLY,  KC_MNXT,  KC_MUTE,  KC_VOLD,    KC_VOLU,  KC_NO,    KC_NO,    RM_NEXT,
        KC_GRV,   KC_1,     KC_2,     KC_3,     KC_4,     KC_5,     KC_6,     KC_7,     KC_8,     KC_9,     KC_0,     KC_MINS,  KC_EQL,     KC_BSPC,  KC_INS,   KC_HOME,  KC_PGUP,
        KC_TAB,   KC_Q,     KC_W,     KC_E,     KC_R,     KC_T,     KC_Y,     KC_U,     KC_I,     KC_O,     KC_P,     KC_LBRC,  KC_RBRC,    KC_BSLS,  KC_DEL,   KC_END,   KC_PGDN,
        KC_CAPS,  KC_A,     KC_S,     KC_D,     KC_F,     KC_G,     KC_H,     KC_J,     KC_K,     KC_L,     KC_SCLN,  KC_QUOT,              KC_ENT,
        KC_LSFT,            KC_Z,     KC_X,     KC_C,     KC_V,     KC_B,     KC_N,     KC_M,     KC_COMM,  KC_DOT,   KC_SLSH,              KC_RSFT,            KC_UP,
        KC_LCTL,  KC_LOPT,  KC_LCMD,                                KC_SPC,                                 KC_RCMD,  KC_ROPT,  MO(MAC_FN), KC_RCTL,  KC_LEFT,  KC_DOWN,  KC_RGHT),

    [MAC_FN] = LAYOUT_tkl_ansi(
        _______,            KC_F1,    KC_F2,    KC_F3,    KC_F4,    KC_F5,    KC_F6,    KC_F7,    KC_F8,    KC_F9,    KC_F10,   KC_F11,     KC_F12,   _______,  _______,  RM_TOGG,
        _______, _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,    _______,  _______,  _______,  _______,
        RM_TOGG,  RM_NEXT,  RM_VALU,  RM_HUEU,  RM_SATU,  RM_SPDU,  _______,  _______,  _______,  _______,  _______,  _______,  _______,    _______,  _______,  _______,  _______,
        _______,  RM_PREV,  RM_VALD,  RM_HUED,  RM_SATD,  RM_SPDD,  _______,  _______,  _______,  _______,  _______,  _______,              _______,
        _______,            _______,  _______,  _______,  _______,  _______,  NK_TOGG,  _______,  _______,  _______,  _______,              _______,            _______,
        _______,  _______,  _______,                                _______,                                _______,  _______,  _______,    _______,  _______,  _______,  _______),

    [WIN_BASE] = LAYOUT_tkl_ansi(
        KC_ESC,             KC_F1,    KC_F2,    KC_F3,    KC_F4,    KC_F5,    KC_F6,    KC_F7,    KC_F8,    KC_F9,    KC_F10,   KC_F11,     KC_F12,   KC_PSCR,  KC_NO,    RM_NEXT,
        KC_GRV,   KC_1,     KC_2,     KC_3,     KC_4,     KC_5,     KC_6,     KC_7,     KC_8,     KC_9,     KC_0,     KC_MINS,  KC_EQL,     KC_BSPC,  KC_INS,   KC_HOME,  KC_PGUP,
        KC_TAB,   KC_Q,     KC_W,     KC_E,     KC_R,     KC_T,     KC_Y,     KC_U,     KC_I,     KC_O,     KC_P,     KC_LBRC,  KC_RBRC,    KC_BSLS,  KC_DEL,   KC_END,   KC_PGDN,
        KC_CAPS,  KC_A,     KC_S,     KC_D,     KC_F,     KC_G,     KC_H,     KC_J,     KC_K,     KC_L,     KC_SCLN,  KC_QUOT,              KC_ENT,
        KC_LSFT,            KC_Z,     KC_X,     KC_C,     KC_V,     KC_B,     KC_N,     KC_M,     KC_COMM,  KC_DOT,   KC_SLSH,              KC_RSFT,            KC_UP,
        KC_LCTL,  KC_LWIN,  KC_LALT,                                KC_SPC,                                 KC_RALT,  KC_RWIN,  MO(WIN_FN), KC_RCTL,  KC_LEFT,  KC_DOWN,  KC_RGHT),

    [WIN_FN] = LAYOUT_tkl_ansi(
        _______,            KC_BRID,  KC_BRIU,  KC_TASK,  KC_FLXP,  RM_VALD,  RM_VALU,  KC_MPRV,  KC_MPLY,  KC_MNXT,  KC_MUTE,  KC_VOLD,    KC_VOLU,  _______,  _______,  RM_TOGG,
        _______, _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,  _______,    _______,  _______,  _______,  _______,
        RM_TOGG,  RM_NEXT,  RM_VALU,  RM_HUEU,  RM_SATU,  RM_SPDU,  _______,  _______,  _______,  _______,  _______,  _______,  _______,    _______,  _______,  _______,  _______,
        _______,  RM_PREV,  RM_VALD,  RM_HUED,  RM_SATD,  RM_SPDD,  _______,  _______,  _______,  _______,  _______,  _______,              _______,
        _______,            _______,  _______,  _______,  _______,  _______,  NK_TOGG,  _______,  _______,  _______,  _______,              _______,            _______,
        _______,  _______,  _______,                                _______,                                _______,  _______,  _______,    _______,  _______,  _______,  _______),
};

// clang-format on

// ---------------------------------------------------------------------------
// Script engine state
// ---------------------------------------------------------------------------

static bool      running             = false;
static runner_t  runner              = {0};
static uint32_t  last_setup_time_ms  = 0;
static uint32_t  last_loot_time_ms   = 0;
static uint32_t  last_buff_time_ms   = 0;
static uint32_t  last_human_time_ms  = 0;
static uint32_t  rotation_cooldown_until_ms = 0;
static bool      rotation_cooldown_active   = false;

#define SETUP_INTERVAL_MS  120000  // 2 minutes
#define LOOT_INTERVAL_MS    60000  // 1 minute
#define BUFF_INTERVAL_MS    15000  // 15 seconds
#define HUMAN_INTERVAL_MS  600000  // up to 10 minutes (random_range pulls down to 90 s)

// ---------------------------------------------------------------------------
// keyboard_post_init_user  –  seed RNG once at boot
// ---------------------------------------------------------------------------

void keyboard_post_init_user(void) {
    timing_seed_rng();
}

// ---------------------------------------------------------------------------
// process_record_user  –  handle RUN_TOGGLE
// ---------------------------------------------------------------------------

bool process_record_user(uint16_t keycode, keyrecord_t *record) {
    if (record->event.pressed) {
        uprintf("Key down: %d\n", keycode);
    }
    switch (keycode) {
        case KC_F7:
            if (record->event.pressed && !running) {
                // RUN ON
                uprintf("Running...\n");
                running            = true;
                runner_start(&runner, BUFF_SCRIPT, MODE_ROTATION);
            }
            return false;  // no OS key output
        case KC_F8:
            if (record->event.pressed && running) {
                uprintf("Pausing...\n");
                running = false;
                runner_stop(&runner);
                // Release any basic keycodes the runner may have left held.
                // Covers letters (KC_A=0x04) through the full navigation cluster
                // (KC_UP=0x52). Modifier keys (LALT etc.) are always TAP'd so
                // they can't be stuck; they live in report.mods, not report.keys.
                for (uint8_t kc = KC_A; kc <= KC_UP; kc++) {
                    if (is_key_pressed(kc)) {
                        uprintf("[F8] releasing stuck key: %d\n", kc);
                        unregister_code(kc);
                        wait_ms(jitter(40, 20));
                    }
                }
            }
            return false;  // no OS key output
        default:
            return true;
    }
}

// ---------------------------------------------------------------------------
// matrix_scan_user  –  policy layer (called every ~1 ms)
// ---------------------------------------------------------------------------
// static uint32_t gate1_time_ms = 53000;
// static uint32_t carcion_time_ms = 46000;
static uint32_t tallahart_time_ms = 46000;
// static uint32_t odium_time_ms = 48000;
void matrix_scan_user(void) {
    if (!running) {
        return;
    }

    // Advance the runner
    runner_task(&runner);

    // If still executing, nothing to decide yet
    if (runner.active) {
        return;
    }

    // Script just finished — decide what comes next
    if (runner.mode == MODE_ROTATION) {
        // Start cooldown timer once when rotation ends
        if (!rotation_cooldown_active) {
            uint32_t cd = random_range(8500, 71, 100);  // 6–8.5 s
            rotation_cooldown_until_ms = timer_read32() + cd;
            rotation_cooldown_active   = true;
            uprintf("\n[scan] rotation done -> cooldown %lu ms\n", cd);
        }
        // High-priority scripts interrupt cooldown immediately
        if (last_setup_time_ms == 0 || timer_elapsed32(last_setup_time_ms) >= random_range(tallahart_time_ms, 70, 100)) {
            rotation_cooldown_active = false;
            uprintf("[scan] rotation -> setup (interval elapsed)\n");
            runner_start(&runner, make_setup_tallahart(), MODE_SETUP);
        } else if (timer_elapsed32(last_loot_time_ms) >= random_range(LOOT_INTERVAL_MS, 80, 100)) {
            rotation_cooldown_active = false;
            uprintf("[scan] rotation -> loot (interval elapsed)\n");
            runner_start(&runner, LOOT_FULL, MODE_LOOT);
        } else if (timer_elapsed32(last_buff_time_ms) >= random_range(BUFF_INTERVAL_MS, 70, 100)) {
            rotation_cooldown_active = false;
            uprintf("[scan] rotation -> buff (interval elapsed)\n");
            runner_start(&runner, BUFF_SCRIPT, MODE_BUFF);
        } else if (timer_elapsed32(last_human_time_ms) >= random_range(HUMAN_INTERVAL_MS, 15, 100)) {
            rotation_cooldown_active = false;
            uprintf("[scan] rotation -> human (interval elapsed)\n");
            runner_start(&runner, make_random_human(), MODE_HUMAN);
        } else if (timer_expired32(timer_read32(), rotation_cooldown_until_ms)) {
            rotation_cooldown_active = false;
            uprintf("[scan] cooldown done -> rotation\n");
            runner_start(&runner, make_rotation_tallahart(), MODE_ROTATION);
        }
        // else: still in cooldown, do nothing this tick
    } else if (runner.mode == MODE_SETUP) {
        uprintf("\n");
        uprintf("[scan] setup done -> rotation\n");
        last_setup_time_ms = timer_read32();
        runner_start(&runner, make_rotation_tallahart(), MODE_ROTATION);
    } else if (runner.mode == MODE_LOOT) {
        uprintf("\n");
        uprintf("[scan] loot done -> rotation\n");
        last_loot_time_ms = timer_read32();
        runner_start(&runner, make_rotation_tallahart(), MODE_ROTATION);
    } else if (runner.mode == MODE_BUFF) {
        uprintf("\n");
        uprintf("[scan] buff done -> rotation\n");
        last_buff_time_ms = timer_read32();
        runner_start(&runner, make_rotation_tallahart(), MODE_ROTATION);
    } else if (runner.mode == MODE_HUMAN) {
        uprintf("\n");
        uprintf("[scan] human done -> rotation\n");
        last_human_time_ms = timer_read32();
        runner_start(&runner, make_rotation_tallahart(), MODE_ROTATION);
    }
}
