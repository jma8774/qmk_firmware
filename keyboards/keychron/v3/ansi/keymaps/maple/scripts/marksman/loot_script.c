// Loot script – pick up nearby items.

#include "loot_script.h"
#include "job.h"

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
