// Setup script – Calm Beach 3 stationary setup sequence.
// Ported from map_calm_beach_3.py setup_stationaries()

#include <stdlib.h>
#include "setup_script.h"
#include "job.h"

// ---------------------------------------------------------------------------
// Random UP variant builder
//
// Generates one of 2^4 = 16 variants at runtime.  Each of the 4 UP taps
// in the base SETUP_TALLAHART_UP pattern can randomly get an extra UP added
// after it (bit 0 = before JANUS, bit 1 = before JANUS2, bit 2 = before
// JANUS3, bit 3 = the final UP).  The buffer is static so it outlives the
// call and can be handed directly to runner_start().
// ---------------------------------------------------------------------------

static cmd_t _setup_up_buf[30];
const cmd_t* make_setup_tallahart_up(void) {
    static const cmd_t c_tap_up[]  = { TAP(KC_UP) }; // noqa — building block, WAIT added by caller loop
    static const cmd_t c_wait_up[] = { WAIT_JITTER_UP_20(70) };
    static const cmd_t c_try[3][3] = {
        { TRY_JANUS() },
        { TRY_JANUS2() },
        { TRY_JANUS3() },
    };
    static const cmd_t c_end[] = { END() };

    static const uint8_t try_len = sizeof(c_try[0]) / sizeof(cmd_t);

    uint8_t n = 0;

    for (uint8_t i = 0; i < 4; i++) {
        _setup_up_buf[n++] = c_tap_up[0];
        _setup_up_buf[n++] = c_wait_up[0];
        if (rand() % 2) {
            _setup_up_buf[n++] = c_tap_up[0];
            _setup_up_buf[n++] = c_wait_up[0];
        }
        if (i < 3) {
            for (uint8_t j = 0; j < try_len; j++) {
                _setup_up_buf[n++] = c_try[i][j];
            }
        }
    }
    _setup_up_buf[n++] = c_end[0];

    return _setup_up_buf;
}

// ---------------------------------------------------------------------------
// Random RESET variant builder
//
// Same optional-double-UP logic for the 3 UP taps, plus a random choice
// between TELEPORT_RESET (2x X) and TELEPORT_RESET_2X (3x X) at the end.
// ---------------------------------------------------------------------------

static cmd_t _setup_reset_buf[30];
const cmd_t* make_setup_tallahart_reset(void) {
    static const cmd_t c_tap_up[]  = { TAP(KC_UP) }; // noqa — building block, WAIT added by caller loop
    static const cmd_t c_wait_up[] = { WAIT_JITTER_UP_20(70) };
    static const cmd_t c_try[3][3] = {
        { TRY_JANUS() },
        { TRY_JANUS2() },
        { TRY_JANUS3() },
    };
    static const cmd_t c_reset1[] = { TELEPORT_RESET() };
    static const cmd_t c_reset2[] = { TELEPORT_RESET_2X() };
    static const cmd_t c_end[]    = { END() };

    static const uint8_t try_len    = sizeof(c_try[0])   / sizeof(cmd_t);
    static const uint8_t reset1_len = sizeof(c_reset1)   / sizeof(cmd_t);
    static const uint8_t reset2_len = sizeof(c_reset2)   / sizeof(cmd_t);

    uint8_t n = 0;

    for (uint8_t i = 0; i < 3; i++) {
        _setup_reset_buf[n++] = c_tap_up[0];
        _setup_reset_buf[n++] = c_wait_up[0];
        if (rand() % 2) {
            _setup_reset_buf[n++] = c_tap_up[0];
            _setup_reset_buf[n++] = c_wait_up[0];
        }
        for (uint8_t j = 0; j < try_len; j++) {
            _setup_reset_buf[n++] = c_try[i][j];
        }
    }

    const cmd_t *c_reset;
    uint8_t reset_len;
    if (rand() % 2) {
        c_reset   = c_reset1;
        reset_len = reset1_len;
    } else {
        c_reset   = c_reset2;
        reset_len = reset2_len;
    }
    for (uint8_t j = 0; j < reset_len; j++) {
        _setup_reset_buf[n++] = c_reset[j];
    }

    _setup_reset_buf[n++] = c_end[0];

    return _setup_reset_buf;
}

const cmd_t* make_setup_tallahart(void) {
    return rand() % 2 ? make_setup_tallahart_up() : make_setup_tallahart_reset();
}
