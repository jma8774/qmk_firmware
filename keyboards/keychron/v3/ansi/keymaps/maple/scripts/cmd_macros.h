// Command macros – high-level macros that expand to cmd_t entries.
//
// Usage:
//   const cmd_t MY_SCRIPT[] = {
//       SHOOT(),
//       WAIT_JITTER(5000, 15),
//       SHOOT(),
//       END(),
//   };
//
// Each macro expands to one or more {cmd_type, kc, a, b, fn} entries
// separated by commas, so they compose naturally inside an array initializer.

#pragma once

#include "../engine/script_engine.h"

// ---------------------------------------------------------------------------
// Defaults
// ---------------------------------------------------------------------------

// Average human gamer tap duration ~65 ms (press-to-release).
#define DEFAULT_TAP_MS           60
#define DEFAULT_TAP_JITTER_PCT   15

// Default jitter percentage for WAIT_JITTER when pct is omitted.
#define DEFAULT_WAIT_JITTER_PCT       20

// ---------------------------------------------------------------------------
// Primitive actions  (1 command each)
// ---------------------------------------------------------------------------

// Tap a key with human-like duration (~65 ms +/- 15 % jitter).
#define TAP(kc)                  {CMD_TAP, (kc), DEFAULT_TAP_MS, DEFAULT_TAP_JITTER_PCT, NULL}

// Press / release a key (for holds or combos).
#define PRESS(kc)                {CMD_PRESS, (kc), 0, 0, NULL}
#define RELEASE(kc)              {CMD_RELEASE, (kc), 0, 0, NULL}

// Fixed delay.
#define WAIT(ms)                 {CMD_WAIT_MS, 0, (ms), 0, NULL}

// Jittered delay: base_ms +/- pct% (default 20 %).
#define WAIT_JITTER(ms, pct)     {CMD_WAIT_JITTER, 0, (ms), (pct), NULL}
#define WAIT_JITTER_D(ms)        {CMD_WAIT_JITTER, 0, (ms), DEFAULT_WAIT_JITTER_PCT, NULL}

// Call a function (must return bool).  Result stored in runner.last_result.
#define CALL(fn_ptr)             {CMD_CALL, 0, 0, 0, (fn_ptr)}

// Probabilistic: set last_result = true with pct% probability (0–100).
#define CHANCE(pct)              {CMD_CHANCE, 0, (pct), 0, NULL}

// Conditional skips — branch based on last result (CALL or CHANCE).
//   n = number of commands to skip forward.
#define SKIP_TRUE(n)             {CMD_SKIP_IF, 0, (n), 0, NULL}
#define SKIP_FALSE(n)            {CMD_SKIP_UNLESS, 0, (n), 0, NULL}

// End marker (every script must end with this).
#define END()                    {CMD_END, 0, 0, 0, NULL}

// ---------------------------------------------------------------------------
// Compound actions  (expand to multiple commands)
// ---------------------------------------------------------------------------

// Hold a key for a fixed duration:  press -> wait -> release
#define HOLD(kc, ms)             PRESS(kc), WAIT(ms), RELEASE(kc)

// Hold a key for a jittered duration:  press -> jittered wait -> release
#define HOLD_JITTER(kc, ms, pct) PRESS(kc), WAIT_JITTER(ms, pct), RELEASE(kc)
