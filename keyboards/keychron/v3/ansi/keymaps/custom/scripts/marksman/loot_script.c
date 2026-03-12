// Loot script – pick up nearby items.

#include <stdlib.h>
#include "loot_script.h"
#include "job.h"

const cmd_t LOOT_EMPTY[] = {
    END(),
};

// ---------------------------------------------------------------------------
// Dynamic LOOT_FULL builder
//
// Walks 2 UP positions; each UP tap may randomly get an extra UP after it
// (50% chance per position).  Buffer worst case: 2×(tap+wait+tap+wait+loot)
// + TELEPORT_RESET_2X (6) + END (1) = 17 entries.
// ---------------------------------------------------------------------------

static cmd_t _loot_full_buf[20];
const cmd_t* make_loot_full(void) {
    static const cmd_t c_tap_up[]    = { TAP(KC_UP) };         // noqa — building block, WAIT added by caller loop
    static const cmd_t c_wait_up[]   = { WAIT_JITTER_UP_20(70) };
    static const cmd_t c_wait_loot[] = { WAIT_JITTER_ABS_UP(3000, 2000) };
    static const cmd_t c_reset[]     = { TELEPORT_RESET_2X() };
    static const cmd_t c_end[]       = { END() };

    static const uint8_t reset_len = sizeof(c_reset) / sizeof(cmd_t);

    uint8_t n = 0;

    for (uint8_t i = 0; i < 2; i++) {
        _loot_full_buf[n++] = c_tap_up[0];
        _loot_full_buf[n++] = c_wait_up[0];
        if (rand() % 2) {
            _loot_full_buf[n++] = c_tap_up[0];
            _loot_full_buf[n++] = c_wait_up[0];
        }
        _loot_full_buf[n++] = c_wait_loot[0];
    }

    for (uint8_t j = 0; j < reset_len; j++) {
        _loot_full_buf[n++] = c_reset[j];
    }
    _loot_full_buf[n++] = c_end[0];

    return _loot_full_buf;
}
