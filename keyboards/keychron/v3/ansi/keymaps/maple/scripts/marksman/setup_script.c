// Setup script – Calm Beach 3 stationary setup sequence.
// Ported from map_calm_beach_3.py setup_stationaries()

#include "setup_script.h"
#include "job.h"

const cmd_t SETUP_SCRIPT[] = {
    // marksman.rope(delayAfter=1.6)
    ROPE(),

    // marksman.dash()
    DASH(),
    DASH(),

    // marksman.janus()
    TRY_JANUS(),

    // marksman.bot.press('left', 0.4) -> release('left')
    WALK_LEFT(400),

    // marksman.jump_down_attack(delayAfter=0.45)
    JUMP_DOWN_ATTACK_DELAY(450),

    // marksman.jump_attack()
    JUMP_ATTACK(),

    // marksman.janus2()
    TRY_JANUS2(),

    DASH(),
    DASH(),

    // marksman.janus3()
    TRY_JANUS3(),

    // marksman.jump_attack()
    JUMP_ATTACK(),

    // marksman.erda_fountain()
    TRY_ERDA_FOUNTAIN(),

    // marksman.teleport_setup(attackAfter=True, boltBurstAfter=True)
    TELEPORT_RESET(),

    END(),
};

const cmd_t SETUP_CERNIUM[] = {
    TAP_D(KC_LEFT, 80),
    TRY_JANUS(),
    DASH(),
    DASH(),
    TRY_JANUS2(),
    JUMP_ATTACK(),
    WAIT_JITTER_D(1000),
    TRY_JANUS3(),
    WAIT_JITTER_D(300),
    DASH(),
    JUMP_ATTACK(),
    TRY_ERDA_FOUNTAIN(),
    DASH(),
    WAIT_JITTER_D(300),
    TAP_D(KC_RIGHT, 200),
    JUMP_DOWN_ATTACK_DELAY(700),
    JUMP_ATTACK(),
    JUMP_ATTACK(),
    JUMP_ATTACK(),
    JUMP_ATTACK(),
    WAIT_JITTER_D(200),
    TAP_D(KC_LEFT, 200),
    WAIT_JITTER_D(200),
    TELEPORT_RESET(),
    WAIT_JITTER_D(500),
    END(),
};

const cmd_t SETUP_ODIUM[] = {
    ROPE(),
    DASH(),
    TRY_JANUS(),
    DASH(),
    FLASH_JUMP(),
    TRY_JANUS2(),
    DASH(),
    FLASH_JUMP(),
    TRY_JANUS3(),
    FLASH_JUMP(),
    TRY_ERDA_FOUNTAIN(),
    DASH(),
    WAIT_JITTER_D(300),
    TELEPORT_RESET(),
    END(),
};
