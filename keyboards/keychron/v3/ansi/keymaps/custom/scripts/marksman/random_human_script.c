// Random human script – occasional UI interactions to simulate human behaviour.
// Pattern: open menu → look at it (500–1000 ms) → close it.

#include <stdlib.h>
#include "random_human_script.h"
#include "job.h"

// Buddy list  ("-" key toggles open/close)
// static const cmd_t HUMAN_BUDDY_LIST[] = {
//     TAP(KC_MINS), WAIT_JITTER_UP(1500, 100),
//     TAP(KC_MINS), WAIT_JITTER_UP_D(200),
//     END(),
// };

// Quests  ("0" key)
// static const cmd_t HUMAN_QUESTS[] = {
//     TAP(KC_0), WAIT_JITTER_UP(1500, 100),
//     TAP(KC_0), WAIT_JITTER_UP_D(200),
//     END(),
// };

// Guild UI  ("g" key)
// static const cmd_t HUMAN_GUILD[] = {
//     TAP(KC_G), WAIT_JITTER_UP(1500, 100),
//     TAP(KC_G), WAIT_JITTER_UP_D(200),
//     END(),
// };

// Skills  ("k" key)
// static const cmd_t HUMAN_SKILLS[] = {
//     TAP(KC_K), WAIT_JITTER_UP(1500, 100),
//     TAP(KC_K), WAIT_JITTER_UP_D(200),
//     END(),
// };

const cmd_t HUMAN_1[] = {
    DASH(),
    JUMP_ATTACK(),
    WALK_RIGHT(400),
    WAIT_JITTER_UP_D(200),
    TELEPORT_RESET_2X(),
    END(),
};

const cmd_t HUMAN_2[] = {
    DASH(),
    JUMP_ATTACK(),
    WALK_LEFT(400),
    WAIT_JITTER_UP_D(100),
    WALK_RIGHT(200),
    SHOOT(),
    WAIT_JITTER_UP_D(200),
    TELEPORT_RESET_2X(),
    END(),
};

const cmd_t HUMAN_3[] = {
    ROPE_DELAY(1000),
    SHOOT(),
    JUMP_ATTACK(),
    JUMP_ATTACK(),
    WAIT_JITTER_UP_D(500),
    WALK_RIGHT(600),
    SHOOT(),
    WAIT_JITTER_UP_D(200),
    TELEPORT_RESET_2X(),
    END(),
};

const cmd_t HUMAN_4[] = {
    WALK_RIGHT(700),
    JUMP_ATTACK(),
    SHOOT(),
    WAIT_JITTER_UP_D(700),
    DASH(),
    JUMP_ATTACK(),
    WAIT_JITTER_UP_D(750),
    TELEPORT_RESET_2X(),
    END(),
};

const cmd_t HUMAN_5[] = {
    DASH(),
    TRY_BOLT_BURST(),
    JUMP_ATTACK(),
    ROPE(),
    JUMP_ATTACK(),
    WAIT_JITTER_UP_D(750),
    TELEPORT_RESET_2X(),
    END(),
};

const cmd_t HUMAN_6[] = {
    DASH(),
    TAP(KC_4),
    WAIT_JITTER_UP_D(700),
    FLASH_UP(),
    SHOOT(),
    WALK_RIGHT(600),
    JUMP_ATTACK(),
    WAIT_JITTER_UP_D(750),
    TELEPORT_RESET_2X(),
    END(),
};

const cmd_t HUMAN_7[] = {
    TAP(KC_UP),
    WAIT_JITTER_UP_D(400),
    SHOOT(),
    ROPE_DELAY(1000),
    JUMP_ATTACK(),
    SHOOT(),
    WALK_RIGHT(700),
    SHOOT(),
    WAIT_JITTER_UP_D(300),
    TELEPORT_RESET_2X(),
    END(),
};

const cmd_t HUMAN_8[] = {
    TAP(KC_UP),
    WAIT_JITTER_UP_D(400),
    TAP(KC_UP),
    WAIT_JITTER_UP_D(400),
    TRY_BOLT_BURST(),
    WALK_LEFT(500),
    JUMP_ATTACK(),
    DASH(),
    WALK_RIGHT(600),
    SHOOT(),
    WAIT_JITTER_UP_D(250),
    TELEPORT_RESET_2X(),
    END(),
};
// ---------------------------------------------------------------------------
// Weighted picker
// ---------------------------------------------------------------------------

typedef struct {
    const cmd_t *script;
    uint8_t      weight;
} weighted_human_t;

const cmd_t* make_random_human(void) {
    static const weighted_human_t table[] = {
        { HUMAN_1, 1 },
        { HUMAN_2, 1 },
        { HUMAN_3, 1 },
        { HUMAN_4, 1 },
        { HUMAN_5, 1 },
        { HUMAN_6, 1 },
        { HUMAN_7, 1 },
        { HUMAN_8, 1 },
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
