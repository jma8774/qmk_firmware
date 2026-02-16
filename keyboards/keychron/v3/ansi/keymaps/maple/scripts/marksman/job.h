// Marksman job – game actions (macros + cooldown-tracked functions).

#pragma once

#include "../cmd_macros.h"
#include QMK_KEYBOARD_H

// ---------------------------------------------------------------------------
// Stateless actions
// ---------------------------------------------------------------------------

#define SHOOT()                  TAP(KC_Q), WAIT_JITTER_D(500)
#define ROPE()                   TAP(KC_C), WAIT_JITTER_D(1600)
#define ROPE_DELAY(ms)           TAP(KC_C), WAIT_JITTER_D(ms)
#define DASH()                   TAP(KC_LALT), WAIT_JITTER_D(520)
#define JUMP()                   TAP(KC_E)
#define TELEPORT_RESET()         TAP(KC_X), WAIT_JITTER_D(525)

#define JUMP_ATTACK()            TAP(KC_E), WAIT_JITTER_D(50),  \
                                 TAP(KC_E), WAIT_JITTER_D(200), \
                                 TAP(KC_Q), WAIT_JITTER_D(540)

#define JUMP_DOWN_ATTACK()       PRESS(KC_DOWN), WAIT_JITTER_D(50), PRESS(KC_E), WAIT_JITTER_D(50), \
                                 TAP(KC_Q), WAIT_JITTER_D(50), RELEASE(KC_E), WAIT_JITTER_D(50), RELEASE(KC_DOWN), \
                                 WAIT_JITTER_D(1000)

#define JUMP_DOWN_ATTACK_DELAY(ms) \
                                 PRESS(KC_DOWN), WAIT_JITTER_D(50), PRESS(KC_E), WAIT_JITTER_D(50), \
                                 TAP(KC_Q), WAIT_JITTER_D(50), RELEASE(KC_E), WAIT_JITTER_D(50), RELEASE(KC_DOWN), \
                                 WAIT_JITTER_D(ms)

#define WALK_LEFT(ms)            PRESS(KC_LEFT), WAIT(ms), RELEASE(KC_LEFT), WAIT_JITTER_D(50)
#define WALK_RIGHT(ms)           PRESS(KC_RIGHT), WAIT(ms), RELEASE(KC_RIGHT), WAIT_JITTER_D(50)

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

// ---------------------------------------------------------------------------
// TRY_* macros  –  call + cast delay (only if fired)
// ---------------------------------------------------------------------------

#define TRY_BOLT_BURST()      CALL(bolt_burst),    SKIP_FALSE(1), WAIT_JITTER_D(410)
#define TRY_SURGEBOLT()       CALL(surgebolt),     SKIP_FALSE(1), WAIT_JITTER_D(70)
#define TRY_WEB()             CALL(web),           SKIP_FALSE(1), WAIT_JITTER_D(300)
#define TRY_SOLAR_CREST()     CALL(solar_crest),   SKIP_FALSE(1), WAIT_JITTER_D(400)
#define TRY_ERDA_FOUNTAIN()   CALL(erda_fountain), SKIP_FALSE(1), WAIT_JITTER_D(450)
#define TRY_JANUS()           CALL(janus),         SKIP_FALSE(1), WAIT_JITTER_D(400)
#define TRY_JANUS2()          CALL(janus2),        SKIP_FALSE(1), WAIT_JITTER_D(400)
#define TRY_JANUS3()          CALL(janus3),         SKIP_FALSE(1), WAIT_JITTER_D(400)
#define TRY_TELEPORT_SETUP()  CALL(teleport_setup), SKIP_FALSE(1), WAIT_JITTER_D(650)
