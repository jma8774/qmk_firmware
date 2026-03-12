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

const cmd_t ROTATION_TALLAHART_SHOOT_4[] = {
    JUMP_GROUND(),
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
    JUMP_ATTACK(),
    TELEPORT_RESET_2X(),
    END(),
};

const cmd_t ROTATION_TALLAHART_JUMP_ATTACK_1[] = {
    SHOOT(),
    JUMP_ATTACK(),
    TELEPORT_RESET_2X(),
    END(),
};

const cmd_t ROTATION_TALLAHART_JUMP_ATTACK_2[] = {
    JUMP_GROUND(),
    JUMP_ATTACK(),
    TELEPORT_RESET_2X(),
    END(),
};

const cmd_t ROTATION_TALLAHART_DASH[] = {
    DASH(),
    SHOOT(),
    END(),
};

const cmd_t ROTATION_TALLAHART_DASH_1[] = {
    DASH(),
    TRY_BOLT_BURST(),
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
    static const weighted_rotation_t table[] = {
        { ROTATION_TALLAHART_SURGEBOLT,    1 },
        { ROTATION_TALLAHART_SHOOT,       38 },
        { ROTATION_TALLAHART_SHOOT_1,      2 },
        { ROTATION_TALLAHART_SHOOT_2,      2 },
        { ROTATION_TALLAHART_SHOOT_3,      2 },
        { ROTATION_TALLAHART_SHOOT_4,      1 },
        { ROTATION_TALLAHART_SHOOT_5,      1 },
        { ROTATION_TALLAHART_BOLT_BURST,  15 },
        { ROTATION_TALLAHART_BOLT_BURST_1, 2 },
        { ROTATION_TALLAHART_BOLT_BURST_2, 2 },
        { ROTATION_TALLAHART_BOLT_BURST_3, 1 },
        { ROTATION_TALLAHART_JUMP_ATTACK,  3 },
        { ROTATION_TALLAHART_JUMP_ATTACK_1,1 },
        { ROTATION_TALLAHART_JUMP_ATTACK_2,1 },
        { ROTATION_TALLAHART_DASH,         2 },
        { ROTATION_TALLAHART_DASH_1,       1 },
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

const cmd_t ROTATION_COOLDOWN[] = {
    WAIT_JITTER_UP(3000, 100), // 3–6 s
    END(),
};
