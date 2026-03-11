// Random human script – occasional UI interactions to simulate human behaviour.
// Pattern: open menu → look at it (500–1000 ms) → close it.

#include <stdlib.h>
#include "random_human_script.h"
#include "job.h"

// Buddy list  ("-" key toggles open/close)
static const cmd_t HUMAN_BUDDY_LIST[] = {
    TAP(KC_MINS), WAIT_JITTER_UP(500, 100),
    TAP(KC_MINS), WAIT_JITTER_UP_D(200),
    END(),
};

// Quests  ("0" key)
static const cmd_t HUMAN_QUESTS[] = {
    TAP(KC_0), WAIT_JITTER_UP(500, 100),
    TAP(KC_0), WAIT_JITTER_UP_D(200),
    END(),
};

// Guild UI  ("g" key)
static const cmd_t HUMAN_GUILD[] = {
    TAP(KC_G), WAIT_JITTER_UP(500, 100),
    TAP(KC_G), WAIT_JITTER_UP_D(200),
    END(),
};

// Skills  ("k" key)
static const cmd_t HUMAN_SKILLS[] = {
    TAP(KC_K), WAIT_JITTER_UP(500, 100),
    TAP(KC_K), WAIT_JITTER_UP_D(200),
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
        { HUMAN_BUDDY_LIST, 1 },
        { HUMAN_QUESTS,     1 },
        { HUMAN_GUILD,      1 },
        { HUMAN_SKILLS,     1 },
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
