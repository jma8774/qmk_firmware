// Loot script – pick up nearby items.

#include "loot_script.h"
#include "job.h"

const cmd_t LOOT_EMPTY[] = {
    END(),
};

const cmd_t LOOT_GATE1[] = {
    DASH(),
    FLASH_JUMP(),
    WAIT_JITTER_UP_D(1000),
    DASH(),
    SHOOT(),
    DASH(),
    WAIT_JITTER_UP_D(1000),
    FLASH_JUMP(),
    DASH(),
    SHOOT(),
    WAIT_JITTER_UP_D(1000),
    TELEPORT_RESET(),
    END(),
};

const cmd_t LOOT_CARCION[] = {
    JUMP_DOWN_DELAY(100),
    JUMP_DOWN_DELAY(600),
    JUMP_ATTACK(),
    JUMP_ATTACK(),
    JUMP_ATTACK(),
    JUMP_ATTACK(),
    TELEPORT_RESET(),
    END(),
};
