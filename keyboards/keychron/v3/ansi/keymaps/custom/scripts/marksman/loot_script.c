// Loot script – pick up nearby items.

#include "loot_script.h"
#include "job.h"

const cmd_t LOOT_EMPTY[] = {
    END(),
};

const cmd_t LOOT_FULL[] = {
    TAP(KC_UP),
    WAIT_JITTER_UP_D(2000),
    TAP(KC_UP),
    WAIT_JITTER_UP_D(2000),
    TAP(KC_UP),
    WAIT_JITTER_UP_D(2000),
    TELEPORT_RESET_2X(),
    END(),
};
