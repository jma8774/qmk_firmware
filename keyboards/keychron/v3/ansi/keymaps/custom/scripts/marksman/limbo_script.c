// Limbo script – placeholder loop script (not yet implemented).

#include "limbo_script.h"
#include "job.h"

const cmd_t LIMBO_SCRIPT[] = {
    TAP(KC_SPACE),
    WAIT_JITTER_UP(1000, 150),
    END(),
};
