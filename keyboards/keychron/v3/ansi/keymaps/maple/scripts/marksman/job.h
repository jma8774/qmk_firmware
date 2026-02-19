// Marksman job – game actions (macros + cooldown-tracked functions).

#pragma once

#include "../cmd_macros.h"
#include QMK_KEYBOARD_H

// ---------------------------------------------------------------------------
// Stateless actions
// ---------------------------------------------------------------------------

#define SHOOT()                  TAP(KC_Q), WAIT_JITTER_UP_D(600)
#define ROPE()                   TAP(KC_C), WAIT_JITTER_UP_D(1600)
#define ROPE_DELAY(ms)           TAP(KC_C), WAIT_JITTER_D(ms)
#define DASH()                   TAP(KC_LALT), WAIT_JITTER_UP(600, 5)
#define DASH_SKIPS_COUNT        2
#define JUMP()                   TAP(KC_E)
#define TELEPORT_RESET()         TAP(KC_X), WAIT_JITTER_D(60), TAP(KC_X), WAIT_JITTER_D(60), TAP(KC_X), WAIT_JITTER_UP_D(650)

#define FLASH_JUMP()             TAP(KC_E), WAIT_JITTER_UP_D(70), TAP(KC_E), WAIT_JITTER_UP(580, 5)

#define JUMP_ATTACK()            TAP(KC_E), WAIT_JITTER_UP_D(70),  \
                                 TAP(KC_E), WAIT_JITTER_D(200), \
                                 TAP(KC_Q), WAIT_JITTER_UP(620, 5)
#define JUMP_ATTACK_SKIPS_COUNT 6

#define JUMP_ATTACK_DELAY(jumpDelay, attackDelay) \
                                 TAP(KC_E), WAIT_JITTER_UP_D(jumpDelay),  \
                                 TAP(KC_E), WAIT_JITTER_D(attackDelay), \
                                 TAP(KC_Q), WAIT_JITTER_UP(610, 5)

#define JUMP_DOWN_ATTACK()       PRESS(KC_DOWN), WAIT_JITTER_D(50), PRESS(KC_E), WAIT_JITTER_D(50), \
                                 TAP(KC_Q), WAIT_JITTER_D(50), RELEASE(KC_E), WAIT_JITTER_D(50), RELEASE(KC_DOWN), \
                                 WAIT_JITTER_UP_D(1000)

#define JUMP_DOWN_ATTACK_DELAY(ms) \
                                 PRESS(KC_DOWN), WAIT_JITTER_D(50), PRESS(KC_E), WAIT_JITTER_D(50), \
                                 TAP(KC_Q), WAIT_JITTER_D(50), RELEASE(KC_E), WAIT_JITTER_D(50), RELEASE(KC_DOWN), \
                                 WAIT_JITTER_UP_D(ms)

#define WALK_LEFT(ms)            PRESS(KC_LEFT), WAIT_JITTER(ms, 5), RELEASE(KC_LEFT), WAIT_JITTER_D(50)
#define WALK_RIGHT(ms)           PRESS(KC_RIGHT), WAIT_JITTER(ms, 5), RELEASE(KC_RIGHT), WAIT_JITTER_D(50)

// ---------------------------------------------------------------------------
// Cooldown-tracked actions  (functions in job.c)
// ---------------------------------------------------------------------------

bool bolt_burst(void);
bool surgebolt(void);
bool web(void);
bool solar_crest(void);
bool erda_fountain(void);
bool janus(void);
bool janus2(void);
bool janus3(void);
bool teleport_setup(void);
bool boss_buffs(void);

// ---------------------------------------------------------------------------
// TRY_* macros  –  call + cast delay (only if fired)
// ---------------------------------------------------------------------------

#define TRY_BOLT_BURST()      CALL_NAMED("bolt_burst", bolt_burst),    SKIP_FALSE(1), WAIT_JITTER_UP_D(430)
#define TRY_SURGEBOLT()       CALL_NAMED("surgebolt", surgebolt),     SKIP_FALSE(1), WAIT_JITTER_UP_D(70)
#define TRY_WEB()             CALL_NAMED("web", web),           SKIP_FALSE(1), WAIT_JITTER_UP_D(300)
#define TRY_SOLAR_CREST()     CALL_NAMED("solar_crest", solar_crest),   SKIP_FALSE(1), WAIT_JITTER_UP_D(400)
#define TRY_ERDA_FOUNTAIN()   CALL_NAMED("erda_fountain", erda_fountain), SKIP_FALSE(1), WAIT_JITTER_UP_D(600)
#define TRY_JANUS()           CALL_NAMED("janus", janus),         SKIP_FALSE(1), WAIT_JITTER_UP_D(560)
#define TRY_JANUS2()          CALL_NAMED("janus2", janus2),        SKIP_FALSE(1), WAIT_JITTER_UP_D(560)
#define TRY_JANUS3()          CALL_NAMED("janus3", janus3),         SKIP_FALSE(1), WAIT_JITTER_UP_D(560)
#define TRY_TELEPORT_SETUP()  CALL_NAMED("teleport_setup", teleport_setup), SKIP_FALSE(1), WAIT_JITTER_UP_D(675)
#define TRY_BOSS_BUFFS()      CALL_NAMED("boss_buffs", boss_buffs), SKIP_FALSE(1), WAIT_JITTER_UP_D(1500)
