// Random human script – occasional idle actions to simulate human behaviour.
// Triggered every 90–120 s while the bot is running.

#pragma once

#include "../../engine/script_engine.h"

// Returns a randomly selected human-like script each call.
const cmd_t* make_random_human(void);
