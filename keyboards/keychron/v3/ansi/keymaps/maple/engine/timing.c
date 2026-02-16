// Timing utilities – jitter and RNG seeding.

#include "timing.h"
#include <stdlib.h>
#include <stdbool.h>
#include QMK_KEYBOARD_H  // timer_read32()

// ---------------------------------------------------------------------------
// timing_seed_rng
// ---------------------------------------------------------------------------

void timing_seed_rng(void) {
    srand((unsigned int)timer_read32());
}

// ---------------------------------------------------------------------------
// jitter_u32
// ---------------------------------------------------------------------------
// Returns base_ms adjusted by a random offset in the range [-pct%, +pct%].
//
// Steps:
//   1. max_offset = base_ms * pct / 100
//   2. random value in [0, 2 * max_offset]
//   3. result = base_ms - max_offset + random_value
//
// Edge cases:
//   - pct == 0  -> returns base_ms unchanged
//   - pct > 100 -> clamped to 100

uint32_t jitter(uint32_t base_ms, uint8_t pct) {
    if (pct == 0) return base_ms;
    if (pct > 100) pct = 100;

    uint32_t max_offset = base_ms * pct / 100;
    uint32_t range      = max_offset * 2 + 1;  // inclusive both ends
    uint32_t offset     = (uint32_t)rand() % range;

    return base_ms - max_offset + offset;
}

// ---------------------------------------------------------------------------
// timing_chance
// ---------------------------------------------------------------------------
// Returns true with probability pct/100.  pct 0 => always false, pct 100 => always true.

bool timing_chance(uint8_t pct) {
    if (pct == 0) return false;
    if (pct >= 100) return true;
    return (uint8_t)(rand() % 100) < pct;
}
