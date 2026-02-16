// Rotation script – one rotation cycle for marksman.
// Uses generator macros for readability.

#include "rotation_script.h"
#include "job.h"

const cmd_t ROTATION_NORMAL[] = {
    SHOOT(),
    WAIT_JITTER(5000, 15),
    END(),
};
