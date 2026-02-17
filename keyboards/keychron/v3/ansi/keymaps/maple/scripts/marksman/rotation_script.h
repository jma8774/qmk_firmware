// Rotation script – one rotation cycle (does not loop internally).

#pragma once

#include "../../engine/script_engine.h"

// Shoot
extern const cmd_t ROTATION_NORMAL[];

// Jump, attack, then 0 or more jumps after
extern const cmd_t ROTATION_JUMP_ATTACK[];

// Jump attack (1-2 times), teleport reset
extern const cmd_t ROTATION_JUMP_ATTACK_TELEPORT_RESET[];

// etc.

extern const cmd_t ROTATION_PS_RIGHT[];
extern const cmd_t ROTATION_CERNIUM[];
extern const cmd_t ROTATION_ODIUM[];
