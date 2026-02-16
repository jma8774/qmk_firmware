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
