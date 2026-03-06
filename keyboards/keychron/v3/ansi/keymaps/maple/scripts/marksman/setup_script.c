// Setup script – Calm Beach 3 stationary setup sequence.
// Ported from map_calm_beach_3.py setup_stationaries()

#include "setup_script.h"
#include "job.h"

const cmd_t SETUP_CARCION[] = {
    JUMP_DOWN_DELAY(100),
    JUMP_DOWN_DELAY(500),
    TAP(KC_UP),
    WAIT_JITTER_UP_D(70),
    TAP(KC_UP),
    WAIT_JITTER_UP_D(70),
    WAIT_JITTER_D(500),
    TAP(KC_RIGHT),
    WAIT_JITTER_UP_D(70),
    TAP(KC_RIGHT),
    WAIT_JITTER_UP_D(70),
    JUMP_DOWN_DELAY(100),
    JUMP_DOWN_DELAY(500),
    JUMP_DOWN_DELAY(100),
    JUMP_DOWN_DELAY(500),
    TRY_JANUS(),
    JUMP_ATTACK(),
    TAP_D(KC_RIGHT, 600),
    WAIT_JITTER_UP_D(70),
    TRY_JANUS2(),
    TAP_D(KC_RIGHT, 600),
    WAIT_JITTER_UP_D(70),
    FLASH_JUMP(),
    TRY_JANUS3(),
    FLASH_JUMP(),
    TAP_D(KC_RIGHT, 200),
    WAIT_JITTER_UP_D(70),
    TRY_ERDA_FOUNTAIN(),
    TAP_D(KC_LEFT, 300),
    WAIT_JITTER_UP_D(70),
    TELEPORT_RESET(),
    END(),
};
