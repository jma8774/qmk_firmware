// Script engine – generic non-blocking command runner for QMK.
// See script_engine.c for the implementation.

#pragma once

#include <stdint.h>
#include <stdbool.h>

// ---------------------------------------------------------------------------
// Command types
// ---------------------------------------------------------------------------

typedef enum {
    CMD_PRESS,        // register_code(kc)
    CMD_RELEASE,      // unregister_code(kc)
    CMD_TAP,          // tap_code_delay(kc, a ? a : 10)
    CMD_WAIT_MS,           // delay for `a` ms
    CMD_WAIT_JITTER,       // delay for jitter(a, b) ms  (+/- pct%)
    CMD_WAIT_JITTER_DOWN,  // delay for jitter_down(a, b) ms  (only down, never over a)
    CMD_WAIT_JITTER_UP,    // delay for jitter_up(a, b) ms    (only up, never under a)
    CMD_CALL,              // call fn(); stores return value in last_result
    CMD_CHANCE,       // last_result = (rand() % 100) < a;  a = 0..100
    CMD_SKIP_TRUE,      // if last_result == true,  skip `a` commands forward
    CMD_SKIP_FALSE,  // if last_result == false, skip `a` commands forward
    CMD_END           // marks end of script
} cmd_type_t;

// ---------------------------------------------------------------------------
// Command structure
// ---------------------------------------------------------------------------

typedef bool (*hook_fn_t)(void);

typedef struct {
    cmd_type_t  type;
    uint16_t    kc;   // keycode (for PRESS / RELEASE / TAP)
    uint32_t    a;    // generic param  (delay ms / tap duration ms / skip count)
    uint32_t    b;    // generic param  (jitter pct for TAP / WAIT_JITTER / WAIT_JITTER_DOWN / WAIT_JITTER_UP)
    hook_fn_t   fn;   // function pointer (for CMD_CALL, returns bool)
    const char *fn_name; // optional name for CMD_CALL logging (NULL if unknown)
} cmd_t;

// ---------------------------------------------------------------------------
// Runner mode (used by policy layer to decide what to do next)
// ---------------------------------------------------------------------------

typedef enum {
    MODE_NONE,
    MODE_ROTATION,
    MODE_SETUP,
    MODE_BUFF
} runner_mode_t;

// ---------------------------------------------------------------------------
// Runner state
// ---------------------------------------------------------------------------

typedef struct {
    bool           active;       // true while a script is executing
    runner_mode_t  mode;         // which script just ran / is running
    const cmd_t   *script;       // pointer to the current script array
    uint16_t       ip;           // instruction pointer (index into script)
    uint32_t       wake_time;    // timer_read32() value to wake at
    bool           last_result;  // return value of the last CMD_CALL
} runner_t;

// ---------------------------------------------------------------------------
// API
// ---------------------------------------------------------------------------

// Load a script into the runner and begin execution on the next runner_task().
void runner_start(runner_t *r, const cmd_t *script, runner_mode_t mode);

// Immediately stop the runner (sets active = false).
void runner_stop(runner_t *r);

// Advance the runner by one or more steps.  Call once per matrix scan.
// Processes instant commands (PRESS/RELEASE/TAP/CALL) in a tight loop;
// yields on CMD_WAIT_* (sets wake_time) or CMD_END (sets active = false).
void runner_task(runner_t *r);
