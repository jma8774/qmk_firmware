// Cubing script – looping cubing script.

#include "cubing_script.h"
#include "job.h"

const cmd_t CUBING_SCRIPT[] = {
    TAP(KC_SPACE),
    WAIT_JITTER_UP(100, 200),
    TAP(KC_SPACE),
    WAIT_JITTER_UP(100, 200),
    TAP(KC_SPACE),
    WAIT_JITTER_UP(100, 200),
    TAP(KC_SPACE),
    WAIT_JITTER_UP(100, 200),
    END(),
};
