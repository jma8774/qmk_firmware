// Script engine – non-blocking command runner implementation.

#include "script_engine.h"
#include "timing.h"
#include QMK_KEYBOARD_H   // brings in timer, register_code, etc.

// ---------------------------------------------------------------------------
// runner_start
// ---------------------------------------------------------------------------

void runner_start(runner_t *r, const cmd_t *script, runner_mode_t mode) {
    r->script    = script;
    r->mode      = mode;
    r->ip        = 0;
    r->wake_time = timer_read32();  // eligible to run immediately
    r->active    = true;
}

// ---------------------------------------------------------------------------
// runner_stop
// ---------------------------------------------------------------------------

void runner_stop(runner_t *r) {
    r->active = false;
}

// ---------------------------------------------------------------------------
// runner_task  –  call once per matrix_scan_user()
// ---------------------------------------------------------------------------
//
// Processes instant commands (PRESS / RELEASE / TAP / CALL) in a tight loop
// so they execute within a single scan cycle.
//
// Yields (returns) on:
//   CMD_WAIT_MS / CMD_WAIT_JITTER  –  sets wake_time, returns
//   CMD_END                         –  sets active = false, returns

void runner_task(runner_t *r) {
    if (!r->active) {
        return;
    }

    // Still sleeping?
    if (!timer_expired32(timer_read32(), r->wake_time)) {
        return;
    }

    while (r->active) {
        uprintf("[runner_task] Running command %d\n", r->ip);
        const cmd_t *c = &r->script[r->ip];

        uprintf("[runner_task] Command type: %d\n", c->type);
        switch (c->type) {

            case CMD_PRESS:
                uprintf("[runner_task] CMD_PRESS\n");
                register_code(c->kc);
                break;

            case CMD_RELEASE:
                uprintf("[runner_task] CMD_RELEASE\n");
                unregister_code(c->kc);
                break;

            case CMD_TAP: {
                uprintf("[runner_task] CMD_TAP\n");
                // a = base tap duration ms, b = jitter pct (0 = no jitter)
                uint16_t delay = c->a ? (uint16_t)c->a : 50;
                if (c->b) {
                    delay = (uint16_t)jitter(delay, (uint8_t)c->b);
                }
                tap_code_delay(c->kc, delay);
                break;
            }

            case CMD_WAIT_MS:
                uprintf("[runner_task] CMD_WAIT_MS\n");
                r->wake_time = timer_read32() + c->a;
                r->ip++;
                return;  // yield

            case CMD_WAIT_JITTER:
                uprintf("[runner_task] CMD_WAIT_JITTER\n");
                r->wake_time = timer_read32() + jitter(c->a, (uint8_t)c->b);
                r->ip++;
                return;  // yield

            case CMD_CALL:
                uprintf("[runner_task] CMD_CALL\n");
                r->last_result = c->fn ? c->fn() : false;
                break;

            case CMD_CHANCE:
                uprintf("[runner_task] CMD_CHANCE\n");
                r->last_result = timing_chance((uint8_t)c->a);
                break;

            case CMD_SKIP_IF:
                uprintf("[runner_task] CMD_SKIP_IF\n");
                if (r->last_result) r->ip += (uint16_t)c->a;
                break;

            case CMD_SKIP_UNLESS:
                uprintf("[runner_task] CMD_SKIP_UNLESS\n");
                if (!r->last_result) r->ip += (uint16_t)c->a;
                break;

            case CMD_END:
                uprintf("[runner_task] CMD_END\n");
                r->active = false;
                return;  // script finished
        }

        r->ip++;
    }
}
