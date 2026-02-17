// Timing utilities – jitter and RNG seeding for the script engine.

#pragma once

#include <stdbool.h>
#include <stdint.h>

// Return base_ms adjusted by a random offset of +/- pct%.
// Example: jitter(5000, 15) returns a value in [4250, 5750].
uint32_t jitter(uint32_t base_ms, uint8_t pct);

// Return base_ms adjusted by a random offset only downward (never over base_ms).
// Result is in [base_ms - base_ms*pct/100, base_ms]. Example: jitter_down(1000, 20) -> [800, 1000].
uint32_t jitter_down(uint32_t base_ms, uint8_t pct);

// Return base_ms adjusted by a random offset only upward (never under base_ms).
// Result is in [base_ms, base_ms + base_ms*pct/100]. Example: jitter_up(1000, 20) -> [1000, 1200].
uint32_t jitter_up(uint32_t base_ms, uint8_t pct);

// Seed the C stdlib rand() from timer_read32().
// Call once (e.g. in keyboard_post_init_user).
void timing_seed_rng(void);

// Return true with probability pct/100 (pct 0–100).  Uses rand() % 100 < pct.
bool timing_chance(uint8_t pct);
