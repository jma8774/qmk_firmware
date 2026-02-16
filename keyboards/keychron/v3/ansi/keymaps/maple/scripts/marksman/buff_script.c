// Buff script – buff keys + teleport setup at end.

#include "buff_script.h"
#include "job.h"

const cmd_t BUFF_SCRIPT[] = {
    TRY_TELEPORT_SETUP(),
    WAIT_JITTER_D(2000),
    END(),
};
