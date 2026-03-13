// Rotation script – one rotation cycle for marksman.
// Uses generator macros for readability.

#include <stdlib.h>
#include "rotation_script.h"
#include "job.h"


const cmd_t ROTATION_TALLAHART_SURGEBOLT[] = {
    SHOOT(),
    TRY_SURGEBOLT(),
    END(),
};

const cmd_t ROTATION_TALLAHART_SHOOT[] = {
    SHOOT(),
    END(),
};

const cmd_t ROTATION_TALLAHART_SHOOT_1[] = {
    JUMP_DELAY(100),
    SHOOT(),
    END(),
};

const cmd_t ROTATION_TALLAHART_SHOOT_2[] = {
    JUMP_GROUND(),
    SHOOT(),
    END(),
};

const cmd_t ROTATION_TALLAHART_SHOOT_3[] = {
    JUMP_GROUND(),
    JUMP_GROUND(),
    SHOOT(),
    END(),
};

const cmd_t ROTATION_TALLAHART_SHOOT_5[] = {
    JUMP_GROUND(),
    JUMP_DELAY(100),
    SHOOT(),
    END(),
};

const cmd_t ROTATION_TALLAHART_BOLT_BURST[] = {
    TRY_BOLT_BURST(),
    END(),
};

const cmd_t ROTATION_TALLAHART_BOLT_BURST_1[] = {
    JUMP_DELAY(100),
    TRY_BOLT_BURST(),
    END(),
};

const cmd_t ROTATION_TALLAHART_BOLT_BURST_2[] = {
    JUMP_GROUND(),
    TRY_BOLT_BURST(),
    END(),
};

const cmd_t ROTATION_TALLAHART_BOLT_BURST_3[] = {
    JUMP_GROUND(),
    JUMP_GROUND(),
    TRY_BOLT_BURST(),
    END(),
};

const cmd_t ROTATION_TALLAHART_JUMP_ATTACK[] = {
    FLASH_UP(),
    JUMP_ATTACK(),
    SHOOT(),
    WAIT_JITTER_UP_D(200),
    TELEPORT_RESET_2X(),
    END(),
};

const cmd_t ROTATION_TALLAHART_JUMP_ATTACK_1[] = {
    SHOOT(),
    DASH(),
    JUMP_ATTACK(),
    WAIT_JITTER_UP_D(400),
    TELEPORT_RESET_2X(),
    END(),
};

const cmd_t ROTATION_TALLAHART_JUMP_ATTACK_2[] = {
    JUMP_GROUND(),
    DASH(),
    JUMP_ATTACK(),
    WAIT_JITTER_UP_D(400),
    TELEPORT_RESET_2X(),
    END(),
};

const cmd_t ROTATION_TALLAHART_DASH[] = {
    DASH(),
    SHOOT(),
    WAIT_JITTER_UP_D(200),
    TELEPORT_RESET_2X(),
    END(),
};

const cmd_t ROTATION_TALLAHART_DASH_1[] = {
    DASH(),
    TRY_BOLT_BURST(),
    WAIT_JITTER_UP_D(300),
    TELEPORT_RESET_2X(),
    END(),
};

// ---------------------------------------------------------------------------
// Weighted rotation picker
// ---------------------------------------------------------------------------

typedef struct {
    const cmd_t *script;
    uint8_t      weight;
} weighted_rotation_t;

const cmd_t* make_rotation_tallahart(void) {
    // Total = 65: SHOOT=39 (60%), BOLT_BURST=13 (20%), 13 rest×1 (20%)
    static const weighted_rotation_t table[] = {
        { ROTATION_TALLAHART_SHOOT,        39 },
        { ROTATION_TALLAHART_BOLT_BURST,   13 },
        { ROTATION_TALLAHART_SURGEBOLT,     1 },
        { ROTATION_TALLAHART_SHOOT_1,       1 },
        { ROTATION_TALLAHART_SHOOT_2,       1 },
        { ROTATION_TALLAHART_SHOOT_3,       1 },
        { ROTATION_TALLAHART_SHOOT_5,       1 },
        { ROTATION_TALLAHART_BOLT_BURST_1,  1 },
        { ROTATION_TALLAHART_BOLT_BURST_2,  1 },
        { ROTATION_TALLAHART_BOLT_BURST_3,  1 },
        { ROTATION_TALLAHART_JUMP_ATTACK,   1 },
        { ROTATION_TALLAHART_JUMP_ATTACK_1, 1 },
        { ROTATION_TALLAHART_JUMP_ATTACK_2, 1 },
        { ROTATION_TALLAHART_DASH,          1 },
        { ROTATION_TALLAHART_DASH_1,        1 },
    };
    static const uint8_t count = sizeof(table) / sizeof(table[0]);

    uint16_t total = 0;
    for (uint8_t i = 0; i < count; i++) total += table[i].weight;

    uint16_t r = (uint16_t)(rand() % total);
    uint16_t cumulative = 0;
    for (uint8_t i = 0; i < count; i++) {
        cumulative += table[i].weight;
        if (r < cumulative) return table[i].script;
    }
    return table[0].script;
}

