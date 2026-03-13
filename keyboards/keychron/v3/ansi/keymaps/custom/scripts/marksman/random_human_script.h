// Random human script – occasional idle actions to simulate human behaviour.
// Triggered every 90–120 s while the bot is running.

#pragma once

#include "../../engine/script_engine.h"

extern const cmd_t HUMAN_1[];
extern const cmd_t HUMAN_2[];
extern const cmd_t HUMAN_3[];
extern const cmd_t HUMAN_4[];
extern const cmd_t HUMAN_5[];
extern const cmd_t HUMAN_6[];

// Returns a randomly selected human-like script each call.
const cmd_t* make_random_human(void);
