// Common utilities for keymap scripts.

#pragma once

#include <stdint.h>

// Return interval_ms scaled by a random percentage in [min_pct, max_pct].
// Example: random_range(5000, 80, 120) returns a value in [4000, 6000].
// Uses rand(); seed with timing_seed_rng() at init.
uint32_t random_range(uint32_t interval_ms, uint8_t min_pct, uint8_t max_pct);
