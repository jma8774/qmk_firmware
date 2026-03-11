// Setup script – runs after a rotation completes when the setup interval elapses.

#pragma once

#include "../../engine/script_engine.h"

// Dynamic builders — pick a random variant each call
const cmd_t* make_setup_tallahart_up(void);
const cmd_t* make_setup_tallahart_reset(void);
const cmd_t* make_setup_tallahart(void);
