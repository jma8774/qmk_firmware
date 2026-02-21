// Rotation script – one rotation cycle for marksman.
// Uses generator macros for readability.

#include "rotation_script.h"
#include "job.h"

const cmd_t ROTATION_NORMAL[] = {
    SHOOT(),
    WAIT_JITTER(5000, 15),
    END(),
};

const cmd_t ROTATION_PS_RIGHT[] = {
    JUMP_ATTACK_DELAY(100, 50),
    JUMP_ATTACK_DELAY(100, 50),
    JUMP_ATTACK_DELAY(100, 50),
    JUMP_ATTACK_DELAY(100, 50),
    JUMP_ATTACK_DELAY(100, 50),
    TELEPORT_RESET(),
    END(),
};

const cmd_t ROTATION_CERNIUM[] = {
    TRY_BOLT_BURST(),
    SHOOT(),
    WAIT_JITTER(700, 15),
    END(),
};

const cmd_t ROTATION_ODIUM[] = {
    JUMP_ATTACK(),
    JUMP_ATTACK(),
    JUMP_ATTACK(),
    JUMP_ATTACK(),
    CHANCE(15),
    SKIP_FALSE(JUMP_ATTACK_SKIPS_COUNT),
    JUMP_ATTACK(),
    TELEPORT_RESET(),
    WAIT_JITTER_UP_D(150),
    END(),
};

const cmd_t ROTATION_GATE1[] = {
    SHOOT(),
    END(),
};

const cmd_t ROTATION_CARCION[] = {
    SHOOT(),
    END(),
};
