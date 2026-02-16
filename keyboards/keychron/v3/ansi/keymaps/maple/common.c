// Common utilities for keymap scripts.

#include "common.h"
#include <stdlib.h>

uint32_t random_range(uint32_t interval_ms, uint8_t min_pct, uint8_t max_pct) {
    if (min_pct > max_pct) {
        uint8_t t = min_pct;
        min_pct = max_pct;
        max_pct = t;
    }
    if (min_pct > 100) min_pct = 100;
    if (max_pct > 100) max_pct = 100;

    uint8_t range = max_pct - min_pct + 1;
    uint8_t pct   = min_pct + (uint8_t)(rand() % range);

    return (uint32_t)((uint64_t)interval_ms * (pct / 100.0));
}
