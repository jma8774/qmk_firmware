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
// jitter_down
// ---------------------------------------------------------------------------
// Returns base_ms minus a random offset in [0, pct% of base_ms]. Result is
// in [base_ms * (1 - pct/100), base_ms], never over base_ms.
//
// Example: jitter_down(1000, 20) returns a value in [800, 1000].

uint32_t jitter_down(uint32_t base_ms, uint8_t pct) {
    if (pct == 0) return base_ms;
    if (pct > 100) pct = 100;

    uint32_t max_offset = base_ms * pct / 100;
    if (max_offset == 0) return base_ms;

    uint32_t offset = (uint32_t)rand() % (max_offset + 1);  // 0..max_offset inclusive
    return base_ms - offset;
}

// ---------------------------------------------------------------------------
// jitter_up
// ---------------------------------------------------------------------------
// Returns base_ms plus a random offset in [0, pct% of base_ms]. Result is
// in [base_ms, base_ms + base_ms*pct/100], never under base_ms.
//
// Example: jitter_up(1000, 20) returns a value in [1000, 1200].

uint32_t jitter_up(uint32_t base_ms, uint8_t pct) {
    if (pct == 0) return base_ms;
    if (pct > 100) pct = 100;

    uint32_t max_offset = base_ms * pct / 100;
    if (max_offset == 0) return base_ms;

    uint32_t offset = (uint32_t)rand() % (max_offset + 1);  // 0..max_offset inclusive
    return base_ms + offset;
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
