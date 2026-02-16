// Timing utilities – jitter and RNG seeding for the script engine.

#pragma once

#include <stdbool.h>
#include <stdint.h>

// Return base_ms adjusted by a random offset of +/- pct%.
// Example: jitter_u32(5000, 15) returns a value in [4250, 5750].
uint32_t jitter(uint32_t base_ms, uint8_t pct);

// Seed the C stdlib rand() from timer_read32().
// Call once (e.g. in keyboard_post_init_user).
void timing_seed_rng(void);

// Return true with probability pct/100 (pct 0–100).  Uses rand() % 100 < pct.
bool timing_chance(uint8_t pct);
