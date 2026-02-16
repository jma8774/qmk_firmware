// Marksman job – cooldown-tracked action implementations.
//
// Each function checks a static cooldown timer.
//   - If ready:  tap the key, reset the cooldown, return true.
//   - If not:    no-op, return false.

#include "job.h"
#include "../../common.h"
#include "../../engine/timing.h"

// ---------------------------------------------------------------------------
// Cooldown helper
// ---------------------------------------------------------------------------
// Tap a key with human-like jitter if the cooldown has expired.
// Returns true if the action fired.

static bool cooldown_tap(uint32_t *cd_timer, uint32_t cd_ms, uint16_t kc) {
    uint32_t now = timer_read32();
    if (!timer_expired32(now, *cd_timer)) return false;
    uint16_t tap_ms = (uint16_t)jitter(DEFAULT_TAP_MS, DEFAULT_TAP_JITTER_PCT);
    tap_code_delay(kc, tap_ms);
    *cd_timer = now + cd_ms;
    return true;
}

// ---------------------------------------------------------------------------
// Bolt Burst  –  D key, 6 s cooldown
// ---------------------------------------------------------------------------

#define BOLT_BURST_KEY   KC_D
#define BOLT_BURST_CD_MS 6000
static uint32_t cd_bolt_burst = 0;
bool bolt_burst(void) {
    return cooldown_tap(&cd_bolt_burst, BOLT_BURST_CD_MS, BOLT_BURST_KEY);
}

// ---------------------------------------------------------------------------
// Surgebolt  –  R key, 18 s cooldown
// ---------------------------------------------------------------------------

#define SURGEBOLT_KEY   KC_R
#define SURGEBOLT_CD_MS 18000
static uint32_t cd_surgebolt = 0;
bool surgebolt(void) {
    return cooldown_tap(&cd_surgebolt, SURGEBOLT_CD_MS, SURGEBOLT_KEY);
}

// ---------------------------------------------------------------------------
// Web  –  4 key, 250 s cooldown
// ---------------------------------------------------------------------------

#define WEB_KEY   KC_4
#define WEB_CD_MS 250000
static uint32_t cd_web = 0;
bool web(void) {
    return cooldown_tap(&cd_web, WEB_CD_MS, WEB_KEY);
}

// ---------------------------------------------------------------------------
// Solar Crest  –  5 key, 250 s cooldown
// ---------------------------------------------------------------------------

#define SOLAR_CREST_KEY   KC_5
#define SOLAR_CREST_CD_MS 250000
static uint32_t cd_solar_crest = 0;
bool solar_crest(void) {
    return cooldown_tap(&cd_solar_crest, SOLAR_CREST_CD_MS, SOLAR_CREST_KEY);
}

// ---------------------------------------------------------------------------
// Erda Fountain  –  B key, 57 s cooldown
// ---------------------------------------------------------------------------

#define ERDA_FOUNTAIN_KEY   KC_B
#define ERDA_FOUNTAIN_CD_MS 57000
static uint32_t cd_erda_fountain = 0;
bool erda_fountain(void) {
    return cooldown_tap(&cd_erda_fountain, ERDA_FOUNTAIN_CD_MS, ERDA_FOUNTAIN_KEY);
}

// ---------------------------------------------------------------------------
// Janus  –  N key, 57 s cooldown, 3 independent charges
// ---------------------------------------------------------------------------

#define JANUS_KEY   KC_N
#define JANUS_CD_MS 57000
static uint32_t cd_janus = 0, cd_janus2 = 0, cd_janus3 = 0;
bool janus(void)  { return cooldown_tap(&cd_janus,  JANUS_CD_MS, JANUS_KEY); }
bool janus2(void) { return cooldown_tap(&cd_janus2, JANUS_CD_MS, JANUS_KEY); }
bool janus3(void) { return cooldown_tap(&cd_janus3, JANUS_CD_MS, JANUS_KEY); }

// ---------------------------------------------------------------------------
// Teleport setup (blink setup)  –  hold DOWN + double-tap X, release DOWN after 0.6 s
// ---------------------------------------------------------------------------
// Activation: press DOWN, tap X, tap X, wait 600 ms, release DOWN.
// Cooldown: 30–45 s (random).  Blocks for 600 ms when it fires.

#define TELEPORT_SETUP_CD_MS    45000   // max; range 67–100% => 30–45 s

static uint32_t cd_teleport_setup = 0;

bool teleport_setup(void) {
    uint32_t now = timer_read32();
    if (!timer_expired32(now, cd_teleport_setup)) return false;

    register_code(KC_DOWN);
    wait_ms(jitter(55, 20));
    tap_code_delay(KC_X, jitter(30, 20));
    wait_ms(jitter(55, 20));
    tap_code_delay(KC_X, jitter(30, 20));
    wait_ms(jitter(70, 20));
    unregister_code(KC_DOWN);

    cd_teleport_setup = now + random_range(TELEPORT_SETUP_CD_MS, 67, 100);
    return true;
}
