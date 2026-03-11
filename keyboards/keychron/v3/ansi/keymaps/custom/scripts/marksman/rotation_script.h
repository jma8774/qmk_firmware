// Rotation script – one rotation cycle (does not loop internally).

#pragma once

#include "../../engine/script_engine.h"


extern const cmd_t ROTATION_TALLAHART_SURGEBOLT[];
extern const cmd_t ROTATION_TALLAHART_SHOOT[];
extern const cmd_t ROTATION_TALLAHART_SHOOT_1[];
extern const cmd_t ROTATION_TALLAHART_SHOOT_2[];
extern const cmd_t ROTATION_TALLAHART_SHOOT_3[];
extern const cmd_t ROTATION_TALLAHART_SHOOT_4[];
extern const cmd_t ROTATION_TALLAHART_SHOOT_5[];
extern const cmd_t ROTATION_TALLAHART_BOLT_BURST[];
extern const cmd_t ROTATION_TALLAHART_BOLT_BURST_1[];
extern const cmd_t ROTATION_TALLAHART_BOLT_BURST_2[];
extern const cmd_t ROTATION_TALLAHART_BOLT_BURST_3[];
extern const cmd_t ROTATION_TALLAHART_JUMP_ATTACK[];
extern const cmd_t ROTATION_TALLAHART_JUMP_ATTACK_1[];
extern const cmd_t ROTATION_TALLAHART_JUMP_ATTACK_2[];
extern const cmd_t ROTATION_TALLAHART_DASH[];
extern const cmd_t ROTATION_TALLAHART_DASH_1[];

const cmd_t* make_rotation_tallahart(void);
